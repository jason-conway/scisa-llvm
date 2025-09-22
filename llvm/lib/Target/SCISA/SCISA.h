//===-- SCISA.h - Top-level interface for SCISA representation ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISA_H
#define LLVM_LIB_TARGET_SCISA_SCISA_H

#include "MCTargetDesc/SCISAMCTargetDesc.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Target/TargetMachine.h"

namespace SCISACC {

enum CondCode {
    INVALID,
    NE = 1u << 0,
    EQ = 1u << 1,
    GE = 1u << 2, // signed GE
    GT = 1u << 3, // signed GT
    LE = 1u << 4, // signed LE
    LT = 1u << 5, // signed LT
    HS = 1u << 6, // unsigned GE
    HI = 1u << 7, // unsigned GT
    LS = 1u << 8, // unsigned LE
    LO = 1u << 9, // unsigned LT
};

} // namespace SCISACC

namespace llvm {

class SCISARegisterBankInfo;
class SCISASubtarget;
class SCISATargetMachine;
class InstructionSelector;
class PassRegistry;

FunctionPass *createSCISAISelDag(SCISATargetMachine &TM);
FunctionPass *createSCISAMIPreEmitPeepholePass();

InstructionSelector *createSCISAInstructionSelector(const SCISATargetMachine &, const SCISASubtarget &, const SCISARegisterBankInfo &);

void initializeSCISADAGToDAGISelLegacyPass(PassRegistry &);
void initializeSCISAMIPreEmitPeepholePass(PassRegistry &);

class SCISAIRPeepholePass : public PassInfoMixin<SCISAIRPeepholePass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

    static bool isRequired()
    {
        return true;
    }
};

} // namespace llvm

#endif
