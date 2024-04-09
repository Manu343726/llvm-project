//===-- CucarachaMCAsmInfo.h - Cucaracha asm properties --------------------*-
//C++
//-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the CucarachaMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef CucarachaTARGETASMINFO_H
#define CucarachaTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class StringRef;
class Target;
class Triple;

class CucarachaMCAsmInfo : public MCAsmInfoELF {
  virtual void anchor() override;

public:
  explicit CucarachaMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
