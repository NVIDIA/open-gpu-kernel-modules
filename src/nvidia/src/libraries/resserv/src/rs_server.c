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

#define NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED
#include "nvlog_inc.h"
#include "resserv/resserv.h"
#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "tls/tls.h"
#include "nv_speculation_barrier.h"
#if !RS_STANDALONE
#include "os/os.h"
#endif

// Describes types of clients to find when getting client entries
enum CLIENT_STATE
{
    CLIENT_PARTIALLY_INITIALIZED = NVBIT(0),
    CLIENT_PENDING_FREE = NVBIT(1)
} CLIENT_STATE;

enum CLIENT_LIST_LOCK_STATE
{
    CLIENT_LIST_LOCK_LOCKED,
    CLIENT_LIST_LOCK_UNLOCKED,
};

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
 * @param[in]   clientState Type of clients to look for
 * @param[in]   clientListLockState State of the global client list lock
 * @param[out]  ppClientEntry The client entry associated with the handle
 */
static NvBool _serverFindClientEntryByHandle(RsServer *pServer, NvHandle hClient, enum CLIENT_STATE clientState, enum CLIENT_LIST_LOCK_STATE clientListLockState, CLIENT_ENTRY **ppClientEntry);

/**
 * Get the CLIENT_ENTRY from a client handle, incrementing the ref count if a reference
 * to the CLIENT_ENTRY is held outside of this function (i.e. outside locks).
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[in]   clientState Type of clients to look for
 * @param[in]   clientListLockState State of the global client list lock
 * @param[out]  ppClientEntry The client entry associated with the handle
 */
static NvBool _serverGetClientEntryByHandle(RsServer *pServer, NvHandle hClient, enum CLIENT_STATE clientState, enum CLIENT_LIST_LOCK_STATE clientListLockState, CLIENT_ENTRY **ppClientEntry);

/**
 * Get the CLIENT_ENTRY from a client handle, incrementing the reference count if a
 * reference is held outside this function and also locking the client object if "access"
 * dictates we should.
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[in]   access Lock access type to lock the client object with
 * @param[out]  ppClientEntry The client entry associated with the handle
 */
static NvBool _serverGetAndLockClientEntryByHandle(RsServer *pServer, NvHandle hClient, LOCK_ACCESS_TYPE access, CLIENT_ENTRY **ppClientEntry);

/**
 * Put the CLIENT_ENTRY, decrementing the reference count, and also
 * unlocking the client object as "access" dictates we should.
 * @param[in]   pServer
 * @param[in]   access Lock access type to lock the client object with
 * @param[in]   pClientEntry The client entry to put/unlock
 */
static void _serverPutAndUnlockClientEntry(RsServer *pServer, LOCK_ACCESS_TYPE access, CLIENT_ENTRY *pClientEntry);

/**
 * Insert a CLIENT_ENTRY in the server database at an arbitrary location, must be
 * called with client list lock taken in RW mode.
 * @param[in]   pServer
 * @param[in]   pClientEntry The client entry associated with the handle
 * @param[in]   ppClientNext The client entry to insert the entry before, or NULL if
 *                            we should just insert at the end of the bucket list.
 */
static NV_STATUS _serverInsertClientEntry(RsServer *pServer, CLIENT_ENTRY *ppClientEntry, CLIENT_ENTRY *pClientNext);

/**
 * Mark a CLIENT_ENTRY as about to be freed
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[out]  ppClientEntry The client entry associated with the handle
 */
static NvBool _serverMarkClientEntryPendingFree(RsServer *pServer, NvHandle hClient, CLIENT_ENTRY **ppClientEntry);

/**
 * Find the next available client handle in bucket, must be called with client list lock
 * @param[in]   pServer
 * @param[in]   hClientIn
 * @param[out]  pClientOut
 */
static NV_STATUS _serverFindNextAvailableClientHandleInBucket(RsServer *pServer, NvHandle hClientIn, NvHandle *phClientOut, CLIENT_ENTRY  **ppClientNext);

/**
 * Create a client entry and a client lock for a client that does not exist yet. Used during client
 * construction. No locks will be taken if this call fails.
 * @param[in]   pServer
 * @param[in]   hClient
 */
static NV_STATUS _serverCreateEntryAndLockForNewClient(RsServer *pServer, NvHandle *phClient, NvBool bInternalHandle, CLIENT_ENTRY **ppClientEntry, API_SECURITY_INFO *pSecInfo);

/**
 * Lock the RsClient given a CLIENT_ENTRY
 * @param[in]   access       Read or write lock access
 * @param[in]   pClientEntry The client entry to lock
 */
static void _serverLockClient(LOCK_ACCESS_TYPE access, CLIENT_ENTRY* pClientEntry);

/**
 * Lock and retrieve the RsClient associated with a client handle, and update lock info.
 * @param[in]    pServer
 * @param[in]    access
 * @param[in]    hClient Handle of client to look-up
 * @param[in]    bValidateLocks Whether to validate currently held locks are sufficient
 * @param[inout] pLockInfo Lock state
 * @param[out]   pReleaseFlags Local lock flags to keep track of what locks to release
 * @param[out]   pplientEntry CLIENT_ENTRY associated with the client handle
 */
static NV_STATUS _serverLockClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient, NvBool bValidateLocks, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags, CLIENT_ENTRY **ppClientEntry);

/**
 * Lock and retrieve two RsClient associated with a pair of client handles, and update lock info.
 * @param[in]    pServer
 * @param[in]    access
 * @param[in]    hClient1, hClient2 Handles of clients to look-up and lock
 * @param[in]    bValidateLocks Whether to validate currently held locks are sufficient
 * @param[inout] pLockInfo Lock state
 * @param[out]   pReleaseFlags Local lock flags to keep track of what locks to release
 * @param[out]   ppClientEntry1 CLIENT_ENTRY associated with the first client handle
 * @param[out]   ppClientEntry2 CLIENT_ENTRY associated with the second client handle
 */
static NV_STATUS _serverLockDualClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, NvHandle hClient1, NvHandle hClient2, NvBool bValidateLocks, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags, CLIENT_ENTRY **ppClientEntry1, CLIENT_ENTRY **ppClientEntry2);

/**
 * Lock all clients, and update lock info.
 * @param[in]    pServer
 * @param[inout] pLockInfo Lock state
 * @param[out]   pReleaseFlags Local lock flags to keep track of what locks to release
 */
static NV_STATUS _serverLockAllClientsWithLockInfo(RsServer *pServer, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Unlock the RsClient given a CLIENT_ENTRY
 * @param[in]   access       Read or write lock access
 * @param[in]   pClientEntry The client entry to unlock
 */
static void _serverUnlockClient(LOCK_ACCESS_TYPE access, CLIENT_ENTRY* pClientEntry);

/**
 * Unlock a client and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   pClientEntry Client entry to unlock
 * @param[inout] pLockInfo Lock state
 * @param[inout] pReleaseFlags   Flags indicating the locks that need to be released
 */
static void _serverUnlockClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, CLIENT_ENTRY *pClientEntry, RS_LOCK_INFO* pLockInfo, NvU32 *pReleaseFlags);

/**
 * Unlock two clients and update lock info.
 * @param[in]   pServer
 * @param[in]   access
 * @param[in]   pClientEntry1 First client's entry to unlock
 * @param[in]   pClientEntry2 Second client's entry to unlock
 * @param[inout] pLockInfo Lock state
 * @param[inout] pReleaseFlags   Flags indicating the locks that need to be released
 */
static void _serverUnlockDualClientWithLockInfo(RsServer *pServer, LOCK_ACCESS_TYPE access, CLIENT_ENTRY *pClientEntry1, CLIENT_ENTRY *pClientEntry2, RS_LOCK_INFO* pLockInfo, NvU32 *pReleaseFlags);

/**
 * Unlock all clients, and update lock info.
 * @param[in]    pServer
 * @param[inout] pLockInfo Lock state
 * @param[inout] pReleaseFlags   Flags indicating the locks that need to be released
 */
