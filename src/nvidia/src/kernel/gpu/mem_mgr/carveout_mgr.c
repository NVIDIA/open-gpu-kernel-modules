/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/carveout_mgr.h"
#include "containers/eheap_old.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"

static NV_STATUS
_carveoutmgrFreeHeapBlockLocked
(
    CarveoutMgr *pCarveoutMgr,
    NvU64        base
)
{
    OBJEHEAP *pHeap = pCarveoutMgr->pHeap;
    EMEMBLOCK *pBlock;
    NV_STATUS status;

    if (pHeap == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pBlock = (EMEMBLOCK *)pHeap->eheapGetBlock(pHeap, base, 0);
    if (pBlock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Not found allocation in carveout region\n");
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    status = pHeap->eheapFree(pHeap, base);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error in freeing eheap 0x%08x\n", status);
    }

    return status;
}

NV_STATUS
carveoutmgrConstruct_IMPL
(
    CarveoutMgr *pCarveoutMgr
)
{
    pCarveoutMgr->pHeap = NULL;
    pCarveoutMgr->pStateLock = NULL;
    pCarveoutMgr->base = 0;
    pCarveoutMgr->size = 0;
    pCarveoutMgr->attachedGpuMask = 0;

    pCarveoutMgr->pStateLock =
        portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pCarveoutMgr->pStateLock == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

void
carveoutmgrDestruct_IMPL
(
    CarveoutMgr *pCarveoutMgr
)
{
    NV_ASSERT(pCarveoutMgr->attachedGpuMask == 0);

    if (pCarveoutMgr->pHeap != NULL)
    {
        NV_ASSERT(pCarveoutMgr->pHeap->free == pCarveoutMgr->pHeap->total);
        pCarveoutMgr->pHeap->eheapDestruct(pCarveoutMgr->pHeap);
        portMemFree(pCarveoutMgr->pHeap);
        pCarveoutMgr->pHeap = NULL;
    }
    pCarveoutMgr->base = 0;
    pCarveoutMgr->size = 0;
    pCarveoutMgr->attachedGpuMask = 0;

    if (pCarveoutMgr->pStateLock != NULL)
    {
        portSyncMutexDestroy(pCarveoutMgr->pStateLock);
        pCarveoutMgr->pStateLock = NULL;
    }
}

NV_STATUS
carveoutmgrAcquireHeap_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    OBJGPU      *pGpu,
    NvU64        base,
    NvU64        size
)
{
    NvU32 gpuInstance = NV_MAX_DEVICES;
    NvU32 gpuMask = 0;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_ERR_INVALID_STATE);
    ct_assert(NV_MAX_DEVICES <= 32);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, size != 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, base <= (NV_U64_MAX - size),
                       NV_ERR_INVALID_ARGUMENT);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);

    if (pCarveoutMgr->pHeap == NULL)
    {
        pCarveoutMgr->pHeap =
            (OBJEHEAP *)portMemAllocNonPaged(sizeof(*pCarveoutMgr->pHeap));
        if (pCarveoutMgr->pHeap == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        constructObjEHeap(pCarveoutMgr->pHeap, base, base + size, 0, 0);
        pCarveoutMgr->base = base;
        pCarveoutMgr->size = size;
        pCarveoutMgr->attachedGpuMask = 0;

        NV_PRINTF(LEVEL_INFO,
                  "Created shared scanout carveout heap with base=0x%llx, size=0x%llx\n",
                  base, size);
    }
    else if ((pCarveoutMgr->base != base) ||
             (pCarveoutMgr->size != size))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "scanout carveout base=0x%llx size=0x%llx does not match existing heap base=0x%llx size=0x%llx gpuInstance=%u\n",
                  base, size,
                  pCarveoutMgr->base,
                  pCarveoutMgr->size,
                  (pGpu != NULL) ? gpuGetInstance(pGpu) : NV_MAX_DEVICES);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    if (pGpu != NULL)
    {
        gpuInstance = gpuGetInstance(pGpu);
        if (gpuInstance >= NV_MAX_DEVICES)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
        gpuMask = NVBIT(gpuInstance);
        pCarveoutMgr->attachedGpuMask |= gpuMask;
    }

