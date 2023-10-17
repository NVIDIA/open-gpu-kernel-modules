/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/fbsr.h"
#include "gpu/bus/kern_bus.h"
#include "vgpu/rpc.h"
#include "os/nv_memory_type.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/gsp/gsp_static_config.h"

static NV_STATUS _memmgrWalkHeap(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJFBSR *pFbsr);
static NV_STATUS _memmgrAllocFbsrReservedRanges(OBJGPU *pGpu, MemoryManager *pMemoryManager);

/*!
 * Save video memory
 */
NV_STATUS
memmgrSavePowerMgmtState_KERNEL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32     fbsrStartMode = pMemoryManager->fbsrStartMode;
    NvU32     i;
    OBJFBSR  *pFbsr;
    NV_STATUS rmStatus = NV_OK;

    if (pMemoryManager->pActiveFbsr)
    {
        NV_PRINTF(LEVEL_ERROR, "!!!!! Calling Suspend on a suspended GPU or ");
        NV_PRINTF(LEVEL_ERROR,
                  "the previous Resume call might have failed !!!!!!\n");
        NV_PRINTF(LEVEL_ERROR, "!!!!! Trying a suspend anyway !!!!!!\n");
    }

    //
    // N.B. there are multiple mechanisms available for this operation. We start
    // with the highest performance approach -- DMA -- and if this fails we fallback
    // to the slower but less intensive CPU approach. A reason why the DMA approach
    // might fail is not enough non-paged memory available.
    //
    pMemoryManager->pActiveFbsr = NULL;

    //
    // Iterate the heap at top level to avoid inconsistent between
    // several pFbsr instance.
    //

    rmStatus = memmgrAddMemNodes(pGpu, pMemoryManager,
                                 pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING));

    if ((rmStatus == NV_OK) &&
        memmgrIsPmaInitialized(pMemoryManager))
    {
        Heap           *pHeap = GPU_GET_HEAP(pGpu);
        RANGELISTTYPE  *pSaveList = NULL;
        RANGELISTTYPE  *pSaveCurr;

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING))
        {
            rmStatus = pmaBuildAllocatedBlocksList(&pHeap->pmaObject, &pSaveList);
        }
        else
        {
            rmStatus = pmaBuildPersistentList(&pHeap->pmaObject, &pSaveList);
        }

        if (rmStatus == NV_OK)
        {
            pSaveCurr = pSaveList;
            while (pSaveCurr)
            {
                MEMORY_DESCRIPTOR  *pMemDescPma;

                // Create memdescs & describe the block
                rmStatus = memdescCreate(&pMemDescPma, pGpu, RM_PAGE_SIZE, PMA_GRANULARITY, NV_TRUE, ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
                if (rmStatus != NV_OK)
                {
                    break;
                }
                memdescDescribe(pMemDescPma, ADDR_FBMEM, pSaveCurr->base, pSaveCurr->limit - pSaveCurr->base + 1);

                // Memory descriptor will be freed during restore
                rmStatus = memmgrAddMemNode(pGpu, pMemoryManager, pMemDescPma, NV_TRUE);
                if (rmStatus != NV_OK)
                {
                    break;
                }

                pSaveCurr = pSaveCurr->pNext;
            }
        }

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING))
        {
            pmaFreeAllocatedBlocksList(&pHeap->pmaObject, &pSaveList);
        }
        else
        {
            pmaFreePersistentList(&pHeap->pmaObject, &pSaveList);
        }
    }

    if (rmStatus != NV_OK)
    {
        memmgrRemoveMemNodes(pGpu, pMemoryManager);
        return rmStatus;
    }

    for (i = fbsrStartMode; i < NUM_FBSR_TYPES; i++)
    {
        if ((pMemoryManager->fixedFbsrModesMask != 0) &&
             !(NVBIT(i) & pMemoryManager->fixedFbsrModesMask))
        {
            continue;
        }

        pFbsr = pMemoryManager->pFbsr[i];

        if (!pFbsr)
        {
            continue;
        }

        if (!pFbsr->bInitialized)
        {
            continue;
        }

        //
        // Determine size of buffer needed
        //
        if (fbsrBegin_HAL(pGpu, pFbsr, FBSR_OP_SIZE_BUF) != NV_OK)
        {
            continue;
        }

        if (_memmgrWalkHeap(pGpu, pMemoryManager, pFbsr) != NV_OK)
        {
            continue;
        }

        if (fbsrEnd_HAL(pGpu, pFbsr) != NV_OK)
        {
            continue;
        }

        //
        // Perform the copy
        //
        if (fbsrBegin_HAL(pGpu, pFbsr, FBSR_OP_SAVE) != NV_OK)
        {
            continue;
        }

        if (_memmgrWalkHeap(pGpu, pMemoryManager, pFbsr) != NV_OK)
        {
            continue;
        }

        if (fbsrEnd_HAL(pGpu, pFbsr) != NV_OK)
        {
            continue;
        }

        pFbsr->bValid = NV_TRUE;
        pMemoryManager->pActiveFbsr = pFbsr; // Restore will restore from active fbsr.
        break;
    }

    if (rmStatus != NV_OK)
    {
        memmgrFreeFbsrMemory_HAL(pGpu, pMemoryManager);
    }

    return rmStatus;
}

