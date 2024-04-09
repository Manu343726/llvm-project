//===-- CucarachaFixupKinds.h - Cucaracha-Specific Fixup Entries ------------*-
// C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CucarachaFIXUPKINDS_H
#define LLVM_CucarachaFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Cucaracha {
enum Fixups {
  fixup_cucaracha_mov_hi16_pcrel = FirstTargetFixupKind,
  fixup_cucaracha_mov_lo16_pcrel,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
} // namespace llvm

#endif
