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

#include "os/os.h"

#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "rmapi/client.h"
#include "rmapi/client_resource.h"
#include "rmapi/resource_fwd_decls.h"
#include "core/locks.h"
#include "core/system.h"
#include "resource_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"

#include "gpu/bus/third_party_p2p.h"
#include "virtualization/hypervisor/hypervisor.h"

OsInfoMap g_osInfoList;
UserInfoList g_userInfoList;
RmClientList g_clientListBehindGpusLock; // RS-TODO remove this WAR

#define RS_FW_UNIQUE_HANDLE_BASE  (0xc9f00000)

static NV_STATUS _registerUserInfo(PUID_TOKEN *ppUidToken, UserInfo **ppUserInfo);
static NV_STATUS _unregisterUserInfo(UserInfo *pUserInfo);
static NV_STATUS _registerOSInfo(RmClient *pClient, void *pOSInfo);
static NV_STATUS _unregisterOSInfo(RmClient *pClient, void *pOSInfo);

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
    OBJGPU            *pGpu = NULL;

    //
    // RM client objects can only be created/destroyed with the RW API lock.
    // Bug 4193761 - allow internal clients to be created with the GPU lock,
    // GR-2409 will remove the possible race condition with the client list.
    //
    LOCK_ASSERT_AND_RETURN(rmapiLockIsWriteOwner() ||
        (serverIsClientInternal(&g_resServ, pRsClient->hClient) && rmGpuLockIsOwner()));

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        pGpu = gpumgrGetSomeGpu();

        if (pGpu == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU is not found\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    pClient->bIsRootNonPriv  = (pParams->externalClassId == NV01_ROOT_NON_PRIV);
    pClient->pUserInfo       = NULL;
    pClient->pSecurityToken  = NULL;
    pClient->pOSInfo         = pSecInfo->clientOSInfo;
    pClient->imexChannel     = -1;

    pClient->cachedPrivilege = pSecInfo->privLevel;

    // TODO: Revisit in M2, see GPUSWSEC-1176
    if (RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        if (pSecInfo->pProcessToken != NULL &&
            ((NvU64) pSecInfo->pProcessToken) < VMMU_MAX_GFID)
        {
            // Trunc to NvU32 to fit ProcID (VMMU_MAX_GFID << MAX_INT)
            pClient->ProcID = (NvU32)((NvU64)pSecInfo->pProcessToken);

            NV_PRINTF(LEVEL_INFO, "Client allocation with GFID = %u\n",
                     (NvU32)((NvU64)pSecInfo->pProcessToken));
        }
    }
    else
    {
        pClient->ProcID = osGetCurrentProcess();
        if (pClient->cachedPrivilege <= RS_PRIV_LEVEL_USER_ROOT)
            pClient->pOsPidInfo = osGetPidInfo();
    }

    // Set user-friendly client name from current process
    osGetCurrentProcessName(pClient->name, NV_PROC_NAME_MAX_LENGTH);

    for (i = 0; i < NV0000_NOTIFIERS_MAXCOUNT; i++)
    {
        pClient->CliSysEventInfo.notifyActions[i] =
            NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    //
    // Enabling this on MODS to avoid clash of client handles. This path gets executed on both
    // guest & host RM for MODs platform, pGPU handle isnt available here to check for IS_VIRTUAL.
    // Later code paths will override this for guest RM.
    // This change affects non-SRIOV case as well, there is no good way to detect SRIOV without pGPU.
    //
    if (hypervisorIsVgxHyper() || NV_IS_MODS)
    {
        //
        // Set RM allocated resource handle range for host RM. This minimize clash of guest RM handles with host RM
        // during VM migration.
        //
        status = clientSetHandleGenerator(pRsClient,
                                          (RS_UNIQUE_HANDLE_BASE + RS_UNIQUE_HANDLE_RANGE/2),
                                          RS_UNIQUE_HANDLE_RANGE/2);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "NVRM_RPC: Failed to set host client resource handle range %x\n", status);
            goto out;
        }
    }

    // Prevent kernel clients from requesting handles in the FW handle generator range
    status = clientSetRestrictedRange(pRsClient,
                                      RS_FW_UNIQUE_HANDLE_BASE, RS_UNIQUE_HANDLE_RANGE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Failed to set host client restricted resource handle range. Status=%x\n", status);
        goto out;
    }

    if (!rmGpuLockIsOwner())
    {
        // LOCK: acquire GPUs lock
        if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT)) != NV_OK)
        {
            NV_ASSERT(0);
            goto out;
        }
        bReleaseLock = NV_TRUE;
    }

    _registerOSInfo(pClient, pClient->pOSInfo);

    pClient->bIsClientVirtualMode = (pSecInfo->pProcessToken != NULL);

    //
    // Cache the security/uid tokens only if the client handle validation is
    // enabled AND its a user mode path or a non privileged kernel class.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE) &&
       ((pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_KERNEL) || pClient->bIsRootNonPriv))
    {
        PSECURITY_TOKEN pSecurityToken = (pClient->bIsClientVirtualMode ?
                                          pSecInfo->pProcessToken : osGetSecurityToken());
        PUID_TOKEN pUidToken = osGetCurrentUidToken();
        UserInfo *pUserInfo = NULL;

        if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            pClient->pSecurityToken = pSecurityToken;
        }
        else
        {
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

    NV_PRINTF(LEVEL_INFO, "New RM Client: hClient=0x%08x (%c), ProcID=%u, name='%s'\n",
        pRsClient->hClient, (pRsClient->type == CLIENT_TYPE_USER) ? 'U' : 'K', pClient->ProcID, pClient->name);

out:
    if (status != NV_OK)
    {
        osPutPidInfo(pClient->pOsPidInfo);
        pClient->pOsPidInfo = NULL;
    }

    return status;
}

