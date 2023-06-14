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

#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "utils/nvassert.h"
#include "core/locks.h"

#include "kernel/gpu/mem_mgr/ce_utils_sizes.h"
#include "kernel/gpu/mem_mgr/channel_utils.h"


/* Static helper functions */
static void channelPushAddress(OBJCHANNEL *pChannel, CHANNEL_PB_INFO *pChannelPbInfo, NvU32 **ppPtr);
static NvU32 channelPushMemoryProperties(OBJCHANNEL *pChannel, CHANNEL_PB_INFO *pChannelPbInfo, NvU32 **ppPtr);
static void channelPushMethod(OBJCHANNEL *pChannel, CHANNEL_PB_INFO *pChannelPbInfo,
                              NvBool bPipelined, NvBool bInsertFinishPayload,
                              NvU32 launchType, NvU32 semaValue, NvU32 **ppPtr);

/* Public APIs */
NV_STATUS
channelSetupIDs
(
    OBJCHANNEL *pChannel,
    OBJGPU     *pGpu,
    NvBool      bUseVasForCeCopy,
    NvBool      bMIGInUse
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->physMemId));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->channelId));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->errNotifierIdVirt));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->errNotifierIdPhys));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->copyObjectId));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->eventId));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->pushBufferId));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->doorbellRegionHandle));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverutilGenResourceHandle(pChannel->hClient, &pChannel->hUserD));

    // For physical CE channels, we will use RM internal VAS to map channel buffers
    NV_ASSERT(pChannel->hVASpaceId == NV01_NULL_OBJECT);

    if (bUseVasForCeCopy || (IS_GSP_CLIENT(pGpu) && bMIGInUse))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              serverutilGenResourceHandle(pChannel->hClient, &pChannel->hVASpaceId));
    }

    return NV_OK;
}

void
channelSetupChannelBufferSizes
(
    OBJCHANNEL *pChannel
)
{
    NV_ASSERT_OR_RETURN_VOID(pChannel != NULL);
    NV_ASSERT_OR_RETURN_VOID(pChannel->type < MAX_CHANNEL_TYPE);

    pChannel->channelNotifierSize       = CE_CHANNEL_NOTIFIER_SIZE;
    pChannel->channelNumGpFifioEntries  = CE_NUM_COPY_BLOCKS;
    pChannel->methodSizePerBlock        = CE_METHOD_SIZE_PER_BLOCK;

    if (pChannel->type == FAST_SCRUBBER_CHANNEL)
    {
        pChannel->methodSizePerBlock = FAST_SCRUBBER_METHOD_SIZE_PER_BLOCK;
    }

    // These sizes depend on size of each block of pushbuffer methods
    pChannel->channelPbSize = pChannel->methodSizePerBlock * CE_NUM_COPY_BLOCKS;
    pChannel->channelSize = pChannel->channelPbSize + CE_GPFIFO_SIZE + CE_CHANNEL_SEMAPHORE_SIZE;
    pChannel->semaOffset = pChannel->channelPbSize + CE_GPFIFO_SIZE;
    pChannel->finishPayloadOffset = pChannel->semaOffset + 4;
}

NvU32
channelReadChannelMemdesc
(
    OBJCHANNEL *pChannel,
    NvU32       offset
)
{
    NV_ASSERT_OR_RETURN(pChannel != NULL, 0);
    NV_ASSERT_OR_RETURN(pChannel->pGpu != NULL, 0);

    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pChannel->pGpu);
    NvBool bReleaseMapping = NV_FALSE;
    NvU32 result = 0;

    //
    // Use BAR1 if CPU access is allowed, otherwise allocate and init shadow
    // buffer for DMA access
    //
    NvU32 transferFlags = (TRANSFER_FLAGS_USE_BAR1     |
                           TRANSFER_FLAGS_SHADOW_ALLOC |
                           TRANSFER_FLAGS_SHADOW_INIT_MEM);

    if (pChannel->pbCpuVA == NULL)
    {
        pChannel->pbCpuVA = memmgrMemDescBeginTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc,
                                                       transferFlags);
        bReleaseMapping = NV_TRUE;
    }

    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, 0);

    result = MEM_RD32((NvU8*)pChannel->pbCpuVA + offset);

    if (bReleaseMapping)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc, transferFlags);
        pChannel->pbCpuVA = NULL;
    }

    return result;
}

