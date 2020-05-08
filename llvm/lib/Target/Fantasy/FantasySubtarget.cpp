//===- FantasySubtarget.cpp - Fantasy Subtarget Information -----------*- C++
//-*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Fantasy specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "FantasySubtarget.h"

#include "Fantasy.h"

#define DEBUG_TYPE "lanai-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "FantasyGenSubtargetInfo.inc"

using namespace llvm;

void FantasySubtarget::initSubtargetFeatures(StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "generic";

  ParseSubtargetFeatures(CPUName, FS);
}

FantasySubtarget &
FantasySubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {
  initSubtargetFeatures(CPU, FS);
  return *this;
}

FantasySubtarget::FantasySubtarget(const Triple &TargetTriple, StringRef Cpu,
                                   StringRef FeatureString,
                                   const TargetMachine &TM,
                                   const TargetOptions & /*Options*/,
                                   CodeModel::Model /*CodeModel*/,
                                   CodeGenOpt::Level /*OptLevel*/)
    : FantasyGenSubtargetInfo(TargetTriple, Cpu, FeatureString),
      FrameLowering(initializeSubtargetDependencies(Cpu, FeatureString)),
      InstrInfo(), TLInfo(TM, *this), TSInfo() {}
