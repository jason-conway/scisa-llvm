//===-- SCISAMCTargetDesc.cpp - SCISA Target Descriptions ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides SCISA specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/SCISAMCTargetDesc.h"
#include "MCTargetDesc/SCISAInstPrinter.h"
#include "MCTargetDesc/SCISAMCAsmInfo.h"
#include "TargetInfo/SCISATargetInfo.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "SCISAGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "SCISAGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "SCISAGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createSCISAMCInstrInfo()
{
    MCInstrInfo *X = new MCInstrInfo();
    InitSCISAMCInstrInfo(X);
    return X;
}

static MCRegisterInfo *createSCISAMCRegisterInfo(const Triple &TT)
{
    MCRegisterInfo *X = new MCRegisterInfo();
    InitSCISAMCRegisterInfo(X, SCISA::LR);
    return X;
}

static MCSubtargetInfo *createSCISAMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS)
{
    return createSCISAMCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCStreamer *createSCISAMCStreamer(const Triple &T, MCContext &Ctx, std::unique_ptr<MCAsmBackend> &&MAB, std::unique_ptr<MCObjectWriter> &&OW, std::unique_ptr<MCCodeEmitter> &&Emitter)
{
    return createELFStreamer(Ctx, std::move(MAB), std::move(OW), std::move(Emitter));
}

static MCInstPrinter *createSCISAMCInstPrinter(const Triple &T, unsigned SyntaxVariant, const MCAsmInfo &MAI, const MCInstrInfo &MII, const MCRegisterInfo &MRI)
{
    if (SyntaxVariant == 0) {
        return new SCISAInstPrinter(MAI, MII, MRI);
    }
    return nullptr;
}

namespace {

class SCISAMCInstrAnalysis : public MCInstrAnalysis {
public:
    explicit SCISAMCInstrAnalysis(const MCInstrInfo *Info)
        : MCInstrAnalysis(Info)
    {
    }

    bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size, uint64_t &Target) const override
    {
        // The target is the 3rd operand of cond inst and the 1st of uncond inst.
        int32_t Imm;
        if (isConditionalBranch(Inst)) {
            Imm = (short)Inst.getOperand(2).getImm();
        }
        else if (isUnconditionalBranch(Inst)) {
            if (Inst.getOpcode() == SCISA::B) {
                Imm = (short)Inst.getOperand(0).getImm();
            }
            else {
                Imm = (int)Inst.getOperand(0).getImm();
            }
        }
        else {
            return false;
        }

        Target = Addr + Size + Imm * Size;
        return true;
    }
};

} // end anonymous namespace

static MCInstrAnalysis *createSCISAInstrAnalysis(const MCInstrInfo *Info)
{
    return new SCISAMCInstrAnalysis(Info);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSCISATargetMC()
{
    Target *T = &getTheSCISATarget();

    // Register the MC asm info.
    RegisterMCAsmInfo<SCISAMCAsmInfo> X(*T);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createSCISAMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createSCISAMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createSCISAMCSubtargetInfo);

    // Register the object streamer
    TargetRegistry::RegisterELFStreamer(*T, createSCISAMCStreamer);

    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T, createSCISAMCInstPrinter);

    // Register the MC instruction analyzer.
    TargetRegistry::RegisterMCInstrAnalysis(*T, createSCISAInstrAnalysis);
}
