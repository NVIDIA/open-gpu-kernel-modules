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

/*!
 * @file
 * @brief Standard local frame buffer allocation and management routines
 */

#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "mem_mgr/video_mem.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/system_mem.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "vgpu/rpc.h"
#include "vgpu/vgpu_util.h"
#include "gpu/mmu/kern_gmmu.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/device/device.h"
#include "kernel/gpu/intr/intr.h"
#include "platform/sli/sli.h"

typedef enum
{
    BLOCK_ADD,
    BLOCK_REMOVE,
    BLOCK_SIZE_CHANGED,
    BLOCK_FREE_STATE_CHANGED,
} BlockAction;

//
// Statics
//
static NV_STATUS _heapBlockFree(OBJGPU *, Heap *, NvHandle, NvHandle, MEM_BLOCK *);
static void      _heapSetTexturePlacement(Heap *, NvU32, NvU32, NvBool*,
                                          NvU32*, NvU8*);
static NV_STATUS _heapGetMaxFree(Heap *, NvU64 *, NvU64 *);
static NV_STATUS _heapGetBankPlacement(OBJGPU *, Heap *, NvU32,
                                       NvU32 *, NvU32, NvU32, NvU32 *);
static MEM_BLOCK *_heapFindAlignedBlockWithOwner(OBJGPU *, Heap *, NvU32,
                                                NvU64/* aligned*/);
static NV_STATUS _heapProcessFreeBlock(OBJGPU *, MEM_BLOCK *, MEM_BLOCK **,
                                       MEM_BLOCK **, Heap *,
                                       MEMORY_ALLOCATION_REQUEST *,
                                       NvHandle, OBJHEAP_ALLOC_DATA *,
                                       FB_ALLOC_INFO *, NvU64, NvU64 *);
static void _heapAddBlockToNoncontigList(Heap *, MEM_BLOCK *);
static void _heapRemoveBlockFromNoncontigList(Heap *, MEM_BLOCK *);
static NV_STATUS _heapFindBlockByOffset(OBJGPU *, Heap *, NvU32,
                                        MEMORY_DESCRIPTOR *, NvU64,
                                        MEM_BLOCK **);
static NV_STATUS _heapAllocNoncontig(OBJGPU *, NvHandle, Heap *,
                                     MEMORY_ALLOCATION_REQUEST *, NvHandle,
                                     OBJHEAP_ALLOC_DATA *, FB_ALLOC_INFO *,
                                     NvU32, NvU64, NvU64 *, MEMORY_DESCRIPTOR *,
                                     HWRESOURCE_INFO **);
static NV_STATUS _heapUpdate(Heap *, MEM_BLOCK *, BlockAction);
static void _heapAdjustFree(Heap *pHeap, NvS64 blockSize);
static void _heapBlacklistChunksInFreeBlocks(OBJGPU *, Heap *);

#ifdef DEBUG

/****************************************************************************/
/*                                                                          */
/*                             DEBUG support!                               */
/*                                                                          */
/****************************************************************************/

NvU32 dbgDumpHeap = 0;
NvU32 dbgReverseDumpHeap = 0;

static void      _heapDump(Heap *);
static void      _heapValidate(Heap *);

#define HEAP_VALIDATE(h)    {_heapValidate(h);if(dbgDumpHeap)_heapDump(h);}

static void ConvertOwnerToString(NvU32 owner, char *string)
{
    int i;
    string[0] = (unsigned char)((owner >> 24));
    string[1] = (unsigned char)((owner >> 16) & 0xFF);
    string[2] = (unsigned char)((owner >>  8) & 0xFF);
    string[3] = (unsigned char)((owner      ) & 0xFF);
    string[4] = 0;
    for (i=0 ;i<4; i++) {
        // Assuming ASCII these should be "safe" printable characters.
        if ((string[i] < ' ') || (string[i] > 0x7E)) {
            string[i] = '?';
        }
    }
}

static void _heapDump
(
    Heap *pHeap
)
{
    NvU64       free;
    MEM_BLOCK  *pBlock;
    char        ownerString[5];

    if (!pHeap) return;

    NV_PRINTF(LEVEL_INFO, "Heap dump.  Size = 0x%08llx\n", pHeap->total);
    NV_PRINTF(LEVEL_INFO, "            Free = 0x%08llx\n", pHeap->free);
    NV_PRINTF(LEVEL_INFO, "        Reserved = 0x%08llx\n", pHeap->reserved);
    NV_PRINTF(LEVEL_INFO,
              "=================================================================\n");
    NV_PRINTF(LEVEL_INFO,
              "\t\t    Begin         End         Size    \t Type     ResId      Owner"
              "   \"owns\"\n");
    NV_PRINTF(LEVEL_INFO, "Block List %s\n",
              dbgReverseDumpHeap ? "Reverse" : "Forward");
    pBlock = pHeap->pBlockList;
    do
    {
        if ( dbgReverseDumpHeap )
            pBlock = pBlock->prev;

        NV_PRINTF(LEVEL_INFO, "\t\t0x%08llx 0x%08llx 0x%08llx", pBlock->begin,
                  pBlock->end, 1 + (pBlock->end - pBlock->begin));

        if (pBlock->owner == NVOS32_BLOCK_TYPE_FREE) {
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\tFREE\n");
        }
        else
        {
            ConvertOwnerToString(pBlock->owner, ownerString);
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO,
                         "\t0x%04x 0x%08x \"%s\"\n", pBlock->u0.type,
                         pBlock->owner, ownerString);
        }

        if ( !dbgReverseDumpHeap )
            pBlock = pBlock->next;
    } while (pBlock != pHeap->pBlockList);

    NV_PRINTF(LEVEL_INFO, "FREE Block List %s\n",
              dbgReverseDumpHeap ? "Reverse" : "Forward");
    free  = 0;
    pBlock = pHeap->pFreeBlockList;
    if (pBlock)
        do
        {
            if ( dbgReverseDumpHeap )
                pBlock = pBlock->u0.prevFree;

            NV_PRINTF(LEVEL_INFO, "\t\t0x%08llx 0x%08llx 0x%08llx\tFREE\n",
                      pBlock->begin, pBlock->end,
                      1 + (pBlock->end - pBlock->begin));

            free += pBlock->end - pBlock->begin + 1;

            if ( !dbgReverseDumpHeap )
                pBlock = pBlock->u1.nextFree;
        } while (pBlock != pHeap->pFreeBlockList);

    NV_PRINTF(LEVEL_INFO, "\tCalculated free count = 0x%08llx\n", free);
}

static void _heapValidate
(
    Heap *pHeap
)
{
    MEM_BLOCK  *pBlock, *pBlockFree;
    NvU64       free, used;

    if (!pHeap) return;

    /*
     * Scan the blocks and check for consistency.
     */
    free      = 0;
    used      = 0;
    pBlock     = pHeap->pBlockList;
    pBlockFree = pHeap->pFreeBlockList;
    do
    {
        if (pBlock->owner == NVOS32_BLOCK_TYPE_FREE)
        {
            if (!pBlockFree)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Invalid free list with free blocks found.\n");
                _heapDump(pHeap);
                DBG_BREAKPOINT();
            }
            free += pBlock->end - pBlock->begin + 1;
            if (pBlock != pBlockFree)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Free list not consistent with block list.\n");
                _heapDump(pHeap);
                DBG_BREAKPOINT();
            }
            pBlockFree = pBlockFree->u1.nextFree;
        }
        else
        {
            used += pBlock->end - pBlock->begin + 1;
        }
        if (pBlock->next != pHeap->pBlockList)
        {
            if (pBlock->end != pBlock->next->begin - 1)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Hole between blocks at offset = 0x%llx\n",
                          pBlock->end);
                _heapDump(pHeap);
                DBG_BREAKPOINT();
            }
        }
        else
        {
            if (pBlock->end != pHeap->base + pHeap->total - 1)
            {
                NV_PRINTF(LEVEL_ERROR, "Last block doesn't end at top.\n");
                _heapDump(pHeap);
                DBG_BREAKPOINT();
            }
            if (pBlock->next->begin != pHeap->base)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "First block doesn't start at bottom.\n");
                _heapDump(pHeap);
                DBG_BREAKPOINT();
            }
        }
        if (pBlock->end < pBlock->begin)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Validate: Invalid block begin = 0x%08llx\n",
                      pBlock->begin);
            NV_PRINTF(LEVEL_ERROR,
                      "                        end   = 0x%08llx\n",
                      pBlock->end);
            _heapDump(pHeap);
            DBG_BREAKPOINT();
        }
        pBlock = pBlock->next;
    } while (pBlock != pHeap->pBlockList);
    if (free != pHeap->free)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Calculated free count (%llx) not consistent with heap free count (%llx).\n",
                  free, pHeap->free);
        _heapDump(pHeap);
        DBG_BREAKPOINT();
    }
    if ((used + free) > pHeap->total)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Calculated used count (%llx) not consistent with heap size (%llx).\n",
                  used + free, pHeap->total);
        _heapDump(pHeap);
        DBG_BREAKPOINT();
    }
}
#else
#define HEAP_VALIDATE(h)
#endif // DEBUG


/****************************************************************************/
/*                                                                          */
/*                             Heap Manager                                 */
/*                                                                          */
/****************************************************************************/

static NV_STATUS heapReserveRegion
(
    MemoryManager      *pMemoryManager,
    Heap               *pHeap,
    NvU64               offset,
    NvU64               size,
    MEMORY_DESCRIPTOR **ppMemDesc,
    NvBool              isRmRsvdRegion,
    NvBool              bProtected
)
{
    NV_STATUS                    rmStatus           = NV_OK;
    OBJGPU                      *pGpu               = ENG_GET_GPU(pMemoryManager);
    NvU64                        heapSize           = (pHeap->base + pHeap->total);
    FB_ALLOC_INFO               *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat = NULL;

    MEMORY_ALLOCATION_REQUEST allocRequest = {0};
    NV_MEMORY_ALLOCATION_PARAMS allocData = {0};

    NvU64 align = 0;
    NvU32 height = 1;
    NvU32 pitch = 1;
    NvU32 attr = DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
        DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS);
    NvU32 attr2 = DRF_DEF(OS32, _ATTR2, _INTERNAL, _YES);

    NV_ASSERT_OR_RETURN((offset < heapSize), NV_OK);

    allocRequest.pUserParams = &allocData;

    allocData.owner = ((isRmRsvdRegion) ? HEAP_OWNER_RM_RESERVED_REGION : HEAP_OWNER_PMA_RESERVED_REGION);
    allocData.height = height;
    allocData.type = ((isRmRsvdRegion) ?  NVOS32_TYPE_RESERVED : NVOS32_TYPE_PMA);
    allocData.flags = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    allocData.attr = attr;
    allocData.attr2 = attr2;
    allocData.pitch = pitch;
    allocData.alignment = align;
    allocData.size = NV_MIN(size, (heapSize - offset));
    allocData.offset = offset;

    if (bProtected)
        allocData.flags |= NVOS32_ALLOC_FLAGS_PROTECTED;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, done);

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, done);

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    memUtilsInitFBAllocInfo(&allocData, pFbAllocInfo, 0, 0);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        memmgrAllocResources(pGpu, pMemoryManager, &allocRequest, pFbAllocInfo),
        done);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        vidmemAllocResources(pGpu, pMemoryManager, &allocRequest, pFbAllocInfo, pHeap),
        done);

    NV_PRINTF(LEVEL_INFO, "Reserved heap for %s %llx..%llx\n",
              ((isRmRsvdRegion) ? "RM" : "PMA"), offset, (offset+size-1));

    *ppMemDesc = allocRequest.pMemDesc;

    // Account for reserved size removed from the total address space size
    if (isRmRsvdRegion)
    {
        pHeap->reserved += allocData.size;
    }

done:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return rmStatus;
}

/*!
 * @brief Initializes a heap object
 *
 * @param[in]     pFb           FB object ptr
 * @param[in/out] pHeap         HEAP object ptr
 * @param[in]     base          Base for this heap
 * @param[in]     size          Size of this heap
 * @param[in]     heapType      Heap type (Global or PMSA)
 * @param[in]     pPtr          A generic pointer which will be typecasted based on heapType
 */
NV_STATUS heapInitInternal_IMPL
(
    OBJGPU            *pGpu,
    Heap              *pHeap,
    NvU64              base,
    NvU64              size,
    HEAP_TYPE_INTERNAL heapType,
    void              *pPtr
)
{
    MEM_BLOCK              *pBlock;
    NvU32                   i;
    NV_STATUS               status;
    MemoryManager          *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32                   typeDataSize = 0;
    FB_REGION_DESCRIPTOR   *pFbRegion;
    MEMORY_DESCRIPTOR      *pPmsaMemDesc = NULL;

    //
    // Simply create a free heap.
    //
    NV_PRINTF(LEVEL_INFO,
              "Heap Manager: HEAP ABOUT TO BE CREATED. (Base: 0x%llx Size: 0x%llx)\n",
              base, size);

    pHeap->base  = base;
    pHeap->total = size;
    pHeap->free  = size;
    pHeap->reserved = 0;
    pHeap->heapType = heapType;

    // Set the flags based on HEAP type
    switch (heapType)
    {
        case HEAP_TYPE_RM_GLOBAL:
            pHeap->bHasFbRegions      = NV_TRUE;
            break;
        case HEAP_TYPE_PHYS_MEM_SUBALLOCATOR:
            NV_ASSERT(pPtr != NULL);

            pHeap->bHasFbRegions      = NV_FALSE;
            typeDataSize = sizeof(PHYS_MEM_SUBALLOCATOR_DATA);
            pPmsaMemDesc = ((PHYS_MEM_SUBALLOCATOR_DATA *)pPtr)->pMemDesc;
            break;
        case HEAP_TYPE_PARTITION_LOCAL:
            pHeap->bHasFbRegions      = NV_TRUE;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    pHeap->pHeapTypeSpecificData = NULL;
    if ((pPtr != NULL) && (typeDataSize > 0))
    {
        pHeap->pHeapTypeSpecificData = portMemAllocNonPaged(typeDataSize);
        if (pHeap->pHeapTypeSpecificData == NULL)
        {
            return NV_ERR_OPERATING_SYSTEM;
        }
        NV_ASSERT(pHeap->pHeapTypeSpecificData != NULL);
        portMemCopy(pHeap->pHeapTypeSpecificData, typeDataSize, pPtr, typeDataSize);
    }

    pBlock = portMemAllocNonPaged(sizeof(MEM_BLOCK));
    if (pBlock == NULL)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }
    portMemSet(pBlock, 0, sizeof(MEM_BLOCK));

    pBlock->owner    = NVOS32_BLOCK_TYPE_FREE;
    pBlock->textureId= 0;
    pBlock->begin    = base;
    pBlock->align    = 0;
    pBlock->alignPad = 0;
    pBlock->end      = base + size - 1;
    pBlock->u0.prevFree = pBlock;
    pBlock->u1.nextFree = pBlock;
    pBlock->next     = pBlock;
    pBlock->prev     = pBlock;
    pBlock->format   = 0;

    pHeap->pBlockList     = pBlock;
    pHeap->pFreeBlockList = pBlock;
    pHeap->pBlockTree     = NULL;

    //
    // Set the client id as invalid since there isn't one that exists
    // Initialize the client texture data structure
    //
    portMemSet(pHeap->textureData, 0,
               sizeof(TEX_INFO) * MAX_TEXTURE_CLIENT_IDS);

    //
    // Call into the hal to get bank placement policy.  Note this will vary chip to chip, but let's allow the HAL to tell us
    // the implementation details.
    //
    status = memmgrGetBankPlacementData_HAL(pGpu, pMemoryManager, pHeap->placementStrategy);
    if (status != NV_OK)
    {
        //
        // ooops, can't get HAL version of where to place things - let's default to something
        //
        NV_PRINTF(LEVEL_ERROR,
                  "Heap Manager unable to get bank placement policy from HAL.\n");
        NV_PRINTF(LEVEL_ERROR,
                  "Heap Manager defaulting to BAD placement policy.\n");

        pHeap->placementStrategy[BANK_PLACEMENT_IMAGE]  = ((0)
                                                            | BANK_MEM_GROW_UP
                                                            | MEM_GROW_UP
                                                            | 0xFFFFFF00);
        pHeap->placementStrategy[BANK_PLACEMENT_DEPTH]  = ((0)
                                                            | BANK_MEM_GROW_DOWN
                                                            | MEM_GROW_DOWN
                                                            | 0xFFFFFF00);
        pHeap->placementStrategy[BANK_PLACEMENT_TEX_OVERLAY_FONT]  = ((0)
                                                            | BANK_MEM_GROW_DOWN
                                                            | MEM_GROW_DOWN
                                                            | 0xFFFFFF00);
        pHeap->placementStrategy[BANK_PLACEMENT_OTHER]  = ((0)
                                                            | BANK_MEM_GROW_DOWN
                                                            | MEM_GROW_DOWN
                                                            | 0xFFFFFF00);
        status = NV_OK;
    }

    // Setup noncontig list
    pHeap->pNoncontigFreeBlockList = NULL;

    // insert first block into rb-tree
    if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_ADD))
    {
        return NV_ERR_INVALID_STATE;
    }

    //
    // If there are FB regions defined, check to see if any of them are
    // marked reserved. Tag those regions as reserved in the heap.
    //
    if ((pMemoryManager->Ram.numFBRegions > 0) && (pHeap->bHasFbRegions))
    {
        FB_REGION_DESCRIPTOR consoleFbRegion;
        portMemSet(&consoleFbRegion, 0, sizeof(consoleFbRegion));

        if (heapType != HEAP_TYPE_PARTITION_LOCAL)
        {
            //
            // If a region of FB is actively being used for console display memory
            // on this GPU, mark it reserved in-place.
            // In case of legacy SLI on Linux, console size is only known at StateInit
            //
            memmgrReserveConsoleRegion_HAL(pGpu, pMemoryManager, &consoleFbRegion);
            status = memmgrAllocateConsoleRegion_HAL(pGpu, pMemoryManager, &consoleFbRegion);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING, "Squashing the error status after failing to allocate console region, status: %x\n",
                            status);
                status = NV_OK;
            }
        }

        //
        // Define PMA-managed regions
        // This will be moved to memmgr once we refactor SMC partitions
        //
        if (memmgrIsPmaEnabled(pMemoryManager) &&
            memmgrIsPmaSupportedOnPlatform(pMemoryManager) &&
            (heapType != HEAP_TYPE_PARTITION_LOCAL))
        {
            memmgrSetPmaInitialized(pMemoryManager, NV_TRUE);
            memmgrRegionSetupForPma(pGpu, pMemoryManager);
        }

        if (heapType != HEAP_TYPE_PARTITION_LOCAL)
        {
            // For GSP RM, all PMA candidate regions are given to CPU RM for its use
            if (RMCFG_FEATURE_PLATFORM_GSP)
            {
                memmgrRegionSetupForPma(pGpu, pMemoryManager);
            }

            for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
            {
                pFbRegion = &pMemoryManager->Ram.fbRegion[i];

                // If the region is marked reserved, reserve it in the heap
                if (pFbRegion->bRsvdRegion ||
                    ((memmgrIsPmaInitialized(pMemoryManager) ||
                      RMCFG_FEATURE_PLATFORM_GSP) &&
                     !pFbRegion->bInternalHeap))
                {
                    NvU64 fbRegionBase;
                    MEMORY_DESCRIPTOR *pMemDesc = NULL;

                    // Skip regions which are outside the heap boundaries
                    if (pFbRegion->base < base && pFbRegion->limit < base)
                    {
                        continue;
                    }

                    // TODO: Remove SRIOV check and enable on baremetal as well.
                    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && (pFbRegion->base >= (base + size)))
                    {
                        continue;
                    }

                    // Adjust base of reserved region on heap
                    fbRegionBase = NV_MAX(base, pFbRegion->base);

                    NV_PRINTF(LEVEL_INFO, "Reserve at %llx of size %llx\n",
                        fbRegionBase, (pFbRegion->limit - fbRegionBase + 1));

                    status = heapReserveRegion(
                        pMemoryManager,
                        pHeap,
                        fbRegionBase,
                        (pFbRegion->limit - fbRegionBase + 1),
                        &pMemDesc,
                        pFbRegion->bRsvdRegion,
                        pFbRegion->bProtected);

                    if (status != NV_OK || pMemDesc == NULL)
                    {
                        NV_PRINTF(LEVEL_ERROR, "failed to reserve %llx..%llx\n",
                                  pFbRegion->base, pFbRegion->limit);
                        return status;
                    }

                    if (pFbRegion->bLostOnSuspend)
                    {
                        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_LOST_ON_SUSPEND, NV_TRUE);
                    }

                    if (pFbRegion->bPreserveOnSuspend)
                    {
                        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND, NV_TRUE);
                    }
                }
            }
        }

