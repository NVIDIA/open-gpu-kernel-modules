/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Description:
//       Various vgpu utility functions.
//
//******************************************************************************

#include "core/core.h"
#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "vgpu/dev_vgpu.h"
#include "vgpu/vgpu_events.h"
#include "vgpu/vgpu_util.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "vgpu/rpc.h"
#include "core/system.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "gpu/bus/kern_bus.h"

/*!
 * @brief Function to free memorry associated with sysmem PFN bitmap node.
 */
void vgpuFreeSysmemPfnBitMapNode(VGPU_SYSMEM_PFN_BITMAP_NODE_P node)
{
    if (node)
    {
        if (node->sysmemPfnMap && node->pMemDesc_sysmemPfnMap)
            memdescUnmapOld(node->pMemDesc_sysmemPfnMap,
                            memdescGetFlag(node->pMemDesc_sysmemPfnMap, MEMDESC_FLAGS_KERNEL_MODE),
                            osGetCurrentProcess(),
                            (void *)node->sysmemPfnMap,
                            (void *)node->sysmemPfnMap_priv);

        if ((node->pMemDesc_sysmemPfnMap) && (node->pMemDesc_sysmemPfnMap->Allocated))
            memdescFree(node->pMemDesc_sysmemPfnMap);

        memdescDestroy(node->pMemDesc_sysmemPfnMap);

        portMemFree(node);
    }
}

NvU64 vgpuGspSysmemPfnMakeBufferAddress(MEMORY_DESCRIPTOR *pMemDesc,  NvU64 pfn)
{
    NV_ADDRESS_SPACE addressSpace = memdescGetAddressSpace(pMemDesc);
    NvU64 pfnBufferAddr = REF_DEF64(VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY, _VALID) |
                          REF_NUM64(VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_PFN, pfn);
    switch (addressSpace)
    {
        case ADDR_SYSMEM:
            pfnBufferAddr |= REF_DEF64(VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_APERTURE, _SYSMEM);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Dirty sysmem pfn: Invlid address space %d\n", addressSpace);
            pfnBufferAddr = VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY_INVALID;
            break;

    }
    return pfnBufferAddr;
}

/*!
 * @brief Function to alloc sysmem PFN bitmap node.
 *
 * Sysmem PFN bitmap node is allocated either during RPC infrastructure
 * initialization or while expanding the sysmem PFN bitmap list.
 * This function is responsible for allocting the node, writing the pfns in the
 * sysmem PFN ring page to facilitate the plugin to access the bitmap PFNs and
 * asking the plugin to map the bitmap pages by writing to emulated NV_VGPU_SYSMEM_BITMAP
 * bar0 register.
 *
 * @param[in]   pGpu    OBJGPU *object pointer
 * @param[in]   node    VGPU_SYSMEM_PFN_BITMAP_NODE pointer
 * @param[in]   index   Index assigned to the node when it is added to linked list
 */
