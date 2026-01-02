//===-- CucarachaInstrInfo.cpp - Cucaracha Instruction Information
//----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Cucaracha implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "CucarachaInstrInfo.h"
#include "Cucaracha.h"
#include "CucarachaMachineFunctionInfo.h"
#include "MCTargetDesc/CucarachaBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "CucarachaGenInstrInfo.inc"

using namespace llvm;

// Pin the vtable to this file.
void CucarachaInstrInfo::anchor() {}

CucarachaInstrInfo::CucarachaInstrInfo()
    : CucarachaGenInstrInfo(Cucaracha::ADJCALLSTACKDOWN,
                            Cucaracha::ADJCALLSTACKUP),
      RI() {}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned CucarachaInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                                 int &FrameIndex) const {
  assert(0 && "Unimplemented");
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned CucarachaInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                                int &FrameIndex) const {
  assert(0 && "Unimplemented");
  return 0;
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//
//
/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with an conditional branch and it falls through to
///    an successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
/// 4. If this block ends with an conditional branch and an unconditional
///    block, it returns the 'true' destination in TBB, the 'false' destination
///    in FBB, and a list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success.
///
bool CucarachaInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                       MachineBasicBlock *&TBB,
                                       MachineBasicBlock *&FBB,
                                       SmallVectorImpl<MachineOperand> &Cond,
                                       bool AllowModify) const {
  // Cucaracha uses register-indirect branches (JMP reg, CJMP reg)
  // After pseudo-expansion, the branches contain register operands, not MBB operands
  // We cannot easily analyze these branches since the target is computed at runtime
  // Return true to indicate we cannot analyze the branch
  //
  // This disables some branch optimizations, but is necessary for correctness
  // with our indirect branch model.
  //
  // For simple cases (unconditional JMP with known target), we could potentially
  // analyze by looking at the MOVIMM16L/H sequence that loads the address.
  // For now, just return failure to keep things simple.
  return true;
}

/// RemoveBranch - Remove the branching code at the end of the specific MBB.
/// This is only invoked in cases where AnalyzeBranch returns success. It
/// returns the number of instructions that were removed.
unsigned CucarachaInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                          int *BytesRemoved) const {
  if (MBB.empty())
    return 0;
  unsigned NumRemoved = 0;
  auto I = MBB.end();
  do {
    --I;
    unsigned Opc = I->getOpcode();
    if ((Opc == Cucaracha::JMP) || (Opc == Cucaracha::CJMP)) {
      auto ToDelete = I;
      ++I;
      MBB.erase(ToDelete);
      NumRemoved++;
    }
  } while (I != MBB.begin());
  return NumRemoved;
}

/// InsertBranch - Insert branch code into the end of the specified
/// MachineBasicBlock.  The operands to this method are the same as those
/// returned by AnalyzeBranch.  This is only invoked in cases where
/// AnalyzeBranch returns success. It returns the number of instructions
/// inserted.
///
/// It is also invoked by tail merging to add unconditional branches in
/// cases where AnalyzeBranch doesn't apply because there was no original
/// branch to analyze.  At least this much must be implemented, else tail
/// merging needs to be disabled.
unsigned CucarachaInstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  unsigned NumInserted = 0;

  // DEBUG: Print branch insertion info
#if (LLVM_ENABLE_DUMP)
  LLVM_DEBUG(dbgs() << "insertBranch: MBB=" << MBB.getName()
                    << " TBB=" << (TBB ? TBB->getName() : "null")
                    << " FBB=" << (FBB ? FBB->getName() : "null")
                    << " Cond.size=" << Cond.size() << "\n");
#endif

  // Insert any conditional branch.
  // Cucaracha CJMP uses register operands, so we need to generate:
  // 1. Load mask into R4
  // 2. Load target address into R5  
  // 3. CJMP R4, R5, R6 (link output in R6)
  if (Cond.size() > 0) {
    // Cond[0] contains the condition mask (from analyzeBranch)
    if (Cond[0].isImm()) {
      unsigned Mask = Cond[0].getImm();
      BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
          .addImm(Mask & 0xffff);
      if ((Mask >> 16) != 0) {
        BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16H))
            .addReg(Cucaracha::R4, RegState::Define)
            .addImm((Mask >> 16) & 0xffff)
            .addReg(Cucaracha::R4);
      }
    } else if (Cond[0].isReg()) {
      // If condition is already in a register, copy it
      BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOV), Cucaracha::R4)
          .addReg(Cond[0].getReg());
    }

    // Load target address
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16L), Cucaracha::R5)
        .addMBB(TBB, CucarachaII::MO_LO16);
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16H))
        .addReg(Cucaracha::R5, RegState::Define)
        .addMBB(TBB, CucarachaII::MO_HI16)
        .addReg(Cucaracha::R5);

    // CJMP: (link output, mask input, target input)
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::CJMP))
        .addReg(Cucaracha::R6, RegState::Define)  // link output
        .addReg(Cucaracha::R4)                    // mask register
        .addReg(Cucaracha::R5);                   // target register
    NumInserted++;
  }

  // Insert any unconditional branch.
  // For unconditional branch, generate:
  // 1. Load target address into R4
  // 2. JMP R4, R5 (link output in R5)
  if (Cond.empty() || FBB) {
    MachineBasicBlock *Target = Cond.empty() ? TBB : FBB;
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
        .addMBB(Target, CucarachaII::MO_LO16);
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::MOVIMM16H))
        .addReg(Cucaracha::R4, RegState::Define)
        .addMBB(Target, CucarachaII::MO_HI16)
        .addReg(Cucaracha::R4);
    BuildMI(MBB, MBB.end(), DL, get(Cucaracha::JMP))
        .addReg(Cucaracha::R5, RegState::Define)  // link output (dead)
        .addReg(Cucaracha::R4);                   // target register
    NumInserted++;
  }
  return NumInserted;
}

