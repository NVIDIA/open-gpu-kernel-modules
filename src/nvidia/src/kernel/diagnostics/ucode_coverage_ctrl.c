/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file    ucode_coverage_ctrl_cpu.c
 * @brief   This module contains the ucode coverage control interface on the CPU layer
*/
#include "core/prelude.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/subdevice/subdevice_diag.h"
#include "kernel/gpu/intr/intr.h"
#include "ctrl/ctrl208f/ctrl208fucodecoverage.h"
#include "nvstatus.h"
#include "gpu/gsp/kernel_gsp.h"
#include "core/system.h"
#include "kernel/vgpu/rpc.h"
#include "diagnostics/code_coverage_mgr.h"

typedef struct {
    NvU8 taskRmCoverage[BULLSEYE_TASK_RM_COVERAGE_SIZE];
    NvU8 taskVgpuCoverage[MAX_PARTITIONS_WITH_GFID][BULLSEYE_TASK_VGPU_COVERAGE_SIZE];
} SYSMEM_BUFFER_LAYOUT;
SYSMEM_BUFFER_LAYOUT sysmemLayout;

NV_STATUS
diagapiCoverageGetState_KERNEL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams
)
{
    switch (pParams->covType)
    {
        case NV208F_BULLSEYE_COVERAGE_TYPE:
            pParams->bEnabled = NV_TRUE;
            return NV_OK;
        case NV208F_SANITIZER_COVERAGE_TYPE:
            return NV_OK;
        default:
            return NV_ERR_NOT_SUPPORTED;
    }
}

NV_STATUS
diagapiCoverageSetState_KERNEL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    if (pParams->covType == NV208F_BULLSEYE_COVERAGE_TYPE)
    {
        if (pParams->bEnabled && pParams->bClear)
        {
            OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
            if (IS_GSP_CLIENT(pGpu))
            {
                OBJSYS *pSys = SYS_GET_INSTANCE();
                codecovmgrResetCoverage(pSys->pCodeCovMgr, pParams->gfid, pGpu->gpuInstance);
            }
        }
        else
        {
            status = NV_ERR_NOT_SUPPORTED;
        }
    }
#endif
    return status;
}

NvU8* getSysmemBuffer(NvU8 *buffer, NvU32 gfid)
{
    portMemCopy(&sysmemLayout, BULLSEYE_GSP_RM_COVERAGE_SIZE, buffer, BULLSEYE_GSP_RM_COVERAGE_SIZE);
    return (IS_GFID_PF(gfid)) ? sysmemLayout.taskRmCoverage : 
                                sysmemLayout.taskVgpuCoverage[gfid-1];
}

NV_STATUS
diagapiCoverageGetData_KERNEL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    if (pParams->covType == NV208F_BULLSEYE_COVERAGE_TYPE)
    { 
        OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
        if (IS_GSP_CLIENT(pGpu))
        {                  
            KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
            OBJSYS *pSys = SYS_GET_INSTANCE();
            if (pParams->offset > (BULLSEYE_GSP_RM_COVERAGE_SIZE - NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_GSPRM))
            {
                return NV_ERR_INVALID_ARGUMENT;
            }
            /* only get new data and merge if first ctrl call */
            if (pParams->offset == 0)
            {
                NvU8 *pSysmemBuffer = getSysmemBuffer(pKernelGsp->taskRmCoverage.pTaskCoverageBuffer, pParams->gfid);
                codecovmgrMergeCoverage(pSys->pCodeCovMgr, pParams->gfid, pGpu->gpuInstance, pSysmemBuffer);
            }
            NvU8 *pCovData = codecovmgrGetCoverageBuffer(pSys->pCodeCovMgr, pParams->gfid, pGpu->gpuInstance);
            if (pCovData != NULL)
            {
                NvU64 bufferSize = codecovmgrGetCoverageNode(pSys->pCodeCovMgr, pParams->gfid,
                                                             pGpu->gpuInstance)->bufferLength;
                if (pParams->offset <= bufferSize)
                {
                    NvLength copySize = NV_MIN(NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_GSPRM, bufferSize);
                    portMemCopy(pParams->data, copySize, pCovData + pParams->offset, copySize);
                }
            }
        }
    }
#endif
    return status;
}

NV_STATUS
diagapiCtrlCmdUcodeCoverageGetState_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }
    if (status != NV_OK)
    {
        return status;
    }
    return diagapiCoverageGetState(pDiagApi, pParams);
}

NV_STATUS
diagapiCtrlCmdUcodeCoverageSetState_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }
    if (status != NV_OK)
    {
        return status;
    }
    return diagapiCoverageSetState(pDiagApi, pParams);
}

NV_STATUS
diagapiCtrlCmdUcodeCoverageGetData_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        //
        // 1. RPC to GSP-RM to dump latest Bullseye coverage data if this is the first request.
        // 2. Sanitizer coverage handled solely by GSP-RM, RPC to GSP-RM on every sanitizer coverage data request.
        //
        if (pParams->offset == 0 || pParams->covType == NV208F_SANITIZER_COVERAGE_TYPE)
        {
            NV_RM_RPC_CONTROL(pGpu,
                              pRmCtrlParams->hClient,
                              pRmCtrlParams->hObject,
                              pRmCtrlParams->cmd,
                              pRmCtrlParams->pParams,
                              pRmCtrlParams->paramsSize,
                              status);
        }
    }
    if (status != NV_OK)
    {
        return status;
    }
    return diagapiCoverageGetData(pDiagApi, pParams);
}