static NV_STATUS _serverUnlockAllClientsWithLockInfo(RsServer *pServer, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Increment reference count for CLIENT_ENTRY, preventing it from being freed
 * @param[in] pClientEntry The client entry to reference
 */
static void _serverGetClientEntry(CLIENT_ENTRY *pClientEntry);

/**
 * Decrement reference count for CLIENT_ENTRY, freeing it if it reaches 0
 * @param[in] pClientEntry The client entry to dereference
 */
static void _serverPutClientEntry(RsServer *pServer, CLIENT_ENTRY *pClientEntry);

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

    if (pResourceRef->pResource == NULL)
    {
        // 
        // We don't need to acquire the resource lock for a resource
        // that already got freed during resource invalidation.
        // 

        status = clientFreeResource(pResourceRef->pClient, pServer, pFreeParams);
        NV_ASSERT(status == NV_OK);
    }
    else
    {
        pLockInfo->flags |= RS_LOCK_FLAGS_FREE_SESSION_LOCK;
        pLockInfo->traceOp = RS_LOCK_TRACE_FREE;
        pLockInfo->traceClassId = pResourceRef->externalClassId;
        status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
        if (status != NV_OK)
            goto done;

        status = clientFreeResource(pResourceRef->pClient, pServer, pFreeParams);
        NV_ASSERT(status == NV_OK);

        serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    }

done:
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
//  fn  [ C[1..3][0..F]] [  *INDEX*  ]
//  bit 31            20 19          0
//

#define RS_CLIENT_HANDLE_DECODE_MASK (RS_CLIENT_HANDLE_MAX - 1)
#define CLIENT_DECODEHANDLE(handle)                 (handle & RS_CLIENT_HANDLE_DECODE_MASK)

#define CLIENT_ENCODEHANDLE(handleBase, index)        (handleBase | index)

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

    pServer->privilegeLevel        = privilegeLevel;
    pServer->bConstructed          = NV_TRUE;
    pServer->pAllocator            = pAllocator;
    pServer->bDebugFreeList        = NV_FALSE;
    pServer->bRsAccessEnabled      = NV_TRUE;
    pServer->allClientLockOwnerTid = ~0;
    pServer->internalHandleBase = RS_CLIENT_INTERNAL_HANDLE_BASE;
    pServer->clientHandleBase   = RS_CLIENT_HANDLE_BASE;
    pServer->activeClientCount  = 0;
    pServer->activeResourceCount= 0;
    pServer->roTopLockApiMask   = 0;
    /* pServer->bUnlockedParamCopy is set in _rmapiLockAlloc */

    pServer->pClientSortedList = PORT_ALLOC(pAllocator, sizeof(RsClientList)*RS_CLIENT_HANDLE_BUCKET_COUNT);
    if (NULL == pServer->pClientSortedList)
        goto fail;

    for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
    {
        listInitIntrusive(&pServer->pClientSortedList[i]);
    }
    pServer->clientCurrentHandleIndex = 0;

    RS_LOCK_VALIDATOR_INIT(&pServer->clientListLockVal, LOCK_VAL_LOCK_CLASS_CLIENT_LIST, 0xcafe0000);
    pServer->pClientListLock = portSyncSpinlockCreate(pAllocator);
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

    listInitIntrusive(&pServer->disabledClientList);
    pServer->pDisabledClientListLock = portSyncSpinlockCreate(pAllocator);

    return NV_OK;
fail:

#if RS_STANDALONE
    if (pServer->pResLock != NULL)
        portSyncRwLockDestroy(pServer->pResLock);

    if (pServer->pTopLock != NULL)
        portSyncRwLockDestroy(pServer->pTopLock);
#endif

    if (pServer->pClientListLock != NULL)
        portSyncSpinlockDestroy(pServer->pClientListLock);

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
        CLIENT_ENTRY *pClientEntry;
        NvHandle hClient = 0;

        while ((pClientEntry = listHead(&pServer->pClientSortedList[i])) != NULL)
        {
            RS_RES_FREE_PARAMS_INTERNAL freeParams;
            lockInfo.pClient = pClientEntry->pClient;
            hClient = lockInfo.pClient->hClient;
            serverInitFreeParams_Recursive(hClient, hClient, &lockInfo, &freeParams);
            serverFreeResourceTree(pServer, &freeParams);
        }

        listDestroy(&pServer->pClientSortedList[i]);
    }

    listDestroy(&pServer->disabledClientList);
    portSyncSpinlockDestroy(pServer->pDisabledClientListLock);

    PORT_FREE(pServer->pAllocator, pServer->pClientSortedList);
    mapDestroy(&pServer->shareMap);
    listDestroy(&pServer->defaultInheritedSharePolicyList);
    listDestroy(&pServer->globalInternalSharePolicyList);

#if RS_STANDALONE
    portSyncRwLockDestroy(pServer->pResLock);
    portSyncRwLockDestroy(pServer->pTopLock);
#endif

    portSyncSpinlockDestroy(pServer->pShareMapLock);
    portSyncSpinlockDestroy(pServer->pClientListLock);

    portMemAllocatorRelease(pServer->pAllocator);

    pServer->bConstructed = NV_FALSE;

    return NV_OK;
}

NV_STATUS
serverSetClientHandleBase
(
    RsServer *pServer,
    NvU32 clientHandleBase
)
{
    NvU32 releaseFlags = 0;
    RS_LOCK_INFO lockInfo;
    portMemSet(&lockInfo, 0, sizeof(lockInfo));

    // Grab top level lock before updating the internal state
    NV_ASSERT_OK_OR_RETURN(serverTopLock_Prologue(pServer, LOCK_ACCESS_WRITE, &lockInfo, &releaseFlags));

    // Do not allow fixedClientHandle base to be same as internalHandleBase
    if (clientHandleBase != pServer->internalHandleBase)
    {
        pServer->clientHandleBase = clientHandleBase;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting fixed Client handle base\n");
    }

    serverTopLock_Epilogue(pServer, LOCK_ACCESS_WRITE, &lockInfo, &releaseFlags);

    return NV_OK;
}

static
void
_serverFreeClient_underlock
(
    RsServer *pServer,
    CLIENT_ENTRY *pClientEntry
)
{
    RsClient *pClient = pClientEntry->pClient;
    NvHandle hClient = pClient->hClient;

    clientFreeAccessBackRefs(pClient, pServer);

    if (pClient->bDisabled)
    {
        portSyncSpinlockAcquire(pServer->pDisabledClientListLock);
        listRemove(&pServer->disabledClientList, pClient);
        portSyncSpinlockRelease(pServer->pDisabledClientListLock);
    }

    objDelete(pClient);

    // Now remove the client entry and decrease the client count
    serverAcquireClientListLock(pServer);
    listRemove(
        &pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK],
        pClientEntry);
    serverReleaseClientListLock(pServer);

    NV_ASSERT(pClientEntry->refCount == 1);
    _serverPutClientEntry(pServer, pClientEntry);
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
        CLIENT_ENTRY  *pClientEntry = listHead(pClientList);
        while (pClientEntry != NULL)
        {
            RS_CLIENT_FREE_PARAMS params;

            portMemSet(&params, 0, sizeof(params));
            params.hClient = pClientEntry->hClient;

            serverFreeClient(pServer, &params);
            pClientEntry = listHead(pClientList);
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
        return NV_ERR_NOT_READY;

    // RS-TODO Assert that the RW top lock is held

    hClient = pParams->hClient;
#if !(RS_COMPATABILITY_MODE)
    // Fail if the server supplied a client id
    if (hClient != 0)
        return NV_ERR_INVALID_ARGUMENT;
#endif

    status = _serverCreateEntryAndLockForNewClient(pServer, &hClient, !!(pParams->allocState & ALLOC_STATE_INTERNAL_CLIENT_HANDLE), &pClientEntry, pParams->pSecInfo);

    if (status != NV_OK)
        goto done;

    pParams->hClient = hClient;
    pParams->hResource = hClient;
    bLockedClient = NV_TRUE;

    status = resservClientFactory(pServer->pAllocator, pParams, &pClient);
    if (NV_OK != status)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    // Automatically allocate client proxy resource
    status = clientAllocResource(pClient, pServer, pParams);
    if (status != NV_OK)
        goto done;

    //
    // Client list lock is required when the client becomes active in order to avoid
    // race conditions with serverLockAllClients.
    //
    serverAcquireClientListLock(pServer);
    pClientEntry->pClient = pClient;

    // Increase client count
    portAtomicIncrementU32(&pServer->activeClientCount);
    serverReleaseClientListLock(pServer);

done:
    if (bLockedClient)
        _serverUnlockClient(LOCK_ACCESS_WRITE, pClientEntry);

    if ((status != NV_OK) && (pClientEntry != NULL))
    {
        serverAcquireClientListLock(pServer);
        listRemove(
            &pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK],
            pClientEntry);
        serverReleaseClientListLock(pServer);

        //
        // Decrement reference count outside of client list lock, memory free is
        // disallowed in the spinlock's critical section on Windows.
        //
        _serverPutClientEntry(pServer, pClientEntry);

        objDelete(pClient);
    }

    if (pClientEntry != NULL)
        _serverPutClientEntry(pServer, pClientEntry);

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
    NV_STATUS     status;
    CLIENT_ENTRY *pClientEntry;
    NvU32         releaseFlags = 0;

    //
    // Mark the client entry as pending free which will allow us to prevent other threads
    // from using the client while we deallocate resources.
    //
    if (!_serverMarkClientEntryPendingFree(pServer, pParams->hClient, &pClientEntry))
        return NV_ERR_INVALID_OBJECT_HANDLE;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE,
        pParams->pResFreeParams->pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    _serverFreeClient_underlock(pServer, pClientEntry);

