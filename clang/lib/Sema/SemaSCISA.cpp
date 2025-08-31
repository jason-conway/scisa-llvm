//===------ SemaSCISA.cpp ---------- SCISA target-specific routines -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file implements semantic analysis functions specific to SCISA.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/TargetBuiltins.h"
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaSCISA.h"
#include "llvm/ADT/APSInt.h"
#include <optional>

namespace clang {

SemaSCISA::SemaSCISA(Sema &S)
    : SemaBase(S)
{
}

} // namespace clang
