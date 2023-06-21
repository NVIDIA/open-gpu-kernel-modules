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
#include "gpu/bus/kern_bus.h"
#include "gpu/gpu.h"

#define NV_VGPU_EMU                           0x0000FFFF:0x0000F000 /* RW--D */

#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

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

NV_STATUS
kbifGetVFSparseMmapRegions_TU102
(
    OBJGPU                  *pGpu,
    KernelBif               *pKernelBif,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64                   osPageSize,
    NvU32                   *pNumAreas,
    NvU64                   *pOffsets,
    NvU64                   *pSizes
)
{
    NvU64 offsetStart = 0;
    NvU64 offsetEnd = 0;
    NvU32 idx = 0;
    NvU32 maxInstance;
    NvU32 i;
    NvBool bDryRun;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelHostVgpuDevice != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pNumAreas != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          kvgpumgrGetMaxInstanceOfVgpu(pKernelHostVgpuDevice->vgpuType,
                                                       &maxInstance));

    // Dry run to calculate the total number of areas
    bDryRun = ((pOffsets == NULL) || (pSizes == NULL));
    if (bDryRun)
    {
        pOffsets = portMemAllocStackOrHeap(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES * sizeof(pOffsets[0]));
        pSizes = portMemAllocStackOrHeap(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES * sizeof(pSizes[0]));
    }

    // For SRIOV heavy, trap BOOT_0 page
    if (gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        offsetStart = osPageSize;
    }

    // For VF TLB emulation, trap MMU FAULT BUFFER page
    if ((maxInstance > 1) && pGpu->getProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE))
    {
        offsetEnd = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0);
        pOffsets[idx] = offsetStart;
        pSizes[idx] = offsetEnd - offsetStart;
        idx++;

        offsetStart = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0) + osPageSize;
    }

    // For non-GSP, trap VGPU_EMU page
    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        offsetEnd = DRF_BASE(NV_VGPU_EMU);
        pOffsets[idx] = offsetStart;
        pSizes[idx] = offsetEnd - offsetStart;
        idx++;

        offsetStart = DRF_BASE(NV_VGPU_EMU) + osPageSize;
    }

    // For non-HyperV, trap MSI-X table page
    if (!hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {
        // Assert whenever the MSI-X table page is not immediately after
        // the NV_VGPU_EMU page, as it will break the current assumption.
        ct_assert((DRF_BASE(NV_VGPU_EMU) + DRF_SIZE(NV_VGPU_EMU)) == NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0));

        offsetEnd = NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0);

        // Since MSI-X page is immediately after VGPU_EMU, if both are
        // trapped, skip creating a 0 size region in between
        if (offsetEnd > offsetStart)
        {
            pOffsets[idx] = offsetStart;
            pSizes[idx] = offsetEnd - offsetStart;
            idx++;
        }

        offsetStart = NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(0) + osPageSize;
    }

    offsetEnd = pGpu->sriovState.vfBarSize[0];
    pOffsets[idx] = offsetStart;
    pSizes[idx] = offsetEnd - offsetStart;
    idx++;

    if (bDryRun)
    {
        portMemFreeStackOrHeap(pOffsets);
        portMemFreeStackOrHeap(pSizes);
    }
    else
    {
        // It might be too late to check if the passed arrays are big enough,
        // but better late than never
        NV_ASSERT_OR_RETURN(idx <= *pNumAreas, NV_ERR_FATAL_ERROR);

        for (i = 0; i < idx; i++)
        {
            NV_PRINTF(LEVEL_INFO, "VF Sparse Mmap Region[%u] range 0x%llx - 0x%llx, size 0x%llx\n",
                    i, pOffsets[i], pOffsets[i] + pSizes[i], pSizes[i]);
        }
    }

    *pNumAreas = idx;
    return NV_OK;
}

/*! @brief Fetch VF details such as no. of VFs, First VF offset etc
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
*/
void
kbifCacheVFInfo_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status = NV_OK;
    NvU32     regVal = 0;
    NvU32     saveLo = 0;
    NvU32     saveHi = 0;

    // Get total VF count
    GPU_BUS_CFG_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR3, &regVal);
    pGpu->sriovState.totalVFs = GPU_DRF_VAL(_XVE, _SRIOV_CAP_HDR3,
                                            _TOTAL_VFS, regVal);

    // Get first VF offset
    GPU_BUS_CFG_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR5, &regVal);
    pGpu->sriovState.firstVFOffset = GPU_DRF_VAL(_XVE, _SRIOV_CAP_HDR5,
                                                 _FIRST_VF_OFFSET, regVal);

    // Get VF BAR0 first address
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR9, &saveLo);
    NV_ASSERT(status == NV_OK);
    pGpu->sriovState.firstVFBarAddress[0] = saveLo & 0xFFFFFFF0;

    // Get VF BAR1 first address
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR10, &saveLo);
    NV_ASSERT(status == NV_OK);
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR11_VF_BAR1_HI, &saveHi);
    NV_ASSERT(status == NV_OK);
    pGpu->sriovState.firstVFBarAddress[1] = (((NvU64)saveHi) << 32) + (saveLo & 0xFFFFFFF0);

    // Get VF BAR2 first address
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR12, &saveLo);
    NV_ASSERT(status == NV_OK);
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR13_VF_BAR2_HI, &saveHi);
    NV_ASSERT(status == NV_OK);
    pGpu->sriovState.firstVFBarAddress[2] = (((NvU64)saveHi) << 32) + (saveLo & 0xFFFFFFF0);

    // Get if VF BARs are 64 bit addressable
    regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_SRIOV_CAP_HDR10);
    pGpu->sriovState.b64bitVFBar1 = IS_BAR_64(regVal);
    regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_SRIOV_CAP_HDR12);
    pGpu->sriovState.b64bitVFBar2 = IS_BAR_64(regVal);
}