done:
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pParams->pResFreeParams->pLockInfo, &releaseFlags);

    // Undo pending free marker
    if (status != NV_OK)
    {
        serverAcquireClientListLock(pServer);
        pClientEntry->bPendingFree = NV_FALSE;
        serverReleaseClientListLock(pServer);
    }

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
    CLIENT_ENTRY       *pClientEntry = NULL;
    CLIENT_ENTRY       *pSecondClientEntry = NULL;
    NvHandle            hSecondClient;
    CALL_CONTEXT        callContext = {0};

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
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            serverDeserializeAllocDown(&callContext, pParams->externalClassId, &pParams->pAllocParams, &pParams->paramsSize, &pParams->allocFlags),
            done);

        if (bClientAlloc)
        {
            status = serverAllocClient(pServer, pParams);
        }
        else
        {
            status = serverAllocLookupSecondClient(pParams->externalClassId, 
                                                   pParams->pAllocParams,
                                                   &hSecondClient);
            if (status != NV_OK)
                goto done;

            if (hSecondClient == 0)
            {
                status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                       pParams->hClient, NV_TRUE,
                                                       pLockInfo, &releaseFlags,
                                                       &pClientEntry);

                if (status != NV_OK)
                    goto done;

                if (!pClientEntry->pClient->bActive)
                {
                    status = NV_ERR_INVALID_STATE;
                    goto done;
                }
            }
            else
            {
                status = _serverLockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                           pParams->hClient, hSecondClient,
                                                           NV_TRUE, pLockInfo,
                                                           &releaseFlags,
                                                           &pClientEntry,
                                                           &pSecondClientEntry);

                if (status != NV_OK)
                    goto done;

                if (!pClientEntry->pClient->bActive ||
                    !pSecondClientEntry->pClient->bActive)
                {
                    status = NV_ERR_INVALID_STATE;
                    goto done;
                }
            }

            pParams->pClient = pClientEntry->pClient;

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
        if (pClientEntry != NULL)
        {
            if (pSecondClientEntry != NULL)
            {
                _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                    pClientEntry, pSecondClientEntry,
                                                    pLockInfo, &releaseFlags);
            }
            else
            {
                _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
                                                pLockInfo, &releaseFlags);
            }
        }
    }

    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
        serverSerializeAllocUp(&callContext, pParams->externalClassId, &pParams->pAllocParams, &pParams->paramsSize, &pParams->allocFlags));
    serverFreeSerializeStructures(&callContext, pParams->pAllocParams);

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

NvU32
serverAllocClientHandleBase
(
    RsServer          *pServer,
    NvBool             bInternalHandle,
    API_SECURITY_INFO *pSecInfo
)
{
    return bInternalHandle ? pServer->internalHandleBase :
                             pServer->clientHandleBase;
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
serverMarkClientListDisabled
(
    RsServer *pServer,
    NvHandle *phClientList,
    NvU32 numClients,
    NvU32 freeState,
    API_SECURITY_INFO *pSecInfo
)
{
    NvU32 i;
    for (i = 0; i < numClients; ++i)
    {
        RS_CLIENT_FREE_PARAMS params;
        portMemSet(&params, 0, sizeof(params));

        if (phClientList[i] == 0)
            continue;

        params.hClient = phClientList[i];
        params.bDisableOnly = NV_TRUE;
        params.state = freeState;
        params.pSecInfo = pSecInfo;

        // If individual calls fail not much to do, just log error and move on
        NV_ASSERT_OK(serverFreeClient(pServer, &params));
    }

    return NV_OK;
}

// Returns pServer->pNextDisabledClient and advances it by one node ahead
static RsClient *
_getNextDisabledClient(RsServer *pServer)
{
    RsClient *pClient;
    portSyncSpinlockAcquire(pServer->pDisabledClientListLock);

    pClient =
        (pServer->pNextDisabledClient != NULL) ?
            pServer->pNextDisabledClient :
            listHead(&pServer->disabledClientList);

    pServer->pNextDisabledClient =
        (pClient != NULL) ?
            listNext(&pServer->disabledClientList, pClient) :
            listHead(&pServer->disabledClientList);

    portSyncSpinlockRelease(pServer->pDisabledClientListLock);
    return pClient;
}

NV_STATUS serverFreeDisabledClients
(
    RsServer *pServer,
    NvU32 freeState,
    NvU32 limit
)
{
    RsClient *pClient;
    RS_RES_FREE_PARAMS params;
    API_SECURITY_INFO secInfo;
    RS_LOCK_INFO lockInfo;
    NV_STATUS status = NV_OK;

    //
    // Only allow one instance of this function at a time.
    // Multiple calls can happen if one thread requested delayed free via worker,
    // while another tries to flush disabled clients immediately.
    // It doesn't matter which one ends up running, they all free everything
    //
    static volatile NvU32 inProgress;
    if (!portAtomicCompareAndSwapU32(&inProgress, 1, 0))
        return NV_ERR_IN_USE;

    portMemSet(&params,   0, sizeof(params));
    portMemSet(&secInfo,  0, sizeof(secInfo));
    portMemSet(&lockInfo, 0, sizeof(lockInfo));

    secInfo.privLevel     = RS_PRIV_LEVEL_KERNEL;
    secInfo.paramLocation = PARAM_LOCATION_KERNEL;
    lockInfo.state        = freeState;
    params.pLockInfo      = &lockInfo;
    params.pSecInfo       = &secInfo;

    while ((pClient = _getNextDisabledClient(pServer)))
    {
        NV_ASSERT(pClient->bDisabled);

        params.hClient   = pClient->hClient;
        params.hResource = pClient->hClient;

        //
        // We call serverFreeClient twice; first for high priority resources
        // then again for remaining resources
        //
        if (!pClient->bHighPriorityFreeDone)
        {
            params.bHiPriOnly = NV_TRUE;
            pClient->bHighPriorityFreeDone = NV_TRUE;
        }
        else
        {
            params.bHiPriOnly = NV_FALSE;
        }

        serverFreeResourceTree(pServer, &params);

        //
        // If limit is 0, it'll wrap-around and count down from 0xFFFFFFFF
        // But RS_CLIENT_HANDLE_MAX is well below that, so it effectively
        // means process all of them
        //
        if (--limit == 0)
        {
            status = NV_WARN_MORE_PROCESSING_REQUIRED;
            break;
        }
    }

    portAtomicSetU32(&inProgress, 0);
    return status;
}


NV_STATUS
serverFreeResourceTree
(
    RsServer *pServer,
    RS_RES_FREE_PARAMS *pParams
)
{
    CLIENT_ENTRY       *pClientEntry = NULL;
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

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient,
        NV_TRUE, pLockInfo, &releaseFlags, &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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
        NV_PRINTF(LEVEL_INFO, "hObject 0x%x not found for client 0x%x\n",
                pParams->hResource,
                pParams->hClient);
#if (RS_COMPATABILITY_MODE)
        status = NV_OK;
#endif
        goto done;
    }
    pParams->pResourceRef = pResourceRef;
    freeStack.pResourceRef = pResourceRef;

    if (pParams->bDisableOnly)
    {
        if (!pClient->bDisabled)
        {
            pClient->bDisabled = NV_TRUE;
            portSyncSpinlockAcquire(pServer->pDisabledClientListLock);
            listAppendExisting(&pServer->disabledClientList, pClient);
            portSyncSpinlockRelease(pServer->pDisabledClientListLock);
        }
        else
        {
            status = NV_ERR_INVALID_STATE;
            goto done;
        }

        pClient->bActive = NV_FALSE;
        status = NV_OK;

        // Unmap all CPU mappings
        {
            CALL_CONTEXT callContext;
            RS_ITERATOR it;
            portMemSet(&callContext, 0, sizeof(callContext));
            callContext.pServer = pServer;
            callContext.pClient = pClient;
            callContext.pLockInfo = pLockInfo;

            it = clientRefIter(pClient, NULL, 0, RS_ITERATE_DESCENDANTS, NV_TRUE);
            while (clientRefIterNext(pClient, &it))
            {
                callContext.pResourceRef = it.pResourceRef;
                clientUnmapResourceRefMappings(pClient, &callContext, pLockInfo);
            }
        }

        goto done;
    }

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
        pClient->pFreeStack = freeStack.pPrev;
        bPopFreeStack = NV_FALSE;
    }

    if (pParams->hClient == pParams->hResource)
    {
        pClient->bActive = NV_FALSE;
    }

    _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry, pLockInfo,
        &releaseFlags);

    pClientEntry = NULL;

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
            NV_CHECK_OK(status, LEVEL_INFO, serverTopLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags));
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

    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
            pClientEntry, pLockInfo, &releaseFlags);
    }
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
    NV_STATUS             status;
    CLIENT_ENTRY         *pClientEntry = NULL;
    CLIENT_ENTRY         *pSecondClientEntry = NULL;
    RsClient             *pClient;
    RsResourceRef        *pResourceRef = NULL;
    RS_LOCK_INFO         *pLockInfo;
    NvU32                 releaseFlags = 0;
    CALL_CONTEXT          callContext;
    CALL_CONTEXT         *pOldContext = NULL;
    LOCK_ACCESS_TYPE      access = LOCK_ACCESS_WRITE;
    enum CLIENT_LOCK_TYPE clientLockType = CLIENT_LOCK_SPECIFIC;
    NvHandle              hSecondClient;

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

    status = serverControlLookupClientLockFlags(pParams->pCookie, &clientLockType);
    if (status != NV_OK)
        goto done;

    if (clientLockType == CLIENT_LOCK_SPECIFIC)
    {
        status = serverControlLookupSecondClient(pParams->cmd, pParams->pParams,
            pParams->pCookie, &hSecondClient);
        if (status != NV_OK)
            goto done;

        if (hSecondClient == 0)
        {
            status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                pParams->hClient,
                (((pParams->flags & NVOS54_FLAGS_IRQL_RAISED) == 0) &&
                 ((pParams->flags & NVOS54_FLAGS_LOCK_BYPASS) == 0)),
                pLockInfo, &releaseFlags, &pClientEntry);
            if (status != NV_OK)
                goto done;

            if (!pClientEntry->pClient->bActive)
            {
                status = NV_ERR_INVALID_STATE;
                goto done;
            }
        }
        else
        {
            status = _serverLockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                pParams->hClient, hSecondClient,
                (((pParams->flags & NVOS54_FLAGS_IRQL_RAISED) == 0) &&
                 ((pParams->flags & NVOS54_FLAGS_LOCK_BYPASS) == 0)),
                pLockInfo, &releaseFlags, &pClientEntry, &pSecondClientEntry);
            if (status != NV_OK)
                goto done;

            if (!pClientEntry->pClient->bActive || !pSecondClientEntry->pClient->bActive)
            {
                status = NV_ERR_INVALID_STATE;
                goto done;
            }
        }

        pClient = pClientEntry->pClient;
    }
    else
    {
        status = _serverLockAllClientsWithLockInfo(pServer, pLockInfo, &releaseFlags);
        if (status != NV_OK)
            goto done;

        status = _serverFindClient(pServer, pParams->hClient, &pClient);
        if (status != NV_OK)
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

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

    status = resControl(pResourceRef->pResource, &callContext, pParams);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

done:

    serverSessionLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    if (clientLockType == CLIENT_LOCK_SPECIFIC)
    {
        if (pClientEntry != NULL)
        {
            if (pSecondClientEntry != NULL)
            {
                _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                                    pClientEntry, pSecondClientEntry,
                                                    pLockInfo, &releaseFlags);
            }
            else
            {
                _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
                                                pLockInfo, &releaseFlags);
            }
        }
    }
    else
    {
        _serverUnlockAllClientsWithLockInfo(pServer, pLockInfo, &releaseFlags);
    }

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
    CLIENT_ENTRY       *pClientEntrySrc = NULL;
    CLIENT_ENTRY       *pClientEntryDst = NULL;
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
                                               hClientSrc, hClientDst, NV_TRUE,
                                               pLockInfo, &releaseFlags,
                                               &pClientEntrySrc, &pClientEntryDst);
    if (status != NV_OK)
        goto done;

    pClientSrc = pClientEntrySrc->pClient;
    pClientDst = pClientEntryDst->pClient;

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

    if (pClientEntrySrc != NULL && pClientEntryDst != NULL)
    {
        _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                            pClientEntrySrc, pClientEntryDst,
                                            pLockInfo, &releaseFlags);
    }
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
    CLIENT_ENTRY       *pClientEntryOwner = NULL;
    CLIENT_ENTRY       *pClientEntryTarget = NULL;
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
                                               hClientOwner, hClientTarget, NV_TRUE,
                                               pLockInfo, &releaseFlags,
                                               &pClientEntryOwner, &pClientEntryTarget);
    if (status != NV_OK)
        goto done;

    pClientOwner = pClientEntryOwner->pClient;
    pClientTarget = pClientEntryTarget->pClient;

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
    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

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
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    // NV_PRINTF(LEVEL_INFO, "hClientOwner %x: Shared hResource: %x with hClientTarget: %x\n",
    //           hClientOwner, pParams->hResource, hClientTarget);

