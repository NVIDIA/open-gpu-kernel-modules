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

#include "gpu/gsp/kernel_gsp_hang_debug_dump.h"
#include "os/os.h"                           /* osDevReadReg032 (GPU_REG_RD32_UNCHECKED) */
#include "kernel/diagnostics/journal.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "lib/protobuf/prb.h"
#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"
#include "g_regs_pb.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"  /* DEVICE_INFO_ENTRY */
#include "kernel/gpu/fifo/kernel_fifo.h"     /* kfifoEngineInfoXlate */

/* Outer-envelope nesting depth held by the encoder while per-block
 * dump programs run (set up in hangDebugDumpCapture):
 *   depth 1 = NVDUMP
 *   depth 2 = NVDUMP_GPU_INFO
 *   depth 3 = GPUINFO_ENG_HANG_DEBUG  <-- block_begin pushes from here
 */
#define HANG_DEBUG_DUMP_OUTER_ENVELOPE_DEPTH 3

static NvBool
hangDebugDumpIsCaptureAllowed
(
    OBJGPU *pGpu
)
{
    return gpuGetMode(pGpu) == NV_GPU_MODE_COMPUTE_MODE &&
           !IS_VIRTUAL(pGpu) &&
           !gpuIsSriovEnabled(pGpu) &&
           !IS_MIG_IN_USE(pGpu);
}

static void
_hangDebugDumpEmitReg
(
    PRB_ENCODER *pEnc,
    NvU32        type,
    NvU64        offset,
    NvU32        val
)
{
    if (prbEncNestedStart(pEnc, NVDEBUG_ENG_HANGDEBUG_BLOCK_REGS) != NV_OK)
        return;

    prbEncAddEnum  (pEnc, REGS_REGSANDMEM_TYPE,   type);
    prbEncAddUInt64(pEnc, REGS_REGSANDMEM_OFFSET, offset);
    prbEncAddUInt32(pEnc, REGS_REGSANDMEM_VAL,    val);

    prbEncNestedEnd(pEnc);
}

/* Read helpers use GPU_REG_RD32_UNCHECKED. The sentinel still lands in
 * Regs.RegsAndMem.val, so the decoder can tell "register == 0" from
 * other errors. */
void
hangDebugDumpRead
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset
)
{
    NvU32 val = GPU_REG_RD32_UNCHECKED(pGpu, (NvU32)offset);

    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS, offset, val);
}

void
hangDebugDumpReadIfNz
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset
)
{
    NvU32 val = GPU_REG_RD32_UNCHECKED(pGpu, (NvU32)offset);

    if (val != 0)
        _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS, offset, val);
}

void
hangDebugDumpBlockBegin
(
    PRB_ENCODER *pEnc,
    NvU32        devicePriBase
)
{
    if (prbEncNestedStart(pEnc, NVDEBUG_ENG_HANGDEBUG_BLOCKS) != NV_OK)
        return;

    if (devicePriBase != 0)
    {
        prbEncAddUInt64(pEnc,
                        NVDEBUG_ENG_HANGDEBUG_BLOCK_DEVICE_PRI_BASE,
                        (NvU64)devicePriBase);
    }
}

void
hangDebugDumpBlockEnd
(
    PRB_ENCODER *pEnc
)
{
    /* Skip the pop when block_begin's prbEncNestedStart failed
     * (PRB_ENCODE_DISABLED / NV_ERR_BUFFER_TOO_SMALL leave depth unchanged)
     */
    if (pEnc->depth <= HANG_DEBUG_DUMP_OUTER_ENVELOPE_DEPTH)
        return;

    prbEncNestedEnd(pEnc);
}

void
hangDebugDumpSave
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU32       *savedSlot,
    NvU64        offset
)
{
    *savedSlot = GPU_REG_RD32_UNCHECKED(pGpu, (NvU32)offset);
    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS, offset, *savedSlot);
}

void
hangDebugDumpRestore
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset,
    NvU32        savedValue
)
{
    GPU_REG_WR32(pGpu, (NvU32)offset, savedValue);
    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS_WR, offset, savedValue);
    hangDebugDumpRead(pGpu, pEnc, offset);
}

