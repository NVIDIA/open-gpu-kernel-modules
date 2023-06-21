/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "mem_mgr/vaspace.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "rmapi/mapping_list.h"
#include "resserv/rs_server.h"
#include "mem_mgr/virtual_mem.h"
#include "mem_mgr/mem.h"
#include "resserv/rs_client.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/mmu/kern_gmmu.h"

#include "gpu/device/device.h"
#include "rmapi/rs_utils.h"

// ****************************************************************************
//                              Client Memory Mappings
// ****************************************************************************

RsCpuMapping*
CliFindMappingInClient
(
    NvHandle hClient,
    NvHandle hDevice,
    NvP64    cpuAddress
)
{
    RsClient *pRsClient;
    RsResourceRef *pDeviceRef;
    RS_ORDERED_ITERATOR it;

    NvU32 processId;
    CALL_CONTEXT *pCallContext;
    NvBool bKernel;

    if ((serverGetClientUnderLock(&g_resServ, hClient, &pRsClient) != NV_OK) ||
        (clientGetResourceRef(pRsClient, hDevice, &pDeviceRef) != NV_OK))
    {
        return NULL;
    }

    pCallContext = resservGetTlsCallContext();

    // This function only called from control call; call context should be available.
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NULL);

    bKernel = (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL);
    processId = osGetCurrentProcess();

    // iterator will match derived classes
    it = clientRefOrderedIter(pRsClient, pDeviceRef, classId(Memory), NV_FALSE);
    while (clientRefOrderedIterNext(pRsClient, &it))
    {
        RsResourceRef *pMemoryRef = it.pResourceRef;
        RsCpuMappingListIter mapIt = listIterAll(&pMemoryRef->cpuMappings);

        while (listIterNext(&mapIt))
        {
            RsCpuMapping *pMapping = mapIt.pValue;
            if ((pMapping->pPrivate->bKernel == bKernel) &&
                (bKernel || (pMapping->processId == processId)) &&
                (pMapping->pPrivate->gpuAddress != ((NvU64) ~0x0)) &&
                (pMapping->pLinearAddress <= cpuAddress) &&
                ((NvU64)cpuAddress < ((NvU64)pMapping->pLinearAddress + pMapping->length)))
            {
                return pMapping;
            }
        }
    }

    return NULL;
}



// ****************************************************************************
//                                  DMA Mappings
// ****************************************************************************

