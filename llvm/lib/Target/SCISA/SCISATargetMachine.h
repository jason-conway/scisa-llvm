//===-- SCISATargetMachine.h - Define TargetMachine for SCISA ----- C++ ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the SCISA specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISATARGETMACHINE_H
#define LLVM_LIB_TARGET_SCISA_SCISATARGETMACHINE_H

#include "SCISASubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include <optional>

namespace llvm {

class SCISATargetMachine : public CodeGenTargetMachineImpl {
    std::unique_ptr<TargetLoweringObjectFile> TLOF;
    SCISASubtarget Subtarget;

public:
    SCISATargetMachine(const Target &T, const Triple &TT, StringRef CPU, StringRef FS, const TargetOptions &Options, std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM, CodeGenOptLevel OL, bool JIT);

    const SCISASubtarget *getSubtargetImpl() const
    {
        return &Subtarget;
    }
    const SCISASubtarget *getSubtargetImpl(const Function &) const override
    {
        return &Subtarget;
    }

    TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

    TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

    TargetLoweringObjectFile *getObjFileLowering() const override
    {
        return TLOF.get();
    }

    void registerPassBuilderCallbacks(PassBuilder &PB) override;
};

} // namespace llvm

#endif