void
hangDebugDumpWriteImm
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset,
    NvU32        value
)
{
    hangDebugDumpRead(pGpu, pEnc, offset);
    GPU_REG_WR32(pGpu, (NvU32)offset, value);
    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS_WR, offset, value);
    hangDebugDumpRead(pGpu, pEnc, offset);
}

void
hangDebugDumpWriteRmw
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset,
    NvU32        savedValue,
    NvU32        mask,
    NvU32        orValue
)
{
    NvU32 val = (savedValue & mask) | orValue;
    GPU_REG_WR32(pGpu, (NvU32)offset, val);
    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS_WR, offset, val);
    hangDebugDumpRead(pGpu, pEnc, offset);
}

/* True if an earlier same-typeEnum device-info entry (table index < idx)
 * shares devicePriBase. Uses the canonical iterator, not a raw
 * pDeviceInfoTable walk. */
static NvBool
_hangDebugDumpBaseSeenEarlier
(
    OBJGPU *pGpu,
    NvU32   typeEnum,
    NvU32   devicePriBase,
    NvU32   idx
)
{
    DeviceInfoIter nvIter;

    if (gpuIterDeviceInfo(pGpu, &nvIter, typeEnum,
                          DEVICE_INFO_DIELET_INSTANCE_ANY) != NV_OK)
        return NV_FALSE;

    while (gpuDeviceInfoIterNext(pGpu, &nvIter))
    {
        if ((NvU32)(nvIter.pEntry - pGpu->pDeviceInfoTable) >= idx)
            break;
        if (nvIter.pEntry->devicePriBase == devicePriBase)
            return NV_TRUE;
    }
    return NV_FALSE;
}

void
hangDebugDumpDeviceIterInit
(
    HangDebugDumpDeviceIter *pIter,
    NvU32              typeEnum,
    NvBool             bUniqueByBase
)
{
    pIter->typeEnum       = typeEnum;
    pIter->cursor         = 0;
    pIter->bUniqueByBase  = bUniqueByBase;
    pIter->priBase        = 0;
    pIter->instId         = 0;
    pIter->runlistPriBase = 0;
    pIter->bIsEngine      = NV_FALSE;
}

NvBool
hangDebugDumpDeviceIterNext
(
    OBJGPU            *pGpu,
    HangDebugDumpDeviceIter *pIter
)
{
    DeviceInfoIter nvIter;

    if (gpuIterDeviceInfo(pGpu, &nvIter, pIter->typeEnum,
                          DEVICE_INFO_DIELET_INSTANCE_ANY) != NV_OK)
    {
        pIter->cursor = pGpu->numDeviceInfoEntries;
        return NV_FALSE;
    }

    /* cursor is the raw table index of the next entry to examine.
     * gpuDeviceInfoIterNext does pEntry++ first, so resume at cursor-1. */
    if (pIter->cursor > 0)
        nvIter.pEntry = &pGpu->pDeviceInfoTable[pIter->cursor - 1];

    while (gpuDeviceInfoIterNext(pGpu, &nvIter))
    {
        const DEVICE_INFO_ENTRY *pE  = nvIter.pEntry;
        NvU32                    idx = (NvU32)(pE - pGpu->pDeviceInfoTable);

        pIter->cursor = idx + 1;

        if (pIter->bUniqueByBase &&
            _hangDebugDumpBaseSeenEarlier(pGpu, pIter->typeEnum, pE->devicePriBase, idx))
            continue;

        pIter->priBase        = pE->devicePriBase;
        pIter->instId         = pE->groupLocalInstanceId;
        pIter->runlistPriBase = pE->runlistPriBase;
        pIter->bIsEngine      = (pE->isEngine != 0) ? NV_TRUE : NV_FALSE;
        return NV_TRUE;
    }

    pIter->cursor = pGpu->numDeviceInfoEntries;
    return NV_FALSE;
}

