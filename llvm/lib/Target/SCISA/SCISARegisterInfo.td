//===-- SCISARegisterInfo.td - SCISA Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the SCISA register file
//===----------------------------------------------------------------------===//

// Base class for a SCISA register.
class SCISAReg <string n> : Register<n> {
  let Namespace = "SCISA";
}

// General-Purpose Registers (GPRs) r0-r31
let Namespace = "SCISA" in {
  def R0  : SCISAReg<"r0">;
  def R1  : SCISAReg<"r1">;
  def R2  : SCISAReg<"r2">;
  def R3  : SCISAReg<"r3">;
  def R4  : SCISAReg<"r4">;
  def R5  : SCISAReg<"r5">;
  def R6  : SCISAReg<"r6">;
  def R7  : SCISAReg<"r7">;
  def R8  : SCISAReg<"r8">;
  def R9  : SCISAReg<"r9">;
  def R10 : SCISAReg<"r10">;
  def R11 : SCISAReg<"r11">;
  def R12 : SCISAReg<"r12">;
  def R13 : SCISAReg<"r13">;
  def R14 : SCISAReg<"r14">;
  def R15 : SCISAReg<"r15">;
  def R16 : SCISAReg<"r16">;
  def R17 : SCISAReg<"r17">;
  def R18 : SCISAReg<"r18">;
  def R19 : SCISAReg<"r19">;
  def R20 : SCISAReg<"r20">;
  def R21 : SCISAReg<"r21">;
  def R22 : SCISAReg<"r22">;
  def R23 : SCISAReg<"r23">;
  def R24 : SCISAReg<"r24">;
  def R25 : SCISAReg<"r25">;
  def R26 : SCISAReg<"r26">;
  def R27 : SCISAReg<"r27">;
  def R28 : SCISAReg<"r28">;
  def R29 : SCISAReg<"r29">;
  def R30 : SCISAReg<"r30">;
  def R31 : SCISAReg<"r31">;

  def LR : SCISAReg<"lr">; // Link Register
  def FP : SCISAReg<"fp">; // Frame Pointer
  def SP : SCISAReg<"sp">; // Stack Pointer
  def CC : SCISAReg<"cc">; // Condition Code Register
  def CYC : SCISAReg<"cyc">; // Cycle Counter
  def PC : SCISAReg<"pc">; // Program Counter

}

// Register classes.
def GPR32 : RegisterClass<"SCISA", [i32], 32, (add
  // Return values
  R0, R1, R2, R3,
  // Function arguments
  R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15,
  // Callee save
  R16, R17, R18, R19, R20, R21, R22, R23,
  // Not preserved across procedure calls
  R24, R25, R26, R27, R28, R29, R30, R31,
  // Reserved
  LR, FP, SP
)>;

def PCREG : RegisterClass<"SCISA", [i32], 32, (add PC)>;
def SPREG : RegisterClass<"SCISA", [i32], 32, (add SP)>;
def FPREG : RegisterClass<"SCISA", [i32], 32, (add FP)>;
def LRREG : RegisterClass<"SCISA", [i32], 32, (add LR)>;
def CCREG : RegisterClass<"SCISA", [i32], 32, (add CC)>;
def CYCREG : RegisterClass<"SCISA", [i32], 32, (add CYC)>;
