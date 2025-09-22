//===-- SCISAInstrInfo.td - Target Description for SCISA Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the SCISA instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "SCISAInstrFormats.td"

// MARK: - SDTypes

// These are target-independent nodes, but have target-specific formats.
def SDT_SCISACallSeqStart : SDCallSeqStart<
  [
    SDTCisVT<0, i32>,
    SDTCisVT<1, i32>
  ]
>;

def SDT_SCISACallSeqEnd : SDCallSeqEnd<
  [
    SDTCisVT<0, i32>,
    SDTCisVT<1, i32>
  ]
>;

def SDT_SCISACall : SDTypeProfile<
  0, // no outputs
  -1, // no variadic inputs
  [
    SDTCisVT<0, i32> // calltarget32
  ]
>;
def SDT_SCISAIndirectCall : SDTypeProfile<
  0, // no outputs
  -1, // no variadic inputs
  [
    SDTCisVT<0, i32> // gpr32
  ]
>;

def SDT_SCISASelectCC : SDTypeProfile<
  1, // one output
  3, // three inputs
  [
    SDTCisSameAs<0, 1>, // lhs
    SDTCisSameAs<1, 2>, // rhs
    SDTCisVT<3, i32>    // cc
  ]
>;

def SDT_SCISABrCC : SDTypeProfile<
  0, // no outputs
  2, // two inputs
  [
    SDTCisVT<0, OtherVT>, // brtarget
    SDTCisVT<1, i32>      // cc
  ]
>;

def SDT_SCISACMov : SDTypeProfile<
  1, // one output
  3, // three inputs
  [
    SDTCisSameAs<0, 1>
  ]
>;

def SDT_SCISACmp : SDTypeProfile<
  0, // no outputs
  2, // two inputs
  [
    SDTCisSameAs<0, 1>
  ]
>;

def SDT_SCISAWrapper : SDTypeProfile<
  1, // one outputs
  1, // one input
  [
    SDTCisSameAs<0, 1>,
    SDTCisPtrTy<0>
  ]
>;

def SDT_SCISAMEMCPY : SDTypeProfile<
  0, // no outputs
  4, // four inputs
  [
    SDTCisVT<0, i32>, // dst
    SDTCisVT<1, i32>, // src
    SDTCisVT<2, i32>, // len
    SDTCisVT<3, i32>  // align
  ]
>;

// MARK: - SDNodes

def SCISAcall : SDNode<
  "SCISAISD::CALL",
  SDT_SCISACall,
  [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]
>;

def SCISAindirectcall : SDNode<
  "SCISAISD::INDIRECT_CALL",
  SDT_SCISAIndirectCall,
  [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

def SCISAretglue : SDNode<
  "SCISAISD::RET_GLUE",
  SDTNone,
  [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]
>;

def SCISAcallseq_start : SDNode<
  "ISD::CALLSEQ_START",
  SDT_SCISACallSeqStart,
  [SDNPHasChain, SDNPOutGlue]
>;

def SCISAcallseq_end : SDNode<
  "ISD::CALLSEQ_END",
  SDT_SCISACallSeqEnd,
  [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]
>;

def SCISAcmov : SDNode<
  "SCISAISD::CMOV",
  SDT_SCISACMov,
  [SDNPInGlue]
>;

def SCISAcmp : SDNode<
  "SCISAISD::CMP",
  SDT_SCISACmp,
  [SDNPOutGlue]
>;

def SCISAbrcc : SDNode<
  "SCISAISD::BR_CC",
  SDT_SCISABrCC,
  [SDNPHasChain, SDNPInGlue]
>;

def SCISAselectcc : SDNode<
  "SCISAISD::SELECT_CC",
  SDT_SCISASelectCC,
  [SDNPInGlue]
>;

def SCISAWrapper : SDNode<
  "SCISAISD::Wrapper",
  SDT_SCISAWrapper
>;

def SCISAmemcpy : SDNode<
  "SCISAISD::MEMCPY",
  SDT_SCISAMEMCPY,
  [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore, SDNPMayLoad]
>;

def brtarget : Operand<OtherVT> {
  let PrintMethod = "printBrTargetOperand";
}

def cc : Operand<i32> {
  let PrintMethod = "printCCOperand";
}

def calltarget32 : Operand<i32>;

def u32imm   : Operand<i32> {
  let PrintMethod = "printImm32Operand";
}

def gpr32_or_imm32 : Operand<i32>;

def i32immSExt32 : PatLeaf<
  (i32 imm),
  [{return isInt<32>(N->getSExtValue()); }]
>;

def imm_to_i32 : SDNodeXForm<
  timm,
  [{ return CurDAG->getTargetConstant(N->getZExtValue(), SDLoc(N), MVT::i32); }]
>;

// Addressing modes.
def ADDRri32 : ComplexPattern<
  i32,
  2,
  "selectAddr",
  [],
  []
>;

def FIri32 : ComplexPattern<
  i32,
  2,
  "selectFIAddr",
  [add, or],
  []
>;

// Address operands
def MEMri32 : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GPR32, i32imm);
}

