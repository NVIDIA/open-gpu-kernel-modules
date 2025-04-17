/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "core/core.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/mem_mgr/channel_utils.h"
#include "rmapi/rs_utils.h"
#include "utils/nvassert.h"
#include "core/prelude.h"
#include "core/locks.h"
#include "gpu/mem_mgr/sec2_utils.h"
#include "kernel/gpu/mem_mgr/ce_utils_sizes.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "nvrm_registry.h"
#include "platform/chipset/chipset.h"
#include "gpu/mem_mgr/heap.h"

#include "class/clcba2.h" // HOPPER_SEC2_WORK_LAUNCH_A
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL

#include "class/cl0080.h"

#include "gpu/conf_compute/conf_compute.h"
#include "gpu/conf_compute/ccsl.h"

static NV_STATUS
_sec2GetClass(OBJGPU *pGpu, NvU32 *pClass)
{
    NV_STATUS status;
    NvU32 numClasses = 0;
    NvU32 *pClassList = NULL;
    NvU32 class = 0;

    NV_ASSERT_OR_RETURN(pClass != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OK_OR_RETURN(gpuGetClassList(pGpu, &numClasses, NULL, ENG_SEC2));
    NV_ASSERT_OR_RETURN((numClasses != 0), NV_ERR_NOT_SUPPORTED);

    pClassList = portMemAllocNonPaged(sizeof(*pClassList) * numClasses);
    NV_ASSERT_OR_RETURN((pClassList != NULL), NV_ERR_INSUFFICIENT_RESOURCES);

    status = gpuGetClassList(pGpu, &numClasses, pClassList, ENG_SEC2);
    if (status == NV_OK)
    {
        for (NvU32 i = 0; i < numClasses; i++)
        {
            class = NV_MAX(class, pClassList[i]);
        }
        if (class == 0)
        {
            status = NV_ERR_INVALID_STATE;
        }
        *pClass = class;
    }
    portMemFree(pClassList);
    return status;
}

static NV_STATUS
_sec2AllocAndMapBuffer
(
    Sec2Utils *pSec2Utils,
    NvU32 size,
    SEC2UTILS_BUFFER_INFO *pSec2Buf
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pSec2Utils->pGpu);

    pSec2Buf->size = size;

    // allocate the physmem
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = pSec2Buf->size;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI) |
                               DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED) |
                               DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG);
    memAllocParams.attr2     = DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED);
    memAllocParams.flags     = 0;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pSec2Utils->hClient,
                                pSec2Utils->hDevice,
                                pSec2Buf->hPhysMem,
                                NV01_MEMORY_SYSTEM,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // allocate Virtual Memory
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = pSec2Buf->size;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL;
    memAllocParams.hVASpace  = pSec2Buf->hVASpace;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pSec2Utils->hClient,
                                pSec2Utils->hDevice,
                                pSec2Buf->hVirtMem,
                                NV50_MEMORY_VIRTUAL,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // map the buffer
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl = SYS_GET_CL(pSys);
    NvU32 cacheSnoopFlag = 0;
    if (pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT))
    {
        cacheSnoopFlag = DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);
    }
    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->Map(pRmApi, pSec2Utils->hClient, pSec2Utils->hDevice,
                    pSec2Buf->hVirtMem, pSec2Buf->hPhysMem, 0, pSec2Buf->size,
                    DRF_DEF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE) | cacheSnoopFlag,
                    &pSec2Buf->gpuVA));

    pSec2Buf->pMemDesc = memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, pSec2Utils->hClient, pSec2Buf->hPhysMem);
    return NV_OK;
}

