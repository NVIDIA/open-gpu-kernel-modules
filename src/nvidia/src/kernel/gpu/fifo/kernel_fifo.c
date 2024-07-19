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
#include "kernel/gpu/fifo/kernel_sched_mgr.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "rmapi/rs_utils.h"
#include "rmapi/client.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"

#include "kernel/core/locks.h"
#include "lib/base_utils.h"

#include "gpu/mmu/kern_gmmu.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "nvrm_registry.h"
#include "containers/eheap_old.h"

#include "nvmisc.h"

#include "class/cl0080.h"
#include "class/cl2080.h"
#include "class/cl208f.h"
#include "class/clc572.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

#define KFIFO_EHEAP_OWNER NvU32_BUILD('f','i','f','o')

//
// Reserve some channels to be used by GSP
// Currently used by CeUtils only
//
#define KFIFO_NUM_GSP_RESERVED_CHANNELS 1

static EHeapOwnershipComparator _kfifoUserdOwnerComparator;

static NV_STATUS _kfifoChidMgrAllocChidHeaps(OBJGPU     *pGpu,
                                             KernelFifo *pKernelFifo,
                                             CHID_MGR   *pChidMgr);

static NV_STATUS _kfifoChidMgrAllocVChidHeapPointers(OBJGPU *pGpu, CHID_MGR *pChidMgr);

static NV_STATUS _kfifoChidMgrInitChannelGroupMgr(OBJGPU *pGpu, CHID_MGR *pChidMgr);

static void _kfifoChidMgrDestroyChidHeaps(CHID_MGR *pChidMgr);

static void _kfifoChidMgrDestroyChannelGroupMgr(CHID_MGR *pChidMgr);

static NV_STATUS _kfifoChidMgrFreeIsolationId(CHID_MGR *pChidMgr, NvU32 ChID);

static NV_STATUS _kfifoChidMgrGetNextKernelChannel(OBJGPU *pGpu, KernelFifo *pKernelFifo,
                                                   CHID_MGR *pChidMgr, CHANNEL_ITERATOR *pIt,
                                                   KernelChannel **ppKernelChannel);


NvU32 kfifoGetNumEschedDrivenEngines_IMPL
(
    KernelFifo *pKernelFifo
)
{
    const ENGINE_INFO *pEngineInfo;
    NV_ASSERT(kfifoGetNumEngines_HAL(ENG_GET_GPU(pKernelFifo), pKernelFifo) >
              0);
    pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    return pEngineInfo->numRunlists;
}


NV_STATUS
kfifoChidMgrConstruct_IMPL
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;
    NvU32     numEngines;

    //
    // Allocate memory for the array of CHID_MGR pointers. Since this is an
    // array, we allocate memory for pointers unto maxNumRunlists. We will only
    // allocate objects for the valid ones.
    //
    if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        //
        // Construct the engine list if it isn't already constructed (internally
        // checks if it was already constructed)
        //
        NV_ASSERT_OK_OR_RETURN(kfifoConstructEngineList_HAL(pGpu, pKernelFifo));
        pKernelFifo->numChidMgrs = kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo);
    }
    else
        pKernelFifo->numChidMgrs = 1;

    if (pKernelFifo->numChidMgrs > MAX_NUM_RUNLISTS)
    {
        //
        // This only currently defines the size of our bitvector
        // pKernelFifo->chidMgrValid. Catch this case if HW expands beyond this so we
        // can increase the size allocated to the bitvector
        //
        NV_PRINTF(LEVEL_ERROR, "numChidMgrs 0x%x exceeds MAX_NUM_RUNLISTS\n",
            pKernelFifo->numChidMgrs);
        DBG_BREAKPOINT();
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    pKernelFifo->ppChidMgr = portMemAllocNonPaged(sizeof(CHID_MGR *) * pKernelFifo->numChidMgrs);
    if (pKernelFifo->ppChidMgr == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        pKernelFifo->ppChidMgr = NULL;
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate pFifo->pChidMgr\n");
        DBG_BREAKPOINT();
        return status;
    }
    portMemSet(pKernelFifo->ppChidMgr, 0, sizeof(CHID_MGR *) * pKernelFifo->numChidMgrs);

    // Initialize the valid mask
    if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        numEngines = kfifoGetNumEngines_HAL(pGpu, pKernelFifo);
        for (i = 0; i < numEngines; i++)
        {
            NvU32 runlistId;
            status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                              ENGINE_INFO_TYPE_INVALID, i,
                                              ENGINE_INFO_TYPE_RUNLIST, &runlistId);
            if (status == NV_OK)
                bitVectorSet(&pKernelFifo->chidMgrValid, runlistId);
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Translation to runlistId failed for engine %d\n", i);
                DBG_BREAKPOINT();
                goto fail;
            }
        }
    }
    else
    {
        bitVectorSet(&pKernelFifo->chidMgrValid, 0); // We only have 1 chidmgr
    }

    // Allocate memory for each CHID_MGR and its members (only the valid ones)
    for (i = 0; i < pKernelFifo->numChidMgrs; i++)
    {
        if (!bitVectorTest(&pKernelFifo->chidMgrValid, i))
            continue;

        pKernelFifo->ppChidMgr[i] = portMemAllocNonPaged(sizeof(CHID_MGR));
        if (pKernelFifo->ppChidMgr[i] == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate pFifo->pChidMgr[%d]\n", i);
            DBG_BREAKPOINT();
            goto fail;
        }
        portMemSet(pKernelFifo->ppChidMgr[i], 0, sizeof(CHID_MGR));

        pKernelFifo->ppChidMgr[i]->runlistId = i;

        pKernelFifo->ppChidMgr[i]->pChanGrpTree = portMemAllocNonPaged(sizeof(KernelChannelGroupMap));
        mapInitIntrusive(pKernelFifo->ppChidMgr[i]->pChanGrpTree);

        status = _kfifoChidMgrAllocChidHeaps(pGpu, pKernelFifo, pKernelFifo->ppChidMgr[i]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error allocating FifoDataHeap in "
                "pChidMgr. Status = %s (0x%x)\n",
                nvstatusToString(status), status);
            DBG_BREAKPOINT();
            goto fail;
        }

        status = _kfifoChidMgrInitChannelGroupMgr(pGpu, pKernelFifo->ppChidMgr[i]);
        if (status != NV_OK)
            goto fail;
    }


    return status;

fail:
    kfifoChidMgrDestruct(pKernelFifo);
    return status;
}

void
kfifoChidMgrDestruct_IMPL
(
    KernelFifo *pKernelFifo
)
{
    NvU32 i;

    for (i = 0; i < pKernelFifo->numChidMgrs; i++)
    {
        if (pKernelFifo->ppChidMgr[i] != NULL)
        {
            mapDestroy(pKernelFifo->ppChidMgr[i]->pChanGrpTree);
            portMemFree(pKernelFifo->ppChidMgr[i]->pChanGrpTree);
            _kfifoChidMgrDestroyChidHeaps(pKernelFifo->ppChidMgr[i]);
            _kfifoChidMgrDestroyChannelGroupMgr(pKernelFifo->ppChidMgr[i]);
            portMemFree(pKernelFifo->ppChidMgr[i]);
            pKernelFifo->ppChidMgr[i] = NULL;
        }
    }

    portMemFree(pKernelFifo->ppChidMgr);
    pKernelFifo->ppChidMgr = NULL;
    bitVectorClrAll(&pKernelFifo->chidMgrValid);
    pKernelFifo->numChidMgrs = 0;
}

/*
 * @brief Allocate and initialize the virtual ChId heap pointers
 */
static NV_STATUS
_kfifoChidMgrAllocVChidHeapPointers
(
    OBJGPU     *pGpu,
    CHID_MGR   *pChidMgr
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;

    if (IS_VIRTUAL(pGpu))
    {
        return NV_OK;
    }

    if (gpuIsSriovEnabled(pGpu))
    {
        //
        // For Virtual Channel Heap
        // Allocate Memory for Heap Object pointers
        //
        pChidMgr->ppVirtualChIDHeap = portMemAllocNonPaged(sizeof(OBJEHEAP *) * (VMMU_MAX_GFID));
        if (pChidMgr->ppVirtualChIDHeap == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Error allocating memory for virtual channel heap pointers\n");
            return NV_ERR_NO_MEMORY;
        }

        // initialize
        for (i = 0; i < VMMU_MAX_GFID; i++)
        {
            pChidMgr->ppVirtualChIDHeap[i] = NULL;
        }
    }
    return status;
}


/*
 * @brief Allocates & initializes ChID heaps
 */
static NV_STATUS
_kfifoChidMgrAllocChidHeaps
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo,
    CHID_MGR    *pChidMgr
)
{
    NV_STATUS status = NV_OK;

    if (pChidMgr->numChannels == 0)
    {
        if (kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr) == 0)
        {
            NV_PRINTF(LEVEL_ERROR, "pChidMgr->numChannels is 0\n");
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_STATE;
        }
    }

    pChidMgr->pFifoDataHeap = portMemAllocNonPaged(sizeof(*pChidMgr->pFifoDataHeap));
    if (pChidMgr->pFifoDataHeap == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "Error in Allocating memory for pFifoDataHeap! Status = %s (0x%x)\n",
                  nvstatusToString(status), status);
        return status;
    }
    constructObjEHeap(pChidMgr->pFifoDataHeap, 0, pChidMgr->numChannels,
                      sizeof(KernelChannel *), 0);

    if (kfifoIsChidHeapEnabled(pKernelFifo))
    {
        NvU32 userdBar1Size;
        NvU32 numChannels         = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
        NvU32 subProcessIsolation = 1;

        pChidMgr->pGlobalChIDHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
        if (pChidMgr->pGlobalChIDHeap == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Error in Allocating memory for global ChID heap!\n");
            return NV_ERR_NO_MEMORY;
        }
        constructObjEHeap(pChidMgr->pGlobalChIDHeap, 0, numChannels,
                          sizeof(PFIFO_ISOLATIONID), 0);

        //
        // Enable USERD allocation isolation. USERD allocated by different clients
        // should not be in the same page
        //
        kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdBar1Size, NULL);
        NvBool bIsolationEnabled = (pKernelFifo->bUsePerRunlistChram && pKernelFifo->bDisableChidIsolation) ? NV_FALSE : NV_TRUE;
        pChidMgr->pGlobalChIDHeap->eheapSetOwnerIsolation(pChidMgr->pGlobalChIDHeap,
                                                          bIsolationEnabled,
                                                          RM_PAGE_SIZE / userdBar1Size);

        // Disable USERD allocation isolation for guest if disabled from vmioplugin
        if (IS_VIRTUAL(pGpu))
        {
            VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
            if (pVSI != NULL)
            {
                subProcessIsolation = pVSI->subProcessIsolation;
            }
        }
        else
        {
            // In this case subProcessIsolation is always 0
            if (IS_GSP_CLIENT(pGpu))
            {
                subProcessIsolation = 0;
            }
        }
        if (!subProcessIsolation)
        {
            pChidMgr->pGlobalChIDHeap->eheapSetOwnerIsolation(
                                            pChidMgr->pGlobalChIDHeap,
                                            NV_FALSE,
                                            RM_PAGE_SIZE / userdBar1Size);
    #if (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !RMCFG_FEATURE_MODS_FEATURES
            NV_PRINTF(LEVEL_INFO,
                      "Sub Process channel isolation disabled by vGPU plugin\n");
    #endif
        }

        status = _kfifoChidMgrAllocVChidHeapPointers(pGpu, pChidMgr);
    }

    return status;
}

static void
_kfifoChidMgrDestroyChidHeaps
(
    CHID_MGR     *pChidMgr
)
{
    if (pChidMgr->pFifoDataHeap != NULL)
    {
        pChidMgr->pFifoDataHeap->eheapDestruct(pChidMgr->pFifoDataHeap);
        portMemFree(pChidMgr->pFifoDataHeap);
        pChidMgr->pFifoDataHeap = NULL;
    }
    if (pChidMgr->pGlobalChIDHeap != NULL)
    {
        pChidMgr->pGlobalChIDHeap->eheapDestruct(pChidMgr->pGlobalChIDHeap);
        portMemFree(pChidMgr->pGlobalChIDHeap);
        pChidMgr->pGlobalChIDHeap = NULL;
    }

    portMemFree(pChidMgr->ppVirtualChIDHeap);
    pChidMgr->ppVirtualChIDHeap = NULL;
}


static NV_STATUS
_kfifoChidMgrInitChannelGroupMgr
(
    OBJGPU     *pGpu,
    CHID_MGR   *pChidMgr
)
{
    KernelFifo *pKernelFifo      = GPU_GET_KERNEL_FIFO(pGpu);
    FIFO_HW_ID *pFifoHwID        = &pChidMgr->channelGrpMgr;
    NvU32       allocSize;
    NvU32       numChannelGroups = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);

    if (numChannelGroups == 0)
    {
        return NV_OK;
    }

    // Rounds up to dword alignemnt, then converts bits to bytes.
    allocSize = RM_ALIGN_UP(numChannelGroups, 32)/8;

    pFifoHwID->pHwIdInUse = portMemAllocNonPaged(allocSize);
    if (pFifoHwID->pHwIdInUse == NULL)
        return NV_ERR_NO_MEMORY;

    // bytes to NvU32[] elements
    pFifoHwID->hwIdInUseSz = allocSize/4;

    portMemSet(pFifoHwID->pHwIdInUse, 0, allocSize);

    //
    // If numChannelGroups isn't a multiple of 32 we need to set the bits > numChannelGroups to
    // 1.  Otherwise when we allocate IDs starting at the top we'll allocate
    // ids >numChannelGroups.
    //
    if (numChannelGroups % 32 != 0)
    {
        pFifoHwID->pHwIdInUse[numChannelGroups/32] |= ~ ((1<<(numChannelGroups%32))-1);
    }

    return NV_OK;
}

static void
_kfifoChidMgrDestroyChannelGroupMgr
(
    CHID_MGR *pChidMgr
)
{
    if (pChidMgr->channelGrpMgr.pHwIdInUse)
    {
        portMemFree(pChidMgr->channelGrpMgr.pHwIdInUse);
        pChidMgr->channelGrpMgr.pHwIdInUse = NULL;
        pChidMgr->channelGrpMgr.hwIdInUseSz = 0;
    }
}

