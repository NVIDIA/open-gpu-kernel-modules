/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_events.h"
#include "nv_sriov_defines.h"
#include "kernel/gpu/intr/intr.h"

#include "mmu/gmmu_fmt.h"
#include "published/hopper/gh100/dev_mmu.h"
#include "published/hopper/gh100/dev_fault.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/dev_vm_addendum.h"

/*!
 * Check if a specific GMMU format version is supported.
 */
NvBool
kgmmuFmtIsVersionSupported_GH10X(KernelGmmu *pKernelGmmu, NvU32 version)
{
    return (version == GMMU_FMT_VERSION_3);
}

/*!
 * Initialize the GMMU format families.
 */
NV_STATUS
kgmmuFmtFamiliesInit_GH100(OBJGPU *pGpu, KernelGmmu* pKernelGmmu)
{
    NvU32            i;
    NvU32            pdePcfHw = 0;
    NvU32            pdePcfSw = 0;
    NvU32            ptePcfHw = 0;
    NvU32            ptePcfSw = 0;

    // Initialize the sparse encoding in the PDE PCF field for V3
    GMMU_FMT_FAMILY *pFam = pKernelGmmu->pFmtFamilies[GMMU_FMT_VERSION_3 - 1];

    if (pFam != NULL)
    {
        // 1.Initialize sparsePde
        pdePcfSw |= (1 << SW_MMU_PCF_SPARSE_IDX);
        pdePcfSw |= (1 << SW_MMU_PCF_ATS_ALLOWED_IDX);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        gmmuFieldSetAperture(&pFam->pde.fldAperture, GMMU_APERTURE_INVALID,
                             pFam->sparsePde.v8);
        nvFieldSet32(&pFam->pde.fldPdePcf, pdePcfHw, pFam->sparsePde.v8);

        // 2.Initialize sparsePdeMulti
        for (i = 0; i < MMU_FMT_MAX_SUB_LEVELS; ++i)
        {
            const GMMU_FMT_PDE *pPdeFmt = &pFam->pdeMulti.subLevels[i];
            gmmuFieldSetAperture(&pPdeFmt->fldAperture, GMMU_APERTURE_INVALID,
                                 pFam->sparsePdeMulti.v8);
            // Set PDE PCF sparse bit only for sub-level 0 for PdeMulti
            if (i == 0)
            {
                nvFieldSet32(&pPdeFmt->fldPdePcf, pdePcfHw, pFam->sparsePdeMulti.v8);
            }
        }

        // 3.Initialize nv4kPte
        ptePcfSw |= (1 << SW_MMU_PCF_NV4K_IDX);
        nvFieldSetBool(&pFam->pte.fldValid, NV_FALSE, pFam->nv4kPte.v8);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw, &ptePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        nvFieldSet32(&pFam->pte.fldPtePcf, ptePcfHw, pFam->nv4kPte.v8);

        // 4.Initialize sparsePte
        ptePcfSw = (1 << SW_MMU_PCF_SPARSE_IDX);
        nvFieldSetBool(&pFam->pte.fldValid, NV_FALSE, pFam->sparsePte.v8);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw, &ptePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        nvFieldSet32(&pFam->pte.fldPtePcf, ptePcfHw, pFam->sparsePte.v8);
    }

    return NV_OK;
}

#define PTE_PCF_INVALID_LIST(fn) \
        fn(INVALID) \
        fn(NO_VALID_4KB_PAGE) \
        fn(SPARSE) \
        fn(MAPPING_NOWHERE)

#define PTE_PCF_VALID_LIST(fn) \
        fn(PRIVILEGE_RW_ATOMIC_CACHED_ACD) \
        fn(PRIVILEGE_RW_ATOMIC_CACHED_ACE) \
        fn(PRIVILEGE_RW_ATOMIC_UNCACHED_ACD) \
        fn(PRIVILEGE_RW_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RW_NO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RW_NO_ATOMIC_CACHED_ACE) \
        fn(PRIVILEGE_RO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RO_NO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RO_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RW_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RW_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RW_NO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RW_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_NO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RW_NO_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RO_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RO_NO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RO_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RO_NO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RO_NO_ATOMIC_UNCACHED_ACE)

