//===-- SCISAMCInstLower.h - Lower MachineInstr to MCInst -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISAMCINSTLOWER_H
#define LLVM_LIB_TARGET_SCISA_SCISAMCINSTLOWER_H

#include "llvm/Support/Compiler.h"

namespace llvm {

class AsmPrinter;
class MCContext;
class MCInst;
class MCOperand;
class MCSymbol;
class MachineInstr;
class MachineOperand;

// SCISAMCInstLower - This class is used to lower an MachineInstr into an MCInst.
class LLVM_LIBRARY_VISIBILITY SCISAMCInstLower {
    MCContext &Ctx;

    AsmPrinter &Printer;

public:
    SCISAMCInstLower(MCContext &Ctx, AsmPrinter &Printer)
        : Ctx(Ctx),
          Printer(Printer)
    {
    }
    void lower(const MachineInstr *MI, MCInst &OutMI) const;

    MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

    MCSymbol *getGlobalAddressSymbol(const MachineOperand &MO) const;
    MCSymbol *getExternalSymbolSymbol(const MachineOperand &MO) const;
};

} // namespace llvm

#endif
