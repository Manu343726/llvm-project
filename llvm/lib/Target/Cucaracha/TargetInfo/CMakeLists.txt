add_llvm_component_library(LLVMCucarachaInfo
  CucarachaTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  Cucaracha
  )
