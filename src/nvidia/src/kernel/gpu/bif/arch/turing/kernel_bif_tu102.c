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
#include "platform/chipset/chipset.h"

#define NV_VGPU_EMU                           0x0000FFFF:0x0000F000 /* RW--D */

#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "published/turing/tu102/dev_nv_xve.h"
#include "published/turing/tu102/dev_vm.h"

#include "published/turing/tu102/dev_nv_pcfg_xve_regmap.h"

// XVE register map for PCIe config space
static const NvU32 xveRegMapValid[] = NV_PCFG_XVE_REGISTER_VALID_MAP;
static const NvU32 xveRegMapWrite[] = NV_PCFG_XVE_REGISTER_WR_MAP;

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

    // Trap MSI-X table page for non-Hyperv and Hyperv non-GSP cases
    if (!hypervisorIsType(OS_HYPERVISOR_HYPERV)
       )
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

/*!
 * @brief Check and cache Function level reset support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCacheFlrSupport_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CAPABILITY,
                         &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_DEVICE_CAPABILITY\n");
        return;
    }

    // Check if FLR is supported
    if (FLD_TEST_DRF(_XVE, _DEVICE_CAPABILITY, _FUNCTION_LEVEL_RESET,
                     _SUPPORTED, regVal))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED, NV_TRUE);
    }
}

/*! 
 * @brief Fetch VF details such as no. of VFs, First VF offset etc
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

/*!
 * @brief Waits for function issued transaction completions(sysmem to GPU) to arrive
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
NV_STATUS
kbifWaitForTransactionsComplete_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32      domain   = gpuGetDomain(pGpu);
    NvU8       bus      = gpuGetBus(pGpu);
    NvU8       device   = gpuGetDevice(pGpu);
    void      *handle   = NULL;
    NvU16      vendorId;
    NvU16      deviceId;
    RMTIMEOUT  timeout;

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    // Wait for number of pending transactions to go to 0
    while (DRF_VAL(_XVE, _DEVICE_CONTROL_STATUS, _TRANSACTIONS_PENDING,
                   osPciReadDword(handle, NV_XVE_DEVICE_CONTROL_STATUS)) != 0)
    {
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Timeout waiting for transactions pending to go to 0\n");
            DBG_BREAKPOINT();
            return NV_ERR_TIMEOUT;
        }
    }

    return NV_OK;
}

/*!
 * @brief Trigger FLR
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  NV_OK if successful.
 */
NV_STATUS
kbifTriggerFlr_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   domain   = gpuGetDomain(pGpu);
    NvU8    bus      = gpuGetBus(pGpu);
    NvU8    device   = gpuGetDevice(pGpu);
    void   *handle   = NULL;
    OBJSYS *pSys     = SYS_GET_INSTANCE();
    OBJCL  *pCl      = SYS_GET_CL(pSys);
    NvU32   regVal   = 0;
    NvU16   vendorId = 0;
    NvU16   deviceId = 0;

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    regVal = osPciReadDword(handle, NV_XVE_DEVICE_CONTROL_STATUS);

    regVal = FLD_SET_DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS ,
                             _INITIATE_FN_LVL_RST, 0x1, regVal);

    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_DEVICE_CONTROL_STATUS,
                     regVal);

    return NV_OK;
}

/*!
 * This function setups the xve register map pointers
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Pointer to KernelBif object
 * @param[in]  func           PCIe function number
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifInitXveRegMap_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU8       func
)
{
    extern NvU32 kbifInitXveRegMap_GM107(OBJGPU *pGpu, KernelBif *pKernelBif, NvU8 func);
    NV_STATUS  status     = NV_OK;

    if (func == 0)
    {
        pKernelBif->xveRegmapRef[0].nFunc              = 0;
        pKernelBif->xveRegmapRef[0].xveRegMapValid     = xveRegMapValid;
        pKernelBif->xveRegmapRef[0].xveRegMapWrite     = xveRegMapWrite;
        pKernelBif->xveRegmapRef[0].numXveRegMapValid  = sizeof(xveRegMapValid)/sizeof(xveRegMapValid[0]);
        pKernelBif->xveRegmapRef[0].numXveRegMapWrite  = sizeof(xveRegMapWrite)/sizeof(xveRegMapWrite[0]);
        pKernelBif->xveRegmapRef[0].bufBootConfigSpace = pKernelBif->cacheData.gpuBootConfigSpace;
        // MSIX table buf not used in Turing, but it could be
        pKernelBif->xveRegmapRef[0].bufMsixTable       = NULL;
    }
    else if (func == 1)
    {
        // Init regmap for Fn1 using older HAL
        status = kbifInitXveRegMap_GM107(pGpu, pKernelBif, 1);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid argument, func: %d.\n", func);
        NV_ASSERT(0);
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}

/*!
 * @brief Returns size of MSIX vector control table
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Pointer to KernelBif object
 */
NvU32
kbifGetMSIXTableVectorControlSize_TU102
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    return NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL__SIZE_1;
}