static NV_STATUS
_sec2InitBuffers
(
    Sec2Utils *pSec2Utils
)
{
    NV_ASSERT_OK_OR_RETURN(serverutilGenResourceHandle(pSec2Utils->hClient, &pSec2Utils->scrubMthdAuthTagBuf.hPhysMem));
    NV_ASSERT_OK_OR_RETURN(serverutilGenResourceHandle(pSec2Utils->hClient, &pSec2Utils->scrubMthdAuthTagBuf.hVirtMem));
    NV_ASSERT_OK_OR_RETURN(_sec2AllocAndMapBuffer(pSec2Utils, RM_PAGE_SIZE_64K, &pSec2Utils->scrubMthdAuthTagBuf));

    NV_ASSERT_OK_OR_RETURN(serverutilGenResourceHandle(pSec2Utils->hClient, &pSec2Utils->semaMthdAuthTagBuf.hPhysMem));
    NV_ASSERT_OK_OR_RETURN(serverutilGenResourceHandle(pSec2Utils->hClient, &pSec2Utils->semaMthdAuthTagBuf.hVirtMem));
    NV_ASSERT_OK_OR_RETURN(_sec2AllocAndMapBuffer(pSec2Utils, RM_PAGE_SIZE_64K, &pSec2Utils->semaMthdAuthTagBuf));
    return NV_OK;
}

NV_STATUS
sec2utilsConstruct_IMPL
(
    Sec2Utils                    *pSec2Utils,
    OBJGPU                       *pGpu,
    KERNEL_MIG_GPU_INSTANCE      *pKernelMIGGPUInstance
)
{
    NV_STATUS status = NV_OK;
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_INVALID_STATE);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    NV_ASSERT_OR_RETURN(((pConfCompute != NULL) && (pConfCompute->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))),
                          NV_ERR_NOT_SUPPORTED);

    pSec2Utils->pGpu = pGpu;

    // Allocate channel with RM internal client
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    OBJCHANNEL *pChannel = (OBJCHANNEL *) portMemAllocNonPaged(sizeof(OBJCHANNEL));
    NV_ASSERT_OR_RETURN(pChannel != NULL, NV_ERR_INSUFFICIENT_RESOURCES);
    portMemSet(pChannel, 0, sizeof(OBJCHANNEL));

    // Allocate client
    NV_ASSERT_OK_OR_GOTO(status, pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                                         NV01_NULL_OBJECT, NV01_ROOT, &pSec2Utils->hClient,
                                                         sizeof(pSec2Utils->hClient)), cleanup);

    pChannel->hClient = pSec2Utils->hClient;

    NV_ASSERT_OK_OR_GOTO(status, serverGetClientUnderLock(&g_resServ, pChannel->hClient, &pChannel->pRsClient), free_client);

    NV_ASSERT_OK_OR_GOTO(status, clientSetHandleGenerator(pChannel->pRsClient, 1U, ~0U - 1U), free_client);

    pChannel->bClientAllocated = NV_TRUE;
    pChannel->pGpu = pGpu;

    pChannel->deviceId = NV01_NULL_OBJECT;
    pChannel->subdeviceId = NV01_NULL_OBJECT;

    pChannel->pKernelMIGGpuInstance = pKernelMIGGPUInstance;

    pChannel->hVASpaceId = NV01_NULL_OBJECT;
    pChannel->bUseVasForCeCopy = NV_FALSE;

    pChannel->type = SWL_SCRUBBER_CHANNEL;
    pChannel->engineType = RM_ENGINE_TYPE_SEC2;

    pChannel->bSecure = NV_TRUE;

    // Detect if we can enable fast scrub on this channel
    NV_ASSERT_OK_OR_GOTO(status, _sec2GetClass(pGpu, &pSec2Utils->sec2Class), free_client);
    pChannel->sec2Class = pSec2Utils->sec2Class;

    // Set up various channel resources
    NV_ASSERT_OK_OR_GOTO(status, channelSetupIDs(pChannel, pGpu, NV_FALSE, IS_MIG_IN_USE(pGpu)), free_client);

    channelSetupChannelBufferSizes(pChannel);

    // save original values
    NvU32 instLocOverrides4 = pGpu->instLocOverrides4;
    NvU32 instLocOverrides = pGpu->instLocOverrides;

    // Force PB, GPFIFO, notifier and userd to sysmem before allocating channel
    pGpu->instLocOverrides4 = FLD_SET_DRF(_REG_STR_RM, _INST_LOC_4, _CHANNEL_PUSHBUFFER, _NCOH, pGpu->instLocOverrides4);
    pGpu->instLocOverrides  = FLD_SET_DRF(_REG_STR_RM, _INST_LOC, _USERD, _NCOH, pGpu->instLocOverrides);

    pChannel->engineType = NV2080_ENGINE_TYPE_SEC2;

    NV_ASSERT_OK_OR_GOTO(status, channelAllocSubdevice(pGpu, pChannel), free_client);

    pMemoryManager->bScrubChannelSetupInProgress = NV_TRUE;
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, pChannel), free_channel);
    pMemoryManager->bScrubChannelSetupInProgress = NV_FALSE;

    pSec2Utils->hDevice = pChannel->deviceId;
    pSec2Utils->hSubdevice = pChannel->subdeviceId;

    NV_PRINTF(LEVEL_INFO, "Channel alloc successful for Sec2Utils\n");
    pSec2Utils->pChannel = pChannel;

    NV_ASSERT_OK_OR_GOTO(status, memmgrMemUtilsSec2CtxInit_HAL(pGpu, pMemoryManager, pChannel), free_channel);

    // restore original values
    pGpu->instLocOverrides4 = instLocOverrides4;
    pGpu->instLocOverrides = instLocOverrides;

    pSec2Utils->lastSubmittedPayload = 0;
    pSec2Utils->lastCompletedPayload = 0;
    pSec2Utils->authTagPutIndex = 0;
    pSec2Utils->authTagGetIndex = 0;

    NV_ASSERT_OK_OR_GOTO(status, _sec2InitBuffers(pSec2Utils), free_channel);

    NV_ASSERT_OK_OR_GOTO(status, ccslContextInitViaChannel(&pSec2Utils->pCcslCtx, pSec2Utils->hClient, pSec2Utils->hSubdevice, pChannel->channelId), free_channel);

    return status;

