/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"

#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "rmapi/client.h"
#include "rmapi/client_resource.h"
#include "rmapi/resource_fwd_decls.h"
#include "core/locks.h"
#include "core/system.h"
#include "gpu/device/device.h"
#include "resource_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"

#include "gpu/bus/third_party_p2p.h"

UserInfoList g_userInfoList;
RmClientList g_clientListBehindGpusLock; // RS-TODO remove this WAR

#define RS_FW_UNIQUE_HANDLE_BASE  (0xc9f00000)

NV_STATUS _registerUserInfo(PUID_TOKEN *ppUidToken, UserInfo **ppUserInfo);
NV_STATUS _unregisterUserInfo(UserInfo *pUserInfo);

NV_STATUS
rmclientConstruct_IMPL
(
    RmClient *pClient,
    PORT_MEM_ALLOCATOR* pAllocator,
    RS_RES_ALLOC_PARAMS_INTERNAL* pParams
)
{
    NV_STATUS          status = NV_OK;
    NvU32              i;
    OBJSYS            *pSys = SYS_GET_INSTANCE();
    RsClient          *pRsClient = staticCast(pClient, RsClient);
    NvBool             bReleaseLock = NV_FALSE;
    API_SECURITY_INFO *pSecInfo = pParams->pSecInfo;

    pClient->bIsRootNonPriv  = (pParams->externalClassId == NV01_ROOT_NON_PRIV);
    pClient->ProcID          = osGetCurrentProcess();
    pClient->pUserInfo       = NULL;
    pClient->pSecurityToken  = NULL;
    pClient->pOSInfo         = pSecInfo->clientOSInfo;

    pClient->cachedPrivilege = pSecInfo->privLevel;

    // Set user-friendly client name from current process
    osGetCurrentProcessName(pClient->name, NV_PROC_NAME_MAX_LENGTH);

    for (i = 0; i < NV0000_NOTIFIERS_MAXCOUNT; i++)
    {
        pClient->CliSysEventInfo.notifyActions[i] =
            NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    // Prevent kernel clients from requesting handles in the FW handle generator range
    status = clientSetRestrictedRange(pRsClient,
                                      RS_FW_UNIQUE_HANDLE_BASE, RS_UNIQUE_HANDLE_RANGE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Failed to set host client restricted resource handle range. Status=%x\n", status);
        return status;
    }

    if (!rmGpuLockIsOwner())
    {
        // LOCK: acquire GPUs lock
        if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT)) != NV_OK)
        {
            NV_ASSERT(0);
            return status;
        }
        bReleaseLock = NV_TRUE;
    }

    pClient->bIsClientVirtualMode = (pSecInfo->pProcessToken != NULL);

    //
    // Cache the security/uid tokens only if the client handle validation is
    // enabled AND its a user mode path or a non privileged kernel class.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE) &&
       ((pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_KERNEL) || pClient->bIsRootNonPriv))
    {
        PSECURITY_TOKEN pSecurityToken;
        PUID_TOKEN pUidToken = osGetCurrentUidToken();
        UserInfo *pUserInfo = NULL;

        pSecurityToken  = (pClient->bIsClientVirtualMode ?
                           pSecInfo->pProcessToken : osGetSecurityToken());

        // pUserInfo takes ownership of pUidToken upon successful registration
        status = _registerUserInfo(&pUidToken, &pUserInfo);

        if (status == NV_OK)
        {
            pClient->pUserInfo = pUserInfo;
            pClient->pSecurityToken = pSecurityToken;
        }
        else
        {
            portMemFree(pUidToken);

            if (pSecurityToken != NULL && !pClient->bIsClientVirtualMode)
                portMemFree(pSecurityToken);
        }
    }

    if (listAppendValue(&g_clientListBehindGpusLock, (void*)&pClient) == NULL)
        status = NV_ERR_INSUFFICIENT_RESOURCES;

    if (bReleaseLock)
    {
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    // RM gets the client handle from the allocation parameters
    if (status == NV_OK && pParams->pAllocParams != NULL)
        *(NvHandle*)(pParams->pAllocParams) = pParams->hClient;

    return status;
}

