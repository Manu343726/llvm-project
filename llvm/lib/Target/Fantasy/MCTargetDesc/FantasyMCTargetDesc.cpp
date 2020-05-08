//===-- FantasyMCTargetDesc.cpp - Fantasy Target Descriptions
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Fantasy specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "FantasyMCTargetDesc.h"
#include "FantasyInstPrinter.h"
#include "FantasyMCAsmInfo.h"
#include "TargetInfo/FantasyTargetInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include <cstdint>
#include <string>

#define GET_INSTRINFO_MC_DESC
#include "FantasyGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "FantasyGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "FantasyGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createFantasyMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitFantasyMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createFantasyMCRegisterInfo(const Triple & /*TT*/) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitFantasyMCRegisterInfo(X, Fantasy::RCA, 0, 0, Fantasy::PC);
  return X;
}

static MCSubtargetInfo *
createFantasyMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "generic";

  return createFantasyMCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCStreamer *createMCStreamer(const Triple &T, MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> &&MAB,
                                    std::unique_ptr<MCObjectWriter> &&OW,
                                    std::unique_ptr<MCCodeEmitter> &&Emitter,
                                    bool RelaxAll) {
  if (!T.isOSBinFormatELF())
    llvm_unreachable("OS not supported");

  return createELFStreamer(Context, std::move(MAB), std::move(OW),
                           std::move(Emitter), RelaxAll);
}

static MCInstPrinter *createFantasyMCInstPrinter(const Triple & /*T*/,
                                                 unsigned SyntaxVariant,
                                                 const MCAsmInfo &MAI,
                                                 const MCInstrInfo &MII,
                                                 const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0)
    return new FantasyInstPrinter(MAI, MII, MRI);
  return nullptr;
}

static MCRelocationInfo *createFantasyElfRelocation(const Triple &TheTriple,
                                                    MCContext &Ctx) {
  return createMCRelocationInfo(TheTriple, Ctx);
}

namespace {

class FantasyMCInstrAnalysis : public MCInstrAnalysis {
public:
  explicit FantasyMCInstrAnalysis(const MCInstrInfo *Info)
      : MCInstrAnalysis(Info) {}

  bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size,
                      uint64_t &Target) const override {
    if (Inst.getNumOperands() == 0)
      return false;

    if (Info->get(Inst.getOpcode()).OpInfo[0].OperandType ==
        MCOI::OPERAND_PCREL) {
      int64_t Imm = Inst.getOperand(0).getImm();
      Target = Addr + Size + Imm;
      return true;
    } else {
      int64_t Imm = Inst.getOperand(0).getImm();

      // Skip case where immediate is 0 as that occurs in file that isn't linked
      // and the branch target inferred would be wrong.
      if (Imm == 0)
        return false;

      Target = Imm;
      return true;
    }
  }
};

} // end anonymous namespace

static MCInstrAnalysis *createFantasyInstrAnalysis(const MCInstrInfo *Info) {
  return new FantasyMCInstrAnalysis(Info);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFantasyTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<FantasyMCAsmInfo> X(getTheFantasyTarget());

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheFantasyTarget(),
                                      createFantasyMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheFantasyTarget(),
                                    createFantasyMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheFantasyTarget(),
                                          createFantasyMCSubtargetInfo);

  // Register the MC code emitter
  TargetRegistry::RegisterMCCodeEmitter(getTheFantasyTarget(),
                                        createFantasyMCCodeEmitter);

  // Register the ASM Backend
  TargetRegistry::RegisterMCAsmBackend(getTheFantasyTarget(),
                                       createFantasyAsmBackend);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheFantasyTarget(),
                                        createFantasyMCInstPrinter);

  // Register the ELF streamer.
  TargetRegistry::RegisterELFStreamer(getTheFantasyTarget(), createMCStreamer);

  // Register the MC relocation info.
  TargetRegistry::RegisterMCRelocationInfo(getTheFantasyTarget(),
                                           createFantasyElfRelocation);

  // Register the MC instruction analyzer.
  TargetRegistry::RegisterMCInstrAnalysis(getTheFantasyTarget(),
                                          createFantasyInstrAnalysis);
}
