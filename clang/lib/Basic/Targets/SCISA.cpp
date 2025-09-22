//===--- SCISA.cpp - Implement SCISA target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements SCISA TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "SCISA.h"
#include "Targets.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringRef.h"

using namespace clang;
using namespace clang::targets;

static constexpr int NumBuiltins =
    clang::SCISA::LastTSBuiltin - Builtin::FirstTSBuiltin;

#define GET_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsSCISA.inc"
#undef GET_BUILTIN_STR_TABLE

static constexpr Builtin::Info BuiltinInfos[] = {
#define GET_BUILTIN_INFOS
#include "clang/Basic/BuiltinsSCISA.inc"
#undef GET_BUILTIN_INFOS
};

static_assert(std::size(BuiltinInfos) == NumBuiltins);

void SCISATargetInfo::getTargetDefines(const LangOptions &Opts, MacroBuilder &Builder) const
{
    Builder.defineMacro("__SCISA__");
    Builder.defineMacro("__scisa__");

    std::string CPU = getTargetOpts().CPU;
    if (CPU.empty()) {
        CPU = "v1";
    }

    std::string CpuVerNumStr = CPU.substr(1);
    Builder.defineMacro("__SCISA_VM_VERSION__", CpuVerNumStr);
}

static constexpr llvm::StringLiteral ValidCPUNames[] = { "generic", "v1" };

bool SCISATargetInfo::isValidCPUName(StringRef Name) const
{
    return llvm::is_contained(ValidCPUNames, Name);
}

void SCISATargetInfo::fillValidCPUList(SmallVectorImpl<StringRef> &Values) const
{
    Values.append(std::begin(ValidCPUNames), std::end(ValidCPUNames));
}

bool SCISATargetInfo::handleTargetFeatures(std::vector<std::string> &Features, DiagnosticsEngine &Diags)
{
    return true;
}

llvm::SmallVector<Builtin::InfosShard> SCISATargetInfo::getTargetBuiltins() const
{
    return {{&BuiltinStrings, BuiltinInfos}};
}
