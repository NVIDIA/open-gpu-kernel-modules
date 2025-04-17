/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvlog_inc.h"
#include "resserv/resserv.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"

#if !(RS_STANDALONE)
#include "os/os.h"
#include "resserv/rs_access_map.h"
#endif

typedef enum
{
    ALLOC_NEW_RESOURCE,
    ALLOC_SHARED_RESOURCE
} ALLOC_TYPE;

/**
 * Allocate a new or shared resource in RM for this client
 * @param[in]       pClient This client
 * @param[in]       pServer The resource server instance
 * @param[in]       pParams Parameters for the resource allocation
 * @param[in,out]   phResource Server will assign a handle if it is 0
 */
static NV_STATUS _clientAllocResourceHelper(RsClient *pClient, RsServer *pServer,
                                            RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
                                            NvHandle *phResource);

/**
 * Add a resource reference to the client's resource hashmap
 * @param[in] pClient This client
 * @param[in] pServer The resource server that owns the resource ref
 * @param[in] pParentRef   The resource's parent reference
 * @param[in] hResource The resource's handle
 * @param[in] classId   The resource's class
 * @param[out] ppResourceRef The new resource reference
 */
static NV_STATUS _clientConstructResourceRef(RsClient *pClient, RsServer *pServer, RsResourceRef *pParentRef,
                                             NvHandle hResource, NvU32 classId, RsResourceRef **ppResourceRef);

/**
 * Release all CPU address mappings that reference this resource
 *
 * @param[in] pClient Client that owns the resource
 * @param[in] pCallContext Caller information (which includes the resource reference
 *                         whose mapping back references will be freed)
 * @param[in] pLockInfo Information about which locks are already held, for recursive calls
 */
static NV_STATUS _clientUnmapBackRefMappings(RsClient *pClient, CALL_CONTEXT *pCallContext, RS_LOCK_INFO *pLockInfo);

static void _clientUnmapInterMappings(RsClient *pClient, CALL_CONTEXT *pCallContext, RS_LOCK_INFO *pLockInfo);
static void _clientUnmapInterBackRefMappings(RsClient *pClient, CALL_CONTEXT *pCallContext, RS_LOCK_INFO *pLockInfo);

NV_STATUS
clientConstruct_IMPL
(
    RsClient *pClient,
    PORT_MEM_ALLOCATOR *pAllocator,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status;
    CLIENT_TYPE type;

    if (pParams->pSecInfo->privLevel >= RS_PRIV_LEVEL_KERNEL)
        type = CLIENT_TYPE_KERNEL;
    else
        type = CLIENT_TYPE_USER;

    pClient->type = type;
    pClient->hClient = pParams->hClient;

    mapInit(&pClient->resourceMap, pAllocator);
    listInitIntrusive(&pClient->pendingFreeList);

    listInit(&pClient->accessBackRefList, pAllocator);

    pClient->handleGenIdx = 0;
    status = clientSetHandleGenerator(pClient, 0, 0);
    if (status != NV_OK)
        return status;

    pClient->bActive = NV_TRUE;

    status = clientSetRestrictedRange(pClient, 0, 0);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

NV_STATUS
clientSetHandleGenerator_IMPL
(
    RsClient *pClient,
    NvHandle handleRangeStart,
    NvHandle handleRangeSize
)
{
    //
    // on vGPU, when client uses RM allocated handles, post allocation of rmclient NV01_ROOT,
    // NV01_DEVICE_0 is allocated which increment the handleGenIdx to 0x1.
    // In order to avoid the handle clash, we split the default RM handle ranges between Guest RM
    // (0xcaf00000, 0xcaf3ffff) and host RM (0xcaf40000, 0xcaf80000).
    // Hence, we should take this overriding into consideration when the ranges over the default
    // RM handle ranges.
    //
    NvBool bShrinkUnusedRange = ((pClient->handleRangeStart == handleRangeStart) &&
                                 (pClient->handleGenIdx <= handleRangeSize));

    if (!((pClient->handleGenIdx == 0) || bShrinkUnusedRange))
    {
        return NV_ERR_INVALID_STATE;
    }

    if ((handleRangeStart == 0) && (handleRangeSize == 0))
    {
        pClient->handleRangeStart = RS_UNIQUE_HANDLE_BASE;
        pClient->handleRangeSize = RS_UNIQUE_HANDLE_RANGE;
    }
    else if ((handleRangeStart != 0) && (handleRangeSize != 0))
    {
        pClient->handleRangeStart = handleRangeStart;
        pClient->handleRangeSize = handleRangeSize;
    }
    else
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    return NV_OK;
}

NV_STATUS clientCanShareResource_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef,
    RS_SHARE_POLICY *pSharePolicy,
    CALL_CONTEXT *pCallContext
)
{
    NV_STATUS status = NV_OK;

    RS_ACCESS_MASK rightsNeeded;
    RS_ACCESS_MASK *pRightsHeld;

    //
    // If sharing, check that the client has the rights it is trying to share
    // Revoking does not require this to allow revoking all rights without checking
    //
    if (!(pSharePolicy->action & RS_SHARE_ACTION_FLAG_REVOKE))
    {
        status = rsAccessCheckRights(pResourceRef, pClient, &pSharePolicy->accessMask);
        if (status == NV_ERR_INSUFFICIENT_PERMISSIONS)
        {
            // Attempt to grant rights which aren't already owned
            portMemCopy(&rightsNeeded, sizeof(rightsNeeded),
                        &pSharePolicy->accessMask, sizeof(pSharePolicy->accessMask));

            pRightsHeld = rsAccessLookup(pResourceRef, pClient);
            if (pRightsHeld != NULL)
            {
                // Skip trying to grant rights already held
                RS_ACCESS_MASK_SUBTRACT(&rightsNeeded, pRightsHeld);
            }

            status = rsAccessGrantRights(pResourceRef, pCallContext, pClient,
                                         &rightsNeeded,    // pRightsRequested
                                         NULL,             // pRightsRequired
                                         NULL);            // pAllocParams
        }
    }

    return status;
}

NV_STATUS
clientShareResource_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef,
    RS_SHARE_POLICY *pSharePolicy,
    CALL_CONTEXT *pCallContext
)
{
    RsServer *pServer = NULL;
    RsShareList *pActiveList;
    NV_STATUS status;

    status = clientCanShareResource(pClient, pResourceRef, pSharePolicy, pCallContext);
    if (status != NV_OK)
        return status;

    if (!pResourceRef->bSharePolicyListModified)
    {
        if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_COMPOSE)
        {
            if (pCallContext != NULL)
                pServer = pCallContext->pServer;

            pActiveList = rsAccessGetActiveShareList(pResourceRef, pServer);
            status = rsShareListCopy(&pResourceRef->sharePolicyList, pActiveList);
            if (status != NV_OK)
                return status;
        }

        pResourceRef->bSharePolicyListModified = NV_TRUE;
    }

    if (!(pSharePolicy->action & RS_SHARE_ACTION_FLAG_COMPOSE))
    {
        listClear(&pResourceRef->sharePolicyList);
    }

    if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REVOKE)
    {
        rsShareListRemove(&pResourceRef->sharePolicyList, pSharePolicy, NULL);
    }
    else
    {
        status = rsShareListInsert(&pResourceRef->sharePolicyList, pSharePolicy, NULL);
    }

    return status;
}

