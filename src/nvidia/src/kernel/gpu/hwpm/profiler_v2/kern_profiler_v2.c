/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/hwpm/kern_hwpm.h"
#include "vgpu/rpc.h"

static NV_INLINE NvBool
_isNonAdminProfilingPermitted(OBJGPU *pGpu)
{
    // Any non-priv clients with RS_ACCESS_PERFMON capability are allowed
    if (osCheckAccess(RS_ACCESS_PERFMON))
    {
        return NV_TRUE;
    }

    // Otherwise, allowed only if profiling is deprivileged for all users
    if (!gpuIsRmProfilingPrivileged(pGpu))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NV_INLINE NvBool
_isProfilingPermitted(OBJGPU *pGpu, ProfilerBase *pProf, API_SECURITY_INFO *pSecInfo)
{
    // Admins are always allowed to access device profiling
    if (pSecInfo->privLevel >= RS_PRIV_LEVEL_USER_ROOT)
    {
        return NV_TRUE;
    }

    // Non-priv clients may device profile only in special cases
    if (_isNonAdminProfilingPermitted(pGpu))
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

NV_STATUS
profilerBaseConstructState_IMPL
(
    ProfilerBase *pProf,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsClient *pRsClient = pCallContext->pClient;

    pProf->profilerId = NV_REQUESTER_CLIENT_OBJECT(pRsClient->hClient, pCallContext->pResourceRef->hResource);
    pProf->bMmaBoostDisabled = NV_FALSE;

    return NV_OK;
}

void
profilerBaseDestruct_IMPL
(
    ProfilerBase *pProf
)
{
    if (pProf->ppBytesAvailable !=  NULL)
    {
        portMemFree(pProf->ppBytesAvailable);
        portMemFree(pProf->ppStreamBuffers);
        pProf->ppStreamBuffers = NULL;
        pProf->ppBytesAvailable = NULL;
    }

    profilerBaseDestructState_HAL(pProf);
}

static NV_STATUS
_profilerPollForUpdatedMembytes(ProfilerBase *pProfBase, OBJGPU *pGpu, KernelHwpm *pKernelHwpm, NvU32 pmaChIdx)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout = {0};
    volatile NvU32 *pMemBytesAddr = NvP64_VALUE(pProfBase->pPmaStreamList[pmaChIdx].pNumBytesCpuAddr);

    if (pMemBytesAddr == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid MEM_BYTES_ADDR.\n");
        return NV_ERR_INVALID_STATE;
    }

    threadStateResetTimeout(pGpu);
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    while (*pMemBytesAddr == NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE)
    {
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "timeout occurred while waiting for PM streamout to idle.\n");
            break;
        }
        osSpinLoop();
        status = gpuCheckTimeout(pGpu, &timeout);
    }

    NV_PRINTF(LEVEL_INFO, "status=0x%08x, *MEM_BYTES_ADDR=0x%08x.\n", status,
              *pMemBytesAddr);

    return status;
}

/*
 * This function does the following:
 * 1. Initialize membytes buffer on guest, ensuring no membytes streamout is in progress.
 * 2. Issue RPC to vGPU host to idle PMA channel and trigger membytes streaming.
 * 3. If required, wait on guest until updated membytes value is received
 */
NV_STATUS profilerBaseQuiesceStreamout_IMPL(ProfilerBase *pProf, OBJGPU *pGpu, KernelHwpm *pKernelHwpm, NvU32 pmaChIdx)
{
    NV_STATUS rmStatus = NV_OK;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS pmaIdleParams = {0};

    if (pProf->pPmaStreamList == NULL)
        return NV_ERR_INVALID_STATE;

    volatile NvU32 *pMemBytesAddr = NvP64_VALUE(pProf->pPmaStreamList[pmaChIdx].pNumBytesCpuAddr);

    if (pMemBytesAddr == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid MEM_BYTES_ADDR.\n");
        return NV_ERR_INVALID_STATE;
    }

    // Check if any membytes streamout is in progress
    if (*pMemBytesAddr == NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE)
    {
        // Complete any pending membytes streamout
        rmStatus = _profilerPollForUpdatedMembytes(pProf, pGpu, pKernelHwpm, pmaChIdx);
    }

    *pMemBytesAddr = NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE;

    pmaIdleParams.pmaChannelIdx = pmaChIdx;

    // Issue RPC to quiesce PMA channel
    NV_RM_RPC_CONTROL(pGpu,
                      pCallContext->pClient->hClient,
                      pCallContext->pResourceRef->hResource,
                      NVB0CC_CTRL_CMD_INTERNAL_QUIESCE_PMA_CHANNEL,
                      &pmaIdleParams, sizeof(NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS),
                      rmStatus);

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Waiting for PMA to be idle failed with error 0x%x\n",
                  rmStatus);
        return rmStatus;
    }

    // If membytes streaming was triggered, wait on guest for it to complete
    if (pmaIdleParams.bMembytesPollingRequired)
    {
        rmStatus = _profilerPollForUpdatedMembytes(pProf, pGpu, pKernelHwpm, pmaChIdx);
    }

    return rmStatus;
}