//
// allocates/initializes a new CLI_DMA_MAPPING_INFO.
//
// Ideally, we would know the dmaOffset by now but we typically don't. Thus the caller needs
// to call intermapRegisterDmaMapping() to record the dma mapping at the proper dmaOffset location
//
NV_STATUS
intermapCreateDmaMapping
(
    RsClient              *pClient,
    VirtualMemory         *pVirtualMemory,
    PCLI_DMA_MAPPING_INFO *ppDmaMapping,
    NvU32                  flags
)
{
    Memory                *pMemory  = NULL;
    PCLI_DMA_MAPPING_INFO  pDmaMapping;
    OBJVASPACE            *pVAS = NULL;

    // Mapping is always virtual memory object
    if (pVirtualMemory == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pMemory = dynamicCast(pVirtualMemory, Memory);

    if (pMemory->pMemDesc == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (vaspaceGetByHandleOrDeviceDefault(pClient, RES_GET_HANDLE(pMemory->pDevice), pVirtualMemory->hVASpace, &pVAS) != NV_OK)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // allocate a new mapping info struct and add to the dma mapping object
    pDmaMapping = portMemAllocNonPaged(sizeof(CLI_DMA_MAPPING_INFO));
    if (NULL == pDmaMapping)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // initialize the dma mapping info (not registered yet)
    portMemSet(pDmaMapping, 0, sizeof(CLI_DMA_MAPPING_INFO));
    pDmaMapping->DmaOffset          = 0;
    pDmaMapping->bP2P               = NV_FALSE;
    pDmaMapping->Flags              = flags; // NV0S46_*
    pDmaMapping->addressTranslation = VAS_ADDRESS_TRANSLATION(pVAS);

    *ppDmaMapping = pDmaMapping;
    return NV_OK;
}

//
// registers/stores a pDmaMapping created by intermapCreateDmaMapping() at the dmaOffset.
//
// important: we assume the dmaOffset does NOT change (needs to be re-registerd)
//
NV_STATUS
intermapRegisterDmaMapping
(
    RsClient              *pClient,
    VirtualMemory         *pVirtualMemory,
    PCLI_DMA_MAPPING_INFO  pDmaMapping,
    NvU64                  dmaOffset,
    NvU32                  gpuMask
)
{
    NV_STATUS             rmStatus = NV_OK;
    PNODE                 pNode;
    NvU64 alignment = 0;

    pDmaMapping->gpuMask = gpuMask;

    if (FLD_TEST_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _TRUE, pDmaMapping->Flags))
    {
        CLI_DMA_MAPPING_INFO *pDmaMappingFirst;
        CLI_DMA_MAPPING_INFO *pDmaMappingCurrent;

        // The node for this DMA offset should be already created
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, btreeSearch(dmaOffset, &pNode, pVirtualMemory->pDmaMappingList));
        pDmaMappingFirst = (CLI_DMA_MAPPING_INFO *)pNode->Data;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, pDmaMappingFirst->DmaOffset == pDmaMapping->DmaOffset &&
                                        pDmaMappingFirst->pMemDesc->Size ==  pDmaMapping->pMemDesc->Size,
                           NV_ERR_INVALID_ARGUMENT);

        // Check gpuMasks for consistency
        NV_CHECK_OR_RETURN(LEVEL_ERROR, (pDmaMappingFirst->gpuMask & (pDmaMappingFirst->gpuMask - 1)) == 0, NV_ERR_INVALID_ARGUMENT);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, (pDmaMapping->gpuMask & (pDmaMapping->gpuMask - 1)) == 0, NV_ERR_INVALID_ARGUMENT);
        pDmaMappingCurrent = pDmaMappingFirst;
        while (pDmaMappingCurrent != NULL)
        {
            NV_CHECK_OR_RETURN(LEVEL_ERROR, (pDmaMapping->gpuMask & pDmaMappingCurrent->gpuMask) == 0, NV_ERR_INVALID_ARGUMENT);
            pDmaMappingCurrent = pDmaMappingCurrent->pNext;
        }

        // Insert the gpuMask element to the list
        pDmaMapping->pNext = pDmaMappingFirst;
        pNode->Data = pDmaMapping;

        return NV_OK;
    }

    if (pDmaMapping->pMemDesc->pGpu != NULL)
    {
        OBJGPU  *pGpu  = pDmaMapping->pMemDesc->pGpu;
        //
        // For verify purposes we should allow small page override for mapping.
        // This will be used for testing VASpace interop.
        // However, this info is not captured in the DMA mapping info for guest.
        // So explicitly check for this case in guest.
        //
        if (IS_VIRTUAL_WITH_SRIOV(pGpu)
            && RMCFG_FEATURE_PLATFORM_MODS
            && FLD_TEST_DRF(OS46, _FLAGS, _PAGE_SIZE, _4KB, pDmaMapping->Flags)
            && kgmmuIsVaspaceInteropSupported(GPU_GET_KERNEL_GMMU(pGpu))
            )
        {
            alignment = RM_PAGE_SIZE;
        }
        else
        {
            alignment = memdescGetPageSize(memdescGetMemDescFromGpu(pDmaMapping->pMemDesc, pGpu),
                                             pDmaMapping->addressTranslation);
        }
    }

    //
    // In some cases page size may not be set (e.g. NV50, AMODEL, VGPU).
    // Ideally we should fix all paths for consistency, but for now
    // allowing fallback to unaligned tracking (no worse than before).
    //
    // TODO: Revisit this with memdesc page size cleanup.
    //
    if (alignment == 0)
    {
        alignment = 1;
    }

    // create the node for this dmaOffset
    pNode = portMemAllocNonPaged(sizeof(NODE));
    if (NULL == pNode)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    portMemSet(pNode, 0, sizeof(NODE));

    //
    // For normal GPU devices, track the mapping over its entire
    // virtual range so overlapping mappings are caught.
    //
    // keyStart and keyEnd must be aligned to the physical page size to
    // ensure no page can be mapped twice.
    // (e.g. small pages mapped into the leftovers of a big page).
    //
    // NOTE: Unfortunately this check occurs after the internal mapping
    //       has already taken place, so the state is already corrupted.
    //       Failure here means "Oops, your're screwed."
    //
    //       For Fermi+ we have added checks in the internal mapping code
    //       that will fail safely.
    //
    pNode->keyStart = RM_ALIGN_DOWN(dmaOffset, alignment);
    pNode->keyEnd   = RM_ALIGN_UP(dmaOffset + pDmaMapping->pMemDesc->Size,
                                  alignment) - 1;
    pNode->Data     = pDmaMapping;

    // register the 'dmaOffset' list itself
    rmStatus = btreeInsert(pNode, &pVirtualMemory->pDmaMappingList);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to insert new mapping node for range 0x%llX-0x%llX!\n",
                  pNode->keyStart, pNode->keyEnd);
        DBG_BREAKPOINT();
        portMemFree(pNode);
        return rmStatus;
    }

    return NV_OK;
}

