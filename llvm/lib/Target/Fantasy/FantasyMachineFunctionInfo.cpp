//===-- FantasyMachineFuctionInfo.cpp - Fantasy machine function info ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FantasyMachineFunctionInfo.h"

using namespace llvm;

void FantasyMachineFunctionInfo::anchor() {}

unsigned FantasyMachineFunctionInfo::getGlobalBaseReg() {
  // Return if it has already been initialized.
  if (GlobalBaseReg)
    return GlobalBaseReg;

  return GlobalBaseReg =
             MF.getRegInfo().createVirtualRegister(&Fantasy::GPRRegClass);
}
