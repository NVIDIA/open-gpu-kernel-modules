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

#include "gpu/gpu.h"
#include "gpu/gpu_child_class_defs.h"
#include "os/os.h"
#include "nverror.h"
#include "vgpu/rpc.h"

#include "published/hopper/gh100/hwproject.h"
#include "published/hopper/gh100/dev_gc6_island.h"
#include "published/hopper/gh100/dev_gc6_island_addendum.h"
#include "published/hopper/gh100/dev_pmc.h"
#include "published/hopper/gh100/dev_xtl_ep_pcfg_gpu.h"
#include "published/hopper/gh100/pri_nv_xal_ep.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/dev_xtl_ep_pri.h"

#include "published/hopper/gh100/dev_therm.h"
#include "published/hopper/gh100/dev_therm_addendum.h"

#include "ctrl/ctrl2080/ctrl2080mc.h"

#include "gpu/gsp/gsp_static_config.h"

/*!
 * @brief Write to non-private registers in XTL using config cycles
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] index      Register offset in PCIe config space
 * @param[in] value      Write this value to the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuWriteBusConfigReg_GH100
(
    OBJGPU    *pGpu,
    NvU32      index,
    NvU32      value
)
{
    return gpuWriteBusConfigCycle(pGpu, index, value);
}

/*!
 * @brief Read the non-private registers in XTL using config cycles
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] index      Register offset in PCIe config space
 * @param[out] pData     Value of the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadBusConfigReg_GH100
(
    OBJGPU    *pGpu,
    NvU32      index,
    NvU32     *pData
)
{
    return gpuReadBusConfigCycle(pGpu, index, pData);
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
gpuReadPassThruConfigReg_GH100
(
    OBJGPU    *pGpu,
    NvU32      index,
    NvU32     *pData
)
{
    *pData = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_EP_PCFGM) + index);

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
gpuGetIdInfo_GH100
(
    OBJGPU *pGpu
)
{
    NvU32   data;
    NvU32   deviceId;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                             NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE,
                             &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "unable to read NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE\n");
        return;
    }

    // we only need the FIB and MASK values
    pGpu->idInfo.PCIRevisionID = (data & ~GPU_DRF_SHIFTMASK(NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_PGM_INTERFACE)
                                       & ~GPU_DRF_SHIFTMASK(NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_SUB_CLASSCODE)
                                       & ~GPU_DRF_SHIFTMASK(NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_BASE_CLASSCODE));

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_SUBSYSTEM_ID,
                             &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_EP_PCFG_GPU_SUBSYSTEM_ID\n");
        return;
    }
    pGpu->idInfo.PCISubDeviceID = data;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_ID, &deviceId) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_EP_PCFG_GPU_ID\n");
        return;
    }
    // For universal GPU use unlatched value
    pGpu->idInfo.PCIDeviceID = deviceId;

}

/*!
 * @brief Returns the physical address width for the given @ref NV_ADDRESS_SPACE
 */
NvU32 gpuGetPhysAddrWidth_GH100
(
    OBJGPU          *pGpu,
    NV_ADDRESS_SPACE addrSp
)
{
    // Currently this function supports only sysmem addresses
    NV_ASSERT_OR_RETURN(ADDR_SYSMEM == addrSp, 0);

    return NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS;
}

/**
 * @brief Check if the GPU supports ATS with SMC memory partitioning enabled.
 *
 * @param pGpu
 *
 * @return NV_TRUE if ATS is supported with SMC memory partitioning
 *         NV_FALSE otherwise
 */
NvBool gpuIsAtsSupportedWithSmcMemPartitioning_GH100(OBJGPU *pGpu)
{
    //
    // ATS is supported only on self-hosted Hopper.
    // Also enable it for simulation platforms for fmodel/RTL verification
    // with CPU model.
    //
    if (gpuIsSelfHosted(pGpu))
    {
        return NV_TRUE;
    }

    if (IS_SIMULATION(pGpu))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 secDebug = 0;
    NvU32 iffPos;

    //
    // Read SEC_FAULT config space to determine what went wrong.
    // Do not return early on error, we must take the GPU down.
    //
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC, &secDebug));

    MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC:0x%x\n", secDebug);
    NV_PRINTF(LEVEL_FATAL, "SEC_FAULT lockdown detected. This is fatal. "
                            "RM will now shut down. NV_EP_PCFG_GPU_VSEC_DEBUG_SEC: 0x%x\n", secDebug);

