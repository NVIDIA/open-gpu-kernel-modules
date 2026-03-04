/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief This file contains the functions managing the NVLink fabric
 */
#define NVOC_FABRIC_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "compute/fabric.h"

static NV_STATUS
_fabricCacheInsert
(
    FabricCache  *pCache,
    NvU64         key1,
    NvU64         key2,
    NvU64         key3,
    void         *pData
)
{
    FabricCacheSubmap *pInsertedSubmap = NULL;
    FabricCacheEntry *pInsertedEntry = NULL;
    FabricCacheEntry *pEntry;
    FabricCacheMapEntry *pMapEntry;

    pEntry = multimapFindItem(pCache, key1, key2);
    if (pEntry != NULL)
        goto insert;

    if (multimapFindSubmap(pCache, key1) == NULL)
    {
        pInsertedSubmap = multimapInsertSubmap(pCache, key1);
        if (pInsertedSubmap == NULL)
            goto fail;
    }

    pInsertedEntry = multimapInsertItemNew(pCache, key1, key2);
    if (pInsertedEntry == NULL)
        goto fail;

    mapInit(&pInsertedEntry->map, portMemAllocatorGetGlobalNonPaged());
    pEntry = pInsertedEntry;

insert:
    pMapEntry = mapInsertNew(&pEntry->map, key3);
    if (pMapEntry == NULL)
        goto fail;

    pMapEntry->pData = pData;

    return NV_OK;

fail:
    if (pInsertedEntry != NULL)
    {
        mapDestroy(&pInsertedEntry->map);
        multimapRemoveItem(pCache, pInsertedEntry);
    }

    if (pInsertedSubmap != NULL)
        multimapRemoveSubmap(pCache, pInsertedSubmap);

    return NV_ERR_INVALID_STATE;
}

static void
_fabricCacheDelete
(
    FabricCache  *pCache,
    NvU64         key1,
    NvU64         key2,
    NvU64         key3
)
{
    FabricCacheSubmap *pSubmap;
    FabricCacheEntry *pEntry;

    pEntry = multimapFindItem(pCache, key1, key2);
    if (pEntry == NULL)
        return;

    mapRemoveByKey(&pEntry->map, key3);
    if (mapCount(&pEntry->map) > 0)
        return;

    mapDestroy(&pEntry->map);
    multimapRemoveItem(pCache, pEntry);

    pSubmap = multimapFindSubmap(pCache, key1);
    NV_ASSERT_OR_RETURN_VOID(pSubmap != NULL);

    if (multimapCountSubmapItems(pCache, pSubmap) > 0)
        return;

    multimapRemoveSubmap(pCache, pSubmap);
}

static FabricCacheMapEntry*
_fabricCacheFind
(
    FabricCache  *pCache,
    NvU64         key1,
    NvU64         key2,
    NvU64         key3
)
{
    FabricCacheEntry *pEntry;
    FabricCacheMapEntry *pMapEntry;

    pEntry = multimapFindItem(pCache, key1, key2);
    if (pEntry == NULL)
        return NULL;

    pMapEntry = mapFind(&pEntry->map, key3);
    if (pMapEntry == NULL)
        return NULL;

    return pMapEntry;
}

void
fabricWakeUpThreadCallback
(
    void *pData
)
{
    osWakeUp((OS_WAIT_QUEUE*)pData);
}

static NvBool
_fabricCacheInvokeCallback
(
    FabricCache *pCache,
    NvU64        key1,
    NvU64        key2,
    NvU64        key3,
    void         (*pCb)(void *pData)
)
{
    FabricCacheMapEntry *pMapEntry;

    pMapEntry = _fabricCacheFind(pCache, key1, key2, key3);
    if (pMapEntry == NULL)
        return NV_FALSE;

    (*pCb)(pMapEntry->pData);

    return NV_TRUE;
}

