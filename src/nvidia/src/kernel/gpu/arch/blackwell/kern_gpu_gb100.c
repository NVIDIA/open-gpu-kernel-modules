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
*         Blackwell specific Descriptor List management functions              *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/eng_desc.h"
#include "gpu/gpu_child_class_defs.h"
#include "os/os.h"
#include "nverror.h"

#include "published/blackwell/gb100/dev_boot.h"
#include "published/blackwell/gb100/dev_boot_addendum.h"

#include "published/blackwell/gb100/dev_pcfg_pf0.h"

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GB100[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 20),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 8),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelCcu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB100);
    return gpuChildrenPresent_GB100;
}


//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GB102[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 10),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 4),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelCcu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB102);
    return gpuChildrenPresent_GB102;
}

/*!
 * @brief Read the non-private registers on vGPU through mirror space
 *
 * @param[in]  pGpu   GPU object pointer
 * @param[in]  index  Register offset in PCIe config space
 * @param[out] pData  Value of the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadPassThruConfigReg_GB100
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32  *pData
)
{
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU8  bus      = gpuGetBus(pGpu);
    NvU8  device   = gpuGetDevice(pGpu);
    NvU8  function = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    *pData = osPciReadDword(pGpu->hPci, index);

    return NV_OK;
}

/*!
 * @brief Get GPU ID based on PCIE config reads.
 * Also determine other properties of the PCIE capabilities.
 *
 * @param[in]   pGpu  OBJGPU pointer
 * @returns void.
 */
void
gpuGetIdInfo_GB100(OBJGPU *pGpu)
{
    NvU32 data;
    NvU32 deviceId;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_REVISION_ID_AND_CLASS_CODE, &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "unable to read NV_PF0_REVISION_ID_AND_CLASS_CODE\n");
        return;
    }

    // we only need the FIB and MASK values
    pGpu->idInfo.PCIRevisionID = (data & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_PROGRAMMING_INTERFACE)
                                       & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_SUB_CLASS_CODE)
                                       & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_BASE_CLASS_CODE));

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_SUBSYSTEM_ID_AND_VENDOR_ID, &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_PF0_SUBSYSTEM_ID_AND_VENDOR_ID\n");
        return;
    }
    pGpu->idInfo.PCISubDeviceID = data;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_VENDOR_ID, &deviceId) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_PF0_DEVICE_VENDOR_ID\n");
        return;
    }
    // For universal GPU use unlatched value
    pGpu->idInfo.PCIDeviceID = deviceId;

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        //
        // In vgpu production environment, RM replaces direct GPU register
        // reads in NV_XTL space w/ OS pci reads when running inside the VF. It
        // therefore gets the correct device id. The same mechanism is not used
        // when running on MODS. Hence, RM gets bogus values of device id on
        // MODS runs. In this case, RM will have to explicitly ask MODS to
        // issue a read of the NV_PF0_DEVICE_VENDOR_ID register from the PF's pci
        // config space.
        //
        if (RMCFG_FEATURE_PLATFORM_MODS && IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            NV_ASSERT_OR_RETURN_VOID(osReadPFPciConfigInVF(NV_PF0_DEVICE_VENDOR_ID,
                                                &deviceId) == NV_OK);
            pGpu->idInfo.PCIDeviceID = deviceId;
            NV_PRINTF(LEVEL_INFO, "pci_dev_id = 0x%x\n", pGpu->idInfo.PCIDeviceID);
        }
    }
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GB100
(
    OBJGPU *pGpu
)
{
    NvU32   secDebug = 0;

    //
    // Read SEC_FAULT config space to determine what went wrong.
    // Do not return early on error, we must take the GPU down.
    //
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1, &secDebug));

    MODS_ARCH_ERROR_PRINTF("NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1:0x%x\n", secDebug);
    NV_PRINTF(LEVEL_FATAL, "SEC_FAULT lockdown detected. This is fatal. "
                           "RM will now shut down. NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1: 0x%x\n", secDebug);

#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_PF0, _DVSEC0_SEC_FAULT_REGISTER_1, field, secDebug) != 0) \
    { \
        MODS_ARCH_ERROR_PRINTF("DVSEC0_SEC_FAULT_REGISTER_1" #field "\n"); \
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_FUSE_POD);
    LOG_SEC_FAULT(_FUSE_SCPM);
    LOG_SEC_FAULT(_FSP_SCPM);
    LOG_SEC_FAULT(_SEC2_SCPM);
    LOG_SEC_FAULT(_FSP_DCLS);
    LOG_SEC_FAULT(_SEC2_DCLS);
    LOG_SEC_FAULT(_GSP_DCLS);
    LOG_SEC_FAULT(_PMU_DCLS);
    LOG_SEC_FAULT(_IFF_SEQUENCE_TOO_BIG);
    LOG_SEC_FAULT(_PRE_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_POST_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_IFF_ECC_UNCORRECTABLE_ERROR);
    LOG_SEC_FAULT(_IFF_CMD_FORMAT_ERROR);
    LOG_SEC_FAULT(_IFF_PRI_ERROR);
    LOG_SEC_FAULT(_C2C_MISC_LINK_ERROR);
    LOG_SEC_FAULT(_FSP_WDT);
    LOG_SEC_FAULT(_GSP_WDT);
    LOG_SEC_FAULT(_PMU_WDT);
    LOG_SEC_FAULT(_SEC2_WDT);
    LOG_SEC_FAULT(_C2C_HBI_LINK_ERROR);
    LOG_SEC_FAULT(_FSP_EMP);
    LOG_SEC_FAULT(_FSP_UNCORRECTABLE_ERRORS);
    LOG_SEC_FAULT(_FUSE_POD_2ND);
    LOG_SEC_FAULT(_FUSE_SCPM_2ND);
    LOG_SEC_FAULT(_IFF_SEQUENCE_TOO_BIG_2ND);
    LOG_SEC_FAULT(_PRE_IFF_CRC_CHECK_FAILED_2ND);
    LOG_SEC_FAULT(_POST_IFF_CRC_CHECK_FAILED_2ND);
    LOG_SEC_FAULT(_IFF_ECC_UNCORRECTABLE_ERROR_2ND);
    LOG_SEC_FAULT(_IFF_CMD_FORMAT_ERROR_2ND);
    LOG_SEC_FAULT(_IFF_PRI_ERROR_2ND);
    LOG_SEC_FAULT(_DEVICE_LOCKDOWN);
    LOG_SEC_FAULT(_FUNCTION_LOCKDOWN);

#undef LOG_SEC_FAULT

    //
    // After SEC_FAULT occurs, the GPU will only return SCPM dummy values until properly reset.
    // Only cold reset will clear SEC_FAULT, not hot reset. This GPU is as good as lost.
    // handleGpuLost first to setGpuDisconnectedProperties so that another reg read does not
    // happen when the notifier is sent below.
    //
    osHandleGpuLost(pGpu);

    //
    // Send SEC_FAULT notification. This should tells any MODS test testing for this
    // error to pass and exit
    //
    gpuNotifySubDeviceEvent(pGpu,
                            NV2080_NOTIFIERS_SEC_FAULT_ERROR,
                            NULL,
                            0,
                            0,
                            SEC_FAULT_ERROR);
}

/*!
 * Check if CC bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsCCEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_2_CC);
    return FLD_TEST_DRF(_PMC, _SCRATCH_RESET_2_CC, _MODE_ENABLED, _TRUE, val);
}

/*!
 * Check if dev mode bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsDevModeEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_2_CC);
    return FLD_TEST_DRF(_PMC, _SCRATCH_RESET_2_CC, _DEV_ENABLED, _TRUE, val);
}

