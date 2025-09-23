//===-- SCISAISelLowering.cpp - SCISA DAG Lowering Implementation  --------===//
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

#include "SCISAISelLowering.h"
#include "SCISA.h"
#include "SCISASubtarget.h"
#include "SCISATargetObjectFile.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/IntrinsicsSCISA.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "scisa-lowering"

static void fail(const SDLoc &DL, SelectionDAG &DAG, const Twine &Msg, SDValue Val = {})
{
    std::string Str;
    if (Val) {
        raw_string_ostream OS(Str);
        Val->print(OS);
        OS << ' ';
    }
    MachineFunction &MF = DAG.getMachineFunction();
    DAG.getContext()->diagnose(DiagnosticInfoUnsupported(MF.getFunction(), Twine(Str).concat(Msg), DL.getDebugLoc()));
}

SCISATargetLowering::SCISATargetLowering(const TargetMachine &TM, const SCISASubtarget &STI)
    : TargetLowering(TM)
{

    addRegisterClass(MVT::i32, &SCISA::GPR32RegClass);

    // Compute derived properties from the register classes
    computeRegisterProperties(STI.getRegisterInfo());

    setStackPointerRegisterToSaveRestore(SCISA::SP);

    setOperationAction(ISD::BR_JT, MVT::Other, Expand);
    setOperationAction(ISD::BRCOND, MVT::Other, Expand);

    setOperationAction({ ISD::GlobalAddress, ISD::ConstantPool }, MVT::i32, Custom);

    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32, Custom);
    setOperationAction(ISD::STACKSAVE, MVT::Other, Custom);
    setOperationAction(ISD::STACKRESTORE, MVT::Other, Custom);

    setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
    setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

    setOperationAction(ISD::MULHU, MVT::i32, Expand);
    setOperationAction(ISD::MULHS, MVT::i32, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::ROTR, MVT::i32, Expand);
    setOperationAction(ISD::ROTL, MVT::i32, Expand);
    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    // setOperationAction(ISD::CTPOP, MVT::i32, Legal);
    // setOperationAction(ISD::CTTZ, MVT::i32, Legal);
    // setOperationAction(ISD::CTLZ, MVT::i32, Legal);
    setOperationAction(ISD::CTTZ_ZERO_UNDEF, MVT::i32, Expand);
    setOperationAction(ISD::CTLZ_ZERO_UNDEF, MVT::i32, Expand);

    setOperationAction(ISD::SETCC, MVT::i32, Custom);

    setOperationAction(ISD::SELECT, MVT::i32, Expand);

    setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
    setOperationAction(ISD::BR_CC, MVT::i32, Custom);

    setOperationAction(ISD::BSWAP, MVT::i32, Expand);

    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Expand);

    for (MVT VT : MVT::integer_valuetypes()) {
        setLoadExtAction(ISD::EXTLOAD, VT, MVT::i1, Promote);
        setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1, Promote);
        setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1, Promote);
    }

    setOperationAction(ISD::READCYCLECOUNTER, MVT::i32, Legal);
    setOperationAction(ISD::READCYCLECOUNTER, MVT::i64, isTypeLegal(MVT::i64) ? Legal : Custom);

    setBooleanContents(ZeroOrOneBooleanContent);
    setMaxAtomicSizeInBitsSupported(0);

    // Function alignments
    setMinFunctionAlignment(Align(1));
    setPrefFunctionAlignment(Align(1));

#if 0
        MaxStoresPerMemset = 0;
        MaxStoresPerMemsetOptSize = 0;
        MaxStoresPerMemcpy = 0;
        MaxStoresPerMemcpyOptSize = 0;
        MaxStoresPerMemmove = 0;
        MaxStoresPerMemmoveOptSize = 0;
        MaxLoadsPerMemcmp = 0;
