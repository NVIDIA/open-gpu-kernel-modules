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
#include "gpu/eng_desc.h"
#include "gpu/gpu_child_class_defs.h"
#include "gpu/mem_mgr/ce_utils.h"
#include "nverror.h"
#include "os/os.h"

#include "published/blackwell/gb202/dev_boot.h"
#include "published/blackwell/gb202/dev_xtl_ep_pcfg_gpu.h"

#include "published/blackwell/gb202/dev_therm.h"
#include "published/blackwell/gb202/dev_therm_addendum.h"

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
static const GPUCHILDPRESENT gpuChildrenPresent_GB202[] =
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
    GPU_CHILD_PRESENT(Spdm, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsplite, 4),
    GPU_CHILD_PRESENT(KernelCcu, 1),
};

const GPUCHILDPRESENT*
gpuGetChildrenPresent_GB202(OBJGPU* pGpu, NvU32* pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB202);
    return gpuChildrenPresent_GB202;
}

/*!
 * @brief Read the pcie spec registers using config cycles
 * 
 * Todo: This function will be common function to read pcie config registers in future 
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  hwDefAddr HW defined register address
 * @param[out] pData     Value of the register
 * @param[in]  func      Function number whose config space to read
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadPcieConfigCycle_GB202
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pData,
    NvU8    func
)
{
    NvU32     domain   = gpuGetDomain(pGpu);
    NvU8      bus      = gpuGetBus(pGpu);
    NvU8      device   = gpuGetDevice(pGpu);
    NvU8      function = 1;
    NV_STATUS status   = NV_OK;

    status = gpuConfigAccessSanityCheck_HAL(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    *pData = osPciReadDword(pGpu->hPci, hwDefAddr);

    return NV_OK;
}

/*!
 * @brief Write to pcie spec registers using config cycles
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 * @param[in] value     Write this value to the register
 * @param[in] func      Function number whose config space to write
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuWritePcieConfigCycle_GB202
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32   value,
    NvU8    func
)
{
    NvU32     domain   = gpuGetDomain(pGpu);
    NvU8      bus      = gpuGetBus(pGpu);
    NvU8      device   = gpuGetDevice(pGpu);
    NvU8      function = 1;
    NV_STATUS status   = NV_OK;

    status = gpuConfigAccessSanityCheck_HAL(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    osPciWriteDword(pGpu->hPci, hwDefAddr, value);

    return NV_OK;
}

//
// Workaround for Bug 5041782.
//
// This function is not created through HAL infrastructure. It needs to be
// called when OBJGPU is not created. HAL infrastructure can't be used for
// this case, so it has been added manually. It will be invoked directly by
// gpumgrWaitForBarFirewall() after checking the GPU devId.
//
// See kfspWaitForSecureBoot_GH100
#define GPU_FSP_BOOT_COMPLETION_TIMEOUT_US 4000000
NvBool gpuWaitForBarFirewall_GB202(NvU32 domain, NvU8 bus, NvU8 device, NvU8 function)
{
    NvU32 data;
    NvU32 timeUs = 0;
    void *hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);

    while (timeUs < GPU_FSP_BOOT_COMPLETION_TIMEOUT_US)
    {
        data = osPciReadDword(hPci,
            NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2);

        // Firewall is lowered if 0
        if (DRF_VAL(_EP_PCFG_GPU,
                    _VSEC_DEBUG_SEC_2,
                    _BAR_FIREWALL_ENGAGE,
                    data) == 0)
        {
            return NV_TRUE;
        }

        osDelayUs(1000);
        timeUs += 1000;
    }

    return NV_FALSE;
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GB202
(
    OBJGPU *pGpu
)
{
    NvU32 secDebug1 = 0;
    NvU32 secDebug2 = 0;
    NvU32 data;

    //
    // Read SEC_FAULT config space to determine what went wrong.
    // Do not return early on error, we must take the GPU down.
    //
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1, &secDebug1));
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2, &secDebug2));

    MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1:0x%x NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2:0x%x\n",
                            secDebug1, secDebug2);
    NV_PRINTF(LEVEL_FATAL, "SEC_FAULT lockdown detected. This is fatal. "
                            "RM will now shut down. NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1: 0x%x"
                            "NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2: 0x%x\n", secDebug1, secDebug2);

    //
    // HW uses directly pins the bits from SYSCTRL to the NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1
    // bits, so decode it using the SYSCTRL definition
    //
    //
    // There are no SEC_FAULTs in SEC_2_FAULT_ERROR so far,
    // but we don't have a way to check for that in the future..
    //
#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_SYSCTRL, _SEC_FAULT_BIT_POSITION, field, secDebug1) != 0) \
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

    // Some remaining bits don't use the SYSCTRL defines

    if (DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC_2, _BAR_FIREWALL_ENGAGE, secDebug2) != 0)
    {
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC _BAR_FIREWALL_ENGAGE\n");
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: _BAR_FIREWALL_ENGAGE\n");
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR,
                      "SEC_FAULT: _BAR_FIREWALL_ENGAGE" );
    }

    // IFF_POS has a multi-bit value
    data = DRF_VAL(_EP_PCFG_GPU, _VSEC_DEBUG_SEC_2, _IFF_POS, secDebug2);
    if (data != 0)
    {
        MODS_ARCH_ERROR_PRINTF("NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_IFF_POS value: 0x%x\n", data);
        NV_PRINTF(LEVEL_FATAL, "SEC_2_FAULT type: _IFF_POS value: 0x%x\n", data);
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR,
                      "SEC_2_FAULT: _IFF_POS value: 0x%x", data);
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
 *  @brief Return the first async LCE
 *
 *  @param[in]  pGpu   OBJGPU pointer
 *
 *  On success this function returns the first async LCE
 *  On failure it will return engine type NULL.
 */