/*!
 * Restore video memory
 */
static volatile NvS32 concurrentfbRestorePowerMgmtStateAccess[NV_MAX_DEVICES] = { 0 };
NV_STATUS
memmgrRestorePowerMgmtState_KERNEL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    OBJFBSR     *pFbsr         = pMemoryManager->pActiveFbsr;
    NV_STATUS    status        = NV_OK;
    NvBool       bIsGpuLost    = NV_FALSE;

    if (portAtomicIncrementS32(&concurrentfbRestorePowerMgmtStateAccess[pGpu->gpuInstance]) != 1)
    {
        NV_PRINTF(LEVEL_INFO, "Concurrent access");
        return NV_OK;
    }

    if (!pFbsr || !pFbsr->bValid)
    {
        NV_PRINTF(LEVEL_ERROR, "!!!!! Calling Resume on an active GPU or "
                  "the previous Suspend call might have failed !!!!!!\n");
        NV_PRINTF(LEVEL_ERROR,
                  "!!!!! So ignoring the resume request !!!!!!\n");
        return NV_ERR_INVALID_STATE;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    {
        bIsGpuLost = NV_TRUE;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        fbsrBegin_HAL(pGpu, pFbsr, bIsGpuLost ? FBSR_OP_DESTROY : FBSR_OP_RESTORE),
                        done);
    if (!bIsGpuLost)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                            _memmgrWalkHeap(pGpu, pMemoryManager, pFbsr), done);
    }
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, fbsrEnd_HAL(pGpu, pFbsr), done);

done:

    // Cleanup
    memmgrFreeFbsrMemory_HAL(pGpu, pMemoryManager);
    pFbsr->bValid = NV_FALSE;

    portAtomicDecrementS32(&concurrentfbRestorePowerMgmtStateAccess[pGpu->gpuInstance]);

    return status;
}

/*
 * FB regions with LostOnSuspend set to NV_TRUE are expected to be grouped together towards the end of FB
 * First region to have this property set would be the end of save area for Suspend.
 * TODO: Handle LostOnSuspend for non-consequetive regions
 */
static NvU64
_memmgrGetFbEndExcludingLostOnSuspendRegions
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32 i = 0;

    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        if (pMemoryManager->Ram.fbRegion[i].bLostOnSuspend == NV_TRUE)
        {
            return pMemoryManager->Ram.fbRegion[i].base;
        }
    }

    return (pMemoryManager->Ram.fbAddrSpaceSizeMb << 20);
}