#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC, field, secDebug) != 0) \
    { \
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC" #field "\n"); \
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_FAULT_FUSE_POD);
    LOG_SEC_FAULT(_FAULT_FUSE_SCPM);
    LOG_SEC_FAULT(_FAULT_FSP_SCPM);
    LOG_SEC_FAULT(_FAULT_SEC2_SCPM);
    LOG_SEC_FAULT(_FAULT_FSP_DCLS);
    LOG_SEC_FAULT(_FAULT_SEC2_DCLS);
    LOG_SEC_FAULT(_FAULT_GSP_DCLS);
    LOG_SEC_FAULT(_FAULT_PMU_DCLS);
    LOG_SEC_FAULT(_FAULT_SEQ_TOO_BIG);
    LOG_SEC_FAULT(_FAULT_PRE_IFF_CRC);
    LOG_SEC_FAULT(_FAULT_POST_IFF_CRC);
    LOG_SEC_FAULT(_FAULT_ECC);
    LOG_SEC_FAULT(_FAULT_CMD);
    LOG_SEC_FAULT(_FAULT_PRI);
    LOG_SEC_FAULT(_FAULT_WDG);
    LOG_SEC_FAULT(_FAULT_BOOTFSM);

#undef LOG_SEC_FAULT

    // IFF_POS has a multi-bit value
    iffPos = DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC, _IFF_POS, secDebug);
    if (iffPos != 0)
    {
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS value: 0x%x\n", iffPos);
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: _IFF_POS value: 0x%x\n", iffPos);
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR,
                      "SEC_FAULT: _IFF_POS value: 0x%x", iffPos);
    }

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
 * @brief Perform gpu-dependent error handling for error during register read sanity check
 *
 * @param[in]       pGpu        GPU object pointer
 * @param[in]       addr        Value address
 * @param[in]       value       Value read during check
 */
void
gpuHandleSanityCheckRegReadError_GH100
(
    OBJGPU *pGpu,
    NvU32 addr,
    NvU32 value
)
{
    // SEC_FAULT possibly detected, confirm by reading NV_PMC_BOOT_0
    if ((value == NV_XAL_EP_SCPM_PRI_DUMMY_DATA_PATTERN_INIT) &&
        (osGpuReadReg032(pGpu, NV_PMC_BOOT_0) == NV_XAL_EP_SCPM_PRI_DUMMY_DATA_PATTERN_INIT))
    {
        gpuHandleSecFault_HAL(pGpu);
    }
    else
    {
        NvU32 intr = ~0U;
        {
            //
            // Read the interrupt status using the direct OS reg read call so we don't recurs
            // if we happen to see GPU_READ_PRI_ERROR_CODE there as well (bug 799876)
            //
            intr = osGpuReadReg032(pGpu, NV_XAL_EP_INTR_0);
        }


        // To be sure, filter this down further by checking the related pri interrupts:
        if (FLD_TEST_DRF(_XAL_EP, _INTR_0, _PRI_FECSERR, _PENDING, intr) ||
            FLD_TEST_DRF(_XAL_EP, _INTR_0, _PRI_REQ_TIMEOUT, _PENDING, intr) ||
            FLD_TEST_DRF(_XAL_EP, _INTR_0, _PRI_RSP_TIMEOUT, _PENDING, intr))
        {
#if NV_PRINTF_STRINGS_ALLOWED
            const char *errorString = "Unknown SYS_PRI_ERROR_CODE";
            gpuGetSanityCheckRegReadError_HAL(pGpu, value,
                                              &errorString);
            NV_PRINTF(LEVEL_ERROR,
                      "Possible bad register read: addr: 0x%x,  regvalue: 0x%x,  error code: %s\n",
                      addr, value, errorString);
#else // NV_PRINTF_STRINGS_ALLOWED
            NV_PRINTF(LEVEL_ERROR,
                      "Possible bad register read: addr: 0x%x,  regvalue: 0x%x\n",
                      addr, value);
#endif // NV_PRINTF_STRINGS_ALLOWED
        }
    }
}