NV_STATUS
channelWaitForFinishPayload
(
    OBJCHANNEL *pChannel,
    NvU64       targetPayload
)
{
    NV_ASSERT_OR_RETURN(pChannel != NULL, NV_ERR_INVALID_STATE);

    OBJGPU *pGpu = pChannel->pGpu;
    RMTIMEOUT timeout;
    NV_STATUS status = NV_OK;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
    while (1) 
    {
        if (READ_CHANNEL_PAYLOAD_SEMA(pChannel) >= targetPayload)
        {
            break;
        }
        
        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            break;
        }

        if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        {
            Intr *pIntr = GPU_GET_INTR(pGpu);
            intrServiceStallSingle_HAL(pGpu, pIntr, MC_ENGINE_IDX_CE(pChannel->ceId), NV_FALSE);
        }
        else
        {
            osSchedule();
        }
    }

    return status;
}


/** helper function which waits for a PB & GPFIO entry to be read by HOST.
 *  After the HOST reads GPFIFO and PB entry, the semaphore will be released.
 */
NV_STATUS
channelWaitForFreeEntry
(
    OBJCHANNEL *pChannel,
    NvU32      *pPutIndex
)
{
    NV_ASSERT_OR_RETURN(pChannel != NULL, NV_ERR_INVALID_STATE);

    OBJGPU *pGpu = pChannel->pGpu;
    RMTIMEOUT timeout;
    NV_STATUS status = NV_OK;

    NvU32 putIndex = 0;
    NvU32 getIndex = 0;

    putIndex = (pChannel->lastSubmittedEntry + 1) % pChannel->channelNumGpFifioEntries;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
    do
    {
        getIndex = READ_CHANNEL_PB_SEMA(pChannel);

        NV_PRINTF(LEVEL_INFO, "Get Index: %x, PayloadIndex: %x\n", getIndex,
                              READ_CHANNEL_PAYLOAD_SEMA(pChannel));

        if (getIndex != putIndex)
        {
            break;
        }
        
        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            break;
        }
        
        if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        {
            Intr *pIntr = GPU_GET_INTR(pGpu);
            intrServiceStallSingle_HAL(pGpu, pIntr, MC_ENGINE_IDX_CE(pChannel->ceId), NV_FALSE);
        }
        else
        {
            osSchedule();
        }
    } while(1);

    if (status == NV_OK)
    {
        NV_ASSERT_OR_RETURN(pPutIndex != NULL, NV_ERR_INVALID_STATE);
        *pPutIndex = putIndex;
    }

    return status;
}


/** helper function to fill GPFIFO entry with a pushbuffer segment. and kick
 *  off the executiion by HOST.
 */