#define PTE_PCF_HW_FROM_SW(name) \
        case (SW_MMU_PTE_PCF_##name): \
        { \
            *pPtePcfHw = NV_MMU_VER3_PTE_PCF_##name; \
            break; \
        }

#define PTE_PCF_SW_FROM_HW(name) \
        case (NV_MMU_VER3_PTE_PCF_##name): \
        { \
            *pPtePcfSw = SW_MMU_PTE_PCF_##name; \
            break; \
        }

//
// Takes a SW PTE PCF and translates to HW PTE PCF
// If bit patterns is not supported by HW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePtePcfFromSw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32    ptePcfSw,
    NvU32   *pPtePcfHw
)
{
    switch (ptePcfSw)
    {
        PTE_PCF_INVALID_LIST(PTE_PCF_HW_FROM_SW)
        PTE_PCF_VALID_LIST(PTE_PCF_HW_FROM_SW)

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported SW PTE PCF pattern requested : %x\n", ptePcfSw);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;
}

NV_STATUS
kgmmuTranslatePtePcfFromHw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       ptePcfHw,
    NvBool      bPteValid,
    NvU32      *pPtePcfSw
)
{
    if (!bPteValid)
    {
        switch (ptePcfHw)
        {
            PTE_PCF_INVALID_LIST(PTE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (ptePcfHw)
        {
            PTE_PCF_VALID_LIST(PTE_PCF_SW_FROM_HW)

            default:
            {
                NV_PRINTF(LEVEL_ERROR, "Unsupported HW PTE PCF pattern requested : %x\n", ptePcfHw);
                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }

    return NV_OK;
}

#define PDE_PCF_INVALID_LIST(fn) \
        fn(INVALID_ATS_ALLOWED) \
        fn(SPARSE_ATS_ALLOWED) \
        fn(INVALID_ATS_NOT_ALLOWED) \
        fn(SPARSE_ATS_NOT_ALLOWED)

#define PDE_PCF_VALID_LIST(fn) \
        fn(VALID_CACHED_ATS_ALLOWED) \
        fn(VALID_CACHED_ATS_NOT_ALLOWED) \
        fn(VALID_UNCACHED_ATS_ALLOWED) \
        fn(VALID_UNCACHED_ATS_NOT_ALLOWED)

#define PDE_PCF_HW_FROM_SW(name) \
        case (SW_MMU_PDE_PCF_##name): \
        { \
            *pPdePcfHw = NV_MMU_VER3_PDE_PCF_##name; \
            break; \
        }

#define PDE_PCF_SW_FROM_HW(name) \
        case (NV_MMU_VER3_PDE_PCF_##name): \
        { \
            *pPdePcfSw = SW_MMU_PDE_PCF_##name; \
            break; \
        }

//
// Takes a SW PDE PCF and translates to HW PDE PCF
// If a bit pattern is not supported by HW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePdePcfFromSw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       pdePcfSw,
    NvU32      *pPdePcfHw
)
{
    switch (pdePcfSw)
    {
        PDE_PCF_INVALID_LIST(PDE_PCF_HW_FROM_SW)
        PDE_PCF_VALID_LIST(PDE_PCF_HW_FROM_SW)

        default: return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

//
// Takes a HW PDE PCF and translates to SW PDE PCF
// If a bit pattern is not supported by SW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePdePcfFromHw_GH100
(
    KernelGmmu     *pKernelGmmu,
    NvU32           pdePcfHw,
    GMMU_APERTURE   aperture,
    NvU32          *pPdePcfSw
)
{
    if (aperture == GMMU_APERTURE_INVALID)
    {
        switch (pdePcfHw)
        {
            PDE_PCF_INVALID_LIST(PDE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (pdePcfHw)
        {
            PDE_PCF_VALID_LIST(PDE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;
}

/*
 * @brief   Validates fabric base address.
 *
 * @param   pKernelGmmu
 * @param   fabricBaseAddr
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
kgmmuValidateFabricBaseAddress_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU64       fabricBaseAddr
)
{
    OBJGPU        *pGpu = ENG_GET_GPU(pKernelGmmu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 fbSizeBytes;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Hopper SKUs will be paired with NVSwitches (Laguna Seca) supporting 2K
    // mapslots that can cover 512GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(39) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(39));

    return NV_OK;
}

/*!
 * @brief Get the engine ID associated with the Graphics Engine
 */
NvU32
kgmmuGetGraphicsEngineId_GH100
(
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_GRAPHICS;
}

NV_STATUS
kgmmuGetFaultRegisterMappings_GH100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       index,
    NvP64      *pFaultBufferGet,
    NvP64      *pFaultBufferPut,
    NvP64      *pFaultBufferInfo,
    NvP64      *pHubIntr,
    NvP64      *pHubIntrEnSet,
    NvP64      *pHubIntrEnClear,
    NvU32      *faultMask,
    NvP64      *pPrefetchCtrl
)
{
    DEVICE_MAPPING *pMapping    = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    NvP64           bar0Mapping = NV_PTR_TO_NvP64(pMapping->gpuNvAddr);

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    //
    // If Hopper CC is not enabled or GSP doesn't entirely own the HW fault buffers
    // use the Turing HAL
    //
    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        return kgmmuGetFaultRegisterMappings_TU102(pGpu, pKernelGmmu, index,
                                                   pFaultBufferGet, pFaultBufferPut,
                                                   pFaultBufferInfo, pHubIntr,
                                                   pHubIntrEnSet, pHubIntrEnClear,
                                                   faultMask, pPrefetchCtrl);
    }

    *pFaultBufferGet = 0;
    *pFaultBufferInfo = 0;
    *pHubIntr = 0;
    *pHubIntrEnSet = 0;
    *pHubIntrEnClear = 0;
    *faultMask = 0;
    *pPrefetchCtrl = 0;

    //
    // When Hopper CC is enabled, we repurpose the access counter registers to
    // hold the PUT pointer of the shadow buffers. Only GSP-RM can write the
    // PUT pointer to these PRIs. CPU has read-only access to these PRIs
    //
    if (index == REPLAYABLE_FAULT_BUFFER)
    {
        Intr *pIntr      = GPU_GET_INTR(pGpu);
        NvU32 intrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_REPLAYABLE_FAULT_CPU, NV_FALSE);
        struct GMMU_FAULT_BUFFER *pFaultBuffer;
        GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf;
        FAULT_BUFFER_SHARED_MEMORY *pFaultBufSharedMem;
        NvU32 leafReg;
        NvU32 leafBit;

        leafReg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
        leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

        pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF];
        pClientShadowFaultBuf =
            KERNEL_POINTER_FROM_NvP64(GMMU_CLIENT_SHADOW_FAULT_BUFFER *,
                        pFaultBuffer->pClientShadowFaultBuffer[index]);

        pFaultBufSharedMem =
            KERNEL_POINTER_FROM_NvP64(FAULT_BUFFER_SHARED_MEMORY *,
                        pClientShadowFaultBuf->pFaultBufferSharedMemoryAddress);

        *pHubIntr = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(leafReg)));
        *pHubIntrEnSet = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(leafReg)));
        *pHubIntrEnClear = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(leafReg)));
        *faultMask = NVBIT(leafBit);
        *pFaultBufferGet = (NvU32*) &(pFaultBufSharedMem->swGetIndex);
        *pFaultBufferPut = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI));
        *pPrefetchCtrl = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_PAGE_FAULT_CTRL));
    }
    else if (index == NON_REPLAYABLE_FAULT_BUFFER)
    {
        *pFaultBufferPut = NvP64_PLUS_OFFSET(bar0Mapping,
                     GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO));
    }
    else
    {
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
    }

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferAllocSharedMemory_GH100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    FAULT_BUFFER_TYPE index
)
{
    NV_STATUS status;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        NV_PRINTF(LEVEL_ERROR, "Fault-Buffer is disabled. Flush Seq memory cannot be created\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }

    if (index != REPLAYABLE_FAULT_BUFFER)
    {
        return NV_OK;
    }

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        return NV_OK;
    }

    //
    // On systems with SEV enabled, the fault buffer flush sequence memory should be allocated
    // in unprotected sysmem as GSP will be writing to this location to let the guest
    // know a flush has finished.
    //
    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer[index];
    status = memdescCreate(&pMemDesc, pGpu,
                           sizeof(FAULT_BUFFER_SHARED_MEMORY), RM_PAGE_SIZE,
                           NV_FALSE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                           flags);
    if (status != NV_OK)
    {
        return status;
    }

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_131, 
                    pMemDesc);
    if (status != NV_OK)
    {
        goto destroy_memdesc;
    }

    status = memdescMap(pMemDesc, 0,
                        memdescGetSize(pMemDesc),
                        NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pClientShadowFaultBuffer->pFaultBufferSharedMemoryAddress,
                        &pClientShadowFaultBuffer->pFaultBufferSharedMemoryPriv);
    if (status != NV_OK)
    {
        goto free_memory;
    }

    pClientShadowFaultBuffer->pFaultBufferSharedMemDesc = pMemDesc;

    return NV_OK;

