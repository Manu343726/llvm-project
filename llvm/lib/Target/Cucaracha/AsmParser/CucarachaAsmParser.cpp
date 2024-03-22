//===-- CucarachaAsmParser.cpp - Parse Cucaracha assembly to MCInst instructions
//--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/CucarachaMCExpr.h"
#include "MCTargetDesc/CucarachaMCTargetDesc.h"
#include "TargetInfo/CucarachaTargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>

using namespace llvm;

// The generated AsmMatcher CucarachaGenAsmMatcher uses "Cucaracha" as the
// target namespace. But Cucaracha backend uses "SP" as its namespace.
namespace llvm {
namespace Cucaracha {

using namespace SP;

} // end namespace Cucaracha
} // end namespace llvm

namespace {

class CucarachaOperand;

class CucarachaAsmParser : public MCTargetAsmParser {
  MCAsmParser &Parser;

  enum class TailRelocKind { Load_GOT, Add_TLS, Load_TLS, Call_TLS };

  /// @name Auto-generated Match Functions
  /// {

#define GET_ASSEMBLER_HEADER
#include "CucarachaGenAsmMatcher.inc"

  /// }

  // public interface of the MCTargetAsmParser.
  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
  bool parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                     SMLoc &EndLoc) override;
  OperandMatchResultTy tryParseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;
  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  ParseStatus parseDirective(AsmToken DirectiveID) override;

  unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                      unsigned Kind) override;

  // Custom parse functions for Cucaracha specific operands.
  OperandMatchResultTy parseMEMOperand(OperandVector &Operands);

  OperandMatchResultTy parseMembarTag(OperandVector &Operands);

  template <TailRelocKind Kind>
  OperandMatchResultTy parseTailRelocSym(OperandVector &Operands);

  template <unsigned N>
  OperandMatchResultTy parseShiftAmtImm(OperandVector &Operands);

  OperandMatchResultTy parseCallTarget(OperandVector &Operands);

  OperandMatchResultTy parseOperand(OperandVector &Operands, StringRef Name);

  OperandMatchResultTy
  parseCucarachaAsmOperand(std::unique_ptr<CucarachaOperand> &Operand,
                           bool isCall = false);

  OperandMatchResultTy parseBranchModifiers(OperandVector &Operands);

  // Helper function for dealing with %lo / %hi in PIC mode.
  const CucarachaMCExpr *adjustPICRelocation(CucarachaMCExpr::VariantKind VK,
                                             const MCExpr *subExpr);

  // returns true if Tok is matched to a register and returns register in RegNo.
  bool matchRegisterName(const AsmToken &Tok, MCRegister &RegNo,
                         unsigned &RegKind);

  bool matchCucarachaAsmModifiers(const MCExpr *&EVal, SMLoc &EndLoc);

  bool is64Bit() const {
    // return getSTI().getTargetTriple().getArch() == Triple::cucarachav9;
    return false;
  }

  bool expandSET(MCInst &Inst, SMLoc IDLoc,
                 SmallVectorImpl<MCInst> &Instructions);

  SMLoc getLoc() const { return getParser().getTok().getLoc(); }

public:
  CucarachaAsmParser(const MCSubtargetInfo &sti, MCAsmParser &parser,
                     const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, sti, MII), Parser(parser) {
    Parser.addAliasForDirective(".half", ".2byte");
    Parser.addAliasForDirective(".uahalf", ".2byte");
    Parser.addAliasForDirective(".word", ".4byte");
    Parser.addAliasForDirective(".uaword", ".4byte");
    Parser.addAliasForDirective(".nword", is64Bit() ? ".8byte" : ".4byte");
    if (is64Bit())
      Parser.addAliasForDirective(".xword", ".8byte");

    // Initialize the set of available features.
    setAvailableFeatures(ComputeAvailableFeatures(getSTI().getFeatureBits()));
  }
};

} // end anonymous namespace

static const MCPhysReg IntRegs[32] = {
    Cucaracha::G0, Cucaracha::G1, Cucaracha::G2, Cucaracha::G3, Cucaracha::G4,
    Cucaracha::G5, Cucaracha::G6, Cucaracha::G7, Cucaracha::O0, Cucaracha::O1,
    Cucaracha::O2, Cucaracha::O3, Cucaracha::O4, Cucaracha::O5, Cucaracha::O6,
    Cucaracha::O7, Cucaracha::L0, Cucaracha::L1, Cucaracha::L2, Cucaracha::L3,
    Cucaracha::L4, Cucaracha::L5, Cucaracha::L6, Cucaracha::L7, Cucaracha::I0,
    Cucaracha::I1, Cucaracha::I2, Cucaracha::I3, Cucaracha::I4, Cucaracha::I5,
    Cucaracha::I6, Cucaracha::I7};

static const MCPhysReg FloatRegs[32] = {
    Cucaracha::F0,  Cucaracha::F1,  Cucaracha::F2,  Cucaracha::F3,
    Cucaracha::F4,  Cucaracha::F5,  Cucaracha::F6,  Cucaracha::F7,
    Cucaracha::F8,  Cucaracha::F9,  Cucaracha::F10, Cucaracha::F11,
    Cucaracha::F12, Cucaracha::F13, Cucaracha::F14, Cucaracha::F15,
    Cucaracha::F16, Cucaracha::F17, Cucaracha::F18, Cucaracha::F19,
    Cucaracha::F20, Cucaracha::F21, Cucaracha::F22, Cucaracha::F23,
    Cucaracha::F24, Cucaracha::F25, Cucaracha::F26, Cucaracha::F27,
    Cucaracha::F28, Cucaracha::F29, Cucaracha::F30, Cucaracha::F31};

static const MCPhysReg DoubleRegs[32] = {
    Cucaracha::D0,  Cucaracha::D1,  Cucaracha::D2,  Cucaracha::D3,
    Cucaracha::D4,  Cucaracha::D5,  Cucaracha::D6,  Cucaracha::D7,
    Cucaracha::D8,  Cucaracha::D9,  Cucaracha::D10, Cucaracha::D11,
    Cucaracha::D12, Cucaracha::D13, Cucaracha::D14, Cucaracha::D15,
    Cucaracha::D16, Cucaracha::D17, Cucaracha::D18, Cucaracha::D19,
    Cucaracha::D20, Cucaracha::D21, Cucaracha::D22, Cucaracha::D23,
    Cucaracha::D24, Cucaracha::D25, Cucaracha::D26, Cucaracha::D27,
    Cucaracha::D28, Cucaracha::D29, Cucaracha::D30, Cucaracha::D31};

