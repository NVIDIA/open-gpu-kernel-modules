/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/fbsr.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "published/maxwell/gm107/dev_ram.h"
#include "core/thread_state.h"

//
// Implementation notes:
//
// This file implements two mechanisms for FB save / restore on Fermi. I
// would've liked to split the implementation for Begin()/End()/CopyMemory()
// into seperate subclasses but our current object model makes inheritance a tad
// cumbersome.
//
// Mechanism #1 (TYPE_DMA): The data is DMA'ed (via CE) to a large non-paged
// system memory allocation. This is the preferred / performance path.
//
// Mechanism #2 (TYPE_CPU): The backup buffer is allocated in system memory
// chunks. The sysmem buffers are currently non-paged, long-term the plan is to
// switch the allocations to be paged. For this approach we pre-allocate a page
// size buffer for CE to DMA into. From that buffer we use CPU IO to move the
// data out into the intended storage buffer. The reason for chunks is that it
// has been noticed that large non-paged allocations tend to fail more often
// than multiple smaller non-paged allocations. Once we move over to paged
// allocations here this *might* not be needed. Bug 579780 and 579765 are
// tracking the RFE for paged memory. The reason for using CE here is bar2 isn't
// yet set up and BAR0 would be really really slow (not that we care about
// performance much for this sheme).
//
// Mechanism #3 (TYPE_PERSISTENT): The video Memory[Fb] data is transferred to
// sys_mem by means of DMA[CE engine], sys_mem allocation is pinned across S3
// transitions. Sys_mem allocations are done at first S3 cycle and release during
// driver unload. this approach reduces system VM fragmentation. Optimus systems,
// keeps GPU in D3 state, as long as there is no work for GPU. Because of frequent
// transitions between D0 & D3, system is running out of *CONTIGOUS* VM, with this
// approach Optimus system could avoid the above problem.
//
// Mechanism #4 (TYPE_PAGED_DMA): It is basically the same with the TYPE_DMA
// method except that we allocate the buffer from pagable memory pool. After the
// buffer is allocated, we need to use memdescLock to lock the buffer in physical
// memory so that CE can access it and then use memdescUnlock to unlock it.
//
// Mechanisms #5 and #6 are targetted for WDDM, a large VA section (Paged region)
// and  a small pinned region are committed on boot.
//
// Mechanism #5 (TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED):
// For power save, map and pin the large va region and if the map succeeds,
// ce copy to this large pinned page. At this point it is similar to TYPE_DMA.
// If the map and pin fails, fall back to TYPE_WDDM_SLOW_CPU
//
// Mechanism #6 (TYPE_WDDM_SLOW_CPU_PAGED):
// When TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED fails, use a small 64k pinned page
// that was preallocated and dma to this pinned page from FB. Once the ce completes,
// the chunk is then copied to the paged CPU memory. A 64k chunk size is chosen
// because the Windows ZwMapViewOfSection requires 64K alignment
//
// While technically mechanism #2 can fail (even with paged memory) another
// approach worth considering would be to pre-allocate the save buffer in the
// video memory allocation path (memdescAlloc). However, with this approach we'd
// incur a memory overhead even if S/R was never used.
//

#ifdef DEBUG
#endif

#define CPU_PINNED_BUFFER_SIZE                              RM_PAGE_SIZE
#define CPU_MAX_PINNED_BUFFER_SIZE                          0x10000

//
// Maximum data copy size in bytes for file operations (read/write)
// which can be transferred with default thread timeout.
//
#define MAX_FILE_COPY_SIZE_WITHIN_DEFAULT_THREAD_TIMEOUT    (64 * 1024 * 1024)

/*!
 * Init
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 *
 * @returns None
 */
