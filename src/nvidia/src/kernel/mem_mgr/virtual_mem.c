/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/virtual_mem.h"
#include "mem_mgr/vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "core/locks.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "gpu/device/device.h"
#include "Nvcm.h"
#include "gpu/mem_mgr/vaspace_api.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "mem_mgr/gpu_vaspace.h"
#include "platform/sli/sli.h"
#include "kernel/rmapi/mapping_list.h"

#include "class/cl0070.h" // NV01_MEMORY_VIRTUAL
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL

static void _virtmemFreeKernelMapping(OBJGPU *, CLI_DMA_MAPPING_INFO *);

/*!
 * _virtmemQueryVirtAllocParams
 *
 * @brief
 *     Queries for the actual size of VA allocation, alignment
 *     and mask of page sizes (needed for page table allocation)
 *
 * @param[in]  pGpu                OBJGPU pointer
 * @param[in]  hClient             Client handle
 * @param[in]  hDevice             Device handle
 * @param[in]  pAllocData          Pointer to VIDHEAP_ALLOC_DATA
 * @param[out] pAlign              Alignment
 * @param[out] pSize               Size of allocation
 * @param[out] ppVAS               Virtual address space for request
 * @param[out] pPageSizeLockMask   Mask of page sizes locked during VA reservation
 *
 * @returns
 *      NV_OK
 */
static NV_STATUS
_virtmemQueryVirtAllocParams
(
    OBJGPU                     *pGpu,
    NvHandle                    hClient,
    NvHandle                    hDevice,
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData,
    NvU64                       *pAlign,
    NvU64                       *pSize,
    OBJVASPACE                 **ppVAS,
    NvU64                       *pPageSizeLockMask
)
{
    MemoryManager          *pMemoryManager     = GPU_GET_MEMORY_MANAGER(pGpu);
    RsClient               *pClient;
    FB_ALLOC_INFO          *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat = NULL;
    NV_STATUS               status             = NV_OK;
    NvBool                  bReleaseGpuLock    = NV_FALSE;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pFbAllocInfo == NULL)
    {
        NV_ASSERT(0);
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    if (pFbAllocPageFormat == NULL) {
        NV_ASSERT(0);
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    pFbAllocInfo->pageFormat->attr  = pAllocData->attr;
    pFbAllocInfo->pageFormat->attr2 = pAllocData->attr2;
    pFbAllocInfo->pageFormat->flags = pAllocData->flags;
    *pSize                          = pAllocData->size;
    *pAlign                         = pAllocData->alignment;

    // LOCK: acquire device lock
    if (!rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)))
    {
        NV_ASSERT_OK_OR_GOTO(status, rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                                             RM_LOCK_MODULES_MEM_PMA), done);
        bReleaseGpuLock = NV_TRUE;
    }

    *pPageSizeLockMask = stdmemQueryPageSize(pMemoryManager, hClient,
                                             pAllocData);
    if (*pPageSizeLockMask == 0)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        serverGetClientUnderLock(&g_resServ, hClient, &pClient),
        done);

    NV_ASSERT_OK_OR_GOTO(status,
        vaspaceGetByHandleOrDeviceDefault(pClient, hDevice, pAllocData->hVASpace, ppVAS),
        done);

    NV_ASSERT_OK_OR_GOTO(status,
        vaspaceApplyDefaultAlignment(*ppVAS, pFbAllocInfo, pAlign, pSize, pPageSizeLockMask),
        done);

done:
    if (bReleaseGpuLock)
    {
        // UNLOCK: release device lock
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }

    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

/*!
 * @brief Handle copy construction for VirtualMemory object
 */
static NV_STATUS
_virtmemCopyConstruct
(
    VirtualMemory *pDstVirtualMemory,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsClient      *pDstClient = pCallContext->pClient;
    RsClient      *pSrcClient = pParams->pSrcClient;
    RsResourceRef *pSrcRef    = pParams->pSrcRef;
    VirtualMemory *pSrcVirtualMemory = dynamicCast(pSrcRef->pResource, VirtualMemory);
    Memory        *pDstMemory = staticCast(pDstVirtualMemory, Memory);
    Memory        *pSrcMemory = staticCast(pSrcVirtualMemory, Memory);
    OBJGPU        *pSrcGpu = pSrcMemory->pGpu;
    OBJVASPACE    *pVASSrc = NULL;
    NvBool         bIncAllocRefCnt = NV_FALSE;

    // Special handling for Dup of the FLA VASpace
    if (pSrcVirtualMemory->bFlaVAS)
    {
        Device        *pDstDevice;
        RsClient      *pFlaClient;
        RM_API        *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NvHandle       hImportedVASpace = NV01_NULL_OBJECT;
        RsResourceRef *pDupedVasRef;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            serverGetClientUnderLock(&g_resServ, GPU_GET_KERNEL_BUS(pSrcGpu)->flaInfo.hClient, &pFlaClient));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            vaspaceGetByHandleOrDeviceDefault(pFlaClient,
                                              RES_GET_HANDLE(pSrcMemory->pDevice),
                                              GPU_GET_KERNEL_BUS(pSrcGpu)->flaInfo.hFlaVASpace,
                                              &pVASSrc));

        //
        // FLA Memory can be duped during import stage and the importing client which might not be the
        // same as exporting client. Also the importing client might not also bind to the the exporting FLA
        // VASpace on the exporting device. In that case, we might see leaks in the exporting FLA VASpace.
        // To avoid those scenarios, we are duping the FLA VAS to the importing client under the exporting device.
        // RS-TODO: Bug 3059751 to track the duped VAS as dependant in ResServer
        //
        NV_ASSERT_OK_OR_RETURN(deviceGetByGpu(pDstClient, pSrcGpu, NV_TRUE, &pDstDevice));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->DupObject(pRmApi,
                              pDstClient->hClient,
                              RES_GET_HANDLE(pDstDevice),
                              &hImportedVASpace,
                              GPU_GET_KERNEL_BUS(pSrcGpu)->flaInfo.hClient,
                              GPU_GET_KERNEL_BUS(pSrcGpu)->flaInfo.hFlaVASpace,
                              0));

        if (clientGetResourceRef(pDstClient, hImportedVASpace, &pDupedVasRef) == NV_OK)
            refAddDependant(pDupedVasRef, RES_GET_REF(pDstVirtualMemory));

        pDstVirtualMemory->hVASpace = hImportedVASpace;

        // Increase refcount if locally managed
        bIncAllocRefCnt = !pSrcMemory->bRpcAlloc;
    }
    else if (pSrcVirtualMemory->hVASpace == NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE)
    {
        // A legacy sysmem dynamic object does not have valid hVASpace
        pDstVirtualMemory->hVASpace = NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE;

        // No VASPACE is update
        bIncAllocRefCnt = NV_FALSE;
    }
    else
    {
        OBJVASPACE *pVASDst = NULL;

        NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
            vaspaceGetByHandleOrDeviceDefault(pSrcClient,
                                              RES_GET_HANDLE(pSrcMemory->pDevice),
                                              pSrcVirtualMemory->hVASpace, &pVASSrc));
        NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
            vaspaceGetByHandleOrDeviceDefault(pDstClient,
                                              RES_GET_HANDLE(pDstMemory->pDevice),
                                              NV01_NULL_OBJECT, &pVASDst));
        if (pVASSrc != pVASDst)
        {
            return NV_ERR_INVALID_DEVICE;
        }

        pDstVirtualMemory->hVASpace = NV01_NULL_OBJECT;

        // Increase refcount for locally managed NV50_MEMORY_VIRTUAL
        bIncAllocRefCnt = pSrcVirtualMemory->bReserveVaOnAlloc && !pSrcMemory->bRpcAlloc;
    }

    pDstVirtualMemory->bAllowUnicastMapping = pSrcVirtualMemory->bAllowUnicastMapping;
    pDstVirtualMemory->bReserveVaOnAlloc = pSrcVirtualMemory->bReserveVaOnAlloc;
    pDstVirtualMemory->bFlaVAS = pSrcVirtualMemory->bFlaVAS;

    // Mappings do not follow virtual memory object
    pDstVirtualMemory->pDmaMappingList = NULL;

    if (bIncAllocRefCnt)
    {
        NvU64 vaddr;
        NvU64 size;

        virtmemGetAddressAndSize(pSrcVirtualMemory, &vaddr, &size);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            vaspaceIncAllocRefCnt(pVASSrc, vaddr));
    }

    return NV_OK;
}

