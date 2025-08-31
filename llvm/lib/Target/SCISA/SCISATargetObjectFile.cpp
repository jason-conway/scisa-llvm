//===-- SCISATargetObjectFile.cpp - SCISA Object Info -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about SCISA target object file.
//
//===----------------------------------------------------------------------===//

#include "SCISATargetObjectFile.h"
#include "SCISA.h"

using namespace llvm;

SCISATargetObjectFile::~SCISATargetObjectFile() = default;

MCSection *SCISATargetObjectFile::SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const
{
    return getDataSection();
}