// MARK: - cmov

def cmov : PatFrag<(ops node:$op1, node:$op2, node:$cc), (SCISAcmov $op1, $op2, $cc)>;

let Constraints = "$rd = $rs2", Uses = [CC] in {

  def CMOV_rr : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs, GPR32:$rs2, cc:$cond),
    "mov$cond\t$rd, $rs",
    [(set GPR32:$rd, (cmov i32:$rs, i32:$rs2, imm:$cond))]
  >;

  def CMOV_ri : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs2, i32imm:$imm, cc:$cond),
    "mov$cond\t$rd, $imm",
    [(set GPR32:$rd, (cmov GPR32:$rs2, i32immSExt32:$imm, imm:$cond))]
  >;
} // isCodeGenOnly = 1, Uses = [CC]

// MARK: - BCC

let isBranch = 1, isTerminator = 1 in {
  let Uses = [CC] in {

    def BCC : InstSCISA<
      (outs),
      (ins brtarget:$BrDst, cc:$cond),
      "b$cond\t$BrDst",
      [(SCISAbrcc bb:$BrDst, imm:$cond)]
    >;

  } // Uses = [CC]
} // isBranch = 1, isTerminator = 1

// MARK: - cmp

def cmp : PatFrag<(ops node:$op1, node:$op2), (SCISAcmp $op1, $op2)>;

let isCompare = 1, Defs = [CC] in {

  def CMP_rr_32 : InstSCISA<
    (outs),
    (ins GPR32:$rd, GPR32:$rs),
    "cmp\t$rd, $rs",
    [(cmp i32:$rd, i32:$rs)]
  >;

  def CMP_ri_32 : InstSCISA<
    (outs),
    (ins GPR32:$rd, i32imm:$imm),
    "cmp\t$rd, $imm",
    [(cmp GPR32:$rd, i32immSExt32:$imm)]
  >;

  def CMP_ir_32 : InstSCISA<
    (outs),
    (ins i32imm:$imm, GPR32:$rs),
    "cmp\t$imm, $rs",
    [(cmp i32immSExt32:$imm, GPR32:$rs)]
  >;
} // isCompare = 1, Defs = [CC]

// MARK: - ALU

multiclass ALU<string OpcodeStr, SDNode OpNode> {
  def _rr_32 : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs2, GPR32:$rs),
    !strconcat(OpcodeStr, "\t$rd, $rs"),
    [(set GPR32:$rd, (OpNode i32:$rs2, i32:$rs))]
  >;

  def _ri_32 : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs2, i32imm:$imm),
    !strconcat(OpcodeStr, "\t$rd, $imm"),
    [(set GPR32:$rd, (OpNode GPR32:$rs2, i32immSExt32:$imm))]
  >;
}

let Constraints = "$rd = $rs2" in {
  let isAsCheapAsAMove = 1 in {
    defm ADD : ALU<"add", add>;
    defm SUB : ALU<"sub", sub>;
    defm OR  : ALU<"or", or>;
    defm AND : ALU<"and", and>;
    defm SLL : ALU<"lsl", shl>;
    defm SRL : ALU<"lsr", srl>;
    defm XOR : ALU<"xor", xor>;
    defm SRA : ALU<"asr", sra>;
    defm MUL : ALU<"mul", mul>;
    defm DIV : ALU<"div", udiv>;
    defm MOD : ALU<"mod", urem>;
  } // isAsCheapAsAMove = 1

  defm SDIV : ALU<"sdiv", sdiv>;
  defm SMOD : ALU<"smod", srem>;
} // Constraints = "$rd = $rs2"

let Constraints = "$rd = $rs", isAsCheapAsAMove = 1 in {
  def NEG : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs),
    "neg\t$rd",
    [(set GPR32:$rd, (ineg i32:$rs))]
  >;

  def INC : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs),
    "inc\t$rd",
    [(set GPR32:$rd, (add i32:$rs, 1))]
  >;

  def DEC : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs),
    "dec\t$rd",
    [(set GPR32:$rd, (add i32:$rs, -1))]
  >;
} // Constraints = "$rd = $rs2", isAsCheapAsAMove = 1

