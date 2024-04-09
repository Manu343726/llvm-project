//===-- Cucaracha.h - Top-level interface for Cucaracha representation --*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Cucaracha back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_Cucaracha_H
#define TARGET_Cucaracha_H

#include "MCTargetDesc/CucarachaMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TargetMachine;
class CucarachaTargetMachine;
class FunctionPass;

FunctionPass *createCucarachaISelDag(CucarachaTargetMachine &TM,
                                     CodeGenOpt::Level OptLevel);
} // namespace llvm

#endif
