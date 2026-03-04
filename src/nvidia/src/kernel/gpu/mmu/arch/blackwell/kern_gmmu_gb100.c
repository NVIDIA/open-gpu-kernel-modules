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
#include "gpu/uvm/uvm.h"

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

    NV_ASSERT_OR_RETURN(IS_GFID_PF(pParams->gfid), NV_ERR_INVALID_ARGUMENT);

    if (!FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_PDB, _TRUE, pParams->regVal))
    {
        kgmmuSetPdbToInvalidate_HAL(pGpu, pKernelGmmu, pParams);
    }

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE, pParams->regVal);

    // Wait for the invalidate command to complete.
    status = kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &pParams->timeout);

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

/*!
 *  @brief Checks whether the access counter reset sequence
 *         should be performed on BAR2 fault servicing
 *
 * @returns NvBool indicating whether servicing should be performed.
 */
NvBool
kgmmuCheckAccessCounterBar2FaultServicingState_GB100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);
    NvU32 i;

    for (i = 0; i < pUvm->accessCounterBufferCount; i++)
    {
        if (uvmIsAccessCntrBufferEnabled_HAL(pGpu, pUvm, i))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

