/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 
#define NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED
#include "nvlog_inc.h"
#include "resserv/resserv.h"
#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "tls/tls.h"
#include "nv_speculation_barrier.h"

/**
 * Get the RsClient from a client handle without taking locks
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[out]  ppClient The RsClient associated with the handle
 */
static NV_STATUS _serverFindClient(RsServer *pServer, NvHandle hClient, RsClient **ppClient);

/**
 * Get the CLIENT_ENTRY from a client handle without taking locks
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[in]   bFindPartial Include entries that have not finished constructing
 * @param[out]  ppClientEntry The client entry associated with the handle
 */
static NV_STATUS _serverFindClientEntry(RsServer *pServer, NvHandle hClient, NvBool bFindPartial, CLIENT_ENTRY **ppClientEntry);

/**
 * Insert a CLIENT_ENTRY in the server database without taking locks
 * @param[in]   pServer
 * @param[in]   pClientEntry The client entry associated with the handle
 */
static NV_STATUS _serverInsertClientEntry(RsServer *pServer, CLIENT_ENTRY *pClientEntry, CLIENT_ENTRY **ppClientNext);

/**
 * Find the next available client handle in bucket.
 * @param[in]   pServer
 * @param[in]   hClientIn
 * @param[out]  pClientOut
 */
static NV_STATUS _serverFindNextAvailableClientHandleInBucket(RsServer *pServer, NvHandle hClientIn, NvHandle *phClientOut, CLIENT_ENTRY  ***pppClientNext);

/**
 * Create a client entry and a client lock for a client that does not exist yet. Used during client
 * construction. No locks will be taken if this call fails.
 * @param[in]   pServer
 * @param[in]   hClient
 */
static NV_STATUS _serverCreateEntryAndLockForNewClient(RsServer *pServer, NvHandle *phClient, NvBool bInternalHandle, CLIENT_ENTRY **ppClientEntry );

/**
 * Lock and retrieve the RsClient associated with a client handle.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient Handle of client to look-up
 * @param[out]  pClient RsClient associated with the client handle
 */
static NV_STATUS _serverLockClient(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient, RsClient **ppClient);

/**
 * Lock and retrieve the RsClient associated with a client handle, and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient Handle of client to look-up
 * @param[inout] pLockInfo Lock state
 * @param[out]  pClient RsClient associated with the client handle
 */
static NV_STATUS _serverLockClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags, RsClient **ppClient);

/**
 * Lock and retrieve two RsClient associated with a pair of client handles, and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient1, hClient2 Handles of clients to look-up and lock
 * @param[inout] pLockInfo Lock state
 * @param[out]  pClient1, pClient2 RsClient associated with the client handles
 */
static NV_STATUS _serverLockDualClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient1, NvHandle hClient2, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags, RsClient **ppClient1, RsClient **ppClient2);

/**
 * Unlock a client by handle
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient Handle of the client to unlock
 */
static NV_STATUS _serverUnlockClient(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient);

/**
 * Unlock a client by handle, and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient Handle of the client to unlock
 * @param[inout] pLockInfo Lock state
 * @param[inout] pReleaseFlags   Flags indicating the locks that need to be released
 */
static NV_STATUS _serverUnlockClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient, RS_LOCK_INFO* pLockInfo, NvU32 *pReleaseFlags);

/**
 * Unlock a client by handle, and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   hClient1, hClient2 Handles of the clients to unlock
 * @param[inout] pLockInfo Lock state
 * @param[inout] pReleaseFlags   Flags indicating the locks that need to be released
 */
static NV_STATUS _serverUnlockDualClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient1, NvHandle hClient2, RS_LOCK_INFO* pLockInfo, NvU32 *pReleaseFlags);

NV_STATUS serverFreeResourceTreeUnderLock(RsServer *pServer, RS_RES_FREE_PARAMS *pFreeParams)
{
    NV_STATUS status;
    RsResourceRef *pResourceRef = pFreeParams->pResourceRef;
    RS_LOCK_INFO *pLockInfo = pFreeParams->pLockInfo;
    NvU32 releaseFlags = 0;

    NV_ASSERT_OR_RETURN(pResourceRef != NULL, NV_ERR_INVALID_OBJECT_HANDLE);

    status = serverUpdateLockFlagsForFree(pServer, pFreeParams);
    if (status != NV_OK)
        return status;

    status = serverSessionLock_Prologue(LOCK_ACCESS_WRITE, pResourceRef, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        return status;

    pLockInfo->flags |= RS_LOCK_FLAGS_FREE_SESSION_LOCK;
    pLockInfo->traceOp = RS_LOCK_TRACE_FREE;
    pLockInfo->traceClassId = pResourceRef->externalClassId;
    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = clientFreeResource(pResourceRef->pClient, pServer, pFreeParams);
    NV_ASSERT(status == NV_OK);

done:
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    serverSessionLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    return status;
}

#if RS_STANDALONE
NV_STATUS
serverInitFreeParams_Recursive(NvHandle hClient, NvHandle hResource, RS_LOCK_INFO* pLockInfo, RS_RES_FREE_PARAMS *pParams)
{
    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->hClient = hClient;
    pParams->hResource = hResource;
    pParams->pLockInfo = pLockInfo;
    return NV_OK;
}

NV_STATUS serverUpdateLockFlagsForCopy(RsServer *pServer, RS_RES_DUP_PARAMS *pParams)
{
    return NV_OK;
}

NV_STATUS serverUpdateLockFlagsForFree(RsServer *pServer, RS_RES_FREE_PARAMS *pParams)
{
    return NV_OK;
}

NV_STATUS serverUpdateLockFlagsForInterAutoUnmap(RsServer *pServer, RS_INTER_UNMAP_PARAMS *pParams)
{
    return NV_OK;
}

NV_STATUS serverFreeResourceRpcUnderLock(RsServer *pServer, RS_RES_FREE_PARAMS *pParams)
{
    return NV_OK;
}
#endif


//
//  Client handle format:
//
//  fn  [ C 1 D/E ] [  *INDEX*  ]
//  bit 31       20 19          0
//

#define RS_CLIENT_HANDLE_DECODE_MASK 0xFFFFF
#define CLIENT_DECODEHANDLE(handle)                 (handle & RS_CLIENT_HANDLE_DECODE_MASK)

#define CLIENT_ENCODEHANDLE(index)                  (RS_CLIENT_HANDLE_BASE | index)
#define CLIENT_ENCODEHANDLE_INTERNAL(internalBase, index)   (internalBase | index)

NV_STATUS
serverConstruct
(
    RsServer *pServer,
    RS_PRIV_LEVEL privilegeLevel,
    NvU32     maxDomains
)
{
    NvU32 i;
    PORT_MEM_ALLOCATOR *pAllocator = portMemAllocatorCreateNonPaged();

    pServer->privilegeLevel     = privilegeLevel;
    pServer->bConstructed       = NV_TRUE;
    pServer->pAllocator         = pAllocator;
    pServer->bDebugFreeList     = NV_FALSE;
    pServer->bRsAccessEnabled   = NV_TRUE;
    pServer->internalHandleBase = RS_CLIENT_INTERNAL_HANDLE_BASE;
    pServer->activeClientCount  = 0;
    pServer->activeResourceCount= 0;
    pServer->roTopLockApiMask   = 0;
    /* pServer->bUnlockedParamCopy is set in _rmapiLockAlloc */

    pServer->pClientSortedList = PORT_ALLOC(pAllocator, sizeof(RsClientList)*RS_CLIENT_HANDLE_BUCKET_COUNT);
    if (NULL == pServer->pClientSortedList)
        goto fail;

    for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
    {
        listInit(&pServer->pClientSortedList[i], pAllocator);
    }
    pServer->clientCurrentHandleIndex = 0;

    pServer->pClientListLock = portSyncRwLockCreate(pAllocator);
    if (pServer->pClientListLock == NULL)
        goto fail;

#if RS_STANDALONE
    RS_LOCK_VALIDATOR_INIT(&pServer->topLockVal, LOCK_VAL_LOCK_CLASS_API, 0xdead0000);
    pServer->pTopLock = portSyncRwLockCreate(pAllocator);
    if (pServer->pTopLock == NULL)
        goto fail;

    RS_LOCK_VALIDATOR_INIT(&pServer->resLockVal, LOCK_VAL_LOCK_CLASS_GPU, 0xbeef0000);
    pServer->pResLock = portSyncRwLockCreate(pAllocator);
    if (pServer->pResLock == NULL)
        goto fail;

    pServer->topLockOwnerTid = ~0;
#endif

    pServer->pShareMapLock = portSyncSpinlockCreate(pAllocator);

    mapInitIntrusive(&pServer->shareMap);

    listInit(&pServer->defaultInheritedSharePolicyList, pAllocator);
    listInit(&pServer->globalInternalSharePolicyList, pAllocator);

    if (NV_OK != serverInitGlobalSharePolicies(pServer))
    {
        mapDestroy(&pServer->shareMap);
        listDestroy(&pServer->defaultInheritedSharePolicyList);
        listDestroy(&pServer->globalInternalSharePolicyList);
        goto fail;
    }

    return NV_OK;
fail:

#if RS_STANDALONE
    if (pServer->pResLock != NULL)
        portSyncRwLockDestroy(pServer->pResLock);

    if (pServer->pTopLock != NULL)
        portSyncRwLockDestroy(pServer->pTopLock);
#endif

    if (pServer->pClientListLock != NULL)
        portSyncRwLockDestroy(pServer->pClientListLock);

    if (pServer->pShareMapLock != NULL)
        portSyncSpinlockDestroy(pServer->pShareMapLock);

    if (pServer->pClientSortedList != NULL)
    {
        for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
        {
            listDestroy(&pServer->pClientSortedList[i]);
        }
        PORT_FREE(pAllocator, pServer->pClientSortedList);
    }

    if (pAllocator != NULL)
        portMemAllocatorRelease(pAllocator);

    return NV_ERR_INSUFFICIENT_RESOURCES;
}


NV_STATUS
serverDestruct
(
    RsServer *pServer
)
{
    NvU32 i;
    RS_LOCK_INFO lockInfo;
    portMemSet(&lockInfo, 0, sizeof(lockInfo));

    if (!pServer->bConstructed)
        return NV_ERR_INVALID_OBJECT;

    for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
    {
        CLIENT_ENTRY **ppClientEntry;
        NvHandle hClient = 0;

        while ((ppClientEntry = listHead(&pServer->pClientSortedList[i])) != NULL)
        {
            RS_RES_FREE_PARAMS_INTERNAL freeParams;
            lockInfo.pClient = (*ppClientEntry)->pClient;
            hClient = lockInfo.pClient->hClient;
            serverInitFreeParams_Recursive(hClient, hClient, &lockInfo, &freeParams);
            serverFreeResourceTree(pServer, &freeParams);
        }

        listDestroy(&pServer->pClientSortedList[i]);
    }

    PORT_FREE(pServer->pAllocator, pServer->pClientSortedList);
    mapDestroy(&pServer->shareMap);
    listDestroy(&pServer->defaultInheritedSharePolicyList);
    listDestroy(&pServer->globalInternalSharePolicyList);

#if RS_STANDALONE
    portSyncRwLockDestroy(pServer->pResLock);
    portSyncRwLockDestroy(pServer->pTopLock);
#endif

    portSyncSpinlockDestroy(pServer->pShareMapLock);
    portSyncRwLockDestroy(pServer->pClientListLock);

    portMemAllocatorRelease(pServer->pAllocator);

    pServer->bConstructed = NV_FALSE;

    return NV_OK;
}

static
NV_STATUS
_serverFreeClient_underlock
(
    RsServer *pServer,
    RsClient *pClient
)
{
    CLIENT_ENTRY *pClientEntry = NULL;
    NvHandle hClient;
    NV_STATUS status;
    PORT_RWLOCK *pLock = NULL;

    status =_serverFindClientEntry(pServer, pClient->hClient, NV_FALSE, &pClientEntry);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT(pClientEntry->pClient != NULL);

    hClient = pClient->hClient;
    pClientEntry->pClient = NULL;
    pClientEntry->hClient = 0;

    clientFreeAccessBackRefs(pClient, pServer);

    objDelete(pClient);

    listRemoveFirstByValue(&pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK], &pClientEntry);
    pLock = pClientEntry->pLock;

    RS_RWLOCK_RELEASE_WRITE_EXT(pLock, &pClientEntry->lockVal, NV_TRUE);
    portSyncRwLockDestroy(pLock);
    PORT_FREE(pServer->pAllocator, pClientEntry);

    return NV_OK;
}

