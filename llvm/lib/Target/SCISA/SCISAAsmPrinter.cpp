//===-- SCISAAsmPrinter.cpp - SCISA LLVM assembly writer ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the SCISA assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/SCISAInstPrinter.h"
#include "SCISA.h"
#include "SCISAInstrInfo.h"
#include "SCISAMCInstLower.h"
#include "TargetInfo/SCISATargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

class SCISAAsmPrinter : public AsmPrinter {
public:
    explicit SCISAAsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer))
    {
    }

    StringRef getPassName() const override
    {
        return "SCISA Assembly Printer";
    }
    bool doInitialization(Module &M) override;
    void printOperand(const MachineInstr *MI, int OpNum, raw_ostream &O);
    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, const char *ExtraCode, raw_ostream &O) override;
    bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum, const char *ExtraCode, raw_ostream &O) override;
    void PrintSymbolOperand(const MachineOperand &MO, raw_ostream &O) override;
    void emitInstruction(const MachineInstr *MI) override;
};

} // namespace

bool SCISAAsmPrinter::doInitialization(Module &M)
{
    AsmPrinter::doInitialization(M);
    return false;
}

void SCISAAsmPrinter::printOperand(const MachineInstr *MI, int OpNum, raw_ostream &O)
{
    const MachineOperand &MO = MI->getOperand(OpNum);

    switch (MO.getType()) {
        case MachineOperand::MO_Register:
            O << SCISAInstPrinter::getRegisterName(MO.getReg());
            break;

        case MachineOperand::MO_Immediate:
            O << MO.getImm();
            break;

        case MachineOperand::MO_MachineBasicBlock:
            O << *MO.getMBB()->getSymbol();
            break;

        case MachineOperand::MO_GlobalAddress:
            O << *getSymbol(MO.getGlobal());
            break;

        case MachineOperand::MO_BlockAddress: {
            MCSymbol *BA = GetBlockAddressSymbol(MO.getBlockAddress());
            O << BA->getName();
            break;
        }

        case MachineOperand::MO_ExternalSymbol:
            O << *GetExternalSymbolSymbol(MO.getSymbolName());
            break;

        case MachineOperand::MO_JumpTableIndex:
        case MachineOperand::MO_ConstantPoolIndex:
        default:
            llvm_unreachable("<unknown operand type>");
    }
}

void SCISAAsmPrinter::PrintSymbolOperand(const MachineOperand &MO, raw_ostream &O)
{
    // Computing the address of a global symbol, not calling it.
    // const GlobalValue *GV = MO.getGlobal();
    // getSymbol(GV)->print(O, MAI);
    // printOffset(MO.getOffset(), O);
}

bool SCISAAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, const char *ExtraCode, raw_ostream &O)
{
    if (ExtraCode && ExtraCode[0]) {
        return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
    }

    printOperand(MI, OpNo, O);
    return false;
}

bool SCISAAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum, const char *ExtraCode, raw_ostream &O)
{
    assert(OpNum + 1 < MI->getNumOperands() && "Insufficient operands");
    const MachineOperand &BaseMO = MI->getOperand(OpNum);
    const MachineOperand &OffsetMO = MI->getOperand(OpNum + 1);
    assert(BaseMO.isReg() && "Unexpected base pointer for inline asm memory operand.");
    assert(OffsetMO.isImm() && "Unexpected offset for inline asm memory operand.");
    int Offset = OffsetMO.getImm();

    if (ExtraCode) {
        return true; // Unknown modifier.
    }

    if (Offset < 0) {
        O << "(" << SCISAInstPrinter::getRegisterName(BaseMO.getReg()) << " - " << -Offset << ")";
    }
    else {
        O << "(" << SCISAInstPrinter::getRegisterName(BaseMO.getReg()) << " + " << Offset << ")";
    }

    return false;
}

void SCISAAsmPrinter::emitInstruction(const MachineInstr *MI)
{
    SCISA_MC::verifyInstructionPredicates(MI->getOpcode(), getSubtargetInfo().getFeatureBits());

    MCInst TmpInst;
    SCISAMCInstLower MCInstLowering(OutContext, *this);
    MCInstLowering.lower(MI, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSCISAAsmPrinter()
{
    RegisterAsmPrinter<SCISAAsmPrinter> X(getTheSCISATarget());
}