NvU32
hangDebugDumpRunlistToChramBase
(
    OBJGPU *pGpu,
    NvU32   runlistPriBase
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32       chramBase   = 0;

    if (pKernelFifo == NULL)
        return 0;

    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
            ENGINE_INFO_TYPE_RUNLIST_PRI_BASE, runlistPriBase,
            ENGINE_INFO_TYPE_CHRAM_PRI_BASE,   &chramBase) != NV_OK)
    {
        return 0;
    }
    return chramBase;
}

/* Per-instance skip guard: present unless the instance's first register reads
 * the PRI-error sentinel. The probed value is encoded either way, so the
 * decoder sees the probe read (sentinel included) instead of a silent skip. */
NvBool
hangDebugDumpProbePresent
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pEnc,
    NvU64        offset
)
{
    NvU32 val = GPU_REG_RD32_UNCHECKED(pGpu, (NvU32)offset);

    _hangDebugDumpEmitReg(pEnc, REGS_REGSANDMEM_GPU_REGS, offset, val);
    /* Absent-device PRI error decodes to GPU_READ_PRI_ERROR_CODE (top 12 bits). */
    return (val & GPU_READ_PRI_ERROR_MASK) != GPU_READ_PRI_ERROR_CODE;
}


// Per-block HAL dispatch. Keep this list in sync with the per-chip EmitBlock<N> count
void
hangDebugDumpEmitAllBlocks(OBJGPU *pGpu, PRB_ENCODER *pEnc)
{
    gpuHangDebugDumpEmitBlock0_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock1_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock2_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock3_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock4_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock5_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock6_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock7_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock8_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock9_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock10_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock11_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock12_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock13_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock14_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock15_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock16_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock17_HAL(pGpu, pEnc);
    gpuHangDebugDumpEmitBlock18_HAL(pGpu, pEnc);
}

static NV_STATUS
_hangDebugDumpEncodeToBuffer
(
    OBJGPU                        *pGpu,
    const HANG_DEBUG_TRIGGER_INFO *pTrigger,
    NvU8                          *pBuf,
    NvU32                          bufSize,
    NvU32                         *pEncLen
)
{
    PRB_ENCODER  hangDebugDumpEnc;
    void        *pFinished = NULL;
    *pEncLen = 0;

    if (pBuf == NULL || bufSize == 0)
        return NV_ERR_INVALID_ARGUMENT;

    /* Fixed buffer, no grow-callback: adds fail once full. */
    prbEncStart(&hangDebugDumpEnc, NVDEBUG_NVDUMP, pBuf, bufSize, NULL);

    if (prbEncNestedStart(&hangDebugDumpEnc, NVDEBUG_NVDUMP_GPU_INFO) != NV_OK ||
        prbEncNestedStart(&hangDebugDumpEnc, NVDEBUG_GPUINFO_ENG_HANG_DEBUG) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "prbEncNestedStart failed for gpu_info/eng_hang_debug; aborting dump\n");
        return NV_ERR_INVALID_STATE;
    }

    /* Trigger info is best-effort: on failure, continue with the register dump. */
    if (pTrigger != NULL)
    {
        if (prbEncNestedStart(&hangDebugDumpEnc, NVDEBUG_ENG_HANGDEBUG_TRIGGER) == NV_OK)
        {
            prbEncAddUInt32(&hangDebugDumpEnc, NVDEBUG_ENG_HANGDEBUG_TRIGGERINFO_XID,                pTrigger->xid);
            prbEncAddUInt32(&hangDebugDumpEnc, NVDEBUG_ENG_HANGDEBUG_TRIGGERINFO_CTXSW_TIMEOUT_INFO, pTrigger->ctxswTimeoutInfo);
            prbEncAddUInt64(&hangDebugDumpEnc, NVDEBUG_ENG_HANGDEBUG_TRIGGERINFO_TIMESTAMP_NS,       pTrigger->timestampNs);
            prbEncNestedEnd(&hangDebugDumpEnc);  // trigger
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "prbEncNestedStart failed for trigger; skipping trigger info\n");
        }
    }

    hangDebugDumpEmitAllBlocks(pGpu, &hangDebugDumpEnc);

    prbEncNestedEnd(&hangDebugDumpEnc);  // eng_hang_debug
    prbEncNestedEnd(&hangDebugDumpEnc);  // gpu_info

    /* Exhaustion/near-cap check, after both prbEncNestedEnd so the envelopes
     * count as freed. An add that ran out of space bails mid-varint, leaving a
     * torn tail that breaks decode -- so reject the whole record rather than
     * journal it. 256 B > the nesting reserves (10 B x depth 5) + one entry, so
     * that floor catches any earlier overflow (free space only shrinks). */
    {
        NvU32 bufLeftAtEnd = prbEncBufLeft(&hangDebugDumpEnc);
        if (bufLeftAtEnd < 256)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "hang debug dump exhausted the %u B capture buffer; "
                      "dropping possibly-truncated record\n", bufSize);
            return NV_ERR_BUFFER_TOO_SMALL;
        }
        if (bufLeftAtEnd < (bufSize >> 4))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "hang debug dump near buffer cap: %u B left of %u B; "
                      "consider raising the capture buffer size\n",
                      bufLeftAtEnd, bufSize);
        }
    }

    *pEncLen = prbEncFinish(&hangDebugDumpEnc, &pFinished);
    if (pFinished == NULL || *pEncLen == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "prbEncFinish returned empty buffer\n");
        *pEncLen = 0;
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