NV_STATUS
serverAllocDomain
(
    RsServer *pServer,
    NvU32 hParentDomain,
    ACCESS_CONTROL *pAccessControl,
    NvHandle *phDomain
)
{
    return NV_OK;
}

NV_STATUS
serverFreeDomain
(
    RsServer *pServer,
    NvHandle hDomain
)
{
    NvU32 bucket;
    for (bucket = 0; bucket < RS_CLIENT_HANDLE_BUCKET_COUNT; bucket ++)
    {
        RsClientList  *pClientList = &(pServer->pClientSortedList[bucket]);
        CLIENT_ENTRY **ppClientEntry = listHead(pClientList);
        while (ppClientEntry != NULL)
        {
            CLIENT_ENTRY *pClientEntry = *ppClientEntry;
            RS_CLIENT_FREE_PARAMS params;

            portMemSet(&params, 0, sizeof(params));
            if (pClientEntry == NULL)
            {
                ppClientEntry = listNext(pClientList, ppClientEntry);
                continue;
            }
            params.hClient = pClientEntry->hClient;

            serverFreeClient(pServer, &params);
            ppClientEntry = listHead(pClientList);
        }
    }
    return NV_OK;
}

NV_STATUS serverValidate
(
    RsServer *pServer,
    NvU32 hDomain,
    NvHandle hClient
)
{
    return NV_OK;
}

NV_STATUS
serverValidateAlloc
(
    RsServer *pServer,
    NvU32 hDomain,
    NvU32 externalClassId
)
{
    // Placeholder for allocation validation
    return NV_OK;
}

NV_STATUS
serverAllocClient
(
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS     status;
    NvHandle      hClient = 0;
    RsClient     *pClient = NULL;
    CLIENT_ENTRY *pClientEntry = NULL;
    NvBool        bLockedClient = NV_FALSE;

    if (!pServer->bConstructed)
    {
        status = NV_ERR_NOT_READY;
        goto done;
    }

    // RS-TODO Assert that the RW top lock is held

    hClient = pParams->hClient;
#if !(RS_COMPATABILITY_MODE)
    if (hClient != 0)
    {
        // Fail if the server supplied a client id
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }
#endif

    status = _serverCreateEntryAndLockForNewClient(pServer, &hClient, !!(pParams->allocState & ALLOC_STATE_INTERNAL_CLIENT_HANDLE), &pClientEntry);

    if (status != NV_OK)
    {
        goto done;
    }
    pParams->hClient = hClient;
    pParams->hResource = hClient;
    bLockedClient = NV_TRUE;

    status = resservClientFactory(pServer->pAllocator, pParams, &pClient);
    if (NV_OK != status)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    pClientEntry->pClient = pClient;

    // Automatically allocate client proxy resource
    status = clientAllocResource(pClient, pServer, pParams);
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "Allocated hClient: %x\n", hClient);
    portAtomicIncrementU32(&pServer->activeClientCount);

done:
    if (bLockedClient)
        _serverUnlockClient(pServer, LOCK_ACCESS_WRITE, pParams->hClient);

    if ((status != NV_OK) && (status != NV_ERR_INSERT_DUPLICATE_NAME) && (hClient != 0))
    {
        if (_serverFindClientEntry(pServer, hClient, NV_TRUE, &pClientEntry) == NV_OK)
        {
            listRemoveFirstByValue(&pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK], &pClientEntry);
            portSyncRwLockDestroy(pClientEntry->pLock);
            PORT_FREE(pServer->pAllocator, pClientEntry);
        }

        if (pClient != NULL)
        {
            objDelete(pClient);
        }
    }

    return status;
}

static
NV_STATUS
_serverFreeClient
(
    RsServer *pServer,
    RS_CLIENT_FREE_PARAMS *pParams
)
{
    NV_STATUS   status;
    NV_STATUS   lockStatus;
    NvU32       releaseFlags = 0;
    RsClient   *pClient;

    lockStatus = _serverLockClient(pServer, LOCK_ACCESS_WRITE, pParams->hClient, &pClient);
    if (lockStatus != NV_OK)
    {
        status = NV_ERR_INVALID_CLIENT;
        goto done;
    }
    releaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pParams->pResFreeParams->pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverFreeClient_underlock(pServer, pClient);
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "Freeing hClient: %x\n", hClient);
    portAtomicDecrementU32(&pServer->activeClientCount);

done:
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pParams->pResFreeParams->pLockInfo, &releaseFlags);

    if (releaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
        _serverUnlockClient(pServer, LOCK_ACCESS_WRITE, pParams->hClient);

    return status;
}

NV_STATUS
serverAllocResource
(
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS *pParams
)
{
    NV_STATUS           status;
    NvU32               releaseFlags = 0;
    API_STATE          *pApiState;
    NvBool              bClientAlloc = (pParams->externalClassId == NV01_ROOT ||
                                        pParams->externalClassId == NV01_ROOT_CLIENT ||
                                        pParams->externalClassId == NV01_ROOT_NON_PRIV);
    LOCK_ACCESS_TYPE    topLockAccess;
    NvU32               initialLockState;
    RS_LOCK_INFO       *pLockInfo;
    RsClient           *pSecondClient = NULL;
    NvHandle            hSecondClient;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    initialLockState = pLockInfo->state;

    status = serverAllocApiCopyIn(pServer, pParams, &pApiState);
    if (status != NV_OK)
        return status;

    status = serverAllocResourceLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    if ((status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags)) != NV_OK)
        goto done;

    if (status == NV_OK)
    {
        if (bClientAlloc)
        {
            status = serverAllocClient(pServer, pParams);
        }
        else
        {
            status = serverLookupSecondClient(pParams, &hSecondClient);

            if (status != NV_OK)
                goto done;

            if (hSecondClient == 0)
            {
                status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                       pParams->hClient, pLockInfo,
                                                       &releaseFlags, &pParams->pClient);

                if (status != NV_OK)
                    goto done;

                if (!pParams->pClient->bActive)
                {
                    status = NV_ERR_INVALID_STATE;
                    goto done;
                }
            }
            else
            {
                status = _serverLockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                           pParams->hClient, hSecondClient,
                                                           pLockInfo, &releaseFlags,
                                                           &pParams->pClient, &pSecondClient);

                if (status != NV_OK)
                    goto done;

                if (!pParams->pClient->bActive || !pSecondClient->bActive)
                {
                    status = NV_ERR_INVALID_STATE;
                    goto done;
                }
            }

            // The second client's usage is class-dependent and should be validated
            // by the class's constructor
            status = clientValidate(pParams->pClient, pParams->pSecInfo);

            if (status != NV_OK)
                goto done;

            status = serverAllocResourceUnderLock(pServer, pParams);
        }
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                   "hParent 0x%08x : hClass 0x%08x allocation failed\n",
                   pParams->hParent, pParams->externalClassId);
    }

    // RS-TODO: Can this be moved before _ResLock?
    status = serverAllocEpilogue_WAR(pServer, status, bClientAlloc, pParams);

done:

    if (!bClientAlloc)
    {
        if (pSecondClient != NULL)
        {
            _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                pParams->hClient, pSecondClient->hClient,
                                                pLockInfo, &releaseFlags);
        }
        else
        {
            _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient,
                                            pLockInfo, &releaseFlags);
        }
    }

    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    // copyout as needed, being careful not to overwrite a useful status value
    status = serverAllocApiCopyOut(pServer, status, pApiState);

    NV_ASSERT(pLockInfo->state == initialLockState);

    return status;
}

#if RS_STANDALONE
// RS-TODO rename to UnderClientLock
NV_STATUS
serverAllocResourceUnderLock
(
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS *pParams
)
{
    NV_STATUS   status;
    RsClient   *pClient = pParams->pClient;
    NvHandle    hResource = pParams->hResource;
    NvU32       releaseFlags = 0;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = clientAssignResourceHandle(pClient, &hResource);
    if (status != NV_OK)
        goto done;

    pParams->hResource = hResource;
    pParams->hParent = (pParams->hParent == 0) ? pParams->hClient : pParams->hParent;
    status = clientAllocResource(pClient, pServer, pParams);
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Allocated hResource %x with class %x\n",
    //           pParams->hClient, pParams->hResource, pParams->externalClassId);

done:
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, &releaseFlags);
    return status;
}
#endif

NV_STATUS
clientUpdatePendingFreeList_IMPL
(
    RsClient *pClient,
    RsResourceRef *pTargetRef,
    RsResourceRef *pReference,
    NvBool bMove
)
{
    RsIndexIter it;
    NvBool bInList = refPendingFree(pTargetRef, pClient);
    RS_FREE_STACK *pFs = pClient->pFreeStack;
    if (bMove)
    {
        if (pReference != pTargetRef)
        {
            // Basic circular dependency check
            while (pFs != NULL)
            {
                RsResourceRef *pFsRef = pFs->pResourceRef;
                NV_ASSERT_OR_GOTO(pFsRef != pTargetRef, done);

                pFs = pFs->pPrev;
            }
        }

        if (bInList)
            listRemove(&pClient->pendingFreeList, pTargetRef);
        listPrependExisting(&pClient->pendingFreeList, pTargetRef);
    }
    else if (!bInList)
    {
        listPrependExisting(&pClient->pendingFreeList, pTargetRef);
    }

    //
    // Recursively add children to the pending free list and move
    // them to the front of the list
    //
    it = indexRefIterAll(&pTargetRef->childRefMap);
    while (indexRefIterNext(&it))
    {
        clientUpdatePendingFreeList(pClient, *it.pValue, pReference, NV_TRUE);
    }

    //
    // Recursively add dependencies to the pending free list and
    // move them to the front of the list
    //
    it = indexRefIterAll(&pTargetRef->depRefMap);
    while (indexRefIterNext(&it))
    {
        clientUpdatePendingFreeList(pClient, *it.pValue, pReference, NV_TRUE);
    }

    if (pTargetRef->pResource != NULL)
    {
        // Allow some objects to add more dependants here
        resAddAdditionalDependants(pClient, pTargetRef->pResource, pReference);
    }

done:
    return NV_OK;
}

