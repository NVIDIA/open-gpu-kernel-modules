/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*         Virtual Address Space Function Definitions.                       *
\***************************************************************************/


#include "mem_mgr/vaspace.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmapi/rs_utils.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/vaspace_api.h"

void
vaspaceIncRefCnt_IMPL(OBJVASPACE *pVAS)
{
    pVAS->refCnt++;
}

void
vaspaceDecRefCnt_IMPL(OBJVASPACE *pVAS)
{
    NV_ASSERT_OR_RETURN_VOID(pVAS->refCnt != 0);
    pVAS->refCnt--;
}

NV_STATUS
vaspaceFillAllocParams_IMPL
(
    OBJVASPACE      *pVAS,
    const FB_ALLOC_INFO *pAllocInfo,
    NvU64           *pSize,
    NvU64           *pAlign,
    NvU64           *pRangeLo,
    NvU64           *pRangeHi,
    NvU64           *pPageSizeLockMask,
    VAS_ALLOC_FLAGS *pFlags
)
{
    NvBool bRestrictedVaRange = NV_FALSE;
    NvBool bEnforce32bitPtr   = NV_FALSE;
    NvU32 vasFlags            =  vaspaceGetFlags(pVAS);

    OBJGPU             *pGpu     = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    VirtMemAllocator   *pDma     = GPU_GET_DMA(pGpu);
    bRestrictedVaRange           = pDma->bDmaRestrictVaRange;
    bEnforce32bitPtr             = !!(pDma->getProperty(pDma, PDB_PROP_DMA_ENFORCE_32BIT_POINTER));

    // Apply default alignment policies to offset alignment and size.
    NV_ASSERT_OK_OR_RETURN(
        vaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize,
                                     pPageSizeLockMask));

    pFlags->bClientAllocation = !!(pAllocInfo->internalflags & NVOS32_ALLOC_INTERNAL_FLAGS_CLIENTALLOC);

    if (pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        // Fixed address allocation implemented by restricting range.
        *pRangeLo = pAllocInfo->offset;
        *pRangeHi = pAllocInfo->offset + *pSize - 1;
    }
    else if (!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END))
    {
        // If user didn't specify fixed or restricted range, allow full VAS range.
        *pRangeLo = vaspaceGetVaStart(pVAS);
        *pRangeHi = vaspaceGetVaLimit(pVAS);

        //
        // For MODS we also allow restricting the range to 40 bits by default.
        // This is needed for Pascal 49b support where some HW units can only
        // access 40b VA. MODS must use range/fixed address allocations to
        // get a VA above 40 bits in this mode.
        //
        if (bRestrictedVaRange && !(vasFlags & VASPACE_FLAGS_FLA))
        {
            *pRangeHi = NV_MIN(*pRangeHi, NVBIT64(40) - 1);
        }
    }

    //
    // Handle 32bit pointer requests.  32b pointers are forced below 32b
    // on all chips.  Non-32b requests are only forced on some chips,
    // typically kepler, and only if there are no other address hints.
    //
    // If requested size cannot be satisfied with range above 4 GB, then relax that
    // restriction.
    //
    if (FLD_TEST_DRF(OS32, _ATTR2, _32BIT_POINTER, _ENABLE, pAllocInfo->pageFormat->attr2))
    {
        *pRangeHi = NV_MIN(*pRangeHi, NVBIT64(32) - 1);
    }

    else if (bEnforce32bitPtr &&
             !(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE) &&
             !(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END) &&
             ((*pRangeHi - *pRangeLo + 1 - *pSize) > NVBIT64(32)) &&
             !(vasFlags & VASPACE_FLAGS_FLA))
    {
        *pRangeLo = NV_MAX(*pRangeLo, NVBIT64(32));
    }

    if ((*pRangeHi - *pRangeLo + 1) < *pSize) // Moved the range check here
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Requested size 0x%llx more than available range. RangeLo=0x%llx, RangeHi=0x%llx\n",
                  *pSize, *pRangeLo, *pRangeHi);
        NV_ASSERT_OR_RETURN(0,  NV_ERR_INSUFFICIENT_RESOURCES);
    }

    // Convert flags.
    pFlags->bReverse =
        !!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN);

    pFlags->bPreferSysmemPageTables =
        !!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_PREFER_PTES_IN_SYSMEMORY);

    pFlags->bExternallyManaged =
        !!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED);

    pFlags->bLazy =
        !!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_LAZY);

    pFlags->bSparse =
        !!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_SPARSE);

    //
    // The protected flag for kernel allocations is honoured only
    // if this is a root client(kernel client).
    //
    pFlags->bPrivileged = pAllocInfo->bIsKernelAlloc;

    return NV_OK;
}

