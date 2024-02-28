//===--- Cucaracha.cpp - Implement Cucaracha target feature support
//---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements Cucaracha TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Cucaracha.h"
#include "Targets.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const char *const CucarachaTargetInfo::GCCRegNames[] = {
    // Integer registers
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",
    "r16",
    "r17",
    "r18",
    "r19",
    "r20",
    "r21",
    "r22",
    "r23",
    "r24",
    "r25",
    "r26",
    "r27",
    "r28",
    "r29",
    "r30",
    "r31",

    // Floating-point registers
    "f0",
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "f13",
    "f14",
    "f15",
    "f16",
    "f17",
    "f18",
    "f19",
    "f20",
    "f21",
    "f22",
    "f23",
    "f24",
    "f25",
    "f26",
    "f27",
    "f28",
    "f29",
    "f30",
    "f31",
    "f32",
    "f34",
    "f36",
    "f38",
    "f40",
    "f42",
    "f44",
    "f46",
    "f48",
    "f50",
    "f52",
    "f54",
    "f56",
    "f58",
    "f60",
    "f62",
};

ArrayRef<const char *> CucarachaTargetInfo::getGCCRegNames() const {
  return llvm::ArrayRef(GCCRegNames);
}

const TargetInfo::GCCRegAlias CucarachaTargetInfo::GCCRegAliases[] = {
    {{"g0"}, "r0"},  {{"g1"}, "r1"},  {{"g2"}, "r2"},        {{"g3"}, "r3"},
    {{"g4"}, "r4"},  {{"g5"}, "r5"},  {{"g6"}, "r6"},        {{"g7"}, "r7"},
    {{"o0"}, "r8"},  {{"o1"}, "r9"},  {{"o2"}, "r10"},       {{"o3"}, "r11"},
    {{"o4"}, "r12"}, {{"o5"}, "r13"}, {{"o6", "sp"}, "r14"}, {{"o7"}, "r15"},
    {{"l0"}, "r16"}, {{"l1"}, "r17"}, {{"l2"}, "r18"},       {{"l3"}, "r19"},
    {{"l4"}, "r20"}, {{"l5"}, "r21"}, {{"l6"}, "r22"},       {{"l7"}, "r23"},
    {{"i0"}, "r24"}, {{"i1"}, "r25"}, {{"i2"}, "r26"},       {{"i3"}, "r27"},
    {{"i4"}, "r28"}, {{"i5"}, "r29"}, {{"i6", "fp"}, "r30"}, {{"i7"}, "r31"},
};

ArrayRef<TargetInfo::GCCRegAlias>
CucarachaTargetInfo::getGCCRegAliases() const {
  return llvm::ArrayRef(GCCRegAliases);
}

bool CucarachaTargetInfo::hasFeature(StringRef Feature) const {
  return llvm::StringSwitch<bool>(Feature)
      .Case("softfloat", SoftFloat)
      .Case("cucaracha", true)
      .Default(false);
}

struct CucarachaCPUInfo {
  llvm::StringLiteral Name;
  CucarachaTargetInfo::CPUKind Kind;
  CucarachaTargetInfo::CPUGeneration Generation;
};

