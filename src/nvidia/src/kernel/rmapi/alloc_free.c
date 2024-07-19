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

#include "rmapi/rmapi.h"
#include "rmapi/rmapi_specific.h"
#include "rmapi/client.h"
#include "entry_points.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "vgpu/rpc.h"
#include "resource_desc.h"
#include "gpu/disp/disp_objs.h"
#include "gpu/disp/disp_channel.h"
#include "nvsecurityinfo.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "gpu/device/device.h"
#include "class/cl0080.h"

#include "class/cl83de.h" // GT200_DEBUGGER
#include "gpu/gr/kernel_sm_debugger_session.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "gpu/timer/tmr.h"

//
// RM Alloc & Free internal flags -- code should be migrated to use rsresdesc
// and rmapi types directly where possible.
//
#define RM_ALLOC_STATES_NONE                    0
#define RM_ALLOC_STATES_INTERNAL_CLIENT_HANDLE  ALLOC_STATE_INTERNAL_CLIENT_HANDLE // NVBIT(5)
#define RM_ALLOC_STATES_SKIP_RPC                NVBIT(6)
#define RM_ALLOC_STATES_INTERNAL_ALLOC          NVBIT(7)

#define RM_FREE_STATES_NONE                     0

static void
rmapiResourceDescToLegacyFlags
(
    const RS_RESOURCE_DESC *pResDesc,
    NvU32 *pAllocFlags,
    NvU32 *pFreeFlags
)
{
    if (pAllocFlags)
    {
        *pAllocFlags = (pResDesc->flags & RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC) ? RM_LOCK_FLAGS_NONE : RM_LOCK_FLAGS_NO_GPUS_LOCK;
        *pAllocFlags |= (pResDesc->flags & RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC) ? RM_LOCK_FLAGS_GPU_GROUP_LOCK : 0;
    }

    if (pFreeFlags)
    {
        *pFreeFlags = (pResDesc->flags & RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE) ? RM_LOCK_FLAGS_NONE : RM_LOCK_FLAGS_NO_GPUS_LOCK;
        *pFreeFlags |= (pResDesc->flags & RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE) ? RM_LOCK_FLAGS_GPU_GROUP_LOCK : 0;
    }
}

NvU32
serverAllocClientHandleBase
(
    RsServer          *pServer,
    NvBool             bInternalHandle,
    API_SECURITY_INFO *pSecInfo
)
{
    NvU32 handleBase;
    NvU32 gfid = (NvU32)((NvU64)pSecInfo->pProcessToken);

    if (bInternalHandle)
    {
        handleBase = pServer->internalHandleBase;
    }
    else
    {
        handleBase = pServer->clientHandleBase;

        if (RMCFG_FEATURE_PLATFORM_GSP && IS_GFID_VF(gfid))
            handleBase = RS_CLIENT_GET_VF_HANDLE_BASE(gfid);
    }

    return handleBase;
}

NV_STATUS
serverAllocApiCopyIn
(
    RsServer                     *pServer,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams,
    API_STATE                   **ppParamCopy
)
{
    NV_STATUS          status;
    API_SECURITY_INFO *pSecInfo = pRmAllocParams->pSecInfo;
    NvBool             bCopyInParams  = pSecInfo->paramLocation == PARAM_LOCATION_USER;
    RMAPI_PARAM_COPY  *pParamCopy = NULL;
    NvU32              allocParamsSize = 0;
    void              *pUserParams = pRmAllocParams->pAllocParams;

    pParamCopy = (RMAPI_PARAM_COPY*)PORT_ALLOC(g_resServ.pAllocator, sizeof(*pParamCopy));
    if (pParamCopy == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }
    portMemSet(pParamCopy, 0, sizeof(*pParamCopy));
    pRmAllocParams->pAllocParams = NULL;

    // Setup for access to param
    // Param size is initialized to zero, and then set via rmapiParamsCopyInit
    RMAPI_PARAM_COPY_INIT(*pParamCopy, pRmAllocParams->pAllocParams, NV_PTR_TO_NvP64(pUserParams), allocParamsSize, 1);

    // Look up param size based on hClass
    status = rmapiParamsCopyInit(pParamCopy, pRmAllocParams->externalClassId);
    if (NV_OK != status)
        goto done;

    // Using the per-class info set above, pull in the parameters for this allocation
    if (pParamCopy->paramsSize > 0)
    {
        // gain access to client's parameters via 'pKernelCtrl'
        status = rmapiParamsAcquire(pParamCopy, bCopyInParams);
        if (status != NV_OK)
            goto done;
    }

    // Prevent requesting rights before rights are enabled, just in case old code doesn't zero it properly.
    if (!pServer->bRsAccessEnabled)
        pRmAllocParams->pRightsRequested = NULL;

    if (pRmAllocParams->pRightsRequested != NULL)
    {
        // copyFromUser requires a non-stack buffer, allocate one to copy into
        RS_ACCESS_MASK *pMaskBuffer = (RS_ACCESS_MASK*)PORT_ALLOC(g_resServ.pAllocator, sizeof(RS_ACCESS_MASK));
        if (pMaskBuffer == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto done;
        }

        // Mask is a fixed size, just copy it directly into allocParams
        status = rmapiParamsCopyIn("RightsRequested",
                                   pMaskBuffer,
                                   NV_PTR_TO_NvP64(pRmAllocParams->pRightsRequested),
                                   sizeof(RS_ACCESS_MASK),
                                   bCopyInParams);

        portMemCopy(&pRmAllocParams->rightsRequestedCopy, sizeof(RS_ACCESS_MASK),
                    pMaskBuffer, sizeof(RS_ACCESS_MASK));

        PORT_FREE(g_resServ.pAllocator, pMaskBuffer);

        if (status != NV_OK)
            goto done;

        pRmAllocParams->pRightsRequested = &pRmAllocParams->rightsRequestedCopy;
    }
done:
    if (status != NV_OK)
    {
        if (pParamCopy != NULL)
            PORT_FREE(g_resServ.pAllocator, pParamCopy);
        pParamCopy = NULL;
    }

    if (ppParamCopy != NULL)
        *ppParamCopy = pParamCopy;

    return status;
}

