add_llvm_component_library(LLVMSCISAInfo
  SCISATargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  SCISA
  )
