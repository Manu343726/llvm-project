add_llvm_component_group(Cucaracha)

# Path to the Cucaracha Go project (relative to this CMakeLists.txt)
set(CUCARACHA_GO_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cucaracha")
set(CUCARACHA_TD_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Cucaracha.td")

# Find Go executable
find_program(GO_EXECUTABLE go REQUIRED)

# Collect all Go source files and TD templates as dependencies
file(GLOB_RECURSE CUCARACHA_GO_SOURCES "${CUCARACHA_GO_PROJECT_DIR}/*.go")
file(GLOB_RECURSE CUCARACHA_TD_TEMPLATES "${CUCARACHA_GO_PROJECT_DIR}/*.td")

# Generate Cucaracha.td using the Go code generator
# This runs: go run . cpu generateLlvmTablegen -o Cucaracha.td
add_custom_command(
  OUTPUT ${CUCARACHA_TD_OUTPUT}
  COMMAND ${GO_EXECUTABLE} run . cpu generateLlvmTablegen -o ${CUCARACHA_TD_OUTPUT}
  WORKING_DIRECTORY ${CUCARACHA_GO_PROJECT_DIR}
  COMMENT "Generating Cucaracha.td from Go code generator"
  DEPENDS
    ${CUCARACHA_GO_SOURCES}
    ${CUCARACHA_TD_TEMPLATES}
)

set(LLVM_TARGET_DEFINITIONS ${CUCARACHA_TD_OUTPUT})

tablegen(LLVM CucarachaGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM CucarachaGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM CucarachaGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM CucarachaGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM CucarachaGenCallingConv.inc -gen-callingconv)
tablegen(LLVM CucarachaGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM CucarachaGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM CucarachaGenSearchableTables.inc -gen-searchable-tables)
add_public_tablegen_target(CucarachaCommonTableGen)

add_llvm_target(CucarachaCodeGen
  CucarachaSubtarget.cpp
  CucarachaTargetMachine.cpp
  CucarachaRegisterInfo.cpp
  CucarachaInstrInfo.cpp
  CucarachaFrameLowering.cpp
  CucarachaISelLowering.cpp
  CucarachaISelDAGToDAG.cpp
  CucarachaAsmPrinter.cpp
  CucarachaMCInstLower.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  CucarachaDesc
  CucarachaInfo
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  Cucaracha
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
