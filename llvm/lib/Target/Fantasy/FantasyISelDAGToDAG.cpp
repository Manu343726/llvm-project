//===-- FantasyISelDAGToDAG.cpp - A dag to dag inst selector for Fantasy
//------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Fantasy target.
//
//===----------------------------------------------------------------------===//

#include "FantasyAluCode.h"
#include "FantasyMachineFunctionInfo.h"
#include "FantasyRegisterInfo.h"
#include "FantasySubtarget.h"
#include "FantasyTargetMachine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "lanai-isel"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// FantasyDAGToDAGISel - Fantasy specific code to select Fantasy machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//
namespace {

class FantasyDAGToDAGISel : public SelectionDAGISel {
public:
  explicit FantasyDAGToDAGISel(FantasyTargetMachine &TargetMachine)
      : SelectionDAGISel(TargetMachine) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    return SelectionDAGISel::runOnMachineFunction(MF);
  }

  // Pass Name
  StringRef getPassName() const override {
    return "Fantasy DAG->DAG Pattern Instruction Selection";
  }

  bool SelectInlineAsmMemoryOperand(const SDValue &Op, unsigned ConstraintCode,
                                    std::vector<SDValue> &OutOps) override;

private:
// Include the pieces autogenerated from the target description.
#include "FantasyGenDAGISel.inc"

  // Instruction Selection not handled by the auto-generated tablgen
  void Select(SDNode *N) override;

  // Support functions for the opcodes of Instruction Selection
  // not handled by the auto-generated tablgen
  void selectFrameIndex(SDNode *N);

  // Complex Pattern for address selection.
  bool selectAddrRi(SDValue Addr, SDValue &Base, SDValue &Offset,
                    SDValue &AluOp);
  bool selectAddrRr(SDValue Addr, SDValue &R1, SDValue &R2, SDValue &AluOp);
  bool selectAddrSls(SDValue Addr, SDValue &Offset);
  bool selectAddrSpls(SDValue Addr, SDValue &Base, SDValue &Offset,
                      SDValue &AluOp);

  // getI32Imm - Return a target constant with the specified value, of type i32.
  inline SDValue getI32Imm(unsigned Imm, const SDLoc &DL) {
    return CurDAG->getTargetConstant(Imm, DL, MVT::i32);
  }

private:
  bool selectAddrRiSpls(SDValue Addr, SDValue &Base, SDValue &Offset,
                        SDValue &AluOp, bool RiMode);
};

bool canBeRepresentedAsSls(const ConstantSDNode &CN) {
  // Fits in 21-bit signed immediate and two low-order bits are zero.
  return isInt<21>(CN.getSExtValue()) && ((CN.getSExtValue() & 0x3) == 0);
}

} // namespace

// Helper functions for ComplexPattern used on FantasyInstrInfo
// Used on Fantasy Load/Store instructions.
bool FantasyDAGToDAGISel::selectAddrSls(SDValue Addr, SDValue &Offset) {
  if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr)) {
    SDLoc DL(Addr);
    // Loading from a constant address.
    if (canBeRepresentedAsSls(*CN)) {
      int32_t Imm = CN->getSExtValue();
      Offset = CurDAG->getTargetConstant(Imm, DL, CN->getValueType(0));
      return true;
    }
  }
  if (Addr.getOpcode() == ISD::OR &&
      Addr.getOperand(1).getOpcode() == FantasyISD::SMALL) {
    Offset = Addr.getOperand(1).getOperand(0);
    return true;
  }
  return false;
}