NV_STATUS
serverAllocApiCopyOut
(
    RsServer  *pServer,
    NV_STATUS  status,
    API_STATE *pParamCopy
)
{
    NV_STATUS cpStatus = NV_OK;
    if (pParamCopy->paramsSize > 0)
    {
        // don't copyout if an error
        if (status != NV_OK)
            pParamCopy->flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;

        cpStatus = rmapiParamsRelease(pParamCopy);
        if (status == NV_OK)
            status = cpStatus;
    }

    PORT_FREE(g_resServ.pAllocator, pParamCopy);

    return status;
}

NV_STATUS
serverTopLock_Prologue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status;
    if ((pLockInfo->flags & RM_LOCK_FLAGS_RM_SEMA) &&
        !(pLockInfo->state & RM_LOCK_STATES_RM_SEMA_ACQUIRED))
    {
        if ((status = osAcquireRmSema(pSys->pSema)) != NV_OK)
            return status;
        pLockInfo->state |= RM_LOCK_STATES_RM_SEMA_ACQUIRED;
        *pReleaseFlags |= RM_LOCK_RELEASE_RM_SEMA;
    }

    if (!(pLockInfo->flags & RM_LOCK_FLAGS_NO_API_LOCK))
    {
        if (!(pLockInfo->state & RM_LOCK_STATES_API_LOCK_ACQUIRED))
        {
            NvU32 flags = RMAPI_LOCK_FLAGS_NONE;
            if (access == LOCK_ACCESS_READ)
                flags |= RMAPI_LOCK_FLAGS_READ;

            if (pLockInfo->flags & RS_LOCK_FLAGS_LOW_PRIORITY)
                flags |= RMAPI_LOCK_FLAGS_LOW_PRIORITY;

            if ((status = rmapiLockAcquire(flags, RM_LOCK_MODULES_CLIENT)) != NV_OK)
            {
                return status;
            }
            pLockInfo->state |= RM_LOCK_STATES_API_LOCK_ACQUIRED;
            *pReleaseFlags |= RM_LOCK_RELEASE_API_LOCK;
        }
        else
        {
            if (!rmapiLockIsOwner())
            {
                NV_ASSERT(0);
                return NV_ERR_INVALID_LOCK_STATE;
            }
        }
    }

    return NV_OK;
}

void
serverTopLock_Epilogue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    if (*pReleaseFlags & RM_LOCK_RELEASE_API_LOCK)
    {
        rmapiLockRelease();
        pLockInfo->state &= ~RM_LOCK_STATES_API_LOCK_ACQUIRED;
        *pReleaseFlags &= ~RM_LOCK_RELEASE_API_LOCK;
    }

    if (*pReleaseFlags & RM_LOCK_RELEASE_RM_SEMA)
    {
        osReleaseRmSema(pSys->pSema, NULL);
        pLockInfo->state &= ~RM_LOCK_STATES_RM_SEMA_ACQUIRED;
        *pReleaseFlags &= ~RM_LOCK_RELEASE_RM_SEMA;
    }
}

NV_STATUS
serverResLock_Prologue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    NV_STATUS status = NV_OK;
    OBJGPU   *pParentGpu = NULL;

    if (pLockInfo->state & RM_LOCK_STATES_GPUS_LOCK_ACQUIRED)
    {
        if (rmGpuLockIsOwner())
        {
            return NV_OK;
        }
        else
        {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_LOCK_STATE;
            goto done;
        }
    }

    if (!(pLockInfo->flags & RM_LOCK_FLAGS_NO_GPUS_LOCK))
    {
        if (rmGpuLockIsOwner())
        {
            if (!(pLockInfo->state & RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS))
            {
                NV_ASSERT(0);
                status = NV_ERR_INVALID_LOCK_STATE;
                goto done;
            }
        }
        else
        {
            if ((status = rmGpuLocksAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT)) != NV_OK)
                goto done;

            *pReleaseFlags |= RM_LOCK_RELEASE_GPUS_LOCK;
            pLockInfo->state |= RM_LOCK_STATES_GPUS_LOCK_ACQUIRED;
        }
    }

    if (pLockInfo->flags & RM_LOCK_FLAGS_GPU_GROUP_LOCK)
    {
        RsResourceRef     *pParentRef;
        GpuResource       *pGpuResource;
        NvU32 gpuMask;
        (void)gpuMask;

        pParentRef = pLockInfo->pContextRef;
        if (pParentRef == NULL)
        {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_OBJECT_PARENT;
            goto done;
        }

        //
        // Use the pGpu from parent resource as it will work on alloc & free.
        // Everything below NV0080_DEVICE uses the same pGpu group
        //
        // GPU teardown paths free client resources before tearing down pGpu so
        // pGpu should always be valid at this point.
        //
        pGpuResource = dynamicCast(pParentRef->pResource, GpuResource);

        if (pGpuResource == NULL)
        {
            //
            // If parent is not a GpuResource, we might still be a NV0080_DEVICE
            // so check and handle that case before reporting an error..
            //
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            if (pCallContext != NULL && pCallContext->pResourceRef != NULL)
            {
                pGpuResource = dynamicCast(pCallContext->pResourceRef->pResource, GpuResource);
            }
            if (pGpuResource == NULL)
            {
                NV_ASSERT_FAILED("Attempting to lock per-GPU lock for a non-GpuResource");
                status = NV_ERR_INVALID_OBJECT_PARENT;
                goto done;
            }
        }

        pParentGpu = GPU_RES_GET_GPU(pGpuResource);

        if (pLockInfo->state & RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED)
        {
            if (rmGpuGroupLockIsOwner(pParentGpu->gpuInstance, GPU_LOCK_GRP_DEVICE, &gpuMask))
            {
                goto done;
            }
            else
            {
                NV_ASSERT(0);
                status = NV_ERR_INVALID_LOCK_STATE;
                goto done;
            }
        }

        if (rmGpuGroupLockIsOwner(pParentGpu->gpuInstance, GPU_LOCK_GRP_DEVICE, &gpuMask))
        {
            if (!(pLockInfo->state & RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS))
            {
                NV_ASSERT(0);
                status = NV_ERR_INVALID_LOCK_STATE;
                goto done;
            }
        }
        else
        {
            status = rmGpuGroupLockAcquire(pParentGpu->gpuInstance,
                                           GPU_LOCK_GRP_DEVICE,
                                           GPUS_LOCK_FLAGS_NONE,
                                           RM_LOCK_MODULES_CLIENT,
                                           &pLockInfo->gpuMask);
            if (status != NV_OK)
                goto done;

            *pReleaseFlags |= RM_LOCK_RELEASE_GPU_GROUP_LOCK;
            pLockInfo->state |= RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED;
        }
    }

