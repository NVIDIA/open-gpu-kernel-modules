/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

//
// Because the field regStatus is set and checked in multiple places in the
// call chain, it must be initialized early on in an entry point function (few
// of them). (Security Bug: 3401950)
// Note: The api doesn't mandate the caller clear the field before calling
//
static void
initRegStatus(NV2080_CTRL_GPU_REG_OP *pRegOps, NvU32 regOpCount)
{
    NvU32 i;

    for (i=0; i < regOpCount; i++)
    {
        pRegOps[i].regStatus = NV2080_CTRL_GPU_REG_OP_STATUS_SUCCESS;
    }
}

//
// Do some pre validation of each regop such as offset and access. There is
// another validation in gpuValidateRegOp above that runs at a later time.
// EXECUTION: CPU-RM if the call is not from GSP-PLUGIN
//            GSP-RM if the call is from GSP-PLUGIN only
//
NV_STATUS
gpuValidateRegOps
(
    OBJGPU *pGpu,
    NV2080_CTRL_GPU_REG_OP *pRegOps,
    NvU32 regOpCount,
    NvBool bNonTransactional,
    NvBool isClientGspPlugin
)
{
    NvU8   regStatus;
    NvU32  i;

    for (i = 0; i < regOpCount; i++)
    {
        regStatus = NV2080_CTRL_GPU_REG_OP_STATUS_SUCCESS;

        if (isClientGspPlugin)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
        else
        {
            NV_STATUS status;

            status = gpuValidateRegOffset(pGpu, pRegOps[i].regOffset);
            if (status != NV_OK)
            {
                regStatus = NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_OFFSET;
            }
        }
        if (regStatus != NV2080_CTRL_GPU_REG_OP_STATUS_SUCCESS)
        {
            pRegOps[i].regStatus |= regStatus;

            if (!bNonTransactional)
            {
                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    return NV_OK;
}

static NV_STATUS
subdeviceCtrlCmdGpuExecRegOps_cmn
(
    Subdevice    *pSubdevice,
    NvHandle      hClientTarget,
    NvHandle      hChannelTarget,
    NvU32         bNonTransactional,
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo,
    NV2080_CTRL_GPU_REG_OP   *pRegOps,
    NvU32        *pOpSmIds,
    NvU32         regOpCount,
    NvBool        isClientGspPlugin
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS         status = NV_OK;
    CALL_CONTEXT     *pCallContext = resservGetTlsCallContext();
    RmCtrlParams     *pRmCtrlParams = pCallContext->pControlParams;
    NvBool            bUseMigratableOps;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance));

    NV_PRINTF(LEVEL_INFO, "client 0x%x channel 0x%x\n", hClientTarget,
              hChannelTarget);

    // sanity check list size
    if (regOpCount == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "regOpCount is 0\n");
        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    if (pRegOps == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "regOps is NULL\n");
        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    if ((hClientTarget == 0) != (hChannelTarget == 0))
    {
        NV_PRINTF(LEVEL_ERROR,
            "hClientTarget and hChannelTarget must both be set or both be 0\n");
        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    //
    // pOpSmIds should will only be non-NULL when this code path is being
    // used by the migratable ops function.
    //
    bUseMigratableOps = (pOpSmIds != NULL);
    //
    // If in a VM, do RPC to the host that has hw access.
    //
    // In case of SRIOV, vGPU guest UMD such as CUDA driver read/write
    // PF registers through this RMCtrl. Since, it is a PF register the
    // validation is to be performed by host RM only. Hence, we need to
    // call RPC early and skip any validation check in Guest RM.
    //
    if (IS_VIRTUAL(pGpu))
    {
        //
        // If this function is being used by a MIGRATABLE call,
        // we route the GSP call to normal DMA controller
        //
        if (bUseMigratableOps)
        {
            NV_RM_RPC_CONTROL(pGpu,
                  pRmCtrlParams->hClient,
                  pRmCtrlParams->hObject,
                  pRmCtrlParams->cmd,
                  pRmCtrlParams->pParams,
                  pRmCtrlParams->paramsSize,
                  status);
        }
        else
        {
            NV_RM_RPC_GPU_EXEC_REG_OPS(pGpu,
                                       pRmCtrlParams->hClient,
                                       pRmCtrlParams->hObject,
                                       pRmCtrlParams->pParams,
                                       pRegOps,
                                       status);
        }
        return status;
    }

    // init once, only in monolithic-rm or the cpu-rm, or gsp-rm if the call
    // is from the gsp plugin
    if (!RMCFG_FEATURE_PLATFORM_GSP || isClientGspPlugin)
    {
        initRegStatus(pRegOps, regOpCount);
    }

    status = gpuValidateRegOps(pGpu, pRegOps, regOpCount, bNonTransactional,
                               isClientGspPlugin);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        //
        // If this function is being used by a MIGRATABLE_OPS call,
        // we route the GSP call to normal DMA controller
        //
        if (bUseMigratableOps)
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

            status = pRmApi->Control(pRmApi,
                                     pRmCtrlParams->hClient,
                                     pRmCtrlParams->hObject,
                                     pRmCtrlParams->cmd,
                                     pRmCtrlParams->pParams,
                                     pRmCtrlParams->paramsSize);
        }
        else
        {
            NV_RM_RPC_GPU_EXEC_REG_OPS(pGpu,
                                       pRmCtrlParams->hClient,
                                       pRmCtrlParams->hObject,
                                       pRmCtrlParams->pParams,
                                       pRegOps,
                                       status);
        }
        return status;
    }

    return status;
}

//
// subdeviceCtrlCmdGpuExecRegOps
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuExecRegOps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams
)
{
    return subdeviceCtrlCmdGpuExecRegOps_cmn(pSubdevice,
                                             pRegParams->hClientTarget,
                                             pRegParams->hChannelTarget,
                                             pRegParams->bNonTransactional,
                                             pRegParams->grRouteInfo,
                                             pRegParams->regOps,
                                             NULL,
                                             pRegParams->regOpCount,
                                             NV_FALSE);
}

//
// subdeviceCtrlCmdGpuMigratableOps
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuMigratableOps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pRegParams
)
{
    if (pRegParams->regOpCount > NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid regOpCount: %ud\n",
                  pRegParams->regOpCount);
        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    return subdeviceCtrlCmdGpuExecRegOps_cmn(pSubdevice,
                                             pRegParams->hClientTarget,
                                             pRegParams->hChannelTarget,
                                             pRegParams->bNonTransactional,
                                             pRegParams->grRouteInfo,
                                             pRegParams->regOps,
                                             pRegParams->smIds,
                                             pRegParams->regOpCount,
                                             NV_FALSE);
}