NV_STATUS
channelFillGpFifo
(
    OBJCHANNEL *pChannel,
    NvU32       putIndex,
    NvU32       methodsLength
)
{
    NvU32  *pGpEntry;
    NvU32   GpEntry0;
    NvU32   GpEntry1;
    NvU64   pbPutOffset;
    OBJGPU *pGpu;
    KernelBus *pKernelBus;
    MemoryManager *pMemoryManager;
    NvBool bReleaseMapping = NV_FALSE;

    //
    // Use BAR1 if CPU access is allowed, otherwise allocate and init shadow
    // buffer for DMA access
    //
    NvU32 transferFlags = (TRANSFER_FLAGS_USE_BAR1     |
                           TRANSFER_FLAGS_SHADOW_ALLOC |
                           TRANSFER_FLAGS_SHADOW_INIT_MEM);

    NV_ASSERT_OR_RETURN(putIndex < pChannel->channelNumGpFifioEntries, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pChannel != NULL, NV_ERR_INVALID_STATE);

    pGpu = pChannel->pGpu;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);

    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (pChannel->pbCpuVA == NULL)
    {
        pChannel->pbCpuVA = memmgrMemDescBeginTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc,
                                                       transferFlags);
        bReleaseMapping = NV_TRUE;
    }

    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, NV_ERR_GENERIC);

    pbPutOffset = (pChannel->pbGpuVA + (putIndex * pChannel->methodSizePerBlock));

    GpEntry0 = DRF_DEF(906F, _GP_ENTRY0, _NO_CONTEXT_SWITCH, _FALSE) | 
               DRF_NUM(906F, _GP_ENTRY0, _GET, NvU64_LO32(pbPutOffset) >> 2);

    GpEntry1 = DRF_NUM(906F, _GP_ENTRY1, _GET_HI, NvU64_HI32(pbPutOffset)) | 
               DRF_NUM(906F, _GP_ENTRY1, _LENGTH, methodsLength >> 2) |
               DRF_DEF(906F, _GP_ENTRY1, _LEVEL, _MAIN);

    pGpEntry = (NvU32 *)(((NvU8 *)pChannel->pbCpuVA) + pChannel->channelPbSize + 
                (pChannel->lastSubmittedEntry * NV906F_GP_ENTRY__SIZE));

    MEM_WR32(&pGpEntry[0], GpEntry0);
    MEM_WR32(&pGpEntry[1], GpEntry1);

    if (bReleaseMapping)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pChannel->pChannelBufferMemdesc, 
                                 transferFlags);
        pChannel->pbCpuVA = NULL;
    }

    //  need to flush WRC buffer
    osFlushCpuWriteCombineBuffer();

    // write GP put
    if (pChannel->pControlGPFifo == NULL)
    {
        pChannel->pControlGPFifo = 
            (void *)memmgrMemDescBeginTransfer(pMemoryManager, pChannel->pUserdMemdesc,
                                               transferFlags);
        NV_ASSERT_OR_RETURN(pChannel->pControlGPFifo != NULL, NV_ERR_INVALID_STATE);
        bReleaseMapping = NV_TRUE;
    }

    MEM_WR32(&pChannel->pControlGPFifo->GPPut, putIndex);

    if (bReleaseMapping)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pChannel->pUserdMemdesc, transferFlags);
        pChannel->pControlGPFifo = NULL;
    }

    osFlushCpuWriteCombineBuffer();
    
    //
    // On some architectures, if doorbell is mapped via bar0, we need to send
    // an extra flush
    //
    if (kbusFlushPcieForBar0Doorbell_HAL(pGpu, pKernelBus) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Busflush failed in _scrubFillGpFifo\n");
        return NV_ERR_GENERIC;
    }

    //
    // removing the FIFO Lite Mode handling
    // Refer older _ceChannelUpdateGpFifo_GF100 code for implementation
    //

    // Update doorbell with work submission token
    if (pChannel->bUseDoorbellRegister)
    {
        if (pChannel->pTokenFromNotifier == NULL)
        {
            NvU8 *pErrNotifierCpuVA = 
                (void *)memmgrMemDescBeginTransfer(pMemoryManager, 
                    pChannel->pErrNotifierMemdesc, transferFlags);

            NV_ASSERT_OR_RETURN(pErrNotifierCpuVA != NULL, NV_ERR_INVALID_STATE);

            pChannel->pTokenFromNotifier =
                (NvNotification *)(pErrNotifierCpuVA +
                               (NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN *
                                sizeof(NvNotification)));
            bReleaseMapping = NV_TRUE;
        }

        // Use the token from notifier memory for VM migration support.
        MEM_WR32(pChannel->pDoorbellRegisterOffset, 
                 MEM_RD32(&(pChannel->pTokenFromNotifier->info32)));

        if (bReleaseMapping)
        {
            memmgrMemDescEndTransfer(pMemoryManager, pChannel->pErrNotifierMemdesc, transferFlags);
            pChannel->pTokenFromNotifier = NULL;
        }
    }

    return NV_OK;
}

