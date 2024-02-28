//===-- CucarachaTargetStreamer.h - Cucaracha Target Streamer ----------*- C++
//-*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaTARGETSTREAMER_H
#define LLVM_LIB_TARGET_Cucaracha_MCTARGETDESC_CucarachaTARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {

class formatted_raw_ostream;

class CucarachaTargetStreamer : public MCTargetStreamer {
  virtual void anchor();

public:
  CucarachaTargetStreamer(MCStreamer &S);
  /// Emit ".register <reg>, #ignore".
  virtual void emitCucarachaRegisterIgnore(unsigned reg){};
  /// Emit ".register <reg>, #scratch".
  virtual void emitCucarachaRegisterScratch(unsigned reg){};
};

// This part is for ascii assembly output
class CucarachaTargetAsmStreamer : public CucarachaTargetStreamer {
  formatted_raw_ostream &OS;

public:
  CucarachaTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  void emitCucarachaRegisterIgnore(unsigned reg) override;
  void emitCucarachaRegisterScratch(unsigned reg) override;
};

// This part is for ELF object output
class CucarachaTargetELFStreamer : public CucarachaTargetStreamer {
public:
  CucarachaTargetELFStreamer(MCStreamer &S);
  MCELFStreamer &getStreamer();
  void emitCucarachaRegisterIgnore(unsigned reg) override {}
  void emitCucarachaRegisterScratch(unsigned reg) override {}
};
} // end namespace llvm

#endif
