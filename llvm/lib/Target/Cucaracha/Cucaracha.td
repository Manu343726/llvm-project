//===-- Cucaracha.td - Describe the Cucaracha Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the Cucaracha target.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Cucaracha documentation
//===----------------------------------------------------------------------===//
/*
total supported opcodes: 24
total implemented instructions: 24
instruction encoding lengh (bits): 32
opcode encoding lengh (bits): 5

Opcodes:

 - LSR (code: 15, binary: 01111, hex: 0xf)
 - ASL (code: 16, binary: 10000, hex: 0x10)
 - ADD (code: 6, binary: 00110, hex: 0x6)
 - DIV (code: 9, binary: 01001, hex: 0x9)
 - ASR (code: 17, binary: 10001, hex: 0x11)
 - HLT (code: 18, binary: 10010, hex: 0x12)
 - RETI (code: 22, binary: 10110, hex: 0x16)
 - SIG (code: 23, binary: 10111, hex: 0x17)
 - MOVIMM16H (code: 1, binary: 00001, hex: 0x1)
 - SUB (code: 7, binary: 00111, hex: 0x7)
 - LSL (code: 14, binary: 01110, hex: 0xe)
 - DI (code: 20, binary: 10100, hex: 0x14)
 - NOP (code: 0, binary: 00000, hex: 0x0)
 - MOVIMM16L (code: 2, binary: 00010, hex: 0x2)
 - MOV (code: 3, binary: 00011, hex: 0x3)
 - LD (code: 4, binary: 00100, hex: 0x4)
 - ST (code: 5, binary: 00101, hex: 0x5)
 - CMP (code: 11, binary: 01011, hex: 0xb)
 - EI (code: 19, binary: 10011, hex: 0x13)
 - INT (code: 21, binary: 10101, hex: 0x15)
 - MUL (code: 8, binary: 01000, hex: 0x8)
 - MOD (code: 10, binary: 01010, hex: 0xa)
 - JMP (code: 12, binary: 01100, hex: 0xc)
 - CJMP (code: 13, binary: 01101, hex: 0xd)

Instructions:

  RETI (code: 22, binary: 10110, hex: 0x16) 

    Description:

      Returns from an interrupt handler, restoring saved state and re-enabling interrupts.

    Memory layout:

      4            0
      +------------+
      |   10110    |
      +------------+
       <- 5 bits -> 

    Operands:

      (none)


  NOP (code: 0, binary: 00000, hex: 0x0) 

    Description:

      No operation. All internal state except from the program counter stays the same afer the execution of the instruction. Takes only 1 CPU cicle

    Memory layout:

      4            0
      +------------+
      |   00000    |
      +------------+
       <- 5 bits -> 

    Operands:

      (none)


  ASR (code: 17, binary: 10001, hex: 0x11) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Arithmetic shift right the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   10001    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:state registers,general purpose integer registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:state registers,general purpose integer registers>: destination register


  HLT (code: 18, binary: 10010, hex: 0x12) 

    Description:

      Halts CPU execution. The CPU will stop executing instructions until reset.

    Memory layout:

      4            0
      +------------+
      |   10010    |
      +------------+
       <- 5 bits -> 

    Operands:

      (none)


  SIG (code: 23, binary: 10111, hex: 0x17) <Source:Immediate:Int32> <Source:Immediate:Int32>

    Description:

      Sends a signal to an external peripheral or system component

    Memory layout:

      20                         12                         4            0
      +--------------------------+--------------------------+------------+
      | <Source:Immediate:Int32> | <Source:Immediate:Int32> |   10111    |
      +--------------------------+--------------------------+------------+
       <-------- 8 bits --------> <-------- 8 bits --------> <- 5 bits -> 

    Operands:

     [0] <Source:Immediate:Int32>: Signal number
     [1] <Source:Immediate:Int32>: Target peripheral or component ID


  MOVIMM16L (code: 2, binary: 00010, hex: 0x2) <Source:Immediate:Int32> <Int32:general purpose integer registers,state registers>

    Description:

      Copies the 16 least significant bits of a 32 bit immediate into a 32 bit integer register

    Memory layout:

      28                                                          20                         4            0
      +-----------------------------------------------------------+--------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Source:Immediate:Int32> |   00010    |
      +-----------------------------------------------------------+--------------------------+------------+
       <------------------------ 8 bits -------------------------> <------- 16 bits --------> <- 5 bits -> 

    Operands:

     [0] <Source:Immediate:Int32>: source immediate. The higher 16 bits are ignored
     [1] <Int32:general purpose integer registers,state registers>: destination register


  ST (code: 5, binary: 00101, hex: 0x5) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Copies a word (32 bit) from a 32 bit integer register and writes it to a location in memory

    Memory layout:

      20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   00101    |
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: Register containing the value to store
     [1] <Int32:general purpose integer registers,state registers>: Register containing the memory address to write to


  SUB (code: 7, binary: 00111, hex: 0x7) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Subsctracts the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   00111    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:state registers,general purpose integer registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:state registers,general purpose integer registers>: destination register


  MOD (code: 10, binary: 01010, hex: 0xa) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Applies integer modulo between the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01010    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  LSL (code: 14, binary: 01110, hex: 0xe) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Logical shift left the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01110    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  ASL (code: 16, binary: 10000, hex: 0x10) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Arithmetic shift left the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   10000    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  CMP (code: 11, binary: 01011, hex: 0xb) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Compares the values of two 32 bit integer registers and stores the comparison result into CPSR

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01011    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: First operand
     [1] <Int32:state registers,general purpose integer registers>: Second operand
     [2] <Int32:general purpose integer registers,state registers>: Destination register (CPSR)


  LD (code: 4, binary: 00100, hex: 0x4) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers>

    Description:

      Copies a word (32 bit) from a memory location and stores it in a register

    Memory layout:

      20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   00100    |
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: Register containing the memory address to copy from
     [1] <Int32:general purpose integer registers,state registers>: destination register


  JMP (code: 12, binary: 01100, hex: 0xc) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Unconditional jump to the address contained in a 32 bit integer register

    Memory layout:

      20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01100    |
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: Register containing the target address
     [1] <Int32:state registers,general purpose integer registers>: Register where the current instruction address will be saved before the jump


  CJMP (code: 13, binary: 01101, hex: 0xd) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Conditional jump to the address contained in a 32 bit integer register if the condition code (stored in a register) is satisfied by the current CPSR flags

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:state registers,general purpose integer registers> |   01101    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: Register containing the condition code (0-14: EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL)
     [1] <Int32:general purpose integer registers,state registers>: Register containing the target address
     [2] <Int32:general purpose integer registers,state registers>: Link register - stores current PC before jump


  ADD (code: 6, binary: 00110, hex: 0x6) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Adds the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   00110    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:state registers,general purpose integer registers>: destination register


  DIV (code: 9, binary: 01001, hex: 0x9) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>

    Description:

      Divides the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01001    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:state registers,general purpose integer registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  EI (code: 19, binary: 10011, hex: 0x13) 

    Description:

      Enables interrupts. After this instruction, the CPU will respond to interrupt requests.

    Memory layout:

      4            0
      +------------+
      |   10011    |
      +------------+
       <- 5 bits -> 

    Operands:

      (none)


  DI (code: 20, binary: 10100, hex: 0x14) 

    Description:

      Disables interrupts. After this instruction, the CPU will ignore interrupt requests.

    Memory layout:

      4            0
      +------------+
      |   10100    |
      +------------+
       <- 5 bits -> 

    Operands:

      (none)


  MOVIMM16H (code: 1, binary: 00001, hex: 0x1) <Source:Immediate:Int32> <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>

    Description:

      Copies the 16 most significant bits of a 32 bit immediate into a 32 bit integer register, preserving the low 16 bits

    Memory layout:

      28                                                          28                                                          20                         4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+--------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Source:Immediate:Int32> |   00001    |
      +-----------------------------------------------------------+-----------------------------------------------------------+--------------------------+------------+
       <------------------------ 0 bits -------------------------> <------------------------ 8 bits -------------------------> <------- 16 bits --------> <- 5 bits -> 

    Operands:

     [0] <Source:Immediate:Int32>: source immediate. The lower 16 bits are ignored
     [1] <Int32:general purpose integer registers,state registers>: destination register
     [2] <Int32:general purpose integer registers,state registers>: source register (tied to dst, implicit)


  MOV (code: 3, binary: 00011, hex: 0x3) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Copies the value of a 32 bit integer register into another 32 bit integer register

    Memory layout:

      20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   00011    |
      +-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: source register
     [1] <Int32:state registers,general purpose integer registers>: destination register


  MUL (code: 8, binary: 01000, hex: 0x8) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Multiplies the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01000    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:state registers,general purpose integer registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  LSR (code: 15, binary: 01111, hex: 0xf) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>

    Description:

      Logical shift right the values of two integer registers and save the result into an integer register

    Memory layout:

      28                                                          20                                                          12                                                          4            0
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
      | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> | <Int32:general purpose integer registers,state registers> |   01111    |
      +-----------------------------------------------------------+-----------------------------------------------------------+-----------------------------------------------------------+------------+
       <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <------------------------ 8 bits -------------------------> <- 5 bits -> 

    Operands:

     [0] <Int32:general purpose integer registers,state registers>: first source register
     [1] <Int32:general purpose integer registers,state registers>: second source register
     [2] <Int32:general purpose integer registers,state registers>: destination register


  INT (code: 21, binary: 10101, hex: 0x15) <Source:Immediate:Int32>

    Description:

      Triggers an interrupt with the specified vector number.

    Memory layout:

      12                         4            0
      +--------------------------+------------+
      | <Source:Immediate:Int32> |   10101    |
      +--------------------------+------------+
       <-------- 8 bits --------> <- 5 bits -> 

    Operands:

     [0] <Source:Immediate:Int32>: interrupt vector number (0-255)



*/

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Descriptions
//===----------------------------------------------------------------------===//

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