static const MCPhysReg QuadFPRegs[32] = {
    Cucaracha::Q0,  Cucaracha::Q1,  Cucaracha::Q2,  Cucaracha::Q3,
    Cucaracha::Q4,  Cucaracha::Q5,  Cucaracha::Q6,  Cucaracha::Q7,
    Cucaracha::Q8,  Cucaracha::Q9,  Cucaracha::Q10, Cucaracha::Q11,
    Cucaracha::Q12, Cucaracha::Q13, Cucaracha::Q14, Cucaracha::Q15};

static const MCPhysReg ASRRegs[32] = {
    SP::Y,     SP::ASR1,  SP::ASR2,  SP::ASR3,  SP::ASR4,  SP::ASR5,  SP::ASR6,
    SP::ASR7,  SP::ASR8,  SP::ASR9,  SP::ASR10, SP::ASR11, SP::ASR12, SP::ASR13,
    SP::ASR14, SP::ASR15, SP::ASR16, SP::ASR17, SP::ASR18, SP::ASR19, SP::ASR20,
    SP::ASR21, SP::ASR22, SP::ASR23, SP::ASR24, SP::ASR25, SP::ASR26, SP::ASR27,
    SP::ASR28, SP::ASR29, SP::ASR30, SP::ASR31};

static const MCPhysReg IntPairRegs[] = {
    Cucaracha::G0_G1, Cucaracha::G2_G3, Cucaracha::G4_G5, Cucaracha::G6_G7,
    Cucaracha::O0_O1, Cucaracha::O2_O3, Cucaracha::O4_O5, Cucaracha::O6_O7,
    Cucaracha::L0_L1, Cucaracha::L2_L3, Cucaracha::L4_L5, Cucaracha::L6_L7,
    Cucaracha::I0_I1, Cucaracha::I2_I3, Cucaracha::I4_I5, Cucaracha::I6_I7};

static const MCPhysReg CoprocRegs[32] = {
    Cucaracha::C0,  Cucaracha::C1,  Cucaracha::C2,  Cucaracha::C3,
    Cucaracha::C4,  Cucaracha::C5,  Cucaracha::C6,  Cucaracha::C7,
    Cucaracha::C8,  Cucaracha::C9,  Cucaracha::C10, Cucaracha::C11,
    Cucaracha::C12, Cucaracha::C13, Cucaracha::C14, Cucaracha::C15,
    Cucaracha::C16, Cucaracha::C17, Cucaracha::C18, Cucaracha::C19,
    Cucaracha::C20, Cucaracha::C21, Cucaracha::C22, Cucaracha::C23,
    Cucaracha::C24, Cucaracha::C25, Cucaracha::C26, Cucaracha::C27,
    Cucaracha::C28, Cucaracha::C29, Cucaracha::C30, Cucaracha::C31};

static const MCPhysReg CoprocPairRegs[] = {
    Cucaracha::C0_C1,   Cucaracha::C2_C3,   Cucaracha::C4_C5,
    Cucaracha::C6_C7,   Cucaracha::C8_C9,   Cucaracha::C10_C11,
    Cucaracha::C12_C13, Cucaracha::C14_C15, Cucaracha::C16_C17,
    Cucaracha::C18_C19, Cucaracha::C20_C21, Cucaracha::C22_C23,
    Cucaracha::C24_C25, Cucaracha::C26_C27, Cucaracha::C28_C29,
    Cucaracha::C30_C31};

namespace {

/// CucarachaOperand - Instances of this class represent a parsed Cucaracha
/// machine instruction.
class CucarachaOperand : public MCParsedAsmOperand {
public:
  enum RegisterKind {
    rk_None,
    rk_IntReg,
    rk_IntPairReg,
    rk_FloatReg,
    rk_DoubleReg,
    rk_QuadReg,
    rk_CoprocReg,
    rk_CoprocPairReg,
    rk_Special,
  };

private:
  enum KindTy {
    k_Token,
    k_Register,
    k_Immediate,
    k_MemoryReg,
    k_MemoryImm
  } Kind;

  SMLoc StartLoc, EndLoc;

  struct Token {
    const char *Data;
    unsigned Length;
  };

  struct RegOp {
    unsigned RegNum;
    RegisterKind Kind;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  struct MemOp {
    unsigned Base;
    unsigned OffsetReg;
    const MCExpr *Off;
  };

  union {
    struct Token Tok;
    struct RegOp Reg;
    struct ImmOp Imm;
    struct MemOp Mem;
  };

public:
  CucarachaOperand(KindTy K) : Kind(K) {}

  bool isToken() const override { return Kind == k_Token; }
  bool isReg() const override { return Kind == k_Register; }
  bool isImm() const override { return Kind == k_Immediate; }
  bool isMem() const override { return isMEMrr() || isMEMri(); }
  bool isMEMrr() const { return Kind == k_MemoryReg; }
  bool isMEMri() const { return Kind == k_MemoryImm; }
  bool isMembarTag() const { return Kind == k_Immediate; }
  bool isTailRelocSym() const { return Kind == k_Immediate; }

  bool isCallTarget() const {
    if (!isImm())
      return false;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Imm.Val))
      return CE->getValue() % 4 == 0;

    return true;
  }