#ifdef DEBUG
        _heapDump(pHeap);
#endif
    } //if ((pMemoryManager->Ram.numFBRegions > 0) && (pHeap->bHasFbRegions))

    // Hand over all the memory of partition-heap to partition-PMA
    if ((heapType == HEAP_TYPE_PARTITION_LOCAL) &&
        (memmgrIsPmaInitialized(pMemoryManager)))
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        NvBool bProtected = NV_FALSE;

    bProtected = gpuIsCCFeatureEnabled(pGpu);
        status = heapReserveRegion(
            pMemoryManager,
            pHeap,
            base,
            size,
            &pMemDesc,
            NV_FALSE,
            bProtected);

        if (status != NV_OK || pMemDesc == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to reserve %llx..%llx\n", base,
                      base + size - 1);

                return status;
        }
    }

    // If PHYS_MEM_SUBALLOCATOR, increase its refCount
    if ((status == NV_OK) && (pPmsaMemDesc != NULL))
    {
        memdescAddRef(pPmsaMemDesc);
    }

    return (status);
}

void
heapDestruct_IMPL
(
    Heap *pHeap
)
{
    MEM_BLOCK            *pBlock, *pBlockFirst, *pBlockNext;
    OBJGPU               *pGpu = ENG_GET_GPU(pHeap);
    MemoryManager        *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool                headptr_updated;
    MEMORY_DESCRIPTOR    *pPmsaMemDesc = NULL;

    NV_PRINTF(LEVEL_INFO, "Heap Manager: HEAP ABOUT TO BE DESTROYED.\n");

#ifdef DEBUG
    _heapDump(pHeap);
#endif

    // Free all blacklisted pages
    if (pHeap->blackListAddresses.count != 0)
    {
        heapFreeBlackListedPages(pGpu, pHeap);
    }

    //
    // Free all allocated blocks, but preserve primary surfaces.
    // If the head of our list changes, restart the search, since our terminating
    // block pointer may not be in the list anymore.
    //
    do
    {
        pBlock = pBlockFirst = pHeap->pBlockList;
        if (pBlock == NULL)
        {
            break;
        }

        headptr_updated = NV_FALSE;

        do
        {
            pBlockNext = pBlock->next;

            // If we are freeing the reserved region created at heapInit, free the memory descriptor too
            if ((pBlock->allocedMemDesc) && ((pBlock->owner == HEAP_OWNER_RM_RESERVED_REGION) ||
                (pBlock->owner == HEAP_OWNER_PMA_RESERVED_REGION)))
            {
                memdescDestroy(pBlock->pMemDesc);
                pBlock->pMemDesc = NULL;
                pBlock->allocedMemDesc = NV_FALSE;
            }

            _heapBlockFree(pGpu, pHeap, NV01_NULL_OBJECT, NV01_NULL_OBJECT, pBlock);

            // restart scanning the list, if the heap->pBlockList changed
            if (pBlockFirst != pHeap->pBlockList)
            {
                headptr_updated = NV_TRUE;
                break;
            }

            pBlock = pBlockNext;

        } while (pBlock != pHeap->pBlockList);

    } while (headptr_updated);

    //
    // Now that the console region is no longer reserved, free the console
    // memdesc.
    //
    if (pHeap->heapType != HEAP_TYPE_PARTITION_LOCAL)
        memmgrReleaseConsoleRegion(pGpu, pMemoryManager);

    //
    // Free the heap structure, if we freed everything
    // (the first block represents the entire free space of the heap).
    // this is only done if the "internal" interface is used.
    // heapDestroy is an exported function now to user/display driver land,
    // and we don't want the heap structures being freed unless we've been
    // called from RM-land during a STATE_DESTROY
    //
    if ((pHeap->pBlockList != NULL) &&
        (pHeap->pBlockList->begin == pHeap->base) &&
        (pHeap->pBlockList->end == (pHeap->base + pHeap->total - 1)))
    {
        portMemFree(pHeap->pBlockList);
    }

    // Free the type specific data allocated
    if (pHeap->pHeapTypeSpecificData != NULL)
    {
        if (pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR)
        {
            pPmsaMemDesc = ((PHYS_MEM_SUBALLOCATOR_DATA *)(pHeap->pHeapTypeSpecificData))->pMemDesc;
            memdescDestroy(pPmsaMemDesc);
        }
        portMemFree(pHeap->pHeapTypeSpecificData);
        pHeap->pHeapTypeSpecificData = NULL;
    }

    if ((pHeap->bHasFbRegions) && (memmgrIsPmaInitialized(pMemoryManager)))
    {
        if (pHeap->heapType != HEAP_TYPE_PARTITION_LOCAL)
            memmgrSetPmaInitialized(pMemoryManager, NV_FALSE);

        pmaDestroy(&pHeap->pmaObject);
        portMemSet(&pHeap->pmaObject, 0, sizeof(pHeap->pmaObject));
    }
}

static NV_STATUS _heapGetBankPlacement
(
    OBJGPU *pGpu,
    Heap   *pHeap,
    NvU32   owner,
    NvU32  *flags,
    NvU32   type,
    NvU32   bank,
    NvU32  *placement
)
{
    NvU32    bankPlacement, i;

    if (type != NVOS32_TYPE_PRIMARY)
    {
        NvU32 bankPlacementType;

        // what kind of allocation is it?
        switch (type)
        {
        case NVOS32_TYPE_IMAGE:
        case NVOS32_TYPE_NOTIFIER:
            bankPlacementType = BANK_PLACEMENT_IMAGE;
            break;
        case NVOS32_TYPE_DEPTH:
        case NVOS32_TYPE_ZCULL:
        case NVOS32_TYPE_STENCIL:
            bankPlacementType = BANK_PLACEMENT_DEPTH;
            break;
        case NVOS32_TYPE_TEXTURE:
        case NVOS32_TYPE_VIDEO:
        case NVOS32_TYPE_FONT:
            bankPlacementType = BANK_PLACEMENT_TEX_OVERLAY_FONT;
            break;
        default:
            bankPlacementType = BANK_PLACEMENT_OTHER;
        }

        //
        // NV50+ doesn't care about bank placement since the fb has bank
        // striding and we dont need to care about allocating primary surfaces
        // in special areas to avoid bank conflicts. This strategy management
        // should be removed in the future.
        //
        bankPlacement = pHeap->placementStrategy[bankPlacementType];
    }
    else
    {
        //
        // primary allocation, default grow direction is up, starting at bank 0
        // Can be overridden with NVOS32_ALLOC_FLAGS_FORCE_MEM_*
        //
        bankPlacement = ((0)
                        | BANK_MEM_GROW_UP
                        | MEM_GROW_UP
                        | 0xFFFFFF00);
    }

    //
    // check if bank placement force was passed in - hint is handled in the first loop below
    //
    if (*flags & NVOS32_ALLOC_FLAGS_BANK_FORCE)
    {
        // replace data in bankplacement
        if (*flags & NVOS32_ALLOC_FLAGS_BANK_GROW_DOWN)
            bankPlacement = bank | BANK_MEM_GROW_DOWN | 0xFFFFFF00;
        else
            bankPlacement = bank | BANK_MEM_GROW_UP   | 0xFFFFFF00;
        *flags &= ~(NVOS32_ALLOC_FLAGS_BANK_HINT);   // remove hint flag
    }

    //
    // Check if FORCE_MEM_GROWS_UP or FORCE_MEM_GROWS_DOWN was passed in
    // to override the MEM_GROWS direction for this allocation.  Make sure
    // to override each of the first MEM_NUM_BANKS_TO_TRY bytes in the NvU32
    //
    if (*flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP)
    {
        *flags |= NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT;
        for (i = 0; i < MEM_NUM_BANKS_TO_TRY; i++)
        {
            bankPlacement = (bankPlacement & ~(MEM_GROW_MASK << (i*MEM_BANK_DATA_SIZE))) |
                            (MEM_GROW_UP << (i*MEM_BANK_DATA_SIZE));
        }
    }
    if (*flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN)
    {
        *flags |= NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT;
        for (i = 0; i < MEM_NUM_BANKS_TO_TRY; i++)
        {
            bankPlacement = (bankPlacement & ~(MEM_GROW_MASK << (i*MEM_BANK_DATA_SIZE))) |
                            (MEM_GROW_DOWN << (i*MEM_BANK_DATA_SIZE));
        }
    }

    // return the bank placement to use
    *placement = bankPlacement;
    return (NV_OK);
}

//
// Workaround for Bug 67690:
//    NV28M-WinXP: (Lindbergh) StencilFloor OpenGL Sample Locks Up when Maximized on Secondary DualView Display
//
// Change heap placement for textures if more than two clients
// are detected.  In the case of two or more clients, ignoreBankPlacement, textureClientIndex,
// and currentBankInfo are modified.  IgnoreBankPlacement flag is set to true, textureClientIndex
// is returned with the index of the client to be used as heap->textureData[textureClientIndex]
// which pertains to the current client.  Lastly, currentBankInfo is modified to grow in the
// opposite direction of the most recently allocated client.
//
static void _heapSetTexturePlacement
(
    Heap   *pHeap,
    NvU32   client,
    NvU32   type,
    NvBool *ignoreBankPlacement,
    NvU32  *textureClientIndex,
    NvU8   *currentBankInfo
)
{
    NvU32 index, numClients, clientFound, mostRecentIndex;
    mostRecentIndex     = 0xFFFFFFFF;
    clientFound         = NV_FALSE;
    numClients          = 0;

    //
    // let's first check to see if the client is already registered
    // We will iterate thru to find number of clients
    //
    for (index = 0; index < MAX_TEXTURE_CLIENT_IDS; index++)
    {
        // client already registered
        if (pHeap->textureData[index].clientId == client)
        {
            // give the currentBankInfo the new flags
            *currentBankInfo = pHeap->textureData[index].placementFlags;
            //
            // Set the client as found so that we skip allocation
            // of the client in the texture data structure
            //
            clientFound = NV_TRUE;
            *textureClientIndex = index;
        }

        //
        // We loop through the whole structure to determine the
        // number of texture clients currently listed
        //
        if (pHeap->textureData[index].clientId != 0)
            numClients++;

        //
        // This is used to assign new textures to the buffer
        // A value of 0xFFFFFFFF indicates that this is the first allocation
        //
        if (pHeap->textureData[index].mostRecentAllocatedFlag == NV_TRUE)
            mostRecentIndex = index;
    }

    //
    // If more than one clinet is detected, ignore bank placement
    // otherwise, defaults to bank placement
    //
    if (numClients > 1)
        *ignoreBankPlacement = NV_TRUE;

    //
    // We fall into this if statement if no client was listed
    // or if we have exceeded the allowable clients available
    //
    if (clientFound == NV_FALSE)
    {
        index = 0;
        while (clientFound == NV_FALSE)
        {
            // the case of full texture buffer of clients, greater than 4 clients
            if (index == MAX_TEXTURE_CLIENT_IDS)
            {
                index = (mostRecentIndex + 1) % MAX_TEXTURE_CLIENT_IDS;

                // assign the new client and update the texture data
                pHeap->textureData[index].clientId                           = client;
                pHeap->textureData[index].mostRecentAllocatedFlag            = NV_TRUE;
                pHeap->textureData[mostRecentIndex].mostRecentAllocatedFlag  = NV_FALSE;
                pHeap->textureData[index].refCount                           = 0;

                //
                // Reverse the placementFlags from the one that was previously allocated
                //
                if (pHeap->textureData[mostRecentIndex].placementFlags & MEM_GROW_MASK)
                    *currentBankInfo = MEM_GROW_UP;
                else
                    *currentBankInfo = MEM_GROW_DOWN;

                // Assign the new value to the texture data structure
                pHeap->textureData[index].placementFlags = *currentBankInfo;
                clientFound                             = NV_TRUE;
                *ignoreBankPlacement                    = NV_TRUE;
                *textureClientIndex                     = index;
            }

            // the case in which there is still room available in the buffer
            if (pHeap->textureData[index].clientId == 0)
            {
                // If we fall in here, it means there is still room available
                pHeap->textureData[index].clientId = client;

                // deal with the grow directivity
                if (mostRecentIndex == 0xFFFFFFFF)
                {
                    // this is the very first client to be allocated
                    pHeap->textureData[index].placementFlags = *currentBankInfo;
                    if (pHeap->textureData[index].placementFlags & MEM_GROW_MASK)
                        *currentBankInfo = MEM_GROW_DOWN;
                    else
                        *currentBankInfo = MEM_GROW_UP;
                    pHeap->textureData[index].mostRecentAllocatedFlag = NV_TRUE;
                }
                else
                {
                    if (pHeap->textureData[mostRecentIndex].placementFlags & MEM_GROW_MASK)
                        *currentBankInfo = MEM_GROW_UP;
                    else
                        *currentBankInfo = MEM_GROW_DOWN;

                    // Set the last client allocated to the new client allocated
                    pHeap->textureData[mostRecentIndex].mostRecentAllocatedFlag  = NV_FALSE;
                    pHeap->textureData[index].mostRecentAllocatedFlag            = NV_TRUE;

                    // update the placement flags
                    pHeap->textureData[index].placementFlags                     = *currentBankInfo;

                    // if this isn't the first client in the heap, then we ignore bank placement
                    *ignoreBankPlacement                                         = NV_TRUE;
                }

                clientFound = NV_TRUE;
                *textureClientIndex = index;
            }
            index++;
        }           // while (clientFound == NV_FALSE)
    }               // if (clientFound == NV_FALSE)
}

//
// If we have two different alignment requirements for a memory
// allocation, this routine calculates the LCM (least common multiple)
// to satisfy both requirements.
//
// An alignment of 0 means "no preferred alignment".  The return value
// will not exceed maxAlignment = NV_U64_MAX; it returns maxAlignment if the limit
// is exceeded.
//
// Called by heapAlloc and heapAllocHint.
//


/*!
 *  @Is Alloc Valid For FB Region
 *
 * Check the prospective allocation to see if the candidate block supports
 * the requested surface type.
 *
 * NOTE: The FB region and FB heap allocation code assume that free blocks
 * reside in a single FB region.  This is true in current implementations that
 * have the regions separated by a reserved block, but may not be true in future
 * implementations.
 *
 *  @param[in]   pGpu           GPU object
 *  @param[in]   pHeap          heap object
 *  @param[in]   pFbAllocInfo   allocation request information
 *  @param[in]   pAllocData     allocation candidate information
 *
 *  @returns NV_TRUE if block can be allocated at the prospective address
 *
 */
static NvBool
_isAllocValidForFBRegion
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    FB_ALLOC_INFO      *pFbAllocInfo,
    OBJHEAP_ALLOC_DATA *pAllocData
)
{
    MemoryManager        *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool                isValid        = NV_FALSE;
    FB_REGION_DESCRIPTOR *fbRegion;

    // Check if any regions are defined.  If not, then we are done.
    if (pMemoryManager->Ram.numFBRegions > 0)
    {
        fbRegion = memmgrLookupFbRegionByOffset(pGpu, pMemoryManager, pAllocData->allocLo, pAllocData->allocHi);

        if (fbRegion != NULL)
        {
            // Because we heapAlloc the reserved region.
            if (pFbAllocInfo->pageFormat->type == NVOS32_TYPE_PMA &&
                pFbAllocInfo->owner == HEAP_OWNER_PMA_RESERVED_REGION)
            {
                if (!fbRegion->bInternalHeap && !fbRegion->bRsvdRegion)
                {
                    isValid = NV_TRUE;
                }
                return isValid;
            }
            // Check if the region is reserved/not usable
            if (fbRegion->bRsvdRegion &&
                (pFbAllocInfo->pageFormat->type != NVOS32_TYPE_RESERVED))
            {
                NV_PRINTF(LEVEL_INFO,
                          "Reserved region.  Rejecting placement\n");
                return NV_FALSE;
            }

            //
            // Check if the region supports compression and if we need it.
            // Surfaces that *require* compression can be allocated *only* in
            // regions that support compression.  *Optionally* compressed surfaces
            // can be allocated anywhere though -- the selection of an uncompressed
            // KIND will be handled in dmaUpdateVASpace.
            //
            if (!fbRegion->bSupportCompressed)
            {
                if (DRF_VAL(OS32, _ATTR, _COMPR , pFbAllocInfo->pageFormat->attr) == NVOS32_ATTR_COMPR_REQUIRED)
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Compression not supported.  Rejecting placement\n");
                    return NV_FALSE;
                }
            }

            // Check if the allocation type is specifically not allowed
            if (pFbAllocInfo->pageFormat->type < NVOS32_NUM_MEM_TYPES)
            {
                if ((!fbRegion->bSupportISO) &&
                    ((pFbAllocInfo->pageFormat->type == NVOS32_TYPE_PRIMARY) ||
                     (pFbAllocInfo->pageFormat->type == NVOS32_TYPE_CURSOR) ||
                     (pFbAllocInfo->pageFormat->type == NVOS32_TYPE_VIDEO)))
                {
                    NV_PRINTF(LEVEL_INFO,
                              "ISO surface type #%d not supported.  Rejecting placement\n",
                              pFbAllocInfo->pageFormat->type);
                    return NV_FALSE;
                }
            }

            if (!!fbRegion->bProtected ^
                !!(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_PROTECTED))
            {
                NV_PRINTF(LEVEL_INFO,
                          "Protection mismatch.  Rejecting placement\n");
                return NV_FALSE;
            }

        }
        else if (pFbAllocInfo->pageFormat->type != NVOS32_TYPE_RESERVED)
        {
            //
            // Allow reserved allocs outside of valid regions, but everything else
            // must be allocated in a region.
            //
            NV_PRINTF(LEVEL_INFO,
                      "pFbAllocInfo->type != NVOS32_TYPE_RESERVED\n");
            return NV_FALSE;
        }

    }

    return NV_TRUE;
}

