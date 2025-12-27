add_llvm_component_group(Cucaracha)

find_program(CUCARACHA cucaracha)

if(CUCARACHA)
  add_custom_command(OUTPUT Cucaracha.td COMMAND ${CUCARACHA} ARGS tools generate-llvm-tablegen -o "${CMAKE_CURRENT_SOURCE_DIR}/Cucaracha.td")
endif()

set(LLVM_TARGET_DEFINITIONS Cucaracha.td)

tablegen(LLVM CucarachaGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM CucarachaGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM CucarachaGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM CucarachaGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM CucarachaGenCallingConv.inc -gen-callingconv)
tablegen(LLVM CucarachaGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM CucarachaGenMCCodeEmitter.inc -gen-emitter)
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