static constexpr CucarachaCPUInfo CPUInfo[] = {
    {{"v8"}, CucarachaTargetInfo::CK_V8, CucarachaTargetInfo::CG_V8},
    {{"supercucaracha"},
     CucarachaTargetInfo::CK_SUPERCUCARACHA,
     CucarachaTargetInfo::CG_V8},
    {{"cucarachalite"},
     CucarachaTargetInfo::CK_CUCARACHALITE,
     CucarachaTargetInfo::CG_V8},
    {{"f934"}, CucarachaTargetInfo::CK_F934, CucarachaTargetInfo::CG_V8},
    {{"hypercucaracha"},
     CucarachaTargetInfo::CK_HYPERCUCARACHA,
     CucarachaTargetInfo::CG_V8},
    {{"cucarachalite86x"},
     CucarachaTargetInfo::CK_CUCARACHALITE86X,
     CucarachaTargetInfo::CG_V8},
    {{"cucarachalet"},
     CucarachaTargetInfo::CK_CUCARACHALET,
     CucarachaTargetInfo::CG_V8},
    {{"tsc701"}, CucarachaTargetInfo::CK_TSC701, CucarachaTargetInfo::CG_V8},
    {{"v9"}, CucarachaTargetInfo::CK_V9, CucarachaTargetInfo::CG_V9},
    {{"ultracucaracha"},
     CucarachaTargetInfo::CK_ULTRACUCARACHA,
     CucarachaTargetInfo::CG_V9},
    {{"ultracucaracha3"},
     CucarachaTargetInfo::CK_ULTRACUCARACHA3,
     CucarachaTargetInfo::CG_V9},
    {{"niagara"}, CucarachaTargetInfo::CK_NIAGARA, CucarachaTargetInfo::CG_V9},
    {{"niagara2"},
     CucarachaTargetInfo::CK_NIAGARA2,
     CucarachaTargetInfo::CG_V9},
    {{"niagara3"},
     CucarachaTargetInfo::CK_NIAGARA3,
     CucarachaTargetInfo::CG_V9},
    {{"niagara4"},
     CucarachaTargetInfo::CK_NIAGARA4,
     CucarachaTargetInfo::CG_V9},
    {{"ma2100"},
     CucarachaTargetInfo::CK_MYRIAD2100,
     CucarachaTargetInfo::CG_V8},
    {{"ma2150"},
     CucarachaTargetInfo::CK_MYRIAD2150,
     CucarachaTargetInfo::CG_V8},
    {{"ma2155"},
     CucarachaTargetInfo::CK_MYRIAD2155,
     CucarachaTargetInfo::CG_V8},
    {{"ma2450"},
     CucarachaTargetInfo::CK_MYRIAD2450,
     CucarachaTargetInfo::CG_V8},
    {{"ma2455"},
     CucarachaTargetInfo::CK_MYRIAD2455,
     CucarachaTargetInfo::CG_V8},
    {{"ma2x5x"},
     CucarachaTargetInfo::CK_MYRIAD2x5x,
     CucarachaTargetInfo::CG_V8},
    {{"ma2080"},
     CucarachaTargetInfo::CK_MYRIAD2080,
     CucarachaTargetInfo::CG_V8},
    {{"ma2085"},
     CucarachaTargetInfo::CK_MYRIAD2085,
     CucarachaTargetInfo::CG_V8},
    {{"ma2480"},
     CucarachaTargetInfo::CK_MYRIAD2480,
     CucarachaTargetInfo::CG_V8},
    {{"ma2485"},
     CucarachaTargetInfo::CK_MYRIAD2485,
     CucarachaTargetInfo::CG_V8},
    {{"ma2x8x"},
     CucarachaTargetInfo::CK_MYRIAD2x8x,
     CucarachaTargetInfo::CG_V8},
    // FIXME: the myriad2[.n] spellings are obsolete,
    // but a grace period is needed to allow updating dependent builds.
    {{"myriad2"},
     CucarachaTargetInfo::CK_MYRIAD2x5x,
     CucarachaTargetInfo::CG_V8},
    {{"myriad2.1"},
     CucarachaTargetInfo::CK_MYRIAD2100,
     CucarachaTargetInfo::CG_V8},
    {{"myriad2.2"},
     CucarachaTargetInfo::CK_MYRIAD2x5x,
     CucarachaTargetInfo::CG_V8},
    {{"myriad2.3"},
     CucarachaTargetInfo::CK_MYRIAD2x8x,
     CucarachaTargetInfo::CG_V8},
    {{"leon2"}, CucarachaTargetInfo::CK_LEON2, CucarachaTargetInfo::CG_V8},
    {{"at697e"},
     CucarachaTargetInfo::CK_LEON2_AT697E,
     CucarachaTargetInfo::CG_V8},
    {{"at697f"},
     CucarachaTargetInfo::CK_LEON2_AT697F,
     CucarachaTargetInfo::CG_V8},
    {{"leon3"}, CucarachaTargetInfo::CK_LEON3, CucarachaTargetInfo::CG_V8},
    {{"ut699"},
     CucarachaTargetInfo::CK_LEON3_UT699,
     CucarachaTargetInfo::CG_V8},
    {{"gr712rc"},
     CucarachaTargetInfo::CK_LEON3_GR712RC,
     CucarachaTargetInfo::CG_V8},
    {{"leon4"}, CucarachaTargetInfo::CK_LEON4, CucarachaTargetInfo::CG_V8},
    {{"gr740"},
     CucarachaTargetInfo::CK_LEON4_GR740,
     CucarachaTargetInfo::CG_V8},
};

CucarachaTargetInfo::CPUGeneration
CucarachaTargetInfo::getCPUGeneration(CPUKind Kind) const {
  if (Kind == CK_GENERIC)
    return CG_V8;
  const CucarachaCPUInfo *Item =
      llvm::find_if(CPUInfo, [Kind](const CucarachaCPUInfo &Info) {
        return Info.Kind == Kind;
      });
  if (Item == std::end(CPUInfo))
    llvm_unreachable("Unexpected CPU kind");
  return Item->Generation;
}

