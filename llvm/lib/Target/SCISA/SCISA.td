//===-- SCISA.td - Describe the SCISA Target Machine -----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "SCISARegisterInfo.td"
include "SCISACallingConv.td"
include "SCISAInstrInfo.td"

def SCISAInstrInfo : InstrInfo;

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;
def : Proc<"v1", []>;

def SCISAInstPrinter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
  bit isMCAsmWriter = 1;
}

def SCISA : Target {
  let InstructionSet = SCISAInstrInfo;
  let AssemblyWriters = [SCISAInstPrinter];
}