#else

        unsigned CommonMaxStores = STI.getSelectionDAGInfo()->getCommonMaxStoresPerMemFunc();

        MaxStoresPerMemsetOptSize = CommonMaxStores;
        MaxStoresPerMemset = MaxStoresPerMemsetOptSize;
        MaxStoresPerMemcpyOptSize = CommonMaxStores;
        MaxStoresPerMemcpy = MaxStoresPerMemcpyOptSize;
        MaxStoresPerMemmoveOptSize = CommonMaxStores;
        MaxStoresPerMemmove = MaxStoresPerMemmoveOptSize;
        MaxLoadsPerMemcmpOptSize = CommonMaxStores;
        MaxLoadsPerMemcmp = MaxLoadsPerMemcmpOptSize;
#endif
}

bool SCISATargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const
{
    return false;
}

SCISATargetLowering::ConstraintType SCISATargetLowering::getConstraintType(StringRef Constraint) const
{
    return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass *> SCISATargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI, StringRef Constraint, MVT VT) const
{
    if (Constraint.size() == 1) {
        // GCC Constraint Letters
        switch (Constraint[0]) {
            case 'r': // GENERAL_REGS
                return std::make_pair(0U, &SCISA::GPR32RegClass);
            default:
                break;
        }
    }

    return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

void SCISATargetLowering::ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results, SelectionDAG &DAG) const
{
    SDLoc DL(N);
    uint32_t Opcode = N->getOpcode();
    switch (Opcode) {
        default:
            report_fatal_error("unhandled custom legalization: " + Twine(Opcode));
            break;
        case ISD::READCYCLECOUNTER:
            if (N->getValueType(0) == MVT::i64) {
                SDValue V = DAG.getNode(ISD::READCYCLECOUNTER, SDLoc(N), DAG.getVTList(MVT::i32, MVT::Other), N->getOperand(0));
                SDValue Op = DAG.getNode(ISD::ZERO_EXTEND, SDLoc(N), MVT::i64, V);
                Results.push_back(Op);
                Results.push_back(V.getValue(1));
            }
            break;
    }
    return;
}

SDValue SCISATargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
    switch (Op.getOpcode()) {
        default:
            report_fatal_error("unimplemented opcode: " + Twine(Op.getOpcode()));
        case ISD::BR_CC:
            return LowerBR_CC(Op, DAG);
        case ISD::SETCC:
            return LowerSETCC(Op, DAG);
        case ISD::GlobalAddress:
            return LowerGlobalAddress(Op, DAG);
        case ISD::ConstantPool:
            return LowerConstantPool(Op, DAG);
        case ISD::SELECT_CC:
            return LowerSELECT_CC(Op, DAG);
        case ISD::DYNAMIC_STACKALLOC:
            return LowerDYNAMIC_STACKALLOC(Op, DAG);
        case ISD::RETURNADDR:
            return LowerRETURNADDR(Op, DAG);
        case ISD::FRAMEADDR:
            return LowerFRAMEADDR(Op, DAG);
        case ISD::STACKSAVE:
            return LowerSTACKSAVE(Op, DAG);
        case ISD::STACKRESTORE:
            return LowerSTACKRESTORE(Op, DAG);
        case ISD::READCYCLECOUNTER:
            assert(Op.getSimpleValueType() == MVT::i32);
            return Op;
    }
}

// Calling Convention Implementation
#include "SCISAGenCallingConv.inc"