static NV_STATUS
_memmgrAllocFbsrReservedRanges
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64      size       = 0;
    NV_STATUS  status     = NV_OK;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    // Alloc the Memory descriptors for Fbsr Reserved regions, if not allocated.
    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] == NULL)
    {
        if(IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            // Allocate Vid Mem descriptor for RM INSTANCE memory from start to BAR2 PDE base.
            size = memdescGetPhysAddr(pKernelBus->virtualBar2[GPU_GFID_PF].pPageLevelsMemDesc, AT_GPU, 0) - pMemoryManager->rsvdMemoryBase;
        }
        else
        {
             // Allocate Vid Mem descriptor for RM INSTANCE memory from start to BAR2PTE
            size = memdescGetPhysAddr(pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc, AT_GPU, 0) - pMemoryManager->rsvdMemoryBase;
        }

        NV_ASSERT_OK_OR_GOTO(status,
                             memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE],
                                            pGpu, size, 0, NV_TRUE, ADDR_FBMEM,
                                            NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                             fail);
        // Describe the MemDescriptor for RM Instance Memory from start to BAR2PTE
        memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE],
                        ADDR_FBMEM, pMemoryManager->rsvdMemoryBase, size);
    }

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE] == NULL)
    {
        RmPhysAddr afterBar2PteRegionStart = 0;
        NvU64 afterBar2PteRegionSize = 0;

        /*
         * Allocate Mem descriptors for AFTER_BAR2PTE, GSP HEAP, WPR, NON WPR and VGA Workspace regions.
         */
        if (IS_GSP_CLIENT(pGpu))
        {
            KernelGsp *pKernelGsp              = GPU_GET_KERNEL_GSP(pGpu);
            NvU64      afterBar2PteRegionEnd   = 0;
            afterBar2PteRegionStart            = memdescGetPhysAddr(pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc, AT_GPU, 0) +
                                                 pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc->Size;
            afterBar2PteRegionEnd              = pMemoryManager->rsvdMemoryBase + pMemoryManager->rsvdMemorySize;
            afterBar2PteRegionSize             = afterBar2PteRegionEnd - afterBar2PteRegionStart;
            NvU64      gspHeapRegionStart      = afterBar2PteRegionEnd;
            NvU64      gspHeapRegionSize       = pKernelGsp->pWprMeta->gspFwRsvdStart - gspHeapRegionStart;
            NvU64      gspNonWprRegionSize     = pKernelGsp->pWprMeta->gspFwWprStart  - pKernelGsp->pWprMeta->gspFwRsvdStart;

            // Create memdesc of AFTER BAR2PTE, GSP HEAP, GSP NON WPR and VGA Workspace regions
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                                                pGpu, afterBar2PteRegionSize, 0, NV_TRUE,
                                                ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP],
                                                pGpu, gspHeapRegionSize, 0, NV_TRUE,
                                                ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR],
                                                pGpu, gspNonWprRegionSize, 0, NV_TRUE,
                                                ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE],
                                                pGpu, pKernelGsp->pWprMeta->vgaWorkspaceSize, 0, NV_TRUE,
                                                ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);

            // Describe memdesc of AFTER BAR2PTE, GSP HEAP, GSP NON WPR and VGA Workspace regions
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                            ADDR_FBMEM,
                            afterBar2PteRegionStart,
                            afterBar2PteRegionSize);
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP],
                            ADDR_FBMEM,
                            gspHeapRegionStart,
                            gspHeapRegionSize);
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR],
                            ADDR_FBMEM,
                            pKernelGsp->pWprMeta->gspFwRsvdStart,
                            gspNonWprRegionSize);
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE],
                            ADDR_FBMEM,
                            pKernelGsp->pWprMeta->vgaWorkspaceOffset,
                            pKernelGsp->pWprMeta->vgaWorkspaceSize);

        }
        // Allocate Vid Mem descriptor for RM INSTANCE memory, specific to VGA  i.e. after BAR2PTE to end.
        else
        {
            NvU64 fbAddrSpaceSize   = _memmgrGetFbEndExcludingLostOnSuspendRegions(pGpu, pMemoryManager);

            if (IS_VIRTUAL_WITH_SRIOV(pGpu))
            {
                /*
                 * From BAR2 region we skip BAR2 PDEs and CPU visible region PTEs as we rebuild them on restore.
                 * But we need to save CPU invisible region PTEs across S/R, hence AFTER_BAR2PTE range starts
                 * after CPU visible region PTEs ends.
                 */
                afterBar2PteRegionStart = pKernelBus->bar2[GPU_GFID_PF].pteBase +
                                          pKernelBus->bar2[GPU_GFID_PF].cpuVisiblePgTblSize;
            }
            else
            {
                afterBar2PteRegionStart = memdescGetPhysAddr(pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc, AT_GPU, 0) +
                                          pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc->Size;
            }

            afterBar2PteRegionSize  = fbAddrSpaceSize - afterBar2PteRegionStart;

            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                                               pGpu, afterBar2PteRegionSize, 0, NV_TRUE, ADDR_FBMEM,
                                               NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                            ADDR_FBMEM,
                            afterBar2PteRegionStart, afterBar2PteRegionSize);

        }
    }
    return status;

