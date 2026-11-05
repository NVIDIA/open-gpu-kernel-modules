/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/bus/kern_bus.h"


#include "platform/sli/sli.h"

#include "containers/eheap_old.h"
#include "gpu/gpu_access.h"
#include "gpu/gpu.h"

#include "published/turing/tu102/dev_ram.h"
#include "published/turing/tu102/dev_fault.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_ctrl.h"
#include "published/turing/tu102/hwproject.h"

NV_STATUS
kchannelGetUserdInfo_TU102
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    NvU64          *userBase,
    NvU64          *offset,
    NvU64          *length
)
{
    NV_STATUS              status;
    NvU64                  bar1MapOffset;
    NvU32                  bar1MapSize;
    CLI_CHANNEL_CLASS_INFO classInfo;
    KernelMemorySystem     *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    NvBool bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    CliGetChannelClassInfo(RES_GET_EXT_CLASS_ID(pKernelChannel), &classInfo);

    switch (classInfo.classType)
    {
        case CHANNEL_CLASS_TYPE_GPFIFO:
            NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

            // USERD is not pre-allocated in BAR1 so there is no offset/userBase
            NV_ASSERT_OR_RETURN(!pKernelChannel->bClientAllocatedUserD,
                                NV_ERR_INVALID_REQUEST);

            status = kchannelGetUserdBar1MapOffset_HAL(pGpu,
                                                       pKernelChannel,
                                                       &bar1MapOffset,
                                                       &bar1MapSize);
            if (status == NV_OK)
            {
                *offset = bar1MapOffset;
                *length = bar1MapSize;

                if (userBase)
                {
                    if (bCoherentCpuMapping)
                    {
                        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
                        *userBase = pKernelMemorySystem->coherentCpuFbBase;
                    }
                    else
                    {
                        *userBase = gpumgrGetGpuPhysFbAddr(pGpu);
                    }
                }
            }
            break;

        default:
            NV_PRINTF(LEVEL_ERROR,
                      "class = %x not supported for user base mapping\n",
                      RES_GET_EXT_CLASS_ID(pKernelChannel));
            status = NV_ERR_GENERIC;
            break;
    }
    return status;
}

//
// Takes as input a Channel * and returns the BAR1 offset that this channel's
// USERD has been mapped to. Also returns the size of the BAR1 mapping that
// pertains to this channel. The BAR1 map of all USERDs should have already
// been setup before the first channel was created.
//
// For example, USERD of 40 channels have been mapped at BAR1 offset 0x100.
// USERD of one channel is of size 4k. In which case this function will return
// ( 0x100 + ( 0x1000 * 0xa ) ) if the input ChID = 0xa.
//
NV_STATUS
kchannelGetUserdBar1MapOffset_TU102
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel,
    NvU64         *bar1MapOffset,
    NvU32         *bar1MapSize
)
{
    KernelFifo        *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(kfifoUserdNeedsIsolation(pKernelFifo),
                        NV_ERR_NOT_SUPPORTED);

    if (pUserdInfo->userdBar1MapSize == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "fifoGetUserdBar1Offset_GF100: BAR1 map of USERD has not "
                  "been setup yet\n");
        NV_ASSERT(0);
        return NV_ERR_GENERIC;
    }

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, bar1MapSize, NULL);

    *bar1MapOffset = pKernelChannel->ChID * *bar1MapSize +
                     pUserdInfo->userdBar1MapStartOffset;

    NV_ASSERT((*bar1MapOffset + *bar1MapSize) <=
              (pUserdInfo->userdBar1MapStartOffset +
               pUserdInfo->userdBar1MapSize));

    return NV_OK;
}

static void
_kchannelUpdateFifoMapping
(
    KernelChannel    *pKernelChannel,
    OBJGPU           *pGpu,
    NvBool            bKernel,
    NvP64             cpuAddress,
    NvP64             priv,
    NvU64             cpuMapLength,
    NvU32             flags,
    NvHandle          hSubdevice,
    RsCpuMapping     *pMapping
)
{
    pMapping->pPrivate->pGpu      = pGpu;
    pMapping->pPrivate->bKernel   = bKernel;
    pMapping->processId = osGetCurrentProcess();
    pMapping->pLinearAddress      = cpuAddress;
    pMapping->pPrivate->pPriv     = priv;
    pMapping->length              = cpuMapLength;
    pMapping->flags               = flags;
    pMapping->pContext            = (void*)(NvUPtr)pKernelChannel->ChID;
}

