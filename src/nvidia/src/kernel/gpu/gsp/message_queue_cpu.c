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

ct_assert(GSP_MSG_QUEUE_HEADER_SIZE > sizeof(msgqTxHeader) + sizeof(msgqRxHeader));

static void
_getMsgQueueParams
(
    OBJGPU *pGpu,
    MESSAGE_QUEUE_INFO *pMQI
)
{
    NvLength queueSize;
    NvU32 numPtes;
    const NvLength defaultCommandQueueSize = 0x40000; // 256 KB
    const NvLength defaultStatusQueueSize  = 0x40000; // 256 KB

    if (IS_SILICON(pGpu))
    {
        pMQI->commandQueueSize = defaultCommandQueueSize;
        pMQI->statusQueueSize = defaultStatusQueueSize;
    }
    else
    {
        //
        // Pre-silicon platforms need a large command queue in order to send
        // the VBIOS image via RPC.
        //
        pMQI->commandQueueSize = defaultCommandQueueSize * 6;
        pMQI->statusQueueSize = defaultStatusQueueSize;
    }

    //
    // Calculate the number of entries required to map both queues in addition
    // to the page table itself.
    //
    queueSize = pMQI->commandQueueSize + pMQI->statusQueueSize;
    NV_ASSERT((queueSize & RM_PAGE_MASK) == 0);
    numPtes = (queueSize >> RM_PAGE_SHIFT);

    // Account for the pages needed to store the PTEs
    numPtes += NV_DIV_AND_CEIL(numPtes * sizeof(RmPhysAddr), RM_PAGE_SIZE);

    //
    // Align the page table size to RM_PAGE_SIZE, so that the command queue is
    // aligned.
    //
    pMQI->pageTableSize = RM_PAGE_ALIGN_UP(numPtes * sizeof(RmPhysAddr));
    pMQI->pageTableEntryCount = numPtes;
}

/*!
 * GspMsgQueueInit
 *
 * Initialize the command queue for CPU side.
 * Must not be called before portInitialize.
 */
NV_STATUS
GspMsgQueueInit
(
    OBJGPU              *pGpu,
    MESSAGE_QUEUE_INFO **ppMQI
)
{
    MESSAGE_QUEUE_INFO *pMQI = NULL;
    RmPhysAddr  *pPageTbl;
    int          nRet;
    NvP64        pVaKernel;
    NvP64        pPrivKernel;
    NV_STATUS    nvStatus         = NV_OK;
    NvLength     sharedBufSize;
    NvLength     firstCmdOffset;
    NvU32        workAreaSize;

    if (*ppMQI != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP message queue was already initialized.\n");
        return NV_ERR_INVALID_STATE;
    }

    pMQI = portMemAllocNonPaged(sizeof *pMQI);
    if (pMQI == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating queue info area.\n");
        nvStatus = NV_ERR_NO_MEMORY;
        goto done;
    }
    portMemSet(pMQI, 0, sizeof *pMQI);

    _getMsgQueueParams(pGpu, pMQI);

    sharedBufSize = pMQI->pageTableSize +
                    pMQI->commandQueueSize +
                    pMQI->statusQueueSize;

    //
    // For now, put all shared queue memory in one block.
    //
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pMQI->pSharedMemDesc, pGpu, sharedBufSize,
            RM_PAGE_SIZE, NV_MEMORY_NONCONTIGUOUS, ADDR_SYSMEM, NV_MEMORY_CACHED,
            MEMDESC_FLAGS_NONE),
        done);

    memdescSetFlag(pMQI->pSharedMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_TRUE);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescAlloc(pMQI->pSharedMemDesc),
        error_ret);

    // Create kernel mapping for command queue.
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pMQI->pSharedMemDesc, 0, sharedBufSize,
            NV_TRUE, NV_PROTECT_WRITEABLE,
            &pVaKernel, &pPrivKernel),
        error_ret);

    memdescSetKernelMapping(pMQI->pSharedMemDesc, pVaKernel);
    memdescSetKernelMappingPriv(pMQI->pSharedMemDesc, pPrivKernel);

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
    //   Command queue header
    //   Command queue entries
    //   Status queue header
    //   Status queue entries
    memdescGetPhysAddrs(pMQI->pSharedMemDesc,
                    AT_GPU,                     // addressTranslation
                    0,                          // offset
                    RM_PAGE_SIZE,               // stride
                    pMQI->pageTableEntryCount,  // count
                    pPageTbl);                  // physical address table

    pMQI->pCommandQueue  = NvP64_VALUE(
        NvP64_PLUS_OFFSET(pVaKernel, pMQI->pageTableSize));

    pMQI->pStatusQueue   = NvP64_VALUE(
        NvP64_PLUS_OFFSET(NV_PTR_TO_NvP64(pMQI->pCommandQueue), pMQI->commandQueueSize));

    NV_ASSERT(NvP64_PLUS_OFFSET(pVaKernel, sharedBufSize) ==
              NvP64_PLUS_OFFSET(NV_PTR_TO_NvP64(pMQI->pStatusQueue), pMQI->statusQueueSize));

    // Allocate work area.
    workAreaSize = (1 << GSP_MSG_QUEUE_ELEMENT_ALIGN) +
                   GSP_MSG_QUEUE_ELEMENT_SIZE_MAX + msgqGetMetaSize();
    pMQI->pWorkArea = portMemAllocNonPaged(workAreaSize);

    if (pMQI->pWorkArea == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating pWorkArea.\n");
        nvStatus = NV_ERR_NO_MEMORY;
        goto error_ret;
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

    NV_PRINTF(LEVEL_INFO, "Created command queue.\n");

    *ppMQI             = pMQI;
    pMQI->sharedMemPA  = pPageTbl[0];
    pMQI->pRpcMsgBuf   = &pMQI->pCmdQueueElement->rpc;

    firstCmdOffset = pMQI->pageTableSize + GSP_MSG_QUEUE_HEADER_SIZE;
    pMQI->pInitMsgBuf  = NvP64_PLUS_OFFSET(pVaKernel, firstCmdOffset);
    pMQI->initMsgBufPA = pPageTbl[firstCmdOffset >> RM_PAGE_SHIFT] +
                         (firstCmdOffset & RM_PAGE_MASK);
done:
    return nvStatus;

error_ret:
    GspMsgQueueCleanup(&pMQI);
    return nvStatus;
}

