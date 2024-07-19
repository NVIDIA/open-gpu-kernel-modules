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

#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

#include "ctrl/ctrla06c.h"  // NVA06C_CTRL_INTERLEAVE_LEVEL_*

// Static functions
static void _kchangrpFreeAllEngCtxDescs(OBJGPU *pGpu, KernelChannelGroup *pKernelChannelGroup);

NV_STATUS
kchangrpConstruct_IMPL(KernelChannelGroup *pKernelChannelGroup)
{
    return NV_OK;
}

void
kchangrpDestruct_IMPL(KernelChannelGroup *pKernelChannelGroup)
{
    return;
}

void
kchangrpSetState_IMPL
(
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               subdevice,
    CHANNELGROUP_STATE  state
)
{
    ct_assert(CHANNELGROUP_STATE_COUNT <= 8 * sizeof(NvU32));
    pKernelChannelGroup->pStateMask[subdevice] |= NVBIT(state);
}

void
kchangrpClearState_IMPL
(
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               subdevice,
    CHANNELGROUP_STATE  state
)
{
    ct_assert(CHANNELGROUP_STATE_COUNT <= 8 * sizeof(NvU32));
    pKernelChannelGroup->pStateMask[subdevice] &= ~NVBIT(state);
}

NvBool
kchangrpIsStateSet_IMPL
(
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               subdevice,
    CHANNELGROUP_STATE  state
)
{
    ct_assert(CHANNELGROUP_STATE_COUNT <= 8 * sizeof(NvU32));
    return !!(pKernelChannelGroup->pStateMask[subdevice] & NVBIT(state));
}

/**
 * @brief Allocates sw state for channel group
 *
 * This allocates and initiazes sw state for channel group.
 * No channels are added to this at this point. It will be done
 * on alloc of a channel to this group using kchangrpAddChannelToGroup.
 *
 * All channels in a group share VASpace and KernelCtxShare. ChannelGroup
 * alloc can be passed in a valid ctx share pointer. If not, we
 * will allocate a new ctx share.
 *
 * This function is not called in broadcast mode
 *
 * @param pGpu
 * @param pChanGrp
 * @param[in] pVAS
 * @param[in] gfid
 * @param[in] engineType
 * @returns NV_OK on success
 */