SDValue SCISATargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const
{
    if (CallConv != CallingConv::C && CallConv != CallingConv::Fast) {
        report_fatal_error("unimplemented calling convention: " + Twine(CallConv));
    }

    MachineFunction &MF = DAG.getMachineFunction();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeFormalArguments(Ins, CC_SCISA32);

    bool HasMemArgs = false;
    for (size_t I = 0; I < ArgLocs.size(); ++I) {
        auto &VA = ArgLocs[I];

        if (VA.isRegLoc()) {
            // Arguments passed in registers
            EVT RegVT = VA.getLocVT();
            MVT::SimpleValueType SimpleTy = RegVT.getSimpleVT().SimpleTy;
            switch (SimpleTy) {
                default: {
                    std::string Str;
                    {
                        raw_string_ostream OS(Str);
                        RegVT.print(OS);
                    }
                    report_fatal_error("unhandled argument type: " + Twine(Str));
                }
                case MVT::i32:
                    Register VReg = RegInfo.createVirtualRegister(&SCISA::GPR32RegClass);
                    RegInfo.addLiveIn(VA.getLocReg(), VReg);
                    SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);

                    // If this is an value that has been promoted to wider types, insert an
                    // assert[sz]ext to capture this, then truncate to the right size.
                    if (VA.getLocInfo() == CCValAssign::SExt) {
                        ArgValue = DAG.getNode(ISD::AssertSext, DL, RegVT, ArgValue, DAG.getValueType(VA.getValVT()));
                    }
                    else if (VA.getLocInfo() == CCValAssign::ZExt) {
                        ArgValue = DAG.getNode(ISD::AssertZext, DL, RegVT, ArgValue, DAG.getValueType(VA.getValVT()));
                    }

                    if (VA.getLocInfo() != CCValAssign::Full) {
                        ArgValue = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), ArgValue);
                    }

                    InVals.push_back(ArgValue);
                    break;
            }
        }
        else {
            if (VA.isMemLoc()) {
                HasMemArgs = true;
            }
            else {
                report_fatal_error("unhandled argument location");
            }
            InVals.push_back(DAG.getConstant(0, DL, VA.getLocVT()));
        }
    }
    if (HasMemArgs) {
        fail(DL, DAG, "stack arguments are not supported");
    }
    if (IsVarArg) {
        fail(DL, DAG, "variadic functions are not supported");
    }
    if (MF.getFunction().hasStructRetAttr()) {
        fail(DL, DAG, "aggregate returns are not supported");
    }

    return Chain;
}

const size_t SCISATargetLowering::MaxArgs = 12;

