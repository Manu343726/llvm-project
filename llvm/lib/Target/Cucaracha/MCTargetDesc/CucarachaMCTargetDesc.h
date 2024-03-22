//===-- CucarachaMCTargetDesc.h - Cucaracha Target Descriptions ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Cucaracha specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaMCTARGETDESC_H
#define LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCCodeEmitter *createCucarachaMCCodeEmitter(const MCInstrInfo &MCII,
                                            MCContext &Ctx);
MCAsmBackend *createCucarachaAsmBackend(const Target &T,
                                        const MCSubtargetInfo &STI,
                                        const MCRegisterInfo &MRI,
                                        const MCTargetOptions &Options);
std::unique_ptr<MCObjectTargetWriter>
createCucarachaELFObjectWriter(uint8_t OSABI);
} // namespace llvm

// Defines symbolic names for Cucaracha registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "CucarachaGenRegisterInfo.inc"

// Defines symbolic names for the Cucaracha instructions.
//
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "CucarachaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "CucarachaGenSubtargetInfo.inc"

#endif
