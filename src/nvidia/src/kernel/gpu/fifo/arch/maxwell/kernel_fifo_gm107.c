/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mmu/kern_gmmu.h"

#include "nvrm_registry.h"

#include "vgpu/rpc.h"
#include "gpu/bus/kern_bus.h"

#include "published/maxwell/gm107/dev_ram.h"
#include "published/maxwell/gm107/dev_mmu.h"


static inline NvBool
_isEngineInfoTypeValidForOnlyHostDriven(ENGINE_INFO_TYPE type);


/*! Construct kfifo object */
NV_STATUS
kfifoConstructHal_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    PREALLOCATED_USERD_INFO *pUserdInfo = &pKernelFifo->userdInfo;

    if (FLD_TEST_DRF(_REG_STR_RM, _INST_VPR, _INSTBLK, _TRUE, pGpu->instVprOverrides))
    {
        pKernelFifo->bInstProtectedMem = NV_TRUE;
    }

    // Instance Memory
    switch (DRF_VAL( _REG_STR_RM, _INST_LOC, _INSTBLK, pGpu->instLocOverrides))
    {
        default:
        case NV_REG_STR_RM_INST_LOC_INSTBLK_DEFAULT:
            pKernelFifo->pInstAllocList  = ADDRLIST_FBMEM_PREFERRED;
            pKernelFifo->InstAttr        = NV_MEMORY_UNCACHED;
            break;
        case NV_REG_STR_RM_INST_LOC_INSTBLK_VID:
            pKernelFifo->pInstAllocList  = ADDRLIST_FBMEM_ONLY;
            pKernelFifo->InstAttr        = NV_MEMORY_UNCACHED;
            break;
        case NV_REG_STR_RM_INST_LOC_INSTBLK_COH:
            pKernelFifo->pInstAllocList  = ADDRLIST_SYSMEM_ONLY;
            pKernelFifo->InstAttr        = NV_MEMORY_CACHED;
            break;
        case NV_REG_STR_RM_INST_LOC_INSTBLK_NCOH:
            pKernelFifo->pInstAllocList  = ADDRLIST_SYSMEM_ONLY;
            pKernelFifo->InstAttr        = NV_MEMORY_UNCACHED;
            break;
    }

    // USERD
    pUserdInfo->userdAperture    = ADDR_FBMEM;
    pUserdInfo->userdAttr        = NV_MEMORY_WRITECOMBINED;
    memdescOverrideInstLoc(DRF_VAL( _REG_STR_RM, _INST_LOC, _USERD, pGpu->instLocOverrides),
                           "USERD",
                           &pUserdInfo->userdAperture,
                           &pUserdInfo->userdAttr);

    return NV_OK;
}

/**
 * @brief Allocate a page for dummy page directory
 *
 * On GV100, PDB corresponding to subcontexts that are freed
 * will point to a dummy page directory instead of setting it to NULL
 * Here we allocate a page for this page directory
 */
static NV_STATUS
_kfifoAllocDummyPage
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NV_STATUS status   = NV_OK;
    NvU32     flags    = MEMDESC_FLAGS_NONE;
    NvBool    bBcState = gpumgrGetBcEnabledStatus(pGpu);

    if (bBcState)
    {
        flags |= MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE;
    }

    // Using instance block attributes to allocate dummy page
    status = memdescCreate(&pKernelFifo->pDummyPageMemDesc, pGpu,
                           RM_PAGE_SIZE,
                           0,
                           NV_FALSE,
                           ADDR_UNKNOWN,
                           pKernelFifo->InstAttr,
                           flags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not memdescCreate for dummy page\n");
        DBG_BREAKPOINT();
        return status;
    }

    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_80, 
                    pKernelFifo->pDummyPageMemDesc, pKernelFifo->pInstAllocList);
    if (status !=  NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate dummy page\n");
        DBG_BREAKPOINT();
        memdescDestroy(pKernelFifo->pDummyPageMemDesc);
        pKernelFifo->pDummyPageMemDesc = NULL;
    }

    return status;
}

/**
 * @brief Free the page used for dummy page directory
 */
static void
_kfifoFreeDummyPage
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    // Free dummy page memdesc
    memdescFree(pKernelFifo->pDummyPageMemDesc);
    memdescDestroy(pKernelFifo->pDummyPageMemDesc);
    pKernelFifo->pDummyPageMemDesc = NULL;
}

NV_STATUS
kfifoStatePostLoad_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       flags
)
{
    NV_STATUS                      status     = NV_OK;
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        // Prealloc USERD
        NV_ASSERT_OK_OR_RETURN(kfifoPreAllocUserD_HAL(pGpu, pKernelFifo));

        if (gpumgrIsParentGPU(pGpu))
        {
            if (kfifoIsZombieSubctxWarEnabled(pKernelFifo))
            {
                NvBool bBcState = gpumgrGetBcEnabledStatus(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);

                status = _kfifoAllocDummyPage(pGpu, pKernelFifo);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to allocate dummy page for zombie subcontexts\n");
                    DBG_BREAKPOINT();
                    gpumgrSetBcEnabledStatus(pGpu, bBcState);
                    return status;
                }

                if (IS_VIRTUAL_WITH_SRIOV(pGpu))
                {
                    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS params;
                    MEMORY_DESCRIPTOR *pDummyPageMemDesc = kfifoGetDummyPageMemDesc(pKernelFifo);

                    portMemSet(&params, 0, sizeof(params));

                    params.base     = memdescGetPhysAddr(pDummyPageMemDesc, AT_GPU, 0);;
                    params.size     = pDummyPageMemDesc->Size;
                    params.addressSpace = memdescGetAddressSpace(pDummyPageMemDesc);
                    params.cacheAttrib  = memdescGetCpuCacheAttrib(pDummyPageMemDesc);

                    NV_RM_RPC_CONTROL(pGpu,
                                      pGpu->hDefaultClientShare,
                                      pGpu->hDefaultClientShareSubDevice,
                                      NV2080_CTRL_CMD_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB,
                                      &params,
                                      sizeof(params),
                                      status);
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR,
                            "RM control call to setup zombie subctx failed, status 0x%x\n", status);
                        DBG_BREAKPOINT();
                        return status;
                    }
                }

                gpumgrSetBcEnabledStatus(pGpu, bBcState);
            }
        }
    }

    // Since we have successfully setup BAR1 USERD rsvd memory
    // lets inform hw (only if the snoop is not disabled.)
    kfifoSetupBar1UserdSnoop_HAL(pGpu, pKernelFifo, NV_TRUE, pUserdInfo->userdBar1MapStartOffset);

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        status = kfifoTriggerPostSchedulingEnableCallback(pGpu, pKernelFifo);
        if (status != NV_OK)
            return status;
    }

    return status;
}

