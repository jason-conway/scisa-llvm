//===-- SCISARegisterInfo.h - SCISA Register Information Impl ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SCISA implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISAREGISTERINFO_H
#define LLVM_LIB_TARGET_SCISA_SCISAREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "SCISAGenRegisterInfo.inc"

namespace llvm {

struct SCISARegisterInfo : public SCISAGenRegisterInfo {

    SCISARegisterInfo();

    const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
    const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF, unsigned Kind) const override;

    const uint32_t *getCallPreservedMask(const MachineFunction &MF, CallingConv::ID) const override;

    BitVector getReservedRegs(const MachineFunction &MF) const override;

    bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj, unsigned FIOperandNum, RegScavenger *RS = nullptr) const override;

    Register getFrameRegister(const MachineFunction &MF) const override;
};
} // namespace llvm

#endif
