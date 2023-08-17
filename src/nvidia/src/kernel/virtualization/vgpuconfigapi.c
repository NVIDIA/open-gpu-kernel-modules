/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "virtualization/vgpuconfigapi.h"

NV_STATUS
vgpuconfigapiConstruct_IMPL
(
    VgpuConfigApi                *pVgpuConfigApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
vgpuconfigapiDestruct_IMPL
(
    VgpuConfigApi *pVgpuConfigApi
)
{
}

void
CliNotifyVgpuConfigEvent
(
    OBJGPU     *pGpu,
    NvU32       notifyIndex
)
{
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS *pNotifyParams
)
{
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_IMPL
(
    VgpuConfigApi *pVgpuConfigApi
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pMigrationCapParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams
)
{

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams
)
{
    pParams->heterogeneousMultiVgpuSupported = NV_FALSE;
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

