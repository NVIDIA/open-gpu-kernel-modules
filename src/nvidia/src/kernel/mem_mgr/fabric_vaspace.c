/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** HW State Rotuines ***************************\
*                                                                           *
*         Fabric Virtual Address Space Function Definitions.                *
*                                                                           *
\***************************************************************************/

#include "gpu/mmu/kern_gmmu.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "mmu/mmu_walk.h"
#include "lib/base_utils.h"
#include "class/cl90f1.h"    // FERMI_VASPACE_A
#include "class/cl00fc.h"    // FABRIC_VASPACE_A
#include "class/cl0040.h"    // NV01_MEMORY_LOCAL_USER
#include "class/cl0080.h"    // NV01_DEVICE_0
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "deprecated/rmapi_deprecated.h"
#include "rmapi/rs_utils.h"
#include "vgpu/vgpu_events.h"
#include "mem_mgr/virt_mem_mgr.h"



//
// TODO: To be removed when legacy FLA VAS (pKernelBus->flaInfo.pFlaVAS) is removed"
// The instance block is setup during kbusAllocateFlaVaspace_HAL(). However, we
// lazily bind it to the new fabric VAS when the very first NV_FABRIC_MEMORY
// allocations happens.
//
static NV_STATUS
_fabricvaspaceBindInstBlk
(
    FABRIC_VASPACE *pFabricVAS
)
{
    OBJVASPACE *pVAS   = staticCast(pFabricVAS, OBJVASPACE);
    OBJGPU     *pGpu   = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelBus  *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_STATUS   status = NV_OK;

    INST_BLK_INIT_PARAMS instblkParams;

    if (gvaspaceIsInUse(dynamicCast(pKernelBus->flaInfo.pFlaVAS, OBJGVASPACE)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FabricVAS and FlaVAS cannot be used simultaneously! "
                  "Instance block setup for fabricVAS failed\n");
        return NV_ERR_INVALID_OPERATION;
    }

    //
    // Check if this is the first fabric vaspace allocation. If this is not the
    // first allocation, instance block is already setup. Return NV_OK.
    //
    if (gvaspaceIsInUse(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE)))
    {
        return NV_OK;
    }

    // Unbind the instance block for FLA vaspace.
    status = kbusSetupUnbindFla_HAL(pGpu, pKernelBus);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to unbind instance block for FlaVAS, status=0x%x\n",
                  status);
        return status;
    }

    // Instantiate the instance block for fabric vaspace.
    portMemSet(&instblkParams, 0, sizeof(instblkParams));
    status = kgmmuInstBlkInit(pKernelGmmu, pKernelBus->flaInfo.pInstblkMemDesc,
                             pFabricVAS->pGVAS, FIFO_PDB_IDX_BASE,
                             &instblkParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to setup instance block for fabricVAS, status=0x%x\n",
                  status);
        goto failed;
    }

    // Bind the instance block for fabric vaspace.
    status = kbusSetupBindFla_HAL(pGpu, pKernelBus,  pFabricVAS->gfid);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to bind instance block for fabricVAS, status=0x%x\n",
                  status);
        goto failed;
    }

    return NV_OK;

failed:
    // Instantiate the instance block for FLA vaspace.
    portMemSet(&instblkParams, 0, sizeof(instblkParams));
    NV_ASSERT(kgmmuInstBlkInit(pKernelGmmu, pKernelBus->flaInfo.pInstblkMemDesc,
                              pKernelBus->flaInfo.pFlaVAS, FIFO_PDB_IDX_BASE,
                              &instblkParams) == NV_OK);

    // Bind the instance block for FLA vaspace.
    NV_ASSERT(kbusSetupBindFla_HAL(pGpu, pKernelBus,  pFabricVAS->gfid) == NV_OK);

    return status;
}