NV_STATUS
clientShareResourceTargetClient_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef,
    RS_SHARE_POLICY *pSharePolicy,
    CALL_CONTEXT *pCallContext
)
{
    NV_STATUS       status;
    RS_ACCESS_MASK *pCurrentRights;

    // Special case: This should only be called when share policy is for own client
    NV_ASSERT(pSharePolicy->type == RS_SHARE_TYPE_CLIENT);
    NV_ASSERT(pSharePolicy->target == pClient->hClient);

    status = clientCanShareResource(pClient, pResourceRef, pSharePolicy, pCallContext);
    if (status != NV_OK)
        return status;

    pCurrentRights = rsAccessLookup(pResourceRef, pClient);

    if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REVOKE)
    {
        RS_ACCESS_MASK_SUBTRACT(pCurrentRights, &pSharePolicy->accessMask);
    }
    else
    {
        RS_ACCESS_MASK_UNION(pCurrentRights, &pSharePolicy->accessMask);
    }

    return NV_OK;
}

NV_STATUS
clientSetRestrictedRange_IMPL
(
    RsClient *pClient,
    NvHandle handleRangeStart,
    NvU32 handleRangeSize
)
{
    NvHandle hFirst = handleRangeStart;
    NvHandle hLast;

    // Only allow modification if we haven't generated any handles
    if (pClient->handleGenIdx != 0)
        return NV_ERR_INVALID_STATE;

    if (handleRangeSize == 0)
    {
        if (handleRangeStart != 0)
            return NV_ERR_INVALID_PARAMETER;

        pClient->handleRestrictRange = NV_RANGE_EMPTY;
        return NV_OK;
    }

    // Wrapping-around the reserved range is not supported
    if (!portSafeAddU32(hFirst, handleRangeSize-1, &hLast))
        return NV_ERR_INVALID_REQUEST;

    pClient->handleRestrictRange = rangeMake(hFirst, hLast);

    return NV_OK;
}

void clientDestruct_IMPL
(
    RsClient *pClient
)
{
    NV_ASSERT(mapCount(&pClient->resourceMap) == 0);
    mapDestroy(&pClient->resourceMap);

    NV_ASSERT(listCount(&pClient->accessBackRefList) == 0);
    listDestroy(&pClient->accessBackRefList);
}

NV_STATUS
clientGetResource_IMPL
(
    RsClient *pClient,
    NvHandle hResource,
    NvU32 internalClassId,
    RsResource **ppResource
)
{
    NV_STATUS      status = NV_OK;
    RsResourceRef *pResourceRef;
    RsResource    *pResource;

    pResourceRef = mapFind(&pClient->resourceMap, hResource);
    if (pResourceRef == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        pResource = NULL;
        goto done;
    }

    if (pResourceRef->internalClassId != internalClassId)
    {
        status = NV_ERR_INVALID_CLASS;
        pResource = NULL;
        goto done;
    }

    pResource = pResourceRef->pResource;

done:
    if (ppResource != NULL)
        *ppResource = pResource;

    return status;
}

NV_STATUS
clientGetResourceByRef_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef,
    RsResource **ppResource
)
{
    if (ppResource != NULL)
        *ppResource = pResourceRef->pResource;

    return NV_OK;
}

NV_STATUS
clientGetResourceRef_IMPL
(
    RsClient *pClient,
    NvHandle hResource,
    RsResourceRef **ppResourceRef
)
{
    RsResourceRef *pResourceRef;

    pResourceRef = mapFind(&pClient->resourceMap, hResource);
    if (pResourceRef == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (ppResourceRef != NULL)
        *ppResourceRef = pResourceRef;

    return NV_OK;
}

NV_STATUS
clientGetResourceRefWithAccess_IMPL
(
    RsClient *pClient,
    NvHandle hResource,
    const RS_ACCESS_MASK *pRightsRequired,
    RsResourceRef **ppResourceRef
)
{
    NV_STATUS status;
    RsResourceRef *pResourceRef;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        return status;

    status = rsAccessCheckRights(pResourceRef, pClient, pRightsRequired);
    if (status != NV_OK)
        return status;

    if (ppResourceRef != NULL)
        *ppResourceRef = pResourceRef;

    return NV_OK;
}

NV_STATUS
clientGetResourceRefByType_IMPL
(
    RsClient *pClient,
    NvHandle hResource,
    NvU32 internalClassId,
    RsResourceRef **ppResourceRef
)
{
    NV_STATUS status;
    RsResourceRef *pResourceRef;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        return status;

    if (pResourceRef->internalClassId != internalClassId)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    if (ppResourceRef != NULL)
        *ppResourceRef = pResourceRef;

    return NV_OK;
}

NV_STATUS
clientValidate_IMPL
(
    RsClient                *pClient,
    const API_SECURITY_INFO *pSecInfo
)
{
    return NV_OK;
}

NV_STATUS
clientValidateLocks_IMPL
(
    RsClient           *pClient,
    RsServer           *pServer,
    const CLIENT_ENTRY *pClientEntry
)
{
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
        pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId(),
        NV_ERR_INVALID_LOCK_STATE);

    return NV_OK;
}

RS_PRIV_LEVEL
clientGetCachedPrivilege_IMPL
(
    RsClient *pClient
)
{
    // Non-functional, base class stubs
    return RS_PRIV_LEVEL_USER;
}

NvBool
clientIsAdmin_IMPL
(
    RsClient *pClient,
    RS_PRIV_LEVEL privLevel
)
{
    // Non-functional, base class stubs
    return NV_FALSE;
}

NV_STATUS
clientAllocResource_IMPL
(
    RsClient   *pClient,
    RsServer   *pServer,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return _clientAllocResourceHelper(pClient, pServer, pParams, &pParams->hResource);
}