NV_STATUS
kfifoStatePreUnload_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       flags
)
{
    NV_STATUS status = NV_OK;
    NvU32     sliLoopReentrancy;

    NV_PRINTF(LEVEL_INFO, "start\n");

    if (!(flags & GPU_STATE_FLAGS_PRESERVING) && gpumgrIsParentGPU(pGpu))
    {
        NvBool bBcState = NV_FALSE;

        if (kfifoIsZombieSubctxWarEnabled(pKernelFifo))
        {
            _kfifoFreeDummyPage(pGpu, pKernelFifo);
        }

        // Notify the handlers that the channel will soon be disabled.
        status = kfifoTriggerPreSchedulingDisableCallback(pGpu, pKernelFifo);

        // Enable broadcast on SLI
        bBcState = gpumgrGetBcEnabledStatus(pGpu);
        gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);

        // As we have forced here SLI broadcast mode, temporarily reset the reentrancy count
        sliLoopReentrancy = gpumgrSLILoopReentrancyPop(pGpu);

        // Ask host to stop snooping
        kfifoSetupBar1UserdSnoop_HAL(pGpu, pKernelFifo, NV_FALSE, 0);

        // Restore the reentrancy count
        gpumgrSLILoopReentrancyPush(pGpu, sliLoopReentrancy);

        // Restore prior broadcast state
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        // Free preallocated userd
        kfifoFreePreAllocUserD_HAL(pGpu, pKernelFifo);
    }

    return status;
}

/**
 * Returns the default timeslice (in us) for a channelgroup as defined by hardware.
 */
NvU64
kfifoChannelGroupGetDefaultTimeslice_GM107
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_TIMESLICE_TIMEOUT_128 << NV_RAMRL_ENTRY_TIMESLICE_SCALE_3;
}

/*! Get size and alignment requirements for instance memory */
NV_STATUS
kfifoGetInstMemInfo_GM107
(
    KernelFifo  *pKernelFifo,
    NvU64       *pSize,
    NvU64       *pAlignment,
    NvBool      *pbInstProtectedMem,
    NvU32       *pInstAttr,
    const NV_ADDRESS_SPACE **ppInstAllocList
)
{
    NV_ASSERT_OR_RETURN(pSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pAlignment != NULL, NV_ERR_INVALID_ARGUMENT);

    *pSize = NV_RAMIN_ALLOC_SIZE;
    *pAlignment = 1 << NV_RAMIN_BASE_SHIFT;

    if(pbInstProtectedMem != NULL)
        *pbInstProtectedMem = pKernelFifo->bInstProtectedMem;

    if(pInstAttr != NULL)
        *pInstAttr = pKernelFifo->InstAttr;

    if(ppInstAllocList != NULL)
        *ppInstAllocList = pKernelFifo->pInstAllocList;

    return NV_OK;
}

/*! Gets instance block size and offset align for instance memory */
void
kfifoGetInstBlkSizeAlign_GM107
(
    KernelFifo *pKernelFifo,
    NvU32      *pSize,
    NvU32      *pShift
)
{
    *pSize  = NV_RAMIN_ALLOC_SIZE;
    *pShift = NV_RAMIN_BASE_SHIFT;

    return;
}

/*!
 * @brief Gets the default runlist id to use for channels allocated with no engines on them.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] rmEngineType      - Engine type of the channel to retrieve default runlist id for
 */
NvU32
kfifoGetDefaultRunlist_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    RM_ENGINE_TYPE rmEngineType
)
{
    NvU32 runlistId = INVALID_RUNLIST_ID;
    ENGDESCRIPTOR engDesc = ENG_GR(0);

    if (RM_ENGINE_TYPE_IS_VALID(rmEngineType))
    {
        // if translation fails, defualt is ENG_GR(0)
        NV_ASSERT_OK(
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)rmEngineType,
                ENGINE_INFO_TYPE_ENG_DESC,       &engDesc));
    }

    // if translation fails, default is INVALID_RUNLIST_ID
    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                 ENGINE_INFO_TYPE_ENG_DESC,
                                 engDesc,
                                 ENGINE_INFO_TYPE_RUNLIST,
                                 &runlistId) != NV_OK)
    {
        runlistId = INVALID_RUNLIST_ID;
    }

    return runlistId;
}

/**
 * @brief Programs a channel's runlist id to a given value
 *
 * Verifies that the requested engine is valid based on the current channel's
 * state.  Does not bind the channel to the runlist in sw or hw. @ref kfifoRunlistSetId.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param[in/out] pKernelChannel
 * @param[in] runlistId runlist ID to use
 */
