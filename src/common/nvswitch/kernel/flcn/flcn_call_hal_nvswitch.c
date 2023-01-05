/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "flcn/haldefs_flcn_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

#include "flcnifcmn.h"

typedef union  RM_FLCN_CMD RM_FLCN_CMD, *PRM_FLCN_CMD;
typedef union  RM_FLCN_MSG RM_FLCN_MSG, *PRM_FLCN_MSG;

// OBJECT Interfaces
NV_STATUS
flcnQueueReadData
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            queueId,
    void            *pData,
    NvBool           bMsg
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueReadData != (void *)0);
    return pFlcn->pHal->queueReadData(device, pFlcn, queueId, pData, bMsg);
}

NV_STATUS
flcnQueueCmdWrite
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            queueId,
    RM_FLCN_CMD     *pCmd,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueCmdWrite != (void *)0);
    return pFlcn->pHal->queueCmdWrite(device, pFlcn, queueId, pCmd, pTimeout);
}

NV_STATUS
flcnQueueCmdCancel
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueCmdCancel != (void *)0);
    return pFlcn->pHal->queueCmdCancel(device, pFlcn, seqDesc);
}

NV_STATUS
flcnQueueCmdPostNonBlocking
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PRM_FLCN_CMD     pCmd,
    PRM_FLCN_MSG     pMsg,
    void            *pPayload,
    NvU32            queueIdLogical,
    FlcnQMgrClientCallback pCallback,
    void            *pCallbackParams,
    NvU32           *pSeqDesc,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueCmdPostNonBlocking != (void *)0);
    return pFlcn->pHal->queueCmdPostNonBlocking(device, pFlcn, pCmd, pMsg, pPayload, queueIdLogical, pCallback, pCallbackParams, pSeqDesc, pTimeout);
}

NV_STATUS
flcnQueueCmdPostBlocking
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PRM_FLCN_CMD     pCmd,
    PRM_FLCN_MSG     pMsg,
    void            *pPayload,
    NvU32            queueIdLogical,
    NvU32           *pSeqDesc,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NV_STATUS status;

    status = flcnQueueCmdPostNonBlocking(device, pFlcn, pCmd, pMsg, pPayload,
                    queueIdLogical, NULL, NULL, pSeqDesc, pTimeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_COMMAND_QUEUE,
            "Fatal, Failed to post command to SOE. Data {0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
            pCmd->cmdGen.hdr.unitId, pCmd->cmdGen.hdr.size, pCmd->cmdGen.hdr.ctrlFlags,
            pCmd->cmdGen.hdr.seqNumId, pCmd->cmdGen.cmd, (NvU8)pCmd->cmdGen.cmd);

        return status;
    }

    status = flcnQueueCmdWait(device, pFlcn, *pSeqDesc, pTimeout);
    if (status == NV_ERR_TIMEOUT)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_TIMEOUT,
            "Fatal, Timed out while waiting for SOE command completion. Data {0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
            pCmd->cmdGen.hdr.unitId, pCmd->cmdGen.hdr.size, pCmd->cmdGen.hdr.ctrlFlags,
            pCmd->cmdGen.hdr.seqNumId, pCmd->cmdGen.cmd, (NvU8)pCmd->cmdGen.cmd);
        flcnQueueCmdCancel(device, pFlcn, *pSeqDesc);
    }

    return status;
}

NV_STATUS
flcnQueueCmdWait
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueCmdWait != (void *)0);

    return pFlcn->pHal->queueCmdWait(device, pFlcn, seqDesc, pTimeout);
}

NvU8
flcnCoreRevisionGet
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->coreRevisionGet != (void *)0);
    return pFlcn->pHal->coreRevisionGet(device, pFlcn);
}

void
flcnMarkNotReady
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->markNotReady != (void *)0);
    pFlcn->pHal->markNotReady(device, pFlcn);
}

NV_STATUS
flcnCmdQueueHeadGet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pHead
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->cmdQueueHeadGet != (void *)0);
    return pFlcn->pHal->cmdQueueHeadGet(device, pFlcn, pQueue, pHead);
}

NV_STATUS
flcnMsgQueueHeadGet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pHead
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->msgQueueHeadGet != (void *)0);
    return pFlcn->pHal->msgQueueHeadGet(device, pFlcn, pQueue, pHead);
}

NV_STATUS
flcnCmdQueueTailGet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pTail
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->cmdQueueTailGet != (void *)0);
    return pFlcn->pHal->cmdQueueTailGet(device, pFlcn, pQueue, pTail);
}

