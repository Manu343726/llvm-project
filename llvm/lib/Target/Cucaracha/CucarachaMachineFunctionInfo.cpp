//===-- CucarachaMachineFunctionInfo.cpp - Cucaracha Machine Function Info
//--------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CucarachaMachineFunctionInfo.h"

using namespace llvm;

void CucarachaMachineFunctionInfo::anchor() {}

MachineFunctionInfo *CucarachaMachineFunctionInfo::clone(
    BumpPtrAllocator &Allocator, MachineFunction &DestMF,
    const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
    const {
  return DestMF.cloneInfo<CucarachaMachineFunctionInfo>(*this);
}