//
// TODO: To be removed when legacy FLA VAS (pKernelBus->flaInfo.pFlaVAS) is removed"
// The instance block is unbind during kbusDestroyFla_HAL(). However, we unbind
// it here and bind back the instance block for the legacy FLA VAS after the
// last NV_FABRIC_MEMORY allocation is freed.
//
static void
_fabricvaspaceUnbindInstBlk
(
    FABRIC_VASPACE *pFabricVAS
)
{
    OBJVASPACE *pVAS  = staticCast(pFabricVAS, OBJVASPACE);
    OBJGPU     *pGpu  = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelBus  *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    INST_BLK_INIT_PARAMS instblkParams = {0};

    //
    // Check if there are any pending allocations for the fabric vaspace.
    // If there are pending allocations, skip restore and return NV_OK.
    //
    if (gvaspaceIsInUse(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE)))
    {
        return;
    }

    // Unbind the instance block for fabric vaspace.
    NV_ASSERT(kbusSetupUnbindFla_HAL(pGpu, pKernelBus) == NV_OK);

    // Instantiate the instance block for FLA vaspace.
    NV_ASSERT(kgmmuInstBlkInit(pKernelGmmu, pKernelBus->flaInfo.pInstblkMemDesc,
                               pKernelBus->flaInfo.pFlaVAS, FIFO_PDB_IDX_BASE,
                               &instblkParams) == NV_OK);

    // Bind the instance block for FLA vaspace.
    NV_ASSERT(kbusSetupBindFla_HAL(pGpu, pKernelBus, pFabricVAS->gfid) == NV_OK);
}

NV_STATUS
fabricvaspaceConstruct__IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU32           classId,
    NvU32           vaspaceId,
    NvU64           vaStart,
    NvU64           vaLimit,
    NvU64           vaStartInternal,
    NvU64           vaLimitInternal,
    NvU32           flags
)
{
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    OBJSYS     *pSys   = SYS_GET_INSTANCE();
    OBJVMM     *pVmm   = SYS_GET_VMM(pSys);
    OBJVASPACE *pVAS   = staticCast(pFabricVAS, OBJVASPACE);
    OBJGPU     *pGpu   = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    NV_STATUS   status = NV_OK;
    NvHandle    hClient = 0;
    NvHandle    hDevice = 0;
    NV0080_ALLOC_PARAMETERS devAllocParams = { 0 };
    NvU32       gfid    = 0;

    // Sanity check input parameters.
    NV_ASSERT_OR_RETURN(FABRIC_VASPACE_A == classId, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaStart <= vaLimit,          NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ONEBITSET(pVAS->gpuMask),    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaspaceId == pGpu->gpuId,    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    status = pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT,
                                     NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                     NV01_ROOT, &hClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating client, status=0x%x\n", status);
        return status;
    }

    status = serverutilGenResourceHandle(hClient, &hDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating device handle, status=0x%x\n", status);
        goto cleanup;
    }

    // Allocate a device handle
    devAllocParams.deviceId = gpuGetDeviceInstance(pGpu);
    status = pRmApi->AllocWithHandle(pRmApi, hClient, hClient, hDevice,
                                     NV01_DEVICE_0, &devAllocParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating device, status=0x%x\n", status);
        goto cleanup;
    }

    // Save off flags.
    pFabricVAS->flags = (flags |
                         VASPACE_FLAGS_ALLOW_ZERO_ADDRESS |
                         VASPACE_FLAGS_INVALIDATE_SCOPE_NVLINK_TLB |
                         VASPACE_FLAGS_DISABLE_SPLIT_VAS);

    if (IS_GFID_VF(gfid))
    {
        pFabricVAS->gfid = gfid;
        pFabricVAS->flags |= VASPACE_FLAGS_ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR;
    }

    pFabricVAS->bRpcAlloc = IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

    // Create the GVASPACE object associated with this fabric vaspace.
    status = vmmCreateVaspace(pVmm, FERMI_VASPACE_A, 0, pVAS->gpuMask,
                              vaStart, vaLimit, 0, 0, NULL, pFabricVAS->flags,
                              &pFabricVAS->pGVAS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed allocating gvaspace associated with the fabric vaspace, "
                  "status=0x%x\n", status);
        goto cleanup;
    }

    pFabricVAS->hClient = hClient;
    pFabricVAS->hDevice = hDevice;

    // Capture the vasStart and vasLimit for the fabric vaspace.
    pVAS->vasStart = pFabricVAS->pGVAS->vasStart;
    pVAS->vasLimit = pFabricVAS->pGVAS->vasLimit;

    return NV_OK;

