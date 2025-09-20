//===-- SCISAInstrInfo.h - SCISA Instruction Information --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SCISA implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISAINSTRINFO_H
#define LLVM_LIB_TARGET_SCISA_SCISAINSTRINFO_H

#include "SCISARegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "SCISAGenInstrInfo.inc"

namespace llvm {

class SCISAInstrInfo : public SCISAGenInstrInfo {
    const SCISARegisterInfo RI;

public:
    SCISAInstrInfo();

    const SCISARegisterInfo &getRegisterInfo() const
    {
        return RI;
    }

    void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, const DebugLoc &DL, Register DestReg, Register SrcReg, bool KillSrc, bool RenamableDest = false, bool RenamableSrc = false) const override;

    bool expandPostRAPseudo(MachineInstr &MI) const override;

    void storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register SrcReg, bool isKill, int FrameIndex, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI, Register VReg, MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

    void loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register DestReg, int FrameIndex, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI, Register VReg, MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;
    bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB, SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const override;

    unsigned removeBranch(MachineBasicBlock &MBB, int *BytesRemoved = nullptr) const override;
    unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded = nullptr) const override;

private:
    void expandMEMCPY(MachineBasicBlock::iterator) const;
};

} // namespace llvm

#endif