NV_STATUS
fbsrInit_GM107(OBJGPU *pGpu, OBJFBSR *pFbsr)
{
    NV_STATUS status;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    portMemSet(&pFbsr->pagedBufferInfo, 0, sizeof(pFbsr->pagedBufferInfo));

    // Commit an upper bound VA for both slow cpu and fast dma.
    if ((pFbsr->type == FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED ||
         pFbsr->type == FBSR_TYPE_WDDM_SLOW_CPU_PAGED))
    {
        // We need it only once not per fbsr scheme though
        pFbsr->pagedBufferInfo.maxLength = memmgrGetRsvdSizeForSr_HAL(pGpu, pMemoryManager);
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT))
        {
            status = osReserveCpuAddressSpaceUpperBound(&pFbsr->pagedBufferInfo.sectionHandle,
                                                        pFbsr->pagedBufferInfo.maxLength);

            if (status != NV_OK)
                return status;
        }
    }

    //
    // Allocate a 64K/4k sized buffer for forward progress if the WDDM Fast path
    // cannot pin the large buffer.
    //
    if (pFbsr->type == FBSR_TYPE_CPU ||
        pFbsr->type == FBSR_TYPE_WDDM_SLOW_CPU_PAGED ||
        pFbsr->type == FBSR_TYPE_FILE)
    {
        NvU32 memSize = 0;

        if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            // Can't allocate sysmem from GSP FW.
            return NV_ERR_NOT_SUPPORTED;

            // GSP FW TODO: Allocate this memory on the client side.
        }

        if (pFbsr->type == FBSR_TYPE_CPU)
        {
            memSize = CPU_PINNED_BUFFER_SIZE;
        }
        else
        {
            memSize = CPU_MAX_PINNED_BUFFER_SIZE;
        }
        //
        // Pre-allocate a page size buffer for CE to DMA into.
        // This buffer is accessed with the CPU so it is best
        // to to use cached memory.
        //
        status = memdescCreate(&pFbsr->pSysMemDesc, pGpu, memSize,
                               0, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                               MEMDESC_FLAGS_NONE);
        if (status != NV_OK)
        {
            NV_ASSERT(status == NV_OK);
            goto fail;
        }

        status = memdescAlloc(pFbsr->pSysMemDesc);
        if (status != NV_OK)
        {
            NV_ASSERT(status == NV_OK);
            memdescDestroy(pFbsr->pSysMemDesc);
            pFbsr->pSysMemDesc = NULL;
            goto fail;
        }

        status = memdescMapOld(pFbsr->pSysMemDesc, 0, memSize, NV_TRUE /*kernel*/ ,
                               NV_PROTECT_READ_WRITE,
                               (pFbsr->type == FBSR_TYPE_FILE ? (void**)&pFbsr->pDmaBuffer:
                                                                (void**)&pFbsr->pPinnedBuffer),
                               (void **)&pFbsr->pMapCookie);
        if (status  != NV_OK)
        {
            NV_ASSERT(0);
            memdescFree(pFbsr->pSysMemDesc);
            memdescDestroy(pFbsr->pSysMemDesc);
            pFbsr->pSysMemDesc = NULL;
            status =  NV_ERR_INSUFFICIENT_RESOURCES;
            goto fail;
        }

        NV_ASSERT(!pFbsr->pSysMemDesc->PteAdjust);
    }
    pFbsr->bInitialized = NV_TRUE;
    return NV_OK;

 fail:
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT) &&
        (pFbsr->type == FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED ||
         pFbsr->type == FBSR_TYPE_WDDM_SLOW_CPU_PAGED))
    {
        osReleaseCpuAddressSpaceUpperBound(pFbsr->pagedBufferInfo.sectionHandle);
    }

    return status;
}

/*!
 * Destroy
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 *
 * @returns None
 */
void
fbsrDestroy_GM107(OBJGPU *pGpu, OBJFBSR *pFbsr)
{
    if (pFbsr->type == FBSR_TYPE_CPU ||
        pFbsr->type == FBSR_TYPE_WDDM_SLOW_CPU_PAGED ||
        pFbsr->type == FBSR_TYPE_FILE)
    {
        if (pFbsr->pSysMemDesc)
        {
            memdescUnmapOld(pFbsr->pSysMemDesc, 1 /*kernel*/, 0,
                            (pFbsr->type == FBSR_TYPE_FILE) ? (void*)pFbsr->pDmaBuffer :
                                                              (void*)pFbsr->pPinnedBuffer,
                            pFbsr->pMapCookie);
            memdescFree(pFbsr->pSysMemDesc);
            memdescDestroy(pFbsr->pSysMemDesc);
            pFbsr->pSysMemDesc = NULL;
        }
    }

    if (pFbsr->type == FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED ||
        pFbsr->type == FBSR_TYPE_WDDM_SLOW_CPU_PAGED)
    {
        osReleaseCpuAddressSpaceUpperBound(pFbsr->pagedBufferInfo.sectionHandle);
    }
}

/*!
 * Start save/restore operation
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 * @param[in]     op           Type of operation
 *
 * @returns NV_OK on success
 */