cleanup:
    NV_ASSERT(pRmApi->Free(pRmApi, hClient, hClient) == NV_OK);

    return status;
}

void
fabricvaspaceDestruct_IMPL
(
    FABRIC_VASPACE *pFabricVAS
)
{
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJVMM     *pVmm = SYS_GET_VMM(pSys);
    OBJVASPACE *pVAS = staticCast(pFabricVAS, OBJVASPACE);

    if (pFabricVAS->pGVAS == NULL)
        return;

    NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                           pFabricVAS->hClient) == NV_OK);

    // There should be no vaspace allocations pending at this point.
    NV_ASSERT(!gvaspaceIsInUse(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE)));

    // Destroy the GVASPACE object associated with this fabric vaspace.
    vmmDestroyVaspace(pVmm, pFabricVAS->pGVAS);

    pFabricVAS->pGVAS = NULL;
    pVAS->vasStart    = 0;
    pVAS->vasLimit    = 0;
}

NV_STATUS
fabricvaspaceAlloc_IMPL
(
    FABRIC_VASPACE  *pFabricVAS,
    NvU64            size,
    NvU64            align,
    NvU64            rangeLo,
    NvU64            rangeHi,
    NvU64            pageSize,
    VAS_ALLOC_FLAGS  flags,
    NvU64           *pAddr
)
{
    NV_STATUS status = NV_OK;

    // Sanity check the input parameters.
    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL,     NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT_OR_RETURN(pAddr != NULL,                 NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pageSize >= RM_PAGE_SIZE_HUGE, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(align != 0,                    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0,                     NV_ERR_INVALID_ARGUMENT);

    // Check the alignment and size are pageSize aligned
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(align, pageSize), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(size, pageSize),  NV_ERR_INVALID_ARGUMENT);

    status = _fabricvaspaceBindInstBlk(pFabricVAS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to bind instance block for fabric vaspace."
                  " Alloc failed\n");
        return status;
    }

    // Adjust rangeLo and rangeHi
    rangeLo = NV_ALIGN_DOWN(rangeLo, pageSize);
    rangeHi = NV_ALIGN_UP(rangeHi, pageSize);

    //
    // Allocate VA space of the size and alignment requested.
    // RM_PAGE_SIZE_HUGE is passed since FLA->PA page size is 2MB.
    //
    status = vaspaceAlloc(pFabricVAS->pGVAS, size, align, rangeLo, rangeHi,
                          RM_PAGE_SIZE_HUGE, flags, pAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate vaspace\n");
        goto failed;
    }

    // Assert that the address returned is pageSize aligned
    NV_ASSERT(NV_IS_ALIGNED64(*pAddr, pageSize));

    return NV_OK;

failed:

    _fabricvaspaceUnbindInstBlk(pFabricVAS);

    return status;
}