/**
 * Blacklists a single page
 * This function will allocate the memory descriptor with a fixed memory offset
 * and allocate the FB physical offset. Will replace the blacklist allocation
 * path in the heapBlackListPages_IMPL.
 *
 * @param[in]    pGpu            OBJGPU pointer
 * @param[in]    pHeap           Heap pointer
 * @param[in]    pBlacklistChunk BLACKLIST_CHUNK pointer
 *
 * @returns NV_OK on success
 *          NV_ERR_OUT_OF_MEMORY, if the memory is already blacklisted
 */

static NV_STATUS
_heapBlacklistSingleChunk
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    BLACKLIST_CHUNK    *pBlacklistChunk
)
{
    NV_STATUS status = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_ASSERT(pBlacklistChunk != NULL);

    status = memdescCreate(&pBlacklistChunk->pMemDesc,
                           pGpu, pBlacklistChunk->size, RM_PAGE_SIZE,
                           NV_TRUE, ADDR_FBMEM, NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE |
                           MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_FATAL,
                  "Error 0x%x creating memdesc for blacklisted chunk for address0x%llx, skipping\n",
                  status, pBlacklistChunk->physOffset);
        NV_ASSERT(NV_FALSE);
        return status;
    }

    // this is how FIXED_ADDRESS_ALLOCATE works
    memdescSetPte(pBlacklistChunk->pMemDesc, AT_GPU, 0, RM_PAGE_ALIGN_DOWN(pBlacklistChunk->physOffset));

    if (pHeap->heapType != HEAP_TYPE_PHYS_MEM_SUBALLOCATOR)
    {
        //
        // Allocate memory for this page. This is marked as an internal RM allocation
        // and will be saved/restored during suspend/resume
        //
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_78, 
                    pBlacklistChunk->pMemDesc);
        if (NV_OK != status)
        {
            // no use for the memdesc if page couldn't be allocated
            memdescDestroy(pBlacklistChunk->pMemDesc);

            NV_PRINTF(LEVEL_FATAL,
                      "Error 0x%x creating page for blacklisting address: 0x%llx, skipping\n",
                      status, pBlacklistChunk->physOffset);
            NV_ASSERT(NV_FALSE);
            return status;
        }
    }

    // set the flags properly
    pBlacklistChunk->bIsValid                = NV_TRUE;

    // if dynamic blacklisteing is enabled, clear the pending retirement flag
    if (pMemoryManager->bEnableDynamicPageOfflining)
    {
        pBlacklistChunk->bPendingRetirement = NV_FALSE;
    }
    return status;
}

/**
 * Free-s the blacklisted pages within the range [begin, begin+size-1]
 * This function will iterate the blacklisted chunks data structure,
 * and free the blacklisted pages within the range [begin, begin+size-1]
 *
 * @param[in]    pGpu           OBJGPU pointer
 * @param[in]    pMemoryManager MemoryManager pointer
 * @param[in]    pBlackList     BLACKLIST pointer
 * @param[in]    begin          starting address of the range
 * @param[in]    size           Size of the region, where blacklisted pages to be free-d
 *
 * @returns NV_OK on success
 */
static NV_STATUS
_heapFreeBlacklistPages
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    BLACKLIST     *pBlackList,
    NvU64          begin,
    NvU64          size
)
{
    NvU32               chunk               = 0;
    NvU64               baseChunkAddress    = 0;
    NvU64               endChunkAddress     = 0;
    BLACKLIST_CHUNK    *pBlacklistChunks    = pBlackList->pBlacklistChunks;

    for (chunk = 0; chunk < pBlackList->count; chunk++)
    {
        baseChunkAddress = 0;
        endChunkAddress  = 0;
        // No need to process the chunk if it's not a valid chunk
        if (pBlacklistChunks[chunk].bIsValid != NV_TRUE ||
           (pMemoryManager->bEnableDynamicPageOfflining &&
            pBlacklistChunks[chunk].bPendingRetirement))
            continue;

        baseChunkAddress = pBlacklistChunks[chunk].physOffset;
        endChunkAddress =  baseChunkAddress + pBlacklistChunks[chunk].size - 1;

        if (baseChunkAddress >= begin && endChunkAddress <= (begin + size - 1))
        {
            //
            // free the mem desc, set the excludeGlobalListFlag
            // invalidate the entry
            //
             NV_PRINTF(LEVEL_FATAL,
                       "removing from blacklist... page start %llx, page end:%llx\n",
                       baseChunkAddress, endChunkAddress);

            memdescFree(pBlacklistChunks[chunk].pMemDesc);
            memdescDestroy(pBlacklistChunks[chunk].pMemDesc);

            pBlacklistChunks[chunk].bIsValid                = NV_FALSE;
        }
    }
    return NV_OK;
}

/**
 * Blacklist pages within the range [begin, begin+size-1]
 * This function will iterate the blacklisted chunks data structure,
 * and blacklist pages within the range [begin, begin+size-1]
 *
 * @param[in]    pGpu       OBJGPU pointer
 * @param[in]    pHeap      Heap pointer
 * @param[in]    pBlackList BLACKLIST pointer
 * @param[in]    begin      starting address of the range
 * @param[in]    size       Size of the region, where pages will be blacklisted
 *
 * @returns NV_OK on success
 *           error, if _heapBlacklistSingleChunk fails
 */
static NV_STATUS
_heapBlacklistChunks
(
    OBJGPU         *pGpu,
    Heap           *pHeap,
    BLACKLIST      *pBlackList,
    NvU64           begin,
    NvU64           size
)
{
    NvU32              chunk                = 0;
    NvU64              baseAddress          = 0;
    NvU64              endAddress           = 0;
    BLACKLIST_CHUNK   *pBlacklistChunks     = pBlackList->pBlacklistChunks;
    NV_STATUS          status               = NV_OK;


    for (chunk = 0; chunk < pBlackList->count; chunk++)
    {
        baseAddress     = 0;
        endAddress      = 0;

        // No need to process the chunk if it's a valid chunk
        if (pBlacklistChunks[chunk].bIsValid == NV_TRUE)
            continue;

        baseAddress   = pBlacklistChunks[chunk].physOffset;
        endAddress    = baseAddress + pBlacklistChunks[chunk].size - 1;

        //TODO: what if the blacklisted chunk is halfway inside the allocated region??
        if (baseAddress >= begin && endAddress <= (begin + size - 1))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "blacklisting chunk from addr: 0x%llx to 0x%llx, new begin :0x%llx, end:0x%llx\n",
                      baseAddress, endAddress, begin, begin + size - 1);
            status = _heapBlacklistSingleChunk(pGpu, pHeap, &pBlacklistChunks[chunk]);
            NV_ASSERT(status == NV_OK);
        }
    }
    return status;
}

/*!
 * @brief allocate memory from heap
 *
 * Allocates a memory region with requested parameters from heap.
 * If requested contiguous allocation is not possible, tries to allocate non-contiguous memory.
 *
 * @param[in]     pGpu                 GPU object
 * @param[in]     hClient              client handle
 * @param[in]     pHeap                heap object
 * @param[in]     pAllocRequest        allocation request
 * @param[in]     memHandle            memory handle
 * @param[in/out] pAllocData           heap-specific allocation data
 * @param[in/out] pFbAllocInfo         allocation data
 * @param[out]    pHwResource          pointer to allocation HW resource info
 * @param[in/out] pNoncontigAllocation the requested/provided allocation is noncotig
 * @param[in]     bNoncontigAllowed    allocation can be made noncontig
 * @param[in]     bAllocedMemdesc      memdesc should be freed if a new one is created
 */
NV_STATUS heapAlloc_IMPL
(
    OBJGPU                        *pGpu,
    NvHandle                       hClient,
    Heap                          *pHeap,
    MEMORY_ALLOCATION_REQUEST     *pAllocRequest,
    NvHandle                       memHandle,
    OBJHEAP_ALLOC_DATA            *pAllocData,
    FB_ALLOC_INFO                 *pFbAllocInfo,
    HWRESOURCE_INFO              **pHwResource,
    NvBool                        *pNoncontigAllocation,
    NvBool                         bNoncontigAllowed,
    NvBool                         bAllocedMemdesc
)
{
    NV_MEMORY_ALLOCATION_PARAMS   *pVidHeapAlloc        = pAllocRequest->pUserParams;
    MEMORY_DESCRIPTOR             *pMemDesc             = pAllocRequest->pMemDesc;
    MemoryManager                 *pMemoryManager       = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32                          textureClientIndex   = 0xFFFFFFFF;
    NvU64                          desiredOffset        = pFbAllocInfo->offset;
    NvU64                          adjustedSize         = pFbAllocInfo->size - pFbAllocInfo->alignPad;
    NvU32                          bankPlacement        = 0;
    NvBool                         ignoreBankPlacement  = NV_FALSE;
    NvU8                           currentBankInfo;
    MEM_BLOCK                     *pBlockFirstFree;
    MEM_BLOCK                     *pBlockFree;
    MEM_BLOCK                     *pBlockNew            = NULL;
    MEM_BLOCK                     *pBlockSplit          = NULL;
    NvU64                          allocatedOffset      = 0;
    NvBool                         bTurnBlacklistOff    = NV_FALSE;
    NvBool                         bDone                = NV_FALSE;
    NV_STATUS                      status               = NV_OK;
    NvU32                          i;

    NV_ASSERT_OR_RETURN(
        (memmgrAllocGetAddrSpace(GPU_GET_MEMORY_MANAGER(pGpu), pVidHeapAlloc->flags, pVidHeapAlloc->attr)
            == ADDR_FBMEM) &&
        (pAllocRequest->pPmaAllocInfo[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] == NULL),
        NV_ERR_INVALID_ARGUMENT);

    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
        desiredOffset -= pFbAllocInfo->alignPad;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) &&
        gpuCheckPageRetirementSupport_HAL(pGpu) &&
        FLD_TEST_DRF(OS32, _ATTR2, _BLACKLIST, _OFF, pVidHeapAlloc->attr2))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Trying to turn blacklisting pages off for this allocation of size: %llx\n",
                  pVidHeapAlloc->size);
        if (!hypervisorIsVgxHyper())
            _heapFreeBlacklistPages(pGpu, pMemoryManager, &pHeap->blackList, desiredOffset, pVidHeapAlloc->size);
        else
            _heapFreeBlacklistPages(pGpu, pMemoryManager, &pHeap->blackList, pHeap->base, pHeap->total);
        bTurnBlacklistOff = NV_TRUE;
        // Now continue with the heap allocation.
    }

    //
    // Check for range-limited request.
    // Range of [0,0] is a special case that means to use the entire heap.
    //
    // A range-limited request allows caller to say: I really want memory
    //   which only falls completely within a particular range.  Returns
    //   error if can't allocate within that range.
    //
    //   Used on Windows by OpenGL.  On Windows during a modeswitch, the
    //   display driver frees all vidmem surfaces.  Unfortunately, OpenGL
    //   writes to some vidmem surface with the CPU from user mode.  If these
    //   surfaces are freed during the modeswitch, then the user mode OpenGL
    //   app might scribble on someone else's surface if that video memory is
    //   reused before OpenGL notices the modeswitch.  Because modeswitches
    //   are asynchronous to the OpenGL client, it does not notice the
    //   modeswitches right away.
    //
    //   A solution is for OpenGL to restrict vidmem surfaces that have
    //   this problem to a range of memory where it is safe *not* to free
    //   the surface during a modeswitch.
    //
    // virtual allocation are checked in dmaAllocVA()
    if (pVidHeapAlloc->rangeLo == 0 && pVidHeapAlloc->rangeHi == 0)
    {
        pVidHeapAlloc->rangeHi = pHeap->base + pHeap->total - 1;
    }
    if (pVidHeapAlloc->rangeHi > pHeap->base + pHeap->total - 1)
    {
        pVidHeapAlloc->rangeHi = pHeap->base + pHeap->total - 1;
    }

    if ((pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE) == 0)
    {
        // Only want to override in one direction at a time
        if (pMemoryManager->overrideInitHeapMin == 0)
        {
            pVidHeapAlloc->rangeHi = NV_MIN(pVidHeapAlloc->rangeHi, pMemoryManager->overrideHeapMax);
        }
        else
        {
            pVidHeapAlloc->rangeLo = NV_MAX(pVidHeapAlloc->rangeLo, pMemoryManager->overrideInitHeapMin);
        }
    }

    //
    // Check for valid range.
    //
    if (pVidHeapAlloc->rangeLo > pVidHeapAlloc->rangeHi)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto return_early;
    }

    //
    // The bank placement loop does not know how to limit allocations to be
    // within a range.
    //
    if (((pVidHeapAlloc->rangeLo > 0) || (pVidHeapAlloc->rangeHi < pHeap->base + pHeap->total - 1)))
    {
        pVidHeapAlloc->flags |= NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT;
    }

    //
    // Set up bank placement data - should have been preselected in heapCreate
    //
    status = _heapGetBankPlacement(pGpu, pHeap, pVidHeapAlloc->owner,
                                   &pVidHeapAlloc->flags,
                                   pVidHeapAlloc->type,
                                   0,
                                   &bankPlacement);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "_heapGetBankPlacement failed for current allocation\n");
        goto return_early;
    }

    //
    // Find the best bank to start looking in for this pVidHeapAlloc->type, but only if we're
    // not ignoring bank placement rules.  Save the current bank info.
    //
    currentBankInfo = (NvU8)bankPlacement; // this is always non zero from above

    //
    // Check for fixed address request.
    // This allows caller to say: I really want this memory at a particular
    //   offset.  Returns error if can't get that offset.
    //   Used initially by Mac display driver twinview code.
    //   On the Mac it is a very bad thing to *ever* move the primary
    //   during a modeset since a lot of sw caches the value and never
    //   checks again.
    //
    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {

        // is our desired offset suitably aligned?
        if (desiredOffset % pAllocData->alignment)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "offset 0x%llx not aligned to 0x%llx\n",
                      desiredOffset, pAllocData->alignment);
            goto failed;
        }

        pBlockFree = pHeap->pFreeBlockList;

        if (pBlockFree == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "no free blocks\n");
            goto failed;
        }

        do {
            //
            // Allocate from the bottom of the memory block.
            //
            pBlockFree = pBlockFree->u1.nextFree;

            // Does this block contain our desired range?
            if ((desiredOffset >= pBlockFree->begin) &&
                (desiredOffset + pAllocData->allocSize - 1) <= pBlockFree->end)
            {
                // we have a match, now remove it from the pool
                pAllocData->allocLo = desiredOffset;
                pAllocData->allocHi = desiredOffset + pAllocData->allocSize - 1;
                pAllocData->allocAl = pAllocData->allocLo;

                // Check that the candidate block can support the allocation type
                if (_isAllocValidForFBRegion(pGpu, pHeap, pFbAllocInfo, pAllocData))
                    goto got_one;
            }

        } while (pBlockFree != pHeap->pFreeBlockList);

        // return error if can't get that particular address
        NV_PRINTF(LEVEL_ERROR,
                  "failed NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE @%llx (%lld bytes)\n",
                  desiredOffset, pAllocData->allocSize);
        goto failed;
    }

    //
    // Check if NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT was passed in with
    // the pVidHeapAlloc->type to ignore placing this allocation in a particular bank.
    // This means we default to the second loop where we choose first fit.
    //
    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT)
        ignoreBankPlacement = NV_TRUE;

    //
    // Bug 67690: Treat textures differently for more than one client (eg. opengl),
    // [IN]:  client, pVidHeapAlloc->type, ignoreBankPlacement
    // [OUT]: heap, ignoreBankPlacement, textureClientIndex
    //
    // Bug 69385: Treat textures differently only if pVidHeapAlloc->flags are also set to zero.
    //   NV30GL-WinXP: Unable to run 3DMark2001SE @ 1600x1200x32bpp.
    //
    if ((pVidHeapAlloc->type == NVOS32_TYPE_TEXTURE) && (!pVidHeapAlloc->flags))
        _heapSetTexturePlacement(pHeap, hClient, pVidHeapAlloc->type, &ignoreBankPlacement, &textureClientIndex, &currentBankInfo);

    if (!ignoreBankPlacement)
    {
        currentBankInfo = (NvU8)bankPlacement & BANK_MEM_GROW_MASK;

        if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_BANK_HINT)
        {
            if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_BANK_GROW_DOWN)
                currentBankInfo = MEM_GROW_DOWN;
            else
                currentBankInfo = MEM_GROW_UP;
            pVidHeapAlloc->flags &= ~(NVOS32_ALLOC_FLAGS_BANK_HINT); // hint flag only lasts for 1 loop
        }
        else
        {
            // Convert bank grow up/down to mem grow up/down
            currentBankInfo = (currentBankInfo & BANK_MEM_GROW_DOWN ? MEM_GROW_DOWN : MEM_GROW_UP);
        }
    } // if (!ignoreBankPlacement)

    pBlockFirstFree = pHeap->pFreeBlockList;
    if (!pBlockFirstFree)
    {
        NV_PRINTF(LEVEL_ERROR, "no free blocks\n");
        goto failed;
    }

    if (*pNoncontigAllocation)
    {
        NV_PRINTF(LEVEL_INFO, "non-contig vidmem requested\n");
        goto non_contig_alloc;
    }

    //
    // Loop through all available regions.
    // Note we don't check for bRsvdRegion here because when blacklisting
    // those regions we need them to succeed.
    //
    bDone = NV_FALSE;
    i = 0;
    while (!bDone)
    {
        NvU64 saveRangeLo = pVidHeapAlloc->rangeLo;
        NvU64 saveRangeHi = pVidHeapAlloc->rangeHi;

        if (!memmgrAreFbRegionsSupported(pMemoryManager) ||
             gpuIsCacheOnlyModeEnabled(pGpu))
        {
            bDone = NV_TRUE;
        }
        else
        {
            NV_ASSERT( pMemoryManager->Ram.numFBRegionPriority > 0 );

            if (FLD_TEST_DRF(OS32, _ATTR2, _PRIORITY, _LOW, pFbAllocInfo->pageFormat->attr2) ||
                (pMemoryManager->bPreferSlowRegion &&
                !FLD_TEST_DRF(OS32, _ATTR2, _PRIORITY, _HIGH, pFbAllocInfo->pageFormat->attr2)))
            {
                NV_ASSERT( pMemoryManager->Ram.fbRegionPriority[pMemoryManager->Ram.numFBRegionPriority-1-i] < pMemoryManager->Ram.numFBRegions );
                NV_ASSERT( !pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[pMemoryManager->Ram.numFBRegionPriority-1-i]].bRsvdRegion );
                //
                // We prefer slow memory, or we want _LOW priority
                // ==>> Try allocations in increasing order of performance,
                // slowest first
                //
                pVidHeapAlloc->rangeLo = NV_MAX(pVidHeapAlloc->rangeLo, pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[pMemoryManager->Ram.numFBRegionPriority-1-i]].base);
                pVidHeapAlloc->rangeHi = NV_MIN(pVidHeapAlloc->rangeHi, pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[pMemoryManager->Ram.numFBRegionPriority-1-i]].limit);
            }
            else
            {
                NV_ASSERT( pMemoryManager->Ram.fbRegionPriority[i] < pMemoryManager->Ram.numFBRegions );
                NV_ASSERT( !pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[i]].bRsvdRegion );
               //
                // We don't explicitly want slow memory or we don't prefer
                // allocations in the slow memory
                // ==>> Try allocations in decreasing order of performance,
                // fastest first
                //
                pVidHeapAlloc->rangeLo = NV_MAX(pVidHeapAlloc->rangeLo, pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[i]].base);
                pVidHeapAlloc->rangeHi = NV_MIN(pVidHeapAlloc->rangeHi, pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[i]].limit);
            }
            i++;

            bDone = !(i < pMemoryManager->Ram.numFBRegionPriority);
        }

        //
        // When scanning upwards, start at the bottom - 1 so the following loop looks symetrical.
        //
        if ( ! (currentBankInfo & MEM_GROW_DOWN))
            pBlockFirstFree = pBlockFirstFree->u0.prevFree;
        pBlockFree = pBlockFirstFree;

        do
        {
            NvU64 blockLo;
            NvU64 blockHi;

            if (currentBankInfo & MEM_GROW_DOWN)
                pBlockFree = pBlockFree->u0.prevFree;
            else
                pBlockFree = pBlockFree->u1.nextFree;

            //
            // Is this block completely in requested range?
            //
            // We *should* check that pBlockFree is wholely resident in the range, but the
            // old check didn't and checking it causes some tests to fail.
            // So check that at least *some* of the block resides within the requested range.
            //
            if ((pBlockFree->end >= pVidHeapAlloc->rangeLo) && (pBlockFree->begin <= pVidHeapAlloc->rangeHi))
            {
                //
                // Find the intersection of the free block and the specified range.
                //
                blockLo = (pVidHeapAlloc->rangeLo > pBlockFree->begin) ? pVidHeapAlloc->rangeLo : pBlockFree->begin;
                blockHi = (pVidHeapAlloc->rangeHi < pBlockFree->end) ? pVidHeapAlloc->rangeHi : pBlockFree->end;

                if (currentBankInfo & MEM_GROW_DOWN)
                {
                    //
                    // Allocate from the top of the memory block.
                    //
                    pAllocData->allocLo = (blockHi - pAllocData->allocSize + 1) / pAllocData->alignment * pAllocData->alignment;
                    pAllocData->allocAl = pAllocData->allocLo;
                    pAllocData->allocHi = pAllocData->allocAl + pAllocData->allocSize - 1;
                }
                else
                {
                    //
                    // Allocate from the bottom of the memory block.
                    //
                    pAllocData->allocAl = (blockLo + (pAllocData->alignment - 1)) / pAllocData->alignment * pAllocData->alignment;
                    pAllocData->allocLo = pAllocData->allocAl;
                    pAllocData->allocHi = pAllocData->allocAl + pAllocData->allocSize - 1;
                }

                //
                // Does the desired range fall completely within this block?
                // Also make sure it does not wrap-around.
                // Also make sure it is within the desired range.
                //
                if ((pAllocData->allocLo >= pBlockFree->begin) && (pAllocData->allocHi <= pBlockFree->end))
                {
                    if (pAllocData->allocLo <= pAllocData->allocHi)
                    {
                        if ((pAllocData->allocLo >= pVidHeapAlloc->rangeLo) && (pAllocData->allocHi <= pVidHeapAlloc->rangeHi))
                        {
                            // Check that the candidate block can support the allocation type
                            if (_isAllocValidForFBRegion(pGpu, pHeap, pFbAllocInfo, pAllocData))
                            {
                                pVidHeapAlloc->rangeLo = saveRangeLo;
                                pVidHeapAlloc->rangeHi = saveRangeHi;
                                goto got_one;
                            }
                        }
                    }
                }
            }

        } while (pBlockFree != pBlockFirstFree);

        pVidHeapAlloc->rangeLo = saveRangeLo;
        pVidHeapAlloc->rangeHi = saveRangeHi;
    }