NV_STATUS
clientCopyResource_IMPL
(
    RsClient   *pClientDst,
    RsServer   *pServer,
    RS_RES_DUP_PARAMS_INTERNAL *pParams
)
{
    RS_RES_ALLOC_PARAMS_INTERNAL params;
    CALL_CONTEXT  callContext;
    CALL_CONTEXT *pOldContext = NULL;

    RsResourceRef *pParentRef = NULL;
    NV_STATUS status;

    status = clientGetResourceRef(pClientDst, pParams->hParentDst, &pParentRef);
    if (status != NV_OK)
        return status;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClientDst;
    callContext.pResourceRef = pParams->pSrcRef;
    callContext.pContextRef = pParentRef;
    callContext.secInfo = *pParams->pSecInfo;
    callContext.pLockInfo = pParams->pLockInfo;

    NV_ASSERT_OK_OR_RETURN(resservSwapTlsCallContext(&pOldContext, &callContext));

    //
    // Kernel clients are allowed to dup anything, unless they request otherwise.
    // Also, if access rights are disabled, owner client should still be able to dup.
    // For anything else, check that the client has dup access on the object
    //
    if (((pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_KERNEL) ||
         (pParams->flags & NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE)) &&
        (pServer->bRsAccessEnabled || (pParams->pSrcClient->hClient != pClientDst->hClient)))
    {
        RS_ACCESS_MASK rightsRequired;

        portMemSet(&rightsRequired, 0, sizeof(rightsRequired));
        RS_ACCESS_MASK_ADD(&rightsRequired, RS_ACCESS_DUP_OBJECT);

        status = rsAccessCheckRights(pParams->pSrcRef, pClientDst, &rightsRequired);
    }
    else
    {
        // Server's globalInternalSharePolicyList applies Require policies even to kernel
        RsShareListIter it = listIterAll(&pServer->globalInternalSharePolicyList);
        while (listIterNext(&it))
        {
            RS_SHARE_POLICY *pSharePolicy = it.pValue;

            // We only care about failing Require policies which apply to Dup, ignore everything else
            if ((pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE) &&
                RS_ACCESS_MASK_TEST(&pSharePolicy->accessMask, RS_ACCESS_DUP_OBJECT) &&
                !resShareCallback(pParams->pSrcRef->pResource, pClientDst, pParentRef, pSharePolicy))
            {
                status = NV_ERR_INVALID_REQUEST;
                break;
            }
        }
    }

    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    if (status != NV_OK)
        return status;

    portMemSet(&params, 0, sizeof(params));

    params.hClient = pClientDst->hClient;
    params.hParent = pParams->hParentDst;
    params.hResource = pParams->hResourceDst;
    params.externalClassId = pParams->pSrcRef->externalClassId;
    params.pSecInfo = pParams->pSecInfo;

    params.pClient = pClientDst;
    params.pSrcClient = pParams->pSrcClient;
    params.pSrcRef = pParams->pSrcRef;
    params.pAllocParams = pParams->pShareParams;
    params.pLockInfo = pParams->pLockInfo;
    params.allocFlags = pParams->flags;

    return _clientAllocResourceHelper(pClientDst, pServer, &params, &pParams->hResourceDst);
}

static
void
_refCleanupDependencies
(
    RsResourceRef *pResourceRef
)
{
    RsResourceRef **ppIndepRef;
    while (NULL != (ppIndepRef = multimapFirstItem(&pResourceRef->depBackRefMap)))
    {
        refRemoveDependant(*ppIndepRef, pResourceRef);
    }
}

static
void
_refCleanupDependants
(
    RsResourceRef *pResourceRef
)
{
    RsResourceRef **ppDepRef;
    while (NULL != (ppDepRef = multimapFirstItem(&pResourceRef->depRefMap)))
    {
        refRemoveDependant(pResourceRef, *ppDepRef);
    }
}

static
void
_refRemoveAllDependencies
(
    RsResourceRef *pResourceRef
)
{
    _refCleanupDependencies(pResourceRef);

    if (pResourceRef->pDependantSession != NULL)
        sessionRemoveDependency(pResourceRef->pDependantSession, pResourceRef);

    if (pResourceRef->pSession != NULL)
        sessionRemoveDependant(pResourceRef->pSession, pResourceRef);
}

static
NV_STATUS
_clientAllocResourceHelper
(
    RsClient *pClient,
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NvHandle *phResource
)
{
    NV_STATUS       status;
    NvHandle        hResource = *phResource;
    NvU32           depth = 0;
    RsResource     *pResource = NULL;
    RsResourceRef  *pParentRef = NULL;
    RsResourceRef  *pResourceRef = NULL;
    CALL_CONTEXT    callContext;
    CALL_CONTEXT   *pOldContext = NULL;
    NvHandle        hParent = pParams->hParent;

    status = clientGetResourceRef(pClient, hParent, &pParentRef);
    if (status != NV_OK && hParent != pClient->hClient && hParent != 0)
        return status;

    status = _clientConstructResourceRef(pClient, pServer, pParentRef, hResource, pParams->externalClassId, &pResourceRef);
    if (status != NV_OK)
        goto fail;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClient;
    callContext.pResourceRef = pResourceRef;
    callContext.pContextRef = pParams->pSrcRef;
    callContext.pLockInfo = pParams->pLockInfo;

    if (pParams->pSecInfo == NULL)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }
    callContext.secInfo = *pParams->pSecInfo;

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), fail);

    status = resservResourceFactory(pServer->pAllocator, &callContext, pParams, &pResource);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    if (status != NV_OK)
        goto fail;

    // Clear free params implicitly set by constructor
    resSetFreeParams(pResource, NULL, NULL);
    pParams->pResourceRef = pResourceRef;

    //
    // resConstruct_IMPL sets these fields but we need to set them again until
    // Bug 2527351 is fixed
    //
    pResourceRef->pResource = pResource;
    pResource->pResourceRef = pResourceRef;

    if (pParentRef != NULL)
    {
        depth = pParentRef->depth + 1;
        pResourceRef->depth = depth;

        // Allow one additional level of depth to offset the depth used up by the RsClientResource at the root
        // of the object hierarchy
        if (RS_MAX_RESOURCE_DEPTH + 1 <= depth)
        {
            status = NV_ERR_ILLEGAL_ACTION;
            goto fail;
        }

        // Add this ref to the parent's child map
        if (NV_OK != indexAdd(&pParentRef->childRefMap, pResourceRef->internalClassId, pResourceRef))
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto fail;
        }
    }

    if (pServer->bRsAccessEnabled)
    {
        status = rsAccessGrantRights(pResourceRef, &callContext, pClient,
                                     pParams->pRightsRequested,
                                     pParams->pRightsRequired,
                                     pParams->pAllocParams);
        if (status != NV_OK)
            goto fail;
    }

    *phResource = hResource;

    return NV_OK;