NV_STATUS
kchangrpInit_IMPL
(
    OBJGPU                *pGpu,
    KernelChannelGroup    *pKernelChannelGroup,
    OBJVASPACE            *pVAS,
    NvU32                  gfid
)
{
    NV_STATUS         status       = NV_OK;
    KernelFifo       *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR         *pChidMgr     = NULL;
    NvU32             grpID        = 0;
    NvU32             maxSubctx;
    NvU32             index;
    NvBool            bMapFaultMthdBuffers = NV_FALSE;
    NvU32             runlistId    = 0;
    NvU32             runQueues    = 0;
    NvU32             subDeviceCount = gpumgrGetSubDeviceMaxValuePlus1(pGpu);

    // Initialize subctx bitmasks, state mask and interleave level
    {
        NvU32 subDeviceCount = gpumgrGetSubDeviceMaxValuePlus1(pGpu);

        pKernelChannelGroup->ppSubctxMask = portMemAllocNonPaged(
            subDeviceCount * (sizeof *pKernelChannelGroup->ppSubctxMask));
        pKernelChannelGroup->ppZombieSubctxMask = portMemAllocNonPaged(
            subDeviceCount * (sizeof *pKernelChannelGroup->ppZombieSubctxMask));
        pKernelChannelGroup->pStateMask = portMemAllocNonPaged(
            subDeviceCount * (sizeof *pKernelChannelGroup->pStateMask));
        pKernelChannelGroup->pInterleaveLevel = portMemAllocNonPaged(
            subDeviceCount * (sizeof *pKernelChannelGroup->pInterleaveLevel));

        NV_ASSERT_OR_ELSE((pKernelChannelGroup->ppSubctxMask != NULL &&
                              pKernelChannelGroup->ppZombieSubctxMask != NULL &&
                              pKernelChannelGroup->pStateMask != NULL &&
                              pKernelChannelGroup->pInterleaveLevel != NULL),
                          status = NV_ERR_NO_MEMORY; goto failed);

        portMemSet(pKernelChannelGroup->ppSubctxMask, 0,
                   subDeviceCount * (sizeof *pKernelChannelGroup->ppSubctxMask));
        portMemSet(pKernelChannelGroup->ppZombieSubctxMask, 0,
                   subDeviceCount * (sizeof *pKernelChannelGroup->ppZombieSubctxMask));
        portMemSet(pKernelChannelGroup->pStateMask, 0,
                   subDeviceCount * (sizeof *pKernelChannelGroup->pStateMask));
        portMemSet(pKernelChannelGroup->pInterleaveLevel, 0,
                   subDeviceCount * (sizeof *pKernelChannelGroup->pInterleaveLevel));
    }

    // Determine initial runlist for this TSG, using engine type if provided
    pKernelChannelGroup->runlistId = kfifoGetDefaultRunlist_HAL(pGpu,
        pKernelFifo,
        pKernelChannelGroup->engineType);

    if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        //
        // pKernelChannelGroup->engineType should hold the valid engine type if
        // bUsePerRunlistChram is set. If it is not set,
        // it will be ignored by the subsequent call to fifoChidMgrGet anyway
        //
        NV_ASSERT_OK_OR_RETURN(
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                     pKernelChannelGroup->engineType,
                                     ENGINE_INFO_TYPE_RUNLIST,
                                     &runlistId));
    }

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

    NV_ASSERT_OK_OR_RETURN(kfifoChidMgrAllocChannelGroupHwID(pGpu, pKernelFifo, pChidMgr, &grpID));

    pKernelChannelGroup->grpID = grpID;
    pKernelChannelGroup->timesliceUs = kfifoChannelGroupGetDefaultTimeslice_HAL(pKernelFifo);

    NV_ASSERT_OK_OR_GOTO(status,
        kfifoChannelGroupSetTimeslice(pGpu, pKernelFifo, pKernelChannelGroup,
            pKernelChannelGroup->timesliceUs, NV_TRUE),
        failed);

    NV_ASSERT_OK_OR_GOTO(status,
        kfifoChannelListCreate(pGpu, pKernelFifo, &pKernelChannelGroup->pChanList),
        failed);

    // Alloc space for one ENGINE_CTX_DESCRIPTOR* per subdevice)
    pKernelChannelGroup->ppEngCtxDesc = portMemAllocNonPaged(subDeviceCount * sizeof(ENGINE_CTX_DESCRIPTOR *));
    NV_ASSERT_OR_ELSE(pKernelChannelGroup->ppEngCtxDesc != NULL, status = NV_ERR_NO_MEMORY; goto failed);
    portMemSet(pKernelChannelGroup->ppEngCtxDesc, 0, subDeviceCount * sizeof(ENGINE_CTX_DESCRIPTOR *));

    pKernelChannelGroup->pSubctxIdHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
    if (pKernelChannelGroup->pSubctxIdHeap == NULL)
    {
        NV_CHECK(LEVEL_ERROR, pKernelChannelGroup->pSubctxIdHeap != NULL);
        status = NV_ERR_NO_MEMORY;
        goto failed;
    }


    maxSubctx = kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu, pKernelFifo,
                                                           pKernelChannelGroup,
                                                           NV_FALSE);

    constructObjEHeap(pKernelChannelGroup->pSubctxIdHeap,
                      0,
                      maxSubctx,
                      sizeof(KernelCtxShare *),
                      0);

    // Subcontext mode is now enabled on all chips.
    pKernelChannelGroup->bLegacyMode = NV_FALSE;

    // We cache the TSG VAS to support legacy mode
    pKernelChannelGroup->pVAS = pVAS;
    pKernelChannelGroup->gfid = gfid;

    // Get number of runqueues
    runQueues = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    NV_ASSERT((runQueues > 0));

    // Allocate method buffer struct. One per runqueue
    pKernelChannelGroup->pMthdBuffers = NULL;
    pKernelChannelGroup->pMthdBuffers = portMemAllocNonPaged(
        (sizeof(HW_ENG_FAULT_METHOD_BUFFER) * runQueues));
    if (pKernelChannelGroup->pMthdBuffers == NULL)
    {
        NV_CHECK(LEVEL_ERROR, pKernelChannelGroup->pMthdBuffers != NULL);
        status = NV_ERR_NO_MEMORY;
        goto failed;
    }
    portMemSet(pKernelChannelGroup->pMthdBuffers,
               0,
               (sizeof(HW_ENG_FAULT_METHOD_BUFFER) * runQueues));

    // Allocate method buffer memory
    status = kchangrpAllocFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  " Fault method buffer allocation failed for group ID 0x%0x with status 0x%0x\n",
                  grpID, status);
        DBG_BREAKPOINT();
        goto failed;
    }

    //
    // Map method buffer to invisible BAR2
    // Skipped for GSP since its done in channel setup
    //
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        bMapFaultMthdBuffers = NV_TRUE;

        for (index = 0; index < runQueues; index++)
        {
            status = kchangrpMapFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup, index);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          " Fault method buffer BAR2 mapping failed for group ID 0x%0x with status 0x%0x\n",
                          grpID, status);
                DBG_BREAKPOINT();
                goto failed;
            }
        }
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    // make sure zombie subcontext mask is reset
    for (index = 0; index < SUBCTX_MASK_ARRAY_SIZE; index++)
        pKernelChannelGroup->ppZombieSubctxMask[gpumgrGetSubDeviceInstanceFromGpu(pGpu)][index] = 0;
    SLI_LOOP_END

    //
    // Add the channel group to the table that keeps track of
    // <grpID, PCHGRP> tuples
    //
    if (mapInsertExisting(pChidMgr->pChanGrpTree, grpID, pKernelChannelGroup))
    {
        status = NV_OK;
    }
    else
    {
        status = NV_ERR_INVALID_STATE;
    }

    NV_ASSERT(status == NV_OK);

    return NV_OK;