non_contig_alloc:
    if (!bNoncontigAllowed)
        goto failed;

    if (!*pNoncontigAllocation)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Contig vidmem allocation failed, running noncontig allocator\n");

        // Create a new noncontig memdescriptor
        memdescDestroy(pAllocRequest->pMemDesc);

        status = memdescCreate(&pAllocRequest->pMemDesc, pGpu, adjustedSize,
                               0, NV_FALSE, ADDR_FBMEM, NV_MEMORY_UNCACHED,
                               MEMDESC_FLAGS_NONE);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "cannot alloc memDesc!\n");
            pMemDesc = pAllocRequest->pMemDesc = NULL;
            goto failed;
        }

        pMemDesc = pAllocRequest->pMemDesc;
        pMemDesc->pHeap = pHeap;

        memdescSetPteKind(pMemDesc, pFbAllocInfo->format);
        memdescSetHwResId(pMemDesc, pFbAllocInfo->hwResId);
    }

    // Try the noncontig allocator
    if (NV_OK == _heapAllocNoncontig(pGpu,
                                     hClient,
                                     pHeap,
                                     pAllocRequest,
                                     memHandle,
                                     pAllocData,
                                     pFbAllocInfo,
                                     textureClientIndex,
                                     pFbAllocInfo->alignPad,
                                     &allocatedOffset,
                                     pMemDesc,
                                     pHwResource))
    {
        *pNoncontigAllocation = NV_TRUE;

        //
        // The noncontig allocator calls _heapProcessFreeBlock()
        // by itself, so we goto done: straight
        //
        status = NV_OK;
        goto return_early;
    }

    NV_PRINTF(LEVEL_INFO,
              "failed to allocate block.  Heap total=0x%llx free=0x%llx\n",
              pHeap->total, pHeap->free);
    // Out of memory.
    goto failed;

    //
    // We have a match.  Now link it in, trimming or splitting
    // any slop from the enclosing block as needed.
    //

got_one:
    if (NV_OK != _heapProcessFreeBlock(pGpu, pBlockFree,
                                       &pBlockNew, &pBlockSplit,
                                       pHeap, pAllocRequest,
                                       memHandle,
                                       pAllocData, pFbAllocInfo,
                                       pFbAllocInfo->alignPad,
                                       &allocatedOffset) ||
        NV_OK != _heapUpdate(pHeap, pBlockNew, BLOCK_FREE_STATE_CHANGED))
failed:
    {

        NV_PRINTF(LEVEL_INFO,
                  "failed to allocate block.  Heap total=0x%llx free=0x%llx\n",
                  pHeap->total, pHeap->free);

        portMemFree(pBlockNew);
        pBlockNew = NULL;
        portMemFree(pBlockSplit);
        status = NV_ERR_NO_MEMORY;
        goto return_early;
    }

    //
    // If a client calls us with pVidHeapAlloc->type == NVOS32_TYPE_TEXTURE, but where flags
    // are non-zero, we won't call _heapSetTexturePlacement and initialize
    // textureClientIndex to a proper value (default is 0xFFFFFFFF). In that
    // case, we won't track this texture allocation. Bug 79586.
    //
    if (pVidHeapAlloc->type == NVOS32_TYPE_TEXTURE &&
        textureClientIndex != 0xFFFFFFFF)
    {
        pBlockNew->textureId = hClient;
        pHeap->textureData[textureClientIndex].refCount++;
    }
    else
    {
        pBlockNew->textureId = 0;
    }

    pFbAllocInfo->offset = allocatedOffset;

    // TODO : This must be inside *all* blocks of a noncontig allocation
    if (!*pNoncontigAllocation)
    {
        pBlockNew->pitch = pFbAllocInfo->pitch;
        pBlockNew->height = pFbAllocInfo->height;
        pBlockNew->width = pFbAllocInfo->width;
    }

    *pHwResource = &pBlockNew->hwResource;

    // Remember memory descriptor
    memdescDescribe(pMemDesc, ADDR_FBMEM, allocatedOffset, adjustedSize);
    pBlockNew->pMemDesc = pMemDesc;
    pBlockNew->allocedMemDesc = bAllocedMemdesc;

    status = NV_OK;

return_early:
    HEAP_VALIDATE(pHeap);

    if (bTurnBlacklistOff)
    {
        if (!hypervisorIsVgxHyper())
            _heapBlacklistChunks(pGpu, pHeap, &pHeap->blackList, desiredOffset, pVidHeapAlloc->size);
        else
            _heapBlacklistChunksInFreeBlocks(pGpu, pHeap);
    }

    return status;
}

static void _heapBlacklistChunksInFreeBlocks
(
    OBJGPU *pGpu,
    Heap   *pHeap
)
{
    MEM_BLOCK *pBlockFirstFree, *pBlockFree;
    NvU64 blockLo;
    NvU64 blockHi;
    NvU64 size;

    pBlockFirstFree = pHeap->pFreeBlockList;

    if (pBlockFirstFree)
    {
        pBlockFirstFree = pBlockFirstFree->u0.prevFree;
        pBlockFree = pBlockFirstFree;
        do
        {
            pBlockFree = pBlockFree->u1.nextFree;
            blockLo    = pBlockFree->begin;
            blockHi    = pBlockFree->end;
            size       = blockHi - blockLo + 1;

            _heapBlacklistChunks(pGpu, pHeap, &pHeap->blackList, blockLo, size);

        } while (pBlockFree != pBlockFirstFree);
    }
}

static NV_STATUS _heapBlockFree
(
    OBJGPU      *pGpu,
    Heap        *pHeap,
    NvHandle     hClient,
    NvHandle     hDevice,
    MEM_BLOCK   *pBlock
)
{
    MEM_BLOCK       *pBlockTmp;
    NvU32            i;
    MemoryManager   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool           bBlocksMerged  = NV_FALSE;

    //
    // Check for valid owner.
    //
    if (pBlock->owner == NVOS32_BLOCK_TYPE_FREE)
        return NV_ERR_INVALID_STATE;

    pBlock->owner = NVOS32_BLOCK_TYPE_FREE;

    if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_FREE_STATE_CHANGED))
    {
        return NV_ERR_INVALID_STATE;
    }

    //
    // Update free count.
    //
    _heapAdjustFree(pHeap, pBlock->end - pBlock->begin + 1);

    //
    // Release any HW resources that might've been in use
    //
    {
        FB_ALLOC_INFO        *pFbAllocInfo       = NULL;
        FB_ALLOC_PAGE_FORMAT *pFbAllocPageFormat = NULL;

        pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
        if (pFbAllocInfo == NULL)
        {
            NV_ASSERT(0);
            return NV_ERR_NO_MEMORY;
        }

        pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
        if (pFbAllocPageFormat == NULL) {
            NV_ASSERT(0);
            portMemFree(pFbAllocInfo);
            return NV_ERR_NO_MEMORY;
        }

        portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
        portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
        pFbAllocInfo->pageFormat = pFbAllocPageFormat;

        pFbAllocInfo->pageFormat->type = pBlock->u0.type;
        pFbAllocInfo->hwResId = pBlock->hwResource.hwResId;
        pFbAllocInfo->height = 0;
        pFbAllocInfo->pitch = 0;
        pFbAllocInfo->size = pBlock->end - pBlock->begin + 1;
        pFbAllocInfo->align = pBlock->align;
        pFbAllocInfo->alignPad = pBlock->alignPad;
        pFbAllocInfo->offset = pBlock->begin;
        pFbAllocInfo->format = pBlock->format;
        pFbAllocInfo->comprCovg = pBlock->hwResource.comprCovg;
        pFbAllocInfo->zcullCovg = 0;
        pFbAllocInfo->pageFormat->attr  = pBlock->hwResource.attr;
        pFbAllocInfo->pageFormat->attr2 = pBlock->hwResource.attr2;
        pFbAllocInfo->ctagOffset = pBlock->hwResource.ctagOffset;
        pFbAllocInfo->hClient = hClient;
        pFbAllocInfo->hDevice = hDevice;

        memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);

        if (FLD_TEST_DRF(OS32, _ATTR2, _INTERNAL, _YES, pFbAllocInfo->pageFormat->attr2))
        {
            osInternalReserveFreeCallback(pFbAllocInfo->offset, pGpu->gpuId);
        }

        // Clear the HW resource associations since this block can be reused or merged.
        portMemSet(&pBlock->hwResource, 0, sizeof(pBlock->hwResource));

        portMemFree(pFbAllocPageFormat);
        portMemFree(pFbAllocInfo);
    }

    if ((pBlock->u0.type == NVOS32_TYPE_TEXTURE) && (pBlock->textureId != 0))
    {
        for (i = 0; i < MAX_TEXTURE_CLIENT_IDS; i++)
        {
            //
            // 1. Find the client within the textureData structure
            // 2. Once found, set the value to 0
            // 3. Then decrement its refCount
            // 4. If refCount goes to zero, reset the textureData structure
            //    that pertains to that index.
            //
            if (pHeap->textureData[i].clientId == pBlock->textureId)
            {
                pBlock->textureId = 0;
                pHeap->textureData[i].refCount--;
                if (pHeap->textureData[i].refCount == 0)
                    portMemSet(&pHeap->textureData[i], 0,
                               sizeof(TEX_INFO));
                break;
            }
        }
    }

    // Account for freeing any reserved RM region
    if ((pBlock->u0.type == NVOS32_TYPE_RESERVED) && (pBlock->owner == HEAP_OWNER_RM_RESERVED_REGION))
    {
        NV_ASSERT(pHeap->reserved >= pBlock->end - pBlock->begin + 1);
        pHeap->reserved -= pBlock->end - pBlock->begin + 1;
    }

    //
    //
    // Can this merge with any surrounding free blocks?
    //
    if ((pBlock->prev->owner == NVOS32_BLOCK_TYPE_FREE) && (pBlock != pHeap->pBlockList))
    {
        //
        // Remove block to be freed and previous one since nodes will be
        // combined into single one.
        //
        if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_REMOVE))
        {
            return NV_ERR_INVALID_STATE;
        }
        if (NV_OK != _heapUpdate(pHeap, pBlock->prev, BLOCK_REMOVE))
        {
            return NV_ERR_INVALID_STATE;
        }

        //
        // Merge with previous block.
        //
        pBlock->prev->next = pBlock->next;
        pBlock->next->prev = pBlock->prev;
        pBlock->prev->end  = pBlock->end;
        pBlockTmp = pBlock;
        pBlock    = pBlock->prev;
        portMemFree(pBlockTmp);

        // re-insert updated free block into rb-tree
        if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_SIZE_CHANGED))
        {
            return NV_ERR_INVALID_STATE;
        }

        bBlocksMerged = NV_TRUE;
    }

    if ((pBlock->next->owner == NVOS32_BLOCK_TYPE_FREE) && (pBlock->next != pHeap->pBlockList))
    {
        //
        // Remove block to be freed and next one since nodes will be
        // combined into single one.
        //
        if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_REMOVE))
        {
            return NV_ERR_INVALID_STATE;
        }
        if (NV_OK != _heapUpdate(pHeap, pBlock->next, BLOCK_REMOVE))
        {
            return NV_ERR_INVALID_STATE;
        }

        //
        // Merge with next block.
        //
        pBlock->prev->next    = pBlock->next;
        pBlock->next->prev    = pBlock->prev;
        pBlock->next->begin   = pBlock->begin;

        if (pHeap->pBlockList == pBlock)
            pHeap->pBlockList  = pBlock->next;

        if (bBlocksMerged)
        {
            if (pHeap->pFreeBlockList == pBlock)
                pHeap->pFreeBlockList  = pBlock->u1.nextFree;

            pBlock->u1.nextFree->u0.prevFree = pBlock->u0.prevFree;
            pBlock->u0.prevFree->u1.nextFree = pBlock->u1.nextFree;
        }

        pBlockTmp = pBlock;
        pBlock    = pBlock->next;
        portMemFree(pBlockTmp);

        // re-insert updated free block into rb-tree
        if (NV_OK != _heapUpdate(pHeap, pBlock, BLOCK_SIZE_CHANGED))
        {
            return NV_ERR_INVALID_STATE;
        }

        bBlocksMerged = NV_TRUE;
    }

    if (!bBlocksMerged)
    {
        //
        // Nothing was merged.  Add to free list.
        //
        pBlockTmp = pHeap->pFreeBlockList;
        if (!pBlockTmp)
        {
            pHeap->pFreeBlockList = pBlock;
            pBlock->u1.nextFree      = pBlock;
            pBlock->u0.prevFree      = pBlock;
        }
        else
        {
            if (pBlockTmp->begin > pBlock->begin)
                //
                // Insert into beginning of free list.
                //
                pHeap->pFreeBlockList = pBlock;
            else if (pBlockTmp->u0.prevFree->begin > pBlock->begin)
                //
                // Insert into free list.
                //
                do
                {
                    pBlockTmp = pBlockTmp->u1.nextFree;
                } while (pBlockTmp->begin < pBlock->begin);
                /*
            else
                 * Insert at end of list.
                 */
            pBlock->u1.nextFree = pBlockTmp;
            pBlock->u0.prevFree = pBlockTmp->u0.prevFree;
            pBlock->u0.prevFree->u1.nextFree = pBlock;
            pBlockTmp->u0.prevFree           = pBlock;
        }
    }

    pBlock->mhandle = 0x0;
    pBlock->align   = pBlock->begin;
    pBlock->alignPad = 0;
    pBlock->format  = 0;

    HEAP_VALIDATE(pHeap);
    return (NV_OK);
}