//
// General purpose word-sized 32 bit integer registers
//

// 
def R0  : CucarachaReg<16, "r0">;

// 
def R1  : CucarachaReg<17, "r1">;

// 
def R2  : CucarachaReg<18, "r2">;

// 
def R3  : CucarachaReg<19, "r3">;

// 
def R4  : CucarachaReg<20, "r4">;

// 
def R5  : CucarachaReg<21, "r5">;

// 
def R6  : CucarachaReg<22, "r6">;

// 
def R7  : CucarachaReg<23, "r7">;

// 
def R8  : CucarachaReg<24, "r8">;

// 
def R9  : CucarachaReg<25, "r9">;


//
// CPU state registers
//

// Program Counter
def PC  : CucarachaReg<0, "pc">;

// Stack Pointer. Points to the current position of the top of the stack
def SP  : CucarachaReg<1, "sp">;

// CPU state register. Stores CPU state such as comparison flag, carry flag, etc
def CPSR  : CucarachaReg<2, "cpsr">;

// Link register. Stores the return adress of a call
def LR  : CucarachaReg<3, "lr">;



// Register classes.

def IntegerRegisters : RegisterClass<"Cucaracha",
  [i32],
  32,
  (add PC, SP, CPSR, LR, R0, R1, R2, R3, R4, R5, R6, R7, R8, R9)>;


