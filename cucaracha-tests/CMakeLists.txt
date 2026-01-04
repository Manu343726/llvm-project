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

message(STATUS "Cucaracha Tests: Added ${CMAKE_CURRENT_SOURCE_DIR} with Go: ${GO_EXECUTABLE} and Clang target dependency")