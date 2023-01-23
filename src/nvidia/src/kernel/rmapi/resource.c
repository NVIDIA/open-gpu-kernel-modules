/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "rmapi/client.h"
#include "rmapi/resource.h"
#include "rmapi/rmapi.h"
#include "rmapi/control.h"
#include "ctrl/ctrlxxxx.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu.h"
#include "vgpu/rpc.h"
#include "core/locks.h"

NV_STATUS
rmrescmnConstruct_IMPL
(
    RmResourceCommon *pResourceCommmon
)
{
    return NV_OK;
}

NV_STATUS
rmresConstruct_IMPL
(
    RmResource *pResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    if (RS_IS_COPY_CTOR(pParams))
    {
        RmResource *pSrcResource = dynamicCast(pParams->pSrcRef->pResource, RmResource);

        pResource->rpcGpuInstance = pSrcResource->rpcGpuInstance;
        pResource->bRpcFree = pSrcResource->bRpcFree;
    }
    else
    {
        pResource->rpcGpuInstance = ~0;
        pResource->bRpcFree = NV_FALSE;
    }

    return NV_OK;
}

NvBool
rmresAccessCallback_IMPL
(
    RmResource *pResource,
    RsClient *pInvokingClient,
    void *pAllocParams,
    RsAccessRight accessRight
)
{
    NV_STATUS status;
    RsResourceRef *pCliResRef;

    status = clientGetResourceRef(RES_GET_CLIENT(pResource),
                                  RES_GET_CLIENT_HANDLE(pResource),
                                  &pCliResRef);

    if (status == NV_OK)
    {
        // Allow access if the resource's owner would get the access right
        if(resAccessCallback(pCliResRef->pResource, pInvokingClient, pAllocParams, accessRight))
            return NV_TRUE;
    }

    // Delegate to superclass
    return resAccessCallback_IMPL(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

NvBool
rmresShareCallback_IMPL
(
    RmResource *pResource,
    RsClient *pInvokingClient,
    RsResourceRef *pParentRef,
    RS_SHARE_POLICY *pSharePolicy
)
{
    NV_STATUS status;
    RsResourceRef *pCliResRef;

    //
    // cliresShareCallback contains some require exceptions for non-GpuResource,
    // which we don't want to hit. ClientResource doesn't normally implement these
    // share types anyway, so we're fine with skipping them.
    //
    switch (pSharePolicy->type)
    {
        case RS_SHARE_TYPE_SMC_PARTITION:
        case RS_SHARE_TYPE_GPU:
        {
            //
            // We do not want to lock down these GpuResource-specific require policies
            // when the check cannot be applied for other resources, so add these checks
            // as an alternative bypass for those policies
            //
            if ((pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE) &&
                (NULL == dynamicCast(pResource, GpuResource)))
            {
                return NV_TRUE;
            }
            break;
        }
        case RS_SHARE_TYPE_FM_CLIENT:
        {
            RmClient *pSrcClient = dynamicCast(RES_GET_CLIENT(pResource), RmClient);
            NvBool bSrcIsKernel = (pSrcClient != NULL) && (rmclientGetCachedPrivilege(pSrcClient) >= RS_PRIV_LEVEL_KERNEL);

            if (rmclientIsCapable(dynamicCast(pInvokingClient, RmClient),
                                  NV_RM_CAP_EXT_FABRIC_MGMT) && !bSrcIsKernel)
            {
                return NV_TRUE;
            }
            break;
        }
        default:
        {
            status = clientGetResourceRef(RES_GET_CLIENT(pResource),
                                          RES_GET_CLIENT_HANDLE(pResource),
                                          &pCliResRef);
            if (status == NV_OK)
            {
                // Allow sharing if the resource's owner would be shared with
                if (resShareCallback(pCliResRef->pResource, pInvokingClient,
                                     pParentRef, pSharePolicy))
                    return NV_TRUE;
            }
            break;
        }
    }

    // Delegate to superclass
    return resShareCallback_IMPL(staticCast(pResource, RsResource),
                                 pInvokingClient, pParentRef, pSharePolicy);
}

void serverControl_InitCookie
(
    const struct NVOC_EXPORTED_METHOD_DEF   *exportedEntry,
    RmCtrlExecuteCookie                     *pRmCtrlExecuteCookie
)
{
    // Copy from NVOC exportedEntry
    pRmCtrlExecuteCookie->cmd       = exportedEntry->methodId;
    pRmCtrlExecuteCookie->ctrlFlags = exportedEntry->flags;
    // One time initialization of a const variable
    *(NvU32 *)&pRmCtrlExecuteCookie->rightsRequired.limbs[0]
                                    = exportedEntry->accessRight;
}

//
// This routine searches through the Resource's NVOC exported methods for an entry
// that matches the specified command.
//
// Same logic as rmControlCmdLookup() in legacy RMCTRL path
//
NV_STATUS rmresControlLookup_IMPL
(
    RmResource                     *pResource,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams,
    const struct NVOC_EXPORTED_METHOD_DEF **ppEntry
)
{
    NvU32 cmd = pRsParams->cmd;

    if (RMCTRL_IS_NULL_CMD(cmd))
        return NV_WARN_NOTHING_TO_DO;

    return resControlLookup_IMPL(staticCast(pResource, RsResource), pRsParams, ppEntry);
}

NV_STATUS
rmresGetMemInterMapParams_IMPL
(
    RmResource                 *pRmResource,
    RMRES_MEM_INTER_MAP_PARAMS *pParams
)
{
    return NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
rmresCheckMemInterUnmap_IMPL
(
    RmResource *pRmResource,
    NvBool      bSubdeviceHandleProvided
)
{
    return NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
rmresGetMemoryMappingDescriptor_IMPL
(
    RmResource *pRmResource,
    struct MEMORY_DESCRIPTOR **ppMemDesc
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
rmresControl_Prologue_IMPL
(
    RmResource *pResource, 
    CALL_CONTEXT *pCallContext, 
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = gpumgrGetGpu(pResource->rpcGpuInstance);

    if (pGpu == NULL)
        return NV_OK;

    if ((IS_VIRTUAL(pGpu)    && (pParams->pCookie->ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_VGPU_HOST)) ||
        (IS_GSP_CLIENT(pGpu) && (pParams->pCookie->ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL)))
    {
        //
        // GPU lock is required to protect the RPC buffers. 
        // However, some controls have  ROUTE_TO_PHYSICAL + NO_GPUS_LOCK flags set.
        // This is not valid in offload mode, but is in monolithic.
        // In those cases, just acquire the lock for the RPC
        //
        GPU_MASK gpuMaskRelease = 0;
        if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        {
            //
            // Log any case where the above assumption is not true, but continue
            // anyway. Use SAFE_LOCK_UPGRADE to try and recover in these cases.
            //
            NV_ASSERT(pParams->pCookie->ctrlFlags & RMCTRL_FLAGS_NO_GPUS_LOCK);
            NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                   GPU_LOCK_GRP_SUBDEVICE,
                                   GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                                   RM_LOCK_MODULES_RPC,
                                   &gpuMaskRelease));
        }

        NV_RM_RPC_CONTROL(pGpu, pParams->hClient, pParams->hObject, pParams->cmd,
                          pParams->pParams, pParams->paramsSize, status);

        if (gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }

        return (status == NV_OK) ? NV_WARN_NOTHING_TO_DO : status;
    }
    return NV_OK;
}

void
rmresControl_Epilogue_IMPL
(
    RmResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
}
