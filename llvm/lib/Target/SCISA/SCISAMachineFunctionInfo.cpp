//===-- SCISAMachineFunctionInfo.cpp - Machine Function Info ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SCISA implementation of the MachineFunctionInfo class.
//
//===----------------------------------------------------------------------===//

#include "SCISAMachineFunctionInfo.h"
#include "SCISAInstrInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"

using namespace llvm;

SCISAMachineFunctionInfo::~SCISAMachineFunctionInfo()
{
}

void SCISAMachineFunctionInfo::anchor()
{
}
