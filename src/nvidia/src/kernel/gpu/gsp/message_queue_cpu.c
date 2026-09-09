/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mctp_format.h"
#include "nvdm_format.h"
#include "gpu/gsp/message_queue.h"
#include "gpu/gsp/message_queue_priv.h"
#include "msgq/msgq_priv.h"
#include "gpu/gsp/kernel_gsp.h"
#include "nvrm_registry.h"
#include "gpu/conf_compute/ccsl.h"
#include "gpu/conf_compute/conf_compute.h"

static void _gspMsgQueueCleanup(MESSAGE_QUEUE_INFO *pMQI);

static void
_getMsgQueueParams
(
    OBJGPU *pGpu,
    MESSAGE_QUEUE_COLLECTION *pMQCollection
)
{
    NvLength queueSize;
    MESSAGE_QUEUE_INFO *pRmQueueInfo = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];
    NvU32 numPtes;
    const NvLength defaultCommandQueueSize = 0x40000; // 256 KB
    const NvLength defaultStatusQueueSize  = 0x40000; // 256 KB
    NvU32 regStatusQueueSize;

    //
    // NOTE: We know if CC is enabled here because ConfidentialCompute engine
    // is marked as bConstructEarly in gpu_child_list.h, while KernelGsp is not.
    // We end up here in kgspConstructEngine, which happens after CC enablement.
    //
    pRmQueueInfo->bEncryptionEnabled = gpuIsCCFeatureEnabled(pGpu);

    pRmQueueInfo->queueElementHdrSize = pRmQueueInfo->bEncryptionEnabled ?
        GSP_MSG_QUEUE_ELEMENT_SIZE_WITH_ENCRYPTION : GSP_MSG_QUEUE_ELEMENT_SIZE_NO_ENCRYPTION;

    pRmQueueInfo->queueElementSizeMin = RM_PAGE_SIZE;
    pRmQueueInfo->queueElementSizeMax = RM_PAGE_SIZE * 16;
    pRmQueueInfo->queueHeaderAlign    = 4;
    pRmQueueInfo->queueElementAlign   = RM_PAGE_SHIFT;

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
        regStatusQueueSize = NV_MAX(pRmQueueInfo->queueElementSizeMax, regStatusQueueSize);
        regStatusQueueSize = NV_ALIGN_UP(regStatusQueueSize, 1 << RM_PAGE_SHIFT);
        pRmQueueInfo->statusQueueSize = regStatusQueueSize;
    }
    else
    {
        pRmQueueInfo->statusQueueSize = defaultStatusQueueSize;
    }
    pRmQueueInfo->bErrorInjectionEnabled = NV_FALSE;

    //
    // Calculate the number of entries required to map both queues in addition
    // to the page table itself.
    //
    queueSize = pRmQueueInfo->commandQueueSize + pRmQueueInfo->statusQueueSize;
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
    MESSAGE_QUEUE_INFO *pMQI,
    OBJGPU *pGpu
)
{
    NvU32 workAreaSize;
    NV_STATUS nvStatus = NV_OK;
    int nRet;

    // Allocate work area.
    workAreaSize = (1 << pMQI->queueElementAlign) +
                   pMQI->queueElementSizeMax + msgqGetMetaSize();
    pMQI->pWorkArea = portMemAllocNonPaged(workAreaSize);
    if (pMQI->pWorkArea == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error allocating pWorkArea.\n");
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pMQI->pWorkArea, 0, workAreaSize);

    pMQI->pCmdQueueElement = (GSP_MSG_QUEUE_ELEMENT *)
        NV_ALIGN_UP((NvUPtr)pMQI->pWorkArea, 1 << pMQI->queueElementAlign);
    pMQI->pMetaData = (void *)((NvUPtr)pMQI->pCmdQueueElement + pMQI->queueElementSizeMax);

    nRet = msgqInit(&pMQI->hQueue, pMQI->pMetaData);
    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqInit failed: %d\n", nRet);
        nvStatus = NV_ERR_GENERIC;
        goto error_ret;
    }

    NvU32 cmdHead, cmdTail;
    NV_ASSERT_OK_OR_GOTO(nvStatus, gpuGetGspMsgQueueRegisters(pGpu, RPC_TASK_RM_QUEUE_IDX, &cmdHead, &cmdTail, NULL, NULL), error_ret);

    nRet = msgqTxCreate(pMQI->hQueue,
                pMQI->pCommandQueue,
                pMQI->commandQueueSize,
                pMQI->queueElementSizeMin,
                pMQI->queueHeaderAlign,
                pMQI->queueElementAlign,
                pGpu,
                cmdHead,
                cmdTail);
    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqTxCreate failed: %d\n", nRet);
        nvStatus = NV_ERR_GENERIC;
        goto error_ret;
    }

    pMQI->pRpcMsgBuf = gspMsgQueueGetRpcMessageHeader(pMQI, pMQI->pCmdQueueElement);

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
    MESSAGE_QUEUE_COLLECTION *pMQCollection = NULL;
    MESSAGE_QUEUE_INFO  *pRmQueueInfo = NULL;
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

    sharedBufSize = pMQCollection->pageTableSize +
                    pRmQueueInfo->commandQueueSize +
                    pRmQueueInfo->statusQueueSize;

    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }

    //
    // For now, put all shared queue memory in one block.
    //
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pMQCollection->pSharedMemDesc, pGpu, sharedBufSize,
            RM_PAGE_SIZE, NV_MEMORY_NONCONTIGUOUS, ADDR_SYSMEM, NV_MEMORY_CACHED,
            flags),
        error_ret);

    memdescSetFlag(pMQCollection->pSharedMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_TRUE);

    memdescSetPageSize(pMQCollection->pSharedMemDesc, AT_GPU, RM_PAGE_SIZE_HUGE);
    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_58,
                    pMQCollection->pSharedMemDesc);

    if (nvStatus == NV_ERR_NO_MEMORY)
    {
        // TODO: Bug 5299603
        NV_PRINTF(LEVEL_ERROR, "Allocation failed with big page size, retrying with default page size\n");
        memdescSetPageSize(pMQCollection->pSharedMemDesc, AT_GPU, RM_PAGE_SIZE);
        memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_58,
                        pMQCollection->pSharedMemDesc);
    }

    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus, error_ret);

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

    // Assert that the last queue offset + size fits into the shared memory.
    NV_ASSERT(NvP64_PLUS_OFFSET(pVaKernel, sharedBufSize) ==
              NvP64_PLUS_OFFSET(lastQueueVa, lastQueueSize));

    NV_ASSERT_OK_OR_GOTO(nvStatus, _gspMsgQueueInit(pRmQueueInfo, pGpu), error_ret);
    pRmQueueInfo->queueIdx = RPC_TASK_RM_QUEUE_IDX;

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

    NvU32 msgHead, msgTail;
    NV_ASSERT_OK_OR_RETURN(gpuGetGspMsgQueueRegisters(pGpu, RPC_TASK_RM_QUEUE_IDX, NULL, NULL, &msgHead, &msgTail));
    // Wait other end of the queue to run msgqInit.  Retry until the timeout.
    for (nRetries = 0; ; nRetries++)
    {
        // Link in status queue
        portAtomicMemoryFenceFull();

        nRet = msgqRxLink((*ppMQI)->hQueue, (*ppMQI)->pStatusQueue,
                          (*ppMQI)->statusQueueSize, (*ppMQI)->queueElementSizeMin, msgHead, msgTail);

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

        kgspDumpGspLogs(pKernelGsp, NV_FALSE);
        if (!kgspHealthCheck_HAL(pGpu, pKernelGsp))
        {
            nvStatus = NV_ERR_RESET_REQUIRED;
            break;
        }
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

    if ((ppMQCollection == NULL) || (*ppMQCollection == NULL))
        return;

    pMQCollection     = *ppMQCollection;
    pRmQueueInfo      = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];

    _gspMsgQueueCleanup(pRmQueueInfo);

    if (pMQCollection->pSharedMemDesc != NULL)
    {
        NvP64 pVaKernel   = memdescGetKernelMapping(pMQCollection->pSharedMemDesc);
        NvP64 pPrivKernel = memdescGetKernelMappingPriv(pMQCollection->pSharedMemDesc);

        // Destroy kernel mapping for command queue.
        if (pVaKernel != 0)
        {
            memdescUnmap(pMQCollection->pSharedMemDesc, NV_TRUE,
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
NV_STATUS GspMsgQueueSendCommand(MESSAGE_QUEUE_INFO *pMQI, OBJGPU *pGpu, NvU32 nvdmType, NvU32 size)
{
    GSP_MSG_QUEUE_ELEMENT *pCQE = pMQI->pCmdQueueElement;
    NvU8      *pSrc             = (NvU8 *)pCQE;
    NvU8      *pNextElement     = NULL;
    int        nRet;
    NvU32      i;
    RMTIMEOUT  timeout;
    NV_STATUS  nvStatus         = NV_OK;
    NvU32      nElements;

    if ((pMQI->queueElementHdrSize + size) > pMQI->queueElementSizeMax)
    {
        NV_PRINTF(LEVEL_ERROR, "Incorrect message length %u\n", size);
        nvStatus = NV_ERR_INVALID_PARAM_STRUCT;
        goto done;
    }

    pCQE->mctpMagic = MCTP_MAGIC;
    pCQE->mctpPayloadSize = pMQI->queueElementHdrSize + size;
    pCQE->mctpHeader = mctpCreateTransportHeader(
        1,                    // SOM = 1 (start of message)
        1,                    // EOM = 1 (assume single packet, large RPC handled in vgpu/rpc.c "CONTINUATION_RECORD")
        0,                    // SEID = 0 (unused)
        0,                    // DEID = 0 (unused)
        0                     // SEQ = 0 (unused)
    );
    pCQE->nvdmHeader = mctpCreateNvdmHeader(nvdmType);

    nElements = gspMsgQueueBytesToElements(pCQE->mctpPayloadSize, pMQI->queueElementSizeMin);

    if (pMQI->bEncryptionEnabled)
    {
        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

        pCQE->withEncryption.nvdmPayloadSize = size;
        pCQE->withEncryption.reserved = 0;

        // We need to encrypt the full queue elements to obscure the data.
        pCQE->mctpPayloadSize = NV_ALIGN_UP(pCQE->mctpPayloadSize, pMQI->queueElementSizeMin);
        const NvU32 encryptedSize = pCQE->mctpPayloadSize - GSP_MSG_QUEUE_ELEMENT_ENCRYPTION_OFFSET;
        nvStatus = gspMsgQueueCCEncrypt(pCC->pRpcCcslCtx, pMQI, pCQE, encryptedSize);

        if (nvStatus != NV_OK)
        {
            // Do not re-try if encryption fails.
            NV_PRINTF(LEVEL_ERROR, "Encryption failed with status = 0x%x.\n", nvStatus);
            if (nvStatus == NV_ERR_INSUFFICIENT_RESOURCES)
            {
                // We hit potential IV overflow, this is fatal.
                NV_PRINTF(LEVEL_ERROR, "Fatal error detected in RPC encrypt: IV overflow!\n");
                confComputeSetErrorState(pGpu, pCC);
            }
            return nvStatus;
        }
    }
    else
    {
        pCQE->noEncryption.nvdmPayloadSize = size;
        pCQE->noEncryption.reserved = 0;
    }

    if (pMQI->bErrorInjectionEnabled)
    {
        NvU32 *pData = (NvU32 *)pSrc;
        *pData = 0xFFFFFFFF;
    }

    for (i = 0; i < nElements; i++)
    {
        NvU32 timeoutFlags = 0;

        if (pMQI->txBufferFull)
            timeoutFlags |= GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG;

        // Set a timeout of 1 sec
        gpuSetTimeout(pGpu, 1000000, &timeout, timeoutFlags);

        // Wait for space to put the next element.
        while (NV_TRUE)
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
            NV_PRINTF_COND(pMQI->txBufferFull == 1, LEVEL_ERROR, LEVEL_INFO,
                           "buffer is full (waiting for %d free elements, got %d)\n",
                           nElements, i);
            nvStatus = NV_ERR_BUSY_RETRY;
            goto done;
        }
        else
        {
            pMQI->txBufferFull = 0;
        }

        portMemCopy(pNextElement, pMQI->queueElementSizeMin,
                    pSrc, pMQI->queueElementSizeMin);
        pSrc += pMQI->queueElementSizeMin;
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
NV_STATUS GspMsgQueueReceiveStatus(MESSAGE_QUEUE_INFO *pMQI, OBJGPU *pGpu, NvU32 *pNvdmType)
{
    const NvU8 *pNextElement = NULL;
    GSP_MSG_QUEUE_ELEMENT *pCQE = pMQI->pCmdQueueElement;
    GSP_MSG_QUEUE_ELEMENT *pFirstElementInSharedMemory = NULL;
    NvU8       *pTgt         = (NvU8 *)pCQE;
    int         nRet;
    NvU32       i;
    NvU32       nRetries;
    NvU32       nMaxRetries  = 3;
    NvU32       seenMsgIgnores = 16;
    NvU32       nElements    = 1;  // Assume record fits in one queue element for now.
    NV_STATUS   nvStatus     = NV_OK;

    for (nRetries = 0; nRetries < nMaxRetries; nRetries++)
    {
        pTgt      = (NvU8 *)pCQE;
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
            portMemCopy(pTgt, pMQI->queueElementSizeMin,
                        pNextElement, pMQI->queueElementSizeMin);
            pTgt += pMQI->queueElementSizeMin;

            if (i == 0)
            {
                // Sanity check for the given RPC length
                if ((pCQE->mctpPayloadSize < pMQI->queueElementHdrSize) ||
                    (pCQE->mctpPayloadSize > pMQI->queueElementSizeMax))
                {
                    // The length is not valid.  If we are running without a fence,
                    // this could mean that the data is still in flight from the CPU.
                    NV_PRINTF(LEVEL_ERROR, "Incorrect message length %u\n", pCQE->mctpPayloadSize);
                    nvStatus = NV_ERR_INVALID_PARAM_STRUCT;
                    break;
                }

                //
                // Special processing for first element of the record.
                // Calculate element count from the message size. This adjusts the loop condition.
                //
                nElements = gspMsgQueueBytesToElements(pCQE->mctpPayloadSize,
                                                       pMQI->queueElementSizeMin);
                pFirstElementInSharedMemory = (GSP_MSG_QUEUE_ELEMENT *)pNextElement;
            }
        }

        // Retry if there was an error.
        if (nvStatus != NV_OK)
            continue;

        // Validate MCTP/NVDM protocol headers.
        {
            NvU32 mctpVersion = REF_VAL(MCTP_HEADER_VERSION, pCQE->mctpHeader);
            NvU32 vendorId    = REF_VAL(MCTP_MSG_HEADER_VENDOR_ID, pCQE->nvdmHeader);

            if (mctpVersion != 0x1)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "MCTP protocol violation: invalid header version 0x%x (expected 0x1)\n",
                          mctpVersion);
                nvStatus = NV_ERR_INVALID_DATA;
                continue;
            }

            if (vendorId != MCTP_MSG_HEADER_VENDOR_ID_NV)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "MCTP protocol violation: invalid vendor ID 0x%x (expected 0x%x)\n",
                          vendorId, MCTP_MSG_HEADER_VENDOR_ID_NV);
                nvStatus = NV_ERR_INVALID_DATA;
                continue;
            }

            if (pCQE->mctpMagic != MCTP_MAGIC && pCQE->mctpMagic != MCTP_MAGIC_SEEN)
            {
                NV_PRINTF(LEVEL_ERROR, "MCTP protocol violation: invalid magic number 0x%x (expected 0x%x)\n",
                          pCQE->mctpMagic, MCTP_MAGIC);
                nvStatus = NV_ERR_INVALID_DATA;
                continue;
            }

            *pNvdmType = REF_VAL(MCTP_MSG_HEADER_NVDM_TYPE, pCQE->nvdmHeader);
        }

        //
        // Extra resiliency in case we read a stale message. This "shouldn't"
        // ever happen, but it has already happened at least twice in the past:
        //    - Because we were missing a fence somewhere
        //    - Because the DMA kickoff from GSP failed silently
        // so, we keep an extra eye open on repeat messages.
        // We do this by changing the MCTP magic number from "MCTP" to "SEEN"
        // when we read a message. If we ever see "SEEN", we skip the message.
        //
        if (pCQE->mctpMagic == MCTP_MAGIC_SEEN)
        {
            NV_PRINTF(LEVEL_ERROR, "Stale message detected (nvdmType=0x%x, mctpPayloadSize=%u). Ignoring.\n",
                *pNvdmType, pCQE->mctpPayloadSize);

            nvStatus = NV_ERR_INVALID_DATA;
            if (seenMsgIgnores > 0)
            {
                seenMsgIgnores--;
                nMaxRetries++;
                nRet = msgqRxMarkConsumed(pMQI->hQueue, nElements);
                if (nRet < 0)
                {
                    NV_PRINTF(LEVEL_ERROR, "msgqRxMarkConsumed failed: %d\n", nRet);
                    nvStatus = NV_ERR_GENERIC;
                    goto exit;
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Too many stale messages detected. Giving up.\n");
                nMaxRetries = 0;
            }
            continue;
        }
        else
        {
            pFirstElementInSharedMemory->mctpMagic = MCTP_MAGIC_SEEN;
            portAtomicMemoryFenceStore();
        }

        // We have the whole record, so break out of the retry loop.
        break;
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
            goto exit;
        }
    }

    if (pMQI->bEncryptionEnabled)
    {
        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

        const NvU32 encryptedSize = pCQE->mctpPayloadSize - GSP_MSG_QUEUE_ELEMENT_ENCRYPTION_OFFSET;
        nvStatus = gspMsgQueueCCDecrypt(pCC->pRpcCcslCtx, pMQI, pCQE, encryptedSize);

        if (nvStatus != NV_OK)
        {
            // Do not re-try if decryption failed. Decryption failure is considered fatal.
            NV_PRINTF(LEVEL_ERROR, "Fatal error detected in RPC decrypt: 0x%x!\n", nvStatus);
            confComputeSetErrorState(pGpu, pCC);
            goto exit;
        }
    }

exit:

    nRet = msgqRxMarkConsumed(pMQI->hQueue, nElements);
    if (nRet < 0)
    {
        NV_PRINTF(LEVEL_ERROR, "msgqRxMarkConsumed failed: %d\n", nRet);
        nvStatus = NV_ERR_GENERIC;
    }

    return nvStatus;
}
