add_llvm_component_library(LLVMSCISADesc
  SCISAMCTargetDesc.cpp
  SCISAInstPrinter.cpp

  LINK_COMPONENTS
  MC
  SCISAInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  SCISA
  )