void
profilerBaseDestructState_VF
(
    ProfilerBase *pProf
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProf);
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);
    NvU32 pmaChIdx;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NV_STATUS rmStatus = NV_OK;

    if (pProf->pPmaStreamList == NULL)
        return;

    // Handle quiesce streamout on guest, then issue RPC to free Profiler
    // object on host, which will handle rest of the teardown
    for (pmaChIdx = 0; pmaChIdx < pKernelHwpm->maxPmaChannels; pmaChIdx++)
    {
        if (!pProf->pPmaStreamList[pmaChIdx].bValid)
        {
            continue;
        }

        rmStatus = profilerBaseQuiesceStreamout(pProf, pGpu, pKernelHwpm, pmaChIdx);
    }

    // Issue RPC to vGPU host to free Profiler object allocated on host
    NV_RM_RPC_FREE(pGpu,
                   pCallContext->pClient->hClient,
                   pCallContext->pResourceRef->pParentRef->hResource,
                   pCallContext->pResourceRef->hResource,
                   rmStatus);

    // Free membytes CPU mapping on guest
    for (pmaChIdx = 0; pmaChIdx < pKernelHwpm->maxPmaChannels; pmaChIdx++)
    {
        if (!pProf->pPmaStreamList[pmaChIdx].bValid)
        {
            continue;
        }

        if (IS_SRIOV_FULL_GUEST(pGpu))
        {
            khwpmStreamoutFreePmaStream(pGpu, pKernelHwpm, pProf->profilerId,
                                        &pProf->pPmaStreamList[pmaChIdx], pmaChIdx);
            continue;
        }

        if (pProf->pPmaStreamList[pmaChIdx].pNumBytesCpuAddr != NvP64_NULL )
        {
            memdescUnmap(pProf->pPmaStreamList[pmaChIdx].pNumBytesBufDesc, NV_TRUE, osGetCurrentProcess(),
                         pProf->pPmaStreamList[pmaChIdx].pNumBytesCpuAddr,
                         pProf->pPmaStreamList[pmaChIdx].pNumBytesCpuAddrPriv);
        }

        if (pProf->pPmaStreamList[pmaChIdx].pNumBytesBufDesc != NULL )
        {
            memdescFree(pProf->pPmaStreamList[pmaChIdx].pNumBytesBufDesc);
            memdescDestroy(pProf->pPmaStreamList[pmaChIdx].pNumBytesBufDesc);
        }

        pProf->pPmaStreamList[pmaChIdx].bValid = NV_FALSE;
    }

    portMemFree(pProf->pPmaStreamList);
    portMemFree(pProf->pBindPointAllocated);
}

NV_STATUS
profilerDevConstruct_IMPL
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS                      *pSys             = SYS_GET_INSTANCE();
    ProfilerBase                *pProfBase        = staticCast(pProfDev, ProfilerBase);
    PROFILER_CLIENT_PERMISSIONS clientPermissions = {0};
    NVB2CC_ALLOC_PARAMETERS *pAllocParams = pParams->pAllocParams;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE) &&
        ((pParams->pSecInfo)->privLevel < RS_PRIV_LEVEL_USER_ROOT) && 
        (pAllocParams->hClientTarget != 0))
    {
        NV_STATUS rmStatus;

        rmStatus = osValidateClientTokens((void*)rmclientGetSecurityTokenByHandle(pCallContext->pClient->hClient),
                                          (void*)rmclientGetSecurityTokenByHandle(pAllocParams->hClientTarget));
        NV_CHECK_OR_RETURN(LEVEL_NOTICE, rmStatus == NV_OK, rmStatus);
    }

    if (!profilerBaseQueryCapabilities_HAL(pProfBase, pCallContext, pParams,
                                            &clientPermissions))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return profilerDevConstructState_HAL(pProfDev, pCallContext, pParams, clientPermissions);
}

