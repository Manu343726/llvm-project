//===-- CucarachaTargetInfo.cpp - Cucaracha Target Implementation
//-----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/CucarachaTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheCucarachaTarget() {
  static Target TheCucarachaTarget;
  return TheCucarachaTarget;
}
Target &llvm::getTheCucarachaV9Target() {
  static Target TheCucarachaV9Target;
  return TheCucarachaV9Target;
}
Target &llvm::getTheCucarachaelTarget() {
  static Target TheCucarachaelTarget;
  return TheCucarachaelTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCucarachaTargetInfo() {
  RegisterTarget<Triple::cucaracha, /*HasJIT=*/false> X(
      getTheCucarachaTarget(), "cucaracha", "Cucaracha", "Cucaracha");
  RegisterTarget<Triple::cucarachav9, /*HasJIT=*/false> Y(
      getTheCucarachaV9Target(), "cucarachav9", "Cucaracha V9", "Cucaracha");
  RegisterTarget<Triple::cucarachael, /*HasJIT=*/false> Z(
      getTheCucarachaelTarget(), "cucarachael", "Cucaracha LE", "Cucaracha");
}