NV_STATUS
kchannelMap_TU102
(
    KernelChannel     *pKernelChannel,
    CALL_CONTEXT      *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping      *pCpuMapping
)
{
    OBJGPU *pGpu;
    NV_STATUS rmStatus;
    RsClient *pRsClient = pCallContext->pClient;
    RmClient *pRmClient = dynamicCast(pRsClient, RmClient);
    GpuResource *pGpuResource;

    NV_ASSERT_OR_RETURN(!pKernelChannel->bClientAllocatedUserD, NV_ERR_INVALID_REQUEST);

    rmStatus = gpuresGetByDeviceOrSubdeviceHandle(pRsClient,
                                                  pCpuMapping->pContextRef->hResource,
                                                  &pGpuResource);
    if (rmStatus != NV_OK)
        return rmStatus;

    pGpu = GPU_RES_GET_GPU(pGpuResource);
    GPU_RES_SET_THREAD_BC_STATE(pGpuResource);

    // If the flags are fifo default then offset/length passed in
    if (DRF_VAL(OS33, _FLAGS, _FIFO_MAPPING, pCpuMapping->flags) == NVOS33_FLAGS_FIFO_MAPPING_DEFAULT)
    {
        // Validate the offset and limit passed in.
        if (pCpuMapping->offset >= pKernelChannel->userdLength)
            return NV_ERR_INVALID_BASE;
        if (pCpuMapping->length == 0)
            return NV_ERR_INVALID_LIMIT;
        if (pCpuMapping->offset + pCpuMapping->length > pKernelChannel->userdLength)
            return NV_ERR_INVALID_LIMIT;
    }
    else
    {
        pCpuMapping->offset = 0x0;
        pCpuMapping->length = pKernelChannel->userdLength;
    }

    rmStatus = kchannelMapUserD(pGpu, pKernelChannel,
                                rmclientGetCachedPrivilege(pRmClient),
                                pCpuMapping->offset,
                                pCpuMapping->pPrivate->protect,
                                &pCpuMapping->pLinearAddress,
                                &(pCpuMapping->pPrivate->pPriv));

    if (rmStatus != NV_OK)
        return rmStatus;

    // Save off the mapping
    _kchannelUpdateFifoMapping(pKernelChannel,
                               pGpu,
                               (pRsClient->type == CLIENT_TYPE_KERNEL),
                               pCpuMapping->pLinearAddress,
                               pCpuMapping->pPrivate->pPriv,
                               pCpuMapping->length,
                               pCpuMapping->flags,
                               pCpuMapping->pContextRef->hResource,
                               pCpuMapping);

    return NV_OK;
}

