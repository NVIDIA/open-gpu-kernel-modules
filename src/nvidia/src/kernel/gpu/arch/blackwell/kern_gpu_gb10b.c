/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu_uuid.h"
#include "gpu/gpu_child_class_defs.h"
#include "os/os.h"
#include "gpu/eng_desc.h"
#include "nverror.h"
#include "gpu/gpu_mods_error.h"

#include "published/blackwell/gb10b/hwproject.h"
#include "published/blackwell/gb10b/dev_xtl_ep_pcfg_gpu.h"
#include "published/blackwell/gb10b/dev_pmc_zb.h"

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

static const GPUCHILDPRESENT gpuChildrenPresent_GB10B[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelVbios, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 4),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 2),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(Spdm, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(OBJGRIDDISPLAYLESS, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(TdispMgr, 1),
    GPU_CHILD_PRESENT(KernelOob, 0),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB10B(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB10B);
    NV_PRINTF(LEVEL_INFO, "pNumEntries[%u]\n", *pNumEntries);
    return gpuChildrenPresent_GB10B;
}

/*!
 * @brief Generate PDI/PCI-based UUID for Tegra dGPU.
 *
 * Uses the PDI fuse combined with PCI domain/bus/device to produce a globally
 * unique UUID. The SOC path (gpuInstance-based) cannot be used on multi-GPU
 * platforms because each GSP instance sees gpuInstance=0 for its own GPU.
 *
 * On multi-die Tegra dGPU packages the PDI fuses may be shared across dies,
 * so PCI domain/bus/device is required to differentiate dies that share PDI.
 */
NV_STATUS
gpuGenGidData_GB10B
(
    OBJGPU *pGpu,
    NvU8   *pGidData,
    NvU32   gidSize,
    NvU32   gidFlags
)
{
    NvU64 pdi64;
    NvU16 chipId = gpuGetChipId(pGpu);

    NV_ASSERT_OR_RETURN(pGidData != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(gidSize == NV_UUID_LEN, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(FLD_TEST_DRF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1, gidFlags),
                        NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(gpuIsDBDFValid(pGpu), NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuGetPdi_HAL(pGpu, &pdi64));

    pdi64 ^= gpuGetDBDF(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, nvGenerateGpuUuid(chipId, pdi64, (NvUuid*)(pGidData)));

    return NV_OK;
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GB10B
(
    OBJGPU *pGpu
)
{
    NvU32 secDebug = 0;
    NvU32 data;

    //
    // Read SEC_FAULT config space to determine what went wrong.
    // Do not return early on error, we must take the GPU down.
    //
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC, &secDebug));

    MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC:0x%x\n",
                            secDebug);
    MODS_REPORT_BUS_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                          MODSDRV_BUS_ERROR_CODE_VSEC_DEBUG_SEC_REGISTER, 0, secDebug);
    NV_PRINTF(LEVEL_FATAL, "SEC_FAULT lockdown detected. This is fatal. "
                            "RM will now shut down. NV_EP_PCFG_GPU_VSEC_DEBUG_SEC: 0x%x\n",
                            secDebug);

    //
    // HW uses directly pins the bits from SYSCTRL to the NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1
    // bits, so decode it using the SYSCTRL definition
    //
#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_SYSCTRL, _SEC_FAULT_BIT_POSITION, field, secDebug) != 0) \
    { \
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC" #field "\n"); \
        MODS_REPORT_BUS_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL, \
                              MODSDRV_BUS_ERROR_CODE_VSEC_DEBUG_SEC_REGISTER, 0, secDebug); \
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_FUSE_POD);
    LOG_SEC_FAULT(_SEC2_SCPM);
    LOG_SEC_FAULT(_SEC2_DCLS);
    LOG_SEC_FAULT(_SEC2_L5_WDT);
    LOG_SEC_FAULT(_GSP_DCLS);
    LOG_SEC_FAULT(_GSP_L5_WDT);
    LOG_SEC_FAULT(_PMU_DCLS);
    LOG_SEC_FAULT(_PMU_L5_WDT);

#undef LOG_SEC_FAULT

    // Some remaining bits don't use the SYSCTRL defines

    // no BAR_FIREWALL_ENGAGE on these chips

    // IFF_POS has a multi-bit value
    data = DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC, _IFF_POS, secDebug);
    if (data != 0)
    {
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS value: 0x%x\n", data);
        MODS_REPORT_BUS_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_BUS_ERROR_CODE_VSEC_DEBUG_SEC_REGISTER, 0, data);
        NV_PRINTF(LEVEL_FATAL, "SEC_2_FAULT type: _IFF_POS value: 0x%x\n", data);
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR,
                      "SEC_FAULT: _IFF_POS value: 0x%x", data);
    }

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