SDValue SCISATargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const
{
    SelectionDAG &DAG = CLI.DAG;
    auto &Outs = CLI.Outs;
    auto &OutVals = CLI.OutVals;
    auto &Ins = CLI.Ins;
    SDValue Chain = CLI.Chain;
    SDValue Callee = CLI.Callee;
    bool &IsTailCall = CLI.IsTailCall;
    CallingConv::ID CallConv = CLI.CallConv;
    bool IsVarArg = CLI.IsVarArg;
    MachineFunction &MF = DAG.getMachineFunction();

    // SCISA target does not support tail call optimization.
    IsTailCall = false;
    if (CallConv != CallingConv::C && CallConv != CallingConv::Fast) {
        report_fatal_error("unsupported calling convention: " + Twine(CallConv));
    }

    // Analyze operands of the call, assigning locations to each operand.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

    CCInfo.AnalyzeCallOperands(Outs, CC_SCISA32);

    unsigned NumBytes = CCInfo.getStackSize();

    if (Outs.size() > MaxArgs) {
        fail(CLI.DL, DAG, "too many arguments", Callee);
    }

    for (auto &Arg : Outs) {
        ISD::ArgFlagsTy Flags = Arg.Flags;
        if (!Flags.isByVal()) {
            continue;
        }
        fail(CLI.DL, DAG, "pass by value not supported", Callee);
        break;
    }

    auto PtrVT = getPointerTy(MF.getDataLayout());
    Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

    SmallVector<std::pair<unsigned, SDValue>, MaxArgs> RegsToPass;

    // Walk arg assignments
    for (size_t I = 0; I < std::min(ArgLocs.size(), MaxArgs); ++I) {
        CCValAssign &VA = ArgLocs[I];
        SDValue &Arg = OutVals[I];

        // Promote the value if needed.
        switch (VA.getLocInfo()) {
            default:
                report_fatal_error("unhandled location info: " + Twine(VA.getLocInfo()));
            case CCValAssign::Full:
                break;
            case CCValAssign::SExt:
                Arg = DAG.getNode(ISD::SIGN_EXTEND, CLI.DL, VA.getLocVT(), Arg);
                break;
            case CCValAssign::ZExt:
                Arg = DAG.getNode(ISD::ZERO_EXTEND, CLI.DL, VA.getLocVT(), Arg);
                break;
            case CCValAssign::AExt:
                Arg = DAG.getNode(ISD::ANY_EXTEND, CLI.DL, VA.getLocVT(), Arg);
                break;
        }

        // Push arguments into RegsToPass vector
        if (VA.isRegLoc()) {
            RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
        }
        else {
            report_fatal_error("stack arguments are not supported");
        }
    }

    SDValue InGlue;

    // Build a sequence of copy-to-reg nodes chained together with token chain and
    // flag operands which copy the outgoing args into registers.  The InGlue in
    // necessary since all emitted instructions must be stuck together.
    for (auto &Reg : RegsToPass) {
        Chain = DAG.getCopyToReg(Chain, CLI.DL, Reg.first, Reg.second, InGlue);
        InGlue = Chain.getValue(1);
    }

    bool IsDirect = true;
    if (auto *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
        Callee = DAG.getTargetGlobalAddress(G->getGlobal(), CLI.DL, PtrVT, G->getOffset(), 0);
    }
    else if (auto *E = dyn_cast<ExternalSymbolSDNode>(Callee)) {
        Callee = DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT, 0);
        fail(CLI.DL, DAG, Twine("A call to built-in function '" + StringRef(E->getSymbol()) + "' is not supported."));
    }
    else {
        IsDirect = false;
    }

    // Returns a chain & a flag for retval copy to use.
    SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
    SmallVector<SDValue, 8> Ops;
    Ops.push_back(Chain);
    Ops.push_back(Callee);

    // Add argument registers to the end of the list so that they are known live into the call.
    for (auto &Reg : RegsToPass) {
        Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
    }

    const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
    Ops.push_back(DAG.getRegisterMask(TRI->getCallPreservedMask(MF, CLI.CallConv)));
    if (InGlue.getNode()) {
        Ops.push_back(InGlue);
    }

    // Chain = DAG.getNode(SCISAISD::CALL, CLI.DL, NodeTys, Ops);

    Chain = DAG.getNode(IsDirect ? SCISAISD::CALL : SCISAISD::INDIRECT_CALL, CLI.DL, NodeTys, Ops);
    InGlue = Chain.getValue(1);

    DAG.addNoMergeSiteInfo(Chain.getNode(), CLI.NoMerge);

    // Create the CALLSEQ_END node.
    Chain = DAG.getCALLSEQ_END(Chain, NumBytes, 0, InGlue, CLI.DL);
    InGlue = Chain.getValue(1);

    // Handle result values, copying them out of physregs into vregs that we
    // return.
    return LowerCallResult(Chain, InGlue, CallConv, IsVarArg, Ins, CLI.DL, DAG, InVals);
}

SDValue SCISATargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL, SelectionDAG &DAG) const
{
    unsigned Opc = SCISAISD::RET_GLUE;

    // CCValAssign - represent the assignment of the return value to a location
    SmallVector<CCValAssign, 16> RVLocs;
    MachineFunction &MF = DAG.getMachineFunction();

    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());

    if (MF.getFunction().getReturnType()->isAggregateType()) {
        fail(DL, DAG, "aggregate returns are not supported");
        return DAG.getNode(Opc, DL, MVT::Other, Chain);
    }

    // Analize return values.
    CCInfo.AnalyzeReturn(Outs, RetCC_SCISA32);

    SDValue Glue;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    // Copy the result values into the output registers.
    for (size_t I = 0; I != RVLocs.size(); ++I) {
        CCValAssign &VA = RVLocs[I];
        if (!VA.isRegLoc()) {
            report_fatal_error("stack return values are not supported");
        }

        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[I], Glue);

        // Guarantee that all emitted copies are stuck together,
        // avoiding something bad.
        Glue = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain; // Update chain.

    // Add the glue if we have it.
    if (Glue.getNode()) {
        RetOps.push_back(Glue);
    }

    return DAG.getNode(Opc, DL, MVT::Other, RetOps);
}

