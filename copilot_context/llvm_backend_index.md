# LLVM-Project Cucaracha Backend Index

**Date:** January 2, 2026  
**Repository:** llvm/llvm-project
**Branch:** cucaracha-backend
**Path:** `/llvm-project/llvm/lib/Target/Cucaracha/`

## Current Status
- **All 48 tests passing** (100%)
- **Execution working**: Both Go emulator (.cucaracha) and native binary (.o) execution
- **Latest fix**: Arrays memcpy constant inlining (shouldConvertConstantLoadToIntImm)

## Project Overview

This is a custom LLVM backend implementation for the Cucaracha CPU architecture. It provides code generation, instruction selection, register allocation, and all the necessary components to compile C/C++ code for the Cucaracha target.

## LLVM Target Structure

### Core Target Files

#### `Cucaracha.td`
- Main target descriptor file
- Defines target features, instruction sets, and architecture properties
- Entry point for TableGen processing

#### `Cucaracha.h`
- Main header with target-specific declarations
- Includes common definitions and enums

### Instruction Management

#### `CucarachaInstrInfo.td`
- Instruction definitions using TableGen
- Defines all Cucaracha instructions and their properties
- Generated from cucaracha project templates

#### `CucarachaInstrInfo.cpp/.h`
- Instruction information implementation
- Provides instruction manipulation APIs
- Handles instruction analysis and transformation

#### `CucarachaInstrFormats.td`
- Instruction encoding formats
- Defines bit patterns and field layouts
- Used for machine code generation

### Register Management

#### `CucarachaRegisterInfo.td`
- Register class definitions
- Register aliases and constraints
- Generated from cucaracha project templates

#### `CucarachaRegisterInfo.cpp/.h`
- Register allocation interface implementation
- Provides register manipulation APIs
- Handles calling conventions

### Code Generation

#### `CucarachaISelDAGToDAG.cpp`
- Instruction selection pass
- Converts LLVM IR to target-specific instructions
- Pattern matching implementation

#### `CucarachaISelLowering.cpp/.h`
- Target-specific lowering operations
- Converts high-level operations to target instructions
- Implements calling conventions

#### `CucarachaSelDAGtoDAG.h`
- DAG-to-DAG selection interface

### Machine Code Generation

#### `CucarachaMCInstLower.cpp/.h`
- Lowers MachineInstr to MCInst
- Handles symbol resolution and relocations

#### `CucarachaAsmPrinter.cpp`
- Assembly output generation
- Formats instructions for assembly files

### Target Description

#### `CucarachaTargetMachine.cpp/.h`
- Main target machine implementation
- Configures optimization passes
- Sets up code generation pipeline

#### `CucarachaSubtarget.cpp/.h`
- Subtarget feature management
- Architecture variant handling

### Frame Management

#### `CucarachaFrameLowering.cpp/.h`
- Stack frame layout and management
- Function prologue/epilogue generation
- Stack pointer manipulation

### Calling Conventions

#### `CucarachaCallingConv.td`
- Defines parameter passing conventions
- Return value handling
- Generated from cucaracha project templates

#### `CucarachaOperators.td`
- Custom DAG operators
- Target-specific operation definitions

### Machine Function Support

#### `CucarachaMachineFunctionInfo.h`
- Target-specific function information
- Stack allocation tracking

### MC Layer (Machine Code)

#### `/MCTargetDesc/`
Target description for machine code layer:

- **CucarachaMCTargetDesc.cpp/.h**: Target registration and MC layer setup
- **CucarachaMCAsmInfo.cpp/.h**: Assembly syntax and formatting
- **CucarachaMCCodeEmitter.cpp**: Binary instruction encoding
- **CucarachaInstPrinter.cpp/.h**: Instruction pretty-printing

#### `/TargetInfo/`
- **CucarachaTargetInfo.h**: Target registration interface

### Build Configuration

#### `CMakeLists.txt`
- Build system configuration
- Links all target components
- Manages dependencies

#### `LLVMBuild.txt`  
- LLVM build system integration
- Component dependencies

## Integration with Cucaracha Project

This LLVM backend is generated and maintained by the main cucaracha project:

1. **Template System**: The `.td` files are generated from Go templates in the cucaracha project
2. **Architecture Definition**: Register classes, instructions, and formats are defined in the cucaracha project's Go code
3. **Code Generation**: The `cucaracha cpu generateLlvmTablegen` command produces the TableGen files
4. **Build Integration**: CMake presets configure the build to include Cucaracha as an experimental target

## Build Configuration

The LLVM project is configured to build with:
- **Targets**: X86, Sparc (stable) + Cucaracha (experimental)
- **Projects**: clang, clang-tools-extra
- **Build Type**: Debug with compile commands export
- **Platform Support**: Windows (MinGW), Linux (GCC), Visual Studio 2022

## Development Workflow

1. Modify architecture in cucaracha project Go code
2. Regenerate TableGen files with `cucaracha cpu generateLlvmTablegen`
3. Build LLVM with cucaracha backend enabled
4. Test compilation of C/C++ code to Cucaracha target

---

## ISA Assembly Syntax Convention

### Operand Order: Source First, Destination Last
All Cucaracha instructions follow a consistent **source-first** operand ordering:

```
MNEMONIC src1, src2, ..., dst
```

| Instruction Type | Format | Example |
|------------------|--------|---------|
| Register transfer | `src, dst` | `MOV r0, r1` |
| Immediate load | `imm, dst` | `MOVIMM16L #42, r0` |
| Binary ALU ops | `src1, src2, dst` | `ADD r0, r1, r2` |
| Memory load | `addr, dst` | `LD r0, r1` |
| Memory store | `src, addr` | `ST r0, r1` |

### Hidden Tied Operands (LLVM_HideFromAsm)

Some instructions like `MOVIMM16H` are read-modify-write operations that need LLVM to know about tied operands for register allocation, but these operands should not appear in assembly syntax.

**Problem:** `MOVIMM16H #0, r0, r0` (redundant third operand)

**Solution:** The Go descriptor system supports `LLVM_HideFromAsm: true` flag:
- Operand is included in LLVM's `(ins ...)` for tied constraint `$src = $dst`
- Operand is excluded from `AsmString`
- Result: `MOVIMM16H #0, r0` (clean 2-operand syntax)

**Implementation in TableGen:**
```tablegen
def MOVIMM16H : Instruction<...> {
  let AsmString = "MOVIMM16H $imm, $dst";  // Only 2 operands shown
  dag ins = (ins i32imm:$imm, IntegerRegisters:$src);  // 2 inputs (src tied)
  dag outs = (outs IntegerRegisters:$dst);  // 1 output
  let Constraints = "$src = $dst";  // Tied constraint for regalloc
}
```

---

## Key Implementation Details

### CucarachaISelLowering.h - Important Overrides

#### `shouldConvertConstantLoadToIntImm()`
Returns `true` to enable LLVM to inline constant data directly as immediate stores during memcpy expansion. Without this, LLVM's fallback load/store path generates incorrect code.

```cpp
bool shouldConvertConstantLoadToIntImm(const APInt &Imm,
                                        Type *Ty) const override {
  return true;
}
```

### CucarachaConditionCodes.h - Condition Code System

Uses ARM-style CPSR flags:
- `FLAG_Z` (0x1) - Zero flag
- `FLAG_N` (0x2) - Negative flag  
- `FLAG_C` (0x4) - Carry flag
- `FLAG_V` (0x8) - Overflow flag

Condition codes (used by CJMP):
| Code | Value | Meaning | CPSR Test |
|------|-------|---------|-----------|
| EQ | 0 | Equal | Z=1 |
| NE | 1 | Not Equal | Z=0 |
| CS | 2 | Carry Set | C=1 |
| CC | 3 | Carry Clear | C=0 |
| MI | 4 | Minus/Negative | N=1 |
| PL | 5 | Plus/Positive | N=0 |
| VS | 6 | Overflow Set | V=1 |
| VC | 7 | Overflow Clear | V=0 |
| HI | 8 | Unsigned Higher | C=1 and Z=0 |
| LS | 9 | Unsigned Lower/Same | C=0 or Z=1 |
| GE | 10 | Signed >= | N=V |
| LT | 11 | Signed < | N≠V |
| GT | 12 | Signed > | Z=0 and N=V |
| LE | 13 | Signed <= | Z=1 or N≠V |
| AL | 14 | Always | true |