/*!
 * virtmemConstruct
 *
 * @brief
 *     This routine provides common allocation services used by the
 *     following heap allocation functions:
 *       NVOS32_FUNCTION_ALLOC_SIZE
 *       NVOS32_FUNCTION_ALLOC_SIZE_RANGE
 *       NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT
 *
 * @param[in]  pVirtualMemory    Pointer to VirtualMemory object
 * @param[in]  pCallContext      Pointer to the current CALL_CONTEXT.
 * @param[in]  pParams           Pointer to the alloc params
 *
 * @return 'NV_OK'
 *     Operation completed successfully.
 * @return 'NV_ERR_NO_MEMORY'
 *     There is not enough available memory to satisfy allocation request.
 * @return 'NV_ERR_INSUFFICIENT_RESOURCES'
 *     Not enough available resources to satisfy allocation request.
 */
NV_STATUS
virtmemConstruct_IMPL
(
    VirtualMemory                *pVirtualMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                      *pMemory               = staticCast(pVirtualMemory, Memory);
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData            = pParams->pAllocParams;
    MEMORY_ALLOCATION_REQUEST    allocRequest          = {0};
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest         = &allocRequest;
    OBJGPU                      *pGpu                  = pMemory->pGpu;
    MemoryManager               *pMemoryManager        = GPU_GET_MEMORY_MANAGER(pGpu);
    OBJVASPACE                  *pVAS                  = NULL;
    HWRESOURCE_INFO              hwResource;
    RsClient                    *pRsClient             = pCallContext->pClient;
    RmClient                    *pRmClient             = dynamicCast(pRsClient, RmClient);
    RsResourceRef               *pResourceRef          = pCallContext->pResourceRef;
    RsResourceRef               *pVASpaceRef           = NULL;
    NvU32                        gpuCacheAttrib;
    NV_STATUS                    status                = NV_OK;
    NvHandle                     hClient               = pCallContext->pClient->hClient;
    NvHandle                     hParent               = pCallContext->pResourceRef->pParentRef->hResource;
    NvU64                        sizeOut;
    NvU64                        offsetOut;
    NvBool                       bLockAcquired         = NV_FALSE;
    NvU32                        attr                  = 0;
    NvU32                        attr2                 = 0;
    NvBool                       bRpcAlloc             = NV_FALSE;
    NvBool                       bResAllocated         = NV_FALSE;
    NvU32                        gpuMask               = 0;
    FB_ALLOC_INFO               *pFbAllocInfo          = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat    = NULL;

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    // Bulk of copy-construction is done by Memory class. Handle our members.
    if (RS_IS_COPY_CTOR(pParams))
    {
        NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                     GPU_LOCK_GRP_ALL,
                                                     GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                                                     RM_LOCK_MODULES_MEM,
                                                     &gpuMask));

        status = _virtmemCopyConstruct(pVirtualMemory, pCallContext, pParams);

        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

        goto done;
    }

    pVirtualMemory->hVASpace = RM_INVALID_VASPACE_HANDLE;
    pVirtualMemory->bAllowUnicastMapping = NV_FALSE;
    pVirtualMemory->bReserveVaOnAlloc = NV_FALSE;
    pVirtualMemory->bFlaVAS = NV_FALSE;
    pVirtualMemory->pDmaMappingList = NULL;

    // NV01_MEMORY_VIRTUAL does not allocate typed memory from the heap
    if (pParams->externalClassId == NV01_MEMORY_VIRTUAL)
        return NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, stdmemValidateParams(pGpu, pRmClient, pAllocData));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pAllocData->flags & NVOS32_ALLOC_FLAGS_VIRTUAL, NV_ERR_INVALID_ARGUMENT);

    stdmemDumpInputAllocParams(pAllocData, pCallContext);

    attr  = pAllocData->attr;
    attr2 = pAllocData->attr2;

    pAllocRequest->classNum = NV50_MEMORY_VIRTUAL;
    pAllocRequest->pUserParams = pAllocData;
    pAllocRequest->hMemory = pResourceRef->hResource;
    pAllocRequest->hClient = hClient;
    pAllocRequest->hParent = hParent;
    pAllocRequest->pGpu = pGpu;
    pAllocRequest->internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_CLIENTALLOC;
    pAllocRequest->pHwResource = &hwResource;

    // Unsure if we need to keep separate copies, but keeping old behavior for now.
    sizeOut = pAllocData->size;
    offsetOut = pAllocData->offset;

    //
    // Reserve memory for page tables in case of non lazy page table
    // allocations.
    //
    // PageLevelMemReserve will reserve only if the PDB property for
    // client managed page tables is set.
    //
    if (memmgrIsPmaInitialized(pMemoryManager) &&
        !(pAllocData->flags & NVOS32_ALLOC_FLAGS_LAZY) &&
        !(pAllocData->flags & NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED))
    {
        NvU64 size;
        NvU64 align;
        NvU64 pageSizeLockMask;
        Device *pDevice;

        NV_ASSERT_OK_OR_GOTO(status,
            deviceGetByHandle(pRsClient, hParent, &pDevice),
            done);

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

        pAllocRequest->pGpu = pGpu;
        size             = 0;
        align            = 0;
        pageSizeLockMask = 0;

        status = _virtmemQueryVirtAllocParams(pGpu, hClient, hParent,
                                              pAllocData, &align, &size,
                                              &pVAS, &pageSizeLockMask);
        if (NV_OK != status)
            SLI_LOOP_GOTO(done);

        status = vaspaceReserveMempool(pVAS, pGpu, pDevice,
                                       size, pageSizeLockMask,
                                       VASPACE_RESERVE_FLAGS_NONE);
        if (NV_OK != status)
            SLI_LOOP_GOTO(done);

        SLI_LOOP_END;
    }

    if (RMCFG_FEATURE_RM_BASIC_LOCK_MODEL)
    {
        //
        // Can't move locking up as PMA locks need to be taken first.
        // Acquire the lock *only after* PMA is done allocating.
        //
        if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance) && !rmGpuLockIsOwner())
        {
            NV_ASSERT_OK_OR_GOTO(status,
                rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                    RM_LOCK_MODULES_MEM),
                done);

            bLockAcquired = NV_TRUE;
        }
    }

    {
        //
        // If using thwap to generate an allocation failure here, fail the
        // alloc right away.
        //
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
        if (pKernelRc != NULL &&
            !krcTestAllowAlloc(pGpu, pKernelRc,
                               NV_ROBUST_CHANNEL_ALLOCFAIL_HEAP))
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto done;
        }
    }

    // Validate virtual address space
    NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
        vaspaceGetByHandleOrDeviceDefault(pRsClient, hParent, pAllocData->hVASpace, &pVAS),
        done);

    pVirtualMemory->bFlaVAS = !!(vaspaceGetFlags(pVAS) & VASPACE_FLAGS_FLA);
    pVirtualMemory->bOptimizePageTableMempoolUsage =
        !!(vaspaceGetFlags(pVAS) & VASPACE_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NvBool bSriovFull = IS_VIRTUAL_WITH_SRIOV(pGpu) &&
                            !gpuIsWarBug200577889SriovHeavyEnabled(pGpu);
        NvBool bBar1VAS   = !!(vaspaceGetFlags(pVAS) & VASPACE_FLAGS_BAR_BAR1);

        //
        // Skip RPC to the Host RM when local RM is managing page tables.  Special case
        // for early SR-IOV that only manages BAR1 and FLA page tables in the guest.
        //
        bRpcAlloc = !(gpuIsSplitVasManagementServerClientRmEnabled(pGpu) ||
                        (bSriovFull && (bBar1VAS || pVirtualMemory->bFlaVAS)));
    }

    if (bRpcAlloc)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
            memdescCreate(&pAllocRequest->pMemDesc, pGpu,
                          pAllocRequest->pUserParams->size, 0, NV_TRUE,
                          ADDR_VIRTUAL,
                          NV_MEMORY_UNCACHED,
                          MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE),
            done);
    }
    else
    {
        pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
        NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, done);

        pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
        NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, done);

        portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
        portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
        pFbAllocInfo->pageFormat = pFbAllocPageFormat;

        memUtilsInitFBAllocInfo(pAllocRequest->pUserParams, pFbAllocInfo, hClient, hParent);

        // Call memmgr to get memory.
        NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
            memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo),
            done);

        NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
            virtmemAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo, pRmClient),
            done);

        bResAllocated = NV_TRUE;
    }

    NV_ASSERT(pAllocRequest->pMemDesc != NULL);

    // Copy final heap size/offset back to client struct
    //
    // What should we return ?. System or the Device physical address.
    // Return the Device physical address for now.
    // May change with the heap refactoring !.
    //
    // System and Device physical address can be got using the nv0041CtrlCmdGetSurfacePhysAttr ctrl call
    offsetOut = memdescGetPhysAddr(pAllocRequest->pMemDesc, AT_GPU, 0);
    sizeOut   = pAllocRequest->pMemDesc->Size;
    pAllocData->limit = sizeOut - 1;

    // To handle < nv50
    if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pAllocData->attr2) ==
        NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT)
    {
        pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO,
                                        pAllocData->attr2);
    }

    if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pAllocData->attr2) ==
        NVOS32_ATTR2_GPU_CACHEABLE_YES)
    {
        gpuCacheAttrib = NV_MEMORY_CACHED;
    }
    else
    {
        gpuCacheAttrib = NV_MEMORY_UNCACHED;
    }

    //
    // Issue RPC if page tables are managed in the Host/GSP RM.  This depends on
    // the type object we have and the VGPU/GSP mode.  We issue this prior to
    // as memConstructCommon as RPC fills in pAllocData->offset.
    //
    if (bRpcAlloc)
    {
        NV_RM_RPC_ALLOC_VIRTMEM(pGpu,
                                hClient,
                                hParent,
                                pAllocData->hVASpace,
                                pAllocRequest->hMemory,
                                &pAllocData->offset,
                                pAllocRequest->pMemDesc->Size,
                                attr,
                                attr2,
                                pAllocData->type,
                                pAllocData->flags,
                                pAllocData->height,
                                pAllocData->width,
                                pAllocData->format,
                                pAllocData->comprCovg,
                                pAllocData->zcullCovg,
                                pAllocData->rangeLo,
                                pAllocData->rangeHi,
                                pAllocData->alignment,
                                status);
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, done);

        // Update memory descriptor with results of the RPC
        memdescDescribe(pAllocRequest->pMemDesc,
                        memdescGetAddressSpace(pAllocRequest->pMemDesc),
                        pAllocData->offset,
                        pAllocRequest->pMemDesc->Size);

        // Assign offset back to caller
        offsetOut = memdescGetPhysAddr(pAllocRequest->pMemDesc, AT_GPU, 0);
    }

    //
    // The idea is to allocate virtual address space and record it (lo, limit) in this mem
    // object. Later call MapMemoryDma(hThisMem, hSomePhysMem) to back it.
    //
    NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
        memConstructCommon(pMemory, pAllocRequest->classNum, pAllocData->flags,
                           pAllocRequest->pMemDesc, pAllocData->owner, NULL, pAllocData->attr,
                           pAllocData->attr2, 0, pAllocData->type, NVOS32_MEM_TAG_NONE, NULL),
        done);
    pMemory->bRpcAlloc = bRpcAlloc;

    pVirtualMemory->hVASpace = pAllocData->hVASpace;
    pVirtualMemory->bReserveVaOnAlloc = NV_TRUE;

    if (pAllocData->hVASpace != NV01_NULL_OBJECT)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_SILENT,
            clientGetResourceRef(pRsClient, pAllocData->hVASpace, &pVASpaceRef),
            done);
        if (pVASpaceRef != NULL)
            refAddDependant(pVASpaceRef, pResourceRef);
    }

    NV_ASSERT(pMemory->pMemDesc);
    NV_ASSERT(memdescGetAddressSpace(pMemory->pMemDesc) == ADDR_VIRTUAL);
    memdescSetGpuCacheAttrib(pMemory->pMemDesc, gpuCacheAttrib);

    pAllocData->size = sizeOut;
    pAllocData->offset = offsetOut;

    stdmemDumpOutputAllocParams(pAllocData);

