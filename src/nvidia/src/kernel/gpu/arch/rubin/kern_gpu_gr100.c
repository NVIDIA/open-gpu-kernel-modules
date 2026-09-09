/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/gpu.h"
#include "os/os.h"
#include "gpu/gpu_child_class_defs.h"

// #include "published/rubin/gr100/dev_ptop.h" Will need this soon

#include "published/rubin/gr100/dev_vm.h"
#include "published/rubin/gr100/dev_pcfg_pf0.h"
#include "published/rubin/gr100/dev_pmc_zb.h"
#include "published/rubin/gr100/hwproject.h"

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GR100
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

    //
    // HW uses directly pins the bits from SYSCTRL to the NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1
    // bits, so decode it using the SYSCTRL definition
    //
    //
    // There are no SEC_FAULTs in SEC_2_FAULT_ERROR so far,
    // but we don't have a way to check for that in the future..
    //
#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_SYSCTRL, _SEC_FAULT_BIT_POSITION, field, secDebug) != 0) \
    { \
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1" #field "\n"); \
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_FUSE_POD);
    LOG_SEC_FAULT(_FUSE_SCPM);
    LOG_SEC_FAULT(_IFF_SEQUENCE_TOO_BIG);
    LOG_SEC_FAULT(_IFF_PRE_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_IFF_POST_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_IFF_ECC_UNCORRECTABLE_ERROR);
    LOG_SEC_FAULT(_IFF_CMD_FORMAT_ERROR);
    LOG_SEC_FAULT(_IFF_PRI_ERROR);
    LOG_SEC_FAULT(_FSP_SCPM);
    LOG_SEC_FAULT(_FSP_DCLS);
    LOG_SEC_FAULT(_FSP_EMP);
    LOG_SEC_FAULT(_FSP_UNCORRECTABLE_ERROR);
    LOG_SEC_FAULT(_FSP_L5_WDT);

    LOG_SEC_FAULT(_SEC2_SCPM);
    LOG_SEC_FAULT(_SEC2_DCLS);
    LOG_SEC_FAULT(_SEC2_L5_WDT);
    LOG_SEC_FAULT(_GSP_DCLS);
    LOG_SEC_FAULT(_GSP_L5_WDT);
    LOG_SEC_FAULT(_PMU_DCLS);
    LOG_SEC_FAULT(_PMU_L5_WDT);

    LOG_SEC_FAULT(_XTAL_CTFDC);
    LOG_SEC_FAULT(_CLOCK_XTAL_FMON);
    LOG_SEC_FAULT(_CLOCK_GPC_FMON);
    LOG_SEC_FAULT(_DEVICE_LOCKDOWN);
    LOG_SEC_FAULT(_FUNCTION_LOCKDOWN);
    LOG_SEC_FAULT(_INTERRUPT);

#undef LOG_SEC_FAULT

    //
    // After SEC_FAULT occurs, the GPU will only return SCPM dummy values until properly reset.
    // Only cold reset will clear SEC_FAULT, not hot reset. This GPU is as good as lost.
    // handleGpuLost first to setGpuDisconnectedProperties so that another reg read does not
    // happen when the notifier is sent below.
    //
    osHandleGpuLost(pGpu, NV_FALSE);

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
 * @brief Determine SoC type if GPU is configured in Self Hosted mode.
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return SoC type
 */
NvU32
gpuDetermineSelfHostedSocType_GR100
(
    OBJGPU *pGpu
)
{
    if (!gpuIsSelfHosted(pGpu))
    {
        return NV0000_CTRL_SYSTEM_SH_SOC_TYPE_NA;
    }
    return NV0000_CTRL_SYSTEM_SH_SOC_TYPE_NA;
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

static const GPUCHILDPRESENT gpuChildrenPresent_GR100[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
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
    GPU_CHILD_PRESENT(OBJGRIDDISPLAYLESS, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(Spdm, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelCcu, 1),
    GPU_CHILD_PRESENT(KernelOob, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GR100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GR100);
    return gpuChildrenPresent_GR100;
}

NV_STATUS
gpuGenGidData_GR100
(
    OBJGPU *pGpu,
    NvU8 *pGidData,
    NvU32 gidSize,
    NvU32 gidFlags
)
{
    NvU64 pdi64;
    NvU16 chipId = gpuGetChipId(pGpu);

    NV_ASSERT_OR_RETURN(gidSize == NV_UUID_LEN, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(FLD_TEST_DRF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1, gidFlags),
                        NV_ERR_NOT_SUPPORTED);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuGetPdi_HAL(pGpu, &pdi64));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, nvGenerateGpuUuid(chipId, pdi64, (NvUuid*)(pGidData)));

    return NV_OK;
}

/*!
 * @brief Compare RM generated GPU UUID with GFW's (PBI) UUID obtained from gpuConstruct_IMPL.
 *
 * @param[in] pGpu       OBJGPU pointer
 */
NV_STATUS
gpuValidateUuid_GR100
(
    OBJGPU *pGpu
)
{
    NvU8 gidData[RM_SHA1_GID_SIZE];

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuGenGidData_HAL(pGpu, gidData, RM_SHA1_GID_SIZE, 0));

    if (portMemCmp(gidData, &pGpu->gpuUuid.uuid[0], RM_SHA1_GID_SIZE) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "GFW's (PBI) UUID and RM generated don't match\n");

        return NV_ERR_INVALID_STATE;
    }
    return NV_OK;
}