NV_STATUS
fabricvaspaceAllocNonContiguous_IMPL
(
    FABRIC_VASPACE   *pFabricVAS,
    NvU64             size,
    NvU64             align,
    NvU64             rangeLo,
    NvU64             rangeHi,
    NvU64             pageSize,
    VAS_ALLOC_FLAGS   flags,
    NvU64           **ppAddr,
    NvU32            *pNumAddr
)
{
    NV_STATUS status    = NV_OK;
    NvU64     freeSize  = 0;
    NvU32     pageCount = (size / pageSize);
    NvU64     addr;
    NvU32     idx;
    NvBool    bDefaultAllocMode;

    // Sanity check the input parameters.
    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL,     NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT_OR_RETURN(ppAddr != NULL,                NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pNumAddr != NULL,              NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pageSize >= RM_PAGE_SIZE_HUGE, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(align != 0,                    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0,                     NV_ERR_INVALID_ARGUMENT);

    // Check the alignment and size are pageSize aligned.
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(align, pageSize), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(size, pageSize),  NV_ERR_INVALID_ARGUMENT);

    // Check if heap can satisfy the request.
    NV_ASSERT_OK_OR_RETURN(fabricvaspaceGetFreeHeap(pFabricVAS, &freeSize));
    if (freeSize < size)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Not enough memory in eheap, size requested = 0x%llx, "
                  "free memory = 0x%llx\n",
                  size, freeSize);
        return NV_ERR_NO_MEMORY;
    }

    if (flags.bForceNonContig && flags.bForceContig)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Forcing both contiguous and noncontiguous is not allowed\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    bDefaultAllocMode = (!flags.bForceNonContig && !flags.bForceContig);

    // Adjust rangeLo and rangeHi.
    rangeLo = NV_ALIGN_DOWN(rangeLo, pageSize);
    rangeHi = NV_ALIGN_UP(rangeHi, pageSize);

    *ppAddr = portMemAllocNonPaged(sizeof(NvU64) * pageCount);
    if (*ppAddr == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(*ppAddr, 0, sizeof(NvU64) * pageCount);

    status = _fabricvaspaceBindInstBlk(pFabricVAS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to bind instance block for fabric vaspace."
                  " Alloc failed\n");
        goto failed;
    }

    // Initialize number of addresses to 0
    *pNumAddr = 0;

    //
    // Attempt to allocate VA space of the size and alignment requested.
    // RM_PAGE_SIZE_HUGE is passed since FLA->PA page size is 2MB.
    //
    if (flags.bForceContig || bDefaultAllocMode)
    {
        status = vaspaceAlloc(pFabricVAS->pGVAS, size, align, rangeLo, rangeHi,
                              RM_PAGE_SIZE_HUGE, flags, &addr);
        if (status == NV_OK)
        {
            (*ppAddr)[0] = addr;
            *pNumAddr    = 1;
        }
    }

    //
    // If size could not be allocated in one memblock, break size into
    // multiple pageSize chunks. RM_PAGE_SIZE_HUGE is passed since
    // FLA->PA page size is 2MB.
    //
    if (flags.bForceNonContig || (bDefaultAllocMode && (status != NV_OK)))
    {
        for (idx = 0; idx < pageCount; idx++)
        {
            status = vaspaceAlloc(pFabricVAS->pGVAS, pageSize, align, rangeLo,
                                  rangeHi, RM_PAGE_SIZE_HUGE, flags, &addr);
            if (status == NV_OK)
            {
                // Assert that the address returned is pageSize aligned
                NV_ASSERT(NV_IS_ALIGNED64(addr, pageSize));

                (*ppAddr)[idx] = addr;
                *pNumAddr      = *pNumAddr + 1;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to allocate vaspace\n");
                goto failed;
            }
        }
    }

    return NV_OK;

failed:

    fabricvaspaceBatchFree(pFabricVAS, *ppAddr, *pNumAddr, 1);
    portMemFree(*ppAddr);
    *ppAddr   = NULL;
    *pNumAddr = 0;

    return status;
}

NV_STATUS
fabricvaspaceFree_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU64           vAddr
)
{
    OBJVASPACE *pVAS = staticCast(pFabricVAS, OBJVASPACE);
    OBJGPU     *pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);

    NV_ASSERT(vaspaceFree(pFabricVAS->pGVAS, vAddr) == NV_OK);

    kbusFlush_HAL(pGpu, pKernelBus, (BUS_FLUSH_VIDEO_MEMORY |
                                     BUS_FLUSH_SYSTEM_MEMORY | 
                                     BUS_FLUSH_USE_PCIE_READ));

    fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, PTE_DOWNGRADE);

    _fabricvaspaceUnbindInstBlk(pFabricVAS);

    return NV_OK;
}

