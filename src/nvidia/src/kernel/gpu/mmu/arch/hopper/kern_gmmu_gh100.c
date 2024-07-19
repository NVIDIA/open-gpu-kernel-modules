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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_events.h"
#include "nv_sriov_defines.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/gsp/kernel_gsp.h"
#include "kernel/gpu/conf_compute/ccsl.h"

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
    // in unprotected sysmem as GSP will be reading this location to check whether the Replayable buffer is full.
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
    KernelGmmu *pKernelGmmu,
    NvBool bCopyAndFlush
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NvU32 arg = !!bCopyAndFlush;

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu) || !IS_GSP_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return kgspIssueNotifyOp_HAL(pGpu, pKernelGsp,
                                 GSP_NOTIFY_OP_FLUSH_REPLAYABLE_FAULT_BUFFER_OPCODE,
                                 &arg,
                                 GSP_NOTIFY_OP_FLUSH_REPLAYABLE_FAULT_BUFFER_VALID_ARGC);
}

/*
 * @brief The GSP client can use this function to toggle the prefetch ctrl register state.
 * The write of the register will be performed by GSP.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelGmmu  KernelGmmu pointer
 * @param[in]  bEnable      Enable/Disable fault on prefetch.
 */
NV_STATUS
kgmmuToggleFaultOnPrefetch_GH100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    NvBool bEnable
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NvU32 arg = !!bEnable;

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return kgspIssueNotifyOp_HAL(pGpu, pKernelGsp, GSP_NOTIFY_OP_TOGGLE_FAULT_ON_PREFETCH_OPCODE, &arg, 1 /* argc */);
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

/*!
 * @brief Check if the given engineID is BAR1
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if BAR1
 */
NvBool
kgmmuIsFaultEngineBar1_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_BAR1);
}

/*!
 * @brief Check if the given engineID is BAR2
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if BAR2
 */
NvBool
kgmmuIsFaultEngineBar2_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_BAR2);
}

/*!
 * @brief Check if the given engineID is PHYSICAL
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if PHYSICAL
 */
NvBool
kgmmuIsFaultEnginePhysical_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_PHYSICAL);
}

NvU32
kgmmuReadClientShadowBufPutIndex_GH100
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    NvU32              gfid,
    FAULT_BUFFER_TYPE  type
)
{
    return 0;
}

void
kgmmuWriteClientShadowBufPutIndex_GH100
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    NvU32              gfid,
    FAULT_BUFFER_TYPE  type,
    NvU32              putIndex
)
{
}