void
rmclientDestruct_IMPL
(
    RmClient *pClient
)
{
    NV_STATUS           status = NV_OK;
    RsClient           *pRsClient = staticCast(pClient, RsClient);
    NV_STATUS           tmpStatus;
    NvHandle            hClient = pRsClient->hClient;
    NvBool              bReleaseLock = NV_FALSE;
    RS_ITERATOR         it;
    RM_API             *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_PRINTF(LEVEL_INFO, "    type: client\n");

    LOCK_METER_DATA(FREE_CLIENT, hClient, 0, 0);

    // Free any association of the client with existing third-party p2p object
    CliUnregisterFromThirdPartyP2P(pClient);

    rmapiControlCacheFreeClient(hClient);

    //
    // Free all of the devices of the client (do it in reverse order to
    // facilitate tear down of things like ctxdmas, etc)
    //
    it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
    while (clientRefIterNext(it.pClient, &it))
    {
        Device *pDeviceInfo = dynamicCast(it.pResourceRef->pResource, Device);

        // This path is deprecated.
        NV_ASSERT(0);

        tmpStatus = pRmApi->Free(pRmApi, hClient, RES_GET_HANDLE(pDeviceInfo));
        if ((tmpStatus != NV_OK) && (status == NV_OK))
            status = tmpStatus;

        // re-snap iterator as Device list was mutated
        it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
    }

    // Updating the client list just before client handle unregister //
    // in case child free functions need to iterate over all clients //
    if (!rmGpuLockIsOwner())
    {
        // LOCK: acquire GPUs lock
        if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT)) != NV_OK)
        {
            // This is the only chance that the shadow client list can be
            // updated so modify it regardless of whether or not we obtained the lock
            NV_ASSERT(0);
        }
        else
        {
            bReleaseLock = NV_TRUE;
        }
    }

    listRemoveFirstByValue(&g_clientListBehindGpusLock, (void*)&pClient);

    if (pClient->pUserInfo != NULL)
    {
        _unregisterUserInfo(pClient->pUserInfo);
        pClient->pUserInfo = NULL;
    }

    if (pClient->pSecurityToken != NULL)
    {
        if (!pClient->bIsClientVirtualMode)
            portMemFree(pClient->pSecurityToken);

        pClient->pSecurityToken = NULL;
    }

    if (bReleaseLock)
    {
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }
}

NV_STATUS
rmclientInterMap_IMPL
(
    RmClient *pClient,
    RsResourceRef *pMapperRef,
    RsResourceRef *pMappableRef,
    RS_INTER_MAP_PARAMS *pParams
)
{
    RS_INTER_MAP_PRIVATE *pPrivate = pParams->pPrivate;
    RS_RES_MAP_TO_PARAMS mapToParams;

    // Use virtual MapTo to perform the class-specific mapping to pMapperRef
    portMemSet(&mapToParams, 0, sizeof(mapToParams));

    mapToParams.pMemoryRef = pMappableRef;
    mapToParams.offset     = pParams->offset;
    mapToParams.length     = pParams->length;
    mapToParams.flags      = pParams->flags;
    mapToParams.pDmaOffset = &pParams->dmaOffset;
    mapToParams.ppMemDesc = (MEMORY_DESCRIPTOR**)&pParams->pMemDesc;

    mapToParams.pGpu             = pPrivate->pGpu;
    mapToParams.pSrcGpu          = pPrivate->pSrcGpu;
    mapToParams.pSrcMemDesc      = pPrivate->pSrcMemDesc;
    mapToParams.hBroadcastDevice = pPrivate->hBroadcastDevice;
    mapToParams.hMemoryDevice    = pPrivate->hMemoryDevice;
    mapToParams.gpuMask          = pPrivate->gpuMask;
    mapToParams.bSubdeviceHandleProvided = pPrivate->bSubdeviceHandleProvided;
    mapToParams.bDmaMapNeeded    = pPrivate->bDmaMapNeeded;
    mapToParams.bFlaMapping      = pPrivate->bFlaMapping;

    return resMapTo(pMapperRef->pResource, &mapToParams);
}