void
rmclientDestruct_IMPL
(
    RmClient *pClient
)
{
    NV_STATUS           status = NV_OK;
    NvBool              bReleaseLock = NV_FALSE;

    //
    // RM client objects can only be created/destroyed with the RW API lock.
    // Bug 4193761 - allow internal clients to be created with the GPU lock,
    // GR-2409 will remove the possible race condition with the client list.
    //
    NV_ASSERT_OR_ELSE(rmapiLockIsWriteOwner() ||
        (serverIsClientInternal(&g_resServ, staticCast(pClient, RsClient)->hClient) &&
         rmGpuLockIsOwner()),
        return);

    NV_PRINTF(LEVEL_INFO, "    type: client\n");

    LOCK_METER_DATA(FREE_CLIENT, hClient, 0, 0);

    // Free any association of the client with existing third-party p2p object
    CliUnregisterFromThirdPartyP2P(pClient);

    osPutPidInfo(pClient->pOsPidInfo);

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

    _unregisterOSInfo(pClient, pClient->pOSInfo);

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
    mapToParams.bFlaMapping      = pPrivate->bFlaMapping;

    return resMapTo(pMapperRef->pResource, &mapToParams);
}

NV_STATUS
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
    unmapFromParams.size      = pParams->size;

    unmapFromParams.pGpu             = pPrivate->pGpu;
    unmapFromParams.hBroadcastDevice = pPrivate->hBroadcastDevice;
    unmapFromParams.gpuMask          = pPrivate->gpuMask;
    unmapFromParams.bSubdeviceHandleProvided = pPrivate->bSubdeviceHandleProvided;

    return resUnmapFrom(pMapperRef->pResource, &unmapFromParams);
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

RS_PRIV_LEVEL rmclientGetCachedPrivilegeByHandle(NvHandle hClient)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    return pClient ? rmclientGetCachedPrivilege(pClient) : RS_PRIV_LEVEL_USER;
}

NvBool rmclientIsAdminByHandle(NvHandle hClient, RS_PRIV_LEVEL privLevel)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    return pClient ? rmclientIsAdmin(pClient, privLevel) : NV_FALSE;
}

static inline NvBool rmclientIsKernelOnly(RmClient *pClient)
{
    return (pClient->pSecurityToken == NULL);
}

NvBool rmclientIsKernelOnlyByHandle(NvHandle hClient)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    return (pClient ? rmclientIsKernelOnly(pClient) : NV_FALSE);
}

NvBool rmclientSetClientFlagsByHandle(NvHandle hClient, NvU32 clientFlags)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    if (pClient)
        rmclientSetClientFlags(pClient, clientFlags);
    return !!pClient;
}

void rmclientPromoteDebuggerStateByHandle(NvHandle hClient, NvU32 newMinimumState)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    if (pClient)
        _rmclientPromoteDebuggerState(pClient, newMinimumState);
}

void *rmclientGetSecurityTokenByHandle(NvHandle hClient)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    return pClient ? rmclientGetSecurityToken(pClient) : NULL;
}

