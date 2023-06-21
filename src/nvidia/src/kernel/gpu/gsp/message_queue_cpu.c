/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * GSP MESSAGE QUEUE - CPU-SIDE CODE
 */

#include "core/core.h"
#include "core/thread_state.h"


#include "os/os.h"

#include "vgpu/rpc_headers.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

#define RPC_MESSAGE_STRUCTURES
#define RPC_MESSAGE_GENERIC_UNION
#include "g_rpc-message-header.h"
#undef RPC_MESSAGE_STRUCTURES
#undef RPC_MESSAGE_GENERIC_UNION

#include "gpu/gsp/message_queue.h"
#include "gpu/gsp/message_queue_priv.h"
#include "msgq/msgq_priv.h"
#include "gpu/gsp/kernel_gsp.h"
#include "nvrm_registry.h"

ct_assert(GSP_MSG_QUEUE_HEADER_SIZE > sizeof(msgqTxHeader) + sizeof(msgqRxHeader));

static void _gspMsgQueueCleanup(MESSAGE_QUEUE_INFO *pMQI);

static void
_getMsgQueueParams
(
    OBJGPU *pGpu,
    MESSAGE_QUEUE_COLLECTION *pMQCollection
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NvLength queueSize;
    MESSAGE_QUEUE_INFO *pRmQueueInfo = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];
    MESSAGE_QUEUE_INFO *pTaskIsrQueueInfo = &pMQCollection->rpcQueues[RPC_TASK_ISR_QUEUE_IDX];
    NvU32 numPtes;
    const NvLength defaultCommandQueueSize = 0x40000; // 256 KB
    const NvLength defaultStatusQueueSize  = 0x40000; // 256 KB
    NvU32 regStatusQueueSize;

    // RmQueue sizes
    if (IS_SILICON(pGpu))
    {
        pRmQueueInfo->commandQueueSize = defaultCommandQueueSize;
    }
    else
    {
        //
        // Pre-silicon platforms need a large command queue in order to send
        // the VBIOS image via RPC.
        //
        pRmQueueInfo->commandQueueSize = defaultCommandQueueSize * 6;
    }

    // Check for status queue size overried
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_STATUS_QUEUE_SIZE, &regStatusQueueSize) == NV_OK)
    {
        regStatusQueueSize *= 1024; // to bytes
        regStatusQueueSize = NV_MAX(GSP_MSG_QUEUE_ELEMENT_SIZE_MAX, regStatusQueueSize);
        regStatusQueueSize = NV_ALIGN_UP(regStatusQueueSize, 1 << GSP_MSG_QUEUE_ALIGN);
        pRmQueueInfo->statusQueueSize = regStatusQueueSize;
    }
    else
    {
        pRmQueueInfo->statusQueueSize = defaultStatusQueueSize;
    }

    // TaskIsrQueue sizes
    if (pKernelGsp->bIsTaskIsrQueueRequired)
    {
        pTaskIsrQueueInfo->commandQueueSize = defaultCommandQueueSize;
        pTaskIsrQueueInfo->statusQueueSize = defaultStatusQueueSize;
    }
    else
    {
        pTaskIsrQueueInfo->commandQueueSize = 0;
        pTaskIsrQueueInfo->statusQueueSize = 0;
    }

    //
    // Calculate the number of entries required to map both queues in addition
    // to the page table itself.
    //
    queueSize = pRmQueueInfo->commandQueueSize      + pRmQueueInfo->statusQueueSize +
                pTaskIsrQueueInfo->commandQueueSize + pTaskIsrQueueInfo->statusQueueSize;
    NV_ASSERT((queueSize & RM_PAGE_MASK) == 0);
    numPtes = (queueSize >> RM_PAGE_SHIFT);

    // Account for the pages needed to store the PTEs
    numPtes += NV_DIV_AND_CEIL(numPtes * sizeof(RmPhysAddr), RM_PAGE_SIZE);

    //
    // Align the page table size to RM_PAGE_SIZE, so that the command queue is
    // aligned.
    //
    pMQCollection->pageTableSize = RM_PAGE_ALIGN_UP(numPtes * sizeof(RmPhysAddr));
    pMQCollection->pageTableEntryCount = numPtes;
}