NvU32
channelFillPbFastScrub
(
    OBJCHANNEL      *pChannel,
    NvU32            putIndex, 
    NvBool           bPipelined,
    NvBool           bInsertFinishPayload,
    CHANNEL_PB_INFO *pChannelPbInfo
)
{
    NvU32   pipelinedValue = 0;
    NvU32   flushValue     = 0;
    NvU32  *pPtr           = (NvU32 *)((NvU8*)pChannel->pbCpuVA + (putIndex * pChannel->methodSizePerBlock));
    NvU32  *pStartPtr      = pPtr;
    NvU32   semaValue      = 0;
    NvU32   data           = 0;
    NvU64   pSemaAddr      = 0;

    NV_PRINTF(LEVEL_INFO, "PutIndex: %x, PbOffset: %x\n", putIndex,
               putIndex * pChannel->methodSizePerBlock);
    // SET OBJECT
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC86F_SET_OBJECT, pChannel->classEngineID);

    // Set Pattern for Memset
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_SET_REMAP_CONST_A, pChannelPbInfo->pattern);
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_SET_REMAP_CONST_B, pChannelPbInfo->pattern);

    // Set Component Size to 1
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_SET_REMAP_COMPONENTS,
                  DRF_DEF(C8B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_A)          |
                  DRF_DEF(C8B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_B)          |
                  DRF_DEF(C8B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, _ONE)     |
                  DRF_DEF(C8B5, _SET_REMAP_COMPONENTS, _NUM_DST_COMPONENTS, _ONE));

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_SET_DST_PHYS_MODE,
                  DRF_DEF(C8B5, _SET_DST_PHYS_MODE, _TARGET, _LOCAL_FB));

    semaValue = (bInsertFinishPayload) ?
        DRF_DEF(C8B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE) : 0;

    if (bPipelined)
        pipelinedValue = DRF_DEF(C8B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PIPELINED);
    else
        pipelinedValue = DRF_DEF(C8B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _NON_PIPELINED);

    if (bInsertFinishPayload)
        flushValue = DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE);
    else
        flushValue = DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _FALSE);

    NV_PUSH_INC_2U(RM_SUBCHANNEL, NVC8B5_OFFSET_OUT_UPPER,
                   DRF_NUM(C8B5, _OFFSET_OUT_UPPER, _UPPER, NvU64_HI32(pChannelPbInfo->dstAddr)),
                   NVC8B5_OFFSET_OUT_LOWER,
                   DRF_NUM(C8B5, _OFFSET_OUT_LOWER, _VALUE,NvU64_LO32(pChannelPbInfo->dstAddr)));

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_LINE_LENGTH_IN, pChannelPbInfo->size);

    if (semaValue)
    {
        NV_PUSH_INC_3U(RM_SUBCHANNEL, NVC8B5_SET_SEMAPHORE_A,
            DRF_NUM(C8B5, _SET_SEMAPHORE_A, _UPPER, NvU64_HI32(pChannel->pbGpuVA + pChannel->finishPayloadOffset)),
            NVC8B5_SET_SEMAPHORE_B,
            DRF_NUM(C8B5, _SET_SEMAPHORE_B, _LOWER, NvU64_LO32(pChannel->pbGpuVA + pChannel->finishPayloadOffset)),
            NVC8B5_SET_SEMAPHORE_PAYLOAD,
            pChannelPbInfo->payload);
    }

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_SET_MEMORY_SCRUB_PARAMETERS,
                       DRF_DEF(C8B5, _SET_MEMORY_SCRUB_PARAMETERS, _DISCARDABLE, _FALSE));

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC8B5_LAUNCH_DMA,
            DRF_DEF(C8B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH)    |
            DRF_DEF(C8B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH)    |
            DRF_DEF(C8B5, _LAUNCH_DMA, _REMAP_ENABLE, _FALSE)         |
            DRF_DEF(C8B5, _LAUNCH_DMA, _MULTI_LINE_ENABLE, _FALSE)    |
            DRF_DEF(C8B5, _LAUNCH_DMA, _MEMORY_SCRUB_ENABLE, _TRUE)   |
            DRF_DEF(C8B5, _LAUNCH_DMA, _DISABLE_PLC, _TRUE)           |
            DRF_DEF(C8B5, _LAUNCH_DMA, _DST_TYPE, _PHYSICAL)          |
            DRF_DEF(C8B5, _LAUNCH_DMA, _SRC_TYPE, _PHYSICAL)          |
            pipelinedValue                                            |
            flushValue                                                |
            semaValue);

    //
    // This should always be at the bottom the push buffer segment, since this
    // denotes that HOST has read all the methods needed for this memory operation
    // and safely assume that this GPFIFO and PB entry can be reused.
    //
    data =  DRF_DEF(C86F, _SEM_EXECUTE, _OPERATION, _RELEASE) |
            DRF_DEF(C86F, _SEM_EXECUTE, _PAYLOAD_SIZE, _32BIT) |
            DRF_DEF(C86F, _SEM_EXECUTE, _RELEASE_WFI, _DIS);

    pSemaAddr = (pChannel->pbGpuVA+pChannel->semaOffset);

    NV_PUSH_INC_4U(RM_SUBCHANNEL, NVC86F_SEM_ADDR_LO,
            DRF_NUM(C86F, _SEM_ADDR_LO, _OFFSET, NvU64_LO32(pSemaAddr) >> 2),
            NVC86F_SEM_ADDR_HI,
            DRF_NUM(C86F, _SEM_ADDR_HI, _OFFSET, NvU64_HI32(pSemaAddr)),
            NVC86F_SEM_PAYLOAD_LO, putIndex,
            NVC86F_SEM_PAYLOAD_HI, 0);

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVC86F_SEM_EXECUTE, data);

    NvU32 methodSize = (NvU32)((NvU8*)pPtr - (NvU8*)pStartPtr);
    NV_ASSERT_OR_RETURN(methodSize <= pChannel->methodSizePerBlock, 0);
    return methodSize;
}