static NV_STATUS
_kfifoChidMgrFreeIsolationId
(
    CHID_MGR   *pChidMgr,
    NvU32       ChID
)
{
    EMEMBLOCK  *pIsolationIdBlock = pChidMgr->pGlobalChIDHeap->eheapGetBlock(
        pChidMgr->pGlobalChIDHeap,
        ChID,
        NV_FALSE);

    NV_ASSERT_OR_RETURN(pIsolationIdBlock, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT(pIsolationIdBlock->refCount > 0);
    NV_ASSERT(pIsolationIdBlock->pData != NULL);
    portMemFree(pIsolationIdBlock->pData);

    pIsolationIdBlock->pData = NULL;

    return NV_OK;
}

/*!
 * @breif Fifo defined call back comparator to compare eheap block ownership ID
 *
 * @param[in]  pRequesterID  Ownership ID constructed by caller
 * @param[in]  pIsolationID
 *
 * @return NV_TRUE if two ownership IDs belong to the same owner
 */
static NvBool
_kfifoUserdOwnerComparator
(
    void *pRequesterID,
    void *pIsolationID
)
{
    PFIFO_ISOLATIONID pAllocID = (PFIFO_ISOLATIONID)pRequesterID;
    PFIFO_ISOLATIONID pBlockID = (PFIFO_ISOLATIONID)pIsolationID;

    //
    // The block's data will be NULL if the channel has been destroyed but there
    // is still a refcount on the channel ID. In that case no work can be issued
    // to that channel ID now or in the future, so we can act as though the
    // channel does not exist.
    //
    if (!pBlockID)
        return NV_TRUE;

    if ((pAllocID->domain       != pBlockID->domain)    ||
        (pAllocID->processID    != pBlockID->processID) ||
        (pAllocID->subProcessID != pBlockID->subProcessID))
    {
        return NV_FALSE;
    }
    else
    {
        return NV_TRUE;
    }
}

/*
 * @brief Returns the number of vGPU plugin channels.
 *
 * Depending on whether this code is executed on the CPU RM or the Physical RM,
 * different structures are used to retrieve the number.
 * On the CPU RM or the monolithic RM, KERNEL_HOST_VGPU_DEVICE::numPluginChannels is used,
 * whereas on the physical RM it's HOST_VGPU_DEVICE::numPluginChannels.
 */
static NV_STATUS
_kfifoGetVgpuPluginChannelsCount
(
    OBJGPU *pGpu,
    NvU32 *pNumPluginChannels
)
{
    NV_ASSERT_OR_RETURN(pNumPluginChannels != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;

        NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextKernelHostVgpuDevice(pGpu, &pKernelHostVgpuDevice));
        NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice != NULL, NV_ERR_OBJECT_NOT_FOUND);

        *pNumPluginChannels = pKernelHostVgpuDevice->numPluginChannels;
    }

    return NV_OK;
}

/*!
 * @brief Allocates one Channel ID on heap
 *
 * @param[in] OBJGPU     GPU Object
 * @param[in] KernelFifo KernelFifo Object
 * @param[in] CHID_MGR   Channel ID manager
 * @param[in] chFlag NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN
 *                   NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE
 *                   NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP
 * @param[in] bForceInternalIdx true if requesting specific index within USERD
 *                              page
 * @param[in] internalIdx requested index within USERD page when
 *                        bForceInternalIdx true
 * @param[in] ChID ChID to assign in case of ADDRESS_ALLOCATE
 * @param[in,out] pKernelChannel The previously allocated KernelChannel structure
 *
 * @return NV_OK if allocation is successful
 *         NV_ERR_NO_FREE_FIFOS: allocated channel ID exceeds MAX channels.
 */
NV_STATUS
kfifoChidMgrAllocChid_IMPL
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    CHID_MGR                *pChidMgr,
    NvHandle                 hClient,
    CHANNEL_HW_ID_ALLOC_MODE chIdFlag,
    NvBool                   bForceInternalIdx,
    NvU32                    internalIdx,
    NvBool                   bForceUserdPage,
    NvU32                    userdPageIdx,
    NvU32                    ChID,
    KernelChannel           *pKernelChannel
)
{
    NvU64             chSize;
    NvU32             chFlag                = chIdFlag;
    NvU64             ChID64                = 0;
    NvU64             subProcessID          = 0;
    NvU64             processID             = 0;
    NvBool            bIsSubProcessDisabled = NV_FALSE;
    RmClient         *pClient;
    NvU32             offsetAlign = 1;
    NvU32             gfid;
    PFIFO_ISOLATIONID pIsolationID = NULL;
    NV_STATUS         status;
    NvU32             numChannels;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);

    switch (chIdFlag)
    {
        case CHANNEL_HW_ID_ALLOC_MODE_GROW_DOWN:
            chFlag = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
            break;
        case CHANNEL_HW_ID_ALLOC_MODE_GROW_UP:
            chFlag = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP;
            break;
        case CHANNEL_HW_ID_ALLOC_MODE_PROVIDED:
            ChID64 = ChID;
            chFlag = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid channel ID alloc mode %d\n", chFlag);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
    }

    // we are allocating only one Channel at a time
    chSize = 1;

    // Create unique isolation ID for each process
    pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid client handle %ux\n", hClient);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_CLIENT;
    }

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // SRIOV: In guest plugin context allocate the chid.
    //        In guest RM context allocate the same chid as guest
    if (IS_GFID_VF(gfid))
    {
        NvU32             numPluginChannels;
        NvU64             rangeLo, rangeHi, base, size;

        NV_ASSERT_OR_RETURN(pChidMgr->ppVirtualChIDHeap[gfid],
                            NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(_kfifoGetVgpuPluginChannelsCount(pGpu, &numPluginChannels));

        pChidMgr->ppVirtualChIDHeap[gfid]->eheapGetBase(
            pChidMgr->ppVirtualChIDHeap[gfid],
            &base);
        pChidMgr->ppVirtualChIDHeap[gfid]->eheapGetSize(
            pChidMgr->ppVirtualChIDHeap[gfid],
            &size);

        rangeLo = base;
        rangeHi = base + size - 1;

        // Route plugin channels to be allocated at the top
        NV_ASSERT_OR_RETURN(numPluginChannels < size,
                            NV_ERR_INVALID_PARAMETER);
        if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bIsCallingContextVgpuPlugin &&
            numPluginChannels > 0)
        {
            rangeLo = rangeHi - numPluginChannels + 1;
        }
        else
        {
            rangeHi = rangeHi - numPluginChannels;
        }

        status = pChidMgr->ppVirtualChIDHeap[gfid]->eheapSetAllocRange(
                     pChidMgr->ppVirtualChIDHeap[gfid],
                     rangeLo,
                     rangeHi);

        NV_ASSERT_OK_OR_RETURN(status);

        if (bForceUserdPage)
        {
            NV_ASSERT_OR_RETURN(!bForceInternalIdx, NV_ERR_INVALID_STATE);
            ChID64 = ((NvU64)userdPageIdx) *
                         pChidMgr->ppVirtualChIDHeap[gfid]->ownerGranularity +
                     internalIdx;
            chFlag |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        }
        else if (bForceInternalIdx)
        {
            chFlag |= NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX;
            offsetAlign = internalIdx;
        }

        if (chFlag & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
            NV_ASSERT_OR_RETURN((ChID64 <= rangeHi) && (ChID64 >= rangeLo),
                                NV_ERR_INVALID_PARAMETER);

        // We'll allocate from the VirtualChIdHeap for the guest
        status = pChidMgr->ppVirtualChIDHeap[gfid]->eheapAlloc(
            pChidMgr->ppVirtualChIDHeap[gfid], // This Heap
            KFIFO_EHEAP_OWNER,                  // owner
            &chFlag,                           // Alloc Flags
            &ChID64,                           // Alloc Offset
            &chSize,                           // Size
            offsetAlign,                       // offsetAlign
            1,                                 // sizeAlign
            NULL,                              // Allocated mem block
            NULL,                              // Isolation ID
            NULL // Fifo defined ownership comparator
        );
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to allocate Channel ID 0x%llx %d on heap \n",
                      ChID64,
                      chIdFlag);
            DBG_BREAKPOINT();
            goto fail;
        }
    }
    else
    {
        NvU64 rangeLo, rangeHi, base, size;

        //
        // Legacy / SRIOV vGPU Host, SRIOV guest, baremetal CPU RM, GSP FW, GSP
        // client allocate from global heap
        //
        pIsolationID = portMemAllocNonPaged(sizeof(FIFO_ISOLATIONID));
        NV_ASSERT_OR_RETURN((pIsolationID != NULL), NV_ERR_NO_MEMORY);
        portMemSet(pIsolationID, 0, sizeof(FIFO_ISOLATIONID));

        //
        // Check if the allocation request is from the guest RM or host RM
        //
        processID             = pClient->ProcID;
        subProcessID          = pClient->SubProcessID;
        bIsSubProcessDisabled = pClient->bIsSubProcessDisabled;

        if (RMCFG_FEATURE_PLATFORM_GSP || kchannelCheckIsKernel(pKernelChannel))
        {
            //
            // If not GSPFW: Allocation request is from host RM kernel
            // If GSPFW: ChID has already been chosen by CPU-RM, but pClient
            //   doesn't have the true processID, so just allow the whole pool.
            //
            pIsolationID->domain = HOST_KERNEL;
            processID            = KERNEL_PID;
        }
        else
        {
            if (0x0 != subProcessID)
            {
                //
                // Allocation request is from the guest RM
                //
                if (KERNEL_PID == subProcessID)
                {
                    pIsolationID->domain = GUEST_KERNEL;
                }
                else
                {
                    pIsolationID->domain = GUEST_USER;
                }
            }
            else
            {
                pIsolationID->domain = HOST_USER;
            }
        }

        pIsolationID->processID    = processID;
        pIsolationID->subProcessID = subProcessID;

        //
        // Overwrite isolation ID if guest USERD isolation is disabled
        //
        if ((subProcessID != 0x0) && (bIsSubProcessDisabled))
        {
            pIsolationID->domain       = GUEST_INSECURE;
            pIsolationID->subProcessID = KERNEL_PID;
        }

        /* Channel USERD manipuliation only supported without GFID */
        if (bForceUserdPage)
        {
            NV_ASSERT_OR_RETURN(!bForceInternalIdx, NV_ERR_INVALID_STATE);
            ChID64 = ((NvU64)userdPageIdx) *
                         pChidMgr->pGlobalChIDHeap->ownerGranularity +
                     internalIdx;
            chFlag |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        }
        else if (bForceInternalIdx)
        {
            chFlag |= NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX;
            offsetAlign = internalIdx;
        }

        pChidMgr->pGlobalChIDHeap->eheapGetBase(pChidMgr->pGlobalChIDHeap, &base);
        pChidMgr->pGlobalChIDHeap->eheapGetSize(pChidMgr->pGlobalChIDHeap, &size);

        rangeLo = base;
        rangeHi = base + size - 1;
        if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            // Reserve channels for GSP unless it's VGPU host
            if (pKernelChannel->bGspOwned)
            {
                rangeHi = rangeLo + KFIFO_NUM_GSP_RESERVED_CHANNELS - 1;
            }
            else if (RMCFG_FEATURE_PLATFORM_GSP || IS_GSP_CLIENT(pGpu))
            {
                rangeLo += KFIFO_NUM_GSP_RESERVED_CHANNELS;
            }
        }

        NV_ASSERT_OK_OR_RETURN(
            pChidMgr->pGlobalChIDHeap->eheapSetAllocRange(pChidMgr->pGlobalChIDHeap, rangeLo, rangeHi));

        status = pChidMgr->pGlobalChIDHeap->eheapAlloc(
            pChidMgr->pGlobalChIDHeap, // This Heap
            KFIFO_EHEAP_OWNER,          // owner
            &chFlag,                   // Alloc Flags
            &ChID64,                   // Alloc Offset
            &chSize,                   // Size
            offsetAlign,               // offsetAlign
            1,                         // sizeAlign
            NULL,                      // Allocated mem block
            pIsolationID,              // Isolation ID
            _kfifoUserdOwnerComparator  // Fifo defined ownership comparator
        );

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate Channel ID on heap\n");
            DBG_BREAKPOINT();
            goto fail;
        }
    }

    //
    // Now allocate at a fixed offset from the pFifoDataHeap once the previous
    // ID allocation told us which ID to use.
    //
    chFlag = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    status = pChidMgr->pFifoDataHeap->eheapAlloc(
        pChidMgr->pFifoDataHeap, // This Heap
        KFIFO_EHEAP_OWNER,        // owner
        &chFlag,                 // Alloc Flags
        &ChID64,                 // Alloc Offset
        &chSize,                 // Size
        1,                       // offsetAlign
        1,                       // sizeAlign
        NULL,                    // Allocated mem block
        NULL,                    // Isolation ID
        NULL                     // ownership comparator
    );

    if (status != NV_OK)
    {
        //
        // Should never happen since we're mirroring the global chid heap, or
        // pre-reserving space on the global chid heap for SR-IOV capable
        // systems.
        //
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate Channel on fifo data heap\n");
        goto fail;
    }

    ChID = NvU64_LO32(ChID64);

    if (ChID < numChannels)
    {
        EMEMBLOCK *pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
            pChidMgr->pFifoDataHeap,
            ChID,
            NV_FALSE);
        EMEMBLOCK *pIsolationIdBlock = pChidMgr->pGlobalChIDHeap->eheapGetBlock(
            pChidMgr->pGlobalChIDHeap,
            ChID,
            NV_FALSE);

        if (IS_GFID_PF(gfid))
            pIsolationIdBlock->pData = pIsolationID;

        pFifoDataBlock->pData = pKernelChannel;
        pKernelChannel->ChID  = ChID;
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING, "No allocatable FIFO available.\n");
        status = NV_ERR_NO_FREE_FIFOS;
        goto fail;
    }
    return NV_OK;

fail:
    // We already know that pIsolationID is non-NULL here.
    portMemFree(pIsolationID);
    return status;
}

/*
 * Retain a channel ID which has already been allocated by
 * kfifoChidMgrAllocChid. Until released, the HW channel ID will not be
 * allocated by any new channels even after kfifoChidMgrFreeChid has been
 * called.
 */
