//===-- CucarachaAsmBackend.cpp - Cucaracha Assembler Backend
//-------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/CucarachaFixupKinds.h"
#include "MCTargetDesc/CucarachaMCTargetDesc.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class CucarachaELFObjectWriter : public MCELFObjectTargetWriter {
public:
  CucarachaELFObjectWriter(uint8_t OSABI)
      : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI,
                                /*ELF::EM_Cucaracha*/ ELF::EM_ARM,
                                /*HasRelocationAddend*/ false) {}
};

class CucarachaAsmBackend : public MCAsmBackend {
public:
  CucarachaAsmBackend(const Target &T, const StringRef TT)
      : MCAsmBackend(support::endianness::little) {}

  ~CucarachaAsmBackend() {}

  unsigned getNumFixupKinds() const override {
    return Cucaracha::NumTargetFixupKinds;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[Cucaracha::NumTargetFixupKinds] = {
        // This table *must* be in the order that the fixup_* kinds are defined
        // in
        // CucarachaFixupKinds.h.
        //
        // Name                      Offset (bits) Size (bits)     Flags
        {"fixup_leg_cucaracha_hi16_pcrel", 0, 32, MCFixupKindInfo::FKF_IsPCRel},
        {"fixup_leg_cucaracha_lo16_pcrel", 0, 32, MCFixupKindInfo::FKF_IsPCRel},
    };

    if (Kind < FirstTargetFixupKind) {
      return MCAsmBackend::getFixupKindInfo(Kind);
    }

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
           "Invalid kind!");
    return Infos[Kind - FirstTargetFixupKind];
  }

  /// processFixupValue - Target hook to process the literal value of a fixup
  /// if necessary.
  bool evaluateTargetFixup(const MCAssembler &Asm, const MCAsmLayout &Layout,
                           const MCFixup &Fixup, const MCFragment *DF,
                           const MCValue &Target, uint64_t &Value,
                           bool &WasForced) override;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override {}

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override {
    if (Count == 0) {
      return true;
    }
    return false;
  }

  unsigned getPointerSize() const { return 4; }
};
} // end anonymous namespace

static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext *Ctx = NULL) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    // A normal fixup (usually as result of resolving an internal symbol when
    // finishing the object file) we return tbe value as is since we didn't ask
    // for the fixup as prt of iselowering orselves (Like the hi/lo16 cases
    // bellow which are explictly asked by our instruction lowering so 32 bit
    // immediate (usually addresses) move instructions can be encoded by means
    // of two consecutive 16 bit moves)
    return Value;
  case Cucaracha::fixup_cucaracha_mov_hi16_pcrel:
    Value >>= 16;
  // Intentional fall-through
  case Cucaracha::fixup_cucaracha_mov_lo16_pcrel:
    unsigned Hi4 = (Value & 0xF000) >> 12;
    unsigned Lo12 = Value & 0x0FFF;
    // inst{19-16} = Hi4;
    // inst{11-0} = Lo12;
    Value = (Hi4 << 16) | (Lo12);
    return Value;
  }
  return Value;
}

bool CucarachaAsmBackend::evaluateTargetFixup(
    const MCAssembler &Asm, const MCAsmLayout &Layout, const MCFixup &Fixup,
    const MCFragment *DF, const MCValue &Target, uint64_t &Value,
    bool &WasForced) {
  // We always have resolved fixups for now.
  WasForced = true;
  return adjustFixupValue(Fixup, Value, &Asm.getContext());
}

void CucarachaAsmBackend::applyFixup(const MCAssembler &Asm,
                                     const MCFixup &Fixup,
                                     const MCValue &Target,
                                     MutableArrayRef<char> Data, uint64_t Value,
                                     bool IsResolved,
                                     const MCSubtargetInfo *STI) const {
  unsigned NumBytes = 4;
  Value = adjustFixupValue(Fixup, Value);
  if (!Value) {
    return; // Doesn't change encoding.
  }

  unsigned Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the bits from
  // the fixup value. The Value has been "split up" into the appropriate
  // bitfields above.
  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

namespace {

class ELFCucarachaAsmBackend : public CucarachaAsmBackend {
public:
  uint8_t OSABI;
  ELFCucarachaAsmBackend(const Target &T, const StringRef TT, uint8_t _OSABI)
      : CucarachaAsmBackend(T, TT), OSABI(_OSABI) {}

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return llvm::createCucarachaELFObjectWriter(OSABI);
  }
};

} // namespace

MCAsmBackend *llvm::createCucarachaAsmBackend(const Target &T,
                                              const MCSubtargetInfo &STI,
                                              const MCRegisterInfo &MRI,
                                              const MCTargetOptions &Options) {
  const uint8_t ABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new ELFCucarachaAsmBackend(T, STI.getTargetTriple().getTriple(), ABI);
}