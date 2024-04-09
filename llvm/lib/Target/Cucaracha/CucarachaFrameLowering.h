//===-- CucarachaFrameLowering.h - Frame info for Cucaracha Target ------*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains Cucaracha frame information that doesn't fit anywhere else
// cleanly...
//
//===----------------------------------------------------------------------===//

#ifndef CucarachaFRAMEINFO_H
#define CucarachaFRAMEINFO_H

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/Alignment.h"

namespace llvm {
class CucarachaSubtarget;

class CucarachaFrameLowering : public TargetFrameLowering {
public:
  CucarachaFrameLowering();

  static llvm::Align stackAlign();

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool hasFP(const MachineFunction &MF) const override;

  //! Stack slot size (4 bytes)
  static int stackSlotSize() { return 4; }

private:
  uint64_t computeStackSize(MachineFunction &MF) const;
};
} // namespace llvm

#endif // CucarachaFRAMEINFO_H
