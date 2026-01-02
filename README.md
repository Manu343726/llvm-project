# The LLVM Compiler Infrastructure

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

---

## ⚠️ Cucaracha Backend Fork

**This is a fork of LLVM with a custom backend for the Cucaracha CPU architecture.**

- **Branch**: `cucaracha-backend`
- **Upstream**: [llvm/llvm-project](https://github.com/llvm/llvm-project)
- **Related Project**: [Manu343726/cucaracha](https://github.com/Manu343726/cucaracha)

### What's Added

This fork adds a complete LLVM target backend for Cucaracha in `llvm/lib/Target/Cucaracha/`:

- **Instruction Selection** (`CucarachaISelLowering.cpp`, `CucarachaISelDAGToDAG.cpp`)
- **Register Allocation** (`CucarachaRegisterInfo.cpp`)
- **Code Generation** (`CucarachaAsmPrinter.cpp`, `CucarachaMCInstLower.cpp`)
- **Frame Management** (`CucarachaFrameLowering.cpp`)
- **TableGen Definitions** (`Cucaracha.td`, `CucarachaInstrInfo.td`, etc.)

### Dependency: Cucaracha Go Project

This LLVM backend works in conjunction with the **[Cucaracha](https://github.com/Manu343726/cucaracha)** Go project, which provides:

- **CPU Emulator/Interpreter**: Executes compiled Cucaracha programs
- **TableGen Generator**: The `.td` files in this backend are generated from Go templates
- **Architecture Definitions**: Instruction set, registers, and encoding are defined in Go
- **Test Infrastructure**: The `cucaracha-tests/` directory uses the Go CLI for execution tests

To regenerate the TableGen files from the Go project:
```bash
cd cucaracha/cucaracha
./cucaracha cpu generateLlvmTablegen --output ../../llvm-project/llvm/lib/Target/Cucaracha/
```

### Building with Cucaracha Target

```bash
mkdir build && cd build

cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang" \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="Cucaracha" \
  -DCMAKE_BUILD_TYPE=Release \
  ../llvm

ninja clang
```

### Using the Cucaracha Target

```bash
# Compile C to Cucaracha assembly
./bin/clang --target=cucaracha -O0 -S -o output.cucaracha input.c

# Compile C to Cucaracha object file
./bin/clang --target=cucaracha -O0 -c -o output.o input.c
```

### Running Tests

```bash
# Run Cucaracha-specific tests
ctest -C Release -R cucaracha --output-on-failure
```

### Current Status

✅ **All 48 tests passing** (January 2026)

| Test Program | Status |
|--------------|--------|
| hello_world | ✅ |
| arithmetic | ✅ |
| fibonacci | ✅ |
| factorial | ✅ |
| loops | ✅ |
| conditionals | ✅ |
| arrays | ✅ |
| functions | ✅ |

### AI Assistance Disclaimer

This project is developed with the assistance of GitHub Copilot (Claude). The AI doesn't do anything I wouldn't be capable of doing myself—it's simply a productivity tool that helps save time, much like the difference between coding with and without autocompletion. All architectural decisions, design choices, and code review remain my responsibility.

---

## Original LLVM Documentation

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](http://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362), or #llvm IRC channel on
[OFTC](https://oftc.net/).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