static NV_STATUS
_gspMsgQueueInit
(
    MESSAGE_QUEUE_INFO *pMQI
)
{
    NvU32 workAreaSize;
    NV_STATUS nvStatus = NV_OK;
    int nRet;

    // Allocate work area.
    workAreaSize = (1 << GSP_MSG_QUEUE_ELEMENT_ALIGN) +
                   GSP_MSG_QUEUE_ELEMENT_SIZE_MAX + msgqGetMetaSize();
    pMQI->pWorkArea = portMemAllocNonPaged(workAreaSize);
    if (pMQI->pWorkArea == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating pWorkArea.\n");
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pMQI->pWorkArea, 0, workAreaSize);

    pMQI->pCmdQueueElement = (GSP_MSG_QUEUE_ELEMENT *)
        NV_ALIGN_UP((NvUPtr)pMQI->pWorkArea, 1 << GSP_MSG_QUEUE_ELEMENT_ALIGN);
    pMQI->pMetaData = (void *)((NvUPtr)pMQI->pCmdQueueElement + GSP_MSG_QUEUE_ELEMENT_SIZE_MAX);

    nRet = msgqInit(&pMQI->hQueue, pMQI->pMetaData);
    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqInit failed: %d\n", nRet);
        nvStatus = NV_ERR_GENERIC;
        goto error_ret;
    }

    nRet = msgqTxCreate(pMQI->hQueue,
                pMQI->pCommandQueue,
                pMQI->commandQueueSize,
                GSP_MSG_QUEUE_ELEMENT_SIZE_MIN,
                GSP_MSG_QUEUE_HEADER_ALIGN,
                GSP_MSG_QUEUE_ELEMENT_ALIGN,
                MSGQ_FLAGS_SWAP_RX);
    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqTxCreate failed: %d\n", nRet);
        nvStatus = NV_ERR_GENERIC;
        goto error_ret;
    }

    pMQI->pRpcMsgBuf   = &pMQI->pCmdQueueElement->rpc;

    NV_PRINTF(LEVEL_INFO, "Created command queue.\n");
    return nvStatus;

error_ret:
    _gspMsgQueueCleanup(pMQI);
    return nvStatus;
}

/*!
 * GspMsgQueueInit
 *
 * Initialize the command queues for CPU side.
 * Must not be called before portInitialize.
 */