NV_STATUS
kfifoRunlistSetId_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32   runlistId
)
{
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    if ((runlistId != kchannelGetRunlistId(pKernelChannel)) &&
        kchannelIsRunlistSet(pGpu, pKernelChannel))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Channel has already been assigned a runlist incompatible with this "
                  "engine (requested: 0x%x current: 0x%x).\n", runlistId,
                  kchannelGetRunlistId(pKernelChannel));
        return NV_ERR_INVALID_STATE;
    }

    //
    // For TSG channel, the RL should support TSG.
    // We relax this requirement if the channel is TSG wrapped by RM.
    // In that case, RM won't write the TSG header in the RL.
    //
    if (!kfifoRunlistIsTsgHeaderSupported_HAL(pGpu, pKernelFifo, runlistId) &&
        (pKernelChannel->pKernelChannelGroupApi != NULL) &&
        !pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm)
    {
        NV_PRINTF(LEVEL_ERROR, "Runlist does not support TSGs\n");
        return NV_ERR_INVALID_STATE;
    }

    // If you want to set runlistId of channel - first set it on TSG
    if (pKernelChannel->pKernelChannelGroupApi != NULL)
    {
        // Change TSG runlist if channel is the only one
        if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->chanCount == 1 ||
            !pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bRunlistAssigned)
        {
            pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->runlistId = runlistId;
            pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bRunlistAssigned = NV_TRUE;
        }
        else
        {
            NV_ASSERT_OR_RETURN(pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->runlistId ==
                                    runlistId,
                                NV_ERR_INVALID_STATE);
        }
    }

    kchannelSetRunlistId(pKernelChannel, runlistId);
    kchannelSetRunlistSet(pGpu, pKernelChannel, NV_TRUE);
    return NV_OK;
}

/**
 * @brief Programs a channel's runlist id given the engine tag
 *
 * Verifies that the requested engine is valid based on the current channel's
 * state.  Does not bind the channel to the runlist in sw or hw. @ref kfifoRunlistSetIdByEngine.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param[in/out] pKernelChannel
 * @param[in] engDesc
 */
NV_STATUS
kfifoRunlistSetIdByEngine_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32   engDesc
)
{
    NvU32 runlistId;
    NV_STATUS status;
    NvU32 subctxType = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    kfifoGetSubctxType_HAL(pGpu, pKernelFifo, pKernelChannel, &subctxType);

    if (!kfifoValidateEngineAndRunqueue_HAL(pGpu, pKernelFifo, engDesc, kchannelGetRunqueue(pKernelChannel)))
        return NV_ERR_INVALID_ARGUMENT;

    if (!kfifoValidateEngineAndSubctxType_HAL(pGpu, pKernelFifo, engDesc, subctxType))
        return NV_ERR_INVALID_ARGUMENT;

    //
    // SW objects can go on any runlist so we defer committing of runlist ID to
    // scheduling or another object's allocation.
    //
    if ((engDesc == ENG_SW) || (engDesc == ENG_BUS))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_ENG_DESC,
                engDesc, ENGINE_INFO_TYPE_RUNLIST, &runlistId));

    status = kfifoRunlistSetId_HAL(pGpu, pKernelFifo, pKernelChannel, runlistId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to program runlist for %s\n",
                  kfifoGetEngineName_HAL(pKernelFifo, ENGINE_INFO_TYPE_ENG_DESC, engDesc));
    }

    return status;
}

NV_STATUS
kfifoChannelGetFifoContextMemDesc_GM107
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    KernelChannel      *pKernelChannel,
    FIFO_CTX            engineState,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    FIFO_INSTANCE_BLOCK *pInstanceBlock;

    /* UVM calls nvGpuOpsGetChannelInstanceMemInfo
     * which calls current function to fetch FIFO_CTX_INST_BLOCK */
    /* Currenltly, UVM supported on SRIOV vGPUs only. */
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    pInstanceBlock = pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    if (pInstanceBlock == NULL)
        return NV_ERR_INVALID_STATE;

    switch (engineState)
    {
        case FIFO_CTX_INST_BLOCK:
            *ppMemDesc = pInstanceBlock->pInstanceBlockDesc;
            break;

        case FIFO_CTX_RAMFC:
            *ppMemDesc = pInstanceBlock->pRamfcDesc;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR,
                      "bad engineState 0x%x on engine 0x%x\n",
                      engineState, ENG_FIFO);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(!memdescHasSubDeviceMemDescs(*ppMemDesc));

    NV_PRINTF(LEVEL_INFO,
              "Channel %d engine 0x%x engineState 0x%x *ppMemDesc %p\n",
              kchannelGetDebugTag(pKernelChannel), ENG_FIFO, engineState, *ppMemDesc);

    return NV_OK;
}

/**
 * @brief lookup the kernelchannel data associated with a given instance address/target
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelFifo        KernelFifo pointer
 * @param[in] pInst              INST_BLOCK_DESC pointer
 * @param[out] ppKernelChannel   KernelChannel ptr
 */
NV_STATUS
kfifoConvertInstToKernelChannel_GM107
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    INST_BLOCK_DESC  *pInst,
    KernelChannel   **ppKernelChannel
)
{
    MemoryManager       *pMemoryManager   = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelChannel       *pKernelChannel   = NULL;
    FIFO_INSTANCE_BLOCK *pInstanceBlock;
    MEMORY_DESCRIPTOR    instMemDesc;
    NV_ADDRESS_SPACE     instAperture;
    CHANNEL_ITERATOR     chanIt;

    NV_ASSERT_OR_RETURN(pInst != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    *ppKernelChannel = NULL;

    switch (pInst->aperture)
    {
        case INST_BLOCK_APERTURE_SYSTEM_COHERENT_MEMORY:
        case INST_BLOCK_APERTURE_SYSTEM_NON_COHERENT_MEMORY:
            instAperture = ADDR_SYSMEM;
            break;
        case INST_BLOCK_APERTURE_VIDEO_MEMORY:
            instAperture = ADDR_FBMEM;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "unknown inst target 0x%x\n", pInst->aperture);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ADDRESS;
    }

    //
    // The MMU_PTE version of aperture is what the HW should always
    // report for an instance block. Compare the SW defines against
    // these values here.
    //
    VERIFY_INST_BLOCK_APERTURE(NV_MMU_PTE_APERTURE_VIDEO_MEMORY,
                               NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY,
                               NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY);

    memdescCreateExisting(&instMemDesc, pGpu, NV_RAMIN_ALLOC_SIZE,
                          instAperture, NV_MEMORY_UNCACHED,
                          MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    memdescDescribe(&instMemDesc, instAperture, pInst->address, NV_RAMIN_ALLOC_SIZE);

    kfifoGetChannelIterator(pGpu, pKernelFifo, &chanIt, INVALID_RUNLIST_ID);
    while (kfifoGetNextKernelChannel(pGpu, pKernelFifo, &chanIt, &pKernelChannel) == NV_OK)
    {
        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);

        pInstanceBlock = pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

        if (pInstanceBlock != NULL &&
            pInstanceBlock->pInstanceBlockDesc != NULL &&
            kchannelGetGfid(pKernelChannel) == pInst->gfid &&
            memmgrComparePhysicalAddresses_HAL(pGpu, pMemoryManager,
                kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu),
                    pInstanceBlock->pInstanceBlockDesc),
                memdescGetPhysAddr(pInstanceBlock->pInstanceBlockDesc,
                                   AT_GPU, 0),
                kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu),
                                                &instMemDesc),
                memdescGetPhysAddr(&instMemDesc, AT_GPU, 0)))
        {
                *ppKernelChannel = pKernelChannel;
                memdescDestroy(&instMemDesc);
                return NV_OK;
        }
    }

    NV_PRINTF(LEVEL_INFO,
              "No channel found for instance 0x%016llx (target 0x%x)\n",
              memdescGetPhysAddr(&instMemDesc, AT_GPU, 0),
              kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), &instMemDesc));
    memdescDestroy(&instMemDesc);

    return NV_ERR_INVALID_CHANNEL;
}

