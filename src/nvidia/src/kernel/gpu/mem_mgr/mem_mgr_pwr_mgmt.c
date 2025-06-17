/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_sys/kern_mem_sys.h"
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

static NV_STATUS
_memmgrAllocFbsrReservedRanges
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS  status     = NV_OK;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU64      rsvdMemEnd = pMemoryManager->rsvdMemoryBase + pMemoryManager->rsvdMemorySize;
    // We need to skip BAR2 CPU visible range during restore, as it needs to be up before allocations are restored
    NvU64 bar2CpuVisiblePteStart = pKernelBus->bar2[GPU_GFID_PF].pteBase;
    NvU64 bar2CpuVisiblePteEnd   = bar2CpuVisiblePteStart + pKernelBus->bar2[GPU_GFID_PF].cpuVisiblePgTblSize;

    // Alloc the Memory descriptors for Fbsr Reserved regions, if not allocated.
    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] == NULL)
    {
        NvU64 size = bar2CpuVisiblePteStart - pMemoryManager->rsvdMemoryBase;

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
        NvU64 size = rsvdMemEnd - bar2CpuVisiblePteEnd;

        NV_ASSERT_OK_OR_GOTO(status,
                             memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                                           pGpu, size, 0, NV_TRUE, ADDR_FBMEM,
                                           NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                             fail);
        memdescDescribe(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE],
                        ADDR_FBMEM,
                        bar2CpuVisiblePteEnd, size);
    }

    if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP] == NULL)
    {
        /*
         * Allocate memdesc for AFTER_BAR2PTE and GSP WPR regions.
         * Allocate sysmem to save GSP allocations
         */
        if (IS_GSP_CLIENT(pGpu))
        {
            KernelGsp    *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
            GspFwWprMeta *pWprMeta   = pKernelGsp->pWprMeta;

            /*
             * Calculate sysmem required to save all GSP allocations
             * This is sum of GSP Heap, GSP non WPR and VGA workspace regions
             * This will also include CBC region if the corresponding flag is set
             *
             * SYSMEM required for GSP allocations will be created and allocated
             * into fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]
             *
             * TODO: Query GSP for size of its allocation instead of this calculation
             */
            NvU64 size = ((pWprMeta->nonWprHeapOffset + pWprMeta->nonWprHeapSize) - rsvdMemEnd) + // GSP Heap start to end of NON WPR region
                         pWprMeta->vgaWorkspaceSize;                                              // VGA Workspace

            // Check if CBC region needs to be saved
            if (GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->bPreserveComptagBackingStoreOnSuspend ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED))
            {
                NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS compbitStoreInfoParams;
                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

                // GET CBC region info from GSP
                NV_ASSERT_OK_OR_GOTO(status,
                                     pRmApi->Control(pRmApi,
                                                     pGpu->hInternalClient,
                                                     pGpu->hInternalDevice,
                                                     NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO,
                                                     &compbitStoreInfoParams,
                                                     sizeof(compbitStoreInfoParams)),
                                     fail);

                size += compbitStoreInfoParams.Size;
            }

            // Create memdesc to save GSP allocations
            NV_ASSERT_OK_OR_GOTO(status,
                                 memdescCreate(&pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP],
                                                pGpu, size, 0, NV_FALSE,
                                                ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
                                 fail);

            // Allocate sysmem
            status = memdescAlloc(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);
            if (status != NV_OK)
            {
                NV_ASSERT(status == NV_OK);
                memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);
                pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP] = NULL;
                goto fail;
            }

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

    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]  = NULL;
    pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]       = NULL;

    return status;
}

/*!
 * Accessor for list of video memory regions that need to be preserved. This
 * routine calls fbsrCopyMemory[|MemDesc] per-region.
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
        if (block->owner != NVOS32_BLOCK_TYPE_FREE && pAllocMemDesc != NULL)
        {
            NV_PRINTF(LEVEL_INFO,
                      "pAllocMemDesc base 0x%llx (size 0x%llx) block owner 0x%X memdesc flags 0x%llx\n",
                      memdescGetPhysAddr(pAllocMemDesc, AT_GPU, 0),
                      pAllocMemDesc->Size,
                      block->owner,
                      pAllocMemDesc->_flags);

            if (memdescGetFlag(pAllocMemDesc, MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND))
            {
                bSaveNode = NV_TRUE;
            }
            else if  (block->owner == HEAP_OWNER_RM_RESERVED_REGION)
            {
                if (bSaveAllRmAllocations && (!memdescGetFlag(pAllocMemDesc, MEMDESC_FLAGS_LOST_ON_SUSPEND)))
                    bSaveNode = NV_TRUE;
            }
            else if ((block->owner == HEAP_OWNER_RM_CHANNEL_CTX_BUFFER) ||
                     (block->owner == HEAP_OWNER_RM_KERNEL_CLIENT))
            {
                if (bSaveAllRmAllocations || (!memdescGetFlag(pAllocMemDesc, MEMDESC_FLAGS_LOST_ON_SUSPEND)))
                    bSaveNode = NV_TRUE;
            }

            if (bSaveNode &&
                ((memdescGetAddressSpace(pAllocMemDesc) == ADDR_FBMEM) ||
                 (memdescGetAddressSpace(pAllocMemDesc) == ADDR_SYSMEM)))
            {
                NV_PRINTF(LEVEL_INFO, "pAllocMemDesc being saved\n");
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
