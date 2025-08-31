add_llvm_component_group(SCISA)

set(LLVM_TARGET_DEFINITIONS SCISA.td)

tablegen(LLVM SCISAGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM SCISAGenCallingConv.inc -gen-callingconv)
tablegen(LLVM SCISAGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM SCISAGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM SCISAGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM SCISAGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM SCISAGenRegisterBank.inc -gen-register-bank)

add_public_tablegen_target(SCISACommonTableGen)

add_llvm_target(SCISACodeGen
  SCISAAsmPrinter.cpp
  SCISAASpaceCastSimplifyPass.cpp
  SCISAFrameLowering.cpp
  SCISAInstrInfo.cpp
  SCISAISelDAGToDAG.cpp
  SCISAISelLowering.cpp
  SCISAMCInstLower.cpp
  SCISARegisterInfo.cpp
  SCISASelectionDAGInfo.cpp
  SCISASubtarget.cpp
  SCISATargetMachine.cpp
  SCISAMIPeephole.cpp
  SCISAMachineFunctionInfo.cpp
  SCISATargetObjectFile.cpp
  LINK_COMPONENTS
  Analysis
  AsmPrinter
  SCISADesc
  SCISAInfo
  CodeGen
  CodeGenTypes
  Core
  IPO
  MC
  Passes
  Scalar
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils

  ADD_TO_COMPONENT
  SCISA
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
