//===- Fantasy.td - Describe the Fantasy Target Machine --------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "FantasySchedule.td"
include "FantasyRegisterInfo.td"
include "FantasyCallingConv.td"
include "FantasyInstrInfo.td"

def FantasyInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Fantasy processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"generic", FantasySchedModel, []>;
def : ProcessorModel<"v11", FantasySchedModel, []>;

def FantasyInstPrinter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter = 1;
}

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def Fantasy : Target {
  // Pull in Instruction Info:
  let InstructionSet = FantasyInstrInfo;
  let AssemblyWriters = [FantasyInstPrinter];
}
