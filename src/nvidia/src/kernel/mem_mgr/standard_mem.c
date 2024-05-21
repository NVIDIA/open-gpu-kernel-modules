/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr_internal.h"
#include "mem_mgr/standard_mem.h"
#include "vgpu/rpc.h"
#include "rmapi/client.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "resserv/rs_server.h"
#include "rmapi/rs_utils.h"
#include "gpu/mem_mgr/heap.h"

NV_STATUS stdmemValidateParams
(
    OBJGPU                      *pGpu,
    NvHandle                     hClient,
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData
)
{
    NvBool         bIso;
    RS_PRIV_LEVEL  privLevel;
    CALL_CONTEXT  *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM || RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
    privLevel = pCallContext->secInfo.privLevel;

    //
    // Make sure UMD does not impact the internal allocation flags
    // Do this check right after copy in. RM is free to set these flags later
    //
    if ((privLevel < RS_PRIV_LEVEL_KERNEL) &&
        (pAllocData->internalflags != 0))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // These flags don't do anything in this path. No mapping on alloc and
    // kernel map is controlled by TYPE
    //
    pAllocData->flags |= NVOS32_ALLOC_FLAGS_MAP_NOT_REQUIRED;
    pAllocData->flags &= ~NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP;

    pAllocData->address = NvP64_NULL;

    //
    // Reject any API calls that pass an invalid owner.
    // Reject any client calls that try to mess with internal RM memory.
    //
    if ((pAllocData->owner == 0) ||
        (pAllocData->owner == 0xFFFFFFFF) ||
        ((pAllocData->owner >= HEAP_OWNER_RM_SCRATCH_BEGIN) &&
         (pAllocData->owner <= HEAP_OWNER_RM_SCRATCH_END)))
    {
        return NV_ERR_INVALID_OWNER;
    }

    bIso = (pAllocData->type == NVOS32_TYPE_PRIMARY) ||
           (pAllocData->type == NVOS32_TYPE_VIDEO) ||
           (pAllocData->type == NVOS32_TYPE_CURSOR);

    //
    // MM-TODO: If surface requires ISO guarantees, ensure it's of the proper
    // NVOS32_TYPE. Eventually, we should decouple NVOS32_TYPE from conveying
    // ISO behavior; RM needs to audit NVOS32_TYPE uses wrt ISO determination.
    //
    if (!bIso && FLD_TEST_DRF(OS32, _ATTR2, _ISO, _YES, pAllocData->attr2))
    {
        NV_PRINTF(LEVEL_INFO, "type is non-ISO but attributes request ISO!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // check PAGE_OFFLINING flag for client
    // If the client is not a ROOT client, then turning PAGE_OFFLINIG OFF is invalid
    //
    if (FLD_TEST_DRF(OS32, _ATTR2, _PAGE_OFFLINING, _OFF, pAllocData->attr2))
    {
        if (hypervisorIsVgxHyper())
        {
            if (!(rmclientIsAdminByHandle(hClient, privLevel) || hypervisorCheckForObjectAccess(hClient)))
            {
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }
        else
        {
            // if the client requesting is not kernel mode, return early
#if defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES)
            if (!rmclientIsAdminByHandle(hClient, privLevel))
#else
            if (privLevel < RS_PRIV_LEVEL_KERNEL)
#endif
            {
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }
    }

    //
    // If NVOS32_TYPE indicates ISO requirements, set
    // NVOS32_ATTR2_NISO_DISPLAY_YES so it can be used within RM instead of
    // NVOS32_TYPE for ISO determination.
    //
    if (bIso)
    {
        pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _ISO, _YES,
                                        pAllocData->attr2);
    }

    if (!(pAllocData->flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END))
    {
        NV_ASSERT_OR_RETURN((pAllocData->rangeLo == 0) &&
                        (pAllocData->rangeHi == 0), NV_ERR_INVALID_ARGUMENT);
    }
    NV_PRINTF(LEVEL_INFO, "MMU_PROFILER Attr 0x%x Type 0x%x Attr2 0x%x\n",
              pAllocData->attr, pAllocData->type, pAllocData->attr2);

    // Make sure that encryption is supported if it is requested
    if ((pAllocData->flags & NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED) &&
        DRF_VAL(OS32, _ATTR, _LOCATION, pAllocData->attr) == NVOS32_ATTR_LOCATION_VIDMEM)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Encryption requested for video memory on a non-0FB chip;\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (FLD_TEST_DRF(OS32, _ATTR2, _ALLOCATE_FROM_SUBHEAP, _YES, pAllocData->attr2))
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pAllocData->attr),
                           NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // When a sparsified VA range is requested by the client, RM constructs
    // the page tables during the VirtMem construct call causing the lazy
    // flag to skip memory reservation. This can cause RM to OOM if the 
    // memPool reserved memory is exhausted.
    //
    if ((pAllocData->flags & NVOS32_ALLOC_FLAGS_SPARSE) && 
        (pAllocData->flags & NVOS32_ALLOC_FLAGS_VIRTUAL))
    {
        pAllocData->flags = pAllocData->flags &~ NVOS32_ALLOC_FLAGS_LAZY;
    }

    return NV_OK;
}

void stdmemDumpInputAllocParams
(
    NV_MEMORY_ALLOCATION_PARAMS  *pAllocData,
    CALL_CONTEXT                 *pCallContext
)
{
    NV_PRINTF(LEVEL_INFO, "stdmemConstruct input\n");
    NV_PRINTF(LEVEL_INFO, "          Owner: 0x%x\n", pAllocData->owner);
    NV_PRINTF(LEVEL_INFO, "        hMemory: 0x%x\n", pCallContext->pResourceRef->hResource);
    NV_PRINTF(LEVEL_INFO, "           Type: 0x%x\n", pAllocData->type);
    NV_PRINTF(LEVEL_INFO, "          Flags: 0x%x\n", pAllocData->flags);
    NV_PRINTF(LEVEL_INFO, "          Begin: 0x%08llx\n", pAllocData->rangeLo);
    NV_PRINTF(LEVEL_INFO, "            End: 0x%08llx\n", pAllocData->rangeHi);
    NV_PRINTF(LEVEL_INFO, "         Height: 0x%x\n", pAllocData->height);
    NV_PRINTF(LEVEL_INFO, "          Width: 0x%x\n", pAllocData->width);
    NV_PRINTF(LEVEL_INFO, "          Pitch: 0x%x\n", pAllocData->pitch);
    NV_PRINTF(LEVEL_INFO, "           Size: 0x%08llx\n", pAllocData->size);
    NV_PRINTF(LEVEL_INFO, "      Alignment: 0x%08llx\n", pAllocData->alignment);
    NV_PRINTF(LEVEL_INFO, "         Offset: 0x%08llx\n", pAllocData->offset);
    NV_PRINTF(LEVEL_INFO, "           Attr: 0x%x\n", pAllocData->attr);
    NV_PRINTF(LEVEL_INFO, "          Attr2: 0x%x\n", pAllocData->attr2);
    NV_PRINTF(LEVEL_INFO, "         Format: 0x%x\n", pAllocData->format);
    NV_PRINTF(LEVEL_INFO, "      ComprCovg: 0x%x\n", pAllocData->comprCovg);
    NV_PRINTF(LEVEL_INFO, "      ZCullCovg: 0x%x\n", pAllocData->zcullCovg);
    NV_PRINTF(LEVEL_INFO, "     CtagOffset: 0x%x\n", pAllocData->ctagOffset);
    NV_PRINTF(LEVEL_INFO, "       hVASpace: 0x%x\n", pAllocData->hVASpace);
    NV_PRINTF(LEVEL_INFO, "            tag: 0x%x\n", pAllocData->tag);
}

void stdmemDumpOutputAllocParams
(
    NV_MEMORY_ALLOCATION_PARAMS  *pAllocData
)
{
    NV_PRINTF(LEVEL_INFO, "stdmemConstruct output\n");
    NV_PRINTF(LEVEL_INFO, "         Height: 0x%x\n", pAllocData->height);
    NV_PRINTF(LEVEL_INFO, "          Width: 0x%x\n", pAllocData->width);
    NV_PRINTF(LEVEL_INFO, "          Pitch: 0x%x\n", pAllocData->pitch);
    NV_PRINTF(LEVEL_INFO, "           Size: 0x%08llx\n", pAllocData->size);
    NV_PRINTF(LEVEL_INFO, "      Alignment: 0x%08llx\n", pAllocData->alignment);
    NV_PRINTF(LEVEL_INFO, "         Offset: 0x%08llx\n", pAllocData->offset);
    NV_PRINTF(LEVEL_INFO, "           Attr: 0x%x\n", pAllocData->attr);
    NV_PRINTF(LEVEL_INFO, "          Attr2: 0x%x\n", pAllocData->attr2);
    NV_PRINTF(LEVEL_INFO, "         Format: 0x%x\n", pAllocData->format);
    NV_PRINTF(LEVEL_INFO, "      ComprCovg: 0x%x\n", pAllocData->comprCovg);
    NV_PRINTF(LEVEL_INFO, "      ZCullCovg: 0x%x\n", pAllocData->zcullCovg);
}

NV_STATUS
stdmemConstruct_IMPL
(
    StandardMemory               *pStandardMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}


NvBool stdmemCanCopy_IMPL(StandardMemory *pStandardMemory)
{
    return NV_TRUE;
}

/*!
 * stdmemQueryPageSize
 *
 * @brief
 *     Returns page size requested by client.
 *
 * @param[in] pMemoryManager MemoryManager pointer
 * @param[in] hClient        Client handle.
 * @param[in] pAllocData     Pointer to VIDHEAP_ALLOC_DATA
 *
 * @returns
 *      The page size in bytes.
 */
NvU64
stdmemQueryPageSize
(
    MemoryManager               *pMemoryManager,
    NvHandle                     hClient,
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData
)
{
    NvU32 retAttr  = pAllocData->attr;
    NvU32 retAttr2 = pAllocData->attr2;

    return memmgrDeterminePageSize(pMemoryManager, hClient, pAllocData->size,
                                   pAllocData->format, pAllocData->flags, &retAttr, &retAttr2);
}

//
// Control calls for system memory objects maintained outside the heap.
//

NvU64 stdmemGetSysmemPageSize_IMPL(OBJGPU * pGpu, StandardMemory *pStdMemory)
{
    return GPU_GET_MEMORY_MANAGER(pGpu)->sysmemPageSize;
}
