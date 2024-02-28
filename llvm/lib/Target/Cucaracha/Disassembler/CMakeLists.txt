add_llvm_component_library(LLVMCucarachaDisassembler
  CucarachaDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  MC
  CucarachaInfo
  Support

  ADD_TO_COMPONENT
  Cucaracha
  )