done:
    if (pClientEntryOwner != NULL && pClientEntryTarget != NULL)
    {
        _serverUnlockDualClientWithLockInfo(pServer, LOCK_ACCESS_WRITE,
                                            pClientEntryOwner, pClientEntryTarget,
                                            pLockInfo, &releaseFlags);
    }
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
    CLIENT_ENTRY       *pClientEntry = NULL;
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

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pParams->hClient,
        NV_TRUE, pLockInfo, &releaseFlags, &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

    status = clientShareResource(pClient, pResourceRef, pParams->pSharePolicy, &callContext);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));
    if (status != NV_OK)
        goto done;

    // NV_PRINTF(LEVEL_INFO, "hClient %x: Shared hResource: %x\n", hClient, pParams->hResource);

done:
    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
            pLockInfo, &releaseFlags);
    }

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
    CLIENT_ENTRY       *pClientEntry = NULL;
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

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient,
        NV_TRUE, pLockInfo, &releaseFlags, &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

    status = resMap(pResource, &callContext, pParams, pCpuMapping);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

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
    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
            pLockInfo, &releaseFlags);
    }
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
    CLIENT_ENTRY       *pClientEntry = NULL;
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

    status = _serverLockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, hClient,
        NV_TRUE, pLockInfo, &releaseFlags, &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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
    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
            pLockInfo, &releaseFlags);
    }
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
    CLIENT_ENTRY       *pClientEntry = NULL;
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
                                           NV_TRUE, pLockInfo, &releaseFlags,
                                           &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

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
    pMapping->size = pParams->length;
    pMapping->pMemDesc = pParams->pMemDesc;

done:
    serverInterMap_Epilogue(pServer, pParams, &releaseFlags);

    if (bRestoreCallContext)
        NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    if (status != NV_OK)
    {
        if (pMapping != NULL)
            refRemoveInterMapping(pMapperRef, pMapping);
    }

    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
            pLockInfo, &releaseFlags);
    }
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

static NV_STATUS
serverInterUnmapMapping
(
    RsClient              *pClient,
    RsResourceRef         *pMapperRef,
    RsInterMapping        *pMapping,
    RS_INTER_UNMAP_PARAMS *pParams,
    NvBool                 bPartialUnmap
)
{
    RsInterMapping *pNewMappingLeft  = NULL;
    RsInterMapping *pNewMappingRight = NULL;
    NV_STATUS       status           = NV_OK;

    if (pParams->dmaOffset > pMapping->dmaOffset)
    {
        NV_ASSERT_OK_OR_GOTO(status, refAddInterMapping(pMapperRef, pMapping->pMappableRef, pMapping->pContextRef, &pNewMappingLeft), done);

        pNewMappingLeft->flags = pMapping->flags;
        pNewMappingLeft->dmaOffset = pMapping->dmaOffset;
        pNewMappingLeft->size = pParams->dmaOffset - pMapping->dmaOffset;
    }

    if (pParams->dmaOffset + pParams->size < pMapping->dmaOffset + pMapping->size)
    {
        NV_ASSERT_OK_OR_GOTO(status, refAddInterMapping(pMapperRef, pMapping->pMappableRef, pMapping->pContextRef, &pNewMappingRight), done);

        pNewMappingRight->flags = pMapping->flags;
        pNewMappingRight->dmaOffset = pParams->dmaOffset + pParams->size;
        pNewMappingRight->size = pMapping->dmaOffset + pMapping->size - pNewMappingRight->dmaOffset;
    }

    pParams->hMappable = pMapping->pMappableRef->hResource;
    pParams->pMemDesc = pMapping->pMemDesc;
    status = clientInterUnmap(pClient, pMapperRef, pParams);

done:
    if (bPartialUnmap && status != NV_OK)
    {
        if (pNewMappingLeft != NULL)
            refRemoveInterMapping(pMapperRef, pNewMappingLeft);

        if (pNewMappingRight != NULL)
            refRemoveInterMapping(pMapperRef, pNewMappingRight);
    }
    else
    {
        // Regular unmap should never fail when the range is found
        NV_ASSERT(status == NV_OK);
        refRemoveInterMapping(pMapperRef, pMapping);
    }

    return status;
}

static NV_STATUS
serverInterUnmapInternal
(

    RsClient              *pClient,
    RsResourceRef         *pMapperRef,
    RsResourceRef         *pContextRef,
    RS_INTER_UNMAP_PARAMS *pParams

)
{
    RsInterMapping *pNextMapping   = listHead(&pMapperRef->interMappings);
    NvU64           unmapDmaOffset = pParams->dmaOffset;
    NvU64           unmapSize      = pParams->size;
    NvBool          bPartialUnmap  = (unmapSize != 0);
    NV_STATUS       unmapStatus    = NV_OK;
    NV_STATUS       status         = bPartialUnmap ? NV_OK : NV_ERR_OBJECT_NOT_FOUND;
    NvU64           unmapEnd;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(unmapDmaOffset, unmapSize, &unmapEnd), NV_ERR_INVALID_ARGUMENT);

    while (pNextMapping != NULL)
    {
        RsInterMapping *pMapping = pNextMapping;
        pNextMapping = listNext(&pMapperRef->interMappings, pMapping);

        if (pMapping->pContextRef != pContextRef)
            continue;

        NvU64 mappingEnd;
        NV_ASSERT_OR_RETURN(portSafeAddU64(pMapping->dmaOffset, pMapping->size, &mappingEnd), NV_ERR_INVALID_STATE);

        if (bPartialUnmap &&
            mappingEnd > unmapDmaOffset &&
            pMapping->dmaOffset < unmapEnd)
        {
            if (pMapping->dmaOffset < unmapDmaOffset || mappingEnd > unmapEnd)
            {
                // If the mapping does not lie entirely in the unmapped range, we are in the "true" partial unmap path
                NV_CHECK_TRUE_OR_GOTO(unmapStatus, LEVEL_ERROR, resIsPartialUnmapSupported(pMapperRef->pResource), NV_ERR_INVALID_ARGUMENT, done);
                // It is unclear what to do with pMemDesc when the mapping is split
                NV_ASSERT_TRUE_OR_GOTO(unmapStatus, pMapping->pMemDesc == NULL, NV_ERR_INVALID_STATE, done);
            }

            pParams->dmaOffset = NV_MAX(pMapping->dmaOffset, unmapDmaOffset);
            pParams->size = NV_MIN(unmapEnd, mappingEnd) - pParams->dmaOffset;
        }
        else if (!bPartialUnmap && pMapping->dmaOffset == unmapDmaOffset)
        {
            pParams->dmaOffset = pMapping->dmaOffset;
            pParams->size = pMapping->size;
        }
        else
        {
            continue;
        }

        NV_ASSERT_OK_OR_GOTO(unmapStatus, serverInterUnmapMapping(pClient, pMapperRef, pMapping, pParams, bPartialUnmap), done);

        if (!bPartialUnmap)
        {
            // non-partial unmap always touches a single mapping
            status = NV_OK;
            break;
        }
    }

