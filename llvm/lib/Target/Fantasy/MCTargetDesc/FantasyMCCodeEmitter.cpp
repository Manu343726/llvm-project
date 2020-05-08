//===-- FantasyMCCodeEmitter.cpp - Convert Fantasy code to machine code
//-------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the FantasyMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "FantasyAluCode.h"
#include "MCTargetDesc/FantasyBaseInfo.h"
#include "MCTargetDesc/FantasyFixupKinds.h"
#include "MCTargetDesc/FantasyMCExpr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <cstdint>

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace llvm {

namespace {

class FantasyMCCodeEmitter : public MCCodeEmitter {
public:
  FantasyMCCodeEmitter(const MCInstrInfo &MCII, MCContext &C) {}
  FantasyMCCodeEmitter(const FantasyMCCodeEmitter &) = delete;
  void operator=(const FantasyMCCodeEmitter &) = delete;
  ~FantasyMCCodeEmitter() override = default;

  // The functions below are called by TableGen generated functions for getting
  // the binary encoding of instructions/opereands.

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &Inst,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &SubtargetInfo) const;

  // getMachineOpValue - Return binary encoding of operand. If the machine
  // operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &Inst, const MCOperand &MCOp,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &SubtargetInfo) const;

  unsigned getRiMemoryOpValue(const MCInst &Inst, unsigned OpNo,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &SubtargetInfo) const;

  unsigned getRrMemoryOpValue(const MCInst &Inst, unsigned OpNo,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &SubtargetInfo) const;

  unsigned getSplsOpValue(const MCInst &Inst, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &SubtargetInfo) const;

  unsigned getBranchTargetOpValue(const MCInst &Inst, unsigned OpNo,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &SubtargetInfo) const;

  void encodeInstruction(const MCInst &Inst, raw_ostream &Ostream,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &SubtargetInfo) const override;

  unsigned adjustPqBitsRmAndRrm(const MCInst &Inst, unsigned Value,
                                const MCSubtargetInfo &STI) const;

  unsigned adjustPqBitsSpls(const MCInst &Inst, unsigned Value,
                            const MCSubtargetInfo &STI) const;
};

} // end anonymous namespace

static Fantasy::Fixups FixupKind(const MCExpr *Expr) {
  if (isa<MCSymbolRefExpr>(Expr))
    return Fantasy::FIXUP_LANAI_21;
  if (const FantasyMCExpr *McExpr = dyn_cast<FantasyMCExpr>(Expr)) {
    FantasyMCExpr::VariantKind ExprKind = McExpr->getKind();
    switch (ExprKind) {
    case FantasyMCExpr::VK_Fantasy_None:
      return Fantasy::FIXUP_LANAI_21;
    case FantasyMCExpr::VK_Fantasy_ABS_HI:
      return Fantasy::FIXUP_LANAI_HI16;
    case FantasyMCExpr::VK_Fantasy_ABS_LO:
      return Fantasy::FIXUP_LANAI_LO16;
    }
  }
  return Fantasy::Fixups(0);
}

// getMachineOpValue - Return binary encoding of operand. If the machine
// operand requires relocation, record the relocation and return zero.
unsigned FantasyMCCodeEmitter::getMachineOpValue(
    const MCInst &Inst, const MCOperand &MCOp, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  if (MCOp.isReg())
    return getFantasyRegisterNumbering(MCOp.getReg());
  if (MCOp.isImm())
    return static_cast<unsigned>(MCOp.getImm());

  // MCOp must be an expression
  assert(MCOp.isExpr());
  const MCExpr *Expr = MCOp.getExpr();

  // Extract the symbolic reference side of a binary expression.
  if (Expr->getKind() == MCExpr::Binary) {
    const MCBinaryExpr *BinaryExpr = static_cast<const MCBinaryExpr *>(Expr);
    Expr = BinaryExpr->getLHS();
  }

  assert(isa<FantasyMCExpr>(Expr) || Expr->getKind() == MCExpr::SymbolRef);
  // Push fixup (all info is contained within)
  Fixups.push_back(
      MCFixup::create(0, MCOp.getExpr(), MCFixupKind(FixupKind(Expr))));
  return 0;
}

// Helper function to adjust P and Q bits on load and store instructions.
static unsigned adjustPqBits(const MCInst &Inst, unsigned Value,
                             unsigned PBitShift, unsigned QBitShift) {
  const MCOperand AluOp = Inst.getOperand(3);
  unsigned AluCode = AluOp.getImm();

  // Set the P bit to one iff the immediate is nonzero and not a post-op
  // instruction.
  const MCOperand Op2 = Inst.getOperand(2);
  Value &= ~(1 << PBitShift);
  if (!LPAC::isPostOp(AluCode) &&
      ((Op2.isImm() && Op2.getImm() != 0) ||
       (Op2.isReg() && Op2.getReg() != Fantasy::R0) || (Op2.isExpr())))
    Value |= (1 << PBitShift);

  // Set the Q bit to one iff it is a post- or pre-op instruction.
  assert(Inst.getOperand(0).isReg() && Inst.getOperand(1).isReg() &&
         "Expected register operand.");
  Value &= ~(1 << QBitShift);
  if (LPAC::modifiesOp(AluCode) &&
      ((Op2.isImm() && Op2.getImm() != 0) ||
       (Op2.isReg() && Op2.getReg() != Fantasy::R0)))
    Value |= (1 << QBitShift);

  return Value;
}

unsigned
FantasyMCCodeEmitter::adjustPqBitsRmAndRrm(const MCInst &Inst, unsigned Value,
                                           const MCSubtargetInfo &STI) const {
  return adjustPqBits(Inst, Value, 17, 16);
}

