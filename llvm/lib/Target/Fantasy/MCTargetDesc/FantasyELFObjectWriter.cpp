//===-- FantasyELFObjectWriter.cpp - Fantasy ELF Writer
//-----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/FantasyBaseInfo.h"
#include "MCTargetDesc/FantasyFixupKinds.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {

class FantasyELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit FantasyELFObjectWriter(uint8_t OSABI);

  ~FantasyELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
  bool needsRelocateWithSymbol(const MCSymbol &SD,
                               unsigned Type) const override;
};

} // end anonymous namespace

FantasyELFObjectWriter::FantasyELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit_=*/false, OSABI, ELF::EM_LANAI,
                              /*HasRelocationAddend_=*/true) {}

unsigned FantasyELFObjectWriter::getRelocType(MCContext & /*Ctx*/,
                                              const MCValue & /*Target*/,
                                              const MCFixup &Fixup,
                                              bool /*IsPCRel*/) const {
  unsigned Type;
  unsigned Kind = static_cast<unsigned>(Fixup.getKind());
  switch (Kind) {
  case Fantasy::FIXUP_LANAI_21:
    Type = ELF::R_LANAI_21;
    break;
  case Fantasy::FIXUP_LANAI_21_F:
    Type = ELF::R_LANAI_21_F;
    break;
  case Fantasy::FIXUP_LANAI_25:
    Type = ELF::R_LANAI_25;
    break;
  case Fantasy::FIXUP_LANAI_32:
  case FK_Data_4:
    Type = ELF::R_LANAI_32;
    break;
  case Fantasy::FIXUP_LANAI_HI16:
    Type = ELF::R_LANAI_HI16;
    break;
  case Fantasy::FIXUP_LANAI_LO16:
    Type = ELF::R_LANAI_LO16;
    break;
  case Fantasy::FIXUP_LANAI_NONE:
    Type = ELF::R_LANAI_NONE;
    break;

  default:
    llvm_unreachable("Invalid fixup kind!");
  }
  return Type;
}

bool FantasyELFObjectWriter::needsRelocateWithSymbol(const MCSymbol & /*SD*/,
                                                     unsigned Type) const {
  switch (Type) {
  case ELF::R_LANAI_21:
  case ELF::R_LANAI_21_F:
  case ELF::R_LANAI_25:
  case ELF::R_LANAI_32:
  case ELF::R_LANAI_HI16:
    return true;
  default:
    return false;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createFantasyELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<FantasyELFObjectWriter>(OSABI);
}