free_channel:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->channelId);

free_client:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);

cleanup:
    portMemFree(pChannel);
    return status;
}

void
sec2utilsDestruct_IMPL
(
    Sec2Utils *pSec2Utils
)
{
    OBJCHANNEL *pChannel = pSec2Utils->pChannel;
    OBJGPU *pGpu = pSec2Utils->pGpu;
    MemoryManager *pMemoryManager = NULL;
    RM_API *pRmApi = NULL;

    // Sanity checks
    if ((pGpu == NULL) || (pChannel == NULL))
    {
        NV_PRINTF(LEVEL_WARNING, "Possible double-free of Sec2Utils!\n");
        return;
    }
    else if (pGpu != pChannel->pGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad state during sec2Utils teardown!\n");
        return;
    }

    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    ccslContextClear(pSec2Utils->pCcslCtx);

    if ((pChannel->bClientUserd) && (pChannel->pControlGPFifo != NULL))
    {
        if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)))
        {
            //
            // When PCIE is blocked, mappings should be created, used and torn
            // down when they are used
            //
            NV_PRINTF(LEVEL_ERROR, "Leaked USERD mapping from ceUtils!\n");
        }
        else
        {
            memmgrMemDescEndTransfer(pMemoryManager, pChannel->pUserdMemdesc, TRANSFER_FLAGS_USE_BAR1);
            pChannel->pControlGPFifo = NULL;
        }
    }

    if (pChannel->pbCpuVA != NULL)
    {
        if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)))
        {
            NV_PRINTF(LEVEL_ERROR, "Leaked pushbuffer mapping!\n");
        }
        else
        {
            memmgrMemDescEndTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc, TRANSFER_FLAGS_USE_BAR1);
            pChannel->pbCpuVA = NULL;
        }
    }

    if (pChannel->pTokenFromNotifier != NULL)
    {
        if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)))
        {
            NV_PRINTF(LEVEL_ERROR, "Leaked notifier mapping!\n");
        }
        else
        {
            memmgrMemDescEndTransfer(pMemoryManager, pChannel->pErrNotifierMemdesc, TRANSFER_FLAGS_USE_BAR1);
            pChannel->pTokenFromNotifier = NULL;
        }
    }

    pRmApi->Free(pRmApi, pSec2Utils->hClient, pSec2Utils->scrubMthdAuthTagBuf.hPhysMem);
    pRmApi->Free(pRmApi, pSec2Utils->hClient, pSec2Utils->scrubMthdAuthTagBuf.hVirtMem);

    pRmApi->Free(pRmApi, pSec2Utils->hClient, pSec2Utils->semaMthdAuthTagBuf.hPhysMem);
    pRmApi->Free(pRmApi, pSec2Utils->hClient, pSec2Utils->semaMthdAuthTagBuf.hVirtMem);

    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->channelId);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->errNotifierIdPhys);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->pushBufferId);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->errNotifierIdVirt);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hVASpaceId);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->deviceId);

    // Resource server makes sure no leak can occur
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    portMemFree(pChannel);
}