NV_STATUS
kchannelUnmap_TU102
(
    KernelChannel *pKernelChannel,
    CALL_CONTEXT  *pCallContext,
    RsCpuMapping  *pCpuMapping
)
{
    OBJGPU   *pGpu;
    RsClient *pRsClient = pCallContext->pClient;
    RmClient *pRmClient = dynamicCast(pRsClient, RmClient);

    if (pKernelChannel->bClientAllocatedUserD)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    pGpu = pCpuMapping->pPrivate->pGpu;

    kchannelUnmapUserD(pGpu,
                       pKernelChannel,
                       rmclientGetCachedPrivilege(pRmClient),
                       &pCpuMapping->pLinearAddress,
                       &pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NV_STATUS
kchannelMapUserD_TU102
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    RS_PRIV_LEVEL   privLevel,
    NvU64           offset,
    NvU32           protect,
    NvP64          *ppCpuVirtAddr,
    NvP64          *ppPriv
)
{
    NV_STATUS status      = NV_OK;
    NvU64     userBase;
    NvU64     userOffset;
    NvU64     userSize;
    NvU32     cachingMode = NV_MEMORY_UNCACHED;

    // if USERD is allocated by client
    if (pKernelChannel->bClientAllocatedUserD)
    {
        return NV_OK;
    }

    status = kchannelGetUserdInfo_HAL(pGpu, pKernelChannel,
                                      &userBase, &userOffset, &userSize);

    if (status != NV_OK)
        return status;


    if (userBase == pGpu->busInfo.gpuPhysAddr)
    {
        // Create a mapping of BAR0
        status = osMapGPU(pGpu, privLevel, NvU64_LO32(userOffset+offset),
                 NvU64_LO32(userSize), protect, ppCpuVirtAddr, ppPriv);
        goto done;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        cachingMode = NV_MEMORY_CACHED;
    }

    //
    // If userBase is not bar0, then it is bar1 and we create a regular memory
    // mapping.
    //
    if (privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        status = osMapPciMemoryKernel64(pGpu, userBase + userOffset + offset,
                                        userSize, protect, ppCpuVirtAddr, cachingMode);
    }
    else
    {
        status = osMapPciMemoryUser(pGpu->pOsGpuInfo,
                                    userBase + userOffset + offset,
                                    userSize, protect, ppCpuVirtAddr,
                                    ppPriv, cachingMode);
    }
    if (!((status == NV_OK) && *ppCpuVirtAddr))
    {
        NV_PRINTF(LEVEL_ERROR,
            "BAR1 offset 0x%llx for USERD of " FMT_CHANNEL_DEBUG_TAG " could not be cpu mapped\n",
            userOffset,
            kchannelGetDebugTag(pKernelChannel));
    }

done:

    // Indicate channel is mapped
    if (status == NV_OK)
    {
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
            kchannelSetCpuMapped(pGpu, pKernelChannel, NV_TRUE);
            SLI_LOOP_END
    }

    return status;
}

void
kchannelUnmapUserD_TU102
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    RS_PRIV_LEVEL   privLevel,
    NvP64          *ppCpuVirtAddr,
    NvP64          *ppPriv
)
{
    NV_STATUS status;
    NvU64     userBase;
    NvU64     userOffset;
    NvU64     userSize;

    if (pKernelChannel->bClientAllocatedUserD)
    {
        return;
    }

    status = kchannelGetUserdInfo_HAL(pGpu, pKernelChannel,
                                      &userBase, &userOffset, &userSize);

    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    if (userBase == pGpu->busInfo.gpuPhysAddr)
    {
        osUnmapGPU(pGpu->pOsGpuInfo, privLevel, *ppCpuVirtAddr,
                   NvU64_LO32(userSize), *ppPriv);
    }
    else
    {
        // GF100+
        // Unmap Cpu virt mapping
        if (privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            osUnmapPciMemoryKernel64(pGpu, *ppCpuVirtAddr);
        }
        else
        {
            osUnmapPciMemoryUser(pGpu->pOsGpuInfo, *ppCpuVirtAddr,
                                 userSize, *ppPriv);
        }
    }

    // Indicate channel is !mapped
    kchannelSetCpuMapped(pGpu, pKernelChannel, NV_FALSE);
    return;
}

NV_STATUS
kchannelGetMapAddrSpace_TU102
(
    KernelChannel    *pKernelChannel,
    CALL_CONTEXT     *pCallContext,
    NvU32             mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 userdAperture;
    NvU32 userdAttribute;

    NV_ASSERT_OK_OR_RETURN(kfifoGetUserdLocation_HAL(pKernelFifo,
                                                     &userdAperture,
                                                     &userdAttribute));
    if (pAddrSpace)
        *pAddrSpace = userdAperture;

    return NV_OK;
}

/**
 * @brief Update the subcontext info in channel instance block
 *
 * The subcontext info is written to the SC array indexed by the VEID.
 *
 * Passing a NULL VASpace object pointer will clear the subcontext entry.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 * @param[in] subctxId
 * @param[in] pSubctxVAS
 * @param[in] pSubctxHandleVAS
 * @param[in] bSkipHandleVASpaceUpdate
 */