static inline NvBool
_isEngineInfoTypeValidForOnlyHostDriven(ENGINE_INFO_TYPE type)
{
    switch (type)
    {
        case ENGINE_INFO_TYPE_RUNLIST:
        case ENGINE_INFO_TYPE_RUNLIST_PRI_BASE:
        case ENGINE_INFO_TYPE_RUNLIST_ENGINE_ID:
        case ENGINE_INFO_TYPE_PBDMA_ID:
        case ENGINE_INFO_TYPE_CHRAM_PRI_BASE:
        case ENGINE_INFO_TYPE_FIFO_TAG:
            return NV_TRUE;
        case ENGINE_INFO_TYPE_ENG_DESC:
        case ENGINE_INFO_TYPE_RM_ENGINE_TYPE:
        case ENGINE_INFO_TYPE_MMU_FAULT_ID:
        case ENGINE_INFO_TYPE_RC_MASK:
        case ENGINE_INFO_TYPE_RESET:
        case ENGINE_INFO_TYPE_INTR:
        case ENGINE_INFO_TYPE_MC:
        case ENGINE_INFO_TYPE_DEV_TYPE_ENUM:
        case ENGINE_INFO_TYPE_INSTANCE_ID:
        case ENGINE_INFO_TYPE_IS_HOST_DRIVEN_ENGINE:
            // The bool itself is valid for non-host-driven engines too.
        case ENGINE_INFO_TYPE_INVALID:
            return NV_FALSE;
        default:
            // Ensure that this function covers every value in ENGINE_INFO_TYPE
            NV_ASSERT(0 && "check all ENGINE_INFO_TYPE are classified as host-driven or not");
            return NV_FALSE;
    }
}


NV_STATUS
kfifoEngineInfoXlate_GM107
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    ENGINE_INFO_TYPE  inType,
    NvU32             inVal,
    ENGINE_INFO_TYPE  outType,
    NvU32            *pOutVal
)
{
    const ENGINE_INFO *pEngineInfo       = kfifoGetEngineInfo(pKernelFifo);
    FIFO_ENGINE_LIST  *pFoundInputEngine = NULL;

    NV_ASSERT_OR_RETURN(pOutVal != NULL, NV_ERR_INVALID_ARGUMENT);

    // PBDMA_ID can only be inType
    NV_ASSERT_OR_RETURN(outType != ENGINE_INFO_TYPE_PBDMA_ID,
                        NV_ERR_INVALID_ARGUMENT);

    if (pEngineInfo == NULL)
    {
        NV_ASSERT_OK_OR_RETURN(kfifoConstructEngineList_HAL(pGpu, pKernelFifo));
        pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    }
    NV_ASSERT_OR_RETURN(pEngineInfo != NULL, NV_ERR_INVALID_STATE);

    if (inType == ENGINE_INFO_TYPE_INVALID)
    {
        NV_ASSERT_OR_RETURN(inVal < pEngineInfo->engineInfoListSize,
                            NV_ERR_INVALID_ARGUMENT);
        pFoundInputEngine = &pEngineInfo->engineInfoList[inVal];
    }
    else
    {
        NvU32 i;
        for (i = 0;
             (i < pEngineInfo->engineInfoListSize) &&
             (pFoundInputEngine == NULL);
             ++i)
        {
            FIFO_ENGINE_LIST *pThisEngine = &pEngineInfo->engineInfoList[i];

            if (inType == ENGINE_INFO_TYPE_PBDMA_ID)
            {
                NvU32 j;
                for (j = 0; j < pThisEngine->numPbdmas; ++j)
                {
                    if (pThisEngine->pbdmaIds[j] == inVal)
                    {
                        pFoundInputEngine = pThisEngine;
                        break;
                    }
                }
            }
            else if (pThisEngine->engineData[inType] == inVal)
            {
                pFoundInputEngine = pThisEngine;
            }
        }
    }

    if (pFoundInputEngine == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (_isEngineInfoTypeValidForOnlyHostDriven(outType) &&
        !pFoundInputEngine->engineData[ENGINE_INFO_TYPE_IS_HOST_DRIVEN_ENGINE])
    {
        //
        // Bug 3748452 TODO
        // Bug 3772199 TODO
        //
        // We can't easily just return an error here because hundreds of
        // callsites would fail their asserts. The above two bugs track fixing
        // all callsites after which, we can uncomment this.
        //
        // return NV_ERR_OBJECT_NOT_FOUND;
        //
        NV_PRINTF(LEVEL_ERROR,
            "Asked for host-specific type(0x%x) for non-host engine type(0x%x),val(0x%08x)\n",
            outType, inType, inVal);
    }

    *pOutVal = pFoundInputEngine->engineData[outType];
    return NV_OK;
}

/**
 * @brief Get the local maximum number of subctx allowed in this TSG
 */
NvU32
kfifoChannelGroupGetLocalMaxSubcontext_GM107
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    KernelChannelGroup *pKernelChannelGroup,
    NvBool              bLegacyMode
)
{
    // Pre-AMPERE, each channel group has the global maximum available
    return kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, bLegacyMode);
}