NV_STATUS
GspMsgQueuesInit
(
    OBJGPU                    *pGpu,
    MESSAGE_QUEUE_COLLECTION **ppMQCollection
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    MESSAGE_QUEUE_COLLECTION *pMQCollection = NULL;
    MESSAGE_QUEUE_INFO  *pRmQueueInfo = NULL;
    MESSAGE_QUEUE_INFO  *pTaskIsrQueueInfo = NULL;
    RmPhysAddr  *pPageTbl;
    NvP64        pVaKernel;
    NvP64        pPrivKernel;
    NV_STATUS    nvStatus         = NV_OK;
    NvLength     sharedBufSize;
    NvP64        lastQueueVa;
    NvLength     lastQueueSize;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    if (*ppMQCollection != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP message queue was already initialized.\n");
        return NV_ERR_INVALID_STATE;
    }

    pMQCollection = portMemAllocNonPaged(sizeof *pMQCollection);
    if (pMQCollection == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating queue info area.\n");
        nvStatus = NV_ERR_NO_MEMORY;
        goto done;
    }
    portMemSet(pMQCollection, 0, sizeof *pMQCollection);

    _getMsgQueueParams(pGpu, pMQCollection);

    pRmQueueInfo      = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];
    pTaskIsrQueueInfo = &pMQCollection->rpcQueues[RPC_TASK_ISR_QUEUE_IDX];

    sharedBufSize = pMQCollection->pageTableSize +
                    pRmQueueInfo->commandQueueSize +
                    pRmQueueInfo->statusQueueSize +
                    pTaskIsrQueueInfo->commandQueueSize +
                    pTaskIsrQueueInfo->statusQueueSize;

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    //
    // For now, put all shared queue memory in one block.
    //
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pMQCollection->pSharedMemDesc, pGpu, sharedBufSize,
            RM_PAGE_SIZE, NV_MEMORY_NONCONTIGUOUS, ADDR_SYSMEM, NV_MEMORY_CACHED,
            flags),
        done);

    memdescSetFlag(pMQCollection->pSharedMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_TRUE);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescAlloc(pMQCollection->pSharedMemDesc),
        error_ret);

    // Create kernel mapping for command queue.
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pMQCollection->pSharedMemDesc, 0, sharedBufSize,
            NV_TRUE, NV_PROTECT_WRITEABLE,
            &pVaKernel, &pPrivKernel),
        error_ret);

    memdescSetKernelMapping(pMQCollection->pSharedMemDesc, pVaKernel);
    memdescSetKernelMappingPriv(pMQCollection->pSharedMemDesc, pPrivKernel);

    if (pVaKernel == NvP64_NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating message queue shared buffer\n");
        nvStatus = NV_ERR_NO_MEMORY;
        goto error_ret;
    }

    portMemSet((void *)pVaKernel, 0, sharedBufSize);

    pPageTbl = pVaKernel;

    // Shared memory layout.
    //
    // Each of the following are page aligned:
    //   Shared memory layout header (includes page table)
    //   RM Command queue header
    //   RM Command queue entries
    //   RM Status queue header
    //   RM Status queue entries
    //   TASKISR Command queue header
    //   TASKISR Command queue entries
    //   TASKISR Status queue header
    //   TASKISR Status queue entries
    memdescGetPhysAddrs(pMQCollection->pSharedMemDesc,
                    AT_GPU,                     // addressTranslation
                    0,                          // offset
                    RM_PAGE_SIZE,               // stride
                    pMQCollection->pageTableEntryCount,  // count
                    pPageTbl);                  // physical address table

    pRmQueueInfo->pCommandQueue = NvP64_VALUE(
        NvP64_PLUS_OFFSET(pVaKernel, pMQCollection->pageTableSize));

    pRmQueueInfo->pStatusQueue  = NvP64_VALUE(
        NvP64_PLUS_OFFSET(NV_PTR_TO_NvP64(pRmQueueInfo->pCommandQueue), pRmQueueInfo->commandQueueSize));

    lastQueueVa   = NV_PTR_TO_NvP64(pRmQueueInfo->pStatusQueue);
    lastQueueSize = pRmQueueInfo->statusQueueSize;

    if (pKernelGsp->bIsTaskIsrQueueRequired)
    {
        pTaskIsrQueueInfo->pCommandQueue = NvP64_VALUE(
            NvP64_PLUS_OFFSET(NV_PTR_TO_NvP64(pRmQueueInfo->pStatusQueue), pRmQueueInfo->statusQueueSize));

        pTaskIsrQueueInfo->pStatusQueue  = NvP64_VALUE(
            NvP64_PLUS_OFFSET(NV_PTR_TO_NvP64(pTaskIsrQueueInfo->pCommandQueue), pTaskIsrQueueInfo->commandQueueSize));

        lastQueueVa   = NV_PTR_TO_NvP64(pTaskIsrQueueInfo->pStatusQueue);
        lastQueueSize = pTaskIsrQueueInfo->statusQueueSize;
    }

    // Assert that the last queue offset + size fits into the shared memory.
    NV_ASSERT(NvP64_PLUS_OFFSET(pVaKernel, sharedBufSize) ==
              NvP64_PLUS_OFFSET(lastQueueVa, lastQueueSize));

    NV_ASSERT_OK_OR_GOTO(nvStatus, _gspMsgQueueInit(pRmQueueInfo), error_ret);
    pRmQueueInfo->queueIdx = RPC_TASK_RM_QUEUE_IDX;

    if (pKernelGsp->bIsTaskIsrQueueRequired)
    {
        NV_ASSERT_OK_OR_GOTO(nvStatus, _gspMsgQueueInit(pTaskIsrQueueInfo), error_ret);
        pTaskIsrQueueInfo->queueIdx = RPC_TASK_ISR_QUEUE_IDX;
    }

    *ppMQCollection             = pMQCollection;
    pMQCollection->sharedMemPA  = pPageTbl[0];

