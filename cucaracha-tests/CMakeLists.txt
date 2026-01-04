# Cucaracha Toolchain Integration Tests
# Always integrated with LLVM build system

# Path to the Cucaracha Go project (relative to this CMakeLists.txt)
set(CUCARACHA_GO_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../cucaracha")

# Find Go executable
find_program(GO_EXECUTABLE go)

message(STATUS "Cucaracha Tests - Go project at: ${CUCARACHA_GO_PROJECT_DIR}")
message(STATUS "Cucaracha Tests - Clang will be resolved from clang target")

# Check for Go but don't stop if not found - just disable execution tests
set(CUCARACHA_CLI_AVAILABLE FALSE)
if(GO_EXECUTABLE AND EXISTS "${CUCARACHA_GO_PROJECT_DIR}/go.mod")
    set(CUCARACHA_CLI_AVAILABLE TRUE)
    message(STATUS "Cucaracha CLI available via: ${GO_EXECUTABLE} run . (in ${CUCARACHA_GO_PROJECT_DIR})")
else()
    if(NOT GO_EXECUTABLE)
        message(WARNING "Go executable not found. Execution tests will be skipped, but compilation tests will still run.")
    else()
        message(WARNING "Cucaracha Go project not found at ${CUCARACHA_GO_PROJECT_DIR}. Execution tests will be skipped, but compilation tests will still run.")
    endif()
endif()

# Test programs directory
set(PROGRAMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/programs")
set(PROGRAMS_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/programs/cpp")
set(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

# Output directory for compiled programs - use source directory for easier inspection
set(TEST_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_output")

# Create the output directory
file(MAKE_DIRECTORY "${TEST_OUTPUT_DIR}")

if(NOT CUCARACHA_TESTS_COMPILE_OPTIONS)
    set(CUCARACHA_TESTS_COMPILE_OPTIONS -O0 -S)
endif()

# Helper function to add compilation and execution tests for a C program
function(add_cucaracha_test TEST_NAME SOURCE_FILE)
    # Parse optional EXPECTED_OUTPUT argument (defaults to "0" for success)
    cmake_parse_arguments(ARG "" "EXPECTED_OUTPUT" "" ${ARGN})
    if(NOT DEFINED ARG_EXPECTED_OUTPUT)
        set(ARG_EXPECTED_OUTPUT "0")
    endif()

    set(SOURCE_PATH "${PROGRAMS_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${TEST_OUTPUT_DIR}/${TEST_NAME}")

    add_test(NAME cucaracha_${TEST_NAME}_setup
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_DIR}"
    )
    
    # Add LLVM IR generation test
    add_test(
        NAME "cucaracha_${TEST_NAME}_llvm_ir"
        COMMAND clang
            --target=cucaracha
            -O0 -S -emit-llvm
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.ll"
    )
    
    # Set test properties for LLVM IR generation
    set_tests_properties("cucaracha_${TEST_NAME}_llvm_ir" PROPERTIES
        LABELS "cucaracha;llvm-ir"
        TIMEOUT 30
        DEPENDS "cucaracha_${TEST_NAME}_setup"
    )

    # Add compilation test (clang is required) - generates cucaracha assembly
    add_test(
        NAME "cucaracha_${TEST_NAME}_compile"
        COMMAND clang
            --target=cucaracha
            ${CUCARACHA_TESTS_COMPILE_OPTIONS}
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.cucaracha"
    )
    
    # Set test properties with clang dependency
    set_tests_properties("cucaracha_${TEST_NAME}_compile" PROPERTIES
        LABELS "cucaracha;compilation"
        TIMEOUT 30
        DEPENDS "cucaracha_${TEST_NAME}_llvm_ir"
    )

    # Add binary object file generation test (.o ELF file)
    add_test(
        NAME "cucaracha_${TEST_NAME}_binary"
        COMMAND clang
            --target=cucaracha
            -O0 -c
            "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.o"
    )
    
    # Set test properties for binary compilation
    set_tests_properties("cucaracha_${TEST_NAME}_binary" PROPERTIES
        LABELS "cucaracha;binary"
        TIMEOUT 30
        DEPENDS "cucaracha_${TEST_NAME}_compile"
    )
    
    # Only add execution test if CLI is available
    # Note: We use .cucaracha assembly files for execution because the LLVM backend's
    # relocation encoding for .o files uses ARM MOVW format which is incompatible with
    # Cucaracha's instruction encoding. The assembly files have proper label names that
    # the Go interpreter can resolve correctly.
    if(CUCARACHA_CLI_AVAILABLE)
        # Execution test using assembly file (reliable - proper label resolution)
        # Uses verify_output.cmake script to also check the program output
        add_test(
            NAME "cucaracha_${TEST_NAME}_execute"
            COMMAND ${CMAKE_COMMAND}
                -DGO_EXECUTABLE=${GO_EXECUTABLE}
                -DCUCARACHA_GO_PROJECT_DIR=${CUCARACHA_GO_PROJECT_DIR}
                -DPROGRAM=${OUTPUT_PATH}.cucaracha
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_${TEST_NAME}_execute" PROPERTIES
            LABELS "cucaracha;execution"
            TIMEOUT 30
            DEPENDS "cucaracha_${TEST_NAME}_compile"
            FIXTURES_REQUIRED "clang_built"
        )

        # Execution test using binary .o file (may fail due to relocation encoding issues)
        # Uses verify_output.cmake script to also check the program output
        add_test(
            NAME "cucaracha_${TEST_NAME}_execute_binary"
            COMMAND ${CMAKE_COMMAND}
                -DGO_EXECUTABLE=${GO_EXECUTABLE}
                -DCUCARACHA_GO_PROJECT_DIR=${CUCARACHA_GO_PROJECT_DIR}
                -DPROGRAM=${OUTPUT_PATH}.o
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_${TEST_NAME}_execute_binary" PROPERTIES
            LABELS "cucaracha;execution-binary"
            TIMEOUT 30
            DEPENDS "cucaracha_${TEST_NAME}_binary"
            FIXTURES_REQUIRED "clang_built"
        )
    endif()

    message(STATUS "Cucaracha Test Added: ${TEST_NAME} (Source: ${SOURCE_FILE}, Expected Output: ${ARG_EXPECTED_OUTPUT})")
endfunction()

# Helper function to add compilation and execution tests for a C++ program
function(add_cucaracha_cpp_test TEST_NAME SOURCE_FILE)
    # Parse optional arguments (EXPECTED_OUTPUT defaults to "0", CXX_STANDARD defaults to 14)
    cmake_parse_arguments(ARG "" "EXPECTED_OUTPUT;CXX_STANDARD" "" ${ARGN})
    if(NOT DEFINED ARG_EXPECTED_OUTPUT)
        set(ARG_EXPECTED_OUTPUT "0")
    endif()
    if(NOT DEFINED ARG_CXX_STANDARD)
        set(ARG_CXX_STANDARD "14")
    endif()

    set(SOURCE_PATH "${PROGRAMS_CPP_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${TEST_OUTPUT_DIR}/cpp_${TEST_NAME}")

    add_test(NAME cucaracha_cpp_${TEST_NAME}_setup
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_DIR}"
    )
    
    # Add LLVM IR generation test for C++
    add_test(
        NAME "cucaracha_cpp_${TEST_NAME}_llvm_ir"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            -fno-rtti
            -O0 -S -emit-llvm
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.ll"
    )
    
    # Set test properties for LLVM IR generation
    set_tests_properties("cucaracha_cpp_${TEST_NAME}_llvm_ir" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;llvm-ir"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_${TEST_NAME}_setup"
    )

    # Add compilation test for C++ - generates cucaracha assembly
    add_test(
        NAME "cucaracha_cpp_${TEST_NAME}_compile"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            -fno-rtti
            ${CUCARACHA_TESTS_COMPILE_OPTIONS}
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.cucaracha"
    )
    
    # Set test properties with clang dependency
    set_tests_properties("cucaracha_cpp_${TEST_NAME}_compile" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;compilation"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_${TEST_NAME}_llvm_ir"
    )

    # Add binary object file generation test (.o ELF file) for C++
    add_test(
        NAME "cucaracha_cpp_${TEST_NAME}_binary"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            -fno-rtti
            -O0 -c
            "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.o"
    )
    
    # Set test properties for binary compilation
    set_tests_properties("cucaracha_cpp_${TEST_NAME}_binary" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;binary"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_${TEST_NAME}_compile"
    )
    
    # Only add execution test if CLI is available
    if(CUCARACHA_CLI_AVAILABLE)
        # Execution test using assembly file
        add_test(
            NAME "cucaracha_cpp_${TEST_NAME}_execute"
            COMMAND ${CMAKE_COMMAND}
                -DCUCARACHA_CLI=${CUCARACHA_CLI}
                -DPROGRAM=${OUTPUT_PATH}.cucaracha
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_cpp_${TEST_NAME}_execute" PROPERTIES
            LABELS "cucaracha;cucaracha-cpp;execution"
            TIMEOUT 60
            DEPENDS "cucaracha_cpp_${TEST_NAME}_compile"
            FIXTURES_REQUIRED "clang_built"
        )

        # Execution test using binary .o file
        add_test(
            NAME "cucaracha_cpp_${TEST_NAME}_execute_binary"
            COMMAND ${CMAKE_COMMAND}
                -DCUCARACHA_CLI=${CUCARACHA_CLI}
                -DPROGRAM=${OUTPUT_PATH}.o
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_cpp_${TEST_NAME}_execute_binary" PROPERTIES
            LABELS "cucaracha;cucaracha-cpp;execution-binary"
            TIMEOUT 60
            DEPENDS "cucaracha_cpp_${TEST_NAME}_binary"
            FIXTURES_REQUIRED "clang_built"
        )
    endif()

    message(STATUS "Cucaracha C++ Test Added: ${TEST_NAME} (Source: ${SOURCE_FILE}, C++${ARG_CXX_STANDARD}, Expected Output: ${ARG_EXPECTED_OUTPUT})")
endfunction()

# Helper function to add C++ tests with RTTI enabled (for virtual functions, dynamic_cast)
function(add_cucaracha_cpp_rtti_test TEST_NAME SOURCE_FILE)
    # Parse optional arguments
    cmake_parse_arguments(ARG "" "EXPECTED_OUTPUT;CXX_STANDARD" "" ${ARGN})
    if(NOT DEFINED ARG_EXPECTED_OUTPUT)
        set(ARG_EXPECTED_OUTPUT "0")
    endif()
    if(NOT DEFINED ARG_CXX_STANDARD)
        set(ARG_CXX_STANDARD "14")
    endif()

    set(SOURCE_PATH "${PROGRAMS_CPP_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${TEST_OUTPUT_DIR}/cpp_rtti_${TEST_NAME}")

    add_test(NAME cucaracha_cpp_rtti_${TEST_NAME}_setup
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_DIR}"
    )
    
    # Add LLVM IR generation test for C++ with RTTI
    add_test(
        NAME "cucaracha_cpp_rtti_${TEST_NAME}_llvm_ir"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            -O0 -S -emit-llvm
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.ll"
    )
    
    set_tests_properties("cucaracha_cpp_rtti_${TEST_NAME}_llvm_ir" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;cucaracha-cpp-rtti;llvm-ir"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_rtti_${TEST_NAME}_setup"
    )

    # Add compilation test for C++ with RTTI
    add_test(
        NAME "cucaracha_cpp_rtti_${TEST_NAME}_compile"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            ${CUCARACHA_TESTS_COMPILE_OPTIONS}
            -c "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.cucaracha"
    )
    
    set_tests_properties("cucaracha_cpp_rtti_${TEST_NAME}_compile" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;cucaracha-cpp-rtti;compilation"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_rtti_${TEST_NAME}_llvm_ir"
    )

    # Add binary object file generation test
    add_test(
        NAME "cucaracha_cpp_rtti_${TEST_NAME}_binary"
        COMMAND clang++
            --target=cucaracha
            -std=c++${ARG_CXX_STANDARD}
            -fno-exceptions
            -O0 -c
            "${SOURCE_PATH}"
            -o "${OUTPUT_PATH}.o"
    )
    
    set_tests_properties("cucaracha_cpp_rtti_${TEST_NAME}_binary" PROPERTIES
        LABELS "cucaracha;cucaracha-cpp;cucaracha-cpp-rtti;binary"
        TIMEOUT 60
        DEPENDS "cucaracha_cpp_rtti_${TEST_NAME}_compile"
    )
    
    if(CUCARACHA_CLI_AVAILABLE)
        add_test(
            NAME "cucaracha_cpp_rtti_${TEST_NAME}_execute"
            COMMAND ${CMAKE_COMMAND}
                -DCUCARACHA_CLI=${CUCARACHA_CLI}
                -DPROGRAM=${OUTPUT_PATH}.cucaracha
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_cpp_rtti_${TEST_NAME}_execute" PROPERTIES
            LABELS "cucaracha;cucaracha-cpp;cucaracha-cpp-rtti;execution"
            TIMEOUT 60
            DEPENDS "cucaracha_cpp_rtti_${TEST_NAME}_compile"
            FIXTURES_REQUIRED "clang_built"
        )

        add_test(
            NAME "cucaracha_cpp_rtti_${TEST_NAME}_execute_binary"
            COMMAND ${CMAKE_COMMAND}
                -DCUCARACHA_CLI=${CUCARACHA_CLI}
                -DPROGRAM=${OUTPUT_PATH}.o
                -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
                -P ${SCRIPTS_DIR}/verify_output.cmake
        )
        
        set_tests_properties("cucaracha_cpp_rtti_${TEST_NAME}_execute_binary" PROPERTIES
            LABELS "cucaracha;cucaracha-cpp;cucaracha-cpp-rtti;execution-binary"
            TIMEOUT 60
            DEPENDS "cucaracha_cpp_rtti_${TEST_NAME}_binary"
            FIXTURES_REQUIRED "clang_built"
        )
    endif()

    message(STATUS "Cucaracha C++ RTTI Test Added: ${TEST_NAME} (Source: ${SOURCE_FILE}, C++${ARG_CXX_STANDARD}, Expected Output: ${ARG_EXPECTED_OUTPUT})")
endfunction()

# Define test programs
# Each test specifies EXPECTED_OUTPUT which is verified after execution
# Most programs return 0 on success, hello_world returns 42 as a sanity check
add_cucaracha_test("hello_world" "hello_world.c" EXPECTED_OUTPUT "42")
add_cucaracha_test("arithmetic" "arithmetic.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("fibonacci" "fibonacci.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("factorial" "factorial.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("loops" "loops.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("conditionals" "conditionals.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("arrays" "arrays.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("functions" "functions.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("structs" "structs.c" EXPECTED_OUTPUT "0")
add_cucaracha_test("function_pointers" "function_pointers.c" EXPECTED_OUTPUT "0")

# ============================================================================
# C++ Test Programs
# ============================================================================
# Tests to verify C++ language support in the Cucaracha target
# All tests are written without stdlib dependencies (freestanding)

message(STATUS "")
message(STATUS "Adding C++ tests for Cucaracha target...")

# Basic C++ features (no RTTI required)
add_cucaracha_cpp_test("basic_class" "basic_class.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("inheritance" "inheritance.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("constructors_destructors" "constructors_destructors.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("member_functions" "member_functions.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("operator_overloading" "operator_overloading.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("references" "references.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("namespaces" "namespaces.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("templates" "templates.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("template_specialization" "template_specialization.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("static_members" "static_members.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("multiple_inheritance" "multiple_inheritance.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("new_delete" "new_delete.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("type_casting" "type_casting.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("default_args_overloading" "default_args_overloading.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("structs_pods" "structs_pods.cpp" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("atomics" "atomics.cpp" EXPECTED_OUTPUT "0")

# C++11 specific features
add_cucaracha_cpp_test("cpp11_features" "cpp11_features.cpp" CXX_STANDARD "11" EXPECTED_OUTPUT "0")
add_cucaracha_cpp_test("lambdas" "lambdas.cpp" CXX_STANDARD "14" EXPECTED_OUTPUT "0")

# Tests requiring RTTI (virtual functions with vtables)
add_cucaracha_cpp_rtti_test("virtual_functions" "virtual_functions.cpp" EXPECTED_OUTPUT "0")

# Report what tests will be available
if(CUCARACHA_CLI_AVAILABLE)
    message(STATUS "Cucaracha Tests: Compilation and execution tests enabled")
else()
    message(STATUS "Cucaracha Tests: Only compilation tests enabled (Go or Cucaracha project not found)")
endif()

# Custom target to run all cucaracha tests
add_custom_target(cucaracha_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L cucaracha --output-on-failure
    COMMENT "Running all Cucaracha toolchain tests"
    USES_TERMINAL
)

# Custom target to run only C tests
add_custom_target(cucaracha_c_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L cucaracha -LE cucaracha-cpp --output-on-failure
    COMMENT "Running Cucaracha C tests only"
    USES_TERMINAL
)

# Custom target to run only C++ tests
add_custom_target(cucaracha_cpp_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L cucaracha-cpp --output-on-failure
    COMMENT "Running Cucaracha C++ tests only"
    USES_TERMINAL
)

# Custom target to run only compilation tests (no execution)
add_custom_target(cucaracha_compile_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L compilation --output-on-failure
    COMMENT "Running Cucaracha compilation tests only"
    USES_TERMINAL
)

message(STATUS "Cucaracha Tests: Added ${CMAKE_CURRENT_SOURCE_DIR} with Go: ${GO_EXECUTABLE} and Clang target dependency")
message(STATUS "Cucaracha Tests: Run 'make cucaracha_tests' for all tests")
message(STATUS "Cucaracha Tests: Run 'make cucaracha_cpp_tests' for C++ tests only")
message(STATUS "Cucaracha Tests: Run 'ctest -L cucaracha-cpp' to run C++ tests via ctest")
