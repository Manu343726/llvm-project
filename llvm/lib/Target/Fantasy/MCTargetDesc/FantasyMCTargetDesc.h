//===-- FantasyMCTargetDesc.h - Fantasy Target Descriptions ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Fantasy specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIMCTARGETDESC_H
#define LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIMCTARGETDESC_H

#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCInstrAnalysis;
class MCObjectTargetWriter;
class MCRelocationInfo;
class MCSubtargetInfo;
class Target;
class Triple;
class StringRef;
class raw_pwrite_stream;

MCCodeEmitter *createFantasyMCCodeEmitter(const MCInstrInfo &MCII,
                                          const MCRegisterInfo &MRI,
                                          MCContext &Ctx);

MCAsmBackend *createFantasyAsmBackend(const Target &T,
                                      const MCSubtargetInfo &STI,
                                      const MCRegisterInfo &MRI,
                                      const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter>
createFantasyELFObjectWriter(uint8_t OSABI);
} // namespace llvm

// Defines symbolic names for Fantasy registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "FantasyGenRegisterInfo.inc"

// Defines symbolic names for the Fantasy instructions.
#define GET_INSTRINFO_ENUM
#include "FantasyGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "FantasyGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_LANAI_MCTARGETDESC_LANAIMCTARGETDESC_H