done:
    return nvStatus;

error_ret:
    GspMsgQueuesCleanup(&pMQCollection);
    return nvStatus;
}

NV_STATUS GspStatusQueueInit(OBJGPU *pGpu, MESSAGE_QUEUE_INFO **ppMQI)
{
    NV_STATUS  nvStatus = NV_ERR_GENERIC;
    int        nRet = 0;
    int        nRetries;
    RMTIMEOUT  timeout;
    NvU32      timeoutUs = 4000000;
    NvU32      timeoutFlags = GPU_TIMEOUT_FLAGS_DEFAULT;
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    // GSP-RM in emulation/simulation environment is extremely slow
    if (IS_EMULATION(pGpu) || IS_SIMULATION(pGpu))
    {
        //
        // Scaling timeoutUs by GSP_SCALE_TIMEOUT_EMU_SIM overflows 32 bits,
        // so just max it out instead.
        //
        timeoutUs = NV_U32_MAX;

        //
        // On slower emulators and simulation, the time it takes to link the
        // status queue is longer than the thread state timeout, so bypass
        // the thread state so our longer timeout applies.
        //
        timeoutFlags |= GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE;
    }

    gpuSetTimeout(pGpu, timeoutUs, &timeout, timeoutFlags);

    // Wait other end of the queue to run msgqInit.  Retry until the timeout.
    for (nRetries = 0; ; nRetries++)
    {
        // Link in status queue
        portAtomicMemoryFenceFull();

        nRet = msgqRxLink((*ppMQI)->hQueue, (*ppMQI)->pStatusQueue,
            (*ppMQI)->statusQueueSize, GSP_MSG_QUEUE_ELEMENT_SIZE_MIN);

        if (nRet == 0)
        {
            NV_PRINTF(LEVEL_INFO, "Status queue linked to command queue.\n");

            //
            // If we've bypassed the thread state timeout check for slower
            // environments, it will have lapsed by now, so reset it so that
            // the next timeout check doesn't fail immediately.
            //
            if (timeoutFlags & GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE)
                threadStateResetTimeout(pGpu);

            return NV_OK;
        }

        osSpinLoop();

        nvStatus = gpuCheckTimeout(pGpu, &timeout);
        if (nvStatus != NV_OK)
            break;

        kgspDumpGspLogs(pGpu, pKernelGsp, NV_FALSE);
    }

    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR,
            "msgqRxLink failed: %d, nvStatus 0x%08x, retries: %d\n",
            nRet, nvStatus, nRetries);
        _gspMsgQueueCleanup(*ppMQI);
    }

    return nvStatus;
}

static void
_gspMsgQueueCleanup(MESSAGE_QUEUE_INFO *pMQI)
{
    if (pMQI == NULL)
    {
        return;
    }

    portMemFree(pMQI->pWorkArea);

    pMQI->pWorkArea        = NULL;
    pMQI->pCmdQueueElement = NULL;
    pMQI->pMetaData        = NULL;
}