void
kfifoSetupUserD_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    TRANSFER_SURFACE tSurf = {.pMemDesc = pMemDesc, .offset = 0};

    NV_ASSERT_OK(memmgrMemSet(GPU_GET_MEMORY_MANAGER(pGpu), &tSurf, 0,
        NV_RAMUSERD_CHAN_SIZE, TRANSFER_FLAGS_NONE));
}
/**
 * @brief return number of HW engines
 *
 *  Can be used to loop over all engines in the system by looping from 0
 *  through the value returned by this function and then using
 *  kfifoEngineInfoXlate() with an input type of ENGINE_INFO_TYPE_INVALID.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelFifo   KernelFifo pointer
 *
 * @returns number of HW engines present on chip.
 */
NvU32
kfifoGetNumEngines_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);

    if (pEngineInfo == NULL)
    {
        NV_ASSERT_OR_RETURN(kfifoConstructEngineList_HAL(pGpu, pKernelFifo) == NV_OK, 0);

        pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
        NV_ASSERT_OR_RETURN(pEngineInfo != NULL, 0);
    }

    NV_ASSERT(pEngineInfo->engineInfoListSize);

    // we don't count the SW engine entry at the end of the list
    return pEngineInfo->engineInfoListSize-1;
}

/**
 * @brief Retrieves the name of the engine corresponding to the given @ref ENGINE_INFO_TYPE
 *
 * @param pKernelFifo
 * @param[in] inType
 * @param[in] inVal
 *
 * @returns a string
 */
const char *
kfifoGetEngineName_GM107
(
    KernelFifo *pKernelFifo,
    ENGINE_INFO_TYPE inType,
    NvU32 inVal
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 i;

    if (inType == ENGINE_INFO_TYPE_INVALID)
    {
        NV_ASSERT_OR_RETURN (inVal < pEngineInfo->engineInfoListSize, NULL);
        return pEngineInfo->engineInfoList[inVal].engineName;
    }
    for (i = 0; i < pEngineInfo->engineInfoListSize; ++i)
    {
        if (pEngineInfo->engineInfoList[i].engineData[inType] == inVal)
        {
            return pEngineInfo->engineInfoList[i].engineName;
        }
    }

    return "UNKNOWN";
}

/**
 * @brief Returns the maximum possible number of runlists.
 *
 * Returns a number which represents the limit of any runlistId indexed
 * registers in hardware.  Does not necessarily return how many runlists are
 * active.  In the range of 0..kfifoGetMaxNumRunlists() there may be runlists
 * that are not used.
 *
 * @param pGpu
 * @param pKernelFifo
 */
NvU32
kfifoGetMaxNumRunlists_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);

    return pEngineInfo->maxNumRunlists;
}

NV_STATUS
kfifoGetEnginePbdmaIds_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    ENGINE_INFO_TYPE type,
    NvU32 val,
    NvU32 **ppPbdmaIds,
    NvU32 *pNumPbdmas
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 i;

    if (pEngineInfo == NULL)
    {
        NV_ASSERT_OK_OR_RETURN(kfifoConstructEngineList_HAL(pGpu, pKernelFifo));

        pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
        NV_ASSERT_OR_RETURN(pEngineInfo != NULL, NV_ERR_INVALID_STATE);
    }

    if (type == ENGINE_INFO_TYPE_INVALID)
    {
        NV_ASSERT_OR_RETURN(val < pEngineInfo->engineInfoListSize, NV_ERR_INVALID_ARGUMENT);
        *ppPbdmaIds = pEngineInfo->engineInfoList[val].pbdmaIds;
        *pNumPbdmas = pEngineInfo->engineInfoList[val].numPbdmas;
        return NV_OK;
    }

    for (i = 0; i < pEngineInfo->engineInfoListSize; i++)
    {
        if (pEngineInfo->engineInfoList[i].engineData[type] == val)
        {
            *ppPbdmaIds = pEngineInfo->engineInfoList[i].pbdmaIds;
            *pNumPbdmas = pEngineInfo->engineInfoList[i].numPbdmas;
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

/**
 * @brief finds all engines on the same pbdma as the input
 *
 * pPartnerListParams->partnershipClassId is currently ignored.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param[in/out] pPartnerListParams engineType is input, partnerList/numPartners are ouput
 *
 * @returns NV_OK if successful, error otherwise
 */
NV_STATUS
kfifoGetEnginePartnerList_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams
)
{
    const NvU32 numEngines = kfifoGetNumEngines_HAL(pGpu, pKernelFifo);
    NvU32 i;
    NvU32 srcRunlist;
    NvU32 runlist;
    NvU32 *pSrcPbdmaIds;
    NvU32 numSrcPbdmaIds;
    NvU32 srcPbdmaId;
    NvU32 *pPbdmaIds;
    NvU32 numPbdmaIds;
    NvU32 numClasses = 0;
    ENGDESCRIPTOR engDesc;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(pPartnerListParams->engineType);

    if (pPartnerListParams->runqueue >= kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo))
        return NV_ERR_INVALID_ARGUMENT;

    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                    ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                    (NvU32)rmEngineType,
                                                    ENGINE_INFO_TYPE_RUNLIST,
                                                    &srcRunlist));

    NV_ASSERT_OK_OR_RETURN(kfifoGetEnginePbdmaIds_HAL(pGpu, pKernelFifo,
                                                      ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                      (NvU32)rmEngineType,
                                                      &pSrcPbdmaIds,
                                                      &numSrcPbdmaIds));

    pPartnerListParams->numPartners = 0;

    // Get the PBDMA ID for the runqueue-th runqueue
    if (pPartnerListParams->runqueue >= numSrcPbdmaIds)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    srcPbdmaId = pSrcPbdmaIds[pPartnerListParams->runqueue];

    //
    // Find all engines sharing a runlist with the input engine, add each to
    // the output array.
    //
    for (i = 0; i < numEngines; i++)
    {
        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                        ENGINE_INFO_TYPE_INVALID, i,
                                                        ENGINE_INFO_TYPE_ENG_DESC, &engDesc));

        NV_ASSERT_OK_OR_RETURN(gpuGetClassList(pGpu, &numClasses, NULL, engDesc));
        if (numClasses == 0)
        {
            NV_PRINTF(LEVEL_INFO,
                      "EngineID %x is not part classDB, skipping\n",
                      engDesc);
            continue;
        }

        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                        ENGINE_INFO_TYPE_INVALID, i,
                                                        ENGINE_INFO_TYPE_RUNLIST, &runlist));

        if (runlist == srcRunlist)
        {
            NvU32 j;
            RM_ENGINE_TYPE localRmEngineType;

            NV_ASSERT_OK_OR_RETURN(kfifoGetEnginePbdmaIds_HAL(pGpu, pKernelFifo,
                                                              ENGINE_INFO_TYPE_INVALID, i,
                                                              &pPbdmaIds, &numPbdmaIds));

            for (j = 0; j < numPbdmaIds; j++)
            {
                if (pPbdmaIds[j] == srcPbdmaId)
                {
                    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                                    ENGINE_INFO_TYPE_INVALID, i,
                                                                    ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&localRmEngineType));

                    // Don't include input in output list
                    if (localRmEngineType != rmEngineType)
                    {
                        pPartnerListParams->partnerList[pPartnerListParams->numPartners++] =
                            gpuGetNv2080EngineType(localRmEngineType);

                        if (pPartnerListParams->numPartners >= NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS)
                            return NV_ERR_INVALID_ARGUMENT;
                    }
                }
            }
        }
    }

    return NV_OK;
}

