//===--- Sparc.h - declare sparc target feature support ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares Sparc TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_LEG_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_LEG_H
#include "clang/Basic/TargetInfo.h"
#include "llvm/TargetParser/Triple.h"
#include <optional>
#include <string_view>
namespace clang {
namespace targets {
class LEGTargetInfo final : public TargetInfo {

public:
  LEGTargetInfo(const llvm::Triple &Triple,
                [[maybe_unused]] const TargetOptions &Opts)
      : TargetInfo(Triple) {
    BigEndian = false;
    NoAsmVariants = true;
    LongLongAlign = 32;
    SuitableAlign = 32;
    DoubleAlign = LongDoubleAlign = 32;
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;
    WCharType = UnsignedChar;
    WIntType = UnsignedInt;
    UseZeroLengthBitfieldAlignment = true;

    resetDataLayout(computeDataLayout(Triple, Opts));
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override {}
  ArrayRef<Builtin::Info> getTargetBuiltins() const override {
    return std::nullopt;
  }
  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }
  std::string_view getClobbers() const override { return ""; }
  ArrayRef<const char *> getGCCRegNames() const override {
    return std::nullopt;
  }
  ArrayRef<GCCRegAlias> getGCCRegAliases() const override {
    return std::nullopt;
  }
  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    return false;
  }
  int getEHDataRegisterNumber(unsigned RegNo) const override {
    // R0=ExceptionPointerRegister R1=ExceptionSelectorRegister
    return -1;
  }

private:
  // Copied from the backend, see
  // llvm/lib/Target/LEG/LEGTargetMachine.cpp:computeDataLayout()
  static std::string_view computeDataLayout(const llvm::Triple &TT,
                                            const TargetOptions &Options) {

    // XXX Build the triple from the arguments.
    // This is hard-coded for now for this example target.
    return "e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:32-f64:32-a:0:32-n32";
  }
};
} // namespace targets
} // namespace clang
#endif // LLVM_CLANG_LIB_BASIC_TARGETS_LEG_H
