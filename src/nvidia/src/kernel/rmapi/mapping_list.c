/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// to call intermapRegisterDmaMapping() to record the dma mapping at the proper hDevice/dmaOffset location
//
NV_STATUS
intermapCreateDmaMapping
(
    RsClient              *pClient,
    RsResourceRef         *pMemoryRef,
    NvHandle               hDevice,
    NvHandle               hMemCtx,
    PCLI_DMA_MAPPING_INFO *ppDmaMapping,
    NvU32                  flags
)
{
    VirtualMemory         *pVirtualMemory;
    Memory                *pMemory  = NULL;
    PCLI_DMA_MAPPING_INFO  pDmaMapping;
    OBJVASPACE            *pVAS = NULL;

    // Mapping is always virtual memory object
    if (memGetByHandleAndDevice(pClient, hMemCtx, hDevice, &pMemory) != NV_OK)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    pVirtualMemory = dynamicCast(pMemory, VirtualMemory);

    if ((pMemory->pMemDesc == NULL) || (pVirtualMemory == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (vaspaceGetByHandleOrDeviceDefault(pClient, hDevice, pVirtualMemory->hVASpace, &pVAS) != NV_OK)
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
    pDmaMapping->hDevice            = hDevice;
    pDmaMapping->DmaOffset          = 0;
    pDmaMapping->pP2PInfo           = NULL;
    pDmaMapping->Flags              = flags; // NV0S46_*
    pDmaMapping->addressTranslation = VAS_ADDRESS_TRANSLATION(pVAS);

    *ppDmaMapping = pDmaMapping;
    return NV_OK;
}

//
// registers/stores a pDmaMapping created by intermapCreateDmaMapping() at the hDevice/dmaOffset.
//
// important: we assume the hDevice/dmaOffset does NOT change (needs to be re-registerd)
//
NV_STATUS
intermapRegisterDmaMapping
(
    RsClient              *pClient,
    NvHandle               hDevice,
    NvHandle               hMemCtx,
    PCLI_DMA_MAPPING_INFO  pDmaMapping,
    NvU64                  dmaOffset,
    NvU32                  gpuMask
)
{
    NV_STATUS             rmStatus = NV_OK;
    VirtualMemory        *pVirtualMemory = NULL;
    PNODE                 pNode;
    PNODE                *ppDmaMappingList;
    PCLI_DMA_MAPPING_INFO pDmaMappingFirst, pDmaMappingNext;

    // eventually remove Next/Prev once all other linear list based on PCLI_DMA_MAPPING_INFO are gone...
    NV_ASSERT(!pDmaMapping->Next && !pDmaMapping->Prev);

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        virtmemGetByHandleAndDevice(pClient, hMemCtx, hDevice, &pVirtualMemory));

    // the top level consists of lists sorted by hDevice - already created the hDevice specific list?
    if (btreeSearch(hDevice, &pNode, pVirtualMemory->pDmaMappingList) != NV_OK)
    {
        // create a NODE for all pDmaMappings of this hDevice
        pNode = portMemAllocNonPaged(sizeof(NODE));
        if (NULL == pNode)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
        portMemSet(pNode, 0, sizeof(NODE));
        pNode->keyStart = hDevice;
        pNode->keyEnd   = hDevice;
        pNode->Data     = NULL;

        // register the hDevice list itself
        rmStatus = btreeInsert(pNode, &pVirtualMemory->pDmaMappingList);
        if (rmStatus != NV_OK)
        {
            portMemFree(pNode);
            return rmStatus;
        }
    }

    NV_ASSERT(pNode);
    ppDmaMappingList = (PNODE*)&pNode->Data;

    pDmaMapping->gpuMask = gpuMask;

    // the second level consists of CLI_DMA_MAPPING_INFO sorted by dmaOffset -
    if (DRF_VAL(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, pDmaMapping->Flags) ==
                       NVOS46_FLAGS_DMA_UNICAST_REUSE_ALLOC_FALSE)
    {
        NvU64 alignment = 0;

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
                alignment = (NvU64)memdescGetPageSize(memdescGetMemDescFromGpu(pDmaMapping->pMemDesc, pGpu),
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
        rmStatus = btreeInsert(pNode, ppDmaMappingList);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to insert new mapping node for range 0x%llX-0x%llX!\n",
                      pNode->keyStart, pNode->keyEnd);
            DBG_BREAKPOINT();
            portMemFree(pNode);
            return rmStatus;
        }
    }
    else
    {
        // The node for this DMA offset should be already created
        rmStatus = btreeSearch(dmaOffset, &pNode, *ppDmaMappingList);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to find existing mapping node for offset 0x%llX!\n",
                      dmaOffset);
            DBG_BREAKPOINT();
            return rmStatus;
        }

        NV_ASSERT(pNode);
        pDmaMappingFirst = (PCLI_DMA_MAPPING_INFO)pNode->Data;

        // check that we do not exceed the original mapping length
        if (pDmaMapping->pMemDesc->Size > pDmaMappingFirst->pMemDesc->Size)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Mapping length 0x%llX exceeds existing mapping length of 0x%llX!\n",
                      pDmaMapping->pMemDesc->Size,
                      pDmaMappingFirst->pMemDesc->Size);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_LIMIT;
        }

        // Insert the gpuMask element to the list
        pDmaMapping->Next = pDmaMappingFirst;
        pDmaMappingFirst->Prev = pDmaMapping;
        pNode->Data = pDmaMapping;

        // Change the other mappings to remove this gpuMask from them
        pDmaMapping = pDmaMapping->Next;
        while (pDmaMapping)
        {
            pDmaMappingNext = pDmaMapping->Next;
            if (pDmaMapping->gpuMask & gpuMask)
            {
                pDmaMapping->gpuMask &= ~gpuMask;
                if (pDmaMapping->gpuMask == 0)
                {
                    // free the pDmaMapping itself
                    intermapFreeDmaMapping(pDmaMapping);
                }
            }
            pDmaMapping = pDmaMappingNext;
        }
    }

    return NV_OK;
}

