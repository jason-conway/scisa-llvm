//===- SCISAInstrFormats.td - SCISA Instruction Formats ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class InstSCISA<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
    let Size = 4;
    let Namespace = "SCISA";
    let DecoderNamespace = "SCISA";
    dag OutOperandList = outs;
    dag InOperandList = ins;
    let AsmString = asmstr;
    let Pattern = pattern;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : InstSCISA<outs, ins, asmstr, pattern> {
    let isPseudo = 1;
    let isCodeGenOnly = 1;
}
