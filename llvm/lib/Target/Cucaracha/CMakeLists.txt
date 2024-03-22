add_llvm_component_group(Cucaracha)

set(LLVM_TARGET_DEFINITIONS Cucaracha.td)

tablegen(LLVM CucarachaGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM CucarachaGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM CucarachaGenCallingConv.inc -gen-callingconv)
tablegen(LLVM CucarachaGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM CucarachaGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM CucarachaGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM CucarachaGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM CucarachaGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM CucarachaGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(CucarachaCommonTableGen)

add_llvm_target(CucarachaCodeGen
  CucarachaAsmPrinter.cpp
  CucarachaInstrInfo.cpp
  CucarachaISelDAGToDAG.cpp
  CucarachaISelLowering.cpp
  CucarachaFrameLowering.cpp
  CucarachaMachineFunctionInfo.cpp
  CucarachaRegisterInfo.cpp
  CucarachaSubtarget.cpp
  CucarachaTargetMachine.cpp
  CucarachaMCInstLower.cpp
  CucarachaTargetObjectFile.cpp

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

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
