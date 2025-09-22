//===--- SCISA.h - Declare SCISA target feature support ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares SCISA TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_SCISA_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_SCISA_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY SCISATargetInfo : public TargetInfo {
public:
    SCISATargetInfo(const llvm::Triple &Triple, const TargetOptions &)
        : TargetInfo(Triple)
    {
        this->LongWidth = 32;
        this->LongAlign = 32;
        this->PointerWidth = 32;
        this->PointerAlign = 32;

        this->SizeType = TargetInfo::UnsignedInt;
        this->PtrDiffType = TargetInfo::SignedInt;
        this->IntPtrType = TargetInfo::SignedInt;

        this->IntMaxType = TargetInfo::SignedLongLong;
        this->Int64Type = TargetInfo::SignedLongLong;

        this->NoAsmVariants = true;
        this->HasLongDouble = false;
        this->LongLongAlign = 32;
        this->SuitableAlign = 32;
        this->UseZeroLengthBitfieldAlignment = true;

        resetDataLayout("e-m:e-p:32:32-i8:8:8-i16:16:16-i32:32-i64:32-a:0:32-n32-S32");

        this->MaxAtomicPromoteWidth = 0;
        this->MaxAtomicInlineWidth = 0;

        this->TLSSupported = false;
        this->BigEndian = false;
        this->VLASupported = false;
    }

    void getTargetDefines(const LangOptions &Opts, MacroBuilder &Builder) const override;

    bool hasFeature(StringRef Feature) const override
    {
        return Feature == "SCISA";
    }

    void setFeatureEnabled(llvm::StringMap<bool> &Features, StringRef Name, bool Enabled) const override
    {
        Features[Name] = Enabled;
    }
    bool handleTargetFeatures(std::vector<std::string> &Features, DiagnosticsEngine &Diags) override;

    std::string_view getClobbers() const override
    {
        return "";
    }

    llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

    BuiltinVaListKind getBuiltinVaListKind() const override
    {
        return TargetInfo::VoidPtrBuiltinVaList;
    }

    bool isValidGCCRegisterName(StringRef Name) const override
    {
        return true;
    }
    ArrayRef<const char *> getGCCRegNames() const override
    {
        return {};
    }

    bool validateAsmConstraint(const char *&Name, TargetInfo::ConstraintInfo &Info) const override
    {
        return true;
    }

    ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override
    {
        return {};
    }

    bool allowDebugInfoForExternalRef() const override
    {
        return false;
    }

    CallingConvCheckResult checkCallingConvention(CallingConv CC) const override
    {
        switch (CC) {
            default:
                return CCCR_Warning;
            case CC_C:
                return CCCR_OK;
        }
    }

    bool isValidCPUName(StringRef Name) const override;

    void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

    bool setCPU(const std::string &Name) override
    {
        StringRef CPUName(Name);
        return isValidCPUName(CPUName);
    }

    std::pair<unsigned, unsigned> hardwareInterferenceSizes() const override
    {
        return std::make_pair(32, 32);
    }
};
} // namespace targets
} // namespace clang
#endif // LLVM_CLANG_LIB_BASIC_TARGETS_SCISA_H