NV_STATUS
kchannelUpdateSubcontextTable_TU102
(
    OBJGPU           *pGpu,
    KernelChannel    *pKernelChannel,
    NvU32            subctxId,
    OBJVASPACE      *pSubctxVAS,
    HANDLE_VASPACE  *pSubctxHandleVAS,
    NvBool           bSkipHandleVASpaceUpdate
)
{
    MEMORY_DESCRIPTOR   *pMemDesc        = NULL;
    INST_BLK_INIT_PARAMS params          = {0};
    NV_STATUS            status          = NV_OK;
    KernelFifo          *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelChannel->pKernelChannelGroup != NULL,
                        NV_ERR_INVALID_STATE);

    // Defer bus flush until subcontext array init
    params.bDeferFlush = NV_TRUE;

    if (pSubctxVAS != NULL)
    {
        params.bIsFaultReplayable = pKernelFifo->bReplayableFaultsOverride ?
            pKernelFifo->replayableFaultsOverride : vaspaceIsFaultCapable(pSubctxVAS);
    }

    status = kfifoChannelGetFifoContextMemDesc_HAL(pGpu, pKernelFifo,
                                                   pKernelChannel,
                                                   FIFO_CTX_INST_BLOCK,
                                                   &pMemDesc);
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        return status;
    }

    params.pInstBlk = kbusMapRmAperture_HAL(pGpu, pMemDesc);
    NV_ASSERT_OR_RETURN(params.pInstBlk != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

    params.bIsZombieSubctx = kchangrpGetSubcontextZombieState(
        pGpu,
        pKernelChannel->pKernelChannelGroup,
        subctxId);

    status = kgmmuInstBlkInit(GPU_GET_KERNEL_GMMU(pGpu),
                              pMemDesc, pSubctxVAS, pSubctxHandleVAS,
                              subctxId, &params);

    kbusUnmapRmAperture_HAL(pGpu, pMemDesc, &params.pInstBlk, NV_FALSE);

    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        return status;
    }

    return NV_OK;
}

/**
 * @brief Commit the valid subcontext mask in the channel instance block
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 *
 */
