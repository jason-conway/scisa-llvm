//===------------ SCISAMIPeephole.cpp - MI Peephole Cleanups  -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass performs peephole optimizations to cleanup ugly code sequences at
// MachineInstruction layer.
//
// Currently, there are two optimizations implemented:
//  - One pre-RA MachineSSA pass to eliminate type promotion sequences, those
//    zero extend 32-bit subregisters to 64-bit registers, if the compiler
//    could prove the subregisters is defined by 32-bit operations in which
//    case the upper half of the underlying 64-bit registers were zeroed
//    implicitly.
//
//  - One post-RA PreEmit pass to do final cleanup on some redundant
//    instructions generated due to bad RA on subregister.
//===----------------------------------------------------------------------===//

#include "SCISA.h"
#include "SCISAInstrInfo.h"
#include "SCISATargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include <set>

#define DEBUG_TYPE "scisa-mipeephole"

using namespace llvm;

namespace {

struct SCISAMIPreEmitPeephole : public MachineFunctionPass {

    static char ID;
    MachineFunction *MF;
    const TargetRegisterInfo *TRI;
    const SCISAInstrInfo *TII;

    SCISAMIPreEmitPeephole()
        : MachineFunctionPass(ID)
    {
        initializeSCISAMIPreEmitPeepholePass(*PassRegistry::getPassRegistry());
    }

private:
    // Initialize class variables.
    void initialize(MachineFunction &MFParm);

    bool eliminateRedundantMov();
    // bool insertMissingCallerSavedSpills();

public:
    // Main entry point for this pass.
    bool runOnMachineFunction(MachineFunction &MF) override
    {
        if (skipFunction(MF.getFunction())) {
            return false;
        }

        initialize(MF);

        return eliminateRedundantMov();
    }
};

// Initialize class variables.
void SCISAMIPreEmitPeephole::initialize(MachineFunction &MFParm)
{
    MF = &MFParm;
    TII = MF->getSubtarget<SCISASubtarget>().getInstrInfo();
    TRI = MF->getSubtarget<SCISASubtarget>().getRegisterInfo();
    LLVM_DEBUG(dbgs() << "*** SCISA PreEmit peephole pass ***\n\n");
}

bool SCISAMIPreEmitPeephole::eliminateRedundantMov()
{
    MachineInstr *ToErase = nullptr;
    bool Eliminated = false;

    for (MachineBasicBlock &MBB : *MF) {
        for (MachineInstr &MI : MBB) {
            if (ToErase) {
                LLVM_DEBUG(dbgs() << "  Redundant Mov Eliminated:");
                LLVM_DEBUG(ToErase->dump());
                ToErase->eraseFromParent();
                ToErase = nullptr;
            }

            unsigned Opcode = MI.getOpcode();
            if (Opcode == SCISA::MOV_rr_32) {
                Register dst = MI.getOperand(0).getReg();
                Register src = MI.getOperand(1).getReg();

                if (dst != src) {
                    continue;
                }

                ToErase = &MI;
                Eliminated = true;
            }
        }
    }

    return Eliminated;
}

} // namespace

INITIALIZE_PASS(SCISAMIPreEmitPeephole, "scisa-mi-pemit-peephole", "SCISA PreEmit Peephole Optimization", false, false)

char SCISAMIPreEmitPeephole::ID = 0;
FunctionPass *llvm::createSCISAMIPreEmitPeepholePass()
{
    return new SCISAMIPreEmitPeephole();
}