done:
    switch(pLockInfo->traceOp)
    {
        case RS_LOCK_TRACE_ALLOC:
            LOCK_METER_DATA(ALLOC, pLockInfo->traceClassId, 0, 0);
            break;
        case RS_LOCK_TRACE_FREE:
            LOCK_METER_DATA(FREE_OBJECT, pLockInfo->traceClassId, 0, 0);
            break;
        case RS_LOCK_TRACE_CTRL:
            LOCK_METER_DATA(RMCTRL, pLockInfo->traceClassId, pLockInfo->flags, status);
            break;
        default:
            break;
    }

    return status;
}

NV_STATUS
serverAllocEpilogue_WAR
(
    RsServer *pServer,
    NV_STATUS status,
    NvBool bClientAlloc,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    //
    // Pre-Volta Linux swapgroups is the only remaining use of channel grabbing.
    // Bug 2869820 is tracking the transition of swapgroups from requiring this
    // RM feature.
    //
    NV_STATUS tmpStatus;
    if (!bClientAlloc && status == NV_ERR_INSERT_DUPLICATE_NAME)
    {
        NvBool gpulockRelease = NV_FALSE;
        RsResourceRef *pResourceRef;

        if (!rmGpuLockIsOwner())
        {
            tmpStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                          RM_LOCK_MODULES_CLIENT);

            if (tmpStatus != NV_OK)
                return tmpStatus;

            gpulockRelease = NV_TRUE;
        }

        //
        // Hack for taking ownership of display channels. Clients call rmAlloc
        // on a previously allocated handle to indicate they want to grab
        // ownership of the underlying hardware channel.
        //
        // TODO - this should be moved to an RM control and called directly by
        // clients instead of the overloaded allocation call. RmAlloc should
        // be for allocating objects only.
        //
        tmpStatus = clientGetResourceRef(pRmAllocParams->pClient, pRmAllocParams->hResource, &pResourceRef);
        if (tmpStatus == NV_OK)
        {
            DispChannel *pDispChannel = dynamicCast(pResourceRef->pResource, DispChannel);
            if (pDispChannel != NULL)
            {
                status = dispchnGrabChannel(pDispChannel,
                                            pRmAllocParams->hClient,
                                            pRmAllocParams->hParent,
                                            pRmAllocParams->hResource,
                                            pRmAllocParams->externalClassId,
                                            pRmAllocParams->pAllocParams);
            }
        }

        if (gpulockRelease)
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    return status;
}

static NV_STATUS
_rmAlloc
(
    NvHandle          hClient,
    NvHandle          hParent,
    NvHandle         *phObject,
    NvU32             hClass,
    NvP64             pUserAllocParams,
    NvU32             paramsSize,
    NvU32             allocFlags,
    NvU32             allocInitStates,
    RS_LOCK_INFO     *pLockInfo,
    NvP64             pRightsRequested,
    API_SECURITY_INFO secInfo
)
{
    NV_STATUS status;
    RS_RES_ALLOC_PARAMS_INTERNAL   rmAllocParams = {0};

    NV_ASSERT_OR_RETURN(phObject != NULL, NV_ERR_INVALID_ARGUMENT);

    // init RmAllocParams
    rmAllocParams.hClient          = hClient;
    rmAllocParams.hParent          = hParent;
    rmAllocParams.hResource        = *phObject;
    rmAllocParams.externalClassId  = hClass;
    rmAllocParams.allocFlags       = allocFlags;
    rmAllocParams.allocState       = allocInitStates;
    rmAllocParams.pSecInfo         = &secInfo;
    rmAllocParams.pResourceRef     = NULL;
    rmAllocParams.pAllocParams     = NvP64_VALUE(pUserAllocParams);
    rmAllocParams.paramsSize       = paramsSize;
    rmAllocParams.pLockInfo        = pLockInfo;
    rmAllocParams.pRightsRequested = NvP64_VALUE(pRightsRequested);
    rmAllocParams.pRightsRequired  = NULL;

    status = serverAllocResource(&g_resServ, &rmAllocParams);
    *phObject = rmAllocParams.hResource;

    return status;

}