NV_STATUS
kfifoChidMgrRetainChid_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32       ChID
)
{
    NvU32       gfid;
    EMEMBLOCK  *pFifoDataBlock = NULL;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (IS_GFID_VF(gfid))
    {
        NV_ASSERT_OR_RETURN(pChidMgr->ppVirtualChIDHeap[gfid] != NULL,
                            NV_ERR_INVALID_STATE);
        EMEMBLOCK  *pVirtChIdBlock = pChidMgr->ppVirtualChIDHeap[gfid]->eheapGetBlock(
            pChidMgr->ppVirtualChIDHeap[gfid],
            ChID,
            NV_FALSE);
        NV_ASSERT_OR_RETURN(pVirtChIdBlock != NULL, NV_ERR_OBJECT_NOT_FOUND);
        NV_ASSERT(pVirtChIdBlock->refCount > 0);
        ++pVirtChIdBlock->refCount;
    }
    else
    {
        NV_ASSERT_OR_RETURN(pChidMgr->pGlobalChIDHeap != NULL, NV_ERR_INVALID_STATE);
        EMEMBLOCK  *pChIdBlock = pChidMgr->pGlobalChIDHeap->eheapGetBlock(
            pChidMgr->pGlobalChIDHeap,
            ChID,
            NV_FALSE);
        NV_ASSERT_OR_RETURN(pChIdBlock != NULL, NV_ERR_OBJECT_NOT_FOUND);
        NV_ASSERT(pChIdBlock->refCount > 0);
        ++pChIdBlock->refCount;
    }

    NV_ASSERT_OR_RETURN(pChidMgr->pFifoDataHeap != NULL, NV_ERR_INVALID_STATE);
    pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
        pChidMgr->pFifoDataHeap,
        ChID,
        NV_FALSE);
    NV_ASSERT_OR_RETURN(pFifoDataBlock != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT(pFifoDataBlock->refCount > 0);
    ++pFifoDataBlock->refCount;

    return NV_OK;
}

/*
 * Drop the refcount on the given channel (ID), removing it from pFifo's heap if
 * its refcount reaches 0.
 */
NV_STATUS
kfifoChidMgrReleaseChid_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32       ChID
)
{
    NvU32 gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (IS_GFID_VF(gfid))
    {
        NV_ASSERT_OR_RETURN(pChidMgr->ppVirtualChIDHeap[gfid] != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OK(pChidMgr->ppVirtualChIDHeap[gfid]->eheapFree(pChidMgr->ppVirtualChIDHeap[gfid], ChID));
    }
    else
    {
        NV_ASSERT_OR_RETURN(pChidMgr->pGlobalChIDHeap != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OK(pChidMgr->pGlobalChIDHeap->eheapFree(pChidMgr->pGlobalChIDHeap, ChID));
    }

    NV_ASSERT_OR_RETURN(pChidMgr->pFifoDataHeap != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OK_OR_RETURN(pChidMgr->pFifoDataHeap->eheapFree(pChidMgr->pFifoDataHeap, ChID));

    return NV_OK;
}

/*
 * Removes the association between pKernelChannel and its channel ID. Note that this
 * will not remove the channel ID itself from pFifo's heap if
 * fifoHeapRetainChannelId has been called.
 */
NV_STATUS
kfifoChidMgrFreeChid_IMPL
(
    OBJGPU       *pGpu,
    KernelFifo   *pKernelFifo,
    CHID_MGR     *pChidMgr,
    NvU32         ChID
)
{
    EMEMBLOCK *pFifoDataBlock;
    NV_STATUS  status;
    NvU32 gfid;

    //
    // This channel is going away, so clear its pointer from the channel ID's heap
    // block.
    //
    pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
        pChidMgr->pFifoDataHeap,
        ChID,
        NV_FALSE);
    NV_ASSERT_OR_RETURN(pFifoDataBlock != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT(pFifoDataBlock->refCount > 0);
    pFifoDataBlock->pData = NULL;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (IS_GFID_PF(gfid))
    {
        //
        // This marks the channel ID as orphaned and causes it to be ignored for
        // isolation purposes. This only matters if there will still be a reference
        // on the ID after we release ours below.
        //
        status = _kfifoChidMgrFreeIsolationId(pChidMgr, ChID);
        if(status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                "Failed to free IsolationId. Status = 0x%x\n", status);
            DBG_BREAKPOINT();
            return status;
        }
    }

    return kfifoChidMgrReleaseChid(pGpu, pKernelFifo, pChidMgr, ChID);
}

/**
 * @brief Reserve a contiguous set of SCHIDs from the end of our CHID heap for
 * the given GFID
 *

 * @param[in] pChidMgr         CHID_MGR pointer
 * @param[in] numChannels      Number of SCHIDs to reserve
 * @param[in] pHostVgpuDevice  HOST_VGPU_DEVICE
 *
 * @return NV_OK if success
 */
NV_STATUS
kfifoChidMgrReserveSystemChids_IMPL
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    CHID_MGR         *pChidMgr,
    NvU32             numChannels,
    NvU32             flags,
    NvU32             gfid,
    NvU32            *pChidOffset,
    NvU64             offset,
    NvU32            *pChannelCount,
    Device           *pMigDevice,
    NvU32             engineFifoListNumEntries,
    FIFO_ENGINE_LIST *pEngineFifoList
)
{
    NV_STATUS         status              = NV_OK;
    NvU64             chSize;
    PFIFO_ISOLATIONID pIsolationID        = NULL;
    EMEMBLOCK        *pIsolationIdBlock;
    NvU32             userdBar1Size;

    if (IS_VIRTUAL(pGpu))
    {
        // Not supported on guest or when SRIOV is disabled
        return NV_ERR_NOT_SUPPORTED;
    }

    pIsolationID = portMemAllocNonPaged(sizeof(FIFO_ISOLATIONID));
    NV_ASSERT_OR_RETURN((pIsolationID != NULL), NV_ERR_NO_MEMORY);
    portMemSet(pIsolationID, 0, sizeof(FIFO_ISOLATIONID));

    chSize = numChannels;

    status = pChidMgr->pGlobalChIDHeap->eheapAlloc(
        pChidMgr->pGlobalChIDHeap,       // This Heap
        KFIFO_EHEAP_OWNER,               // owner
        &flags,                          // Alloc Flags
        &offset,                         // Alloc Offset
        &chSize,                         // Size
        1,                               // offsetAlign
        1,                               // sizeAlign
        NULL,                            // Allocated mem block
        pIsolationID,                    // IsolationID
        _kfifoUserdOwnerComparator       // Fifo defined ownership comparator
    );

    if(status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to reserve channel IDs. Status = 0x%x\n", status);
        DBG_BREAKPOINT();

        //
        // Free the allocated memory and return early. After this, all failure
        // points can goto the common cleanup label
        //
        portMemFree(pIsolationID);
        return status;
    }

    pIsolationIdBlock = pChidMgr->pGlobalChIDHeap->eheapGetBlock(
        pChidMgr->pGlobalChIDHeap,
        offset,
        NV_FALSE);
    if (pIsolationIdBlock == NULL)
    {
        // Something bad happened. This should not fail if allocation succeeded
        NV_PRINTF(LEVEL_ERROR, "Could not fetch block from eheap\n");
        DBG_BREAKPOINT();
        goto cleanup;
    }
    pIsolationIdBlock->pData = pIsolationID;

    status = kfifoSetChidOffset(pGpu, pKernelFifo, pChidMgr, (NvU32)offset,
                                numChannels, gfid, pChidOffset, pChannelCount,
                                pMigDevice, engineFifoListNumEntries, pEngineFifoList);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to program the CHID table\n");
        goto cleanup;
    }

    pChidMgr->ppVirtualChIDHeap[gfid] = portMemAllocNonPaged(sizeof(OBJEHEAP));
    if (pChidMgr->ppVirtualChIDHeap[gfid] == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "Error allocating memory for virtual "
            "channel ID heap\n");
        goto cleanup;
    }
    portMemSet(pChidMgr->ppVirtualChIDHeap[gfid], 0, sizeof(OBJEHEAP));

    //
    // Construct heap using low as offset and size of numChannels. This heap
    // will be used for guest channel ID allocations, but will be in the
    // system channel ID space, hence it only manages IDs from offset to
    // (offset + numChannels).
    //
    constructObjEHeap(pChidMgr->ppVirtualChIDHeap[gfid], offset,
                      (offset + numChannels), 0, 0);

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdBar1Size, NULL);
    pChidMgr->ppVirtualChIDHeap[gfid]->eheapSetOwnerIsolation(
        pChidMgr->ppVirtualChIDHeap[gfid],
        NV_FALSE,
        RM_PAGE_SIZE/userdBar1Size);

    return status;

cleanup:
    portMemFree(pChidMgr->ppVirtualChIDHeap[gfid]);
    NV_ASSERT(kfifoSetChidOffset(pGpu, pKernelFifo, pChidMgr, 0, 0,
                                 gfid, pChidOffset, pChannelCount, pMigDevice,
                                 engineFifoListNumEntries, pEngineFifoList) == NV_OK);
    NV_ASSERT(pChidMgr->pGlobalChIDHeap->eheapFree(pChidMgr->pGlobalChIDHeap, offset) == NV_OK);
    portMemFree(pIsolationID);
    return status;
}

/*! Frees a block of contiguous SCHIDs previously reserved for the given GFID */
NV_STATUS
kfifoChidMgrFreeSystemChids_IMPL
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    CHID_MGR         *pChidMgr,
    NvU32             gfid,
    NvU32            *pChidOffset,
    NvU32            *pChannelCount,
    Device           *pMigDevice,
    NvU32             engineFifoListNumEntries,
    FIFO_ENGINE_LIST *pEngineFifoList
)
{
    NV_STATUS status, tmpStatus;
    NvU64     chId;

    if (IS_VIRTUAL(pGpu))
    {
        // Not supported on guest or when SRIOV is disabled
        return NV_ERR_NOT_SUPPORTED;
    }

    // Get the schid base
    pChidMgr->ppVirtualChIDHeap[gfid]->eheapGetBase(
        pChidMgr->ppVirtualChIDHeap[gfid],
        &chId);

    status = _kfifoChidMgrFreeIsolationId(pChidMgr, (NvU32)chId);
    if(status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to free IsolationId. Status = 0x%x\n",
            status);
        DBG_BREAKPOINT();
        return status;
    }

    status = pChidMgr->pGlobalChIDHeap->eheapFree(pChidMgr->pGlobalChIDHeap, chId);
    if(status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to free channel IDs. Status = 0x%x\n",
            status);
        DBG_BREAKPOINT();

        //
        // March on anyway to program the ChId table. We'll return an error
        // if we get here though.
        //
    }

    tmpStatus = kfifoSetChidOffset(pGpu, pKernelFifo, pChidMgr, 0, 0,
                                   gfid, pChidOffset, pChannelCount, pMigDevice,
                                   engineFifoListNumEntries, pEngineFifoList);
    if (tmpStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to program the CHID table\n");
        DBG_BREAKPOINT();
        return tmpStatus;
    }

    pChidMgr->ppVirtualChIDHeap[gfid]->eheapDestruct(pChidMgr->ppVirtualChIDHeap[gfid]);
    portMemFree(pChidMgr->ppVirtualChIDHeap[gfid]);
    pChidMgr->ppVirtualChIDHeap[gfid] = NULL;

    return status;
}

NvU32
kfifoChidMgrGetNumChannels_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR *pChidMgr
)
{
    // Cache ChidMgr's numChannels if not set
    if (pChidMgr->numChannels == 0)
    {
        NvU32 numChannels = kfifoRunlistQueryNumChannels_HAL(pGpu, pKernelFifo,
                                                             pChidMgr->runlistId);

        if (pKernelFifo->bNumChannelsOverride)
        {
            pChidMgr->numChannels = NV_MIN(pKernelFifo->numChannelsOverride, numChannels);
        }
        else
        {
            pChidMgr->numChannels = numChannels;
        }

        // Once we have set calculated value disable any overrides.
        pKernelFifo->bNumChannelsOverride = 0;
    }

    return pChidMgr->numChannels;
}

NvU32
kfifoRunlistQueryNumChannels_KERNEL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    NvU32 runlistId
)
{
    NvU32 numChannels = 0;
    NvU32 status;

    // For vgpu, read numChannels from VGPU_STATIC_INFO
    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI;
        pVSI = GPU_GET_STATIC_INFO(pGpu);

        if (pVSI)
        {
            numChannels = pVSI->vgpuStaticProperties.channelCount;
        }
        else
        {
            DBG_BREAKPOINT();
            return 0;
        }
    }

    // Do internal control call and set numChannels
    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API  *pRmApi   = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS numChannelsParams = {0};

        numChannelsParams.runlistId = runlistId;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_CHANNELS,
                                 &numChannelsParams,
                                 sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS));
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            return 0;
        }

        numChannels = numChannelsParams.numChannels;
    }

    NV_ASSERT(numChannels > 0);

    return numChannels;
}

/**
 * @brief reserves a hardware channel slot for a channel group
 *
 * Only responsible for indicating a hardware channel is in use, does not set
 * any other software state.
 *
 * This function is not called in broadcast mode
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pChidMgr
 * @param[out] grpID
 */
NV_STATUS
kfifoChidMgrAllocChannelGroupHwID_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32      *pChGrpID
)
{
    NvU32 maxChannelGroups;

    if (pChGrpID == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    maxChannelGroups = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    if (maxChannelGroups == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Zero max channel groups!!!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Find the least unused grpID
    *pChGrpID = nvBitFieldLSZero(pChidMgr->channelGrpMgr.pHwIdInUse,
                                 pChidMgr->channelGrpMgr.hwIdInUseSz);

    if (*pChGrpID < maxChannelGroups)
    {
        nvBitFieldSet(pChidMgr->channelGrpMgr.pHwIdInUse,
                      pChidMgr->channelGrpMgr.hwIdInUseSz, *pChGrpID, NV_TRUE);
    }
    else
    {
        *pChGrpID = maxChannelGroups;
        NV_PRINTF(LEVEL_ERROR, "No allocatable FIFO available.\n");
        return NV_ERR_NO_FREE_FIFOS;
    }
    return NV_OK;
}


/**
 * @brief Releases a hardware channel group ID.
 *
 * Not responsible for freeing any software state beyond that which indicates a
 * hardware channel is in use.
 *
 * This function is not called in broadcast mode
 *
 * @param pGpu
 * @param pFifo
 * @param pChidMgr
 * @param chGrpID
 */
NV_STATUS
kfifoChidMgrFreeChannelGroupHwID_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32       chGrpID
)
{
    NvU32 maxChannelGroups;

    maxChannelGroups = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    if (maxChannelGroups == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Zero max channel groups!!!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }
    NV_ASSERT_OR_RETURN(chGrpID < maxChannelGroups, NV_ERR_INVALID_ARGUMENT);

    //
    // Look for the channel group, check to make sure it's InUse bit is set
    // and then clear it to indicate the grpID is no longer in use
    //
    NV_ASSERT(nvBitFieldTest(pChidMgr->channelGrpMgr.pHwIdInUse,
                             pChidMgr->channelGrpMgr.hwIdInUseSz, chGrpID));
    nvBitFieldSet(pChidMgr->channelGrpMgr.pHwIdInUse,
                  pChidMgr->channelGrpMgr.hwIdInUseSz, chGrpID, NV_FALSE);

    return NV_OK;
}

CHID_MGR *
kfifoGetChidMgr_IMPL
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo,
    NvU32        runlistId
)
{
    if (!kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        // We only have 1 chidmgr when we don't have a per-runlist channel RAM
        if ((pKernelFifo->numChidMgrs != 1) ||
            (pKernelFifo->ppChidMgr == NULL) ||
            !bitVectorTest(&pKernelFifo->chidMgrValid, 0))
        {
            return NULL;
        }
        return pKernelFifo->ppChidMgr[0];
    }
    else
    {
        if (runlistId >= pKernelFifo->numChidMgrs)
        {
            return NULL;
        }
        //
        // It is valid to return a NULL value as long as runlistId is less than
        // maxNumRunlists since it is possible that not everything in the range
        // [0, numChidMgrs) represents a valid runlistId. The onus is on the
        // caller to check for NULL and only then use the CHIDMGR pointer
        //
        return pKernelFifo->ppChidMgr[runlistId];
    }
}

