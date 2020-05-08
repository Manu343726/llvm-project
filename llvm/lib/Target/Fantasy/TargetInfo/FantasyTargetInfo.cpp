//===-- FantasyTargetInfo.cpp - Fantasy Target Implementation
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/FantasyTargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheFantasyTarget() {
  static Target TheFantasyTarget;
  return TheFantasyTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFantasyTargetInfo() {
  RegisterTarget<Triple::lanai> X(getTheFantasyTarget(), "lanai", "Fantasy",
                                  "Fantasy");
}
