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

let TargetPrefix = "scisa" in {

    def int_scisa_in : Intrinsic<
        [llvm_i32_ty], // return u32
        [llvm_ptr_ty, llvm_i32_ty], // (void *, u32)
        [IntrWriteMem]
    >;

    def int_scisa_out : Intrinsic<
        [], // void
        [llvm_ptr_ty, llvm_i32_ty], // (void *, u32)
        [IntrReadMem]
    >;
}
