//===-- CucarachaISelLowering.h - Cucaracha DAG Lowering Interface ------*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Cucaracha uses to lower LLVM code into
// a selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef CucarachaISELLOWERING_H
#define CucarachaISELLOWERING_H

#include "Cucaracha.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

// Forward delcarations
class CucarachaSubtarget;
class CucarachaTargetMachine;

namespace CucarachaISD {
enum NodeType {
  // Start the numbering where the builtin ops and target ops leave off.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  // This loads the symbol (e.g. global address) into a register.
  LOAD_SYM,
  // This loads a 32-bit immediate into a register.
  MOVEi32,
  CALL,
  // Select condition code
  SELECT_CC,
  // Compare two values and set CPSR flags
  CMP,
  // Conditional branch: BR_COND mask, target
  // Semantics: if (CPSR & mask) != 0 { PC = target }
  BR_COND
};
}

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//
class CucarachaTargetLowering : public TargetLowering {
public:
  explicit CucarachaTargetLowering(CucarachaTargetMachine &TM);

  /// LowerOperation - Provide custom lowering hooks for some operations.
  virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
  virtual const char *getTargetNodeName(unsigned Opcode) const override;

  /// EmitInstrWithCustomInserter - Expand SELECT and SELECT_CC pseudos.
  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *BB) const override;

private:
  const CucarachaSubtarget &Subtarget;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &dl, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &dl,
                      SelectionDAG &DAG) const override;

  SDValue LowerCallResult(SDValue Chain, SDValue InGlue,
                          CallingConv::ID CallConv, bool isVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &ArgsFlags,
                      LLVMContext &Context) const override;

  // LowerGlobalAddress - Emit a constant load to the global address.
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  // LowerBRCOND - Custom lower conditional branch.
  SDValue LowerBRCOND(SDValue Op, SelectionDAG &DAG) const;

  // LowerBR_CC - Custom lower branch with condition code.
  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;

  // LowerSELECT_CC - Custom lower select with condition code.
  SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;

  // Allow constant memcpy source data to be inlined as immediate stores.
  // This avoids the need for load/store sequences when copying constant arrays.
  bool shouldConvertConstantLoadToIntImm(const APInt &Imm,
                                          Type *Ty) const override {
    return true;
  }
};
} // namespace llvm

#endif // CucarachaISELLOWERING_H
