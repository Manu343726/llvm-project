//===-- LEGTargetInfo.cpp - LEG Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/LEGTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheLEGTarget() {
  static Target TheLEGTarget;
  return TheLEGTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeLEGTargetInfo() {
  RegisterTarget<Triple::sparc, /*HasJIT=*/false> X(getTheLEGTarget(), "leg",
                                                    "LEG", "LEG");
}