NV_STATUS
serverFreeClientList
(
    RsServer *pServer,
    NvHandle *phClientList,
    NvU32 numClients,
    NvU32 freeState,
    API_SECURITY_INFO *pSecInfo
)
{
    NvU32 i, j;

    //
    // Call serverFreeClient twice; first for high priority resources
    // then again for remaining resources
    //
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < numClients; ++j)
        {
            RS_CLIENT_FREE_PARAMS params;
            portMemSet(&params, 0, sizeof(params));

            if (phClientList[j] == 0)
                continue;

            params.hClient = phClientList[j];
            params.bHiPriOnly = (i == 0);
            params.state = freeState;
            params.pSecInfo = pSecInfo;

            serverFreeClient(pServer, &params);
        }
    }

    return NV_OK;
}

NV_STATUS
serverFreeResourceTree
(
    RsServer *pServer,
    RS_RES_FREE_PARAMS *pParams
)
{
    RsClient           *pClient = NULL;
    NV_STATUS           status;
    RsResourceRef      *pResourceRef = NULL;
    RsResourceRef      *pTargetRef;
    RsResourceRef      *pFirstLowPriRef;
    NvBool              bHiPriOnly = pParams->bHiPriOnly;
    NvBool              bRecursive = NV_FALSE;
    RS_FREE_STACK       freeStack;
    NvBool              bPopFreeStack = NV_FALSE;
    RS_LOCK_INFO       *pLockInfo;
    NvU32               initialLockState;
    NvU32               releaseFlags = 0;
    LOCK_ACCESS_TYPE    topLockAccess;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    initialLockState = pLockInfo->state;

    portMemSet(&freeStack, 0, sizeof(freeStack));

    status = serverFreeResourceLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    if (pClient->pFreeStack != NULL)
        freeStack.pPrev = pClient->pFreeStack;
    pClient->pFreeStack = &freeStack;
    bPopFreeStack = NV_TRUE;

    status = clientGetResourceRef(pClient, pParams->hResource, &pResourceRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "hObject 0x%x not found for client 0x%x\n",
                pParams->hResource,
                pParams->hClient);
#if (RS_COMPATABILITY_MODE)
        status = NV_OK;
#endif
        goto done;
    }
    pParams->pResourceRef = pResourceRef;
    freeStack.pResourceRef = pResourceRef;

    if (pParams->bInvalidateOnly && pResourceRef->bInvalidated)
    {
        status = NV_OK;
        goto done;
    }

    bRecursive = (freeStack.pPrev != NULL);
    status = clientUpdatePendingFreeList(pClient, pResourceRef, pResourceRef, bRecursive);
    if (status != NV_OK)
        goto done;

    clientPostProcessPendingFreeList(pClient, &pFirstLowPriRef);

    if (pServer->bDebugFreeList)
    {
        NV_PRINTF(LEVEL_INFO, "PENDING FREE LIST START (0x%x)\n", pClient->hClient);
        NV_PRINTF(LEVEL_INFO, "  _HI_PRIORITY_:\n");
        pTargetRef = listHead(&pClient->pendingFreeList);
        while (pTargetRef != NULL)
        {
            if (pTargetRef == pFirstLowPriRef)
                NV_PRINTF(LEVEL_INFO, "  _LO_PRIORITY_:\n");

            NV_PRINTF(LEVEL_INFO, "  0x%08x [%04x]\n",
                    pTargetRef->hResource,
                    pTargetRef->externalClassId);
            pTargetRef = listNext(&pClient->pendingFreeList, pTargetRef);
        }
        NV_PRINTF(LEVEL_INFO, "PENDING FREE LIST END (0x%x)\n", pClient->hClient);
    }

    while ((pTargetRef = listHead(&pClient->pendingFreeList)) != NULL)
    {
        NvBool bInvalidateOnly = NV_TRUE;
        RS_FREE_STACK *pFs = &freeStack;
        RS_RES_FREE_PARAMS_INTERNAL freeParams;
        NvHandle hTarget = pTargetRef->hResource;

        if (bHiPriOnly && pTargetRef == pFirstLowPriRef)
            goto done;

        if (pServer->bDebugFreeList)
        {
            NV_PRINTF(LEVEL_INFO, "(%08x, %08x)\n", pClient->hClient, hTarget);
        }

        if (hTarget == pParams->hResource)
        {
            // Target resource should always be the last one to be freed
            NV_ASSERT((listCount(&pClient->pendingFreeList) == 1) || bRecursive);
            status = serverFreeResourceTreeUnderLock(pServer, pParams);
            break;
        }

        while (pFs != NULL)
        {
            RsResourceRef *pFsRef = pFs->pResourceRef;
            if (refHasAncestor(pTargetRef, pFsRef))
            {
                bInvalidateOnly = pParams->bInvalidateOnly;
                break;
            }
            pFs = pFs->pPrev;
        }

        serverInitFreeParams_Recursive(pClient->hClient, hTarget, pLockInfo, &freeParams);
        freeParams.pResourceRef = pTargetRef;
        freeParams.bInvalidateOnly = bInvalidateOnly;
        freeParams.pSecInfo = pParams->pSecInfo;
        status = serverFreeResourceTreeUnderLock(pServer, &freeParams);
        NV_ASSERT(status == NV_OK);

        if (pServer->bDebugFreeList)
        {
            NV_PRINTF(LEVEL_INFO, "(%08x, %08x) status=0x%x\n",
                    pClient->hClient,
                    hTarget,
                    status);
        }
    }

    if (bPopFreeStack)
    {
        if (pClient != NULL)
            pClient->pFreeStack = freeStack.pPrev;
        bPopFreeStack = NV_FALSE;
    }

    if (pParams->hClient == pParams->hResource)
    {
        pClient->bActive = NV_FALSE;
    }

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);

    if (pParams->hClient == pParams->hResource)
    {
        NvBool bReAcquireLock = (topLockAccess != LOCK_ACCESS_WRITE);
        RS_CLIENT_FREE_PARAMS_INTERNAL clientFreeParams;
        portMemSet(&clientFreeParams, 0, sizeof(clientFreeParams));
        clientFreeParams.pResFreeParams = pParams;
        clientFreeParams.hClient = pParams->hClient;

        if (bReAcquireLock)
        {
            serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);
            serverTopLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
            _serverFreeClient(pServer, &clientFreeParams);
            serverTopLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
            initialLockState &= ~RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
        }
        else
        {
            _serverFreeClient(pServer, &clientFreeParams);
        }

        pClient = NULL;
    }

done:
    if (bPopFreeStack)
    {
        if (pClient != NULL)
            pClient->pFreeStack = freeStack.pPrev;
        bPopFreeStack = NV_FALSE;
    }

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    //
    // Log any changes to lock state, but ignore the ALLOW_RECURSIVE_LOCKS flag
    // as that can be set by serverUpdateLockFlagsForFree() when dealing with
    // RPCs to GSP; this would have already printed the relevant message.
    //
    NV_ASSERT((pLockInfo->state == initialLockState) ||
              (pLockInfo->state == (initialLockState | RS_LOCK_STATE_ALLOW_RECURSIVE_RES_LOCK)));

    return status;
}

NV_STATUS
serverControl
(
    RsServer *pServer,
    RS_RES_CONTROL_PARAMS *pParams
)
{
    NV_STATUS           status;
    RsClient           *pClient;
    RsResourceRef      *pResourceRef = NULL;
    RS_LOCK_INFO       *pLockInfo;
    NvU32               releaseFlags = 0;
    CALL_CONTEXT        callContext;
    CALL_CONTEXT       *pOldContext = NULL;
    LOCK_ACCESS_TYPE    access = LOCK_ACCESS_WRITE;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    status = serverControlLookupLockFlags(pServer, RS_LOCK_TOP, pParams, pParams->pCookie, &access);
    if (status != NV_OK)
        goto done;

    if (pServer->bUnlockedParamCopy)
    {
        status = serverControlApiCopyIn(pServer, pParams, pParams->pCookie);
        if (status != NV_OK)
            goto done;
    }

    status = serverTopLock_Prologue(pServer, access, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    if (!pClient->bActive)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    status = clientValidate(pClient, &pParams->secInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hObject, &pResourceRef);
    if (status != NV_OK)
        goto done;
    pParams->pResourceRef = pResourceRef;

    if (pResourceRef->bInvalidated || pResourceRef->pResource == NULL)
    {
        status = NV_ERR_RESOURCE_LOST;
        goto done;
    }

    pLockInfo->flags |= RS_LOCK_FLAGS_NO_DEPENDANT_SESSION_LOCK;

    status = serverSessionLock_Prologue(LOCK_ACCESS_WRITE, pResourceRef,
                                        pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    if (pResourceRef->pSession != NULL)
    {
        if (!pResourceRef->pSession->bValid)
        {
            status = NV_ERR_RESOURCE_LOST;
            goto done;
        }
    }

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pResourceRef   = pResourceRef;
    callContext.pClient        = pClient;
    callContext.secInfo        = pParams->secInfo;
    callContext.pServer        = pServer;
    callContext.pControlParams = pParams;
    callContext.pLockInfo      = pParams->pLockInfo;

    // RS-TODO removeme
    pParams->pLegacyParams = pParams;

    if (pParams->hClient == pParams->hObject)
    {
        pParams->hParent = pParams->hClient;
    }
    else
    {
        pParams->hParent = pResourceRef->pParentRef->hResource;
    }
    pLockInfo->pContextRef = pResourceRef->pParentRef;

    resservSwapTlsCallContext(&pOldContext, &callContext);
    status = resControl(pResourceRef->pResource, &callContext, pParams);
    resservRestoreTlsCallContext(pOldContext);

done:

    serverSessionLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, access, pLockInfo, &releaseFlags);

    if (pServer->bUnlockedParamCopy)
    {
        status = serverControlApiCopyOut(pServer, pParams, pParams->pCookie, status);
    }

    return status;
}

NV_STATUS
serverCopyResource
(
    RsServer *pServer,
    RS_RES_DUP_PARAMS *pParams
)
{
    NV_STATUS           status;
    RS_LOCK_INFO       *pLockInfo = pParams->pLockInfo;
    NvU32               releaseFlags = 0;
    RsClient           *pClientSrc;
    RsClient           *pClientDst;
    RsResourceRef      *pResourceRefSrc;
    LOCK_ACCESS_TYPE    topLockAccess;

    NvHandle hClientSrc = pParams->hClientSrc;
    NvHandle hClientDst = pParams->hClientDst;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    status = serverCopyResourceLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                               hClientSrc, hClientDst,
                                               pLockInfo, &releaseFlags,
                                               &pClientSrc, &pClientDst);
    if (status != NV_OK)
        goto done;

    if (!pClientSrc->bActive || !pClientDst->bActive)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    status = clientValidate(pClientDst, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClientSrc, pParams->hResourceSrc, &pResourceRefSrc);
    if (status != NV_OK)
        goto done;

    if (pResourceRefSrc->bInvalidated)
    {
        status = NV_ERR_RESOURCE_LOST;
        goto done;
    }

    if (!resCanCopy(pResourceRefSrc->pResource))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    status = clientAssignResourceHandle(pClientDst, &pParams->hResourceDst);
    if (status != NV_OK)
        goto done;

    pParams->pSrcClient = pClientSrc;
    pParams->pSrcRef = pResourceRefSrc;

    status = serverUpdateLockFlagsForCopy(pServer, pParams);
    if (status != NV_OK)
        return status;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = clientCopyResource(pClientDst, pServer, pParams);
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Copied hResource: %x from hClientSrc: %x hResourceSrc: %x\n",
    //          hClientDst, hResourceDst, hClientSrc, hResourceSrc);

done:
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, &releaseFlags);

    _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                        hClientSrc, hClientDst,
                                        pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