done:
    if (status != NV_OK)
    {
        if (pAllocRequest->pMemDesc != NULL)
        {
            if (pMemory->pMemDesc != NULL)
            {
                memDestructCommon(pMemory);
                pMemory->pMemDesc = NULL;
            }

            if (bResAllocated)
            {
                memmgrFree(pGpu, pMemoryManager, NULL,
                           hClient, hParent, pAllocData->hVASpace,
                           pAllocData->owner,
                           pAllocRequest->pMemDesc);
            }

            if (bRpcAlloc)
            {
                memdescDestroy(pAllocRequest->pMemDesc);
            }
        }
        // vaspaceReserveMempool allocations are clean up is managed independently
    }

    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    if (bLockAcquired)
    {
        // UNLOCK: release GPUs lock
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }

    return status;
}

/*!
 * @brief Handle destruction of VirtualMemory specific fields
 */
void
virtmemDestruct_IMPL
(
    VirtualMemory *pVirtualMemory
)
{
    Memory             *pMemory         = staticCast(pVirtualMemory, Memory);
    OBJGPU             *pGpu            = pMemory->pGpu;
    MemoryManager      *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
    NvHandle            hClient;
    NvHandle            hParent;
    NvHandle            hVASpace;
    MEMORY_DESCRIPTOR  *pMemDesc;
    NvU32               heapOwner;
    NV_STATUS           status = NV_OK;

    // Save needed state from memory object before common destruction
    hClient = RES_GET_CLIENT_HANDLE(pVirtualMemory);
    hParent = RES_GET_PARENT_HANDLE(pVirtualMemory);
    hVASpace = pVirtualMemory->hVASpace;
    pMemDesc = pMemory->pMemDesc;
    heapOwner = pMemory->HeapOwner;

    if (!pMemory->bConstructed)
        return;

    NV_ASSERT(pMemDesc);

    memDestructCommon(pMemory);

    //
    // NV50_MEMORY_VIRTUAL may have underlying heap allocation associated with the object
    // to free depending on which RM/VGPU context we are in. This is tracked at object
    // creation time.
    //
    // If we RPCed a NV50_MEMORY_VIRTUAL or we have a NV01_MEMORY_VIRTUAL than just destroy
    // the memdesc and RPC the free if required.
    //
    if (pMemory->bRpcAlloc || pMemory->categoryClassId == NV01_MEMORY_VIRTUAL)
    {
        NV_ASSERT(pMemDesc->Allocated == 0);
        memdescDestroy(pMemDesc);
    }
    else
    {
        NV_ASSERT(heapOwner != 0);

        // Get the relevant information from the client memory info and free it
        status = memmgrFree(pGpu,
                            pMemoryManager,
                            NULL,
                            hClient,
                            hParent,
                            hVASpace,
                            heapOwner,
                            pMemDesc);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "VirtualMemory memmgrFree failed, client: %x, hVASpace: %x, gpu: %x\n",
                      RES_GET_CLIENT_HANDLE(pVirtualMemory),
                      hVASpace,
                      pGpu->gpuInstance);
        }
    }
}