void GspMsgQueuesCleanup(MESSAGE_QUEUE_COLLECTION **ppMQCollection)
{
    MESSAGE_QUEUE_COLLECTION *pMQCollection = NULL;
    MESSAGE_QUEUE_INFO       *pRmQueueInfo  = NULL;
    MESSAGE_QUEUE_INFO       *pTaskIsrQueueInfo = NULL;

    if ((ppMQCollection == NULL) || (*ppMQCollection == NULL))
        return;

    pMQCollection     = *ppMQCollection;
    pRmQueueInfo      = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];
    pTaskIsrQueueInfo = &pMQCollection->rpcQueues[RPC_TASK_ISR_QUEUE_IDX];

    _gspMsgQueueCleanup(pRmQueueInfo);
    _gspMsgQueueCleanup(pTaskIsrQueueInfo);

    if (pMQCollection->pSharedMemDesc != NULL)
    {
        NvP64 pVaKernel   = memdescGetKernelMapping(pMQCollection->pSharedMemDesc);
        NvP64 pPrivKernel = memdescGetKernelMappingPriv(pMQCollection->pSharedMemDesc);

        // Destroy kernel mapping for command queue.
        if (pVaKernel != 0)
        {
            memdescUnmap(pMQCollection->pSharedMemDesc, NV_TRUE, osGetCurrentProcess(),
                         pVaKernel, pPrivKernel);
        }

        // Free command queue memory.
        memdescFree(pMQCollection->pSharedMemDesc);
        memdescDestroy(pMQCollection->pSharedMemDesc);
        pMQCollection->pSharedMemDesc = NULL;
    }

    portMemFree(pMQCollection);
    *ppMQCollection = NULL;
}

/*!
 * Calculate 32-bit checksum
 *
 * This routine assumes that the data is padded out with zeros to the next
 * 8-byte alignment, and it is OK to read past the end to the 8-byte alignment.
 */
static NV_INLINE NvU32 _checkSum32(void *pData, NvU32 uLen)
{
    NvU64 *p        = (NvU64 *)pData;
    NvU64 *pEnd     = (NvU64 *)((NvUPtr)pData + uLen);
    NvU64  checkSum = 0;

    while (p < pEnd)
        checkSum ^= *p++;

    return NvU64_HI32(checkSum) ^ NvU64_LO32(checkSum);
}

/*!
 * GspMsgQueueSendCommand
 *
 * Move a command record from our staging area to the command queue.
 *
 * Returns
 *  NV_OK                       - Record sucessfully sent.
 *  NV_ERR_INVALID_PARAM_STRUCT - Bad record length.
 *  NV_ERR_BUSY_RETRY           - No space in the queue.
 *  NV_ERR_INVALID_STATE        - Something really bad happenned.
 */