void
rmclientInterUnmap_IMPL
(
    RmClient *pClient,
    RsResourceRef *pMapperRef,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    RS_INTER_UNMAP_PRIVATE *pPrivate = pParams->pPrivate;
    RS_RES_UNMAP_FROM_PARAMS unmapFromParams;

    // Use virtual UnmapFrom to perform the class-specific unmapping from pMapperRef
    portMemSet(&unmapFromParams, 0, sizeof(unmapFromParams));

    unmapFromParams.pMemDesc  = pParams->pMemDesc;
    unmapFromParams.hMemory   = pParams->hMappable;
    unmapFromParams.flags     = pParams->flags;
    unmapFromParams.dmaOffset = pParams->dmaOffset;

    unmapFromParams.pGpu             = pPrivate->pGpu;
    unmapFromParams.hBroadcastDevice = pPrivate->hBroadcastDevice;
    unmapFromParams.gpuMask          = pPrivate->gpuMask;
    unmapFromParams.bSubdeviceHandleProvided = pPrivate->bSubdeviceHandleProvided;

    resUnmapFrom(pMapperRef->pResource, &unmapFromParams);
}

RS_PRIV_LEVEL
rmclientGetCachedPrivilege_IMPL
(
    RmClient *pClient
)
{
    return pClient->cachedPrivilege;
}

NvBool
rmclientIsAdmin_IMPL
(
    RmClient *pClient,
    RS_PRIV_LEVEL privLevel
)
{
    if (pClient == NULL)
        return NV_FALSE;

    return (privLevel >= RS_PRIV_LEVEL_USER_ROOT) && !pClient->bIsRootNonPriv;
}

void
rmclientSetClientFlags_IMPL
(
    RmClient *pClient,
    NvU32 clientFlags
)
{
    pClient->Flags |= clientFlags;
}

static void
_rmclientPromoteDebuggerState
(
   RmClient *pClient,
   NvU32     newMinimumState
)
{
    if (pClient->ClientDebuggerState < newMinimumState)
    {
        pClient->ClientDebuggerState = newMinimumState;
    }
}

void *
rmclientGetSecurityToken_IMPL
(
    RmClient *pClient
)
{
    return pClient->pSecurityToken;
}

/*!
 * @brief Given a client handle, validate the handle for security.
 *
 * Important!! This function should be called ONLY in the user mode paths.
 * The security validations will fail in kernel paths, especially if called
 * with privileged kernel handles.
 *
 * @param[in] hClient        The client handle
 * @param[in] pSecInfo       The new calling context's security info.
 *
 * @return                   NV_OK if validated
 *                           NV_ERR_INVALID_CLIENT  if client cannot be found
 *                           or if there isn't a match.
 */
static NV_STATUS
_rmclientUserClientSecurityCheck
(
    RmClient *pClient,
    const API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS        status = NV_OK;
    OBJSYS          *pSys = SYS_GET_INSTANCE();
    PSECURITY_TOKEN  pCurrentToken = NULL;
    PSECURITY_TOKEN  pSecurityToken = pSecInfo->pProcessToken;

    if ((pSys == NULL) ||
        (!pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE)))
    {
        return NV_OK;
    }

    //
    // Check 1:
    // The following check to make sure that user paths cannot be called with
    // privileged kernel handles
    //
    // Note: For the user paths, we are checking against both kernel and admin.
    // client The reason is that KMD today creates unprivileged kernel handles
    // (of class NV01_ROOT_NON_PRIV) on behalf of user clients (cuda debugger,
    // profiler, OGL etc) and gives out those handles. These handles are
    // kernel, but they do not have admin privileges and since clients already
    // use these handles to call into RM through the user paths, we are allowing
    // them through ... for now.
    //
    // Till we either fix the clients to wean off these kernel handles or change
    // KMD to not give out the kernel handles, we need to keep the check restricted
    // to handles created with NV01_ROOT using the the CliCheckAdmin interface.
    //
    if ((pSecInfo->privLevel >= RS_PRIV_LEVEL_KERNEL) && !pClient->bIsRootNonPriv)
    {
        NV_PRINTF(LEVEL_WARNING, "Incorrect client handle used in the User export\n");
        return NV_ERR_INVALID_CLIENT;
    }

    //
    // Check 2:
    // Validate the client handle to make sure that the user who created the
    // handle is the one that uses it. Otherwise a malicious user can guess the
    // client handle created by another user and access information that its
    // not privy to.
    //
    pCurrentToken = (pSecurityToken != NULL ? pSecurityToken : osGetSecurityToken());
    if (pCurrentToken == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Cannot get the security token for the current user.\n");
        NV_PRINTF(LEVEL_WARNING,
                  "The user client cannot be validated\n");
        status = NV_ERR_INVALID_CLIENT;
        DBG_BREAKPOINT();
        goto CliUserClientSecurityCheck_exit;
    }

    status = osValidateClientTokens((void*)rmclientGetSecurityToken(pClient),
                                    (void*)pCurrentToken);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Error validating client token. Status = 0x%08x\n", status);
        goto CliUserClientSecurityCheck_exit;
    }

