/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "published/blackwell/gb100/dev_top.h"
#include "published/blackwell/gb100/dev_vm.h"
#include "published/blackwell/gb100/dev_fault.h"

/*!
 * @brief Commit the invalidate command to H/W.
 *
 * @param pGpu
 * @param pKernelGmmu
 * @param pParams      Pointer to TLB_INVALIDATE_PARAMS data.
 */
NV_STATUS
kgmmuCommitTlbInvalidate_GB100
(
    OBJGPU                *pGpu,
    KernelGmmu            *pKernelGmmu,
    TLB_INVALIDATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;

    if (!FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_PDB, _TRUE, pParams->regVal))
    {
        kgmmuSetPdbToInvalidate_HAL(pGpu, pKernelGmmu, pParams);
    }

    {
        NV_ASSERT(IS_GFID_PF(pParams->gfid));

        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE, pParams->regVal);
    }

    // Wait for the invalidate command to complete.
    status = kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &pParams->timeout, pParams->gfid);

    return status;
}

/**
 * @brief Converts a MMU fault type (NV_PFAULT_FAULT_TYPE_*) into a string.
 *
 * @param[in] faultType NV_PFAULT_FAULT_TYPE_*
 *
 * @returns a string (always non-null)
 */
const char *
kgmmuGetFaultTypeString_GB100(KernelGmmu *pKernelGmmu, NvU32 faultType)
{
    switch (faultType)
    {
        case NV_PFAULT_FAULT_TYPE_PDE:
            return "FAULT_PDE";
        case NV_PFAULT_FAULT_TYPE_PDE_SIZE:
            return "FAULT_PDE_SIZE";
        case NV_PFAULT_FAULT_TYPE_PTE:
            return "FAULT_PTE";
        case NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION:
            return "FAULT_VA_LIMIT_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK:
            return "FAULT_UNBOUND_INST_BLOCK";
        case NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION:
            return "FAULT_PRIV_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            return "FAULT_RO_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION:
            return "FAULT_PITCH_MASK_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_WORK_CREATION:
            return "FAULT_WORK_CREATION";
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE:
            return "FAULT_UNSUPPORTED_APERTURE";
        case NV_PFAULT_FAULT_TYPE_CC_VIOLATION:
            return "FAULT_CC_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND:
            return "FAULT_INFO_TYPE_UNSUPPORTED_KIND";
        case NV_PFAULT_FAULT_TYPE_REGION_VIOLATION:
            return "FAULT_INFO_TYPE_REGION_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_POISONED:
            return "FAULT_INFO_TYPE_POISONED";
        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            return "FAULT_INFO_TYPE_ATOMIC_VIOLATION";
        default:
            return "UNRECOGNIZED_FAULT";
    }
}

/*!
 * @brief Find and save the CE MMU fault ID range
 *
 * @param[in]   pGpu
 * @param[in]   pKernelGmmu
 *
 * @return NV_OK if there is no error
 *         Or NV_ERR
 */
NV_STATUS
kgmmuInitCeMmuFaultIdRange_GB100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NvU32 i;
    NvU32 minMmuFaultId = (NvU32)-1;
    NvU32 maxMmuFaultId = 0;

    // Init the value in case the function fails
    pKernelGmmu->minCeMmuFaultId = 0;
    pKernelGmmu->maxCeMmuFaultId = 0;

    NV_ASSERT_OK_OR_RETURN(gpuConstructDeviceInfoTable_HAL(pGpu));

    for (i = 0; i < pGpu->numDeviceInfoEntries; i++)
    {
        if (pGpu->pDeviceInfoTable[i].typeEnum == NV_PTOP_DEVICE_INFO2_DEV_TYPE_ENUM_LCE)
        {
            minMmuFaultId = NV_MIN(minMmuFaultId, pGpu->pDeviceInfoTable[i].faultId);
            maxMmuFaultId = NV_MAX(maxMmuFaultId, pGpu->pDeviceInfoTable[i].faultId);
        }
    }

    if ((minMmuFaultId == (NvU32)-1 || maxMmuFaultId == 0))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find any MMU Fault ID\n");
        NV_ASSERT(0);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pKernelGmmu->minCeMmuFaultId = minMmuFaultId;
    pKernelGmmu->maxCeMmuFaultId = maxMmuFaultId;

    NV_PRINTF(LEVEL_INFO, "CE MMU Fault ID range [0x%x - 0x%x]\n",
                           pKernelGmmu->minCeMmuFaultId, pKernelGmmu->maxCeMmuFaultId);

    return NV_OK;
}

/*!
 * @brief Get the min CE MMU fault ID
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 *
 * return min CE MMU fault ID
 */
NvU32
kgmmuGetMinCeEngineId_GB100
(
    KernelGmmu *pKernelGmmu
)
{
    return pKernelGmmu->minCeMmuFaultId;
}

/*!
 * @brief Get the max CE MMU fault ID
 *
 * @param[in] pGpu         OBJGPU object
 * @param[in] pKernelGmmu  KernelGmmu object
 *
 * return max CE MMU fault ID
 */
NvU32
kgmmuGetMaxCeEngineId_GB100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    return pKernelGmmu->maxCeMmuFaultId;
}

/*!
 * @brief Parses Faultbuffer entry and returns Fault Type
 *
 * @param[in]  fault          Fault Value
 * @param[out] pMmuFaultType  Fault Type
 *
 * @returns
 */
NV_STATUS
kgmmuGetFaultType_GB100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       fault,
    FAULT_TYPE *pMmuFaultType
)
{
    NV_ASSERT_OR_RETURN(pMmuFaultType != NULL, NV_ERR_INVALID_POINTER);
    switch (fault)
    {
        case NV_PFAULT_FAULT_TYPE_PDE:
            *pMmuFaultType = fault_invalidPde;
            break;

        case NV_PFAULT_FAULT_TYPE_PDE_SIZE:
            *pMmuFaultType = fault_invalidPdeSize;
            break;

        case NV_PFAULT_FAULT_TYPE_PTE:
            *pMmuFaultType = fault_invalidPte;
            break;

        case NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION:
            *pMmuFaultType = fault_limitViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK:
            *pMmuFaultType = fault_unboundInstBlock;
            break;

        case NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION:
            *pMmuFaultType = fault_privViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            *pMmuFaultType = fault_write;
            break;

        case NV_PFAULT_FAULT_TYPE_WO_VIOLATION:
            *pMmuFaultType = fault_read;
            break;

        case NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION:
            *pMmuFaultType = fault_pitchMaskViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_WORK_CREATION:
            *pMmuFaultType = fault_workCreation;
            break;

        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE:
            *pMmuFaultType = fault_unsupportedAperture;
            break;

        case NV_PFAULT_FAULT_TYPE_CC_VIOLATION:
            *pMmuFaultType = fault_cc_violation;
            break;

        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND:
            *pMmuFaultType = fault_unsupportedKind;
            break;

        case NV_PFAULT_FAULT_TYPE_REGION_VIOLATION:
            *pMmuFaultType = fault_regionViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_POISONED:
            *pMmuFaultType = fault_poison;
            break;

        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            *pMmuFaultType = fault_atomic;
            break;

        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NvU32
kgmmuReadClientShadowBufPutIndex_GB100
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
kgmmuWriteClientShadowBufPutIndex_GB100
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    NvU32              gfid,
    FAULT_BUFFER_TYPE  type,
    NvU32              putIndex
)
{
}