NV_STATUS heapReference_IMPL
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    NvU32                owner,
    MEMORY_DESCRIPTOR  *pMemDesc
)
{
    NvU64       offsetAlign = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    MEM_BLOCK  *pBlock;

    // Bail out in case allocation is in PMA owned FB region.
    if (pMemDesc->pPmaAllocInfo)
    {
        if (0 != pMemDesc->pPmaAllocInfo->refCount)
        {
            pMemDesc->pPmaAllocInfo->refCount++;
            if (IsSLIEnabled(pGpu) &&
                (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
            {                        //
                memdescAddRef(pMemDesc); // Otherwise we have a fake parent descriptor removed with existing submem descriptors.
                                     // In SLI only (not fully understood yet!). In non SLI, that memAddref() causes a memleak.
                                     //
            }
        }
        return NV_OK;
    }

    if (owner == NVOS32_BLOCK_TYPE_FREE)
        return NV_ERR_INVALID_STATE;

    pBlock = _heapFindAlignedBlockWithOwner(pGpu, pHeap, owner, offsetAlign);

    if (!pBlock)
        return NV_ERR_INVALID_OFFSET;

    if (pBlock->refCount == HEAP_MAX_REF_COUNT)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "heapReference: reference count %x will exceed maximum 0x%x:\n",
                  pBlock->refCount, HEAP_MAX_REF_COUNT);
        return NV_ERR_GENERIC;
    }

    pBlock->refCount++;
    if (IsSLIEnabled(pGpu) &&
        (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
    {                        //
        memdescAddRef(pMemDesc); // Otherwise we have a fake parent descriptor removed with existing submem descriptors.
                             // In SLI only (not fully understood yet!). In non SLI, that memAddref() causes a memleak.
                             //
    }
    return NV_OK;
}

static NV_STATUS
_heapFindBlockByOffset
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    NvU32               owner,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU64               offset,
    MEM_BLOCK         **ppBlock
)
{
    NV_STATUS status;

    // IRQL TEST: must be running at equivalent of passive-level
    NV_ASSERT_OR_RETURN(!osIsRaisedIRQL(), NV_ERR_INVALID_IRQ_LEVEL);

    *ppBlock = _heapFindAlignedBlockWithOwner(pGpu, pHeap, owner,
                                              offset);

    if (!*ppBlock)
    {
        // Try finding block based solely on offset.  This is primarily needed
        // to successfully locate a block that was allocated multiple times via
        // NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE in heapAlloc:  there can
        // be multiple owners, so that _heapFindAlignedBlockWithOwner may fail.
        if ((status = heapGetBlock(pHeap, offset, ppBlock)) != NV_OK
                || !*ppBlock)
            return NV_ERR_INVALID_OFFSET;
    }

    return NV_OK;
}

NV_STATUS
heapFree_IMPL
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    NvHandle            hClient,
    NvHandle            hDevice,
    NvU32               owner,
    MEMORY_DESCRIPTOR  *pMemDesc
)
{
    NV_STATUS   status;
    MEM_BLOCK  *pBlock;
    MEM_BLOCK  *pNextBlock;
    NvU64       offsetAlign       = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    NvU64       allocBegin        = 0;
    NvU64       allocEnd          = 0;
    NvBool      bTurnBlacklistOff = NV_FALSE;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pMemDesc->pHeap == pHeap, NV_ERR_INVALID_ARGUMENT);

    if (memdescGetContiguity(pMemDesc, AT_GPU))
    {
        status = _heapFindBlockByOffset(pGpu, pHeap,
                                        owner, pMemDesc, offsetAlign,
                                        &pBlock);
        if (NV_OK != status)
        {
            return status;
        }

        if (pBlock->allocedMemDesc)
        {
            if (pMemDesc != pBlock->pMemDesc)
            {
                NV_ASSERT(pMemDesc == pBlock->pMemDesc);
                return NV_ERR_INVALID_ARGUMENT;
            }

            // Clear only if the memdesc is about to be freed by memdescDestroy()
            if (pMemDesc->RefCount == 1)
            {
                pBlock->pMemDesc = NULL;
            }

            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
        }

        if (--pBlock->refCount != 0)
            return NV_OK;


        if(pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) &&
            gpuCheckPageRetirementSupport_HAL(pGpu))
        {
            if (FLD_TEST_DRF(OS32, _ATTR2, _BLACKLIST, _OFF, pBlock->hwResource.attr2))
            {
                    bTurnBlacklistOff = NV_TRUE;
                    allocBegin        = pBlock->begin;
                    allocEnd          = pBlock->end;
            }
        }

        //
        // Since _heapBlockFree() unconditionally releases HW resources
        // such as compression tags, some memory descriptor fields
        // are rendered stale.  These fields need to be reset to safer
        // default values (e.g. invalid HW resource ID, pitch PTE
        // kind, etc.) - they may be referenced again before the memory
        // descriptor itself is freed.
        //
        if (pBlock->allocedMemDesc && (pBlock->pMemDesc != NULL))
        {
            memdescSetHwResId(pMemDesc, 0);
            // XXX We cannot reset the PTE kind here since it cause corruption
            // in RAGE. See bug 949059
            //
            // This is an ugly hack to help OGL recover from modeswitch.
            // A cleaner fix would be to change the way memory is managed in OGL,
            // but it doesn't worth the effort to fix that on XP, since the OS is
            // close to end of life. The OGL linux team have plan to change their
            // memory management in the future, so later this hack may not be
            // required anymore
            // pMemDesc->PteKind = 0;
        }

        if ((status = _heapBlockFree(pGpu, pHeap, hClient, hDevice, pBlock)) != NV_OK)
        {
            NV_ASSERT(0);
        }

        //
        // since the mem desc  is freed, now we can reallocate the blacklisted pages
        // in the [allocBegin, allocEnd]
        //
        if (bTurnBlacklistOff)
            status = _heapBlacklistChunks(pGpu, pHeap, &pHeap->blackList, allocBegin, allocEnd-allocBegin+1);

        if (pMemoryManager->bEnableDynamicPageOfflining)
        {
            NvU32            i = 0;
            BLACKLIST       *pBlacklist       = &pHeap->blackList;
            BLACKLIST_CHUNK *pBlacklistChunks = pBlacklist->pBlacklistChunks;

            for (i = 0; i < pBlacklist->count; i++)
            {
                if (pBlacklistChunks[i].bPendingRetirement &&
                   (pBlacklistChunks[i].physOffset >= allocBegin &&
                   pBlacklistChunks[i].physOffset <= allocEnd))
                {
                    status = _heapBlacklistSingleChunk(pGpu, pHeap, &pBlacklist->pBlacklistChunks[i]);
                    if (NV_OK != status)
                    {
                        NV_PRINTF(LEVEL_ERROR, "heapBlacklistSingleChunk, status: %x!\n", status);
                        return status;
                    }
                }
            }
        }
        return status;
    }
    else
    {
        NvBool bBlacklistFailed = NV_FALSE;
        //
        // Use the pMemDesc->PteArray[0] to find the first block
        // The remaining blocks can be found from each block's
        // noncontigAllocListNext pointer
        //
        status = _heapFindBlockByOffset(pGpu, pHeap,
                owner, pMemDesc,
                memdescGetPte(pMemDesc, AT_GPU, 0), &pBlock);

        if (NV_OK != status)
        {
            return status;
        }

        while (pBlock != NULL)
        {
            // _heapBlockFree() clears pBlock, so save the next pointer
            pNextBlock = pBlock->noncontigAllocListNext;

            if (--pBlock->refCount != 0)
            {
                // Remove this block from the noncontig allocation list
                pBlock->noncontigAllocListNext = NULL;
                pBlock = pNextBlock;
                continue;
            }

            if (NV_OK != (status = _heapBlockFree(pGpu, pHeap, hClient, hDevice, pBlock)))
                return status;

            // check if we need to dynamically blacklist the page
            if (pMemoryManager->bEnableDynamicPageOfflining)
            {
                NvU32            i = 0;
                BLACKLIST       *pBlacklist       = &pHeap->blackList;
                BLACKLIST_CHUNK *pBlacklistChunks = pBlacklist->pBlacklistChunks;
                for (i = 0; i < pBlacklist->count; i++)
                {
                    if (pBlacklistChunks[i].bPendingRetirement &&
                    (pBlacklistChunks[i].physOffset >= pBlock->begin &&
                    pBlacklistChunks[i].physOffset <= pBlock->end))
                    {
                        status = _heapBlacklistSingleChunk(pGpu, pHeap, &pBlacklist->pBlacklistChunks[i]);
                        if (NV_OK != status)
                        {
                            NV_PRINTF(LEVEL_ERROR, "heapBlacklistSingleChunk, status: %x!\n", status);
                            bBlacklistFailed = NV_TRUE;
                        }
                    }
                }
            }
            pBlock = pNextBlock;
        }

        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);

        if (bBlacklistFailed)
        {
            return NV_ERR_INVALID_STATE;
        }
        else
        {
            return status;
        }
    }
}

NV_STATUS heapGetBlock_IMPL
(
    Heap       *pHeap,
    NvU64       offset,
    MEM_BLOCK **ppMemBlock
)
{
    NODE *pNode;

    if (btreeSearch(offset, &pNode, pHeap->pBlockTree) != NV_OK)
    {
        if (ppMemBlock)
        {
            *ppMemBlock = NULL;
        }
        return NV_ERR_GENERIC;
    }

    if (ppMemBlock)
    {
        *ppMemBlock = (MEM_BLOCK *)pNode->Data;
    }

    return NV_OK;
}

static MEM_BLOCK *_heapFindAlignedBlockWithOwner
(
    OBJGPU   *pGpu,
    Heap     *pHeap,
    NvU32     owner,
    NvU64     offset // aligned
)
{
    MEM_BLOCK  *pBlock;
    NODE       *pNode;

    HEAP_VALIDATE(pHeap);

    if (btreeSearch(offset, &pNode, pHeap->pBlockTree) != NV_OK)
    {
        return NULL;
    }

    pBlock = (MEM_BLOCK *)pNode->Data;
    if (pBlock->owner != owner)
    {
        return NULL;
    }

    return pBlock;
}

NV_STATUS heapGetSize_IMPL
(
    Heap  *pHeap,
    NvU64 *size
)
{
    *size = pHeap->total;
    HEAP_VALIDATE(pHeap);
    return (NV_OK);
}

NV_STATUS heapGetUsableSize_IMPL
(
    Heap  *pHeap,
    NvU64 *usableSize
)
{
    *usableSize = pHeap->total - pHeap->reserved;
    HEAP_VALIDATE(pHeap);
    return (NV_OK);
}

NV_STATUS heapGetFree_IMPL
(
    Heap  *pHeap,
    NvU64 *free
)
{
    *free = pHeap->free;
    HEAP_VALIDATE(pHeap);
    return (NV_OK);
}

NV_STATUS heapGetBase_IMPL
(
    Heap  *pHeap,
    NvU64 *base
)
{
    *base = pHeap->base;
    HEAP_VALIDATE(pHeap);
    return (NV_OK);
}

static NV_STATUS _heapGetMaxFree
(
    Heap  *pHeap,
    NvU64 *maxOffset,
    NvU64 *maxFree
)
{
    MEM_BLOCK  *pBlockFirstFree, *pBlockFree;
    NvU64       freeBlockSize;

    *maxFree = 0;

    pBlockFirstFree = pHeap->pFreeBlockList;
    if (!pBlockFirstFree)
        // There are no free blocks. Max free is already set to 0
        return (NV_OK);

    // Walk the free block list.
    pBlockFree = pBlockFirstFree;
    do {
        freeBlockSize = pBlockFree->end - pBlockFree->begin + 1;
        if (freeBlockSize > *maxFree)
        {
            *maxOffset = pBlockFree->begin;
            *maxFree = freeBlockSize;
        }
        pBlockFree = pBlockFree->u1.nextFree;
    } while (pBlockFree != pBlockFirstFree);

    return (NV_OK);
}

NV_STATUS heapInfo_IMPL
(
    Heap  *pHeap,
    NvU64 *bytesFree,
    NvU64 *bytesTotal,
    NvU64 *base,
    NvU64 *largestOffset,      // largest free blocks offset
    NvU64 *largestFree         // largest free blocks size
)
{
    NV_STATUS status;

    *bytesFree  = pHeap->free;
    *bytesTotal = pHeap->total - pHeap->reserved;
    *base  = pHeap->base;
    status = _heapGetMaxFree(pHeap, largestOffset, largestFree);
    HEAP_VALIDATE(pHeap);

    return status;
}

void
heapGetClientAddrSpaceSize_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap,
    NvU64  *pSize
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    FB_REGION_DESCRIPTOR   *pFbRegion;
    NvU64 highestAddr = 0;

    //
    // Iterate the regions that are allocatable to the client -
    // non-reserved and non RM-internal - same logic as
    // heapInitInternal_IMPL used to remove memory
    //
    for (NvU32 i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        pFbRegion = &pMemoryManager->Ram.fbRegion[i];

        // If the region is reserved or internal, ignore it here
        if (!(pFbRegion->bRsvdRegion || pFbRegion->bInternalHeap))
        {
            // Skip regions which are outside the heap boundaries
            if (pFbRegion->base < pHeap->base && pFbRegion->limit < pHeap->base)
            {
                continue;
            }

            highestAddr = NV_MAX(highestAddr, pFbRegion->limit);
        }
    }

    // should end in 0xFFFF as a limit
    NV_ASSERT((highestAddr & RM_PAGE_MASK) != 0);
    HEAP_VALIDATE(pHeap);

    *pSize = highestAddr + 1;
}