NvBool
profilerBaseQueryCapabilities_IMPL
(
    ProfilerBase *pProfBase,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    PROFILER_CLIENT_PERMISSIONS *pClientPermissions
)
{
    OBJGPU              *pGpu                   = GPU_RES_GET_GPU(pProfBase);
    API_SECURITY_INFO   *pSecInfo               = pParams->pSecInfo;
    NvBool               bAnyProfilingPermitted = NV_FALSE;
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);

    // SYS memory profiling is permitted if video memory profiling is permitted.
    pClientPermissions->bVideoMemoryProfilingPermitted = _isMemoryProfilingPermitted(pGpu, pProfBase);
    pClientPermissions->bSysMemoryProfilingPermitted = pClientPermissions->bVideoMemoryProfilingPermitted;

    //
    // bAdminProfilingPermitted controls access to privileged profiling registers.
    // This is admin-only; no exceptions, not even CAP_PERFMON
    //
    pClientPermissions->bAdminProfilingPermitted = NV_FALSE;
    if (pSecInfo->privLevel >= RS_PRIV_LEVEL_USER_ROOT)
    {
        bAnyProfilingPermitted = NV_TRUE;
        pClientPermissions->bAdminProfilingPermitted = NV_TRUE;
    }

    pClientPermissions->bDevProfilingPermitted = _isProfilingPermitted(pGpu, pProfBase, pSecInfo);
    pClientPermissions->bCtxProfilingPermitted = _isProfilingPermitted(pGpu, pProfBase, pSecInfo);

    if (pClientPermissions->bDevProfilingPermitted || pClientPermissions->bCtxProfilingPermitted)
    {
        bAnyProfilingPermitted = NV_TRUE;
    }

    if (pKernelHwpm->getProperty(pKernelHwpm, PDB_PROP_KHWPM_HES_CWD_SUPPORTED) && !IS_VIRTUAL(pGpu))
    {
        bAnyProfilingPermitted = NV_TRUE;
    }

    return bAnyProfilingPermitted;
}

/*
 * To be called on vGPU guest only
 * Profiler object will not be fully initialized on vGPU guest,
 * this request will be passed on to vGPU host.
 * Initialize pPmaStreamList on guest to store details PMA stream
 */
static NV_STATUS
_profilerDevConstructVgpuGuest
(
    ProfilerBase *pProfBase,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfBase);
    HWPM_PMA_STREAM *pPmaStreamList = NULL;
    NvBool *pBindPointAllocated = NULL;

    // Allocate the pPmaStreamList to store info about memaddr buffer CPU mapping
    pPmaStreamList = portMemAllocNonPaged(sizeof(HWPM_PMA_STREAM) * GPU_GET_KERNEL_HWPM(pGpu)->maxPmaChannels);
    if (pPmaStreamList == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pPmaStreamList, 0, sizeof(HWPM_PMA_STREAM) * GPU_GET_KERNEL_HWPM(pGpu)->maxPmaChannels);

    pBindPointAllocated = portMemAllocNonPaged(sizeof(NvBool) * GPU_GET_KERNEL_HWPM(pGpu)->maxPmaChannels);
    if (pBindPointAllocated == NULL)
    {
        portMemFree(pPmaStreamList);
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pBindPointAllocated, NV_FALSE, sizeof(NvBool) * GPU_GET_KERNEL_HWPM(pGpu)->maxPmaChannels);

    pProfBase->pPmaStreamList = pPmaStreamList;
    pProfBase->pBindPointAllocated = pBindPointAllocated;

    return NV_OK;
}

NV_STATUS
profilerDevConstructState_VF
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    PROFILER_CLIENT_PERMISSIONS clientPermissions
)
{
    OBJGPU          *pGpu       = GPU_RES_GET_GPU(pProfDev);
    ProfilerBase    *pProfBase  = staticCast(pProfDev, ProfilerBase);
    NV_STATUS        rmStatus   = NV_OK;

    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         _profilerDevConstructVgpuGuest(pProfBase, pParams),
                         profilerDevConstruct_VF_exit);

    // Issue RPC to allocate Profiler object on vGPU host as well
    NV_RM_RPC_ALLOC_OBJECT(pGpu,
                           pCallContext->pClient->hClient,
                           pCallContext->pResourceRef->pParentRef->hResource,
                           pCallContext->pResourceRef->hResource,
                           MAXWELL_PROFILER_DEVICE,
                           pParams->pAllocParams,
                           pParams->paramsSize,
                           rmStatus);

profilerDevConstruct_VF_exit:
    return rmStatus;
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
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pProfDev);
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
    OBJGPU         *pGpu        = GPU_RES_GET_GPU(pProfDev);
    RM_API         *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle        hClient     = RES_GET_CLIENT_HANDLE(pProfDev);
    NvHandle        hObject     = RES_GET_HANDLE(pProfDev);

    NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS params = {0};

    params.bDevProfilingPermitted = clientPermissions.bDevProfilingPermitted;
    params.bAdminProfilingPermitted = clientPermissions.bAdminProfilingPermitted;
    params.bVideoMemoryProfilingPermitted = clientPermissions.bVideoMemoryProfilingPermitted;
    params.bSysMemoryProfilingPermitted = clientPermissions.bSysMemoryProfilingPermitted;

    return pRmApi->Control(pRmApi,
                           hClient,
                           hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_PERMISSIONS_INIT,
                           &params, sizeof(params));
}