failed:
    if (pKernelChannelGroup->pSubctxIdHeap != NULL)
    {
        pKernelChannelGroup->pSubctxIdHeap->eheapDestruct(
            pKernelChannelGroup->pSubctxIdHeap);
        portMemFree(pKernelChannelGroup->pSubctxIdHeap);
        pKernelChannelGroup->pSubctxIdHeap = NULL;
    }

    _kchangrpFreeAllEngCtxDescs(pGpu, pKernelChannelGroup);

    if (pKernelChannelGroup->pChanList != NULL)
    {
        kfifoChannelListDestroy(pGpu, pKernelFifo, pKernelChannelGroup->pChanList);
        pKernelChannelGroup->pChanList = NULL;
    }

    if (pKernelChannelGroup->pMthdBuffers != NULL)
    {
        if (bMapFaultMthdBuffers)
        {
            for (index = 0; index < runQueues; index++)
            {
                kchangrpUnmapFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup, index);
            }
        }

        kchangrpFreeFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup);

        portMemFree(pKernelChannelGroup->pMthdBuffers);
        pKernelChannelGroup->pMthdBuffers = NULL;
    }

    if (pChidMgr != NULL)
        kfifoChidMgrFreeChannelGroupHwID(pGpu, pKernelFifo, pChidMgr, grpID);

    portMemFree(pKernelChannelGroup->ppSubctxMask);
    pKernelChannelGroup->ppSubctxMask = NULL;
    portMemFree(pKernelChannelGroup->ppZombieSubctxMask);
    pKernelChannelGroup->ppZombieSubctxMask = NULL;
    portMemFree(pKernelChannelGroup->pStateMask);
    pKernelChannelGroup->pStateMask = NULL;
    portMemFree(pKernelChannelGroup->pInterleaveLevel);
    pKernelChannelGroup->pInterleaveLevel = NULL;

    return status;
}

