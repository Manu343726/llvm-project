//===-- CucarachaELFObjectWriter.cpp - Cucaracha ELF Writer
//-----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/CucarachaFixupKinds.h"
#include "MCTargetDesc/CucarachaMCExpr.h"
#include "MCTargetDesc/CucarachaMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class CucarachaELFObjectWriter : public MCELFObjectTargetWriter {
public:
  CucarachaELFObjectWriter(bool Is64Bit, uint8_t OSABI)
      : MCELFObjectTargetWriter(
            Is64Bit, OSABI, Is64Bit ? ELF::EM_CucarachaV9 : ELF::EM_Cucaracha,
            /*HasRelocationAddend*/ true) {}

  ~CucarachaELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;

  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override;
};
} // namespace

unsigned CucarachaELFObjectWriter::getRelocType(MCContext &Ctx,
                                                const MCValue &Target,
                                                const MCFixup &Fixup,
                                                bool IsPCRel) const {
  MCFixupKind Kind = Fixup.getKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;

  if (const CucarachaMCExpr *SExpr =
          dyn_cast<CucarachaMCExpr>(Fixup.getValue())) {
    if (SExpr->getKind() == CucarachaMCExpr::VK_Cucaracha_R_DISP32)
      return ELF::R_Cucaracha_DISP32;
  }

  if (IsPCRel) {
    switch (Fixup.getTargetKind()) {
    default:
      llvm_unreachable("Unimplemented fixup -> relocation");
    case FK_Data_1:
      return ELF::R_Cucaracha_DISP8;
    case FK_Data_2:
      return ELF::R_Cucaracha_DISP16;
    case FK_Data_4:
      return ELF::R_Cucaracha_DISP32;
    case FK_Data_8:
      return ELF::R_Cucaracha_DISP64;
    case Cucaracha::fixup_cucaracha_call30:
      return ELF::R_Cucaracha_WDISP30;
    case Cucaracha::fixup_cucaracha_br22:
      return ELF::R_Cucaracha_WDISP22;
    case Cucaracha::fixup_cucaracha_br19:
      return ELF::R_Cucaracha_WDISP19;
    case Cucaracha::fixup_cucaracha_br16:
      return ELF::R_Cucaracha_WDISP16;
    case Cucaracha::fixup_cucaracha_pc22:
      return ELF::R_Cucaracha_PC22;
    case Cucaracha::fixup_cucaracha_pc10:
      return ELF::R_Cucaracha_PC10;
    case Cucaracha::fixup_cucaracha_wplt30:
      return ELF::R_Cucaracha_WPLT30;
    }
  }

  switch (Fixup.getTargetKind()) {
  default:
    llvm_unreachable("Unimplemented fixup -> relocation");
  case FK_NONE:
    return ELF::R_Cucaracha_NONE;
  case FK_Data_1:
    return ELF::R_Cucaracha_8;
  case FK_Data_2:
    return ((Fixup.getOffset() % 2) ? ELF::R_Cucaracha_UA16
                                    : ELF::R_Cucaracha_16);
  case FK_Data_4:
    return ((Fixup.getOffset() % 4) ? ELF::R_Cucaracha_UA32
                                    : ELF::R_Cucaracha_32);
  case FK_Data_8:
    return ((Fixup.getOffset() % 8) ? ELF::R_Cucaracha_UA64
                                    : ELF::R_Cucaracha_64);
  case Cucaracha::fixup_cucaracha_13:
    return ELF::R_Cucaracha_13;
  case Cucaracha::fixup_cucaracha_hi22:
    return ELF::R_Cucaracha_HI22;
  case Cucaracha::fixup_cucaracha_lo10:
    return ELF::R_Cucaracha_LO10;
  case Cucaracha::fixup_cucaracha_h44:
    return ELF::R_Cucaracha_H44;
  case Cucaracha::fixup_cucaracha_m44:
    return ELF::R_Cucaracha_M44;
  case Cucaracha::fixup_cucaracha_l44:
    return ELF::R_Cucaracha_L44;
  case Cucaracha::fixup_cucaracha_hh:
    return ELF::R_Cucaracha_HH22;
  case Cucaracha::fixup_cucaracha_hm:
    return ELF::R_Cucaracha_HM10;
  case Cucaracha::fixup_cucaracha_lm:
    return ELF::R_Cucaracha_LM22;
  case Cucaracha::fixup_cucaracha_got22:
    return ELF::R_Cucaracha_GOT22;
  case Cucaracha::fixup_cucaracha_got10:
    return ELF::R_Cucaracha_GOT10;
  case Cucaracha::fixup_cucaracha_got13:
    return ELF::R_Cucaracha_GOT13;
  case Cucaracha::fixup_cucaracha_tls_gd_hi22:
    return ELF::R_Cucaracha_TLS_GD_HI22;
  case Cucaracha::fixup_cucaracha_tls_gd_lo10:
    return ELF::R_Cucaracha_TLS_GD_LO10;
  case Cucaracha::fixup_cucaracha_tls_gd_add:
    return ELF::R_Cucaracha_TLS_GD_ADD;
  case Cucaracha::fixup_cucaracha_tls_gd_call:
    return ELF::R_Cucaracha_TLS_GD_CALL;
  case Cucaracha::fixup_cucaracha_tls_ldm_hi22:
    return ELF::R_Cucaracha_TLS_LDM_HI22;
  case Cucaracha::fixup_cucaracha_tls_ldm_lo10:
    return ELF::R_Cucaracha_TLS_LDM_LO10;
  case Cucaracha::fixup_cucaracha_tls_ldm_add:
    return ELF::R_Cucaracha_TLS_LDM_ADD;
  case Cucaracha::fixup_cucaracha_tls_ldm_call:
    return ELF::R_Cucaracha_TLS_LDM_CALL;
  case Cucaracha::fixup_cucaracha_tls_ldo_hix22:
    return ELF::R_Cucaracha_TLS_LDO_HIX22;
  case Cucaracha::fixup_cucaracha_tls_ldo_lox10:
    return ELF::R_Cucaracha_TLS_LDO_LOX10;
  case Cucaracha::fixup_cucaracha_tls_ldo_add:
    return ELF::R_Cucaracha_TLS_LDO_ADD;
  case Cucaracha::fixup_cucaracha_tls_ie_hi22:
    return ELF::R_Cucaracha_TLS_IE_HI22;
  case Cucaracha::fixup_cucaracha_tls_ie_lo10:
    return ELF::R_Cucaracha_TLS_IE_LO10;
  case Cucaracha::fixup_cucaracha_tls_ie_ld:
    return ELF::R_Cucaracha_TLS_IE_LD;
  case Cucaracha::fixup_cucaracha_tls_ie_ldx:
    return ELF::R_Cucaracha_TLS_IE_LDX;
  case Cucaracha::fixup_cucaracha_tls_ie_add:
    return ELF::R_Cucaracha_TLS_IE_ADD;
  case Cucaracha::fixup_cucaracha_tls_le_hix22:
    return ELF::R_Cucaracha_TLS_LE_HIX22;
  case Cucaracha::fixup_cucaracha_tls_le_lox10:
    return ELF::R_Cucaracha_TLS_LE_LOX10;
  case Cucaracha::fixup_cucaracha_hix22:
    return ELF::R_Cucaracha_HIX22;
  case Cucaracha::fixup_cucaracha_lox10:
    return ELF::R_Cucaracha_LOX10;
  case Cucaracha::fixup_cucaracha_gotdata_hix22:
    return ELF::R_Cucaracha_GOTDATA_HIX22;
  case Cucaracha::fixup_cucaracha_gotdata_lox10:
    return ELF::R_Cucaracha_GOTDATA_LOX10;
  case Cucaracha::fixup_cucaracha_gotdata_op:
    return ELF::R_Cucaracha_GOTDATA_OP;
  }

  return ELF::R_Cucaracha_NONE;
}

bool CucarachaELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
                                                       unsigned Type) const {
  switch (Type) {
  default:
    return false;

  // All relocations that use a GOT need a symbol, not an offset, as
  // the offset of the symbol within the section is irrelevant to
  // where the GOT entry is. Don't need to list all the TLS entries,
  // as they're all marked as requiring a symbol anyways.
  case ELF::R_Cucaracha_GOT10:
  case ELF::R_Cucaracha_GOT13:
  case ELF::R_Cucaracha_GOT22:
  case ELF::R_Cucaracha_GOTDATA_HIX22:
  case ELF::R_Cucaracha_GOTDATA_LOX10:
  case ELF::R_Cucaracha_GOTDATA_OP_HIX22:
  case ELF::R_Cucaracha_GOTDATA_OP_LOX10:
    return true;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createCucarachaELFObjectWriter(bool Is64Bit, uint8_t OSABI) {
  return std::make_unique<CucarachaELFObjectWriter>(Is64Bit, OSABI);
}