/*! Gets associated CHIDMGR object for given FIFO_ENGINE_INFO_TYPE and value */
NV_STATUS
kfifoGetChidMgrFromType_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineInfoType,
    NvU32       val,
    CHID_MGR  **ppChidMgr
)
{
    NV_STATUS status = NV_OK;
    NvU32     runlistId;

    NV_CHECK_OR_RETURN(LEVEL_INFO, ppChidMgr != NULL, NV_ERR_INVALID_ARGUMENT);

    // Initialize the pointer to NULL, in case we fail and return early
    *ppChidMgr = NULL;

    status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                      engineInfoType, val,
                                      ENGINE_INFO_TYPE_RUNLIST, &runlistId);
    NV_CHECK_OR_RETURN(LEVEL_INFO, NV_OK == status, status);

    *ppChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

    return NV_OK;
}

/*!
 * @brief Fetch pKernelChannel based on chidmgr and chid.
 *
 * This look-up uses the chid heap. It should find the first allocation of the channel,
 * which is useful if the handle is duped to another client.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pChidMgr      the ChIDMgr (per-runlist)
 * @param[in] ChID          the ChID
 *
 * @return the KernelChannel * or NULL
 */
KernelChannel *
kfifoChidMgrGetKernelChannel_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32       ChID
)
{
    EMEMBLOCK  *pFifoDataBlock;
    NvU32       numChannels;

    NV_ASSERT_OR_RETURN(pChidMgr != NULL, NULL);
    // Lite mode channels don't have KernelChannel yet
    NV_ASSERT_OR_RETURN(!kfifoIsLiteModeEnabled_HAL(pGpu, pKernelFifo), NULL);

    numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    if (ChID >= numChannels)
    {
        return NULL;
    }

    pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
        pChidMgr->pFifoDataHeap,
        ChID,
        NV_FALSE);
    if (pFifoDataBlock != NULL)
    {
        return (KernelChannel *)pFifoDataBlock->pData;
    }

    return NULL;
}

/*! Gets channel group data corresponding to grpID */
KernelChannelGroup *
kfifoChidMgrGetKernelChannelGroup_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    CHID_MGR   *pChidMgr,
    NvU32       grpID
)
{
    KernelChannelGroup *pKernelChannelGroup = NULL;

    pKernelChannelGroup = mapFind(pChidMgr->pChanGrpTree, grpID);
    if (pKernelChannelGroup == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "Can't find channel group %d\n", grpID);
    }

    return pKernelChannelGroup;
}

/*!
 * @brief Gets channel group data corresponding to grpID
 *
 * This function is not called in broadcast mode
 *
 * @param     pGpu
 * @param     pFifo
 * @param[in] grpID
 * @param[in] runlistID pass CHIDMGR_RUNLIST_ID_LEGACY if not known
 *
 * @returns KernelChannelGroup * on success
 *        NULL      if channel group is not found
 */
KernelChannelGroup *
kfifoGetChannelGroup_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       grpID,
    NvU32       runlistID
)
{
    CHID_MGR *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistID);

    return kfifoChidMgrGetKernelChannelGroup(pGpu, pKernelFifo, pChidMgr, grpID);
}

/*! Gets total number of channel groups in use */
NvU32
kfifoGetChannelGroupsInUse_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32    numChannelGroups      = 0;
    NvU32    numChannelGroupsInUse = 0;
    NvU32    chGrpID, i;

    for (i = 0; i < pKernelFifo->numChidMgrs; i++)
    {
        if (pKernelFifo->ppChidMgr[i] != NULL)
        {
            numChannelGroups = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo,
                                                      pKernelFifo->ppChidMgr[i]);

            for (chGrpID = 0; chGrpID < numChannelGroups; chGrpID++)
            {
                if (nvBitFieldTest(pKernelFifo->ppChidMgr[i]->channelGrpMgr.pHwIdInUse,
                                   pKernelFifo->ppChidMgr[i]->channelGrpMgr.hwIdInUseSz,
                                   chGrpID))
                {
                    numChannelGroupsInUse++;
                }
            }
        }
    }
    return numChannelGroupsInUse;
}

/*! Gets total number of channel groups in use per engine */
NvU32
kfifoGetRunlistChannelGroupsInUse_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       runlistId
)
{
    NvU32      numChannelGroups      = 0;
    NvU32      numChannelGroupsInUse = 0;
    NvU32      chGrpID;
    CHID_MGR  *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

    numChannelGroups = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    for (chGrpID = 0; chGrpID < numChannelGroups; chGrpID++)
    {
        if (nvBitFieldTest(pChidMgr->channelGrpMgr.pHwIdInUse,
                           pChidMgr->channelGrpMgr.hwIdInUseSz,
                           chGrpID))
        {
            numChannelGroupsInUse++;
        }
    }
    return numChannelGroupsInUse;
}

/**
 * @brief Sets the timeslice for the specified channel group.
 *
 * @returns NV_OK if success, appropriate error otherwise
 */
NV_STATUS
kfifoChannelGroupSetTimeslice_IMPL
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    KernelChannelGroup *pKernelChannelGroup,
    NvU64               timesliceUs,
    NvBool              bSkipSubmit
)
{
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Setting TSG %d Timeslice to %lldus\n",
              pKernelChannelGroup->grpID, timesliceUs);

    if (timesliceUs < kfifoRunlistGetMinTimeSlice_HAL(pKernelFifo))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Setting Timeslice to %lldus not allowed. Min value is %lldus\n",
                  timesliceUs, kfifoRunlistGetMinTimeSlice_HAL(pKernelFifo));
        return NV_ERR_NOT_SUPPORTED;
    }

    pKernelChannelGroup->timesliceUs = timesliceUs;

    NV_ASSERT_OK_OR_RETURN(kfifoChannelGroupSetTimesliceSched(pGpu,
                                                              pKernelFifo,
                                                              pKernelChannelGroup,
                                                              timesliceUs,
                                                              bSkipSubmit));

    return status;
}

void
kfifoFillMemInfo_IMPL
(
    KernelFifo                *pKernelFifo,
    MEMORY_DESCRIPTOR         *pMemDesc,
    NV2080_CTRL_FIFO_MEM_INFO *pMemory
)
{
    if (pMemDesc == NULL)
    {
        pMemory->aperture = NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_INVALID;
        NV_PRINTF(LEVEL_ERROR, "kfifoFillMemInfo: pMemDesc = NULL\n");
    }
    else
    {
        if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
        {
            pMemory->aperture = NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_VIDMEM;
        }
        else if (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM)
        {
            if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
            {
                pMemory->aperture = NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_COH;
            }
            else if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
            {
                pMemory->aperture = NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_NCOH;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "kfifoFillMemInfo: Unknown cache attribute for sysmem aperture\n");
                NV_ASSERT(NV_FALSE);
            }
        }
        pMemory->base = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
        pMemory->size = pMemDesc->Size;
    }
}

/*
 * Initializes an iterator to iterate through all channels of a runlist
 * If runlistId is INVALID_RUNLIST_ID then it iterates channels for all runlists
 */
void
kfifoGetChannelIterator_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    CHANNEL_ITERATOR *pIt,
    NvU32 runlistId
)
{
    portMemSet(pIt, 0, sizeof(*pIt));
    pIt->physicalChannelID = 0;
    pIt->pFifoDataBlock    = NULL;
    pIt->runlistId         = 0;
    pIt->numRunlists       = 1;

    // Do we want to ierate all runlist channels
    if (runlistId == INVALID_RUNLIST_ID)
    {
        if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
        {
            pIt->numRunlists = kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo);
        }
    }
    else
    {
        pIt->runlistId = runlistId;
    }
}

// return next channel for a specific chidMgr
static NV_STATUS
_kfifoChidMgrGetNextKernelChannel
(
    OBJGPU              *pGpu,
    KernelFifo          *pKernelFifo,
    CHID_MGR            *pChidMgr,
    CHANNEL_ITERATOR    *pIt,
    KernelChannel      **ppKernelChannel
)
{
    KernelChannel *pKernelChannel = NULL;
    pIt->numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);

    if (pIt->pFifoDataBlock == NULL)
    {
        pIt->pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
            pChidMgr->pFifoDataHeap,
            pIt->physicalChannelID,
            NV_TRUE);
    }

    while (pIt->physicalChannelID < pIt->numChannels)
    {
        if (pIt->pFifoDataBlock->owner == NVOS32_BLOCK_TYPE_FREE)
        {
            pIt->physicalChannelID = pIt->pFifoDataBlock->end + 1;
        }
        else
        {
            pIt->physicalChannelID++;
            pKernelChannel = (KernelChannel *)pIt->pFifoDataBlock->pData;

            //
            // This iterator can be used during an interrupt, when a KernelChannel may
            // be in the process of being destroyed. If a KernelChannel expects a pChannel
            // but does not have one, it means it's being destroyed and we don't want to
            // return it.
            //
            if (pKernelChannel != NULL && kchannelIsValid_HAL(pKernelChannel))
            {
                // Prepare iterator to check next block in pChidMgr->pFifoDataHeap
                pIt->pFifoDataBlock = pIt->pFifoDataBlock->next;
               *ppKernelChannel = pKernelChannel;
                return NV_OK;
            }
        }

        // Check next block in pChidMgr->pFifoDataHeap
        pIt->pFifoDataBlock = pIt->pFifoDataBlock->next;
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

/**
 * @brief Returns the next KernelChannel from the iterator.
 *
 * Iterates over runlist IDs and ChIDs and returns the next KernelChannel found
 * on the heap, if any.
 *
 * (error guaranteed if pointer is NULL; non-NULL pointer guaranteed if NV_OK)
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[in]  pIt                   the channel iterator
 * @param[out] ppKernelChannel      returns a KernelChannel *
 *
 * @return NV_OK if the returned pointer is valid or error
 */
NV_STATUS kfifoGetNextKernelChannel_IMPL
(
    OBJGPU              *pGpu,
    KernelFifo          *pKernelFifo,
    CHANNEL_ITERATOR    *pIt,
    KernelChannel      **ppKernelChannel
)
{
    if (ppKernelChannel == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *ppKernelChannel = NULL;

    if (pIt->numRunlists == 1)
    {
        CHID_MGR *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, pIt->runlistId);
        NV_ASSERT_OR_RETURN(pChidMgr != NULL, NV_ERR_INVALID_ARGUMENT);
        return _kfifoChidMgrGetNextKernelChannel(pGpu, pKernelFifo,
                                                 pChidMgr, pIt, ppKernelChannel);
    }
    while (pIt->runlistId < pIt->numRunlists)
    {
        CHID_MGR *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, pIt->runlistId);

        if (pChidMgr == NULL)
        {
            pIt->runlistId++;
            continue;
        }

        if (_kfifoChidMgrGetNextKernelChannel(pGpu, pKernelFifo, pChidMgr,
                                              pIt, ppKernelChannel) == NV_OK)
        {
            return NV_OK;
        }
        else
        {
            pIt->runlistId++;
            // Reset iterator for next runlist
            pIt->physicalChannelID = 0;
            pIt->pFifoDataBlock = NULL;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief Localize the engine info received from the host
 *
 * The host and the guest can run in different version of drivers, the guest driver
 * can not directly use the raw MC_ENGINE_IDX values from the host.
 * RM does not guarantee those values are consistent cross branches.
 *
 * To keep the compatibility between different version of branches, this function reconstructs
 * of RM_ENGINE_TYPE, MC_ENGINE_IDX values based on NV2080_ENGINE_TYPE
 *
 * @param[in] pGpu                 OBJGPU pointer
 * @param[in] pFifo                KernelFifo pointer
 * @param[in/out] pEngine          Pointer to engine info table to update
 *
 */
static void
_kfifoLocalizeGuestEngineData
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    ENGINE_INFO *pEngineInfo
)
{
    const FIFO_GUEST_ENGINE_TABLE *guestEngineTable;
    NvU32 guestEngineTableSz;
    NvU32 nv2080EngineType;
    NvU32 engineIdx;
    NvU32 newEngineIdx;
    NvU32 guestTableIdx;

    // This function should only be called in a vgpu guest RM
    NV_ASSERT_OR_RETURN_VOID(IS_VIRTUAL(pGpu));

    guestEngineTable = kfifoGetGuestEngineLookupTable(&guestEngineTableSz);

    newEngineIdx = 0;

    for (engineIdx = 0; engineIdx < pEngineInfo->engineInfoListSize; engineIdx++)
    {
        FIFO_ENGINE_LIST *pEngine = &pEngineInfo->engineInfoList[engineIdx];

        // The actual data in engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE] is NV2080 ENGINE TYPE.
        nv2080EngineType = pEngine->engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE];

        for (guestTableIdx = 0; guestTableIdx < guestEngineTableSz; guestTableIdx++)
        {
            // Find the engine type supported by the guest
            if (guestEngineTable[guestTableIdx].nv2080EngineType == nv2080EngineType)
                break;
        }

        if (guestTableIdx < guestEngineTableSz)
        {
            // Update the MC for the guest
            pEngine->engineData[ENGINE_INFO_TYPE_MC] = guestEngineTable[guestTableIdx].mcIdx;

            // Update it with correct engine type
            pEngine->engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE] = gpuGetRmEngineType(nv2080EngineType);

            if (newEngineIdx != engineIdx)
            {
                //
                // Move the engine info up to make sure the engine info table only contains data for
                // guest supported engine types.
                //
                portMemCopy(&pEngineInfo->engineInfoList[newEngineIdx], sizeof(FIFO_ENGINE_LIST),
                            &pEngineInfo->engineInfoList[engineIdx], sizeof(FIFO_ENGINE_LIST));
            }

            newEngineIdx++;
        }
    }

    pEngineInfo->engineInfoListSize = newEngineIdx;
}