/**
 * @brief Frees sw state for channel group
 *
 * This frees sw state for channel group.
 *
 * This function is not called in broadcast mode
 *
 * @param pGpu
 * @param pKernelChannelGroup
 *
 * @returns NV_OK on success
 */
NV_STATUS
kchangrpDestroy_IMPL
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NV_STATUS           status;
    KernelFifo         *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR           *pChidMgr;
    KernelChannelGroup *pKernelChannelGroupTemp;
    NvU64               maxSubctx;
    NvU64               numFreeSubctx;
    NvU32               runlistId = 0;
    NvU32               runQueues = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    NvU32               index;

    if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        //
        // pKernelChannelGroup->engineType should hold the valid engine type if
        // bUsePerRunlistChram is set. If it is not set,
        // it will be ignored by the subsequent call to fifoChidMgrGet anyway
        //
        NV_ASSERT_OK_OR_RETURN(
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                     pKernelChannelGroup->engineType,
                                     ENGINE_INFO_TYPE_RUNLIST,
                                     &runlistId));
    }

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

    // If already destroyed, nothing to be done
    if (pKernelChannelGroup == NULL)
        return NV_OK;

    // There should be no channels in this group
    NV_ASSERT(pKernelChannelGroup->chanCount == 0);

    status = pKernelChannelGroup->pSubctxIdHeap->eheapGetSize(
        pKernelChannelGroup->pSubctxIdHeap,
        &maxSubctx);
    NV_ASSERT(status == NV_OK);

    status = pKernelChannelGroup->pSubctxIdHeap->eheapGetFree(
        pKernelChannelGroup->pSubctxIdHeap,
        &numFreeSubctx);
    NV_ASSERT(status == NV_OK);

    //
    // Resource Server should have already freed any client allocated or legacy
    // pre-allocated context shares
    //
    NV_ASSERT(maxSubctx == kfifoChannelGroupGetLocalMaxSubcontext_HAL(
                               pGpu,
                               pKernelFifo,
                               pKernelChannelGroup,
                               pKernelChannelGroup->bLegacyMode));
    NV_ASSERT(maxSubctx == numFreeSubctx);

    pKernelChannelGroup->pSubctxIdHeap->eheapDestruct(
        pKernelChannelGroup->pSubctxIdHeap);
    portMemFree(pKernelChannelGroup->pSubctxIdHeap);
    pKernelChannelGroup->pSubctxIdHeap = NULL;

    _kchangrpFreeAllEngCtxDescs(pGpu, pKernelChannelGroup);

    kfifoChannelListDestroy(pGpu, pKernelFifo, pKernelChannelGroup->pChanList);
    pKernelChannelGroup->pChanList= NULL;

    if (pChidMgr != NULL)
    {
        // Remove this from the <grIPD, PCHGRP> that we maintain in OBJFIFO
        pKernelChannelGroupTemp = mapFind(pChidMgr->pChanGrpTree, pKernelChannelGroup->grpID);
        if (pKernelChannelGroupTemp == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Could not find channel group %d\n",
                      pKernelChannelGroup->grpID);
            return NV_ERR_OBJECT_NOT_FOUND;
        }
        mapRemove(pChidMgr->pChanGrpTree, pKernelChannelGroupTemp);

        // Release the free grpID
        kfifoChidMgrFreeChannelGroupHwID(pGpu, pKernelFifo, pChidMgr, pKernelChannelGroup->grpID);
    }

    //
    // Free the method buffer if applicable
    // For SR-IOV, Guest RM allocates the mthd buffers but later RPCs into the
    // host, and populates the data structure, but it should be free-d only by
    // guest RM.
    // In host RM, we only need to free the memory allocated for pMthdBuffers.
    // In case of SRIOV heavy, we need to free the method buffer in host RM.
    //
    if ((IS_GFID_PF(pKernelChannelGroup->gfid) ||
         gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) &&
        pKernelChannelGroup->pMthdBuffers)
    {
        //
        // Unmap method buffer from invisible BAR2
        // Skipped for GSP since its done during fifoFree
        //
        if (!RMCFG_FEATURE_PLATFORM_GSP)
        {
            for (index = 0; index < runQueues; index++)
            {
                kchangrpUnmapFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup, index);
            }
        }

        kchangrpFreeFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup);
    }

    portMemFree(pKernelChannelGroup->pMthdBuffers);
    pKernelChannelGroup->pMthdBuffers = NULL;

    portMemFree(pKernelChannelGroup->ppSubctxMask);
    pKernelChannelGroup->ppSubctxMask = NULL;
    portMemFree(pKernelChannelGroup->ppZombieSubctxMask);
    pKernelChannelGroup->ppZombieSubctxMask = NULL;
    portMemFree(pKernelChannelGroup->pStateMask);
    pKernelChannelGroup->pStateMask = NULL;
    portMemFree(pKernelChannelGroup->pInterleaveLevel);
    pKernelChannelGroup->pInterleaveLevel = NULL;

    return NV_OK;
}