NV_STATUS heapAllocHint_IMPL
(
    OBJGPU                 *pGpu,
    Heap                   *pHeap,
    NvHandle                hClient,
    NvHandle                hDevice,
    HEAP_ALLOC_HINT_PARAMS *pAllocHint
)
{
    MemoryManager          *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64                   alignment;
    NV_STATUS               status;
    FB_ALLOC_INFO          *pFbAllocInfo        = NULL;
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat  = NULL;
    NvU64                   pageSize            = 0;
    NvU32                   flags;

    // Check for valid size.
    NV_ASSERT_OR_RETURN((pAllocHint->pSize != NULL), NV_ERR_INVALID_ARGUMENT);

    // Ensure a valid allocation type was passed in
    NV_ASSERT_OR_RETURN((pAllocHint->type < NVOS32_NUM_MEM_TYPES), NV_ERR_INVALID_ARGUMENT);

    // As we will dereference these two later, we should not allow NULL value.
    NV_ASSERT_OR_RETURN(((pAllocHint->pHeight != NULL) && (pAllocHint->pAttr != NULL)), NV_ERR_INVALID_ARGUMENT);

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pFbAllocInfo == NULL)
    {
        NV_ASSERT(0);
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    if (pFbAllocPageFormat == NULL) {
        NV_ASSERT(0);
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    pFbAllocInfo->pageFormat->type  = pAllocHint->type;
    pFbAllocInfo->hwResId       = 0;
    pFbAllocInfo->pad           = 0;
    pFbAllocInfo->height        = *pAllocHint->pHeight;
    pFbAllocInfo->width         = *pAllocHint->pWidth;
    pFbAllocInfo->pitch         = (pAllocHint->pPitch) ? (*pAllocHint->pPitch) : 0;
    pFbAllocInfo->size          = *pAllocHint->pSize;
    pFbAllocInfo->pageFormat->kind  = 0;
    pFbAllocInfo->offset        = ~0;
    pFbAllocInfo->hClient       = hClient;
    pFbAllocInfo->hDevice       = hDevice;
    pFbAllocInfo->pageFormat->flags = pAllocHint->flags;
    pFbAllocInfo->pageFormat->attr  = *pAllocHint->pAttr;
    pFbAllocInfo->retAttr       = *pAllocHint->pAttr;
    pFbAllocInfo->pageFormat->attr2 = *pAllocHint->pAttr2;
    pFbAllocInfo->retAttr2      = *pAllocHint->pAttr2;
    pFbAllocInfo->format        = 0;

    if ((pAllocHint->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
        (pAllocHint->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        pFbAllocInfo->align = *pAllocHint->pAlignment;
    else
        pFbAllocInfo->align = RM_PAGE_SIZE;

    // Fetch RM page size
    pageSize = memmgrDeterminePageSize(pMemoryManager, pFbAllocInfo->hClient, pFbAllocInfo->size,
                                       pFbAllocInfo->format, pFbAllocInfo->pageFormat->flags,
                                       &pFbAllocInfo->retAttr, &pFbAllocInfo->retAttr2);
    if (pageSize == 0)
    {
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed, status: 0x%x\n", status);
        goto exit;
    }

    // Fetch memory alignment
    status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &pFbAllocInfo->size, &pFbAllocInfo->align,
                                               pFbAllocInfo->alignPad, pFbAllocInfo->pageFormat->flags,
                                               pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memmgrAllocDetermineAlignment failed, status: 0x%x\n", status);
        goto exit;
    }

    //
    // Call into HAL to reserve any hardware resources for
    // the specified memory type.
    // If the alignment was changed due to a HW limitation, and the
    // flag NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE is set, bad_argument
    // will be passed back from the HAL
    //
    flags = pFbAllocInfo->pageFormat->flags;
    pFbAllocInfo->pageFormat->flags |= NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC;
    status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    pFbAllocInfo->pageFormat->flags = flags;
    *pAllocHint->pAttr  = pFbAllocInfo->retAttr;
    *pAllocHint->pAttr2 = pFbAllocInfo->retAttr2;
    *pAllocHint->pKind  = pFbAllocInfo->pageFormat->kind;

    // Save retAttr as Possible Attributes that have passed error checking and
    // clear retAttr because we have not allocated them yet
    pFbAllocInfo->possAttr = pFbAllocInfo->retAttr;
    // pFbAllocInfo->possAttr2 = pFbAllocInfo->retAttr2;
    pFbAllocInfo->retAttr = 0x0;
    pFbAllocInfo->retAttr2 = 0x0;
    if (status != NV_OK)
    {
        //
        // probably means we passed in a bogus type or no tiling resources available
        // when tiled memory attribute was set to REQUIRED
        //
        NV_PRINTF(LEVEL_ERROR, "memmgrAllocHwResources failed, status: 0x%x\n",
                  status);
        goto exit;
    }

    //
    // Refresh search parameters.
    //
    if ((DRF_VAL(OS32, _ATTR, _FORMAT, *pAllocHint->pAttr) != NVOS32_ATTR_FORMAT_BLOCK_LINEAR))
    {
        *pAllocHint->pHeight     = pFbAllocInfo->height;
        if (pAllocHint->pPitch)
            *pAllocHint->pPitch  = pFbAllocInfo->pitch;
    }

    //
    // The heap allocator has assumed required alignments are powers of 2
    // (aligning FB offsets has been done using bit masks).
    //
    //
    *pAllocHint->pAlignment = pFbAllocInfo->align + 1;      // convert mask to size
    alignment = pFbAllocInfo->align + 1;

    //
    // Allow caller to request host page alignment to make it easier
    // to move things around with host os VM subsystem
    //

    if (pAllocHint->flags & NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        NvU64   hostPageSize = pSys->cpuInfo.hostPageSize;

        // hostPageSize *should* always be set, but....
        if (hostPageSize == 0)
            hostPageSize = RM_PAGE_SIZE;

        alignment = memUtilsLeastCommonAlignment(alignment, hostPageSize);
    }

    if (memmgrAllocGetAddrSpace(pMemoryManager, pAllocHint->flags, *pAllocHint->pAttr) == ADDR_FBMEM)
    {
        if (alignment >= pHeap->total)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_PRINTF(LEVEL_ERROR, "heapAllocHint failed due to alignmend >= pHeap->total\n");
            goto exit;
        }
    }

    *pAllocHint->pHeight = pFbAllocInfo->height;
    pAllocHint->pad = pFbAllocInfo->pad;

    *pAllocHint->pSize = pFbAllocInfo->size;           // returned to caller

exit:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

NV_STATUS heapHwAlloc_IMPL
(
    OBJGPU         *pGpu,
    Heap           *pHeap,
    NvHandle        hClient,
    NvHandle        hDevice,
    NvHandle        hMemory,
    MEMORY_HW_RESOURCES_ALLOCATION_REQUEST *pHwAlloc,
    NvU32           *pAttr,
    NvU32           *pAttr2
)
{
    MemoryManager          *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS               status = NV_OK;
    FB_ALLOC_INFO          *pFbAllocInfo = NULL;
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat = NULL;
    NvU64                   pageSize = 0;
    NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS *pUserParams = pHwAlloc->pUserParams;

    // Ensure a valid allocation type was passed in
    if (pUserParams->type > NVOS32_NUM_MEM_TYPES - 1)
        return NV_ERR_GENERIC;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (NULL == pFbAllocInfo)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for Resource %p\n",
                  pHwAlloc->pHandle);
        status = NV_ERR_GENERIC;
        goto failed;
    }
    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    if (NULL == pFbAllocPageFormat)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for Resource %p\n",
                  pHwAlloc->pHandle);
        status = NV_ERR_GENERIC;
        goto failed;
    }

    portMemSet(pFbAllocInfo, 0x0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0x0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat        = pFbAllocPageFormat;
    pFbAllocInfo->pageFormat->type  = pUserParams->type;
    pFbAllocInfo->hwResId       = 0;
    pFbAllocInfo->pad           = 0;
    pFbAllocInfo->height        = pUserParams->height;
    pFbAllocInfo->width         = pUserParams->width;
    pFbAllocInfo->pitch         = pUserParams->pitch;
    pFbAllocInfo->size          = pUserParams->size;
    pFbAllocInfo->origSize      = pUserParams->size;
    pFbAllocInfo->pageFormat->kind  = pUserParams->kind;
    pFbAllocInfo->offset        = memmgrGetInvalidOffset_HAL(pGpu, pMemoryManager);
    pFbAllocInfo->hClient       = hClient;
    pFbAllocInfo->hDevice       = hDevice;
    pFbAllocInfo->pageFormat->flags = pUserParams->flags;
    pFbAllocInfo->pageFormat->attr  = pUserParams->attr;
    pFbAllocInfo->pageFormat->attr2 = pUserParams->attr2;
    pFbAllocInfo->retAttr       = pUserParams->attr;
    pFbAllocInfo->retAttr2      = pUserParams->attr2;
    pFbAllocInfo->comprCovg     = pUserParams->comprCovg;
    pFbAllocInfo->zcullCovg     = 0;
    pFbAllocInfo->internalflags = 0;

    if ((pUserParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
        (pUserParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        pFbAllocInfo->align = pUserParams->alignment;
    else
        pFbAllocInfo->align = RM_PAGE_SIZE;

    // Fetch RM page size
    pageSize = memmgrDeterminePageSize(pMemoryManager, pFbAllocInfo->hClient, pFbAllocInfo->size,
                                       pFbAllocInfo->format, pFbAllocInfo->pageFormat->flags,
                                       &pFbAllocInfo->retAttr, &pFbAllocInfo->retAttr2);
    if (pageSize == 0)
    {
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed\n");
    }

    // Fetch memory alignment
    status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &pFbAllocInfo->size, &pFbAllocInfo->align,
                                               pFbAllocInfo->alignPad, pFbAllocInfo->pageFormat->flags,
                                               pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memmgrAllocDetermineAlignment failed\n");
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //
    if ((status == NV_OK) && IS_VIRTUAL(pGpu))
    {
        if (vgpuIsGuestManagedHwAlloc(pGpu) &&
            (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, pFbAllocInfo->pageFormat->attr)))
        {
            status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
            pHwAlloc->hwResource.isVgpuHostAllocated = NV_FALSE;
            NV_ASSERT(status == NV_OK);
        }
        else
        {
            NV_RM_RPC_MANAGE_HW_RESOURCE_ALLOC(pGpu,
                                               hClient,
                                               hDevice,
                                               hMemory,
                                               pFbAllocInfo,
                                               status);
            pHwAlloc->hwResource.isVgpuHostAllocated = NV_TRUE;
        }

        pUserParams->uncompressedKind      = pFbAllocInfo->uncompressedKind;
        pUserParams->compPageShift         = pFbAllocInfo->compPageShift;
        pUserParams->compressedKind        = pFbAllocInfo->compressedKind;
        pUserParams->compTagLineMin        = pFbAllocInfo->compTagLineMin;
        pUserParams->compPageIndexLo       = pFbAllocInfo->compPageIndexLo;
        pUserParams->compPageIndexHi       = pFbAllocInfo->compPageIndexHi;
        pUserParams->compTagLineMultiplier = pFbAllocInfo->compTagLineMultiplier;
    }
    else
    {
        //
        // Call into HAL to reserve any hardware resources for
        // the specified memory type.
        // If the alignment was changed due to a HW limitation, and the
        // flag NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE is set, bad_argument
        // will be passed back from the HAL
        //
        status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    }

    // Is status bad or did we request attributes and they failed
    if ((status != NV_OK) || ((pUserParams->attr) && (0x0 == pFbAllocInfo->retAttr)))
    {
        //
        // probably means we passed in a bogus type or no tiling resources available
        // when tiled memory attribute was set to REQUIRED
        //
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "nvHalFbAlloc failure status = 0x%x Requested Attr 0x%x!\n",
                      status, pUserParams->attr);
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING,
                      "nvHalFbAlloc Out of Resources Requested=%x Returned=%x !\n",
                      pUserParams->attr, pFbAllocInfo->retAttr);
        }
        goto failed;
    }

    //
    // Refresh search parameters.
    //
    pUserParams->pitch  = pFbAllocInfo->pitch;

    pUserParams->height = pFbAllocInfo->height;
    pHwAlloc->pad = NvU64_LO32(pFbAllocInfo->pad);
    pUserParams->kind = pFbAllocInfo->pageFormat->kind;
    pHwAlloc->hwResId = pFbAllocInfo->hwResId;

    pUserParams->size = pFbAllocInfo->size;           // returned to caller

    pHwAlloc->hwResource.attr = pFbAllocInfo->retAttr;
    pHwAlloc->hwResource.attr2 = pFbAllocInfo->retAttr2;
    pHwAlloc->hwResource.comprCovg = pFbAllocInfo->comprCovg;
    pHwAlloc->hwResource.ctagOffset = pFbAllocInfo->ctagOffset;
    pHwAlloc->hwResource.hwResId = pFbAllocInfo->hwResId;

    *pAttr  = pFbAllocInfo->retAttr;
    *pAttr2 = pFbAllocInfo->retAttr2;

failed:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

void heapHwFree_IMPL
(
    OBJGPU   *pGpu,
    Heap     *pHeap,
    Memory   *pMemory,
    NvU32     flags
)
{
    MemoryManager        *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    FB_ALLOC_INFO        *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT *pFbAllocPageFormat = NULL;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pFbAllocInfo == NULL)
    {
        NV_ASSERT(0);
        goto exit;
    }

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    if (pFbAllocPageFormat == NULL) {
        NV_ASSERT(0);
        goto exit;
    }

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    pFbAllocInfo->pageFormat->type = pMemory->Type;
    pFbAllocInfo->pageFormat->attr  = pMemory->pHwResource->attr;
    pFbAllocInfo->pageFormat->attr2 = pMemory->pHwResource->attr2;
    pFbAllocInfo->hwResId = pMemory->pHwResource->hwResId;
    pFbAllocInfo->size = pMemory->Length;
    pFbAllocInfo->format = memdescGetPteKind(pMemory->pMemDesc);
    pFbAllocInfo->offset = ~0;
    pFbAllocInfo->hClient = RES_GET_CLIENT_HANDLE(pMemory);
    pFbAllocInfo->hDevice = RES_GET_HANDLE(pMemory->pDevice);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //

    if (IS_VIRTUAL(pGpu))
    {
        if (vgpuIsGuestManagedHwAlloc(pGpu) && !pMemory->pHwResource->isVgpuHostAllocated)
        {
            memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
        }
        else
        {
            NV_STATUS rmStatus = NV_OK;

            NV_RM_RPC_MANAGE_HW_RESOURCE_FREE(pGpu,
                    RES_GET_CLIENT_HANDLE(pMemory),
                    RES_GET_HANDLE(pMemory->pDevice),
                    RES_GET_HANDLE(pMemory),
                    flags,
                    rmStatus);
        }
    }
    else
    {
        memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    }

exit:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);
}

/*!
 * @brief: Adjust heap free accounting
 *
 * @param[in] pHeap         Heap pointer
 * @param[in] blockSize     +: Size of block being freed
 *                          -: Size of block being allocated
 * @param[in] internalHeap  NV_TRUE if the allocation is 'INTERNAL'
 *
 * @return                  void
 */

static void
_heapAdjustFree
(
    Heap     *pHeap,
    NvS64     blockSize
)
{
    pHeap->free += blockSize;

    NV_ASSERT(pHeap->free <= pHeap->total);
    if(pHeap->free > pHeap->total)
    {
        DBG_BREAKPOINT();
    }
}

static NV_STATUS
_heapProcessFreeBlock
(
    OBJGPU             *pGpu,
    MEM_BLOCK          *pBlockFree,
    MEM_BLOCK         **ppBlockNew,
    MEM_BLOCK         **ppBlockSplit,
    Heap               *pHeap,
    MEMORY_ALLOCATION_REQUEST *pAllocRequest,
    NvHandle            memHandle,
    OBJHEAP_ALLOC_DATA *pAllocData,
    FB_ALLOC_INFO      *pFbAllocInfo,
    NvU64               alignPad,
    NvU64              *offset
)
{
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc = pAllocRequest->pUserParams;
    MEM_BLOCK  *pBlockNew = NULL, *pBlockSplit = NULL;
    NV_STATUS   status    = NV_OK;

    if ((pAllocData->allocLo == pBlockFree->begin) &&
         (pAllocData->allocHi == pBlockFree->end))
    {
        //
        // Wow, exact match so replace free block.
        // Remove from free list.
        //
        pBlockFree->u1.nextFree->u0.prevFree = pBlockFree->u0.prevFree;
        pBlockFree->u0.prevFree->u1.nextFree = pBlockFree->u1.nextFree;

        if (pHeap->pFreeBlockList == pBlockFree)
        {
            //
            // This could be the last free block.
            //
            if (pBlockFree->u1.nextFree == pBlockFree)
                pHeap->pFreeBlockList = NULL;
            else
                pHeap->pFreeBlockList = pBlockFree->u1.nextFree;
        }

        //
        // Set pVidHeapAlloc->owner/pVidHeapAlloc->type values here.
        // Don't move because some fields are unions.
        //
        pBlockFree->owner    = pVidHeapAlloc->owner;
        pBlockFree->mhandle  = memHandle;
        pBlockFree->refCount = 1;
        pBlockFree->u0.type  = pVidHeapAlloc->type;
        pBlockFree->align    = pAllocData->allocAl;
        pBlockFree->alignPad  = alignPad;
        pBlockFree->format    = pFbAllocInfo->format;

        // tail end code below assumes 'blockNew' is the new block
        pBlockNew = pBlockFree;
    }
    else if ((pAllocData->allocLo >= pBlockFree->begin) &&
         (pAllocData->allocHi <= pBlockFree->end))
    {
        //
        // Found a fit.
        // It isn't exact, so we'll have to do a split
        //
        pBlockNew = portMemAllocNonPaged(sizeof(MEM_BLOCK));
        if (pBlockNew == NULL)
        {
            // Exit with failure and free any local allocations
            NV_ASSERT(0);
            status = NV_ERR_NO_MEMORY;
            goto _heapProcessFreeBlock_error;
        }

        portMemSet(pBlockNew, 0, sizeof(MEM_BLOCK));

        pBlockNew->owner     = pVidHeapAlloc->owner;
        pBlockNew->mhandle   = memHandle;
        pBlockNew->refCount  = 1;
        pBlockNew->u0.type   = pVidHeapAlloc->type;
        pBlockNew->begin     = pAllocData->allocLo;
        pBlockNew->align     = pAllocData->allocAl;
        pBlockNew->alignPad  = alignPad;
        pBlockNew->end       = pAllocData->allocHi;
        pBlockNew->format    = pFbAllocInfo->format;

        if (gpuIsCacheOnlyModeEnabled(pGpu))
        {
            //
            // In L2 Cache only mode, set the beginning of the new allocation
            // block to aligned (allocAl) offset rather then the start of
            // the free block (allocLo). And that the end of the new block is
            // is calculated as (allocSize - 1) from the beginning.
            // This insures that we don't "over allocate"  for the surface in the
            // case where start of the free block is not properly aligned for both
            // the grow down and grow up cases.
            // Only applying this in L2 cache mode for now, as we don't want to "waste"
            // L2 cache space, though wonder if there are any implications to doing
            // it this way in normal operation.
            //
            pBlockNew->begin = pAllocData->allocAl;
            pBlockNew->end   = pBlockNew->begin + pAllocData->allocSize - 1;
        }

        if ((pBlockFree->begin < pBlockNew->begin) &&
             (pBlockFree->end > pBlockNew->end))
        {
            // Split free block in two.
            pBlockSplit = portMemAllocNonPaged(sizeof(MEM_BLOCK));
            if (pBlockSplit == NULL)
            {
                // Exit with failure and free any local allocations
                status = NV_ERR_NO_MEMORY;
                goto _heapProcessFreeBlock_error;
            }

            portMemSet(pBlockSplit, 0, sizeof(MEM_BLOCK));

            // remove free block from rb-tree since node's range will be changed
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_REMOVE)))
            {
                // Exit with failure and free any local allocations
                goto _heapProcessFreeBlock_error;
            }

            pBlockSplit->owner = NVOS32_BLOCK_TYPE_FREE;
            pBlockSplit->format= 0;
            pBlockSplit->begin = pBlockNew->end + 1;
            pBlockSplit->align = pBlockSplit->begin;
            pBlockSplit->alignPad = 0;
            pBlockSplit->end   = pBlockFree->end;
            pBlockFree->end    = pBlockNew->begin - 1;
            //
            // Insert free split block into free list.
            //
            pBlockSplit->u1.nextFree = pBlockFree->u1.nextFree;
            pBlockSplit->u0.prevFree = pBlockFree;
            pBlockSplit->u1.nextFree->u0.prevFree = pBlockSplit;
            pBlockFree->u1.nextFree = pBlockSplit;
            //
            //  Insert new and split blocks into block list.
            //
            pBlockNew->next   = pBlockSplit;
            pBlockNew->prev   = pBlockFree;
            pBlockSplit->next = pBlockFree->next;
            pBlockSplit->prev = pBlockNew;
            pBlockFree->next  = pBlockNew;
            pBlockSplit->next->prev = pBlockSplit;

            // re-insert updated free block into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_SIZE_CHANGED)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR,
                          "_heapUpdate failed to _SIZE_CHANGE block\n");
                goto _heapProcessFreeBlock_exit;
            }

            // insert new and split blocks into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockNew, BLOCK_ADD)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR, "_heapUpdate failed to _ADD block\n");
                goto _heapProcessFreeBlock_exit;
            }

            if (NV_OK != (status = _heapUpdate(pHeap, pBlockSplit, BLOCK_ADD)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR, "_heapUpdate failed to _ADD block\n");
                goto _heapProcessFreeBlock_exit;
            }
        }
        else if (pBlockFree->end == pBlockNew->end)
        {
            // remove free block from rb-tree since node's range will be changed
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_REMOVE)))
            {
                // Exit with failure and free any local allocations
                goto _heapProcessFreeBlock_error;
            }

            //
            // New block inserted after free block.
            //
            pBlockFree->end = pBlockNew->begin - 1;
            pBlockNew->next = pBlockFree->next;
            pBlockNew->prev = pBlockFree;
            pBlockFree->next->prev = pBlockNew;
            pBlockFree->next       = pBlockNew;

            // re-insert updated free block into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_SIZE_CHANGED)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR,
                          "_heapUpdate failed to _SIZE_CHANGE block\n");
                goto _heapProcessFreeBlock_exit;
            }

            // insert new block into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockNew, BLOCK_ADD)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR, "_heapUpdate failed to _ADD block\n");
                goto _heapProcessFreeBlock_exit;
            }
        }
        else if (pBlockFree->begin == pBlockNew->begin)
        {
            // remove free block from rb-tree since node's range will be changed
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_REMOVE)))
            {
                // Exit with failure and free any local allocations
                goto _heapProcessFreeBlock_error;
            }

            //
            // New block inserted before free block.
            //
            pBlockFree->begin = pBlockNew->end + 1;
            pBlockFree->align = pBlockFree->begin;
            pBlockNew->next   = pBlockFree;
            pBlockNew->prev   = pBlockFree->prev;
            pBlockFree->prev->next = pBlockNew;
            pBlockFree->prev       = pBlockNew;
            if (pHeap->pBlockList == pBlockFree)
                pHeap->pBlockList  = pBlockNew;

            // re-insert updated free block into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockFree, BLOCK_SIZE_CHANGED)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR,
                          "_heapUpdate failed to _SIZE_CHANGE block\n");
                goto _heapProcessFreeBlock_exit;
            }

            // insert new block into rb-tree
            if (NV_OK != (status = _heapUpdate(pHeap, pBlockNew, BLOCK_ADD)))
            {
                //
                // Exit and report success.  The new block was allocated, but the
                // noncontig info is now out-of-sync with reality.
                //
                NV_PRINTF(LEVEL_ERROR, "_heapUpdate failed to _ADD block\n");
                goto _heapProcessFreeBlock_exit;
            }
        }
        else
        {
            status = NV_ERR_NO_MEMORY;
            // Exit with failure and free any local allocations
            goto _heapProcessFreeBlock_error;
        }
    }

    if (NULL == pBlockNew)
        status = NV_ERR_NO_MEMORY;

