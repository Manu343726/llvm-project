set(LLVM_TARGET_DEFINITIONS Fantasy.td)

tablegen(LLVM FantasyGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM FantasyGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM FantasyGenCallingConv.inc -gen-callingconv)
tablegen(LLVM FantasyGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM FantasyGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM FantasyGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM FantasyGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM FantasyGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM FantasyGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(FantasyCommonTableGen)

add_llvm_target(FantasyCodeGen
  FantasyAsmPrinter.cpp
  FantasyDelaySlotFiller.cpp
  FantasyFrameLowering.cpp
  FantasyInstrInfo.cpp
  FantasyISelDAGToDAG.cpp
  FantasyISelLowering.cpp
  FantasyMachineFunctionInfo.cpp
  FantasyMCInstLower.cpp
  FantasyMemAluCombiner.cpp
  FantasyRegisterInfo.cpp
  FantasySelectionDAGInfo.cpp
  FantasySubtarget.cpp
  FantasyTargetMachine.cpp
  FantasyTargetObjectFile.cpp
)

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