/**
 * @brief Adds channel to a channel group
 *
 * This function is not called in broadcast mode
 *
 * @returns NV_OK on success
 */
NV_STATUS
kchangrpAddChannel_IMPL
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    KernelChannel      *pKernelChannel
)
{
    NV_STATUS       status;
    KernelFifo     *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32           maxChanCount;
    NvU32           subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    KernelCtxShare *pKernelCtxShare;


    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_POINTER);

    maxChanCount = kfifoGetMaxChannelGroupSize_HAL(pKernelFifo);
    if (pKernelChannelGroup->chanCount == maxChanCount)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "There are already max %d channels in this group\n",
                  maxChanCount);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    NV_ASSERT_OR_RETURN(pKernelChannel->pKernelCtxShareApi != NULL, NV_ERR_INVALID_STATE);
    pKernelCtxShare = pKernelChannel->pKernelCtxShareApi->pShareData;
    NV_ASSERT_OR_RETURN(pKernelCtxShare != NULL, NV_ERR_INVALID_STATE);

    // If runlist was set on channel - assert that it's the same runlist as TSG,
    if (kchannelIsRunlistSet(pGpu, pKernelChannel))
        NV_ASSERT_OR_RETURN(pKernelChannelGroup->runlistId ==
                                kchannelGetRunlistId(pKernelChannel),
                            NV_ERR_INVALID_STATE);

    pKernelChannel->subctxId = pKernelCtxShare->subctxId;

    NV_PRINTF(LEVEL_INFO,
              "Channel 0x%x within TSG 0x%x is using subcontext 0x%x\n",
              kchannelGetDebugTag(pKernelChannel), pKernelChannelGroup->grpID, pKernelChannel->subctxId);

    status = kfifoChannelListAppend(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                                   pKernelChannel,
                                   pKernelChannelGroup->pChanList);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not add channel to channel list\n");
        return status;
    }
    pKernelChannelGroup->chanCount++;

    // Initialize channel's interleave level to match TSG's
    NV_ASSERT_OK_OR_RETURN(
        kchangrpSetInterleaveLevel(pGpu,
                             pKernelChannelGroup,
                             pKernelChannelGroup->pInterleaveLevel[subdevInst]));

    return NV_OK;
}


/**
 * @brief Removes channel from a channel group
 *
 * This function is not called in broadcast mode
 *
 * @param pGpu
 * @param pKernelChannelGroup
 * @param[in] pKernelChannel
 *
 * @returns NV_OK on success
 */
