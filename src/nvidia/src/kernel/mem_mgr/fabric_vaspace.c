/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*                                                                           *
*         Fabric Virtual Address Space Function Definitions.                *
*                                                                           *
\***************************************************************************/

#include "gpu/mmu/kern_gmmu.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "os/os.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "mmu/mmu_walk.h"
#include "lib/base_utils.h"
#include "class/cl90f1.h"    // FERMI_VASPACE_A
#include "class/cl00fc.h"    // FABRIC_VASPACE_A
#include "class/cl0040.h"    // NV01_MEMORY_LOCAL_USER
#include "class/cl0080.h"    // NV01_DEVICE_0
#include "deprecated/rmapi_deprecated.h"
#include "rmapi/rs_utils.h"
#include "vgpu/vgpu_events.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "compute/fabric.h"

#include "published/ampere/ga100/dev_mmu.h"
#include "vgpu/rpc.h"
#include "virtualization/hypervisor/hypervisor.h"



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

    if (!pKernelBus->flaInfo.bToggleBindPoint)
    {
        return NV_OK;
    }

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
// TODO: To be removed when legacy FLA VAS (pKernelBus->flaInfo.pFlaVAS)is removed"
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

    if (!pKernelBus->flaInfo.bToggleBindPoint)
    {
        return;
    }

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

    if (pKernelBus->flaInfo.pFlaVAS != NULL)
    {
        // Instantiate the instance block for FLA vaspace.
        NV_ASSERT(kgmmuInstBlkInit(pKernelGmmu,
                                   pKernelBus->flaInfo.pInstblkMemDesc,
                                   pKernelBus->flaInfo.pFlaVAS,
                                   FIFO_PDB_IDX_BASE,
                                   &instblkParams) == NV_OK);

        // Bind the instance block for FLA vaspace.
        NV_ASSERT(kbusSetupBindFla_HAL(pGpu, pKernelBus,
                                       pFabricVAS->gfid) == NV_OK);
    }
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
                                     NV01_ROOT, &hClient, sizeof(hClient));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating client, status=0x%x\n", status);
        return status;
    }

    status = serverutilGenResourceHandle(hClient, &hDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed creating device handle, status=0x%x\n", status);
        goto cleanup;
    }

    // Allocate a device handle
    devAllocParams.deviceId = gpuGetDeviceInstance(pGpu);
    status = pRmApi->AllocWithHandle(pRmApi, hClient, hClient, hDevice,
                                     NV01_DEVICE_0,
                                     &devAllocParams, sizeof(devAllocParams));
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

    pFabricVAS->bRpcAlloc = IS_VIRTUAL(pGpu) &&
                                gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

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
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    if (fabricIsMemAllocDisabled(pFabric))
        return NV_ERR_INVALID_STATE;

    //
    // TODO: If needed, can call into fabricvaspaceAllocNonContiguous_IMPL()
    // by forcing contig flag.
    //
    return NV_ERR_NOT_SUPPORTED;
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
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    if (fabricIsMemAllocDisabled(pFabric))
        return NV_ERR_INVALID_STATE;

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
    //
    // RM_PAGE_SIZE_HUGE is passed since FLA->PA mappings support minimum
    // 2MB pagesize.
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
        else if (flags.bForceContig)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate contig vaspace\n");
            goto failed;
        }
    }

    //
    // If size could not be allocated in one memblock, break size into
    // multiple pageSize chunks.
    //
    // RM_PAGE_SIZE_HUGE is passed since FLA->PA mappings support minimum
    // 2MB pagesize.
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

    pFabricVAS->ucFabricFreeSize  -= size;
    pFabricVAS->ucFabricInUseSize += size;

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
    NvU64       blockSize;
    NvBool      bUcFla;

    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);

    bUcFla = (vAddr >= fabricvaspaceGetUCFlaStart(pFabricVAS) &&
              vAddr < fabricvaspaceGetUCFlaLimit(pFabricVAS));

    NV_ASSERT(vaspaceFreeV2(pFabricVAS->pGVAS, vAddr, &blockSize) == NV_OK);

    kbusFlush_HAL(pGpu, pKernelBus, (BUS_FLUSH_VIDEO_MEMORY |
                                     BUS_FLUSH_SYSTEM_MEMORY));

    fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, PTE_DOWNGRADE);

    _fabricvaspaceUnbindInstBlk(pFabricVAS);

    if (bUcFla)
    {
        pFabricVAS->ucFabricFreeSize  += blockSize;
        pFabricVAS->ucFabricInUseSize -= blockSize;
    }

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

    *freeSize = pFabricVAS->ucFabricFreeSize;
    return NV_OK;
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
    NvU64       totalFreeSize = 0;
    NvU64       freeSize;
    NvU32 count = 0;
    NvU32 idx   = 0;
    NvBool bUcFla;


    for (count = 0; count < numAddr; count++)
    {
        bUcFla = (pAddr[idx] >= fabricvaspaceGetUCFlaStart(pFabricVAS) &&
                  pAddr[idx] < fabricvaspaceGetUCFlaLimit(pFabricVAS));

        NV_ASSERT(vaspaceFreeV2(pFabricVAS->pGVAS,
                                pAddr[idx], &freeSize) == NV_OK);

        idx += stride;

        if (bUcFla)
            totalFreeSize += freeSize;
    }

    kbusFlush_HAL(pGpu, pKernelBus, (BUS_FLUSH_VIDEO_MEMORY |
                                     BUS_FLUSH_SYSTEM_MEMORY));

    fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, PTE_DOWNGRADE);

    _fabricvaspaceUnbindInstBlk(pFabricVAS);

    pFabricVAS->ucFabricFreeSize  += totalFreeSize;
    pFabricVAS->ucFabricInUseSize -= totalFreeSize;
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
    KernelNvlink      *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pMappingGpu);
    MEMORY_DESCRIPTOR *pRootMemDesc       = NULL;
    NODE              *pNode              = NULL;
    NV_STATUS          status             = NV_OK;
    NvU64              rootOffset         = 0;
    NvBool             bLoopbackSupported = NV_FALSE;

    NV_ASSERT_OR_RETURN(ppAdjustedMemdesc != NULL, NV_ERR_INVALID_ARGUMENT);

    {
        bLoopbackSupported = pKernelNvlink != NULL &&
                (knvlinkIsP2pLoopbackSupported(pMappingGpu, pKernelNvlink) ||
                 knvlinkIsForcedConfig(pMappingGpu, pKernelNvlink));
    }

    if (memdescGetAddressSpace(pFabricMemdesc) != ADDR_FABRIC_V2 ||
        bLoopbackSupported)
    {
        *ppAdjustedMemdesc = pFabricMemdesc;
        return NV_OK;
    }

    pRootMemDesc = memdescGetRootMemDesc(pFabricMemdesc, &rootOffset);

    RmPhysAddr *pteArray = memdescGetPteArray(pRootMemDesc, AT_GPU);

    // Check if pteArray[0] is within the VAS range for the mapping GPU.
    if ((pteArray[0] < fabricvaspaceGetUCFlaStart(pFabricVAS)) ||
        (pteArray[0] > fabricvaspaceGetUCFlaLimit(pFabricVAS)))
    {
        *ppAdjustedMemdesc = pFabricMemdesc;
        return NV_OK;
    }

    //
    // If the address space is of type ADDR_FABRIC_V2 then determine if the
    // FLA import is on the mapping GPU. If FLA import is on the mapping GPU
    // and NVLink P2P over loopback is not supported, then map GVA->PA directly.
    // For discontiguous fabric memory allocation, searching for the first entry
    // in the pteArray should be fine to determine if FLA import is on the
    // mapping GPU.
    //
    NV_ASSERT_OK_OR_RETURN(btreeSearch(pteArray[0], &pNode,
                                       pFabricVAS->pFabricVaToGpaMap));

    FABRIC_VA_TO_GPA_MAP_NODE *pFabricNode =
                                 (FABRIC_VA_TO_GPA_MAP_NODE *)pNode->Data;

    //
    // Create a sub-memdesc for the offset into the vidMemDesc where the GVA
    // would be mapped. Note this includes two offsets:
    // 1. Offset into the fabric memdesc where the GVA is mapped.
    // 2. Offset into the physical vidmem memdesc where the fabric memory is
    // mapped.
    //
    status = memdescCreateSubMem(ppAdjustedMemdesc, pFabricNode->pVidMemDesc,
                                 pMappingGpu,
                                 rootOffset + pFabricNode->offset,
                                 memdescGetSize(pFabricMemdesc));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to create submMemdesc for the GVA->PA mapping\n");
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

