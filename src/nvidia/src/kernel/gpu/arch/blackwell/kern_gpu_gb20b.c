/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu_child_class_defs.h"
#include "os/os.h"
#include "gpu/eng_desc.h"
#include "nverror.h"

#include "published/blackwell/gb20b/dev_boot.h"
#include "published/blackwell/gb20b/dev_xtl_ep_pcfg_gpu.h"

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

static const GPUCHILDPRESENT gpuChildrenPresent_GB20B[] =
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
    GPU_CHILD_PRESENT(KernelCE, 2),
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
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelGsplite, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB20B(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB20B);
    NV_PRINTF(LEVEL_INFO, "pNumEntries[%u]\n", *pNumEntries);
    return gpuChildrenPresent_GB20B;
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GB20B
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
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_SEC2_DCLS);
    LOG_SEC_FAULT(_SEC2_L5_WDT);
    LOG_SEC_FAULT(_GSP_DCLS);
    LOG_SEC_FAULT(_GSP_L5_WDT);
    LOG_SEC_FAULT(_PMU_DCLS);
    LOG_SEC_FAULT(_PMU_L5_WDT);
    LOG_SEC_FAULT(_GPMVDD_VMON);
    LOG_SEC_FAULT(_GPCVDD_VMON);
    LOG_SEC_FAULT(_SOC2GPU_SEC_FAULT_FUNCTION_LOCKDOWN_REQ);
    LOG_SEC_FAULT(_FUNCTION_LOCKDOWN);
    LOG_SEC_FAULT(_DEVICE_LOCKDOWN);

#undef LOG_SEC_FAULT

    // Some remaining bits don't use the SYSCTRL defines

    // no BAR_FIREWALL_ENGAGE on these chips

    // IFF_POS has a multi-bit value
    data = DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC, _IFF_POS, secDebug);
    if (data != 0)
    {
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS value: 0x%x\n", data);
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