// MARK: - MOV

let isReMaterializable = 1, isAsCheapAsAMove = 1, hasSideEffects = 0 in {
  def MOV_rr_32 : InstSCISA<
    (outs GPR32:$rd),
    (ins GPR32:$rs),
    "mov\t$rd, $rs",
    []
  >;

  def MOV_ri_32 : InstSCISA<
    (outs GPR32:$rd),
    (ins i32imm:$imm),
    "mov\t$rd, $imm",
    [(set GPR32:$rd, (i32 i32immSExt32:$imm))]
  >;
} // isReMaterializable = 1, isAsCheapAsAMove = 1, hasSideEffects = 0

// MARK: - readcyclecounter

// Read cycle counter
let Uses = [CYC] in {
  def READ_REG : InstSCISA<
    (outs GPR32:$rd),
    (ins CYCREG:$rs),
    "mov\t$rd, $rs",
    []
  >;
} // Uses = [CYC]

def: Pat<(i32 readcyclecounter), (READ_REG CYC) >;

// MARK: - lea

def LD_imm32 : InstSCISA<
  (outs GPR32:$rd),
  (ins u32imm:$imm),
  "lea\t$rd, ${imm}",
  [(set GPR32:$rd, (i32 imm:$imm))]
>;

let isPseudo = 1 in {
  def FI_ri32 : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "lea\t$rd, $addr",
    [(set i32:$rd, FIri32:$addr)]
  >;
} // isPseudo = 1

// MARK: - store imm

let mayStore = 1 in {
  def STW_imm32 : InstSCISA<
    (outs),
    (ins i32imm:$imm, MEMri32:$addr),
    "stw\t$imm, $addr",
    [(store (i32 i32immSExt32:$imm), ADDRri32:$addr)]
  >;

  def STH_imm32 : InstSCISA<
    (outs),
    (ins i32imm:$imm, MEMri32:$addr),
    "sth\t$imm, $addr",
    [(truncstorei16 (i32 i32immSExt32:$imm), ADDRri32:$addr)]
  >;

  def STB_imm32 : InstSCISA<
    (outs),
    (ins i32imm:$imm, MEMri32:$addr),
    "stb\t$imm, $addr",
    [(truncstorei8 (i32 i32immSExt32:$imm), ADDRri32:$addr)]
  >;
} // mayStore = 1


def : Pat<(store (i32 imm:$src), ADDRri32:$dst), (STW_imm32 (imm_to_i32 $src), ADDRri32:$dst)>;

def : Pat<(truncstorei16 (i32 imm:$src), ADDRri32:$dst), (STH_imm32 (imm_to_i32 imm:$src), ADDRri32:$dst)>;

def : Pat<(truncstorei8 (i32 imm:$src), ADDRri32:$dst), (STB_imm32 (imm_to_i32 imm:$src), ADDRri32:$dst)>;

// MARK: - branches

let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  // unconditional branch
  def B : InstSCISA<
    (outs),
    (ins brtarget:$BrDst),
    "b\t$BrDst",
    [(br bb:$BrDst)]
  >;

  // indirect branch
  let isIndirectBranch = 1 in {
      def BR : InstSCISA<
        (outs),
        (ins GPR32:$BrDst),
        "br\t($BrDst)",
        [(brind GPR32:$BrDst)]
    >;
  }
}

let isCall = 1, isBarrier = 1, Defs = [LR], Uses = [SP] in {
  // branch & link (call)
  def BL : InstSCISA<
    (outs),
    (ins calltarget32:$func),
    "bl\t$func",
    []
  >;

  // branch & link (indirect call)
  let isIndirectBranch = 1 in {
    def BLR : InstSCISA<
      (outs),
      (ins GPR32:$BrDst),
      "blr\t($BrDst)",
      [(SCISAindirectcall i32:$BrDst)]
    >;
  } // isIndirectBranch = 1
} // isCall = 1, isBarrier = 1, Defs = [LR], Uses = [SP]

def : Pat<(SCISAcall tglobaladdr:$dst),  (BL tglobaladdr:$dst)>;
def : Pat<(SCISAcall texternalsym:$dst), (BL texternalsym:$dst)>;

// I don't *think* `LR` needs included in Defs since `RET` only reads it
// TODO: confirm and remove comment
let Defs = [SP], Uses = [SP, LR] in {
  let isReturn = 1, isTerminator = 1, isBarrier = 1, isNotDuplicable = 1 in {
    def RET32 : InstSCISA<
      (outs),
      (ins),
      "ret",
      [(SCISAretglue)]
    >;
  } // isReturn = 1, isTerminator = 1, isBarrier = 1, isNotDuplicable = 1
} // Defs = [SP], Uses = [SP, LR]