/*
 * @brief Copies a single fault packet from the replayable/non-replayable
 *        HW fault buffer to the corresponding client shadow buffer
 *
 * @param[in]  pFaultBuffer        Pointer to GMMU_FAULT_BUFFER
 * @param[in]  type                Replayable/Non-replayable fault type
 * @param[in]  getIndex            Get pointer of the HW fault buffer
 * @param[in]  shadowBufPutIndex   Put pointer of the shadow buffer
 * @param[in]  maxBufferEntries    Maximum possible entries in the HW buffer
 * @param[in]  pThreadState        Pointer to THREAD_STATE_NODE
 * @param[out] pFaultsCopied       Number of fault packets copied by the function
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuCopyFaultPacketToClientShadowBuffer_GH100
(
    OBJGPU                   *pGpu,
    KernelGmmu               *pKernelGmmu,
    struct GMMU_FAULT_BUFFER *pFaultBuffer,
    FAULT_BUFFER_TYPE         type,
    NvU32                     getIndex,
    NvU32                     shadowBufPutIndex,
    NvU32                     maxBufferEntries,
    THREAD_STATE_NODE        *pThreadState,
    NvU32                    *pFaultsCopied
)
{
    struct HW_FAULT_BUFFER *pHwFaultBuffer = NULL;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf = NULL;
    GMMU_FAULT_PACKET faultPacket;
    NvU32 faultPacketsPerPage;
    NvU32 faultPacketPageIndex;
    NvU32 faultPacketPageOffset;
    void *pSrc;
    NvU8 *pDst;
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    NV_STATUS status;
    NvU8 *pDstMetadata;
    NvU32 metadataStartIndex;
    NvU32 metadataPerPage;
    NvU32 metadataPageIndex;
    NvU32 metadataPageOffset;
    NvU8  validBit = 1;
    void *pCslCtx = NULL;

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        return kgmmuCopyFaultPacketToClientShadowBuffer_GV100(pGpu, pKernelGmmu,
                                                              pFaultBuffer,
                                                              type,
                                                              getIndex,
                                                              shadowBufPutIndex,
                                                              maxBufferEntries,
                                                              pThreadState,
                                                              pFaultsCopied);
    }

    *pFaultsCopied = 0;

    pHwFaultBuffer = &pFaultBuffer->hwFaultBuffers[type];
    pClientShadowFaultBuf = pFaultBuffer->pClientShadowFaultBuffer[type];

    // Read the fault packet from HW buffer
    pSrc = kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pHwFaultBuffer, getIndex);
    portMemCopy(&faultPacket, sizeof(GMMU_FAULT_PACKET), pSrc, sizeof(GMMU_FAULT_PACKET));

    //
    // The following is the sequence to be followed for replayable faults
    // as per production design when Hopper CC is enabled
    //
    if (type == REPLAYABLE_FAULT_BUFFER)
    {
        NvU32 nextGetIndex;

        kgmmuFaultBufferClearPackets_HAL(pGpu, pKernelGmmu, pHwFaultBuffer, getIndex, 1);

        //
        // Ensure all writes to the current entry are completed before updating the
        // GET pointer.
        //
        portAtomicMemoryFenceStore();

        nextGetIndex = (getIndex + 1) % maxBufferEntries;

        // Update cached GET to a valid value.
        pHwFaultBuffer->cachedGetIndex = nextGetIndex;

        // Increment the GET pointer to enable HW to write new fault packets
        kgmmuWriteFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, type, pHwFaultBuffer->cachedGetIndex, pThreadState);

        // Check if there is space in the shadow buffer
        if (kgmmuIsReplayableShadowFaultBufferFull_HAL(pGpu, pKernelGmmu,
                                                       pClientShadowFaultBuf,
                                                       shadowBufPutIndex,
                                                       maxBufferEntries))
        {
            // The design allows the SW Repalyable shadow fault buffer to overflow.
            return NV_OK;
        }
    }

    faultPacketsPerPage = RM_PAGE_SIZE / sizeof(GMMU_FAULT_PACKET);
    faultPacketPageIndex = shadowBufPutIndex / faultPacketsPerPage;
    faultPacketPageOffset = shadowBufPutIndex % faultPacketsPerPage;

    pDst = KERNEL_POINTER_FROM_NvP64(NvU8 *,
               pClientShadowFaultBuf->pBufferPages[faultPacketPageIndex].pAddress);
    pDst += (faultPacketPageOffset * sizeof(GMMU_FAULT_PACKET));

    //
    // Metadata is packed at the end of the buffer.
    // Calculate the page index and offset at which RM needs to fill the metadata
    // and copy it over.
    //
    metadataStartIndex = pClientShadowFaultBuf->metadataStartIndex;
    metadataPerPage = RM_PAGE_SIZE / sizeof(GMMU_FAULT_PACKET_METADATA);
    metadataPageIndex = shadowBufPutIndex / metadataPerPage;
    metadataPageOffset = shadowBufPutIndex % faultPacketsPerPage;

    pDstMetadata = KERNEL_POINTER_FROM_NvP64(NvU8 *,
                   pClientShadowFaultBuf->pBufferPages[metadataStartIndex + metadataPageIndex].pAddress);
    pDstMetadata += (metadataPageOffset * sizeof(GMMU_FAULT_PACKET_METADATA));

    // Sanity check client reset the Valid bit.
    if (pDstMetadata[GMMU_FAULT_PACKET_METADATA_VALID_IDX] != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Plaintext valid bit not reset by client.\n");
        return NV_ERR_INVALID_STATE;
    }

    pCslCtx = kgmmuGetShadowFaultBufferCslContext(pGpu, pKernelGmmu, type);
    if (pCslCtx == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "CSL context for type 0x%x unexpectedtly NULL\n", type);
        return NV_ERR_INVALID_STATE;
    }

    status = ccslEncryptWithRotationChecks(pCslCtx,
                                           sizeof(GMMU_FAULT_PACKET),
                                           (NvU8*) &faultPacket,
                                           &validBit,
                                           GMMU_FAULT_PACKET_METADATA_VALID_SIZE,
                                           pDst,
                                           &pDstMetadata[GMMU_FAULT_PACKET_METADATA_AUTHTAG_IDX]);
    if (status != NV_OK)
    {
        if (status == NV_ERR_INSUFFICIENT_RESOURCES)
        {
            // IV overflow is considered fatal.
            NV_PRINTF(LEVEL_ERROR, "Fatal error detected in fault buffer packet encryption: IV overflow!\n");
            confComputeSetErrorState(pGpu, pConfCompute);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Error detected in fault buffer packet encryption: 0x%x\n", status);
        }
        return status;
    }

    //
    // Ensure that the encrypted packet and authTag have reached point of coherence
    // before writing the plaintext valid bit.
    //
    portAtomicMemoryFenceStore();

    // Write the valid bit and increment the number of faults copied.
    portMemCopy((void*)&pDstMetadata[GMMU_FAULT_PACKET_METADATA_VALID_IDX],
                GMMU_FAULT_PACKET_METADATA_VALID_SIZE,
                &validBit,
                GMMU_FAULT_PACKET_METADATA_VALID_SIZE);

    *pFaultsCopied = 1;

    return NV_OK;
}

/*
 * @brief Checks if the client shadow buffer has space
 *
 * @param[in]  pClientShadowFaultBuf  Pointer to the shadow buffer
 * @param[in]  shadowBufPutIndex      Put index inside shadow buffer
 * @param[in]  maxBufferEntries       Maximum possible entries in the HW buffer
 *
 * @returns NV_TRUE/NV_FALSE
 */