fail:
    NV_PRINTF(LEVEL_ERROR, "Failure during allocation of FBSR Reserved ranges: %d\n", status);

    // Cleanup fbsrReservedRanges
    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE]);

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]);

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR]);

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_WPR] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_WPR]);

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE] != NULL)
        memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE]);

    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]  = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]       = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR]    = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_WPR]        = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE]  = NULL;

    return status;
}

/*!
 * Accessor for list of video memory regions that need to be preserved. This
 * routine calls POBJFBSR::fbsrCopyMemory[|MemDesc] per-region.
 *
 * @param[in]   pFbsr     OBJFBSR pointer
 *
 * @returns None
 */
static NV_STATUS
_memmgrWalkHeap(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJFBSR *pFbsr)
{
    KernelBus   *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    FB_MEM_NODE *pMemNode   = pMemoryManager->pMemHeadNode;
    NvU64        size       = 0;
    NV_STATUS    status     = NV_OK;

    //
    // Walk the heap and preserve any allocations marked as non-volatile. N.B. if
    // walking this list turns out to be expensive the heap could be changed to keep
    // a separate list for preserved allocations.
    //
    while (pMemNode)
    {
        fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr, pMemNode->pMemDesc);
        pMemNode = pMemNode->pNext;
    }

    // Save reserved memory region
    if (memdescGetAddressSpace(pKernelBus->virtualBar2[GPU_GFID_PF].pPTEMemDesc) == ADDR_FBMEM)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                            _memmgrAllocFbsrReservedRanges(pGpu, pMemoryManager), done);

        fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                  pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE]);
        fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                  pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]);

        if (IS_GSP_CLIENT(pGpu))
        {
            //
            // FBSR_OP_SIZE_BUF increments pFbsr->numRegions and updates pFbsr->Length to add sysmem for allocations
            // FBSR_OP_SAVE sends memdesc info to GSP for CE save
            //
            // Handle both FBSR_OP_SIZE_BUF and FBSR_OP_SAVE for GSP NON WPR regions
            // to allocate sysmem for the regions and to use CE to save/restore the region
            //
            fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                      pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR]);

            //
            // Handle only FBSR_OP_SIZE_BUF for GSP HEAP, GSP WPR and VGA workspace regions
            // to allocate sysmem for these regions
            // Use Bus copy to save/restore the GSP WPR region if allocated
            //
            if (pFbsr->op == FBSR_OP_SIZE_BUF)
            {
                // SysOffset for GSP memory allocations is the AFTER BAR2PTE region's sysoffset
                pFbsr->gspFbAllocsSysOffset = pFbsr->length;

                fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                          pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);
                fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                          pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE]);
                pFbsr->numRegions -= 2;

                // GSP WPR memdesc is not NULL when using unauthenticated GSP boot and bus copy
                if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_WPR] != NULL)
                {
                    fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                              pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_WPR]);
                    pFbsr->numRegions--;
                }
            }

        }
    }
    else
    {
        // Alloc the Memory descriptors for Fbsr Reserved regions, if not allocated.
        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] == NULL)
        {
            // Allocate Vid Mem descriptor for RM INSTANCE memory from start to BAR2PTE
            size = pMemoryManager->rsvdMemorySize;
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE],
                                               pGpu, size, 0, NV_TRUE, ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                done);

            // Describe the MemDescriptor for RM Instance Memory from start to BAR2PTE
            memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE],
                            ADDR_FBMEM, pMemoryManager->rsvdMemoryBase, size);
        }

        // Save all reserved memory
        fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr,
                                  pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE]);
    }