static void
_fabricWaitOnUnimportCallback
(
    void *pCbData
)
{
    NvU64 unimportEventId = (NvU64)pCbData;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    OS_WAIT_QUEUE *pWq;

    if ((pWq = fabricUnimportCacheGet(pFabric, unimportEventId)) != NULL)
    {
        osWaitInterruptible(pWq);
        fabricUnimportCacheDelete(pFabric, unimportEventId);
        osFreeWaitQueue(pWq);
    }
}

static void
_fabricUnsetUnimportCallback
(
    NV00F1_CTRL_FABRIC_EVENT *pEvent
)
{
    THREAD_STATE_NODE *pThreadNode;
    THREAD_STATE_FREE_CALLBACK callback;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    OS_WAIT_QUEUE *pWq;
    NV_STATUS status;

    // Nothing to do
    if (pEvent->type != NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_UNIMPORT)
        return;

    status = threadStateGetCurrent(&pThreadNode, NULL);
    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    callback.pCb = _fabricWaitOnUnimportCallback;
    callback.pCbData = (void*)pEvent->id;

    threadStateRemoveCallbackOnFree(pThreadNode, &callback);

    pWq = fabricUnimportCacheGet(pFabric, pEvent->id);
    if (pWq != NULL)
    {
        fabricUnimportCacheDelete(pFabric, pEvent->id);
        osFreeWaitQueue(pWq);
    }
}

static NV_STATUS
_fabricSetUnimportCallback
(
    NV00F1_CTRL_FABRIC_EVENT *pEvent
)
{
    THREAD_STATE_NODE *pThreadNode = NULL;
    THREAD_STATE_FREE_CALLBACK callback;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV_STATUS status;
    OS_WAIT_QUEUE *pWq = NULL;

    // Nothing to do
    if (pEvent->type != NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_UNIMPORT)
        return NV_OK;

    status = threadStateGetCurrent(&pThreadNode, NULL);
    if (status != NV_OK)
        return status;

    callback.pCb = _fabricWaitOnUnimportCallback;
    callback.pCbData = (void*)pEvent->id;

    status = osAllocWaitQueue(&pWq);
    if (status != NV_OK)
        return status;

    status = fabricUnimportCacheInsert(pFabric, pEvent->id, pWq);
    if (status != NV_OK)
        goto fail;

    status = threadStateEnqueueCallbackOnFree(pThreadNode, &callback);
    if (status != NV_OK)
        goto fail;

    return NV_OK;

fail:
    fabricUnimportCacheDelete(pFabric, pEvent->id);

    if (pWq != NULL)
        osFreeWaitQueue(pWq);

    return status;
}

static NV_STATUS
_fabricNotifyEvent(Fabric *pFabric)
{
    if (pFabric->pOsImexEvent == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to notify ImexSessionApi\n");
        return NV_ERR_NOT_READY;
    }

    osSetEvent(NULL, pFabric->pOsImexEvent);

    return NV_OK;
}

NV_STATUS
fabricSetImexEvent_IMPL
(
    Fabric *pFabric,
    NvP64   pOsEvent
)
{
    //
    // RS-TODO: Thread-safety of the cached pOsImexEvent is
    // guaranteed by the RMAPI lock. If the semantics of the RMAPI
    // lock change in the future then we would need to revisit this.
    //
    if ((pOsEvent != NULL) && (pFabric->pOsImexEvent != NULL))
        return NV_ERR_IN_USE;

    pFabric->pOsImexEvent = pOsEvent;

    return NV_OK;
}

NvP64
fabricGetImexEvent_IMPL
(
    Fabric *pFabric
)
{
    return pFabric->pOsImexEvent;
}

void
fabricSetNodeId_IMPL
(
    Fabric *pFabric,
    NvU16   nodeId
)
{
    pFabric->nodeId = nodeId;
}

NvU16
fabricGetNodeId_IMPL
(
    Fabric *pFabric
)
{
    return pFabric->nodeId;
}

NvBool
fabricIsMemAllocDisabled_IMPL
(
    Fabric *pFabric
)
{
    return !pFabric->bAllowFabricMemAlloc;
}

