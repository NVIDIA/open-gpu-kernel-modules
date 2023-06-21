/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/ce/kernel_ce.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/mem_mgr/channel_utils.h"
#include "rmapi/rs_utils.h"
#include "utils/nvassert.h"
#include "core/prelude.h"
#include "core/locks.h"
#include "gpu/mem_mgr/ce_utils.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/gpu/mem_mgr/ce_utils_sizes.h"

#include "class/clb0b5.h" // MAXWELL_DMA_COPY_A
#include "class/clc0b5.h" // PASCAL_DMA_COPY_A
#include "class/clc1b5.h" // PASCAL_DMA_COPY_B
#include "class/clc3b5.h" // VOLTA_DMA_COPY_A
#include "class/clc5b5.h" // TURING_DMA_COPY_A
#include "class/clc8b5.h" // HOPPER_DMA_COPY_A
#include "class/clc86f.h" // HOPPER_CHANNEL_GPFIFO_A

#include "class/cl0080.h"

NV_STATUS
ceutilsConstruct_IMPL
(
    CeUtils                      *pCeUtils,
    OBJGPU                       *pGpu,
    KERNEL_MIG_GPU_INSTANCE      *pKernelMIGGPUInstance,
    NV0050_ALLOCATION_PARAMETERS *pAllocParams
)
{
    NV_STATUS status = NV_OK;
    NvU64 allocFlags = pAllocParams->flags;
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_INVALID_STATE);

    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    // Allocate channel with RM internal client
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RmClient *pClient;

    OBJCHANNEL *pChannel = (OBJCHANNEL *) portMemAllocNonPaged(sizeof(OBJCHANNEL));
    if (pChannel == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    portMemSet(pChannel, 0, sizeof(OBJCHANNEL));

    if (pCeUtils->hClient == NV01_NULL_OBJECT)
    {
        // Allocate client
        status = pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                         NV01_NULL_OBJECT, NV01_ROOT, &pCeUtils->hClient,
                                         sizeof(pCeUtils->hClient));
        NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
    }

    pChannel->hClient = pCeUtils->hClient;
    pClient = serverutilGetClientUnderLock(pChannel->hClient);
    NV_ASSERT_OR_GOTO(pClient != NULL, free_client);

    status = serverGetClientUnderLock(&g_resServ, pChannel->hClient, &pChannel->pRsClient);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    status = clientSetHandleGenerator(staticCast(pClient, RsClient), 1U, ~0U - 1U);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    pChannel->bClientAllocated = NV_TRUE;
    pChannel->pGpu = pGpu;

    pChannel->deviceId = pCeUtils->hDevice;
    pChannel->subdeviceId = pCeUtils->hSubdevice;

    pChannel->pKernelMIGGpuInstance = pKernelMIGGPUInstance;

    // We'll allocate new VAS for now. Sharing client VAS will be added later
    pChannel->hVASpaceId = NV01_NULL_OBJECT;
    pChannel->bUseVasForCeCopy = FLD_TEST_DRF(0050_CEUTILS, _FLAGS, _VIRTUAL_MODE, _TRUE, allocFlags);

    // Detect if we can enable fast scrub on this channel
    status = memmgrMemUtilsGetCopyEngineClass_HAL(pGpu, pMemoryManager, &pCeUtils->hTdCopyClass);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_channel);

    if (((pCeUtils->hTdCopyClass == HOPPER_DMA_COPY_A)
        ) && !pChannel->bUseVasForCeCopy)
    {
        pChannel->type = FAST_SCRUBBER_CHANNEL;
        NV_PRINTF(LEVEL_INFO, "Enabled fast scrubber in construct.\n");
    }

    // Set up various channel resources
    status = channelSetupIDs(pChannel, pGpu, pChannel->bUseVasForCeCopy, bMIGInUse);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    channelSetupChannelBufferSizes(pChannel);

    status = memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, pChannel);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_channel);

    NV_PRINTF(LEVEL_INFO, "Channel alloc successful for ceUtils\n");
    pCeUtils->pChannel = pChannel;

    // Allocate CE states
    status = memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, pChannel);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_channel);

    pCeUtils->pGpu = pGpu;

    return status;

free_channel:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->channelId);

    if (pAllocParams->hVaspace != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hVASpaceId);
    }
free_client:
    if (FLD_TEST_DRF(0050_CEUTILS, _FLAGS, _EXTERNAL, _FALSE, allocFlags))
    {
        // If client allocated client, we should not free it in RM
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    }