NV_STATUS
fabricvaspaceMap_IMPL
(
    FABRIC_VASPACE       *pFabricVAS,
    OBJGPU               *pGpu,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const MMU_MAP_TARGET *pTarget,
    const VAS_MAP_FLAGS   flags
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
fabricvaspaceUnmap_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    OBJGPU         *pGpu,
    const NvU64     vaLo,
    const NvU64     vaHi
)
{
    return;
}

NV_STATUS
fabricvaspaceApplyDefaultAlignment_IMPL
(
    FABRIC_VASPACE      *pFabricVAS,
    const FB_ALLOC_INFO *pAllocInfo,
    NvU64               *pAlign,
    NvU64               *pSize,
    NvU64               *pPageSizeLockMask
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
fabricvaspaceGetVasInfo_IMPL
(
    FABRIC_VASPACE                                *pFabricVAS,
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS  *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
fabricvaspacePinRootPageDir_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    OBJGPU         *pGpu
)
{
    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);

    return vaspacePinRootPageDir(pFabricVAS->pGVAS, pGpu);
}

void
fabricvaspaceUnpinRootPageDir_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    OBJGPU         *pGpu
)
{
    NV_ASSERT(pFabricVAS->pGVAS != NULL);

    vaspaceUnpinRootPageDir(pFabricVAS->pGVAS, pGpu);
}

NV_STATUS
fabricvaspaceGetFreeHeap_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU64          *freeSize
)
{
    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT_OR_RETURN(freeSize != NULL,         NV_ERR_INVALID_ARGUMENT);

    return gvaspaceGetFreeHeap(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE),
                               freeSize);
}

void
fabricvaspaceBatchFree_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU64          *pAddr,
    NvU32           numAddr,
    NvU32           stride
)
{
    OBJVASPACE *pVAS = staticCast(pFabricVAS, OBJVASPACE);
    OBJGPU     *pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NvU32 count = 0;
    NvU32 idx   = 0;

    for (count = 0; count < numAddr; count++)
    {
        NV_ASSERT(vaspaceFree(pFabricVAS->pGVAS, pAddr[idx]) == NV_OK);

        idx += stride;
    }

    kbusFlush_HAL(pGpu, pKernelBus, (BUS_FLUSH_VIDEO_MEMORY |
                                     BUS_FLUSH_SYSTEM_MEMORY | 
                                     BUS_FLUSH_USE_PCIE_READ));

    fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, PTE_DOWNGRADE);

    _fabricvaspaceUnbindInstBlk(pFabricVAS);
}

void
fabricvaspaceInvalidateTlb_IMPL
(
    FABRIC_VASPACE      *pFabricVAS,
    OBJGPU              *pGpu,
    VAS_PTE_UPDATE_TYPE  type
)
{
    vaspaceInvalidateTlb(pFabricVAS->pGVAS, pGpu, type);
}