CucarachaTargetInfo::CPUKind
CucarachaTargetInfo::getCPUKind(StringRef Name) const {
  const CucarachaCPUInfo *Item =
      llvm::find_if(CPUInfo, [Name](const CucarachaCPUInfo &Info) {
        return Info.Name == Name;
      });

  if (Item == std::end(CPUInfo))
    return CK_GENERIC;
  return Item->Kind;
}

void CucarachaTargetInfo::fillValidCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  for (const CucarachaCPUInfo &Info : CPUInfo)
    Values.push_back(Info.Name);
}

void CucarachaTargetInfo::getTargetDefines(const LangOptions &Opts,
                                           MacroBuilder &Builder) const {
  DefineStd(Builder, "cucaracha", Opts);
  Builder.defineMacro("__REGISTER_PREFIX__", "");

  if (SoftFloat)
    Builder.defineMacro("SOFT_FLOAT", "1");
}

void CucarachaV8TargetInfo::getTargetDefines(const LangOptions &Opts,
                                             MacroBuilder &Builder) const {
  CucarachaTargetInfo::getTargetDefines(Opts, Builder);
  if (getTriple().getOS() == llvm::Triple::Solaris)
    Builder.defineMacro("__cucarachav8");
  else {
    switch (getCPUGeneration(CPU)) {
    case CG_V8:
      Builder.defineMacro("__cucarachav8");
      Builder.defineMacro("__cucarachav8__");
      break;
    case CG_V9:
      Builder.defineMacro("__cucaracha_v9__");
      break;
    }
  }
  if (getTriple().getVendor() == llvm::Triple::Myriad) {
    std::string MyriadArchValue, Myriad2Value;
    Builder.defineMacro("__cucaracha_v8__");
    Builder.defineMacro("__leon__");
    switch (CPU) {
    case CK_MYRIAD2100:
      MyriadArchValue = "__ma2100";
      Myriad2Value = "1";
      break;
    case CK_MYRIAD2150:
      MyriadArchValue = "__ma2150";
      Myriad2Value = "2";
      break;
    case CK_MYRIAD2155:
      MyriadArchValue = "__ma2155";
      Myriad2Value = "2";
      break;
    case CK_MYRIAD2450:
      MyriadArchValue = "__ma2450";
      Myriad2Value = "2";
      break;
    case CK_MYRIAD2455:
      MyriadArchValue = "__ma2455";
      Myriad2Value = "2";
      break;
    case CK_MYRIAD2x5x:
      Myriad2Value = "2";
      break;
    case CK_MYRIAD2080:
      MyriadArchValue = "__ma2080";
      Myriad2Value = "3";
      break;
    case CK_MYRIAD2085:
      MyriadArchValue = "__ma2085";
      Myriad2Value = "3";
      break;
    case CK_MYRIAD2480:
      MyriadArchValue = "__ma2480";
      Myriad2Value = "3";
      break;
    case CK_MYRIAD2485:
      MyriadArchValue = "__ma2485";
      Myriad2Value = "3";
      break;
    case CK_MYRIAD2x8x:
      Myriad2Value = "3";
      break;
    default:
      MyriadArchValue = "__ma2100";
      Myriad2Value = "1";
      break;
    }
    if (!MyriadArchValue.empty()) {
      Builder.defineMacro(MyriadArchValue, "1");
      Builder.defineMacro(MyriadArchValue + "__", "1");
    }
    if (Myriad2Value == "2") {
      Builder.defineMacro("__ma2x5x", "1");
      Builder.defineMacro("__ma2x5x__", "1");
    } else if (Myriad2Value == "3") {
      Builder.defineMacro("__ma2x8x", "1");
      Builder.defineMacro("__ma2x8x__", "1");
    }
    Builder.defineMacro("__myriad2__", Myriad2Value);
    Builder.defineMacro("__myriad2", Myriad2Value);
  }
  if (getCPUGeneration(CPU) == CG_V9) {
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4");
    Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8");
  }
}

void CucarachaV9TargetInfo::getTargetDefines(const LangOptions &Opts,
                                             MacroBuilder &Builder) const {
  CucarachaTargetInfo::getTargetDefines(Opts, Builder);
  Builder.defineMacro("__cucarachav9");
  Builder.defineMacro("__arch64__");
  // Solaris doesn't need these variants, but the BSDs do.
  if (getTriple().getOS() != llvm::Triple::Solaris) {
    Builder.defineMacro("__cucaracha64__");
    Builder.defineMacro("__cucaracha_v9__");
    Builder.defineMacro("__cucarachav9__");
  }

  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1");
  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2");
  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4");
  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8");
}

void CucarachaV9TargetInfo::fillValidCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  for (const CucarachaCPUInfo &Info : CPUInfo)
    if (Info.Generation == CG_V9)
      Values.push_back(Info.Name);
}