fail:
    if (pResource != NULL)
    {
        NV_STATUS callContextStatus;

        RS_RES_FREE_PARAMS_INTERNAL params;
        pOldContext = NULL;

        // First undo dependency tracking since it might access the resource
        _refRemoveAllDependencies(pResourceRef);

        portMemSet(&params, 0, sizeof(params));
        portMemSet(&callContext, 0, sizeof(callContext));
        callContext.pServer = pServer;
        callContext.pClient = pClient;
        callContext.secInfo = *pParams->pSecInfo;
        callContext.pResourceRef = pResourceRef;
        callContext.pLockInfo = pParams->pLockInfo;

        callContextStatus = resservSwapTlsCallContext(&pOldContext, &callContext);
        if (callContextStatus == NV_OK)
        {
            resSetFreeParams(pResource, &callContext, &params);

            objDelete(pResource);
            NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to set call context! Error: 0x%x\n",
                callContextStatus);
        }

    }

    if (pResourceRef != NULL)
    {
        if (pParentRef != NULL)
        {
            indexRemove(&pParentRef->childRefMap, pResourceRef->internalClassId, pResourceRef);
        }

        clientDestructResourceRef(pClient, pServer, pResourceRef, pParams->pLockInfo, pParams->pSecInfo);
    }

    return status;
}

NV_STATUS
clientFreeResource_IMPL
(
    RsClient   *pClient,
    RsServer   *pServer,
    RS_RES_FREE_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS       status = NV_OK;
    NV_STATUS       tmpStatus;
    CALL_CONTEXT    callContext;
    CALL_CONTEXT   *pOldContext = NULL;
    RsResourceRef  *pClientRef = NULL;
    RsResourceRef  *pParentRef = NULL;
    RsResourceRef  *pResourceRef;
    RsResource     *pResource;

    pResourceRef = mapFind(&pClient->resourceMap, pParams->hResource);
    if (pResourceRef == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (refPendingFree(pResourceRef, pClient))
        listRemove(&pClient->pendingFreeList, pResourceRef);

    pResource = pResourceRef->pResource;
    pParentRef = pResourceRef->pParentRef;

    if (pResourceRef->bInvalidated)
        goto done;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pClient = pClient;
    callContext.pResourceRef = pResourceRef;
    callContext.pServer = pServer;
    callContext.pLockInfo = pParams->pLockInfo;

    // Some MODS tests don't set secInfo.
    if (pParams->pSecInfo != NULL)
        callContext.secInfo = *pParams->pSecInfo;

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

    resSetFreeParams(pResource, &callContext, pParams);

    resPreDestruct(pResource);

    // Remove all CPU mappings
    clientUnmapResourceRefMappings(pClient, &callContext, pParams->pLockInfo);
    _clientUnmapBackRefMappings(pClient, &callContext, pParams->pLockInfo);

    // Remove all inter-mappings
    _clientUnmapInterMappings(pClient, &callContext, pParams->pLockInfo);
    _clientUnmapInterBackRefMappings(pClient, &callContext, pParams->pLockInfo);

    // Remove this resource as a dependency from other resources
    pResourceRef->bInvalidated = NV_TRUE;
    _refRemoveAllDependencies(pResourceRef);

    status = serverFreeResourceRpcUnderLock(pServer, pParams);
    NV_ASSERT((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET));

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Freeing hResource: %x\n",
    //          pClient->hClient, pResourceRef->hResource);

    objDelete(pResource);

    pResourceRef->pResource = NULL;

    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

done:
    if (!pParams->bInvalidateOnly)
    {
        // Remove this ref from its parent's child ref list
        if (pParentRef != NULL)
        {
            multimapRemoveItemByKey(&pParentRef->childRefMap,
                pResourceRef->internalClassId, pResourceRef->hResource);
        }

        pClientRef = mapFind(&pClient->resourceMap, pClient->hClient);
        if (pClientRef != NULL)
            refUncacheRef(pClientRef, pResourceRef);

        tmpStatus = clientDestructResourceRef(pClient, pServer, pResourceRef, pParams->pLockInfo, pParams->pSecInfo);
        NV_ASSERT(tmpStatus == NV_OK);
    }

    return status;
}

NV_STATUS
clientUnmapMemory_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef,
    RS_LOCK_INFO  *pLockInfo,
    RsCpuMapping **ppCpuMapping,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;
    CALL_CONTEXT callContext;
    CALL_CONTEXT *pOldContext = NULL;
    RsCpuMapping *pCpuMapping = *ppCpuMapping;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pClient = pClient;
    callContext.pResourceRef = pResourceRef;
    callContext.pLockInfo = pLockInfo;

    // Some MODS tests don't set secInfo.
    if (pSecInfo != NULL)
        callContext.secInfo = *pSecInfo;

    NV_ASSERT_OK_OR_RETURN(resservSwapTlsCallContext(&pOldContext, &callContext));

    status = resUnmap(pResourceRef->pResource, &callContext, pCpuMapping);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "hClient %x: Failed to unmap cpu mapping: hResource: %x error: 0x%x\n",
                pClient->hClient,
                pResourceRef->hResource,
                status);

        if (pCpuMapping != NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "hContext: %x\n",
                      (pCpuMapping->pContextRef == NULL) ? 0 : pCpuMapping->pContextRef->hResource);
        }
    }

    refRemoveMapping(pResourceRef, pCpuMapping);
    *ppCpuMapping = NULL;

    return status;
}

NV_STATUS
clientInterMap_IMPL
(
    RsClient *pClient,
    RsResourceRef *pMapperRef,
    RsResourceRef *pMappableRef,
    RS_INTER_MAP_PARAMS *pParams
)
{
    return NV_ERR_INVALID_CLIENT;
}

NV_STATUS
clientInterUnmap_IMPL
(
    RsClient *pClient,
    RsResourceRef *pMapperRef,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    return NV_ERR_INVALID_CLIENT;
}

NV_STATUS
clientGenResourceHandle_IMPL
(
    RsClient *pClient,
    NvHandle *pHandle
)
{
    NvHandle hFirst;
    NvHandle hResource;
    NV_STATUS status;

    NV_ASSERT(pClient->handleRangeStart != 0);
    NV_ASSERT(pClient->handleRangeSize != 0);

    hResource = pClient->handleRangeStart + ((pClient->handleGenIdx++) % pClient->handleRangeSize);
    status = clientValidateNewResourceHandle(pClient, hResource, NV_FALSE);
    if (status == NV_OK)
    {
        goto done;
    }

    hFirst = hResource;
    do
    {
        hResource = pClient->handleRangeStart + ((pClient->handleGenIdx++) % pClient->handleRangeSize);
        status = clientValidateNewResourceHandle(pClient, hResource, NV_FALSE);
    } while(hResource != hFirst && status != NV_OK);

    if (status != NV_OK)
        return NV_ERR_INSUFFICIENT_RESOURCES;

done:
    NV_ASSERT(hResource - pClient->handleRangeStart < pClient->handleRangeSize);

    *pHandle = hResource;
    return NV_OK;
}