/**
 * @brief Check if the runlist has TSG support
 *
 * Currently, we only enable the TSG runlist for GR
 *
 *  @return NV_TRUE if TSG is supported, NV_FALSE if not
 */
NvBool
kfifoRunlistIsTsgHeaderSupported_GM107
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    NvU32 runlistId
)
{
    NvU32 tmp_runlist;

    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_ENG_DESC,
        ENG_GR(0), ENGINE_INFO_TYPE_RUNLIST, &tmp_runlist) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "can't find runlist ID for engine ENG_GR(0)!\n");
        NV_ASSERT(0);
        return NV_FALSE;
    }

    return tmp_runlist == runlistId;
}

/**
 * @brief Get the runlist entry size
 *
 * @param pKernelFifo
 *
 * @return size in bytes
 */
NvU32
kfifoRunlistGetEntrySize_GM107
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_SIZE;
}

/**
 * @brief Get the runlist base shift amount
 *
 * @param pKernelFifo
 *
 * @return shift amount
 */
NvU32
kfifoRunlistGetBaseShift_GM107
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_BASE_SHIFT;
}

/**
 * @brief Pre-allocate BAR1 userd space
 *
 * @param   pGpu
 * @param   pKernelFifo
 *
 * @returns NV_STATUS
 */
NV_STATUS
kfifoPreAllocUserD_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    OBJGPU     *pParentGpu             = gpumgrGetParentGPU(pGpu);
    KernelFifo *pParentKernelFifo      = GPU_GET_KERNEL_FIFO(pParentGpu);
    KernelBus  *pKernelBus             = GPU_GET_KERNEL_BUS(pGpu);
    NvBool      bCoherentCpuMapping    = NV_FALSE;
    NV_STATUS   status                 = NV_OK;
    NvU64       temp                   = 0;
    NvU32       userdSize;
    NvU32       userdShift;
    NvU32       numChannels;
    NvBool      bFifoFirstInit;
    NvU32       flags                  = MEMDESC_FLAGS_NONE;
    NvU32       mapFlags               = BUS_MAP_FB_FLAGS_MAP_DOWNWARDS |
                                         BUS_MAP_FB_FLAGS_MAP_UNICAST;
    NvU32       currentGpuInst         = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    CHID_MGR   *pChidMgr               = kfifoGetChidMgr(pGpu, pKernelFifo, 0);

    MemoryManager     *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager  *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    PREALLOCATED_USERD_INFO *pUserdInfo  = &pParentKernelFifo->userdInfo;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // We don't support RM allocated USERD for vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        return NV_OK;
    }

    bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    if (pUserdInfo->userdBar1CpuPtr == NULL)
    {
        bFifoFirstInit = NV_TRUE;
    }
    else
    {
        mapFlags |= BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED;
        bFifoFirstInit = NV_FALSE;
    }

    //
    // Allocate the physical memory associated with the UserD if this is
    // the first GPU to init fifo. This relies on the assumption that
    // UserD is shared physmem.
    //
    if (bFifoFirstInit)
    {
        pUserdInfo->userdBar1MapStartOffset   =  0;
        pUserdInfo->userdBar1MapSize          =  0;

        // This is a WAR for HW bug 600241
        if (pUserdInfo->userdAperture == ADDR_SYSMEM)
        {
            pKernelFifo->bUserdInSystemMemory = NV_TRUE;
        }
    }

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, &userdShift);

    numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);

    // Alloc USERD of size numChannels * sizeof( USERD ) for each gpu
    status = memdescCreate(&pUserdInfo->userdPhysDesc[currentGpuInst], pGpu,
                           userdSize * numChannels,
                           1ULL << userdShift,
                           NV_TRUE,
                           pUserdInfo->userdAperture,
                           pUserdInfo->userdAttr,
                           flags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not memdescCreate for USERD for %x #channels\n",
                  numChannels);
        DBG_BREAKPOINT();
        goto fail;
    }
    temp = pUserdInfo->userdPhysDesc[currentGpuInst]->Size;

    //
    // For vGPU, do not allocate USERD memory in guest.
    // vGPU does all HW management in host, so host RM will
    // allocate the real USERD memory.
    //
    if (IS_VIRTUAL(pGpu))
    {
        // Force page size to 4KB to match host phys access
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager,
                                     pUserdInfo->userdPhysDesc[currentGpuInst],
                                     AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
        if (bFifoFirstInit)
        {
            pUserdInfo->userdBar1MapStartOffset = kfifoGetUserdBar1MapStartOffset_HAL(pGpu, pKernelFifo);
        }
    }
    else
    {
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_81, 
                        pUserdInfo->userdPhysDesc[currentGpuInst]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate USERD for %x #channels\n",
                      numChannels);
            DBG_BREAKPOINT();
            goto fail;
        }

        // Force page size to 4KB in broadcast to match host phys access
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pUserdInfo->userdPhysDesc[currentGpuInst],
                                     AT_GPU, RM_ATTR_PAGE_SIZE_4KB);

        //
        // If coherent link is available, just get a coherent mapping to USERD and
        // lie about the BAR1 offset, since we are not using BAR1
        // TODO: Make these bar1 offsets unicast on each gpu as well
        //
        if (bCoherentCpuMapping &&
            (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM))
        {

            NV_PRINTF(LEVEL_INFO, "Mapping USERD with coherent link (USERD in FBMEM).\n");
            NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
            NV_ASSERT(pUserdInfo->userdPhysDesc[currentGpuInst]->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);

            if (bFifoFirstInit)
            {
                pUserdInfo->userdBar1MapStartOffset =  pUserdInfo->userdPhysDesc[currentGpuInst]->_pteArray[0] +
                                                       pUserdInfo->userdPhysDesc[currentGpuInst]->PteAdjust;
            }
        }
        //
        // get sysmem mapping for USERD if USERD is in sysmem and reflected BAR access is not allowed
        //
        else if ((bCoherentCpuMapping &&
                 memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_SYSMEM &&
                 !kbusIsReflectedMappingAccessAllowed(pKernelBus)) ||
                 kbusIsBar1Disabled(pKernelBus))
        {
            NV_PRINTF(LEVEL_INFO, "Mapping USERD with coherent link (USERD in SYSMEM).\n");

            if (bFifoFirstInit)
            {
                pUserdInfo->userdBar1MapStartOffset =
                        memdescGetPhysAddr(pUserdInfo->userdPhysDesc[currentGpuInst], AT_CPU, 0);
            }
        }
        else
        {
            // vGpu may boot with partitioning enabled but that's not true for host RM
            if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
            {
                status = NV_ERR_INVALID_STATE;
                NV_PRINTF(LEVEL_ERROR, "Pre-allocated USERD is not supported with MIG\n");
                DBG_BREAKPOINT();
                goto fail;
            }
            // Now BAR1 map it
            status = kbusMapFbApertureSingle(pGpu, pKernelBus, pUserdInfo->userdPhysDesc[currentGpuInst], 0,
                                             &pUserdInfo->userdBar1MapStartOffset,
                                             &temp, mapFlags | BUS_MAP_FB_FLAGS_PRE_INIT, NULL);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Could not map USERD to BAR1\n");
                DBG_BREAKPOINT();
                goto fail;
            }

            // Add current GPU to list of GPUs referencing pFifo userD bar1
            pUserdInfo->userdBar1RefMask |= NVBIT(pGpu->gpuInstance);
        }
    }

    if (bFifoFirstInit)
    {
        pUserdInfo->userdBar1MapSize = NvU64_LO32(temp);

        if (bCoherentCpuMapping &&
            (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM))
        {
            status = kbusMapCoherentCpuMapping_HAL(pGpu, pKernelBus,
                                                   pUserdInfo->userdPhysDesc[currentGpuInst],
                                                   0,
                                                   pUserdInfo->userdBar1MapSize,
                                                   NV_PROTECT_READ_WRITE,
                                                   (void**)&pUserdInfo->userdBar1CpuPtr,
                                                   (void**)&pUserdInfo->userdBar1Priv);
        }
        else if ((bCoherentCpuMapping &&
                 memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_SYSMEM &&
                 !kbusIsReflectedMappingAccessAllowed(pKernelBus)) &&
                 !kbusIsBar1Disabled(pKernelBus))
        {
            status = osMapPciMemoryKernelOld(pGpu,
                                             pUserdInfo->userdBar1MapStartOffset,
                                             pUserdInfo->userdBar1MapSize,
                                             NV_PROTECT_READ_WRITE,
                                             (void**)&pUserdInfo->userdBar1CpuPtr,
                                             NV_MEMORY_CACHED);
        }
        else if (kbusIsBar1Disabled(pKernelBus))
        {
            status = memdescMap(pUserdInfo->userdPhysDesc[currentGpuInst],
                                0,
                                pUserdInfo->userdBar1MapSize,
                                NV_TRUE,
                                NV_PROTECT_READ_WRITE,
                                (void**)&pUserdInfo->userdBar1CpuPtr,
                                (void**)&pUserdInfo->userdBar1Priv);
        }
        else
        {
            // Cpu map the BAR1 snoop range
            status = osMapPciMemoryKernelOld(pGpu, gpumgrGetGpuPhysFbAddr(pGpu) +
                                             pUserdInfo->userdBar1MapStartOffset,
                                             pUserdInfo->userdBar1MapSize,
                                             NV_PROTECT_READ_WRITE,
                                             (void**)&pUserdInfo->userdBar1CpuPtr,
                                             NV_MEMORY_UNCACHED);
        }

        if ((pUserdInfo->userdBar1CpuPtr == NULL) && (status != NV_OK))
        {
            NV_PRINTF(LEVEL_ERROR, "Could not cpu map BAR1 snoop range\n");
            DBG_BREAKPOINT();
            goto fail;
        }
    }

    NV_PRINTF(LEVEL_INFO,
              "USERD Preallocated phys @ 0x%llx bar1 offset @ 0x%llx of size 0x%x\n",
              memdescGetPhysAddr(pUserdInfo->userdPhysDesc[currentGpuInst], AT_GPU, 0),
              pUserdInfo->userdBar1MapStartOffset,
              pUserdInfo->userdBar1MapSize);

    return status;

