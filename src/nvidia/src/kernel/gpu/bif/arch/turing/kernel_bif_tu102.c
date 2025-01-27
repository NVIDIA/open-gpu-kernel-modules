/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/mc/kernel_mc.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/gpu.h"
#include "platform/chipset/chipset.h"

#include "nvmisc.h"

#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "published/turing/tu102/dev_nv_xve.h"
#include "published/turing/tu102/dev_vm.h"

#include "published/turing/tu102/dev_nv_pcfg_xve_regmap.h"
#include "published/turing/tu102/dev_boot.h"

#include "ctrl/ctrla084.h"

#define RTLSIM_DELAY_SCALE_US          8

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

    // For VF TLB emulation, trap MMU FAULT BUFFER page
    if ((maxInstance > 1) && pGpu->getProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE))
    {
        offsetEnd = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0);
        pOffsets[idx] = offsetStart;
        pSizes[idx] = offsetEnd - offsetStart;
        idx++;

        offsetStart = NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(0) + osPageSize;
    }

    // Trap MSI-X table page for non-Hyperv and Hyperv non-GSP cases
    if (!hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {

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
    status = GPU_BUS_CFG_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR3, &regVal);
    NV_ASSERT(status == NV_OK);
    pGpu->sriovState.totalVFs = GPU_DRF_VAL(_XVE, _SRIOV_CAP_HDR3,
                                            _TOTAL_VFS, regVal);

    // Get first VF offset
    status = GPU_BUS_CFG_RD32(pGpu, NV_XVE_SRIOV_CAP_HDR5, &regVal);
    NV_ASSERT(status == NV_OK);
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
        pKernelBif->xveRegmapRef[0].numXveRegMapValid  = NV_ARRAY_ELEMENTS(xveRegMapValid);
        pKernelBif->xveRegmapRef[0].numXveRegMapWrite  = NV_ARRAY_ELEMENTS(xveRegMapWrite);
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
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    return NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL__SIZE_1;
}

/*!
 * This function saves MSIX vector control masks which can later be restored
 * using _kbifRestoreMSIXVectorControlMasks.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelBif      Pointer to KernelBif object
 *
 * @return  'msixVectorMask'    MSIX vector control mask state for all
 *                              MSIX table entries
 */
static NvU32
_kbifSaveMSIXVectorControlMasks
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32     i;
    NvU32     regVal;
    NvU32     controlSize = kbifGetMSIXTableVectorControlSize_HAL(pGpu, pKernelBif);
    NvU32     msixVectorMask = 0U;

    // All implementations of kbifGetMSIXTableVectorControlSize_HAL() return a
    // value less than 32.
    NV_ASSERT(controlSize < 32);

    // Set the bits in msixVectorMask if NV_MSIX_TABLE_VECTOR_CONTROL(i) is masked
    for (i = 0U; i < controlSize; i++)
    {
        regVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL(i));

        if (FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _MSIX_TABLE_VECTOR_CONTROL,
                         _MASK_BIT, _MASKED, regVal))
        {
            msixVectorMask |= NVBIT(i);
        }
    }

    return msixVectorMask;
}

/*!
 * This function restores MSIX vector control masks to the saved state. Vector
 * control mask needs to be saved using _kbifSaveMSIXVectorControlMasks.
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[in]  pKernelBif      Pointer to KernelBif object
 * @param[in]  msixVectorMask  State of MSIX vector control masks
 */
static void
_kbifRestoreMSIXVectorControlMasks
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      msixVectorMask
)
{
    NvU32     i;
    NvU32     controlSize = kbifGetMSIXTableVectorControlSize_HAL(pGpu, pKernelBif);

    // Initialize the base offset for the virtual registers for physical function
    NvU32 vRegOffset = pGpu->sriovState.virtualRegPhysOffset;

    // All implementations of kbifGetMSIXTableVectorControlSize_HAL() return a
    // value less than 32.
    NV_ASSERT(controlSize < 32);

    //
    // Restore NV_MSIX_TABLE_VECTOR_CONTROL(i) based on msixVectorMask
    // In FLR path, we don't want to use usual register r/w macros
    //
    for (i = 0U; i < controlSize; i++)
    {
        if ((NVBIT(i) & msixVectorMask) != 0U)
        {
            osGpuWriteReg032(pGpu,
                vRegOffset + NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL(i),
                NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL_MASK_BIT_MASKED);
        }
        else
        {
            osGpuWriteReg032(pGpu,
                vRegOffset + NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL(i),
                NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL_MASK_BIT_UNMASKED);
        }
    }
}