done:
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
    return status;
}

void
carveoutmgrReleaseHeap_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    OBJGPU      *pGpu
)
{
    NvU32 gpuInstance;
    NvU32 gpuMask;

    NV_ASSERT_OR_RETURN_VOID(pCarveoutMgr != NULL);
    NV_ASSERT_OR_RETURN_VOID(pCarveoutMgr->pStateLock != NULL);
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pGpu != NULL);

    gpuInstance = gpuGetInstance(pGpu);
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, gpuInstance < NV_MAX_DEVICES);
    gpuMask = NVBIT(gpuInstance);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);

    if ((pCarveoutMgr->attachedGpuMask & gpuMask) == 0)
    {
        goto done;
    }

    pCarveoutMgr->attachedGpuMask &= ~gpuMask;

    //
    // GPU attachment controls admission to new allocations and duplicates,
    // not the lifetime of already-published memdesc associations. Keep the
    // shared heap alive until CarveoutMgr destruction so delayed frees cannot
    // observe a destroyed allocator or overlap a newly-created heap.
    //
    if (pCarveoutMgr->pHeap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU %u is attached to a missing scanout carveout heap\n",
                  gpuInstance);
    }

done:
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
}

/**
 * @brief Return a point-in-time snapshot of a GPU's attachment state.
 *
 * The attachment can change after this function releases the manager lock.
 * Callers must treat the result as advisory and rely on the locked heap
 * operation to revalidate the heap and GPU attachment before use.
 */
NvBool
carveoutmgrIsGpuAttached_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    OBJGPU      *pGpu
)
{
    NvBool bAttached;
    NvU32 gpuInstance;
    NvU32 gpuMask;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_FALSE);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpu != NULL, NV_FALSE);

    gpuInstance = gpuGetInstance(pGpu);
    if (gpuInstance >= NV_MAX_DEVICES)
    {
        return NV_FALSE;
    }
    gpuMask = NVBIT(gpuInstance);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);
    bAttached = ((pCarveoutMgr->attachedGpuMask & gpuMask) != 0);
    portSyncMutexRelease(pCarveoutMgr->pStateLock);

    return bAttached;
}

/**
 * @brief Return a locked snapshot of the shared carveout heap geometry.
 *
 * The heap geometry remains immutable until CarveoutMgr destruction. Callers
 * may therefore use the snapshot after the manager lock is released without
 * holding the lock across memory copies or OS mapping operations. Geometry
 * lookup is independent of GPU attachment so teardown can always unmap an
 * existing per-GPU CPU mapping.
 */
NV_STATUS
carveoutmgrGetHeapInfo_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    NvU64       *pBase,
    NvU64       *pSize
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_INFO, (pBase != NULL) || (pSize != NULL),
                       NV_ERR_INVALID_ARGUMENT);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);

    if (pCarveoutMgr->pHeap == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    if (pBase != NULL)
    {
        *pBase = pCarveoutMgr->base;
    }
    if (pSize != NULL)
    {
        *pSize = pCarveoutMgr->size;
    }

done:
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
    return status;
}