NV_STATUS
virtmemAllocResources
(
    OBJGPU                      *pGpu,
    MemoryManager               *pMemoryManager,
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest,
    FB_ALLOC_INFO               *pFbAllocInfo,
    RmClient                    *pFbAllocInfoClient
)
{
    NV_STATUS                    status          = NV_OK;
    MEMORY_DESCRIPTOR           *pMemDesc        = NULL;
    RsClient                    *pRsClient       = NULL;
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc   = pAllocRequest->pUserParams;
    NvHandle                     hVASpace        = pVidHeapAlloc->hVASpace;
    NvBool                       bAllocedMemDesc = NV_FALSE;
    NvBool                       bBar1VA         = NV_FALSE;
    NvBool                       bFlaVA          = NV_FALSE;

    NV_ASSERT(!(pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_WPR1) && !(pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_WPR2));
    NV_ASSERT_OR_RETURN(pFbAllocInfoClient != NULL, NV_ERR_INVALID_CLIENT);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, memUtilsAllocMemDesc(pGpu, pAllocRequest, pFbAllocInfo, &pMemDesc, NULL,
                                                                  ADDR_VIRTUAL, NV_TRUE, &bAllocedMemDesc), failed);

    // Only a kernel client can request for a protected allocation
    if (pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RS_PRIV_LEVEL privLevel;

        //
        // This fn has usescases where call context is unavailable.
        // In those cases, fall back to cached privileges.
        //
        if (pCallContext == NULL)
        {
            privLevel = rmclientGetCachedPrivilege(pFbAllocInfoClient);
        }
        else
        {
            privLevel = pCallContext->secInfo.privLevel;
        }

        if (
            (privLevel >= RS_PRIV_LEVEL_KERNEL))
        {
            pFbAllocInfo->bIsKernelAlloc = NV_TRUE;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "NV_ERR_INSUFFICIENT_PERMISSIONS\n");
            status = NV_ERR_INSUFFICIENT_PERMISSIONS;
            goto failed;
        }
    }

    // Allocate a virtual surface
    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
        pFbAllocInfo->offset = pVidHeapAlloc->offset - pFbAllocInfo->alignPad;

    //
    // pFbAllocInfo->hClient=0 is sometimes passed and not always needed,
    // do not immediately fail if this call, only if the client needs to be used.
    //
    pRsClient = staticCast(pFbAllocInfoClient, RsClient);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    // In case of SR-IOV, the VAS is managed by the guest. So, no need
    // to communicate with the host for VA allocation.
    //
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        OBJVASPACE *pVAS = NULL;

        // Only try this if GetClient succeeded, else pass through the status from its fail.
        if (pRsClient != NULL)
            status = vaspaceGetByHandleOrDeviceDefault(pRsClient, pFbAllocInfo->hDevice, hVASpace, &pVAS);
        if (NV_OK != status)
            goto failed;

        bBar1VA = !!(vaspaceGetFlags(pVAS) & VASPACE_FLAGS_BAR_BAR1);
        bFlaVA = !!(vaspaceGetFlags(pVAS) & VASPACE_FLAGS_FLA);
    }

    // For Virtual FLA allocations, we don't have to RPC
    if ((!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu)) ||
        bBar1VA || bFlaVA ||
        gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        OBJVASPACE     *pVAS  = NULL;
        OBJGVASPACE    *pGVAS = NULL;
        NvU64           align = pFbAllocInfo->align + 1;
        VAS_ALLOC_FLAGS flags = {0};
        NvU64           pageSizeLockMask = 0;
        pFbAllocInfo->internalflags = pAllocRequest->internalflags;

        // Only try this if GetClient succeeded, else pass through the status from its fail.
        if (pRsClient != NULL)
            status = vaspaceGetByHandleOrDeviceDefault(pRsClient, pFbAllocInfo->hDevice, hVASpace, &pVAS);
        if (NV_OK != status)
            goto failed;

        //
        // Feature requested for RM unlinked SLI:
        // Clients can pass an allocation flag to the device or VA space constructor
        // so that mappings and allocations will fail without an explicit address.
        //
        pGVAS = dynamicCast(pVAS, OBJGVASPACE);
        if (pGVAS != NULL)
        {
            if ((pGVAS->flags & VASPACE_FLAGS_REQUIRE_FIXED_OFFSET) &&
                !(pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE))
            {
                status = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_ERROR, "The VA space requires all allocations to specify a fixed address\n");
                goto failed;
            }
        }

        status = vaspaceFillAllocParams(pVAS, pFbAllocInfo,
                                        &pFbAllocInfo->size, &align,
                                        &pVidHeapAlloc->rangeLo, &pVidHeapAlloc->rangeHi,
                                        &pageSizeLockMask, &flags);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR, "FillAllocParams failed.\n");
            DBG_BREAKPOINT();
        }
        else
        {
            status = vaspaceAlloc(pVAS, pFbAllocInfo->size, align,
                                  pVidHeapAlloc->rangeLo, pVidHeapAlloc->rangeHi,
                                  pageSizeLockMask, flags, &pFbAllocInfo->offset);
            if (NV_OK != status)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "VA Space alloc failed! Status Code: 0x%x Size: 0x%llx RangeLo: 0x%llx,"
                          " RangeHi: 0x%llx, pageSzLockMask: 0x%llx\n",
                          status, pFbAllocInfo->size,
                          pVidHeapAlloc->rangeLo, pVidHeapAlloc->rangeHi,
                          pageSizeLockMask);
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto failed;
            }

            memdescDescribe(pMemDesc, ADDR_VIRTUAL,
                            pFbAllocInfo->offset,
                            pFbAllocInfo->size);

            // Return alignment info.
            pFbAllocInfo->align        = align - 1;
            pVidHeapAlloc->alignment   = align;
        }
    }
    else
    {
        // Possibly dead code: IS_VIRTUAL and bSplitVAs are only enabled on legacy vGPU.
        memdescDescribe(pMemDesc, ADDR_VIRTUAL, memdescGetPte(pMemDesc, AT_GPU, 0),
            pMemDesc->Size);
    }

    //
    // Report default (any) page size for virtual allocations with no page size restriction.
    // Actual page size will be determined at map time.
    //
    if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT, pFbAllocInfo->pageFormat->attr))
    {
        pFbAllocInfo->retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT, pFbAllocInfo->retAttr);
    }

    // get possibly updated surface attributes
    pVidHeapAlloc->attr = pFbAllocInfo->retAttr;
    pVidHeapAlloc->attr2 = pFbAllocInfo->retAttr2;

    // update contiguity attribute to reflect memdesc
    if (memdescGetContiguity(pAllocRequest->pMemDesc, AT_GPU))
    {
        pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                          _CONTIGUOUS,
                                          pVidHeapAlloc->attr);
    }
    else
    {
        pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                          _NONCONTIGUOUS,
                                          pVidHeapAlloc->attr);
    }

    pVidHeapAlloc->offset = pFbAllocInfo->offset;

    if (pAllocRequest->pHwResource != NULL)
    {
        pAllocRequest->pHwResource->attr       = pFbAllocInfo->retAttr;
        pAllocRequest->pHwResource->attr2      = pFbAllocInfo->retAttr2;
        pAllocRequest->pHwResource->hwResId    = pFbAllocInfo->hwResId;
        pAllocRequest->pHwResource->comprCovg  = pFbAllocInfo->comprCovg;
        pAllocRequest->pHwResource->ctagOffset = pFbAllocInfo->ctagOffset;
        pAllocRequest->pHwResource->hwResId    = pFbAllocInfo->hwResId;
    }

    return NV_OK;

failed:
    memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);

    if (bAllocedMemDesc)
    {
        memdescDestroy(pAllocRequest->pMemDesc);
        pAllocRequest->pMemDesc = NULL;
    }

    return status;
}

/*
 * @brief Interface to vaspaceReserveMempool to reserve PMA memory for page tables
 */
