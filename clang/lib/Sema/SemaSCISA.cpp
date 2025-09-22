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

bool SemaSCISA::CheckSCISABuiltinFunctionCall(const TargetInfo &TI, unsigned BuiltinID, CallExpr *Call)
{
    ASTContext &Context = getASTContext();
    switch (BuiltinID) {
        case SCISA::BI__builtin_scisa_in:
        case SCISA::BI__builtin_scisa_out: {
            if (SemaRef.checkArgCount(Call, 2)) {
                return true;
            }

            Expr *Arg0 = Call->getArg(0);
            Expr *Arg1 = Call->getArg(1);

            ExprResult FirstArg = SemaRef.DefaultFunctionArrayLvalueConversion(Arg0);
            if (FirstArg.isInvalid()) {
                return true;
            }
            QualType Arg1Type = FirstArg.get()->getType();
            if (!Arg1Type->isAnyPointerType()) {
                return Diag(Call->getBeginLoc(), diag::err_attribute_argument_type) << "first" << Arg1Type << Arg0->getSourceRange();
            }
            QualType Arg2Type = Arg1->getType();
            if (!Arg2Type->isIntegerType()) {
                return Diag(Call->getBeginLoc(), diag::err_attribute_argument_type) << "second" << Arg2Type << Arg1->getSourceRange();
            }
            Call->setType(BuiltinID == SCISA::BI__builtin_scisa_in ? Context.IntTy : Context.VoidTy);
            return false;
        }
    }
    return true;
}

} // namespace clang