fail:
    kfifoFreePreAllocUserD_HAL(pGpu, pKernelFifo);

    return status;
}

/**
 * @brief Free the pre-allocated BAR1 userd space
 *
 * @param   pGpu
 * @param   pKernelFifo
 *
 * @returns NV_STATUS
 */
void
kfifoFreePreAllocUserD_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    OBJGPU            *pParentGpu           = gpumgrGetParentGPU(pGpu);
    KernelBus         *pKernelBus           = GPU_GET_KERNEL_BUS(pGpu);
    NvU32              currentGpuInst       = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    KernelFifo        *pParentKernelFifo    = GPU_GET_KERNEL_FIFO(pParentGpu);
    PREALLOCATED_USERD_INFO *pUserdInfo     = &pParentKernelFifo->userdInfo;
    NvBool             bCoherentCpuMapping  = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM);

    // We don't support RM allocated USERD for vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        return;
    }

    if (gpumgrGetBcEnabledStatus(pGpu))
    {
        DBG_BREAKPOINT();
    }

    if (bCoherentCpuMapping)
    {
        NV_PRINTF(LEVEL_INFO, "Unmapping USERD from NVLINK.\n");
        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
    }

    if (pUserdInfo->userdBar1CpuPtr)
    {
        if (bCoherentCpuMapping)
        {
            kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus,
                pUserdInfo->userdPhysDesc[currentGpuInst],
                pUserdInfo->userdBar1CpuPtr,
                pUserdInfo->userdBar1Priv);
        }
        else if (kbusIsBar1Disabled(pKernelBus))
        {
            memdescUnmap(pUserdInfo->userdPhysDesc[currentGpuInst],
                            NV_TRUE,
                            osGetCurrentProcess(),
                            (void*)pUserdInfo->userdBar1CpuPtr,
                            (void*)pUserdInfo->userdBar1Priv);
        }
        else
        {
            osUnmapPciMemoryKernelOld(pGpu, pUserdInfo->userdBar1CpuPtr);
        }

        pUserdInfo->userdBar1CpuPtr = NULL;
    }

    if (pUserdInfo->userdBar1MapSize)
    {
        if ((!IS_VIRTUAL(pGpu)) && (!bCoherentCpuMapping))
        {
            if ((pUserdInfo->userdBar1RefMask & NVBIT(pGpu->gpuInstance)) != 0)
            {
                //
                // Unmap in UC for each GPU with a pKernelFifo userd
                // reference mapped through bar1
                //
                kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                          pUserdInfo->userdPhysDesc[currentGpuInst],
                                          pUserdInfo->userdBar1MapStartOffset,
                                          pUserdInfo->userdBar1MapSize,
                                          BUS_MAP_FB_FLAGS_MAP_UNICAST | BUS_MAP_FB_FLAGS_PRE_INIT);
                pUserdInfo->userdBar1RefMask &= (~NVBIT(pGpu->gpuInstance));
            }

        }
    }

    // Unallocated memdescFrees are allowed.
    memdescFree(pUserdInfo->userdPhysDesc[currentGpuInst]);
    memdescDestroy(pUserdInfo->userdPhysDesc[currentGpuInst]);
    pUserdInfo->userdPhysDesc[currentGpuInst] = NULL;
    NV_PRINTF(LEVEL_INFO, "Freeing preallocated USERD phys and bar1 range\n");
}

