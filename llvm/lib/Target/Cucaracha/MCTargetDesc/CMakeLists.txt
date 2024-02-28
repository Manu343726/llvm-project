add_llvm_component_library(LLVMCucarachaDesc
  CucarachaAsmBackend.cpp
  CucarachaELFObjectWriter.cpp
  CucarachaInstPrinter.cpp
  CucarachaMCAsmInfo.cpp
  CucarachaMCCodeEmitter.cpp
  CucarachaMCTargetDesc.cpp
  CucarachaMCExpr.cpp
  CucarachaTargetStreamer.cpp

  LINK_COMPONENTS
  MC
  CucarachaInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  Cucaracha
  )
