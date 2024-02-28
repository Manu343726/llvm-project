add_llvm_component_library(LLVMCucarachaAsmParser
  CucarachaAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  CucarachaDesc
  CucarachaInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  Cucaracha
  )