static NV_STATUS
_serverAlloc_ValidateVgpu
(
    RsClient *pClient,
    NvU32 hParent,
    NvU32 externalClassId,
    RS_PRIV_LEVEL privLevel,
    const NvU32 flags
)
{
    //
    // Get any GPU to check environmental properties
    // Assume: In multi-vGPU systems, no mix and matching SRIOV and non-SRIOV GPUs.
    // Assume: This OBJGPU will only used to read system properties.
    //
    OBJGPU *pGpu = gpumgrGetSomeGpu();

    // Check whether context is already sufficiently privileged
    if (flags & RS_FLAGS_ALLOC_PRIVILEGED)
    {
        if (privLevel >= RS_PRIV_LEVEL_USER_ROOT)
        {
            return NV_TRUE;
        }
    }

    // Check whether object is allowed in current environment
    if
    (
        (IS_SRIOV_WITH_VGPU_GSP_ENABLED(pGpu) && (flags & RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_VGPU_GSP))
    )
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NV_STATUS
_serverAllocValidatePrivilege
(
    RsServer *pServer,
    RS_RESOURCE_DESC *pResDesc,
    RS_RES_ALLOC_PARAMS *pParams
)
{
    RsClient *pClient = pParams->pClient;

    // Reject allocations for objects with no flags.
    if (!(pResDesc->flags & RS_FLAGS_ALLOC_NON_PRIVILEGED) &&
        !(pResDesc->flags & RS_FLAGS_ALLOC_PRIVILEGED) &&
        !(pResDesc->flags & RS_FLAGS_ALLOC_KERNEL_PRIVILEGED))
    {
        // See GPUSWSEC-1560 for more details on object privilege flag requirements
        NV_PRINTF(LEVEL_WARNING, "external class 0x%08x is missing its privilege flag in RS_ENTRY\n", pParams->externalClassId);
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (hypervisorIsVgxHyper() &&
        clientIsAdmin(pClient, clientGetCachedPrivilege(pClient)) &&
        (pParams->pSecInfo->privLevel != RS_PRIV_LEVEL_KERNEL) &&
        !(pResDesc->flags & RS_FLAGS_ALLOC_NON_PRIVILEGED))
    {
        // Host CPU-RM context
        if (!_serverAlloc_ValidateVgpu(pClient, pParams->hParent, pParams->externalClassId,
                                      pParams->pSecInfo->privLevel, pResDesc->flags))
        {
            NV_PRINTF(LEVEL_WARNING,
                      "hClient: 0x%08x, externalClassId: 0x%08x: CPU hypervisor does not have permission to allocate object\n",
                      pParams->hClient, pParams->externalClassId);
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }
    else
    {
        RS_PRIV_LEVEL privLevel = pParams->pSecInfo->privLevel;

        // Default case, verify admin and kernel privileges
        if (pResDesc->flags & RS_FLAGS_ALLOC_PRIVILEGED)
        {
            if (privLevel < RS_PRIV_LEVEL_USER_ROOT)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "hClient: 0x%08x, externalClassId: 0x%08x: non-privileged context tried to allocate privileged object\n",
                          pParams->hClient, pParams->externalClassId);
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }

        if (pResDesc->flags & RS_FLAGS_ALLOC_KERNEL_PRIVILEGED)
        {
            if (privLevel < RS_PRIV_LEVEL_KERNEL)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "hClient: 0x%08x, externalClassId: 0x%08x: non-privileged context tried to allocate kernel privileged object\n",
                          pParams->hClient, pParams->externalClassId);
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }
    }

    return NV_OK;
}

NV_STATUS
serverAllocResourceUnderLock
(
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS *pRmAllocParams
)
{
    NvHandle                 hClient = pRmAllocParams->hClient;
    NvHandle                 hParent;
    RS_RESOURCE_DESC        *pResDesc;
    NV_STATUS                status = NV_OK;
    NV_STATUS                tmpStatus;
    RsClient                *pClient = pRmAllocParams->pClient;
    RsResourceRef           *pParentRef = NULL;
    RsResourceRef           *pResourceRef = NULL;
    NvU32                    i = 0;
    RS_LOCK_INFO            *pLockInfo = pRmAllocParams->pLockInfo;
    NvU32                    releaseFlags = 0;
    RS_ACCESS_MASK           rightsRequired;
    LOCK_ACCESS_TYPE         resLockAccess = LOCK_ACCESS_WRITE;
    OBJGPU                  *pGpu = NULL;
    NvBool                   bClearRecursiveStateFlag = NV_FALSE;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    if (pRmAllocParams->pSecInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pResDesc = RsResInfoByExternalClassId(pRmAllocParams->externalClassId);
    if (pResDesc == NULL)
    {
        return NV_ERR_INVALID_CLASS;
    }

    NV_ASSERT_OK_OR_RETURN(rmapiFixupAllocParams(&pResDesc, pRmAllocParams));
    rmapiResourceDescToLegacyFlags(pResDesc, &pLockInfo->flags, NULL);

    status = _serverAllocValidatePrivilege(pServer, pResDesc, pRmAllocParams);
    if (status != NV_OK)
        goto done;

    pLockInfo->traceOp = RS_LOCK_TRACE_ALLOC;
    pLockInfo->traceClassId = pRmAllocParams->externalClassId;
    hParent = pRmAllocParams->hParent;
    if (pRmAllocParams->hResource == hClient)
    {
        if (pResDesc->pParentList[i] != 0)
        {
            status = NV_ERR_INVALID_OBJECT_PARENT;
            goto done;
        }
        hParent = 0;

        // Single instance restriction is implied
        NV_ASSERT(!pResDesc->bMultiInstance);
    }
    else
    {
        // Check if parent is valid
        status = clientGetResourceRef(pClient, hParent, &pParentRef);
        if (status != NV_OK)
        {
            goto done;
        }
        pLockInfo->pContextRef = pParentRef;
    }

    if ((pResDesc->flags & RS_FLAGS_INTERNAL_ONLY) &&
        !(pRmAllocParams->allocState & RM_ALLOC_STATES_INTERNAL_ALLOC))
    {
        status = NV_ERR_INVALID_CLASS;
        goto done;
    }

    status = serverAllocResourceLookupLockFlags(&g_resServ, RS_LOCK_RESOURCE, pRmAllocParams, &resLockAccess);
    if (status != NV_OK)
        goto done;

    //
    // We can get the GPU pointer for alloc of a device child.
    // Device allocs need to be handled separately. See deviceInit_IMPL()
    //
    tmpStatus = gpuGetByRef(pParentRef, NULL, &pGpu);

    // Override locking flags if we'll need to RPC to GSP
    if (pGpu != NULL && IS_GSP_CLIENT(pGpu) &&
        (pResDesc->flags & RS_FLAGS_ALLOC_RPC_TO_PHYS_RM))
    {
        resLockAccess = LOCK_ACCESS_WRITE; // always write as we're RPCing to GSP

        //
        // If the resource desc says no need for GPU locks, we still need to lock
        // the current pGpu in order to send the RPC
        //
        if (pLockInfo->flags & RM_LOCK_FLAGS_NO_GPUS_LOCK)
        {
            NV_PRINTF(LEVEL_INFO, "Overriding flags for alloc of class %04x\n",
                pRmAllocParams->externalClassId);
            pLockInfo->flags &= ~RM_LOCK_FLAGS_NO_GPUS_LOCK;
            pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
            if ((pLockInfo->state & RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS) == 0)
            {
                pLockInfo->state |= RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;
                bClearRecursiveStateFlag = NV_TRUE;
            }
        }
    }

    status = serverResLock_Prologue(&g_resServ, resLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    if (pParentRef != NULL)
    {
        //
        // Check if GPU in fatal condition.
        //
        if (pGpu != NULL)
        {
            //
            // Abort if any GPU in SLI has fatal error.
            // This property will be cleared on RM init, by which time the
            // client should have reset the GPUs.
            //
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY |
                           SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FATAL_ERROR))
                {
                    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
                    if (pKernelRc != NULL)
                    {
                        krcBreakpoint(pKernelRc);
                    }
                    status = NV_ERR_RESET_REQUIRED;
                    SLI_LOOP_GOTO(done);
                }
            }
            SLI_LOOP_END;
        }

        // If single instance, ensure parent doesn't yet have a class of this type
        if (!pResDesc->bMultiInstance)
        {
            if (refFindChildOfType(pParentRef, pResDesc->pClassInfo->classId, NV_TRUE, NULL) == NV_OK)
            {
                status = NV_ERR_STATE_IN_USE;
            }
        }

        // Check if hParent is an allowed parent for this resource
        if (status == NV_OK && !pResDesc->bAnyParent)
        {
            status = NV_ERR_INVALID_OBJECT_PARENT;
            for (i = 0; pResDesc->pParentList[i]; i++)
            {
                if (pParentRef->internalClassId == pResDesc->pParentList[i])
                {
                    status = NV_OK;
                    break;
                }
            }
        }
    }

    if (status != NV_OK)
        goto done;

    status = clientAssignResourceHandle(pClient, &pRmAllocParams->hResource);
    if (status != NV_OK)
        goto done;

    pRmAllocParams->hParent = (pRmAllocParams->hParent == 0) ? pRmAllocParams->hClient : pRmAllocParams->hParent;

    if (pServer->bRsAccessEnabled)
    {
        rsAccessMaskFromArray(&rightsRequired, pResDesc->pRightsRequiredArray,
                              pResDesc->rightsRequiredLength);
        pRmAllocParams->pRightsRequired = &rightsRequired;
    }

    status = clientAllocResource(pClient, &g_resServ, pRmAllocParams);
    if (status != NV_OK)
        goto done;

    pResourceRef = pRmAllocParams->pResourceRef;

    //
    // Alloc RPC handling
    //
    if (!(pRmAllocParams->allocState & RM_ALLOC_STATES_SKIP_RPC))
    {
        if (pResDesc->flags & (RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM))
        {
            OBJGPU *pGpu = NULL;
            RmResource *pRmResource = dynamicCast(pResourceRef->pResource, RmResource);
            CALL_CONTEXT callContext = {0};
            CALL_CONTEXT *pOldContext = NULL;

            status = gpuGetByRef(pResourceRef, NULL, &pGpu);
            if (status != NV_OK || pRmResource == NULL)
            {
                status = NV_ERR_INVALID_CLASS;
                goto done;
            }

            if (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
            {
                status = NV_OK;
                goto done;
            }

            // if physical RM RPC make sure we're a GSP client otherwise skip
            if (((pResDesc->flags & (RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM)) == RS_FLAGS_ALLOC_RPC_TO_PHYS_RM) &&
                (!IS_GSP_CLIENT(pGpu)))
            {
                status = NV_OK;
                goto done;
            }

            // Set the call context to allow vgpuGetCallingContextDevice()
            // and other context dependent functions to operate in the RPC code.
            //
            // The context is assigned in the above clientAllocResource() call,
            // but we can't simply extend the context scope to this place
            // as pResourceRef is allocated internally in clientAllocResource().
            //
            // Instead, we create basically the same context here once again
            // and use it for the RPC call.
            callContext.pServer = pServer;
            callContext.pClient = pClient;
            callContext.pResourceRef = pResourceRef;
            callContext.pLockInfo = pRmAllocParams->pLockInfo;
            callContext.secInfo = *pRmAllocParams->pSecInfo;

            NV_ASSERT_OK_OR_GOTO(status,
                resservSwapTlsCallContext(&pOldContext, &callContext), done);
            NV_RM_RPC_ALLOC_OBJECT(pGpu,
                                   pRmAllocParams->hClient,
                                   pRmAllocParams->hParent,
                                   pRmAllocParams->hResource,
                                   pRmAllocParams->externalClassId,
                                   pRmAllocParams->pAllocParams,
                                   pRmAllocParams->paramsSize,
                                   status);
            NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

            if (status != NV_OK)
                goto done;

            pRmResource->bRpcFree = NV_TRUE;
        }
    }

done:
    if ((status != NV_OK) && (pResourceRef != NULL))
    {
        RS_RES_FREE_PARAMS params;

        portMemSet(&params, 0, sizeof(params));
        params.hClient = hClient;
        params.hResource = pRmAllocParams->hResource;
        params.pResourceRef = pResourceRef;
        params.pSecInfo = pRmAllocParams->pSecInfo;
        params.pLockInfo = pRmAllocParams->pLockInfo;
        tmpStatus = clientFreeResource(pClient, &g_resServ, &params);
        NV_ASSERT(tmpStatus == NV_OK);
        pRmAllocParams->pResourceRef = NULL;
    }

    serverResLock_Epilogue(&g_resServ, resLockAccess, pLockInfo, &releaseFlags);

    if (bClearRecursiveStateFlag)
    {
        pLockInfo->state &= ~RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;
    }
    return status;
}

NV_STATUS
serverFreeResourceRpcUnderLock
(
    RsServer *pServer,
    RS_RES_FREE_PARAMS *pFreeParams
)
{
    NV_STATUS status;
    RsResourceRef *pResourceRef = pFreeParams->pResourceRef;
    OBJGPU *pGpu = NULL;
    NvBool  bBcResource;
    RmResource *pRmResource = NULL;

    NV_ASSERT_OR_RETURN(pResourceRef != NULL, NV_ERR_INVALID_OBJECT_HANDLE);

    pRmResource = dynamicCast(pResourceRef->pResource, RmResource);
    status = gpuGetByRef(pResourceRef, &bBcResource, &pGpu);
    if ((status != NV_OK) ||
        (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu)) ||
        (pRmResource == NULL) ||
        (pRmResource->bRpcFree == NV_FALSE))
    {
        status = NV_OK;
        goto rpc_done;
    }

    gpuSetThreadBcState(pGpu, bBcResource);
    NV_RM_RPC_FREE(pGpu, pResourceRef->pClient->hClient,
                   pResourceRef->pParentRef->hResource,
                   pResourceRef->hResource, status);

    NvBool clientInUse = NV_FALSE;
    if ( IS_VIRTUAL(pGpu) && (pResourceRef->externalClassId == GT200_DEBUGGER))
    {
        RS_ITERATOR             it;

        it = clientRefIter(pResourceRef->pClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(it.pClient, &it))
        {
            Device *pDeviceTest = dynamicCast(it.pResourceRef->pResource, Device);
            // In VGPU-GSP mode each plugin only handles it's own GPU,
            // so we need to check if Device has the same OBJGPU.
            if (pDeviceTest != NULL && (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || GPU_RES_GET_GPU(pDeviceTest) == pGpu))
            {
                clientInUse = NV_TRUE;
                break;
            }
        }

        it = clientRefIter(pResourceRef->pClient, NULL, classId(KernelSMDebuggerSession), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(it.pClient, &it))
        {
            KernelSMDebuggerSession *pKernelSMDebuggerSession = dynamicCast(it.pResourceRef->pResource, KernelSMDebuggerSession);
            if ((dynamicCast(pResourceRef->pResource, KernelSMDebuggerSession) != pKernelSMDebuggerSession) && pKernelSMDebuggerSession != NULL &&
                (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || GPU_RES_GET_GPU(pKernelSMDebuggerSession) == pGpu))
            {
                clientInUse = NV_TRUE;
                break;
            }
        }
        // If neither any devices nor KernelSMDebuggerSession are in use, free up the client on host.
        if (!clientInUse)
        {
            //
            // vGPU:
            //
            // Since vGPU does all real hardware management in the
            // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
            // do an RPC to the host to do the hardware update.
            //
            NV_RM_RPC_FREE(pGpu, pResourceRef->pClient->hClient, NV01_NULL_OBJECT, pResourceRef->pClient->hClient, status);
        }
    }

rpc_done:
    return status;
}