NvBool
kgmmuIsReplayableShadowFaultBufferFull_GH100
(
    OBJGPU                          *pGpu,
    KernelGmmu                      *pKernelGmmu,
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf,
    NvU32                            shadowBufPutIndex,
    NvU32                            maxBufferEntries
)
{
    FAULT_BUFFER_SHARED_MEMORY *pFaultBufSharedMem;

    pFaultBufSharedMem =
        KERNEL_POINTER_FROM_NvP64(FAULT_BUFFER_SHARED_MEMORY *,
                        pClientShadowFaultBuf->pFaultBufferSharedMemoryAddress);

    return (pFaultBufSharedMem->swGetIndex ==
            ((shadowBufPutIndex + 1) % maxBufferEntries)) ? NV_TRUE : NV_FALSE;
}

/*!
 * @brief Get the engine ID associated with the min CE
 *
 * @param[in] pKenrelGmmu  KernelGmmu object
 *
 * return engine ID of the min CE
 */
NvU32
kgmmuGetMinCeEngineId_GH100
(
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_CE0;
}

/*!
 * @brief Get the engine ID associated with the max CE
 *
 * @param[in] pGpu         OBJGPU object
 * @param[in] pKenrelGmmu  KernelGmmu object
 *
 * return engine ID of the max CE
 */
NvU32
kgmmuGetMaxCeEngineId_GH100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_CE9;
}

/**
  * @brief  Sign extend a fault address to a supported width as per UVM requirements
  */
void
kgmmuSignExtendFaultAddress_GH100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU64      *pMmuFaultAddress
)
{
    NvU32 cpuAddrShift   = osGetCpuVaAddrShift();
    NvU32 gpuVaAddrShift = portUtilCountTrailingZeros64(pKernelGmmu->maxVASize);

    // Sign extend VA to ensure it's in canonical form if required
    if (gpuVaAddrShift >= cpuAddrShift)
    {
        switch (pGpu->busInfo.oorArch)
        {
            case OOR_ARCH_X86_64:
            case OOR_ARCH_ARM:
            case OOR_ARCH_AARCH64:
                *pMmuFaultAddress = (NvU64)(((NvS64)*pMmuFaultAddress << (64 - 57)) >>
                                            (64 - 57));
                break;
            case OOR_ARCH_PPC64LE:
                break;
            case OOR_ARCH_NONE:
                NV_ASSERT_FAILED("Invalid oor address mode type.");
                break;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "UVM has not defined what to do here, doing nothing\n");
        NV_ASSERT(0);
    }
}

/*
 * @brief Program PD2-PD4 fake sparse tables with fake sparse entries.
 *      Sparse entries work correctly PDE1 and below. So PD2 contains true (not fake) sparse entries
 *
 * @param[in] pKernelGmmu           KernGmmu object
 * @param[in] bufferBaseOffset      Base offset of fake sparse tables buffer
 * @param[in] pTablesBasePtr        Base pointer to mapped fake sparse tables buffer
 *
 * @return NV_STATUS
 */
