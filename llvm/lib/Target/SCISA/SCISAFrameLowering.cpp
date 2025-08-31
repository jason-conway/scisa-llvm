//===-- SCISAFrameLowering.cpp - SCISA Frame Information ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SCISA implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "SCISAFrameLowering.h"
#include "MCTargetDesc/SCISAMCTargetDesc.h"
#include "SCISAInstrInfo.h"
#include "SCISASubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

bool SCISAFrameLowering::hasFPImpl(const MachineFunction &MF) const
{
    return true;
}


// Determines the size of the frame and maximum call frame size.
void SCISAFrameLowering::determineFrameLayout(MachineFunction &MF) const
{
    MachineFrameInfo &MFI = MF.getFrameInfo();
    const SCISARegisterInfo &SRI = *static_cast<const SCISARegisterInfo *>(MF.getSubtarget().getRegisterInfo());

    // Get the number of bytes to allocate from the FrameInfo.
    unsigned FrameSize = MFI.getStackSize();

    // Get the alignment.
    Align StackAlign = SRI.hasStackRealignment(MF) ? MFI.getMaxAlign() : getStackAlign();

    // Get the maximum call frame size of all the calls.
    unsigned MaxCallFrameSize = MFI.getMaxCallFrameSize();

    // If we have dynamic alloca then MaxCallFrameSize needs to be aligned so
    // that allocations will be aligned.
    if (MFI.hasVarSizedObjects()) {
        MaxCallFrameSize = alignTo(MaxCallFrameSize, StackAlign);
    }

    // Update maximum call frame size.
    MFI.setMaxCallFrameSize(MaxCallFrameSize);

    // Include call frame size in total.
    if (!(hasReservedCallFrame(MF) && MFI.adjustsStack())) {
        FrameSize += MaxCallFrameSize;
    }

    // Make sure the frame is aligned.
    FrameSize = alignTo(FrameSize, StackAlign);

    // Update frame info.
    MFI.setStackSize(FrameSize);
}


void SCISAFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
#if 0
    MachineFrameInfo &MFI = MF.getFrameInfo();

    const SCISAInstrInfo &SII = *static_cast<const SCISAInstrInfo *>(MF.getSubtarget().getInstrInfo());

    MachineBasicBlock::iterator MBBI = MBB.begin();

    // Debug location must be unknown since the first debug location is used
    // to determine the end of the prologue.
    DebugLoc DL;

    // Determine the correct frame layout
    determineFrameLayout(MF);

    // FIXME: This appears to be overallocating.  Needs investigation.
    // Get the number of bytes to allocate from the FrameInfo.
    unsigned StackSize = MFI.getStackSize();

    // Push old FP
    // st %fp,-4[*%sp]
    BuildMI(MBB, MBBI, DL, SII.get(SCISA::PUSH))
        .addReg(SCISA::FP)
        .setMIFlag(MachineInstr::FrameSetup);

    // Generate new FP
    // add %sp,8,%fp
    
    // mov sp, fp
    BuildMI(MBB, MBBI, DL, SII.get(SCISA::MOV_rr_32), SCISA::SP)
        .addReg(SCISA::FP)
        .setMIFlag(MachineInstr::FrameSetup);
    // add sp, 8
    BuildMI(MBB, MBBI, DL, SII.get(SCISA::ADD_ri_32), SCISA::SP)
        .addReg(SCISA::SP)
        .addImm(8)
        .setMIFlag(MachineInstr::FrameSetup);

    // Allocate space on the stack if needed
    // sub %sp,StackSize,%sp
    if (StackSize != 0) {
        BuildMI(MBB, MBBI, DL, SII.get(SCISA::SUB_ri_32), SCISA::SP)
            .addReg(SCISA::SP)
            .addImm(StackSize)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    // Replace ADJDYNANALLOC
    if (MFI.hasVarSizedObjects()) {
        // replaceAdjDynAllocPseudo(MF);
    }
#endif
}

void SCISAFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
#if 0
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const SCISAInstrInfo &SII = *static_cast<const SCISAInstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc DL = MBBI->getDebugLoc();

  // Restore the stack pointer using the callee's frame pointer value.
  BuildMI(MBB, MBBI, DL, SII.get(SCISA::ADD_rr_32), SCISA::SP)
      .addReg(SCISA::FP);

  // Restore the frame pointer from the stack.
  BuildMI(MBB, MBBI, DL, SII.get(SCISA::POP), SCISA::FP);
#endif
}

void SCISAFrameLowering::determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs, RegScavenger *RS) const
{
    TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
    // SavedRegs.reset(SCISA::R16);
    // SavedRegs.reset(SCISA::R17);
    // SavedRegs.reset(SCISA::R18);
    // SavedRegs.reset(SCISA::R19);
    // SavedRegs.reset(SCISA::R20);
    // SavedRegs.reset(SCISA::R21);
    // SavedRegs.reset(SCISA::R22);
    // SavedRegs.reset(SCISA::R23);
}