done:
    return status;
}

/*!
 *  @brief Construct memory descriptor list
 *
 *  @param[in]  pGpu            GPU object pointer
 *  @param[in]  bFreeDescriptor NV_TRUE to free descriptor when done
 *
 * @returns     NV_OK on success.
 *              Error otherwise.
 */
NV_STATUS
memmgrAddMemNode_IMPL
(
    OBJGPU            *pGpu,
    MemoryManager     *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool             bFreeDescriptor
)
{
    FB_MEM_NODE *pNode = NULL;
    NV_STATUS    rmStatus = NV_OK;

    pNode = portMemAllocNonPaged(sizeof(FB_MEM_NODE));
    if (pNode == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_WARNING, "can't allocate FB_MEM_NODE, err:0x%x!\n", rmStatus);
        NV_ASSERT_OK_OR_RETURN(rmStatus);
    }

    pNode->pNext = NULL;
    pNode->pMemDesc = pMemDesc;
    pNode->bFreeDescriptor = bFreeDescriptor;
    //
    // Hold this memory descriptor till we fully restore all GPUs.
    // This is needed for a few RM allocations to prevent them from
    // being freed after adding them to this list
    // Note: Allocated flag is set only for RM internal allocations.
    //
    if (pMemDesc->Allocated)
    {
        memdescAddRef(pMemDesc);
        pMemDesc->Allocated++;
    }

    if (pMemoryManager->pMemHeadNode == NULL)
    {
        pMemoryManager->pMemHeadNode = pNode;
        pMemoryManager->pMemTailNode = pNode;
    }
    else
    {
        pMemoryManager->pMemTailNode->pNext = pNode;
        pMemoryManager->pMemTailNode = pNode;
    }

    return rmStatus;
}