  bool isShiftAmtImm5() const {
    if (!isImm())
      return false;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Imm.Val))
      return isUInt<5>(CE->getValue());

    return false;
  }

  bool isShiftAmtImm6() const {
    if (!isImm())
      return false;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Imm.Val))
      return isUInt<6>(CE->getValue());

    return false;
  }

  bool isIntReg() const {
    return (Kind == k_Register && Reg.Kind == rk_IntReg);
  }

  bool isFloatReg() const {
    return (Kind == k_Register && Reg.Kind == rk_FloatReg);
  }

  bool isFloatOrDoubleReg() const {
    return (Kind == k_Register &&
            (Reg.Kind == rk_FloatReg || Reg.Kind == rk_DoubleReg));
  }

  bool isCoprocReg() const {
    return (Kind == k_Register && Reg.Kind == rk_CoprocReg);
  }

  StringRef getToken() const {
    assert(Kind == k_Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  unsigned getReg() const override {
    assert((Kind == k_Register) && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert((Kind == k_Immediate) && "Invalid access!");
    return Imm.Val;
  }

  unsigned getMemBase() const {
    assert((Kind == k_MemoryReg || Kind == k_MemoryImm) && "Invalid access!");
    return Mem.Base;
  }

  unsigned getMemOffsetReg() const {
    assert((Kind == k_MemoryReg) && "Invalid access!");
    return Mem.OffsetReg;
  }

  const MCExpr *getMemOff() const {
    assert((Kind == k_MemoryImm) && "Invalid access!");
    return Mem.Off;
  }

  /// getStartLoc - Get the location of the first token of this operand.
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Get the location of the last token of this operand.
  SMLoc getEndLoc() const override { return EndLoc; }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case k_Token:
      OS << "Token: " << getToken() << "\n";
      break;
    case k_Register:
      OS << "Reg: #" << getReg() << "\n";
      break;
    case k_Immediate:
      OS << "Imm: " << getImm() << "\n";
      break;
    case k_MemoryReg:
      OS << "Mem: " << getMemBase() << "+" << getMemOffsetReg() << "\n";
      break;
    case k_MemoryImm:
      assert(getMemOff() != nullptr);
      OS << "Mem: " << getMemBase() << "+" << *getMemOff() << "\n";
      break;
    }
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCExpr *Expr = getImm();
    addExpr(Inst, Expr);
  }

  void addShiftAmtImm5Operands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }
  void addShiftAmtImm6Operands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediate when possible.  Null MCExpr = 0.
    if (!Expr)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addMEMrrOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");

    Inst.addOperand(MCOperand::createReg(getMemBase()));

    assert(getMemOffsetReg() != 0 && "Invalid offset");
    Inst.addOperand(MCOperand::createReg(getMemOffsetReg()));
  }

  void addMEMriOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");

    Inst.addOperand(MCOperand::createReg(getMemBase()));

    const MCExpr *Expr = getMemOff();
    addExpr(Inst, Expr);
  }

  void addMembarTagOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCExpr *Expr = getImm();
    addExpr(Inst, Expr);
  }

  void addCallTargetOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  void addTailRelocSymOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  static std::unique_ptr<CucarachaOperand> CreateToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<CucarachaOperand>(k_Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<CucarachaOperand>
  CreateReg(unsigned RegNum, unsigned Kind, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<CucarachaOperand>(k_Register);
    Op->Reg.RegNum = RegNum;
    Op->Reg.Kind = (CucarachaOperand::RegisterKind)Kind;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<CucarachaOperand> CreateImm(const MCExpr *Val, SMLoc S,
                                                     SMLoc E) {
    auto Op = std::make_unique<CucarachaOperand>(k_Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static bool MorphToIntPairReg(CucarachaOperand &Op) {
    unsigned Reg = Op.getReg();
    assert(Op.Reg.Kind == rk_IntReg);
    unsigned regIdx = 32;
    if (Reg >= Cucaracha::G0 && Reg <= Cucaracha::G7)
      regIdx = Reg - Cucaracha::G0;
    else if (Reg >= Cucaracha::O0 && Reg <= Cucaracha::O7)
      regIdx = Reg - Cucaracha::O0 + 8;
    else if (Reg >= Cucaracha::L0 && Reg <= Cucaracha::L7)
      regIdx = Reg - Cucaracha::L0 + 16;
    else if (Reg >= Cucaracha::I0 && Reg <= Cucaracha::I7)
      regIdx = Reg - Cucaracha::I0 + 24;
    if (regIdx % 2 || regIdx > 31)
      return false;
    Op.Reg.RegNum = IntPairRegs[regIdx / 2];
    Op.Reg.Kind = rk_IntPairReg;
    return true;
  }

  static bool MorphToDoubleReg(CucarachaOperand &Op) {
    unsigned Reg = Op.getReg();
    assert(Op.Reg.Kind == rk_FloatReg);
    unsigned regIdx = Reg - Cucaracha::F0;
    if (regIdx % 2 || regIdx > 31)
      return false;
    Op.Reg.RegNum = DoubleRegs[regIdx / 2];
    Op.Reg.Kind = rk_DoubleReg;
    return true;
  }

  static bool MorphToQuadReg(CucarachaOperand &Op) {
    unsigned Reg = Op.getReg();
    unsigned regIdx = 0;
    switch (Op.Reg.Kind) {
    default:
      llvm_unreachable("Unexpected register kind!");
    case rk_FloatReg:
      regIdx = Reg - Cucaracha::F0;
      if (regIdx % 4 || regIdx > 31)
        return false;
      Reg = QuadFPRegs[regIdx / 4];
      break;
    case rk_DoubleReg:
      regIdx = Reg - Cucaracha::D0;
      if (regIdx % 2 || regIdx > 31)
        return false;
      Reg = QuadFPRegs[regIdx / 2];
      break;
    }
    Op.Reg.RegNum = Reg;
    Op.Reg.Kind = rk_QuadReg;
    return true;
  }

  static bool MorphToCoprocPairReg(CucarachaOperand &Op) {
    unsigned Reg = Op.getReg();
    assert(Op.Reg.Kind == rk_CoprocReg);
    unsigned regIdx = 32;
    if (Reg >= Cucaracha::C0 && Reg <= Cucaracha::C31)
      regIdx = Reg - Cucaracha::C0;
    if (regIdx % 2 || regIdx > 31)
      return false;
    Op.Reg.RegNum = CoprocPairRegs[regIdx / 2];
    Op.Reg.Kind = rk_CoprocPairReg;
    return true;
  }

  static std::unique_ptr<CucarachaOperand>
  MorphToMEMrr(unsigned Base, std::unique_ptr<CucarachaOperand> Op) {
    unsigned offsetReg = Op->getReg();
    Op->Kind = k_MemoryReg;
    Op->Mem.Base = Base;
    Op->Mem.OffsetReg = offsetReg;
    Op->Mem.Off = nullptr;
    return Op;
  }

  static std::unique_ptr<CucarachaOperand> CreateMEMr(unsigned Base, SMLoc S,
                                                      SMLoc E) {
    auto Op = std::make_unique<CucarachaOperand>(k_MemoryReg);
    Op->Mem.Base = Base;
    Op->Mem.OffsetReg = Cucaracha::G0; // always 0
    Op->Mem.Off = nullptr;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<CucarachaOperand>
  MorphToMEMri(unsigned Base, std::unique_ptr<CucarachaOperand> Op) {
    const MCExpr *Imm = Op->getImm();
    Op->Kind = k_MemoryImm;
    Op->Mem.Base = Base;
    Op->Mem.OffsetReg = 0;
    Op->Mem.Off = Imm;
    return Op;
  }
};

} // end anonymous namespace

bool CucarachaAsmParser::expandSET(MCInst &Inst, SMLoc IDLoc,
                                   SmallVectorImpl<MCInst> &Instructions) {
  MCOperand MCRegOp = Inst.getOperand(0);
  MCOperand MCValOp = Inst.getOperand(1);
  assert(MCRegOp.isReg());
  assert(MCValOp.isImm() || MCValOp.isExpr());

  // the imm operand can be either an expression or an immediate.
  bool IsImm = Inst.getOperand(1).isImm();
  int64_t RawImmValue = IsImm ? MCValOp.getImm() : 0;

  // Allow either a signed or unsigned 32-bit immediate.
  if (RawImmValue < -2147483648LL || RawImmValue > 4294967295LL) {
    return Error(IDLoc,
                 "set: argument must be between -2147483648 and 4294967295");
  }

  // If the value was expressed as a large unsigned number, that's ok.
  // We want to see if it "looks like" a small signed number.
  int32_t ImmValue = RawImmValue;
  // For 'set' you can't use 'or' with a negative operand on V9 because
  // that would splat the sign bit across the upper half of the destination
  // register, whereas 'set' is defined to zero the high 32 bits.
  bool IsEffectivelyImm13 =
      IsImm && ((is64Bit() ? 0 : -4096) <= ImmValue && ImmValue < 4096);
  const MCExpr *ValExpr;
  if (IsImm)
    ValExpr = MCConstantExpr::create(ImmValue, getContext());
  else
    ValExpr = MCValOp.getExpr();

  MCOperand PrevReg = MCOperand::createReg(Cucaracha::G0);

  // If not just a signed imm13 value, then either we use a 'sethi' with a
  // following 'or', or a 'sethi' by itself if there are no more 1 bits.
  // In either case, start with the 'sethi'.
  if (!IsEffectivelyImm13) {
    MCInst TmpInst;
    const MCExpr *Expr =
        adjustPICRelocation(CucarachaMCExpr::VK_CUCARACHA_HI, ValExpr);
    TmpInst.setLoc(IDLoc);
    TmpInst.setOpcode(SP::SETHIi);
    TmpInst.addOperand(MCRegOp);
    TmpInst.addOperand(MCOperand::createExpr(Expr));
    Instructions.push_back(TmpInst);
    PrevReg = MCRegOp;
  }

  // The low bits require touching in 3 cases:
  // * A non-immediate value will always require both instructions.
  // * An effectively imm13 value needs only an 'or' instruction.
  // * Otherwise, an immediate that is not effectively imm13 requires the
  //   'or' only if bits remain after clearing the 22 bits that 'sethi' set.
  // If the low bits are known zeros, there's nothing to do.
  // In the second case, and only in that case, must we NOT clear
  // bits of the immediate value via the %lo() assembler function.
  // Note also, the 'or' instruction doesn't mind a large value in the case
  // where the operand to 'set' was 0xFFFFFzzz - it does exactly what you mean.
  if (!IsImm || IsEffectivelyImm13 || (ImmValue & 0x3ff)) {
    MCInst TmpInst;
    const MCExpr *Expr;
    if (IsEffectivelyImm13)
      Expr = ValExpr;
    else
      Expr = adjustPICRelocation(CucarachaMCExpr::VK_CUCARACHA_LO, ValExpr);
    TmpInst.setLoc(IDLoc);
    TmpInst.setOpcode(SP::ORri);
    TmpInst.addOperand(MCRegOp);
    TmpInst.addOperand(PrevReg);
    TmpInst.addOperand(MCOperand::createExpr(Expr));
    Instructions.push_back(TmpInst);
  }
  return false;
}

bool CucarachaAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                                 OperandVector &Operands,
                                                 MCStreamer &Out,
                                                 uint64_t &ErrorInfo,
                                                 bool MatchingInlineAsm) {
  MCInst Inst;
  SmallVector<MCInst, 8> Instructions;
  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);
  switch (MatchResult) {
  case Match_Success: {
    switch (Inst.getOpcode()) {
    default:
      Inst.setLoc(IDLoc);
      Instructions.push_back(Inst);
      break;
    case SP::SET:
      if (expandSET(Inst, IDLoc, Instructions))
        return true;
      break;
    }

    for (const MCInst &I : Instructions) {
      Out.emitInstruction(I, getSTI());
    }
    return false;
  }

  case Match_MissingFeature:
    return Error(IDLoc,
                 "instruction requires a CPU feature not currently enabled");

  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0ULL) {
      if (ErrorInfo >= Operands.size())
        return Error(IDLoc, "too few operands for instruction");

      ErrorLoc = ((CucarachaOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }

    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_MnemonicFail:
    return Error(IDLoc, "invalid instruction mnemonic");
  }
  llvm_unreachable("Implement any new match types added!");
}

bool CucarachaAsmParser::parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                                       SMLoc &EndLoc) {
  if (tryParseRegister(RegNo, StartLoc, EndLoc) != MatchOperand_Success)
    return Error(StartLoc, "invalid register name");
  return false;
}

OperandMatchResultTy CucarachaAsmParser::tryParseRegister(MCRegister &RegNo,
                                                          SMLoc &StartLoc,
                                                          SMLoc &EndLoc) {
  const AsmToken &Tok = Parser.getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  RegNo = 0;
  if (getLexer().getKind() != AsmToken::Percent)
    return MatchOperand_NoMatch;
  Parser.Lex();
  unsigned regKind = CucarachaOperand::rk_None;
  if (matchRegisterName(Tok, RegNo, regKind)) {
    Parser.Lex();
    return MatchOperand_Success;
  }

  getLexer().UnLex(Tok);
  return MatchOperand_NoMatch;
}

static void applyMnemonicAliases(StringRef &Mnemonic,
                                 const FeatureBitset &Features,
                                 unsigned VariantID);

bool CucarachaAsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                          StringRef Name, SMLoc NameLoc,
                                          OperandVector &Operands) {

  // First operand in MCInst is instruction mnemonic.
  Operands.push_back(CucarachaOperand::CreateToken(Name, NameLoc));

  // apply mnemonic aliases, if any, so that we can parse operands correctly.
  applyMnemonicAliases(Name, getAvailableFeatures(), 0);

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    // Read the first operand.
    if (getLexer().is(AsmToken::Comma)) {
      if (parseBranchModifiers(Operands) != MatchOperand_Success) {
        SMLoc Loc = getLexer().getLoc();
        return Error(Loc, "unexpected token");
      }
    }
    if (parseOperand(Operands, Name) != MatchOperand_Success) {
      SMLoc Loc = getLexer().getLoc();
      return Error(Loc, "unexpected token");
    }

    while (getLexer().is(AsmToken::Comma) || getLexer().is(AsmToken::Plus)) {
      if (getLexer().is(AsmToken::Plus)) {
        // Plus tokens are significant in software_traps (p83, cucarachav8.pdf).
        // We must capture them.
        Operands.push_back(
            CucarachaOperand::CreateToken("+", Parser.getTok().getLoc()));
      }
      Parser.Lex(); // Eat the comma or plus.
      // Parse and remember the operand.
      if (parseOperand(Operands, Name) != MatchOperand_Success) {
        SMLoc Loc = getLexer().getLoc();
        return Error(Loc, "unexpected token");
      }
    }
  }
  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    return Error(Loc, "unexpected token");
  }
  Parser.Lex(); // Consume the EndOfStatement.
  return false;
}