unsigned
FantasyMCCodeEmitter::adjustPqBitsSpls(const MCInst &Inst, unsigned Value,
                                       const MCSubtargetInfo &STI) const {
  return adjustPqBits(Inst, Value, 11, 10);
}

void FantasyMCCodeEmitter::encodeInstruction(
    const MCInst &Inst, raw_ostream &Ostream, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  // Get instruction encoding and emit it
  unsigned Value = getBinaryCodeForInstr(Inst, Fixups, SubtargetInfo);
  ++MCNumEmitted; // Keep track of the number of emitted insns.

  // Emit bytes in big-endian
  for (int i = (4 - 1) * 8; i >= 0; i -= 8)
    Ostream << static_cast<char>((Value >> i) & 0xff);
}

// Encode Fantasy Memory Operand
unsigned FantasyMCCodeEmitter::getRiMemoryOpValue(
    const MCInst &Inst, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  unsigned Encoding;
  const MCOperand Op1 = Inst.getOperand(OpNo + 0);
  const MCOperand Op2 = Inst.getOperand(OpNo + 1);
  const MCOperand AluOp = Inst.getOperand(OpNo + 2);

  assert(Op1.isReg() && "First operand is not register.");
  assert((Op2.isImm() || Op2.isExpr()) &&
         "Second operand is neither an immediate nor an expression.");
  assert((LPAC::getAluOp(AluOp.getImm()) == LPAC::ADD) &&
         "Register immediate only supports addition operator");

  Encoding = (getFantasyRegisterNumbering(Op1.getReg()) << 18);
  if (Op2.isImm()) {
    assert(isInt<16>(Op2.getImm()) &&
           "Constant value truncated (limited to 16-bit)");

    Encoding |= (Op2.getImm() & 0xffff);
    if (Op2.getImm() != 0) {
      if (LPAC::isPreOp(AluOp.getImm()))
        Encoding |= (0x3 << 16);
      if (LPAC::isPostOp(AluOp.getImm()))
        Encoding |= (0x1 << 16);
    }
  } else
    getMachineOpValue(Inst, Op2, Fixups, SubtargetInfo);

  return Encoding;
}

unsigned FantasyMCCodeEmitter::getRrMemoryOpValue(
    const MCInst &Inst, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  unsigned Encoding;
  const MCOperand Op1 = Inst.getOperand(OpNo + 0);
  const MCOperand Op2 = Inst.getOperand(OpNo + 1);
  const MCOperand AluMCOp = Inst.getOperand(OpNo + 2);

  assert(Op1.isReg() && "First operand is not register.");
  Encoding = (getFantasyRegisterNumbering(Op1.getReg()) << 15);
  assert(Op2.isReg() && "Second operand is not register.");
  Encoding |= (getFantasyRegisterNumbering(Op2.getReg()) << 10);

  assert(AluMCOp.isImm() && "Third operator is not immediate.");
  // Set BBB
  unsigned AluOp = AluMCOp.getImm();
  Encoding |= LPAC::encodeFantasyAluCode(AluOp) << 5;
  // Set P and Q
  if (LPAC::isPreOp(AluOp))
    Encoding |= (0x3 << 8);
  if (LPAC::isPostOp(AluOp))
    Encoding |= (0x1 << 8);
  // Set JJJJ
  switch (LPAC::getAluOp(AluOp)) {
  case LPAC::SHL:
  case LPAC::SRL:
    Encoding |= 0x10;
    break;
  case LPAC::SRA:
    Encoding |= 0x18;
    break;
  default:
    break;
  }

  return Encoding;
}

unsigned FantasyMCCodeEmitter::getSplsOpValue(
    const MCInst &Inst, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  unsigned Encoding;
  const MCOperand Op1 = Inst.getOperand(OpNo + 0);
  const MCOperand Op2 = Inst.getOperand(OpNo + 1);
  const MCOperand AluOp = Inst.getOperand(OpNo + 2);

  assert(Op1.isReg() && "First operand is not register.");
  assert((Op2.isImm() || Op2.isExpr()) &&
         "Second operand is neither an immediate nor an expression.");
  assert((LPAC::getAluOp(AluOp.getImm()) == LPAC::ADD) &&
         "Register immediate only supports addition operator");

  Encoding = (getFantasyRegisterNumbering(Op1.getReg()) << 12);
  if (Op2.isImm()) {
    assert(isInt<10>(Op2.getImm()) &&
           "Constant value truncated (limited to 10-bit)");

    Encoding |= (Op2.getImm() & 0x3ff);
    if (Op2.getImm() != 0) {
      if (LPAC::isPreOp(AluOp.getImm()))
        Encoding |= (0x3 << 10);
      if (LPAC::isPostOp(AluOp.getImm()))
        Encoding |= (0x1 << 10);
    }
  } else
    getMachineOpValue(Inst, Op2, Fixups, SubtargetInfo);

  return Encoding;
}

unsigned FantasyMCCodeEmitter::getBranchTargetOpValue(
    const MCInst &Inst, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &SubtargetInfo) const {
  const MCOperand &MCOp = Inst.getOperand(OpNo);
  if (MCOp.isReg() || MCOp.isImm())
    return getMachineOpValue(Inst, MCOp, Fixups, SubtargetInfo);

  Fixups.push_back(MCFixup::create(
      0, MCOp.getExpr(), static_cast<MCFixupKind>(Fantasy::FIXUP_LANAI_25)));

  return 0;
}

#include "FantasyGenMCCodeEmitter.inc"

} // end namespace llvm

llvm::MCCodeEmitter *
llvm::createFantasyMCCodeEmitter(const MCInstrInfo &InstrInfo,
                                 const MCRegisterInfo & /*MRI*/,
                                 MCContext &context) {
  return new FantasyMCCodeEmitter(InstrInfo, context);
}