//
// Returns the BAR1 offset and size of the entire USERD mapping.
//
NV_STATUS
kfifoGetUserdBar1MapInfo_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU64      *pBar1MapOffset,
    NvU32      *pBar1MapSize
)
{
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    // We don't support RM allocated USERD in vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        *pBar1MapOffset = 0;
        *pBar1MapSize   = 0;

        return NV_OK;
    }

    if (pUserdInfo->userdBar1MapSize == 0 )
    {
        NV_PRINTF(LEVEL_ERROR, "BAR1 map of USERD has not been setup yet\n");
        NV_ASSERT( 0 );
        return NV_ERR_GENERIC;
    }

    *pBar1MapOffset = pUserdInfo->userdBar1MapStartOffset;
    *pBar1MapSize   = pUserdInfo->userdBar1MapSize;

    return NV_OK;
}

/**
 * @brief Determines the aperture and attribute of memory where userd is located.
 *
 * @param pKernelFifo[in]
 * @param pUserdAperture[out]
 * @param pUserdAttribute[out]
 *
 * @returns NV_STATUS
 */
NV_STATUS
kfifoGetUserdLocation_GM107
(
    KernelFifo *pKernelFifo,
    NvU32 *pUserdAperture,
    NvU32 *pUserdAttribute
)
{
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    NV_ASSERT_OR_RETURN(pUserdAperture != NULL && pUserdAttribute != NULL,
                        NV_ERR_INVALID_POINTER);

    *pUserdAperture = pUserdInfo->userdAperture;
    *pUserdAttribute = pUserdInfo->userdAttr;

    return NV_OK;
}

/**
 * @brief Returns size/address shift for USERD's BAR1 mapping
 *
 * @param pKernelFifo
 * @param[out] pSize populated with USERD size if non-null
 * @param[out] pAddrShift populated with USERD address shift if non-null
 */
void
kfifoGetUserdSizeAlign_GM107
(
    KernelFifo *pKernelFifo,
    NvU32 *pSize,
    NvU32 *pAddrShift
)
{
    if (pSize != NULL)
        *pSize = 1<<NV_RAMUSERD_BASE_SHIFT;
    if (pAddrShift != NULL)
        *pAddrShift = NV_RAMUSERD_BASE_SHIFT;
}

/**
 * @brief Determines if an engine is a host engine and if so, if it is present.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param[in] engDesc
 * @param[out]  pPresent NV_TRUE if the engine is present, NV_FALSE if not.
 *
 * @return OK if host could determine the engine's presence.  ERROR otherwise
 */
NV_STATUS
kfifoCheckEngine_GM107
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engDesc,
    NvBool     *pPresent
)
{
    NvU32 bEschedDriven = NV_FALSE;
    NV_STATUS status;

    status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
        ENGINE_INFO_TYPE_ENG_DESC,              engDesc,
        ENGINE_INFO_TYPE_IS_HOST_DRIVEN_ENGINE, &bEschedDriven);

    *pPresent = (status == NV_OK) && bEschedDriven;

    return NV_OK;
}