//===-- CucarachaConditionCodes.td - Cucaracha Condition Codes -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines condition codes for the Cucaracha target.
// AUTO-GENERATED - Do not edit manually
//
// Uses LLVM's SearchableTables to generate C++ enums and lookup functions.
//
//===----------------------------------------------------------------------===//

include "llvm/TableGen/SearchableTable.td"

//===----------------------------------------------------------------------===//
// CPSR Flags - Individual flags in the Current Program Status Register
//===----------------------------------------------------------------------===//

class CPSRFlagEntry<bits<4> val, string nameStr> {
  bits<4> Value = val;
  string Name = nameStr;
}
def FLAG_Z : CPSRFlagEntry<0x1, "FLAG_Z">;
def FLAG_N : CPSRFlagEntry<0x2, "FLAG_N">;
def FLAG_C : CPSRFlagEntry<0x4, "FLAG_C">;
def FLAG_V : CPSRFlagEntry<0x8, "FLAG_V">;

def CPSRFlagsList : GenericEnum {
  let FilterClass = "CPSRFlagEntry";
  let NameField = "Name";
  let ValueField = "Value";
}

//===----------------------------------------------------------------------===//
// Condition Code Definition
//===----------------------------------------------------------------------===//

class CucarachaCondCodeEntry<bits<4> codeVal, string nameStr,
                             string oppositeStr, string testStr> {
  bits<4> Code = codeVal;
  string Name = nameStr;
  string OppositeName = oppositeStr;
  string TestExpr = testStr;
}
def COND_EQ : CucarachaCondCodeEntry<0, "COND_EQ", "COND_NE", "z">;
def COND_NE : CucarachaCondCodeEntry<1, "COND_NE", "COND_EQ", "!z">;
def COND_CS : CucarachaCondCodeEntry<2, "COND_CS", "COND_CC", "c">;
def COND_CC : CucarachaCondCodeEntry<3, "COND_CC", "COND_CS", "!c">;
def COND_MI : CucarachaCondCodeEntry<4, "COND_MI", "COND_PL", "n">;
def COND_PL : CucarachaCondCodeEntry<5, "COND_PL", "COND_MI", "!n">;
def COND_VS : CucarachaCondCodeEntry<6, "COND_VS", "COND_VC", "v">;
def COND_VC : CucarachaCondCodeEntry<7, "COND_VC", "COND_VS", "!v">;
def COND_HI : CucarachaCondCodeEntry<8, "COND_HI", "COND_LS", "c && !z">;
def COND_LS : CucarachaCondCodeEntry<9, "COND_LS", "COND_HI", "!c || z">;
def COND_GE : CucarachaCondCodeEntry<10, "COND_GE", "COND_LT", "n == v">;
def COND_LT : CucarachaCondCodeEntry<11, "COND_LT", "COND_GE", "n != v">;
def COND_GT : CucarachaCondCodeEntry<12, "COND_GT", "COND_LE", "!z && (n == v)">;
def COND_LE : CucarachaCondCodeEntry<13, "COND_LE", "COND_GT", "z || (n != v)">;
def COND_AL : CucarachaCondCodeEntry<14, "COND_AL", "COND_AL", "true">;
def COND_INVALID : CucarachaCondCodeEntry<15, "COND_INVALID", "COND_INVALID", "false">;