static NV_STATUS
kgmmuFillFakeSparseTables
(
    KernelGmmu* pKernelGmmu,
    NvU64 bufferBaseOffset,
    volatile NvU64 *pTablesBasePtr
)
{
    GMMU_FMT_FAMILY *pFam = pKernelGmmu->pFmtFamilies[GMMU_FMT_VERSION_3 - 1];
    GMMU_ENTRY_VALUE templateFakeEntry;
    const GMMU_FIELD_ADDRESS *pFldAddr;

    // Intentionally set pdePcf to 0 to create a valid PDE mapping.
    NvU32 templatePdePcfSw = 0;
    NvU32 templatePdePcfHw = 0;
    NvU8 i;

    NV_ASSERT_OR_RETURN(pFam != NULL, NV_ERR_INVALID_STATE);
    pFldAddr = gmmuFmtPdePhysAddrFld(&pFam->pde, GMMU_APERTURE_VIDEO);

    NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, templatePdePcfSw, &templatePdePcfHw) == NV_OK),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize Fake PDE entry attrs without address
    portMemSet(templateFakeEntry.v8, 0, NV_MMU_VER3_PDE__SIZE);
    nvFieldSet32(&pFam->pde.fldPdePcf, templatePdePcfHw, templateFakeEntry.v8);
    gmmuFieldSetAperture(&pFam->pde.fldAperture, GMMU_APERTURE_VIDEO, templateFakeEntry.v8);

    for (i = 0; i < NV_GMMU_FAKE_SPARSE_TABLE_LEVELS; i++)
    {
        // Pointer to base of mapped page directory
        volatile NvU64 *pTablePtr = (volatile NvU64 *)((NvU64)pTablesBasePtr + i * RM_PAGE_SIZE);
        // Base vidmem offset of page directory
        NvU64 pdBaseOffset = bufferBaseOffset + i * RM_PAGE_SIZE;
        // Fake sparse PDE to fill page directory. Points to next level of page directory.
        GMMU_ENTRY_VALUE fillEntry;
        // Current level PDE value. Filled into previous level page directory.
        GMMU_ENTRY_VALUE entry;
        NvU32 j;

        portMemCopy(entry.v8, NV_MMU_VER3_PDE__SIZE, templateFakeEntry.v8, NV_MMU_VER3_PDE__SIZE);

        if (i == 0)
        {
            NvU32 pdePcfSw = 0;
            NvU32 pdePcfHw = 0;

            // PD2 has true sparse entries as PDE1 sparse translation is not broken in HW
            // TODO: Bug 4615812: Handle fake sparse PD2 creation for ATS
            pdePcfSw |= (1 << SW_MMU_PCF_SPARSE_IDX);
            NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK),
                NV_ERR_INVALID_ARGUMENT);

            portMemSet(fillEntry.v8, 0, NV_MMU_VER3_PDE__SIZE);
            gmmuFieldSetAperture(&pFam->pde.fldAperture, GMMU_APERTURE_INVALID, fillEntry.v8);
            nvFieldSet32(&pFam->pde.fldPdePcf, pdePcfHw, fillEntry.v8);
        }
        else
        {
            NvU64 nextLevelPD = pKernelGmmu->fakeSparseEntry[i-1];
            portMemCopy(fillEntry.v8, NV_MMU_VER3_PDE__SIZE, templateFakeEntry.v8, NV_MMU_VER3_PDE__SIZE);

            // Update PDE address field to point to next level fake PD
            gmmuFieldSetAddress(pFldAddr, nextLevelPD, fillEntry.v8);
        }

        gmmuFieldSetAddress(pFldAddr, pdBaseOffset, entry.v8);
        pKernelGmmu->fakeSparseEntry[i] = entry.v64[0];

        for (j = 0; j < RM_PAGE_SIZE / NV_GMMU_FAKE_SPARSE_TABLE_ENTRY_SIZE; j++)
        {
            pTablePtr[j] = fillEntry.v64[0];
        }
    }

    return NV_OK;
}

/**
 * @brief Create BAR0 mapping and program fake sparse tables.
 *
 * @param[in] pGpu          OBJGPU object
 * @param[in] pKernelGmmu   KernGmmu object
 *
 * @return NV_STATUS
 */