ParseStatus CucarachaAsmParser::parseDirective(AsmToken DirectiveID) {
  StringRef IDVal = DirectiveID.getString();

  if (IDVal == ".register") {
    // For now, ignore .register directive.
    Parser.eatToEndOfStatement();
    return ParseStatus::Success;
  }
  if (IDVal == ".proc") {
    // For compatibility, ignore this directive.
    // (It's supposed to be an "optimization" in the Sun assembler)
    Parser.eatToEndOfStatement();
    return ParseStatus::Success;
  }

  // Let the MC layer to handle other directives.
  return ParseStatus::NoMatch;
}

OperandMatchResultTy
CucarachaAsmParser::parseMEMOperand(OperandVector &Operands) {
  SMLoc S, E;

  std::unique_ptr<CucarachaOperand> LHS;
  if (parseCucarachaAsmOperand(LHS) != MatchOperand_Success)
    return MatchOperand_NoMatch;

  // Single immediate operand
  if (LHS->isImm()) {
    Operands.push_back(
        CucarachaOperand::MorphToMEMri(Cucaracha::G0, std::move(LHS)));
    return MatchOperand_Success;
  }

  if (!LHS->isIntReg()) {
    Error(LHS->getStartLoc(), "invalid register kind for this operand");
    return MatchOperand_ParseFail;
  }

  AsmToken Tok = getLexer().getTok();
  // The plus token may be followed by a register or an immediate value, the
  // minus one is always interpreted as sign for the immediate value
  if (Tok.is(AsmToken::Plus) || Tok.is(AsmToken::Minus)) {
    (void)Parser.parseOptionalToken(AsmToken::Plus);

    std::unique_ptr<CucarachaOperand> RHS;
    if (parseCucarachaAsmOperand(RHS) != MatchOperand_Success)
      return MatchOperand_NoMatch;

    if (RHS->isReg() && !RHS->isIntReg()) {
      Error(RHS->getStartLoc(), "invalid register kind for this operand");
      return MatchOperand_ParseFail;
    }

    Operands.push_back(
        RHS->isImm()
            ? CucarachaOperand::MorphToMEMri(LHS->getReg(), std::move(RHS))
            : CucarachaOperand::MorphToMEMrr(LHS->getReg(), std::move(RHS)));

    return MatchOperand_Success;
  }

  Operands.push_back(CucarachaOperand::CreateMEMr(LHS->getReg(), S, E));
  return MatchOperand_Success;
}