def CucarachaCondCodesList : GenericEnum {
  let FilterClass = "CucarachaCondCodeEntry";
  let NameField = "Name";
  let ValueField = "Code";
}

def CucarachaCondCodeLookup : GenericTable {
  let FilterClass = "CucarachaCondCodeEntry";
  let CppTypeName = "CondCodeInfo";
  let Fields = ["Code", "Name", "OppositeName", "TestExpr"];
  let PrimaryKey = ["Code"];
  let PrimaryKeyName = "lookupCondCodeByCode";
}

def CucarachaCondCodeByName : SearchIndex {
  let Table = CucarachaCondCodeLookup;
  let Key = ["Name"];
}

//===----------------------------------------------------------------------===//
// Condition Code Operand for Assembly Parsing
//===----------------------------------------------------------------------===//

def CucarachaConditionCodeOperand : AsmOperandClass {
  let Name = "ConditionCode";
  let RenderMethod = "addCondCodeOperands";
  let PredicateMethod = "isCondCode";
  let DiagnosticType = "InvalidCondCode";
}

def cond_code : Operand<i32> {
  let PrintMethod = "printCondCode";
  let ParserMatchClass = CucarachaConditionCodeOperand;
  let DecoderMethod = "DecodeCondCode";
}

//===----------------------------------------------------------------------===//
// Convenience ImmLeaf definitions for DAG patterns
//===----------------------------------------------------------------------===//

// Flag value matchers
def IsFLAG_Z : ImmLeaf<i32, [{ return Imm == 0x1; }]>;
def IsFLAG_N : ImmLeaf<i32, [{ return Imm == 0x2; }]>;
def IsFLAG_C : ImmLeaf<i32, [{ return Imm == 0x4; }]>;
def IsFLAG_V : ImmLeaf<i32, [{ return Imm == 0x8; }]>;

//===-- CucarachaInstrInfo.td - Target Description for Cucaracha ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Cucaracha instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
class InstCucaracha<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<32> Inst;
  let Namespace = "Cucaracha";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  let Size = 4;
}

// Cucaracha pseudo instructions format
class CucarachaPseudoInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstCucaracha<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
}

class BranchInst<bits<4> opcode, dag outs, dag ins,
                 string asmstr, list<dag> pattern>
    : InstCucaracha<outs, ins, asmstr, pattern> {
  let Inst{27-24} = opcode;
}

//===-- CucarachaOperators.td - Cucaracha-specific operators ------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Type profiles
//===----------------------------------------------------------------------===//

def MoveImm32Ty : SDTypeProfile<1, 1, [
  SDTCisSameAs<0, 1>, SDTCisInt<0>
]>;

def SDT_CucarachaCallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>, SDTCisVT<1, i32> ]>;

def SDT_CucarachaCallSeqEnd   : SDCallSeqEnd<[
  SDTCisVT<0, i32>, SDTCisVT<1, i32>
]>;

def CucarachaRetFlag    : SDNode<"CucarachaISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CucarachaCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_CucarachaCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_CucarachaCall    : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;

//===----------------------------------------------------------------------===//
// Custom SDNodes.
//===----------------------------------------------------------------------===//

def load_sym : SDNode<"CucarachaISD::LOAD_SYM", SDTIntUnaryOp>;

def movei32 : SDNode<"CucarachaISD::MOVEi32", MoveImm32Ty>;

def leg_call
    : SDNode<"CucarachaISD::CALL", SDT_CucarachaCall,
             [ SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic ]>;

// SELECT_CC node type profile
def SDT_CucarachaSelectCC : SDTypeProfile<1, 5, [
  SDTCisVT<0, i32>,
  SDTCisSameAs<0, 1>,
  SDTCisSameAs<1, 2>,
  SDTCisSameAs<0, 3>,
  SDTCisSameAs<0, 4>,
  SDTCisVT<5, i32>
]>;

def Cucaracha_select_cc : SDNode<"CucarachaISD::SELECT_CC", SDT_CucarachaSelectCC>;

// CMP node type profile - compares two values and produces result in CPSR
// Result: i32 (CPSR value)
// Operands: LHS, RHS (both i32 registers)
def SDT_CucarachaCmp : SDTypeProfile<1, 2, [
  SDTCisVT<0, i32>,    // Result (CPSR)
  SDTCisVT<1, i32>,    // LHS
  SDTCisVT<2, i32>     // RHS
]>;

