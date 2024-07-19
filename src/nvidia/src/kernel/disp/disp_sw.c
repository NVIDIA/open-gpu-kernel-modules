/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/mem_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmapi/control.h"
#include "rmapi/mapping_list.h"
#include "gpu/device/device.h"
#include "gpu/disp/dispsw.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

#include "class/cl9072.h"

//---------------------------------------------------------------------------
//
//  Class object creation and destruction
//
//---------------------------------------------------------------------------

NV_STATUS
dispswConstruct_IMPL
(
    DispSwObject                 *pDispSw,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pDispSw);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    RM_API *pRmApi   = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32 hClient        = pGpu->hInternalClient;
    NvU32 hSubdevice     = pGpu->hInternalSubdevice;
    NV_STATUS status;

    NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS ctrlParams = { 0 };
    NV9072_ALLOCATION_PARAMETERS *pUserParams = pParams->pAllocParams;

    if (!pUserParams)
    {
        NV_ASSERT(pUserParams);
        return (NV_ERR_INVALID_ARGUMENT);
    }

    if (!pKernelDisplay)
    {
        NV_PRINTF(LEVEL_INFO, "Display is not enabled, can't create class\n");
        return (NV_ERR_INVALID_ARGUMENT);
    }

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC error, can't get the displaymask and number of heads\n");
        return status;
    }

    if (pUserParams->logicalHeadId >= ctrlParams.numHeads)
    {
        NV_PRINTF(LEVEL_ERROR, "invalid logical head number: %d\n",
                  pUserParams->logicalHeadId);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (pUserParams->displayMask)
    {
        if (!(pUserParams->displayMask & ctrlParams.displayMask))
        {
            NV_PRINTF(LEVEL_ERROR, "Device not active: 0x%08x, RM display mask: 0x%08x\n",
                      pUserParams->displayMask, ctrlParams.displayMask);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    pDispSw->DispCommon.DispObject   = pDispSw;
    pDispSw->DispCommon.Head         = pUserParams->logicalHeadId;

    return (NV_OK);
}

void
dispswDestruct_IMPL
(
    DispSwObject *pDispSw
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pDispSw, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead    *pKernelHead    = KDISP_GET_HEAD(pKernelDisplay, pDispSw->DispCommon.Head);

    chandesIsolateOnDestruct(pChannelDescendant);

    //
    // Cleanup any pending VBlank callbacks
    //
    // These should only be set up on the parent
    //
    NV_ASSERT(gpumgrIsParentGPU(pGpu));
    kheadDeleteVblankCallback(pGpu, pKernelHead, &(pDispSw->NotifyOnVBlank.Callback));
    kheadDeleteVblankCallback(pGpu, pKernelHead, &(pDispSw->DispCommon.Semaphore.ReleaseCallback));
}

NV_STATUS dispswReleaseSemaphoreAndNotifierFill
(
    OBJGPU   *pGpu,
    NvU64     gpuVA,
    NvU32     vaSpace,
    NvU32     releasevalue,
    NvU32     flags,
    NvU32     completionStatus,
    Device   *pDevice
)
{
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
    NvBool     bFound = NV_FALSE;
    NV_STATUS  status;

    if (flags & F_SEMAPHORE_ADDR_VALID)
    {
        bFound = CliGetDmaMappingInfo(RES_GET_CLIENT(pDevice),
                                      RES_GET_HANDLE(pDevice),
                                      vaSpace,
                                      gpuVA,
                                      gpumgrGetDeviceGpuMask(pGpu->deviceInstance),
                                      &pDmaMappingInfo);
        if (!bFound)
            return NV_ERR_INVALID_ADDRESS;
    }
    else if (flags & F_SEMAPHORE_RELEASE)
    {
        status =  semaphoreFillGPUVA(pGpu,
                                     pDevice,
                                     vaSpace,
                                     gpuVA,
                                     releasevalue,
                                     0 /* Index */,
                                     NV_TRUE);
        return status;
    }
    else if (flags & F_NOTIFIER_FILL)
    {
        status = notifyFillNotifierGPUVA(pGpu,
                                         pDevice,
                                         vaSpace,
                                         gpuVA,
                                         releasevalue, /* Info32 */
                                         0, /* Info16 */
                                         completionStatus,
                                         NV9072_NOTIFIERS_NOTIFY_ON_VBLANK /* Index */);
        return status;
    }
    return NV9072_NOTIFICATION_STATUS_DONE_SUCCESS;
}