template <unsigned N>
OperandMatchResultTy
CucarachaAsmParser::parseShiftAmtImm(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);

  // This is a register, not an immediate
  if (getLexer().getKind() == AsmToken::Percent)
    return MatchOperand_NoMatch;

  const MCExpr *Expr;
  if (getParser().parseExpression(Expr))
    return MatchOperand_ParseFail;

  const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr);
  if (!CE) {
    Error(S, "constant expression expected");
    return MatchOperand_ParseFail;
  }

  if (!isUInt<N>(CE->getValue())) {
    Error(S, "immediate shift value out of range");
    return MatchOperand_ParseFail;
  }

  Operands.push_back(CucarachaOperand::CreateImm(Expr, S, E));
  return MatchOperand_Success;
}

template <CucarachaAsmParser::TailRelocKind Kind>
OperandMatchResultTy
CucarachaAsmParser::parseTailRelocSym(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);

  auto MatchesKind = [](CucarachaMCExpr::VariantKind VK) -> bool {
    switch (Kind) {
    case TailRelocKind::Load_GOT:
      // Non-TLS relocations on ld (or ldx).
      // ld [%rr + %rr], %rr, %rel(sym)
      return VK == CucarachaMCExpr::VK_CUCARACHA_GOTDATA_OP;
    case TailRelocKind::Add_TLS:
      // TLS relocations on add.
      // add %rr, %rr, %rr, %rel(sym)
      switch (VK) {
      case CucarachaMCExpr::VK_CUCARACHA_TLS_GD_ADD:
      case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_ADD:
      case CucarachaMCExpr::VK_CUCARACHA_TLS_LDM_ADD:
      case CucarachaMCExpr::VK_CUCARACHA_TLS_LDO_ADD:
        return true;
      default:
        return false;
      }
    case TailRelocKind::Load_TLS:
      // TLS relocations on ld (or ldx).
      // ld[x] %addr, %rr, %rel(sym)
      switch (VK) {
      case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_LD:
      case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_LDX:
        return true;
      default:
        return false;
      }
    case TailRelocKind::Call_TLS:
      // TLS relocations on call.
      // call sym, %rel(sym)
      switch (VK) {
      case CucarachaMCExpr::VK_CUCARACHA_TLS_GD_CALL:
      case CucarachaMCExpr::VK_CUCARACHA_TLS_LDM_CALL:
        return true;
      default:
        return false;
      }
    }
    llvm_unreachable("Unhandled CucarachaAsmParser::TailRelocKind enum");
  };

  if (getLexer().getKind() != AsmToken::Percent) {
    Error(getLoc(), "expected '%' for operand modifier");
    return MatchOperand_ParseFail;
  }

  const AsmToken Tok = Parser.getTok();
  getParser().Lex(); // Eat '%'

  if (getLexer().getKind() != AsmToken::Identifier) {
    Error(getLoc(), "expected valid identifier for operand modifier");
    return MatchOperand_ParseFail;
  }

  StringRef Name = getParser().getTok().getIdentifier();
  CucarachaMCExpr::VariantKind VK = CucarachaMCExpr::parseVariantKind(Name);
  if (VK == CucarachaMCExpr::VK_CUCARACHA_None) {
    Error(getLoc(), "invalid operand modifier");
    return MatchOperand_ParseFail;
  }

  if (!MatchesKind(VK)) {
    // Did not match the specified set of relocation types, put '%' back.
    getLexer().UnLex(Tok);
    return MatchOperand_NoMatch;
  }

  Parser.Lex(); // Eat the identifier.
  if (getLexer().getKind() != AsmToken::LParen) {
    Error(getLoc(), "expected '('");
    return MatchOperand_ParseFail;
  }

  getParser().Lex(); // Eat '('
  const MCExpr *SubExpr;
  if (getParser().parseParenExpression(SubExpr, E)) {
    return MatchOperand_ParseFail;
  }

  const MCExpr *Val = adjustPICRelocation(VK, SubExpr);
  Operands.push_back(CucarachaOperand::CreateImm(Val, S, E));
  return MatchOperand_Success;
}