done:
    if (unmapStatus != NV_OK)
        status = unmapStatus;

    return status;
}

NV_STATUS
serverInterUnmap
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    CLIENT_ENTRY       *pClientEntry = NULL;
    RsClient           *pClient;
    RsResourceRef      *pMapperRef;
    RsResourceRef      *pContextRef;
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
                                           NV_TRUE, pLockInfo, &releaseFlags,
                                           &pClientEntry);
    if (status != NV_OK)
        goto done;

    pClient = pClientEntry->pClient;

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

    status = clientGetResourceRef(pClient, pParams->hDevice, &pContextRef);
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

    NV_ASSERT_OK_OR_GOTO(status,
        resservSwapTlsCallContext(&pOldContext, &callContext), done);

    bRestoreCallContext = NV_TRUE;

    status = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);
    if (status != NV_OK)
        goto done;

    status = serverInterUnmap_Prologue(pServer, pParams);
    if (status != NV_OK)
        goto done;

    status = serverInterUnmapInternal(pClient, pMapperRef, pContextRef, pParams);
done:
    serverInterUnmap_Epilogue(pServer, pParams);

    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pLockInfo, &releaseFlags);

    if (bRestoreCallContext)
        NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));

    if (pClientEntry != NULL)
    {
        _serverUnlockClientWithLockInfo(pServer, LOCK_ACCESS_WRITE, pClientEntry,
            pLockInfo, &releaseFlags);
    }
    serverTopLock_Epilogue(pServer, topLockAccess, pLockInfo, &releaseFlags);

    return status;
}

NV_STATUS
serverAcquireClient
(
    RsServer *pServer,
    NvHandle hClient,
    LOCK_ACCESS_TYPE lockAccess,
    CLIENT_ENTRY **ppClientEntry
)
{
    CLIENT_ENTRY *pClientEntry;

    if (ppClientEntry == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // NV_PRINTF(LEVEL_INFO, "Acquiring hClient %x\n", hClient);
    if (!_serverGetAndLockClientEntryByHandle(pServer, hClient, lockAccess,
            &pClientEntry))
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    *ppClientEntry = pClientEntry;

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

void
serverReleaseClient
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE lockAccess,
    CLIENT_ENTRY *pClientEntry
)
{
    _serverPutAndUnlockClientEntry(pServer, lockAccess, pClientEntry);
}

NvBool
serverIsClientLocked
(
    RsServer *pServer,
    NvHandle hClient
)
{
    CLIENT_ENTRY *pClientEntry;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, _serverFindClientEntryByHandle(pServer, hClient,
            0, CLIENT_LIST_LOCK_UNLOCKED, &pClientEntry), NV_ERR_INVALID_OBJECT_HANDLE);

    return (pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId());
}


NvBool
serverIsClientInternal
(
    RsServer *pServer,
    NvHandle  hClient
)
{
    return ((hClient & pServer->internalHandleBase) == pServer->internalHandleBase);
}

static NV_STATUS _serverBuildAllClientLockList
(
    RsServer *pServer
)
{
    NvU32 i;
    NvU32 activeClientCount;
    NvBool bClientsRemaining;
    NvHandle hClientBucket = RS_CLIENT_HANDLE_BASE;
    CLIENT_ENTRY **ppClientListLocations;
    RsLockedClientListIter lockedClientListIter;

    //
    // Perform memory allocations outside of the client list lock's
    // critical section since it's a spinlock.
    //
    ppClientListLocations = PORT_ALLOC(pServer->pAllocator,
        sizeof(*ppClientListLocations) * RS_CLIENT_HANDLE_BUCKET_COUNT);

    if (ppClientListLocations == NULL)
        return NV_ERR_NO_MEMORY;

    i = 0;
    while (1)
    {
        activeClientCount = pServer->activeClientCount;

        //
        // Allocate memory for locked client list outside of client list lock critical
        // section.
        //
        listInit(&pServer->lockedClientList, pServer->pAllocator);

        for (; i < activeClientCount; i++)
        {
            CLIENT_ENTRY **ppClientEntry = listAppendNew(&pServer->lockedClientList);

            if (ppClientEntry == NULL)
            {
                listDestroy(&pServer->lockedClientList);
                PORT_FREE(pServer->pAllocator, ppClientListLocations);
                return NV_ERR_NO_MEMORY;
            }

            *ppClientEntry = NULL;
        }

        serverAcquireClientListLock(pServer);

        //
        // Ensure that active client count didn't increase while we checked it
        // outside of the client list lock. Increase size of the list if so.
        //
        if (activeClientCount >= pServer->activeClientCount)
            break;

        serverReleaseClientListLock(pServer);
    }

    // Initialize the client location array to start of all used client lists
    for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
    {
        RsClientList *pClientList = &(pServer->pClientSortedList[i]);

        ppClientListLocations[i] = listHead(pClientList);
    }

    bClientsRemaining = (activeClientCount > 0);
    lockedClientListIter = listIterAll(&pServer->lockedClientList);

    //
    // Add client entries to all clients lock list, keeping it sorted,
    // using the client location array.
    //
    while (bClientsRemaining)
    {
        NvHandle hClientNextBucket = ~0;

        bClientsRemaining = NV_FALSE;

        // Iterate over client location array
        for (i = 0; i < RS_CLIENT_HANDLE_BUCKET_COUNT; i++)
        {
            CLIENT_ENTRY *pClientEntry = ppClientListLocations[i];

            if (pClientEntry == NULL)
                continue;

            //
            // Add this client to the all clients lock list if it's in range of
            // the current bucket to ensure sorted order.
            //
            if (pClientEntry->hClient >= hClientBucket &&
                (pClientEntry->hClient < (hClientBucket + RS_CLIENT_HANDLE_BUCKET_COUNT)))
            {
                CLIENT_ENTRY **ppLockedClientEntry;
                RsClientList  *pClientList = &(pServer->pClientSortedList[i]);

                NV_ASSERT(listIterNext(&lockedClientListIter));
                ppLockedClientEntry = lockedClientListIter.pValue;

                //
                // Ignore any partially constructed client.
                // Ignore anything pending free since nothing can use this client
                // object after it's been marked pending free
                //
                if ((pClientEntry->pClient != NULL) && !pClientEntry->bPendingFree)
                {
                    *ppLockedClientEntry = pClientEntry;

                    //
                    // Increase the ref count so client entry doesn't get freed when
                    // we release the client list lock.
                    //
                    _serverGetClientEntry(pClientEntry);
                }

                pClientEntry = listNext(pClientList, pClientEntry);
                ppClientListLocations[i] = pClientEntry;

                // Move to next bucket if at end of the list
                if (pClientEntry == NULL)
                    continue;
            }

            // Any remaining non-NULL client entries must be in a larger bucket
            if (pClientEntry->hClient >=
                (hClientBucket + RS_CLIENT_HANDLE_BUCKET_COUNT))
            {
                // Update next bucket if there are remaining clients to process
                hClientNextBucket = NV_MIN(hClientNextBucket,
                    (pClientEntry->hClient & ~RS_CLIENT_HANDLE_BUCKET_MASK));
                bClientsRemaining = NV_TRUE;
            }
        }

        hClientBucket = hClientNextBucket;
    }

    serverReleaseClientListLock(pServer);

    // Free client list locations array since it's no longer needed
    PORT_FREE(pServer->pAllocator, ppClientListLocations);

    return NV_OK;
}

NV_STATUS
serverLockAllClients
(
    RsServer *pServer
)
{
    RsLockedClientListIter lockedClientListIter;

    NV_ASSERT_OK_OR_RETURN(_serverBuildAllClientLockList(pServer));

    lockedClientListIter = listIterAll(&pServer->lockedClientList);

    // Lock all clients in order
    while (listIterNext(&lockedClientListIter))
    {
        CLIENT_ENTRY *pClientEntry = *lockedClientListIter.pValue;

        if (pClientEntry != NULL)
            _serverLockClient(LOCK_ACCESS_WRITE, pClientEntry);
    }

    // Set all client lock owner TID
    pServer->allClientLockOwnerTid = portThreadGetCurrentThreadId();

    return NV_OK;
}

NV_STATUS
serverUnlockAllClients
(
    RsServer *pServer
)
{
    CLIENT_ENTRY **ppClientEntry = listTail(&pServer->lockedClientList);

    NV_ASSERT_OR_RETURN(serverAllClientsLockIsOwner(pServer), NV_ERR_INVALID_LOCK_STATE);

    // Unlock clients in reverse order
    while (ppClientEntry != NULL)
    {
        CLIENT_ENTRY *pClientEntry = *ppClientEntry;

        if (pClientEntry != NULL)
        {
            // Unlock and restore reference count for clients
            _serverPutAndUnlockClientEntry(pServer, LOCK_ACCESS_WRITE, *ppClientEntry);
        }

        ppClientEntry = listPrev(&pServer->lockedClientList, ppClientEntry);
    }

    // Destroy locked client list
    listDestroy(&pServer->lockedClientList);

    // Unset all client lock owner TID
    pServer->allClientLockOwnerTid = ~0;

    return NV_OK;
}