NV_STATUS
intermapDelDmaMapping
(
    RsClient *pClient,
    NvHandle hDevice,
    NvHandle hMemCtx,
    NvU64    dmaOffset,
    NvU32    gpuMask,
    NvBool  *pbUnmapped
)
{
    NV_STATUS               rmStatus = NV_OK;
    VirtualMemory          *pVirtualMemory = NULL;
    PCLI_DMA_MAPPING_INFO   pDmaMapping, pDmaMappingNext, pDmaMappingPrev;
    PNODE                   pDeviceNode;
    PNODE                   pOffsetNode;
    PNODE                   pNode;

    if (pbUnmapped != NULL)
        *pbUnmapped = NV_FALSE;

    // Mapping is always virtual memory object
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        virtmemGetByHandleAndDevice(pClient, hMemCtx, hDevice, &pVirtualMemory));

    // first find the list specific to the hDevice
    rmStatus = btreeSearch(hDevice, &pDeviceNode, pVirtualMemory->pDmaMappingList);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    // then get the node belonging to the search offset
    rmStatus = btreeSearch(dmaOffset, &pOffsetNode, (PNODE)pDeviceNode->Data);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    pDmaMapping = pOffsetNode->Data;

    // Remove the first dma mappings intersecting with this GPU mask
    while (pDmaMapping != NULL)
    {
        pDmaMappingNext = pDmaMapping->Next;

        if (pDmaMapping->gpuMask & gpuMask)
        {
            // Remove the element
            pDmaMappingPrev = pDmaMapping->Prev;

            if (pDmaMappingPrev != NULL)
            {
                pDmaMappingPrev->Next = pDmaMappingNext;
            }
            else
            {
                pOffsetNode->Data = pDmaMappingNext;
            }

            if (pDmaMappingNext != NULL)
            {
                pDmaMappingNext->Prev = pDmaMappingPrev;
            }

            // free the pDmaMapping itself
            intermapFreeDmaMapping(pDmaMapping);

            if (pbUnmapped != NULL)
                *pbUnmapped = NV_TRUE;

            break;
        }

        pDmaMapping = pDmaMappingNext;
    }

    // Is the list empty ?
    if (pOffsetNode->Data == NULL)
    {
        // unlink the node
        rmStatus = btreeSearch(dmaOffset, &pNode, (PNODE)pDeviceNode->Data);
        if (rmStatus != NV_OK)
        {
            return rmStatus;
        }

        rmStatus = btreeUnlink(pNode, (PNODE*)&pDeviceNode->Data);
        if (rmStatus == NV_OK)
        {
            // free the node memory itself
            portMemFree(pOffsetNode);

            // is our dmaOffset list empty now?
            if (pDeviceNode->Data == NULL)
            {
                // remove the whole hDevice list
                rmStatus = btreeSearch(hDevice, &pNode, pVirtualMemory->pDmaMappingList);
                if (rmStatus != NV_OK)
                {
                    return rmStatus;
                }

                rmStatus = btreeUnlink(pNode, &pVirtualMemory->pDmaMappingList);
                if (rmStatus == NV_OK)
                {
                    portMemFree(pDeviceNode);
                }
            }
        }
    }

    return rmStatus;
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