// ADJCALLSTACKDOWN/UP pseudo insns
let Defs = [SP], Uses = [SP] in {

  def ADJCALLSTACKDOWN : Pseudo<
    (outs),
    (ins i32imm:$amt1, i32imm:$amt2),
    "#ADJCALLSTACKDOWN $amt1 $amt2",
    [(SCISAcallseq_start timm:$amt1, timm:$amt2)]
  >;

  def ADJCALLSTACKUP : Pseudo<
    (outs),
    (ins i32imm:$amt1, i32imm:$amt2),
    "#ADJCALLSTACKUP $amt1 $amt2",
    [(SCISAcallseq_end timm:$amt1, timm:$amt2)]
  >;
} // Defs = [SP], Uses = [SP]

let usesCustomInserter = 1 in {
  def Select_32 : Pseudo<
    (outs GPR32:$rd),
    (ins GPR32:$lhs, GPR32:$rhs, i32imm:$cc),
    "# Select_rr PSEUDO",
    [(set i32:$rd, (SCISAselectcc i32:$lhs, i32:$rhs, imm:$cc))]
  >;
} // usesCustomInserter = 1

def : Pat<(SCISAWrapper tglobaladdr:$in), (LD_imm32 tglobaladdr:$in)>;
def : Pat<(SCISAWrapper tconstpool:$in), (LD_imm32 tconstpool:$in)>;

// MARK: - store

let mayStore = 1 in {
  def STW32 : InstSCISA<
    (outs),
    (ins GPR32:$rs, MEMri32:$addr),
    "stw\t$rs, $addr",
    [(store GPR32:$rs, ADDRri32:$addr)]
  >;

  def STH32 : InstSCISA<
    (outs),
    (ins GPR32:$rs, MEMri32:$addr),
    "sth\t$rs, $addr",
    [(truncstorei16 GPR32:$rs, ADDRri32:$addr)]
  >;

  def STB32 : InstSCISA<
    (outs),
    (ins GPR32:$rs, MEMri32:$addr),
    "stb\t$rs, $addr",
    [(truncstorei8 GPR32:$rs, ADDRri32:$addr)]
  >;
} // mayStore = 1

// MARK: - load

let mayLoad = 1 in {
  def LDW32  : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "ldw\t$rd, $addr",
    [(set i32:$rd, (load ADDRri32:$addr))]
  >;

  def LDUH32 : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "lduh\t$rd, $addr",
    [(set i32:$rd, (zextloadi16 ADDRri32:$addr))]
  >;

  def LDUB32 : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "ldub\t$rd, $addr",
    [(set i32:$rd, (zextloadi8 ADDRri32:$addr))]
  >;

  def LDSH32 : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "ldsh\t$rd, $addr",
    [(set i32:$rd, (sextloadi16 ADDRri32:$addr))]
  >;

  def LDSB32 : InstSCISA<
    (outs GPR32:$rd),
    (ins MEMri32:$addr),
    "ldsb\t$rd, $addr",
    [(set i32:$rd, (sextloadi8 ADDRri32:$addr))]
  >;
} // mayLoad = 1

def : Pat<(i32 (extloadi8 ADDRri32:$src)), (i32 (LDUB32 ADDRri32:$src))>;

def : Pat<(i32 (extloadi16 ADDRri32:$src)), (i32 (LDUH32 ADDRri32:$src))>;


// MARK: - push/pop

let Defs = [SP], Uses = [SP], hasSideEffects = 0 in {
  // Stack push operations.
  let mayStore = 1 in {
    def PUSH : InstSCISA<
      (outs),
      (ins GPR32:$rd),
      "push\t$rd",
      []>;
  } // mayStore = 1

  // Stack pop operations.
  let mayLoad = 1 in {
    def POP : InstSCISA<
      (outs GPR32:$rd),
      (ins),
      "pop\t$rd",
      []>;
  } // mayLoad = 1
} // Defs = [SP], Uses = [SP], hasSideEffects = 0

let usesCustomInserter = 1 in {
  def MEMCPY : Pseudo<
    (outs),
    (ins GPR32:$dst, GPR32:$src, i32imm:$len, i32imm:$align, variable_ops),
    "#memcpy dst: $dst, src: $src, len: $len, align: $align",
    [(SCISAmemcpy GPR32:$dst, GPR32:$src, imm:$len, imm:$align)]
  >;
} // usesCustomInserter = 1
