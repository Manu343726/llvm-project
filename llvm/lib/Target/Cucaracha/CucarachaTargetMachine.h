//===-- CucarachaTargetMachine.h - Define TargetMachine for Cucaracha ---*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Cucaracha specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef CucarachaTARGETMACHINE_H
#define CucarachaTARGETMACHINE_H

#include "Cucaracha.h"
#include "CucarachaFrameLowering.h"
#include "CucarachaISelLowering.h"
#include "CucarachaSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class CucarachaTargetMachine : public LLVMTargetMachine {
  CucarachaSubtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  CucarachaTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                         StringRef FS, const TargetOptions &Options,
                         std::optional<Reloc::Model> RM,
                         std::optional<CodeModel::Model> CM,
                         CodeGenOpt::Level OL, bool JIT);

  const CucarachaSubtarget *getSubtargetImpl() const { return &Subtarget; }

  virtual const TargetSubtargetInfo *
  getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};

} // end namespace llvm

#endif
