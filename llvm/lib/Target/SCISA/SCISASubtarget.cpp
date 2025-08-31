//===-- SCISASubtarget.cpp - SCISA Subtarget Information ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the SCISA specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "SCISASubtarget.h"
#include "SCISA.h"
#include "SCISATargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"

using namespace llvm;

#define DEBUG_TYPE "scisa-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "SCISAGenSubtargetInfo.inc"

void SCISASubtarget::anchor()
{
}

SCISASubtarget &SCISASubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS)
{
    initializeEnvironment();
    initSubtargetFeatures(CPU, FS);
    ParseSubtargetFeatures(CPU, CPU, FS);
    return *this;
}

void SCISASubtarget::initializeEnvironment()
{
}

void SCISASubtarget::initSubtargetFeatures(StringRef CPU, StringRef FS)
{
    if (CPU.empty()) {
        CPU = "v1";
    }
}

SCISASubtarget::SCISASubtarget(const Triple &TT, const std::string &CPU, const std::string &FS, const TargetMachine &TM)
    : SCISAGenSubtargetInfo(TT, CPU, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(CPU, FS)),
      TLInfo(TM, *this)
{
}
