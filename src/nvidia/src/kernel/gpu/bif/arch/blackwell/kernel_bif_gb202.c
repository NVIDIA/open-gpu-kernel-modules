/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"

#include "published/blackwell/gb202/dev_xtl_ep_pcfg_gpu.h"

/*!
 * @brief Save PCIe Config space for Fn1
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif Kernel BIF object pointer
 * @param[in]  pRegmapRef Pointer to config space register map structure
 * @param[in]  maxSize    Max size of the Fn1 config space
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifSavePcieConfigRegistersFn1_GB202
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    const PKBIF_XVE_REGMAP_REF pRegmapRef,
    NvU32      maxSize
)
{
    NV_STATUS status;
    NvU32     regOffset;
    NvU32     bufOffset = 0;
    NvU8      function  = 1;

    // Read and save entire config space
    for (regOffset = 0x0; regOffset < maxSize; regOffset+=0x4)
    {
        status = gpuReadPcieConfigCycle_HAL(pGpu, regOffset, &pRegmapRef->bufBootConfigSpace[bufOffset], function);
        if (status == NV_ERR_INVALID_STATE)
        {
            NV_PRINTF(LEVEL_ERROR, "Config read failed.\n");
            return status;
        }
        bufOffset++;
    }

    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED, NV_TRUE);

    return NV_OK;
}

/*!
 * @brief Restore PCIe Config space for Fn1
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 * @param[in] pRegmapRef  Pointer to config space register map structure
 * @param[in] maxSize     Max size of the Fn1 config space
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifRestorePcieConfigRegistersFn1_GB202
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    const PKBIF_XVE_REGMAP_REF pRegmapRef,
    NvU32      maxSize
)
{
    NvU32      domain    = gpuGetDomain(pGpu);
    NvU8       bus       = gpuGetBus(pGpu);
    NvU8       device    = gpuGetDevice(pGpu);
    NvU8       function  = 1;
    void      *pHandle;
    NvU32      regOffset;
    NvU32      bufOffset = 0;
    NV_STATUS  status;

    pHandle = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    NV_ASSERT_OR_RETURN(pHandle, NV_ERR_INVALID_POINTER);

    // Restore entire config space
    for (regOffset = 0x0; regOffset < maxSize; regOffset+=0x4)
    {
        status = gpuWritePcieConfigCycle_HAL(pGpu, regOffset, pRegmapRef->bufBootConfigSpace[bufOffset], function);
        if (status == NV_ERR_INVALID_STATE)
        {
            NV_PRINTF(LEVEL_ERROR, "Config write failed.\n");
            NV_ASSERT(0);
            return status;
        }
        bufOffset++;
    }

    return NV_OK;
}

/*!
 * @brief Initialize LTR settings from config space
 *
 * param[in]  pGpu        GPU object pointer
 * param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifInitLtr_GB202
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJCL     *pCl  = SYS_GET_CL(pSys);
    NV_STATUS  status;
    NvU32      regVal;

    if (pCl->getProperty(pCl, PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED))
    {
        status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2, &regVal);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2\n");
            return;
        }

        regVal = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS_2,
                                 _LTR_ENABLE, 0x1, regVal);

        status = GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2, regVal);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to write NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2\n");
            return;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "LTR is disabled in the hierarchy\n");
    }
}

/*!
 * @brief Polling BAR firewall disengage when resuming from D3
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @returns NV_OK
 */
NV_STATUS
kbifPollBarFirewallDisengage_GB202
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif
)
{
    RMTIMEOUT timeout;
    NvU32     val;

    //
    // Polling for CFG BAR firewall disenage
    // In Blackwell, Bootfsm triggers FSP execution out of chip reset.
    // FSP disengage firewall around CRS release.
    // And CRS release generally happens ~800msec,
    // so the worst case scenario is 800msec.
    // At this point CRS will be already released as
    // RM was polling for CFG space vendor id prior to reach here,
    // so using 500msec timeout should be enough per FSP team suggested.
    //
    
    gpuSetTimeout(pGpu, NV_MAX(gpuScaleTimeout(pGpu, 500000), pGpu->timeoutData.defaultus),
                  &timeout, GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2, &val);
    while (!FLD_TEST_DRF(_EP_PCFG_GPU, _VSEC_DEBUG_SEC_2, _BAR_FIREWALL_ENGAGE, _INIT, val))
    {
        GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2, &val);
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "Timeout polling CFG BAR firewall disengage.\n");
            DBG_BREAKPOINT();
            return NV_ERR_TIMEOUT;
        }
        osDelayUs(100);
    }

    return NV_OK;
}

/*!
 * @brief determine PCIe requester/outbound atomics support 
 *
 * @param[in] pGpu  GPU object pointer
 * @param[in] pKernelBif KernelBif object pointer
 *
 * @return  'NV_TRUE' if PCIe requester/outbound atomics is supported, 'NV_FALSE' otherwise.
 */
NvBool kbifAllowGpuReqPcieAtomics_GB202
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    // Bug 4919951
    if (gpuIsTeslaBranded(pGpu))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}
