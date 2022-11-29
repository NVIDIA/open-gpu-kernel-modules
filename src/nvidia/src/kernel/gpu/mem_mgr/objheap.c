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
/***************************** Heap Routines *******************************\
*         Heap object function definitions.                                 *
\***************************************************************************/

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"              // To move to MIT license
#include "nvRmReg.h"
#include "os/os.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "rmapi/rmapi_utils.h"
#include "vgpu/rpc.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"

NV_STATUS
heapInit_IMPL(OBJGPU *pGpu, Heap *pHeap, NvU64 base, NvU64 size, HEAP_TYPE_INTERNAL heapType, NvU32 gfid, void *pPtr)
{
    NV_STATUS   status;

    status = memmgrGetBlackListPagesForHeap_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pHeap);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Failed to read blackList pages (0x%x).\n",
                  status);
    }

    status = heapInitRegistryOverrides(pGpu, pHeap);

    if (NV_OK != status)
    {
        // Warn and continue
        NV_PRINTF(LEVEL_ERROR, "Error 0x%x reading registry\n", status);
    }

    //
    // Not all offlined addresses belong to this partition
    // Filter out and keep only the ones that fall in this partition's memory
    //
    if (heapType == HEAP_TYPE_PARTITION_LOCAL ||
        (heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR && hypervisorIsVgxHyper()))
    {
        if (IS_GFID_VF(gfid)) {

            RM_API     *pRmApi  = GPU_GET_PHYSICAL_RMAPI(pGpu);
            NvU32       blIndex = 0;
            RmPhysAddr  guestFbOffsetSpa;
            NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams;

            pParams = portMemAllocStackOrHeap(sizeof(*pParams));
            if (pParams == NULL)
            {
                return NV_ERR_NO_MEMORY;
            }
            portMemSet(pParams, 0, sizeof(*pParams));

            pParams->gfid          = gfid;
            pParams->numEntries    = 1;
            pParams->gpaEntries[0] = 0;  // translate gpa 0 (assumes contig)

            status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                     NV2080_CTRL_CMD_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES,
                                     pParams, sizeof(*pParams));
            guestFbOffsetSpa = pParams->spaEntries[0];
            portMemFreeStackOrHeap(pParams);
            pParams = NULL;

            if (status != NV_OK)
            {
                return status;
            }

            heapFilterBlackListPages(pHeap, guestFbOffsetSpa + base, size);

            // Convert the offlined page offsets from SPA -> GPA
            for (blIndex = 0; blIndex < pHeap->blackListAddresses.count; blIndex++)
            {
                pHeap->blackListAddresses.data[blIndex].address -= guestFbOffsetSpa;
            }
        }
        else {
            heapFilterBlackListPages(pHeap, base, size);
        }
    }

    status = heapInitInternal(pGpu, pHeap, base, size, heapType, pPtr);

    if (NV_OK != status)
    {
        return status;
    }

    return status;
}

//
// Initialize the heap from any possible registry overrides
//
NV_STATUS
heapInitRegistryOverrides_IMPL
(
    OBJGPU *pGpu,
    Heap   *pHeap
)
{

    return NV_OK;
}

//
// Not all offlined addresses belong to a range
// Filter out and keep only the ones that fall in the range
//
void
heapFilterBlackListPages_IMPL
(
    Heap *pHeap,
    NvU64 base,
    NvU64 size
)
{
    NvU32 blIndex   = 0;
    NvU32 blCount   = 0;
    NvU64 blAddress = 0;
    NvU32 blType;
    NvU32 unusedEntries;

    if (!pHeap || !pHeap->blackListAddresses.data)
        return;

    for (blIndex = 0; blIndex < pHeap->blackListAddresses.count; blIndex++)
    {
        blAddress = pHeap->blackListAddresses.data[blIndex].address;
        blType    = pHeap->blackListAddresses.data[blIndex].type;
        if (blAddress >= base && blAddress < (base + size))
        {
            pHeap->blackListAddresses.data[blCount].address = blAddress;
            pHeap->blackListAddresses.data[blCount].type    = blType;
            blCount++;
        }
    }

    if (blCount != 0)
    {
        // clear out all other entries
        unusedEntries = pHeap->blackListAddresses.count - blCount;
        portMemSet(&pHeap->blackListAddresses.data[blCount], 0,
                   sizeof(*(pHeap->blackListAddresses.data)) * unusedEntries);

        pHeap->blackListAddresses.count = blCount;
    }
}
