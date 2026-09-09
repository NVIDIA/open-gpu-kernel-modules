/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   File: wpps_config_session.c
 *
 *   Description:
 *       WppsConfigSession class implementation.  Validates
 *       NV_RM_CAP_SYS_WPPS_ACCESS on allocation, then provides
 *       NVD0CD_CTRL_CMD_SET_PROFILES which forwards to the physical
 *       RM implementation for the actual perf modes set.
 *
 *****************************************************************************/

#define NVOC_WPPS_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED



#include "core/core.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "class/cld0cd.h"
#include "ctrl/ctrld0cd.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "kernel/gpu/perf/wpps_config_session.h"
#include "os/os.h"

NV_STATUS
wppsConfigSessionConstruct_IMPL
(
    WppsConfigSession *pWppsConfigSession,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NVD0CD_ALLOC_PARAMETERS *pUserParams = pParams->pAllocParams;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    osRmCapInitDescriptor(&pWppsConfigSession->dupedCapDescriptor);

    return osRmCapAcquire(pSys->pOsRmCaps,
                          NV_RM_CAP_SYS_WPPS_ACCESS,
                          pUserParams->capDescriptor,
                          &pWppsConfigSession->dupedCapDescriptor);
}

void
wppsConfigSessionDestruct_IMPL
(
    WppsConfigSession *pWppsConfigSession
)
{
    osRmCapRelease(pWppsConfigSession->dupedCapDescriptor);
}

NvBool
wppsConfigSessionCanCopy_IMPL
(
    WppsConfigSession *pWppsConfigSession
)
{
    return NV_FALSE;
}

NV_STATUS
wppsConfigSessionCtrlCmdSetProfiles_IMPL
(
    WppsConfigSession *pWppsConfigSession,
    NVD0CD_CTRL_SET_PROFILES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pWppsConfigSession);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    ct_assert(sizeof(NVD0CD_CTRL_SET_PROFILES_PARAMS) ==
              sizeof(NV2080_CTRL_INTERNAL_PERF_CLIENT_PERF_MODES_SET_CONTROL_PARAMS));

    return pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_PERF_CLIENT_PERF_MODES_SET_CONTROL,
                           pParams, sizeof(*pParams));
}
