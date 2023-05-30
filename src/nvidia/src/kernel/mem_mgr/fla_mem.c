/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "mem_mgr_internal.h"
#include "mem_mgr/fla_mem.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "rmapi/rmapi.h"
#include "rmapi/client.h"
#include "gpu/device/device.h"

#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl00f3.h" // NV01_MEMORY_FLA

NV_STATUS
flamemConstruct_IMPL
(
    FlaMemory                    *pFlaMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_FLA_MEMORY_ALLOCATION_PARAMS *pAllocParams;
    Memory                          *pMemory      = staticCast(pFlaMemory, Memory);
    OBJGPU                          *pGpu         = pMemory->pGpu;
    MEMORY_DESCRIPTOR               *pMemDesc     = NULL;
    MemoryManager                   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS                        status       = NV_OK;
    NvU32                            idx          = 0;
    NV_ADDRESS_SPACE                 addressSpace = ADDR_FBMEM;
    NvU32                            Cache        = 0;
    NvBool                           bContig;
    NvU32                            hwResId      = 0;
    NvU32                            format       = 0;
    NvU64                            pageSize     = 0;
    NvU64                            pageCount    = 0;
    NvU64                            newBase      = 0;
    NvU64                            flaBaseAddr  = 0;
    NvU64                            flaSize      = 0;
    RsClient                        *pExportClient;
    RsClient                        *pImportClient;
    OBJGPU                          *pExportGpu   = NULL;
    RM_API                          *pRmApi       = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsResourceRef                   *pExportRef   = NULL;
    NvBool                           bLoopback    = NV_FALSE;
    NvU32                            gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
    {
        return flamemCopyConstruct_IMPL(pFlaMemory, pCallContext, pParams);
    }

    pAllocParams = pParams->pAllocParams;

    flaBaseAddr = pAllocParams->base;
    flaSize     = pAllocParams->limit + 1;

    //
    // NV01_MEMORY_FLA class is used by the clients to create memory objects for memory
    // to be imported from other GPU(s)/FAM/Process.
    //

    if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_UNCACHED)
        Cache = NV_MEMORY_UNCACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_CACHED)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE)
        Cache = NV_MEMORY_WRITECOMBINED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_THROUGH)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_PROTECT)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_BACK)
        Cache = NV_MEMORY_CACHED;

    // Must be of valid type, in FBMEM
    if ((pAllocParams->type >= NVOS32_NUM_MEM_TYPES) ||
        (!FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pAllocParams->attr)) ||
        (pAllocParams->flags & (NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED |
                             NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT |
                             NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE |
                             NVOS32_ALLOC_FLAGS_BANK_FORCE |
                             NVOS32_ALLOC_FLAGS_VIRTUAL)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Retrieving the exporting device
    status = serverGetClientUnderLock(&g_resServ, pAllocParams->hExportClient, &pExportClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,"hExportClient: %x not valid, status: %x\n", pAllocParams->hExportClient,
                  status);
        return status;
    }

    status = serverGetClientUnderLock(&g_resServ, pParams->hClient, &pImportClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,"hImportClient: %x not valid, status: %x\n", pParams->hClient,
                  status);
        return status;
    }

    status = gpuGetByHandle(pImportClient, pAllocParams->hExportSubdevice, NULL, &pExportGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,"hParent: %x not valid under client: %x, status: %x\n", pAllocParams->hExportSubdevice,
            pParams->hClient, status);
        return status;
    }

    if (pExportGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid Parent handle: %x in client: %x \n",
                    pAllocParams->hExportSubdevice, pParams->hClient);
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
    }
    pFlaMemory->peerDeviceInst = gpuGetDeviceInstance(pExportGpu);
    pFlaMemory->peerGpuInst    = gpuGetInstance(pExportGpu);

    bLoopback = (gpuGetDeviceInstance(pGpu) == pFlaMemory->peerDeviceInst);

    switch (DRF_VAL(OS32, _ATTR, _PAGE_SIZE, pAllocParams->attr))
    {
        case NVOS32_ATTR_PAGE_SIZE_4KB:
            pageSize = RM_PAGE_SIZE;
            break;
        case NVOS32_ATTR_PAGE_SIZE_BIG:
            pageSize = RM_PAGE_SIZE_64K;
            break;
        case NVOS32_ATTR_PAGE_SIZE_HUGE:
            if (DRF_VAL(OS32, _ATTR2, _PAGE_SIZE_HUGE, pAllocParams->attr2) ==
                        NVOS32_ATTR2_PAGE_SIZE_HUGE_2MB)
                pageSize = RM_PAGE_SIZE_HUGE;
            else if ((DRF_VAL(OS32, _ATTR2, _PAGE_SIZE_HUGE, pAllocParams->attr2) ==
                            NVOS32_ATTR2_PAGE_SIZE_HUGE_512MB) ||
                     (DRF_VAL(OS32, _ATTR2, _PAGE_SIZE_HUGE, pAllocParams->attr2) ==
                            NVOS32_ATTR2_PAGE_SIZE_HUGE_DEFAULT))
            {
                pageSize = RM_PAGE_SIZE_512M;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Unknown page size specified");
                return NV_ERR_INVALID_ARGUMENT;
            }
            break;
        case NVOS32_ATTR_PAGE_SIZE_DEFAULT:
                pageSize = RM_PAGE_SIZE_512M;
            break;
    }

     // AlignUp flaSize to pageSize
    flaSize = NV_ALIGN_UP64(flaSize, pageSize);

    // if hExportHandle is passed, we need to bump the refcount as well as deduct the base and size
    // from the handle
    if (IS_GFID_PF(gfid) && pAllocParams->hExportHandle)
    {
        RMRES_MEM_INTER_MAP_PARAMS memInterMapParams;
        MEMORY_DESCRIPTOR          *pExportMemDesc;

        // Decoding the export handle
        status = clientGetResourceRef(pExportClient, pAllocParams->hExportHandle, &pExportRef);
        if (status != NV_OK)
             return status;

        // Use virtual GetMemInterMapParams to get information needed for mapping from pExportRef->pResource
        portMemSet(&memInterMapParams, 0, sizeof(memInterMapParams));

        memInterMapParams.pGpu = pExportGpu;
        memInterMapParams.pMemoryRef = pExportRef;
        memInterMapParams.bSubdeviceHandleProvided = NV_TRUE;

        status = rmresGetMemInterMapParams(dynamicCast(pExportRef->pResource, RmResource), &memInterMapParams);
        if (status != NV_OK)
            return status;

        pExportMemDesc = memInterMapParams.pSrcMemDesc;
        if (pExportMemDesc)
        {
            if (!(pExportMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS))
            {
                NV_PRINTF(LEVEL_ERROR, "Exported handle is discontiguous, bailing out, export handle: %x gpu: %x\n",
                          pAllocParams->hExportHandle, pExportGpu->gpuInstance);
                return NV_ERR_INVALID_ARGUMENT;
            }

            flaBaseAddr = pExportMemDesc->_pteArray[0];
            flaSize = NV_ALIGN_UP64(memdescGetSize(pExportMemDesc), pageSize);
            NV_PRINTF(LEVEL_INFO, "fla base addr: %llx,. size: %llx, gpu: %x\n", flaBaseAddr, flaSize, pGpu->gpuInstance);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid Export Handle: %x, export gpu: %x\n", pAllocParams->hExportHandle, pExportGpu->gpuInstance);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if(!NV_IS_ALIGNED(flaBaseAddr, pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "flaBaseAsddr: 0x%llx is not aligned to page size: 0x%llx\n",
                  flaBaseAddr, pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((flaSize == 0)
           || (flaSize < pageSize)
       )
    {
         NV_PRINTF(LEVEL_ERROR, "flaSize: %llx is either 0 or less than page size: %llx, gpu: %x\n",
                   flaSize, pageSize, pGpu->gpuInstance);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((status = memmgrGetFlaKind_HAL(pGpu, pMemoryManager, &format)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FLA is not supported for this platform, only from ampere\n");
        return status;
    }

    if (!bLoopback && pAllocParams->hExportHandle)
    {
        //
        // TODO: This might break SMC assumption whenver we support FLA in SMC (currently not supported for Ampere)
        // Duping the exported handle under device on the import side
        // we need this to make sure that the export handle doesn't get reused after FM crash or app crash
        //
        // During Import, we need to increment the refCount on the exported FLA VA Handle, else exporting client can
        // free the FLA VA Handle and can be reused by same/another client for a different memory leading to a security riskk.
        // Currently, we are duping the exported handle under this hierarchy (importing client-> exporting device->
        // ..duped exported FLA VAS -> duped exported FLA Handle)
        //
        // Notes for scalabilty: Since this import call touches two clients, we should be locking both clients before calling
        // into the constructor
        //
        RsClient *pImportClient;
        NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, pParams->hClient, &pImportClient));
        Device *pImportDevice;
        NV_ASSERT_OK_OR_RETURN(deviceGetByGpu(pImportClient, pExportGpu, NV_TRUE, &pImportDevice));

        status = pRmApi->DupObject(pRmApi,
                                   pParams->hClient,
                                   RES_GET_HANDLE(pImportDevice),
                                   &pFlaMemory->hDupedExportMemory,
                                   pAllocParams->hExportClient,
                                   pAllocParams->hExportHandle,
                                   0 //flags
                                   );
        if (status != NV_OK)
            return status;

    }
    bContig = FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, pAllocParams->flagsOs02);

    pAllocParams->flags |= (NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED |
                        NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT);

    status = memdescCreate(&pMemDesc, pGpu, flaSize, 0, bContig, addressSpace,
                             Cache,
                             MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE_FB_BC_ONLY(pGpu, addressSpace));
    if (status != NV_OK)
    {
        goto done_fbmem;
    }

    //
    //  Now arrange the fla range based on the page size attributes
    //  TODO:  what if the page size is more than the flaSize or
    //         flaSize not aligned for pageSize
    //
    newBase  = flaBaseAddr;
    idx = 0;
    if (bContig)
    {
        memdescSetPte(pMemDesc, AT_GPU, idx, newBase);
    }
    else
    {
        pageCount = flaSize/pageSize;
        // reformat the base to PFN and store it in a list
        for ( ;idx < pageCount; idx++, newBase += pageSize)
        {
            memdescSetPte(pMemDesc, AT_GPU, idx, newBase);
        }
    }
    memdescSetPageSize(pMemDesc, AT_GPU, pageSize);

    status = memConstructCommon(pMemory,
                                pCallContext->pResourceRef->externalClassId, // FLA_MEMORY
                                0,                       // flags
                                pMemDesc,
                                0,                       // heapOwner
                                NULL,
                                pAllocParams->attr,
                                pAllocParams->attr2,
                                0,                       // pitch
                                pAllocParams->type,      // type
                                NVOS32_MEM_TAG_NONE,
                                NULL);                   // pHwResource
    if (status != NV_OK)
    {
        goto done_fbmem;
    }

    memdescSetPteKind(pMemory->pMemDesc, format);
    memdescSetHwResId(pMemory->pMemDesc, hwResId);

    NV_PRINTF(LEVEL_INFO, "FLA Memory imported into peerDeviceInst: %x, base: %llx., size: %llx, exported from: %x \n",
             gpuGetDeviceInstance(pGpu), newBase, flaSize, pFlaMemory->peerDeviceInst);

    // Restrict RPC when guest is running in FULL SRIOV mode (split vas is enabled)
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        pAllocParams->base  = newBase;
        pAllocParams->limit = flaSize - 1;
        NV_RM_RPC_ALLOC_OBJECT(pGpu,
                               pParams->hClient,
                               pParams->hParent,
                               pParams->hResource,
                               pParams->externalClassId,
                               pAllocParams,
                               sizeof(*pAllocParams),
                               status);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                        "vGPU-RPC to FLA Memory Construct failed with status: %x \n",
                        status);
            goto delete_memory;
        }

        pMemory->bRpcAlloc = NV_TRUE;
    }

    return NV_OK;