NV_STATUS
fbsrBegin_GM107(OBJGPU *pGpu, OBJFBSR *pFbsr, FBSR_OP_TYPE op)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * End save/restore operation
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 *
 * @returns NV_OK on success
 */
NV_STATUS
fbsrEnd_GM107(OBJGPU *pGpu, OBJFBSR *pFbsr)
{
    NvBool bIommuEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT);
    NV_STATUS status = NV_OK;

    if (pFbsr->op != FBSR_OP_SIZE_BUF && pFbsr->op != FBSR_OP_DESTROY)
    {
    }

    if (pFbsr->op == FBSR_OP_RESTORE || pFbsr->bOperationFailed || pFbsr->op == FBSR_OP_DESTROY)
    {
        switch (pFbsr->type)
        {
            case FBSR_TYPE_PAGED_DMA:
            case FBSR_TYPE_DMA:
                {
                    if (pFbsr->type == FBSR_TYPE_PAGED_DMA)
                    {
                        memdescUnlock(pFbsr->pSysMemDesc);
                    }
                    memdescFree(pFbsr->pSysMemDesc);
                    memdescDestroy(pFbsr->pSysMemDesc);
                    pFbsr->pSysMemDesc = NULL;
                    break;
                }
            case FBSR_TYPE_WDDM_SLOW_CPU_PAGED:
                // Flush out writes
                osFlushCpuWriteCombineBuffer();
                if (pFbsr->pagedBufferInfo.avblViewSz)
                {
                    if (osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                    NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                    bIommuEnabled) != NV_OK)
                    {
                        pFbsr->bOperationFailed = NV_TRUE;
                    }
                }
                break;
            case FBSR_TYPE_FILE:
                // Flush out writes
                osFlushCpuWriteCombineBuffer();

                // Close the file
                osCloseFile(pFbsr->pagedBufferInfo.sectionHandle);
                break;
            case FBSR_TYPE_CPU:
                {
                    PFBSR_NODE pNode;
                    PFBSR_NODE pNext;

                    // Flush out writes
                    osFlushCpuWriteCombineBuffer();

                    // Free up list
                    pNode = pFbsr->pSysMemNodeHead;

                    while (pNode)
                    {
                        pNext = pNode->pNext;
                        portMemFree(pNode);
                        pNode = pNext;
                    }

                    pFbsr->pSysMemNodeHead = NULL;

                    break;
                }

            case FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED:
                memdescDestroy(pFbsr->pSysMemDesc);
                pFbsr->pSysMemDesc = NULL;
                if(bIommuEnabled)
                {
                    status = osSrUnpinSysmem(pGpu->pOsGpuInfo);
                }
                else
                {
                    status = osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                        NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                        bIommuEnabled);
                }
                if (status!= NV_OK)
                {
                    pFbsr->bOperationFailed = NV_TRUE;
                }
                break;

        }
    }
    else if (pFbsr->op == FBSR_OP_SAVE)
    {
        switch (pFbsr->type)
        {
            case FBSR_TYPE_PAGED_DMA:
                memdescUnlock(pFbsr->pSysMemDesc);
                break;
            case FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED:
                memdescDestroy(pFbsr->pSysMemDesc);
                pFbsr->pSysMemDesc = NULL;
                if(bIommuEnabled)
                {
                    status = osSrUnpinSysmem(pGpu->pOsGpuInfo);
                }
                else
                {
                    status = osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                            NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                            bIommuEnabled);
                }
                if (status != NV_OK)
                {
                    pFbsr->bOperationFailed = NV_TRUE;
                }
                break;
            case FBSR_TYPE_WDDM_SLOW_CPU_PAGED:
                if (pFbsr->pagedBufferInfo.avblViewSz)
                {
                    if (osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                        NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                        bIommuEnabled) != NV_OK)
                    {
                        pFbsr->bOperationFailed = NV_TRUE;
                    }
                }
                break;
        }
    }

    return pFbsr->bOperationFailed ? NV_ERR_GENERIC : NV_OK;
}

/*!
 * Saves or restores a region of video memory.
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 * @param[in]     pVidMemDesc  Memory descriptor for vidmem region
 *
 * @returns None
 */
void
fbsrCopyMemoryMemDesc_GM107(OBJGPU *pGpu, OBJFBSR *pFbsr, MEMORY_DESCRIPTOR *pVidMemDesc)
{
}

#ifdef DEBUG
#endif