SDValue SCISATargetLowering::LowerCallResult(SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const
{
#if 1
    // Assign locations to each value returned by this call.
    SmallVector<CCValAssign, 16> RVLocs;
    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs, *DAG.getContext());

    //   AnalyzeReturnValues(CCInfo, RVLocs, Ins);
    CCInfo.AnalyzeCallResult(Ins, RetCC_SCISA32);

    // Copy all of the result registers out of their specified physreg.
    for (unsigned I = 0; I != RVLocs.size(); ++I) {
        Chain = DAG.getCopyFromReg(Chain, DL, RVLocs[I].getLocReg(), RVLocs[I].getValVT(), InGlue).getValue(1);
        InGlue = Chain.getValue(2);
        InVals.push_back(Chain.getValue(0));
    }

    return Chain;
#else
    SmallVector<CCValAssign, 16> RVLocs;
    SmallVector<std::pair<int, unsigned>, 4> ResultMemLocs;
    // Copy results out of physical registers.
    for (unsigned I = 0, E = RVLocs.size(); I != E; ++I) {
        const CCValAssign &VA = RVLocs[I];
        if (VA.isRegLoc()) {
            SDValue RetValue;
            RetValue = DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getValVT(), InGlue);
            Chain = RetValue.getValue(1);
            InGlue = RetValue.getValue(2);
            InVals.push_back(RetValue);
        }
        else {
            assert(VA.isMemLoc() && "Must be memory location.");
            ResultMemLocs.push_back(std::make_pair(VA.getLocMemOffset(), InVals.size()));

            // Reserve space for this result.
            InVals.push_back(SDValue());
        }
    }

    // Copy results out of memory.
    SmallVector<SDValue, 4> MemOpChains;
    for (unsigned I = 0, E = ResultMemLocs.size(); I != E; ++I) {
        int Offset = ResultMemLocs[I].first;
        unsigned Index = ResultMemLocs[I].second;
        SDValue StackPtr = DAG.getRegister(SCISA::SP, MVT::i32);
        SDValue SpLoc = DAG.getNode(ISD::ADD, DL, MVT::i32, StackPtr, DAG.getConstant(Offset, DL, MVT::i32));
        SDValue Load = DAG.getLoad(MVT::i32, DL, Chain, SpLoc, MachinePointerInfo());
        InVals[Index] = Load;
        MemOpChains.push_back(Load.getValue(1));
    }

    if (!MemOpChains.empty()) {
        Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);
    }

    return Chain;
#endif
}

SDValue SCISATargetLowering::LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const
{
    SDValue Chain = Op.getOperand(0); // Legalize the chain.
    SDValue Size = Op.getOperand(1);  // Legalize the size.
    EVT VT = Size->getValueType(0);
    SDLoc DL(Op);

    // Round up Size to 32
    SDValue SizeTmp = DAG.getNode(ISD::ADD, DL, VT, Size, DAG.getConstant(31, DL, MVT::i32));
    SDValue SizeRoundUp = DAG.getNode(ISD::AND, DL, VT, SizeTmp, DAG.getSignedConstant(~31, DL, MVT::i32));

    unsigned SPReg = SCISA::SP;
    SDValue SP = DAG.getCopyFromReg(Chain, DL, SPReg, VT);
    SDValue NewSP = DAG.getNode(ISD::SUB, DL, VT, SP, SizeRoundUp); // Value
    Chain = DAG.getCopyToReg(SP.getValue(1), DL, SPReg, NewSP);     // Output chain

    SDValue NewVal = DAG.getCopyFromReg(Chain, DL, SPReg, MVT::i32);
    Chain = NewVal.getValue(1);

    SDValue Ops[2] = { NewVal, Chain };
    return DAG.getMergeValues(Ops, DL);
}