NV_STATUS GspStatusQueueInit(OBJGPU *pGpu, MESSAGE_QUEUE_INFO **ppMQI)
{
    NV_STATUS  nvStatus = NV_ERR_GENERIC;
    int        nRet = 0;
    int        nRetries;
    RMTIMEOUT  timeout;
    NvU32      timeoutUs = 2000000;
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

    // Wait other end of the queue to run msgqInit.  Retry for up to 10 ms.
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
        GspMsgQueueCleanup(ppMQI);
    }

    return nvStatus;
}

void GspMsgQueueCleanup(MESSAGE_QUEUE_INFO **ppMQI)
{
    MESSAGE_QUEUE_INFO *pMQI = NULL;

    if ((ppMQI == NULL) || (*ppMQI == NULL))
        return;

    pMQI         = *ppMQI;
    pMQI->hQueue = NULL;

    if (pMQI->pWorkArea != NULL)
    {
        portMemFree(pMQI->pWorkArea);
        pMQI->pWorkArea        = NULL;
        pMQI->pCmdQueueElement = NULL;
        pMQI->pMetaData        = NULL;
    }

    if (pMQI->pSharedMemDesc != NULL)
    {
        NvP64 pVaKernel   = memdescGetKernelMapping(pMQI->pSharedMemDesc);
        NvP64 pPrivKernel = memdescGetKernelMappingPriv(pMQI->pSharedMemDesc);

        // Destroy kernel mapping for command queue.
        if (pVaKernel != 0)
        {
            memdescUnmap(pMQI->pSharedMemDesc, NV_TRUE, osGetCurrentProcess(),
                         pVaKernel, pPrivKernel);
        }

        // Free command queue memory.
        memdescFree(pMQI->pSharedMemDesc);
        memdescDestroy(pMQI->pSharedMemDesc);
        pMQI->pSharedMemDesc = NULL;
    }

    portMemFree(pMQI);
    *ppMQI = NULL;
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

    pCQE->seqNum   = pMQI->txSeqNum++;
    pCQE->checkSum = 0;
    pCQE->checkSum = _checkSum32(pSrc, uElementSize);

    nElements = GSP_MSG_QUEUE_BYTES_TO_ELEMENTS(uElementSize);

    for (i = 0; i < nElements; i++)
    {
        // Set a timeout of 1 sec
        gpuSetTimeout(pGpu, 1000000, &timeout, 0);

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
            NV_PRINTF(LEVEL_ERROR, "buffer is full\n");
            nvStatus = NV_ERR_BUSY_RETRY;
            goto done;
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
    int         nElements    = 1;  // Assume record fits in one 256-byte queue element for now.
    NvU32       uElementSize = 0;
    NvU32       seqMismatchDiff = NV_U32_MAX;
    NV_STATUS   nvStatus     = NV_OK;

    for (nRetries = 0; nRetries < nMaxRetries; nRetries++)
    {
        pTgt      = (NvU8 *)pMQI->pCmdQueueElement;
        nvStatus  = NV_OK;
        nElements = 1;  // Assume record fits in one 256-byte queue element for now.

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