delete_memory:
    memDestructCommon(pMemory);

done_fbmem:
    memdescDestroy(pMemDesc);

    // free the duped memory
    if (!bLoopback &&  pAllocParams->hExportHandle)
    {
        pRmApi->Free(pRmApi, pParams->hClient, pFlaMemory->hDupedExportMemory);
    }

    return status;
}

NvBool
flamemCanCopy_IMPL
(
    FlaMemory *pFlaMemory
)
{
    return NV_TRUE;
}

NV_STATUS
flamemCopyConstruct_IMPL
(
    FlaMemory                    *pFlaMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    FlaMemory   *pSrcFlaMem;

    pSrcFlaMem = dynamicCast(pParams->pSrcRef->pResource, FlaMemory);

    pFlaMemory->peerDeviceInst = pSrcFlaMem->peerDeviceInst;
    pFlaMemory->peerGpuInst    = pSrcFlaMem->peerGpuInst;
    pFlaMemory->hDupedExportMemory = NV01_NULL_OBJECT;
    return NV_OK;
}

void
flamemDestruct_IMPL
(
    FlaMemory *pFlaMemory
)
{
    Memory     *pMemory    = staticCast(pFlaMemory, Memory);
    RM_API     *pRmApi     = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsClient   *pClient    = RES_GET_CLIENT(pMemory);
    NvHandle    hClient    = pClient->hClient;
    OBJGPU     *pGpu       = pMemory->pGpu;

    if (!pFlaMemory)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid Object in destruct, bailing out ..\n");
        return;
    }
    //
    // Releasing the Duped export memory handle
    // Note: RM dupes the export memory handle with the importing client handle
    // both the export memory client and import memory client are external clients
    // hClient here can be same/different than duped memory client (single process/multiprocess)
    // This could cause issues when we move to per-client locking, need to revisit this at that point
    //
    if (pFlaMemory->peerDeviceInst != gpuGetDeviceInstance(pGpu) &&
        pFlaMemory->hDupedExportMemory != NV01_NULL_OBJECT)
        pRmApi->Free(pRmApi, hClient, pFlaMemory->hDupedExportMemory);
}