bool CucarachaInstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  // Since we return true from analyzeBranch (cannot analyze), this should
  // rarely be called. However, we implement it for completeness.
  //
  // Cucaracha's CJMP branches if (cpsr & mask) != 0
  // To reverse, we would need to branch if (cpsr & mask) == 0
  // But our instruction can only check != 0, not == 0
  //
  // The proper way to handle this would be to swap the branch targets,
  // which is handled by the caller when we return false from analyzeBranch.
  //
  // Since we can't actually reverse the condition (we'd need a different
  // instruction or negated mask semantics), return true to indicate failure.
  return true;
}

void CucarachaInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I,
                                     const DebugLoc &DL, MCRegister DestReg,
                                     MCRegister SrcReg, bool KillSrc) const {
  BuildMI(MBB, I, DL, get(Cucaracha::MOV), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
}

void CucarachaInstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  BuildMI(MBB, I, DL, get(Cucaracha::PseudoST))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex)
      .addImm(0);
}

void CucarachaInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                              MachineBasicBlock::iterator I,
                                              Register DestReg, int FrameIndex,
                                              const TargetRegisterClass *RC,
                                              const TargetRegisterInfo *TRI,
                                              Register VReg) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  BuildMI(MBB, I, DL, get(Cucaracha::PseudoLD), DestReg)
      .addFrameIndex(FrameIndex)
      .addImm(0);
}