static SCISACC::CondCode intCCToSCISACC(ISD::CondCode CC)
{
    switch (CC) {
        default:
            llvm_unreachable("Unknown condition code!");
        case ISD::SETEQ:
            return SCISACC::EQ;
        case ISD::SETNE:
            return SCISACC::NE;
        case ISD::SETGE:
            return SCISACC::GE;
        case ISD::SETGT:
            return SCISACC::GT;
        case ISD::SETLE:
            return SCISACC::LE;
        case ISD::SETLT:
            return SCISACC::LT;
        case ISD::SETUGE:
            return SCISACC::HS;
        case ISD::SETUGT:
            return SCISACC::HI;
        case ISD::SETULE:
            return SCISACC::LS;
        case ISD::SETULT:
            return SCISACC::LO;
    }
}

static SDValue getSCISACmp(SDValue &LHS, SDValue &RHS, ISD::CondCode CC, SDValue &SCISAcc, SelectionDAG &DAG, SDLoc DL)
{
    SCISAcc = DAG.getConstant(intCCToSCISACC(CC), DL, LHS.getValueType());
    return DAG.getNode(SCISAISD::CMP, DL, MVT::Glue, LHS, RHS);
}

SDValue SCISATargetLowering::getSCISACmov(const SDLoc &DL, EVT VT, SDValue FalseVal, SDValue TrueVal, SDValue SCISAcc, SDValue Flags, SelectionDAG &DAG) const
{
    return DAG.getNode(SCISAISD::CMOV, DL, VT, FalseVal, TrueVal, SCISAcc, Flags);
}

SDValue SCISATargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const
{
    SDValue Chain = Op.getOperand(0);
    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
    SDValue LHS = Op.getOperand(2);
    SDValue RHS = Op.getOperand(3);
    SDValue Dest = Op.getOperand(4);
    SDLoc DL(Op);

    SDValue SCISAcc;
    SDValue Cmp = getSCISACmp(LHS, RHS, CC, SCISAcc, DAG, DL);
    return DAG.getNode(SCISAISD::BR_CC, DL, Op.getValueType(), Chain, Dest, SCISAcc, Cmp);
}

SDValue SCISATargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const
{
    SDValue LHS = Op.getOperand(0);
    SDValue RHS = Op.getOperand(1);
    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
    SDLoc DL(Op);

    SDValue SCISAcc;
    SDValue Cmp = getSCISACmp(LHS, RHS, CC, SCISAcc, DAG, DL);

    SDValue TrueV = DAG.getConstant(1, DL, Op.getValueType());
    SDValue FalseV = DAG.getConstant(0, DL, Op.getValueType());
    SDValue Ops[] = { TrueV, FalseV, SCISAcc, Cmp };

    return DAG.getNode(SCISAISD::SELECT_CC, DL, Op.getValueType(), Ops);
}

SDValue SCISATargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const
{
    SDValue LHS = Op.getOperand(0);
    SDValue RHS = Op.getOperand(1);
    SDValue TrueV = Op.getOperand(2);
    SDValue FalseV = Op.getOperand(3);
    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
    SDLoc DL(Op);
    SCISACC::CondCode SCISAcc = intCCToSCISACC(CC);

    SDValue Cmp = DAG.getNode(SCISAISD::CMP, DL, MVT::Glue, LHS, RHS);
    return DAG.getNode(SCISAISD::CMOV, DL, TrueV.getValueType(), TrueV, FalseV, DAG.getConstant(SCISAcc, DL, MVT::i32), Cmp);
}

SDValue SCISATargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const
{
    MachineFunction &MF = DAG.getMachineFunction();
    MachineFrameInfo &MFI = MF.getFrameInfo();
    MFI.setReturnAddressIsTaken(true);

    EVT VT = Op.getValueType();
    SDLoc DL(Op);
    unsigned Depth = Op.getConstantOperandVal(0);
    if (Depth) {
        SDValue FrameAddr = LowerFRAMEADDR(Op, DAG);
        SDValue Offset = DAG.getConstant(1, DL, MVT::i32);
        return DAG.getLoad(VT, DL, DAG.getEntryNode(), DAG.getNode(ISD::ADD, DL, VT, FrameAddr, Offset), MachinePointerInfo());
    }

    // Return LR, which contains the return address. Mark it an implicit live-in.
    Register Reg = MF.addLiveIn(SCISA::LR, getRegClassFor(MVT::i32));
    return DAG.getCopyFromReg(DAG.getEntryNode(), DL, Reg, VT);
}

