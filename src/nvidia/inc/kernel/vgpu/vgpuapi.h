/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_vgpuapi_nvoc.h"

#ifndef _VGPUAPI_H_
#define _VGPUAPI_H_

#include "core/core.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "rmapi/control.h"
#include "ctrl/ctrla080.h"

NVOC_PREFIX(vgpuapi) class VgpuApi : GpuResource
{
public:
    NV_STATUS vgpuapiConstruct(VgpuApi *pVgpuApi, CALL_CONTEXT *pCallContext,
                                         RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
                                    : GpuResource(pCallContext, pParams);
    void vgpuapiDestruct(VgpuApi *pVgpuApi);

    NODE       node;
    NvHandle   handle;
    NvHandle   hDevice;

    //
    // RMCTRL Exported methods -- Category: VGPU_DISPLAY
    //
    RMCTRL_EXPORT(NVA080_CTRL_CMD_VGPU_DISPLAY_SET_SURFACE_PROPERTIES,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties(VgpuApi *pVgpuApi,
                                NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams);

    RMCTRL_EXPORT(NVA080_CTRL_CMD_VGPU_DISPLAY_CLEANUP_SURFACE,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS vgpuapiCtrlCmdVgpuDisplayCleanupSurface(VgpuApi *pVgpuApi,
                                NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams);

    //
    // RMCTRL Exported methods -- Category: VGPU_OTHERS
    //
    RMCTRL_EXPORT(NVA080_CTRL_CMD_VGPU_GET_CONFIG,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS vgpuapiCtrlCmdVGpuGetConfig(VgpuApi *pVgpuApi,
                                NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams);

};

#endif // _VGPUAPI_H_
