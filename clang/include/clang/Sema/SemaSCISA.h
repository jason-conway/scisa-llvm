//===----- SemaSCISA.h --- SCISA target-specific routines -----*- C++ -*---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares semantic analysis functions specific to SCISA.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SEMA_SEMASCISA_H
#define LLVM_CLANG_SEMA_SEMASCISA_H

#include "clang/AST/ASTFwd.h"
#include "clang/Sema/SemaBase.h"

namespace clang {

class ParsedAttr;

class SemaSCISA : public SemaBase {
public:
    SemaSCISA(Sema &S);
    bool CheckSCISABuiltinFunctionCall(const TargetInfo &TI, unsigned BuiltinID, CallExpr *Call);
};

} // namespace clang

#endif // LLVM_CLANG_SEMA_SEMASCISA_H