SDValue SCISATargetLowering::LowerSTACKSAVE(SDValue Op, SelectionDAG &DAG) const
{
    return DAG.getCopyFromReg(Op.getOperand(0), SDLoc(Op), SCISA::SP, Op.getValueType());
}

SDValue SCISATargetLowering::LowerSTACKRESTORE(SDValue Op, SelectionDAG &DAG) const
{
    return DAG.getCopyToReg(Op.getOperand(0), SDLoc(Op), SCISA::SP, Op.getOperand(1));
}

SDValue SCISATargetLowering::LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const
{
    if (Op.getConstantOperandVal(0) != 0) {
        return SDValue();
    }

    MachineFunction &MF = DAG.getMachineFunction();
    MachineFrameInfo &MFI = MF.getFrameInfo();
    MFI.setFrameAddressIsTaken(true);
    EVT VT = Op.getValueType();
    SDLoc DL(Op);
    const SCISARegisterInfo &SRI = *static_cast<const SCISARegisterInfo *>(MF.getSubtarget().getRegisterInfo());
    Register FrameRegister = SRI.getFrameRegister(MF);
    SDValue FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), DL, FrameRegister, VT);
    return FrameAddr;
}

const char *SCISATargetLowering::getTargetNodeName(unsigned Opcode) const
{
    switch ((SCISAISD::NodeType)Opcode) {
        case SCISAISD::FIRST_NUMBER:
            break;
        case SCISAISD::RET_GLUE:
            return "SCISAISD::RET_GLUE";
        case SCISAISD::CALL:
            return "SCISAISD::CALL";
        case SCISAISD::INDIRECT_CALL:
            return "SCISAISD::INDIRECT_CALL";
        case SCISAISD::SELECT_CC:
            return "SCISAISD::SELECT_CC";
        case SCISAISD::BR_CC:
            return "SCISAISD::BR_CC";
        case SCISAISD::Wrapper:
            return "SCISAISD::Wrapper";
        case SCISAISD::MEMCPY:
            return "SCISAISD::MEMCPY";
        case SCISAISD::CMP:
            return "SCISAISD::CMP";
        case SCISAISD::CMOV:
            return "SCISAISD::CMOV";
    }
    return nullptr;
}

static SDValue getTargetNode(ConstantPoolSDNode *N, const SDLoc &DL, EVT Ty, SelectionDAG &DAG, unsigned Flags)
{
    return DAG.getTargetConstantPool(N->getConstVal(), Ty, N->getAlign(), N->getOffset(), Flags);
}

template <class NodeTy>
SDValue SCISATargetLowering::getAddr(NodeTy *N, SelectionDAG &DAG, unsigned Flags) const
{
    SDLoc DL(N);
    SDValue GA = getTargetNode(N, DL, MVT::i32, DAG, Flags);
    return DAG.getNode(SCISAISD::Wrapper, DL, MVT::i32, GA);
}

SDValue SCISATargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
    const GlobalAddressSDNode *GN = cast<GlobalAddressSDNode>(Op);
    const GlobalValue *GV = GN->getGlobal();
    SDLoc DL(GN);
    int64_t Offset = GN->getOffset();
    SDValue GA = DAG.getTargetGlobalAddress(GV, DL, MVT::i32, Offset);
    return DAG.getNode(SCISAISD::Wrapper, DL, MVT::i32, GA);
}

SDValue SCISATargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const
{
    ConstantPoolSDNode *N = cast<ConstantPoolSDNode>(Op);
    return getAddr(N, DAG);
}

MachineBasicBlock *SCISATargetLowering::emitInstrWithCustomInserterMemcpy(MachineInstr &MI, MachineBasicBlock *BB) const
{
    MachineFunction *MF = MI.getParent()->getParent();
    MachineRegisterInfo &MRI = MF->getRegInfo();
    MachineInstrBuilder MIB(*MF, MI);
    Register ScratchReg = MRI.createVirtualRegister(&SCISA::GPR32RegClass);
    MIB.addReg(ScratchReg, RegState::Define | RegState::Dead | RegState::EarlyClobber);
    return BB;
}

