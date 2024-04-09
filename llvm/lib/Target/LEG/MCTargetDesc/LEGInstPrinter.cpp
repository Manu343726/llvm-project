//===-- LEGInstPrinter.cpp - Convert LEG MCInst to assembly syntax ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an LEG MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSubtargetInfo.h"
#define DEBUG_TYPE "asm-printer"
#include "LEGInstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
#include "LEGGenAsmWriter.inc"

void LEGInstPrinter::printRegName(raw_ostream &OS, MCRegister RegNo) const {
  OS << StringRef(getRegisterName(RegNo)).lower();
}

void LEGInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  if (!printAliasInstr(MI, Address, STI, O))
    printInstruction(MI, Address, STI, O);
  printAnnotation(O, Annot);
}

namespace {
const char *condCodeToString(ISD::CondCode CC) {
  switch (CC) {
  default:
  case ISD::SETCC_INVALID:
  case ISD::SETFALSE:  //    0 0 0 0       Always false (always folded)
  case ISD::SETFALSE2: //  1 X 0 0 0       Always false (always folded)
  case ISD::SETOEQ:    //    0 0 0 1       True if ordered and equal
  case ISD::SETOGT:    //    0 0 1 0       True if ordered and greater than
  case ISD::SETOGE:    //    0 0 1 1       True if ordered and greater than or
                       //    equal
  case ISD::SETOLT:    //    0 1 0 0       True if ordered and less than
  case ISD::SETOLE:    //    0 1 0 1       True if ordered and less than or
                       //    equal
  case ISD::SETONE:    //    0 1 1 0       True if ordered and operands are
                       //    unequal
  case ISD::SETO:      //    0 1 1 1       True if ordered (no nans)
  case ISD::SETUO:     //    1 0 0 0       True if unordered: isnan(X) |
                       //    isnan(Y)
  case ISD::SETUEQ:    //    1 0 0 1       True if unordered or equal
  case ISD::SETUGT:    //    1 0 1 0       True if unordered or greater than
  case ISD::SETUGE:    //    1 0 1 1       True if unordered, greater than, or
                       //    equal
  case ISD::SETULT:    //    1 1 0 0       True if unordered or less than
  case ISD::SETULE:    //    1 1 0 1       True if unordered, less than, or
                       //    equal
  case ISD::SETUNE:    //    1 1 1 0       True if unordered or not equal
    llvm_unreachable("Invalid or unsupported condition code");
    return nullptr;

  case ISD::SETTRUE:  //    1 1 1 1       Always true (always folded)
  case ISD::SETTRUE2: //  1 X 1 1 1       Always true (always folded)
    return "";

  // Don't care operations: undefined if the input is a nan.
  case ISD::SETEQ: //  1 X 0 0 1       True if equal
    return "eq";
  case ISD::SETGT: //  1 X 0 1 0       True if greater than
    return "gt";
  case ISD::SETGE: //  1 X 0 1 1       True if greater than or equal
    return "ge";
  case ISD::SETLT: //  1 X 1 0 0       True if less than
    return "lt";
  case ISD::SETLE: //  1 X 1 0 1       True if less than or equal
    return "le";
  case ISD::SETNE: //  1 X 1 1 0       True if not equal
    return "ne";
  }
}
} // namespace
// Print a condition code (e.g. for predication).
void LEGInstPrinter::printCondCode(const MCInst *MI, unsigned OpNum,
                                   const MCSubtargetInfo &STI, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNum);
  ISD::CondCode CC = (ISD::CondCode)Op.getImm();
  const char *Str = condCodeToString(CC);
  O << Str;
}

// Print a 'memsrc' operand which is a (Register, Offset) pair.
void LEGInstPrinter::printAddrModeMemSrc(const MCInst *MI, unsigned OpNum,
                                         const MCSubtargetInfo &STI,
                                         raw_ostream &O) {
  const MCOperand &Op1 = MI->getOperand(OpNum);
  const MCOperand &Op2 = MI->getOperand(OpNum + 1);
  O << "[";
  printRegName(O, Op1.getReg());

  unsigned Offset = Op2.getImm();
  if (Offset) {
    O << ", #" << Offset;
  }
  O << "]";
}

void LEGInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  const MCSubtargetInfo &STI, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << "#" << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  Op.getExpr()->print(O, &MAI);
}
