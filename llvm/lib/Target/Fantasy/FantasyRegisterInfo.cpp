//===-- FantasyRegisterInfo.cpp - Fantasy Register Information ------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Fantasy implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "FantasyRegisterInfo.h"
#include "FantasyAluCode.h"
#include "FantasyCondCode.h"
#include "FantasyFrameLowering.h"
#include "FantasyInstrInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "FantasyGenRegisterInfo.inc"

using namespace llvm;

FantasyRegisterInfo::FantasyRegisterInfo()
    : FantasyGenRegisterInfo(Fantasy::RCA) {}

const uint16_t *
FantasyRegisterInfo::getCalleeSavedRegs(const MachineFunction * /*MF*/) const {
  return CSR_SaveList;
}

BitVector
FantasyRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(Fantasy::R0);
  Reserved.set(Fantasy::R1);
  Reserved.set(Fantasy::PC);
  Reserved.set(Fantasy::R2);
  Reserved.set(Fantasy::SP);
  Reserved.set(Fantasy::R4);
  Reserved.set(Fantasy::FP);
  Reserved.set(Fantasy::R5);
  Reserved.set(Fantasy::RR1);
  Reserved.set(Fantasy::R10);
  Reserved.set(Fantasy::RR2);
  Reserved.set(Fantasy::R11);
  Reserved.set(Fantasy::RCA);
  Reserved.set(Fantasy::R15);
  if (hasBasePointer(MF))
    Reserved.set(getBaseRegister());
  return Reserved;
}

bool FantasyRegisterInfo::requiresRegisterScavenging(
    const MachineFunction & /*MF*/) const {
  return true;
}

bool FantasyRegisterInfo::trackLivenessAfterRegAlloc(
    const MachineFunction & /*MF*/) const {
  return true;
}

static bool isALUArithLoOpcode(unsigned Opcode) {
  switch (Opcode) {
  case Fantasy::ADD_I_LO:
  case Fantasy::SUB_I_LO:
  case Fantasy::ADD_F_I_LO:
  case Fantasy::SUB_F_I_LO:
  case Fantasy::ADDC_I_LO:
  case Fantasy::SUBB_I_LO:
  case Fantasy::ADDC_F_I_LO:
  case Fantasy::SUBB_F_I_LO:
    return true;
  default:
    return false;
  }
}

static unsigned getOppositeALULoOpcode(unsigned Opcode) {
  switch (Opcode) {
  case Fantasy::ADD_I_LO:
    return Fantasy::SUB_I_LO;
  case Fantasy::SUB_I_LO:
    return Fantasy::ADD_I_LO;
  case Fantasy::ADD_F_I_LO:
    return Fantasy::SUB_F_I_LO;
  case Fantasy::SUB_F_I_LO:
    return Fantasy::ADD_F_I_LO;
  case Fantasy::ADDC_I_LO:
    return Fantasy::SUBB_I_LO;
  case Fantasy::SUBB_I_LO:
    return Fantasy::ADDC_I_LO;
  case Fantasy::ADDC_F_I_LO:
    return Fantasy::SUBB_F_I_LO;
  case Fantasy::SUBB_F_I_LO:
    return Fantasy::ADDC_F_I_LO;
  default:
    llvm_unreachable("Invalid ALU lo opcode");
  }
}

static unsigned getRRMOpcodeVariant(unsigned Opcode) {
  switch (Opcode) {
  case Fantasy::LDBs_RI:
    return Fantasy::LDBs_RR;
  case Fantasy::LDBz_RI:
    return Fantasy::LDBz_RR;
  case Fantasy::LDHs_RI:
    return Fantasy::LDHs_RR;
  case Fantasy::LDHz_RI:
    return Fantasy::LDHz_RR;
  case Fantasy::LDW_RI:
    return Fantasy::LDW_RR;
  case Fantasy::STB_RI:
    return Fantasy::STB_RR;
  case Fantasy::STH_RI:
    return Fantasy::STH_RR;
  case Fantasy::SW_RI:
    return Fantasy::SW_RR;
  default:
    llvm_unreachable("Opcode has no RRM variant");
  }
}

void FantasyRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                              int SPAdj, unsigned FIOperandNum,
                                              RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  bool HasFP = TFI->hasFP(MF);
  DebugLoc DL = MI.getDebugLoc();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex) +
               MI.getOperand(FIOperandNum + 1).getImm();

  // Addressable stack objects are addressed using neg. offsets from fp
  // or pos. offsets from sp/basepointer
  if (!HasFP || (needsStackRealignment(MF) && FrameIndex >= 0))
    Offset += MF.getFrameInfo().getStackSize();

  Register FrameReg = getFrameRegister(MF);
  if (FrameIndex >= 0) {
    if (hasBasePointer(MF))
      FrameReg = getBaseRegister();
    else if (needsStackRealignment(MF))
      FrameReg = Fantasy::SP;
  }

  // Replace frame index with a frame pointer reference.
  // If the offset is small enough to fit in the immediate field, directly
  // encode it.
  // Otherwise scavenge a register and encode it into a MOVHI, OR_I_LO sequence.
  if ((isSPLSOpcode(MI.getOpcode()) && !isInt<10>(Offset)) ||
      !isInt<16>(Offset)) {
    assert(RS && "Register scavenging must be on");
    unsigned Reg = RS->FindUnusedReg(&Fantasy::GPRRegClass);
    if (!Reg)
      Reg = RS->scavengeRegister(&Fantasy::GPRRegClass, II, SPAdj);
    assert(Reg && "Register scavenger failed");

    bool HasNegOffset = false;
    // ALU ops have unsigned immediate values. If the Offset is negative, we
    // negate it here and reverse the opcode later.
    if (Offset < 0) {
      HasNegOffset = true;
      Offset = -Offset;
    }

    if (!isInt<16>(Offset)) {
      // Reg = hi(offset) | lo(offset)
      BuildMI(*MI.getParent(), II, DL, TII->get(Fantasy::MOVHI), Reg)
          .addImm(static_cast<uint32_t>(Offset) >> 16);
      BuildMI(*MI.getParent(), II, DL, TII->get(Fantasy::OR_I_LO), Reg)
          .addReg(Reg)
          .addImm(Offset & 0xffffU);
    } else {
      // Reg = mov(offset)
      BuildMI(*MI.getParent(), II, DL, TII->get(Fantasy::ADD_I_LO), Reg)
          .addImm(0)
          .addImm(Offset);
    }
    // Reg = FrameReg OP Reg
    if (MI.getOpcode() == Fantasy::ADD_I_LO) {
      BuildMI(*MI.getParent(), II, DL,
              HasNegOffset ? TII->get(Fantasy::SUB_R)
                           : TII->get(Fantasy::ADD_R),
              MI.getOperand(0).getReg())
          .addReg(FrameReg)
          .addReg(Reg)
          .addImm(LPCC::ICC_T);
      MI.eraseFromParent();
      return;
    }
    if (isSPLSOpcode(MI.getOpcode()) || isRMOpcode(MI.getOpcode())) {
      MI.setDesc(TII->get(getRRMOpcodeVariant(MI.getOpcode())));
      if (HasNegOffset) {
        // Change the ALU op (operand 3) from LPAC::ADD (the default) to
        // LPAC::SUB with the already negated offset.
        assert((MI.getOperand(3).getImm() == LPAC::ADD) &&
               "Unexpected ALU op in RRM instruction");
        MI.getOperand(3).setImm(LPAC::SUB);
      }
    } else
      llvm_unreachable("Unexpected opcode in frame index operation");

    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, /*isDef=*/false);
    MI.getOperand(FIOperandNum + 1)
        .ChangeToRegister(Reg, /*isDef=*/false, /*isImp=*/false,
                          /*isKill=*/true);
    return;
  }

  // ALU arithmetic ops take unsigned immediates. If the offset is negative,
  // we replace the instruction with one that inverts the opcode and negates
  // the immediate.
  if ((Offset < 0) && isALUArithLoOpcode(MI.getOpcode())) {
    unsigned NewOpcode = getOppositeALULoOpcode(MI.getOpcode());
    // We know this is an ALU op, so we know the operands are as follows:
    // 0: destination register
    // 1: source register (frame register)
    // 2: immediate
    BuildMI(*MI.getParent(), II, DL, TII->get(NewOpcode),
            MI.getOperand(0).getReg())
        .addReg(FrameReg)
        .addImm(-Offset);
    MI.eraseFromParent();
  } else {
    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, /*isDef=*/false);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
  }
}

bool FantasyRegisterInfo::hasBasePointer(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  // When we need stack realignment and there are dynamic allocas, we can't
  // reference off of the stack pointer, so we reserve a base pointer.
  if (needsStackRealignment(MF) && MFI.hasVarSizedObjects())
    return true;

  return false;
}

unsigned FantasyRegisterInfo::getRARegister() const { return Fantasy::RCA; }

Register
FantasyRegisterInfo::getFrameRegister(const MachineFunction & /*MF*/) const {
  return Fantasy::FP;
}

Register FantasyRegisterInfo::getBaseRegister() const { return Fantasy::R14; }

const uint32_t *
FantasyRegisterInfo::getCallPreservedMask(const MachineFunction & /*MF*/,
                                          CallingConv::ID /*CC*/) const {
  return CSR_RegMask;
}