NvBool
memmgrIsGspOwnedMemory_KERNEL
(
    OBJGPU            *pGpu,
    MemoryManager     *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc
)
{

    GspStaticConfigInfo *pGSCI        = GPU_GET_GSP_STATIC_INFO(pGpu);
    RmPhysAddr           physAddr     = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    NvU64                fbRegionSize = 0;

    NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS    *pFbRegionInfoParams = &pGSCI->fbRegionInfoParams;
    NV2080_CTRL_CMD_FB_GET_FB_REGION_FB_REGION_INFO *pFbRegionInfo       = NULL;

    // Return NV_TRUE if input MEMORY_DESCRIPTOR corresponds to any GSP managed regions
    for (NvU32 i = 0; i < pFbRegionInfoParams->numFBRegions; i++)
    {
        pFbRegionInfo = &pFbRegionInfoParams->fbRegion[i];
        fbRegionSize  = pFbRegionInfo->limit - pFbRegionInfo->base + 1;

        if ((pFbRegionInfo->base == physAddr) && (fbRegionSize == pMemDesc->Size))
        {
            NV_PRINTF(LEVEL_INFO,
                      "Skipping GSP FB Region with addr 0x%llx and size 0x%llx\n",
                      physAddr, pMemDesc->Size);
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NV_STATUS
memmgrAddMemNodes_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvBool         bSaveAllRmAllocations
)
{
    NV_STATUS           status    = NV_OK;
    Heap               *pHeap     = GPU_GET_HEAP(pGpu);
    MEMORY_DESCRIPTOR  *pAllocMemDesc;
    MEM_BLOCK          *block;
    NvBool              bSaveNode = NV_FALSE;

    block = pHeap->pBlockList;
    do
    {
        pAllocMemDesc = block->pMemDesc;
        bSaveNode     = NV_FALSE;

        //
        // TODO: Bug 1778161: Let memory descriptor lost on suspend be default,
        // use MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND explicitly to preserve
        // content of memory descriptor, remove MEMDESC_FLAGS_LOST_ON_SUSPEND
        // flag.
        //
        if (pAllocMemDesc != NULL)
        {
            //
            // Save RM_RESERVED_REGION if it is not GSP managed FB region and
            // - bSaveRmAllocations true or
            // - MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND is set
            // TODO: Use LOST_ON_SUSPEND flag to skip GSP managed FB regions
            //
            if  ((block->owner == HEAP_OWNER_RM_RESERVED_REGION) &&
                 !memmgrIsGspOwnedMemory_HAL(pGpu, pMemoryManager, pAllocMemDesc))
            {
                if (bSaveAllRmAllocations || memdescGetFlag(pAllocMemDesc, MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND))
                    bSaveNode = NV_TRUE;
            }
            //
            // Save RM_CHANNEL_CTX_BUFFER or RM_KERNEL_CLIENT regions if
            // - bSaveRmAllocations true or
            // - MEMDESC_FLAGS_PRESERVE_LOST_ON_SUSPEND is not set
            //
            else if ((block->owner == HEAP_OWNER_RM_CHANNEL_CTX_BUFFER) || (block->owner == HEAP_OWNER_RM_KERNEL_CLIENT))
            {
                if (bSaveAllRmAllocations || (!memdescGetFlag(pAllocMemDesc, MEMDESC_FLAGS_LOST_ON_SUSPEND)))
                    bSaveNode = NV_TRUE;
            }

            if (bSaveNode &&
                ((memdescGetAddressSpace(pAllocMemDesc) == ADDR_FBMEM) ||
                 (memdescGetAddressSpace(pAllocMemDesc) == ADDR_SYSMEM)))
            {
                NV_CHECK_OK_OR_GOTO(status,
                                    LEVEL_ERROR,
                                    memmgrAddMemNode(pGpu, pMemoryManager, pAllocMemDesc, NV_FALSE),
                                    done);
            }
        }

        block = block->next;
    } while (block != pHeap->pBlockList);

done:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failure during memmgrAddMemNodes: %d\n", status);
        memmgrRemoveMemNodes(pGpu, pMemoryManager);
    }

    return status;
}

void memmgrRemoveMemNodes_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    FB_MEM_NODE *pMemNode    = pMemoryManager->pMemHeadNode;
    FB_MEM_NODE *pMemNodeTmp = NULL;

    while (pMemNode)
    {
        pMemNodeTmp = pMemNode;
        pMemNode = pMemNode->pNext;
        //
        // We hold a refcount on saved RM allocations during suspend
        // to prevent them from being freed till resume completes
        // Free such allocations now to avoid leaks
        //
        if (pMemNodeTmp->pMemDesc->Allocated)
        {
            memdescFree(pMemNodeTmp->pMemDesc);
            memdescDestroy(pMemNodeTmp->pMemDesc);
        }
        else if (pMemNodeTmp->bFreeDescriptor)
        {
            // Temporary descriptor -- free it
            memdescDestroy(pMemNodeTmp->pMemDesc);
        }
        portMemFree(pMemNodeTmp);
    }

    pMemoryManager->pMemHeadNode = NULL;
    pMemoryManager->pMemTailNode = NULL;

    return;
}

void
memmgrFreeFbsrMemory_KERNEL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    OBJFBSR *pFbsr = pMemoryManager->pActiveFbsr;

    // Cleanup memory list
    memmgrRemoveMemNodes(pGpu, pMemoryManager);

    // Free sysmem allocated for S/R
    if ((pFbsr != NULL) && (pFbsr->pSysMemDesc != NULL))
    {
        //
        // Free memory only for FBSR_TYPE_DMA and FBSR_TYPE_PAGED_DMA
        // Other types like FBSR_TYPE_CPU rely on fixed sysmem allocation
        // done during fbsrInit and require its sysmem to be freed
        // only during fbsrDestroy
        //
        if ((pFbsr->type == FBSR_TYPE_PAGED_DMA) ||
            (pFbsr->type == FBSR_TYPE_DMA))
        {
            memdescFree(pFbsr->pSysMemDesc);
            memdescDestroy(pFbsr->pSysMemDesc);
            pFbsr->pSysMemDesc = NULL;
        }
    }

    pMemoryManager->pActiveFbsr = NULL;
}