NV_STATUS
clientAssignResourceHandle_IMPL
(
    RsClient *pClient,
    NvHandle *phResource
)
{
    NV_STATUS status;

    if (phResource == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (*phResource == 0)
    {
        status = clientGenResourceHandle(pClient, phResource);
    }
    else
    {
        status = clientValidateNewResourceHandle(pClient, *phResource, NV_TRUE);
    }

    return status;

}

static
NV_STATUS
_clientConstructResourceRef
(
    RsClient *pClient,
    RsServer *pServer,
    RsResourceRef *pParentRef,
    NvHandle hResource,
    NvU32    externalClassId,
    RsResourceRef **ppResourceRef
)
{
    PORT_MEM_ALLOCATOR *pAllocator = pServer->pAllocator;
    RsResourceRef *pResourceRef = mapInsertNew(&pClient->resourceMap, hResource);
    if (pResourceRef == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    if (!pClient->bResourceWarning && (mapCount(&pClient->resourceMap) >= RS_CLIENT_RESOURCE_WARNING_THRESHOLD))
    {
        NV_PRINTF(LEVEL_WARNING, "Client 0x%08x has allocated a large number of resources. [Current classid: 0x%04x]\n", pClient->hClient, externalClassId);
        NV_PRINTF(LEVEL_WARNING, "The client may be leaking resources. This warning can be ignored if the allocations were intentional.\n");
        pClient->bResourceWarning = NV_TRUE;
    }

    pResourceRef->pClient = pClient;
    pResourceRef->pResourceDesc = RsResInfoByExternalClassId(externalClassId);
    pResourceRef->externalClassId = externalClassId;
    pResourceRef->internalClassId = RsResInfoGetInternalClassId(pResourceRef->pResourceDesc);
    pResourceRef->pResource = NULL;
    pResourceRef->pParentRef = pParentRef;
    pResourceRef->hResource = hResource;
    pResourceRef->depth = 0;

    multimapInit(&pResourceRef->childRefMap, pAllocator);
    multimapInit(&pResourceRef->cachedRefMap, pAllocator);
    multimapInit(&pResourceRef->depRefMap, pAllocator);
    multimapInit(&pResourceRef->depBackRefMap, pAllocator);
    listInit(&pResourceRef->cpuMappings, pAllocator);
    listInitIntrusive(&pResourceRef->backRefs);
    listInit(&pResourceRef->interMappings, pAllocator);
    listInitIntrusive(&pResourceRef->interBackRefsContext);
    listInitIntrusive(&pResourceRef->interBackRefsMappable);
    listInit(&pResourceRef->sharePolicyList, pAllocator);

    portAtomicExIncrementU64(&pServer->activeResourceCount);

    *ppResourceRef = pResourceRef;
    return NV_OK;
}

NV_STATUS
clientDestructResourceRef_IMPL
(
    RsClient *pClient,
    RsServer *pServer,
    RsResourceRef *pResourceRef,
    RS_LOCK_INFO *pLockInfo,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_ASSERT(pResourceRef != NULL);
    NV_ASSERT(listCount(&pResourceRef->backRefs) == 0);
    NV_ASSERT(listCount(&pResourceRef->cpuMappings) == 0);
    NV_ASSERT(listCount(&pResourceRef->interBackRefsMappable) == 0);
    NV_ASSERT(listCount(&pResourceRef->interBackRefsContext) == 0);
    NV_ASSERT(listCount(&pResourceRef->interMappings) == 0);

    listDestroy(&pResourceRef->backRefs);
    listDestroy(&pResourceRef->cpuMappings);
    listDestroy(&pResourceRef->interBackRefsMappable);
    listDestroy(&pResourceRef->interBackRefsContext);
    listDestroy(&pResourceRef->interMappings);
    listDestroy(&pResourceRef->sharePolicyList);

    // All children should be free
    if (0 != multimapCountItems(&pResourceRef->childRefMap))
    {
        RS_RES_FREE_PARAMS_INTERNAL params;
        NV_STATUS      tmpStatus;

        NV_ASSERT(0 == multimapCountItems(&pResourceRef->childRefMap));

        NV_PRINTF(LEVEL_ERROR, "Resource %x (Class %x) has unfreed children!\n",
                  pResourceRef->hResource, pResourceRef->externalClassId);

        RsIndexSupermapIter it = multimapSubmapIterAll(&pResourceRef->childRefMap);
        while (multimapSubmapIterNext(&it))
        {
            RsIndexSubmap *pSubmap = it.pValue;
            RsIndexIter subIt = multimapSubmapIterItems(&pResourceRef->childRefMap, pSubmap);
            while (multimapItemIterNext(&subIt))
            {
                RsResourceRef *pChildRef = *subIt.pValue;
                NV_PRINTF(LEVEL_ERROR, "Child %x (Class %x) is still alive!\n",
                          pChildRef->hResource, pChildRef->externalClassId);

                //
                // Attempt to kill any leaked children. If they are not deleted here,
                // they are likely to use-after-free when interacting with this parent object later.
                //
                portMemSet(&params, 0, sizeof(params));
                params.hClient = pChildRef->pClient->hClient;
                params.hResource = pChildRef->hResource;
                params.pResourceRef = pChildRef;
                params.pSecInfo = pSecInfo;
                params.pLockInfo = pLockInfo;
                tmpStatus = clientFreeResource(pChildRef->pClient, pServer, &params);
                NV_ASSERT(tmpStatus == NV_OK);
            }
        }
    }
    multimapDestroy(&pResourceRef->childRefMap);

    // Nothing should be cached
    NV_ASSERT(0 == multimapCountItems(&pResourceRef->cachedRefMap));
    multimapDestroy(&pResourceRef->cachedRefMap);

    _refCleanupDependencies(pResourceRef);
    multimapDestroy(&pResourceRef->depBackRefMap);

    _refCleanupDependants(pResourceRef);
    multimapDestroy(&pResourceRef->depRefMap);

    mapRemove(&pClient->resourceMap, pResourceRef);

    portAtomicExDecrementU64(&pServer->activeResourceCount);

    return NV_OK;
}

NV_STATUS
clientUnmapResourceRefMappings
(
    RsClient *pClient,
    CALL_CONTEXT *pCallContext,
    RS_LOCK_INFO *pLockInfo
)
{
    RsResourceRef  *pResourceRef = pCallContext->pResourceRef;
    RsCpuMapping   *pCpuMapping;
    NV_STATUS status;
    RS_LOCK_INFO lockInfo;
    RS_CPU_UNMAP_PARAMS params;

    pCpuMapping = listHead(&pResourceRef->cpuMappings);
    while(pCpuMapping != NULL)
    {
        portMemSet(&params, 0, sizeof(params));
        portMemSet(&lockInfo, 0, sizeof(lockInfo));

        params.hClient = pClient->hClient;
        params.hDevice = (pCpuMapping->pContextRef == NULL)
            ? pClient->hClient
            : pCpuMapping->pContextRef->hResource;
        params.hMemory = pResourceRef->hResource;
        params.pLinearAddress = pCpuMapping->pLinearAddress;
        params.processId = pCpuMapping->processId;
        params.bTeardown = NV_TRUE;
        params.flags = pCpuMapping->flags;
        params.pSecInfo = &pCallContext->secInfo;
        params.pLockInfo = &lockInfo;
        lockInfo.pClient = pLockInfo->pClient;
        lockInfo.state = pLockInfo->state;
        lockInfo.flags = pLockInfo->flags;

        // TODO: temp WAR for bug 2840284: deadlock during recursive free operation
        lockInfo.flags |= RS_LOCK_FLAGS_NO_CLIENT_LOCK;

        status = serverUnmap(pCallContext->pServer, params.hClient, params.hMemory, &params);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to auto-unmap (status=0x%x) hClient %x: hResource: %x\n",
                    status, pClient->hClient, pResourceRef->hResource);
            NV_PRINTF(LEVEL_ERROR, "hContext: %x at addr " NvP64_fmt "\n",
                    params.hDevice, params.pLinearAddress);

            if (pCpuMapping == listHead(&pResourceRef->cpuMappings))
            {
#if !(RS_STANDALONE_TEST)
                NV_ASSERT(0);
#endif
                refRemoveMapping(pResourceRef, pCpuMapping);
            }
        }
        pCpuMapping = listHead(&pResourceRef->cpuMappings);
    }

    return NV_OK;
}

