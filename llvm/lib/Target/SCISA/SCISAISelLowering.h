//===-- SCISAISelLowering.h - SCISA DAG Lowering Interface ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that SCISA uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISAISELLOWERING_H
#define LLVM_LIB_TARGET_SCISA_SCISAISELLOWERING_H

#include "SCISA.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class SCISASubtarget;
namespace SCISAISD {

enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END,
    RET_GLUE,
    CALL,
    INDIRECT_CALL,
    SELECT_CC,
    BR_CC,
    CMP,
    CMOV,
    Wrapper,
    MEMCPY
};

} // namespace SCISAISD

class SCISATargetLowering : public TargetLowering {
public:
    explicit SCISATargetLowering(const TargetMachine &TM, const SCISASubtarget &STI);

    // Provide custom lowering hooks for some operations.
    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

    // This method returns the name of a target specific DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;

    // This method decides whether folding a constant offset with the given GlobalAddress is legal.
    bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;

    SCISATargetLowering::ConstraintType getConstraintType(StringRef Constraint) const override;

    std::pair<unsigned, const TargetRegisterClass *> getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI, StringRef Constraint, MVT VT) const override;

    MachineBasicBlock *EmitInstrWithCustomInserter(MachineInstr &MI, MachineBasicBlock *BB) const override;

    EVT getSetCCResultType(const DataLayout &, LLVMContext &, EVT VT) const override
    {
        return MVT::i32;
    }

    MVT getScalarShiftAmountTy(const DataLayout &DL, EVT VT) const override
    {
        return MVT::i32;
    }

private:
    SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerSETCC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const;
    SDValue getSCISACmov(const SDLoc &DL, EVT VT, SDValue FalseVal, SDValue TrueVal, SDValue SCISAcc, SDValue Flags, SelectionDAG &DAG) const;
    SDValue LowerSTACKSAVE(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerSTACKRESTORE(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerConstantPool(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

    template <class NodeTy>
    SDValue getAddr(NodeTy *N, SelectionDAG &DAG, unsigned Flags = 0) const;

    // Lower the result values of a call, copying them out of physregs into vregs
    SDValue LowerCallResult(SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const;

    // Maximum number of arguments to a call
    static const size_t MaxArgs;

    // Lower a call into CALLSEQ_START - SCISAISD:CALL - CALLSEQ_END chain
    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const override;

    // Lower incoming arguments, copy physregs into vregs
    SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL, SelectionDAG &DAG) const override;

    void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results, SelectionDAG &DAG) const override;

    EVT getOptimalMemOpType(LLVMContext &Context, const MemOp &Op, const AttributeList &FuncAttributes) const override {
        return MVT::i32;
    }

    bool isIntDivCheap(EVT VT, AttributeList Attr) const override
    {
        return false;
    }

    bool shouldConvertConstantLoadToIntImm(const APInt &Imm, Type *Ty) const override
    {
        assert(Ty->isIntegerTy());
        unsigned Bits = Ty->getPrimitiveSizeInBits();
        if (Bits == 0 || Bits > 32) {
            return false;
        }
        return true;
    }

    bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, Type *Ty, unsigned AS, Instruction *I = nullptr) const override;

    MachineBasicBlock *emitInstrWithCustomInserterMemcpy(MachineInstr &MI, MachineBasicBlock *BB) const;
    MachineBasicBlock *emitInstrWithCustomInserterSelect(MachineInstr &MI, MachineBasicBlock *BB) const;
};

} // namespace llvm

#endif