static NvBool
_getDmaMappingInfoFromMemory
(
    VirtualMemory         *pVirtualMemory,
    NvHandle               hDevice,
    NvU64                  dmaOffset,
    NvU32                  gpuMask,
    PCLI_DMA_MAPPING_INFO *ppDmaMappingInfo
)
{
    PNODE pDmaMappingList;
    PNODE pNode;
    PCLI_DMA_MAPPING_INFO pDmaMappingInfo;

    // first find the list specific to the hDevice
    pDmaMappingList = pVirtualMemory->pDmaMappingList;
    if (btreeSearch(hDevice, &pNode, pDmaMappingList) == NV_OK)
    {
        pDmaMappingList = (PNODE)pNode->Data;
        NV_ASSERT(pDmaMappingList);

        // then get the node belonging to the search offset
        if (btreeSearch(dmaOffset, &pNode, pDmaMappingList) == NV_OK)
        {
            // Then look for the GPU mask
            pDmaMappingInfo = pNode->Data;
            while (pDmaMappingInfo)
            {
                if (pDmaMappingInfo->gpuMask & gpuMask)
                {
                    // Returns the first mapping that intersects with this gpu mask.
                    break;
                }
                pDmaMappingInfo = pDmaMappingInfo->Next;
            }
            if (pDmaMappingInfo != NULL)
            {
                *ppDmaMappingInfo = pDmaMappingInfo;
                return NV_TRUE;
            }
        }
    }
    return NV_FALSE;
}

/*!
 * @brief Lookup mapping info in memory context or VA space
 *
 * This is useful when processing SW methods.  We can find the hVASpace
 * from the channel context uniquely.  Previous lookup within the whole
 * client could mistakenly find an alias on another device.
 */
NvBool
CliGetDmaMappingInfo
(
    NvHandle               hClient,
    NvHandle               hDevice,
    NvHandle               hMemCtx,
    NvU64                  dmaOffset,
    NvU32                  gpuMask,
    PCLI_DMA_MAPPING_INFO *ppDmaMappingInfo
)
{
    VirtualMemory      *pVirtualMemory;
    RsClient           *pClient;
    Device             *pDevice;
    NODE               *pNode;
    NV_STATUS           status;
    NvBool              bFound;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return NV_FALSE;

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
            return _getDmaMappingInfoFromMemory(pVirtualMemory, hDevice, dmaOffset, gpuMask, ppDmaMappingInfo);
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
             virtmemMatchesVASpace(pVirtualMemory, hClient, hMemCtx))
        {
            bFound = _getDmaMappingInfoFromMemory(pVirtualMemory, hDevice,
                                                  dmaOffset, gpuMask,
                                                  ppDmaMappingInfo);
            if (bFound)
                return bFound;
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
    if (pDmaMappingList != NULL)
    {
        // find the first hDevice list
        pIt->pDmaMappingList = pDmaMappingList;
        btreeEnumStart(0, &pIt->pCurrentList, pIt->pDmaMappingList);
        if (pIt->pCurrentList != NULL)
        {
            // find the first pDmaMapping of the hDevice list (hDevice lists can't be empty *ever*)
            NV_ASSERT(pIt->pCurrentList->Data);
            btreeEnumStart(0, &pIt->pNextDmaMapping, pIt->pCurrentList->Data);
            NV_ASSERT(pIt->pNextDmaMapping);
            NV_ASSERT(pIt->pNextDmaMapping->Data);

            CliGetDmaMappingNext(ppFirstDmaMapping, pIt);
        }
    }
}

void
CliGetDmaMappingNext
(
    PCLI_DMA_MAPPING_INFO *ppDmaMapping,
    PCLI_DMA_MAPPING_INFO_ITERATOR pIt
)
{
    PCLI_DMA_MAPPING_INFO pDmaMapping = NULL;

    // are we done with all hDevice lists?
    if ((pIt->pDmaMappingList != NULL) &&
        (pIt->pCurrentList != NULL) &&
        (pIt->pNextDmaMapping != NULL))
    {
        // return the current node.
        NV_ASSERT(pIt->pNextDmaMapping->Data);
        pDmaMapping = (PCLI_DMA_MAPPING_INFO)pIt->pNextDmaMapping->Data;

        // iterate to the next DmaOffset (so the caller is free to delete the node)
        btreeEnumNext(&pIt->pNextDmaMapping, pIt->pCurrentList);

        // reached the end of the hDevice list? move to next hDevice
        if (pIt->pNextDmaMapping == NULL)
        {
            btreeEnumNext(&pIt->pCurrentList, pIt->pDmaMappingList);
            if (pIt->pCurrentList != NULL)
            {
                // restart iteration process for the new list
                NV_ASSERT(pIt->pCurrentList->Data);
                btreeEnumStart(0, &pIt->pNextDmaMapping, pIt->pCurrentList->Data);
                NV_ASSERT(pIt->pNextDmaMapping);
            }
        }
    }

    // stop iterating once we hit the end of list [or something bad happened]
    if (pDmaMapping == NULL)
    {
        pIt->pDmaMappingList = NULL;
        pIt->pCurrentList    = NULL;
        pIt->pNextDmaMapping = NULL;
    }
    *ppDmaMapping = pDmaMapping;
}