OperandMatchResultTy
CucarachaAsmParser::parseMembarTag(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  const MCExpr *EVal;
  int64_t ImmVal = 0;

  std::unique_ptr<CucarachaOperand> Mask;
  if (parseCucarachaAsmOperand(Mask) == MatchOperand_Success) {
    if (!Mask->isImm() || !Mask->getImm()->evaluateAsAbsolute(ImmVal) ||
        ImmVal < 0 || ImmVal > 127) {
      Error(S, "invalid membar mask number");
      return MatchOperand_ParseFail;
    }
  }

  while (getLexer().getKind() == AsmToken::Hash) {
    SMLoc TagStart = getLexer().getLoc();
    Parser.Lex(); // Eat the '#'.
    unsigned MaskVal = StringSwitch<unsigned>(Parser.getTok().getString())
                           .Case("LoadLoad", 0x1)
                           .Case("StoreLoad", 0x2)
                           .Case("LoadStore", 0x4)
                           .Case("StoreStore", 0x8)
                           .Case("Lookaside", 0x10)
                           .Case("MemIssue", 0x20)
                           .Case("Sync", 0x40)
                           .Default(0);

    Parser.Lex(); // Eat the identifier token.

    if (!MaskVal) {
      Error(TagStart, "unknown membar tag");
      return MatchOperand_ParseFail;
    }

    ImmVal |= MaskVal;

    if (getLexer().getKind() == AsmToken::Pipe)
      Parser.Lex(); // Eat the '|'.
  }

  EVal = MCConstantExpr::create(ImmVal, getContext());
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(CucarachaOperand::CreateImm(EVal, S, E));
  return MatchOperand_Success;
}

OperandMatchResultTy
CucarachaAsmParser::parseCallTarget(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);

  switch (getLexer().getKind()) {
  default:
    return MatchOperand_NoMatch;
  case AsmToken::LParen:
  case AsmToken::Integer:
  case AsmToken::Identifier:
  case AsmToken::Dot:
    break;
  }

  const MCExpr *DestValue;
  if (getParser().parseExpression(DestValue))
    return MatchOperand_NoMatch;

  bool IsPic = getContext().getObjectFileInfo()->isPositionIndependent();
  CucarachaMCExpr::VariantKind Kind =
      IsPic ? CucarachaMCExpr::VK_CUCARACHA_WPLT30
            : CucarachaMCExpr::VK_CUCARACHA_WDISP30;

  const MCExpr *DestExpr =
      CucarachaMCExpr::create(Kind, DestValue, getContext());
  Operands.push_back(CucarachaOperand::CreateImm(DestExpr, S, E));
  return MatchOperand_Success;
}

OperandMatchResultTy CucarachaAsmParser::parseOperand(OperandVector &Operands,
                                                      StringRef Mnemonic) {

  OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);

  // If there wasn't a custom match, try the generic matcher below. Otherwise,
  // there was a match, but an error occurred, in which case, just return that
  // the operand parsing failed.
  if (ResTy == MatchOperand_Success || ResTy == MatchOperand_ParseFail)
    return ResTy;

  if (getLexer().is(AsmToken::LBrac)) {
    // Memory operand
    Operands.push_back(
        CucarachaOperand::CreateToken("[", Parser.getTok().getLoc()));
    Parser.Lex(); // Eat the [

    if (Mnemonic == "cas" || Mnemonic == "casx" || Mnemonic == "casa") {
      SMLoc S = Parser.getTok().getLoc();
      if (getLexer().getKind() != AsmToken::Percent)
        return MatchOperand_NoMatch;
      Parser.Lex(); // eat %

      MCRegister RegNo;
      unsigned RegKind;
      if (!matchRegisterName(Parser.getTok(), RegNo, RegKind))
        return MatchOperand_NoMatch;

      Parser.Lex(); // Eat the identifier token.
      SMLoc E =
          SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
      Operands.push_back(CucarachaOperand::CreateReg(RegNo, RegKind, S, E));
      ResTy = MatchOperand_Success;
    } else {
      ResTy = parseMEMOperand(Operands);
    }

    if (ResTy != MatchOperand_Success)
      return ResTy;

    if (!getLexer().is(AsmToken::RBrac))
      return MatchOperand_ParseFail;

    Operands.push_back(
        CucarachaOperand::CreateToken("]", Parser.getTok().getLoc()));
    Parser.Lex(); // Eat the ]

    // Parse an optional address-space identifier after the address.
    if (getLexer().is(AsmToken::Integer)) {
      std::unique_ptr<CucarachaOperand> Op;
      ResTy = parseCucarachaAsmOperand(Op, false);
      if (ResTy != MatchOperand_Success || !Op)
        return MatchOperand_ParseFail;
      Operands.push_back(std::move(Op));
    }
    return MatchOperand_Success;
  }

  std::unique_ptr<CucarachaOperand> Op;

  ResTy = parseCucarachaAsmOperand(Op, (Mnemonic == "call"));
  if (ResTy != MatchOperand_Success || !Op)
    return MatchOperand_ParseFail;

  // Push the parsed operand into the list of operands
  Operands.push_back(std::move(Op));

  return MatchOperand_Success;
}

