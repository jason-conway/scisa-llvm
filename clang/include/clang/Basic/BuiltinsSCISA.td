//===--- BuiltinsSCISA.td - SCISA Builtin function database ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "clang/Basic/BuiltinsBase.td"

class SCISABuiltin<string prototype> : TargetBuiltin {
  let Spellings = [NAME];
  let Prototype = prototype;
}

let Attributes = [NoThrow] in {
  def __scisa_read_reg_cyc : SCISABuiltin<"unsigned int(...)">;
}
