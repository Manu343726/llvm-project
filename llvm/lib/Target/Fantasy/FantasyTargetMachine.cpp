//===-- FantasyTargetMachine.cpp - Define TargetMachine for Fantasy
//---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Fantasy target spec.
//
//===----------------------------------------------------------------------===//

#include "FantasyTargetMachine.h"

#include "Fantasy.h"
#include "FantasyTargetObjectFile.h"
#include "FantasyTargetTransformInfo.h"
#include "TargetInfo/FantasyTargetInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

namespace llvm {
void initializeFantasyMemAluCombinerPass(PassRegistry &);
} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFantasyTarget() {
  // Register the target.
  RegisterTargetMachine<FantasyTargetMachine> registered_target(
      getTheFantasyTarget());
}

static std::string computeDataLayout() {
  // Data layout (keep in sync with clang/lib/Basic/Targets.cpp)
  return "E"        // Big endian
         "-m:e"     // ELF name manging
         "-p:32:32" // 32-bit pointers, 32 bit aligned
         "-i64:64"  // 64 bit integers, 64 bit aligned
         "-a:0:32"  // 32 bit alignment of objects of aggregate type
         "-n32"     // 32 bit native integer width
         "-S64";    // 64 bit natural stack alignment
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::PIC_;
  return *RM;
}

FantasyTargetMachine::FantasyTargetMachine(
    const Target &T, const Triple &TT, StringRef Cpu, StringRef FeatureString,
    const TargetOptions &Options, Optional<Reloc::Model> RM,
    Optional<CodeModel::Model> CodeModel, CodeGenOpt::Level OptLevel, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(), TT, Cpu, FeatureString, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CodeModel, CodeModel::Medium),
                        OptLevel),
      Subtarget(TT, Cpu, FeatureString, *this, Options, getCodeModel(),
                OptLevel),
      TLOF(new FantasyTargetObjectFile()) {
  initAsmInfo();
}

TargetTransformInfo
FantasyTargetMachine::getTargetTransformInfo(const Function &F) {
  return TargetTransformInfo(FantasyTTIImpl(this, F));
}

namespace {
// Fantasy Code Generator Pass Configuration Options.
class FantasyPassConfig : public TargetPassConfig {
public:
  FantasyPassConfig(FantasyTargetMachine &TM, PassManagerBase *PassManager)
      : TargetPassConfig(TM, *PassManager) {}

  FantasyTargetMachine &getFantasyTargetMachine() const {
    return getTM<FantasyTargetMachine>();
  }

  bool addInstSelector() override;
  void addPreSched2() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *
FantasyTargetMachine::createPassConfig(PassManagerBase &PassManager) {
  return new FantasyPassConfig(*this, &PassManager);
}

// Install an instruction selector pass.
bool FantasyPassConfig::addInstSelector() {
  addPass(createFantasyISelDag(getFantasyTargetMachine()));
  return false;
}

// Implemented by targets that want to run passes immediately before
// machine code is emitted.
void FantasyPassConfig::addPreEmitPass() {
  addPass(createFantasyDelaySlotFillerPass(getFantasyTargetMachine()));
}

// Run passes after prolog-epilog insertion and before the second instruction
// scheduling pass.
void FantasyPassConfig::addPreSched2() {
  addPass(createFantasyMemAluCombinerPass());
}