void
serverResLock_Epilogue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if (*pReleaseFlags & RM_LOCK_RELEASE_GPU_GROUP_LOCK)
    {
        // UNLOCK: release GPU group lock
        rmGpuGroupLockRelease(pLockInfo->gpuMask, GPUS_LOCK_FLAGS_NONE);
        pLockInfo->state &= ~RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED;
        *pReleaseFlags &= ~RM_LOCK_RELEASE_GPU_GROUP_LOCK;
    }

    if (*pReleaseFlags & RM_LOCK_RELEASE_GPUS_LOCK)
    {
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        pLockInfo->state &= ~RM_LOCK_STATES_GPUS_LOCK_ACQUIRED;
        *pReleaseFlags &= ~RM_LOCK_RELEASE_GPUS_LOCK;
    }
}

NV_STATUS
serverInitFreeParams_Recursive(NvHandle hClient, NvHandle hResource, RS_LOCK_INFO *pLockInfo, RS_RES_FREE_PARAMS *pParams)
{
    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->hClient = hClient;
    pParams->hResource = hResource;
    pParams->pLockInfo = pLockInfo;
    return NV_OK;
}

NV_STATUS
serverUpdateLockFlagsForFree
(
    RsServer *pServer,
    RS_RES_FREE_PARAMS_INTERNAL *pRmFreeParams
)
{
    RS_LOCK_INFO *pLockInfo = pRmFreeParams->pLockInfo;
    OBJGPU *pGpu = NULL;

    rmapiResourceDescToLegacyFlags(pRmFreeParams->pResourceRef->pResourceDesc, NULL, &pLockInfo->flags);

    pLockInfo->pContextRef = pRmFreeParams->pResourceRef->pParentRef;
    if (gpuGetByRef(pLockInfo->pContextRef, NULL, &pGpu) == NV_OK)
    {
        RmResource *pRmResource = dynamicCast(pRmFreeParams->pResourceRef->pResource, RmResource);
        if (pGpu != NULL && IS_GSP_CLIENT(pGpu) && pRmResource != NULL && pRmResource->bRpcFree)
        {
            //
            // If the resource desc says no need for GPU locks, we still need to lock
            // the current pGpu in order to send the RPC
            //
            if (pLockInfo->flags & RM_LOCK_FLAGS_NO_GPUS_LOCK)
            {
                NV_PRINTF(LEVEL_INFO, "Overriding flags for free of class %04x\n",
                    pRmFreeParams->pResourceRef->externalClassId);
                pLockInfo->flags &= ~RM_LOCK_FLAGS_NO_GPUS_LOCK;
                pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
                pLockInfo->state |= RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;
            }
        }
    }

    return NV_OK;
}

