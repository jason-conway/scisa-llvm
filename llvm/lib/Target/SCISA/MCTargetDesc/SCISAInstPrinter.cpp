//===-- SCISAInstPrinter.cpp - Convert SCISA MCInst to asm syntax -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an SCISA MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/SCISAInstPrinter.h"
#include "SCISA.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#include "SCISAGenAsmWriter.inc"

void SCISAInstPrinter::printInst(const MCInst *MI, uint64_t Address, StringRef Annot, const MCSubtargetInfo &STI, raw_ostream &O)
{
    printInstruction(MI, Address, O);
    printAnnotation(O, Annot);
}

static void printExpr(const MCExpr *Expr, raw_ostream &O)
{
    const MCSymbolRefExpr *SRE;

    if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
        SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    }
    else {
        SRE = dyn_cast<MCSymbolRefExpr>(Expr);
    }
    if (!SRE) {
        report_fatal_error("Unexpected MCExpr type.");
    }

    // MCSymbolRefExpr::VariantKind Kind = SRE->getKind();
    // assert(Kind == MCSymbolRefExpr::VK_None);

    O << *Expr;
}

void SCISAInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O, const char *Modifier)
{
    assert((Modifier == nullptr || Modifier[0] == 0) && "No modifiers supported");
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isReg()) {
        O << getRegisterName(Op.getReg());
    }
    else if (Op.isImm()) {
        O << formatImm((int32_t)Op.getImm());
    }
    else {
        assert(Op.isExpr() && "Expected an expression");
        printExpr(Op.getExpr(), O);
    }
}

void SCISAInstPrinter::printMemOperand(const MCInst *MI, int OpNo, raw_ostream &O, const char *Modifier)
{
    const MCOperand &RegOp = MI->getOperand(OpNo);
    const MCOperand &OffsetOp = MI->getOperand(OpNo + 1);

    // offset
    if (OffsetOp.isImm()) {
        auto Imm = OffsetOp.getImm();
        if (Imm >= 0) {
            O << formatImm(Imm);
        }
        else {
            O << "-" << formatImm(-Imm);
        }
    }
    else {
        assert(0 && "Expected an immediate");
    }
    // register
    assert(RegOp.isReg() && "Register operand not a register");
    O << "(" << getRegisterName(RegOp.getReg()) << ")";
}

void SCISAInstPrinter::printImm32Operand(const MCInst *MI, unsigned OpNo, raw_ostream &O)
{
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isImm()) {
        O << formatImm(Op.getImm());
    }
    else if (Op.isExpr()) {
        O << "(" << *Op.getExpr() << ")";
    }
    else {
        O << Op;
    }
}

void SCISAInstPrinter::printBrTargetOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O)
{
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isImm()) {
        if (MI->getOpcode() == SCISA::B) {
            int32_t Imm = Op.getImm();
            O << ((Imm >= 0) ? "+" : "") << formatImm(Imm);
        }
    }
    else if (Op.isExpr()) {
        printExpr(Op.getExpr(), O);
    }
    else {
        O << Op;
    }
}

void SCISAInstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O)
{
    unsigned CC = MI->getOperand(OpNo).getImm();
    switch (CC) {
        default:
            llvm_unreachable("Unsupported CC code");
        case SCISACC::NE:
            O << "ne";
            break;
        case SCISACC::EQ:
            O << "eq";
            break;
        case SCISACC::GE:
            O << "ge";
            break;
        case SCISACC::GT:
            O << "gt";
            break;
        case SCISACC::LE:
            O << "le";
            break;
        case SCISACC::LT:
            O << "lt";
            break;
        case SCISACC::HS:
            O << "hs";
            break;
        case SCISACC::HI:
            O << "hi";
            break;
        case SCISACC::LS:
            O << "ls";
            break;
        case SCISACC::LO:
            O << "lo";
            break;
    }
}