NV_STATUS
_clientUnmapBackRefMappings
(
    RsClient *pClient,
    CALL_CONTEXT *pCallContext,
    RS_LOCK_INFO *pLockInfo
)
{
    NV_STATUS       status;
    RsResourceRef  *pResourceRef = pCallContext->pResourceRef;
    RsCpuMapping *pBackRefItem;
    RS_LOCK_INFO lockInfo;
    RS_CPU_UNMAP_PARAMS params;

    pBackRefItem = listHead(&pResourceRef->backRefs);
    while(pBackRefItem != NULL)
    {
        RsCpuMapping *pCpuMapping = pBackRefItem;
        RsResourceRef *pBackRef = pCpuMapping->pResourceRef;

        portMemSet(&params, 0, sizeof(params));
        portMemSet(&lockInfo, 0, sizeof(lockInfo));

        params.hClient = pClient->hClient;
        params.hDevice = (pCpuMapping->pContextRef == NULL)
            ? pClient->hClient
            : pCpuMapping->pContextRef->hResource;
        params.hMemory = pBackRef->hResource;
        params.pLinearAddress = pCpuMapping->pLinearAddress;
        params.processId = pCpuMapping->processId;
        params.bTeardown = NV_TRUE;
        params.flags = pCpuMapping->flags;
        params.pSecInfo = &pCallContext->secInfo;
        params.pLockInfo = &lockInfo;

        lockInfo.pClient = pLockInfo->pClient;
        lockInfo.state = pLockInfo->state;

        status = serverUnmap(pCallContext->pServer, pClient->hClient, pBackRef->hResource, &params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to auto-unmap backref (status=0x%x) hClient %x: hResource: %x\n",
                    status, pClient->hClient, pBackRef->hResource);
            NV_PRINTF(LEVEL_ERROR, "hContext: %x at addr " NvP64_fmt "\n",
                    params.hDevice, params.pLinearAddress);

            if (pBackRefItem == listHead(&pResourceRef->backRefs))
            {
                NV_ASSERT(0);
                listRemove(&pResourceRef->backRefs, pBackRefItem);
            }
        }

        pBackRefItem = listHead(&pResourceRef->backRefs);
    }

    return NV_OK;
}

static NV_STATUS
_unmapInterMapping
(
    RsServer *pServer,
    RsClient *pClient,
    RsResourceRef *pMapperRef,
    RsInterMapping *pMapping,
    RS_LOCK_INFO *pLockInfo,
    API_SECURITY_INFO *pSecInfo
)
{
    RS_INTER_UNMAP_PARAMS params;
    RS_LOCK_INFO lockInfo;
    NV_STATUS status;

    portMemSet(&params, 0, sizeof(params));
    portMemSet(&lockInfo, 0, sizeof(lockInfo));

    params.hClient = pClient->hClient;
    params.hMapper = pMapperRef->hResource;
    params.hDevice = pMapping->pContextRef->hResource;
    params.flags = pMapping->flags;
    params.dmaOffset = pMapping->dmaOffset;
    params.size = 0;
    params.pMemDesc = pMapping->pMemDesc;
    params.pSecInfo = pSecInfo;
    params.pLockInfo = &lockInfo;

    lockInfo.pClient = pLockInfo->pClient;
    lockInfo.pContextRef = (pLockInfo->pContextRef != NULL)
        ? pLockInfo->pContextRef
        : pMapping->pContextRef;
    lockInfo.state = pLockInfo->state;
    lockInfo.flags = pLockInfo->flags;

    status = serverUpdateLockFlagsForInterAutoUnmap(pServer, &params);
    if (status != NV_OK)
        return status;

    return serverInterUnmap(pServer, &params);
}

void
_clientUnmapInterMappings
(
    RsClient *pClient,
    CALL_CONTEXT *pCallContext,
    RS_LOCK_INFO *pLockInfo
)
{
    NV_STATUS status;
    RsResourceRef *pMapperRef = pCallContext->pResourceRef;
    RsInterMapping *pMapping;

    pMapping = listHead(&pMapperRef->interMappings);
    while (pMapping != NULL)
    {
        status = _unmapInterMapping(pCallContext->pServer, pClient, pMapperRef,
                                    pMapping, pLockInfo, &pCallContext->secInfo);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to auto-unmap (status=0x%x) hClient %x: hMapper: %x\n",
                      status, pClient->hClient, pMapperRef->hResource);
            NV_PRINTF(LEVEL_ERROR, "hMappable: %x hContext: %x\n",
                      pMapping->pMappableRef->hResource, pMapping->pContextRef->hResource);

            if (pMapping == listHead(&pMapperRef->interMappings))
            {
                NV_ASSERT(0);
                refRemoveInterMapping(pMapperRef, pMapping);
            }
        }

        pMapping = listHead(&pMapperRef->interMappings);
    }
}

