//===-- LEGMCTargetDesc.cpp - LEG Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides LEG specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "LEGMCTargetDesc.h"
#include "LEGInstPrinter.h"
#include "LEGMCAsmInfo.h"
#include "TargetInfo/LEGTargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "LEGGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "LEGGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "LEGGenRegisterInfo.inc"

static MCAsmInfo *createLEGMCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TT,
                                     const MCTargetOptions &Options) {
  return new LEGMCAsmInfo(TT);
}

static MCInstrInfo *createLEGMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitLEGMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createLEGMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitLEGMCRegisterInfo(X, LEG::LR);
  return X;
}

static MCSubtargetInfo *createLEGMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  return createLEGMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCInstPrinter *createLEGMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new LEGInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeLEGTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(getTheLEGTarget(), createLEGMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheLEGTarget(), createLEGMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheLEGTarget(), createLEGMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheLEGTarget(),
                                          createLEGMCSubtargetInfo);

  // Register the MC Code Emitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheLEGTarget(),
                                        createLEGMCCodeEmitter);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(getTheLEGTarget(), createLEGAsmBackend);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(getTheLEGTarget(),
                                        createLEGMCInstPrinter);
}
