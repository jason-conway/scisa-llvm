//===-- SCISARegisterInfo.cpp - SCISA Register Information ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SCISA implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "SCISARegisterInfo.h"
#include "SCISASubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "SCISAGenRegisterInfo.inc"
using namespace llvm;

SCISARegisterInfo::SCISARegisterInfo()
    : SCISAGenRegisterInfo(SCISA::LR)
{
}

const MCPhysReg *SCISARegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
    return CSR_SaveList;
}

const uint32_t *SCISARegisterInfo::getCallPreservedMask(const MachineFunction &MF, CallingConv::ID CC) const
{
    return CSR_RegMask;
}

BitVector SCISARegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
    BitVector Reserved(getNumRegs());
    markSuperRegs(Reserved, SCISA::FP);
    markSuperRegs(Reserved, SCISA::SP);
    markSuperRegs(Reserved, SCISA::LR);
    markSuperRegs(Reserved, SCISA::CC);
    markSuperRegs(Reserved, SCISA::CYC);
    return Reserved;
}

const TargetRegisterClass *SCISARegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const
{
    return &SCISA::GPR32RegClass;
}

bool SCISARegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const
{
    assert(SPAdj == 0 && "Unexpected");

    unsigned I = 0;
    MachineInstr &MI = *II;
    MachineBasicBlock &MBB = *MI.getParent();
    MachineFunction &MF = *MBB.getParent();
    DebugLoc DL = MI.getDebugLoc();

    while (!MI.getOperand(I).isFI()) {
        ++I;
        assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
    }

    Register FrameReg = getFrameRegister(MF);
    int FrameIndex = MI.getOperand(I).getIndex();
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

    if (MI.getOpcode() == SCISA::MOV_rr_32) {
        int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);

        MI.getOperand(I).ChangeToRegister(FrameReg, false);
        Register Reg = MI.getOperand(I - 1).getReg();
        BuildMI(MBB, ++II, DL, TII.get(SCISA::ADD_ri_32), Reg)
            .addReg(Reg)
            .addImm(Offset);
        return false;
    }

    int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex) + MI.getOperand(I + 1).getImm();

    if (!isInt<32>(Offset)) {
        llvm_unreachable("bug in frame offset");
    }

    if (MI.getOpcode() == SCISA::FI_ri32) {
        // architecture does not really support FI_ri, replace it with
        //    MOV_rr <target_reg>, frame_reg
        //    ADD_ri <target_reg>, imm
        Register Reg = MI.getOperand(I - 1).getReg();

        BuildMI(MBB, ++II, DL, TII.get(SCISA::MOV_rr_32), Reg)
            .addReg(FrameReg);
        BuildMI(MBB, II, DL, TII.get(SCISA::ADD_ri_32), Reg)
            .addReg(Reg)
            .addImm(Offset);

        // Remove FI_ri instruction
        MI.eraseFromParent();
    }
    else {
        MI.getOperand(I).ChangeToRegister(FrameReg, false);
        MI.getOperand(I + 1).ChangeToImmediate(Offset);
    }
    return false;
}

Register SCISARegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
    return SCISA::SP;
}