NV_STATUS
intermapDelDmaMapping
(
    RsClient *pClient,
    VirtualMemory *pVirtualMemory,
    NvU64    dmaOffset,
    NvU32    gpuMask
)
{
    PNODE                   pNode;
    CLI_DMA_MAPPING_INFO   *pDmaMapping;
    CLI_DMA_MAPPING_INFO   *pDmaMappingPrev = NULL;

    // then get the node belonging to the search offset
    NV_ASSERT_OK_OR_RETURN(btreeSearch(dmaOffset, &pNode, pVirtualMemory->pDmaMappingList));

    pDmaMapping = (CLI_DMA_MAPPING_INFO *)pNode->Data;
    while (pDmaMapping != NULL)
    {
        if (pDmaMapping->gpuMask & gpuMask)
        {
            CLI_DMA_MAPPING_INFO *pDmaMappingNext = pDmaMapping->pNext;

            NV_ASSERT_OR_RETURN(pDmaMapping->gpuMask == gpuMask, NV_ERR_INVALID_ARGUMENT);

            // Remove the element
            if (pDmaMappingPrev != NULL)
            {
                pDmaMappingPrev->pNext = pDmaMappingNext;
            }
            else if (pDmaMappingNext != NULL)
            {
                pNode->Data = pDmaMappingNext;
            }
            else
            {
                NV_ASSERT_OK_OR_RETURN(btreeUnlink(pNode, &pVirtualMemory->pDmaMappingList));
                portMemFree(pNode);
            }

            intermapFreeDmaMapping(pDmaMapping);
            return NV_OK;
        }
        pDmaMappingPrev = pDmaMapping;
        pDmaMapping = pDmaMapping->pNext;
    }

    // mapping with the right gpuMask was not found
    NV_ASSERT(0);
    return NV_ERR_OBJECT_NOT_FOUND;
}

void
intermapFreeDmaMapping
(
    PCLI_DMA_MAPPING_INFO pDmaMapping
)
{
    NV_ASSERT(pDmaMapping != NULL);

    // free the list element
    portMemFree(pDmaMapping);
}

/*!
 * @brief Lookup mapping info in a virtual memory allocation
 */
CLI_DMA_MAPPING_INFO *
intermapGetDmaMapping
(
    VirtualMemory         *pVirtualMemory,
    NvU64                  dmaOffset,
    NvU32                  gpuMask
)
{
    PNODE pNode;
    CLI_DMA_MAPPING_INFO *pDmaMapping;

    // get the node belonging to the search offset
    if (btreeSearch(dmaOffset, &pNode, pVirtualMemory->pDmaMappingList) != NV_OK)
        return NULL;

    pDmaMapping = pNode->Data;

    while (pDmaMapping != NULL)
    {
        if (pDmaMapping->gpuMask & gpuMask)
            return pDmaMapping;

        pDmaMapping = pDmaMapping->pNext;
    }

    return pDmaMapping;
}

/*!
 * @brief Lookup mapping info in VA space
 *
 * This is useful for semaphores/notifiers when processing SW methods.
 */