NV_STATUS
fabricvaspaceAllocMulticast_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    NvU64           pageSize,
    NvU64           alignment,
    VAS_ALLOC_FLAGS flags,
    NvU64           base,
    NvU64           size
)
{
    NvU64 rangeLo;
    NvU64 rangeHi;
    NvU64 addr = 0;
    NV_STATUS status;

    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    if (fabricIsMemAllocDisabled(pFabric))
        return NV_ERR_INVALID_STATE;

    NV_ASSERT_OR_RETURN(pFabricVAS->pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT_OR_RETURN(pageSize >= RM_PAGE_SIZE_HUGE, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(alignment != 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(alignment, pageSize), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(base, pageSize), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(size, pageSize), NV_ERR_INVALID_ARGUMENT);

    rangeLo = base;
    rangeHi = base + size - 1;

    //
    // RM_PAGE_SIZE_HUGE is passed since MCFLA->PA mappings support minimum
    // 2MB pagesize.
    //
    status = vaspaceAlloc(pFabricVAS->pGVAS, size, alignment, rangeLo,
                          rangeHi, RM_PAGE_SIZE_HUGE, flags, &addr);

    NV_ASSERT(addr == base);

    return status;
}

static NV_STATUS
_fabricVaspaceValidateMapAttrs
(
    NvU64  fabricOffset,
    NvU64  fabricAllocSize,
    NvU64  fabricPageSize,
    NvU64  physMapOffset,
    NvU64  physMapLength,
    NvU64  physAllocSize,
    NvU64  physPageSize
)
{
    // Fabric mem offset should be at least phys page size aligned.
    if (!NV_IS_ALIGNED64(fabricOffset, physPageSize) ||
        (fabricOffset >= fabricAllocSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid offset passed for the fabric handle\n");

        return NV_ERR_INVALID_OFFSET;
    }

    if (!NV_IS_ALIGNED64(physMapOffset, physPageSize) ||
        (physMapOffset >= physAllocSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid offset passed for the physmem handle\n");

        return NV_ERR_INVALID_OFFSET;
    }

    if ((physMapLength == 0) ||
        (!NV_IS_ALIGNED64(physMapLength, physPageSize))   ||
        (physMapLength > (physAllocSize - physMapOffset)) ||
        (physMapLength > (fabricAllocSize - fabricOffset)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid map length passed for the physmem handle\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

typedef struct FABRIC_VASPACE_MAPPING_REGION
{
    NvU64  offset;
    NvU64  length;
} FABRIC_VASPACE_MAPPING_REGION;

//
// In worst case, we can have three regions to map. Two partially filled fabric
// pages and one (or more) fully filled fabric page(s).
//
#define FABRIC_VASPACE_MAPPING_REGIONS_MAX 3

typedef struct FABRIC_VASPACE_MAPPING_REGIONS
{
    FABRIC_VASPACE_MAPPING_REGION r[FABRIC_VASPACE_MAPPING_REGIONS_MAX];
} FABRIC_VASPACE_MAPPING_REGIONS;

static void
_fabricvaspaceGetMappingRegions
(
    NvU64                           fabricOffset,
    NvU64                           fabricPageSize,
    NvU64                           physMapLength,
    FABRIC_VASPACE_MAPPING_REGIONS *pRegions,
    NvU32                          *pNumRegions
)
{
    NvU64 fabricOffsetAligned = NV_ALIGN_UP64(fabricOffset, fabricPageSize);
    NvU64 mapLengthAligned = NV_ALIGN_DOWN64(physMapLength, fabricPageSize);

    *pNumRegions = 0;

    if ((fabricOffset < fabricOffsetAligned) &&
        (physMapLength >= (fabricOffsetAligned - fabricOffset)))
    {
        pRegions->r[*pNumRegions].offset = fabricOffset;
        pRegions->r[*pNumRegions].length = fabricOffsetAligned - fabricOffset;

        fabricOffset += pRegions->r[*pNumRegions].length;
        physMapLength -= pRegions->r[*pNumRegions].length;
        mapLengthAligned = NV_ALIGN_DOWN64(physMapLength, fabricPageSize);

        (*pNumRegions)++;
    }

    if (physMapLength == 0)
        return;

    if ((fabricOffset == fabricOffsetAligned) &&
        (mapLengthAligned >= fabricPageSize))
    {
        pRegions->r[*pNumRegions].offset = fabricOffset;
        pRegions->r[*pNumRegions].length = mapLengthAligned;

        fabricOffset += pRegions->r[*pNumRegions].length;
        physMapLength -= pRegions->r[*pNumRegions].length;

        (*pNumRegions)++;
    }

    if (physMapLength == 0)
        return;

    pRegions->r[*pNumRegions].offset = fabricOffset;
    pRegions->r[*pNumRegions].length = physMapLength;

    (*pNumRegions)++;
}

void
fabricvaspaceUnmapPhysMemdesc_IMPL
(
    FABRIC_VASPACE    *pFabricVAS,
    MEMORY_DESCRIPTOR *pFabricMemDesc,
    NvU64              fabricOffset,
    MEMORY_DESCRIPTOR *pPhysMemDesc,
    NvU64              physMapLength
)
{
    OBJGPU *pGpu = pPhysMemDesc->pGpu;
    NvU32 fabricPageCount;
    NvU64 fabricAddr;
    NvU64 fabricPageSize;
    NvU32 i, j;
    NvU64 mapLength;
    FABRIC_VASPACE_MAPPING_REGIONS regions;
    NvU32 numRegions;
    RmPhysAddr *pFabricPteArray;

    fabricPageSize = memdescGetPageSize(pFabricMemDesc, AT_GPU);

    NV_ASSERT_OR_RETURN_VOID(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE) ==
                             pFabricVAS);

    _fabricvaspaceGetMappingRegions(fabricOffset, fabricPageSize, physMapLength,
                                    &regions, &numRegions);
    NV_ASSERT_OR_RETURN_VOID(numRegions != 0);

    pFabricPteArray = memdescGetPteArray(pFabricMemDesc, AT_GPU);

    for (i = 0; i < numRegions; i++)
    {
        fabricPageCount =
            ((memdescGetPteArraySize(pFabricMemDesc, AT_GPU) == 1) ||
             (regions.r[i].length < fabricPageSize)) ?
            1 : (regions.r[i].length / fabricPageSize);

        mapLength = (fabricPageCount == 1) ? regions.r[i].length : fabricPageSize;

        fabricOffset = regions.r[i].offset;

        for (j = 0; j < fabricPageCount; j++)
        {
            if (fabricPageCount == 1)
            {
                fabricAddr = pFabricPteArray[0] + fabricOffset;
            }
            else
            {
                fabricAddr = pFabricPteArray[fabricOffset /
                    pFabricMemDesc->pageArrayGranularity];
            }

            vaspaceUnmap(pFabricVAS->pGVAS, pPhysMemDesc->pGpu, fabricAddr,
                         fabricAddr + mapLength - 1);

            fabricOffset = fabricOffset + mapLength;
        }
    }

    fabricvaspaceInvalidateTlb(pFabricVAS, pPhysMemDesc->pGpu, PTE_DOWNGRADE);
}

NV_STATUS
fabricvaspaceMapPhysMemdesc_IMPL
(
    FABRIC_VASPACE    *pFabricVAS,
    MEMORY_DESCRIPTOR *pFabricMemDesc,
    NvU64              fabricOffset,
    MEMORY_DESCRIPTOR *pPhysMemDesc,
    NvU64              physOffset,
    NvU64              physMapLength,
    NvU32              flags
)
{
    OBJGPU *pGpu = pPhysMemDesc->pGpu;
    VirtMemAllocator *pDma = GPU_GET_DMA(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS status;
    DMA_PAGE_ARRAY pageArray;
    NvU32 kind;
    COMPR_INFO comprInfo;
    NvU32 mapFlags = DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL |
                     DMA_UPDATE_VASPACE_FLAGS_SKIP_4K_PTE_CHECK;
    NvU32 fabricPageCount;
    NvU64 fabricAddr;
    NvU64 physPageSize;
    NvU64 fabricPageSize;
    NvU64 physAddr;
    NvU32 i, j;
    NvU64 mapLength;
    NvBool bReadOnly = !!(flags & FABRIC_VASPACE_MAP_FLAGS_READ_ONLY);
    FABRIC_VASPACE_MAPPING_REGIONS regions;
    NvU32 numRegions;
    MEMORY_DESCRIPTOR *pTempMemdesc;
    NvU32 aperture;
    NvU32 peerNumber = BUS_INVALID_PEER;
    RmPhysAddr *pFabricPteArray;
    RmPhysAddr *pPhysPteArray;

    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pPhysMemDesc != NULL,   NV_ERR_INVALID_ARGUMENT);

    mapFlags |= bReadOnly ? DMA_UPDATE_VASPACE_FLAGS_READ_ONLY : 0;

    NV_ASSERT_OR_RETURN(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE) == pFabricVAS,
                        NV_ERR_INVALID_ARGUMENT);

    physPageSize = memdescGetPageSize(pPhysMemDesc, AT_GPU);
    fabricPageSize = memdescGetPageSize(pFabricMemDesc, AT_GPU);

    status = _fabricVaspaceValidateMapAttrs(fabricOffset,
                                            memdescGetSize(pFabricMemDesc),
                                            fabricPageSize,
                                            physOffset,
                                            physMapLength,
                                            memdescGetSize(pPhysMemDesc),
                                            physPageSize);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

    if (pFabricVAS->bRpcAlloc)
        return NV_OK;

    status = memmgrGetKindComprFromMemDesc(pMemoryManager, pPhysMemDesc,
                                           physOffset, &kind, &comprInfo);
    NV_ASSERT_OK_OR_RETURN(status);

    if (memdescGetAddressSpace(pPhysMemDesc) == ADDR_FBMEM)
    {
        aperture = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
    }
    else if (memdescIsEgm(pPhysMemDesc))
    {
        aperture = NV_MMU_PTE_APERTURE_PEER_MEMORY;
        //
        // Make sure that we receive a mapping request for EGM memory
        // only if local EGM is enabled.
        //
        NV_ASSERT_OR_RETURN(pMemoryManager->bLocalEgmEnabled, NV_ERR_INVALID_STATE);
        peerNumber = pMemoryManager->localEgmPeerId;
    }
    else if (memdescGetAddressSpace(pPhysMemDesc) == ADDR_SYSMEM)
    {
        if (memdescGetCpuCacheAttrib(pPhysMemDesc) == NV_MEMORY_CACHED)
        {
            aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
        }
        else
        {
            aperture = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Unsupported aperture\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
    }

    _fabricvaspaceGetMappingRegions(fabricOffset, fabricPageSize, physMapLength,
                                    &regions, &numRegions);
    NV_ASSERT_OR_RETURN(numRegions != 0, NV_ERR_INVALID_ARGUMENT);

    pFabricPteArray = memdescGetPteArray(pFabricMemDesc, AT_GPU);
    pPhysPteArray = memdescGetPteArray(pPhysMemDesc, AT_GPU);

    for (i = 0; i < numRegions; i++)
    {
        fabricPageCount = ((memdescGetPteArraySize(pFabricMemDesc, AT_GPU) == 1) ||
                           (regions.r[i].length < fabricPageSize)) ? \
                          1 : (regions.r[i].length / fabricPageSize);
        mapLength = (fabricPageCount == 1) ? regions.r[i].length : fabricPageSize;
        fabricOffset = regions.r[i].offset;

        portMemSet(&pageArray, 0, sizeof(DMA_PAGE_ARRAY));
        pageArray.count = (memdescGetPteArraySize(pPhysMemDesc, AT_GPU) == 1) ? \
                          1 : (mapLength / pPhysMemDesc->pageArrayGranularity);

        for (j = 0; j < fabricPageCount; j++)
        {
            if (fabricPageCount == 1)
            {
                fabricAddr = pFabricPteArray[0] + fabricOffset;
            }
            else
            {
                fabricAddr = pFabricPteArray[fabricOffset /
                    pFabricMemDesc->pageArrayGranularity];
            }

            if (pageArray.count == 1)
            {
                physAddr = pPhysPteArray[0] + physOffset;
                pageArray.pData = &physAddr;
            }
            else
            {
                pageArray.pData = &pPhysPteArray[physOffset /
                    pPhysMemDesc->pageArrayGranularity];
            }

            //
            // When physPageSize is greater than fabricPageSize, to avoid fabric
            // VAs getting aligned using physPageSize by dmaUpdateVASpace_HAL,
            // create a tempMemdesc and override its pageSize.
            //
            if (fabricPageSize < physPageSize)
            {
                status = memdescCreateSubMem(&pTempMemdesc, pPhysMemDesc,
                                             pPhysMemDesc->pGpu,
                                             physOffset, mapLength);
                if (status != NV_OK)
                    goto fail;

                memdescSetPageSize(pTempMemdesc, AT_GPU, fabricPageSize);
            }
            else
            {
                pTempMemdesc = pPhysMemDesc;
            }

            // Map the memory fabric object at the given physical memory offset.
            status = dmaUpdateVASpace_HAL(pGpu, pDma, pFabricVAS->pGVAS, pTempMemdesc,
                                      NULL, fabricAddr, fabricAddr + mapLength - 1,
                                      mapFlags, &pageArray, 0, &comprInfo, 0,
                                      NV_MMU_PTE_VALID_TRUE,
                                      aperture,
                                      peerNumber, NVLINK_INVALID_FABRIC_ADDR,
                                      DMA_DEFER_TLB_INVALIDATE, NV_FALSE,
                                      memdescGetPageSize(pTempMemdesc, AT_GPU));

            if (pTempMemdesc != pPhysMemDesc)
                memdescDestroy(pTempMemdesc);

            if (status != NV_OK)
                goto fail;

            physOffset = physOffset + mapLength;
            fabricOffset = fabricOffset + mapLength;
        }
    }

    fabricvaspaceInvalidateTlb(pFabricVAS, pPhysMemDesc->pGpu, PTE_UPGRADE);

    return NV_OK;

fail:
    for (j = 0; j < i; j++)
        fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc,
                                      regions.r[j].offset, pPhysMemDesc,
                                      regions.r[j].length);

    return status;
}

NV_STATUS
fabricvaspaceInitUCRange_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    OBJGPU         *pGpu,
    NvU64           fabricBase,
    NvU64           fabricSize
)
{
    if (fabricvaspaceGetUCFlaLimit(pFabricVAS) != 0)
        return NV_ERR_IN_USE;

    if (fabricSize != 0)
    {
        NV_PRINTF(LEVEL_INFO, "Setting UC Base: %llx, size: %llx \n",
                  fabricBase, fabricSize);
        pFabricVAS->ucFabricBase  = fabricBase;
        pFabricVAS->ucFabricLimit = fabricBase + fabricSize - 1;
        pFabricVAS->ucFabricInUseSize = 0;
        pFabricVAS->ucFabricFreeSize = fabricSize;

        if (IS_VIRTUAL(pGpu))
        {
            VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

            pVSI->flaInfo.base = fabricBase;
            pVSI->flaInfo.size = fabricSize;
        }
    }

    return NV_OK;
}

void
fabricvaspaceClearUCRange_IMPL
(
    FABRIC_VASPACE *pFabricVAS
)
{
    pFabricVAS->ucFabricBase      = 0;
    pFabricVAS->ucFabricLimit     = 0;
    pFabricVAS->ucFabricInUseSize = 0;
    pFabricVAS->ucFabricFreeSize  = 0;
}

NV_STATUS
fabricvaspaceGetPageLevelInfo_IMPL
(
    FABRIC_VASPACE *pFabricVAS,
    OBJGPU         *pGpu,
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams
)
{
    OBJGVASPACE *pGVAS = dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_OBJECT_NOT_FOUND);

    return gvaspaceGetPageLevelInfo(pGVAS, pGpu, pParams);
}

NvBool
fabricvaspaceIsInUse_IMPL
(
    FABRIC_VASPACE *pFabricVAS
)
{
    return gvaspaceIsInUse(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE));
}