void
_clientUnmapInterBackRefMappings
(
    RsClient *pClient,
    CALL_CONTEXT *pCallContext,
    RS_LOCK_INFO *pLockInfo
)
{
    NV_STATUS status;
    RsInterMapping *pBackRefItem;

    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    NvBool         bSwitched = NV_FALSE;

    pBackRefItem = listHead(&(pResourceRef->interBackRefsMappable));
    if (pBackRefItem == NULL)
    {
        bSwitched = NV_TRUE;
        pBackRefItem = listHead(&(pResourceRef->interBackRefsContext));
    }
    while (pBackRefItem != NULL)
    {
        RsResourceRef *pMapperRef = pBackRefItem->pMapperRef;
        RsInterMapping *pMapping = pBackRefItem;

        status = _unmapInterMapping(pCallContext->pServer, pClient, pMapperRef,
                                    pMapping, pLockInfo, &pCallContext->secInfo);
        if (status != NV_OK)
        {
            RsInterMapping *pCurHead = bSwitched ? listHead(&(pResourceRef->interBackRefsContext)) :
                listHead(&(pResourceRef->interBackRefsMappable));

            NV_PRINTF(LEVEL_ERROR, "Failed to auto-unmap backref (status=0x%x) hClient %x: hMapper: %x\n",
                      status, pClient->hClient, pMapperRef->hResource);
            NV_PRINTF(LEVEL_ERROR, "hMappable: %x hContext: %x\n",
                      pMapping->pMappableRef->hResource, pMapping->pContextRef->hResource);

            if (pBackRefItem == pCurHead)
            {
                NV_ASSERT(0);
                refRemoveInterMapping(pMapperRef, pMapping);
            }
        }

        pBackRefItem = bSwitched ? listHead(&(pResourceRef->interBackRefsContext)) :
            listHead(&(pResourceRef->interBackRefsMappable));

        if (pBackRefItem == NULL && (!bSwitched))
        {
            bSwitched = NV_TRUE;
            pBackRefItem = listHead(&(pResourceRef->interBackRefsContext));
        }
    }
}

