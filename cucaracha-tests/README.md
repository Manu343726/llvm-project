# Cucaracha Toolchain Test Suite

This directory contains a comprehensive test suite for the Cucaracha toolchain, validating both compilation and execution capabilities.

## Directory Structure

```
cucaracha-tests/
├── CMakeLists.txt           # Main test configuration
├── programs/                # Test C programs
│   ├── hello_world.c       # Basic hello world
│   ├── arithmetic.c        # Arithmetic operations
│   ├── fibonacci.c         # Recursive functions
│   ├── factorial.c         # Iterative algorithms
│   ├── loops.c             # Loop constructs
│   ├── conditionals.c      # If/switch/ternary
│   ├── arrays.c            # Array operations
│   └── functions.c         # Function calls & pointers
└── scripts/
    └── run_test.sh         # Test execution script
```

## Test Categories

### Compilation Tests
- **Target**: `{test_name}_compile`
- **Purpose**: Verify C code compiles successfully for Cucaracha target
- **Command**: `clang -target cucaracha -c source.c -o output.o`

### Execution Tests  
- **Target**: `{test_name}_execute`
- **Purpose**: Verify compiled code runs correctly in Cucaracha interpreter
- **Command**: `cucaracha interpreter run output.o`

### Integration Tests
- **Target**: `{test_name}_full`
- **Purpose**: Combined compilation + execution workflow
- **Script**: Uses `run_test.sh` for complete pipeline

## Test Programs

1. **hello_world.c** - Basic program structure and output
2. **arithmetic.c** - Integer arithmetic operations (+, -, *, /, %)
3. **fibonacci.c** - Recursive function calls and stack management
4. **factorial.c** - Iterative algorithms and loops
5. **loops.c** - For, while, and do-while constructs
6. **conditionals.c** - If/else, switch/case, ternary operators
7. **arrays.c** - Array declaration, access, and modification
8. **functions.c** - Function calls, parameters, and pointers

## Running Tests

### Prerequisites
- Built LLVM with Cucaracha backend (`cucaracha-backend` branch)
- Cucaracha CLI tool available
- CMake 3.20+

### Build and Run
```bash
cd cucaracha-tests
mkdir build && cd build
cmake ..
ctest --output-on-failure
```

### Run Specific Test Categories
```bash
# Only compilation tests
ctest -L compilation

# Only execution tests  
ctest -L execution

# Only integration tests
ctest -L integration

# All cucaracha tests
ctest -L cucaracha
```

### Run Individual Tests
```bash
# Single test
ctest -R hello_world_compile

# Full test for specific program
ctest -R arithmetic_full
```

## Test Labels

Tests are organized with the following labels:
- `cucaracha` - All cucaracha-related tests
- `compilation` - Compilation-only tests
- `execution` - Execution-only tests  
- `integration` - Full pipeline tests

## Expected Behavior

Each test program:
1. **Compiles successfully** - No compilation errors for Cucaracha target
2. **Executes correctly** - Returns exit code 0 for success, 1 for failure
3. **Validates functionality** - Internal logic checks ensure correct behavior

## Integration with CI/CD

This test suite can be integrated into continuous integration by:
1. Building LLVM with Cucaracha backend
2. Building Cucaracha CLI tool
3. Running `make cucaracha_tests` target
4. Checking exit codes for test results

## Extending Tests

To add new test programs:
1. Create C file in `programs/` directory
2. Add `add_cucaracha_test("test_name", "file.c")` to CMakeLists.txt
3. Ensure program returns 0 for success, 1 for failure
4. Rebuild and run tests