//===-- CucarachaInstrFormats.td - Cucaracha Instruction Formats ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "CucarachaRegisterInfo.td"

class CucarachaOpcode<bits<8> value> {
  bits<8> Value = value;
}

class CucInstr<dag outs, dag ins, string asmstr, list<dag> pattern>
   : Instruction {
  let Namespace = "CUC";

  field bits<32> Inst;

  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
}

// Pseudo instruction base class
class IPseudo<dag outs, dag ins, list<dag> pattern>
: CucInstr<outs, ins, "", pattern> {
  let isPseudo = 1;
}

// Instruction base class
class I<CucarachaOpcode opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : CucInstr<outs, ins, asmstr, pattern> {
  let Inst{7-0} = opcode.Value
}

// Instructions with a single register operand
class Ir<CucarachaOpcode opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : I<opcode, outs, ins, asmstr, pattern> {
  bits<4> R;
  let Inst{11-8} = R;
}

// Instructions with two register operands (source, destination)
class Irr<CucarachaOpcode opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : I<opcode, outs, ins, asmstr, pattern> {
  bits<4> Rs;
  bits<4> Rd;
  let Inst{11-8} = Rd;
  let Inst{15-12} = Rs;
}

// Instructions with one register operand and one immediate operand
class Iri<CucarachaOpcode opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : Ir<opcode, outs, ins, asmstr, pattern> {
  bits<16> Imm;
  let Inst{28-12} = Imm;
}

def wimm : ImmLeaf<i32, [{return isInt<32>(Imm);}]>;
def fimm : ImmLeaf<f32, [{return isFloat<32>(Imm);}]>;