/*!
 * @brief Performs an RPC into Host RM to read its device info table.
 *
 * This is necessary because in virtual environments, we cannot directly read
 * the device info table, and do not have the physical GPU partitioning
 * information to determine which engines belong to this guest, so we have Host
 * RM do the filtering and send us the filtered table.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 *
 * @return NV_OK if succcessful,
 *         NV_ERR_NOT_SUPPORTED if Host RM calls this interface
 *         NV_ERR_INVALID_STATE if host supplied invalid data
 *         NV_STATUS supplied by RPC response from Host
 */

NV_STATUS
kfifoGetHostDeviceInfoTable_KERNEL
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo,
    ENGINE_INFO *pEngineInfo,
    Device      *pMigDevice
)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle hClient = NV01_NULL_OBJECT;
    NvHandle hObject = NV01_NULL_OBJECT;
    NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams;
    NV2080_CTRL_FIFO_DEVICE_ENTRY *pFetchedTable;
    NvU32 numEntries;
    NvU32 device;
    NvU32 entry;
    NvU32 numRunlists;
    NvU32 maxRunlistId;
    NvU32 maxPbdmaId;
    NvU32 i;
    struct {
        NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS params;
        NV2080_CTRL_FIFO_DEVICE_ENTRY entries[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_DEVICES];
    } *pLocals;

    if (!IS_MIG_IN_USE(pGpu))
    {
        hClient = pGpu->hInternalClient;
        hObject = pGpu->hInternalSubdevice;
    }
    else
    {
        RsClient *pClient = RES_GET_CLIENT(pMigDevice);
        Subdevice *pSubdevice;

        NV_ASSERT_OK_OR_RETURN(
            subdeviceGetByInstance(pClient, RES_GET_HANDLE(pMigDevice), 0, &pSubdevice));

        GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

        hClient = pClient->hClient;
        hObject = RES_GET_HANDLE(pSubdevice);
    }

    // Allocate pFetchedTable and params on the heap to avoid stack overflow
    pLocals = portMemAllocNonPaged(sizeof(*pLocals));
    NV_ASSERT_OR_RETURN((pLocals != NULL), NV_ERR_NO_MEMORY);

    pParams = &pLocals->params;
    pFetchedTable = pLocals->entries;

    //
    // Read device info table entries from Host RM until Host indicates that
    // there are no more valid entries in the table (by setting bMore flag)
    //
    numEntries = 0;
    for (device = 0;
         device < NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_DEVICES;
         device += NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES)
    {
        portMemSet(pParams, 0x0, sizeof(*pParams));
        pParams->baseIndex = device;

        status = pRmApi->Control(pRmApi,
                                 hClient,
                                 hObject,
                                 NV2080_CTRL_CMD_FIFO_GET_DEVICE_INFO_TABLE,
                                 pParams,
                                 sizeof(*pParams));

        if (status != NV_OK)
            goto cleanup;

        // Assert that host RM didn't tell us an invalid number of entries
        if (pParams->numEntries >
            NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES)
        {
            DBG_BREAKPOINT();
            status = NV_ERR_INVALID_STATE;
            goto cleanup;
        }

        portMemCopy(&pFetchedTable[device],
                    pParams->numEntries * (sizeof *pFetchedTable),
                    pParams->entries,
                    pParams->numEntries * (sizeof *pParams->entries));

        numEntries += pParams->numEntries;

        if (!pParams->bMore)
        {
            break;
        }
    }

    pEngineInfo->engineInfoListSize = numEntries;
    pEngineInfo->engineInfoList = portMemAllocNonPaged(sizeof(*pEngineInfo->engineInfoList) *
                                                       pEngineInfo->engineInfoListSize);
    if (pEngineInfo->engineInfoList == NULL)
    {
        NV_CHECK(LEVEL_ERROR, pEngineInfo->engineInfoList != NULL);
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    // Copy each entry from the table
    numRunlists = 0;
    maxRunlistId = 0;
    maxPbdmaId = 0;
    for (entry = 0; entry < numEntries; ++entry)
    {
        FIFO_ENGINE_LIST *pLocalEntry = &pEngineInfo->engineInfoList[entry];
        NV2080_CTRL_FIFO_DEVICE_ENTRY *pFetchedEntry = &pFetchedTable[entry];

        ct_assert(sizeof pLocalEntry->engineData <=
                  sizeof pFetchedEntry->engineData);
        portMemCopy(pLocalEntry->engineData,
                    sizeof pLocalEntry->engineData,
                    pFetchedEntry->engineData,
                    NV_ARRAY_ELEMENTS(pLocalEntry->engineData) *
                        (sizeof *pFetchedEntry->engineData));

        pLocalEntry->numPbdmas = pFetchedEntry->numPbdmas;
        NV_ASSERT_TRUE_OR_GOTO(status,
            pLocalEntry->numPbdmas <=
                    NV_ARRAY_ELEMENTS(pLocalEntry->pbdmaIds) &&
                pLocalEntry->numPbdmas <=
                    NV_ARRAY_ELEMENTS(pLocalEntry->pbdmaFaultIds),
            NV_ERR_INVALID_STATE,
            cleanup);
        portMemCopy(
            pLocalEntry->pbdmaIds,
            pLocalEntry->numPbdmas * (sizeof *(pLocalEntry->pbdmaIds)),
            pFetchedEntry->pbdmaIds,
            pLocalEntry->numPbdmas * (sizeof *(pFetchedEntry->pbdmaIds)));
        portMemCopy(
            pLocalEntry->pbdmaFaultIds,
            pLocalEntry->numPbdmas * (sizeof *(pLocalEntry->pbdmaFaultIds)),
            pFetchedEntry->pbdmaFaultIds,
            pLocalEntry->numPbdmas * (sizeof *(pFetchedEntry->pbdmaFaultIds)));

        portStringCopy(pLocalEntry->engineName,
                       sizeof pLocalEntry->engineName,
                       pFetchedEntry->engineName,
                       sizeof pFetchedEntry->engineName);

        if (pLocalEntry->engineData[ENGINE_INFO_TYPE_IS_HOST_DRIVEN_ENGINE] != 0)
        {
            numRunlists++;
        }
        maxRunlistId = NV_MAX(maxRunlistId,
                              pFetchedEntry->engineData[ENGINE_INFO_TYPE_RUNLIST]);

        for (i = 0; i < pLocalEntry->numPbdmas; i++)
        {
            maxPbdmaId = NV_MAX(maxPbdmaId, pLocalEntry->pbdmaIds[i]);

            //
            // SW engine while being constructed does not populate any PBDMA Fault IDs.
            // Hence, skipping it.
            //
            if (pLocalEntry->engineData[ENGINE_INFO_TYPE_ENG_DESC] != ENG_SW)
            {
                bitVectorSet(&pEngineInfo->validEngineIdsForPbdmas, pLocalEntry->pbdmaFaultIds[i]);
            }
        }
    }

    NV_ASSERT_OK_OR_GOTO(status,
        kfifoReservePbdmaFaultIds_HAL(pGpu, pKernelFifo,
                                      pEngineInfo->engineInfoList,
                                      pEngineInfo->engineInfoListSize),
        cleanup);

    if (IS_VIRTUAL(pGpu))
    {
        _kfifoLocalizeGuestEngineData(pGpu, pKernelFifo, pEngineInfo);
    }

    pEngineInfo->numRunlists = numRunlists;
    pEngineInfo->maxNumRunlists = maxRunlistId + 1;
    pEngineInfo->maxNumPbdmas = maxPbdmaId + 1;

cleanup:
    portMemFree(pLocals);

    return status;
}

/*!
 * @brief Constructs EngineInfo List
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 *
 * @return NV_OK if succcessful,
 *         NV_STATUS supplied by HALs called
 */
NV_STATUS
kfifoConstructEngineList_KERNEL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    ENGINE_INFO *pEngineInfo = &pKernelFifo->engineInfo;

    // Return early if EngineList is already constructed
    if (pEngineInfo->engineInfoList != NULL)
        return NV_OK;

    if (IS_GSP_CLIENT(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(gpuConstructDeviceInfoTable_HAL(pGpu));
    }

    NV_ASSERT_OK_OR_RETURN(kfifoGetHostDeviceInfoTable_HAL(pGpu, pKernelFifo, pEngineInfo, 0));

    return NV_OK;
}

/**
 * @brief Create a list of channels.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pList
 */
NV_STATUS
kfifoChannelListCreate_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    CHANNEL_LIST **ppList
)
{
    if (!ppList)
        return NV_ERR_INVALID_ARGUMENT;

    *ppList = portMemAllocNonPaged(sizeof(CHANNEL_LIST));
    NV_ASSERT_OR_RETURN((*ppList != NULL), NV_ERR_NO_MEMORY);

    (*ppList)->pHead = NULL;
    (*ppList)->pTail = NULL;

    return NV_OK;
}

/**
 * @brief Append a channel to a channel list.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pKernelChannel
 * @param pList
 */

NV_STATUS
kfifoChannelListAppend_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernel,
    KernelChannel *pKernelChannel,
    CHANNEL_LIST *pList
)
{
    PCHANNEL_NODE pNewNode = NULL;

    if (!pKernelChannel || !pList)
        return NV_ERR_INVALID_ARGUMENT;

    pNewNode = portMemAllocNonPaged(sizeof(CHANNEL_NODE));
    NV_ASSERT_OR_RETURN((pNewNode != NULL), NV_ERR_NO_MEMORY);

    pNewNode->pKernelChannel = pKernelChannel;
    pKernelChannel->refCount++;

    pNewNode->pNext = NULL;

    // Searching based on the ChID
    if (pList->pTail)
    {
        pList->pTail->pNext = pNewNode;
        pList->pTail = pNewNode;
    }
    else
    {
        pList->pHead = pNewNode;
        pList->pTail = pNewNode;
    }

    return NV_OK;
}

/**
 * @brief remove channel from the given channel list
 *  look for duplicates.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pKernelChannel
 * @param pList
 */
NV_STATUS
kfifoChannelListRemove_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    KernelChannel *pKernelChannel,
    CHANNEL_LIST *pList
)
{
    PCHANNEL_NODE pNewNode   = NULL;
    PCHANNEL_NODE pPrevNode  = NULL;
    PCHANNEL_NODE pTempNode  = NULL;
    NvBool        bFoundOnce = NV_FALSE;
    NV_STATUS     status     = NV_OK;

    if (!pKernelChannel)
        return NV_ERR_INVALID_ARGUMENT;

    if (!pList)
        return NV_ERR_INVALID_ARGUMENT;

    pNewNode = pList->pHead;
    pPrevNode = NULL;

    while (pNewNode)
    {

        if (pKernelChannel != pNewNode->pKernelChannel)
        {
            pPrevNode = pNewNode;
            pNewNode  = pNewNode->pNext;
            continue;
        }

        // Deleting first node
        if (pList->pHead == pNewNode)
        {
            pList->pHead = pNewNode->pNext;
        }

        // Deleting tail node
        if (pList->pTail == pNewNode)
        {
            pList->pTail =  pPrevNode;
        }

        // First node does not have previous node.
        if (pPrevNode)
        {
            pPrevNode->pNext = pNewNode->pNext;
        }

        pTempNode        = pNewNode;
        pNewNode         = pNewNode->pNext;
        portMemFree(pTempNode);

        bFoundOnce       = NV_TRUE;

        if (0 == pKernelChannel->refCount)
        {
            NV_PRINTF(LEVEL_ERROR, "RefCount for channel is not right!!!\n");
            DBG_BREAKPOINT();
            status = NV_ERR_GENERIC;
            break;
        }

        pKernelChannel->refCount--;
    }


    if (!bFoundOnce)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Can't find channel in channelGroupList (Normal during RC Recovery on "
                  "GK110+ or if software scheduling is enabled).\n");

        status =  NV_ERR_INVALID_CHANNEL;
    }

    return status;
}

/**
 * @brief Destroy channel list.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pList
 */
NV_STATUS
kfifoChannelListDestroy_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernel,
    CHANNEL_LIST *pList
)
{
    PCHANNEL_NODE pTempNode;

    if (!pList)
        return NV_OK;

    while (pList->pHead)
    {
        pTempNode = pList->pHead;

        NV_ASSERT_OR_RETURN(pTempNode->pKernelChannel && pTempNode->pKernelChannel->refCount, NV_ERR_INVALID_STATE);

        pTempNode->pKernelChannel->refCount--;

        pList->pHead = pTempNode->pNext;
        portMemFree(pTempNode);
    }

    portMemFree(pList);

    return NV_OK;
}

/*!
 * @brief   Determines whether provided engines have any channels/contexts assigned
 *
 * @param[IN]   pGpu             OBJGPU
 * @param[IN]   pKernelFifo      KernelFifo
 * @param[IN]   pEngines         Which engines to check (RM_ENGINE_TYPE_***)
 * @param[IN]   engineCount      Number of engines to check
 *
 * @return  Returns NV_TRUE if any provided engines are active
 */
NvBool
kfifoEngineListHasChannel_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    RM_ENGINE_TYPE *pEngines,
    NvU32       engineCount
)
{
    KernelChannel *pKernelChannel;
    CHANNEL_ITERATOR it;
    NvU32 i;

    NV_ASSERT_OR_RETURN((pEngines != NULL) && (engineCount > 0), NV_TRUE);

    // Find any channels or contexts on passed engines
    kfifoGetChannelIterator(pGpu, pKernelFifo, &it, INVALID_RUNLIST_ID);
    while (kchannelGetNextKernelChannel(pGpu, &it, &pKernelChannel) == NV_OK)
    {
        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);

        // If the client supplied the engine type, directly check it
        if (RM_ENGINE_TYPE_IS_VALID(kchannelGetEngineType(pKernelChannel)))
        {
            for (i = 0; i < engineCount; ++i)
            {
                if (kchannelGetEngineType(pKernelChannel) == pEngines[i])
                {
                    NV_PRINTF(LEVEL_ERROR,
                        "Found channel on engine 0x%x owned by 0x%x\n",
                         kchannelGetEngineType(pKernelChannel), RES_GET_CLIENT_HANDLE(pKernelChannel));

                    return NV_TRUE;
                }
            }
        }
        else
        {
            NvU32 runlistId;

            // Ideally valid engine Id should always be set in channel if this property is enabled
            NV_ASSERT_OR_RETURN(!kfifoIsPerRunlistChramEnabled(pKernelFifo), NV_TRUE);

            //
            // If runlist Id for channel is set then check if it matches with any of the engines
            // If channel is not associated with any engine then there is a chance
            // it can be created on one of the engines we care about.
            //
            if (kchannelIsRunlistSet(pGpu, pKernelChannel))
            {
                for (i = 0; i < engineCount; ++i)
                {
                    NV_ASSERT_OR_RETURN((kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                           ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)pEngines[i],
                                           ENGINE_INFO_TYPE_RUNLIST, &runlistId) == NV_OK), NV_TRUE);
                    if (kchannelGetRunlistId(pKernelChannel) == runlistId)
                    {
                        NV_PRINTF(LEVEL_ERROR,
                            "Found channel on runlistId 0x%x owned by 0x%x\n",
                             kchannelGetRunlistId(pKernelChannel), RES_GET_CLIENT_HANDLE(pKernelChannel));

                        return NV_TRUE;
                    }
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Found channel owned by 0x%x that can be associated to any engine\n",
                     RES_GET_CLIENT_HANDLE(pKernelChannel));

                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

/**
 * @brief Maximum number of subcontexts for a non-legacy mode TSG
 */
CTX_BUF_POOL_INFO *
kfifoGetRunlistBufPool_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    RM_ENGINE_TYPE rmEngineType
)
{
    return pKernelFifo->pRunlistBufPool[rmEngineType];
}