NV_STATUS vgpuAllocSysmemPfnBitMapNode(OBJGPU *pGpu, VGPU_SYSMEM_PFN_BITMAP_NODE_P *node, NvU32 index)
{
    NV_STATUS status = NV_OK;
    NvU64 pfn;
    NvU64 *dest;
    NvU32 i;
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    VGPU_GSP_SYSMEM_BITMAP_ROOT_NODE *sysmemBitmapRootNode = NULL;
    NvU32 memFlags = 0;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        memFlags = MEMDESC_FLAGS_CPU_ONLY;

    if (index != listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)) ||
        node == NULL) {
         status = NV_ERR_INVALID_ARGUMENT;
         NV_PRINTF(LEVEL_ERROR,
                   "Invalid argumets passed while allocating sysmem pfn bitmap node \n");
         return status;
    }

    *node = portMemAllocNonPaged(sizeof(VGPU_SYSMEM_PFN_BITMAP_NODE));
     if (*node == NULL) {
         status = NV_ERR_NO_MEMORY;
         NV_PRINTF(LEVEL_ERROR,
                   "failed to allocate memory for sysmem pfn bitmap node \n");
         return status;
     }

     portMemSet(*node, 0, sizeof(VGPU_SYSMEM_PFN_BITMAP_NODE));

     (*node)->pMemDesc_sysmemPfnMap = NULL;
     (*node)->sysmemPfnMap          = NULL;
     (*node)->sysmemPfnMap_priv     = NULL;
     (*node)->nodeStartPfn          = index * MAX_PFNS_PER_SYSMEM_BITMAP_NODE;
     (*node)->nodeEndPfn            = (index + 1) * MAX_PFNS_PER_SYSMEM_BITMAP_NODE;
     (*node)->index                 = index;

     if (pVGpu->bGspPlugin)
     {
         // Allocate an extra 4K memory to hold pfn data of level 2 table entries.
         (*node)->sizeInBytes           = (MAX_PFNS_PER_SYSMEM_BITMAP_NODE / 8) +
                                          RM_PAGE_SIZE;
     } else {
         (*node)->sizeInBytes           = MAX_PFNS_PER_SYSMEM_BITMAP_NODE / 8;
     }

     status = memdescCreate(&((*node)->pMemDesc_sysmemPfnMap), pGpu,
                            (*node)->sizeInBytes,
                            0,
                            NV_MEMORY_NONCONTIGUOUS,
                            ADDR_SYSMEM,
                            NV_MEMORY_CACHED,
                            memFlags);
     if (status != NV_OK)
     {
         NV_PRINTF(LEVEL_ERROR,
                   "Cannot alloc memory descriptor for sysmem pfn bitmap node (size = 0x%llx)\n",
                   (*node)->sizeInBytes);
         goto done;
     }

     memdescSetFlag((*node)->pMemDesc_sysmemPfnMap, MEMDESC_FLAGS_KERNEL_MODE, NV_TRUE);

     memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_88, 
                        (*node)->pMemDesc_sysmemPfnMap);
     if (status != NV_OK)
     {
         NV_PRINTF(LEVEL_ERROR, "Cannot alloc sysmem pfn bitmap node buffer\n");
         goto done;
     }

     status = memdescMapOld((*node)->pMemDesc_sysmemPfnMap,
                            0, // Offset
                            (*node)->sizeInBytes,
                            memdescGetFlag((*node)->pMemDesc_sysmemPfnMap, MEMDESC_FLAGS_KERNEL_MODE),
                            NV_PROTECT_READ_WRITE,
                            (void **)&((*node)->sysmemPfnMap),
                            (void **)&((*node)->sysmemPfnMap_priv));
     if (status != NV_OK)
     {
         NV_PRINTF(LEVEL_ERROR,
                   "Cannot map sysmem pfn bitmap node buffer (size = 0x%llx)\n",
                   (*node)->sizeInBytes);
         goto done;
     }

    if (pVGpu->bGspPlugin)
    {
        dest = (*node)->sysmemPfnMap + ((MAX_PFNS_PER_SYSMEM_BITMAP_NODE / 8) / sizeof(NvU64));
    }
    else
    {
        dest = vgpuSysmemPfnInfo.sysmemPfnRing;
    }

    for (i = 0; i < MAX_PFNS_PER_4K_PAGE; i++)
    {
        pfn = (memdescGetPte((*node)->pMemDesc_sysmemPfnMap, AT_GPU, i) >> (NvU32) RM_PAGE_SHIFT);
        // writing the pfns in the ring page to facilitate the plugin to access the sysmem pfn map
        dest[i] = pfn;
    }

    if (pVGpu->bGspPlugin)
    {
        sysmemBitmapRootNode = (VGPU_GSP_SYSMEM_BITMAP_ROOT_NODE *)vgpuSysmemPfnInfo.sysmemPfnRing;
        if (index < (MAX_PFNS_PER_4K_PAGE - 1))
        {
            sysmemBitmapRootNode->nodePfns[index] =
                    vgpuGspSysmemPfnMakeBufferAddress((*node)->pMemDesc_sysmemPfnMap,
                            (memdescGetPte((*node)->pMemDesc_sysmemPfnMap,
                                           AT_GPU, MAX_PFNS_PER_4K_PAGE) >> (NvU32) RM_PAGE_SHIFT));
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Exhausted limit of dirty sysmem tracking. Migration will not work correctly.\n");
            NV_ASSERT(0);
        }
        sysmemBitmapRootNode->nodeCount = index + 1;
    }

    // Add pointer to the node in bitmapNodes array at input index for faster lookups
    vgpuSysmemPfnInfo.bitmapNodes[index] = *node;