/**
 * Special case of serverShareResourceAccess for sharing with a specific client
 * Requires two client locks, so separated into a different function from the normal
 * @param[in] pServer
 * @param[in] pParams Parameters passed into share function
 */
static NV_STATUS
_serverShareResourceAccessClient
(
    RsServer *pServer,
    RS_RES_SHARE_PARAMS *pParams
)
{
    NV_STATUS           status;
    RS_LOCK_INFO       *pLockInfo = pParams->pLockInfo;
    NvU32               releaseFlags = 0;
    RsClient           *pClientOwner;
    RsClient           *pClientTarget;
    RsResourceRef      *pResourceRef;
    LOCK_ACCESS_TYPE    topLockAccess;

    NvHandle       hClientOwner = pParams->hClient;
    NvHandle       hClientTarget = pParams->pSharePolicy->target;

    CALL_CONTEXT   callContext;
    CALL_CONTEXT  *pOldContext = NULL;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    status = serverShareResourceLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                               hClientOwner, hClientTarget,
                                               pLockInfo, &releaseFlags,
                                               &pClientOwner, &pClientTarget);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClientOwner, pParams->hResource, &pResourceRef);
    if (status != NV_OK)
        goto done;

    if (pResourceRef->bInvalidated)
    {
        status = NV_ERR_RESOURCE_LOST;
        goto done;
    }

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClientOwner;
    callContext.pResourceRef = pResourceRef;
    callContext.secInfo = *pParams->pSecInfo;
    callContext.pLockInfo = pParams->pLockInfo;
    resservSwapTlsCallContext(&pOldContext, &callContext);

    if (hClientOwner == hClientTarget)
    {
        //
        // Special case: RS_SHARE_TYPE_CLIENT with own client
        // Allows the caller to directly modify the access map of their object
        //
        status = clientShareResourceTargetClient(pClientOwner, pResourceRef, pParams->pSharePolicy, &callContext);
        if (status != NV_OK)
            goto restore_context;
    }

    // Add backref into pClientTarget to prevent stale client handles
    status = clientAddAccessBackRef(pClientTarget, pResourceRef);
    if (status != NV_OK)
        goto restore_context;

    status = clientShareResource(pClientOwner, pResourceRef, pParams->pSharePolicy, &callContext);
    if (status != NV_OK)
        goto restore_context;

restore_context:
    resservRestoreTlsCallContext(pOldContext);

    // NV_PRINTF(LEVEL_INFO, "hClientOwner %x: Shared hResource: %x with hClientTarget: %x\n",
    //           hClientOwner, pParams->hResource, hClientTarget);

done:
    _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                        hClientOwner, hClientTarget,
                                        pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}


NV_STATUS
serverShareResourceAccess
(
    RsServer *pServer,
    RS_RES_SHARE_PARAMS *pParams
)
{
    NV_STATUS           status;
    RS_LOCK_INFO       *pLockInfo;
    NvU32               releaseFlags = 0;
    RsClient           *pClient;
    RsResourceRef      *pResourceRef;
    NvU16               shareType;
    CALL_CONTEXT        callContext;
    CALL_CONTEXT       *pOldContext = NULL;
    LOCK_ACCESS_TYPE    topLockAccess;

    if (!pServer->bConstructed)
        return NV_ERR_NOT_READY;

    if (!pServer->bRsAccessEnabled)
        return NV_ERR_FEATURE_NOT_ENABLED;

    if (pParams->pSharePolicy == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    shareType = pParams->pSharePolicy->type;
    if (shareType >= RS_SHARE_TYPE_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    if (shareType == RS_SHARE_TYPE_CLIENT)
    {
        // Special case: This requires two locks, so it has its own function
        return _serverShareResourceAccessClient(pServer, pParams);
    }

    status = serverShareResourceLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hResource, &pResourceRef);
    if (status != NV_OK)
        goto done;

    if (pResourceRef->bInvalidated)
    {
        status = NV_ERR_RESOURCE_LOST;
        goto done;
    }

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClient;
    callContext.pResourceRef = pResourceRef;
    callContext.secInfo = *pParams->pSecInfo;
    callContext.pLockInfo = pParams->pLockInfo;

    resservSwapTlsCallContext(&pOldContext, &callContext);
    status = clientShareResource(pClient, pResourceRef, pParams->pSharePolicy, &callContext);
    resservRestoreTlsCallContext(pOldContext);
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Shared hResource: %x\n", hClient, pParams->hResource);

done:
    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);

    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverMap
(
    RsServer *pServer,
    NvHandle hClient,
    NvHandle hResource,
    RS_CPU_MAP_PARAMS *pParams
)
{
    NV_STATUS           status = NV_ERR_INVALID_STATE;
    CALL_CONTEXT        callContext;
    CALL_CONTEXT       *pOldContext = NULL;
    RsClient           *pClient;
    RsResourceRef      *pResourceRef;
    RsResourceRef      *pContextRef = NULL;
    RsResource         *pResource;
    RsCpuMapping       *pCpuMapping = NULL;
    RS_LOCK_INFO       *pLockInfo;
    NvU32               releaseFlags = 0;
    LOCK_ACCESS_TYPE    topLockAccess = LOCK_ACCESS_WRITE;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_GOTO(pLockInfo != NULL, done);

    status = serverMapLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient, pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    if (!pClient->bActive)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        goto done;

    pResource = pResourceRef->pResource;
    if (pResource == NULL)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    status = serverMap_Prologue(pServer, pParams);
    if (status != NV_OK)
        goto done;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    if (pParams->hContext != 0)
    {
        status = clientGetResourceRef(pClient, pParams->hContext, &pContextRef);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "hClient %x: Cannot find hContext: 0x%x\n", pClient->hClient, pParams->hContext);
            goto done;
        }
    }

    status = refAddMapping(pResourceRef, pParams, pContextRef, &pCpuMapping);
    if (status != NV_OK)
        goto done;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pClient = pClient;
    callContext.pResourceRef = pResourceRef;
    callContext.pLockInfo = pParams->pLockInfo;

    // Some MODS tests don't set secInfo.
    if (pParams->pSecInfo != NULL)
        callContext.secInfo = *pParams->pSecInfo;

    resservSwapTlsCallContext(&pOldContext, &callContext);
    status = resMap(pResource, &callContext, pParams, pCpuMapping);
    resservRestoreTlsCallContext(pOldContext);

    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Mapped hResource: 0x%x hContext: %x at addr: " NvP64_fmt "\n",
    //           hClient, hResource, pParams->hContext, pCpuMapping->pAddress);

    if (pParams->ppCpuVirtAddr != NULL)
        *pParams->ppCpuVirtAddr = pCpuMapping->pLinearAddress;

done:
    if (status != NV_OK)
    {
        if (pCpuMapping != NULL)
            refRemoveMapping(pResourceRef, pCpuMapping);
    }

    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverUnmap
(
    RsServer *pServer,
    NvHandle hClient,
    NvHandle hResource,
    RS_CPU_UNMAP_PARAMS *pParams
)
{
    NV_STATUS           status = NV_ERR_INVALID_STATE;
    RsClient           *pClient;
    RsResourceRef      *pResourceRef;
    RsResource         *pResource;
    RsCpuMapping       *pCpuMapping;
    RS_LOCK_INFO       *pLockInfo;
    NvU32               releaseFlags = 0;
    LOCK_ACCESS_TYPE    topLockAccess = LOCK_ACCESS_WRITE;

    pLockInfo = pParams->pLockInfo;
    NV_ASSERT_OR_GOTO(pLockInfo != NULL, done);

    status = serverUnmapLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient, pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        goto done;

    pResource = pResourceRef->pResource;
    if (pResource == NULL)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    status = serverUnmap_Prologue(pServer, pParams);
    if (status != NV_OK)
        goto done;

    status = refFindCpuMappingWithFilter(pResourceRef,
                                         pParams->pLinearAddress,
                                         pParams->fnFilter,
                                         &pCpuMapping);
    if (status != NV_OK)
        goto done;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = clientUnmapMemory(pClient, pResourceRef, pLockInfo, &pCpuMapping, pParams->pSecInfo);

done:
    serverUnmap_Epilogue(pServer, pParams);
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverInterMap
(
    RsServer *pServer,
    RS_INTER_MAP_PARAMS *pParams
)
{
    RsClient           *pClient;
    RsResourceRef      *pMapperRef;
    RsResourceRef      *pMappableRef;
    RsResourceRef      *pContextRef;
    RsInterMapping     *pMapping = NULL;
    LOCK_ACCESS_TYPE    topLockAccess;

    NV_STATUS status;
    RS_LOCK_INFO *pLockInfo = pParams->pLockInfo;
    NvU32         releaseFlags = 0;
    CALL_CONTEXT  callContext;
    CALL_CONTEXT *pOldContext = NULL;
    NvBool        bRestoreCallContext = NV_FALSE;

    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pParams->length == 0)
        return NV_ERR_INVALID_LIMIT;

    status = serverInterMapLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient,
                                           pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    if (!pClient->bActive)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hMapper, &pMapperRef);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hMappable, &pMappableRef);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hDevice, &pContextRef);
    if (status != NV_OK)
        goto done;

    pLockInfo->pContextRef = pContextRef;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClient;
    callContext.pResourceRef = pMapperRef;
    callContext.pContextRef = pContextRef;
    callContext.pLockInfo = pLockInfo;

    // Some MODS tests don't set secInfo.
    if (pParams->pSecInfo != NULL)
        callContext.secInfo = *pParams->pSecInfo;

    resservSwapTlsCallContext(&pOldContext, &callContext);
    bRestoreCallContext = NV_TRUE;

    status = refAddInterMapping(pMapperRef, pMappableRef, pContextRef, &pMapping);
    if (status != NV_OK)
        goto done;

    // serverResLock_Prologue should be called during serverInterMap_Prologue
    status = serverInterMap_Prologue(pServer, pMapperRef, pMappableRef, pParams, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = clientInterMap(pClient, pMapperRef, pMappableRef, pParams);
    if (status != NV_OK)
        goto done;

    pMapping->flags = pParams->flags;
    pMapping->dmaOffset = pParams->dmaOffset;
    pMapping->pMemDesc = pParams->pMemDesc;

done:
    serverInterMap_Epilogue(pServer, pParams, &releaseFlags);

    if (bRestoreCallContext)
        resservRestoreTlsCallContext(pOldContext);

    if (status != NV_OK)
    {
        if (pMapping != NULL)
            refRemoveInterMapping(pMapperRef, pMapping);
    }

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverInterUnmap
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    RsClient           *pClient;
    RsResourceRef      *pMapperRef;
    RsResourceRef      *pMappableRef;
    RsResourceRef      *pContextRef;
    RsInterMapping     *pMapping;
    LOCK_ACCESS_TYPE    topLockAccess;

    NV_STATUS status;
    RS_LOCK_INFO *pLockInfo = pParams->pLockInfo;
    NvU32         releaseFlags = 0;
    CALL_CONTEXT  callContext;
    CALL_CONTEXT *pOldContext = NULL;
    NvBool        bRestoreCallContext = NV_FALSE;

    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    status = serverInterUnmapLookupLockFlags(pServer, RS_LOCK_TOP, pParams, &topLockAccess);
    if (status != NV_OK)
        goto done;

    status = serverTopLock_Prologue(pServer, topLockAccess, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient,
                                           pLockInfo, &releaseFlags, &pClient);
    if (status != NV_OK)
        goto done;

    status = clientValidate(pClient, pParams->pSecInfo);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hMapper, &pMapperRef);
    if (status != NV_OK)
        goto done;

    if ((pMapperRef->bInvalidated) && (pMapperRef->pResource == NULL))
    {
        // Object has already been freed and unmapped
        goto done;
    }

    status = clientGetResourceRef(pClient, pParams->hMappable, &pMappableRef);
    if (status != NV_OK)
        goto done;

    status = clientGetResourceRef(pClient, pParams->hDevice, &pContextRef);
    if (status != NV_OK)
        goto done;

    status = refFindInterMapping(pMapperRef, pMappableRef, pContextRef, pParams->dmaOffset, &pMapping);
    if (status != NV_OK)
        goto done;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = pServer;
    callContext.pClient = pClient;
    callContext.pResourceRef = pMapperRef;
    callContext.pContextRef = pContextRef;
    callContext.pLockInfo = pLockInfo;

    // Some MODS tests don't set secInfo.
    if (pParams->pSecInfo != NULL)
        callContext.secInfo = *pParams->pSecInfo;

    if (pLockInfo->pContextRef == NULL)
        pLockInfo->pContextRef = pContextRef;

    resservSwapTlsCallContext(&pOldContext, &callContext);
    bRestoreCallContext = NV_TRUE;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = serverInterUnmap_Prologue(pServer, pParams);
    if (status != NV_OK)
        goto done;

    clientInterUnmap(pClient, pMapperRef, pParams);

    refRemoveInterMapping(pMapperRef, pMapping);

