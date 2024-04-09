//===-- CucarachaMachineFuctionInfo.h - Cucaracha machine function info -*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Cucaracha-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef CucarachaMACHINEFUNCTIONINFO_H
#define CucarachaMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

// Forward declarations
class Function;

/// CucarachaFunctionInfo - This class is derived from MachineFunction private
/// Cucaracha target-specific information for each MachineFunction.
class CucarachaFunctionInfo : public MachineFunctionInfo {
public:
  CucarachaFunctionInfo() {}

  ~CucarachaFunctionInfo() {}
};
} // namespace llvm

#endif // CucarachaMACHINEFUNCTIONINFO_H
