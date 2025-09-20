//===-- SCISASelectionDAGInfo.cpp - SCISA SelectionDAG Info ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the SCISASelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#include "SCISATargetMachine.h"
#include "llvm/CodeGen/SelectionDAG.h"
using namespace llvm;

#define DEBUG_TYPE "scisa-selectiondag-info"

SDValue SCISASelectionDAGInfo::EmitTargetCodeForMemcpy(SelectionDAG &DAG, const SDLoc &DL, SDValue Chain, SDValue Dst, SDValue Src, SDValue Size, Align Alignment, bool isVolatile, bool AlwaysInline, MachinePointerInfo DstPtrInfo, MachinePointerInfo SrcPtrInfo) const
{
    // Requires the copy size to be a constant.
    ConstantSDNode *ConstantSize = dyn_cast<ConstantSDNode>(Size);
    if (!ConstantSize) {
        return SDValue();
    }

    unsigned CopyLen = ConstantSize->getZExtValue();
    unsigned StoresNumEstimate = alignTo(CopyLen, Alignment) >> Log2(Alignment);
    // Impose the same copy length limit as MaxStoresPerMemcpy.
    if (StoresNumEstimate > getCommonMaxStoresPerMemFunc()) {
        return SDValue();
    }

    SDVTList VTs = DAG.getVTList(MVT::Other, MVT::Glue);

    Dst = DAG.getNode(SCISAISD::MEMCPY, DL, VTs, Chain, Dst, Src, DAG.getConstant(CopyLen, DL, MVT::i32), DAG.getConstant(Alignment.value(), DL, MVT::i32));

    return Dst.getValue(0);
}
