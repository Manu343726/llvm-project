//===-- CucarachaMCTargetDesc.cpp - Cucaracha Target Descriptions
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Cucaracha specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "CucarachaMCTargetDesc.h"
#include "CucarachaInstPrinter.h"
#include "CucarachaMCAsmInfo.h"
#include "CucarachaTargetStreamer.h"
#include "TargetInfo/CucarachaTargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "CucarachaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "CucarachaGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "CucarachaGenRegisterInfo.inc"

static MCAsmInfo *createCucarachaMCAsmInfo(const MCRegisterInfo &MRI,
                                           const Triple &TT,
                                           const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new CucarachaELFMCAsmInfo(TT);
  unsigned Reg = MRI.getDwarfRegNum(SP::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 0);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCAsmInfo *createCucarachaV9MCAsmInfo(const MCRegisterInfo &MRI,
                                             const Triple &TT,
                                             const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new CucarachaELFMCAsmInfo(TT);
  unsigned Reg = MRI.getDwarfRegNum(SP::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 2047);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstrInfo *createCucarachaMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitCucarachaMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createCucarachaMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitCucarachaMCRegisterInfo(X, SP::O7);
  return X;
}

static MCSubtargetInfo *
createCucarachaMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  if (CPU.empty())
    CPU = (TT.getArch() == Triple::cucarachav9) ? "v9" : "v8";
  return createCucarachaMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCTargetStreamer *
createObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new CucarachaTargetELFStreamer(S);
}

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter *InstPrint,
                                                 bool isVerboseAsm) {
  return new CucarachaTargetAsmStreamer(S, OS);
}

static MCTargetStreamer *createNullTargetStreamer(MCStreamer &S) {
  return new CucarachaTargetStreamer(S);
}

static MCInstPrinter *createCucarachaMCInstPrinter(const Triple &T,
                                                   unsigned SyntaxVariant,
                                                   const MCAsmInfo &MAI,
                                                   const MCInstrInfo &MII,
                                                   const MCRegisterInfo &MRI) {
  return new CucarachaInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCucarachaTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(getTheCucarachaTarget(), createCucarachaMCAsmInfo);
  RegisterMCAsmInfoFn Y(getTheCucarachaV9Target(), createCucarachaV9MCAsmInfo);
  RegisterMCAsmInfoFn Z(getTheCucarachaelTarget(), createCucarachaMCAsmInfo);

  for (Target *T : {&getTheCucarachaTarget(), &getTheCucarachaV9Target(),
                    &getTheCucarachaelTarget()}) {
    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createCucarachaMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createCucarachaMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createCucarachaMCSubtargetInfo);

    // Register the MC Code Emitter.
    TargetRegistry::RegisterMCCodeEmitter(*T, createCucarachaMCCodeEmitter);

    // Register the asm backend.
    TargetRegistry::RegisterMCAsmBackend(*T, createCucarachaAsmBackend);

    // Register the object target streamer.
    TargetRegistry::RegisterObjectTargetStreamer(*T,
                                                 createObjectTargetStreamer);

    // Register the asm streamer.
    TargetRegistry::RegisterAsmTargetStreamer(*T, createTargetAsmStreamer);

    // Register the null streamer.
    TargetRegistry::RegisterNullTargetStreamer(*T, createNullTargetStreamer);

    // Register the MCInstPrinter
    TargetRegistry::RegisterMCInstPrinter(*T, createCucarachaMCInstPrinter);
  }
}
