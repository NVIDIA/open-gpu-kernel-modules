/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef KERNEL_GSP_HANG_DEBUG_DUMP_H
#define KERNEL_GSP_HANG_DEBUG_DUMP_H

#include "nvtypes.h"
#include "nvstatus.h"


typedef struct PRB_ENCODER PRB_ENCODER;
typedef struct OBJGPU OBJGPU;


typedef struct
{
    NvU32 xid;               // ROBUST_CHANNEL_* code of the trigger
    NvU32 ctxswTimeoutInfo;  // NV_RUNLIST_ENGINE_CTXSW_TIMEOUT_INFO; 0 unless xid 109
    NvU64 timestampNs;
} HANG_DEBUG_TRIGGER_INFO;

NV_STATUS hangDebugDumpCaptureAlloc(OBJGPU *pGpu, const HANG_DEBUG_TRIGGER_INFO *pTrigger,
                                    NvU8 **ppBuf, NvU32 bufSize, NvU32 *pEncLen);

void hangDebugDumpCapture(OBJGPU *pGpu, const HANG_DEBUG_TRIGGER_INFO *pTrigger);

void hangDebugDumpEmitAllBlocks(OBJGPU *pGpu, PRB_ENCODER *pEnc);

void hangDebugDumpRead(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset);

/* Like hangDebugDumpRead but skips encoding when val == 0. */
void hangDebugDumpReadIfNz(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset);

/* Open a block tagged with devicePriBase (0 = chipwide, field omitted). */
void hangDebugDumpBlockBegin(PRB_ENCODER *pEnc, NvU32 devicePriBase);

void hangDebugDumpBlockEnd(PRB_ENCODER *pEnc);

/* BAR0 save / restore / write helpers. */
void hangDebugDumpSave(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU32 *savedSlot, NvU64 offset);
void hangDebugDumpRestore(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset, NvU32 savedValue);
void hangDebugDumpWriteImm(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset, NvU32 value);
void hangDebugDumpWriteRmw(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset, NvU32 savedValue,
                           NvU32 mask, NvU32 orValue);

/* Device-info iterator over entries matching typeEnum; bUniqueByBase
 * suppresses duplicate-priBase hits. */
typedef struct HangDebugDumpDeviceIter {
    NvU32  typeEnum;
    NvU32  cursor;
    NvBool bUniqueByBase;
    /* outputs, valid after a successful hangDebugDumpDeviceIterNext */
    NvU32  priBase;
    NvU32  instId;
    NvU32  runlistPriBase;
    NvBool bIsEngine;
} HangDebugDumpDeviceIter;

void   hangDebugDumpDeviceIterInit(HangDebugDumpDeviceIter *pIter, NvU32 typeEnum,
                             NvBool bUniqueByBase);
NvBool hangDebugDumpDeviceIterNext(OBJGPU *pGpu, HangDebugDumpDeviceIter *pIter);

/* Resolve an engine's CHRAM pri base from its runlist pri base; 0 if none. */
NvU32 hangDebugDumpRunlistToChramBase(OBJGPU *pGpu, NvU32 runlistPriBase);

/* Probe one register of an instance: NV_FALSE when the read returns the
 * PRI-error sentinel. The probed value is encoded either way. */
NvBool hangDebugDumpProbePresent(OBJGPU *pGpu, PRB_ENCODER *pEnc, NvU64 offset);

#endif // KERNEL_GSP_HANG_DEBUG_DUMP_H