done:
    serverInterUnmap_Epilogue(pServer, pParams);

    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    if (bRestoreCallContext)
        resservRestoreTlsCallContext(pOldContext);

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient, pLockInfo, &releaseFlags);
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverAcquireClient
(
    RsServer *pServer,
    NvHandle hClient,
    LOCK_ACCESS_TYPE lockAccess,
    RsClient **ppClient
)
{
    NV_STATUS   status;
    RsClient   *pClient;

    // NV_PRINTF(LEVEL_INFO, "Acquiring hClient %x\n", hClient);
    status = _serverLockClient(pServer, lockAccess, hClient, &pClient);
    if (status != NV_OK)
        return status;

    if (ppClient != NULL)
        *ppClient = pClient;

    return NV_OK;
}

NV_STATUS
serverGetClientUnderLock
(
    RsServer   *pServer,
    NvHandle    hClient,
    RsClient  **ppClient
)
{
    NV_STATUS   status;
    RsClient   *pClient;

    // NV_PRINTF(LEVEL_INFO, "Acquiring hClient %x (without lock)\n", hClient);
    status = _serverFindClient(pServer, hClient, &pClient);
    if (status != NV_OK)
    {
        return status;
    }

    if (ppClient != NULL)
        *ppClient = pClient;

    return NV_OK;
}

NV_STATUS
serverReleaseClient
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE lockAccess,
    RsClient *pClient
)
{
    NV_STATUS status;
    status = _serverUnlockClient(pServer, lockAccess, pClient->hClient);
    return status;
}

static
NV_STATUS
_serverFindClientEntry
(
    RsServer      *pServer,
    NvHandle       hClient,
    NvBool         bFindPartial,
    CLIENT_ENTRY **ppClientEntry
)
{
    RsClientList  *pClientList       = &(pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK]);
    CLIENT_ENTRY **ppClientEntryLoop = listHead(pClientList);

    if (ppClientEntry != NULL)
        *ppClientEntry = NULL;

    while (ppClientEntryLoop != NULL)
    {
        CLIENT_ENTRY *pClientEntry = *ppClientEntryLoop;
        ppClientEntryLoop = listNext(pClientList, ppClientEntryLoop);
        if (pClientEntry == NULL)
        {
            continue;
        }
        else if (pClientEntry->hClient == hClient)
        {
            // Client may not have finished constructing yet
            if (pClientEntry->pClient == NULL && !bFindPartial)
                return NV_ERR_INVALID_OBJECT_HANDLE;

            if (ppClientEntry != NULL)
                *ppClientEntry = pClientEntry;

            return NV_OK;
        }
        else if (pClientEntry->hClient > hClient)
        {
            // Not found in sorted list
            return NV_ERR_INVALID_OBJECT;
        }
    }

    return NV_ERR_INVALID_OBJECT_HANDLE;
}

static
NV_STATUS
_serverFindClient
(
    RsServer      *pServer,
    NvHandle       hClient,
    RsClient     **ppClient
)
{
    CLIENT_ENTRY *pClientEntry;
    NV_STATUS status;
    status =_serverFindClientEntry(pServer, hClient, NV_FALSE, &pClientEntry);
    if (status != NV_OK)
    {
        return status;
    }

    *ppClient = pClientEntry->pClient;
    return NV_OK;
}

static
NV_STATUS
_serverInsertClientEntry
(
    RsServer      *pServer,
    CLIENT_ENTRY  *pClientEntry,
    CLIENT_ENTRY **ppClientNext
)
{
    RsClientList  *pClientList;
    CLIENT_ENTRY **ppClientEntry;
    NvHandle       hClient = pClientEntry->hClient;

    if (hClient == 0)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    pClientList  = &(pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK]);

    if (ppClientNext == NULL)
    {
        ppClientEntry = (CLIENT_ENTRY **)listAppendNew(pClientList);
    }
    else
    {
        ppClientEntry = (CLIENT_ENTRY **)listInsertNew(pClientList, ppClientNext);
    }
    *ppClientEntry = pClientEntry;

    return NV_OK;
}

static
NV_STATUS
_serverFindNextAvailableClientHandleInBucket
(
    RsServer        *pServer,
    NvHandle         hClientIn,
    NvHandle         *phClientOut,
    CLIENT_ENTRY  ***pppClientNext
)
{
    NvHandle        hPrefixIn, hPrefixOut;
    RsClientList   *pClientList  = &(pServer->pClientSortedList[hClientIn & RS_CLIENT_HANDLE_BUCKET_MASK]);
    NvHandle        hClientOut   = hClientIn;
    CLIENT_ENTRY **ppClientEntry = listHead(pClientList);

    *pppClientNext = NULL;
    if (ppClientEntry == NULL)
    {
        *phClientOut = hClientOut;
        return NV_OK;
    }

    //
    // The list is ordered by increased client handles
    // We need to find a value to insert or change the handle
    //
    while (ppClientEntry != NULL)
    {
        CLIENT_ENTRY *pClientEntry = *ppClientEntry;
        if ((pClientEntry == NULL) || (pClientEntry->hClient < hClientOut))
        {
            ppClientEntry = listNext(pClientList, ppClientEntry);
            continue;
        }
        else if (pClientEntry->hClient == hClientOut)
        {
            // Increase client handle by one unit in same bucket
            hClientOut = hClientOut + RS_CLIENT_HANDLE_BUCKET_COUNT;
            NV_ASSERT((hClientIn & RS_CLIENT_HANDLE_BUCKET_MASK) == (hClientOut & RS_CLIENT_HANDLE_BUCKET_MASK));
        }
        else // last pClientEntry->hClient > hClientOut
        {
            break;
        }
        ppClientEntry = listNext(pClientList, ppClientEntry);
    }

    hPrefixIn = hClientIn & ~RS_CLIENT_HANDLE_DECODE_MASK;
    hPrefixOut = hClientOut & ~RS_CLIENT_HANDLE_DECODE_MASK;
    if (hPrefixIn != hPrefixOut)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    *phClientOut = hClientOut;
    if (ppClientEntry != NULL)
    {
        *pppClientNext = ppClientEntry;
    }
    return  NV_OK;
}


static
NV_STATUS
_serverCreateEntryAndLockForNewClient
(
    RsServer      *pServer,
    NvHandle      *phClient,
    NvBool         bInternalHandle,
    CLIENT_ENTRY **ppClientEntry
)
{
    CLIENT_ENTRY  *pClientEntry;
    NV_STATUS      status = NV_OK;
    NvHandle       hClient = *phClient;
    CLIENT_ENTRY **ppClientNext = 0;
    PORT_RWLOCK    *pLock=NULL;

    if (hClient == 0)
    {
        NvU32 clientHandleIndex = pServer->clientCurrentHandleIndex;
        NvU16 clientHandleBucketInit = clientHandleIndex & RS_CLIENT_HANDLE_BUCKET_MASK;
        do
        {
            hClient = bInternalHandle
                ? CLIENT_ENCODEHANDLE_INTERNAL(pServer->internalHandleBase, clientHandleIndex)
                : CLIENT_ENCODEHANDLE(clientHandleIndex);

            clientHandleIndex++;
            if (clientHandleIndex > RS_CLIENT_HANDLE_DECODE_MASK)
            {
                // We will override the client base, loop over
                clientHandleIndex = 0;
            }
            if (clientHandleBucketInit == (clientHandleIndex & RS_CLIENT_HANDLE_BUCKET_MASK))
            {
                // We looked through all buckets and we did not find any available client (very unlikely)
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto _serverCreateEntryAndLockForNewClient_exit;
            }
        }
        while (_serverFindNextAvailableClientHandleInBucket(pServer, hClient, &hClient, &ppClientNext) != NV_OK);

        pServer->clientCurrentHandleIndex = clientHandleIndex;
    }
    else
    {
        NvHandle hClientOut = 0;

#if !(RS_COMPATABILITY_MODE)
        // Re-encode handle so it matches expected format
        NvU32 clientIndex = CLIENT_DECODEHANDLE(hClient);
        hClient = bInternalHandle
            ? CLIENT_ENCODEHANDLE_INTERNAL(clientIndex)
            : CLIENT_ENCODEHANDLE(clientIndex);
#endif

        if (_serverFindClientEntry(pServer, hClient, NV_FALSE, NULL) == NV_OK)
        {
            // The handle already exists
            status = NV_ERR_INSERT_DUPLICATE_NAME;
            goto _serverCreateEntryAndLockForNewClient_exit;
        }
        status = _serverFindNextAvailableClientHandleInBucket(pServer, hClient, &hClientOut, &ppClientNext);
        if (status != NV_OK)
        {
             goto _serverCreateEntryAndLockForNewClient_exit;
        }
        if (hClient != hClientOut)
        {
            // This should not happen as we checked for duplicates already
            NV_PRINTF(LEVEL_ERROR, "Client handle mismatch: %x != %x.\n", hClient, hClientOut);
            status = NV_ERR_INVALID_STATE;
            goto _serverCreateEntryAndLockForNewClient_exit;
        }
    }

    pLock = portSyncRwLockCreate(pServer->pAllocator);
    if (pLock == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto _serverCreateEntryAndLockForNewClient_exit;
    }

    // At this point we have a hClient,  we know in which bucket and where in the bucket to insert the entry.
    pClientEntry = (CLIENT_ENTRY *)PORT_ALLOC(pServer->pAllocator, sizeof(CLIENT_ENTRY));
    if (pClientEntry == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto _serverCreateEntryAndLockForNewClient_exit;
    }
    portMemSet(pClientEntry, 0, sizeof(*pClientEntry));

    pClientEntry->hClient = hClient;
    pClientEntry->pLock = pLock;


    RS_LOCK_VALIDATOR_INIT(&pClientEntry->lockVal,
                           bInternalHandle ? LOCK_VAL_LOCK_CLASS_CLIENT_INTERNAL : LOCK_VAL_LOCK_CLASS_CLIENT,
                           hClient);

    status = _serverInsertClientEntry(pServer, pClientEntry, ppClientNext);
    if (status != NV_OK)
    {
        PORT_FREE(pServer->pAllocator, pClientEntry);
        goto _serverCreateEntryAndLockForNewClient_exit;
    }

    RS_RWLOCK_ACQUIRE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
    pClientEntry->lockOwnerTid = portThreadGetCurrentThreadId();

    *phClient = hClient;
    *ppClientEntry = pClientEntry;

_serverCreateEntryAndLockForNewClient_exit:
    if (status != NV_OK && pLock != NULL)
        portSyncRwLockDestroy(pLock);

    return status;
}