def cucaracha_cmp : SDNode<"CucarachaISD::CMP", SDT_CucarachaCmp, []>;

// BR_COND node type profile - takes CPSR value explicitly
def SDT_CucarachaBrCond : SDTypeProfile<0, 3, [
  SDTCisVT<0, i32>,    // cpsr register value
  SDTCisVT<1, i32>,    // mask immediate
  SDTCisVT<2, OtherVT> // target basic block
]>;

def cucaracha_brcond : SDNode<"CucarachaISD::BR_COND", SDT_CucarachaBrCond,
                              [SDNPHasChain]>;

//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

def bl_target : Operand<i32>;

def b_target : Operand<OtherVT>;

// Branch target operand for direct branches to basic blocks
def brtarget : Operand<OtherVT> {
  let EncoderMethod = "getBranchTargetOpValue";
}

def cc_val : Operand<i32> {
  let PrintMethod = "printCondCode";
}

def memsrc : Operand<i32> {
  let MIOperandInfo = (ops IntegerRegisters, i32imm);
  let PrintMethod = "printAddrModeMemSrc";
  let EncoderMethod = "getMemSrcValue";
}

def i32imm_lo : Operand<i32>, ImmLeaf<i32, [{
  return Imm >= 0 && Imm < 65536;
}]>;

def Cucarachaimm8 : Operand<i32>, ImmLeaf<i32, [{
  return Imm >= 0 && Imm < 256;
}]>;

//===----------------------------------------------------------------------===//
// Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], []>;


//===----------------------------------------------------------------------===//
// Automatically defined instructions
//===----------------------------------------------------------------------===//



// DI (code: 20, binary: 10100, hex: 0x14) 
// Disables interrupts. After this instruction, the CPU will ignore interrupt requests.
def DI  : InstCucaracha<(outs  ),
                   (ins ),
                   "DI ",
                   []> {
  let Inst{ 4 - 0 } = 0b10100; // opcode
}


// MOVIMM16H (code: 1, binary: 00001, hex: 0x1) <Source:Immediate:Int32> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Copies the 16 most significant bits of a 32 bit immediate into a 32 bit integer register, preserving the low 16 bits
let Constraints = "$src = $dst" in
def MOVIMM16H  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins i32imm:$imm, IntegerRegisters:$src),
                   "MOVIMM16H $imm, $dst",
                   []> {
  let Inst{ 4 - 0 } = 0b00001; // opcode
  bits<16> imm;
  let Inst{ 20-5 } = imm;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// MOV (code: 3, binary: 00011, hex: 0x3) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers>
// Copies the value of a 32 bit integer register into another 32 bit integer register
def MOV  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src),
                   "MOV $src, $dst",
                   []> {
  let Inst{ 4 - 0 } = 0b00011; // opcode
  bits<8> src;
  let Inst{ 12-5 } = src;
  bits<8> dst;
  let Inst{ 20-13 } = dst;
}