NV_STATUS rmclientUserClientSecurityCheckByHandle(NvHandle hClient, const API_SECURITY_INFO *pSecInfo)
{
    RmClient *pClient = serverutilGetClientUnderLock(hClient);

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
static NV_STATUS
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
static NV_STATUS
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
rmclientValidateLocks_IMPL
(
    RmClient *pClient,
    RsServer *pServer,
    const CLIENT_ENTRY *pClientEntry
)
{
    // Possessing the client lock means it's always safe to use this client object
    if (pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId())
        return NV_OK;

    //
    // Without the client lock, the API lock in write mode guarantees safety for the
    // client object since nothing else can execute in parallel when holding it.
    //
    if (rmapiLockIsWriteOwner())
        return NV_OK;

    //
    // Without the client lock, the API lock in read mode guarantees safety for the
    // client object IF it's a client that cannot be used directly by user space (i.e.
    // kernel privileged client and/or internal client).
    //
    if (rmapiLockIsOwner() &&
        (rmclientIsKernelOnly(pClient) ||
         serverIsClientInternal(pServer, pClientEntry->hClient)))
    {
        return NV_OK;
    }


    NV_ASSERT(0);
    // Otherwise we don't have the required locks to use this RM client
    return NV_ERR_INVALID_LOCK_STATE;
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
    RsClient *pRsClient = staticCast(pClient, RsClient);

    if (gpuGetByRef(pRmFreeParams->pResourceRef, NULL, &pGpu) == NV_OK)
    {
        bBcState = gpumgrGetBcEnabledStatus(pGpu);
        bRestoreBcState = NV_TRUE;
    }

    rmapiFreeResourcePrologue(pRmFreeParams);

    //
    // In the RTD3 case, the API lock isn't taken since it can be initiated
    // from another thread that holds the API lock and because we now hold
    // the GPU lock.
    //
    if (rmapiInRtd3PmPath())
    {
        pRmFreeParams->pLockInfo->flags |= RM_LOCK_FLAGS_NO_API_LOCK;
    }

    status = clientFreeResource_IMPL(pRsClient, pServer, pRmFreeParams);

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
        case NV_RM_CAP_SYS_FABRIC_IMEX_MGMT:
        {
            internalClassId = classId(ImexSessionApi);
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
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
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
    RmClient *pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
    {
        return NV_FALSE;
    }

    return rmclientIsCapable(pClient, capability);
}

/**
 *
 * Register a client with a user info list
 *
 * This function must be protected by a lock (currently the GPUs lock.)
 *
 * @param[in] pClient
 * @param[in] pOSInfo
 */
static NV_STATUS
_registerOSInfo
(
   RmClient *pClient,
   void *pOSInfo
)
{
    OsInfoMapSubmap *pSubmap = NULL;
    RmClient **pInsert = NULL;
    NvU64 key1 = (NvUPtr)pOSInfo;
    NvU64 key2 = (NvU64)(staticCast(pClient,RsClient))->hClient;

    if (multimapFindItem(&g_osInfoList, key1, key2) != NULL)
        return NV_ERR_INSERT_DUPLICATE_NAME;

    if (multimapFindSubmap(&g_osInfoList, key1) == NULL)
    {
        pSubmap = multimapInsertSubmap(&g_osInfoList, key1);
        if (pSubmap == NULL)
            return NV_ERR_NO_MEMORY;
    }

    pInsert = multimapInsertItemNew(&g_osInfoList, key1, key2);
    if (pInsert == NULL)
        return NV_ERR_NO_MEMORY;

    osAllocatedRmClient(pOSInfo);

    *pInsert = pClient;

    return NV_OK;
}

/**
 *
 * Unregister a client from a user info list
 *
 * This function must be protected by a lock (currently the GPUs lock.)
 *
 * @param[in] pClient
 * @param[in] pOSInfo
 */
static NV_STATUS
_unregisterOSInfo
(
    RmClient *pClient,
    void *pOSInfo
)
{
     NvU64 key1 = (NvUPtr)pOSInfo;
     NvU64 key2 = (NvU64)(staticCast(pClient, RsClient))->hClient;
     OsInfoMapSubmap *pSubmap = NULL;
     RmClient **pFind = NULL;

     pFind = multimapFindItem(&g_osInfoList, key1, key2);
     if (pFind != NULL)
         multimapRemoveItem(&g_osInfoList, pFind);

     pSubmap = multimapFindSubmap(&g_osInfoList, key1);
     if (pSubmap == NULL || multimapCountSubmapItems(&g_osInfoList, pSubmap) > 0)
         return NV_OK;

     multimapRemoveSubmap(&g_osInfoList, pSubmap);

     return NV_OK;
}
