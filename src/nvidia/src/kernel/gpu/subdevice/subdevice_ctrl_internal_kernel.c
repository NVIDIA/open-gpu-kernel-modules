/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief This module contains the gpu control interfaces for the
 *        subdevice (NV20_SUBDEVICE_0) class. Subdevice-level control calls
 *        are directed unicast to the associated GPU.
 *        File contains ctrls related to general GPU
 */

#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

NV_STATUS
subdeviceCtrlCmdInternalGetChipInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams
)
{
    NvU32 i;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    pParams->chipSubRev     = gpuGetChipSubRev_HAL(pGpu);
    pParams->isCmpSku       = gpuGetIsCmpSku_HAL(pGpu);
    pParams->pciDeviceId    = pGpu->idInfo.PCIDeviceID;
    pParams->pciSubDeviceId = pGpu->idInfo.PCISubDeviceID;
    pParams->pciRevisionId  = pGpu->idInfo.PCIRevisionID;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pParams->regBases); i++)
    {
        if (gpuGetRegBaseOffset_HAL(pGpu, i, &pParams->regBases[i]) != NV_OK)
            pParams->regBases[i] = 0xFFFFFFFF;
    }


    return NV_OK;
}