/** single helper function to fill the push buffer with the methods needed for
 *  memsetting using CE. This function is much more efficient in the sense it
 *  decouples the mem(set/copy) operation from managing channel resources.
 **/

NvU32
channelFillPb
(
    OBJCHANNEL      *pChannel,
    NvU32            putIndex, 
    NvBool           bPipelined,
    NvBool           bInsertFinishPayload,
    CHANNEL_PB_INFO *pChannelPbInfo
)
{
    NvU32  launchType = 0;
    NvU32 *pPtr       = (NvU32 *)((NvU8 *)pChannel->pbCpuVA + (putIndex * pChannel->methodSizePerBlock));
    NvU32 *pStartPtr  = pPtr;
    NvU32  semaValue  = 0;
    NvU32  data       = 0;
    NvU64  pSemaAddr  = 0;

    NV_PRINTF(LEVEL_INFO, "PutIndex: %x, PbOffset: %x\n", putIndex, putIndex * pChannel->methodSizePerBlock);

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NV906F_SET_OBJECT, pChannel->classEngineID);

    // Side effect - pushed target aperture and REMAP method for memset
    launchType = channelPushMemoryProperties(pChannel, pChannelPbInfo, &pPtr);

    // Side effect - pushed addresses
    channelPushAddress(pChannel, pChannelPbInfo, &pPtr);
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_LINE_LENGTH_IN, pChannelPbInfo->size);

    if (bInsertFinishPayload)
    {
        semaValue = DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE);
        
        // Do not support client semaphore for now
        NV_ASSERT(pChannelPbInfo->clientSemaAddr == 0);

        NV_PUSH_INC_3U(RM_SUBCHANNEL, NVB0B5_SET_SEMAPHORE_A,
                       DRF_NUM(B0B5, _SET_SEMAPHORE_A, _UPPER, NvU64_HI32(pChannel->pbGpuVA + pChannel->finishPayloadOffset)),
                       NVB0B5_SET_SEMAPHORE_B,
                       DRF_NUM(B0B5, _SET_SEMAPHORE_B, _LOWER, NvU64_LO32(pChannel->pbGpuVA + pChannel->finishPayloadOffset)),
                       NVB0B5_SET_SEMAPHORE_PAYLOAD,
                       pChannelPbInfo->payload);
    }

    // Side effect - pushed LAUNCH_DMA methods
    channelPushMethod(pChannel, pChannelPbInfo, bPipelined, bInsertFinishPayload, launchType, semaValue, &pPtr);

    // Release HOST semaphore after work is completed by CE
    data = DRF_DEF(906F, _SEMAPHORED, _OPERATION, _RELEASE) |
           DRF_DEF(906F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE) |
           DRF_DEF(906F, _SEMAPHORED, _RELEASE_WFI, _DIS);

    pSemaAddr = (pChannel->pbGpuVA + pChannel->semaOffset);

    //
    // This should always be at the bottom the push buffer segment, since this
    // denotes that HOST has read all the methods needed for this memory operation
    // and safely assume that this GPFIFO and PB entry can be reused.
    //
    NV_PUSH_INC_4U(RM_SUBCHANNEL, NV906F_SEMAPHOREA,
                   DRF_NUM(906F, _SEMAPHOREA_OFFSET, _UPPER, NvU64_HI32(pSemaAddr)),
                   NV906F_SEMAPHOREB,
                   DRF_NUM(906F, _SEMAPHOREB_OFFSET, _LOWER, NvU64_LO32(pSemaAddr) >> 2),
                   NV906F_SEMAPHOREC,
                   putIndex,
                   NV906F_SEMAPHORED, 
                   data);

    NvU32 methodSize = (NvU32)((NvU8*)pPtr - (NvU8*)pStartPtr);
    NV_ASSERT_OR_RETURN(methodSize <= pChannel->methodSizePerBlock, 0);
    return methodSize;
}