NV_STATUS GspMsgQueueSendCommand(MESSAGE_QUEUE_INFO *pMQI, OBJGPU *pGpu)
{
    GSP_MSG_QUEUE_ELEMENT *pCQE = pMQI->pCmdQueueElement;
    NvU8      *pSrc             = (NvU8 *)pCQE;
    NvU8      *pNextElement     = NULL;
    int        nRet;
    int        i;
    int        nRetries;
    int        nElements;
    RMTIMEOUT  timeout;
    NV_STATUS  nvStatus         = NV_OK;
    NvU32      uElementSize     = GSP_MSG_QUEUE_ELEMENT_HDR_SIZE +
                                  pMQI->pCmdQueueElement->rpc.length;

    if ((uElementSize < sizeof(GSP_MSG_QUEUE_ELEMENT)) ||
        (uElementSize > GSP_MSG_QUEUE_ELEMENT_SIZE_MAX))
    {
        NV_PRINTF(LEVEL_ERROR, "Incorrect length %u\n",
            pMQI->pCmdQueueElement->rpc.length);
        nvStatus = NV_ERR_INVALID_PARAM_STRUCT;
        goto done;
    }

    // Make sure the queue element in our working space is zero padded for checksum.
    if ((uElementSize & 7) != 0)
        portMemSet(pSrc + uElementSize, 0, 8 - (uElementSize & 7));

    pCQE->seqNum   = pMQI->txSeqNum;
    pCQE->checkSum = 0;
    pCQE->checkSum = _checkSum32(pSrc, uElementSize);

    nElements = GSP_MSG_QUEUE_BYTES_TO_ELEMENTS(uElementSize);

    for (i = 0; i < nElements; i++)
    {
        NvU32 timeoutFlags = 0;

        if (pMQI->txBufferFull)
            timeoutFlags |= GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG;

        // Set a timeout of 1 sec
        gpuSetTimeout(pGpu, 1000000, &timeout, timeoutFlags);

        // Wait for space to put the next element. Retry for up to 10 ms.
        for (nRetries = 0; ; nRetries++)
        {
            // Must get the buffers one at a time, since they could wrap.
            pNextElement = (NvU8 *)msgqTxGetWriteBuffer(pMQI->hQueue, i);

            if (pNextElement != NULL)
                break;

            if (gpuCheckTimeout(pGpu, &timeout) != NV_OK)
                break;

            portAtomicMemoryFenceFull();

            osSpinLoop();
        }

        if (pNextElement == NULL)
        {
            pMQI->txBufferFull++;
            NV_PRINTF_COND(pMQI->txBufferFull == 1, LEVEL_ERROR, LEVEL_INFO, "buffer is full\n");
            nvStatus = NV_ERR_BUSY_RETRY;
            goto done;
        }
        else
        {
            pMQI->txBufferFull = 0;
        }

        portMemCopy(pNextElement, GSP_MSG_QUEUE_ELEMENT_SIZE_MIN,
                    pSrc,         GSP_MSG_QUEUE_ELEMENT_SIZE_MIN);
        pSrc += GSP_MSG_QUEUE_ELEMENT_SIZE_MIN;
    }

    //
    // If write after write (WAW) memory ordering is relaxed in a CPU, then
    // it's possible that below msgq update reaches memory first followed by
    // above portMemCopy data. This is an issue for GSP RM which will read
    // incorrect data because msgq was updated first. This is a typical
    // example of producer consumer problem in memory ordering world. Hence,
    // a store fence is needed here.
    //
    portAtomicMemoryFenceStore();

    nRet = msgqTxSubmitBuffers(pMQI->hQueue, nElements);

    if (nRet != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqTxSubmitBuffers failed: %d\n", nRet);
        nvStatus = NV_ERR_INVALID_STATE;
        goto done;
    }

    // Advance seq num only if we actually used it.
    pMQI->txSeqNum++;

    nvStatus = NV_OK;

done:
    return nvStatus;
}

/*!
 * GspMsgQueueReceiveStatus
 *
 * Get a status record from the GSP and move it from the rx queue to our
 * staging area.
 *
 * Returns
 *  NV_OK                       - Record sucessfully read.
 *  NV_ERR_INVALID_PARAM_STRUCT - Bad record length.
 *  NV_ERR_NOT_READY            - Partial read.
 *  NV_ERR_INVALID_STATE        - Something really bad happenned.
 */