/**
 * @brief Get size and alignment requirements for runlist buffers
 *
 * @param[in]  pGpu                 Pointer to OBJGPU
 * @param[in]  pKernelFifo          Pointer to KernelFifo
 * @param[in]  runlistId            Runlist ID
 * @param[in]  bTsgSupported        Is TSG supported
 * @param[in]  maxRunlistEntries    Max entries to be supported in a runlist
 * @param[out] pSize                Size of runlist buffer
 * @param[out] pAlignment           Alignment for runlist buffer
 */
NV_STATUS
kfifoGetRunlistBufInfo_IMPL
(
    OBJGPU       *pGpu,
    KernelFifo   *pKernelFifo,
    NvU32         runlistId,
    NvBool        bTsgSupported,
    NvU32         maxRunlistEntries,
    NvU64        *pSize,
    NvU64        *pAlignment
)
{
    NvU32           runlistEntrySize = 0;
    NvU32           maxRunlistEntriesSupported = 0;
    CHID_MGR       *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

    NV_ASSERT_OR_RETURN(pSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pAlignment != NULL, NV_ERR_INVALID_ARGUMENT);

    if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        NV_ASSERT_OR_RETURN(pChidMgr != NULL, NV_ERR_INVALID_ARGUMENT);
        //
        // We assume worst case of one TSG wrapper per channel, and
        // the number of TSGs + number of channels is how we get
        // the 2 x number of fifos.
        //
        maxRunlistEntriesSupported = 2 * kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    }
    else
    {
        maxRunlistEntriesSupported = kfifoGetMaxChannelsInSystem(pGpu, pKernelFifo);
        maxRunlistEntriesSupported += (bTsgSupported ?
                                       kfifoGetMaxChannelGroupsInSystem(pGpu, pKernelFifo)
                                       : 0);
    }

    NV_ASSERT_OR_RETURN(maxRunlistEntries <= maxRunlistEntriesSupported, NV_ERR_INVALID_ARGUMENT);

    if (maxRunlistEntries == 0)
    {
        maxRunlistEntries = maxRunlistEntriesSupported;
    }

    runlistEntrySize = kfifoRunlistGetEntrySize_HAL(pKernelFifo);
    *pSize = (NvU64)runlistEntrySize * maxRunlistEntries;

    *pAlignment = NVBIT64(kfifoRunlistGetBaseShift_HAL(pKernelFifo));
    return NV_OK;
}

/*!
 * @brief Gets total number of channels supported by the system
 */
NvU32
kfifoGetMaxChannelsInSystem_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32 numChannels = 0;
    NvU32 i;

    for (i = 0; i < pKernelFifo->numChidMgrs; i++)
    {
        if (pKernelFifo->ppChidMgr[i] != NULL)
        {
            numChannels += kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pKernelFifo->ppChidMgr[i]);
        }
    }
    return numChannels;
}

/*!
 * @brief Gets total number of channel groups supported by the system
 */
NvU32
kfifoGetMaxChannelGroupsInSystem_IMPL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo
)
{
    // Max channel groups is the same as max channels
    return kfifoGetMaxChannelsInSystem(pGpu, pKernelFifo);
}

/*!
 * @brief Get runlist buffer allocation params
 *
 * @param[in]    pGpu
 * @param[out]  *pAperture      Aperture to use for runlist buffer allocation
 * @param[out]  *pAttr          Attributes to use for runlits buffer allocation
 * @param[out]  *pAllocFlags    Allocation flags to use for runlist buffer allocation
 */
void
kfifoRunlistGetBufAllocParams_IMPL
(
    OBJGPU           *pGpu,
    NV_ADDRESS_SPACE *pAperture,
    NvU32            *pAttr,
    NvU64            *pAllocFlags
)
{
    *pAperture = ADDR_FBMEM;
    *pAttr = NV_MEMORY_WRITECOMBINED;

    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC, _RUNLIST, pGpu->instLocOverrides),
                           "runlist", pAperture, pAttr);

    *pAllocFlags = FLD_TEST_DRF(_REG_STR_RM, _INST_VPR, _RUNLIST, _TRUE, pGpu->instVprOverrides)
                       ? MEMDESC_ALLOC_FLAGS_PROTECTED : MEMDESC_FLAGS_NONE;
}

/*!
 * @brief Allocate Runlist buffers for a single runlistId
 *
 * @param[in]   pGpu
 * @param[in]   pKernelFifo
 * @param[in]   bSupportTsg         Will this runlist support TSGs?
 * @param[in]   aperture            NV_ADDRESS_SPACE requested
 * @param[in]   runlistId           runlistId to allocate buffer for
 * @param[in]   attr                CPU cacheability requested
 * @param[in]   allocFlags          MEMDESC_FLAGS_*
 * @param[in]   maxRunlistEntries   Can pass zero to determine in function
 * @param[in]   bHWRL               Is this runlist a HW runlist? (verif feature specific)
 * @param[out]  ppMemDesc           memdesc created/allocated by function
 */
NV_STATUS
kfifoRunlistAllocBuffers_IMPL
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    NvBool              bSupportTsg,
    NV_ADDRESS_SPACE    aperture,
    NvU32               runlistId,
    NvU32               attr,
    NvU64               allocFlags,
    NvU64               maxRunlistEntries,
    NvBool              bHWRL,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    NV_STATUS status        = NV_OK;
    NvU64     runlistSz     = 0;
    NvU64     runlistAlign  = 0;
    NvU32     counter;

    status = kfifoGetRunlistBufInfo(pGpu, pKernelFifo, runlistId, bSupportTsg,
        maxRunlistEntries, &runlistSz, &runlistAlign);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to get runlist buffer info 0x%08x\n",
                  status);
        DBG_BREAKPOINT();
        goto failed;
    }

    for (counter = 0; counter < NUM_BUFFERS_PER_RUNLIST; ++counter)
    {
        ppMemDesc[counter] = NULL;

        status = memdescCreate(&ppMemDesc[counter], pGpu, runlistSz, runlistAlign,
                               NV_TRUE, aperture, attr, allocFlags);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Runlist buffer memdesc create failed 0x%08x\n", status);
            DBG_BREAKPOINT();
            goto failed;
        }

        // If flag is set then allocate runlist from ctx buf pool
        if (allocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL)
        {
            RM_ENGINE_TYPE rmEngineType;
            CTX_BUF_POOL_INFO *pCtxBufPool = NULL;
            status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RUNLIST,
                runlistId, ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to translate runlistId 0x%x to NV2080 engine type\n", runlistId);
                DBG_BREAKPOINT();
                goto failed;
            }
            status = ctxBufPoolGetGlobalPool(pGpu, CTX_BUF_ID_RUNLIST, rmEngineType, &pCtxBufPool);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to get ctx buf pool for engine type 0x%x (0x%x)\n",
                          gpuGetNv2080EngineType(rmEngineType), rmEngineType);
                DBG_BREAKPOINT();
                goto failed;
            }
            status = memdescSetCtxBufPool(ppMemDesc[counter], pCtxBufPool);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to set ctx buf pool for runlistId 0x%x\n", runlistId);
                DBG_BREAKPOINT();
                goto failed;
            }
        }

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_101,
                        ppMemDesc[counter]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Runlist buffer mem alloc failed 0x%08x\n",
                      status);
            DBG_BREAKPOINT();
            goto failed;
        }
    }

    return NV_OK;

failed:
    for (counter = 0; counter < NUM_BUFFERS_PER_RUNLIST; counter++)
    {
        if (ppMemDesc[counter])
        {
            memdescFree(ppMemDesc[counter]);
            memdescDestroy(ppMemDesc[counter]);
            ppMemDesc[counter] = NULL;
        }
    }
    return status;
}

NvU32
kfifoGetMaxSubcontextFromGr_KERNEL
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pKernelGraphicsManager != NULL, 0);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, 0);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, 0);

    return kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT].data;
}

NvU32
kfifoReturnPushbufferCaps_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32 kfifoBitMask = 0;

    // PCI is always supported
    kfifoBitMask = PCI_PB_ALLOWED;

    if (!gpuIsUnifiedMemorySpaceEnabled(pGpu))
    {
        kfifoBitMask |= VID_PB_ALLOWED;
    }

    return kfifoBitMask;
}

/*!
 * @brief kfifoGetDeviceCaps
 *
 * This routine gets cap bits in unicast. If pbCapsInitialized is passed as
 * NV_FALSE, the caps will be copied into pKfifoCaps without OR/ANDING.
 * Otherwise, the caps bits for the current GPU will be ORed/ANDed together with
 * pKfifoCaps to create a single set of caps that accurately represents the
 * functionality of the device.
 */
void kfifoGetDeviceCaps_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU8       *pKfifoCaps,
    NvBool      bCapsInitialized
)
{
    NvU8        tempCaps[NV0080_CTRL_FIFO_CAPS_TBL_SIZE];
    NvU8        temp;
    NvU32       kfifoBitMask;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    portMemSet(tempCaps, 0, NV0080_CTRL_FIFO_CAPS_TBL_SIZE);

    kfifoBitMask = kfifoReturnPushbufferCaps(pGpu, pKernelFifo);

    if (kfifoBitMask & PCI_PB_ALLOWED)
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _SUPPORT_PCI_PB);
    if (kfifoBitMask & VID_PB_ALLOWED)
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _SUPPORT_VID_PB);

    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
        !gpuIsPipelinedPteMemEnabled(pGpu))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _NO_PIPELINED_PTE_BLIT);

    if (kfifoIsUserdInSystemMemory(pKernelFifo))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _USERD_IN_SYSMEM);

    if (kfifoIsUserdMapDmaSupported(pKernelFifo))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _GPU_MAP_CHANNEL);

    if (kfifoHostHasLbOverflow(pKernelFifo))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _HAS_HOST_LB_OVERFLOW_BUG_1667921);

    if (kfifoIsSubcontextSupported(pKernelFifo))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _MULTI_VAS_PER_CHANGRP);

    if (kfifoIsWddmInterleavingPolicyEnabled(pKernelFifo))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FIFO_CAPS, _SUPPORT_WDDM_INTERLEAVING);

    // if this is the first GPU in the device, then start with it's caps
    if (bCapsInitialized == NV_FALSE)
    {
        portMemCopy(pKfifoCaps, NV0080_CTRL_FIFO_CAPS_TBL_SIZE,
                    tempCaps, NV0080_CTRL_FIFO_CAPS_TBL_SIZE);
        return;
    }

    RMCTRL_AND_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _SUPPORT_PCI_PB);
    RMCTRL_AND_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _SUPPORT_VID_PB);
    RMCTRL_AND_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _GPU_MAP_CHANNEL);

    RMCTRL_OR_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _MULTI_VAS_PER_CHANGRP);

    RMCTRL_OR_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _HAS_HOST_LB_OVERFLOW_BUG_1667921);

    RMCTRL_OR_CAP(pKfifoCaps, tempCaps, temp,
                   NV0080_CTRL_FIFO_CAPS, _SUPPORT_WDDM_INTERLEAVING);
    return;
}

/**
 * @brief Get the start offset of USERD BAR1 map region
 */
NvU64
kfifoGetUserdBar1MapStartOffset_VF(OBJGPU *pGpu, KernelFifo *pKernelFifo)
{

    return 0;
}

/*!
 * @brief Add handlers for scheduling enable and/or disable.
 *
 * @param[in] pGpu                             OBJGPU pointer
 * @param[in] pKernelFifo                      KernelFifo pointer
 * @param[in] pPostSchedulingEnableHandler     No action if NULL
 * @param[in] pPostSchedulingEnableHandlerData Data to pass to
 *                                             @p pPostSchedulingEnableHandler
 * @param[in] pPreSchedulingDisableHandler     No action if NULL
 * @param[in] pPreSchedulingDisableHandlerData Data to pass to
 *                                             @p pPreSchedulingDisableHandler
 *
 * @returns  NV_OK if successfully processed both handlers
 *           NV_WARN_NOTHING_TO_DO if:  - Both handlers are NULL
 *                                      - Both handlers are already installed
 *           NV_ERR_INVALID_STATE if one handler is already installed, but not both
 */