NvU32
gpuGetFirstAsyncLce_GB202
(
    OBJGPU  *pGpu
)
{
    NV_STATUS status = NV_OK;
    NvU32 pCeInstance;

    status = ceutilsGetFirstAsyncCe(NULL, pGpu, 0, 0, &pCeInstance, NV_TRUE);
    if (status != NV_OK)
    {
        return NV2080_ENGINE_TYPE_NULL;
    }

    return NV2080_ENGINE_TYPE_COPY(pCeInstance);
}

/*!
 * @brief Check if GSP-FMC Inst_in_sys ucode needs to be booted.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @return NV_TRUE if GSP Inst_in_sys FMC needs to be booted, or NV_FALSE otherwise
 */
NvBool
gpuIsGspToBootInInstInSysMode_GB202
(
    OBJGPU    *pGpu
)
{

    return NV_FALSE;
}

/*!
 * @brief Check if internal SKU fuse is enabled
 *
 * Reads internal SKU fuse value to determine if chip is for internal use only.
 *
 * @param[in]  pGpu            OBJGPU pointer
 *
 * @return NV_TRUE if internal SKU fuse enabled, NV_FALSE otherwise
 */
NvBool
gpuIsInternalSkuFuseEnabled_GB202
(
    OBJGPU *pGpu
)
{
    NvBool bInternalSkuEnabled = NV_FALSE;

    return bInternalSkuEnabled;
}

/*!
 * @brief Check if GRCE that is present is required or not
 *
 * @param[in]  pGpu            OBJGPU pointer
 * @param[in]  engDesc
 *
 * @return NV_TRUE if GRCE that is present is required, NV_FALSE otherwise
 */
NvBool
gpuRequireGrCePresence_GB202
(
    OBJGPU *pGpu,
    ENGDESCRIPTOR  engDesc
)
{
    KernelFifo *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);
    NvBool    bSupported;

    NV_ASSERT_OR_RETURN(pKernelFifo != NULL, NV_FALSE);

    NvBool isEnginePresent = (kfifoCheckEngine_HAL(pGpu, pKernelFifo,
                                         engDesc,
                                         &bSupported) == NV_OK &&
                                         bSupported);

    RM_ENGINE_TYPE   rmCeEngineType = RM_ENGINE_TYPE_COPY(GET_CE_IDX(engDesc));

    if (isEnginePresent)
    {
        // NOTE: This is a temporary WAR. This will be updated with a generic fix by finding partnered GR.
        if ((rmCeEngineType == RM_ENGINE_TYPE_COPY1 && !(kfifoCheckEngine_HAL(pGpu, pKernelFifo,
                                     ENG_GR(1),
                                     &bSupported) == NV_OK &&
               bSupported)) ||
           (rmCeEngineType == RM_ENGINE_TYPE_COPY2 && !(kfifoCheckEngine_HAL(pGpu, pKernelFifo,
                                     ENG_GR(2),
                                     &bSupported) == NV_OK &&
               bSupported)) ||
           (rmCeEngineType == RM_ENGINE_TYPE_COPY3 && !(kfifoCheckEngine_HAL(pGpu, pKernelFifo,
                                     ENG_GR(3),
                                     &bSupported) == NV_OK &&
               bSupported)))
        {
            isEnginePresent = NV_FALSE;
        }
    }
    return isEnginePresent;
}