void
fabricDisableMemAlloc_IMPL
(
    Fabric *pFabric
)
{
    pFabric->bAllowFabricMemAlloc = NV_FALSE;
}

void
fabricEnableMemAlloc_IMPL
(
    Fabric *pFabric
)
{
    pFabric->bAllowFabricMemAlloc = NV_TRUE;
}

NV_STATUS
fabricPostEventsV2_IMPL
(
    Fabric                      *pFabric,
    NV00F1_CTRL_FABRIC_EVENT    *pEvents,
    NvU32                        numEvents
)
{
    NV_STATUS status = NV_OK;
    NV00F1_CTRL_FABRIC_EVENT *pNode;
    NvU32 i, j;

    if (numEvents == 0)
        return NV_OK;

    portSyncRwLockAcquireWrite(pFabric->pListLock);

    for (i = 0; i < numEvents; i++)
    {
        pNode = listAppendNew(&pFabric->fabricEventListV2);
        if (pNode == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        status = _fabricSetUnimportCallback(&pEvents[i]);
        if (status != NV_OK)
            goto done;

        portMemCopy(pNode, sizeof(*pNode), &pEvents[i], sizeof(pEvents[i]));
    }

    status = _fabricNotifyEvent(pFabric);

done:
    if (status != NV_OK)
    {
        for (j = 0; j < i; j++)
        {
            pNode = listTail(&pFabric->fabricEventListV2);
            _fabricUnsetUnimportCallback(pNode);
            listRemove(&pFabric->fabricEventListV2, pNode);
        }
    }

    portSyncRwLockReleaseWrite(pFabric->pListLock);
    return status;
}

NvBool
fabricExtractEventsV2_IMPL
(
    Fabric                      *pFabric,
    NV00F1_CTRL_FABRIC_EVENT    *pEventArray,
    NvU32                       *pNumEvents
)
{
    NV00F1_CTRL_FABRIC_EVENT *pEvent;
    NvU32 count = 0;
    NvBool bMoreEvents;

    portSyncRwLockAcquireWrite(pFabric->pListLock);

    while ((count < *pNumEvents) &&
           ((pEvent = listHead(&pFabric->fabricEventListV2)) != NULL))
    {
        portMemCopy(&pEventArray[count], sizeof(*pEvent),
                    pEvent, sizeof(*pEvent));

        listRemove(&pFabric->fabricEventListV2, pEvent);

        count++;
    }

    *pNumEvents = count;

    bMoreEvents = listCount(&pFabric->fabricEventListV2) > 0;

    portSyncRwLockReleaseWrite(pFabric->pListLock);

    return bMoreEvents;
}

void
fabricFlushUnhandledEvents_IMPL
(
    Fabric *pFabric
)
{
    void *pEvent;

    portSyncRwLockAcquireWrite(pFabric->pListLock);

    while ((pEvent = listHead(&pFabric->fabricEventListV2)) != NULL)
        listRemove(&pFabric->fabricEventListV2, pEvent);

    portSyncRwLockReleaseWrite(pFabric->pListLock);
}

NvU64
fabricGenerateEventId_IMPL
(
    Fabric *pFabric
)
{
    return portAtomicExIncrementU64(&pFabric->eventId);
}

static void
_fabricCacheIterateAll
(
    FabricCache *pCache,
    void        (*pCb)(void *pData)
)
{
    FabricCacheIter mmIter;
    FabricCacheEntry *pEntry;
    FabricCacheMapIter mIter;
    FabricCacheMapEntry *pMapEntry;

    mmIter = multimapItemIterAll(pCache);

    while (multimapItemIterNext(&mmIter))
    {
        pEntry = mmIter.pValue;
        mIter = mapIterAll(&pEntry->map);

        while (mapIterNext(&mIter))
        {
            pMapEntry = mIter.pValue;
            (*pCb)(pMapEntry->pData);
        }
    }
}

static void
_fabricCacheClearAll
(
    FabricCache *pCache
)
{
    FabricCacheIter mmIter;
    FabricCacheEntry *pEntry;

    mmIter = multimapItemIterAll(pCache);

    while (multimapItemIterNext(&mmIter))
    {
        pEntry = mmIter.pValue;
        mapClear(&pEntry->map);
    }

    multimapClear(pCache);
}

NV_STATUS
fabricImportCacheInsert_IMPL
(
    Fabric         *pFabric,
    const NvUuid   *pExportUuid,
    NvU64           key,
    void           *pData
)
{
    NV_STATUS status;

    portSyncRwLockAcquireWrite(pFabric->pImportCacheLock);

    status = _fabricCacheInsert(&pFabric->importCache,
                                NV_UUID_HI(pExportUuid),
                                NV_UUID_LO(pExportUuid),
                                key, pData);

    portSyncRwLockReleaseWrite(pFabric->pImportCacheLock);

    return status;
}

void
fabricImportCacheDelete_IMPL
(
    Fabric         *pFabric,
    const NvUuid   *pExportUuid,
    NvU64           key
)
{
    portSyncRwLockAcquireWrite(pFabric->pImportCacheLock);

    _fabricCacheDelete(&pFabric->importCache,
                       NV_UUID_HI(pExportUuid),
                       NV_UUID_LO(pExportUuid),
                       key);

    portSyncRwLockReleaseWrite(pFabric->pImportCacheLock);
}

void
fabricImportCacheClear_IMPL
(
    Fabric *pFabric
)
{
    portSyncRwLockAcquireWrite(pFabric->pImportCacheLock);

    _fabricCacheClearAll(&pFabric->importCache);

    portSyncRwLockReleaseWrite(pFabric->pImportCacheLock);
}

void*
fabricImportCacheGet_IMPL
(
    Fabric         *pFabric,
    const NvUuid   *pExportUuid,
    NvU64           key
)
{
    FabricCacheMapEntry *pMapEntry;

    portSyncRwLockAcquireRead(pFabric->pImportCacheLock);

    pMapEntry = _fabricCacheFind(&pFabric->importCache,
                                 NV_UUID_HI(pExportUuid),
                                 NV_UUID_LO(pExportUuid),
                                 key);

    portSyncRwLockReleaseRead(pFabric->pImportCacheLock);

    return (pMapEntry == NULL ? NULL : pMapEntry->pData);
}

NV_STATUS
fabricUnimportCacheInsert_IMPL
(
    Fabric *pFabric,
    NvU64   unimportEventId,
    void   *pData
)
{
    NV_STATUS status;

    portSyncRwLockAcquireWrite(pFabric->pUnimportCacheLock);

    status = _fabricCacheInsert(&pFabric->unimportCache,
                                unimportEventId, 0, 0, pData);

    portSyncRwLockReleaseWrite(pFabric->pUnimportCacheLock);

    return status;
}

void
fabricUnimportCacheDelete_IMPL
(
    Fabric *pFabric,
    NvU64   unimportEventId
)
{
    portSyncRwLockAcquireWrite(pFabric->pUnimportCacheLock);

    _fabricCacheDelete(&pFabric->unimportCache, unimportEventId, 0, 0);

    portSyncRwLockReleaseWrite(pFabric->pUnimportCacheLock);
}

void*
fabricUnimportCacheGet_IMPL
(
    Fabric *pFabric,
    NvU64   unimportEventId
)
{
    FabricCacheMapEntry *pMapEntry;

    portSyncRwLockAcquireRead(pFabric->pUnimportCacheLock);

    pMapEntry = _fabricCacheFind(&pFabric->unimportCache,
                                 unimportEventId, 0, 0);

    portSyncRwLockReleaseRead(pFabric->pUnimportCacheLock);

    return (pMapEntry == NULL ? NULL : pMapEntry->pData);
}

NvBool
fabricUnimportCacheInvokeCallback_IMPL
(
    Fabric *pFabric,
    NvU64   unimportEventId,
    void    (*pCb)(void *pData)
)
{
    NvBool result;

    portSyncRwLockAcquireWrite(pFabric->pUnimportCacheLock);

    result = _fabricCacheInvokeCallback(&pFabric->unimportCache,
                                        unimportEventId, 0, 0, pCb);

    portSyncRwLockReleaseWrite(pFabric->pUnimportCacheLock);

    return result;
}

void
fabricUnimportCacheIterateAll_IMPL
(
    Fabric *pFabric,
    void    (*pCb)(void *pData)
)
{
    portSyncRwLockAcquireWrite(pFabric->pUnimportCacheLock);

    _fabricCacheIterateAll(&pFabric->unimportCache, pCb);

    portSyncRwLockReleaseWrite(pFabric->pUnimportCacheLock);
}

NV_STATUS
fabricMulticastSetupCacheInsert_IMPL
(
    Fabric *pFabric,
    NvU64   requestId,
    void   *pData
)
{
    NV_STATUS status;

    portSyncRwLockAcquireWrite(pFabric->pMulticastFabriCacheLock);

    status = _fabricCacheInsert(&pFabric->fabricMulticastCache,
                                0, requestId, 0, pData);

    portSyncRwLockReleaseWrite(pFabric->pMulticastFabriCacheLock);

    return status;
}

void
fabricMulticastSetupCacheDelete_IMPL
(
    Fabric *pFabric,
    NvU64   requestId
)
{
    portSyncRwLockAcquireWrite(pFabric->pMulticastFabriCacheLock);

    _fabricCacheDelete(&pFabric->fabricMulticastCache,
                       0, requestId, 0);

    portSyncRwLockReleaseWrite(pFabric->pMulticastFabriCacheLock);
}

void*
fabricMulticastSetupCacheGet_IMPL
(
    Fabric *pFabric,
    NvU64   requestId
)
{
    FabricCacheMapEntry *pMapEntry;

    portSyncRwLockAcquireRead(pFabric->pMulticastFabriCacheLock);

    pMapEntry = _fabricCacheFind(&pFabric->fabricMulticastCache,
                                 0, requestId, 0);

    portSyncRwLockReleaseRead(pFabric->pMulticastFabriCacheLock);

    return (pMapEntry == NULL ? NULL : pMapEntry->pData);
}

NV_STATUS
fabricMulticastCleanupCacheInsert_IMPL
(
    Fabric *pFabric,
    NvU64   requestId,
    void   *pData
)
{
    NV_STATUS status;

    portSyncRwLockAcquireWrite(pFabric->pMulticastFabriCacheLock);

    status = _fabricCacheInsert(&pFabric->fabricMulticastCache,
                                1, requestId, 0, pData);

    portSyncRwLockReleaseWrite(pFabric->pMulticastFabriCacheLock);

    return status;
}

void
fabricMulticastCleanupCacheDelete_IMPL
(
    Fabric *pFabric,
    NvU64   requestId
)
{
    portSyncRwLockAcquireWrite(pFabric->pMulticastFabriCacheLock);

    _fabricCacheDelete(&pFabric->fabricMulticastCache,
                       1, requestId, 0);

    portSyncRwLockReleaseWrite(pFabric->pMulticastFabriCacheLock);
}

void*
fabricMulticastCleanupCacheGet_IMPL
(
    Fabric *pFabric,
    NvU64   requestId
)
{
    FabricCacheMapEntry *pMapEntry;

    portSyncRwLockAcquireRead(pFabric->pMulticastFabriCacheLock);

    pMapEntry = _fabricCacheFind(&pFabric->fabricMulticastCache,
                                 1, requestId, 0);

    portSyncRwLockReleaseRead(pFabric->pMulticastFabriCacheLock);

    return (pMapEntry == NULL ? NULL : pMapEntry->pData);
}

void
fabricMulticastCleanupCacheInvokeCallback_IMPL
(
    Fabric *pFabric,
    NvU64   requestId,
    void    (*pCb)(void *pData)
)
{
    portSyncRwLockAcquireWrite(pFabric->pMulticastFabriCacheLock);

    (void)_fabricCacheInvokeCallback(&pFabric->fabricMulticastCache,
                                     1, requestId, 0, pCb);

    portSyncRwLockReleaseWrite(pFabric->pMulticastFabriCacheLock);
}

void
fabricSetFmSessionFlags_IMPL
(
    Fabric  *pFabric,
    NvU32   flags
)
{
    pFabric->flags = flags;
}

NvU32
fabricGetFmSessionFlags_IMPL
(
    Fabric  *pFabric
)
{
    return pFabric->flags;
}

NV_STATUS
fabricConstruct_IMPL
(
    Fabric *pFabric
)
{
    NV_STATUS status = NV_OK;

    listInit(&pFabric->fabricEventListV2, portMemAllocatorGetGlobalNonPaged());
    multimapInit(&pFabric->importCache, portMemAllocatorGetGlobalNonPaged());
    multimapInit(&pFabric->unimportCache, portMemAllocatorGetGlobalNonPaged());

    multimapInit(&pFabric->fabricMulticastCache,
                 portMemAllocatorGetGlobalNonPaged());

    pFabric->pListLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pListLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pFabric->pImportCacheLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pImportCacheLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pFabric->pUnimportCacheLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pUnimportCacheLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pFabric->pFabricImportModuleLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pFabricImportModuleLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pFabric->nodeId = NV_FABRIC_INVALID_NODE_ID;

    pFabric->bAllowFabricMemAlloc = NV_TRUE;

    pFabric->pMulticastFabricModuleLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pMulticastFabricModuleLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pFabric->pMulticastFabriCacheLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());

    if (pFabric->pMulticastFabriCacheLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    return NV_OK;

//TODO: Remove the WAR to suppress unused label warning
goto fail;
fail:
    fabricDestruct_IMPL(pFabric);
    return status;
}

void
fabricDestruct_IMPL
(
    Fabric *pFabric
)
{
    NV_ASSERT(multimapCountItems(&pFabric->fabricMulticastCache) == 0);

    multimapDestroy(&pFabric->fabricMulticastCache);

    if (pFabric->pMulticastFabricModuleLock != NULL)
        portSyncRwLockDestroy(pFabric->pMulticastFabricModuleLock);

    if (pFabric->pMulticastFabriCacheLock != NULL)
        portSyncRwLockDestroy(pFabric->pMulticastFabriCacheLock);

    NV_ASSERT(listCount(&pFabric->fabricEventListV2) == 0);

    NV_ASSERT(multimapCountItems(&pFabric->unimportCache) == 0);
    NV_ASSERT(multimapCountItems(&pFabric->importCache) == 0);

    NV_ASSERT(pFabric->nodeId == NV_FABRIC_INVALID_NODE_ID);

    if (pFabric->pFabricImportModuleLock != NULL)
        portSyncRwLockDestroy(pFabric->pFabricImportModuleLock);

    if (pFabric->pUnimportCacheLock != NULL)
        portSyncRwLockDestroy(pFabric->pUnimportCacheLock);

    if (pFabric->pImportCacheLock != NULL)
        portSyncRwLockDestroy(pFabric->pImportCacheLock);

    if (pFabric->pListLock != NULL)
        portSyncRwLockDestroy(pFabric->pListLock);

    multimapDestroy(&pFabric->unimportCache);
    multimapDestroy(&pFabric->importCache);

    listDestroy(&pFabric->fabricEventListV2);
}

void
fabricMulticastWaitOnTeamCleanupCallback
(
    void *pCbData
)
{
    NvU64 inbandReqId = (NvU64)pCbData;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    OS_WAIT_QUEUE *pWq;

    pWq = (OS_WAIT_QUEUE *)fabricMulticastCleanupCacheGet(pFabric, inbandReqId);
    if (pWq == NULL)
        return;

    osWaitInterruptible(pWq);
    fabricMulticastCleanupCacheDelete(pFabric, inbandReqId);
    osFreeWaitQueue(pWq);
}
