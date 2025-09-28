//===-- SCISAMCTargetDesc.h - SCISA Target Descriptions -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides SCISA specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SCISA_MCTARGETDESC_SCISAMCTARGETDESC_H
#define LLVM_LIB_TARGET_SCISA_MCTARGETDESC_SCISAMCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

} // namespace llvm

// Defines symbolic names for SCISA registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "SCISAGenRegisterInfo.inc"

// Defines symbolic names for the SCISA instructions.
//
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "SCISAGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "SCISAGenSubtargetInfo.inc"

#endif
