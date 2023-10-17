/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief This file contains the functions managing the FmSession
 */

#include "core/core.h"
#include "os/os.h"
#include "compute/fm_session_api.h"
#include "class/cl000f.h"
#include "resserv/rs_client.h"
#include "core/system.h"
#include "core/locks.h"
#include "compute/fabric.h"
#include "Nvcm.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "rmapi/client.h"

static void
_clearOutstandingComputeChannels(void)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuMask = 0;
    NvU32 gpuCount = 0;
    NvU32 gpuInstance = 0;
    RM_API *pRmApi;

    NV_ASSERT(rmGpuLockIsOwner());

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        if (pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_RECOVER_ALL_COMPUTE_CONTEXTS,
                            NULL,
                            0) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to recover all compute channels for GPU %d\n",
                      pGpu->gpuInstance);
        }
    }
}

static void
_clearFmState(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);
    NvU32 flags = fabricGetFmSessionFlags(pFabric);

    if (!pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric manager state is already cleared.\n");
        return;
    }

    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED, NV_FALSE);

    NV_PRINTF(LEVEL_INFO, "Fabric manager state is cleared.\n");

    if (FLD_TEST_REF(NV000F_FLAGS_CHANNEL_RECOVERY, _ENABLED, flags))
    {
        _clearOutstandingComputeChannels();
    }
}

NV_STATUS
fmsessionapiConstruct_IMPL
(
    FmSessionApi                 *pFmSessionApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);
    NvHandle hClient = pCallContext->pClient->hClient;
    NV000F_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    osRmCapInitDescriptor(&pFmSessionApi->dupedCapDescriptor);

    if ((pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL)
        && !RMCFG_FEATURE_PLATFORM_MODS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "only supported for usermode clients\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED))
    {
        NV_PRINTF(LEVEL_ERROR, "duplicate object creation\n");
        return NV_ERR_STATE_IN_USE;
    }

    status = osRmCapAcquire(NULL, NV_RM_CAP_EXT_FABRIC_MGMT,
                            pAllocParams->capDescriptor,
                            &pFmSessionApi->dupedCapDescriptor);

    //
    // On platforms where capability isn't implemented,
    // enforce the admin-only check.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        if (rmclientIsAdminByHandle(hClient, pCallContext->secInfo.privLevel))
        {
            status = NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "insufficient permissions\n");
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }
    else if (status != NV_OK)
    {
         NV_PRINTF(LEVEL_ERROR, "Capability validation failed\n");
         return status;
    }

    if (pFabric != NULL)
    {
        fabricSetFmSessionFlags(pFabric, pAllocParams->flags);
    }

    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED, NV_TRUE);

    return NV_OK;
}

void
fmsessionapiDestruct_IMPL
(
    FmSessionApi *pFmSessionApi
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    NV_PRINTF(LEVEL_INFO, "Fabric manager is shutting down.\n");

    _clearFmState();

    osRmCapRelease(pFmSessionApi->dupedCapDescriptor);
    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED, NV_FALSE);
}

NV_STATUS
fmsessionapiCtrlCmdSetFmState_IMPL
(
    FmSessionApi *pFmSessionApi
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric manager state is already set.\n");
        return NV_OK;
    }

    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED, NV_TRUE);

    NV_PRINTF(LEVEL_INFO, "Fabric manager state is set.\n");

    return NV_OK;
}

NV_STATUS
fmsessionapiCtrlCmdClearFmState_IMPL
(
    FmSessionApi *pFmSessionApi
)
{
    _clearFmState();

    return NV_OK;
}
