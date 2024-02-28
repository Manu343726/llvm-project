//===-- CucarachaTargetStreamer.cpp - Cucaracha Target Streamer Methods
//-----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Cucaracha specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "CucarachaTargetStreamer.h"
#include "CucarachaInstPrinter.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

// pin vtable to this file
CucarachaTargetStreamer::CucarachaTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}

void CucarachaTargetStreamer::anchor() {}

CucarachaTargetAsmStreamer::CucarachaTargetAsmStreamer(
    MCStreamer &S, formatted_raw_ostream &OS)
    : CucarachaTargetStreamer(S), OS(OS) {}

void CucarachaTargetAsmStreamer::emitCucarachaRegisterIgnore(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(CucarachaInstPrinter::getRegisterName(reg)).lower()
     << ", #ignore\n";
}

void CucarachaTargetAsmStreamer::emitCucarachaRegisterScratch(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(CucarachaInstPrinter::getRegisterName(reg)).lower()
     << ", #scratch\n";
}

CucarachaTargetELFStreamer::CucarachaTargetELFStreamer(MCStreamer &S)
    : CucarachaTargetStreamer(S) {}

MCELFStreamer &CucarachaTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