void
sec2utilsServiceInterrupts_IMPL(Sec2Utils *pSec2Utils)
{
    OBJCHANNEL *pChannel = pSec2Utils->pChannel;

    //
    // FIXME: Bug 2463959: objmemscrub is called with the rmDeviceGpuLock in the
    // heapFree_IMPL->_stdmemPmaFree->pmaFreePages->scrubSubmitPages path.
    // This can result in RM waiting for scrubbing to complete and yielding while holding the
    // rmDeviceGpuLock. This can lead to deadlock.
    // Instead, if the lock is held, service any interrupts on SEC2 to help the engine make progress.
    // Bug 2527660 is filed to remove this change.
    //
    // pChannel is null when PMA scrub requests are handled in vGPU plugin.
    // In this case vGpu plugin allocates scrubber channel in PF domain so
    // above mention deadlock is not present here.
    //
    if ((pChannel != NULL) && (rmDeviceGpuLockIsOwner(pChannel->pGpu->gpuInstance)))
    {
        channelServiceScrubberInterrupts(pChannel);
    }
    else
    {
        osSchedule();
    }
}

static NvU32
_sec2utilsUpdateGetPtr
(
    Sec2Utils *pSec2Utils
)
{
    return channelReadChannelMemdesc(pSec2Utils->pChannel, pSec2Utils->pChannel->authTagBufSemaOffset);
}

static NV_STATUS
_sec2utilsGetNextAuthTagSlot
(
    Sec2Utils *pSec2Utils
)
{
    NvU32 totalSlots = pSec2Utils->scrubMthdAuthTagBuf.size / SHA_256_HASH_SIZE_BYTES;
    NvU32 nextPut = (pSec2Utils->authTagPutIndex + 1) % totalSlots;

    // check if slots have freed up
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;
    gpuSetTimeout(pSec2Utils->pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
    while (nextPut == pSec2Utils->authTagGetIndex)
    {
        status = gpuCheckTimeout(pSec2Utils->pGpu, &timeout);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Timed out waiting for next auth tag buf slot to free up: nextPut = %d get = %d\n",
                nextPut, _sec2utilsUpdateGetPtr(pSec2Utils));
            return status;
        }
        pSec2Utils->authTagGetIndex = _sec2utilsUpdateGetPtr(pSec2Utils);
        osSpinLoop();
    }

    pSec2Utils->authTagPutIndex = nextPut;
    return NV_OK;
}

