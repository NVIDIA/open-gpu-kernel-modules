/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file    ucode_instrumentation_ctrl.c
 * @brief   This module contains the ucode instrumentation control interface
 *
 */
#include "core/prelude.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/subdevice/subdevice_diag.h"
#include "kernel/gpu/intr/intr.h"
#include "ctrl/ctrl208f/ctrl208fucodeinstrumentation.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "nvstatus.h"
#include "gpu/gsp/kernel_gsp.h"
#include "core/system.h"
#include "kernel/vgpu/rpc.h"
#include "diagnostics/instrumentation_manager.h"
#include "virtualization/common_vgpu_mgr.h"

static NV_STATUS
_diagapiCtrlCmdUcodeInstrumentationValidateGfid
(
    NvU32 ucode,
    NvU32 gfid
)
{
    if (ucode == NV208F_UCODE_INSTRUMENTATION_GSP_TASK_VGPU)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                           gfid > 0 && gfid <= VGPU_MAX_GFID,
                           NV_ERR_INVALID_ARGUMENT);
    }

    return NV_OK;
}

NV_STATUS
diagapiCtrlCmdUcodeInstrumentationGetState_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS internalParams;

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _diagapiCtrlCmdUcodeInstrumentationValidateGfid(pParams->ucode, pParams->gfid));

    internalParams.ucode = pParams->ucode;
    internalParams.gfid = pParams->gfid;
    internalParams.bEnabled = pParams->bEnabled;
    internalParams.bClear = pParams->bClear;
    internalParams.instrumentationType = pParams->instrumentationType;

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = pRmApi->Control(pRmApi,
                                       pGpu->hInternalClient,
                                       pGpu->hInternalSubdevice,
                                       NV2080_CTRL_CMD_INTERNAL_UCODE_INSTRUMENTATION_GET_STATE,
                                       &internalParams,
                                       sizeof(internalParams));

    pParams->bEnabled = internalParams.bEnabled;
    pParams->bClear = internalParams.bClear;

    return status;
}

NV_STATUS
diagapiCtrlCmdUcodeInstrumentationSetState_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS internalParams;

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _diagapiCtrlCmdUcodeInstrumentationValidateGfid(pParams->ucode, pParams->gfid));

    internalParams.ucode = pParams->ucode;
    internalParams.gfid = pParams->gfid;
    internalParams.bEnabled = pParams->bEnabled;
    internalParams.bClear = pParams->bClear;
    internalParams.instrumentationType = pParams->instrumentationType;

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_UCODE_INSTRUMENTATION_SET_STATE,
                             &internalParams,
                             sizeof(internalParams));

    if (status == NV_OK &&
        pParams->instrumentationType == NV208F_BULLSEYE_COVERAGE_TYPE &&
        pParams->bEnabled &&
        pParams->bClear)
    {
        instrumentationmanagerReset(pSys->pInstrumentationManager, pParams->gfid, pGpu->gpuInstance);
    }

    return status;
}

NV_STATUS
diagapiCtrlCmdUcodeInstrumentationGetData_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pInternalParams;

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _diagapiCtrlCmdUcodeInstrumentationValidateGfid(pParams->ucode, pParams->gfid));

    pInternalParams = portMemAllocNonPaged(sizeof(*pInternalParams));
    if (pInternalParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for internal instrumentation params\n");
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pInternalParams, 0, sizeof(*pInternalParams));
    pInternalParams->ucode = pParams->ucode;
    pInternalParams->gfid = pParams->gfid;
    pInternalParams->offset = pParams->offset;
    pInternalParams->instrumentationType = pParams->instrumentationType;

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_UCODE_INSTRUMENTATION_GET_DATA,
                             pInternalParams,
                             sizeof(*pInternalParams));

    if (status == NV_OK)
    {
        NvU32 copySize = pInternalParams->dataSize;
        if (copySize > sizeof(pParams->data))
        {
            copySize = sizeof(pParams->data);
        }
        portMemCopy(pParams->data, copySize, pInternalParams->data, copySize);
        pParams->dataSize = copySize;
        pParams->bComplete = pInternalParams->bComplete;

        if (pParams->instrumentationType == NV208F_BULLSEYE_COVERAGE_TYPE &&
            pParams->dataSize > 0)
        {
            OBJSYS *pSys = SYS_GET_INSTANCE();
            NvU8 *pBuffer;

            if ((pParams->offset > BULLSEYE_GSP_RM_COVERAGE_SIZE) ||
                (pParams->dataSize > (BULLSEYE_GSP_RM_COVERAGE_SIZE - pParams->offset)))
            {
                portMemFree(pInternalParams);
                return NV_ERR_INVALID_ARGUMENT;
            }

            // Merge this chunk of data into the instrumentation buffer
            instrumentationmanagerMerge(pSys->pInstrumentationManager,
                                        pParams->gfid,
                                        pGpu->gpuInstance,
                                        pParams->data,
                                        pParams->offset,
                                        pParams->dataSize);

            pBuffer = instrumentationmanagerGetBuffer(pSys->pInstrumentationManager,
                                                      pParams->gfid,
                                                      pGpu->gpuInstance);
            if (pBuffer != NULL)
            {
                portMemCopy(pParams->data, pParams->dataSize,
                            pBuffer + pParams->offset, pParams->dataSize);
            }
        }
    }

    portMemFree(pInternalParams);
    return status;
}