NV_STATUS
kchannelCommitSubcontextMask_TU102
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    KernelFifo        *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);
    PMEMORY_DESCRIPTOR pInstBlkDesc = NULL;
    NvU8              *pInstBlk     = NULL;
    NV_STATUS          status       = NV_OK;
    NvU32             *pSubCtxMask;
    NvU32              offset, numDwords, i;

    NV_ASSERT(pKernelChannel != NULL);
    NV_ASSERT_OR_RETURN(pKernelChannel->pKernelChannelGroup != NULL, NV_ERR_INVALID_STATE);    
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    offset = SF_OFFSET(NV_RAMIN_SC_PDB_VALID(0));

    numDwords = kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, NV_FALSE);
    numDwords /= (sizeof(NvU32) * 8);
    NV_ASSERT(numDwords == SUBCTX_MASK_ARRAY_SIZE);

    status = kfifoChannelGetFifoContextMemDesc_HAL(pGpu, pKernelFifo, pKernelChannel,
                                            FIFO_CTX_INST_BLOCK, &pInstBlkDesc);
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        return status;
    }

    pSubCtxMask = pKernelChannel->pKernelChannelGroup
                      ->ppSubctxMask[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    pInstBlk = kbusMapRmAperture_HAL(pGpu, pInstBlkDesc);
    NV_ASSERT_OR_RETURN(pInstBlk != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

    for(i = 0; i < numDwords; i++)
    {
        MEM_WR32(pInstBlk + offset, pSubCtxMask[i]);
        offset += sizeof(NvU32);
    }

    kbusUnmapRmAperture_HAL(pGpu, pInstBlkDesc, &pInstBlk, NV_FALSE);
    return NV_OK;
}

NV_STATUS
kchannelCommitPdb_TU102
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel
)
{
    KernelBus           *pKernelBusLoop;
    KernelGmmu          *pKernelGmmuLoop;
    KernelFifo          *pKernelFifoLoop;
    KernelChannelGroup  *pKernelChannelGroup;
    FIFO_INSTANCE_BLOCK *pInstanceBlock;
    MEMORY_DESCRIPTOR   *pGpuInstMemDesc;
    OBJVASPACE          *pVAS;
    HANDLE_VASPACE      *pHandleVAS;
    NV_STATUS            status  = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);
    NV_ASSERT_OR_RETURN(pKernelChannel->pKernelChannelGroupApi != NULL, NV_ERR_INVALID_CHANNEL);

    pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_STATE);
 
    pVAS = pKernelChannel->pVAS;
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_STATE);
    pHandleVAS = pKernelChannel->pHandleVAS;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    pKernelFifoLoop  = GPU_GET_KERNEL_FIFO(pGpu);
    pKernelGmmuLoop  = GPU_GET_KERNEL_GMMU(pGpu);
    pKernelBusLoop   = GPU_GET_KERNEL_BUS(pGpu);
    pGpuInstMemDesc  = NULL;

    pInstanceBlock = pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    if (pInstanceBlock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to retrieve channel instance block!\n");
        SLI_LOOP_RETURN(NV_ERR_INSUFFICIENT_RESOURCES);
    }

    pGpuInstMemDesc = pInstanceBlock->pInstanceBlockDesc;

    if (!pGpuInstMemDesc)
        SLI_LOOP_RETURN(NV_ERR_INSUFFICIENT_RESOURCES);

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pGpuInstMemDesc));
    
    INST_BLK_INIT_PARAMS params = {0};
    // Defer bus flush until subcontext array init
    params.bDeferFlush = NV_TRUE;

    params.bIsFaultReplayable = pKernelFifoLoop->bReplayableFaultsOverride ?
        pKernelFifoLoop->replayableFaultsOverride : vaspaceIsFaultCapable(pVAS);

    params.pInstBlk = kbusMapRmAperture_HAL(pGpu, pGpuInstMemDesc);
    if (params.pInstBlk == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto fail;
    }
  
    status = kgmmuInstBlkInit(pKernelGmmuLoop, pGpuInstMemDesc, pVAS, pHandleVAS,
                             FIFO_PDB_IDX_BASE, &params);

    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("kgmmuInstBlkInit failed", status);
        goto fail;
    }

    if (kfifoIsSubcontextSupported(pKernelFifoLoop))
    {
        //
        // If the channel is within a TSG, we use TSG's subcontext
        // Eheap to search for valid SubctxId within a TSG and
        // initialize instance block.
        // From Volta and onwards, we should also populate the
        // subcontext PDBs.
        //
        OBJVASPACE     *pSubctxVAS       = NULL;
        HANDLE_VASPACE *pSubctxHandleVAS = NULL;
        OBJEHEAP       *pSubctxIdHeap;
        NvU32           maxSubCtx;
        NvU32           i;

        maxSubCtx = kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu,
            pKernelFifoLoop, pKernelChannelGroup, NV_FALSE);

        NV_ASSERT_OR_ELSE(pKernelChannelGroup != NULL,
                          status = NV_ERR_INVALID_STATE; goto fail);


        pSubctxIdHeap = pKernelChannelGroup->pSubctxIdHeap;
        
        for (i = 0; i < maxSubCtx; i++)
        {
            EMEMBLOCK *pBlock;
            params.bIsFaultReplayable  = NV_FALSE;

            pBlock = pSubctxIdHeap->eheapGetBlock(pSubctxIdHeap,
                                                  i,
                                                  NV_FALSE);
            
            //
            // If a Valid sub context is found with given
            // SubctxId, then we obtain that sub context's VAS.
            // Otherwise, it means that a subcontext is not
            // allocated with the given SubctxId, so we set VAS
            // to be NULL, since VAS being NULL indicates that
            // the following PDB(SubctxId) is invalid.
            //
            if (pBlock)
            {
                KernelCtxShare *pShare = (KernelCtxShare *)pBlock->pData;
                pSubctxVAS             = pShare->pVAS;
                pSubctxHandleVAS       = pShare->pHandleVAS;
            }
            else
            {
                pSubctxVAS       = NULL;
                pSubctxHandleVAS = NULL;
            }

            NV_ASSERT_OK_OR_GOTO(
                status,
                kchannelUpdateSubcontextTable_HAL(pGpu, pKernelChannel, i, pSubctxVAS, pSubctxHandleVAS, NV_FALSE),
                fail);
        }

        NV_ASSERT_OK_OR_GOTO(status,
            kchannelCommitSubcontextMask_HAL(pGpu, pKernelChannel),
            fail);
    }

    kbusUnmapRmAperture_HAL(pGpu, pGpuInstMemDesc, &params.pInstBlk, NV_FALSE);

    kbusFlush_HAL(pGpu, pKernelBusLoop,
                  kbusGetFlushAperture(pKernelBusLoop, memdescGetAddressSpace(pGpuInstMemDesc)));

fail:
    if (status != NV_OK)
    {
        kbusUnmapRmAperture_HAL(pGpu, pGpuInstMemDesc, &params.pInstBlk, NV_FALSE);
        SLI_LOOP_BREAK;
    }
    SLI_LOOP_END

    return status;
}