_heapProcessFreeBlock_error:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to allocate block\n");

        portMemFree(pBlockNew);
        portMemFree(pBlockSplit);

        *ppBlockNew = NULL;
        *ppBlockSplit = NULL;

        return status;
    }

_heapProcessFreeBlock_exit:
    *ppBlockNew = pBlockNew;
    *ppBlockSplit = pBlockSplit;

    // alignPad == 0 for all but >= NV5x
    *offset = pBlockNew->align + pBlockNew->alignPad;

    // Reduce free amount by allocated block size.
    _heapAdjustFree(pHeap, -((NvS64) (pBlockNew->end - pBlockNew->begin + 1)));

    if (FLD_TEST_DRF(OS32, _ATTR2, _INTERNAL, _YES, pFbAllocInfo->pageFormat->attr2))
    {
        osInternalReserveAllocCallback(*offset, pFbAllocInfo->size, pGpu->gpuId);
    }

    return NV_OK;
}

static void
_heapAddBlockToNoncontigList
(
    Heap       *pHeap,
    MEM_BLOCK  *pBlock
)
{
    if (NULL == pHeap->pNoncontigFreeBlockList)
    {
        pHeap->pNoncontigFreeBlockList = pBlock;
        pBlock->nextFreeNoncontig = NULL;
        pBlock->prevFreeNoncontig = NULL;
    }
    else
    {
        MEM_BLOCK *pNextBlock = pHeap->pNoncontigFreeBlockList;
        NvU64 size, nextSize = 0;
        size = pBlock->end - pBlock->begin + 1;

        NV_ASSERT(pBlock->prevFreeNoncontig == NULL &&
                  pBlock->nextFreeNoncontig == NULL);

        // The noncontig block list is arranged in the descending order of size
        while (NULL != pNextBlock)
        {
            nextSize = pNextBlock->end - pNextBlock->begin + 1;

            if (size > nextSize)
            {
                // Insert pBlock in front of pNextBlock
                pBlock->prevFreeNoncontig = pNextBlock->prevFreeNoncontig;
                pBlock->nextFreeNoncontig = pNextBlock;
                pNextBlock->prevFreeNoncontig = pBlock;

                if (pHeap->pNoncontigFreeBlockList == pNextBlock)
                {
                    // We inserted at the head of the list
                    pHeap->pNoncontigFreeBlockList = pBlock;
                }
                else
                {
                    pBlock->prevFreeNoncontig->nextFreeNoncontig = pBlock;
                }

                break;
            }

            if (NULL == pNextBlock->nextFreeNoncontig)
            {
                // We reached the end of the list, insert here
                pNextBlock->nextFreeNoncontig = pBlock;
                pBlock->prevFreeNoncontig = pNextBlock;
                pBlock->nextFreeNoncontig = NULL;

                break;
            }

            pNextBlock = pNextBlock->nextFreeNoncontig;
        }
    }
}

static void
_heapRemoveBlockFromNoncontigList
(
    Heap       *pHeap,
    MEM_BLOCK  *pBlock
)
{
    //
    // Unless pBlock is at the head of the list (and is the only element in the
    // list), both prev and nextFreeNoncontig cannot be NULL at the same time.
    // That would imply a bug in the noncontig list building code.
    //
    NV_ASSERT(pBlock == pHeap->pNoncontigFreeBlockList ||
              pBlock->prevFreeNoncontig != NULL ||
              pBlock->nextFreeNoncontig != NULL);

    // Removing first block?
    if (pHeap->pNoncontigFreeBlockList == pBlock)
    {
        pHeap->pNoncontigFreeBlockList = pBlock->nextFreeNoncontig;
    }
    else
    {
        if (NULL != pBlock->prevFreeNoncontig)
        {
            pBlock->prevFreeNoncontig->nextFreeNoncontig
                = pBlock->nextFreeNoncontig;
        }
    }

    // Removing last block?
    if (NULL != pBlock->nextFreeNoncontig)
    {
        pBlock->nextFreeNoncontig->prevFreeNoncontig
            = pBlock->prevFreeNoncontig;
    }

    pBlock->nextFreeNoncontig = pBlock->prevFreeNoncontig = NULL;
}

//
// The allocation is done using two loops. The first loop traverses the heap's
// free list to build a list of blocks that can satisfy the allocation. If we
// don't find enough blocks, we can exit quickly without needing to unwind,
// which can happen quite frequently in low memory or heavy fragmentation
// conditions.
//
// The second loop does the actual allocations. It calls _heapProcessFreeBlock()
// to cut down a free block into the required size, which can fail, albeit
// rarely. We need to unwind at that point. The two loops keep the unwinding
// as infrequent as possible.
//
static NV_STATUS
_heapAllocNoncontig
(
    OBJGPU             *pGpu,
    NvHandle            hClient,
    Heap               *pHeap,
    MEMORY_ALLOCATION_REQUEST *pAllocRequest,
    NvHandle            memHandle,
    OBJHEAP_ALLOC_DATA *pAllocData,
    FB_ALLOC_INFO      *pFbAllocInfo,
    NvU32               textureClientIndex,
    NvU64               alignPad,
    NvU64              *offset,
    MEMORY_DESCRIPTOR  *pMemDesc,
    HWRESOURCE_INFO   **ppHwResource
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc = pAllocRequest->pUserParams;
    NvBool      bFirstBlock = NV_TRUE;
    NvU32       pteIndexOffset = 0, i = 0;
    NvU32       blockId = 0;
    NV_STATUS   status = NV_OK;
    NvU64       pageSize = 0;
    NvS64       numPagesLeft;
    MEM_BLOCK  *pCurrBlock;
    MEM_BLOCK  *pNextBlock;
    MEM_BLOCK  *pSavedAllocList = NULL;
    MEM_BLOCK  *pLastBlock = NULL;
    MEM_BLOCK  *pBlockNew, *pBlockSplit;
    NvU32       k, shuffleStride = 1;
    NvU64       addr, j, numPages;
    RM_ATTR_PAGE_SIZE pageSizeAttr = dmaNvos32ToPageSizeAttr(pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2);
    NvU64       alignedSize = NV_ALIGN_UP64(pMemDesc->Size, RM_PAGE_SIZE);

    switch (pageSizeAttr)
    {
        case RM_ATTR_PAGE_SIZE_DEFAULT:
        case RM_ATTR_PAGE_SIZE_INVALID:
        case RM_ATTR_PAGE_SIZE_256GB:
            NV_PRINTF(LEVEL_ERROR, "Invalid page size attribute!\n");
            return NV_ERR_INVALID_ARGUMENT;
        case RM_ATTR_PAGE_SIZE_4KB:
            pageSize = RM_PAGE_SIZE;
            break;
        case RM_ATTR_PAGE_SIZE_BIG:
        {
            pageSize = kgmmuGetMaxBigPageSize_HAL(pKernelGmmu);
            break;
        }
        case RM_ATTR_PAGE_SIZE_HUGE:
        {
            NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                NV_ERR_INVALID_ARGUMENT);
            pageSize = RM_PAGE_SIZE_HUGE;
            break;
        }
        case RM_ATTR_PAGE_SIZE_512MB:
        {
            NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                              NV_ERR_INVALID_ARGUMENT);
            pageSize = RM_PAGE_SIZE_512M;
            break;
        }
    }

    //
    // pAllocData->allocSize already incorporates pFbAllocInfo->size,
    // which in turn is up aligned to pFbAllocInfo->align and alignPad,
    // so nothing else needs to be added here.
    //
    numPagesLeft = RM_ALIGN_UP(pAllocData->allocSize, pageSize) / pageSize;
    NV_PRINTF(LEVEL_INFO,
              "pageSize: 0x%llx, numPagesLeft: 0x%llx, allocSize: 0x%llx\n",
              pageSize / 1024, numPagesLeft, pAllocData->allocSize);

    for (pCurrBlock = pHeap->pNoncontigFreeBlockList;
        numPagesLeft > 0 && NULL != pCurrBlock;
        pCurrBlock = pNextBlock)
    {
        NvU64 blockBegin = 0;
        NvU64 blockEnd = 0;
        NvU64 blockAligned;
        NvU64 blockSizeInPages, blockSize;
        NvU64 alignPad;
        NvU64 pteAddress;
        NvU64 offset;

        // Get the next free block pointer before lists get re-linked
        pNextBlock = pCurrBlock->nextFreeNoncontig;

        // Selecting blocks: Is this block completely out of range?
        if ((pCurrBlock->end < pVidHeapAlloc->rangeLo) ||
            (pCurrBlock->begin > pVidHeapAlloc->rangeHi))
        {
            continue;
        }

        // Find the intersection of the block and the specified range.
        blockBegin = ((pVidHeapAlloc->rangeLo >= pCurrBlock->begin) ?
                pVidHeapAlloc->rangeLo : pCurrBlock->begin);
        blockEnd = ((pVidHeapAlloc->rangeHi <= pCurrBlock->end) ?
                pVidHeapAlloc->rangeHi : pCurrBlock->end);

        // Check if the derived block is usable
        if ((blockBegin >= blockEnd) ||
            (blockEnd-blockBegin+1 < pageSize))
        {
            // Skip if the usable size is invalid or insufficient.
            continue;
        }

        //
        // Checks above should protect against underflow, but we might still
        // end up with a post-aligned block that is unusable.
        // "end" should be RM_PAGE_SIZE-1 aligned.
        //
        blockBegin = RM_ALIGN_UP(blockBegin, pageSize);
        blockEnd = RM_ALIGN_DOWN(blockEnd+1, pageSize)-1;

        if (blockBegin >= blockEnd)
        {
            //
            // When blockSize < page_size and blockBegin and/or blockEnd are
            // not page aligned initially, the above alignment can cause
            // blockBegin to become > blockEnd.
            //
            continue;
        }

        // The first block has to handle pAllocData->alignment
        if (bFirstBlock)
        {
            // Align the starting address of the block to
            // pAllocData->alignment.
            blockAligned = (blockBegin +
                    pAllocData->alignment - 1) / pAllocData->alignment
                * pAllocData->alignment;

            //
            // Check that we'll still be within this block when
            // alignPad is added.
            //
            if (blockAligned + pFbAllocInfo->alignPad > blockEnd)
            {
                continue;
            }

            // Then make sure this is page aligned.
            blockBegin = RM_ALIGN_DOWN(blockAligned, pageSize);

            //
            // blockBegin is now the page aligned starting address of a
            // block that holds an address aligned to
            // pAllocData->alignment, and can take padding from
            // alignPad.
            //
        }
        else
        {
            blockAligned = blockBegin;
        }

        blockSizeInPages = (blockEnd - blockBegin + 1) / pageSize;

        // A usable block has to supply at least one page
        if (blockSizeInPages < 1)
        {
            continue;
        }

        // blockEnd may need to be corrected for the last page
        if (((NvU64)numPagesLeft < blockSizeInPages))
        {
            blockEnd = blockBegin + pageSize * numPagesLeft - 1;
            blockSizeInPages = numPagesLeft;
        }

        blockSize = blockEnd - blockBegin + 1;

        numPagesLeft -= blockSizeInPages;

        NV_PRINTF(LEVEL_INFO,
                  "\tblockId: %d, blockBegin: 0x%llx, blockEnd: 0x%llx, blockSize: "
                  "0x%llx, blockSizeInPages: 0x%llx, numPagesLeft: 0x%llx\n",
                  blockId, blockBegin, blockEnd, blockSize, blockSizeInPages,
                  numPagesLeft >= 0 ? numPagesLeft : 0);

        blockId++;

        //
        // Set pAllocData values before the call to
        // _heapProcessFreeBlock()
        //
        pAllocData->allocLo = blockBegin;
        pAllocData->allocHi = blockEnd;
        pAllocData->allocAl = blockAligned;
        pAllocData->allocSize = blockSize;

        if (bFirstBlock)
        {
            alignPad = pFbAllocInfo->alignPad;
        }
        else
        {
            alignPad = 0;
        }

        //
        // Cut this new block down to size. pBlockNew will be the block to use
        // when this returns.
        //
        if (NV_OK != (status = _heapProcessFreeBlock(pGpu, pCurrBlock,
                    &pBlockNew, &pBlockSplit, pHeap, pAllocRequest,
                    memHandle, pAllocData, pFbAllocInfo,
                    alignPad, &offset)))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "ERROR: Could not process free block, error: 0x%x\n",
                      status);
            goto unwind_and_exit;
        }

        // Never fails
        (void)_heapUpdate(pHeap, pBlockNew, BLOCK_FREE_STATE_CHANGED);

        //
        // Save the allocation off in case we need to unwind
        // This also ensures that all blocks that make up the noncontig
        // allocation are strung together in a list, which is useful when
        // freeing them.
        //
        if (pSavedAllocList == NULL)
        {
            // First block
            pSavedAllocList = pLastBlock = pBlockNew;
            pSavedAllocList->noncontigAllocListNext = NULL;
        }
        else
        {
            pLastBlock->noncontigAllocListNext = pBlockNew;
            pLastBlock = pBlockNew;
            pLastBlock->noncontigAllocListNext = NULL;
        }

        pteAddress = RM_PAGE_ALIGN_DOWN(pBlockNew->begin);

        numPages = NV_MIN(blockSizeInPages, ((alignedSize - (((NvU64) pteIndexOffset) * RM_PAGE_SIZE)) / pageSize));

        if (pHeap->getProperty(pHeap, PDB_PROP_HEAP_PAGE_SHUFFLE))
        {
            i             = pHeap->shuffleStrideIndex;
            shuffleStride = pHeap->shuffleStrides[i];

            // Select a stride greater the the number of pages
            while(numPages < shuffleStride && i > 0)
            {
                i--;
                shuffleStride = pHeap->shuffleStrides[i];
            }

            pHeap->shuffleStrideIndex = (pHeap->shuffleStrideIndex + 1) % SHUFFLE_STRIDE_MAX;
        }

        //
        // Shuffling logic.
        // We scatter the contiguous pages at multiple of stride length.
        // For 5 pages with stride length 2, we have the following shuffling.
        // Before: 0, 1, 2, 3, 4
        // After : 0, 2, 4, 1, 3
        //
        for (i = 0; i < shuffleStride; i++)
        {
            for(j = i; j < numPages; j = j + shuffleStride)
            {
                addr = pteAddress + j * pageSize;
                for (k = 0; k < pageSize/RM_PAGE_SIZE; k++)
                {
                    //
                    // The memDesc has everything in terms of 4k pages.
                    // If allocationSize % pageSize != 0, there will not be enough PTEs in
                    // the memdesc for completely specifying the final block, but that's
                    // ok. The mapping code will be mapping in the whole pageSize final
                    // block anyway, and the heapBlockFree() code will free the whole
                    // block.
                    //
                    memdescSetPte(pMemDesc, AT_GPU, pteIndexOffset, addr);
                    pteIndexOffset++;
                    addr += RM_PAGE_SIZE;
                }
            }
        }

        //
        // If a client calls us with pVidHeapAlloc->type ==
        // NVOS32_TYPE_TEXTURE, but where flags are non-zero, we won't
        // call objHeapSetTexturePlacement and initialize
        // textureClientIndex to a proper value (default is 0xFFFFFFFF).
        // In that case, we won't track this texture allocation. Bug
        // 79586.
        //
        if (pVidHeapAlloc->type == NVOS32_TYPE_TEXTURE &&
                textureClientIndex != 0xFFFFFFFF)
        {
            pBlockNew->textureId = hClient;
            if (bFirstBlock)
                pHeap->textureData[textureClientIndex].refCount++;
        }
        else
        {
            pBlockNew->textureId = 0;
        }

        if (bFirstBlock)
        {
            pFbAllocInfo->offset = offset;
            *ppHwResource = &pBlockNew->hwResource;
        }

        pBlockNew->pMemDesc = pMemDesc;
        pBlockNew->allocedMemDesc = bFirstBlock; // avoid multiple frees

        bFirstBlock = NV_FALSE;
    }

    // Did we find enough pages?
    if (numPagesLeft > 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Could not satisfy request: allocSize: 0x%llx\n",
                  pAllocData->allocSize);

        status = NV_ERR_NO_MEMORY;

unwind_and_exit:

        while (pSavedAllocList != NULL)
        {
            NV_STATUS unwindStatus;

            pCurrBlock = pSavedAllocList->noncontigAllocListNext;

            unwindStatus = _heapBlockFree(pGpu, pHeap, hClient, pFbAllocInfo->hDevice, pSavedAllocList);

            if (unwindStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "ERROR: Could not free block,  error 0x%x!\n",
                          unwindStatus);
            }

            pSavedAllocList = pCurrBlock;
        }
    }

    NV_ASSERT_OK_OR_RETURN(memdescSetAllocSizeFields(pMemDesc, alignedSize, RM_PAGE_SIZE));
    return status;
}

