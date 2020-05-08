//===-- FantasyAsmPrinter.cpp - Fantasy LLVM assembly writer
//------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the Fantasy assembly language.
//
//===----------------------------------------------------------------------===//

#include "FantasyAluCode.h"
#include "FantasyCondCode.h"
#include "FantasyInstrInfo.h"
#include "FantasyMCInstLower.h"
#include "FantasyTargetMachine.h"
#include "MCTargetDesc/FantasyInstPrinter.h"
#include "TargetInfo/FantasyTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "asm-printer"

using namespace llvm;

namespace {
class FantasyAsmPrinter : public AsmPrinter {
public:
  explicit FantasyAsmPrinter(TargetMachine &TM,
                             std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Fantasy Assembly Printer"; }

  void printOperand(const MachineInstr *MI, int OpNum, raw_ostream &O);
  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &O) override;
  void EmitInstruction(const MachineInstr *MI) override;
  bool isBlockOnlyReachableByFallthrough(
      const MachineBasicBlock *MBB) const override;

private:
  void customEmitInstruction(const MachineInstr *MI);
  void emitCallInstruction(const MachineInstr *MI);
};
} // end of anonymous namespace

void FantasyAsmPrinter::printOperand(const MachineInstr *MI, int OpNum,
                                     raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(OpNum);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << FantasyInstPrinter::getRegisterName(MO.getReg());
    break;

  case MachineOperand::MO_Immediate:
    O << MO.getImm();
    break;

  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol();
    break;

  case MachineOperand::MO_GlobalAddress:
    O << *getSymbol(MO.getGlobal());
    break;

  case MachineOperand::MO_BlockAddress: {
    MCSymbol *BA = GetBlockAddressSymbol(MO.getBlockAddress());
    O << BA->getName();
    break;
  }

  case MachineOperand::MO_ExternalSymbol:
    O << *GetExternalSymbolSymbol(MO.getSymbolName());
    break;

  case MachineOperand::MO_JumpTableIndex:
    O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber() << '_'
      << MO.getIndex();
    break;

  case MachineOperand::MO_ConstantPoolIndex:
    O << MAI->getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << '_'
      << MO.getIndex();
    return;

  default:
    llvm_unreachable("<unknown operand type>");
  }
}

// PrintAsmOperand - Print out an operand for an inline asm expression.
bool FantasyAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                        const char *ExtraCode, raw_ostream &O) {
  // Does this asm operand have a single letter operand modifier?
  if (ExtraCode && ExtraCode[0]) {
    if (ExtraCode[1])
      return true; // Unknown modifier.

    switch (ExtraCode[0]) {
    // The highest-numbered register of a pair.
    case 'H': {
      if (OpNo == 0)
        return true;
      const MachineOperand &FlagsOP = MI->getOperand(OpNo - 1);
      if (!FlagsOP.isImm())
        return true;
      unsigned Flags = FlagsOP.getImm();
      unsigned NumVals = InlineAsm::getNumOperandRegisters(Flags);
      if (NumVals != 2)
        return true;
      unsigned RegOp = OpNo + 1;
      if (RegOp >= MI->getNumOperands())
        return true;
      const MachineOperand &MO = MI->getOperand(RegOp);
      if (!MO.isReg())
        return true;
      Register Reg = MO.getReg();
      O << FantasyInstPrinter::getRegisterName(Reg);
      return false;
    }
    default:
      return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
    }
  }
  printOperand(MI, OpNo, O);
  return false;
}

//===----------------------------------------------------------------------===//
void FantasyAsmPrinter::emitCallInstruction(const MachineInstr *MI) {
  assert(
      (MI->getOpcode() == Fantasy::CALL || MI->getOpcode() == Fantasy::CALLR) &&
      "Unsupported call function");

  FantasyMCInstLower MCInstLowering(OutContext, *this);
  MCSubtargetInfo STI = getSubtargetInfo();
  // Insert save rca instruction immediately before the call.
  // TODO: We should generate a pc-relative mov instruction here instead
  // of pc + 16 (should be mov .+16 %rca).
  OutStreamer->EmitInstruction(MCInstBuilder(Fantasy::ADD_I_LO)
                                   .addReg(Fantasy::RCA)
                                   .addReg(Fantasy::PC)
                                   .addImm(16),
                               STI);

  // Push rca onto the stack.
  //   st %rca, [--%sp]
  OutStreamer->EmitInstruction(MCInstBuilder(Fantasy::SW_RI)
                                   .addReg(Fantasy::RCA)
                                   .addReg(Fantasy::SP)
                                   .addImm(-4)
                                   .addImm(LPAC::makePreOp(LPAC::ADD)),
                               STI);

  // Lower the call instruction.
  if (MI->getOpcode() == Fantasy::CALL) {
    MCInst TmpInst;
    MCInstLowering.Lower(MI, TmpInst);
    TmpInst.setOpcode(Fantasy::BT);
    OutStreamer->EmitInstruction(TmpInst, STI);
  } else {
    OutStreamer->EmitInstruction(MCInstBuilder(Fantasy::ADD_R)
                                     .addReg(Fantasy::PC)
                                     .addReg(MI->getOperand(0).getReg())
                                     .addReg(Fantasy::R0)
                                     .addImm(LPCC::ICC_T),
                                 STI);
  }
}

void FantasyAsmPrinter::customEmitInstruction(const MachineInstr *MI) {
  FantasyMCInstLower MCInstLowering(OutContext, *this);
  MCSubtargetInfo STI = getSubtargetInfo();
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer->EmitInstruction(TmpInst, STI);
}

void FantasyAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  MachineBasicBlock::const_instr_iterator I = MI->getIterator();
  MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();

  do {
    if (I->isCall()) {
      emitCallInstruction(&*I);
      continue;
    }

    customEmitInstruction(&*I);
  } while ((++I != E) && I->isInsideBundle());
}

// isBlockOnlyReachableByFallthough - Return true if the basic block has
// exactly one predecessor and the control transfer mechanism between
// the predecessor and this block is a fall-through.
// FIXME: could the overridden cases be handled in AnalyzeBranch?
bool FantasyAsmPrinter::isBlockOnlyReachableByFallthrough(
    const MachineBasicBlock *MBB) const {
  // The predecessor has to be immediately before this block.
  const MachineBasicBlock *Pred = *MBB->pred_begin();

  // If the predecessor is a switch statement, assume a jump table
  // implementation, so it is not a fall through.
  if (const BasicBlock *B = Pred->getBasicBlock())
    if (isa<SwitchInst>(B->getTerminator()))
      return false;

  // Check default implementation
  if (!AsmPrinter::isBlockOnlyReachableByFallthrough(MBB))
    return false;

  // Otherwise, check the last instruction.
  // Check if the last terminator is an unconditional branch.
  MachineBasicBlock::const_iterator I = Pred->end();
  while (I != Pred->begin() && !(--I)->isTerminator()) {
  }

  return !I->isBarrier();
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFantasyAsmPrinter() {
  RegisterAsmPrinter<FantasyAsmPrinter> X(getTheFantasyTarget());
}