// MUL (code: 8, binary: 01000, hex: 0x8) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>
// Multiplies the values of two integer registers and save the result into an integer register
def MUL  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "MUL $src1, $src2, $dst",
                   [(set i32:$dst, (mul i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b01000; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// LSR (code: 15, binary: 01111, hex: 0xf) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>
// Logical shift right the values of two integer registers and save the result into an integer register
def LSR  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "LSR $src1, $src2, $dst",
                   [(set i32:$dst, (srl i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b01111; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// INT (code: 21, binary: 10101, hex: 0x15) <Source:Immediate:Int32>
// Triggers an interrupt with the specified vector number.
def INT  : InstCucaracha<(outs  ),
                   (ins i32imm:$src),
                   "INT $src",
                   []> {
  let Inst{ 4 - 0 } = 0b10101; // opcode
  bits<8> src;
  let Inst{ 12-5 } = src;
}


// RETI (code: 22, binary: 10110, hex: 0x16) 
// Returns from an interrupt handler, restoring saved state and re-enabling interrupts.
def RETI  : InstCucaracha<(outs  ),
                   (ins ),
                   "RETI ",
                   []> {
  let Inst{ 4 - 0 } = 0b10110; // opcode
}


// NOP (code: 0, binary: 00000, hex: 0x0) 
// No operation. All internal state except from the program counter stays the same afer the execution of the instruction. Takes only 1 CPU cicle
def NOP  : InstCucaracha<(outs  ),
                   (ins ),
                   "NOP ",
                   []> {
  let Inst{ 4 - 0 } = 0b00000; // opcode
}


// ASR (code: 17, binary: 10001, hex: 0x11) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Arithmetic shift right the values of two integer registers and save the result into an integer register
def ASR  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "ASR $src1, $src2, $dst",
                   [(set i32:$dst, (sra i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b10001; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// HLT (code: 18, binary: 10010, hex: 0x12) 
// Halts CPU execution. The CPU will stop executing instructions until reset.
def HLT  : InstCucaracha<(outs  ),
                   (ins ),
                   "HLT ",
                   []> {
  let Inst{ 4 - 0 } = 0b10010; // opcode
}


// SIG (code: 23, binary: 10111, hex: 0x17) <Source:Immediate:Int32> <Source:Immediate:Int32>
// Sends a signal to an external peripheral or system component
def SIG  : InstCucaracha<(outs  ),
                   (ins i32imm:$src1, i32imm:$src2),
                   "SIG $src1, $src2",
                   []> {
  let Inst{ 4 - 0 } = 0b10111; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
}


// MOVIMM16L (code: 2, binary: 00010, hex: 0x2) <Source:Immediate:Int32> <Int32:general purpose integer registers,state registers>
// Copies the 16 least significant bits of a 32 bit immediate into a 32 bit integer register
def MOVIMM16L  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins i32imm:$imm),
                   "MOVIMM16L $imm, $dst",
                   [(set i32:$dst, i32imm_lo:$imm)]> {
  let Inst{ 4 - 0 } = 0b00010; // opcode
  bits<16> imm;
  let Inst{ 20-5 } = imm;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// ST (code: 5, binary: 00101, hex: 0x5) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>
// Copies a word (32 bit) from a 32 bit integer register and writes it to a location in memory
def ST  : InstCucaracha<(outs  ),
                   (ins IntegerRegisters:$src, IntegerRegisters:$addr),
                   "ST $src, $addr",
                   []> {
  let Inst{ 4 - 0 } = 0b00101; // opcode
  bits<8> src;
  let Inst{ 12-5 } = src;
  bits<8> addr;
  let Inst{ 20-13 } = addr;
  let mayStore = true;
}


// SUB (code: 7, binary: 00111, hex: 0x7) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers>
// Subsctracts the values of two integer registers and save the result into an integer register
def SUB  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "SUB $src1, $src2, $dst",
                   [(set i32:$dst, (sub i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b00111; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// MOD (code: 10, binary: 01010, hex: 0xa) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Applies integer modulo between the values of two integer registers and save the result into an integer register
def MOD  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "MOD $src1, $src2, $dst",
                   [(set i32:$dst, (srem i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b01010; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// LSL (code: 14, binary: 01110, hex: 0xe) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Logical shift left the values of two integer registers and save the result into an integer register
def LSL  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "LSL $src1, $src2, $dst",
                   [(set i32:$dst, (shl i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b01110; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// ASL (code: 16, binary: 10000, hex: 0x10) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Arithmetic shift left the values of two integer registers and save the result into an integer register
def ASL  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "ASL $src1, $src2, $dst",
                   [(set i32:$dst, (shl i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b10000; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// CMP (code: 11, binary: 01011, hex: 0xb) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers>
// Compares the values of two 32 bit integer registers and stores the comparison result into CPSR
def CMP  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$lhs, IntegerRegisters:$rhs),
                   "CMP $lhs, $rhs, $dst",
                   [(set i32:$dst, (cucaracha_cmp i32:$lhs, i32:$rhs))]> {
  let Inst{ 4 - 0 } = 0b01011; // opcode
  bits<8> lhs;
  let Inst{ 12-5 } = lhs;
  bits<8> rhs;
  let Inst{ 20-13 } = rhs;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// LD (code: 4, binary: 00100, hex: 0x4) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Copies a word (32 bit) from a memory location and stores it in a register
def LD  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$addr),
                   "LD $addr, $dst",
                   []> {
  let Inst{ 4 - 0 } = 0b00100; // opcode
  bits<8> addr;
  let Inst{ 12-5 } = addr;
  bits<8> dst;
  let Inst{ 20-13 } = dst;
  let mayLoad = true;
}


// JMP (code: 12, binary: 01100, hex: 0xc) <Int32:general purpose integer registers,state registers> <Int32:state registers,general purpose integer registers>
// Unconditional jump to the address contained in a 32 bit integer register
def JMP  : InstCucaracha<(outs IntegerRegisters:$link ),
                   (ins IntegerRegisters:$target),
                   "JMP $target, $link",
                   []> {
  let Inst{ 4 - 0 } = 0b01100; // opcode
  bits<8> target;
  let Inst{ 12-5 } = target;
  bits<8> link;
  let Inst{ 20-13 } = link;
  let isBranch = true;
  let isBarrier = true;
  let isTerminator = true;
  let isIndirectBranch = true;
}


// CJMP (code: 13, binary: 01101, hex: 0xd) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Conditional jump to the address contained in a 32 bit integer register if the condition code (stored in a register) is satisfied by the current CPSR flags
def CJMP  : InstCucaracha<(outs IntegerRegisters:$link ),
                   (ins IntegerRegisters:$condcode, IntegerRegisters:$target),
                   "CJMP $condcode, $target, $link",
                   []> {
  let Inst{ 4 - 0 } = 0b01101; // opcode
  bits<8> condcode;
  let Inst{ 12-5 } = condcode;
  bits<8> target;
  let Inst{ 20-13 } = target;
  bits<8> link;
  let Inst{ 28-21 } = link;
  let isBranch = true;
  let isTerminator = true;
}


// ADD (code: 6, binary: 00110, hex: 0x6) <Int32:state registers,general purpose integer registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Adds the values of two integer registers and save the result into an integer register
def ADD  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "ADD $src1, $src2, $dst",
                   [(set i32:$dst, (add i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b00110; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// DIV (code: 9, binary: 01001, hex: 0x9) <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers> <Int32:general purpose integer registers,state registers>
// Divides the values of two integer registers and save the result into an integer register
def DIV  : InstCucaracha<(outs IntegerRegisters:$dst ),
                   (ins IntegerRegisters:$src1, IntegerRegisters:$src2),
                   "DIV $src1, $src2, $dst",
                   [(set i32:$dst, (sdiv i32:$src1, i32:$src2))]> {
  let Inst{ 4 - 0 } = 0b01001; // opcode
  bits<8> src1;
  let Inst{ 12-5 } = src1;
  bits<8> src2;
  let Inst{ 20-13 } = src2;
  bits<8> dst;
  let Inst{ 28-21 } = dst;
}


// EI (code: 19, binary: 10011, hex: 0x13) 
// Enables interrupts. After this instruction, the CPU will respond to interrupt requests.
def EI  : InstCucaracha<(outs  ),
                   (ins ),
                   "EI ",
                   []> {
  let Inst{ 4 - 0 } = 0b10011; // opcode
}


//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

def MOVi32 : InstCucaracha<(outs IntegerRegisters:$dst), (ins i32imm:$src), "",
                     [(set i32:$dst, (movei32 imm:$src))]> {
  let isPseudo = 1;
}

//===----------------------------------------------------------------------===//
// Return Instructions
//===----------------------------------------------------------------------===//

// Return is implemented as JMP LR, LR (unconditional jump to link register)
// The JMP instruction saves the current PC to the second operand before jumping,
// but since we're returning we don't care about saving the return address again.
// Using LR as both target and link output makes the instruction: JMP LR, LR
let isTerminator = 1, isReturn = 1, isBarrier = 1, Uses = [LR], Defs = [LR] in {
  def RET : InstCucaracha<(outs), (ins variable_ops),
                    "JMP lr, lr",  [(CucarachaRetFlag)]> {
    // JMP instruction encoding: opcode 0b01100 (bits 4-0)
    // target register (LR = 3) in bits 12-5
    // link register output (LR = 3) in bits 20-13
    let Inst{ 4 - 0 } = 0b01100;  // JMP opcode
    let Inst{ 12-5 } = 3;         // target = LR (register 3)
    let Inst{ 20-13 } = 3;        // link = LR (register 3)
    let Inst{ 31-21 } = 0;        // unused bits
  }
}


//===----------------------------------------------------------------------===//
// Call Instructions
//===----------------------------------------------------------------------===//

// Call is implemented as JMP $addr, LR (jump to address, store return address in LR)
// The JMP instruction saves PC+4 to the link register, making this perfect for function calls
let isCall = 1, Defs = [LR], Uses = [SP] in {
  def BL : InstCucaracha<(outs), (ins IntegerRegisters:$addr),
                      "JMP $addr, lr",
                      [(leg_call i32:$addr)]> {
    bits<8> addr;
    // JMP instruction encoding: opcode 0b01100 (bits 4-0)
    // target register in bits 12-5
    // link register output (LR = 3) in bits 20-13
    let Inst{ 4 - 0 } = 0b01100;  // JMP opcode
    let Inst{ 12-5 } = addr;      // target = address register
    let Inst{ 20-13 } = 3;        // link = LR (register 3)
    let Inst{ 31-21 } = 0;        // unused bits
  }
}

def : Pattern<(i32 (load_sym tglobaladdr:$addr)),  [(MOVi32 $addr)]>;

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

// SELECT pseudo instruction - expands to conditional branches
// This selects between two values based on a condition
let usesCustomInserter = 1 in {
  def SELECT : CucarachaPseudoInst<(outs IntegerRegisters:$dst),
                                   (ins IntegerRegisters:$cond, IntegerRegisters:$T, IntegerRegisters:$F),
                                   "# SELECT $dst = $cond ? $T : $F",
                                   [(set i32:$dst, (select i32:$cond, i32:$T, i32:$F))]>;
                                   
  // SELECT_CC pseudo instruction - does comparison and selection in one
  def SELECT_CC : CucarachaPseudoInst<(outs IntegerRegisters:$dst),
                                      (ins IntegerRegisters:$lhs, IntegerRegisters:$rhs, IntegerRegisters:$T, IntegerRegisters:$F, i32imm:$cc),
                                      "# SELECT_CC $dst = ($lhs cmp $rhs) ? $T : $F",
                                      [(set i32:$dst, (Cucaracha_select_cc i32:$lhs, i32:$rhs, i32:$T, i32:$F, (i32 timm:$cc)))]>;
}

let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : CucarachaPseudoInst<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    "# ADJCALLSTACKDOWN $amt1",
                                    [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : CucarachaPseudoInst<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    "# ADJCALLSTACKUP $amt1",
                                    [(callseq_end timm:$amt1, timm:$amt2)]>;
}

// Unconditional branch pseudo-instruction
// This matches LLVM's 'br' to basic blocks and will be expanded to load address + JMP
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  def PseudoBR : CucarachaPseudoInst<(outs), (ins brtarget:$target),
                                     "JMP $target",
                                     [(br bb:$target)]>;
}

// Conditional branch pseudo-instruction
// This matches CucarachaISD::BR_COND and will be expanded to CJMP with the mask
// Takes CPSR value explicitly as input register
let isBranch = 1, isTerminator = 1 in {
  def PseudoBRCOND : CucarachaPseudoInst<(outs), (ins IntegerRegisters:$cpsr, i32imm:$mask, brtarget:$target),
                                         "CJMP $cpsr, $mask, $target",
                                         [(cucaracha_brcond i32:$cpsr, (i32 timm:$mask), bb:$target)]>;
}
// Pseudo-instructions for loads/stores with base+offset addressing
// The hardware LD/ST only take register operands, so we use pseudos to match
// LLVM's addressing modes and then expand them to compute effective addresses

// PseudoLD - Load with base register and immediate offset
// Will be expanded to: compute effective addr (base + offset) -> temp reg, then LD
def PseudoLD : CucarachaPseudoInst<(outs IntegerRegisters:$dst),
                                   (ins IntegerRegisters:$base, i32imm:$offset),
                                   "# PseudoLD $dst, [$base, #$offset]",
                                   [(set i32:$dst, (load (add i32:$base, (i32 imm:$offset))))]> {
  let mayLoad = 1;
}

// Pattern for load from a simple register address (no offset)
def : Pat<(load i32:$addr), (PseudoLD i32:$addr, 0)>;

// Pattern for load from frame index
def : Pat<(load (add frameindex:$fi, (i32 imm:$offset))),
          (PseudoLD (COPY_TO_REGCLASS $fi, IntegerRegisters), imm:$offset)>;
def : Pat<(load frameindex:$fi),
          (PseudoLD (COPY_TO_REGCLASS $fi, IntegerRegisters), 0)>;

// PseudoST - Store with base register and immediate offset
// Will be expanded to: compute effective addr (base + offset) -> temp reg, then ST
def PseudoST : CucarachaPseudoInst<(outs),
                                   (ins IntegerRegisters:$src, IntegerRegisters:$base, i32imm:$offset),
                                   "# PseudoST $src, [$base, #$offset]",
                                   [(store i32:$src, (add i32:$base, (i32 imm:$offset)))]> {
  let mayStore = 1;
}

// Pattern for store to a simple register address (no offset)
def : Pat<(store i32:$src, i32:$addr), (PseudoST i32:$src, i32:$addr, 0)>;

// Pattern for store to frame index
def : Pat<(store i32:$src, (add frameindex:$fi, (i32 imm:$offset))),
          (PseudoST i32:$src, (COPY_TO_REGCLASS $fi, IntegerRegisters), imm:$offset)>;
def : Pat<(store i32:$src, frameindex:$fi),
          (PseudoST i32:$src, (COPY_TO_REGCLASS $fi, IntegerRegisters), 0)>;

//===- CucarachaCallingConv.td - Calling Conventions for Cucaracha -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for Cucaracha architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Cucaracha Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_Cucaracha : CallingConv<[
  // i32 are returned in registers R0, R1, R2, R3
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// Cucaracha Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_Cucaracha : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // The first 4 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CC_Save : CalleeSavedRegs<(add R4, R5, R6, R7, R8, R9)>;


def CucarachaInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Cucaracha processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//


def CucarachaAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int PassSubtarget = 1;
  int Variant = 0;
}

def Cucaracha : Target {
  let InstructionSet = CucarachaInstrInfo;
  let AssemblyWriters = [CucarachaAsmWriter];
  let AllowRegisterRenaming = 1;
}
