/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/fbsr.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "published/maxwell/gm107/dev_ram.h"
#include "core/thread_state.h"
#include "nvrm_registry.h"

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

static NV_STATUS _fbsrInitGsp
(
    OBJGPU *pGpu,
    OBJFBSR *pFbsr
)
{
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvHandle           hSysMem        = NV01_NULL_OBJECT;
    RM_API            *pRmApi         = GPU_GET_PHYSICAL_RMAPI(pGpu);
    MEMORY_DESCRIPTOR *pGspSysMemDesc = pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP];
    NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS params;

    NV_ASSERT_OR_RETURN(pGspSysMemDesc != NULL, NV_ERR_INVALID_STATE);

    // Register sysmem memdesc with GSP. This creates memlist object
    NV_ASSERT_OK_OR_RETURN(memdescSendMemDescToGSP(pGpu, pGspSysMemDesc, &hSysMem));

    params.hClient    = pMemoryManager->hClient;
    params.hSysMem    = hSysMem;
    params.bEnteringGcoffState  = pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING);

    // Send S/R init information to GSP
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_FBSR_INIT,
                                           &params,
                                           sizeof(params)));

    // Free memlist object
    pRmApi->Free(pRmApi, pMemoryManager->hClient, hSysMem);

    return NV_OK;
}

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

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_126, 
                        pFbsr->pSysMemDesc);
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
    NV_STATUS status = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    pFbsr->op = op;
    pFbsr->bOperationFailed = NV_FALSE;

    if (op != FBSR_OP_SIZE_BUF && op != FBSR_OP_DESTROY)
    {
        if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
        {
            pFbsr->pCe = NULL;
            NV_ASSERT_OK_OR_RETURN(memmgrInitCeUtils(pMemoryManager, NV_FALSE));
        }

        NV_PRINTF(LEVEL_INFO, "%s %lld bytes of data\n",
                  pFbsr->op == FBSR_OP_SAVE ? "saving" : "restoring",
                  pFbsr->length);
    }

    if (op == FBSR_OP_SAVE)
    {
        switch (pFbsr->type)
        {
            case FBSR_TYPE_PAGED_DMA:
            case FBSR_TYPE_DMA:
                //
                // Check if system memory is pre-allocated for DMA type FBSR
                // and use the same for performing FBSR.
                //
                if (pFbsr->pSysReservedMemDesc)
                {
                    //
                    // Validate if reserved system memory size is sufficient,
                    // Otherwise generate the assert and free the
                    // pre-allocated reserved system memory
                    //
                    if (pFbsr->pSysReservedMemDesc->Size >= pFbsr->length)
                    {
                        pFbsr->pSysMemDesc = pFbsr->pSysReservedMemDesc;
                        //
                        // The reference to the reserved memory is transferred
                        // and the pSysMemDesc pointer will be used for actual
                        // FBSR operations. The actual object will be freed
                        // during FBSR_OP_RESTORE operation.
                        //
                        pFbsr->pSysReservedMemDesc = NULL;
                        break;
                    }

                    NV_ASSERT(pFbsr->pSysReservedMemDesc->Size >= pFbsr->length);
                    memdescFree(pFbsr->pSysReservedMemDesc);
                    memdescDestroy(pFbsr->pSysReservedMemDesc);
                    pFbsr->pSysReservedMemDesc = NULL;
                    status = NV_ERR_GENERIC;
                    break;
                }

                if (pFbsr->length)
                {
                    if (pFbsr->type == FBSR_TYPE_DMA)
                    {
                        // This buffer is never touched by the CPU, so it can be uncached.
                        status = memdescCreate(&pFbsr->pSysMemDesc, pGpu,
                                               pFbsr->length, 0, NV_FALSE,
                                               ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                                               MEMDESC_FLAGS_NONE);
                    }
                    else if (pFbsr->type == FBSR_TYPE_PAGED_DMA)
                    {
                        // On Windows, pageable memory is also cacheable.
                        status = memdescCreate(&pFbsr->pSysMemDesc, pGpu,
                                               pFbsr->length, 0, NV_FALSE,
                                               ADDR_SYSMEM, NV_MEMORY_CACHED,
                                               MEMDESC_FLAGS_PAGED_SYSMEM);
                    }
                    if (status != NV_OK)
                    {
                        NV_ASSERT(status == NV_OK);
                        break;
                    }

                    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_127, 
                                    pFbsr->pSysMemDesc);
                    if (status != NV_OK)
                    {
                        NV_ASSERT(status == NV_OK);
                        memdescDestroy(pFbsr->pSysMemDesc);
                        pFbsr->pSysMemDesc = NULL;
                        break;
                    }

                    if (pFbsr->type == FBSR_TYPE_PAGED_DMA)
                    {
                        status = memdescLock(pFbsr->pSysMemDesc);
                        if (status != NV_OK)
                        {
                            NV_ASSERT(status == NV_OK);
                            memdescFree(pFbsr->pSysMemDesc);
                            memdescDestroy(pFbsr->pSysMemDesc);
                            pFbsr->pSysMemDesc = NULL;
                            break;
                        }
                    }
                }

                break;

            case FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED:
                {
                    NvBool bIommuEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT);

                    if (pFbsr->length > pFbsr->pagedBufferInfo.maxLength)
                    {
                        status = NV_ERR_GENERIC;
                        break;
                    }

                    if (bIommuEnabled)
                    {
                        status = osSrPinSysmem(pGpu->pOsGpuInfo,
                                                    pFbsr->length,
                                                    &pFbsr->pagedBufferInfo.pMdl);

                        if (status != NV_OK)
                            break;

                        NV_ASSERT(pFbsr->pagedBufferInfo.pMdl);
                        status = osCreateMemFromOsDescriptorInternal(pGpu,
                                                                     pFbsr->pagedBufferInfo.pMdl,
                                                                     0,
                                                                     pFbsr->length,
                                                                     &pFbsr->pSysMemDesc,
                                                                     NV_TRUE,
                                                                     RS_PRIV_LEVEL_KERNEL
                                                                     );
                        if (status != NV_OK)
                            (void) osSrUnpinSysmem(pGpu->pOsGpuInfo);
                    }
                    else
                    {
                        pFbsr->pagedBufferInfo.sysAddr = 0;
                        status = osMapViewToSection(pGpu->pOsGpuInfo,
                                                    pFbsr->pagedBufferInfo.sectionHandle,
                                                    (void **) (&pFbsr->pagedBufferInfo.sysAddr),
                                                    pFbsr->length, 0, bIommuEnabled);
                        NV_ASSERT(pFbsr->pagedBufferInfo.sysAddr);
                        if (status != NV_OK)
                            break;

                        status = osCreateMemFromOsDescriptorInternal(pGpu,
                                                           NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                                           0,
                                                           pFbsr->length,
                                                           &pFbsr->pSysMemDesc,
                                                           NV_TRUE,
                                                           RS_PRIV_LEVEL_KERNEL);
                        // would return error
                        if (status != NV_OK)
                        {
                             NV_ASSERT(osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                                NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                                bIommuEnabled) == NV_OK);
                        }
                    }
                }
                break;

            case FBSR_TYPE_WDDM_SLOW_CPU_PAGED:
                if (pFbsr->length > pFbsr->pagedBufferInfo.maxLength ||
                    (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT) &&
                     !pFbsr->pagedBufferInfo.sectionHandle))
                {
                    status = NV_ERR_GENERIC;
                    break;
                }
                pFbsr->pagedBufferInfo.avblViewSz = 0;
                // fallthrough
            case FBSR_TYPE_CPU:
                if (!pFbsr->pSysMemDesc)
                {
                    status = NV_ERR_GENERIC;
                }

                break;
            case FBSR_TYPE_PERSISTENT:
                break;
            case FBSR_TYPE_FILE:
                // XXX can this condition ever evaluate to true?
                if (!pFbsr->pSysMemDesc)
                {
                    status = NV_ERR_GENERIC;
                    break;
                }

                // Open a temporary file for writing
                status = osOpenTemporaryFile(&pFbsr->pagedBufferInfo.sectionHandle);
                if (status != NV_OK)
                    break;

                pFbsr->pagedBufferInfo.avblViewSz = 0;
                break;
            default:
                status = NV_ERR_GENERIC;
                NV_ASSERT(0);
                break;
        }

        // Initialize FBSR on GSP
        if ((status == NV_OK) && IS_GSP_CLIENT(pGpu) && (pFbsr->pSysMemDesc != NULL))
        {
            NV_ASSERT_OK_OR_RETURN(_fbsrInitGsp(pGpu, pFbsr));
        }
    }
    else if (pFbsr->op == FBSR_OP_RESTORE || pFbsr->op == FBSR_OP_DESTROY)
    {
        switch (pFbsr->type)
        {
            case FBSR_TYPE_PAGED_DMA:
                status = memdescLock(pFbsr->pSysMemDesc);
                NV_ASSERT(status == NV_OK);
                break;
            case FBSR_TYPE_FILE:
                 if (!pFbsr->pagedBufferInfo.sectionHandle)
                 {
                     status = NV_ERR_GENERIC;
                     break;
                 }
                 pFbsr->pagedBufferInfo.avblViewSz = 0;
                 break;
            case FBSR_TYPE_WDDM_SLOW_CPU_PAGED:
                if (pFbsr->length > pFbsr->pagedBufferInfo.maxLength ||
                    (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT) &&
                     !pFbsr->pagedBufferInfo.sectionHandle))
                {
                    status = NV_ERR_GENERIC;
                    break;
                }
                pFbsr->pagedBufferInfo.avblViewSz = 0;
                break;
            case FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED:
                {
                    NvBool  bIommuEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT);
                    // Error checked during SAVE
                    if (pFbsr->length > pFbsr->pagedBufferInfo.maxLength)
                    {
                        status = NV_ERR_GENERIC;
                        break;
                    }
                    if(bIommuEnabled)
                    {
                        status = osSrPinSysmem(pGpu->pOsGpuInfo,
                                                    pFbsr->length,
                                                    &pFbsr->pagedBufferInfo.pMdl);

                        if (status != NV_OK)
                            break;

                        NV_ASSERT(pFbsr->pagedBufferInfo.pMdl);
                        status = osCreateMemFromOsDescriptorInternal(pGpu,
                                                                     pFbsr->pagedBufferInfo.pMdl,
                                                                     0,
                                                                     pFbsr->length,
                                                                     &pFbsr->pSysMemDesc,
                                                                     NV_TRUE,
                                                                     RS_PRIV_LEVEL_KERNEL
                                                                     );
                        if (status != NV_OK)
                            (void) osSrUnpinSysmem(pGpu->pOsGpuInfo);
                    }
                    else
                    {
                        pFbsr->pagedBufferInfo.sysAddr = 0;
                        status = osMapViewToSection(pGpu->pOsGpuInfo,
                                                    pFbsr->pagedBufferInfo.sectionHandle,
                                                    (void **)(&pFbsr->pagedBufferInfo.sysAddr),
                                                    pFbsr->length, 0, bIommuEnabled);

                        if (status != NV_OK)
                            break;

                        NV_ASSERT(pFbsr->pagedBufferInfo.sysAddr);
                        status = osCreateMemFromOsDescriptorInternal(pGpu,
                                                                     NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                                                     0,
                                                                     pFbsr->length,
                                                                     &pFbsr->pSysMemDesc,
                                                                     NV_TRUE,
                                                                     RS_PRIV_LEVEL_KERNEL);
                        // would return error
                        if (status != NV_OK)
                        {
                            NV_ASSERT(osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                                NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                                bIommuEnabled) == NV_OK);
                        }
                    }
                }

                break;
        }
    }

    pFbsr->pSysMemNodeCurrent = pFbsr->pSysMemNodeHead;
    pFbsr->length = 0;
    pFbsr->sysOffset = 0;

    return status;
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
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (pFbsr->op != FBSR_OP_SIZE_BUF && pFbsr->op != FBSR_OP_DESTROY)
    {

        if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) && (pMemoryManager->pCeUtils != NULL))
        {
            memmgrDestroyCeUtils(pMemoryManager);
        }
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
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS  status = NV_OK;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    pVidMemDesc = memdescGetMemDescFromGpu(pVidMemDesc, pGpu);

    if (pFbsr->bOperationFailed)
    {
        // If we hit a failure igonre the rest of the copy requests
        return;
    }

    pFbsr->length += pVidMemDesc->Size;

    // We should have nothing reserved when FB is broken
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB))
    {
        NV_ASSERT(pVidMemDesc->Size == 0);
        NV_PRINTF(LEVEL_WARNING, "return early since FB is broken!\n");
        return;
    }

    if (pFbsr->op == FBSR_OP_SIZE_BUF)
    {
        switch (pFbsr->type)
        {
            case FBSR_TYPE_CPU:
                {
                    PFBSR_NODE pNode;

                    pNode = portMemAllocNonPaged(
                        sizeof(FBSR_NODE) + (NvU32)pVidMemDesc->Size - sizeof(pNode->data));

                    if (pNode == NULL)
                    {
                        NV_ASSERT(0);
                        pFbsr->bOperationFailed = NV_TRUE;
                        return;
                    }

                    pNode->pNext = NULL;

                    // Insert node
                    if (!pFbsr->pSysMemNodeHead)
                    {
                        pFbsr->pSysMemNodeHead = pNode;
                    }
                    else
                    {
                        pFbsr->pSysMemNodeCurrent->pNext = pNode;
                    }

                    pFbsr->pSysMemNodeCurrent = pNode;

                    break;
                }
            case FBSR_TYPE_PERSISTENT:
                {
                    MEMORY_DESCRIPTOR *pStandbyBuffer;

                    if (memdescGetStandbyBuffer(pVidMemDesc) == NULL)
                    {
                        status = memdescCreate(&pStandbyBuffer, pGpu,
                                               pVidMemDesc->Size, 0, NV_FALSE,
                                               ADDR_SYSMEM,
                                               NV_MEMORY_WRITECOMBINED,
                                               MEMDESC_FLAGS_NONE);
                        if (status == NV_OK)
                        {
                            memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_128, 
                                            pStandbyBuffer);
                            if (status != NV_OK )
                            {
                                memdescDestroy(pStandbyBuffer);
                                pFbsr->bOperationFailed = NV_TRUE;
                                break;
                            }
                            memdescSetStandbyBuffer(pVidMemDesc, pStandbyBuffer);
                        }
                        else
                        {
                            pFbsr->bOperationFailed = NV_TRUE;
                        }
                    }
                    break;
                }
            default:
                break;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "%s allocation %llx-%llx [%s]\n",
                  pFbsr->op == FBSR_OP_SAVE ? "saving" : "restoring",
                  memdescGetPhysAddr(pVidMemDesc, AT_GPU, 0),
                  memdescGetPhysAddr(pVidMemDesc, AT_GPU, 0) + pVidMemDesc->Size - 1,
                  pFbsr->type == FBSR_TYPE_DMA ? "DMA" : "CPU");

        switch (pFbsr->type)
        {
            case FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED:
            case FBSR_TYPE_PAGED_DMA:
            case FBSR_TYPE_DMA:
                {
                    TRANSFER_SURFACE   vidSurface = {0};
                    TRANSFER_SURFACE   sysSurface = {0};

                    vidSurface.pMemDesc = pVidMemDesc;
                    sysSurface.pMemDesc = pFbsr->pSysMemDesc;
                    sysSurface.offset   = pFbsr->sysOffset;

                    if (pFbsr->op == FBSR_OP_RESTORE)
                    {
                        NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &vidSurface, &sysSurface, pVidMemDesc->Size,
                                                   TRANSFER_FLAGS_PREFER_CE | TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH));
                    }
                    else
                    {
                        NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &sysSurface, &vidSurface, pVidMemDesc->Size,
                                                   TRANSFER_FLAGS_PREFER_CE | TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH));
                    }
                    break;
                }
            case FBSR_TYPE_PERSISTENT:
                {
                    TRANSFER_SURFACE   vidSurface = {0};
                    TRANSFER_SURFACE   sysSurface = {0};

                    vidSurface.pMemDesc = pVidMemDesc;
                    sysSurface.pMemDesc = memdescGetStandbyBuffer(pVidMemDesc);

                    if (pFbsr->op == FBSR_OP_RESTORE)
                    {
                        NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &vidSurface, &sysSurface, pVidMemDesc->Size,
                                                   TRANSFER_FLAGS_PREFER_CE | TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH));
                    }
                    else
                    {
                        NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &sysSurface, &vidSurface, pVidMemDesc->Size,
                                                   TRANSFER_FLAGS_PREFER_CE | TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH));
                    }
                    break;
                }
            case FBSR_TYPE_WDDM_SLOW_CPU_PAGED:
                {
                    NvU64      totalCopySize = pVidMemDesc->Size;
                    NvU64      vidOffset = 0;
                    NvU64      copySize, i;
                    NvU32      *cpuCopyOffset = 0;
                    NvBool     bIommuEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT);

                    // Saves/restores a fbregion at memdesc granularity
                    while(totalCopySize)
                    {
                        // Backing system buffer (paged VA) is 64k and only map new 64k section
                        // when backing buffer is consumed.
                        if (pFbsr->pagedBufferInfo.avblViewSz == 0)
                        {
                            NV_ASSERT(((pFbsr->sysOffset + vidOffset)& 0xffff) == 0);
                            pFbsr->pagedBufferInfo.avblViewSz = CPU_MAX_PINNED_BUFFER_SIZE;
                            pFbsr->pagedBufferInfo.sysAddr = 0;

                            // Get VA to a 64K view in the section at the offset sysOffset + vidOffset
                            // sysOffset tracks across memdesc, vidOffset tracks the 4k copy
                            status = osMapViewToSection(pGpu->pOsGpuInfo,
                                                  pFbsr->pagedBufferInfo.sectionHandle,
                                                  (void **)(&pFbsr->pagedBufferInfo.sysAddr),
                                                  pFbsr->pagedBufferInfo.avblViewSz,
                                                  pFbsr->sysOffset + vidOffset,
                                                  bIommuEnabled);
                            if (status != NV_OK)
                            {
                                pFbsr->bOperationFailed = NV_TRUE;
                                NV_ASSERT(0);
                                break;
                            }
                        }
                        // Compute the cpuOffset to copy to / from
                        cpuCopyOffset = KERNEL_POINTER_FROM_NvP64(NvU32*, NvP64_PLUS_OFFSET(pFbsr->pagedBufferInfo.sysAddr,
                                                                   ((NvU64) CPU_MAX_PINNED_BUFFER_SIZE - pFbsr->pagedBufferInfo.avblViewSz)));
                        copySize = NV_MIN(pFbsr->pagedBufferInfo.avblViewSz, totalCopySize);

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            // pPinnedBuffer is the 64K scratch buffer
                            // cpuCopyOffset is the mapped paged CPU VA
                            // Restore from the backing store to the pinned 64k scratch buffer
                            // We copy the region in 4byte granularity
                            for (i = 0; i < (copySize / 4); i++)
                            {
                                pFbsr->pPinnedBuffer[i] = cpuCopyOffset[i];
                            }
                        }

                        TRANSFER_SURFACE vidSurface = {0};
                        vidSurface.pMemDesc = pVidMemDesc;
                        vidSurface.offset   = vidOffset;

                        TRANSFER_SURFACE sysSurface = {0};
                        sysSurface.pMemDesc = pFbsr->pSysMemDesc;

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &vidSurface, &sysSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }
                        else
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &sysSurface, &vidSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }

                        if (pFbsr->op == FBSR_OP_SAVE)
                        {
                            // Copy from the scratch buffer to the sysmem backing store
                            for (i = 0; i < (copySize / 4); i++)
                            {
                                cpuCopyOffset[i] = pFbsr->pPinnedBuffer[i];
                            }
                        }

                        vidOffset += copySize;
                        totalCopySize -= copySize;
                        pFbsr->pagedBufferInfo.avblViewSz -= copySize;
                        if (pFbsr->pagedBufferInfo.avblViewSz == 0)
                        {
                            status = osUnmapViewFromSection(pGpu->pOsGpuInfo,
                                            NvP64_VALUE(pFbsr->pagedBufferInfo.sysAddr),
                                            bIommuEnabled);
                            if (status != NV_OK)
                            {
                                pFbsr->bOperationFailed = NV_TRUE;
                                NV_ASSERT(0);
                                break;
                            }
                        }
                    }
                }
                break;
            case FBSR_TYPE_FILE:
                {
                    NvU64      totalCopySize = pVidMemDesc->Size;
                    NvU64      vidOffset = 0;
                    NvU64      copySize;
                    NvU64      threadTimeoutCopySize = 0;

                    //
                    // File based operation can take longer time in completion, if the
                    // FB usage is high. Also, the File read/write time is
                    // system dependent (the temporary file location, Free
                    // system RAM, secondary storage type, etc.). Reset the thread
                    // timeout at the starting and at the end to prevent GPU
                    // thread timeout errors. Also, keep track of file copy size
                    // since last threadStateResetTimeout() in variable
                    // threadTimeoutCopySize. When this copy size go above certain
                    // threshold, then also reset the thread timeout and
                    // threadTimeoutCopySize variable.
                    //
                    NV_ASSERT_OK(threadStateResetTimeout(pGpu));

                    // Saves/restores a fbregion at memdesc granularity
                    while (totalCopySize)
                    {
                        // Backing system buffer (paged VA) is 64k and only map new 64k section
                        // when backing buffer is consumed.
                        if (pFbsr->pagedBufferInfo.avblViewSz == 0)
                        {
                            NV_ASSERT(((pFbsr->sysOffset + vidOffset) & (CPU_MAX_PINNED_BUFFER_SIZE - 1)) == 0);
                            pFbsr->pagedBufferInfo.avblViewSz = CPU_MAX_PINNED_BUFFER_SIZE;
                        }

                        copySize = NV_MIN(pFbsr->pagedBufferInfo.avblViewSz, totalCopySize);

                        if (threadTimeoutCopySize >= MAX_FILE_COPY_SIZE_WITHIN_DEFAULT_THREAD_TIMEOUT)
                        {
                            NV_ASSERT_OK(threadStateResetTimeout(pGpu));
                            threadTimeoutCopySize = 0;
                        }

                        threadTimeoutCopySize += copySize;

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            // pPinnedBuffer is the 64K scratch buffer
                            // cpuCopyOffset is the mapped paged CPU VA
                            // Restore from the backing store to the pinned 64k scratch buffer
                            // We copy the region in 4byte granularity
                                                            // replace this with file read
                            status = osReadFromFile(pFbsr->pagedBufferInfo.sectionHandle,
                                                    &pFbsr->pDmaBuffer[0],
                                                    copySize,
                                                    pFbsr->sysOffset + vidOffset);
                            if (status != NV_OK)
                            {
                                pFbsr->bOperationFailed = NV_TRUE;
                                NV_ASSERT(0);
                                break;
                            }
                        }

                        TRANSFER_SURFACE vidSurface = {0};
                        vidSurface.pMemDesc = pVidMemDesc;
                        vidSurface.offset   = vidOffset;

                        TRANSFER_SURFACE sysSurface = {0};
                        sysSurface.pMemDesc = pFbsr->pSysMemDesc;

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &vidSurface, &sysSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }
                        else
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &sysSurface, &vidSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }

                        if (pFbsr->op == FBSR_OP_SAVE)
                        {
                            status = osWriteToFile(pFbsr->pagedBufferInfo.sectionHandle,
                                                   &pFbsr->pDmaBuffer[0],
                                                   copySize,
                                                   pFbsr->sysOffset + vidOffset);
                            if (status != NV_OK)
                            {
                                pFbsr->bOperationFailed = NV_TRUE;
                                NV_ASSERT(0);
                                break;
                            }
                        }

                        vidOffset += copySize;
                        totalCopySize -= copySize;
                        pFbsr->pagedBufferInfo.avblViewSz -= copySize;
                    }
                }

                NV_ASSERT_OK(threadStateResetTimeout(pGpu));
                break;
            case FBSR_TYPE_CPU:
                {
                    PFBSR_NODE pNode = pFbsr->pSysMemNodeCurrent;
                    NvU64      vidOffset = 0;
                    NvU64      copySize;
                    NvU64      size = pVidMemDesc->Size;
                    NvU64      i;

                    NV_ASSERT(pNode);
                    NV_ASSERT((pFbsr->sysOffset & 3) == 0);

                    while (size)
                    {
                        copySize = NV_MIN(CPU_PINNED_BUFFER_SIZE, size);

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            for (i = 0; i < (copySize / 4); i++)
                            {
                                pFbsr->pPinnedBuffer[i] = pNode->data[i];
                            }
                        }

                        TRANSFER_SURFACE vidSurface = {0};
                        vidSurface.pMemDesc = pVidMemDesc;
                        vidSurface.offset   = vidOffset;

                        TRANSFER_SURFACE sysSurface = {0};
                        sysSurface.pMemDesc = pFbsr->pSysMemDesc;

                        if (pFbsr->op == FBSR_OP_RESTORE)
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &vidSurface, &sysSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }
                        else
                        {
                            NV_ASSERT_OK(memmgrMemCopy(pMemoryManager, &sysSurface, &vidSurface, copySize, TRANSFER_FLAGS_PREFER_CE));
                        }

                        if (pFbsr->op == FBSR_OP_SAVE)
                        {
                            for (i = 0; i < (copySize / 4); i++)
                            {
                                pNode->data[i] = pFbsr->pPinnedBuffer[i];
                            }
                        }

                        vidOffset += copySize;
                        size -= copySize;
                    }

                    pFbsr->pSysMemNodeCurrent = pFbsr->pSysMemNodeCurrent->pNext;
                }

                break;
        }

        pFbsr->sysOffset += pVidMemDesc->Size;
    }
}

#ifdef DEBUG
#endif