free_memory:
    memdescFree(pMemDesc);

destroy_memdesc:
    memdescDestroy(pMemDesc);

    return status;
}

void
kgmmuFaultBufferFreeSharedMemory_GH100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    FAULT_BUFFER_TYPE index
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;

    if (index != REPLAYABLE_FAULT_BUFFER)
    {
        return;
    }

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        return;
    }

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer[index];
    pMemDesc = pClientShadowFaultBuffer->pFaultBufferSharedMemDesc;

    memdescUnmap(pMemDesc,
                 NV_TRUE, osGetCurrentProcess(),
                 pClientShadowFaultBuffer->pFaultBufferSharedMemoryAddress,
                 pClientShadowFaultBuffer->pFaultBufferSharedMemoryPriv);

    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
    return;
}

/*
 * @brief GSP client can use this function to initiate a replayable fault buffer flush when the
 * HW fault buffer is owned by GSP.
 */
NV_STATUS
kgmmuIssueReplayableFaultBufferFlush_GH100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    FAULT_BUFFER_SHARED_MEMORY *pFaultBufSharedMem;
    NvU32 gfid;
    volatile NvU32 *pFlushSeqAddr;
    NvU32 replayableFlushSeqValue;
    NV_STATUS status;
    RMTIMEOUT timeout;

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu) || !IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[gfid].clientShadowFaultBuffer[REPLAYABLE_FAULT_BUFFER];
    pFaultBufSharedMem = KERNEL_POINTER_FROM_NvP64(FAULT_BUFFER_SHARED_MEMORY *,
                                                   pClientShadowFaultBuffer->pFaultBufferSharedMemoryAddress);
    pFlushSeqAddr = (NvU32*) &(pFaultBufSharedMem->flushBufferSeqNum);
    replayableFlushSeqValue = *pFlushSeqAddr;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_DOORBELL, NV_DOORBELL_NOTIFY_LEAF_SERVICE_REPLAYABLE_FAULT_FLUSH_HANDLE);

    while (replayableFlushSeqValue + 1 != *pFlushSeqAddr)
    {
        status = gpuCheckTimeout(pGpu, &timeout);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "gpuCheckTimeout failed, status = 0x%x\n", status);
            return status;
        }
        osSpinLoop();
    }

    return NV_OK;
}

/*
 * @brief When Hopper Confidential Compute is enabled, the put index of the
 *        client replayable/non-replayable shadow buffers gets stored in the
 *        access counter PRIs. This function is used by Kernel RM to read the put index.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelGmmu  KernelGmmu pointer
 * @param[in]  type         Replayable/Non-replayable fault buffer
 *
 * @returns NvU32
 */
NvU32
kgmmuReadShadowBufPutIndex_GH100
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    FAULT_BUFFER_TYPE  type
)
{
    NvU32 val;
    if (type == REPLAYABLE_FAULT_BUFFER)
    {
        val = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT);
    }
    else
    {
        val = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT);
        val = DRF_VAL(_VIRTUAL_FUNCTION_PRIV, _NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT, _PTR, val);
    }
    return val;
}
