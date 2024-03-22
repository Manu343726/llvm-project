//===-- CucarachaCallingConv.td - Calling Conventions Cucaracha ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Cucaracha architectures.
//
//===----------------------------------------------------------------------===//

def CC_Cucaracha : CallingConv<[
  // Custom assign SRet to [sp+64].
  CCIfSRet<CCCustom<"CC_Cucaracha_Assign_SRet">>,
  // word arguments get passed through first 4 general purpose word registers
  CCIfType<[i32], CCAssignToReg<[w0, w1, w2, w3]>>,
  // float arguments get passed through first 4 general purpose float registers
  CCIfType<[f32], CCAssignToReg<[f0, f1, f2, f3]>>,

  // Alternatively, they are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;

def RetCC_Cucaracha : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[w0, w1]>>,
  CCIfType<[f32], CCAssignToReg<[f0, f1]>>,
]>;

// Callee-saved registers are handled by the register window mechanism.
def CSR : CalleeSavedRegs<(add)> {
  let OtherPreserved = (add (sequence "w%u", 0, 7),  // save general purpose word registers 0 to 7
                            (sequence "f%u", 0, 7)); // save general purpose float registers 0 to 7
}

// Callee-saved registers for calls with ReturnsTwice attribute.
def RTCSR : CalleeSavedRegs<(add)> {
  // let OtherPreserved = (add I6, I7);
}
