//===-- LEGRegisterInfo.cpp - LEG Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the LEG implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "LEGRegisterInfo.h"
#include "LEG.h"
#include "LEGFrameLowering.h"
#include "LEGInstrInfo.h"
#include "LEGMachineFunctionInfo.h"
#include "MCTargetDesc/LEGMCTargetDesc.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define GET_REGINFO_TARGET_DESC
#include "LEGGenRegisterInfo.inc"

using namespace llvm;

LEGRegisterInfo::LEGRegisterInfo() : LEGGenRegisterInfo(LEG::LR) {}

const uint16_t *
LEGRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  static const uint16_t CalleeSavedRegs[] = {LEG::R4, LEG::R5, LEG::R6, LEG::R7,
                                             LEG::R8, LEG::R9, 0};
  return CalleeSavedRegs;
}

BitVector LEGRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(LEG::SP);
  Reserved.set(LEG::LR);
  return Reserved;
}

const uint32_t *LEGRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
  return CC_Save_RegMask;
}

bool LEGRegisterInfo::requiresRegisterScavenging(
    const MachineFunction &MF) const {
  return true;
}
bool LEGRegisterInfo::requiresFrameIndexScavenging(
    const MachineFunction &MF) const {
  return true;
}

bool LEGRegisterInfo::trackLivenessAfterRegAlloc(
    const MachineFunction &MF) const {
  return true;
}

bool LEGRegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const {
  return false;
}

bool eliminateLoadStoreFrameIndex(MachineInstr &MI, const MachineFrameInfo &MFI,
                                  MachineOperand &FIOp, unsigned FIOperandNum,
                                  unsigned FI) {
  unsigned ImmOpIdx = FIOperandNum + 1;
  auto &ImmOp = MI.getOperand(ImmOpIdx);

  // FIXME: check the size of offset.
  int Offset = MFI.getObjectOffset(FI) + MFI.getStackSize() + ImmOp.getImm();
  FIOp.ChangeToRegister(LEG::SP, false);
  ImmOp.setImm(Offset);

  return true;
}

bool eliminateMoveFrameIndex(MachineBasicBlock &MBB, MachineFunction &MF,
                             MachineInstr &MI, MachineBasicBlock::iterator II,
                             MachineOperand &FIOp, const TargetInstrInfo &TII,
                             const MachineFrameInfo &MFI, unsigned FI) {
  // FIXME: check the size of offset.
  int Offset = MFI.getObjectOffset(FI) + MFI.getStackSize();

  assert(MI.getNumOperands() == 2 && "Expected two operands, the destination "
                                     "register and the FrameIndex immediate");
  const auto &DestReg = MI.getOperand(0);
  assert(DestReg.isReg() &&
         "Wrong operand, this not seems to be the destination register");

  DebugLoc DL;

  // Move offset into destination register so we can do DestReg = SP +
  // FrameIndex offset later (Remember we don't have Add reg immediate
  // instructions, only add reg reg)
  FIOp.ChangeToImmediate(Offset);

  // Now perform do the DestReg = SP + FrameIndex Offset
  BuildMI(MF, DL, TII.get(LEG::ADDrr), DestReg.getReg())
      .addReg(LEG::SP)
      .addReg(DestReg.getReg());

  return true;
}

bool LEGRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  MachineInstr &MI = *II;
  auto &MBB = *II->getParent();
  MachineFunction &MF = *MBB.getParent();
  auto &TII = *MF.getSubtarget().getInstrInfo();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineOperand &FIOp = MI.getOperand(FIOperandNum);
  unsigned FI = FIOp.getIndex();

  // Determine if we can eliminate the index from this kind of instruction.
  switch (MI.getOpcode()) {
  default:
    // Not supported yet.
    return false;
  case LEG::LDR:
  case LEG::STR:
    return eliminateLoadStoreFrameIndex(MI, MFI, FIOp, FIOperandNum, FI);
  case LEG::MOVi32:
    return eliminateMoveFrameIndex(MBB, MF, MI, II, FIOp, TII, MFI, FI);
  }
}

Register LEGRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return LEG::SP;
}