cleanup:
    portMemFree(pChannel);
    return status;
}

void
ceutilsDestruct_IMPL
(
    CeUtils *pCeUtils
)
{
    OBJCHANNEL *pChannel = pCeUtils->pChannel;
    OBJGPU *pGpu = pCeUtils->pGpu;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Sanity checks
    if ((pGpu == NULL) || (pChannel == NULL))
    {
        NV_PRINTF(LEVEL_WARNING, "Possible double-free of CeUtils!\n");
        return;
    }
    else if (pGpu != pChannel->pGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad state during ceUtils teardown!\n");
        return;
    }

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
ceutilsServiceInterrupts_IMPL(CeUtils *pCeUtils)
{
    OBJCHANNEL *pChannel = pCeUtils->pChannel;

    //
    // FIXME: Bug 2463959: objmemscrub is called with the rmDeviceGpuLock in the
    // heapFree_IMPL->_stdmemPmaFree->pmaFreePages->scrubSubmitPages path.
    // Yielding while holding the rmDeviceGpuLock can lead to deadlock. Instead,
    // if the lock is held, service any interrupts on the owned CE to make progress.
    // Bug 2527660 is filed to remove this change.
    //
    // pChannel is null when PMA scrub requests are handled in vGPU plugin.
    // In this case vGpu plugin allocates scrubber channel in PF domain so
    // above mention deadlock is not present here.
    //
    if ((pChannel != NULL) && (rmDeviceGpuLockIsOwner(pChannel->pGpu->gpuInstance)))
    {
        Intr *pIntr = GPU_GET_INTR(pChannel->pGpu);
        intrServiceStallSingle_HAL(pChannel->pGpu, pIntr, MC_ENGINE_IDX_CE(pChannel->ceId), NV_FALSE);
    }
    else
    {
        osSchedule();
    }
}


static NvBool
_ceUtilsFastScrubEnabled
(
    POBJCHANNEL      pChannel,
    CHANNEL_PB_INFO *pChannelPbInfo
)
{
    OBJGPU *pGpu = pChannel->pGpu;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (!memmgrIsFastScrubberEnabled(pMemoryManager))
    {
        return NV_FALSE;
    }

    //
    // Enable the  memory fast scrubbing only when
    // Channel was allocated as fastScrub channel
    // We are doing a memset operation
    // Memset pattern is 0
    // DstPhysMode.target == LOCAL_FB
    // Address is 4KB aligned
    // LineLength is 4KB aligned
    //

    return ((pChannel->type == FAST_SCRUBBER_CHANNEL) && 
            (!pChannelPbInfo->bCeMemcopy) &&
            (pChannelPbInfo->pattern == 0) &&
            (pChannelPbInfo->dstAddressSpace == ADDR_FBMEM) &&
            (NV_IS_ALIGNED64(pChannelPbInfo->dstAddr, MEMUTIL_SCRUB_OFFSET_ALIGNMENT)) &&
            (NV_IS_ALIGNED(pChannelPbInfo->size, MEMUTIL_SCRUB_LINE_LENGTH_ALIGNMENT)));
}


//
// Helper to deal with CE_MAX_BYTES_PER_LINE
// This function may modify some fileds in pChannelPbInfo
//
static NV_STATUS
_ceutilsSubmitPushBuffer
(
    POBJCHANNEL       pChannel,
    NvBool            bPipelined,
    NvBool            bInsertFinishPayload,
    CHANNEL_PB_INFO * pChannelPbInfo
)
{
    NV_STATUS status = NV_OK;
    NvU32 methodsLength, putIndex = 0;

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

    if (_ceUtilsFastScrubEnabled(pChannel, pChannelPbInfo))
    {
        methodsLength = channelFillPbFastScrub(pChannel, putIndex, bPipelined, bInsertFinishPayload, pChannelPbInfo);
    }
    else
    {
        methodsLength = channelFillPb(pChannel, putIndex, bPipelined, bInsertFinishPayload, pChannelPbInfo);
    }

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
ceutilsMemset_IMPL
(
    CeUtils *pCeUtils,
    CEUTILS_MEMSET_PARAMS *pParams
)
{
    OBJCHANNEL *pChannel = pCeUtils->pChannel;
    NV_STATUS   status = NV_OK;

    NvU32 pteArraySize;
    NvU64 offset, memsetLength, size, pageGranularity;
    NvBool bContiguous;

    MEMORY_DESCRIPTOR *pMemDesc = pParams->pMemDesc;
    CHANNEL_PB_INFO channelPbInfo = {0};

    NvBool bPipelined = pParams->flags & NV0050_CTRL_MEMSET_FLAGS_PIPELINED;

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memdesc for CeUtils memset.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM) ||
        (pMemDesc->pGpu != pCeUtils->pChannel->pGpu))
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

    NV_PRINTF(LEVEL_INFO, "CeUtils Args to memset - offset: %llx, size: %llx \n",
              pParams->offset, pParams->length);

    if ((pParams->length == 0) || (pParams->length > (size - pParams->offset)))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memset length passed.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    channelPbInfo.bCeMemcopy = NV_FALSE;
    channelPbInfo.payload = pCeUtils->lastSubmittedPayload + 1;
    pCeUtils->lastSubmittedPayload = channelPbInfo.payload;

    channelPbInfo.pattern = pParams->pattern;
    channelPbInfo.dstAddressSpace = memdescGetAddressSpace(pMemDesc);
    channelPbInfo.dstCpuCacheAttrib = pMemDesc->_cpuCacheAttrib;

    pageGranularity = pMemDesc->pageArrayGranularity;
    memsetLength = pParams->length;
    offset = pParams->offset;

    do
    {
        NvU64 maxContigSize = bContiguous ? memsetLength : (pageGranularity - offset % pageGranularity);
        NvU32 memsetSizeContig = (NvU32)NV_MIN(NV_MIN(memsetLength, maxContigSize), CE_MAX_BYTES_PER_LINE);

        channelPbInfo.dstAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, offset);

        NV_PRINTF(LEVEL_INFO, "CeUtils Memset dstAddr: %llx,  size: %x\n",
                  channelPbInfo.dstAddr, memsetSizeContig);

        channelPbInfo.size = memsetSizeContig;
        status = _ceutilsSubmitPushBuffer(pChannel, bPipelined, memsetSizeContig == memsetLength, &channelPbInfo);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Cannot submit push buffer for memset.\n");
            return status;
        }

         // Allow _LAUNCH_DMA methods that belong to the same memset operation to be pipelined after each other, as there are no dependencies
        bPipelined = NV_TRUE;

        memsetLength -= memsetSizeContig;
        offset       += memsetSizeContig;
    } while (memsetLength != 0);

    if (pParams->flags & NV0050_CTRL_MEMSET_FLAGS_ASYNC)
    {
        NV_PRINTF(LEVEL_INFO, "Async memset payload returned: 0x%x\n", channelPbInfo.payload);
        pParams->submittedWorkId = channelPbInfo.payload;
    }
    else
    {
        // Check semaProgress and then timeout
        status = channelWaitForFinishPayload(pChannel, channelPbInfo.payload);
        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Work was done from RM PoV lastSubmitted = 0x%x\n", channelPbInfo.payload);
        }
    }

    return status;
}