NV_STATUS
rmapiFreeResourcePrologue
(
    RS_RES_FREE_PARAMS_INTERNAL *pRmFreeParams
)
{
    RsResourceRef     *pResourceRef = pRmFreeParams->pResourceRef;
    NV_STATUS          tmpStatus;
    OBJGPU            *pGpu         = NULL;
    NvBool             bBcResource;

    NV_ASSERT_OR_RETURN(pResourceRef, NV_ERR_INVALID_OBJECT_HANDLE);

    //
    // Use gpuGetByRef instead of GpuResource because gpuGetByRef will work even
    // if resource isn't a GpuResource (e.g.: Memory which can be allocated
    // under a subdevice, device or client root)
    //
    tmpStatus = gpuGetByRef(pResourceRef, &bBcResource, &pGpu);
    if (tmpStatus == NV_OK)
        gpuSetThreadBcState(pGpu, bBcResource);

    //
    // Need to cancel pending timer callbacks before event structs are freed.
    // RS-TODO: provide notifications to objects referencing events or add
    // dependency
    //
    TimerApi *pTimerApi = dynamicCast(pResourceRef->pResource, TimerApi);
    if (pTimerApi != NULL)
    {
        tmrapiDeregisterEvents(pTimerApi);
    }

    CliDelObjectEvents(pResourceRef);

    return NV_OK;
}