NvBool
CliGetDmaMappingInfo
(
    RsClient              *pClient,
    NvHandle               hDevice,
    NvHandle               hMemCtx,
    NvU64                  dmaOffset,
    NvU32                  gpuMask,
    PCLI_DMA_MAPPING_INFO *ppDmaMappingInfo
)
{
    VirtualMemory      *pVirtualMemory;
    Device             *pDevice;
    NODE               *pNode;
    NV_STATUS           status;

    // Try a non-zero handle as a NVxx_MEMORY_VIRTUAL object
    if (hMemCtx != NV01_NULL_OBJECT)
    {
        RsResourceRef *pMemCtxRef;

        status = clientGetResourceRef(pClient, hMemCtx, &pMemCtxRef);
        if (status != NV_OK)
            return NV_FALSE;

        // If passed a memory handle directly go ahead and look for an associated mapping
        pVirtualMemory = dynamicCast(pMemCtxRef->pResource, VirtualMemory);
        if (pVirtualMemory != NULL)
        {

            *ppDmaMappingInfo = intermapGetDmaMapping(pVirtualMemory, dmaOffset, gpuMask);
            return *ppDmaMappingInfo != NULL;
        }
    }

    status = deviceGetByHandle(pClient, hDevice, &pDevice);
    if (status != NV_OK)
        return NV_FALSE;

    btreeEnumStart(0, &pNode, pDevice->DevMemoryTable);
    while (pNode != NULL)
    {
        Memory *pMemory = pNode->Data;
        VirtualMemory *pVirtualMemory = dynamicCast(pMemory, VirtualMemory);
        btreeEnumNext(&pNode, pDevice->DevMemoryTable);

        if ((pVirtualMemory != NULL) &&
             virtmemMatchesVASpace(pVirtualMemory, pClient->hClient, hMemCtx))
        {
            *ppDmaMappingInfo = intermapGetDmaMapping(pVirtualMemory, dmaOffset, gpuMask);

            if (*ppDmaMappingInfo != NULL)
                return NV_TRUE;
        }
    }

    return NV_FALSE;
}

void
CliGetDmaMappingIterator
(
    PCLI_DMA_MAPPING_INFO         *ppFirstDmaMapping,       // [OUT] first found pDmaMapping
    PCLI_DMA_MAPPING_INFO_ITERATOR pIt,                     // [OUT] iterator object to enum all other pDmaMappings
    PNODE                          pDmaMappingList          // [IN]  the two level pDmaMapping list to iterate
)
{
    // don't iterate if we didn't get a empty list
    *ppFirstDmaMapping = NULL;
    portMemSet(pIt, 0, sizeof(*pIt));

    if (pDmaMappingList == NULL)
        return;

    // find the first pDmaMapping
    pIt->pDmaMappingList = pDmaMappingList;
    btreeEnumStart(0, &pIt->pNextDmaMapping, pIt->pDmaMappingList);

    if (pIt->pNextDmaMapping == NULL)
        return;

    NV_ASSERT(pIt->pNextDmaMapping->Data);
    CliGetDmaMappingNext(ppFirstDmaMapping, pIt);
}

void
CliGetDmaMappingNext
(
    PCLI_DMA_MAPPING_INFO *ppDmaMapping,
    PCLI_DMA_MAPPING_INFO_ITERATOR pIt
)
{
    PCLI_DMA_MAPPING_INFO pDmaMapping = NULL;

    if (pIt->pNextDmaMapping != NULL)
    {
        // return the current node.
        NV_ASSERT(pIt->pNextDmaMapping->Data);
        pDmaMapping = (PCLI_DMA_MAPPING_INFO)pIt->pNextDmaMapping->Data;

        // not supporting legacy _DMA_UNICAST_REUSE_ALLOC case
        NV_ASSERT(pDmaMapping->pNext == NULL);

        // iterate to the next DmaOffset (so the caller is free to delete the node)
        btreeEnumNext(&pIt->pNextDmaMapping, pIt->pDmaMappingList);
    }

    // stop iterating once we hit the end of list [or something bad happened]
    if (pDmaMapping == NULL)
    {
        pIt->pDmaMappingList = NULL;
        pIt->pNextDmaMapping = NULL;
    }
    *ppDmaMapping = pDmaMapping;
}