static
NV_STATUS
_serverLockClient
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient,
    RsClient **ppClient
)
{
    RsClient *pClient;
    CLIENT_ENTRY *pClientEntry = NULL;
    NV_STATUS status = NV_OK;

    status =_serverFindClientEntry(pServer, hClient, NV_FALSE, &pClientEntry);
    if (status != NV_OK)
    {
        return status;
    }

    nv_speculation_barrier();

    if (pClientEntry->pLock == NULL)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    if (access == LOCK_ACCESS_READ)
    {
        RS_RWLOCK_ACQUIRE_READ(pClientEntry->pLock, &pClientEntry->lockVal);
    }
    else
    {
        RS_RWLOCK_ACQUIRE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
        pClientEntry->lockOwnerTid = portThreadGetCurrentThreadId();
    }

    pClient = pClientEntry->pClient;
    NV_ASSERT(pClient->hClient == pClientEntry->hClient);

    if ((pClient == NULL) || (pClient->hClient != hClient))
    {
        if (access == LOCK_ACCESS_READ)
            RS_RWLOCK_RELEASE_READ(pClientEntry->pLock, &pClientEntry->lockVal);
        else
            RS_RWLOCK_RELEASE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);

        return NV_ERR_INVALID_OBJECT;
    }

    if (ppClient != NULL)
        *ppClient = pClient;

    return NV_OK;
}

static
NV_STATUS
_serverLockClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags,
    RsClient **ppClient
)
{
    NV_STATUS status;
    if ((pLockInfo->flags & RS_LOCK_FLAGS_NO_CLIENT_LOCK))
    {
        status = _serverFindClient(pServer, hClient, ppClient);
        return status;
    }

    if ((pLockInfo->state & RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED))
    {
        CLIENT_ENTRY *pClientEntry;
        NV_ASSERT_OK_OR_RETURN(_serverFindClientEntry(pServer, hClient, NV_FALSE, &pClientEntry));
        NV_ASSERT_OR_RETURN(pLockInfo->pClient != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pLockInfo->pClient == pClientEntry->pClient, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId(), NV_ERR_INVALID_STATE);

        *ppClient = pLockInfo->pClient;
        return NV_OK;
    }

    status = _serverLockClient(pServer, access, hClient, ppClient);
    if (status != NV_OK)
        return status;

    pLockInfo->state |= RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
    pLockInfo->pClient = *ppClient;
    *pReleaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;

    return NV_OK;
}

static
NV_STATUS
_serverLockDualClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient1,
    NvHandle hClient2,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags,
    RsClient **ppClient1,
    RsClient **ppClient2
)
{
    NV_STATUS status;

    // 1st and 2nd in handle order, as opposed to fixed 1 and 2
    NvHandle    hClient1st;
    NvHandle    hClient2nd;
    RsClient  **ppClient1st;
    RsClient  **ppClient2nd;

    *ppClient1 = NULL;
    *ppClient2 = NULL;

    if ((pLockInfo->flags & RS_LOCK_FLAGS_NO_CLIENT_LOCK))
    {
        status = _serverFindClient(pServer, hClient1, ppClient1);
        if (status != NV_OK)
            return status;

        if (hClient1 == hClient2)
        {
            *ppClient2 = *ppClient1;
        }
        else
        {
            status = _serverFindClient(pServer, hClient2, ppClient2);
        }

        return status;
    }

    if (hClient1 <= hClient2)
    {
        hClient1st = hClient1;
        ppClient1st = ppClient1;

        hClient2nd = hClient2;
        ppClient2nd = ppClient2;
    }
    else
    {
        hClient1st = hClient2;
        ppClient1st = ppClient2;

        hClient2nd = hClient1;
        ppClient2nd = ppClient1;
    }

    if ((pLockInfo->state & RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED))
    {
        CLIENT_ENTRY *pClientEntry, *pSecondClientEntry;

        NV_ASSERT_OR_RETURN(pLockInfo->pSecondClient != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pLockInfo->pClient->hClient == hClient1st, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pLockInfo->pSecondClient->hClient == hClient2nd, NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(_serverFindClientEntry(pServer, hClient1st, NV_FALSE, &pClientEntry));
        NV_ASSERT_OR_RETURN(pClientEntry->pClient == pLockInfo->pClient, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId(), NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(_serverFindClientEntry(pServer, hClient2nd, NV_FALSE, &pSecondClientEntry));
        NV_ASSERT_OR_RETURN(pSecondClientEntry->pClient == pLockInfo->pSecondClient, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pSecondClientEntry->lockOwnerTid == pClientEntry->lockOwnerTid, NV_ERR_INVALID_STATE);

        *ppClient1st = pLockInfo->pClient;
        *ppClient2nd = pLockInfo->pSecondClient;
        return NV_OK;
    }

    status = _serverLockClient(pServer, access, hClient1st, ppClient1st);
    if (status != NV_OK)
        return status;

    if (hClient1 == hClient2)
    {
        *ppClient2nd = *ppClient1st;
    }
    else
    {
        status = _serverLockClient(pServer, access, hClient2nd, ppClient2nd);
        if (status != NV_OK)
        {
            _serverUnlockClient(pServer, access, hClient1st);
            return status;
        }
    }

    pLockInfo->state |= RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
    pLockInfo->pClient = *ppClient1st;
    pLockInfo->pSecondClient = *ppClient2nd;
    *pReleaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;

    return NV_OK;
}

static
NV_STATUS
_serverUnlockClient
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient
)
{
    CLIENT_ENTRY *pClientEntry = NULL;
    NV_STATUS status = NV_OK;

    status =_serverFindClientEntry(pServer, hClient, NV_TRUE, &pClientEntry);
    if (status != NV_OK)
    {
        return status;
    }

    if (access == LOCK_ACCESS_READ)
    {
        RS_RWLOCK_RELEASE_READ(pClientEntry->pLock, &pClientEntry->lockVal);
    }
    else
    {
        pClientEntry->lockOwnerTid = ~0;
        RS_RWLOCK_RELEASE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
    }

    return NV_OK;
}

static
NV_STATUS
_serverUnlockClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    NV_STATUS status;
    if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
    {
        status = _serverUnlockClient(pServer, access, hClient);
        if (status != NV_OK)
            return status;

        pLockInfo->state &= ~RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
        pLockInfo->pClient = NULL;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_CLIENT_LOCK;
    }
    return NV_OK;
}

static
NV_STATUS
_serverUnlockDualClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient1,
    NvHandle hClient2,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    // 1st and 2nd in handle order, as opposed to fixed 1 and 2
    NvHandle    hClient1st = NV_MIN(hClient1, hClient2);
    NvHandle    hClient2nd = NV_MAX(hClient1, hClient2);

    if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
    {
        // Try to unlock both, even if one fails
        NV_ASSERT_OK(_serverUnlockClient(pServer, access, hClient2nd));
        if (hClient1 != hClient2)
            NV_ASSERT_OK(_serverUnlockClient(pServer, access, hClient1st));

        pLockInfo->state &= ~RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
        pLockInfo->pClient = NULL;
        pLockInfo->pSecondClient = NULL;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_CLIENT_LOCK;
    }

    return NV_OK;
}

NvU32
serverGetClientCount(RsServer *pServer)
{
    return pServer->activeClientCount;
}

NvU64
serverGetResourceCount(RsServer *pServer)
{
    return pServer->activeResourceCount;
}

