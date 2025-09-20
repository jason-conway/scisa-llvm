//===-- SCISAInstrInfo.cpp - SCISA Instruction Information ------*- C++ -*-===//
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

#include "SCISAInstrInfo.h"
#include "SCISA.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <iterator>

#define GET_INSTRINFO_CTOR_DTOR
#include "SCISAGenInstrInfo.inc"

using namespace llvm;

SCISAInstrInfo::SCISAInstrInfo()
    : SCISAGenInstrInfo(SCISA::ADJCALLSTACKDOWN, SCISA::ADJCALLSTACKUP)
{
}

void SCISAInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, const DebugLoc &DL, Register DestReg, Register SrcReg, bool KillSrc, bool RenamableDest, bool RenamableSrc) const
{
    assert(SCISA::GPR32RegClass.contains(DestReg, SrcReg) && "Invalid register copy");

    BuildMI(MBB, I, DL, get(SCISA::MOV_rr_32), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
}

void SCISAInstrInfo::expandMEMCPY(MachineBasicBlock::iterator MI) const
{
    Register DstReg = MI->getOperand(0).getReg();
    Register SrcReg = MI->getOperand(1).getReg();
    uint64_t Len = MI->getOperand(2).getImm();
    uint64_t Align = MI->getOperand(3).getImm();

    Register ScratchReg = MI->getOperand(4).getReg();

    MachineBasicBlock *BB = MI->getParent();

    DebugLoc DL = MI->getDebugLoc();

    unsigned LoadOp = 0;
    unsigned StoreOp = 0;

    switch (Align) {
        case 1:
            LoadOp = SCISA::LDUB32;
            StoreOp = SCISA::STB32;
            break;
        case 2:
            LoadOp = SCISA::LDUH32;
            StoreOp = SCISA::STH32;
            break;
        case 4:
            LoadOp = SCISA::LDW32;
            StoreOp = SCISA::STW32;
            break;
        default:
            llvm_unreachable("unsupported memcpy alignment");
    }

    unsigned TotalItr = Len >> Log2_64(Align);

    for (unsigned I = 0; I < TotalItr; ++I) {
        BuildMI(*BB, MI, DL, get(LoadOp))
            .addReg(ScratchReg, RegState::Define)
            .addReg(SrcReg)
            .addImm(I * Align);
        BuildMI(*BB, MI, DL, get(StoreOp))
            .addReg(ScratchReg, RegState::Kill)
            .addReg(DstReg)
            .addImm(I * Align);
    }

    unsigned Rem = Len & (Align - 1);
    unsigned Offset = TotalItr * Align;

    bool HaveHanging2 = Rem & 0x2;
    bool HaveHanging1 = Rem & 0x1;

    if (HaveHanging2) {
        BuildMI(*BB, MI, DL, get(SCISA::LDUH32))
            .addReg(ScratchReg, RegState::Define)
            .addReg(SrcReg)
            .addImm(Offset);
        BuildMI(*BB, MI, DL, get(SCISA::STH32))
            .addReg(ScratchReg, RegState::Kill)
            .addReg(DstReg)
            .addImm(Offset);
        Offset += 2;
    }

    if (HaveHanging1) {
        BuildMI(*BB, MI, DL, get(SCISA::LDUB32))
            .addReg(ScratchReg, RegState::Define)
            .addReg(SrcReg)
            .addImm(Offset);
        BuildMI(*BB, MI, DL, get(SCISA::STB32))
            .addReg(ScratchReg, RegState::Kill)
            .addReg(DstReg)
            .addImm(Offset);
    }

    BB->erase(MI);
}

bool SCISAInstrInfo::expandPostRAPseudo(MachineInstr &MI) const
{
    if (MI.getOpcode() == SCISA::MEMCPY) {
        expandMEMCPY(MI);
        return true;
    }

    return false;
}

void SCISAInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register SrcReg, bool IsKill, int FI, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI, Register VReg, MachineInstr::MIFlag Flags) const
{
    DebugLoc DL;
    if (I != MBB.end()) {
        DL = I->getDebugLoc();
    }

    assert(RC == &SCISA::GPR32RegClass && "Can't store this register to stack slot");

    BuildMI(MBB, I, DL, get(SCISA::PUSH))
        .addReg(SrcReg, getKillRegState(IsKill));
}

void SCISAInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register DestReg, int FI, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI, Register VReg, MachineInstr::MIFlag Flags) const
{
    DebugLoc DL;
    if (I != MBB.end()) {
        DL = I->getDebugLoc();
    }

    assert(RC == &SCISA::GPR32RegClass && "Can't load this register from stack slot");
    BuildMI(MBB, I, DL, get(SCISA::POP))
        .addReg(DestReg, getDefRegState(true));
}


bool SCISAInstrInfo::analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB, SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const
{
    // Start from the bottom of the block and work up, examining the
    // terminator instructions.
    MachineBasicBlock::iterator I = MBB.end();
    while (I != MBB.begin()) {
        --I;
        if (I->isDebugInstr()) {
            continue;
        }

        // Working from the bottom, when we see a non-terminator
        // instruction, we're done.
        if (!isUnpredicatedTerminator(*I)) {
            break;
        }

        // A terminator that isn't a branch can't easily be handled
        // by this analysis.
        if (!I->isBranch()) {
            return true;
        }

        // Handle unconditional branches.
        if (I->getOpcode() == SCISA::B) {
            if (!AllowModify) {
                TBB = I->getOperand(0).getMBB();
                continue;
            }

            // If the block has any instructions after a branch, delete them.
            MBB.erase(std::next(I), MBB.end());
            Cond.clear();
            FBB = nullptr;

            // Delete the branch if it's equivalent to a fall-through.
            if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
                TBB = nullptr;
                I->eraseFromParent();
                I = MBB.end();
                continue;
            }

            // TBB is used to indicate the unconditinal destination.
            TBB = I->getOperand(0).getMBB();
            continue;
        }

        // Handle conditional branches.
        assert(I->getOpcode() == SCISA::BCC && "Invalid conditional branch");
        SCISACC::CondCode BranchCode = static_cast<SCISACC::CondCode>(I->getOperand(1).getImm());
        if (BranchCode == SCISACC::INVALID) {
            return true; // Can't handle weird stuff.
        }

        // Working from the bottom, handle the first conditional branch.
        if (Cond.empty()) {
            FBB = TBB;
            TBB = I->getOperand(0).getMBB();
            Cond.push_back(MachineOperand::CreateImm(BranchCode));
            continue;
        }

        // Handle subsequent conditional branches. Only handle the case where all
        // conditional branches branch to the same destination.
        assert(Cond.size() == 1);
        assert(TBB);

        // Only handle the case where all conditional branches branch to
        // the same destination.
        if (TBB != I->getOperand(0).getMBB()) {
            return true;
        }

        SCISACC::CondCode Prev = (SCISACC::CondCode)Cond[0].getImm();
        // If the conditions are the same, we can leave them alone.
        if (Prev == BranchCode) {
            continue;
        }

        return true;
    }

    return false;
}

unsigned SCISAInstrInfo::insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const
{
    assert(!BytesAdded && "code size not handled");

    // Shouldn't be a fall through.
    assert(TBB && "insertBranch must not be told to insert a fallthrough");


    // Unconditional branch
    if (Cond.empty()) {
        assert(!FBB && "Unconditional branch with multiple successors!");
        BuildMI(&MBB, DL, get(SCISA::B)).addMBB(TBB);
        return 1;
    }

    // Conditional branch.
    unsigned Count = 0;
    BuildMI(&MBB, DL, get(SCISA::BCC)).addMBB(TBB).addImm(Cond[0].getImm());
    ++Count;

    if (FBB) {
        // Two-way Conditional branch. Insert the second branch.
        BuildMI(&MBB, DL, get(SCISA::B)).addMBB(FBB);
        ++Count;
    }

    return Count;
}

unsigned SCISAInstrInfo::removeBranch(MachineBasicBlock &MBB, int *BytesRemoved) const
{
    assert(!BytesRemoved && "code size not handled");

    MachineBasicBlock::iterator I = MBB.end();
    unsigned Count = 0;

    while (I != MBB.begin()) {
        --I;
        if (I->isDebugInstr()) {
            continue;
        }
        bool Done = I->getOpcode() != SCISA::B && I->getOpcode() != SCISA::BCC && I->getOpcode() != SCISA::BR;
        if (Done) {
            break;
        }
        // Remove the branch.
        I->eraseFromParent();
        I = MBB.end();
        ++Count;
    }

    return Count;
}
