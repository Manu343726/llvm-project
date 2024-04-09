//===-- CucarachaRegisterInfo.td - Cucaracha Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Cucaracha register file 
//===----------------------------------------------------------------------===//

class CucarachaReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "Cucaracha";
}

// CPU registers
def R0  : CucarachaReg< 0, "r0">;
def R1  : CucarachaReg< 1, "r1">;
def R2  : CucarachaReg< 2, "r2">;
def R3  : CucarachaReg< 3, "r3">;
def R4  : CucarachaReg< 4, "r4">;
def R5  : CucarachaReg< 5, "r5">;
def R6  : CucarachaReg< 6, "r6">;
def R7  : CucarachaReg< 7, "r7">;
def R8  : CucarachaReg< 8, "r8">;
def R9  : CucarachaReg< 9, "r9">;

def SP  : CucarachaReg<13, "sp">;
def LR  : CucarachaReg<14, "lr">;

def CPSR  : CucarachaReg<16, "cpsr">;

// Register classes.
//
def GRRegs : RegisterClass<"Cucaracha", [i32], 32,
  // Return values and arguments
  (add R0, R1, R2, R3,
       R4, R5, R6, R7, R8, R9, SP)>;