static
NvBool
__serverFindClientEntryByHandle
(
    RsServer                   *pServer,
    NvHandle                    hClient,
    enum CLIENT_STATE           clientState,
    enum CLIENT_LIST_LOCK_STATE clientListLockState,
    NvBool                      bIncRefCount,
    CLIENT_ENTRY              **ppClientEntry
)
{
    NvBool         bClientFound = NV_FALSE;
    RsClientList  *pClientList;
    CLIENT_ENTRY  *pClientEntryLoop;

    if (clientListLockState == CLIENT_LIST_LOCK_UNLOCKED)
        serverAcquireClientListLock(pServer);

    pClientList = &(pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK]);
    pClientEntryLoop = listHead(pClientList);

    while (pClientEntryLoop != NULL)
    {
        CLIENT_ENTRY *pClientEntry = pClientEntryLoop;
        pClientEntryLoop = listNext(pClientList, pClientEntryLoop);

        if (pClientEntry->hClient == hClient)
        {
            // Client may not have finished constructing yet
            if ((pClientEntry->pClient == NULL) &&
                ((clientState & CLIENT_PARTIALLY_INITIALIZED) == 0))
            {
                goto done;
            }

            // Client may be pending free
            if (pClientEntry->bPendingFree &&
                ((clientState & CLIENT_PENDING_FREE) == 0))
            {
                goto done;
            }

            if (bIncRefCount)
                _serverGetClientEntry(pClientEntry);

            if (ppClientEntry != NULL)
                *ppClientEntry = pClientEntry;

            bClientFound = NV_TRUE;
            goto done;
        }
        else if (pClientEntry->hClient > hClient)
        {
            // Not found in sorted list
            goto done;
        }
    }

done:
    if (clientListLockState == CLIENT_LIST_LOCK_UNLOCKED)
        serverReleaseClientListLock(pServer);

    return bClientFound;
}

static
NvBool
_serverFindClientEntryByHandle
(
    RsServer                   *pServer,
    NvHandle                    hClient,
    enum CLIENT_STATE           clientState,
    enum CLIENT_LIST_LOCK_STATE clientListLockState,
    CLIENT_ENTRY              **ppClientEntry
)
{
    return __serverFindClientEntryByHandle(pServer, hClient, clientState,
        clientListLockState, NV_FALSE, ppClientEntry);
}

static
NvBool
_serverGetClientEntryByHandle
(
    RsServer                   *pServer,
    NvHandle                    hClient,
    enum CLIENT_STATE           clientState,
    enum CLIENT_LIST_LOCK_STATE clientListLockState,
    CLIENT_ENTRY              **ppClientEntry
)
{
    return __serverFindClientEntryByHandle(pServer, hClient, clientState,
        clientListLockState, NV_TRUE, ppClientEntry);
}

static
NvBool
_serverGetAndLockClientEntryByHandle
(
    RsServer        *pServer,
    NvHandle         hClient,
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY   **ppClientEntry
)
{
    CLIENT_ENTRY *pClientEntry;

    if (!_serverGetClientEntryByHandle(pServer, hClient, 0, CLIENT_LIST_LOCK_UNLOCKED,
            &pClientEntry))
    {
        return NV_FALSE;
    }

    _serverLockClient(access, pClientEntry);

    // Handle race condition where client entry was marked pending free
    if (pClientEntry->bPendingFree)
    {
        _serverPutAndUnlockClientEntry(pServer, access, pClientEntry);
        return NV_FALSE;
    }

    *ppClientEntry = pClientEntry;
    return NV_TRUE;
}

static
void
_serverPutAndUnlockClientEntry
(
    RsServer        *pServer,
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY    *pClientEntry
)
{
    _serverUnlockClient(access, pClientEntry);
    _serverPutClientEntry(pServer, pClientEntry);
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

    NV_CHECK_OR_RETURN(LEVEL_SILENT, _serverFindClientEntryByHandle(pServer, hClient,
            CLIENT_PENDING_FREE, CLIENT_LIST_LOCK_UNLOCKED, &pClientEntry),
        NV_ERR_INVALID_OBJECT_HANDLE);

    *ppClient = pClientEntry->pClient;
    return NV_OK;
}

static
NV_STATUS
_serverInsertClientEntry
(
    RsServer      *pServer,
    CLIENT_ENTRY  *pClientEntry,
    CLIENT_ENTRY  *pClientNext
)
{
    RsClientList  *pClientList;
    NvHandle       hClient = pClientEntry->hClient;

    if (hClient == 0)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    pClientList  = &(pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK]);

    if (pClientNext == NULL)
    {
        listAppendExisting(pClientList, pClientEntry);
    }
    else
    {
        listInsertExisting(pClientList, pClientNext, pClientEntry);
    }

    return NV_OK;
}

/**
 * Mark a CLIENT_ENTRY as about to be freed
 * @param[in]   pServer
 * @param[in]   hClient The handle to lookup
 * @param[out]   pClientEntry The client entry associated with the handle
 *
 * @return NV_TRUE if client entry was found and marked pending free
 *         NV_FALSE if client couldn't be found
 */
static NvBool _serverMarkClientEntryPendingFree(RsServer *pServer, NvHandle hClient, CLIENT_ENTRY **ppClientEntry)
{
    RsClientList *pClientList;
    CLIENT_ENTRY *pClientEntry;

    serverAcquireClientListLock(pServer);

    pClientList = &(pServer->pClientSortedList[hClient & RS_CLIENT_HANDLE_BUCKET_MASK]);
    pClientEntry = listHead(pClientList);

    while (pClientEntry != NULL)
    {
        if (pClientEntry->hClient == hClient)
        {
            if (pClientEntry->pClient == NULL)
                goto fail;

            *ppClientEntry = pClientEntry;

            //
            // Mark client entry pending free if it isn't already in the process of
            // being freed
            //
            if (pClientEntry->bPendingFree)
                goto fail;

            pClientEntry->bPendingFree = NV_TRUE;

            //
            // Release client list lock - retaining it while attempting to acquire a
            // client lock could deadlock.
            //
            serverReleaseClientListLock(pServer);

            //
            // If we locked this client as part of locking all client locks, ensure we
            // unlock this client first to avoid self-deadlocking. Also decrement
            // reference count since serverLockAllClients increments it. Remove from
            // locked client list to prevent dangling reference to the CLIENT_ENTRY.
            //
            if (serverAllClientsLockIsOwner(pServer) &&
                (pClientEntry->lockOwnerTid == portThreadGetCurrentThreadId()))
            {
                _serverPutAndUnlockClientEntry(pServer, LOCK_ACCESS_WRITE, pClientEntry);
                listRemoveFirstByValue(&pServer->lockedClientList, &pClientEntry);
            }

            //
            // Wait for all API's using the CLIENT_ENTRY to finish, waiting till the
            // reference count reaches 1 and using the client lock to postpone execution
            // until these API's finish. The caller will take care of the final decrement
            // of the reference count to 0.
            //
            while (pClientEntry->refCount > 1)
            {
                _serverLockClient(LOCK_ACCESS_WRITE, pClientEntry);
                _serverUnlockClient(LOCK_ACCESS_WRITE, pClientEntry);
            }

            // Client is no longer active so decrement the count
            portAtomicDecrementU32(&pServer->activeClientCount);

            return NV_TRUE;
        }
        else if (pClientEntry->hClient > hClient)
        {
            serverReleaseClientListLock(pServer);

            // Not found in sorted list
            return NV_FALSE;
        }

        pClientEntry = listNext(pClientList, pClientEntry);
    }

fail:
    serverReleaseClientListLock(pServer);

    return NV_FALSE;
}

static
void
_serverGetClientEntry(CLIENT_ENTRY *pClientEntry)
{
    NV_ASSERT(!pClientEntry->bPendingFree);
    portAtomicIncrementU32(&pClientEntry->refCount);
}

static
void
_serverPutClientEntry
(
    RsServer *pServer,
    CLIENT_ENTRY *pClientEntry
)
{
    if (portAtomicDecrementU32(&pClientEntry->refCount) == 0)
    {
        pClientEntry->pClient = NULL;
        pClientEntry->hClient = 0;

        portSyncRwLockDestroy(pClientEntry->pLock);
        PORT_FREE(pServer->pAllocator, pClientEntry);
    }
}

