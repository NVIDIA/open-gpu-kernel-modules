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

#include "gpu/hwpm/profiler_v2.h"
#include "vgpu/rpc.h"

static NV_INLINE NvBool
_isDeviceProfilingPermitted(OBJGPU *pGpu, ProfilerBase *pProf, API_SECURITY_INFO *pSecInfo)
{
    if (pSecInfo->privLevel >= RS_PRIV_LEVEL_USER_ROOT)
    {
        return NV_TRUE;
    }

    if (!gpuIsRmProfilingPrivileged(pGpu))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NvBool
_isMemoryProfilingPermitted(OBJGPU *pGpu, ProfilerBase *pProf)
{
    NvBool bSmcGpuPartitioningEnabled = IS_MIG_IN_USE(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pProf);

    if (bSmcGpuPartitioningEnabled && !kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        MIG_INSTANCE_REF ref;

        if (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref) != NV_OK)
            return NV_FALSE;

        if (!kmigmgrIsMIGReferenceValid(&ref))
            return NV_FALSE;

        NV_ASSERT_OR_RETURN((ref.pKernelMIGGpuInstance != NULL) && (ref.pMIGComputeInstance != NULL), NV_FALSE);
        return (ref.pKernelMIGGpuInstance->resourceAllocation.gpcCount ==
                ref.pMIGComputeInstance->resourceAllocation.gpcCount);

    }

    return NV_TRUE;
}

NV_STATUS
profilerBaseConstruct_IMPL
(
    ProfilerBase *pProf,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return profilerBaseConstructState_HAL(pProf, pCallContext, pParams);
}

void
profilerBaseDestruct_IMPL
(
    ProfilerBase *pProf
)
{
    profilerBaseDestructState_HAL(pProf);
}

NV_STATUS
profilerDevConstruct_IMPL
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    PROFILER_CLIENT_PERMISSIONS clientPermissions = {0};

    if (!profilerDevQueryCapabilities_HAL(pProfDev, pCallContext, pParams,
                                            &clientPermissions))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return profilerDevConstructState_HAL(pProfDev, pCallContext, pParams, clientPermissions);
}

NvBool
profilerDevQueryCapabilities_IMPL
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    PROFILER_CLIENT_PERMISSIONS *pClientPermissions
)
{
    OBJGPU              *pGpu                   = GPU_RES_GET_GPU(pProfDev);
    ProfilerBase        *pProfBase              = staticCast(pProfDev, ProfilerBase);
    API_SECURITY_INFO   *pSecInfo               = pParams->pSecInfo;
    NvBool              bAnyProfilingPermitted  = NV_FALSE;

    pClientPermissions->bMemoryProfilingPermitted =
        _isMemoryProfilingPermitted(pGpu, pProfBase);

    pClientPermissions->bAdminProfilingPermitted = NV_FALSE;
    if (pSecInfo->privLevel >= RS_PRIV_LEVEL_USER_ROOT)
    {
        bAnyProfilingPermitted = NV_TRUE;
        pClientPermissions->bAdminProfilingPermitted = NV_TRUE;
    }

    pClientPermissions->bDevProfilingPermitted =
        _isDeviceProfilingPermitted(pGpu, pProfBase, pSecInfo);

    if (pClientPermissions->bDevProfilingPermitted)
    {
        bAnyProfilingPermitted = NV_TRUE;
    }

    return bAnyProfilingPermitted;
}

NV_STATUS
profilerDevConstructState_IMPL
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams,
    PROFILER_CLIENT_PERMISSIONS clientPermissions
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerDevConstructStatePrologue_HAL(pProfDev,
                            pCallContext, pAllocParams));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerDevConstructStateInterlude_HAL(pProfDev,
                            pCallContext, pAllocParams, clientPermissions));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerDevConstructStateEpilogue_HAL(pProfDev,
                            pCallContext, pAllocParams));

    return NV_OK;
}

NV_STATUS
profilerDevConstructStatePrologue_FWCLIENT
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams
)
{
    OBJGPU      *pGpu       = GPU_RES_GET_GPU(pProfDev);
    NvHandle    hClient     = RES_GET_CLIENT_HANDLE(pProfDev);
    NvHandle    hParent     = RES_GET_PARENT_HANDLE(pProfDev);
    NvHandle    hObject     = RES_GET_HANDLE(pProfDev);
    NvU32       class       = RES_GET_EXT_CLASS_ID(pProfDev);
    NV_STATUS   status      = NV_OK;

    NV_RM_RPC_ALLOC_OBJECT(pGpu, hClient, hParent, hObject, class,
        pAllocParams->pAllocParams, pAllocParams->paramsSize, status);

    return status;
}

NV_STATUS
profilerDevConstructStateInterlude_IMPL
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams,
    PROFILER_CLIENT_PERMISSIONS clientPermissions
)
{
    OBJGPU          *pGpu       = GPU_RES_GET_GPU(pProfDev);
    RM_API          *pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle        hClient     = RES_GET_CLIENT_HANDLE(pProfDev);
    NvHandle        hObject     = RES_GET_HANDLE(pProfDev);

    NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS params = {0};

    params.bDevProfilingPermitted = clientPermissions.bDevProfilingPermitted;
    params.bAdminProfilingPermitted = clientPermissions.bAdminProfilingPermitted;
    params.bMemoryProfilingPermitted = clientPermissions.bMemoryProfilingPermitted;

    return pRmApi->Control(pRmApi,
                           hClient,
                           hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_PERMISSIONS_INIT,
                           &params, sizeof(params));
}

void
profilerDevDestruct_IMPL
(
    ProfilerDev *pProfDev
)
{
    profilerDevDestructState_HAL(pProfDev);
}

void
profilerDevDestructState_FWCLIENT
(
    ProfilerDev *pProfDev
)
{
    NvHandle                    hClient;
    NvHandle                    hParent;
    NvHandle                    hObject;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    CALL_CONTEXT                *pCallContext;
    OBJGPU                      *pGpu           = GPU_RES_GET_GPU(pProfDev);
    NV_STATUS                   status          = NV_OK;

    resGetFreeParams(staticCast(pProfDev, RsResource), &pCallContext, &pParams);
    hClient = pCallContext->pClient->hClient;
    hParent = pCallContext->pResourceRef->pParentRef->hResource;
    hObject = pCallContext->pResourceRef->hResource;

    NV_RM_RPC_FREE(pGpu, hClient, hParent, hObject, status);
}
