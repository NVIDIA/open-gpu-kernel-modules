/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/bif/kernel_bif.h"
#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/sdk-structures.h"

/*!
 * @brief Set the coherency cap for vGPU
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifInitDmaCaps_VF
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    //
    // Ideally, we should query this from host RM.
    // Current all VGPU systems support coherency.
    // So it should be OK to hardcode it.
    //
    // As per Neo, we only want to expose CPU cache snoop caps.
    // This is to force all VGPU memory allocations to be coherent.
    //
    pKernelBif->dmaCaps = REF_DEF(BIF_DMA_CAPS_SNOOP, _CTXDMA);
}

/*!
 * @brief Get static info for VGPU
 *
 * @param[in]  pGpu     GPU object pointer
 * @param[in]  pParams  Static info parameters
 *
 * @return     NV_OK
 */
NV_STATUS
subdeviceCtrlCmdBifGetStaticInfo_VF
(
    Subdevice                                       *pSubdevice,
    NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    //
    // Ideally, we should query this from host RM.
    // However, except for C2C, other parameters are do-not-care
    // on guest RM as of today, hence enforcing default values
    //
    pParams->bIsC2CLinkUp = pVSI->bC2CLinkUp;
    pParams->dmaWindowStartAddress  = 0ull;
    pParams->bPcieGen4Capable       = NV_FALSE;
    pParams->bIsDeviceMultiFunction = NV_FALSE;
    pParams->bGcxPmuCfgSpaceRestore = NV_FALSE;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_VF
(
    Subdevice                                            *pSubdevice,
    NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *pParams
)
{
    // Atomics not supported in VF. See bug 3497203.
    pParams->atomicsCaps = 0x0;
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_VF
(
    Subdevice                                            *pSubdevice,
    NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *pParams
)
{
    // Atomics not supported in VF. See bug 3497203.
    pParams->atomicsCaps = 0x0;
    return NV_OK;
}
