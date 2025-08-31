//===-- SCISAMachineFuctionInfo.h - SCISA machine function info -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares SCISA-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SCISA_MACHINE_FUNCTION_INFO_H
#define LLVM_SCISA_MACHINE_FUNCTION_INFO_H


#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include <map>

namespace llvm {

class SCISAMachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();

    MachineFunction &MF;

    /// SRetReturnReg - Some subtargets require that sret lowering includes
    /// returning the value of the returned struct in a register. This field
    /// holds the virtual register into which the sret argumçent is passed.
    unsigned SRetReturnReg;

    /// index where call saves PC and status word
    int call_ret_idx;

    /// VarArgsFrameOffset - offset for start of varargs area.
    int VarArgsFrameOffset;

    /// VarArgsFrameIndex - index for start of varargs area.
    int VarArgsFrameIndex;

    /// True if function has a byval argument.
    bool HasByvalArg;

    /// Size of incoming argument area.
    unsigned IncomingArgSize;

    /// Size of the callee-saved register portion of the stack frame in bytes.
    unsigned CalleeSavedFrameSize;

public:
    SCISAMachineFunctionInfo(MachineFunction &MF)
        : MF(MF),
          SRetReturnReg(0),
          VarArgsFrameOffset(0),
          VarArgsFrameIndex(0),
          IncomingArgSize(0),
          CalleeSavedFrameSize(0)
    {
        (void)MF;
    }

    ~SCISAMachineFunctionInfo();

    int getCallRetIdx() const
    {
        return call_ret_idx;
    }
    void setCallRetIdx(int i)
    {
        call_ret_idx = i;
    }

    int getVarArgsFrameOffset() const
    {
        return VarArgsFrameOffset;
    }
    void setVarArgsFrameOffset(int off)
    {
        VarArgsFrameOffset = off;
    }

    int getVarArgsFrameIndex() const
    {
        return VarArgsFrameIndex;
    }
    void setVarArgsFrameIndex(int idx)
    {
        VarArgsFrameIndex = idx;
    }

    unsigned getSRetReturnReg() const
    {
        return SRetReturnReg;
    }
    void setSRetReturnReg(unsigned Reg)
    {
        SRetReturnReg = Reg;
    }

    bool hasByvalArg() const
    {
        return HasByvalArg;
    }
    void setFormalArgInfo(unsigned Size, bool HasByval)
    {
        IncomingArgSize = Size;
        HasByvalArg = HasByval;
    }

    unsigned getCalleeSavedFrameSize() const
    {
        return CalleeSavedFrameSize;
    }
    void setCalleeSavedFrameSize(unsigned Bytes)
    {
        CalleeSavedFrameSize = Bytes;
    }

    unsigned getIncomingArgSize() const
    {
        return IncomingArgSize;
    }
};

} // end of namespace llvm

#endif // LLVM_SCISA_MACHINE_FUNCTION_INFO_H