NvU64
vaspaceGetVaStart_IMPL(OBJVASPACE *pVAS)
{
    return pVAS->vasStart;
}

NvU64
vaspaceGetVaLimit_IMPL(OBJVASPACE *pVAS)
{
    return pVAS->vasLimit;
}

void
vaspaceInvalidateTlb_IMPL
(
    OBJVASPACE          *pVAS,
    OBJGPU              *pGpu,
    VAS_PTE_UPDATE_TYPE  type
)
{
    NV_ASSERT(0);
}

NV_STATUS
vaspaceGetByHandleOrDeviceDefault_IMPL
(
    RsClient    *pClient,
    NvHandle     hDeviceOrSubDevice,
    NvHandle     hVASpace,
    OBJVASPACE **ppVAS
)
{
    NV_STATUS      status = NV_OK;
    NvHandle       _hDeviceOrSubDevice;
    Device        *pDevice    = NULL;
    RsResourceRef *pResourceRef;

    if (hVASpace == NV01_NULL_OBJECT)
    {
        if (hDeviceOrSubDevice == 0)
        {
            return NV_ERR_INVALID_OBJECT_HANDLE;
        }
        _hDeviceOrSubDevice = hDeviceOrSubDevice;
    }
    else
    {
        status = serverutilGetResourceRefWithType(pClient->hClient, hVASpace,
                                                  classId(VaSpaceApi), &pResourceRef);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid object handle 0x%x\n", hVASpace);
            return status;
        }
        _hDeviceOrSubDevice = pResourceRef->pParentRef ? pResourceRef->pParentRef->hResource : 0;
    }

    status = deviceGetByHandle(pClient, _hDeviceOrSubDevice, &pDevice);
    if (status != NV_OK)
    {
        Subdevice *pSubdevice;

        status = subdeviceGetByHandle(pClient, _hDeviceOrSubDevice, &pSubdevice);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid parent handle!\n");
            return status;
        }

        pDevice = pSubdevice->pDevice;
    }

    // Allocates/Finds VA Space according to the handle type.
    if (hVASpace == NV01_NULL_OBJECT)
    {
        // Check the vaspace mode
        if (pDevice->vaMode == NV_DEVICE_ALLOCATION_VAMODE_MULTIPLE_VASPACES)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "VA mode %d (PRIVATE) doesn't support allocating an implicit VA space.\n",
                      pDevice->vaMode);
            return NV_ERR_INVALID_STATE;
        }
        return deviceGetDefaultVASpace(pDevice, ppVAS);
    }
    else
    {
         VaSpaceApi       *pVaSpaceApi = NULL;
        // Check the vaspace mode
        if (pDevice->vaMode == NV_DEVICE_ALLOCATION_VAMODE_SINGLE_VASPACE)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "VA mode %d (GLOBAL) doesn't support allocating private VA spaces.\n",
                      pDevice->vaMode);
            return NV_ERR_INVALID_STATE;
        }
        pVaSpaceApi = dynamicCast(pResourceRef->pResource, VaSpaceApi);
        *ppVAS = pVaSpaceApi->pVASpace;
    }

    return NV_OK;
}