NV_STATUS virtmemReserveMempool_IMPL
(
    VirtualMemory *pVirtualMemory,
    OBJGPU        *pGpu,
    Device        *pDevice,
    NvU64          size,
    NvU64          pageSizeMask
)
{
    RsClient   *pClient = RES_GET_CLIENT(pVirtualMemory);
    OBJVASPACE *pVAS    = NULL;
    NvU32       mempoolFlags = VASPACE_RESERVE_FLAGS_NONE;

    //
    // Reject mappings for a legacy NV01_MEMORY_SYSTEM_DYNAMIC
    // object silently.
    //
    if (pVirtualMemory->hVASpace == NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    if (pVirtualMemory->bOptimizePageTableMempoolUsage)
    {
        mempoolFlags = VASPACE_RESERVE_FLAGS_ALLOC_UPTO_TARGET_LEVEL_ONLY;
    }

    NV_ASSERT_OK_OR_RETURN(
        vaspaceGetByHandleOrDeviceDefault(pClient, RES_GET_HANDLE(pDevice),
                                          pVirtualMemory->hVASpace, &pVAS));

    return vaspaceReserveMempool(pVAS, pGpu, pDevice,
                                 size, pageSizeMask, mempoolFlags);
}

/*!
 * @brief Does this VirtualMemory object use the specified hVASpace?
 */
NvBool
virtmemMatchesVASpace_IMPL
(
    VirtualMemory *pVirtualMemory,
    NvHandle hClient,
    NvHandle hVASpace
)
{
    return (RES_GET_CLIENT_HANDLE(pVirtualMemory) == hClient) && (pVirtualMemory->hVASpace == hVASpace);
}

/*!
 * @brief Helper to look up a VirtualMemory object
 */
NV_STATUS
virtmemGetByHandleAndDevice_IMPL
(
    RsClient          *pClient,
    NvHandle           hMemory,
    NvHandle           hDevice,
    VirtualMemory    **ppVirtualMemory
)
{
    Memory *pMemory;

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        memGetByHandleAndDevice(pClient, hMemory, hDevice, &pMemory));

    *ppVirtualMemory = dynamicCast(pMemory, VirtualMemory);

    return (*ppVirtualMemory != NULL) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

/*!
 * @brief Create a CPU mapping in addition to the DMA mapping
 */
static NV_STATUS
_virtmemAllocKernelMapping
(
    OBJGPU               *pGpu,
    OBJVASPACE           *pVAS,
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo,
    NvU64                 offset,
    NvU64                 size,
    Memory               *pMemoryInfo
)
{
    NV_STATUS  status              = NV_OK;
    NvBool     bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);
    NvU32      gpuSubDevInst;
    RmPhysAddr bar1PhysAddr;

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)

    gpuSubDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    if (bCoherentCpuMapping)
    {
        // Use a temp pointer to prevent overwriting the previous pointer by accident
        MEMORY_DESCRIPTOR *pMemDesc      = memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu);
        KernelBus         *pKernelBus    = GPU_GET_KERNEL_BUS(pGpu);

        NV_PRINTF(LEVEL_INFO,
                  "Allocating coherent link mapping. length=%lld, memDesc->size=%lld\n",
                  size, pDmaMappingInfo->pMemDesc->Size);

        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));

        status = kbusMapCoherentCpuMapping_HAL(pGpu, pKernelBus, pMemDesc, offset, size,
                                               NV_PROTECT_READ_WRITE,
                                               &pDmaMappingInfo->KernelVAddr[gpuSubDevInst],
                                               &pDmaMappingInfo->KernelPriv);
    }
    else
    {
        //
        // Allocate GPU virtual address space for the video memory region
        // for those GPUs that support it.
        //
        pDmaMappingInfo->FbApertureLen[gpuSubDevInst] = pDmaMappingInfo->pMemDesc->Size;
        if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            status = osMapSystemMemory(pMemoryInfo->pMemDesc,
                                       offset,
                                       pDmaMappingInfo->pMemDesc->Size,
                                       NV_TRUE /*Kernel*/,
                                       NV_PROTECT_READ_WRITE,
                                       (NvP64 *) &pDmaMappingInfo->KernelVAddr[gpuSubDevInst],
                                       (NvP64 *) &pDmaMappingInfo->KernelPriv);

            if (status != NV_OK)
            {
                pDmaMappingInfo->FbApertureLen[gpuSubDevInst] = 0;
                pDmaMappingInfo->FbAperture[gpuSubDevInst]    = 0;
                pDmaMappingInfo->KernelPriv                   = 0;
                SLI_LOOP_BREAK;
            }
        }
        else
        {
            KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            Device *pDevice = NULL;
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            if ((pCallContext != NULL) && (pCallContext->pClient != NULL))
            {
                RsResourceRef *pDeviceRef = NULL;

                status = refFindAncestorOfType(pCallContext->pResourceRef,
                                               classId(Device), &pDeviceRef);
                if (status == NV_OK)
                {
                    pDevice = dynamicCast(pDeviceRef->pResource, Device);
                }
            }

            status = kbusMapFbApertureSingle(pGpu, pKernelBus,
                                             pMemoryInfo->pMemDesc, offset,
                                             &pDmaMappingInfo->FbAperture[gpuSubDevInst],
                                             &pDmaMappingInfo->FbApertureLen[gpuSubDevInst],
                                             BUS_MAP_FB_FLAGS_MAP_UNICAST, pDevice);

            if (status != NV_OK)
            {
                pDmaMappingInfo->FbApertureLen[gpuSubDevInst] = 0;
                pDmaMappingInfo->FbAperture[gpuSubDevInst]    = 0;
                SLI_LOOP_BREAK;
            }

            bar1PhysAddr = gpumgrGetGpuPhysFbAddr(pGpu) + pDmaMappingInfo->FbAperture[gpuSubDevInst];
            status = osMapPciMemoryKernelOld(pGpu, bar1PhysAddr,
                                             pDmaMappingInfo->pMemDesc->Size,
                                             NV_PROTECT_READ_WRITE,
                                             &pDmaMappingInfo->KernelVAddr[gpuSubDevInst],
                                             NV_MEMORY_WRITECOMBINED);
        }
    }

    if (status != NV_OK)
    {
        SLI_LOOP_BREAK;
    }

    SLI_LOOP_END

    if (status != NV_OK)
    {
        _virtmemFreeKernelMapping(pGpu, pDmaMappingInfo);
    }

    return status;
}
/*!
 * @brief Free CPU mapping
 */
static void
_virtmemFreeKernelMapping
(
    OBJGPU               *pGpu,
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo
)
{
    NvU32    gpuSubDevInst;
    NvBool   bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    gpuSubDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    // Unmap a kernel CPU mapping if one exists
    if (pDmaMappingInfo->KernelVAddr[gpuSubDevInst] != NULL)
    {
        if (bCoherentCpuMapping)
        {
            KernelBus         *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            MEMORY_DESCRIPTOR *pMemDesc   = memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu);
            kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus, pMemDesc, pDmaMappingInfo->KernelVAddr[gpuSubDevInst],
                                            pDmaMappingInfo->KernelPriv);
        }
        else
        {
            osUnmapPciMemoryKernelOld(pGpu, pDmaMappingInfo->KernelVAddr[gpuSubDevInst]);
        }

        pDmaMappingInfo->KernelVAddr[gpuSubDevInst] = NULL;
    }

    // Unmap the FB aperture mapping if one exists
    if ((pDmaMappingInfo->FbApertureLen[gpuSubDevInst]) && (!bCoherentCpuMapping))
    {
        if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            // This is a no-op in GSP, but document it here as code in case it changes.
            osUnmapSystemMemory(pDmaMappingInfo->pMemDesc,
                                NV_TRUE /*Kernel*/,
                                0 /*ProcessId*/,
                                (NvP64)pDmaMappingInfo->FbAperture[gpuSubDevInst],
                                NV_PTR_TO_NvP64(pDmaMappingInfo->KernelPriv));
        }
        else
        {
            KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            kbusUnmapFbApertureSingle(pGpu,
                                      pKernelBus,
                                      pDmaMappingInfo->pMemDesc,
                                      pDmaMappingInfo->FbAperture[gpuSubDevInst],
                                      pDmaMappingInfo->FbApertureLen[gpuSubDevInst],
                                      BUS_MAP_FB_FLAGS_MAP_UNICAST);
        }
        pDmaMappingInfo->FbAperture[gpuSubDevInst] = 0;
        pDmaMappingInfo->FbApertureLen[gpuSubDevInst] = 0;
        pDmaMappingInfo->KernelPriv = 0;
    }

    SLI_LOOP_END
}

/*!
 * @brief Map an object into a VirtualMemory object
 */