NV_STATUS
rmapiAlloc
(
    RM_API      *pRmApi,
    NvHandle     hClient,
    NvHandle     hParent,
    NvHandle    *phObject,
    NvU32        hClass,
    void        *pAllocParams,
    NvU32        paramsSize
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->AllocWithSecInfo(pRmApi, hClient, hParent, phObject, hClass, NV_PTR_TO_NvP64(pAllocParams), paramsSize,
                                    RMAPI_ALLOC_FLAGS_NONE, NvP64_NULL, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiAllocWithHandle
(
    RM_API      *pRmApi,
    NvHandle     hClient,
    NvHandle     hParent,
    NvHandle     hObject,
    NvU32        hClass,
    void        *pAllocParams,
    NvU32        paramsSize
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->AllocWithSecInfo(pRmApi, hClient, hParent, &hObject, hClass, NV_PTR_TO_NvP64(pAllocParams), paramsSize,
                                    RMAPI_ALLOC_FLAGS_NONE, NvP64_NULL, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiAllocWithSecInfo
(
    RM_API              *pRmApi,
    NvHandle             hClient,
    NvHandle             hParent,
    NvHandle            *phObject,
    NvU32                hClass,
    NvP64                pAllocParams,
    NvU32                paramsSize,
    NvU32                flags,
    NvP64                pRightsRequested,
    API_SECURITY_INFO   *pSecInfo
)
{
    NV_STATUS      status;
    NvU32          allocInitStates = RM_ALLOC_STATES_NONE;
    RM_API_CONTEXT rmApiContext    = {0};
    RS_LOCK_INFO  *pLockInfo;
    NvHandle       hSecondClient = NV01_NULL_OBJECT;

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
        return status;

    pLockInfo = portMemAllocNonPaged(sizeof(*pLockInfo));
    if (pLockInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    if (pSecInfo->paramLocation == PARAM_LOCATION_KERNEL)
    {
        status = serverAllocLookupSecondClient(hClass,
                                               NvP64_VALUE(pAllocParams),
                                               &hSecondClient);
        if (status != NV_OK)
            goto done;
    }

    portMemSet(pLockInfo, 0, sizeof(*pLockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, hSecondClient, pLockInfo);
    if (status != NV_OK)
        goto done;

    // RS-TODO: Fix calls that use RMAPI_GPU_LOCK_INTERNAL without holding the API lock
    if (pRmApi->bGpuLockInternal && !rmapiLockIsOwner())
    {
        // CORERM-6052 targets fixing the API lockless path for RTD3.
        if (!rmapiInRtd3PmPath())
        {
            NV_PRINTF(LEVEL_ERROR,
                "NVRM: %s: RMAPI_GPU_LOCK_INTERNAL alloc requested without holding the RMAPI lock: client:0x%x parent:0x%x object:0x%x class:0x%x\n",
                __FUNCTION__, hClient, hParent, *phObject, hClass);
        }

        pLockInfo->flags |= RM_LOCK_FLAGS_NO_API_LOCK;
        pLockInfo->state &= ~RM_LOCK_STATES_API_LOCK_ACQUIRED;
    }


    // This flag applies to both VGPU and GSP cases
    if (flags & RMAPI_ALLOC_FLAGS_SKIP_RPC)
        allocInitStates |= RM_ALLOC_STATES_SKIP_RPC;

    //
    // Mark internal client allocations as such, so the resource server generates
    // the internal client handle with a distinct template.
    // The distinct template purpose is to make sure that GSP client provided
    // client handles do not collide with the client handles ganerated by the GSP/FW RM.
    //
    if ((pSecInfo->privLevel >= RS_PRIV_LEVEL_KERNEL) &&
        (pSecInfo->paramLocation == PARAM_LOCATION_KERNEL) && pRmApi->bGpuLockInternal)
        allocInitStates |= RM_ALLOC_STATES_INTERNAL_CLIENT_HANDLE;

    if ((pSecInfo->paramLocation == PARAM_LOCATION_KERNEL) &&
        (pRmApi->bApiLockInternal || pRmApi->bGpuLockInternal))
        allocInitStates |= RM_ALLOC_STATES_INTERNAL_ALLOC;

    NV_PRINTF(LEVEL_INFO, "client:0x%x parent:0x%x object:0x%x class:0x%x\n",
              hClient, hParent, *phObject, hClass);

    status = _rmAlloc(hClient,
                      hParent,
                      phObject,
                      hClass,
                      pAllocParams,
                      paramsSize,
                      flags,
                      allocInitStates,
                      pLockInfo,
                      pRightsRequested,
                      *pSecInfo);


    //
    // If hClient is allocated behind GPU locks, client is marked as internal
    //
    if ((status == NV_OK) && ((hClass == NV01_ROOT) || (hClass == NV01_ROOT_NON_PRIV) || (hClass == NV01_ROOT_CLIENT)) &&
        pSecInfo->paramLocation == PARAM_LOCATION_KERNEL && pRmApi->bGpuLockInternal)
    {
        void *pHClient = *(void **)&pAllocParams;

        // flag this client as an RM internal client
        rmclientSetClientFlagsByHandle(*(NvU32*)pHClient /* hClient */, RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT);
    }

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "allocation complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING, "allocation failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
        NV_PRINTF(LEVEL_WARNING,
                  "client:0x%x parent:0x%x object:0x%x class:0x%x\n", hClient,
                  hParent, *phObject, hClass);
    }

    portMemFree(pLockInfo);

done:
    rmapiEpilogue(pRmApi, &rmApiContext);

    return status;
}

NV_STATUS
resservClientFactory
(
    PORT_MEM_ALLOCATOR *pAllocator,
    RS_RES_ALLOC_PARAMS *pParams,
    RsClient **ppRsClient
)
{
    RmClient *pClient;
    NV_STATUS status;

    status = objCreate(&pClient, NVOC_NULL_OBJECT, RmClient, pAllocator, pParams);

    if (status != NV_OK)
    {
        return status;
    }
    NV_ASSERT(pClient != NULL);

    *ppRsClient = staticCast(pClient, RsClient);
    return NV_OK;
}

NV_STATUS
resservResourceFactory
(
    PORT_MEM_ALLOCATOR   *pAllocator,
    CALL_CONTEXT         *pCallContext,
    RS_RES_ALLOC_PARAMS  *pParams,
    RsResource          **ppResource
)
{
    RS_RESOURCE_DESC *pResDesc;
    NV_STATUS   status;
    Dynamic    *pDynamic = NULL;
    RsResource *pResource = NULL;
    OBJGPU     *pGpu = NULL;

    pResDesc = RsResInfoByExternalClassId(pParams->externalClassId);
    if (pResDesc == NULL)
        return NV_ERR_INVALID_CLASS;

    if (pCallContext->pResourceRef->pParentRef != NULL &&
        pCallContext->pResourceRef->pParentRef->pResource != NULL)
    {
        GpuResource *pParentGpuResource = dynamicCast(pCallContext->pResourceRef->pParentRef->pResource,
                                                      GpuResource);
        if (pParentGpuResource != NULL)
        {
            pGpu = GPU_RES_GET_GPU(pParentGpuResource);
        }
    }

    if (pResDesc->internalClassId == classId(Device))
    {
        if (pParams->pAllocParams == NULL)
            return NV_ERR_INVALID_ARGUMENT;

        NV0080_ALLOC_PARAMETERS *pNv0080AllocParams = pParams->pAllocParams;
        NvU32 deviceInst = pNv0080AllocParams->deviceId;

        if (deviceInst >= NV_MAX_DEVICES)
            return NV_ERR_INVALID_ARGUMENT;

        NvU32 gpuInst = gpumgrGetPrimaryForDevice(deviceInst);

        if ((pGpu = gpumgrGetGpu(gpuInst)) == NULL)
        {
            return NV_ERR_INVALID_STATE;
        }
    }

    status = objCreateDynamicWithFlags(&pDynamic,
                                       (Object*)pGpu,
                                       pResDesc->pClassInfo,
                                       NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY,
                                       pCallContext,
                                       pParams);
    if (status != NV_OK)
        return status;

    pResource = dynamicCast(pDynamic, RsResource);

    if (pResource == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    if (pResDesc->internalClassId == classId(Subdevice) || pResDesc->internalClassId == classId(Device))
    {
        pGpu = GPU_RES_GET_GPU(dynamicCast(pDynamic, GpuResource));

        if (pGpu && !IS_MIG_IN_USE(pGpu))
        {
            rmapiControlCacheSetGpuAttrForObject(pParams->hClient, pParams->hResource, pGpu);
        }
    }

    *ppResource = pResource;

    return status;
}

NV_STATUS
rmapiAllocWithSecInfoTls
(
    RM_API              *pRmApi,
    NvHandle             hClient,
    NvHandle             hParent,
    NvHandle            *phObject,
    NvU32                hClass,
    NvP64                pAllocParams,
    NvU32                paramsSize,
    NvU32                flags,
    NvP64                pRightsRequested,
    API_SECURITY_INFO   *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiAllocWithSecInfo(pRmApi, hClient, hParent, phObject, hClass,
                                   pAllocParams, paramsSize, flags, pRightsRequested, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
rmapiFree
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hObject
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->FreeWithSecInfo(pRmApi, hClient, hObject, RMAPI_FREE_FLAGS_NONE, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiFreeWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS          status;
    RS_RES_FREE_PARAMS freeParams;
    RS_LOCK_INFO lockInfo;
    RM_API_CONTEXT rmApiContext = {0};

    portMemSet(&freeParams, 0, sizeof(freeParams));

    NV_PRINTF(LEVEL_INFO, "Nv01Free: client:0x%x object:0x%x\n", hClient,
              hObject);

    status = rmapiPrologue(pRmApi, &rmApiContext);

    if (status != NV_OK)
        return status;

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, NV01_NULL_OBJECT, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return status;
    }

    // RS-TODO: Fix calls that use RMAPI_GPU_LOCK_INTERNAL without holding the API lock
    if (pRmApi->bGpuLockInternal && !rmapiLockIsOwner())
    {
        // CORERM-6052 targets fixing the API lockless path for RTD3.
        if (!rmapiInRtd3PmPath())
        {
            NV_PRINTF(LEVEL_ERROR, "RMAPI_GPU_LOCK_INTERNAL free requested without holding the RMAPI lock\n");
        }

        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
        lockInfo.state &= ~RM_LOCK_STATES_API_LOCK_ACQUIRED;
    }

    freeParams.hClient = hClient;
    freeParams.hResource = hObject;
    freeParams.freeState = RM_FREE_STATES_NONE;
    freeParams.pLockInfo = &lockInfo;
    freeParams.freeFlags = flags;
    freeParams.pSecInfo = pSecInfo;

    rmapiControlCacheFreeObjectEntry(hClient, hObject);

    status = serverFreeResourceTree(&g_resServ, &freeParams);

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv01Free: free complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Nv01Free: free failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
        NV_PRINTF(LEVEL_WARNING, "Nv01Free:  client:0x%x object:0x%x\n",
                  hClient, hObject);
    }

    return status;
}

NV_STATUS
rmapiFreeWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiFreeWithSecInfo(pRmApi, hClient, hObject, flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
rmapiDisableClients
(
    RM_API   *pRmApi,
    NvHandle *phClientList,
    NvU32     numClients
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->DisableClientsWithSecInfo(pRmApi, phClientList, numClients, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiDisableClientsWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle          *phClientList,
    NvU32              numClients,
    API_SECURITY_INFO *pSecInfo
)
{
    OBJSYS            *pSys = SYS_GET_INSTANCE();
    NvU32              lockState = 0;
    NvU32              i;

    NV_PRINTF(LEVEL_INFO, "numClients: %d\n", numClients);

    if (!pRmApi->bRmSemaInternal && osAcquireRmSema(pSys->pSema) != NV_OK)
        return NV_ERR_INVALID_LOCK_STATE;

    if (pRmApi->bApiLockInternal)
        lockState |= RM_LOCK_STATES_API_LOCK_ACQUIRED;

    if (pRmApi->bGpuLockInternal)
        lockState |= RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;

    for (i = 0; i < numClients; ++i)
        rmapiControlCacheFreeClientEntry(phClientList[i]);

    serverMarkClientListDisabled(&g_resServ, phClientList, numClients, lockState, pSecInfo);

    if (!pRmApi->bRmSemaInternal)
        osReleaseRmSema(pSys->pSema, NULL);

    NV_PRINTF(LEVEL_INFO, "Disable clients complete\n");

    return NV_OK;
}

NV_STATUS
rmapiDisableClientsWithSecInfoTls
(
    RM_API              *pRmApi,
    NvHandle            *phClientList,
    NvU32                numClients,
    API_SECURITY_INFO   *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiDisableClientsWithSecInfo(pRmApi, phClientList, numClients, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NvBool
serverRwApiLockIsOwner
(
    RsServer *pServer
)
{
    return rmapiLockIsWriteOwner();
}

NV_STATUS
serverAllocResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    if (lock == RS_LOCK_TOP)
    {
        RS_RESOURCE_DESC *pResDesc;

        if (!serverSupportsReadOnlyLock(&g_resServ, RS_LOCK_TOP, RS_API_ALLOC_RESOURCE))
        {
            *pAccess = LOCK_ACCESS_WRITE;
            return NV_OK;
        }

        pResDesc = RsResInfoByExternalClassId(pParams->externalClassId);

        if (pResDesc == NULL)
        {
            return NV_ERR_INVALID_CLASS;
        }

        if (pResDesc->flags & RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC)
            *pAccess = LOCK_ACCESS_READ;
        else
            *pAccess = LOCK_ACCESS_WRITE;

        return NV_OK;
    }

    if (lock == RS_LOCK_RESOURCE)
    {
        *pAccess = LOCK_ACCESS_WRITE;
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
serverFreeResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_FREE_PARAMS_INTERNAL *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_FREE_RESOURCE))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}