static NV_STATUS
_sec2utilsSubmitPushBuffer
(
    Sec2Utils        *pSec2Utils,
    OBJCHANNEL       *pChannel,
    NvBool            bInsertFinishPayload,
    NvU32             nextIndex,
    CHANNEL_PB_INFO  *pChannelPbInfo
)
{
    NV_STATUS status = NV_OK;
    NvU32 methodsLength = 0;
    NvU32 putIndex = 0;

    NV_ASSERT_OR_RETURN(pChannelPbInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pChannel->pGpu);
    NvBool bReleaseMapping = NV_FALSE;

    //
    // Use BAR1 if CPU access is allowed, otherwise allocate and init shadow
    // buffer for DMA access
    //
    NvU32 transferFlags = (TRANSFER_FLAGS_USE_BAR1     |
                           TRANSFER_FLAGS_SHADOW_ALLOC |
                           TRANSFER_FLAGS_SHADOW_INIT_MEM);
    NV_PRINTF(LEVEL_INFO, "Actual size of copying to be pushed: %x\n", pChannelPbInfo->size);

    status = channelWaitForFreeEntry(pChannel, &putIndex);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot get putIndex.\n");
        return status;
    }

    if (pChannel->pbCpuVA == NULL)
    {
        pChannel->pbCpuVA = memmgrMemDescBeginTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc,
                                                       transferFlags);
        bReleaseMapping = NV_TRUE;
    }
    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, NV_ERR_GENERIC);

    NV_ASSERT_OK_OR_RETURN(_sec2utilsGetNextAuthTagSlot(pSec2Utils));
    NV_ASSERT_OK_OR_RETURN(channelFillSec2Pb(pChannel, putIndex, bInsertFinishPayload, pChannelPbInfo, pSec2Utils->pCcslCtx,
                                             pSec2Utils->scrubMthdAuthTagBuf.pMemDesc, pSec2Utils->semaMthdAuthTagBuf.pMemDesc,
                                             pSec2Utils->scrubMthdAuthTagBuf.gpuVA, pSec2Utils->authTagPutIndex,
                                             pSec2Utils->semaMthdAuthTagBuf.gpuVA, nextIndex, &methodsLength));

    if (bReleaseMapping)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc, transferFlags);
        pChannel->pbCpuVA = NULL;
    }

    if (methodsLength == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot push methods to channel.\n");
        return NV_ERR_NO_FREE_FIFOS;
    }

    //
    // Pushbuffer can be written in a batch, but GPFIFO and doorbell require
    // careful ordering so we do each write one-by-one
    //
    status = channelFillGpFifo(pChannel, putIndex, methodsLength);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Channel operation failures during memcopy\n");
        return status;
    }

    pChannel->lastSubmittedEntry = putIndex;

    return status;
}