NV_STATUS
kchangrpRemoveChannel_IMPL
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    KernelChannel      *pKernelChannel
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_POINTER);

    status = kfifoChannelListRemove(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                                   pKernelChannel,
                                   pKernelChannelGroup->pChanList);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not remove channel from channel list\n");
        return status;
    }

    if (pKernelChannelGroup->chanCount == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Channelcount in channel group not right!!!\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    pKernelChannelGroup->chanCount--;

    if (pKernelChannelGroup->chanCount == 0)
    {
        pKernelChannelGroup->bRunlistAssigned = NV_FALSE;

        //
        // Unmap method buffer from invisible BAR2 if this is the last channel in TSG
        // Done for GSP only
        //
        if (pKernelChannelGroup->pMthdBuffers != NULL && RMCFG_FEATURE_PLATFORM_GSP)
        {
            KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
            NvU32 runQueues = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
            NvU32 index;

            for (index = 0; index < runQueues; index++)
            {
                kchangrpUnmapFaultMethodBuffers_HAL(pGpu,
                    pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup,
                    index);
            }
        }
    }

    return NV_OK;
}


/**
 * @brief Set interleave level for channel group
 *
 * This function sets interleave level for channel group
 * and also updates interleave level for all channels
 *
 * @param pGpu
 * @param pKernelChannelGroup
 * @param value
 *
 * @returns NV_OK on success
 */
NV_STATUS
kchangrpSetInterleaveLevel_IMPL
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               value
)
{
    switch (value)
    {
        case NVA06C_CTRL_INTERLEAVE_LEVEL_LOW:
        case NVA06C_CTRL_INTERLEAVE_LEVEL_MEDIUM:
        case NVA06C_CTRL_INTERLEAVE_LEVEL_HIGH:
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
            {
                NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
                pKernelChannelGroup->pInterleaveLevel[subdevInst] = value;
            }
            SLI_LOOP_END

            NV_ASSERT_OK_OR_RETURN(kchangrpSetInterleaveLevelSched(pGpu,
                                              pKernelChannelGroup, value));
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

// Helper function to clean up all EngCtxDescs as well as the pointer storage
static void
_kchangrpFreeAllEngCtxDescs
(
    OBJGPU *pGpu,
    KernelChannelGroup *pKernelChannelGroup
)
{
    ENGINE_CTX_DESCRIPTOR *pEngCtxDescriptor;

    // Nothing to do
    if (pKernelChannelGroup->ppEngCtxDesc == NULL)
        return;

    // Destroy each of the EngCtxDescriptors
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    pEngCtxDescriptor = pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    if (pEngCtxDescriptor != NULL)
    {
        vaListDestroy(&pEngCtxDescriptor->vaList);
    }
    portMemFree(pEngCtxDescriptor);
    pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = NULL;

    SLI_LOOP_END

    // Destroy the array of pointers
    portMemFree(pKernelChannelGroup->ppEngCtxDesc);
    pKernelChannelGroup->ppEngCtxDesc = NULL;

    return;
}

NV_STATUS
kchangrpAllocEngineContextDescriptor_IMPL
(
    OBJGPU *pGpu,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NvU32 subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    NV_STATUS status;

    pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance] = portMemAllocNonPaged(sizeof(ENGINE_CTX_DESCRIPTOR));
    NV_ASSERT_OR_RETURN(pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance] != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance], 0, sizeof(ENGINE_CTX_DESCRIPTOR));

    NV_ASSERT_OK_OR_GOTO(status,
        vaListInit(&pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance]->vaList),
        failed);
    return NV_OK;

failed:
    portMemFree(pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance]);
    pKernelChannelGroup->ppEngCtxDesc[subdeviceInstance] = NULL;
    return status;
}

NV_STATUS
kchangrpGetEngineContextMemDesc_IMPL
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    ENGINE_CTX_DESCRIPTOR *pEngCtxDesc;
    NvU32                  subDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    pEngCtxDesc = pKernelChannelGroup->ppEngCtxDesc[subDevInst];

    if (NULL != pEngCtxDesc)
        *ppMemDesc = pEngCtxDesc->pMemDesc;
    else
        *ppMemDesc = NULL;

    return NV_OK;
}