OperandMatchResultTy CucarachaAsmParser::parseCucarachaAsmOperand(
    std::unique_ptr<CucarachaOperand> &Op, bool isCall) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  const MCExpr *EVal;

  Op = nullptr;
  switch (getLexer().getKind()) {
  default:
    break;

  case AsmToken::Percent: {
    Parser.Lex(); // Eat the '%'.
    MCRegister RegNo;
    unsigned RegKind;
    if (matchRegisterName(Parser.getTok(), RegNo, RegKind)) {
      StringRef name = Parser.getTok().getString();
      Parser.Lex(); // Eat the identifier token.
      E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
      switch (RegNo) {
      default:
        Op = CucarachaOperand::CreateReg(RegNo, RegKind, S, E);
        break;
      case Cucaracha::PSR:
        Op = CucarachaOperand::CreateToken("%psr", S);
        break;
      case Cucaracha::FSR:
        Op = CucarachaOperand::CreateToken("%fsr", S);
        break;
      case Cucaracha::FQ:
        Op = CucarachaOperand::CreateToken("%fq", S);
        break;
      case Cucaracha::CPSR:
        Op = CucarachaOperand::CreateToken("%csr", S);
        break;
      case Cucaracha::CPQ:
        Op = CucarachaOperand::CreateToken("%cq", S);
        break;
      case Cucaracha::WIM:
        Op = CucarachaOperand::CreateToken("%wim", S);
        break;
      case Cucaracha::TBR:
        Op = CucarachaOperand::CreateToken("%tbr", S);
        break;
      case Cucaracha::PC:
        Op = CucarachaOperand::CreateToken("%pc", S);
        break;
      case Cucaracha::ICC:
        if (name == "xcc")
          Op = CucarachaOperand::CreateToken("%xcc", S);
        else
          Op = CucarachaOperand::CreateToken("%icc", S);
        break;
      }
      break;
    }
    if (matchCucarachaAsmModifiers(EVal, E)) {
      E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
      Op = CucarachaOperand::CreateImm(EVal, S, E);
    }
    break;
  }

  case AsmToken::Plus:
  case AsmToken::Minus:
  case AsmToken::Integer:
  case AsmToken::LParen:
  case AsmToken::Dot:
  case AsmToken::Identifier:
    if (getParser().parseExpression(EVal, E))
      break;

    int64_t Res;
    if (!EVal->evaluateAsAbsolute(Res)) {
      CucarachaMCExpr::VariantKind Kind = CucarachaMCExpr::VK_CUCARACHA_13;

      if (getContext().getObjectFileInfo()->isPositionIndependent()) {
        if (isCall)
          Kind = CucarachaMCExpr::VK_CUCARACHA_WPLT30;
        else
          Kind = CucarachaMCExpr::VK_CUCARACHA_GOT13;
      }
      EVal = CucarachaMCExpr::create(Kind, EVal, getContext());
    }
    Op = CucarachaOperand::CreateImm(EVal, S, E);
    break;
  }
  return (Op) ? MatchOperand_Success : MatchOperand_ParseFail;
}

OperandMatchResultTy
CucarachaAsmParser::parseBranchModifiers(OperandVector &Operands) {
  // parse (,a|,pn|,pt)+

  while (getLexer().is(AsmToken::Comma)) {
    Parser.Lex(); // Eat the comma

    if (!getLexer().is(AsmToken::Identifier))
      return MatchOperand_ParseFail;
    StringRef modName = Parser.getTok().getString();
    if (modName == "a" || modName == "pn" || modName == "pt") {
      Operands.push_back(
          CucarachaOperand::CreateToken(modName, Parser.getTok().getLoc()));
      Parser.Lex(); // eat the identifier.
    }
  }
  return MatchOperand_Success;
}

