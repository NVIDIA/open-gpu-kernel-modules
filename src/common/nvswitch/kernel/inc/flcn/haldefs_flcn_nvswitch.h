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

#ifndef _HALDEFS_FLCN_NVSWITCH_H_
#define _HALDEFS_FLCN_NVSWITCH_H_


#include "nvstatus.h"
#include "flcn/flcnqueue_nvswitch.h"
#include "flcnifcmn.h"

struct nvswitch_device;
struct NVSWITCH_TIMEOUT;
struct FLCN;
union  RM_FLCN_MSG;
union  RM_FLCN_CMD;
struct FLCNQUEUE;
struct FLCN_QMGR_SEQ_INFO;

typedef struct {
    // OBJECT Interfaces
    NV_STATUS   (*queueReadData)                    (struct nvswitch_device *, struct FLCN *, NvU32 queueId, void *pData, NvBool bMsg);
    NV_STATUS   (*queueCmdWrite)                    (struct nvswitch_device *, struct FLCN *, NvU32 queueId, union RM_FLCN_CMD *pCmd, struct NVSWITCH_TIMEOUT *pTimeout);
    NV_STATUS   (*queueCmdCancel)                   (struct nvswitch_device *, struct FLCN *, NvU32 seqDesc);
    NV_STATUS   (*queueCmdPostNonBlocking)          (struct nvswitch_device *, struct FLCN *, union RM_FLCN_CMD *pCmd, union RM_FLCN_MSG *pMsg, void *pPayload, NvU32 queueIdLogical, FlcnQMgrClientCallback pCallback, void *pCallbackParams, NvU32 *pSeqDesc, struct NVSWITCH_TIMEOUT *pTimeout);
    NV_STATUS   (*queueCmdWait)                     (struct nvswitch_device *, struct FLCN *, NvU32 seqDesc, struct NVSWITCH_TIMEOUT *pTimeout);
    NvU8        (*coreRevisionGet)                  (struct nvswitch_device *, struct FLCN *);
    void        (*markNotReady)                     (struct nvswitch_device *, struct FLCN *);
    NV_STATUS   (*cmdQueueHeadGet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 *pHead);
    NV_STATUS   (*msgQueueHeadGet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 *pHead);
    NV_STATUS   (*cmdQueueTailGet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 *pTail);
    NV_STATUS   (*msgQueueTailGet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 *pTail);
    NV_STATUS   (*cmdQueueHeadSet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 head);
    NV_STATUS   (*msgQueueHeadSet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 head);
    NV_STATUS   (*cmdQueueTailSet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 tail);
    NV_STATUS   (*msgQueueTailSet)                  (struct nvswitch_device *, struct FLCN *, struct FLCNQUEUE *pQueue, NvU32 tail);
    struct FLCN_QMGR_SEQ_INFO *(*queueSeqInfoFind)  (struct nvswitch_device *, struct FLCN *, NvU32 seqDesc);
    struct FLCN_QMGR_SEQ_INFO *(*queueSeqInfoAcq)   (struct nvswitch_device *, struct FLCN *);
    void        (*queueSeqInfoRel)                  (struct nvswitch_device *, struct FLCN *, struct FLCN_QMGR_SEQ_INFO *pSeqInfo);
    void        (*queueSeqInfoStateInit)            (struct nvswitch_device *, struct FLCN *);
    void        (*queueSeqInfoCancelAll)            (struct nvswitch_device *, struct FLCN *);
    NV_STATUS   (*queueSeqInfoFree)                 (struct nvswitch_device *, struct FLCN *, struct FLCN_QMGR_SEQ_INFO *);
    NV_STATUS   (*queueEventRegister)               (struct nvswitch_device *, struct FLCN *, NvU32 unitId, NvU8 *pMsg, FlcnQMgrClientCallback pCallback, void *pParams, NvU32 *pEvtDesc);
    NV_STATUS   (*queueEventUnregister)             (struct nvswitch_device *, struct FLCN *, NvU32 evtDesc);
    NV_STATUS   (*queueEventHandle)                 (struct nvswitch_device *, struct FLCN *, union RM_FLCN_MSG *pMsg, NV_STATUS evtStatus);
    NV_STATUS   (*queueResponseHandle)              (struct nvswitch_device *, struct FLCN *, union RM_FLCN_MSG *pMsg);
    NvU32       (*queueCmdStatus)                   (struct nvswitch_device *, struct FLCN *, NvU32 seqDesc);
    NV_STATUS   (*dmemCopyFrom)                     (struct nvswitch_device *, struct FLCN *, NvU32 src, NvU8 *pDst, NvU32 sizeBytes, NvU8 port);
    NV_STATUS   (*dmemCopyTo)                       (struct nvswitch_device *, struct FLCN *, NvU32 dst, NvU8 *pSrc, NvU32 sizeBytes, NvU8 port);
    void        (*postDiscoveryInit)                (struct nvswitch_device *, struct FLCN *);
    void        (*dbgInfoDmemOffsetSet)             (struct nvswitch_device *, struct FLCN *, NvU16 debugInfoDmemOffset);


    //HAL Interfaces
    NV_STATUS   (*construct)                        (struct nvswitch_device *, struct FLCN *);
    void        (*destruct)                         (struct nvswitch_device *, struct FLCN *);
    NvU32       (*regRead)                          (struct nvswitch_device *, struct FLCN *, NvU32 offset);
    void        (*regWrite)                         (struct nvswitch_device *, struct FLCN *, NvU32 offset, NvU32 data);
    const char *(*getName)                          (struct nvswitch_device *, struct FLCN *);
    NvU8        (*readCoreRev)                      (struct nvswitch_device *, struct FLCN *);
    void        (*getCoreInfo)                      (struct nvswitch_device *, struct FLCN *);
    NV_STATUS   (*dmemTransfer)                     (struct nvswitch_device *, struct FLCN *, NvU32 src, NvU8 *pDst, NvU32 sizeBytes, NvU8 port, NvBool bCopyFrom);
    void        (*intrRetrigger)                    (struct nvswitch_device *, struct FLCN *);
    NvBool      (*areEngDescsInitialized)           (struct nvswitch_device *, struct FLCN *);
    NV_STATUS   (*waitForResetToFinish)             (struct nvswitch_device *, struct FLCN *);
    void        (*dbgInfoCapturePcTrace)            (struct nvswitch_device *, struct FLCN *);
    void        (*dbgInfoCaptureRiscvPcTrace)       (struct nvswitch_device *, struct FLCN *);
    NvU32       (*dmemSize)                         (struct nvswitch_device *, struct FLCN *);
    NvU32       (*setImemAddr)                      (struct nvswitch_device *, struct FLCN *, NvU32 dst);
    void        (*imemCopyTo)                       (struct nvswitch_device *, struct FLCN *, NvU32 dst, NvU8 *pSrc, NvU32 sizeBytes, NvBool bSecure, NvU32 tag, NvU8 port);
    NvU32       (*setDmemAddr)                      (struct nvswitch_device *, struct FLCN *, NvU32 dst);
    NvU32       (*riscvRegRead)                     (struct nvswitch_device *, struct FLCN *, NvU32 offset);
    void        (*riscvRegWrite)                    (struct nvswitch_device *, struct FLCN *, NvU32 offset, NvU32 data);
    NV_STATUS   (*debugBufferInit)                  (struct nvswitch_device *, struct FLCN *, NvU32 debugBufferMaxSize, NvU32 writeRegAddr, NvU32 readRegAddr);
    NV_STATUS   (*debugBufferDestroy)               (struct nvswitch_device *, struct FLCN *);
    NV_STATUS   (*debugBufferDisplay)               (struct nvswitch_device *, struct FLCN *);
    NvBool      (*debugBufferIsEmpty)               (struct nvswitch_device *, struct FLCN *);
} flcn_hal;

void flcnQueueSetupHal(struct FLCN *pFlcn);
void flcnRtosSetupHal(struct FLCN *pFlcn);
void flcnQueueRdSetupHal(struct FLCN *pFlcn);

void flcnSetupHal_LS10(struct FLCN *pFlcn);
void flcnSetupHal_LR10(struct FLCN *pFlcn);

void flcnSetupHal_v03_00(struct FLCN *pFlcn);
void flcnSetupHal_v04_00(struct FLCN *pFlcn);
void flcnSetupHal_v05_01(struct FLCN *pFlcn);
void flcnSetupHal_v06_00(struct FLCN *pFlcn);

#endif //_HALDEFS_FLCN_NVSWITCH_H_