NV_STATUS
fabricvaspaceGetGpaMemdesc_IMPL
(
    FABRIC_VASPACE     *pFabricVAS,
    MEMORY_DESCRIPTOR  *pFabricMemdesc,
    OBJGPU             *pMappingGpu,
    MEMORY_DESCRIPTOR **ppAdjustedMemdesc
)
{
    KernelNvlink      *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pMappingGpu);
    MEMORY_DESCRIPTOR *pRootMemDesc = NULL;
    NODE              *pNode        = NULL;
    NV_STATUS          status       = NV_OK;
    NvU64              rootOffset   = 0;

    NV_ASSERT_OR_RETURN(ppAdjustedMemdesc != NULL, NV_ERR_INVALID_ARGUMENT);

    if (memdescGetAddressSpace(pFabricMemdesc) != ADDR_FABRIC_V2 ||
        (pKernelNvlink != NULL &&
         knvlinkIsP2pLoopbackSupported(pMappingGpu, pKernelNvlink)))
    {
        *ppAdjustedMemdesc = pFabricMemdesc;
        return NV_OK;
    }

    pRootMemDesc = memdescGetRootMemDesc(pFabricMemdesc, &rootOffset);

    RmPhysAddr *pteArray = memdescGetPteArray(pRootMemDesc, AT_GPU);

    // Check if pteArray[0] is within the VAS range for the mapping GPU.
    if ((pteArray[0] < vaspaceGetVaStart(staticCast(pFabricVAS, OBJVASPACE))) ||
        (pteArray[0] > vaspaceGetVaLimit(staticCast(pFabricVAS, OBJVASPACE))))
    {
        *ppAdjustedMemdesc = pFabricMemdesc;
        return NV_OK;
    }

    //
    // If the address space is of type ADDR_FABRIC_V2 then determine if the FLA import
    // is on the mapping GPU. If FLA import is on the mapping GPU and NVLink P2P over
    // loopback is not supported, then map GVA->PA directly. For discontiguous fabric
    // memory allocation, searching for the first entry in the pteArray should be fine
    // to determine if FLA import is on the mapping GPU.
    //
    NV_ASSERT_OK_OR_RETURN(btreeSearch(pteArray[0], &pNode, pFabricVAS->pFabricVaToGpaMap));

    FABRIC_VA_TO_GPA_MAP_NODE *pFabricNode = (FABRIC_VA_TO_GPA_MAP_NODE *)pNode->Data;

    //
    // Create a sub-memdesc for the offset into the vidMemDesc where the GVA would be
    // mapped. Note this includes two offsets:
    // 1. Offset into the fabric memdesc where the GVA is mapped.
    // 2. Offset into the physical vidmem memdesc where the fabric memory is mapped.
    //
    status = memdescCreateSubMem(ppAdjustedMemdesc, pFabricNode->pVidMemDesc, pMappingGpu,
                                 rootOffset + pFabricNode->offset,
                                 memdescGetSize(pFabricMemdesc));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create submMemdesc for the GVA->PA mapping\n");
        return status;
    }

    return NV_OK;
}

void
fabricvaspacePutGpaMemdesc_IMPL
(
    FABRIC_VASPACE    *pFabricVAS,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    memdescDestroy(pMemDesc);
}

void
fabricvaspaceVaToGpaMapRemove_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU64           vAddr
)
{
    FABRIC_VA_TO_GPA_MAP_NODE *pFabricNode = NULL;
    NODE                      *pNode       = NULL;

    if (btreeSearch(vAddr, &pNode, pFabricVAS->pFabricVaToGpaMap) == NV_OK)
    {
        pFabricNode = (FABRIC_VA_TO_GPA_MAP_NODE *)pNode->Data;

        btreeUnlink(&pFabricNode->Node, &pFabricVAS->pFabricVaToGpaMap);

        portMemFree(pFabricNode);
    }
}

NV_STATUS
fabricvaspaceVaToGpaMapInsert_IMPL
(
    FABRIC_VASPACE    *pFabricVAS,
    NvU64              vAddr,
    MEMORY_DESCRIPTOR *pVidMemDesc,
    NvU64              offset
)
{
    FABRIC_VA_TO_GPA_MAP_NODE *pFabricNode = NULL;
    NV_STATUS                  status      = NV_OK;

    pFabricNode = portMemAllocNonPaged(sizeof(FABRIC_VA_TO_GPA_MAP_NODE));
    if (pFabricNode == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pFabricNode, 0, sizeof(FABRIC_VA_TO_GPA_MAP_NODE));

    pFabricNode->pVidMemDesc   = pVidMemDesc;
    pFabricNode->offset        = offset;
    pFabricNode->Node.keyStart = vAddr;
    pFabricNode->Node.keyEnd   = vAddr;
    pFabricNode->Node.Data     = pFabricNode;

    // Insert into the btree tracking memory fabric allocations for this GPU.
    status = btreeInsert(&pFabricNode->Node, &pFabricVAS->pFabricVaToGpaMap);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to insert addr 0x%llx into the memory fabric tree\n",
                  pFabricNode->Node.keyStart);

        portMemFree(pFabricNode);
        return status;
    }

    return NV_OK;
}