NV_STATUS
resservSwapTlsCallContext
(
    CALL_CONTEXT **ppOldCallContext,
    CALL_CONTEXT *pNewCallContext
)
{
    CALL_CONTEXT **ppTlsCallContext;

    if (ppOldCallContext == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    ppTlsCallContext = (CALL_CONTEXT**)tlsEntryAcquire(TLS_ENTRY_ID_RESSERV_CALL_CONTEXT);
    if (ppTlsCallContext == NULL)
        return NV_ERR_INVALID_STATE;

    *ppOldCallContext = *ppTlsCallContext;
    *ppTlsCallContext = pNewCallContext;

    // New call contexts inherit the bDeferredApi flag from the old
    if ((*ppOldCallContext != NULL) && (pNewCallContext != NULL) &&
        (pNewCallContext->pControlParams != NULL) &&
        ((*ppOldCallContext)->pControlParams != NULL))
    {
        pNewCallContext->pControlParams->bDeferredApi |=
            (*ppOldCallContext)->pControlParams->bDeferredApi;
    }

    return NV_OK;
}

CALL_CONTEXT *
resservGetTlsCallContext(void)
{
    CALL_CONTEXT *pTlsCallContext = NvP64_VALUE(tlsEntryGet(TLS_ENTRY_ID_RESSERV_CALL_CONTEXT));
    return pTlsCallContext;
}

NV_STATUS
resservRestoreTlsCallContext
(
    CALL_CONTEXT *pOldCallContext
)
{
    CALL_CONTEXT **ppTlsCallContext = (CALL_CONTEXT**)tlsEntryAcquire(TLS_ENTRY_ID_RESSERV_CALL_CONTEXT);
    if (ppTlsCallContext == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *ppTlsCallContext = pOldCallContext;
    tlsEntryRelease(TLS_ENTRY_ID_RESSERV_CALL_CONTEXT);
    tlsEntryRelease(TLS_ENTRY_ID_RESSERV_CALL_CONTEXT);

    return NV_OK;
}

RsResourceRef *
resservGetContextRefByType(NvU32 internalClassId, NvBool bSearchAncestors)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RsResourceRef *pContextRef = NULL;

    if (pCallContext == NULL)
        return NULL;

    if (pCallContext->pResourceRef != NULL)
    {
        if (pCallContext->pResourceRef->internalClassId == internalClassId)
        {
            return pCallContext->pResourceRef;
        }
        else if (bSearchAncestors &&
                 (refFindAncestorOfType(pCallContext->pResourceRef, internalClassId, &pContextRef) == NV_OK))
        {
            return pContextRef;
        }
    }

    if (pCallContext->pContextRef != NULL)
    {
       if (pCallContext->pContextRef->internalClassId == internalClassId)
       {
           return pCallContext->pContextRef;
       }
       else if (bSearchAncestors &&
                (refFindAncestorOfType(pCallContext->pContextRef, internalClassId, &pContextRef) == NV_OK))
       {
           return pContextRef;
       }
    }

    return NULL;
}

NV_STATUS serverFreeClient(RsServer *pServer, RS_CLIENT_FREE_PARAMS* pParams)
{
    RS_RES_FREE_PARAMS params;
    RS_LOCK_INFO lockInfo;

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    portMemSet(&params, 0, sizeof(params));
    params.hClient = pParams->hClient;
    params.hResource = pParams->hClient;
    params.bHiPriOnly = pParams->bHiPriOnly;
    lockInfo.state = pParams->state;
    params.pLockInfo = &lockInfo;
    params.pSecInfo = pParams->pSecInfo;

    return serverFreeResourceTree(pServer, &params);
}

NV_STATUS
shrConstruct_IMPL
(
    RsShared *pShare
)
{
    return NV_OK;
}

void
shrDestruct_IMPL
(
    RsShared *pShare
)
{
}

NV_STATUS
sessionConstruct_IMPL
(
    RsSession *pSession
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    pSession->bValid = NV_TRUE;
    listInit(&pSession->dependencies, pCallContext->pServer->pAllocator);
    listInit(&pSession->dependants, pCallContext->pServer->pAllocator);
    pSession->pLock = portSyncRwLockCreate(pCallContext->pServer->pAllocator);

    RS_LOCK_VALIDATOR_INIT(&pSession->lockVal, LOCK_VAL_LOCK_CLASS_SESSION, LOCK_VAL_LOCK_GENERATE);
    return NV_OK;
}

void
sessionDestruct_IMPL
(
    RsSession *pSession
)
{
    NV_ASSERT(listCount(&pSession->dependencies) == 0);
    NV_ASSERT(listCount(&pSession->dependants) == 0);
    listDestroy(&pSession->dependencies);
    listDestroy(&pSession->dependants);
    pSession->pLock = NULL;
}

NV_STATUS
sessionAddDependant_IMPL
(
    RsSession *pSession,
    RsResourceRef *pResourceRef
)
{
    NV_STATUS status;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    status = sessionCheckLocksForAdd(pSession, pResourceRef);

    if (status != NV_OK)
        return status;

    if (pResourceRef->pSession == pSession)
        return NV_OK;

    NV_ASSERT_OR_RETURN(pResourceRef->pSession == NULL, NV_ERR_INVALID_ARGUMENT);

    if (listAppendValue(&pSession->dependants, &pResourceRef) == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    serverRefShare(pCallContext->pServer, staticCast(pSession, RsShared));

    pResourceRef->pSession = pSession;

    return NV_OK;
}

NV_STATUS
sessionAddDependency_IMPL
(
    RsSession *pSession,
    RsResourceRef *pResourceRef
)
{
    NV_STATUS status;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    status = sessionCheckLocksForAdd(pSession, pResourceRef);

    if (status != NV_OK)
        return status;

    if (pResourceRef->pDependantSession == pSession)
        return NV_OK;

    NV_ASSERT_OR_RETURN(pResourceRef->pDependantSession == NULL, NV_ERR_INVALID_ARGUMENT);

    if (listAppendValue(&pSession->dependencies, &pResourceRef) == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    serverRefShare(pCallContext->pServer, staticCast(pSession, RsShared));

    pResourceRef->pDependantSession = pSession;

    return NV_OK;
}

void
sessionRemoveDependant_IMPL
(
    RsSession *pSession,
    RsResourceRef *pResourceRef
)
{
    listRemoveFirstByValue(&pSession->dependants, &pResourceRef);
    sessionCheckLocksForRemove(pSession, pResourceRef);
    pResourceRef->pSession = NULL;
}

void
sessionRemoveDependency_IMPL
(
    RsSession *pSession,
    RsResourceRef *pResourceRef
)
{
    listRemoveFirstByValue(&pSession->dependencies, &pResourceRef);
    pSession->bValid = NV_FALSE;
    sessionCheckLocksForRemove(pSession, pResourceRef);
    pResourceRef->pDependantSession = NULL;
}

NV_STATUS sessionCheckLocksForAdd_IMPL(RsSession *pSession, RsResourceRef *pResourceRef)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_LOCK_INFO *pLockInfo;

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
    pLockInfo = pCallContext->pLockInfo;

    NV_ASSERT_OR_RETURN((pLockInfo != NULL), NV_ERR_INVALID_STATE);

    if (!serverRwApiLockIsOwner(pCallContext->pServer))
    {
        // Assert clients locked or RW lock
        if (pLockInfo->state & RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED)
        {
            NV_ASSERT_OR_RETURN((pLockInfo->pClient == pResourceRef->pClient) ||
                                (pLockInfo->pSecondClient == pResourceRef->pClient),
                                NV_ERR_INVALID_ARGUMENT);
        }
        else if (pLockInfo->state & RS_LOCK_STATE_TOP_LOCK_ACQUIRED)
        {
            NV_ASSERT_OR_RETURN((pLockInfo->pClient == NULL) && (pLockInfo->pSecondClient == NULL), NV_ERR_INVALID_ARGUMENT);
        }
        else
        {
            NV_ASSERT_FAILED("Incorrect locks taken");
            return NV_ERR_INVALID_LOCK_STATE;
        }
    }

    return NV_OK;
}

void sessionCheckLocksForRemove_IMPL(RsSession *pSession, RsResourceRef *pResourceRef)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_LOCK_INFO *pLockInfo;

    NV_ASSERT(pCallContext != NULL);
    pLockInfo = pCallContext->pLockInfo;

    NV_ASSERT(pLockInfo != NULL);

    if (pLockInfo->flags & RS_LOCK_FLAGS_FREE_SESSION_LOCK)
    {
        RsShared *pShared = staticCast(pSession, RsShared);
        PORT_RWLOCK *pSessionLock = pSession->pLock;
        NvBool bDestroy = (pShared->refCount == 1);

        if (!(pLockInfo->state & RS_LOCK_STATE_SESSION_LOCK_ACQUIRED) || !bDestroy)
        {
            serverFreeShare(pCallContext->pServer, pShared);
            pLockInfo->flags &= ~RS_LOCK_FLAGS_FREE_SESSION_LOCK;
        }

        if (!(pLockInfo->state & RS_LOCK_STATE_SESSION_LOCK_ACQUIRED) && bDestroy)
            portSyncRwLockDestroy(pSessionLock);
    }
}

NV_STATUS
serverAllocShareWithHalspecParent
(
    RsServer  *pServer,
    const NVOC_CLASS_INFO *pClassInfo,
    RsShared **ppShare,
    Object    *pHalspecParent
)
{
    RsShared   *pShare;
    NV_STATUS   status;
    Dynamic    *pDynamic = NULL;
    NvU32       flags    = NVOC_OBJ_CREATE_FLAGS_NONE;

    if (pClassInfo == NULL)
        return NV_ERR_INVALID_CLASS;

    if (pHalspecParent != NULL)
        flags |= NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY;

    status = objCreateDynamicWithFlags(&pDynamic,
                                       pHalspecParent,
                                       (const NVOC_CLASS_INFO*)(const void*)pClassInfo,
                                       flags); 
    if (status != NV_OK)
        return status;

    if (pDynamic == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pShare = dynamicCast(pDynamic, RsShared);
    if (pShare == NULL)
    {
        status = NV_ERR_INVALID_CLASS;
        goto fail;
    }

    pShare->refCount = 1;

    portSyncSpinlockAcquire(pServer->pShareMapLock);
    if (mapInsertExisting(&pServer->shareMap, (NvUPtr)pShare, pShare) != NV_TRUE)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        portSyncSpinlockRelease(pServer->pShareMapLock);
        goto fail;
    }
    portSyncSpinlockRelease(pServer->pShareMapLock);

    if (ppShare != NULL)
        *ppShare = pShare;

    return NV_OK;

fail:
    if (pShare != NULL)
    {
        objDelete(pShare);
    }

    return status;
}

NV_STATUS
serverAllocShare
(
    RsServer *pServer,
    const NVOC_CLASS_INFO *pClassInfo,
    RsShared **ppShare
)
{
    return serverAllocShareWithHalspecParent(pServer, pClassInfo, ppShare, NULL);
}

NvS32
serverGetShareRefCount
(
    RsServer *pServer,
    RsShared *pShare
)
{
    return pShare->refCount;
}

NV_STATUS
serverRefShare
(
    RsServer *pServer,
    RsShared *pShare
)
{
    portAtomicIncrementS32(&pShare->refCount);
    return NV_OK;
}

NV_STATUS
serverFreeShare
(
    RsServer *pServer,
    RsShared *pShare
)
{
    if (portAtomicDecrementS32(&pShare->refCount) == 0)
    {
        portSyncSpinlockAcquire(pServer->pShareMapLock);
        mapRemove(&pServer->shareMap, pShare);
        portSyncSpinlockRelease(pServer->pShareMapLock);

        objDelete(pShare);
    }
    return NV_OK;
}

RS_SHARE_ITERATOR
serverShareIter
(
    RsServer *pServer,
    NvU32 internalClassId
)
{
    RS_SHARE_ITERATOR it;
    portMemSet(&it, 0, sizeof(it));
    it.internalClassId = internalClassId;
    it.mapIt = mapIterAll(&pServer->shareMap);

    return it;
}

NvBool
serverShareIterNext
(
    RS_SHARE_ITERATOR* pIt
)
{
    NvBool bLoop = NV_TRUE;
    if (pIt == NULL)
        return NV_FALSE;

    pIt->pShared = NULL;
    bLoop = mapIterNext(&pIt->mapIt);
    while(bLoop)
    {
        RsShared *pShared = pIt->mapIt.pValue;
        if ((pIt->internalClassId == 0) || (objDynamicCastById(pShared, pIt->internalClassId) != NULL))
        {
            pIt->pShared = pShared;
            return NV_TRUE;
        }
        bLoop = mapIterNext(&pIt->mapIt);
    }

    return NV_FALSE;
}

#if (RS_PROVIDES_API_STATE)
NV_STATUS
serverAllocApiCopyIn
(
    RsServer *pServer,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams,
    API_STATE **ppApiState
)
{
    if (ppApiState != NULL)
        *ppApiState = NULL;

    return NV_OK;
}

NV_STATUS
serverAllocApiCopyOut
(
    RsServer *pServer,
    NV_STATUS status,
    API_STATE *pApiState
)
{
    return status;
}
#endif

#if (RS_STANDALONE)
NV_STATUS
serverAllocEpilogue_WAR
(
    RsServer *pServer,
    NV_STATUS status,
    NvBool bClientAlloc,
    RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams
)
{
    return status;
}

NV_STATUS
serverLookupSecondClient
(
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NvHandle *phClient
)
{
    *phClient = 0;

    return NV_OK;
}

NV_STATUS serverTopLock_Prologue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if ((pLockInfo->flags & RS_LOCK_FLAGS_NO_TOP_LOCK))
        return NV_OK;

    if (!(pLockInfo->state & RS_LOCK_STATE_TOP_LOCK_ACQUIRED))
    {
        if (access == LOCK_ACCESS_READ)
        {
            RS_RWLOCK_ACQUIRE_READ(pServer->pTopLock, &pServer->topLockVal);
        }
        else
        {
            RS_RWLOCK_ACQUIRE_WRITE(pServer->pTopLock, &pServer->topLockVal);
            pServer->topLockOwnerTid = portThreadGetCurrentThreadId();
        }

        pLockInfo->state |= RS_LOCK_STATE_TOP_LOCK_ACQUIRED;
        *pReleaseFlags |= RS_LOCK_RELEASE_TOP_LOCK;
    }
    else if (access == LOCK_ACCESS_WRITE)
    {
        NV_ASSERT_OR_RETURN(pServer->topLockOwnerTid == portThreadGetCurrentThreadId(),
                            NV_ERR_INVALID_LOCK_STATE);
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
    if (*pReleaseFlags & RS_LOCK_RELEASE_TOP_LOCK)
    {
        if (access == LOCK_ACCESS_READ)
            RS_RWLOCK_RELEASE_READ(pServer->pTopLock, &pServer->topLockVal);
        else
        {
            pServer->topLockOwnerTid = ~0;
            RS_RWLOCK_RELEASE_WRITE(pServer->pTopLock, &pServer->topLockVal);
        }

        pLockInfo->state &= ~RS_LOCK_STATE_TOP_LOCK_ACQUIRED;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_TOP_LOCK;
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
    if (!(pLockInfo->state & RS_LOCK_STATE_CUSTOM_LOCK_1_ACQUIRED))
    {
        if (access == LOCK_ACCESS_READ)
            RS_RWLOCK_ACQUIRE_READ(pServer->pResLock, &pServer->resLockVal);
        else
            RS_RWLOCK_ACQUIRE_WRITE(pServer->pResLock, &pServer->resLockVal);

        pLockInfo->state |= RS_LOCK_STATE_CUSTOM_LOCK_1_ACQUIRED;
        *pReleaseFlags |= RS_LOCK_RELEASE_CUSTOM_LOCK_1;
    }

    return NV_OK;
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
    if (*pReleaseFlags & RS_LOCK_RELEASE_CUSTOM_LOCK_1)
    {
        if (access == LOCK_ACCESS_READ)
            RS_RWLOCK_RELEASE_READ(pServer->pResLock, &pServer->resLockVal);
        else
            RS_RWLOCK_RELEASE_WRITE(pServer->pResLock, &pServer->resLockVal);

        pLockInfo->state &= ~RS_LOCK_STATE_CUSTOM_LOCK_1_ACQUIRED;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_CUSTOM_LOCK_1;
    }
}

#if !(RS_STANDALONE_TEST)
NV_STATUS
serverMap_Prologue
(
    RsServer *pServer,
    RS_CPU_MAP_PARAMS *pMapParams
)
{
    return NV_OK;
}
#endif /* !RS_STANDALONE_TEST */

void
serverMap_Epilogue
(
    RsServer *pServer,
    RS_CPU_MAP_PARAMS *pMapParams
)
{
}

#if !(RS_STANDALONE_TEST)
NV_STATUS
serverUnmap_Prologue
(
    RsServer *pServer,
    RS_CPU_UNMAP_PARAMS *pUnmapParams
)
{
    return NV_OK;
}
#endif /* !RS_STANDALONE_TEST */

void
serverUnmap_Epilogue
(
    RsServer *pServer,
    RS_CPU_UNMAP_PARAMS *pUnmapParams
)
{
}

void
serverControl_InitCookie
(
    const struct NVOC_EXPORTED_METHOD_DEF *pExportedEntry,
    RS_CONTROL_COOKIE *pCookie
)
{
}

NV_STATUS
serverInterMap_Prologue
(
    RsServer *pServer,
    RsResourceRef *pMapperRef,
    RsResourceRef *pMappableRef,
    RS_INTER_MAP_PARAMS *pMapParams,
    NvU32 *pReleaseFlags
)
{
    NV_STATUS status;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pMapParams->pLockInfo, pReleaseFlags);

    return status;
}

void
serverInterMap_Epilogue
(
    RsServer *pServer,
    RS_INTER_MAP_PARAMS *pMapParams,
    NvU32 *pReleaseFlags
)
{
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pMapParams->pLockInfo, pReleaseFlags);
}