NV_STATUS
kfifoAddSchedulingHandler_IMPL
(
    OBJGPU                 *pGpu,
    KernelFifo             *pKernelFifo,
    PFifoSchedulingHandler  pPostSchedulingEnableHandler,
    void                   *pPostSchedulingEnableHandlerData,
    PFifoSchedulingHandler  pPreSchedulingDisableHandler,
    void                   *pPreSchedulingDisableHandlerData
)
{
    FifoSchedulingHandlerEntry *pEntry;
    NvBool bPostHandlerAlreadyPresent = NV_FALSE;
    NvBool bPreHandlerAlreadyPresent = NV_FALSE;
    FifoSchedulingHandlerEntry postEntry;
    FifoSchedulingHandlerEntry preEntry;

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       (pPostSchedulingEnableHandler != NULL) ||
                       (pPreSchedulingDisableHandler != NULL),
                       NV_WARN_NOTHING_TO_DO);

    // Check for already installed handler if non-NULL
    if (pPostSchedulingEnableHandler != NULL)
    {
        for (pEntry = listHead(&pKernelFifo->postSchedulingEnableHandlerList);
             pEntry != NULL;
             pEntry = listNext(&pKernelFifo->postSchedulingEnableHandlerList, pEntry))
        {
            if (pEntry->pCallback == pPostSchedulingEnableHandler &&
                pEntry->pCallbackParam == pPostSchedulingEnableHandlerData)
            {
                bPostHandlerAlreadyPresent = NV_TRUE;
                break;
            }
        }
    }

    // Check for already installed handler if non-NULL
    if (pPreSchedulingDisableHandler != NULL)
    {
        for (pEntry = listHead(&pKernelFifo->preSchedulingDisableHandlerList);
             pEntry != NULL;
             pEntry = listNext(&pKernelFifo->preSchedulingDisableHandlerList, pEntry))
        {
            if (pEntry->pCallback == pPreSchedulingDisableHandler &&
                pEntry->pCallbackParam == pPreSchedulingDisableHandlerData)
            {
                bPreHandlerAlreadyPresent = NV_TRUE;
                break;
            }
        }
    }

    //
    // If we are installing both handlers, and one is already present, but not
    // the other, we will do nothing, so assert loudly in that case
    //
    if ((pPostSchedulingEnableHandler != NULL) && (pPreSchedulingDisableHandler != NULL))
    {
        NV_ASSERT_OR_RETURN(!(bPostHandlerAlreadyPresent ^ bPreHandlerAlreadyPresent),
                            NV_ERR_INVALID_STATE);
    }

    // Return early unless all non-null handlers are not already installed
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       !bPostHandlerAlreadyPresent && !bPreHandlerAlreadyPresent,
                       NV_WARN_NOTHING_TO_DO);

    // Add handler entry to list unless NULL
    if (pPostSchedulingEnableHandler != NULL)
    {
        postEntry.pCallback = pPostSchedulingEnableHandler;
        postEntry.pCallbackParam = pPostSchedulingEnableHandlerData;
        postEntry.bHandled = NV_FALSE;
        NV_ASSERT_OR_RETURN(listPrependValue(&pKernelFifo->postSchedulingEnableHandlerList, &postEntry),
                            NV_ERR_NO_MEMORY);
    }

    // Add handler entry to list unless NULL
    if (pPreSchedulingDisableHandler != NULL)
    {
        preEntry.pCallback = pPreSchedulingDisableHandler;
        preEntry.pCallbackParam = pPreSchedulingDisableHandlerData;
        preEntry.bHandled = NV_FALSE;
        NV_ASSERT_OR_RETURN(listPrependValue(&pKernelFifo->preSchedulingDisableHandlerList, &preEntry),
                            NV_ERR_NO_MEMORY);
    }

    return NV_OK;
}

/*!
 * @brief Remove handlers for scheduling enable and/or disable.
 *
 * @param[in] pGpu                             OBJGPU pointer
 * @param[in] pKernelFifo                      KernelFifo pointer
 * @param[in] pPostSchedulingEnableHandler     No action if NULL
 * @param[in] pPostSchedulingEnableHandlerData Data argument set for the
 *                                             handler.
 * @param[in] pPreSchedulingDisableHandler     No action if NULL
 * @param[in] pPreSchedulingDisableHandlerData Data argument set for the
 *                                             handler.
 */
void
kfifoRemoveSchedulingHandler_IMPL
(
    OBJGPU                 *pGpu,
    KernelFifo             *pKernelFifo,
    PFifoSchedulingHandler  pPostSchedulingEnableHandler,
    void                   *pPostSchedulingEnableHandlerData,
    PFifoSchedulingHandler  pPreSchedulingDisableHandler,
    void                   *pPreSchedulingDisableHandlerData
)
{
    FifoSchedulingHandlerEntry *pEntry;
    FifoSchedulingHandlerEntry *pTemp;

    // Search for the post handler in the post handler list and remove it if present
    pEntry = listHead(&pKernelFifo->postSchedulingEnableHandlerList);
    while (pEntry != NULL)
    {
        pTemp = listNext(&pKernelFifo->postSchedulingEnableHandlerList, pEntry);

        if (pEntry->pCallback == pPostSchedulingEnableHandler &&
            pEntry->pCallbackParam == pPostSchedulingEnableHandlerData)
        {
            listRemove(&pKernelFifo->postSchedulingEnableHandlerList, pEntry);
        }

        pEntry = pTemp;
    }

    // Search for the pre handler in the pre handler list and remove it if present
    pEntry = listHead(&pKernelFifo->preSchedulingDisableHandlerList);
    while (pEntry != NULL)
    {
        pTemp = listNext(&pKernelFifo->preSchedulingDisableHandlerList, pEntry);

        if (pEntry->pCallback == pPreSchedulingDisableHandler &&
            pEntry->pCallbackParam == pPreSchedulingDisableHandlerData)
        {
            listRemove(&pKernelFifo->preSchedulingDisableHandlerList, pEntry);
        }

        pEntry = pTemp;
    }
}


/*!
 * @brief Notify handlers that scheduling has been enabled.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelFifo   KernelFifo pointer
 *
 * @returns  NV_STATUS
 */
NV_STATUS
kfifoTriggerPostSchedulingEnableCallback_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NV_STATUS status = NV_OK;
    FifoSchedulingHandlerEntry *pEntry;
    NvBool bFirstPass = NV_TRUE;
    NvBool bRetry;

    do
    {
        NvBool bMadeProgress = NV_FALSE;

        bRetry = NV_FALSE;

        for (pEntry = listHead(&pKernelFifo->postSchedulingEnableHandlerList);
             pEntry != NULL;
             pEntry = listNext(&pKernelFifo->postSchedulingEnableHandlerList, pEntry))
        {
            NV_ASSERT_OR_ELSE(pEntry->pCallback != NULL,
                status = NV_ERR_INVALID_STATE; break;);

            if (bFirstPass)
            {
                // Reset bHandled set by previous call (fore example, for dor suspend-resume)
                pEntry->bHandled = NV_FALSE;
            }
            else if (pEntry->bHandled)
            {
                continue;
            }

            status = pEntry->pCallback(pGpu, pEntry->pCallbackParam);

            if (status == NV_WARN_MORE_PROCESSING_REQUIRED)
            {
                // Retry mechanism: Some callbacks depend on other callbacks in this list.
                bRetry = NV_TRUE;
                // Quash retry status
                status = NV_OK;
            }
            else if (status == NV_OK)
            {
                // Successfully handled, no need to retry
                pEntry->bHandled = NV_TRUE;
                bMadeProgress = NV_TRUE;
            }
            else
            {
                // Actual error, abort
                NV_ASSERT(0);
                break;
            }
        }

        // We are stuck in a loop, and all remaining callbacks are returning NV_WARN_MORE_PROCESSING_REQUIRED
        NV_ASSERT_OR_RETURN(bMadeProgress || status != NV_OK, NV_ERR_INVALID_STATE);

        bFirstPass = NV_FALSE;
    } while (bRetry && status == NV_OK);

    return status;
}

/*!
 * @brief Notify handlers that scheduling will soon be disabled.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelFifo   KernelFifo pointer
 *
 * @returns  NV_STATUS
 */
NV_STATUS
kfifoTriggerPreSchedulingDisableCallback_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NV_STATUS status = NV_OK;
    FifoSchedulingHandlerEntry *pEntry;
    NvBool bFirstPass = NV_TRUE;
    NvBool bRetry;

    do
    {
        NvBool bMadeProgress = NV_FALSE;

        bRetry = NV_FALSE;

        for (pEntry = listHead(&pKernelFifo->preSchedulingDisableHandlerList);
             pEntry != NULL;
             pEntry = listNext(&pKernelFifo->preSchedulingDisableHandlerList, pEntry))
        {
            NV_ASSERT_OR_ELSE(pEntry->pCallback != NULL,
                status = NV_ERR_INVALID_STATE; break;);

            if (bFirstPass)
            {
                // Reset bHandled set by previous call (fore example, for dor suspend-resume)
                pEntry->bHandled = NV_FALSE;
            }
            else if (pEntry->bHandled)
            {
                continue;
            }

            status = pEntry->pCallback(pGpu, pEntry->pCallbackParam);

            if (status == NV_WARN_MORE_PROCESSING_REQUIRED)
            {
                // Retry mechanism: Some callbacks depend on other callbacks in this list.
                bRetry = NV_TRUE;
                // Quash retry status
                status = NV_OK;
            }
            else if (status == NV_OK)
            {
                // Successfully handled, no need to retry
                pEntry->bHandled = NV_TRUE;
                bMadeProgress = NV_TRUE;
            }
            else
            {
                // Actual error, abort
                NV_ASSERT(0);
                break;
            }
        }

        // We are stuck in a loop, and all remaining callbacks are returning NV_WARN_MORE_PROCESSING_REQUIRED
        NV_ASSERT_OR_RETURN(bMadeProgress || status != NV_OK, NV_ERR_INVALID_STATE);

        bFirstPass = NV_FALSE;
    } while (bRetry && status == NV_OK);

    return status;
}

/**
 * @brief Gets vChid corresponding to a sChid
 */
NV_STATUS
kfifoGetVChIdForSChId_FWCLIENT
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       sChId,
    NvU32       gfid,
    NvU32       engineId,
    NvU32      *pVChid
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;

    NV_ASSERT_OR_RETURN(pVChid != NULL, NV_ERR_INVALID_ARGUMENT);
    *pVChid = sChId;

    NV_CHECK_OR_RETURN(LEVEL_INFO, IS_GFID_VF(gfid), NV_OK);
    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextKernelHostVgpuDevice(pGpu, &pKernelHostVgpuDevice));
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice->gfid == gfid, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(engineId < (NV_ARRAY_ELEMENTS(pKernelHostVgpuDevice->chidOffset)),
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice->chidOffset[engineId] != 0, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(sChId >= pKernelHostVgpuDevice->chidOffset[engineId], NV_ERR_INVALID_ARGUMENT);

    *pVChid = sChId - pKernelHostVgpuDevice->chidOffset[engineId];

    return NV_OK;
}

/**
 * @brief cache vChid <-> sChid offset
 */
NV_STATUS
kfifoSetChidOffset_IMPL
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    CHID_MGR         *pChidMgr,
    NvU32             offset,
    NvU32             numChannels,
    NvU32             gfid,
    NvU32            *pChidOffset,
    NvU32            *pChannelCount,
    Device           *pMigDevice,
    NvU32             engineFifoListNumEntries,
    FIFO_ENGINE_LIST *pEngineFifoList
)
{
    NV_STATUS status = NV_OK;
    RM_ENGINE_TYPE *pEngineIds = NULL;
    NvU32 maxEngines = kfifoGetNumEngines_HAL(pGpu, pKernelFifo);
    NvU32 numEngines, i;

    NV_ASSERT_OR_RETURN(pChidMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pChidOffset != NULL, NV_ERR_INVALID_ARGUMENT);

    pEngineIds = portMemAllocNonPaged(sizeof(RM_ENGINE_TYPE) * maxEngines);
    NV_ASSERT_OR_RETURN((pEngineIds != NULL), NV_ERR_NO_MEMORY);
    portMemSet(pEngineIds, 0, sizeof(RM_ENGINE_TYPE) * maxEngines);

    NV_ASSERT_OK_OR_GOTO(status, kfifoGetEngineListForRunlist(pGpu, pKernelFifo, pChidMgr->runlistId, pEngineIds, &numEngines), cleanup)

    for (i = 0; i < numEngines; i++)
    {
        NV_ASSERT_OR_ELSE(NV2080_ENGINE_TYPE_IS_VALID(pEngineIds[i]), status = NV_ERR_INVALID_STATE; goto cleanup);
        pChidOffset[pEngineIds[i]] = offset;
        pChannelCount[pEngineIds[i]] = numChannels;
    }

    NV_ASSERT_OK_OR_GOTO(status, kfifoProgramChIdTable_HAL(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid,
                                                           pMigDevice, engineFifoListNumEntries, pEngineFifoList), cleanup);

cleanup:

    portMemFree(pEngineIds);

    return status;
}

/*
 * @brief Gets a list of engine ids that use this runlist
 *
 * @param[in]  runlistId           Runlist id
 * @param[out] pOutEngineIds       List of engineids
 * @param[out] pNumEngines         # of entries in pOutEngines
 */
NV_STATUS
kfifoGetEngineListForRunlist_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       runlistId,
    RM_ENGINE_TYPE *pOutEngineIds,
    NvU32      *pNumEngines
)
{
    NV_STATUS  status      = NV_OK;
    NvU32      numEngines  = kfifoGetNumEngines_HAL(pGpu, pKernelFifo);
    NvU32      i;

    // Sanity check the input
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pOutEngineIds != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pNumEngines != NULL, NV_ERR_INVALID_ARGUMENT);

    *pNumEngines = 0;
    NV_PRINTF(LEVEL_INFO, "Engine list for runlistId 0x%x:\n", runlistId);

    for (i = 0; i < numEngines; i++)
    {
        RM_ENGINE_TYPE rmEngineType;
        NvU32 thisRunlistId;

        NV_ASSERT_OK_OR_GOTO(status,
                             kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                     ENGINE_INFO_TYPE_INVALID,
                                                     i,
                                                     ENGINE_INFO_TYPE_RUNLIST,
                                                     &thisRunlistId), done);
        if (runlistId == thisRunlistId)
        {
            NV_ASSERT_OK_OR_GOTO(status,
                                 kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                         ENGINE_INFO_TYPE_INVALID,
                                                         i,
                                                         ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                         (NvU32 *)&rmEngineType), done);
            pOutEngineIds[(*pNumEngines)++] = rmEngineType;

            NV_PRINTF(LEVEL_INFO, "Engine name: %s\n",
                       kfifoGetEngineName_HAL(pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                             (NvU32)rmEngineType));
        }
    }
done:
    if ((status != NV_OK) && (*pNumEngines != 0))
    {
        portMemSet(pOutEngineIds, 0, sizeof(NvU32) * (*pNumEngines));
        *pNumEngines = 0;
    }
    return status;
}

/**
 * @brief Return bitmask of currently allocated channels for a given runlist
 *
 * @param[in] bitMaskSize @p pBitMask size in bytes
 *
 */
NV_STATUS
kfifoGetAllocatedChannelMask_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       runlistId,
    NvU32      *pBitMask,
    NvLength    bitMaskSize
)
{
    CHID_MGR *pChidMgr;
    NvU32     chId;
    NvU32     numChannels;

    NV_ASSERT(pBitMask != NULL);
    portMemSet(pBitMask, 0, bitMaskSize);

    NV_ASSERT_OR_RETURN(bitMaskSize % sizeof(NvU32) == 0,
                        NV_ERR_INVALID_ARGUMENT);

    if (!kfifoIsPerRunlistChramEnabled(pKernelFifo))
    {
        if (runlistId > 0)
        {
            return NV_ERR_OUT_OF_RANGE;
        }
        else
        {
            runlistId = CHIDMGR_RUNLIST_ID_LEGACY;
        }
    }
    else
    {
        if (!(runlistId < kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo)))
        {
            return NV_ERR_OUT_OF_RANGE;
        }
    }

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);
    if (pChidMgr == NULL)
    {
        //
        // This runlist is not valid. This is not an error since it might be
        // possible for some runlists between [0, maxRunlists) to be invalid.
        // Simply return that it has no channels to simplify clients iterating
        // over all runlists.
        //
        numChannels = 0;
    }
    else
    {
        numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
    }

    if (((numChannels + 7) / 8) > bitMaskSize)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    for (chId = 0; chId < numChannels; chId++)
    {
        KernelChannel *pKernelChannel;
        pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                      pChidMgr,
                                                      chId);
        if (pKernelChannel != NULL)
        {
            NV_BITMASK32_SET(pBitMask, chId);
        }
    }

    return NV_OK;
}