NV_STATUS
kgmmuCreateFakeSparseTablesInternal_KERNEL
(
    OBJGPU *pGpu,
    KernelGmmu* pKernelGmmu
)
{
    NvU64 bufferBaseOffset = pKernelGmmu->pFakeSparseBuffer->_pteArray[0];
    volatile NvU64 *pTablesBasePtr;
    NV_STATUS status;

    KernelBus *pKernelBus       = GPU_GET_KERNEL_BUS(pGpu);
    NvU64 origBar0Mapping       = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
    NvU64 bar0WindowSize        = pKernelBus->physicalBar0WindowSize;
    NvU8 *pBar0WindowAddress    = pKernelBus->pDefaultBar0Pointer;
    NvU64 bufBar0OffsetMask     = bar0WindowSize - 1;

    pTablesBasePtr = (volatile NvU64*) &pBar0WindowAddress[bufferBaseOffset & bufBar0OffsetMask];

    NV_ASSERT_OK_OR_RETURN(kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bufferBaseOffset & ~bufBar0OffsetMask));

    status = kgmmuFillFakeSparseTables(pKernelGmmu, bufferBaseOffset, pTablesBasePtr);

    NV_ASSERT(kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, origBar0Mapping) == NV_OK);

    return status;
}

/**
 * @brief Create PCI mapping and program fake sparse tables.
 *
 * @param[in] pGpu          OBJGPU object
 * @param[in] pKernelGmmu   KernGmmu object
 *
 * @return NV_STATUS
 */
NV_STATUS
kgmmuCreateFakeSparseTablesInternal_PHYSICAL
(
    OBJGPU *pGpu,
    KernelGmmu* pKernelGmmu
)
{
    NvU64 bufferBaseOffset = pKernelGmmu->pFakeSparseBuffer->_pteArray[0];
    volatile NvU64 *pTablesBasePtr;
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(osMapPciMemoryKernel64(pGpu, bufferBaseOffset,
        RM_PAGE_SIZE * NV_GMMU_FAKE_SPARSE_TABLE_LEVELS, NV_PROTECT_READ_WRITE, (NvP64 *)&pTablesBasePtr,
        NV_MEMORY_UNCACHED));

    status = kgmmuFillFakeSparseTables(pKernelGmmu, bufferBaseOffset, pTablesBasePtr);

    osUnmapPciMemoryKernel64(pGpu, (NvP64) pTablesBasePtr);

    return status;
}

/**
 * @brief On Hopper, a HW bug in the GMMU causes sparse PDEs to not function correctly in CC mode.
 *      To emulate sparse correctly, pre-allocate 3 levels of fake sparse tables. See Bug 3341692 for details.
 *
 * @param[in] pGpu          OBJGPU object
 * @param[in] pKernelGmmu   KernGmmu object
 *
 * @return NV_STATUS
 */
NV_STATUS kgmmuCreateFakeSparseTables_GH100
(
    OBJGPU *pGpu,
    KernelGmmu* pKernelGmmu
)
{
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(memdescCreate(&pKernelGmmu->pFakeSparseBuffer, pGpu,
        RM_PAGE_SIZE * NV_GMMU_FAKE_SPARSE_TABLE_LEVELS, RM_PAGE_SIZE, NV_TRUE, ADDR_FBMEM, NV_MEMORY_WRITECOMBINED,
        MEMDESC_FLAGS_NONE));
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pKernelGmmu->pFakeSparseBuffer), error_alloc);

    NV_ASSERT_OK_OR_GOTO(status, kgmmuCreateFakeSparseTablesInternal(pGpu, pKernelGmmu), error);

    return NV_OK;
error:
    memdescFree(pKernelGmmu->pFakeSparseBuffer);
error_alloc:
    memdescDestroy(pKernelGmmu->pFakeSparseBuffer);
    pKernelGmmu->pFakeSparseBuffer = NULL;
    return status;
}

/**
 * @brief Get fake sparse entry for page directories.
 *
 * @param[in] pGpu          OBJGPU object
 * @param[in] pKernelGmmu   KernGmmu object
 * @param[in] pLevelFmt     Format of the level
 *
 * @return NvU8* pointer to sparse entry if relevant, NULL otherwise
 */
NvU8 *kgmmuGetFakeSparseEntry_GH100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    const MMU_FMT_LEVEL *pLevelFmt
)
{
    NvU32 level = 0;

    // Fake sparse entry is needed only if hopper in CC mode & PDE levels 2-4
    if (pKernelGmmu->pFakeSparseBuffer == NULL || (pLevelFmt->virtAddrBitLo < NV_GMMU_FAKE_SPARSE_TABLE_LEVEL_LO))
        return NULL;

    // Level index with PDE2 as index 0
    level = (pLevelFmt->virtAddrBitLo - NV_GMMU_FAKE_SPARSE_TABLE_LEVEL_LO) / NV_GMMU_FAKE_SPARSE_TABLE_BITS_PER_LEVEL;

    return (NvU8 *) &pKernelGmmu->fakeSparseEntry[level];
}