NV_STATUS
serverInterUnmap_Prologue
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pUnmapParams
)
{
    return NV_OK;
}

void
serverInterUnmap_Epilogue
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pUnmapParams
)
{
}

NvBool
serverRwApiLockIsOwner
(
    RsServer *pServer
)
{
    return (pServer->topLockOwnerTid == portThreadGetCurrentThreadId());
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
    if (lock == RS_LOCK_TOP)
    {
        NvBool bClientAlloc = (pParams->externalClassId == NV01_ROOT ||
                pParams->externalClassId == NV01_ROOT_CLIENT ||
                pParams->externalClassId == NV01_ROOT_NON_PRIV);

        if (bClientAlloc)
        {
            *pAccess = LOCK_ACCESS_WRITE;
            return NV_OK;
        }
    }

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_ALLOC_RESOURCE))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
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
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_FREE_RESOURCE))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverCopyResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_DUP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_COPY))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverShareResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_SHARE_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_SHARE))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
}

#if !(RS_STANDALONE_TEST)
NV_STATUS
serverControlLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    RS_CONTROL_COOKIE *pCookie,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_CTRL))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
}
#endif

NV_STATUS
serverMapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_CPU_MAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_MAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverUnmapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_CPU_UNMAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_UNMAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
}

NV_STATUS
serverInterMapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_INTER_MAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_INTER_MAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
}

NV_STATUS
serverInterUnmapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_INTER_UNMAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_INTER_UNMAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;

    return NV_OK;
}

NV_STATUS
serverControl_ValidateCookie
(
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    RS_CONTROL_COOKIE *pCookie
)
{
    return NV_OK;
}

NV_STATUS
serverControlApiCopyIn
(
    RsServer *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    RS_CONTROL_COOKIE *pCookie
)
{
    return NV_OK;
}

NV_STATUS
serverControlApiCopyOut
(
    RsServer *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    RS_CONTROL_COOKIE *pCookie,
    NV_STATUS rmStatus
)
{
    return NV_OK;
}

NV_STATUS
serverInitGlobalSharePolicies
(
    RsServer *pServer
)
{
    return NV_OK;
}
#endif

NV_STATUS
serverSessionLock_Prologue
(
    LOCK_ACCESS_TYPE access,
    RsResourceRef *pResourceRef,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    RsSession *pSession = pResourceRef->pSession;
    RsSession *pDependantSession = pResourceRef->pDependantSession;

    if (!(pLockInfo->state & RS_LOCK_STATE_SESSION_LOCK_ACQUIRED))
    {
        if (pSession != NULL)
        {
            if (access == LOCK_ACCESS_READ)
                RS_RWLOCK_ACQUIRE_READ(pSession->pLock, &pSession->lockVal);
            else
                RS_RWLOCK_ACQUIRE_WRITE(pSession->pLock, &pSession->lockVal);
            pLockInfo->state |= RS_LOCK_STATE_SESSION_LOCK_ACQUIRED;
            *pReleaseFlags |= RS_LOCK_RELEASE_SESSION_LOCK;

            pLockInfo->pSession = pSession;
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(pLockInfo->pSession == pSession, NV_ERR_INVALID_LOCK_STATE);
    }

    if (!(pLockInfo->flags & RS_LOCK_FLAGS_NO_DEPENDANT_SESSION_LOCK) &&
        (pDependantSession != NULL))
    {
        if (!(pLockInfo->state & RS_LOCK_STATE_SESSION_LOCK_ACQUIRED))
        {
            //
            // The only reason we lock the back reference session is if we're freeing the
            // resource so take the write lock in all cases
            //
            RS_RWLOCK_ACQUIRE_WRITE(pDependantSession->pLock, &pDependantSession->lockVal);

            pLockInfo->state |= RS_LOCK_STATE_SESSION_LOCK_ACQUIRED;
            *pReleaseFlags |= RS_LOCK_RELEASE_SESSION_LOCK;

            pLockInfo->pSession = pDependantSession;
        }
        else
        {
            //
            // For now, don't allow a resource to be both depended on and depending on a
            // session to keep this locking code simpler. We'll have to revisit if that
            // becomes necessary.
            //
            NV_ASSERT_OR_RETURN(pLockInfo->pSession == pDependantSession, NV_ERR_INVALID_LOCK_STATE);
        }
    }

    pLockInfo->flags &= ~RS_LOCK_FLAGS_NO_DEPENDANT_SESSION_LOCK;

    return NV_OK;
}

void
serverSessionLock_Epilogue
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    RsSession *pSession = pLockInfo->pSession;

    if ((pSession != NULL) && (*pReleaseFlags & RS_LOCK_RELEASE_SESSION_LOCK))
    {
        if (access == LOCK_ACCESS_READ)
            RS_RWLOCK_RELEASE_READ(pSession->pLock, &pSession->lockVal);
        else
            RS_RWLOCK_RELEASE_WRITE(pSession->pLock, &pSession->lockVal);

        pLockInfo->state &= ~RS_LOCK_STATE_SESSION_LOCK_ACQUIRED;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_SESSION_LOCK;

        if (pLockInfo->flags & RS_LOCK_FLAGS_FREE_SESSION_LOCK)
        {
            RsShared *pShared = staticCast(pSession, RsShared);
            PORT_RWLOCK *pSessionLock = pSession->pLock;

            serverFreeShare(pServer, pShared);
            portSyncRwLockDestroy(pSessionLock);
        }

        pLockInfo->pSession = NULL;
    }

    pLockInfo->flags &= ~RS_LOCK_FLAGS_FREE_SESSION_LOCK;
}

NV_STATUS serverControl_Prologue
(
    RsServer *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    LOCK_ACCESS_TYPE *pAccess,
    NvU32* pReleaseFlags
)
{
    NV_STATUS status;
    RS_LOCK_INFO *pLockInfo = pParams->pLockInfo;

    status = serverControl_ValidateCookie(pParams, pParams->pCookie);
    if (status != NV_OK)
        return status;

    serverControlLookupLockFlags(pServer, RS_LOCK_RESOURCE, pParams, pParams->pCookie, pAccess);
    if (status != NV_OK)
        return status;

    if (!pServer->bUnlockedParamCopy)
    {
        status = serverControlApiCopyIn(pServer, pParams, pParams->pCookie);
        if (status != NV_OK)
            return status;
    }

    pLockInfo->traceOp = RS_LOCK_TRACE_CTRL;
    pLockInfo->traceClassId = pParams->cmd;
    status = serverResLock_Prologue(pServer, *pAccess, pParams->pLockInfo, pReleaseFlags);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

NV_STATUS
serverControl_Epilogue
(
    RsServer *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    LOCK_ACCESS_TYPE access,
    NvU32 *pReleaseFlags,
    NV_STATUS status
)
{
    serverResLock_Epilogue(pServer, access, pParams->pLockInfo, pReleaseFlags);

    if (!pServer->bUnlockedParamCopy)
    {
        status = serverControlApiCopyOut(pServer, pParams, pParams->pCookie, status);
    }

    return status;
}

NvBool
serverSupportsReadOnlyLock
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_API_ENUM api
)
{
    NV_ASSERT(api < RS_API_MAX);
    if (lock == RS_LOCK_TOP)
    {
        return (!!(pServer->roTopLockApiMask & NVBIT(api)));
    }

    return NV_FALSE;
}