NV_STATUS
virtmemMapTo_IMPL
(
    VirtualMemory *pVirtualMemory,
    RS_RES_MAP_TO_PARAMS *pParams
)
{
    NV_STATUS   status                = NV_ERR_NOT_SUPPORTED;
    Memory         *pMemory           = staticCast(pVirtualMemory, Memory);
    OBJGPU         *pGpu              = pParams->pGpu;
    OBJGPU         *pSrcGpu           = pParams->pSrcGpu;
    RsClient       *pClient           = RES_GET_CLIENT(pVirtualMemory);
    MemoryManager  *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    RsResourceRef  *pMemoryRef        = pParams->pMemoryRef;
    NvHandle        hClient           = pClient->hClient;
    NvHandle        hBroadcastDevice  = pParams->hBroadcastDevice;
    NvHandle        hVirtualMem       = RES_GET_HANDLE(pVirtualMemory);
    NvHandle        hMemoryDevice     = pParams->hMemoryDevice;
    NvU32           gpuMask           = pParams->gpuMask;
    NvU64           offset            = pParams->offset;    // offset into pMemoryRef to map
    NvU64           length            = pParams->length;
    NvU32           flags             = pParams->flags;
    NvU32           p2p               = DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, pParams->flags);

    VirtMemAllocator     *pDma                  = GPU_GET_DMA(pGpu);
    MEMORY_DESCRIPTOR    *pSrcMemDesc           = pParams->pSrcMemDesc;
    NvU64                *pDmaOffset            = pParams->pDmaOffset;  // return VirtualMemory offset
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo       = NULL;
    OBJVASPACE           *pVas                  = NULL;
    Memory               *pSrcMemory            = dynamicCast(pMemoryRef->pResource, Memory);

    NvU32       tgtAddressSpace   = ADDR_UNKNOWN;
    NvBool      bDmaMappingRegistered = NV_FALSE;
    NvBool      bFlaMapping           = pParams->bFlaMapping;
    NvBool      bIsIndirectPeer       = NV_FALSE;
    NvBool      bEncrypted;
    NvBool      bIsSysmem             = NV_FALSE;
    NvBool      bBar1P2P              = (p2p && kbusHasPcieBar1P2PMapping_HAL(pGpu,
                                                                              GPU_GET_KERNEL_BUS(pGpu),
                                                                              pSrcGpu,
                                                                              GPU_GET_KERNEL_BUS(pSrcGpu)));
    NvBool      bKernelMappingRequired = FLD_TEST_DRF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE, flags);

    //
    // Allow unicast on NV01_MEMORY_VIRTUAL object, but maintain the broadcast
    // requirement for NV50_MEMORY_VIRTUAL.
    //
    if (pParams->bSubdeviceHandleProvided && !pVirtualMemory->bAllowUnicastMapping)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast mappings into virtual memory object not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient, hBroadcastDevice, pVirtualMemory->hVASpace, &pVas);
    if (status != NV_OK)
        return status;

    //
    // Use the encryption setting of the virtual allocation.
    // This makes sense, since the same physical mem descriptor could have
    // more than one mapping, each with different encryption settings.
    //
    bEncrypted = memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_ENCRYPTED);

    // Validate the offset and limit passed in.
    if (offset + length > pSrcMemDesc->Size)
        return NV_ERR_INVALID_BASE;

    status = intermapCreateDmaMapping(pClient, pVirtualMemory, &pDmaMappingInfo, flags);
    if (status != NV_OK)
        return status;

    if (bBar1P2P)
    {
        DMA_BAR1P2P_MAPPING_PRARAMS params = {0};

        params.pVas = pVas;
        params.pPeerGpu = pSrcGpu;
        params.pPeerMemDesc = pSrcMemDesc;
        params.flags = flags;
        params.offset = offset;
        params.length = length;
        params.pDmaMappingInfo = pDmaMappingInfo;

        status = dmaAllocBar1P2PMapping_HAL(pGpu, pDma, &params);
        if (status != NV_OK)
            goto done;

        // Adjust local variables for the BAR1 P2P mappings
        pSrcMemDesc = params.pMemDescOut;
        flags = params.flagsOut;
        offset = params.offsetOut;
    }

    //
    // Determine target address space.  If we're mapping fbmem from
    // one gpu for use by another, then we need to treat that memory as
    // ADDR_SYSMEM.
    //
    tgtAddressSpace = memdescGetAddressSpace(memdescGetMemDescFromGpu(pSrcMemDesc, pGpu));
    if ((pSrcGpu != pGpu) && (tgtAddressSpace == ADDR_FBMEM))
    {
        tgtAddressSpace = ADDR_SYSMEM;

        if (gpumgrCheckIndirectPeer(pGpu, pSrcGpu))
            bIsIndirectPeer = NV_TRUE;
    }

    // Different cases for vidmem & system memory/fabric memory.
    bIsSysmem = (tgtAddressSpace == ADDR_SYSMEM) || (tgtAddressSpace == ADDR_EGM);

    //
    // Create a MEMORY_DESCRIPTOR describing this region of the memory
    // alloc in question
    //
    status = memdescCreateSubMem(&pDmaMappingInfo->pMemDesc, pSrcMemDesc, pGpu, offset, length);
    if (status != NV_OK)
        goto done;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    memdescSetFlag(memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu),
               MEMDESC_FLAGS_ENCRYPTED,
               bEncrypted);
    SLI_LOOP_END

    if (FLD_TEST_DRF(OS46, _FLAGS, _ENABLE_FORCE_COMPRESSED_MAP, _TRUE, flags) &&
        pSrcMemDesc->pGpu != NULL && memmgrIsKindCompressible(pMemoryManager, memdescGetPteKind(pSrcMemDesc)))
    {
        // Only makes sense for compressed allocations
        memdescSetFlag(pSrcMemDesc, MEMDESC_FLAGS_MAP_FORCE_COMPRESSED_MAP, NV_TRUE);
    }

    if (FLD_TEST_DRF(OS46, _FLAGS, _PAGE_KIND, _VIRTUAL, flags))
    {
        NvU32 kind = memdescGetPteKind(pMemory->pMemDesc);

        NV_ASSERT(memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_SET_KIND));

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY);
        if (memdescGetFlag(pSrcMemDesc, MEMDESC_FLAGS_MAP_FORCE_COMPRESSED_MAP) &&
            !memmgrIsKindCompressible(pMemoryManager, kind))
        {
            NvBool bDisablePlc =
                memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, memdescGetPteKind(pSrcMemDesc));

            // don't just override kind with physical allocation kind, as it can have different swizzling
            kind = memmgrGetCompressedKind_HAL(pMemoryManager, kind, bDisablePlc);
        }
        if (tgtAddressSpace == ADDR_SYSMEM && !memmgrComprSupported(pMemoryManager, ADDR_SYSMEM))
        {
            //
            // If system memory does not support compression, the virtual kind is compressible,
            // and being mapped into system memory fallback to using the uncompressed kind.
            //
            kind = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager, kind, 0);
        }
        memdescSetPteKind(memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu), kind);
        SLI_LOOP_END;
    }

    if (bIsSysmem ||
        (tgtAddressSpace == ADDR_FABRIC_MC) ||
        (tgtAddressSpace == ADDR_FABRIC_V2))
    {
        // if GPUs are indirect peers, create TCE mappings
        if (bIsIndirectPeer)
        {
            //
            // TODO: Ideally memdescMapIommu should be called on FB memdesc with
            // pSrcGpu That would clearly convey that memory is owned by pSrcGpu and
            // we are trying to create IOMMU mappings for pGpu. This effort is being
            // tracked in bug 2043603
            //
            status = memdescMapIommu(pDmaMappingInfo->pMemDesc, pGpu->busInfo.iovaspaceId);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "DMA map pages failed for requested GPU!\n");
                goto done;
            }
        }

        // Monolithic CPU RM or SPLIT_VAS_MGMT
        if (!pMemory->bRpcAlloc || gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
        {
            pDmaMappingInfo->DmaOffset = *pDmaOffset; // in case this is 'in'

            // allocate mapping in VirtualMemory object
            status = dmaAllocMap(pGpu, pDma, pVas, pVirtualMemory, pSrcMemory, pDmaMappingInfo);
            if (status != NV_OK)
                goto done;

            status = intermapRegisterDmaMapping(pClient, pVirtualMemory, pDmaMappingInfo, pDmaMappingInfo->DmaOffset, gpuMask);
            if (status != NV_OK)
            {
                dmaFreeMap(pGpu, pDma, pVas,
                           pVirtualMemory, pDmaMappingInfo,
                           DRF_DEF(OS47, _FLAGS, _DEFER_TLB_INVALIDATION, _FALSE));
                goto done;
            }

            bDmaMappingRegistered = NV_TRUE;

            // If a kernel mapping has been requested, create one
            if (bKernelMappingRequired)
            {
                status = memdescMapOld(pDmaMappingInfo->pMemDesc,
                                       0,
                                       pDmaMappingInfo->pMemDesc->Size,
                                       NV_TRUE, NV_PROTECT_READ_WRITE,
                                       &pDmaMappingInfo->KernelVAddr[gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu))],
                                       &pDmaMappingInfo->KernelPriv);

                if (status != NV_OK)
                    goto done;
            }

            *pDmaOffset = pDmaMappingInfo->DmaOffset;
        } // !IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu)
    }
    else if (tgtAddressSpace == ADDR_FBMEM)
    {
        pDmaMappingInfo->DmaOffset = *pDmaOffset; // in case this is 'in'

        // Monolithic CPU RM or SPLIT_VAS_MGMT
        if (!pMemory->bRpcAlloc || gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
        {
            // allocate mapping in VirtualMemory object
            status = dmaAllocMap(pGpu, pDma, pVas, pVirtualMemory, pSrcMemory, pDmaMappingInfo);
            if (status != NV_OK)
                goto done;

            *pDmaOffset = pDmaMappingInfo->DmaOffset;

            status = intermapRegisterDmaMapping(pClient, pVirtualMemory, pDmaMappingInfo, pDmaMappingInfo->DmaOffset, gpuMask);
            if (status != NV_OK)
            {
                dmaFreeMap(pGpu, pDma, pVas,
                           pVirtualMemory, pDmaMappingInfo,
                           DRF_DEF(OS47, _FLAGS, _DEFER_TLB_INVALIDATION, _FALSE));
                goto done;
            }

            bDmaMappingRegistered = NV_TRUE;

            if (bKernelMappingRequired)
            {
                status = _virtmemAllocKernelMapping(pGpu, pVas, pDmaMappingInfo, offset, length, pSrcMemory);
                if (status != NV_OK)
                    goto done;
            }

            *pDmaOffset = pDmaMappingInfo->DmaOffset;
        } // if (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
    }
    else
    {
        // unknown (or mixed vidmem+sysmem?) mem case
        status = NV_ERR_INVALID_OBJECT_HANDLE;
        goto done;
    }

    if (RMCFG_CLASS_NV50_P2P &&
        !bFlaMapping &&
        (bBar1P2P || DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, pDmaMappingInfo->Flags) == NVOS46_FLAGS_P2P_ENABLE_NOSLI))
    {
        //
        // if we are on SLI and trying to map peer memory between two GPUs
        // on the same device, we don't rely on dynamic p2p mailbox setup.
        // SLI uses static p2p mailbox and hence will not have any
        // P2P object associated with it
        //
        if ((hBroadcastDevice == hMemoryDevice) && IsSLIEnabled(pGpu))
        {
            goto vgpu_send_rpc;
        }

        pDmaMappingInfo->bP2P = NV_TRUE;
    }

