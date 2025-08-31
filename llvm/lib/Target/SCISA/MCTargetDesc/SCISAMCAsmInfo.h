//===-- SCISAMCAsmInfo.h - SCISA asm properties -------------------*- C++ -*--====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the SCISAMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_MCTARGETDESC_SCISAMCASMINFO_H
#define LLVM_LIB_TARGET_SCISA_MCTARGETDESC_SCISAMCASMINFO_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

namespace llvm {

class SCISAMCAsmInfo : public MCAsmInfo {
public:
    explicit SCISAMCAsmInfo(const Triple &TT, const MCTargetOptions &Options)
    {
        IsLittleEndian = true;
        SupportsSignedData = true;
        CodePointerSize = 4;

        CommentString = ";";
        PrivateGlobalPrefix = "L";
        ZeroDirective = "\t.zero\t";
        Data8bitsDirective = "\t.byte\t";
        Data16bitsDirective = "\t.hword\t";
        Data32bitsDirective = "\t.word\t";
        AsciiDirective = "\t.ascii\t";
        Data64bitsDirective = nullptr;
        AscizDirective = nullptr;
        GlobalDirective = nullptr;

        ExceptionsType = ExceptionHandling::None;
        AllowAdditionalComments = false;
        HasFunctionAlignment = false;
        HasDotTypeDotSizeDirective = false;
        HasLEB128Directives = false;
        HasSingleParameterDotFile = false;
        HasIdentDirective = false;
    }
    bool shouldOmitSectionDirective(StringRef SectionName) const override
    {
        return true;
    }
};

} // namespace llvm

#endif
