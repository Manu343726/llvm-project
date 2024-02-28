//===-- CucarachaFixupKinds.h - Cucaracha Specific Fixup Entries --------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaFIXUPKINDS_H
#define LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Cucaracha {
enum Fixups {
  // fixup_cucaracha_call30 - 30-bit PC relative relocation for call
  fixup_cucaracha_call30 = FirstTargetFixupKind,

  /// fixup_cucaracha_br22 - 22-bit PC relative relocation for
  /// branches
  fixup_cucaracha_br22,

  /// fixup_cucaracha_br19 - 19-bit PC relative relocation for
  /// branches on icc/xcc
  fixup_cucaracha_br19,

  /// fixup_cucaracha_bpr  - 16-bit fixup for bpr
  fixup_cucaracha_br16,

  /// fixup_cucaracha_13 - 13-bit fixup
  fixup_cucaracha_13,

  /// fixup_cucaracha_hi22  - 22-bit fixup corresponding to %hi(foo)
  /// for sethi
  fixup_cucaracha_hi22,

  /// fixup_cucaracha_lo10  - 10-bit fixup corresponding to %lo(foo)
  fixup_cucaracha_lo10,

  /// fixup_cucaracha_h44  - 22-bit fixup corresponding to %h44(foo)
  fixup_cucaracha_h44,

  /// fixup_cucaracha_m44  - 10-bit fixup corresponding to %m44(foo)
  fixup_cucaracha_m44,

  /// fixup_cucaracha_l44  - 12-bit fixup corresponding to %l44(foo)
  fixup_cucaracha_l44,

  /// fixup_cucaracha_hh  -  22-bit fixup corresponding to %hh(foo)
  fixup_cucaracha_hh,

  /// fixup_cucaracha_hm  -  10-bit fixup corresponding to %hm(foo)
  fixup_cucaracha_hm,

  /// fixup_cucaracha_lm  -  22-bit fixup corresponding to %lm(foo)
  fixup_cucaracha_lm,

  /// fixup_cucaracha_pc22 - 22-bit fixup corresponding to %pc22(foo)
  fixup_cucaracha_pc22,

  /// fixup_cucaracha_pc10 - 10-bit fixup corresponding to %pc10(foo)
  fixup_cucaracha_pc10,

  /// fixup_cucaracha_got22 - 22-bit fixup corresponding to %got22(foo)
  fixup_cucaracha_got22,

  /// fixup_cucaracha_got10 - 10-bit fixup corresponding to %got10(foo)
  fixup_cucaracha_got10,

  /// fixup_cucaracha_got13 - 13-bit fixup corresponding to %got13(foo)
  fixup_cucaracha_got13,

  /// fixup_cucaracha_wplt30
  fixup_cucaracha_wplt30,

  /// fixups for Thread Local Storage
  fixup_cucaracha_tls_gd_hi22,
  fixup_cucaracha_tls_gd_lo10,
  fixup_cucaracha_tls_gd_add,
  fixup_cucaracha_tls_gd_call,
  fixup_cucaracha_tls_ldm_hi22,
  fixup_cucaracha_tls_ldm_lo10,
  fixup_cucaracha_tls_ldm_add,
  fixup_cucaracha_tls_ldm_call,
  fixup_cucaracha_tls_ldo_hix22,
  fixup_cucaracha_tls_ldo_lox10,
  fixup_cucaracha_tls_ldo_add,
  fixup_cucaracha_tls_ie_hi22,
  fixup_cucaracha_tls_ie_lo10,
  fixup_cucaracha_tls_ie_ld,
  fixup_cucaracha_tls_ie_ldx,
  fixup_cucaracha_tls_ie_add,
  fixup_cucaracha_tls_le_hix22,
  fixup_cucaracha_tls_le_lox10,

  /// 22-bit fixup corresponding to %hix(foo)
  fixup_cucaracha_hix22,
  /// 13-bit fixup corresponding to %lox(foo)
  fixup_cucaracha_lox10,

  /// 22-bit fixup corresponding to %gdop_hix22(foo)
  fixup_cucaracha_gotdata_hix22,
  /// 13-bit fixup corresponding to %gdop_lox10(foo)
  fixup_cucaracha_gotdata_lox10,
  /// 32-bit fixup corresponding to %gdop(foo)
  fixup_cucaracha_gotdata_op,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
} // namespace llvm

#endif