done:
    if (status != NV_OK)
    {
        vgpuFreeSysmemPfnBitMapNode(*node);
    }

    return status;
}

/*!
 * @brief Function to expand sysmem PFN bitmap list based on input pfn
 *
 * Each sysmem PFN bitmap node represets the bitmap of 64GB sysmem block.
 * Whenever we encouter a PFN which goes beyond the max tracked PFN, sysmem PFN
 * bitmap list is expanded to track the new PFN.
 * This function is responsible for adding the VGPU_SYSMEM_PFN_BITMAP_NODE nodes
 * to the list until input PFN can be tracked and also reallocating the refcount
 * buffer to accomodate the new sysmem PFN.
 *
 * @param[in]   pGpu    OBJGPU *object pointer
 * @param[in]   pfn     New sysmem PFN not being tracked
 */
static NV_STATUS vgpuExpandSysmemPfnBitMapList(OBJGPU *pGpu, NvU64 pfn)
{
    NV_STATUS status = NV_OK;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P node = NULL;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P nodeNext = NULL;
    NvU16 *temp_pfn_ref_count = NULL;
    NvU64 old_max_pfn = vgpuSysmemPfnInfo.guestMaxPfn;

    do
    {
        node = NULL;

        // Add a new bitmap node to track allocations in subsequent 64GB PA space.
        status = vgpuAllocSysmemPfnBitMapNode(pGpu, &node,
                                              listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to alloc sysmem pfn bitmap node\n");
            goto done;
        }

        listAppendExisting(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);

        vgpuSysmemPfnInfo.guestMaxPfn  = node->nodeEndPfn;
        vgpuSysmemPfnInfo.sizeInBytes  = vgpuSysmemPfnInfo.guestMaxPfn / 8;

    } while (vgpuSysmemPfnInfo.guestMaxPfn < pfn);

    // Alloc the ref count buffer
    temp_pfn_ref_count = portMemAllocNonPaged(sizeof(NvU16) * vgpuSysmemPfnInfo.guestMaxPfn);
    if (temp_pfn_ref_count == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to allocate sysmem pfn refcount array\n");
        NV_ASSERT(0);
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }
    portMemSet(temp_pfn_ref_count, 0, sizeof(NvU16) * vgpuSysmemPfnInfo.guestMaxPfn);

    // copy old ref count memory to new
    portMemCopy(NvP64_VALUE(temp_pfn_ref_count),
                (sizeof(NvU16) * vgpuSysmemPfnInfo.guestMaxPfn),
                NvP64_VALUE(vgpuSysmemPfnInfo.sysmemPfnRefCount),
                (sizeof(NvU16) * old_max_pfn));

    portMemFree(vgpuSysmemPfnInfo.sysmemPfnRefCount);
    vgpuSysmemPfnInfo.sysmemPfnRefCount = temp_pfn_ref_count;

done:
    if (status != NV_OK)
    {

        portMemFree(vgpuSysmemPfnInfo.sysmemPfnRefCount);

        for (node = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
             node != NULL;
             node = nodeNext)
        {
            nodeNext = listNext(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
            listRemove(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
            vgpuFreeSysmemPfnBitMapNode(node);
        }
    }

    return status;
}

/*!
 * @brief Helper function to update sysmem PFN bitmap based on input pfn
 *
 * Each sysmem PFN bitmap node represets the bitmap of 64GB sysmem block.
 * This function is responsible for identifying the the VGPU_SYSMEM_PFN_BITMAP_NODE node
 * based on input PFN and then set/clear the corresponding bit based on whether it is
 * allocation request or free.
 *
 * @param[in]   pfn     sysmem PFN corrsponding to which bitmap needs to be updated
 * @param[in]   bAlloc  to identify is bit correspond to PFN needs to be set/cleared
 */
static NV_STATUS _updateSysmemPfnBitMap
(
    NvU64 pfn,
    NvBool bAlloc
)
{
    NV_STATUS status = NV_OK;
    NvU32 mapIndex, bitIndex;
    NvU64 bits;
    NvU32 bitmapNodeIndex, nodeBitIndex;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P node = NULL;

    bitmapNodeIndex = pfn / MAX_PFNS_PER_SYSMEM_BITMAP_NODE;
    nodeBitIndex    = pfn % MAX_PFNS_PER_SYSMEM_BITMAP_NODE;

    if (bitmapNodeIndex < listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)))
    {
        node = vgpuSysmemPfnInfo.bitmapNodes[bitmapNodeIndex];
    }

    if (node)
    {
        mapIndex  = PAGE_MAPIDX64(nodeBitIndex);
        bitIndex  = PAGE_BITIDX64(nodeBitIndex);

        if (bAlloc)
        {
            bits = node->sysmemPfnMap[mapIndex];
            node->sysmemPfnMap[mapIndex] = SETBIT64(bits, bitIndex);
            vgpuSysmemPfnInfo.sysmemPfnRefCount[pfn] += 1;
        }
        else
        {
            NV_ASSERT(vgpuSysmemPfnInfo.sysmemPfnRefCount[pfn] > 0);

            if (vgpuSysmemPfnInfo.sysmemPfnRefCount[pfn] > 0)
            {
                vgpuSysmemPfnInfo.sysmemPfnRefCount[pfn] -= 1;

                if (vgpuSysmemPfnInfo.sysmemPfnRefCount[pfn] == 0)
                {
                    bits = node->sysmemPfnMap[mapIndex];
                    node->sysmemPfnMap[mapIndex] = CLEARBIT64(bits, bitIndex);
                }
            }
        }
    }
    else
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
    }

    return status;
}

