# Cucaracha Toolchain Testing Infrastructure

**Date:** December 27, 2025
**Location:** `/llvm-project/cucaracha-tests/`

## Overview

Created comprehensive testing infrastructure for the Cucaracha toolchain that validates both compilation and execution capabilities through CMake CTest framework.

## Directory Structure Created

```
cucaracha-tests/
├── CMakeLists.txt           # Main CMake test configuration
├── README.md               # Comprehensive documentation
├── programs/               # Test C programs (8 programs)
│   ├── hello_world.c      # Basic I/O and program structure
│   ├── arithmetic.c       # Integer arithmetic operations
│   ├── fibonacci.c        # Recursive function implementation
│   ├── factorial.c        # Iterative algorithms
│   ├── loops.c           # For/while/do-while constructs  
│   ├── conditionals.c    # If/else, switch, ternary operators
│   ├── arrays.c          # Array operations and indexing
│   └── functions.c       # Function calls and pointers
└── scripts/
    └── run_test.sh        # Bash script for integration testing
```

## Test Framework Design

### Three-Tier Testing Approach

1. **Compilation Tests** (`{name}_compile`)
   - Validates C code compiles for Cucaracha target
   - Uses: `clang -target cucaracha -c source.c -o output.o`
   - Label: `compilation`

2. **Execution Tests** (`{name}_execute`) 
   - Validates compiled code runs in Cucaracha interpreter
   - Uses: `cucaracha interpreter run output.o`
   - Label: `execution`

3. **Integration Tests** (`{name}_full`)
   - Complete compilation + execution pipeline
   - Uses: Custom bash script for end-to-end testing
   - Label: `integration`

### CMake Configuration Features

- **Automatic Tool Discovery**: Finds cucaracha CLI and clang binaries
- **Graceful Degradation**: Skips tests if tools not available
- **Organized Labeling**: Tests grouped by type and functionality
- **Timeout Protection**: 30s for individual, 60s for integration tests
- **Custom Targets**: `cucaracha_tests` target for batch execution

## Test Program Coverage

### Language Feature Testing

| Program | Features Tested |
|---------|----------------|
| hello_world | Basic structure, I/O |
| arithmetic | +, -, *, /, % operators |
| fibonacci | Recursion, stack management |
| factorial | Iteration, loops |
| loops | for, while, do-while |
| conditionals | if/else, switch, ternary |
| arrays | Declaration, indexing, modification |
| functions | Calls, parameters, pointers |

### Validation Strategy

Each test program includes internal validation logic:
- Returns `0` for successful execution
- Returns `1` for validation failure
- Self-contained correctness checks

## Integration Points

### With LLVM Build System
- **Integrated as subdirectory**: Added to `llvm/CMakeLists.txt` under `LLVM_INCLUDE_TESTS`
- **Conditional inclusion**: Only built when tests are enabled
- **Automatic detection**: Finds cucaracha-tests directory relative to LLVM source
- **Dependency management**: `cucaracha_tests` target depends on `clang` being built
- **Path resolution**: Automatically handles different build configurations (standalone vs integrated)

### With Cucaracha Project
- References cucaracha CLI tool from sibling directory
- Uses standard cucaracha interpreter commands
- Compatible with existing toolchain workflow

## Usage Instructions

### As part of LLVM Build
```bash
cd llvm-project/build_vs2022
cmake --build . --target clang  # Build clang first
ctest -L cucaracha --output-on-failure  # Run cucaracha tests
# OR
cmake --build . --target cucaracha_tests  # Build and run tests
```

### Standalone Build
```bash
cd cucaracha-tests
mkdir build && cd build
cmake ..
ctest --output-on-failure
```

### Selective Testing
```bash
ctest -L compilation  # Only compilation tests
ctest -L execution    # Only execution tests
ctest -L integration  # Only full pipeline tests
ctest -R fibonacci    # Only fibonacci-related tests
```

## Future Extensibility

### Adding New Tests
1. Create C file in `programs/` directory
2. Add `add_cucaracha_test()` call in CMakeLists.txt
3. Ensure proper return code conventions
4. Document test purpose and coverage

### Advanced Test Types
- Performance benchmarking tests
- Memory usage validation
- Debug information testing
- Optimization level comparisons
- Cross-compilation validation

## Documentation Integration

Added to project context files:
- Testing infrastructure overview
- Integration with existing toolchain
- Usage instructions and examples
- Extensibility guidelines

This testing infrastructure provides a solid foundation for continuous validation of the Cucaracha toolchain development and ensures reliability across compilation and execution workflows.