/*!
 * @brief Returns FLA VASpace Size for Hopper+
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] bNvSwitchVirtualization     boolean
 *
 * @returns NvU64 -> size of FLA VASpace
 */
NvU64
gpuGetFlaVasSize_GH100
(
    OBJGPU *pGpu,
    NvBool  bNvswitchVirtualization
)
{
    return 0x8000000000;  // 512GB
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

static const GPUCHILDPRESENT gpuChildrenPresent_GH100[] =
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
gpuGetChildrenPresent_GH100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GH100);
    return gpuChildrenPresent_GH100;
}

/*!
 * @brief Determine if GPU is configured in Self Hosted mode.
 * In Hopper+, GPU can be configured to work in Self Hosted,
 * Externally Hosted and Endpoint mode.
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return void
 */
void
gpuDetermineSelfHostedMode_KERNEL_GH100
(
    OBJGPU *pGpu
)
{
    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        if (pVSI->bSelfHostedMode)
        {
            pGpu->bIsSelfHosted = NV_TRUE;
            NV_PRINTF(LEVEL_INFO, "SELF HOSTED mode detected after reading VGPU static info.\n");
        }
    }
    if (IS_GSP_CLIENT(pGpu))
    {
        GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        if (pGSCI->bSelfHostedMode)
        {
            pGpu->bIsSelfHosted = NV_TRUE;
            NV_PRINTF(LEVEL_INFO, "SELF HOSTED mode detected after reading GSP static info.\n");
        }
    }
}

/*!
 * @brief Determine if MIG can be supported.
 * In self hosted hopper, MIG can be supported only from specific
 * GH100 revisions.
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return void
 */
void
gpuDetermineMIGSupport_GH100
(
    OBJGPU *pGpu
)
{
    if (gpuIsSelfHosted(pGpu) &&
        (gpuGetChipSubRev_HAL(pGpu) < NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_R))
    {
        NV_PRINTF(LEVEL_ERROR, "Disabling MIG Support. MIG can be supported on self hosted hopper "
                  "only from revision R onwards\n");
        pGpu->setProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED, NV_FALSE);
    }
}

/*!
 * Check if CC bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsCCEnabledInHw_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC);
    return FLD_TEST_DRF(_PGC6, _AON_SECURE_SCRATCH_GROUP_20_CC, _MODE_ENABLED, _TRUE, val);
}

/*!
 * Check if dev mode bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsDevModeEnabledInHw_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC);
    return FLD_TEST_DRF(_PGC6, _AON_SECURE_SCRATCH_GROUP_20_CC, _DEV_ENABLED, _TRUE, val);
}

/*!
 * Check if protected pcie has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsProtectedPcieEnabledInHw_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC);
    return FLD_TEST_DRF(_PGC6, _AON_SECURE_SCRATCH_GROUP_20_CC, _MULTI_GPU_MODE,
                        _PROTECTED_PCIE, val);
}

/*!
 * @brief Check if register being accessed is within guest BAR0 space.
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] addr   Address being validated
 */
NV_STATUS
gpuSanityCheckVirtRegAccess_GH100
(
    OBJGPU *pGpu,
    NvU32   addr
)
{
    // Not applicable in PV mode
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    if ((addr >= DEVICE_BASE(NV_EP_PCFGM)) &&
        (addr < DEVICE_EXTENT(NV_EP_PCFGM)))
    {
        return NV_OK;
    }

    // Check if address in NV_VIRTUAL_FUNCTION range, if not error out.
    if ((addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION_PRIV)) ||
        ((addr >= DRF_BASE(NV_VIRTUAL_FUNCTION)) && (addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION))))
    {
        return NV_OK;
    }

    return NV_ERR_INVALID_ADDRESS;
}

/*!
 * @brief Check if GSP-FMC Inst_in_sys ucode needs to be booted.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @return NV_TRUE if GSP Inst_in_sys FMC needs to be booted, or NV_FALSE otherwise
 */
NvBool
gpuIsGspToBootInInstInSysMode_GH100
(
    OBJGPU    *pGpu
)
{

    return NV_FALSE;
}