NV_STATUS
indexAdd
(
    RsIndex *pIndex,
    NvU32 index,
    RsResourceRef *pResourceRef
)
{
    NV_ASSERT(pResourceRef != NULL && pResourceRef->hResource != 0);

    if (NULL == multimapFindSubmap(pIndex, index))
    {
        if (NULL == multimapInsertSubmap(pIndex, index))
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (NULL == multimapInsertItemValue(pIndex, index, pResourceRef->hResource,
                                        &pResourceRef))
        return NV_ERR_INSUFFICIENT_RESOURCES;

    return NV_OK;
}

NV_STATUS
indexRemove
(
    RsIndex *pIndex,
    NvU32 index,
    RsResourceRef *pResourceRef
)
{
    RsResourceRef **ppResourceRef;

    NV_ASSERT(pResourceRef != NULL && pResourceRef->hResource != 0);

    ppResourceRef = multimapFindItem(pIndex, index, pResourceRef->hResource);
    if (ppResourceRef == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    multimapRemoveItem(pIndex, ppResourceRef);

    return NV_OK;
}

NV_STATUS
clientValidateNewResourceHandle_IMPL
(
    RsClient *pClient,
    NvHandle  hResource,
    NvBool    bRestrict
)
{
    //
    // Resource handle should not be the same as the client handle
    // because some control calls pass hClient in the hObject field
    //
    if (pClient->hClient == hResource || hResource == 0)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    if (bRestrict && !rangeIsEmpty(pClient->handleRestrictRange))
    {
        NV_RANGE requestedRange = rangeMake(hResource, hResource);
        if (rangeContains(pClient->handleRestrictRange, requestedRange))
            return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    if (clientGetResourceRef(pClient, hResource, NULL) == NV_OK)
        return NV_ERR_INSERT_DUPLICATE_NAME;

    return NV_OK;
}

NV_STATUS
clientresConstruct_IMPL
(
    RsClientResource *pClientRes,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsClient *pClient = pCallContext->pClient;
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;

    // Client proxy resource must have the same handle as its client
    if (pClient->hClient != pResourceRef->hResource)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    pClientRes->pClient = pCallContext->pClient;
    return NV_OK;
}

void
clientresDestruct_IMPL
(
    RsClientResource *pClientRes
)
{
}

RsIndexIter
indexRefIter
(
    RsIndex *pIndex,
    NvU32 index
)
{
    RsIndexIter it;
    RsIndexSubmap *pSubmap;

    portMemSet(&it, 0, sizeof(it));
    NV_ASSERT(pIndex);

    pSubmap = multimapFindSubmap(pIndex, index);
    if (pSubmap != NULL)
        it = multimapSubmapIterItems(pIndex, pSubmap);

    return it;
}

RsIndexIter
indexRefIterAll
(
    RsIndex *pIndex
)
{
    NV_ASSERT(pIndex);
    return multimapItemIterAll(pIndex);
}

NvBool
indexRefIterNext
(
    RsIndexIter *pIt
)
{
    return multimapItemIterNext(pIt);
}

RS_ITERATOR
clientRefIter
(
    RsClient *pClient,
    RsResourceRef *pScopeRef,
    NvU32 internalClassId,
    RS_ITER_TYPE type,
    NvBool bExactMatch
)
{
    RS_ITERATOR it;
    RsIndex *pIndex = NULL;
    NvBool bChildrenOnly = (type == RS_ITERATE_CHILDREN);
    NvBool bCachedOnly = (type == RS_ITERATE_CACHED);
    NvBool bDependantsOnly = (type == RS_ITERATE_DEPENDANTS);
    portMemSet(&it, 0, sizeof(it));

    if (pClient == NULL)
    {
        NV_ASSERT(0);
        return it;
    }

    if (pScopeRef == NULL)
    {
        if (NV_OK != clientGetResourceRef(pClient, pClient->hClient, &pScopeRef))
            return it;
    }

    if (bChildrenOnly || bCachedOnly || bDependantsOnly)
    {
        NvBool bIterAll = (internalClassId == 0) || !bExactMatch;

        if (bChildrenOnly)
        {
            pIndex = &pScopeRef->childRefMap;
        }
        else if (bCachedOnly)
        {
            pIndex = &pScopeRef->cachedRefMap;
        }
        else if (bDependantsOnly)
        {
            pIndex = &pScopeRef->depRefMap;
        }

        if (!bIterAll && multimapFindSubmap(pIndex, internalClassId) == NULL)
            goto done;

        it.idxIt = (bIterAll)
            ? indexRefIterAll(pIndex)
            : indexRefIter(pIndex, internalClassId);
    }
    else
    {
        // Match descendants of the scope resource (specific class / any class)
        it.mapIt = mapIterAll(&pClient->resourceMap);
    }

    it.pClient = pClient;
    it.pScopeRef = pScopeRef;
    it.internalClassId = internalClassId;
    it.type = type;
    it.bExactMatch = bExactMatch;

done:
    return it;
}

RS_ORDERED_ITERATOR
clientRefOrderedIter
(
    RsClient *pClient,
    RsResourceRef *pScopeRef,
    NvU32 internalClassId,
    NvBool bExactMatch
)
{
    RS_ORDERED_ITERATOR it;
    RsIndex *pIndex = NULL;
    portMemSet(&it, 0, sizeof(it));

    if (pClient == NULL)
    {
        NV_ASSERT(0);
        return it;
    }

    if (pScopeRef == NULL)
    {
        if (NV_OK != clientGetResourceRef(pClient, pClient->hClient, &pScopeRef))
            return it;
    }

    it.depth = -1;
    pIndex = &pScopeRef->childRefMap;
    it.idxIt[0] = indexRefIterAll(pIndex);

    it.pClient = pClient;
    it.pScopeRef = pScopeRef;
    it.internalClassId = internalClassId;
    it.bExactMatch = bExactMatch;

    return it;
}

NvBool
clientRefOrderedIterNext
(
    RsClient *pClient,
    RS_ORDERED_ITERATOR *pIt
)
{
    RsResourceRef *pResourceRef;
    NvBool bNext;

    if ((pIt == NULL) || (pIt->pClient != pClient) || pIt->pScopeRef == NULL)
    {
        // Iterator not initialized or nothing to iterate over
        NV_ASSERT(pIt != NULL && pIt->pClient == NULL);
        return NV_FALSE;
    }

    // Iterate over the scope reference if the scope is not the client
    if (pIt->depth == -1)
    {
        pIt->depth = 0;
        if ((pIt->pScopeRef->hResource != pIt->pClient->hClient) &&
            ((pIt->internalClassId == 0) || (pIt->internalClassId == pIt->pScopeRef->internalClassId)) &&
            (pIt->pScopeRef->pResource != NULL))
        {
            pIt->pResourceRef = pIt->pScopeRef;
            return NV_TRUE;
        }
    }

    pIt->pResourceRef = NULL;

    bNext = NV_TRUE;
    while (1)
    {
        // Get the next sibling, or else backtrack to parent and get its next sibling
        do
        {
            if (!bNext)
                --pIt->depth;
            bNext = indexRefIterNext(&pIt->idxIt[pIt->depth]);
        } while (!bNext && pIt->depth != 0);

        if (!bNext)
            break;

        pResourceRef = *pIt->idxIt[pIt->depth].pValue;

        // Iterate over this resource's children next (up to max depth)
        if (pIt->depth < RS_MAX_RESOURCE_DEPTH)
        {
            ++pIt->depth;
            pIt->idxIt[pIt->depth] = indexRefIterAll(&pResourceRef->childRefMap);
        }

        if (refHasAncestor(pResourceRef, pIt->pScopeRef))
        {
            NvBool bMatch = NV_TRUE;
            if (pIt->internalClassId != 0)
            {
                if (pIt->bExactMatch && (pIt->internalClassId != pResourceRef->internalClassId))
                    bMatch = NV_FALSE;

                if (!pIt->bExactMatch && objDynamicCastById(pResourceRef->pResource, pIt->internalClassId) == NULL)
                    bMatch = NV_FALSE;
            }

            if (bMatch && (pResourceRef->pResource != NULL))
            {
                pIt->pResourceRef = pResourceRef;
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

NvBool
clientRefIterNext
(
    RsClient *pClient,
    RS_ITERATOR *pIt
)
{
    RsResourceRef *pResourceRef;
    NvBool bLoop;
    NvBool bUseIdx;

    if ((pIt == NULL) || (pIt->pClient != pClient) || pIt->pScopeRef == NULL)
    {
        // Iterator not initialized or nothing to iterate over
        NV_ASSERT(pIt != NULL && pIt->pClient == NULL);
        return NV_FALSE;
    }

    bUseIdx = (pIt->type == RS_ITERATE_CACHED) ||
              (pIt->type == RS_ITERATE_CHILDREN) ||
              (pIt->type == RS_ITERATE_DEPENDANTS);

    pIt->pResourceRef = NULL;

    bLoop = bUseIdx ? indexRefIterNext(&pIt->idxIt) : mapIterNext(&pIt->mapIt);
    while (bLoop)
    {
        pResourceRef = bUseIdx ? *pIt->idxIt.pValue : pIt->mapIt.pValue;

        if (bUseIdx ||
            ((pResourceRef == pIt->pScopeRef) ||
             (refHasAncestor(pResourceRef, pIt->pScopeRef))))
        {
            NvBool bMatch = NV_TRUE;
            if (pIt->internalClassId != 0)
            {
                if (pIt->bExactMatch && (pIt->internalClassId != pResourceRef->internalClassId))
                    bMatch = NV_FALSE;

                if (!pIt->bExactMatch && objDynamicCastById(pResourceRef->pResource, pIt->internalClassId) == NULL)
                    bMatch = NV_FALSE;
            }

            if (bMatch && (pResourceRef->pResource != NULL))
            {
                pIt->pResourceRef = pResourceRef;
                return NV_TRUE;
            }
        }

        bLoop = bUseIdx ? indexRefIterNext(&pIt->idxIt) : mapIterNext(&pIt->mapIt);
    }

    return NV_FALSE;
}

NV_STATUS
clientPostProcessPendingFreeList_IMPL
(
    RsClient *pClient,
    RsResourceRef **ppFirstLowPriRef
)
{
    if (ppFirstLowPriRef != NULL)
        *ppFirstLowPriRef = NULL;

    return NV_OK;
}

NV_STATUS
clientAddAccessBackRef_IMPL
(
    RsClient *pClient,
    RsResourceRef *pResourceRef
)
{
    AccessBackRef *pAccessBackRef = listPrependNew(&pClient->accessBackRefList);;

    if (pAccessBackRef == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pAccessBackRef->hClient = pResourceRef->pClient->hClient;
    pAccessBackRef->hResource = pResourceRef->hResource;

    return NV_OK;
}

void clientFreeAccessBackRefs_IMPL
(
    RsClient *pClient,
    RsServer *pServer
)
{
    AccessBackRef *pAccessBackRef;
    NV_STATUS      status;

    while ((pAccessBackRef = listHead(&pClient->accessBackRefList)) != NULL)
    {
        RsClient *pSharedClient;

        //
        // Remove access rights entry if client/resource pair is still in use
        // so that another client doesn't get unauthorized access to them
        //
        status = serverGetClientUnderLock(pServer, pAccessBackRef->hClient, &pSharedClient);
        if (status == NV_OK)
        {
            RsResourceRef *pResourceRef;

            status = clientGetResourceRef(pSharedClient, pAccessBackRef->hResource, &pResourceRef);
            if (status == NV_OK)
            {
                RS_SHARE_POLICY revokePolicy;

                revokePolicy.type = RS_SHARE_TYPE_CLIENT;
                revokePolicy.target = pClient->hClient;
                revokePolicy.action = RS_SHARE_ACTION_FLAG_REVOKE;
                RS_ACCESS_MASK_FILL(&revokePolicy.accessMask);

                // Check the resource's share policy for matching client policies
                rsShareListRemove(&pResourceRef->sharePolicyList, &revokePolicy, NULL);
            }
        }

        listRemove(&pClient->accessBackRefList, pAccessBackRef);
    }
}
