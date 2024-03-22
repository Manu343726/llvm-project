  //===-- Cucaracha.td - Describe the Cucaracha Target Machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "CucarachaRegisterInfo.td"
include "CucarachaCallingConv.td"
include "CucarachaSchedule.td"
include "CucarachaInstrInfo.td"

def CucarachaInstrInfo : InstrInfo;

def CucarachaAsmParser : AsmParser {
  bit ShouldEmitMatchRegisterName = 0;
}

//===----------------------------------------------------------------------===//
// Cucaracha processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic",         []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def CucarachaAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int PassSubtarget = 1;
  int Variant = 0;
}

def Cucaracha : Target {
  // Pull in Instruction Info:
  let InstructionSet = CucarachaInstrInfo;
  let AssemblyParsers  = [CucarachaAsmParser];
  let AssemblyWriters = [CucarachaAsmWriter];
  let AllowRegisterRenaming = 1;
}