static
NV_STATUS
_serverFindNextAvailableClientHandleInBucket
(
    RsServer        *pServer,
    NvHandle         hClientIn,
    NvHandle        *phClientOut,
    CLIENT_ENTRY   **ppClientNext
)
{
    NvHandle        hPrefixIn, hPrefixOut;
    RsClientList   *pClientList  = &(pServer->pClientSortedList[hClientIn & RS_CLIENT_HANDLE_BUCKET_MASK]);
    NvHandle        hClientOut   = hClientIn;
    CLIENT_ENTRY   *pClientEntry = listHead(pClientList);

    *ppClientNext = NULL;
    if (pClientEntry == NULL)
    {
        *phClientOut = hClientOut;
        return NV_OK;
    }

    //
    // The list is ordered by increased client handles
    // We need to find a value to insert or change the handle
    //
    while (pClientEntry != NULL)
    {
        if (pClientEntry->hClient < hClientOut)
        {
            pClientEntry = listNext(pClientList, pClientEntry);
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
        pClientEntry = listNext(pClientList, pClientEntry);
    }

    hPrefixIn = hClientIn & ~RS_CLIENT_HANDLE_DECODE_MASK;
    hPrefixOut = hClientOut & ~RS_CLIENT_HANDLE_DECODE_MASK;
    if (hPrefixIn != hPrefixOut)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    *phClientOut = hClientOut;
    if (pClientEntry != NULL)
    {
        *ppClientNext = pClientEntry;
    }
    return  NV_OK;
}

static
NV_STATUS
_serverCreateEntryAndLockForNewClient
(
    RsServer          *pServer,
    NvHandle          *phClient,
    NvBool             bInternalHandle,
    CLIENT_ENTRY     **ppClientEntry,
    API_SECURITY_INFO *pSecInfo
)
{
    CLIENT_ENTRY  *pClientEntry = NULL;
    NV_STATUS      status = NV_OK;
    NvHandle       hClient = *phClient;
    CLIENT_ENTRY  *pClientNext = NULL;
    PORT_RWLOCK   *pLock = NULL;
    NvU32          handleBase = serverAllocClientHandleBase(pServer, bInternalHandle, pSecInfo);
    NvBool         bLockedClientList = NV_FALSE;

    //
    // Perform memory allocations before taking the client list spinlock, they must
    // be performed outside of the spinlock's critical section.
    //
    pLock = portSyncRwLockCreate(pServer->pAllocator);
    if (pLock == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pClientEntry = (CLIENT_ENTRY *)PORT_ALLOC(pServer->pAllocator, sizeof(CLIENT_ENTRY));
    if (pClientEntry == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto _serverCreateEntryAndLockForNewClient_exit;
    }

    portMemSet(pClientEntry, 0, sizeof(*pClientEntry));

    pClientEntry->pLock = pLock;

    serverAcquireClientListLock(pServer);
    bLockedClientList = NV_TRUE;

    if (hClient == 0)
    {
        NvU32 clientHandleIndex = pServer->clientCurrentHandleIndex;
        NvU16 clientHandleBucketInit = clientHandleIndex & RS_CLIENT_HANDLE_BUCKET_MASK;
        do
        {
            hClient = CLIENT_ENCODEHANDLE(handleBase, clientHandleIndex);
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
        while (_serverFindNextAvailableClientHandleInBucket(pServer, hClient, &hClient, &pClientNext) != NV_OK);

        pServer->clientCurrentHandleIndex = clientHandleIndex;
    }
    else
    {
        NvHandle hClientOut = 0;

#if !(RS_COMPATABILITY_MODE)
        // Re-encode handle so it matches expected format
        NvU32 clientIndex = CLIENT_DECODEHANDLE(hClient);
        hClient = CLIENT_ENCODEHANDLE(handleBase, clientIndex);
#endif

        if (_serverFindClientEntryByHandle(pServer, hClient,
                CLIENT_PARTIALLY_INITIALIZED | CLIENT_PENDING_FREE,
                CLIENT_LIST_LOCK_LOCKED, NULL))
        {
            // The handle already exists
            status = NV_ERR_INSERT_DUPLICATE_NAME;
            goto _serverCreateEntryAndLockForNewClient_exit;
        }

        status = _serverFindNextAvailableClientHandleInBucket(pServer, hClient, &hClientOut, &pClientNext);
        if (status != NV_OK)
             goto _serverCreateEntryAndLockForNewClient_exit;

        if (hClient != hClientOut)
        {
            // This should not happen as we checked for duplicates already
            NV_PRINTF(LEVEL_ERROR, "Client handle mismatch: %x != %x.\n", hClient, hClientOut);
            status = NV_ERR_INVALID_STATE;
            goto _serverCreateEntryAndLockForNewClient_exit;
        }
    }

    // At this point we have a hClient,  we know in which bucket and where in the bucket to insert the entry.
    pClientEntry->hClient = hClient;
    pClientEntry->pLock = pLock;
    pClientEntry->refCount = 1;
    pClientEntry->bPendingFree = NV_FALSE;

    RS_LOCK_VALIDATOR_INIT(&pClientEntry->lockVal,
                           bInternalHandle ? LOCK_VAL_LOCK_CLASS_CLIENT_INTERNAL : LOCK_VAL_LOCK_CLASS_CLIENT,
                           hClient);

    status = _serverInsertClientEntry(pServer, pClientEntry, pClientNext);
    if (status != NV_OK)
        goto _serverCreateEntryAndLockForNewClient_exit;

    //
    // Increase the reference count so this CLIENT_ENTRY can't be freed until we're
    // done using it.
    //
    _serverGetClientEntry(pClientEntry);

    // Release client list lock
    serverReleaseClientListLock(pServer);
    bLockedClientList = NV_FALSE;

    //
    // Acquire the client lock here. Nothing else should have acquired it since
    // pClientEntry->pClient is still NULL.
    //
    RS_RWLOCK_ACQUIRE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
    pClientEntry->lockOwnerTid = portThreadGetCurrentThreadId();

    *phClient = hClient;
    *ppClientEntry = pClientEntry;

_serverCreateEntryAndLockForNewClient_exit:
    if (bLockedClientList)
        serverReleaseClientListLock(pServer);

    if (status != NV_OK)
    {
        if (pClientEntry != NULL)
            PORT_FREE(pServer->pAllocator, pClientEntry);

        if (pLock != NULL)
            portSyncRwLockDestroy(pLock);
    }

    return status;
}

static
void
_serverLockClient
(
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY* pClientEntry
)
{
    if (access == LOCK_ACCESS_READ)
    {
        RS_RWLOCK_ACQUIRE_READ(pClientEntry->pLock, &pClientEntry->lockVal);
    }
    else
    {
        RS_RWLOCK_ACQUIRE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
        pClientEntry->lockOwnerTid = portThreadGetCurrentThreadId();
    }
}

static
NV_STATUS
_serverLockClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient,
    NvBool bValidateLocks,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags,
    CLIENT_ENTRY **ppClientEntry
)
{
    NV_STATUS status = NV_OK;
    if ((pLockInfo->flags & RS_LOCK_FLAGS_NO_CLIENT_LOCK) ||
        serverAllClientsLockIsOwner(pServer))
    {
        if (!_serverGetClientEntryByHandle(pServer, hClient, 0,
                CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry))
        {
            return NV_ERR_INVALID_OBJECT_HANDLE;
        }

        goto check_locks;
    }

    if ((pLockInfo->state & RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED))
    {
        NV_ASSERT_OR_RETURN(_serverGetClientEntryByHandle(pServer, hClient, 0,
                CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry),
            NV_ERR_INVALID_OBJECT_HANDLE);
        NV_ASSERT_OR_ELSE(pLockInfo->pClient != NULL,
            status = NV_ERR_INVALID_STATE; goto done);
        NV_ASSERT_OR_ELSE(pLockInfo->pClient == (*ppClientEntry)->pClient,
            status = NV_ERR_INVALID_STATE; goto done);
        NV_ASSERT_OR_ELSE((*ppClientEntry)->lockOwnerTid ==
            portThreadGetCurrentThreadId(),
            status = NV_ERR_INVALID_STATE; goto done);

        goto check_locks;
    }

    if (!_serverGetAndLockClientEntryByHandle(pServer, hClient, access, ppClientEntry))
        return NV_ERR_INVALID_OBJECT_HANDLE;

    pLockInfo->state |= RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
    pLockInfo->pClient = (*ppClientEntry)->pClient;
    *pReleaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;

check_locks:
    if (bValidateLocks)
        status = clientValidateLocks((*ppClientEntry)->pClient, pServer, *ppClientEntry);

done:
    if (status != NV_OK)
    {
        if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
        {
            _serverUnlockClientWithLockInfo(pServer, access, *ppClientEntry,
                pLockInfo, pReleaseFlags);
        }
        else if (*ppClientEntry != NULL)
        {
            _serverPutClientEntry(pServer, *ppClientEntry);
            *ppClientEntry = NULL;
        }
    }

    return status;
}

static
NV_STATUS
_serverLockDualClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    NvHandle hClient1,
    NvHandle hClient2,
    NvBool bValidateLocks,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags,
    CLIENT_ENTRY **ppClientEntry1,
    CLIENT_ENTRY **ppClientEntry2
)
{
    NV_STATUS status = NV_OK;

    // 1st and 2nd in handle order, as opposed to fixed 1 and 2
    NvHandle       hClient1st;
    NvHandle       hClient2nd;
    CLIENT_ENTRY **ppClientEntry1st;
    CLIENT_ENTRY **ppClientEntry2nd;

    *ppClientEntry1 = NULL;
    *ppClientEntry2 = NULL;

    if ((pLockInfo->flags & RS_LOCK_FLAGS_NO_CLIENT_LOCK) ||
        serverAllClientsLockIsOwner(pServer))
    {
        ppClientEntry1st = ppClientEntry1;
        ppClientEntry2nd = ppClientEntry2;

        if (!_serverGetClientEntryByHandle(pServer, hClient1, 0,
                CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry1st))
        {
            return NV_ERR_INVALID_OBJECT_HANDLE;
        }

        if (hClient1 == hClient2)
        {
            *ppClientEntry2nd = *ppClientEntry1st;
        }
        else
        {
            NV_ASSERT_OR_ELSE(_serverGetClientEntryByHandle(pServer, hClient2, 0,
                    CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry2nd),
                status = NV_ERR_INVALID_OBJECT_HANDLE; goto done);
        }

        goto check_locks;
    }

    if (hClient1 <= hClient2)
    {
        hClient1st = hClient1;
        ppClientEntry1st = ppClientEntry1;

        hClient2nd = hClient2;
        ppClientEntry2nd = ppClientEntry2;
    }
    else
    {
        hClient1st = hClient2;
        ppClientEntry1st = ppClientEntry2;

        hClient2nd = hClient1;
        ppClientEntry2nd = ppClientEntry1;
    }

    if ((pLockInfo->state & RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED))
    {
        NV_ASSERT_OR_RETURN(pLockInfo->pSecondClient != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pLockInfo->pClient->hClient == hClient1st, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pLockInfo->pSecondClient->hClient == hClient2nd, NV_ERR_INVALID_STATE);

        NV_ASSERT_OR_ELSE(_serverGetClientEntryByHandle(pServer, hClient1st, 0,
                CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry1st),
            status = NV_ERR_INVALID_OBJECT_HANDLE; goto done);
        NV_ASSERT_OR_ELSE((*ppClientEntry1st)->pClient == pLockInfo->pClient,
            status =  NV_ERR_INVALID_STATE; goto done);
        NV_ASSERT_OR_ELSE((*ppClientEntry1st)->lockOwnerTid ==
            portThreadGetCurrentThreadId(),
            status = NV_ERR_INVALID_STATE; goto done);

        if (hClient1st == hClient2nd)
        {
            *ppClientEntry2nd = *ppClientEntry1st;
        }
        else
        {
            NV_ASSERT_OR_ELSE(_serverGetClientEntryByHandle(pServer, hClient2nd, 0,
                    CLIENT_LIST_LOCK_UNLOCKED, ppClientEntry2nd),
                status = NV_ERR_INVALID_OBJECT_HANDLE; goto done);
        }

        NV_ASSERT_OR_ELSE((*ppClientEntry2nd)->pClient == pLockInfo->pSecondClient,
            status = NV_ERR_INVALID_STATE; goto done);
        NV_ASSERT_OR_ELSE(
            (*ppClientEntry2nd)->lockOwnerTid == (*ppClientEntry1st)->lockOwnerTid,
            status = NV_ERR_INVALID_STATE; goto done);

        goto check_locks;
    }

    if (!_serverGetAndLockClientEntryByHandle(pServer, hClient1st, access,
            ppClientEntry1st))
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    if (hClient1 == hClient2)
        *ppClientEntry2nd = *ppClientEntry1st;
    else
    {
        if (!_serverGetAndLockClientEntryByHandle(pServer, hClient2nd, access,
                ppClientEntry2nd))
        {
            _serverUnlockClient(access, *ppClientEntry1st);
            status = NV_ERR_INVALID_OBJECT_HANDLE;
            goto done;
        }
    }

    pLockInfo->state |= RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
    pLockInfo->pClient = (*ppClientEntry1st)->pClient;
    pLockInfo->pSecondClient = (*ppClientEntry2nd)->pClient;
    *pReleaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;

check_locks:
    if (bValidateLocks)
    {
        status = clientValidateLocks((*ppClientEntry1st)->pClient, pServer, 
            *ppClientEntry1st);

        if ((status == NV_OK) && (hClient1 != hClient2))
        {
            status = clientValidateLocks((*ppClientEntry2nd)->pClient, pServer, 
                *ppClientEntry2nd);
        }
    }

done:

    if (status != NV_OK)
    {
        if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
        {
            _serverUnlockDualClientWithLockInfo(pServer, access, *ppClientEntry1st,
                *ppClientEntry2nd, pLockInfo, pReleaseFlags);
        }
        else
        {
            if (*ppClientEntry1st != NULL)
            {
                _serverPutClientEntry(pServer, *ppClientEntry1st);
                *ppClientEntry1st = NULL;
            }

            if ((*ppClientEntry2nd != NULL) && (*ppClientEntry2nd != *ppClientEntry1st))
            {
                _serverPutClientEntry(pServer, *ppClientEntry2nd);
                *ppClientEntry2nd = NULL;
            }
        }
    }

    return status;
}

static
NV_STATUS
_serverLockAllClientsWithLockInfo
(
    RsServer *pServer,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if (!serverAllClientsLockIsOwner(pServer))
    {
        NV_STATUS status = serverLockAllClients(pServer);
        if (status != NV_OK)
            return status;

        *pReleaseFlags |= RS_LOCK_RELEASE_CLIENT_LOCK;
    }

    return NV_OK;
}

static
void
_serverUnlockClient
(
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY* pClientEntry
)
{
    if (access == LOCK_ACCESS_READ)
    {
        RS_RWLOCK_RELEASE_READ(pClientEntry->pLock, &pClientEntry->lockVal);
    }
    else
    {
        pClientEntry->lockOwnerTid = ~0;
        RS_RWLOCK_RELEASE_WRITE(pClientEntry->pLock, &pClientEntry->lockVal);
    }
}

static
void
_serverUnlockClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY *pClientEntry,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
    {
        _serverUnlockClient(access, pClientEntry);

        pLockInfo->state &= ~RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
        pLockInfo->pClient = NULL;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_CLIENT_LOCK;
    }

    _serverPutClientEntry(pServer, pClientEntry);
}