NV_STATUS
flcnMsgQueueTailGet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pTail
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->msgQueueTailGet != (void *)0);
    return pFlcn->pHal->msgQueueTailGet(device, pFlcn, pQueue, pTail);
}

NV_STATUS
flcnCmdQueueHeadSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            head
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->cmdQueueHeadSet != (void *)0);
    return pFlcn->pHal->cmdQueueHeadSet(device, pFlcn, pQueue, head);
}

NV_STATUS
flcnMsgQueueHeadSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            head
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->msgQueueHeadSet != (void *)0);
    return pFlcn->pHal->msgQueueHeadSet(device, pFlcn, pQueue, head);
}

NV_STATUS
flcnCmdQueueTailSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            tail
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->cmdQueueTailSet != (void *)0);
    return pFlcn->pHal->cmdQueueTailSet(device, pFlcn, pQueue, tail);
}

NV_STATUS
flcnMsgQueueTailSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            tail
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->msgQueueTailSet != (void *)0);
    return pFlcn->pHal->msgQueueTailSet(device, pFlcn, pQueue, tail);
}

PFLCN_QMGR_SEQ_INFO
flcnQueueSeqInfoFind
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoFind != (void *)0);
    return pFlcn->pHal->queueSeqInfoFind(device, pFlcn, seqDesc);
}

PFLCN_QMGR_SEQ_INFO
flcnQueueSeqInfoAcq
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoAcq != (void *)0);
    return pFlcn->pHal->queueSeqInfoAcq(device, pFlcn);
}

void
flcnQueueSeqInfoRel
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoRel != (void *)0);
    pFlcn->pHal->queueSeqInfoRel(device, pFlcn, pSeqInfo);
}

void
flcnQueueSeqInfoStateInit
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoStateInit != (void *)0);
    pFlcn->pHal->queueSeqInfoStateInit(device, pFlcn);
}

void
flcnQueueSeqInfoCancelAll
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoCancelAll != (void *)0);
    pFlcn->pHal->queueSeqInfoCancelAll(device, pFlcn);
}

NV_STATUS
flcnQueueSeqInfoFree
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueSeqInfoFree != (void *)0);
    return pFlcn->pHal->queueSeqInfoFree(device, pFlcn, pSeqInfo);
}

NV_STATUS
flcnQueueEventRegister
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            unitId,
    NvU8            *pMsg,
    FlcnQMgrClientCallback pCallback,
    void            *pParams,
    NvU32           *pEvtDesc
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueEventRegister != (void *)0);
    return pFlcn->pHal->queueEventRegister(device, pFlcn, unitId, pMsg, pCallback, pParams, pEvtDesc);
}

NV_STATUS
flcnQueueEventUnregister
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            evtDesc
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueEventUnregister != (void *)0);
    return pFlcn->pHal->queueEventUnregister(device, pFlcn, evtDesc);
}

NV_STATUS
flcnQueueEventHandle
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    RM_FLCN_MSG     *pMsg,
    NV_STATUS        evtStatus
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueEventHandle != (void *)0);
    return pFlcn->pHal->queueEventHandle(device, pFlcn, pMsg, evtStatus);
}

NV_STATUS
flcnQueueResponseHandle
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    RM_FLCN_MSG     *pMsg
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueResponseHandle != (void *)0);
    return pFlcn->pHal->queueResponseHandle(device, pFlcn, pMsg);
}

NvU32
flcnQueueCmdStatus
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->queueCmdStatus != (void *)0);
    return pFlcn->pHal->queueCmdStatus(device, pFlcn, seqDesc);
}

NV_STATUS
flcnDmemCopyFrom
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            src,
    NvU8            *pDst,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->dmemCopyFrom != (void *)0);
    return pFlcn->pHal->dmemCopyFrom(device, pFlcn, src, pDst, sizeBytes, port);
}

NV_STATUS
flcnDmemCopyTo
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->dmemCopyTo != (void *)0);
    return pFlcn->pHal->dmemCopyTo(device, pFlcn, dst, pSrc, sizeBytes, port);
}

void
flcnPostDiscoveryInit
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->postDiscoveryInit != (void *)0);
    pFlcn->pHal->postDiscoveryInit(device, pFlcn);
}

void
flcnDbgInfoDmemOffsetSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU16            debugInfoDmemOffset
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->dbgInfoDmemOffsetSet != (void *)0);
    pFlcn->pHal->dbgInfoDmemOffsetSet(device, pFlcn, debugInfoDmemOffset);
}



// HAL Interfaces
NV_STATUS
flcnConstruct_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->construct != (void *)0);
    return pFlcn->pHal->construct(device, pFlcn);
}

