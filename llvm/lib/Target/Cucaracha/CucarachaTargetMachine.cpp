//===-- CucarachaTargetMachine.cpp - Define TargetMachine for Cucaracha
//-----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "CucarachaTargetMachine.h"
#include "Cucaracha.h"
#include "CucarachaMachineFunctionInfo.h"
#include "CucarachaTargetObjectFile.h"
#include "LeonPasses.h"
#include "TargetInfo/CucarachaTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCucarachaTarget() {
  // Register the target.
  RegisterTargetMachine<CucarachaTargetMachine> X(getTheCucarachaTarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeCucarachaDAGToDAGISelPass(PR);
}

static cl::opt<bool>
    BranchRelaxation("cucaracha-enable-branch-relax", cl::Hidden,
                     cl::init(true),
                     cl::desc("Relax out of range conditional branches"));

static std::string computeDataLayout(const Triple &T) {
  // Cucaracha is typically big endian, but some are little.
  std::string Ret = T.getArch() == Triple::cucaracha ? "e" : "E";
  Ret += "-m:e";

  // Alignments for 64 bit integers.
  Ret += "-i64:64";

  Ret += "-f128:64-n32";

  Ret += "-S64";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.
static CodeModel::Model
getEffectiveCucarachaCodeModel(std::optional<CodeModel::Model> CM,
                               Reloc::Model RM, bool JIT) {
  if (CM) {
    if (*CM == CodeModel::Tiny)
      report_fatal_error("Target does not support the tiny CodeModel", false);
    if (*CM == CodeModel::Kernel)
      report_fatal_error("Target does not support the kernel CodeModel", false);
    return *CM;
  }
  return CodeModel::Small;
}

/// Create an ILP32 architecture model
CucarachaTargetMachine::CucarachaTargetMachine(
    const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
    const TargetOptions &Options, std::optional<Reloc::Model> RM,
    std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(
          T, computeDataLayout(TT), TT, CPU, FS, Options,
          getEffectiveRelocModel(RM),
          getEffectiveCucarachaCodeModel(CM, getEffectiveRelocModel(RM), JIT),
          OL),
      TLOF(std::make_unique<CucarachaELFTargetObjectFile>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this,
                false /* not 64 bit */) {
  initAsmInfo();
}

CucarachaTargetMachine::~CucarachaTargetMachine() = default;

const CucarachaSubtarget *
CucarachaTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  // FIXME: This is related to the code below to reset the target options,
  // we need to know whether or not the soft float flag is set on the
  // function, so we can enable it as a subtarget feature.
  bool softFloat = F.getFnAttribute("use-soft-float").getValueAsBool();

  if (softFloat)
    FS += FS.empty() ? "+soft-float" : ",+soft-float";

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<CucarachaSubtarget>(TargetTriple, CPU, FS, *this,
                                             this->is64Bit);
  }
  return I.get();
}

MachineFunctionInfo *CucarachaTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return CucarachaMachineFunctionInfo::create<CucarachaMachineFunctionInfo>(
      Allocator, F, STI);
}

namespace {
/// Cucaracha Code Generator Pass Configuration Options.
class CucarachaPassConfig : public TargetPassConfig {
public:
  CucarachaPassConfig(CucarachaTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  CucarachaTargetMachine &getCucarachaTargetMachine() const {
    return getTM<CucarachaTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *
CucarachaTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new CucarachaPassConfig(*this, PM);
}

void CucarachaPassConfig::addIRPasses() {
  addPass(createAtomicExpandPass());

  TargetPassConfig::addIRPasses();
}

bool CucarachaPassConfig::addInstSelector() {
  addPass(createCucarachaISelDag(getCucarachaTargetMachine()));
  return false;
}

void CucarachaPassConfig::addPreEmitPass() {
  if (BranchRelaxation)
    addPass(&BranchRelaxationPassID);

  addPass(createCucarachaDelaySlotFillerPass());

  if (this->getCucarachaTargetMachine().getSubtargetImpl()->insertNOPLoad()) {
    addPass(new InsertNOPLoad());
  }
  if (this->getCucarachaTargetMachine()
          .getSubtargetImpl()
          ->detectRoundChange()) {
    addPass(new DetectRoundChange());
  }
  if (this->getCucarachaTargetMachine().getSubtargetImpl()->fixAllFDIVSQRT()) {
    addPass(new FixAllFDIVSQRT());
  }
}