/*!
 * Get host channel class
 */
NvU32
kfifoGetChannelClassId_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32 numClasses;
    NvU32 *pClassList = NULL;
    CLI_CHANNEL_CLASS_INFO classInfo;
    NvU32 i;
    NvU32 class = 0;

    NV_ASSERT_OR_RETURN(NV_OK == gpuGetClassList(pGpu, &numClasses, NULL, ENG_KERNEL_FIFO), 0);
    NV_ASSERT_OR_RETURN(numClasses > 0, 0);
    pClassList = portMemAllocNonPaged(sizeof(NvU32) * numClasses);
    NV_ASSERT_OR_RETURN((pClassList != NULL), 0);

    if (NV_OK == gpuGetClassList(pGpu, &numClasses, pClassList, ENG_KERNEL_FIFO))
    {
        for (i = 0; i < numClasses; i++)
        {
            if (pClassList[i] == PHYSICAL_CHANNEL_GPFIFO)
            {
                // Skip the physical channel class
                continue;
            }
            CliGetChannelClassInfo(pClassList[i], &classInfo);
            if (classInfo.classType == CHANNEL_CLASS_TYPE_GPFIFO)
                class = NV_MAX(class, pClassList[i]);
        }
    }

    NV_ASSERT(class);
    portMemFree(pClassList);
    return class;
}

/**
 * @brief Return the FIFO_GUEST_ENGINE_TABLE
 *
 * @param[out]  pEngLookupTblSize   To get the table size
 *
 * @return a pointer to FIFO_GUEST_ENGINE_TABLE
 *
 */
const FIFO_GUEST_ENGINE_TABLE *
kfifoGetGuestEngineLookupTable_IMPL
(
    NvU32 *pEngLookupTblSize
)
{
    //
    // This table is used for a guest RM to reconstruct the engine list data
    // received from the host RM. The host and guest RM can be running on
    // different versions.
    //
    // This table does not need to be HALified. It need to match NV2080_ENGINE_TYPE
    // defined in cl2080_notification.h
    //
    const static FIFO_GUEST_ENGINE_TABLE guestEngineLookupTable[] =
    {
        // nv2080EngineType             mcIdx
        {NV2080_ENGINE_TYPE_GR0,        MC_ENGINE_IDX_GR0},
        {NV2080_ENGINE_TYPE_GR1,        MC_ENGINE_IDX_GR1},
        {NV2080_ENGINE_TYPE_GR2,        MC_ENGINE_IDX_GR2},
        {NV2080_ENGINE_TYPE_GR3,        MC_ENGINE_IDX_GR3},
        {NV2080_ENGINE_TYPE_GR4,        MC_ENGINE_IDX_GR4},
        {NV2080_ENGINE_TYPE_GR5,        MC_ENGINE_IDX_GR5},
        {NV2080_ENGINE_TYPE_GR6,        MC_ENGINE_IDX_GR6},
        {NV2080_ENGINE_TYPE_GR7,        MC_ENGINE_IDX_GR7},
        {NV2080_ENGINE_TYPE_COPY0,      MC_ENGINE_IDX_CE0},
        {NV2080_ENGINE_TYPE_COPY1,      MC_ENGINE_IDX_CE1},
        {NV2080_ENGINE_TYPE_COPY2,      MC_ENGINE_IDX_CE2},
        {NV2080_ENGINE_TYPE_COPY3,      MC_ENGINE_IDX_CE3},
        {NV2080_ENGINE_TYPE_COPY4,      MC_ENGINE_IDX_CE4},
        {NV2080_ENGINE_TYPE_COPY5,      MC_ENGINE_IDX_CE5},
        {NV2080_ENGINE_TYPE_COPY6,      MC_ENGINE_IDX_CE6},
        {NV2080_ENGINE_TYPE_COPY7,      MC_ENGINE_IDX_CE7},
        {NV2080_ENGINE_TYPE_COPY8,      MC_ENGINE_IDX_CE8},
        {NV2080_ENGINE_TYPE_COPY9,      MC_ENGINE_IDX_CE9},
        {NV2080_ENGINE_TYPE_NVENC0,     MC_ENGINE_IDX_NVENC},
        {NV2080_ENGINE_TYPE_NVENC1,     MC_ENGINE_IDX_NVENC1},
        {NV2080_ENGINE_TYPE_NVENC2,     MC_ENGINE_IDX_NVENC2},
        {NV2080_ENGINE_TYPE_NVDEC0,     MC_ENGINE_IDX_NVDEC0},
        {NV2080_ENGINE_TYPE_NVDEC1,     MC_ENGINE_IDX_NVDEC1},
        {NV2080_ENGINE_TYPE_NVDEC2,     MC_ENGINE_IDX_NVDEC2},
        {NV2080_ENGINE_TYPE_NVDEC3,     MC_ENGINE_IDX_NVDEC3},
        {NV2080_ENGINE_TYPE_NVDEC4,     MC_ENGINE_IDX_NVDEC4},
        {NV2080_ENGINE_TYPE_NVDEC5,     MC_ENGINE_IDX_NVDEC5},
        {NV2080_ENGINE_TYPE_NVDEC6,     MC_ENGINE_IDX_NVDEC6},
        {NV2080_ENGINE_TYPE_NVDEC7,     MC_ENGINE_IDX_NVDEC7},
        {NV2080_ENGINE_TYPE_SW,         MC_ENGINE_IDX_NULL},
        {NV2080_ENGINE_TYPE_SEC2,       MC_ENGINE_IDX_SEC2},
        {NV2080_ENGINE_TYPE_NVJPEG0,    MC_ENGINE_IDX_NVJPEG0},
        {NV2080_ENGINE_TYPE_NVJPEG1,    MC_ENGINE_IDX_NVJPEG1},
        {NV2080_ENGINE_TYPE_NVJPEG2,    MC_ENGINE_IDX_NVJPEG2},
        {NV2080_ENGINE_TYPE_NVJPEG3,    MC_ENGINE_IDX_NVJPEG3},
        {NV2080_ENGINE_TYPE_NVJPEG4,    MC_ENGINE_IDX_NVJPEG4},
        {NV2080_ENGINE_TYPE_NVJPEG5,    MC_ENGINE_IDX_NVJPEG5},
        {NV2080_ENGINE_TYPE_NVJPEG6,    MC_ENGINE_IDX_NVJPEG6},
        {NV2080_ENGINE_TYPE_NVJPEG7,    MC_ENGINE_IDX_NVJPEG7},
        {NV2080_ENGINE_TYPE_OFA0,       MC_ENGINE_IDX_OFA0},
        {NV2080_ENGINE_TYPE_OFA1,       MC_ENGINE_IDX_OFA1},
        // removal tracking bug: 3748354
        {NV2080_ENGINE_TYPE_COPY10,     MC_ENGINE_IDX_CE10},
        {NV2080_ENGINE_TYPE_COPY11,     MC_ENGINE_IDX_CE11},
        {NV2080_ENGINE_TYPE_COPY12,     MC_ENGINE_IDX_CE12},
        {NV2080_ENGINE_TYPE_COPY13,     MC_ENGINE_IDX_CE13},
        {NV2080_ENGINE_TYPE_COPY14,     MC_ENGINE_IDX_CE14},
        {NV2080_ENGINE_TYPE_COPY15,     MC_ENGINE_IDX_CE15},
        {NV2080_ENGINE_TYPE_COPY16,     MC_ENGINE_IDX_CE16},
        {NV2080_ENGINE_TYPE_COPY17,     MC_ENGINE_IDX_CE17},
        {NV2080_ENGINE_TYPE_COPY18,     MC_ENGINE_IDX_CE18},
        {NV2080_ENGINE_TYPE_COPY19,     MC_ENGINE_IDX_CE19},
        // removal tracking bug: 3748354
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0,      MC_ENGINE_IDX_CE0},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY1,      MC_ENGINE_IDX_CE1},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY2,      MC_ENGINE_IDX_CE2},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY3,      MC_ENGINE_IDX_CE3},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY4,      MC_ENGINE_IDX_CE4},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY5,      MC_ENGINE_IDX_CE5},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY6,      MC_ENGINE_IDX_CE6},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY7,      MC_ENGINE_IDX_CE7},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY8,      MC_ENGINE_IDX_CE8},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY9,      MC_ENGINE_IDX_CE9},
        // removal tracking bug: 3748354
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY10,     MC_ENGINE_IDX_CE10},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY11,     MC_ENGINE_IDX_CE11},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY12,     MC_ENGINE_IDX_CE12},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY13,     MC_ENGINE_IDX_CE13},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY14,     MC_ENGINE_IDX_CE14},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY15,     MC_ENGINE_IDX_CE15},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY16,     MC_ENGINE_IDX_CE16},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY17,     MC_ENGINE_IDX_CE17},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY18,     MC_ENGINE_IDX_CE18},
        {NV2080_ENGINE_TYPE_COMP_DECOMP_COPY19,     MC_ENGINE_IDX_CE19},
    };

    //
    // To trap NV2080_ENGINE_TYPE expansions.
    // Please update the table guestEngineLookupTable if this assertion is triggered.
    //
    ct_assert(NV2080_ENGINE_TYPE_LAST == 0x00000054);

    *pEngLookupTblSize = NV_ARRAY_ELEMENTS(guestEngineLookupTable);

    return guestEngineLookupTable;
};

/**
 * @brief Fetch the maximum number of secure channels supported by SEC2
 *        and secure CEs when confidential compute is enabled
 *
 */
NV_STATUS
kfifoGetMaxSecureChannels_KERNEL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS numSecureChannelsParams = {0};

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS,
                            &numSecureChannelsParams,
                            sizeof(numSecureChannelsParams)));
    }

    pKernelFifo->maxSec2SecureChannels = numSecureChannelsParams.maxSec2SecureChannels;
    pKernelFifo->maxCeSecureChannels = numSecureChannelsParams.maxCeSecureChannels;

    return NV_OK;
}

/**
 * @brief Checking if the engine ID belongs to a PBDMA or not
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineId
 *
 * @return TRUE if engine ID belongs to a PBDMA
 */
NvBool
kfifoIsMmuFaultEngineIdPbdma_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineId
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);

    NV_ASSERT_OR_RETURN(pEngineInfo != NULL, NV_FALSE);
    return bitVectorTest(&pEngineInfo->validEngineIdsForPbdmas, engineId);
}

/**
 * @brief Function to get PBDMA ID from the given MMU falut ID
  *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] mmuFaultId
 * @param[out] pPbdmaId
 */
NV_STATUS
kfifoGetPbdmaIdFromMmuFaultId_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       mmuFaultId,
    NvU32      *pPbdmaId
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 pbdmaFaultIdStart;

    // This function relies on pKernelFifo->bIsPbdmaMmuEngineIdContiguous to be set.
    NV_ASSERT_OR_RETURN(pKernelFifo->bIsPbdmaMmuEngineIdContiguous, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pEngineInfo != NULL, NV_ERR_INVALID_STATE);

    //
    // HW guarantees mmu fault engine ids used for PBDMAs will be assigned in sequential order
    // "PBDMA of MMU_ENGINE_ID = MMU_ENGINE_ID - BASE_PBDMA_FAULT_ID" relation holds for all assignments
    // This is helping SW to derive the pbdma id using base pbdma fault id and mmu fault engine id
    //
    pbdmaFaultIdStart = bitVectorCountTrailingZeros(&pEngineInfo->validEngineIdsForPbdmas);
    *pPbdmaId = mmuFaultId - pbdmaFaultIdStart;

    return NV_OK;
}

/*!
 * @brief Function to get RM engine type from the given pbdma falut id
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[in]  pbdmaFaultId
 * @param[out] pRmEngineType
 *
 * @returns NV_OK when engine type found for given pbdma falut id
 */
NV_STATUS
kfifoGetEngineTypeFromPbdmaFaultId_IMPL
(
    OBJGPU          *pGpu,
    KernelFifo      *pKernelFifo,
    NvU32            pbdmaFaultId,
    RM_ENGINE_TYPE  *pRmEngineType
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 i, j;

    if (!pKernelFifo->bIsPbdmaMmuEngineIdContiguous)
    {
        NvU32  maxSubctx;
        NvU32  baseGrFaultId;
        NvU32 *pPbdmaFaultIds;
        NvU32  numPbdma = 0;

         NV_ASSERT_OK_OR_RETURN(kfifoGetEnginePbdmaFaultIds_HAL(pGpu, pKernelFifo,
                                            ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)RM_ENGINE_TYPE_GR0,
                                            &pPbdmaFaultIds, &numPbdma));

        baseGrFaultId = pPbdmaFaultIds[0];
        maxSubctx = kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, NV_FALSE);

        if ((pbdmaFaultId >= baseGrFaultId) && (pbdmaFaultId < (baseGrFaultId + maxSubctx)))
        {
            // We need extra logic when SMC is enabled
            if (IS_MIG_IN_USE(pGpu))
            {
                KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
                NvU32 grIdx;
                NvU32 subctxId;

                subctxId = pbdmaFaultId - baseGrFaultId;
                NV_ASSERT_OK_OR_RETURN(kgrmgrGetGrIdxForVeid(pGpu, pKernelGraphicsManager, subctxId, &grIdx));
                *pRmEngineType = RM_ENGINE_TYPE_GR(grIdx);
            }
            else
            {
                *pRmEngineType = RM_ENGINE_TYPE_GR(0);
            }

            return NV_OK;
        }
    }

    for (i = 0; i < pEngineInfo->engineInfoListSize; i++)
    {
        for (j = 0; j < pEngineInfo->engineInfoList[i].numPbdmas; j++)
        {
            if (pbdmaFaultId == pEngineInfo->engineInfoList[i].pbdmaFaultIds[j])
            {
                *pRmEngineType = pEngineInfo->engineInfoList[i].engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE];
                return NV_OK;
            }
        }
    }

    *pRmEngineType = RM_ENGINE_TYPE_NULL;
    return NV_ERR_OBJECT_NOT_FOUND;
}