CliUserClientSecurityCheck_exit:
    if (pCurrentToken != NULL && pSecurityToken == NULL)
    {
        portMemFree(pCurrentToken);
        pCurrentToken = NULL;
    }
    return status;
}

NV_STATUS
rmclientPostProcessPendingFreeList_IMPL
(
    RmClient *pClient,
    RsResourceRef **ppFirstLowPriRef
)
{
    RsClient *pRsClient = staticCast(pClient, RsClient);
    RsResourceRef *pTargetRef = NULL;
    RsResourceRef *pStopRef = NULL;
    RsResourceRef *pFirstLowPriRef = NULL;

    pStopRef = pRsClient->pFreeStack->pResourceRef;
    pTargetRef = listHead(&pRsClient->pendingFreeList);
    while (pTargetRef != pStopRef)
    {
        RsResourceRef *pNextRef = listNext(&pRsClient->pendingFreeList, pTargetRef);

        // Ensure that high priority resources (and their children/dependents) are freed first
        if (pTargetRef->pResourceDesc->freePriority == RS_FREE_PRIORITY_HIGH)
        {
            clientUpdatePendingFreeList(pRsClient, pTargetRef, pTargetRef, NV_TRUE);
        }
        pTargetRef = pNextRef;
    }

    //
    // Find the last high-priority resource in the list.
    // The next resource will be the first low priority resource.
    // If there are no high-priority resources: use the head of the list
    //
    pTargetRef = (pStopRef != NULL)
               ? pStopRef
               : listTail(&pRsClient->pendingFreeList);
    pFirstLowPriRef = listHead(&pRsClient->pendingFreeList);

    while (pTargetRef != NULL)
    {
        RsResourceRef *pPrevRef = listPrev(&pRsClient->pendingFreeList, pTargetRef);

        if (pTargetRef->pResourceDesc->freePriority == RS_FREE_PRIORITY_HIGH)
        {
            pFirstLowPriRef = listNext(&pRsClient->pendingFreeList, pTargetRef);
            break;
        }
        pTargetRef = pPrevRef;
    }

    if (ppFirstLowPriRef)
        *ppFirstLowPriRef = pFirstLowPriRef;

    return NV_OK;
}

static RmClient *handleToObject(NvHandle hClient)
{
    RmClient *pClient;
    return (NV_OK == serverutilGetClientUnderLock(hClient, &pClient)) ? pClient : NULL;
}

RS_PRIV_LEVEL rmclientGetCachedPrivilegeByHandle(NvHandle hClient)
{
    RmClient *pClient = handleToObject(hClient);
    return pClient ? rmclientGetCachedPrivilege(pClient) : RS_PRIV_LEVEL_USER;
}

NvBool rmclientIsAdminByHandle(NvHandle hClient, RS_PRIV_LEVEL privLevel)
{
    RmClient *pClient = handleToObject(hClient);
    return pClient ? rmclientIsAdmin(pClient, privLevel) : NV_FALSE;
}

