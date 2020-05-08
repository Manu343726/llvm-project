//===-- FantasyBaseInfo.h - Top level definitions for Fantasy MC ----*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Fantasy target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIBASEINFO_H
#define LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIBASEINFO_H

#include "FantasyMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

// FantasyII - This namespace holds all of the target specific flags that
// instruction info tracks.
namespace FantasyII {
// Target Operand Flag enum.
enum TOF {
  //===------------------------------------------------------------------===//
  // Fantasy Specific MachineOperand flags.
  MO_NO_FLAG,

  // MO_ABS_HI/LO - Represents the hi or low part of an absolute symbol
  // address.
  MO_ABS_HI,
  MO_ABS_LO,
};
} // namespace FantasyII

static inline unsigned getFantasyRegisterNumbering(unsigned Reg) {
  switch (Reg) {
  case Fantasy::R0:
    return 0;
  case Fantasy::R1:
    return 1;
  case Fantasy::R2:
  case Fantasy::PC:
    return 2;
  case Fantasy::R3:
    return 3;
  case Fantasy::R4:
  case Fantasy::SP:
    return 4;
  case Fantasy::R5:
  case Fantasy::FP:
    return 5;
  case Fantasy::R6:
    return 6;
  case Fantasy::R7:
    return 7;
  case Fantasy::R8:
  case Fantasy::RV:
    return 8;
  case Fantasy::R9:
    return 9;
  case Fantasy::R10:
  case Fantasy::RR1:
    return 10;
  case Fantasy::R11:
  case Fantasy::RR2:
    return 11;
  case Fantasy::R12:
    return 12;
  case Fantasy::R13:
    return 13;
  case Fantasy::R14:
    return 14;
  case Fantasy::R15:
  case Fantasy::RCA:
    return 15;
  case Fantasy::R16:
    return 16;
  case Fantasy::R17:
    return 17;
  case Fantasy::R18:
    return 18;
  case Fantasy::R19:
    return 19;
  case Fantasy::R20:
    return 20;
  case Fantasy::R21:
    return 21;
  case Fantasy::R22:
    return 22;
  case Fantasy::R23:
    return 23;
  case Fantasy::R24:
    return 24;
  case Fantasy::R25:
    return 25;
  case Fantasy::R26:
    return 26;
  case Fantasy::R27:
    return 27;
  case Fantasy::R28:
    return 28;
  case Fantasy::R29:
    return 29;
  case Fantasy::R30:
    return 30;
  case Fantasy::R31:
    return 31;
  default:
    llvm_unreachable("Unknown register number!");
  }
}
} // namespace llvm
#endif // LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIBASEINFO_H
