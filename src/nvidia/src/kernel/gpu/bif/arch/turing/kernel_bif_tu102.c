/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------- System Includes -------------------------------- */
#include "gpu/bif/kernel_bif.h"
#include "gpu/gpu.h"

#define NV_VGPU_EMU                           0x0000FFFF:0x0000F000 /* RW--D */

#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#define NUM_VF_SPARSE_MMAP_REGIONS     2

#include "published/turing/tu102/dev_nv_xve.h"
#include "published/turing/tu102/dev_vm.h"


/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief Check if MSIX is enabled in HW
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  True if MSIX enabled else False
 */
NvBool
kbifIsMSIXEnabledInHW_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    if (IS_VIRTUAL(pGpu))
    {
        // SR-IOV guests only support MSI-X
        return IS_VIRTUAL_WITH_SRIOV(pGpu);
    }
    else
    {
        if (GPU_BUS_CFG_RD32(pGpu, NV_XVE_MSIX_CAP_HDR, &data32) != NV_OK)
        {
            NV_ASSERT_FAILED("Unable to read NV_XVE_MSIX_CAP_HDR\n");
            return NV_FALSE;
        }
        return FLD_TEST_DRF(_XVE, _MSIX_CAP_HDR, _ENABLE, _ENABLED, data32);
    }
}

/*!
 * @brief Disables P2P reads/writes on VF
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifDisableP2PTransactions_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED, NV_TRUE);
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_TRUE);
    }
}

/*!
 * @brief Get the number of sparse mmap regions
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[out] numAreas    Number of sparse mmap regions
 *
 * @returns    NV_OK                   If all args are proper
 *             NV_ERR_INVALID_ARGUMENT In case of erroneous args
 */

NV_STATUS
kbifGetNumVFSparseMmapRegions_TU102
(
    OBJGPU                  *pGpu,
    KernelBif               *pKernelBif,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU32                   *numAreas
)
{
    if (numAreas && pKernelHostVgpuDevice != NULL)
    {
        NvU32 maxInstance = 0;
        NV_STATUS status;

        status = kvgpumgrGetMaxInstanceOfVgpu(pKernelHostVgpuDevice->vgpuType, &maxInstance);
        if (status != NV_OK)
            goto exit;

        if (maxInstance != 1 && pGpu->getProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE))
            *numAreas = NUM_VF_SPARSE_MMAP_REGIONS + 1;     // +1 for TLB invalidation
        else
            *numAreas = NUM_VF_SPARSE_MMAP_REGIONS;

        return NV_OK;
    }

exit:
    return NV_ERR_INVALID_ARGUMENT;
}

NV_STATUS
kbifGetVFSparseMmapRegions_TU102
(
    OBJGPU                  *pGpu,
    KernelBif               *pKernelBif,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64                   os_page_size,
    NvU64                   *offsets,
    NvU64                   *sizes)
{
    NvBool bEmulateVfTlbInvalidation = pGpu->getProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE);

    if (offsets && sizes && pKernelHostVgpuDevice != NULL)
    {
        NvU32 maxInstance = 0;
        NV_STATUS status;

        status = kvgpumgrGetMaxInstanceOfVgpu(pKernelHostVgpuDevice->vgpuType, &maxInstance);
        if (status != NV_OK)
            return NV_ERR_INVALID_ARGUMENT;

        if (gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
        {
            // For SRIOV heavy trap BOOT_* and BAR setup registers in plugin
            offsets[0] = 0 + os_page_size;                                                      // 0x1000 ==> 0x2FFF
            if (maxInstance > 1 && bEmulateVfTlbInvalidation)
            {
                sizes[0]   = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0) - offsets[0];
            }
            else
            {
                sizes[0]   = DRF_BASE(NV_VGPU_EMU) - offsets[0];
            }
        }
        else
        {
            offsets[0] = 0;                                                                     // 0x0000 => 0x2FFF
            if (maxInstance > 1 && bEmulateVfTlbInvalidation)
            {
                sizes[0]   = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0);
            }
            else
            {
                sizes[0]   = DRF_BASE(NV_VGPU_EMU);
            }
        }

        if (hypervisorIsType(OS_HYPERVISOR_HYPERV))
        {
            // For Hyperv, we need to mitigate RPC page
            if (maxInstance > 1 && bEmulateVfTlbInvalidation)
            {
                offsets[1] = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0) + os_page_size;    // 0x4000 ==> 0xEFFF
                sizes[1]   = DRF_BASE(NV_VGPU_EMU) - offsets[1];

                offsets[2] =  DRF_BASE(NV_VGPU_EMU) + os_page_size;                             // 0x10000 ==> 0x40000
                sizes[2]   = pGpu->sriovState.vfBarSize[0] - offsets[2];
            }
            else
            {
                offsets[1] = DRF_BASE(NV_VGPU_EMU) + os_page_size;                              // 0x10000 ==> 0x40000
                sizes[1]   = pGpu->sriovState.vfBarSize[0] - offsets[1];
            }
        }
        else
        {
            if (maxInstance > 1 && bEmulateVfTlbInvalidation)
            {
                offsets[1] = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0) + os_page_size;    // 0x4000 ==> 0xEFFF
                sizes[1]   = DRF_BASE(NV_VGPU_EMU) - offsets[1];

                offsets[2] = NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0) + os_page_size;     // 0x11000 ==> 0x40000
                sizes[2]   = pGpu->sriovState.vfBarSize[0] - offsets[2];
            }
            else
            {
                offsets[1] = NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0) + os_page_size;     // 0x11000 ==> 0x40000
                sizes[1]   = pGpu->sriovState.vfBarSize[0] - offsets[1];
            }

            // Assert whenever the MSI-X table page is not immediately after
            // the NV_VGPU_EMU page, as it will break the current assumption.
            NV_ASSERT((DRF_BASE(NV_VGPU_EMU) + DRF_SIZE(NV_VGPU_EMU)) ==
                      NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0));
        }
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}
