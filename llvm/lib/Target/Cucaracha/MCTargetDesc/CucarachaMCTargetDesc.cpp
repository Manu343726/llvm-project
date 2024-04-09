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
  return new CucarachaMCAsmInfo(TT);
}

static MCInstrInfo *createCucarachaMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitCucarachaMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createCucarachaMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitCucarachaMCRegisterInfo(X, Cucaracha::LR);
  return X;
}

static MCSubtargetInfo *
createCucarachaMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createCucarachaMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
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

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheCucarachaTarget(),
                                      createCucarachaMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheCucarachaTarget(),
                                    createCucarachaMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheCucarachaTarget(),
                                          createCucarachaMCSubtargetInfo);

  // Register the MC Code Emitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheCucarachaTarget(),
                                        createCucarachaMCCodeEmitter);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(getTheCucarachaTarget(),
                                       createCucarachaAsmBackend);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(getTheCucarachaTarget(),
                                        createCucarachaMCInstPrinter);
}
