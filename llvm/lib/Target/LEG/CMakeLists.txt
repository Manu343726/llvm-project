add_llvm_component_group(LEG)

set(LLVM_TARGET_DEFINITIONS LEG.td)

tablegen(LLVM LEGGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM LEGGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM LEGGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM LEGGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM LEGGenCallingConv.inc -gen-callingconv)
tablegen(LLVM LEGGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM LEGGenMCCodeEmitter.inc -gen-emitter)
add_public_tablegen_target(LEGCommonTableGen)

add_llvm_target(LEGCodeGen
  LEGSubtarget.cpp
  LEGTargetMachine.cpp
  LEGRegisterInfo.cpp
  LEGInstrInfo.cpp
  LEGFrameLowering.cpp
  LEGISelLowering.cpp
  LEGISelDAGToDAG.cpp
  LEGAsmPrinter.cpp
  LEGMCInstLower.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  LEGDesc
  LEGInfo
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  LEG
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