/**
 *
 * Do function level reset for Fn0.
 *
 */
NV_STATUS
kbifDoFunctionLevelReset_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32      domain     = gpuGetDomain(pGpu);
    NvU8       bus        = gpuGetBus(pGpu);
    NvU8       device     = gpuGetDevice(pGpu);
    void      *handle     = NULL;
    NV_STATUS  status     = NV_OK;
    NvU32      tempRegVal;
    NvU16      vendorId;
    NvU16      deviceId;
    // 'i'th bit set to 1 indicates NV_MSIX_TABLE_VECTOR_CONTROL(i) is masked
    NvU32      msixVectorMask = 0;
    NvBool     bMSIXEnabled;

    //
    // From the experimental data: We need to get the handle before asserting FLR
    // GPU is always at function 0
    //
    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    pKernelBif->bPreparingFunctionLevelReset = NV_TRUE;

    status = kbifSavePcieConfigRegisters_HAL(pGpu, pKernelBif);
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        NV_PRINTF(LEVEL_ERROR, "Config registers save failed!\n");
        goto bifDoFunctionLevelReset_TU102_exit;
    }

    bMSIXEnabled = kbifIsMSIXEnabledInHW_HAL(pGpu, pKernelBif);
    if (bMSIXEnabled)
    {
        msixVectorMask = _kbifSaveMSIXVectorControlMasks(pGpu, pKernelBif);
    }

    pKernelBif->bPreparingFunctionLevelReset = NV_FALSE;
    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED))
    {
        //
        // SW WARs required for Function Level Reset -
        // Clear Bus Master Enable bit in command register so that no more requests to sysmem are made by Fn0
        // Executing these WARs after save config space so that restore config space does not restore-
        // incorrect command register
        // For other WARs which are executed in kbifPrepareForFullChipReset_HAL, gpu re-init sequence after FLR makes
        // sure to revert these WARs
        //
        if (kbifStopSysMemRequests_HAL(pGpu, pKernelBif, NV_TRUE) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "BIF Stop Sys Mem requests failed.\n");
            DBG_BREAKPOINT();
        }
        // Wait for number of sysmem transactions pending to go to 0
        if (kbifWaitForTransactionsComplete_HAL(pGpu, pKernelBif) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "BIF Wait for Transactions complete failed.\n");
            DBG_BREAKPOINT();
        }
    }

    // Trigger FLR now
    kbifTriggerFlr_HAL(pGpu, pKernelBif);

    pKernelBif->bInFunctionLevelReset = NV_TRUE;

    {
        // Wait for 100 ms before attempting to read PCI config space
        osDelayUs(100000);
    }

    status = kbifRestorePcieConfigRegisters_HAL(pGpu, pKernelBif);
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        NV_PRINTF(LEVEL_ERROR, "Config registers restore failed!\n");
        goto bifDoFunctionLevelReset_TU102_exit;
    }

    // As of GA10X, miata rom cannot be run on simulation.
    if (IS_SILICON(pGpu) || IS_EMULATION(pGpu))
    {
        // On emulation VBIOS boot can take long so add prints for better visibility
        if (IS_EMULATION(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR, "Entering secure boot completion wait.\n");
        }

        if (IS_GSP_CLIENT(pGpu))
        {
            status = gpuWaitForGfwBootComplete_HAL(pGpu);
        }
        else
        {
            status = NV_ERR_NOT_SUPPORTED;
        }
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            NV_PRINTF(LEVEL_ERROR, "VBIOS boot failed!!\n");
            goto bifDoFunctionLevelReset_TU102_exit;
        }
        if (IS_EMULATION(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Exited secure boot completion wait with status = NV_OK.\n");
        }
    }

    if (bMSIXEnabled)
    {
        // TODO-NM: Check if this needed for other NVPM flows like GC6
        _kbifRestoreMSIXVectorControlMasks(pGpu, pKernelBif, msixVectorMask);
    }

    //
    // After FLR 'outstanding downstream read counter' does not reflect the
    // correct value until cleared. We clear this counter without checking if
    // FLR succeeded or not. This is considering that we will attempt SBR after
    // FLR fails and if we don't clear this counter here, it will not reflect
    // the correct value after SBR (SBR does not clear it).
    //
    kbifClearDownstreamReadCounter_HAL(pGpu, pKernelBif);

    // Re-init handle as well since gpu is reset
    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);
    tempRegVal = osPciReadDword(handle, NV_XVE_ID);
    if (!FLD_TEST_DRF(_XVE, _ID, _VENDOR, _NVIDIA, tempRegVal))
    {
        status = NV_ERR_GENERIC;
        goto bifDoFunctionLevelReset_TU102_exit;
    }