/*** Implementation for static methods ***/
static NvU32 
channelPushMemoryProperties
(
    OBJCHANNEL      *pChannel,
    CHANNEL_PB_INFO *pChannelPbInfo,
    NvU32           **ppPtr
)
{
    NV_ADDRESS_SPACE dstAddressSpace = pChannelPbInfo->dstAddressSpace;
    NV_ADDRESS_SPACE srcAddressSpace = pChannelPbInfo->srcAddressSpace;
    NvU32 dstCpuCacheAttrib = pChannelPbInfo->dstCpuCacheAttrib;
    NvU32 srcCpuCacheAttrib = pChannelPbInfo->srcCpuCacheAttrib;

    NvU32 data = 0;
    NvU32 retVal = 0;
    NvU32 *pPtr = *ppPtr;
    
    if (!pChannelPbInfo->bCeMemcopy)
    {
        // If memset, push remap components
        NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_REMAP_CONST_A, pChannelPbInfo->pattern);
        NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_REMAP_COMPONENTS,
                       DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_A)          |
                       DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, _ONE)     |
                       DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _NUM_DST_COMPONENTS, _ONE));
    }
    else
    {
        // If memcopy, push src addr properties
        if (srcAddressSpace == ADDR_FBMEM)
        {
            data = DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _LOCAL_FB);
        }
        else if (srcCpuCacheAttrib == NV_MEMORY_CACHED)
        {
            data = DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _COHERENT_SYSMEM);
        }
        else
        {
            data = DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _NONCOHERENT_SYSMEM);
        }
        
        NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_SRC_PHYS_MODE, data);
    }

    // Push dst addr properties
    if (dstAddressSpace == ADDR_FBMEM)
    {
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _LOCAL_FB);
    }
    else if (dstCpuCacheAttrib == NV_MEMORY_CACHED)
    {
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _COHERENT_SYSMEM);
    }
    else
    {
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _NONCOHERENT_SYSMEM);
    }
    
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_DST_PHYS_MODE, data);
    
    if (pChannel->bUseVasForCeCopy)
    {
        retVal = DRF_DEF(B0B5, _LAUNCH_DMA, _DST_TYPE, _VIRTUAL) | 
                 DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_TYPE, _VIRTUAL);
    }
    else
    {
        retVal = DRF_DEF(B0B5, _LAUNCH_DMA, _DST_TYPE, _PHYSICAL) |
                 DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_TYPE, _PHYSICAL);
    }

    *ppPtr = pPtr;
    return retVal;
}