bool CucarachaInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  default:
    return false;
  case Cucaracha::PseudoLD: {
    // Expand PseudoLD to: compute effective address, then LD
    // PseudoLD has: dst, base, offset
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();
    Register DstReg = MI.getOperand(0).getReg();
    Register BaseReg = MI.getOperand(1).getReg();
    int64_t Offset = MI.getOperand(2).getImm();

    if (Offset == 0) {
      // No offset - just emit LD directly with base register
      BuildMI(MBB, MI, DL, get(Cucaracha::LD), DstReg)
          .addReg(BaseReg);
    } else {
      // Need to compute effective address: base + offset
      // Use R4 as temp for offset, R5 as temp for effective address
      // 1. Load offset immediate into R4
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
          .addImm(Offset & 0xffff);
      if ((Offset >> 16) != 0 || Offset < 0) {
        BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
            .addReg(Cucaracha::R4, RegState::Define)
            .addImm((Offset >> 16) & 0xffff)
            .addReg(Cucaracha::R4);
      }
      // 2. ADD base + offset -> R5
      BuildMI(MBB, MI, DL, get(Cucaracha::ADD), Cucaracha::R5)
          .addReg(BaseReg)
          .addReg(Cucaracha::R4);
      // 3. LD from effective address
      BuildMI(MBB, MI, DL, get(Cucaracha::LD), DstReg)
          .addReg(Cucaracha::R5);
    }
    MBB.erase(MI);
    return true;
  }
  case Cucaracha::PseudoST: {
    // Expand PseudoST to: compute effective address, then ST
    // PseudoST has: src, base, offset
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();
    Register SrcReg = MI.getOperand(0).getReg();
    Register BaseReg = MI.getOperand(1).getReg();
    int64_t Offset = MI.getOperand(2).getImm();

    if (Offset == 0) {
      // No offset - just emit ST directly with base register
      BuildMI(MBB, MI, DL, get(Cucaracha::ST))
          .addReg(SrcReg)
          .addReg(BaseReg);
    } else {
      // Need to compute effective address: base + offset
      // Use R4 as temp for offset, R5 as temp for effective address
      // 1. Load offset immediate into R4
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
          .addImm(Offset & 0xffff);
      if ((Offset >> 16) != 0 || Offset < 0) {
        BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
            .addReg(Cucaracha::R4, RegState::Define)
            .addImm((Offset >> 16) & 0xffff)
            .addReg(Cucaracha::R4);
      }
      // 2. ADD base + offset -> R5
      BuildMI(MBB, MI, DL, get(Cucaracha::ADD), Cucaracha::R5)
          .addReg(BaseReg)
          .addReg(Cucaracha::R4);
      // 3. ST to effective address
      BuildMI(MBB, MI, DL, get(Cucaracha::ST))
          .addReg(SrcReg)
          .addReg(Cucaracha::R5);
    }
    MBB.erase(MI);
    return true;
  }
  case Cucaracha::PseudoBR: {
    // Expand PseudoBR to: load address into temp reg, then JMP via register
    // PseudoBR has one operand: the target basic block
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();
    const MachineOperand &Target = MI.getOperand(0);

    // Use R4 as a temporary register for the target address
    // First, load the block address into R4 using MOVIMM16L/MOVIMM16H
    BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
        .addMBB(Target.getMBB(), CucarachaII::MO_LO16);
    BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
        .addReg(Cucaracha::R4, RegState::Define)
        .addMBB(Target.getMBB(), CucarachaII::MO_HI16)
        .addReg(Cucaracha::R4);

    // Now use JMP with the register - JMP takes (target, link)
    // Use R5 as the link register (will be discarded for unconditional branch)
    BuildMI(MBB, MI, DL, get(Cucaracha::JMP))
        .addReg(Cucaracha::R5, RegState::Define)  // link output (dead)
        .addReg(Cucaracha::R4);                   // target register

    MBB.erase(MI);
    return true;
  }
  case Cucaracha::PseudoBRCOND: {
    // Expand PseudoBRCOND to: load mask, load address, then CJMP via registers
    // PseudoBRCOND has three operands: cpsr (register), mask (immediate), target (basic block)
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();
    const MachineOperand &CpsrOp = MI.getOperand(0);
    const MachineOperand &MaskOp = MI.getOperand(1);
    const MachineOperand &Target = MI.getOperand(2);

    // The CPSR value is in a register - we need to copy it to R6 for CJMP to read
    // CJMP reads CPSR implicitly from hardware, but we have the value in a register
    // For now, just use R6 as a placeholder - the hardware CJMP reads CPSR directly
    Register CpsrReg = CpsrOp.getReg();
    // Copy CPSR value to R6 (CJMP uses R6 as link output, but reads CPSR from hardware)
    BuildMI(MBB, MI, DL, get(Cucaracha::MOV), Cucaracha::R6)
        .addReg(CpsrReg);

    // Load the mask immediate into R4
    unsigned Mask = MaskOp.getImm();
    BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), Cucaracha::R4)
        .addImm(Mask & 0xffff);
    if ((Mask >> 16) != 0) {
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
          .addReg(Cucaracha::R4, RegState::Define)
          .addImm((Mask >> 16) & 0xffff)
          .addReg(Cucaracha::R4);
    }

    // Load the block address into R5
    BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), Cucaracha::R5)
        .addMBB(Target.getMBB(), CucarachaII::MO_LO16);
    BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
        .addReg(Cucaracha::R5, RegState::Define)
        .addMBB(Target.getMBB(), CucarachaII::MO_HI16)
        .addReg(Cucaracha::R5);

    // Now use CJMP with the registers - CJMP takes (mask, target, link)
    // Use R6 as the link register output
    BuildMI(MBB, MI, DL, get(Cucaracha::CJMP))
        .addReg(Cucaracha::R6, RegState::Define)  // link output
        .addReg(Cucaracha::R4)                    // mask register
        .addReg(Cucaracha::R5);                   // target register

    MBB.erase(MI);
    return true;
  }
  case Cucaracha::MOVi32: {
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();

    const Register DstReg = MI.getOperand(0).getReg();
    const bool DstIsDead = MI.getOperand(0).isDead();

    const MachineOperand &MO = MI.getOperand(1);

    if (MO.isImm()) {
      const unsigned Imm = MO.getImm();
      const unsigned Lo16 = Imm & 0xffff;
      const unsigned Hi16 = (Imm >> 16) & 0xffff;
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), DstReg)
          .addImm(Lo16);
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
          .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
          .addImm(Hi16)
          .addReg(DstReg);
    } else {
      const GlobalValue *GV = MO.getGlobal();
      const unsigned TF = MO.getTargetFlags();
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16L), DstReg)
          .addGlobalAddress(GV, MO.getOffset(), TF | CucarachaII::MO_LO16);
      BuildMI(MBB, MI, DL, get(Cucaracha::MOVIMM16H))
          .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
          .addGlobalAddress(GV, MO.getOffset(), TF | CucarachaII::MO_HI16)
          .addReg(DstReg);
    }

    MBB.erase(MI);
    return true;
  }
  }
}