NV_STATUS
sec2utilsMemset_IMPL
(
    Sec2Utils *pSec2Utils,
    SEC2UTILS_MEMSET_PARAMS *pParams
)
{
    OBJCHANNEL *pChannel = pSec2Utils->pChannel;
    NV_STATUS   status = NV_OK;
    RMTIMEOUT   timeout;

    NvU32 pteArraySize;
    NvU64 offset, memsetLength, size, pageGranularity;
    NvBool bContiguous;

    MEMORY_DESCRIPTOR *pMemDesc = pParams->pMemDesc;
    CHANNEL_PB_INFO channelPbInfo = {0};

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memdesc for Sec2Utils memset.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM) ||
        (pMemDesc->pGpu != pSec2Utils->pChannel->pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memory descriptor passed.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    size = memdescGetSize(pMemDesc);
    pteArraySize = memdescGetPteArraySize(pMemDesc, AT_GPU);
    bContiguous = (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS) || (pteArraySize == 1);

    if (pParams->offset >= size)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid offset passed for the memdesc.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pParams->length == 0) || (pParams->length > (size - pParams->offset)))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memset length passed.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Make sure free auth tag buffers are available
    NvU32 totalSlots = pSec2Utils->semaMthdAuthTagBuf.size / SHA_256_HASH_SIZE_BYTES;
    NvU32 nextIndex = (pSec2Utils->lastSubmittedPayload + 1) % totalSlots;
    NvU32 lastCompleted = sec2utilsUpdateProgress(pSec2Utils);
    NvU32 currentIndex = lastCompleted % totalSlots;

    gpuSetTimeout(pSec2Utils->pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
    while (nextIndex == currentIndex)
    {
        status = gpuCheckTimeout(pSec2Utils->pGpu, &timeout);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to finish previous scrub op before re-using method stream auth tag buf: lastCompleted = %d lastSubmitted = %lld\n",
               lastCompleted, pSec2Utils->lastSubmittedPayload);
            return status;
        }
        lastCompleted = sec2utilsUpdateProgress(pSec2Utils);
        currentIndex = lastCompleted % totalSlots;
        osSpinLoop();
    }

    channelPbInfo.payload = pSec2Utils->lastSubmittedPayload + 1;
    pSec2Utils->lastSubmittedPayload = channelPbInfo.payload;

    channelPbInfo.dstAddressSpace = memdescGetAddressSpace(pMemDesc);
    channelPbInfo.dstCpuCacheAttrib = pMemDesc->_cpuCacheAttrib;

    pageGranularity = pMemDesc->pageArrayGranularity;
    memsetLength = pParams->length;
    offset = pParams->offset;

    //
    // We need not just the physical address,
    // but the physical address to be used by the engine
    // sec2utils is only used for AT_GPU
    //

    do
    {
        //
        // Use the memdesc phys addr for calculations, but the pte address for the value
        // programmed into SEC2
        //
        NvU64 dstAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, offset);
        NvU64 maxContigSize = bContiguous ? memsetLength : (pageGranularity - dstAddr % pageGranularity);
        NvU32 memsetSizeContig = (NvU32)NV_MIN(NV_MIN(memsetLength, maxContigSize), NVCBA2_DECRYPT_SCRUB_SIZE_MAX_BYTES);

        NV_PRINTF(LEVEL_INFO, "Sec2Utils Memset dstAddr: %llx,  size: %x\n", dstAddr, memsetSizeContig);

        channelPbInfo.dstAddr = memdescGetPtePhysAddr(pMemDesc, AT_GPU, offset);
        channelPbInfo.size = memsetSizeContig;

        status = _sec2utilsSubmitPushBuffer(pSec2Utils, pChannel, memsetSizeContig == memsetLength, nextIndex, &channelPbInfo);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Cannot submit push buffer for memset.\n");
            return status;
        }

        memsetLength -= memsetSizeContig;
        offset       += memsetSizeContig;
    } while (memsetLength != 0);

    NV_PRINTF(LEVEL_INFO, "Async memset payload returned: 0x%x\n", channelPbInfo.payload);
    pParams->submittedWorkId = channelPbInfo.payload;

    return status;
}

// This function updates pSec2Utils->lastCompletedPayload and handles wrap-around
NvU64
sec2utilsUpdateProgress_IMPL
(
    Sec2Utils *pSec2Utils
)
{
    NV_ASSERT((pSec2Utils != NULL) && (pSec2Utils->pChannel != NULL));

    NvU32 hwCurrentCompletedPayload = 0;
    NvU64 swLastCompletedPayload = pSec2Utils->lastCompletedPayload;

    //
    // Sec2Utils uses 64 bit index to track the work submitted. But HW supports
    // only 32 bit semaphore. The current completed Id is calculated here, based
    // on the lastSubmittedPayload and current HW semaphore value.
    //
    hwCurrentCompletedPayload = READ_CHANNEL_PAYLOAD_SEMA(pSec2Utils->pChannel);

    // No work has been completed since we checked last time
    if (hwCurrentCompletedPayload == (NvU32)swLastCompletedPayload)
    {
        return swLastCompletedPayload;
    }

    // Check for wrap around case. Increment the upper 32 bits
    if (hwCurrentCompletedPayload < (NvU32)swLastCompletedPayload)
    {
        swLastCompletedPayload += 0x100000000ULL;
    }

    // Update lower 32 bits regardless if wrap-around happened
    swLastCompletedPayload &= 0xFFFFFFFF00000000ULL;
    swLastCompletedPayload |= (NvU64)hwCurrentCompletedPayload;

    pSec2Utils->lastCompletedPayload = swLastCompletedPayload;
    return swLastCompletedPayload;
}