bool FantasyDAGToDAGISel::selectAddrRiSpls(SDValue Addr, SDValue &Base,
                                           SDValue &Offset, SDValue &AluOp,
                                           bool RiMode) {
  SDLoc DL(Addr);

  if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr)) {
    if (RiMode) {
      // Fits in 16-bit signed immediate.
      if (isInt<16>(CN->getSExtValue())) {
        int16_t Imm = CN->getSExtValue();
        Offset = CurDAG->getTargetConstant(Imm, DL, CN->getValueType(0));
        Base = CurDAG->getRegister(Fantasy::R0, CN->getValueType(0));
        AluOp = CurDAG->getTargetConstant(LPAC::ADD, DL, MVT::i32);
        return true;
      }
      // Allow SLS to match if the constant doesn't fit in 16 bits but can be
      // represented as an SLS.
      if (canBeRepresentedAsSls(*CN))
        return false;
    } else {
      // Fits in 10-bit signed immediate.
      if (isInt<10>(CN->getSExtValue())) {
        int16_t Imm = CN->getSExtValue();
        Offset = CurDAG->getTargetConstant(Imm, DL, CN->getValueType(0));
        Base = CurDAG->getRegister(Fantasy::R0, CN->getValueType(0));
        AluOp = CurDAG->getTargetConstant(LPAC::ADD, DL, MVT::i32);
        return true;
      }
    }
  }

  // if Address is FI, get the TargetFrameIndex.
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(
        FIN->getIndex(),
        getTargetLowering()->getPointerTy(CurDAG->getDataLayout()));
    Offset = CurDAG->getTargetConstant(0, DL, MVT::i32);
    AluOp = CurDAG->getTargetConstant(LPAC::ADD, DL, MVT::i32);
    return true;
  }

  // Skip direct calls
  if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
       Addr.getOpcode() == ISD::TargetGlobalAddress))
    return false;

  // Address of the form imm + reg
  ISD::NodeType AluOperator = static_cast<ISD::NodeType>(Addr.getOpcode());
  if (AluOperator == ISD::ADD) {
    AluOp = CurDAG->getTargetConstant(LPAC::ADD, DL, MVT::i32);
    // Addresses of the form FI+const
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1)))
      if ((RiMode && isInt<16>(CN->getSExtValue())) ||
          (!RiMode && isInt<10>(CN->getSExtValue()))) {
        // If the first operand is a FI, get the TargetFI Node
        if (FrameIndexSDNode *FIN =
                dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
          Base = CurDAG->getTargetFrameIndex(
              FIN->getIndex(),
              getTargetLowering()->getPointerTy(CurDAG->getDataLayout()));
        } else {
          Base = Addr.getOperand(0);
        }

        Offset = CurDAG->getTargetConstant(CN->getSExtValue(), DL, MVT::i32);
        return true;
      }
  }

  // Let SLS match SMALL instead of RI.
  if (AluOperator == ISD::OR && RiMode &&
      Addr.getOperand(1).getOpcode() == FantasyISD::SMALL)
    return false;

  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, DL, MVT::i32);
  AluOp = CurDAG->getTargetConstant(LPAC::ADD, DL, MVT::i32);
  return true;
}

bool FantasyDAGToDAGISel::selectAddrRi(SDValue Addr, SDValue &Base,
                                       SDValue &Offset, SDValue &AluOp) {
  return selectAddrRiSpls(Addr, Base, Offset, AluOp, /*RiMode=*/true);
}

bool FantasyDAGToDAGISel::selectAddrSpls(SDValue Addr, SDValue &Base,
                                         SDValue &Offset, SDValue &AluOp) {
  return selectAddrRiSpls(Addr, Base, Offset, AluOp, /*RiMode=*/false);
}

