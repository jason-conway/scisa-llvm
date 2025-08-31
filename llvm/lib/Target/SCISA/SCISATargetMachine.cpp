//===-- SCISATargetMachine.cpp - Define TargetMachine for SCISA ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about SCISA target spec.
//
//===----------------------------------------------------------------------===//

#include "SCISATargetMachine.h"
#include "MCTargetDesc/SCISAMCAsmInfo.h"
#include "SCISA.h"
#include "SCISATargetTransformInfo.h"
#include "TargetInfo/SCISATargetInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils/SimplifyCFGOptions.h"
#include <optional>
using namespace llvm;

static cl::opt<bool> DisableMIPeephole("disable-scisa-peephole", cl::Hidden, cl::desc("Disable machine peepholes for SCISA"));

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSCISATarget()
{
    // Register the target.
    RegisterTargetMachine<SCISATargetMachine> X(getTheSCISATarget());

    PassRegistry &PR = *PassRegistry::getPassRegistry();
    initializeGlobalISel(PR);
    initializeSCISADAGToDAGISelLegacyPass(PR);
}

static std::string computeDataLayout(const Triple &TT)
{
    return "e-m:e-p:32:32-i32:32-a:0:32-n32-S32";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM)
{
    return RM.value_or(Reloc::PIC_);
}

SCISATargetMachine::SCISATargetMachine(const Target &T, const Triple &TT, StringRef CPU, StringRef FS, const TargetOptions &Options, std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM, CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, computeDataLayout(TT), TT, CPU, FS, Options, getEffectiveRelocModel(RM), getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this)
{
    initAsmInfo();
}

namespace {

// SCISA Code Generator Pass Configuration Options.
class SCISAPassConfig : public TargetPassConfig {
public:
    SCISAPassConfig(SCISATargetMachine &TM, PassManagerBase &PM)
        : TargetPassConfig(TM, PM)
    {
    }

    SCISATargetMachine &getSCISATargetMachine() const
    {
        return getTM<SCISATargetMachine>();
    }

    void addIRPasses() override;
    bool addInstSelector() override;
    void addMachineSSAOptimization() override;
    void addPreEmitPass() override;
    bool addIRTranslator() override;
    bool addLegalizeMachineIR() override;
    bool addRegBankSelect() override;
    bool addGlobalInstructionSelect() override;
};

} // namespace

TargetPassConfig *SCISATargetMachine::createPassConfig(PassManagerBase &PM)
{
    return new SCISAPassConfig(*this, PM);
}

void SCISATargetMachine::registerPassBuilderCallbacks(PassBuilder &PB)
{
#define GET_PASS_REGISTRY "SCISAPassRegistry.def"
#include "llvm/Passes/TargetPassRegistry.inc"

    PB.registerPipelineStartEPCallback(
        [=](ModulePassManager &MPM, OptimizationLevel) {
            FunctionPassManager FPM;
            MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
        });
    PB.registerPeepholeEPCallback([=](FunctionPassManager &FPM, OptimizationLevel Level) {
        FPM.addPass(SimplifyCFGPass(SimplifyCFGOptions().hoistCommonInsts(true)));
        FPM.addPass(SCISAASpaceCastSimplifyPass());
    });
    PB.registerScalarOptimizerLateEPCallback(
        [=](FunctionPassManager &FPM, OptimizationLevel Level) {
            // Run this after loop unrolling but before
            // SimplifyCFGPass(... .sinkCommonInsts(true))
        });
    PB.registerPipelineEarlySimplificationEPCallback(
        [=](ModulePassManager &MPM, OptimizationLevel, ThinOrFullLTOPhase) {
        });
}

void SCISAPassConfig::addIRPasses()
{
    TargetPassConfig::addIRPasses();
}

TargetTransformInfo SCISATargetMachine::getTargetTransformInfo(const Function &F) const
{
    return TargetTransformInfo(SCISATTIImpl(this, F));
}

// Install an instruction selector pass using
// the ISelDag to gen SCISA code.
bool SCISAPassConfig::addInstSelector()
{
    addPass(createSCISAISelDag(getSCISATargetMachine()));
    return false;
}

void SCISAPassConfig::addMachineSSAOptimization()
{
    TargetPassConfig::addMachineSSAOptimization();
}

void SCISAPassConfig::addPreEmitPass()
{
    if (getOptLevel() != CodeGenOptLevel::None) {
        if (!DisableMIPeephole) {
            addPass(createSCISAMIPreEmitPeepholePass());
        }
    }
}

bool SCISAPassConfig::addIRTranslator()
{
    addPass(new IRTranslator());
    return false;
}

bool SCISAPassConfig::addLegalizeMachineIR()
{
    addPass(new Legalizer());
    return false;
}

bool SCISAPassConfig::addRegBankSelect()
{
    addPass(new RegBankSelect());
    return false;
}

bool SCISAPassConfig::addGlobalInstructionSelect()
{
    addPass(new InstructionSelect(getOptLevel()));
    return false;
}
