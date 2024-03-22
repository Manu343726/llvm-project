//===-- CucarachaRegisterInfo.td - Cucaracha Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Cucaracha register file
//===----------------------------------------------------------------------===//

class CucarachaReg<string Name> : Register<Name> {
  let Namespace = "CUC";
  field bits<4> Number;
}

// Word (32 bit signed integer) word registers
class Wr<bits<4> Number, string Name> : CucarachaReg<Name> {
  let Number = Number;
}

// Control registers (really word registers, but named separately as in if in a isolated bank)
class Cr<bits<4> Number, string Name> : Wr<Number, Name>;

// 32 bit float registers
class Fr<bits<4> Number, string Name> : CucarachaReg<Name> {
  let Number = Number;
}

// Control Registers

def pc : Cr<0, "pc">;
def sp : Cr<1, "sp">;
def fp : Cr<2, "fp">;

def CR : RegisterClass<"CUC", [i32], 32, (add
  pc, sp, fp
)>;

// Word registers
foreach i = 0-7 in
  def w#i : Wr<i, "w"#i>;

def WR : RegisterClass<"CUC", [f32], 32, (add
  (sequence "w%u", 0, 7)
)>;

// Any word compatible registers (w registers and control registers)
def AWR : RegisterClass<"CUC", [i32], 32, (add CR, WR)>;

// Floating-point registers
foreach i = 0-7 in
  def f#i : Fr<i, "f"#i>;

def FR : RegisterClass<"CUC", [f32], 32, (add
  (sequence "f%u", 0, 7)
)>;