//
// Explanation of BlockAction values:
// - BLOCK_ADD,
//    A new block is added to the heap
//      o The block's node structure needs to be inited.
//      o The block is added to the rb-tree.
//      o The block is added to the noncontig freelist.
// - BLOCK_REMOVE
//    A block is removed from the heap for good
//      o The block is removed from the rb-tree.
//      o The block is removed from the noncontig freelist.
// - BLOCK_SIZE_CHANGED
//    A block's size has changed
//      o The rb-tree needs to be updated.
//      o The noncontig freelist needs to be updated.
// - BLOCK_FREE_STATE_CHANGED
//    if pBlock->owner != NVOS32_BLOCK_TYPE_FREE
//      A block is allocated to a client
//       o The block is removed from the noncontig freelist.
//    else
//      A block is freed by the client
//       o The block is added to the noncontig freelist.
//
static NV_STATUS
_heapUpdate
(
    Heap       *pHeap,
    MEM_BLOCK  *pBlock,
    BlockAction action
)
{
    // A new block is to be added, init its node structure.
    if (BLOCK_ADD == action)
    {
        portMemSet((void *)&pBlock->node, 0, sizeof(NODE));
        pBlock->node.Data     = (void *)pBlock;
    }

    // Both new and updated blocks need to be re-inserted into the rb tree.
    if ((BLOCK_SIZE_CHANGED == action) ||
        (BLOCK_ADD == action))
    {
        pBlock->node.keyStart = pBlock->begin;
        pBlock->node.keyEnd = pBlock->end;

        if (btreeInsert(&pBlock->node, &pHeap->pBlockTree) != NV_OK)
        {
            NV_ASSERT_FAILED("btreeInsert failed to ADD/SIZE_CHANGE block");
            return NV_ERR_INVALID_STATE;
        }
    }

    //
    // Updated, new and freed blocks need to be added back to the noncontig
    // freelist.
    //
    if ((BLOCK_SIZE_CHANGED == action) ||
        (BLOCK_ADD == action) ||
        (BLOCK_FREE_STATE_CHANGED == action &&
         pBlock->owner == NVOS32_BLOCK_TYPE_FREE))
    {
        _heapAddBlockToNoncontigList(pHeap, pBlock);
    }

    // Remove the block from the heap
    if (BLOCK_REMOVE == action)
    {
        if (btreeUnlink(&pBlock->node, &pHeap->pBlockTree) != NV_OK)
        {
            NV_ASSERT_FAILED("btreeUnlink failed to REMOVE block");
            return NV_ERR_INVALID_STATE;
        }
    }

    // An allocated block is only removed from the noncontig freelist.
    if ((BLOCK_REMOVE == action) ||
        ((BLOCK_FREE_STATE_CHANGED == action &&
          pBlock->owner != NVOS32_BLOCK_TYPE_FREE)))
    {
        _heapRemoveBlockFromNoncontigList(pHeap, pBlock);
    }

    return NV_OK;
}

static NvU32
_heapGetPageBlackListGranularity(void)
{
    return RM_PAGE_SIZE;
}

//
// This function blacklists pages from the heap.
// The addresses of the pages to blacklist are available from
// pHeap->blackListAddresses.
//
NV_STATUS
heapBlackListPages_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    PMA                 *pPma       = &pHeap->pmaObject;
    NvU32                i = 0, j = 0;
    NV_STATUS            status     = NV_OK;
    BLACKLIST           *pBlackList = &pHeap->blackList;
    BLACKLIST_ADDRESSES *pAddresses = &pHeap->blackListAddresses;
    NvU32                count = pHeap->blackListAddresses.count;
    NvU32                staticBlacklistSize, dynamicBlacklistSize;
    NvU32                dynamicRmBlackListedCount;
    NvU32                staticRmBlackListedCount;
    NvU16                maximumBlacklistPages = kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem);

    if (NULL == pAddresses)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pBlackList->count != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Error: BlackList already exists!\n");
        return NV_ERR_INVALID_STATE;
    }

    //
    // We may not be able to allocate all pages requested, but alloc enough
    // space anyway
    //
    pBlackList->pBlacklistChunks = portMemAllocNonPaged(sizeof(BLACKLIST_CHUNK) * maximumBlacklistPages);
    if (NULL == pBlackList->pBlacklistChunks)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate memory for blackList!\n");
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pBlackList->pBlacklistChunks, 0, sizeof(BLACKLIST_CHUNK) * maximumBlacklistPages);

    dynamicRmBlackListedCount = 0;
    staticRmBlackListedCount  = 0;
    for (i = 0, j = 0; i < count; i++)
    {
        if (NV2080_CTRL_FB_OFFLINED_PAGES_INVALID_ADDRESS == pAddresses->data[i].address)
        {
            continue;
        }

        //
        // If PMA is enabled, only blacklist pages in the internal heap.
        // PMA blacklisting is handled in pmaRegisterRegion.
        //
        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            if (heapIsPmaManaged(pGpu, pHeap, pAddresses->data[i].address, pAddresses->data[i].address))
            {
                // Skipping non-internal address
                continue;
            }
        }

        if ((pAddresses->data[i].type == NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_MULTIPLE_SBE) ||
            (pAddresses->data[i].type == NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE))
        {
            dynamicRmBlackListedCount++;
        }
        else
        {
            staticRmBlackListedCount++;
        }

        // Create a memdesc
        status = memdescCreate(&pBlackList->pBlacklistChunks[j].pMemDesc,
                               pGpu,
                               RM_PAGE_SIZE,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               ADDR_FBMEM,
                               NV_MEMORY_UNCACHED,
                               MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE |
                               MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
        if (NV_OK != status)
        {
            portMemSet(&pBlackList->pBlacklistChunks[j], 0, sizeof(BLACKLIST_CHUNK));
            NV_PRINTF(LEVEL_ERROR,
                      "Error 0x%x creating blacklisted page memdesc for address 0x%llx, skipping\n",
                      status, pAddresses->data[i].address);
            continue;
        }

        if (pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR)
            pBlackList->pBlacklistChunks[j].pMemDesc->pHeap = pHeap;

        // This is how _FIXED_ADDRESS_ALLOCATE works
        memdescSetPte(pBlackList->pBlacklistChunks[j].pMemDesc,
                      AT_GPU, 0, RM_PAGE_ALIGN_DOWN(pAddresses->data[i].address));

        if (pHeap->heapType != HEAP_TYPE_PHYS_MEM_SUBALLOCATOR)
        {
            //
            // Allocate memory for this page. This is marked as an internal RM
            // allocation and WILL be saved/restored during suspend/resume.
            //
            memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_79, 
                    pBlackList->pBlacklistChunks[j].pMemDesc);
            if (NV_OK != status)
            {
                // No use for the memdesc if the page couldn't be allocated
                memdescDestroy(pBlackList->pBlacklistChunks[j].pMemDesc);

                portMemSet(&pBlackList->pBlacklistChunks[j], 0, sizeof(BLACKLIST_CHUNK));

                NV_PRINTF(LEVEL_ERROR,
                          "Error 0x%x blacklisting page at address  0x%llx, skipping\n",
                          status, pAddresses->data[i].address);
                continue;
            }
        }

        // Page blacklisting is successful, add entries to the BLACKLIST
        pBlackList->pBlacklistChunks[j].physOffset              = pAddresses->data[i].address;
        pBlackList->pBlacklistChunks[j].size                    = RM_PAGE_SIZE;
        pBlackList->pBlacklistChunks[j].bIsValid                = NV_TRUE;

        // If the page was successfully blacklisted, move to the next entry
        j++;
    }

    pBlackList->count = j;

    pmaGetBlacklistSize(pPma, &dynamicBlacklistSize, &staticBlacklistSize);
    dynamicBlacklistSize = dynamicBlacklistSize >> 10;
    staticBlacklistSize  = staticBlacklistSize  >> 10;

    dynamicBlacklistSize += (dynamicRmBlackListedCount * _heapGetPageBlackListGranularity()) >> 10;
    staticBlacklistSize  += (staticRmBlackListedCount  * _heapGetPageBlackListGranularity()) >> 10;

    pHeap->dynamicBlacklistSize = dynamicBlacklistSize;
    pHeap->staticBlacklistSize = staticBlacklistSize;

    if (0 == pBlackList->count)
    {
        // No address was blacklisted
        portMemFree(pBlackList->pBlacklistChunks);
        pBlackList->pBlacklistChunks = NULL;
    }

    return NV_OK;
}

//
// This function frees all blacklisted pages.
// The pHeap->blackList structure holds a list of memdescs, one for each
// blacklisted page.
//
NV_STATUS
heapFreeBlackListedPages_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap
)
{
    NvU32 i;
    BLACKLIST *pBlackList = &pHeap->blackList;

    // Also free the blacklistAddresses data here
    if (pHeap->blackListAddresses.data)
    {
        portMemFree(pHeap->blackListAddresses.data);
        pHeap->blackListAddresses.count = 0;
        pHeap->blackListAddresses.data  = NULL;
    }

    if (0 == pBlackList->count)
    {
        return NV_OK;
    }

    if (NULL == pBlackList->pBlacklistChunks)
    {
        return NV_ERR_INVALID_STATE;
    }

    for (i = 0; i < pBlackList->count; i++)
    {
        if (pBlackList->pBlacklistChunks[i].bIsValid)
        {
            // Free the blacklisted page
            memdescFree(pBlackList->pBlacklistChunks[i].pMemDesc);

            // Free the memdesc
            memdescDestroy(pBlackList->pBlacklistChunks[i].pMemDesc);
        }
    }

    portMemFree(pBlackList->pBlacklistChunks);

    pBlackList->count            = 0;
    pBlackList->pBlacklistChunks = NULL;

    return NV_OK;
}

NV_STATUS
heapStorePendingBlackList_IMPL
(
    OBJGPU  *pGpu,
    Heap    *pHeap,
    NvU64    pageAddressesWithEccOn,
    NvU64    pageAddressWithEccOff
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_STATUS status = NV_OK;
    NvU64  physicalAddress;
    NvU64  pageNumber;
    BLACKLIST *pBlacklist = &pHeap->blackList;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    if (pMemorySystemConfig->bEnabledEccFBPA)
    {
        physicalAddress = pageAddressesWithEccOn;
    }
    else
    {
        physicalAddress = pageAddressWithEccOff;
    }

    pageNumber = (physicalAddress >> RM_PAGE_SHIFT);

    // This code is called only when DBE happens, so marking it as type DBE
    status = heapAddPageToBlackList(pGpu, pHeap,
             DRF_VAL64(_HEAP, _PAGE_OFFLINE, _PAGE_NUMBER, pageNumber),
             NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE);
    if (NV_OK != status)
    {
        // No more space in the blacklist
        NV_PRINTF(LEVEL_ERROR, "No more space in blacklist, status: %x!\n", status);
        return status;
    }

    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        if (heapIsPmaManaged(pGpu, pHeap, physicalAddress, physicalAddress))
        {
            NV_PRINTF(LEVEL_INFO, "Calling PMA helper function to blacklist page offset: %llx\n", physicalAddress);
            status = pmaAddToBlacklistTracking(&pHeap->pmaObject, physicalAddress);
            return status;
        }
        else
        {
             // blacklisting needs to be done like CBC error recovery
             return NV_ERR_RESET_REQUIRED;
        }
    }
    else
    {
        if (pMemoryManager->bEnableDynamicPageOfflining)
        {
            // adding a new entry to heap managed blacklist
            if (pBlacklist->count == kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem))
            {
                NV_PRINTF(LEVEL_ERROR, "We have blacklisted maximum number of pages possible. returning error \n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }
            portMemSet(&pBlacklist->pBlacklistChunks[pBlacklist->count], 0 , sizeof(BLACKLIST_CHUNK));
            pBlacklist->pBlacklistChunks[pBlacklist->count].physOffset = physicalAddress;
            pBlacklist->pBlacklistChunks[pBlacklist->count].size = RM_PAGE_SIZE;
            pBlacklist->pBlacklistChunks[pBlacklist->count].bPendingRetirement = NV_TRUE;
            pBlacklist->count++;
        }
    }
    return status;
}

//
// This function copies the addresses of pages to be blacklisted from
// pPageNumbers into Heap's internal blackListAddresses structure.
//
NV_STATUS
heapStoreBlackList_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap,
    NvU64 *pPageNumbersWithEccOn,
    NvU64 *pPageNumbersWithECcOff,
    NvU32 maxInputPages
)
{
    NvU32       i;
    NvU64      *pPageNumbers;
    NV_STATUS   status  = NV_OK;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    if (pMemorySystemConfig->bEnabledEccFBPA)
    {
        pPageNumbers = pPageNumbersWithEccOn;
    }
    else
    {
        pPageNumbers = pPageNumbersWithECcOff;
    }

    for (i = 0; i < maxInputPages; i++)
    {
        //
        // Bug: 2999257
        // currently pre-Hopper we have 37b FB PA, whose PFN will be 25b
        // From Hopper+ we have 52b PA, whose PFN will be 40b PA and hence
        // the macro NV_INFOROM_BLACKLIST_PAGE_NUMBER width of 28b will not be
        // sufficient to capture the entire address, this needs to be fixed.
        //
        status = heapAddPageToBlackList(pGpu, pHeap,
                DRF_VAL64(_HEAP, _PAGE_OFFLINE, _PAGE_NUMBER, pPageNumbers[i]),
                (NvU32)DRF_VAL64(_HEAP, _PAGE_OFFLINE, _TYPE, pPageNumbers[i]));
        if (NV_OK != status)
        {
            // No more space in the blacklist
            NV_PRINTF(LEVEL_ERROR, "No more space in blacklist!\n");
            return status;
        }
    }

    return status;
}

NV_STATUS
heapAddPageToBlackList_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap,
    NvU64   pageNumber,
    NvU32   type
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU16 maximumBlacklistPages = kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem);
    NvU32 index = pHeap->blackListAddresses.count;

    if (index == maximumBlacklistPages)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (pHeap->blackListAddresses.data == NULL)
    {
        NvU64 listSize = sizeof(BLACKLIST_ADDRESS) * maximumBlacklistPages;

        pHeap->blackListAddresses.data = portMemAllocNonPaged(listSize);
        if (pHeap->blackListAddresses.data == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }

        portMemSet(pHeap->blackListAddresses.data, 0, listSize);
    }

    pHeap->blackListAddresses.data[index].address = (pageNumber << RM_PAGE_SHIFT);
    pHeap->blackListAddresses.data[index].type = type;

    pHeap->blackListAddresses.count++;

    NV_PRINTF(LEVEL_INFO, "Added 0x%0llx (blacklist count: %u)\n",
              pHeap->blackListAddresses.data[index].address,
              pHeap->blackListAddresses.count);

    return NV_OK;
}

/*!
 * @brief: Identify if an FB range is PMA-managed
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] pHeap     Heap pointer
 * @param[in] offset    FB block offset
 * @param[in] limit     FB block limit
 *
 * @return NV_TRUE      offset is PMA-managed
 *         NV_FALSE     offset is not managed by PMA
 */
NvBool
heapIsPmaManaged_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap,
    NvU64   offset,
    NvU64   limit
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        NV_ASSERT(offset <= limit);

        return pmaIsPmaManaged(&pHeap->pmaObject, offset, limit);
    }

    return NV_FALSE;
}

/*!
 * @brief Increase the reference count
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] pHeap    Heap pointer
 *
 * @return Current refcount value
 */
NvU32
heapAddRef_IMPL
(
    Heap *pHeap
)
{
    if (pHeap == NULL)
        return 0;

    return portAtomicExIncrementU64(&pHeap->refCount);
}

/*!
 * @brief Increase the reference count
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] pHeap    Heap pointer
 *
 * @return Current refcount value
 */
NvU32
heapRemoveRef_IMPL
(
    Heap   *pHeap
)
{
    NvU64 refCount = 0;

    if (pHeap == NULL)
        return 0;

    refCount = portAtomicExDecrementU64(&pHeap->refCount);
    if (refCount == 0)
    {
        objDelete(pHeap);
    }

    return refCount;
}

/*!
 * @brief Adjust the heap size
 *
 * @param[in] pHeap    Heap pointer
 * @param[in] resizeBy NVS64 resizeBy value
 */

NV_STATUS heapResize_IMPL
(
    Heap *pHeap,
    NvS64 resizeBy
)
{
    MEM_BLOCK  *pBlockLast;
    MEM_BLOCK  *pBlockNew;
    NV_STATUS   status  = NV_OK;
    OBJGPU     *pGpu    = ENG_GET_GPU(pHeap);

    NV_ASSERT_OR_RETURN(pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR, NV_ERR_NOT_SUPPORTED);

    // Free all blacklisted pages
    if ((pHeap->blackListAddresses.count != 0) &&
         pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) &&
         gpuCheckPageRetirementSupport_HAL(pGpu))
    {
        heapFreeBlackListedPages(pGpu, pHeap);
    }

    // Go to last block if the heap w.r.t. the start address
    pBlockLast = pHeap->pBlockList;
    while (pBlockLast->next != pHeap->pBlockList)
        pBlockLast = pBlockLast->next;

    if (resizeBy < 0) // Shrink the allocation
    {
        NvS64 newSize;

        NV_ASSERT_OR_RETURN(pBlockLast->owner == NVOS32_BLOCK_TYPE_FREE, NV_ERR_NO_MEMORY);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddS64(pBlockLast->end - pBlockLast->begin, resizeBy, &newSize) &&
                                        (newSize > 0), NV_ERR_INVALID_LIMIT);
        pBlockLast->end += resizeBy;
    }
    else // Grow the allocation
    {
        if (pBlockLast->owner == NVOS32_BLOCK_TYPE_FREE)
        {
            // Found a free block at the end Just resize it.
            pBlockLast->end += resizeBy;
        }
        else
        {
            // Could not find a free block at the end. Add a new free block.
            pBlockNew = portMemAllocNonPaged(sizeof(MEM_BLOCK));
            if (pBlockNew != NULL)
            {

                portMemSet(pBlockNew, 0, sizeof(MEM_BLOCK));

                pBlockNew->owner    = NVOS32_BLOCK_TYPE_FREE;
                pBlockNew->refCount = 1;

                // Set block boundaries
                pBlockNew->begin    = pBlockLast->end + 1;
                pBlockNew->end      = pBlockLast->end + resizeBy;

                if (pHeap->pFreeBlockList == NULL)
                    pHeap->pFreeBlockList = pBlockNew;

                // Add the block in the free blocks list
                pBlockNew->u1.nextFree              = pHeap->pFreeBlockList;
                pBlockNew->u0.prevFree              = pHeap->pFreeBlockList->u0.prevFree;
                pBlockNew->u1.nextFree->u0.prevFree = pBlockNew;
                pBlockNew->u0.prevFree->u1.nextFree = pBlockNew;

                // Add the block in the blocks list
                pBlockNew->next       = pBlockLast->next;
                pBlockNew->prev       = pBlockLast;
                pBlockNew->next->prev = pBlockNew;
                pBlockNew->prev->next = pBlockNew;

                if ((status = _heapUpdate(pHeap, pBlockNew, BLOCK_ADD)) != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "_heapUpdate failed to _ADD block\n");

                    if (pHeap->pFreeBlockList == pBlockNew) // There was no free block in the heap.
                        pHeap->pFreeBlockList = NULL;       // We had added this one.
                    portMemFree(pBlockNew);
                }
            }
        }
    }

    if (status == NV_OK)
    {
        pHeap->total += resizeBy;
        pHeap->free  += resizeBy;

        status = memmgrGetBlackListPagesForHeap_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pHeap);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Failed to read blackList pages (0x%x).\n",
                      status);
        }

        heapFilterBlackListPages(pHeap, pHeap->base, pHeap->total);

        if (pHeap->blackListAddresses.count != 0)
        {
            status = heapBlackListPages(pGpu, pHeap);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "Error 0x%x creating blacklist\n",
                          status);
            }
        }
    }
    return status;
}