bool CucarachaAsmParser::matchRegisterName(const AsmToken &Tok,
                                           MCRegister &RegNo,
                                           unsigned &RegKind) {
  int64_t intVal = 0;
  RegNo = 0;
  RegKind = CucarachaOperand::rk_None;
  if (Tok.is(AsmToken::Identifier)) {
    StringRef name = Tok.getString();

    // %fp
    if (name.equals("fp")) {
      RegNo = Cucaracha::I6;
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }
    // %sp
    if (name.equals("sp")) {
      RegNo = Cucaracha::O6;
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }

    if (name.equals("y")) {
      RegNo = Cucaracha::Y;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.substr(0, 3).equals_insensitive("asr") &&
        !name.substr(3).getAsInteger(10, intVal) && intVal > 0 && intVal < 32) {
      RegNo = ASRRegs[intVal];
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    // %fprs is an alias of %asr6.
    if (name.equals("fprs")) {
      RegNo = ASRRegs[6];
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("icc")) {
      RegNo = Cucaracha::ICC;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("psr")) {
      RegNo = Cucaracha::PSR;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("fsr")) {
      RegNo = Cucaracha::FSR;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("fq")) {
      RegNo = Cucaracha::FQ;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("csr")) {
      RegNo = Cucaracha::CPSR;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("cq")) {
      RegNo = Cucaracha::CPQ;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("wim")) {
      RegNo = Cucaracha::WIM;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("tbr")) {
      RegNo = Cucaracha::TBR;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    if (name.equals("xcc")) {
      // FIXME:: check 64bit.
      RegNo = Cucaracha::ICC;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    // %fcc0 - %fcc3
    if (name.substr(0, 3).equals_insensitive("fcc") &&
        !name.substr(3).getAsInteger(10, intVal) && intVal < 4) {
      // FIXME: check 64bit and  handle %fcc1 - %fcc3
      RegNo = Cucaracha::FCC0 + intVal;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }

    // %g0 - %g7
    if (name.substr(0, 1).equals_insensitive("g") &&
        !name.substr(1).getAsInteger(10, intVal) && intVal < 8) {
      RegNo = IntRegs[intVal];
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }
    // %o0 - %o7
    if (name.substr(0, 1).equals_insensitive("o") &&
        !name.substr(1).getAsInteger(10, intVal) && intVal < 8) {
      RegNo = IntRegs[8 + intVal];
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }
    if (name.substr(0, 1).equals_insensitive("l") &&
        !name.substr(1).getAsInteger(10, intVal) && intVal < 8) {
      RegNo = IntRegs[16 + intVal];
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }
    if (name.substr(0, 1).equals_insensitive("i") &&
        !name.substr(1).getAsInteger(10, intVal) && intVal < 8) {
      RegNo = IntRegs[24 + intVal];
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }
    // %f0 - %f31
    if (name.substr(0, 1).equals_insensitive("f") &&
        !name.substr(1, 2).getAsInteger(10, intVal) && intVal < 32) {
      RegNo = FloatRegs[intVal];
      RegKind = CucarachaOperand::rk_FloatReg;
      return true;
    }
    // %f32 - %f62
    if (name.substr(0, 1).equals_insensitive("f") &&
        !name.substr(1, 2).getAsInteger(10, intVal) && intVal >= 32 &&
        intVal <= 62 && (intVal % 2 == 0)) {
      // FIXME: Check V9
      RegNo = DoubleRegs[intVal / 2];
      RegKind = CucarachaOperand::rk_DoubleReg;
      return true;
    }

    // %r0 - %r31
    if (name.substr(0, 1).equals_insensitive("r") &&
        !name.substr(1, 2).getAsInteger(10, intVal) && intVal < 31) {
      RegNo = IntRegs[intVal];
      RegKind = CucarachaOperand::rk_IntReg;
      return true;
    }

    // %c0 - %c31
    if (name.substr(0, 1).equals_insensitive("c") &&
        !name.substr(1).getAsInteger(10, intVal) && intVal < 32) {
      RegNo = CoprocRegs[intVal];
      RegKind = CucarachaOperand::rk_CoprocReg;
      return true;
    }

    if (name.equals("tpc")) {
      RegNo = Cucaracha::TPC;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tnpc")) {
      RegNo = Cucaracha::TNPC;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tstate")) {
      RegNo = Cucaracha::TSTATE;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tt")) {
      RegNo = Cucaracha::TT;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tick")) {
      RegNo = Cucaracha::TICK;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tba")) {
      RegNo = Cucaracha::TBA;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("pstate")) {
      RegNo = Cucaracha::PSTATE;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("tl")) {
      RegNo = Cucaracha::TL;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("pil")) {
      RegNo = Cucaracha::PIL;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("cwp")) {
      RegNo = Cucaracha::CWP;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("cansave")) {
      RegNo = Cucaracha::CANSAVE;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("canrestore")) {
      RegNo = Cucaracha::CANRESTORE;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("cleanwin")) {
      RegNo = Cucaracha::CLEANWIN;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("otherwin")) {
      RegNo = Cucaracha::OTHERWIN;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("wstate")) {
      RegNo = Cucaracha::WSTATE;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
    if (name.equals("pc")) {
      RegNo = Cucaracha::PC;
      RegKind = CucarachaOperand::rk_Special;
      return true;
    }
  }
  return false;
}

// Determine if an expression contains a reference to the symbol
// "_GLOBAL_OFFSET_TABLE_".
static bool hasGOTReference(const MCExpr *Expr) {
  switch (Expr->getKind()) {
  case MCExpr::Target:
    if (const CucarachaMCExpr *SE = dyn_cast<CucarachaMCExpr>(Expr))
      return hasGOTReference(SE->getSubExpr());
    break;

  case MCExpr::Constant:
    break;

  case MCExpr::Binary: {
    const MCBinaryExpr *BE = cast<MCBinaryExpr>(Expr);
    return hasGOTReference(BE->getLHS()) || hasGOTReference(BE->getRHS());
  }

  case MCExpr::SymbolRef: {
    const MCSymbolRefExpr &SymRef = *cast<MCSymbolRefExpr>(Expr);
    return (SymRef.getSymbol().getName() == "_GLOBAL_OFFSET_TABLE_");
  }

  case MCExpr::Unary:
    return hasGOTReference(cast<MCUnaryExpr>(Expr)->getSubExpr());
  }
  return false;
}

const CucarachaMCExpr *
CucarachaAsmParser::adjustPICRelocation(CucarachaMCExpr::VariantKind VK,
                                        const MCExpr *subExpr) {
  // When in PIC mode, "%lo(...)" and "%hi(...)" behave differently.
  // If the expression refers contains _GLOBAL_OFFSET_TABLE, it is
  // actually a %pc10 or %pc22 relocation. Otherwise, they are interpreted
  // as %got10 or %got22 relocation.

  if (getContext().getObjectFileInfo()->isPositionIndependent()) {
    switch (VK) {
    default:
      break;
    case CucarachaMCExpr::VK_CUCARACHA_LO:
      VK = (hasGOTReference(subExpr) ? CucarachaMCExpr::VK_CUCARACHA_PC10
                                     : CucarachaMCExpr::VK_CUCARACHA_GOT10);
      break;
    case CucarachaMCExpr::VK_CUCARACHA_HI:
      VK = (hasGOTReference(subExpr) ? CucarachaMCExpr::VK_CUCARACHA_PC22
                                     : CucarachaMCExpr::VK_CUCARACHA_GOT22);
      break;
    }
  }

  return CucarachaMCExpr::create(VK, subExpr, getContext());
}

bool CucarachaAsmParser::matchCucarachaAsmModifiers(const MCExpr *&EVal,
                                                    SMLoc &EndLoc) {
  AsmToken Tok = Parser.getTok();
  if (!Tok.is(AsmToken::Identifier))
    return false;

  StringRef name = Tok.getString();

  CucarachaMCExpr::VariantKind VK = CucarachaMCExpr::parseVariantKind(name);
  switch (VK) {
  case CucarachaMCExpr::VK_CUCARACHA_None:
    Error(getLoc(), "invalid operand modifier");
    return false;

  case CucarachaMCExpr::VK_CUCARACHA_GOTDATA_OP:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_GD_ADD:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_GD_CALL:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_ADD:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_LD:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_IE_LDX:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_LDM_ADD:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_LDM_CALL:
  case CucarachaMCExpr::VK_CUCARACHA_TLS_LDO_ADD:
    // These are special-cased at tablegen level.
    return false;

  default:
    break;
  }

  Parser.Lex(); // Eat the identifier.
  if (Parser.getTok().getKind() != AsmToken::LParen)
    return false;

  Parser.Lex(); // Eat the LParen token.
  const MCExpr *subExpr;
  if (Parser.parseParenExpression(subExpr, EndLoc))
    return false;

  EVal = adjustPICRelocation(VK, subExpr);
  return true;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCucarachaAsmParser() {
  RegisterMCAsmParser<CucarachaAsmParser> A(getTheCucarachaTarget());
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "CucarachaGenAsmMatcher.inc"

unsigned CucarachaAsmParser::validateTargetOperandClass(MCParsedAsmOperand &GOp,
                                                        unsigned Kind) {
  CucarachaOperand &Op = (CucarachaOperand &)GOp;
  if (Op.isFloatOrDoubleReg()) {
    switch (Kind) {
    default:
      break;
    case MCK_DFPRegs:
      if (!Op.isFloatReg() || CucarachaOperand::MorphToDoubleReg(Op))
        return MCTargetAsmParser::Match_Success;
      break;
    case MCK_QFPRegs:
      if (CucarachaOperand::MorphToQuadReg(Op))
        return MCTargetAsmParser::Match_Success;
      break;
    }
  }
  if (Op.isIntReg() && Kind == MCK_IntPair) {
    if (CucarachaOperand::MorphToIntPairReg(Op))
      return MCTargetAsmParser::Match_Success;
  }
  if (Op.isCoprocReg() && Kind == MCK_CoprocPair) {
    if (CucarachaOperand::MorphToCoprocPairReg(Op))
      return MCTargetAsmParser::Match_Success;
  }
  return Match_InvalidOperand;
}
