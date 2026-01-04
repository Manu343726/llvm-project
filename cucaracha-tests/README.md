# Cucaracha Toolchain Test Suite

This directory contains a comprehensive test suite for the Cucaracha toolchain, validating both compilation and execution capabilities for C and C++ programs.

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
│   ├── functions.c         # Function calls & pointers
│   └── cpp/                # C++ test programs
│       ├── basic_class.cpp           # Classes, constructors, methods
│       ├── inheritance.cpp           # Class inheritance
│       ├── constructors_destructors.cpp  # RAII patterns
│       ├── member_functions.cpp      # const, static, inline methods
│       ├── operator_overloading.cpp  # Operator overloading
│       ├── references.cpp            # References (lvalue)
│       ├── namespaces.cpp            # Namespaces and scope
│       ├── templates.cpp             # Function and class templates
│       ├── template_specialization.cpp # Template specialization
│       ├── virtual_functions.cpp     # Virtual functions, polymorphism
│       ├── static_members.cpp        # Static members and methods
│       ├── multiple_inheritance.cpp  # Multiple/diamond inheritance
│       ├── new_delete.cpp            # Dynamic memory management
│       ├── cpp11_features.cpp        # C++11: nullptr, auto, enum class
│       ├── lambdas.cpp               # Lambda expressions
│       ├── type_casting.cpp          # static_cast, const_cast, etc.
│       ├── default_args_overloading.cpp # Default args, overloading
│       └── structs_pods.cpp          # POD types, aggregates
└── scripts/
    └── run_test.sh         # Test execution script
```

## Test Categories

### C Tests
- **Target**: `cucaracha_{test_name}_*`
- **Purpose**: Verify C code compiles and executes correctly for Cucaracha target

### C++ Tests  
- **Target**: `cucaracha_cpp_{test_name}_*`
- **Purpose**: Verify C++ language features work correctly on Cucaracha target
- **Flags**: `-std=c++14 -fno-exceptions -fno-rtti` (for most tests)

### C++ RTTI Tests
- **Target**: `cucaracha_cpp_rtti_{test_name}_*`
- **Purpose**: Test C++ features requiring RTTI (virtual functions, dynamic_cast)
- **Flags**: `-std=c++14 -fno-exceptions` (RTTI enabled)

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

# Single C++ test
ctest -R cpp_templates
```

## Test Labels

Tests are organized with the following labels:
- `cucaracha` - All cucaracha-related tests (C and C++)
- `cucaracha-cpp` - C++ tests only
- `cucaracha-cpp-rtti` - C++ tests requiring RTTI
- `compilation` - Compilation-only tests
- `execution` - Execution-only tests  
- `llvm-ir` - LLVM IR generation tests
- `binary` - Binary object file generation tests

## C++ Test Coverage

The C++ test suite covers the following language features:

### Core Language Features
- **Classes**: constructors, destructors, member functions, access control
- **Inheritance**: single, multiple, virtual inheritance (diamond problem)
- **Polymorphism**: virtual functions, vtables, pure virtual functions
- **Operator Overloading**: arithmetic, comparison, compound assignment, subscript
- **References**: lvalue references, reference parameters
- **Namespaces**: nested namespaces, using declarations, anonymous namespaces

### Templates
- **Function Templates**: basic templates, multiple type parameters
- **Class Templates**: template classes, non-type template parameters
- **Template Specialization**: explicit specialization, partial specialization
- **Compile-time Computation**: Factorial, Fibonacci via templates

### Modern C++ (C++11/14)
- **nullptr**: null pointer literal
- **auto**: type inference
- **enum class**: scoped enumerations
- **constexpr**: compile-time evaluation
- **Lambdas**: capture by value/reference, mutable lambdas
- **Uniform Initialization**: brace initialization

### Memory Management
- **new/delete**: single object and array allocation
- **Custom Allocators**: operator new/delete overloading
- **RAII**: automatic resource management via destructors

### Type System
- **static_cast**: safe conversions
- **const_cast**: const/volatile manipulation
- **reinterpret_cast**: low-level casts

## Expected Behavior

Each test program:
1. **Compiles successfully** - No compilation errors for Cucaracha target
2. **Executes correctly** - Returns exit code 0 for success, non-zero for failure
3. **Validates functionality** - Internal logic checks ensure correct behavior

## Integration with CI/CD

This test suite can be integrated into continuous integration by:
1. Building LLVM with Cucaracha backend
2. Building Cucaracha CLI tool
3. Running `make cucaracha_tests` target
4. Checking exit codes for test results

## Extending Tests

### Adding C Tests
1. Create C file in `programs/` directory
2. Add `add_cucaracha_test("test_name", "file.c")` to CMakeLists.txt
3. Ensure program returns 0 for success, non-zero for failure

### Adding C++ Tests
1. Create C++ file in `programs/cpp/` directory
2. Add appropriate test function call to CMakeLists.txt:
   - `add_cucaracha_cpp_test("test_name", "file.cpp")` for standard tests
   - `add_cucaracha_cpp_rtti_test("test_name", "file.cpp")` for RTTI-dependent tests
3. Use `CXX_STANDARD` option to specify C++ version (default: 14)
4. Avoid stdlib dependencies (tests should be freestanding)

### Example:
```cmake
# Basic C++ test with C++14
add_cucaracha_cpp_test("my_test" "my_test.cpp" EXPECTED_OUTPUT "0")

# C++17 test
add_cucaracha_cpp_test("cpp17_test" "cpp17_test.cpp" CXX_STANDARD "17" EXPECTED_OUTPUT "0")

# Test with RTTI enabled
add_cucaracha_cpp_rtti_test("dynamic_cast_test" "dynamic_cast_test.cpp" EXPECTED_OUTPUT "0")
```