NvBool rmclientSetClientFlagsByHandle(NvHandle hClient, NvU32 clientFlags)
{
    RmClient *pClient = handleToObject(hClient);
    if (pClient)
        rmclientSetClientFlags(pClient, clientFlags);
    return !!pClient;
}

void rmclientPromoteDebuggerStateByHandle(NvHandle hClient, NvU32 newMinimumState)
{
    RmClient *pClient = handleToObject(hClient);
    if (pClient)
        _rmclientPromoteDebuggerState(pClient, newMinimumState);
}

void *rmclientGetSecurityTokenByHandle(NvHandle hClient)
{
    RmClient *pClient = handleToObject(hClient);
    return pClient ? rmclientGetSecurityToken(pClient) : NULL;
}

NV_STATUS rmclientUserClientSecurityCheckByHandle(NvHandle hClient, const API_SECURITY_INFO *pSecInfo)
{
    RmClient *pClient = handleToObject(hClient);

    //
    // Return early if it's a null object. This is probably the allocation of
    // the root client object, so the client class is going to be null.
    //
    // RS-TODO - This check should move to the caller.
    //
    if (hClient == NV01_NULL_OBJECT)
    {
        return NV_OK;
    }

    if (pClient)
    {
        return _rmclientUserClientSecurityCheck(pClient, pSecInfo);
    }
    else
        return NV_ERR_INVALID_CLIENT;
}

/**
 * Register a uid token with the client database and return a UserInfo that
 * corresponds to the uid token.
 *
 * If the uid token has not been registered before, a new UserInfo will be registered and returned.
 * If the uid token is already registered, an existing UserInfo will be ref-counted and
 * returned.
 *
 * This function must be protected by a lock (currently the GPUs lock.)
 *
 * @param[inout] ppUidToken
 * @param[out] ppUserInfo
 */
NV_STATUS
_registerUserInfo
(
    PUID_TOKEN *ppUidToken,
    UserInfo **ppUserInfo
)
{
    NV_STATUS status = NV_OK;
    NvBool bFound = NV_FALSE;
    UserInfo *pUserInfo = NULL;
    UserInfoListIter it = listIterAll(&g_userInfoList);
    PUID_TOKEN pUidToken;

    if ((!ppUidToken) || (!(*ppUidToken)))
        return NV_ERR_INVALID_ARGUMENT;

    pUidToken = *ppUidToken;

    // Find matching user token
    while(listIterNext(&it))
    {
        pUserInfo = *it.pValue;
        if (osUidTokensEqual(pUserInfo->pUidToken, pUidToken))
        {
            bFound = NV_TRUE;
            break;
        }
    }

    if (!bFound)
    {
        RsShared *pShared;
        status = serverAllocShare(&g_resServ, classInfo(UserInfo), &pShared);
        if (status != NV_OK)
            return status;

        pUserInfo = dynamicCast(pShared, UserInfo);
        pUserInfo->pUidToken = pUidToken;

        if (listAppendValue(&g_userInfoList, (void*)&pUserInfo) == NULL)
        {
            serverFreeShare(&g_resServ, pShared);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        serverRefShare(&g_resServ, staticCast(pUserInfo, RsShared));
        portMemFree(pUidToken);
        *ppUidToken = NULL;
    }

    *ppUserInfo = pUserInfo;

    return NV_OK;
}

/**
 *
 * Unregister a client from a user info list
 *
 * This function must be protected by a lock (currently the GPUs lock.)
 *
 * @param[in] pUserInfo
 */
NV_STATUS
_unregisterUserInfo
(
    UserInfo *pUserInfo
)
{
    NvS32 refCount = serverGetShareRefCount(&g_resServ, staticCast(pUserInfo, RsShared));
    if (--refCount == 0)
    {
        listRemoveFirstByValue(&g_userInfoList, (void*)&pUserInfo);
    }
    return serverFreeShare(&g_resServ, staticCast(pUserInfo, RsShared));
}

NV_STATUS userinfoConstruct_IMPL
(
    UserInfo *pUserInfo
)
{
    return NV_OK;
}

void
userinfoDestruct_IMPL
(
    UserInfo *pUserInfo
)
{
    portMemFree(pUserInfo->pUidToken);
}

NV_STATUS
rmclientValidate_IMPL
(
    RmClient *pClient,
    const API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status = NV_OK;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE) &&
        pSecInfo != NULL)
    {
        if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT) &&
            pSecInfo->clientOSInfo != NULL)
        {
            if (pClient->pOSInfo != pSecInfo->clientOSInfo)
            {
                status = NV_ERR_INVALID_CLIENT;
            }
        }
        else if (pSecInfo->privLevel < RS_PRIV_LEVEL_KERNEL)
        {
            status = _rmclientUserClientSecurityCheck(pClient, pSecInfo);
        }
    }

    return status;
}

