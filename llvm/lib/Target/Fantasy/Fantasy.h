//===-- Fantasy.h - Top-level interface for Fantasy representation --*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Fantasy back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LANAI_LANAI_H
#define LLVM_LIB_TARGET_LANAI_LANAI_H

#include "llvm/Pass.h"

namespace llvm {
class FunctionPass;
class FantasyTargetMachine;
class MachineFunctionPass;
class TargetMachine;
class formatted_raw_ostream;

// createFantasyISelDag - This pass converts a legalized DAG into a
// Fantasy-specific DAG, ready for instruction scheduling.
FunctionPass *createFantasyISelDag(FantasyTargetMachine &TM);

// createFantasyDelaySlotFillerPass - This pass fills delay slots
// with useful instructions or nop's
FunctionPass *createFantasyDelaySlotFillerPass(const FantasyTargetMachine &TM);

// createFantasyMemAluCombinerPass - This pass combines loads/stores and
// arithmetic operations.
FunctionPass *createFantasyMemAluCombinerPass();

// createFantasySetflagAluCombinerPass - This pass combines SET_FLAG and ALU
// operations.
FunctionPass *createFantasySetflagAluCombinerPass();

} // namespace llvm

#endif // LLVM_LIB_TARGET_LANAI_LANAI_H