static
void
_serverUnlockDualClientWithLockInfo
(
    RsServer *pServer,
    LOCK_ACCESS_TYPE access,
    CLIENT_ENTRY *pClientEntry1,
    CLIENT_ENTRY *pClientEntry2,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
    {
        // 1st and 2nd in handle order, as opposed to fixed 1 and 2
        CLIENT_ENTRY *pClientEntry1st;
        CLIENT_ENTRY *pClientEntry2nd;

        if (pClientEntry1->pClient->hClient <= pClientEntry2->pClient->hClient)
        {
            pClientEntry1st = pClientEntry1;
            pClientEntry2nd = pClientEntry2;
        }
        else
        {
            pClientEntry1st = pClientEntry2;
            pClientEntry2nd = pClientEntry1;
        }

        _serverUnlockClient(access, pClientEntry2nd);
        if (pClientEntry1->pClient->hClient != pClientEntry2->pClient->hClient)
            _serverUnlockClient(access, pClientEntry1st);

        pLockInfo->state &= ~RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED;
        pLockInfo->pClient = NULL;
        pLockInfo->pSecondClient = NULL;
        *pReleaseFlags &= ~RS_LOCK_RELEASE_CLIENT_LOCK;
    }

    _serverPutClientEntry(pServer, pClientEntry1);

    if (pClientEntry1 != pClientEntry2)
        _serverPutClientEntry(pServer, pClientEntry2);
}

NV_STATUS
_serverUnlockAllClientsWithLockInfo
(
    RsServer *pServer,
    RS_LOCK_INFO *pLockInfo,
    NvU32 *pReleaseFlags
)
{
    if (*pReleaseFlags & RS_LOCK_RELEASE_CLIENT_LOCK)
    {
        NV_STATUS status = serverUnlockAllClients(pServer);
        if (status != NV_OK)
            return status;

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

    lockInfo.state      = pParams->state;
    lockInfo.flags      = RS_LOCK_FLAGS_LOW_PRIORITY;
    params.pLockInfo    = &lockInfo;
    params.hClient      = pParams->hClient;
    params.hResource    = pParams->hClient;
    params.bHiPriOnly   = pParams->bHiPriOnly;
    params.bDisableOnly = pParams->bDisableOnly;
    params.pSecInfo     = pParams->pSecInfo;

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
    objDelete(pShare);

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

#if RS_STANDALONE
NV_STATUS
serverSerializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverDeserializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverSerializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverDeserializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverSerializeAllocDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverDeserializeAllocDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverSerializeAllocUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

NV_STATUS
serverDeserializeAllocUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **pParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    return NV_OK;
}

void
serverFreeSerializeStructures
(
    CALL_CONTEXT *pCallContext,
    void *pParams
)
{
}
#endif // RS_STANDALONE

void
serverDisableReserializeControl
(
    CALL_CONTEXT *pCallContext
)
{
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pCallContext != NULL);
    pCallContext->bReserialize = NV_FALSE;
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

void
serverAcquireClientListLock
(
    RsServer *pServer
)
{
    RS_SPINLOCK_ACQUIRE(pServer->pClientListLock, &pServer->clientListLockVal);
}

void
serverReleaseClientListLock
(
    RsServer *pServer
)
{
    RS_SPINLOCK_RELEASE(pServer->pClientListLock, &pServer->clientListLockVal);
}

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
serverAllocLookupSecondClient
(
    NvU32     externalClassId,
    void     *pAllocParams,
    NvHandle *phSecondClient
)
{
    *phSecondClient = 0;
    return NV_OK;
}

NV_STATUS
serverControlLookupSecondClient
(
    NvU32              cmd,
    void              *pControlParams,
    RS_CONTROL_COOKIE *pCookie,
    NvHandle          *phSecondClient
)
{
    *phSecondClient = 0;
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
serverControlLookupClientLockFlags
(
    RS_CONTROL_COOKIE *pCookie,
    enum CLIENT_LOCK_TYPE  *pClientLockType
)
{
    *pClientLockType = CLIENT_LOCK_SPECIFIC;
    return NV_OK;
}

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

    status = serverControlLookupLockFlags(pServer, RS_LOCK_RESOURCE, pParams, pParams->pCookie, pAccess);
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
