//===-- CucarachaELFObjectWriter.cpp - Cucaracha ELF Writer
//---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/CucarachaFixupKinds.h"
#include "MCTargetDesc/CucarachaMCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

using namespace llvm;

namespace {
class CucarachaELFObjectWriter : public MCELFObjectTargetWriter {
public:
  CucarachaELFObjectWriter(uint8_t OSABI);

  virtual ~CucarachaELFObjectWriter();

  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // namespace

unsigned CucarachaELFObjectWriter::getRelocType(MCContext &Ctx,
                                                const MCValue &Target,
                                                const MCFixup &Fixup,
                                                bool IsPCRel) const {
  if (!IsPCRel) {
    // We keep PC-relative relocations because these were already implemented in
    // LEG, but cucaracha programs are not meant to be relocatable since they
    // run inside a freestanding execution environment. We just basically
    // tell ELF to not bother relocating symbol references
    //
    // Anyway I might remove relocations altogether in the future just to
    // simplify the backend
    return ELF::R_ARM_NONE;
  }

  unsigned Type = 0;
  switch ((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("Unimplemented");
  case Cucaracha::fixup_cucaracha_mov_hi16_pcrel:
    Type = ELF::R_ARM_MOVT_PREL;
    break;
  case Cucaracha::fixup_cucaracha_mov_lo16_pcrel:
    Type = ELF::R_ARM_MOVW_PREL_NC;
    break;
  }
  return Type;
}

CucarachaELFObjectWriter::CucarachaELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI,
                              /*ELF::EM_Cucaracha*/ ELF::EM_ARM,
                              /*HasRelocationAddend*/ false) {}

CucarachaELFObjectWriter::~CucarachaELFObjectWriter() {}

std::unique_ptr<MCObjectTargetWriter>
llvm::createCucarachaELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<CucarachaELFObjectWriter>(OSABI);
}