NV_STATUS GspMsgQueueReceiveStatus(MESSAGE_QUEUE_INFO *pMQI)
{
    const NvU8 *pNextElement = NULL;
    NvU8       *pTgt         = (NvU8 *)pMQI->pCmdQueueElement;
    int         nRet;
    int         i;
    int         nRetries;
    int         nMaxRetries  = 3;
    int         nElements    = 1;  // Assume record fits in one queue element for now.
    NvU32       uElementSize = 0;
    NvU32       seqMismatchDiff = NV_U32_MAX;
    NV_STATUS   nvStatus     = NV_OK;

    for (nRetries = 0; nRetries < nMaxRetries; nRetries++)
    {
        pTgt      = (NvU8 *)pMQI->pCmdQueueElement;
        nvStatus  = NV_OK;
        nElements = 1;  // Assume record fits in one queue element for now.

        for (i = 0; i < nElements; i++)
        {
            // Get the pointer to the next queue element.
            pNextElement = msgqRxGetReadBuffer(pMQI->hQueue, i);
            if (pNextElement == NULL)
            {
                // Early exit if this is the first read and there is no data.
                if (i == 0)
                    return NV_WARN_NOTHING_TO_DO;

                //
                // We already successfully read part of the record, so we are here
                // because the data is in flight (no fence) or the length was wrong.
                //
                NV_PRINTF(LEVEL_ERROR, "Incomplete read.\n");
                nvStatus = NV_ERR_NOT_READY;
                break;
            }

            // Copy the next element to our staging area.
            portMemCopy(pTgt, GSP_MSG_QUEUE_ELEMENT_SIZE_MIN,
                        pNextElement, GSP_MSG_QUEUE_ELEMENT_SIZE_MIN);
            pTgt += GSP_MSG_QUEUE_ELEMENT_SIZE_MIN;

            if (i != 0)
                continue;

            //
            // Special processing for first element of the record.
            // Pull out the length and make sure it is valid.
            //
            uElementSize = GSP_MSG_QUEUE_ELEMENT_HDR_SIZE +
                pMQI->pCmdQueueElement->rpc.length;

            if ((uElementSize < sizeof(GSP_MSG_QUEUE_ELEMENT)) ||
                (uElementSize > GSP_MSG_QUEUE_ELEMENT_SIZE_MAX))
            {
                // The length is not valid.  If we are running without a fence,
                // this could mean that the data is still in flight from the CPU.
                NV_PRINTF(LEVEL_ERROR, "Incorrect length %u\n",
                    pMQI->pCmdQueueElement->rpc.length);
                nvStatus = NV_ERR_INVALID_PARAM_STRUCT;
                break;
            }

            // This adjusts the loop condition.
            nElements = GSP_MSG_QUEUE_BYTES_TO_ELEMENTS(uElementSize);
        }

        // Retry if there was an error.
        if (nvStatus != NV_OK)
            continue;

        // Retry if checksum fails.
        if (_checkSum32(pMQI->pCmdQueueElement, uElementSize) != 0)
        {
            NV_PRINTF(LEVEL_ERROR, "Bad checksum.\n");
            nvStatus = NV_ERR_INVALID_DATA;
            continue;
        }

        // Retry if sequence number is wrong.
        if (pMQI->pCmdQueueElement->seqNum != pMQI->rxSeqNum)
        {
            NV_PRINTF(LEVEL_ERROR, "Bad sequence number.  Expected %u got %u. Possible memory corruption.\n",
                pMQI->rxSeqNum, pMQI->pCmdQueueElement->seqNum);

            // If we read an old piece of data, try to ignore it and move on..
            if (pMQI->pCmdQueueElement->seqNum < pMQI->rxSeqNum)
            {
                // Make sure we're converging to the desired pMQI->rxSeqNum
                if ((pMQI->rxSeqNum - pMQI->pCmdQueueElement->seqNum) < seqMismatchDiff)
                {
                    NV_PRINTF(LEVEL_ERROR, "Attempting recovery: ignoring old package with seqNum=%u of %u elements.\n",
                        pMQI->pCmdQueueElement->seqNum, nElements);

                    seqMismatchDiff = pMQI->rxSeqNum - pMQI->pCmdQueueElement->seqNum;
                    nRet = msgqRxMarkConsumed(pMQI->hQueue, nElements);
                    if (nRet < 0)
                    {
                        NV_PRINTF(LEVEL_ERROR, "msgqRxMarkConsumed failed: %d\n", nRet);
                    }
                    nMaxRetries++;
                }
            }

            nvStatus = NV_ERR_INVALID_DATA;
            continue;
        }

        // We have the whole record, so break out of the retry loop.
        break;
    }

    if (nvStatus == NV_OK)
    {
        pMQI->rxSeqNum++;

        nRet = msgqRxMarkConsumed(pMQI->hQueue, nElements);
        if (nRet < 0)
        {
            NV_PRINTF(LEVEL_ERROR, "msgqRxMarkConsumed failed: %d\n", nRet);
            nvStatus = NV_ERR_GENERIC;
        }
    }

    if (nRetries > 0)
    {
        if (nvStatus == NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Read succeeded with %d retries.\n", nRetries);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Read failed after %d retries.\n", nRetries);
        }
    }

    return nvStatus;
}