/*!
 * @brief Function to update sysmem PFN bitmap based on input pfn
 *
 * Sysmem PFN bitmap is updated whenever there is sysmem allocation or free.
 * Whenever we encouter a PFN which goes beyond the max tracked PFN sysmem PFN
 * bitmap list is expanded to track the new PFN.
 * This function is responsible for updating the bitmap for the sysmem PFNs under
 * the input memory desriptor and expanding the bitmap list if we encouter a PFN
 * which goes beyond the max tracked PFN
 *
 * @param[in]   pGpu      OBJGPU *object pointer
 * @param[in]   pMemDesc  Memory descriptor corresponding to the sysmem
 * @param[in]   bAlloc    To identify if memory is being allocated or freed
 */
NV_STATUS vgpuUpdateSysmemPfnBitMap
(
    OBJGPU *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool bAlloc
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;
    NvU64 pfn;

    // Return early if sysmem dirty page tracking is disabled
    if (!(pGpu->getProperty(pGpu, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED)))
    {
        return NV_OK;
    }
    else
    {
        if (!vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized)
            return NV_OK;
    }

    NV_ASSERT_OR_RETURN(listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)) > 0,
                        NV_ERR_INVALID_OPERATION);
    NV_ASSERT_OR_RETURN((vgpuSysmemPfnInfo.sysmemPfnRefCount != NULL),
                        NV_ERR_INVALID_OPERATION);

    for (i = 0; i < pMemDesc->PageCount; i++)
    {
        pfn = (memdescGetPte(pMemDesc, AT_CPU, i) >> RM_PAGE_SHIFT);

        if (bAlloc)
        {
            if (pfn > vgpuSysmemPfnInfo.guestMaxPfn)
            {
                status = vgpuExpandSysmemPfnBitMapList(pGpu, pfn);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                            "Cannot re-allocate sysmem PFN bitmap :%x\n",
                            status);
                    return status;
                }
            }
        }

        status = _updateSysmemPfnBitMap(pfn, bAlloc);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Sysmem PFN bitmap update failed :%x\n", status);
            return status;
        }
    }

    return status;
}

/*
 * @brief Wrapper of vgpuUpdateSysmemPfnBitMap
 *
 * This wrapper abstracts away Guest and memory config checks which
 * otherwise get duplicated at the callsites in RM.
 *
 * @param[in]   pGpu      OBJGPU *object pointer
 * @param[in]   pMemDesc  Memory descriptor corresponding to the sysmem
 * @param[in]   bAlloc    To identify if memory is being allocated or freed
 */
NV_STATUS vgpuUpdateGuestSysmemPfnBitMap
(
    OBJGPU *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool bAlloc
)
{
    if ((pGpu != NULL) && IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) &&
        (pMemDesc->_addressSpace == ADDR_SYSMEM) && !(pMemDesc->_flags & MEMDESC_FLAGS_CPU_ONLY))
    {
        return vgpuUpdateSysmemPfnBitMap(pGpu, pMemDesc, bAlloc);
    }

    return NV_OK;
}

