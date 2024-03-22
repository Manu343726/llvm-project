//===-- CucarachaTargetMachine.h - Define TargetMachine for Cucaracha ---*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Cucaracha specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Cucaracha_CucarachaTARGETMACHINE_H
#define LLVM_LIB_TARGET_Cucaracha_CucarachaTARGETMACHINE_H

#include "CucarachaInstrInfo.h"
#include "CucarachaSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

/// Cucaracha 32-bit target machine
///
class CucarachaTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  CucarachaSubtarget Subtarget;
  bool is64Bit;
  mutable StringMap<std::unique_ptr<CucarachaSubtarget>> SubtargetMap;

public:
  CucarachaTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                         StringRef FS, const TargetOptions &Options,
                         std::optional<Reloc::Model> RM,
                         std::optional<CodeModel::Model> CM,
                         CodeGenOpt::Level OL, bool JIT);
  ~CucarachaTargetMachine() override;

  const CucarachaSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const CucarachaSubtarget *getSubtargetImpl(const Function &) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

} // end namespace llvm

#endif
