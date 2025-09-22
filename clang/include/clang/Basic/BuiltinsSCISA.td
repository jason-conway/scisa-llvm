//===--- BuiltinsSCISA.td - SCISA Builtin function database -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "clang/Basic/BuiltinsBase.td"

class SCISABuiltin<string prototype, string features = ""> : TargetBuiltin {
  let Spellings = ["__builtin_scisa" # NAME];
  let Prototype = prototype;
  let Features = features;
}

let Attributes = [NoThrow] in {
  def _in : SCISABuiltin<"unsigned int(void *, unsigned int)">;
  def _out : SCISABuiltin<"void(void *, unsigned int)">;
}
