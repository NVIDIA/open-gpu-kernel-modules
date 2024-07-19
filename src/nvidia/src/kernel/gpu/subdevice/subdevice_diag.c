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

#include "core/core.h"
#include "os/os.h"
#include "gpu/gpu.h"
#include "rmapi/rs_utils.h"

#include "gpu/subdevice/subdevice_diag.h"

#include "class/cl208f.h" // NV208F_NOTIFIERS_MAXCOUNT
#include "ctrl/ctrl208f/ctrl208fevent.h" // NV208F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE

#include "resserv/rs_client.h"



//
// All state associated with subdevice diag objects is protected by
// the API sleep lock (there is no intersection with ISR/DPC code).
//
NV_STATUS diagapiConstruct_IMPL
(
    DiagApi                      *pDiagApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NvU32 i;

    for (i = 0; i < NV208F_NOTIFIERS_MAXCOUNT; i++)
    {
        pDiagApi->notifyActions[i] =
            NV208F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    return NV_OK;
}

NV_STATUS
diagapiControl_IMPL
(
    DiagApi                        *pDiagApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    RmCtrlParams *pRmCtrlParams = pParams->pLegacyParams;
    gpuresControlSetup(pParams, staticCast(pDiagApi, GpuResource));

    (void)pRmCtrlParams;
    NV_PRINTF(LEVEL_INFO, "gpuControlSubDevice: cmd 0x%x\n",
              pRmCtrlParams->cmd);

    return gpuresControl_IMPL(staticCast(pDiagApi, GpuResource), pCallContext, pParams);
}


NV_STATUS
diagapiControlFilter_IMPL
(
    DiagApi *pDiagApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    RmCtrlParams *pRmCtrlParams = pParams->pLegacyParams;

    if (IS_VIRTUAL(pGpu))
    {
        switch (pRmCtrlParams->cmd)
        {
            // For Guest OS, only below RM controls are supported.
            case NV208F_CTRL_CMD_FIFO_ENABLE_VIRTUAL_CONTEXT:
            case NV208F_CTRL_CMD_FIFO_CHECK_ENGINE_CONTEXT:
            case NV208F_CTRL_CMD_DMA_GET_VAS_BLOCK_DETAILS:
                break;

            // Fall though for unsupported CTRL
            default:
                return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;
}

