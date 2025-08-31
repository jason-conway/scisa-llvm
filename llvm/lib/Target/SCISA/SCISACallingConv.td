//===-- SCISACallingConv.td - Calling Conventions SCISA --------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the SCISA architecture.
//
//===----------------------------------------------------------------------===//

def RetCC_SCISA32 : CallingConv<[
  // The first 4 integer values are returned in registers R0, R1, R2, R3
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CC_SCISA32 : CallingConv<[
  // Pass by value if the byval attribute is given
  CCIfByVal<CCPassByVal<4, 4>>,

  // Promote i1/i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // The first 12 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CSR : CalleeSavedRegs<(add R16, R17, R18, R19, R20, R21, R22, R23, SP, LR)>;