bool FantasyDAGToDAGISel::selectAddrRr(SDValue Addr, SDValue &R1, SDValue &R2,
                                       SDValue &AluOp) {
  // if Address is FI, get the TargetFrameIndex.
  if (Addr.getOpcode() == ISD::FrameIndex)
    return false;

  // Skip direct calls
  if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
       Addr.getOpcode() == ISD::TargetGlobalAddress))
    return false;

  // Address of the form OP + OP
  ISD::NodeType AluOperator = static_cast<ISD::NodeType>(Addr.getOpcode());
  LPAC::AluCode AluCode = LPAC::isdToFantasyAluCode(AluOperator);
  if (AluCode != LPAC::UNKNOWN) {
    // Skip addresses of the form FI OP const
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1)))
      if (isInt<16>(CN->getSExtValue()))
        return false;

    // Skip addresses with hi/lo operands
    if (Addr.getOperand(0).getOpcode() == FantasyISD::HI ||
        Addr.getOperand(0).getOpcode() == FantasyISD::LO ||
        Addr.getOperand(0).getOpcode() == FantasyISD::SMALL ||
        Addr.getOperand(1).getOpcode() == FantasyISD::HI ||
        Addr.getOperand(1).getOpcode() == FantasyISD::LO ||
        Addr.getOperand(1).getOpcode() == FantasyISD::SMALL)
      return false;

    // Addresses of the form register OP register
    R1 = Addr.getOperand(0);
    R2 = Addr.getOperand(1);
    AluOp = CurDAG->getTargetConstant(AluCode, SDLoc(Addr), MVT::i32);
    return true;
  }

  // Skip addresses with zero offset
  return false;
}

bool FantasyDAGToDAGISel::SelectInlineAsmMemoryOperand(
    const SDValue &Op, unsigned ConstraintCode, std::vector<SDValue> &OutOps) {
  SDValue Op0, Op1, AluOp;
  switch (ConstraintCode) {
  default:
    return true;
  case InlineAsm::Constraint_m: // memory
    if (!selectAddrRr(Op, Op0, Op1, AluOp) &&
        !selectAddrRi(Op, Op0, Op1, AluOp))
      return true;
    break;
  }

  OutOps.push_back(Op0);
  OutOps.push_back(Op1);
  OutOps.push_back(AluOp);
  return false;
}

// Select instructions not customized! Used for
// expanded, promoted and normal instructions
void FantasyDAGToDAGISel::Select(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    return;
  }

  // Instruction Selection not handled by the auto-generated tablegen selection
  // should be handled here.
  EVT VT = Node->getValueType(0);
  switch (Opcode) {
  case ISD::Constant:
    if (VT == MVT::i32) {
      ConstantSDNode *ConstNode = cast<ConstantSDNode>(Node);
      // Materialize zero constants as copies from R0. This allows the coalescer
      // to propagate these into other instructions.
      if (ConstNode->isNullValue()) {
        SDValue New = CurDAG->getCopyFromReg(
            CurDAG->getEntryNode(), SDLoc(Node), Fantasy::R0, MVT::i32);
        return ReplaceNode(Node, New.getNode());
      }
      // Materialize all ones constants as copies from R1. This allows the
      // coalescer to propagate these into other instructions.
      if (ConstNode->isAllOnesValue()) {
        SDValue New = CurDAG->getCopyFromReg(
            CurDAG->getEntryNode(), SDLoc(Node), Fantasy::R1, MVT::i32);
        return ReplaceNode(Node, New.getNode());
      }
    }
    break;
  case ISD::FrameIndex:
    selectFrameIndex(Node);
    return;
  default:
    break;
  }

  // Select the default instruction
  SelectCode(Node);
}

void FantasyDAGToDAGISel::selectFrameIndex(SDNode *Node) {
  SDLoc DL(Node);
  SDValue Imm = CurDAG->getTargetConstant(0, DL, MVT::i32);
  int FI = cast<FrameIndexSDNode>(Node)->getIndex();
  EVT VT = Node->getValueType(0);
  SDValue TFI = CurDAG->getTargetFrameIndex(FI, VT);
  unsigned Opc = Fantasy::ADD_I_LO;
  if (Node->hasOneUse()) {
    CurDAG->SelectNodeTo(Node, Opc, VT, TFI, Imm);
    return;
  }
  ReplaceNode(Node, CurDAG->getMachineNode(Opc, DL, VT, TFI, Imm));
}

// createFantasyISelDag - This pass converts a legalized DAG into a
// Fantasy-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createFantasyISelDag(FantasyTargetMachine &TM) {
  return new FantasyDAGToDAGISel(TM);
}
