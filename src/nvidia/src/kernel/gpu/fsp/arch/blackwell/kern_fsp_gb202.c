/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *
 * @file    kern_fsp_gb202.c
 * @brief   Provides the implementation for GB202 chip specific FSP HAL
 *          interfaces.
 */
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/fsp/kern_fsp_retval.h"

#include "published/blackwell/gb202/dev_fsp_pri.h"
#include "published/blackwell/gb202/dev_therm.h"
#include "published/blackwell/gb202/dev_fsp_addendum.h"
#include "published/blackwell/gb202/dev_therm_addendum.h"

#include "nvRmReg.h"
#include "nverror.h"

static NvBool _kfspWaitBootCond_GB202(OBJGPU *pGpu, void *pArg);

static NvBool
_kfspWaitBootCond_GB202
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    //
    // In GB202, Bootfsm triggers FSP execution out of chip reset.
    // FSP writes 0xFF value in NV_THERM_I2CS_SCRATCH register after completion of boot
    //
    return GPU_FLD_TEST_DRF_DEF(pGpu, _THERM_I2CS_SCRATCH, _FSP_BOOT_COMPLETE, _STATUS, _SUCCESS);
}

NV_STATUS
kfspWaitForSecureBoot_GB202
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;

    //
    // Polling for FSP boot complete
    // FBFalcon training during devinit alone takes 2 seconds, up to 3 on HBM3,
    // but the default threadstate timeout on windows is 1800 ms. Increase to 4 seconds
    // for this wait to match MODS GetGFWBootTimeoutMs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    // Increase to 5 seconds for WS SKUs especially needed for SKU's with larger VRAM.
    //
    gpuSetTimeout(pGpu, NV_MAX(gpuScaleTimeout(pGpu, 5000000), pGpu->timeoutData.defaultus),
                  &timeout, GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GB202, NULL, &timeout);

    if (status != NV_OK)
    {
        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
        NV_ERROR_LOG((void*) pGpu, GPU_INIT_ERROR, "Error status 0x%x while polling for FSP boot complete, "
                     "0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
                     status,
                     GPU_REG_RD32(pGpu, NV_THERM_I2CS_SCRATCH_FSP_BOOT_COMPLETE),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));
    }

    return status;
}

/*!
 * @brief Dump debug registers for FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or error if failed
 */
void
kfspDumpDebugState_GB202
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    //
    // Older microcodes did not have the version populated in scratch.
    // They will report a version of 0.
    //
    const NvU32 fspUcodeVersion = GPU_REG_RD_DRF(pGpu, _GFW, _FSP_UCODE_VERSION, _FULL);
    if (fspUcodeVersion > 0)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP microcode v%u.%u\n",
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MAJOR, fspUcodeVersion),
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MINOR, fspUcodeVersion));
    }

    NV_PRINTF(LEVEL_ERROR, "GPU %04x:%02x:%02x\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));
}
