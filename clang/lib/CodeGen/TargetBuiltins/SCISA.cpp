//===----------- SCISA.cpp - Emit LLVM Code for builtins ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This contains code to emit Builtin calls as LLVM code.
//
//===----------------------------------------------------------------------===//

#include "CodeGenFunction.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/IR/IntrinsicsSCISA.h"

using namespace clang;
using namespace CodeGen;
using namespace llvm;

// EmitSCISABuiltinExpr - Emit a call to a SCISA builtin function.
Value *CodeGenFunction::EmitSCISABuiltinExpr(unsigned BuiltinID, const CallExpr *E)
{
    switch (BuiltinID) {
        case SCISA::BI__builtin_scisa_in: {
            Value *Addr = EmitScalarExpr(E->getArg(0));
            Value *Size  = EmitScalarExpr(E->getArg(1));
            Function *F = CGM.getIntrinsic(Intrinsic::scisa_in);
            return Builder.CreateCall(F, {Addr, Size});
        }
        case SCISA::BI__builtin_scisa_out: {
            Value *Addr = EmitScalarExpr(E->getArg(0));
            Value *Size = EmitScalarExpr(E->getArg(1));
            Function *F = CGM.getIntrinsic(Intrinsic::scisa_out);
            return Builder.CreateCall(F, {Addr, Size});
        }
    }
    return nullptr;
}
