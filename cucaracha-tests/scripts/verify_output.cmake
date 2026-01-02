# CMake script to run cucaracha program and verify output
# Usage: cmake -DCUCARACHA_CLI=<path> -DPROGRAM=<path> -DEXPECTED_OUTPUT=<value> -P verify_output.cmake

if(NOT DEFINED CUCARACHA_CLI)
    message(FATAL_ERROR "CUCARACHA_CLI not defined")
endif()

if(NOT DEFINED PROGRAM)
    message(FATAL_ERROR "PROGRAM not defined")
endif()

if(NOT DEFINED EXPECTED_OUTPUT)
    message(FATAL_ERROR "EXPECTED_OUTPUT not defined")
endif()

# Run the program and capture output
execute_process(
    COMMAND "${CUCARACHA_CLI}" cpu exec "${PROGRAM}"
    OUTPUT_VARIABLE ACTUAL_OUTPUT
    ERROR_VARIABLE ERROR_OUTPUT
    RESULT_VARIABLE EXIT_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Check if execution succeeded
if(NOT EXIT_CODE EQUAL 0)
    message(FATAL_ERROR "Program execution failed with exit code ${EXIT_CODE}\nError: ${ERROR_OUTPUT}")
endif()

# Compare output
if(NOT "${ACTUAL_OUTPUT}" STREQUAL "${EXPECTED_OUTPUT}")
    message(FATAL_ERROR "Output mismatch!\n  Expected: '${EXPECTED_OUTPUT}'\n  Actual:   '${ACTUAL_OUTPUT}'")
endif()

message(STATUS "Output verified: ${ACTUAL_OUTPUT}")