NV_STATUS
ceutilsMemcopy_IMPL
(
    CeUtils *pCeUtils,
    CEUTILS_MEMCOPY_PARAMS *pParams
)
{
    OBJCHANNEL *pChannel = pCeUtils->pChannel;
    NV_STATUS   status = NV_OK;

    NvU64  srcSize, dstSize, copyLength, srcPageGranularity, dstPageGranularity;
    NvBool bSrcContig, bDstContig;

    CHANNEL_PB_INFO channelPbInfo  = {0};
    MEMORY_DESCRIPTOR *pDstMemDesc = pParams->pDstMemDesc;
    MEMORY_DESCRIPTOR *pSrcMemDesc = pParams->pSrcMemDesc;

    NvU64 length = pParams->length;
    NvU64 srcOffset = pParams->srcOffset;
    NvU64 dstOffset = pParams->dstOffset;

    NvBool bPipelined = pParams->flags & NV0050_CTRL_MEMCOPY_FLAGS_PIPELINED;

    // Validate params
    if ((pSrcMemDesc == NULL) || (pDstMemDesc == NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Src/Dst Memory descriptor should be valid.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((memdescGetAddressSpace(pSrcMemDesc) != ADDR_FBMEM) &&
        (memdescGetAddressSpace(pDstMemDesc) != ADDR_FBMEM))
    {
        NV_PRINTF(LEVEL_ERROR, "Either Dst or Src memory should be in vidmem.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pSrcMemDesc->pGpu != pCeUtils->pChannel->pGpu) ||
        (pDstMemDesc->pGpu != pCeUtils->pChannel->pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "CeUtils does not support p2p copies right now. \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    srcSize = memdescGetSize(pSrcMemDesc);
    dstSize = memdescGetSize(pDstMemDesc);

    if ((srcOffset >= srcSize) || (dstOffset >= dstSize))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid offset passed for the src/dst memdesc.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((length == 0) ||
        (srcOffset + length > srcSize) || (dstOffset + length > dstSize))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid memcopy length.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    channelPbInfo.bCeMemcopy = NV_TRUE;
    channelPbInfo.payload = pCeUtils->lastSubmittedPayload + 1;
    pCeUtils->lastSubmittedPayload = channelPbInfo.payload;

    channelPbInfo.srcAddressSpace = memdescGetAddressSpace(pSrcMemDesc);
    channelPbInfo.dstAddressSpace = memdescGetAddressSpace(pDstMemDesc);

    channelPbInfo.srcCpuCacheAttrib = pSrcMemDesc->_cpuCacheAttrib;
    channelPbInfo.dstCpuCacheAttrib = pDstMemDesc->_cpuCacheAttrib;

    srcPageGranularity = pSrcMemDesc->pageArrayGranularity;
    dstPageGranularity = pDstMemDesc->pageArrayGranularity;
    bSrcContig = memdescGetContiguity(pSrcMemDesc, AT_GPU);
    bDstContig = memdescGetContiguity(pDstMemDesc, AT_GPU);

    copyLength = length;

    do
    {
        //
        // This algorithm finds the maximum contig region from both src and dst
        // for each copy and iterate until we submitted the whole range to CE
        //
        NvU64 maxContigSizeSrc = bSrcContig ? copyLength : (srcPageGranularity - srcOffset % srcPageGranularity);
        NvU64 maxContigSizeDst = bDstContig ? copyLength : (dstPageGranularity - dstOffset % dstPageGranularity);
        NvU32 copySizeContig = (NvU32)NV_MIN(NV_MIN(copyLength, NV_MIN(maxContigSizeSrc, maxContigSizeDst)), CE_MAX_BYTES_PER_LINE);

        channelPbInfo.srcAddr = memdescGetPhysAddr(pSrcMemDesc, AT_GPU, srcOffset);
        channelPbInfo.dstAddr = memdescGetPhysAddr(pDstMemDesc, AT_GPU, dstOffset);

        NV_PRINTF(LEVEL_INFO, "CeUtils Memcopy dstAddr: %llx, srcAddr: %llx, size: %x\n",
                  channelPbInfo.dstAddr, channelPbInfo.srcAddr, copySizeContig);

        channelPbInfo.size = copySizeContig;
        status = _ceutilsSubmitPushBuffer(pChannel, bPipelined, copySizeContig == copyLength, &channelPbInfo);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Cannot submit push buffer for memcopy.\n");
            return status;
        }

         // Allow _LAUNCH_DMA methods that belong to the same copy operation to be pipelined after each other, as there are no dependencies
        bPipelined = NV_TRUE;

        copyLength -= copySizeContig;
        srcOffset  += copySizeContig;
        dstOffset  += copySizeContig;
    } while (copyLength != 0);

    if (pParams->flags & NV0050_CTRL_MEMSET_FLAGS_ASYNC)
    {
        NV_PRINTF(LEVEL_INFO, "Async memset payload returned: 0x%x\n", channelPbInfo.payload);
        pParams->submittedWorkId = channelPbInfo.payload;
    }
    else
    {
        // Check semaProgress and then timeout
        status = channelWaitForFinishPayload(pChannel, channelPbInfo.payload);
        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Work was done from RM PoV lastSubmitted = 0x%x\n", channelPbInfo.payload);
        }
    }

    return status;
}


// This function updates pCeUtils->lastCompletedPayload and handles wrap-around
NvU64
ceutilsUpdateProgress_IMPL
(
    CeUtils *pCeUtils
)
{
    NV_ASSERT((pCeUtils != NULL) && (pCeUtils->pChannel != NULL));

    NvU32 hwCurrentCompletedPayload = 0;
    NvU64 swLastCompletedPayload = pCeUtils->lastCompletedPayload;

    //
    // CeUtils uses 64 bit index to track the work submitted. But HW supports
    // only 32 bit semaphore. The current completed Id is calculated here, based
    // on the lastSubmittedPayload and current HW semaphore value.
    //
    hwCurrentCompletedPayload = READ_CHANNEL_PAYLOAD_SEMA(pCeUtils->pChannel);

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

    pCeUtils->lastCompletedPayload = swLastCompletedPayload;
    return swLastCompletedPayload;
}

#if defined(DEBUG) || defined (DEVELOP)
NV_STATUS
ceutilsapiCtrlCmdCheckProgress_IMPL
(
    CeUtilsApi *pCeUtilsApi,
    NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams
)
{
    if (pParams->submittedWorkId <= ceutilsUpdateProgress(pCeUtilsApi->pCeUtils))
    {
        pParams->result = NV0050_CTRL_CHECK_PROGRESS_RESULT_FINISHED;
    }

    return NV_OK;
}

NV_STATUS
ceutilsapiConstruct_IMPL
(
    CeUtilsApi                   *pCeUtilsApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV0050_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;

    if (FLD_TEST_DRF(0050_CEUTILS, _FLAGS, _EXTERNAL, _TRUE, pAllocParams->flags))
    {
        NV_PRINTF(LEVEL_ERROR, "CeUtils: unsupported flags = 0x%llx\n", pAllocParams->flags);
        return NV_ERR_NOT_SUPPORTED;
    }

    return objCreate(&pCeUtilsApi->pCeUtils, pCeUtilsApi, CeUtils, GPU_RES_GET_GPU(pCeUtilsApi), NULL, pAllocParams);
}

void
ceutilsapiDestruct_IMPL
(
    CeUtilsApi *pCeUtilsApi
)
{
    objDelete(pCeUtilsApi->pCeUtils);
}

NV_STATUS
ceutilsapiCtrlCmdMemset_IMPL
(
    CeUtilsApi *pCeUtilsApi,
    NV0050_CTRL_MEMSET_PARAMS *pParams
)
{
    NV_STATUS          status = NV_OK;
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pCeUtilsApi);
    RsResourceRef     *pPhysmemRef;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    CEUTILS_MEMSET_PARAMS internalParams = {0};

    if (pParams->hMemory == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = serverutilGetResourceRef(hClient, pParams->hMemory, &pPhysmemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get resource in resource server for physical memory handle.\n");
        return status;
    }
    pMemDesc = (dynamicCast(pPhysmemRef->pResource, Memory))->pMemDesc;

    internalParams.pMemDesc = pMemDesc;
    internalParams.offset = pParams->offset;
    internalParams.length = pParams->length;
    internalParams.pattern = pParams->pattern;
    internalParams.flags = pParams->flags;

    status = ceutilsMemset(pCeUtilsApi->pCeUtils, &internalParams);
    if (status == NV_OK)
    {
        pParams->submittedWorkId = internalParams.submittedWorkId;
    }

    return status;
}

NV_STATUS
ceutilsapiCtrlCmdMemcopy_IMPL
(
    CeUtilsApi *pCeUtilsApi,
    NV0050_CTRL_MEMCOPY_PARAMS *pParams
)
{
    NV_STATUS          status = NV_OK;
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pCeUtilsApi);
    RsResourceRef     *pSrcPhysmemRef;
    RsResourceRef     *pDstPhysmemRef;
    MEMORY_DESCRIPTOR *pSrcMemDesc = NULL;
    MEMORY_DESCRIPTOR *pDstMemDesc = NULL;
    CEUTILS_MEMCOPY_PARAMS internalParams = {0};

    if ((pParams->hSrcMemory == 0) || (pParams->hDstMemory == 0))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = serverutilGetResourceRef(hClient, pParams->hDstMemory, &pDstPhysmemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get resource in resource server for physical memory handle.\n");
        return status;
    }
    pDstMemDesc = (dynamicCast(pDstPhysmemRef->pResource, Memory))->pMemDesc;

    status = serverutilGetResourceRef(hClient, pParams->hSrcMemory, &pSrcPhysmemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get resource in resource server for physical memory handle.\n");
        return status;
    }
    pSrcMemDesc = (dynamicCast(pSrcPhysmemRef->pResource, Memory))->pMemDesc;

    internalParams.pSrcMemDesc = pSrcMemDesc;
    internalParams.pDstMemDesc = pDstMemDesc;
    internalParams.srcOffset = pParams->srcOffset;
    internalParams.dstOffset = pParams->dstOffset;
    internalParams.length = pParams->length;
    internalParams.flags = pParams->flags;

    status = ceutilsMemcopy(pCeUtilsApi->pCeUtils, &internalParams);
    if (status == NV_OK)
    {
        pParams->submittedWorkId = internalParams.submittedWorkId;
    }

    return status;
}
#endif // defined(DEBUG) || defined (DEVELOP)