NV_STATUS
carveoutmgrAllocHeapBlock_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    OBJGPU      *pGpu,
    NvU32        owner,
    NvU32       *pHeapFlag,
    NvU64       *pOffset,
    NvU64       *pSize,
    NvU64        alignment,
    NvU64        rangeLo,
    NvU64        rangeHi
)
{
    OBJEHEAP *pHeap;
    NvU32 gpuInstance;
    NvU32 gpuMask;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pHeapFlag != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pOffset != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, rangeLo <= rangeHi, NV_ERR_INVALID_ARGUMENT);

    gpuInstance = gpuGetInstance(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, gpuInstance < NV_MAX_DEVICES,
                       NV_ERR_INVALID_ARGUMENT);
    gpuMask = NVBIT(gpuInstance);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);

    pHeap = pCarveoutMgr->pHeap;
    if ((pHeap == NULL) ||
        ((pCarveoutMgr->attachedGpuMask & gpuMask) == 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU %u attempted to allocate from an unattached scanout carveout heap\n",
                  gpuInstance);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    NV_CHECK_OR_ELSE(LEVEL_ERROR,
                     (rangeLo >= pCarveoutMgr->base) &&
                     (rangeHi < (pCarveoutMgr->base + pCarveoutMgr->size)),
                     status = NV_ERR_INVALID_ARGUMENT;
                     goto done);

    status = pHeap->eheapAllocInRange(pHeap, owner, pHeapFlag,
                                      pOffset, pSize, alignment, alignment,
                                      rangeLo, rangeHi,
                                      NULL, NULL, NULL);

done:
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
    return status;
}

NV_STATUS
carveoutmgrDuplicateHeapBlock_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    OBJGPU      *pGpu,
    NvU64        base
)
{
    OBJEHEAP *pHeap;
    EMEMBLOCK *pBlock;
    NvU32 gpuInstance;
    NvU32 gpuMask;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    gpuInstance = gpuGetInstance(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, gpuInstance < NV_MAX_DEVICES,
                       NV_ERR_INVALID_ARGUMENT);
    gpuMask = NVBIT(gpuInstance);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);

    pHeap = pCarveoutMgr->pHeap;
    if ((pHeap == NULL) ||
        ((pCarveoutMgr->attachedGpuMask & gpuMask) == 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU %u attempted to duplicate from an unattached scanout carveout heap\n",
                  gpuInstance);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    pBlock = (EMEMBLOCK *)pHeap->eheapGetBlock(pHeap, base, 0);
    if (pBlock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Not found allocation in carveout region\n");
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    pBlock->refCount++;

done:
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
    return status;
}

NV_STATUS
carveoutmgrFreeHeapBlock_IMPL
(
    CarveoutMgr *pCarveoutMgr,
    NvU64        base
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pCarveoutMgr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCarveoutMgr->pStateLock != NULL, NV_ERR_INVALID_STATE);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);
    status = _carveoutmgrFreeHeapBlockLocked(pCarveoutMgr, base);
    portSyncMutexRelease(pCarveoutMgr->pStateLock);
    return status;
}

/**
 * @brief Release one published memdesc association with the carveout heap.
 *
 * Existing associations remain releasable after their GPU detaches. Save the
 * PTE used to identify the allocation before destroying the descriptor. On
 * the final descriptor reference, memdescDestroy releases the allocated
 * backing and IOMMU mappings before the eheap range becomes reusable.
 */
void
carveoutmgrReleaseMemDesc_IMPL
(
    CarveoutMgr       *pCarveoutMgr,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NvU64 base;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(pCarveoutMgr != NULL);
    NV_ASSERT_OR_RETURN_VOID(pCarveoutMgr->pStateLock != NULL);
    NV_ASSERT_OR_RETURN_VOID(pMemDesc != NULL);
    NV_ASSERT(memdescGetFlag(pMemDesc,
                             MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT));

    base = memdescGetPte(pMemDesc, AT_CPU, 0);

    //
    // Tear down descriptor state before making the range reusable. This also
    // makes descriptor and IOMMU cleanup independent of eheap bookkeeping.
    // Scanout carveout memdescs are SYSMEM and must not retain a Heap pointer.
    //
    pMemDesc->pHeap = NULL;
    memdescDestroy(pMemDesc);

    portSyncMutexAcquire(pCarveoutMgr->pStateLock);
    status = _carveoutmgrFreeHeapBlockLocked(pCarveoutMgr, base);
    portSyncMutexRelease(pCarveoutMgr->pStateLock);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to release scanout carveout heap block, status=0x%x\n",
                  status);
        NV_ASSERT(status == NV_OK);
    }
}