MachineBasicBlock *SCISATargetLowering::emitInstrWithCustomInserterSelect(MachineInstr &MI, MachineBasicBlock *BB) const
{
    const TargetInstrInfo &TII = *BB->getParent()->getSubtarget().getInstrInfo();
    DebugLoc DL = MI.getDebugLoc();

    // To "insert" a SELECT instruction, we actually have to insert the diamond
    // control-flow pattern.  The incoming instruction knows the destination vreg
    // to set, the condition code register to branch on, the true/false values to
    // select between, and a branch opcode to use.
    const BasicBlock *LlvmBB = BB->getBasicBlock();
    MachineFunction::iterator I = ++BB->getIterator();

    //  thisMBB:
    //  ...
    //   TrueVal = ...
    //   cmpTY ccX, r1, r2
    //   jCC copy1MBB
    //   fallthrough --> copy0MBB
    MachineBasicBlock *ThisMBB = BB;
    MachineFunction *F = BB->getParent();
    MachineBasicBlock *Copy0MBB = F->CreateMachineBasicBlock(LlvmBB);
    MachineBasicBlock *Copy1MBB = F->CreateMachineBasicBlock(LlvmBB);
    F->insert(I, Copy0MBB);
    F->insert(I, Copy1MBB);

    // Update machine-CFG edges by transferring all successors of the current
    // block to the new block which will contain the Phi node for the select.
    Copy1MBB->splice(Copy1MBB->begin(), BB, std::next(MachineBasicBlock::iterator(MI)), BB->end());
    Copy1MBB->transferSuccessorsAndUpdatePHIs(BB);
    // Next, add the true and fallthrough blocks as its successors.
    BB->addSuccessor(Copy0MBB);
    BB->addSuccessor(Copy1MBB);

    BuildMI(BB, DL, TII.get(SCISA::BCC))
        .addMBB(Copy1MBB)
        .addImm(MI.getOperand(3).getImm());

    //  copy0MBB:
    //   %FalseValue = ...
    //   # fallthrough to copy1MBB
    BB = Copy0MBB;

    // Update machine-CFG edges
    BB->addSuccessor(Copy1MBB);

    //  copy1MBB:
    //   %Result = phi [ %FalseValue, Copy0MBB ], [ %TrueValue, ThisMBB ]
    //  ...
    BB = Copy1MBB;
    BuildMI(*BB, BB->begin(), DL, TII.get(SCISA::PHI), MI.getOperand(0).getReg())
        .addReg(MI.getOperand(2).getReg())
        .addMBB(Copy0MBB)
        .addReg(MI.getOperand(1).getReg())
        .addMBB(ThisMBB);

    MI.eraseFromParent(); // The pseudo instruction is gone now.
    return BB;
}

MachineBasicBlock *SCISATargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI, MachineBasicBlock *BB) const
{
    DebugLoc DL = MI.getDebugLoc();
    unsigned Opc = MI.getOpcode();

    if (Opc == SCISA::MEMCPY) {
        return emitInstrWithCustomInserterMemcpy(MI, BB);
    }
    if (Opc == SCISA::Select_32) {
        return emitInstrWithCustomInserterSelect(MI, BB);
    }
    report_fatal_error("unhandled instruction type: " + Twine(Opc));
}


bool SCISATargetLowering::isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, Type *Ty, unsigned AS, Instruction *I) const
{
    if (AM.BaseGV) {
        if (!AM.HasBaseReg && AM.Scale == 0 && AM.BaseOffs == 0) {
            return true;
        }
        return false;
    }

    switch (AM.Scale) {
        case 0: // "r+i" or just "i", depending on HasBaseReg.
            break;
        case 1:
            if (!AM.HasBaseReg) { // allow "r+i".
                break;
            }
            return false; // disallow "r+r" or "r+r+i".
        default:
            return false;
    }

    return true;
}