void
flcnDestruct_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->destruct != (void *)0);
    pFlcn->pHal->destruct(device, pFlcn);
}

NvU32
flcnRegRead_HAL
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn,
    NvU32                   offset
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->regRead != (void *)0);
    return pFlcn->pHal->regRead(device, pFlcn, offset);
}

void
flcnRegWrite_HAL
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn,
    NvU32                   offset,
    NvU32                   data
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->regWrite != (void *)0);
    pFlcn->pHal->regWrite(device, pFlcn, offset, data);
}

const char *
flcnGetName_HAL
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->getName != (void *)0);
    return pFlcn->pHal->getName(device, pFlcn);
}

NvU8
flcnReadCoreRev_HAL
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->readCoreRev != (void *)0);
    return pFlcn->pHal->readCoreRev(device, pFlcn);
}

void
flcnGetCoreInfo_HAL
(
    struct nvswitch_device *device,
    PFLCN                   pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->getCoreInfo != (void *)0);
    pFlcn->pHal->getCoreInfo(device, pFlcn);
}

NV_STATUS
flcnDmemTransfer_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            src,
    NvU8            *pDst,
    NvU32            sizeBytes,
    NvU8             port,
    NvBool           bCopyFrom
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->dmemTransfer != (void *)0);
    return pFlcn->pHal->dmemTransfer(device, pFlcn, src, pDst, sizeBytes, port, bCopyFrom);
}

void
flcnIntrRetrigger_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->intrRetrigger != (void *)0);
    pFlcn->pHal->intrRetrigger(device, pFlcn);
}

NvBool
flcnAreEngDescsInitialized_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->areEngDescsInitialized != (void *)0);
    return pFlcn->pHal->areEngDescsInitialized(device, pFlcn);
}

NV_STATUS
flcnWaitForResetToFinish_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->waitForResetToFinish != (void *)0);
    return pFlcn->pHal->waitForResetToFinish(device, pFlcn);
}

void
flcnDbgInfoCapturePcTrace_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    if (pFlcn->pHal->dbgInfoCapturePcTrace == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pFlcn->pHal->dbgInfoCapturePcTrace(device, pFlcn);
}

void
flcnDbgInfoCaptureRiscvPcTrace_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    if (pFlcn->pHal->dbgInfoCaptureRiscvPcTrace == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pFlcn->pHal->dbgInfoCaptureRiscvPcTrace(device, pFlcn);
}


NvU32
flcnDmemSize_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    if (pFlcn->pHal->dmemSize == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pFlcn->pHal->dmemSize(device, pFlcn);    
}

NvU32
flcnSetImemAddr_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst
)
{
    if (pFlcn->pHal->setImemAddr == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pFlcn->pHal->setImemAddr(device, pFlcn, dst);    
}

void
flcnImemCopyTo_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvBool           bSecure,
    NvU32            tag,
    NvU8             port
)
{
    if (pFlcn->pHal->imemCopyTo == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pFlcn->pHal->imemCopyTo(device, pFlcn, dst, pSrc, sizeBytes, bSecure, tag, port);    
}

NvU32
flcnSetDmemAddr_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst
)
{
    if (pFlcn->pHal->setDmemAddr == (void *)0)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pFlcn->pHal->setDmemAddr(device, pFlcn, dst);    
}

NvU32
flcnRiscvRegRead_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            offset
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->riscvRegRead != (void *)0);
    return pFlcn->pHal->riscvRegRead(device, pFlcn, offset);
}

void
flcnRiscvRegWrite_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            offset,
    NvU32            data
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->riscvRegWrite != (void *)0);
    pFlcn->pHal->riscvRegWrite(device, pFlcn, offset, data);
}

NV_STATUS
flcnDebugBufferInit_HAL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            debugBufferMaxSize,
    NvU32            writeRegAddr,
    NvU32            readRegAddr
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->debugBufferInit != (void *)0);
    return pFlcn->pHal->debugBufferInit(device, pFlcn, debugBufferMaxSize, writeRegAddr, readRegAddr);
}

NV_STATUS
flcnDebugBufferDestroy_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->debugBufferDestroy != (void *)0);
    return pFlcn->pHal->debugBufferDestroy(device, pFlcn);
}

NV_STATUS
flcnDebugBufferDisplay_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->debugBufferDisplay != (void *)0);
    return pFlcn->pHal->debugBufferDisplay(device, pFlcn);
}

NvBool
flcnDebugBufferIsEmpty_HAL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NVSWITCH_ASSERT(pFlcn->pHal->debugBufferIsEmpty != (void *)0);
    return pFlcn->pHal->debugBufferIsEmpty(device, pFlcn);
}

