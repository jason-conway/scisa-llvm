//===-- SCISASelectionDAGInfo.h - SCISA SelectionDAG Info -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the SCISA subclass for SelectionDAGTargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_SCISASELECTIONDAGINFO_H
#define LLVM_LIB_TARGET_SCISA_SCISASELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class SCISASelectionDAGInfo : public SelectionDAGTargetInfo {
public:
    SDValue EmitTargetCodeForMemcpy(SelectionDAG &DAG, const SDLoc &DL, SDValue Chain, SDValue Dst, SDValue Src, SDValue Size, Align Alignment, bool isVolatile, bool AlwaysInline, MachinePointerInfo DstPtrInfo, MachinePointerInfo SrcPtrInfo) const override;

    unsigned getCommonMaxStoresPerMemFunc() const
    {
        return 128;
    }
};

} // namespace llvm

#endif
