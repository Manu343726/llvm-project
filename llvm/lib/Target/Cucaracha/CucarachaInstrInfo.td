//===-- CucarachaInstrInfo.td - Target Description for Cucaracha Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the Cucaracha instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "CucarachaInstrFormats.td"

// Write immediate into word register
def OPC_WWRITE : CucarachaOpcode<0>;
// Write immediate into float register
def OPC_FWRITE : CucarachaOpcode<1>;
// Read value of word register
// Note this instruction only makes sense in an interactive interpreter
// (it's used to read register values and print them in the UI)
def OPC_WREAD : CucarachaOpcode<2>;
// Read value of float register
// Note this instruction only makes sense in an interactive interpreter
// (it's used to read register values and print them in the UI)
def OPC_FREAD : CucarachaOpcode<3>;

// Move value from register to register (of the same type)
def OPC_MOV : CucarachaOpcode<4>;
// Move value from word register to float register (integer ->; float conversion)
def OPC_WTOF : CucarachaOpcode<5>;
// Move value from float register to word register (float ->; integer conversion)
def OPC_FTOW : CucarachaOpcode<6>;

// Load value from memory and put it into a register
def OPC_LOAD : CucarachaOpcode<7>;
// Get a value from a register and store it in memory
def OPC_STORE : CucarachaOpcode<8>;

// Add two values from two word registers and save the result into a word register
def OPC_WADD : CucarachaOpcode<9>;
// Sub two values from two word registers and save the result into a word register
def OPC_WSUB : CucarachaOpcode<10>;
// Mul two values from two word registers and save the result into a word register
def OPC_WMUL : CucarachaOpcode<11>;
// Div two values from two word registers and save the result into a word register
def OPC_WDIV : CucarachaOpcode<12>;
// Add two values from two float registers and save the result into a float register
def OPC_FADD : CucarachaOpcode<13>;
// Sub two values from two float registers and save the result into a float register
def OPC_FSUB : CucarachaOpcode<14>;
// Mul two values from two float registers and save the result into a float register
def OPC_FMUL : CucarachaOpcode<15>;
// Div two values from two float registers and save the result into a float register
def OPC_FDIV : CucarachaOpcode<16>;