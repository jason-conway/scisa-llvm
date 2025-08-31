//===- IntrinsicsSCISA.td - Defines SCISA intrinsics -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the SCISA-specific intrinsics.
//
//===----------------------------------------------------------------------===//
class ReadRegIntrinsic<string name>
  : DefaultAttrsIntrinsic<[llvm_i32_ty], [], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>,
    ClangBuiltin<"__scisa_read_reg_" # name>;

// Specialized loads from packet
let TargetPrefix = "scisa" in {
  def int_scisa_read_reg_clock : ReadRegIntrinsic<"cyc">;
}