bifDoFunctionLevelReset_TU102_exit:
    pKernelBif->bPreparingFunctionLevelReset = NV_FALSE;
    pKernelBif->bInFunctionLevelReset        = NV_FALSE;
    return status;
}

/*!
 * @brief  Get the PMC bit of the valid Engines to reset.
 *
 * @return All valid engines
 */
NvU32
kbifGetValidEnginesToReset_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    return (DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE3, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE4, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE5, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE6, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE7, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE8, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PWR, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PDISP, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _SEC, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PERFMON, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVJPG0, _ENABLED));
}


/**
 *
 * Execute SBR if FLR is not supported, else do pre-FLR sequence followed by FLR
 *
 */
NV_STATUS
kbifDoSecondaryBusResetOrFunctionLevelReset_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    KernelMc  *pKernelMc  = GPU_GET_KERNEL_MC(pGpu);
    NV_STATUS  rmStatus   = NV_OK;
    NvU32 engineMask;
    NvBool bIsFLRSupportedAndEnabled;

    // Issue FLR if capable and not force disabled
    bIsFLRSupportedAndEnabled = (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED)) &&
                                !(pKernelBif->bForceDisableFLR);

    if (!bIsFLRSupportedAndEnabled)
    {
        // Issue SBR
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET, NV_TRUE);
        rmStatus = kbifDoSecondaryBusHotReset_HAL(pGpu, pKernelBif);
    }
    else
    {
        //
        // Execute any workarounds or changes needed to make sure we can reset
        // properly.
        //
        kbifPrepareForFullChipReset_HAL(pGpu, pKernelBif);

        // First Reset PMC
        engineMask = kbifGetValidEnginesToReset_HAL(pGpu, pKernelBif);
        if (engineMask)
        {
            NV_ASSERT_OK(kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, engineMask, NV_FALSE, NV_FALSE));
        }

        //
        // Reset engines in NV_PMC_DEVICE_ENABLE. For Ampere and later chips,
        // host engines has moved from NV_PMC_ENABLE to NV_PMC_DEVICE_ENABLE,
        // hence we need to ensure that engines in the NV_PMC_DEVICE_ENABLE are
        // reset too.
        //
        engineMask = kbifGetValidDeviceEnginesToReset_HAL(pGpu, pKernelBif);
        if (engineMask)
        {
            NV_ASSERT_OK(kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, engineMask, NV_FALSE,
                gpuIsUsePmcDeviceEnableForHostEngineEnabled(pGpu)));
        }

        // Issue FLR
        rmStatus = kbifDoFunctionLevelReset_HAL(pGpu, pKernelBif);

        // If FLR did not work, fall back to SBR
        if (NV_OK != rmStatus)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET, NV_TRUE);
            rmStatus = kbifDoSecondaryBusHotReset_HAL(pGpu, pKernelBif);
        }
        else
        {
            // RM init code is dependent on this flag. If FLR or SW_RESET is used for reset, we set this flag
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);
        }
    }

    return rmStatus;
}