static void
channelPushAddress
(
    OBJCHANNEL      *pChannel,
    CHANNEL_PB_INFO *pChannelPbInfo,
    NvU32           **ppPtr
)
{
    NvU64 dstAddr = pChannelPbInfo->dstAddr;
    NvU64 srcAddr = pChannelPbInfo->srcAddr;
    NvU32 *pPtr = *ppPtr;

    // Linear translation to map the PA to VA
    if (pChannel->bUseVasForCeCopy)
    {
        dstAddr = dstAddr + pChannel->fbAliasVA - pChannel->startFbOffset;
        srcAddr = srcAddr + pChannel->fbAliasVA - pChannel->startFbOffset;
    }

    // Set dst addr 
    NV_PUSH_INC_2U(RM_SUBCHANNEL, NVB0B5_OFFSET_OUT_UPPER,
                   DRF_NUM(B0B5, _OFFSET_OUT_UPPER, _UPPER, NvU64_HI32(dstAddr)),
                   NVB0B5_OFFSET_OUT_LOWER,
                   DRF_NUM(B0B5, _OFFSET_OUT_LOWER, _VALUE, NvU64_LO32(dstAddr)));

    // Set src addr if memcopy
    if (pChannelPbInfo->bCeMemcopy)
    {
        NV_PUSH_INC_2U(RM_SUBCHANNEL, NVB0B5_OFFSET_IN_UPPER,
                       DRF_NUM(B0B5, _OFFSET_IN_UPPER, _UPPER, NvU64_HI32(srcAddr)),
                       NVB0B5_OFFSET_IN_LOWER,
                       DRF_NUM(B0B5, _OFFSET_IN_LOWER, _VALUE, NvU64_LO32(srcAddr)));
    }

    *ppPtr = pPtr;
}


static void
channelPushMethod
(
    OBJCHANNEL      *pChannel,
    CHANNEL_PB_INFO *pChannelPbInfo,
    NvBool           bPipelined,
    NvBool           bInsertFinishPayload,
    NvU32            launchType,
    NvU32            semaValue,
    NvU32          **ppPtr
)
{
    NvU32 pipelinedValue = 0;
    NvU32 flushValue = 0;
    NvU32 disablePlcKind = 0;
    NvU32 launchParams = 0;
    NvU32 *pPtr = *ppPtr;

    if (bPipelined)
    {
        pipelinedValue = DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PIPELINED);
    }
    else
    {
        pipelinedValue = DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _NON_PIPELINED);
    }

    if (bInsertFinishPayload)
    {
        flushValue = DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE);
    }
    else
    {
        flushValue = DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _FALSE);
    }

    if (!pChannelPbInfo->bCeMemcopy)
    {
        switch (pChannel->hTdCopyClass)
        {
            case MAXWELL_DMA_COPY_A:
            case PASCAL_DMA_COPY_A:
            case PASCAL_DMA_COPY_B:
            case VOLTA_DMA_COPY_A:
                disablePlcKind = 0;
                break;
            default: // For anything after Turing, set the kind
                disablePlcKind = DRF_DEF(C5B5, _LAUNCH_DMA, _DISABLE_PLC, _TRUE);
            break;
        }

        launchParams =  DRF_DEF(B0B5, _LAUNCH_DMA, _REMAP_ENABLE, _TRUE) | disablePlcKind;
    }

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_LAUNCH_DMA, 
                   launchParams | 
                   DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH) |
                   DRF_DEF(B0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH) |
                   DRF_DEF(B0B5, _LAUNCH_DMA, _MULTI_LINE_ENABLE, _FALSE) |
                   launchType |
                   pipelinedValue |
                   flushValue |
                   semaValue);
    *ppPtr = pPtr;
}
