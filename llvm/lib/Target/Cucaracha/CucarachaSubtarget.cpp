//===-- CucarachaSubtarget.cpp - Cucaracha Subtarget Information
//------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Cucaracha specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "CucarachaSubtarget.h"
#include "Cucaracha.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "Cucaracha-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "CucarachaGenSubtargetInfo.inc"

using namespace llvm;

void CucarachaSubtarget::anchor() {}

CucarachaSubtarget::CucarachaSubtarget(const Triple &TT, StringRef CPU,
                                       StringRef FS, CucarachaTargetMachine &TM)
    : CucarachaGenSubtargetInfo(TT, CPU, /* Tune CPU */ CPU, FS),
      DL("e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:32-f64:32-a:0:32-n32"),
      InstrInfo(), TLInfo(TM), TSInfo(), FrameLowering() {}