NV_STATUS
profilerCtxConstructStateInterlude_IMPL
(
    ProfilerCtx *pProfCtx,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams,
    PROFILER_CLIENT_PERMISSIONS clientPermissions
)
{
    OBJGPU         *pGpu        = GPU_RES_GET_GPU(pProfCtx);
    RM_API         *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle        hClient     = RES_GET_CLIENT_HANDLE(pProfCtx);
    NvHandle        hObject     = RES_GET_HANDLE(pProfCtx);

    NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS params = {0};

    params.bCtxProfilingPermitted = clientPermissions.bCtxProfilingPermitted;
    params.bAdminProfilingPermitted = clientPermissions.bAdminProfilingPermitted;
    params.bVideoMemoryProfilingPermitted = clientPermissions.bVideoMemoryProfilingPermitted;
    params.bSysMemoryProfilingPermitted = clientPermissions.bSysMemoryProfilingPermitted;

    return pRmApi->Control(pRmApi,
                           hClient,
                           hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_PERMISSIONS_INIT,
                           &params, sizeof(params));
}

NV_STATUS
profilerDevConstructStateEpilogue_FWCLIENT
(
    ProfilerDev *pProfDev,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ProfilerBase *pProfBase = staticCast(pProfDev, ProfilerBase);
    RsResourceRef *pParentRef = pCallContext->pResourceRef->pParentRef;

    NV_ASSERT_OR_RETURN((pParentRef->internalClassId == classId(Subdevice)), NV_ERR_INVALID_OBJECT_PARENT);

    pProfBase->hSubDevice = pParentRef->hResource;

    return NV_OK;
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
    NvHandle                     hClient;
    NvHandle                     hParent;
    NvHandle                     hObject;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    CALL_CONTEXT                *pCallContext;
    OBJGPU                      *pGpu            = GPU_RES_GET_GPU(pProfDev);
    NV_STATUS                    status          = NV_OK;

    resGetFreeParams(staticCast(pProfDev, RsResource), &pCallContext, &pParams);
    hClient = pCallContext->pClient->hClient;
    hParent = pCallContext->pResourceRef->pParentRef->hResource;
    hObject = pCallContext->pResourceRef->hResource;

    NV_RM_RPC_FREE(pGpu, hClient, hParent, hObject, status);
}

NV_STATUS
profilerCtxConstructState_IMPL
(
    ProfilerCtx *pProfCtx,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams,
    PROFILER_CLIENT_PERMISSIONS clientPermissions
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerCtxConstructStatePrologue_HAL(pProfCtx,
                            pCallContext, pAllocParams));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerCtxConstructStateInterlude_HAL(pProfCtx,
                            pCallContext, pAllocParams, clientPermissions));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerCtxConstructStateEpilogue_HAL(pProfCtx,
                            pCallContext, pAllocParams));
    return NV_OK;
}

NV_STATUS
profilerCtxConstruct_IMPL
(
    ProfilerCtx *pProfCtx,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ProfilerBase      *pProfBase  = staticCast(pProfCtx, ProfilerBase);
    PROFILER_CLIENT_PERMISSIONS clientPermissions = {0};

    if (!profilerBaseQueryCapabilities_HAL(pProfBase, pCallContext, pParams,
                                            &clientPermissions))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return profilerCtxConstructState(pProfCtx, pCallContext, pParams, clientPermissions);
}

NV_STATUS
profilerCtxConstructStatePrologue_FWCLIENT
(
    ProfilerCtx *pProfCtx,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pProfCtx);
    NvHandle    hClient     = RES_GET_CLIENT_HANDLE(pProfCtx);
    NvHandle    hParent     = RES_GET_PARENT_HANDLE(pProfCtx);
    NvHandle    hObject     = RES_GET_HANDLE(pProfCtx);
    NvU32       class       = RES_GET_EXT_CLASS_ID(pProfCtx);
    NV_STATUS   status      = NV_OK;

    NV_RM_RPC_ALLOC_OBJECT(pGpu, hClient, hParent, hObject, class,
        pParams->pAllocParams, pParams->paramsSize, status);

    return status;
}

void
profilerCtxDestruct_FWCLIENT
(
    ProfilerCtx *pProfCtx
)
{
    NvHandle                     hClient;
    NvHandle                     hParent;
    NvHandle                     hObject;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    CALL_CONTEXT                *pCallContext;
    OBJGPU                      *pGpu            = GPU_RES_GET_GPU(pProfCtx);
    NV_STATUS                    status          = NV_OK;

    resGetFreeParams(staticCast(pProfCtx, RsResource), &pCallContext, &pParams);
    hClient = pCallContext->pClient->hClient;
    hParent = pCallContext->pResourceRef->pParentRef->hResource;
    hObject = pCallContext->pResourceRef->hResource;

    NV_RM_RPC_FREE(pGpu, hClient, hParent, hObject, status);
}