NV_STATUS
rmclientFreeResource_IMPL
(
    RmClient *pClient,
    RsServer *pServer,
    RS_RES_FREE_PARAMS_INTERNAL *pRmFreeParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu;
    NvBool bBcState;
    NvBool bRestoreBcState = NV_FALSE;

    if (gpuGetByRef(pRmFreeParams->pResourceRef, NULL, &pGpu) == NV_OK)
    {
        bBcState = gpumgrGetBcEnabledStatus(pGpu);
        bRestoreBcState = NV_TRUE;
    }

    rmapiFreeResourcePrologue(pRmFreeParams);

    status = clientFreeResource_IMPL(staticCast(pClient, RsClient), pServer, pRmFreeParams);

    if (bRestoreBcState)
    {
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }
    return status;
}

static NvBool _rmclientIsCapable
(
    NvHandle hClient,
    NvU32 capability
)
{
    NvU32 internalClassId;
    RsResourceRef *pResourceRef = NULL;

    switch(capability)
    {
        case NV_RM_CAP_SYS_SMC_CONFIG:
        {
            internalClassId = classId(MIGConfigSession);
            break;
        }
        case NV_RM_CAP_EXT_FABRIC_MGMT:
        {
            internalClassId = classId(FmSessionApi);
            break;
        }
        case NV_RM_CAP_SYS_SMC_MONITOR:
        {
            internalClassId = classId(MIGMonitorSession);
            break;
        }
        default:
        {
            NV_ASSERT(0);
            return NV_FALSE;
        }
    }

    // Check if client has allocated a given class
    pResourceRef = serverutilFindChildRefByType(hClient, hClient, internalClassId, NV_TRUE);
    if (pResourceRef == NULL)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvBool rmclientIsCapableOrAdmin_IMPL
(
    RmClient *pClient,
    NvU32 capability,
    RS_PRIV_LEVEL privLevel
)
{
    RsClient *pRsClient = staticCast(pClient, RsClient);
    NvHandle  hClient = pRsClient->hClient;

    if (rmclientIsAdmin(pClient, privLevel))
    {
        return NV_TRUE;
    }

    return _rmclientIsCapable(hClient, capability);
}

//
// RS-TODO: Delete this function once the RM Capabilities framework is in place.
// JIRA GR-139
//
NvBool rmclientIsCapableOrAdminByHandle
(
    NvHandle hClient,
    NvU32 capability,
    RS_PRIV_LEVEL privLevel
)
{
    RmClient *pClient = handleToObject(hClient);
    if (pClient == NULL)
    {
        return NV_FALSE;
    }

    return rmclientIsCapableOrAdmin(pClient, capability, privLevel);
}

NvBool rmclientIsCapable_IMPL
(
    RmClient *pClient,
    NvU32 capability
)
{
    RsClient *pRsClient = staticCast(pClient, RsClient);
    NvHandle  hClient = pRsClient->hClient;

    return _rmclientIsCapable(hClient, capability);
}

//
// RS-TODO: Delete this function once the RM Capabilities framework is in place.
// JIRA GR-139
//
NvBool rmclientIsCapableByHandle
(
    NvHandle hClient,
    NvU32 capability
)
{
    RmClient *pClient = handleToObject(hClient);
    if (pClient == NULL)
    {
        return NV_FALSE;
    }

    return rmclientIsCapable(pClient, capability);
}