NV_STATUS
hangDebugDumpCaptureAlloc
(
    OBJGPU                        *pGpu,
    const HANG_DEBUG_TRIGGER_INFO *pTrigger,
    NvU8                         **ppBuf,
    NvU32                          bufSize,
    NvU32                         *pEncLen
)
{
    NvU8     *pBuf;
    NV_STATUS status;

    if (ppBuf != NULL)
        *ppBuf = NULL;

    if (pEncLen != NULL)
        *pEncLen = 0;

    if (pGpu == NULL || ppBuf == NULL || pEncLen == NULL || bufSize == 0)
        return NV_ERR_INVALID_ARGUMENT;

    if (!hangDebugDumpIsCaptureAllowed(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    pBuf = portMemAllocNonPaged(bufSize);
    if (pBuf == NULL)
        return NV_ERR_NO_MEMORY;

    status = _hangDebugDumpEncodeToBuffer(pGpu, pTrigger, pBuf, bufSize, pEncLen);
    if (status != NV_OK)
    {
        portMemFree(pBuf);
        return status;
    }

    *ppBuf = pBuf;
    return NV_OK;
}

/* Kernel wrapper; see header for trigger + responsibilities. */
void
hangDebugDumpCapture(OBJGPU *pGpu, const HANG_DEBUG_TRIGGER_INFO *pTrigger)
{
    #define HANG_DEBUG_DUMP_BUF_SIZE    (4 * 1024 * 1024)

    NvU8     *pBuf = NULL;
    NvU32     encLen = 0;
    NV_STATUS status;

    status = hangDebugDumpCaptureAlloc(pGpu, pTrigger, &pBuf,
                                       HANG_DEBUG_DUMP_BUF_SIZE, &encLen);
    if (status != NV_OK)
    {
        if (status != NV_ERR_NOT_SUPPORTED)
            NV_PRINTF(LEVEL_ERROR, "hang debug dump capture failed: 0x%x\n", status);
        return;
    }

    /* Shrink to fit: the journal holds the buffer for the driver's life and
     * records run 3-24x smaller than the capture buffer. On alloc failure keep
     * the oversized buffer -- a fat record beats none on the fatal path. */
    {
        NvU8 *pFit = portMemAllocNonPaged(encLen);
        if (pFit != NULL)
        {
            portMemCopy(pFit, encLen, pBuf, encLen);
            portMemFree(pBuf);
            pBuf = pFit;
        }
    }

    NV_PRINTF(LEVEL_NOTICE, "encoded %u bytes, storing in journal\n", encLen);

    /* Takes ownership of pBuf on NV_OK; on error pBuf is still ours to free. */
    status = rcdbAddProtobufDump(pGpu, pBuf, encLen);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rcdbAddProtobufDump failed: 0x%x\n", status);
        portMemFree(pBuf);
    }

    #undef HANG_DEBUG_DUMP_BUF_SIZE
}
