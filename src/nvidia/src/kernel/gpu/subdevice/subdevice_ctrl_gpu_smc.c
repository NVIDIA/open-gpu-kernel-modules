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

#include "core/core.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "compute/mps_api.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"

#include "class/clc637.h"

//
// Control call to provide information about partitions which can be created on
// this GPU.
//
NV_STATUS
subdeviceCtrlCmdGpuGetSkylineInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_SKYLINE_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS internalParams;
    NvU32 i;

    portMemSet(pParams, 0, sizeof(*pParams));
    portMemSet(&internalParams, 0, sizeof(internalParams));

    NV_CHECK_OR_RETURN(LEVEL_WARNING, !IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Call to physical RM directly in the event static info isn't populated yet
    if ((pStaticInfo == NULL) || (pStaticInfo->pSkylineInfo == NULL))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO,
                        &internalParams,
                        sizeof(internalParams)));
    }
    else
    {
        portMemCopy(&internalParams, sizeof(internalParams),
                    pStaticInfo->pSkylineInfo, sizeof(*pStaticInfo->pSkylineInfo));
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR,internalParams.validEntries <= NV_ARRAY_ELEMENTS(pParams->skylineTable), NV_ERR_INVALID_STATE);

    for (i = 0; i < internalParams.validEntries; i++)
    {
        NvU32 vgpcId;
        pParams->skylineTable[i].maxInstances      = internalParams.skylineTable[i].maxInstances;
        pParams->skylineTable[i].singletonVgpcMask = internalParams.skylineTable[i].singletonVgpcMask;
        pParams->skylineTable[i].computeSizeFlag   = internalParams.skylineTable[i].computeSizeFlag;

        for (vgpcId = 0; vgpcId < NV_ARRAY_ELEMENTS(internalParams.skylineTable[i].skylineVgpcSize); vgpcId++)
        {
            NV_ASSERT_OR_RETURN(vgpcId < NV_ARRAY_ELEMENTS(pParams->skylineTable[i].skylineVgpcSize), NV_ERR_INVALID_STATE);
            pParams->skylineTable[i].skylineVgpcSize[vgpcId] =internalParams.skylineTable[i].skylineVgpcSize[vgpcId];
        }
    }
    pParams->validEntries = internalParams.validEntries;

    return NV_OK;
}
