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
#include "ctrl/ctrl2080.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "virtualization/vgpuconfigapi.h"

NV_STATUS
kernelhostvgpudeviceshrConstruct_IMPL
(
    KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr
)
{
    return NV_OK;
}

void
kernelhostvgpudeviceshrDestruct_IMPL
(
    KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr
)
{
}

NV_STATUS
kernelhostvgpudeviceapiConstruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool
kernelhostvgpudeviceapiCanCopy_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
    return NV_TRUE;
}

NV_STATUS
kernelhostvgpudeviceapiCopyConstruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
destroyKernelHostVgpuDeviceShare(OBJGPU *pGpu, KernelHostVgpuDeviceShr* pShare)
{
}

void
kernelhostvgpudeviceapiDestruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceGetGuestFbInfo
(
    OBJGPU                      *pGpu,
    KERNEL_HOST_VGPU_DEVICE     *pKernelHostVgpuDevice,
    VGPU_DEVICE_GUEST_FB_INFO   *pFbInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceSetGuestFbInfo
(
    OBJGPU *pGpu,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64 offset,
    NvU64 length
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
    return NV_ERR_NOT_SUPPORTED;
}