vgpu_send_rpc:

    if (pMemory->bRpcAlloc)
    {
        NV_RM_RPC_MAP_MEMORY_DMA(pGpu, hClient, hBroadcastDevice, hVirtualMem, pMemoryRef->hResource,
                                 offset, length, flags, pDmaOffset, status);
        if (status != NV_OK)
            goto done;

        if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
            !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
        {
            //
            // vGPU doesn't understand subdevice handles.  But clients map memory
            // with subdevice handles and we don't want that to fail on vGPU.
            // Currently, we just pass down the broadcast device handle to the host
            // (which should be equivalent if SLI is disabled).  This will need to
            // be revisited if vGPU ever supports SLI.
            //
            NV_ASSERT(!IsSLIEnabled(pGpu));

            pDmaMappingInfo->DmaOffset = *pDmaOffset;

            status = intermapRegisterDmaMapping(pClient, pVirtualMemory, pDmaMappingInfo,
                                                pDmaMappingInfo->DmaOffset, gpuMask);
            if (status != NV_OK)
                goto done;

            bDmaMappingRegistered = NV_TRUE;

            if (tgtAddressSpace == ADDR_SYSMEM)
            {
                // If a kernel mapping has been requested, create one
                if (bKernelMappingRequired)
                {
                    status = memdescMapOld(pDmaMappingInfo->pMemDesc,
                                           0,
                                           pDmaMappingInfo->pMemDesc->Size,
                                           NV_TRUE, NV_PROTECT_READ_WRITE,
                                           &pDmaMappingInfo->KernelVAddr[gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu))],
                                           &pDmaMappingInfo->KernelPriv);
                    if (status != NV_OK)
                        goto done;
                }
            }
        }
    }

done:
    if (status != NV_OK)
    {
        if (pDmaMappingInfo != NULL)
        {
            if ((pDmaMappingInfo->pMemDesc != NULL) && bKernelMappingRequired)
            {
                //
                // if Kernel cookie exists and mapping is in sysmem, free sysmem mapping
                // for ADDR_FBMEM function determines whether mapping was created itself
                //
                if ((pDmaMappingInfo->KernelPriv != NULL) &&
                    (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_SYSMEM))
                {
                        memdescUnmapOld(pDmaMappingInfo->pMemDesc, NV_TRUE, 0,
                                        pDmaMappingInfo->KernelVAddr[gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu))],
                                        pDmaMappingInfo->KernelPriv);
                        pDmaMappingInfo->KernelPriv = NULL;
                }
                else if (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_FBMEM)
                {
                    _virtmemFreeKernelMapping(pGpu, pDmaMappingInfo);
                }
            }

            if (pDmaMappingInfo->pMemDesc != NULL && bIsIndirectPeer)
            {
                memdescUnmapIommu(pDmaMappingInfo->pMemDesc, pGpu->busInfo.iovaspaceId);
            }

            dmaFreeBar1P2PMapping_HAL(pDma, pDmaMappingInfo);

            memdescDestroy(pDmaMappingInfo->pMemDesc);
            pDmaMappingInfo->pMemDesc = NULL;

            if (bDmaMappingRegistered)
            {
                NV_ASSERT_OK(intermapDelDmaMapping(pClient, pVirtualMemory, *pDmaOffset, gpuMask));
            }
            else
            {
                // Explicitly free the DMA mapping if mapping was not yet registered
                intermapFreeDmaMapping(pDmaMappingInfo);
            }
        }
    }

    return status;
}

/*!
 * @brief Unmap object from VirtualMemory object
 */
NV_STATUS
virtmemUnmapFrom_IMPL
(
    VirtualMemory *pVirtualMemory,
    RS_RES_UNMAP_FROM_PARAMS *pParams
)
{
    OBJGPU     *pGpu              = pParams->pGpu;
    Memory     *pMemory           = staticCast(pVirtualMemory, Memory);
    RsClient   *pClient           = RES_GET_CLIENT(pVirtualMemory);
    NvHandle    hClient           = pClient->hClient;
    NvHandle    hMemory           = pParams->hMemory;
    NvHandle    hVirtualMem       = RES_GET_HANDLE(pVirtualMemory);
    NvHandle    hBroadcastDevice  = pParams->hBroadcastDevice;
    NvU32       gpuMask           = pParams->gpuMask;
    NvU64       dmaOffset         = pParams->dmaOffset;
    OBJVASPACE *pVas              = NULL;
    NV_STATUS   status            = NV_OK;
    NvBool      bIsIndirectPeer   = NV_FALSE;
    CLI_DMA_MAPPING_INFO *pDmaMappingInfoLeft = NULL;
    NvBool                bDmaMappingInfoLeftRegistered = NV_FALSE;
    CLI_DMA_MAPPING_INFO *pDmaMappingInfoRight = NULL;
    NvBool                bDmaMappingInfoRightRegistered = NV_FALSE;
    CLI_DMA_MAPPING_INFO *pDmaMappingInfoUnmap = NULL;

    CLI_DMA_MAPPING_INFO *pDmaMappingInfo   = NULL;

    if (hMemory != NV01_NULL_OBJECT)
    {
        RsResourceRef  *pSrcMemoryRef;
        Memory         *pMemorySrc;

        if (clientGetResourceRef(pClient, hMemory, &pSrcMemoryRef) != NV_OK)
            return NV_ERR_OBJECT_NOT_FOUND;

        status = rmresCheckMemInterUnmap(dynamicCast(pSrcMemoryRef->pResource, RmResource), pParams->bSubdeviceHandleProvided);

        // Exit if failed or invalid class, otherwise continue on to next part
        if (status != NV_OK)
            return status;

        pMemorySrc = dynamicCast(pSrcMemoryRef->pResource, Memory);
        if (pMemorySrc != NULL)
        {
            if (gpumgrCheckIndirectPeer(pMemorySrc->pGpu, pGpu))
                bIsIndirectPeer = NV_TRUE;
        }
    }

    if (pParams->bSubdeviceHandleProvided && !pVirtualMemory->bAllowUnicastMapping)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast DMA mappings into virtual memory object not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient, hBroadcastDevice, pVirtualMemory->hVASpace, &pVas);
    if (status != NV_OK)
        return status;

    // Get DMA mapping info.
    pDmaMappingInfo = intermapGetDmaMapping(pVirtualMemory, dmaOffset, gpuMask);
    NV_ASSERT_OR_RETURN(pDmaMappingInfo != NULL, NV_ERR_INVALID_OBJECT_HANDLE);
    NvBool bPartialUnmap = dmaOffset != pDmaMappingInfo->DmaOffset || pParams->size != pDmaMappingInfo->pMemDesc->Size;
    NV_ASSERT_OR_RETURN(!bPartialUnmap || (gpuMask & (gpuMask - 1)) == 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!bPartialUnmap || !bIsIndirectPeer, NV_ERR_INVALID_ARGUMENT);

    if (FLD_TEST_DRF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE, pDmaMappingInfo->Flags))
    {
        NV_ASSERT_OR_RETURN(!bPartialUnmap, NV_ERR_INVALID_ARGUMENT);

        //
        // if Kernel cookie exists and mapping is in sysmem, free sysmem mapping
        // for ADDR_FBMEM function determines whether mapping was created itself
        //
        if ((pDmaMappingInfo->KernelPriv != NULL) &&
            (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_SYSMEM))
        {
            memdescUnmapOld(pDmaMappingInfo->pMemDesc, NV_TRUE, 0,
                            pDmaMappingInfo->KernelVAddr[gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu))],
                            pDmaMappingInfo->KernelPriv);
            pDmaMappingInfo->KernelPriv = NULL;
        }
        else if (memdescGetAddressSpace(memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu)) == ADDR_FBMEM)
        {
            _virtmemFreeKernelMapping(pGpu, pDmaMappingInfo);
        }
    }

    // if this was peer mapped context dma, remove it from P2P object
    if (RMCFG_CLASS_NV50_P2P && pDmaMappingInfo->bP2P)
    {
        NV_ASSERT_OR_RETURN(!bPartialUnmap, NV_ERR_INVALID_ARGUMENT);
        dmaFreeBar1P2PMapping_HAL(GPU_GET_DMA(pGpu), pDmaMappingInfo);
    }

    if (dmaOffset > pDmaMappingInfo->DmaOffset)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            intermapCreateDmaMapping(pClient, pVirtualMemory, &pDmaMappingInfoLeft, pDmaMappingInfo->Flags),
            failed);

        pDmaMappingInfoLeft->DmaOffset          = pDmaMappingInfo->DmaOffset;
        pDmaMappingInfoLeft->bP2P               = pDmaMappingInfo->bP2P;
        pDmaMappingInfoLeft->addressTranslation = pDmaMappingInfo->addressTranslation;
        pDmaMappingInfoLeft->mapPageSize        = pDmaMappingInfo->mapPageSize;

        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreateSubMem(&pDmaMappingInfoLeft->pMemDesc, pDmaMappingInfo->pMemDesc, pGpu,
                                pDmaMappingInfoLeft->DmaOffset - pDmaMappingInfo->DmaOffset,
                                dmaOffset - pDmaMappingInfoLeft->DmaOffset),
            failed);
    }

    if (dmaOffset + pParams->size < pDmaMappingInfo->DmaOffset + pDmaMappingInfo->pMemDesc->Size)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            intermapCreateDmaMapping(pClient, pVirtualMemory, &pDmaMappingInfoRight, pDmaMappingInfo->Flags),
            failed);

        pDmaMappingInfoRight->DmaOffset          = dmaOffset + pParams->size;
        pDmaMappingInfoRight->bP2P               = pDmaMappingInfo->bP2P;
        pDmaMappingInfoRight->addressTranslation = pDmaMappingInfo->addressTranslation;
        pDmaMappingInfoRight->mapPageSize        = pDmaMappingInfo->mapPageSize;

        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreateSubMem(&pDmaMappingInfoRight->pMemDesc, pDmaMappingInfo->pMemDesc, pGpu,
                pDmaMappingInfoRight->DmaOffset - pDmaMappingInfo->DmaOffset,
                pDmaMappingInfo->DmaOffset + pDmaMappingInfo->pMemDesc->Size - pDmaMappingInfoRight->DmaOffset),
            failed);
    }

    pDmaMappingInfoUnmap = pDmaMappingInfo;
    if (pDmaMappingInfoLeft != NULL || pDmaMappingInfoRight != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            intermapCreateDmaMapping(pClient, pVirtualMemory, &pDmaMappingInfoUnmap, pDmaMappingInfo->Flags),
            failed);

        pDmaMappingInfoUnmap->DmaOffset          = dmaOffset;
        pDmaMappingInfoUnmap->bP2P               = pDmaMappingInfo->bP2P;
        pDmaMappingInfoUnmap->addressTranslation = pDmaMappingInfo->addressTranslation;
        pDmaMappingInfoUnmap->mapPageSize        = pDmaMappingInfo->mapPageSize;
        pDmaMappingInfoUnmap->gpuMask            = pDmaMappingInfo->gpuMask;

        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreateSubMem(&pDmaMappingInfoUnmap->pMemDesc, pDmaMappingInfo->pMemDesc, pGpu,
                                pDmaMappingInfoUnmap->DmaOffset - pDmaMappingInfo->DmaOffset,
                                pParams->size),
            failed);
    }

    if (!pMemory->bRpcAlloc || gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        // free mapping in context dma
        dmaFreeMap(pGpu, GPU_GET_DMA(pGpu), pVas, pVirtualMemory, pDmaMappingInfoUnmap, pParams->flags);

        if ((memdescGetAddressSpace(memdescGetMemDescFromGpu(pDmaMappingInfo->pMemDesc, pGpu)) == ADDR_FBMEM) &&
             bIsIndirectPeer)
        {
            memdescUnmapIommu(pDmaMappingInfo->pMemDesc, pGpu->busInfo.iovaspaceId);
        }
    }

    // free memory descriptor
    memdescFree(pDmaMappingInfo->pMemDesc);
    memdescDestroy(pDmaMappingInfo->pMemDesc);
    pDmaMappingInfo->pMemDesc = NULL;

    // delete client dma mapping
    intermapDelDmaMapping(pClient, pVirtualMemory, pDmaMappingInfo->DmaOffset, gpuMask);

    if (pDmaMappingInfoLeft != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            intermapRegisterDmaMapping(pClient, pVirtualMemory, pDmaMappingInfoLeft,
                                       pDmaMappingInfoLeft->DmaOffset, gpuMask),
            failed);
        bDmaMappingInfoLeftRegistered = NV_TRUE;
    }

    if (pDmaMappingInfoRight != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            intermapRegisterDmaMapping(pClient, pVirtualMemory, pDmaMappingInfoRight,
                                       pDmaMappingInfoRight->DmaOffset, gpuMask),
            failed);
        bDmaMappingInfoRightRegistered = NV_TRUE;
    }

failed:
    if (pDmaMappingInfoUnmap != NULL && pDmaMappingInfoUnmap != pDmaMappingInfo)
    {
        memdescFree(pDmaMappingInfoUnmap->pMemDesc);
        memdescDestroy(pDmaMappingInfoUnmap->pMemDesc);
        intermapFreeDmaMapping(pDmaMappingInfoUnmap);
    }

    if (status != NV_OK)
    {
        if (pDmaMappingInfoLeft != NULL)
        {
            if (bDmaMappingInfoLeftRegistered)
                intermapDelDmaMapping(pClient, pVirtualMemory, pDmaMappingInfoLeft->DmaOffset, gpuMask);
            else
                intermapFreeDmaMapping(pDmaMappingInfoLeft);
        }

        if (pDmaMappingInfoRight != NULL)
        {
            if (bDmaMappingInfoRightRegistered)
                intermapDelDmaMapping(pClient, pVirtualMemory, pDmaMappingInfoRight->DmaOffset, gpuMask);
            else
                intermapFreeDmaMapping(pDmaMappingInfoRight);
        }

    }
    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //
    // vGPU doesn't understand subdevice handles.  But clients map memory
    // with subdevice handles and we don't want that to fail on vGPU.
    // Currently, we just pass down the broadcast device handle to the host
    // (which should be equivalent if SLI is disabled).  This will need to
    // be revisited if vGPU ever supports SLI.
    //
    NV_ASSERT((!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu)) || !IsSLIEnabled(pGpu));

    if (pMemory->bRpcAlloc &&
        (NV01_NULL_OBJECT != hMemory) &&
        (resGetRefCount(staticCast(pVirtualMemory, RsResource)) || (hVirtualMem == hMemory)))
    {
        //
        // resGetRefCount(pMemCtx->pResource) is zero when we are here from call of
        // RmFree -> clientFreeResourceTree_IMPL -> clientFreeResource_IMPL -> __nvoc_objDelete
        //
        // memDestruct_IMPL-> CliDelDeviceMemory(i.e. hVirtualMem == hMemory) -> RmUnmapMemoryDma are valid calls since we
        // call RPC_FREE later in memDestruct_IMPL.
        //
        // ifbDestruct_IMPL-> RmUnmapMemoryDma should not RPC_UNMAP_MEMORY_DMA since RPC_FREE is invoked in call stack earlier.
        //
        NV_RM_RPC_UNMAP_MEMORY_DMA(pGpu, hClient, hBroadcastDevice, hVirtualMem, hMemory, 0, dmaOffset, status);
    }

    return status;
}

/*!
 * @brief return address and size of a VirtualMemory object
 */
void virtmemGetAddressAndSize_IMPL
(
    VirtualMemory *pVirtualMemory,
    NvU64 *pVAddr,
    NvU64 *pSize
)
{
    MEMORY_DESCRIPTOR *pMemDesc = staticCast(pVirtualMemory, Memory)->pMemDesc;

    *pVAddr = memdescGetPhysAddr(pMemDesc, AT_GPU_VA, 0);
    *pSize  = memdescGetSize(pMemDesc);
}
