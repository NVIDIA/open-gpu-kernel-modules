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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions managing the memory fabric import
 *
 *****************************************************************************/

#define NVOC_MEM_FABRIC_IMPORT_V2_H_PRIVATE_ACCESS_ALLOWED

/*
 * Lock ordering
 *
 * RMAPI Lock
 * |_Client Lock
 *   |_GPU(s) Lock (Should not be needed for this device-less class)
 *     |_Fabric Import Module Lock
 *       |_Fabric Import Descriptor Lock
 */

#include "core/core.h"
#include "core/system.h"
#include "compute/fabric.h"
#include "rmapi/resource.h"
#include "rmapi/client.h"
#include "mem_mgr/mem_fabric_import_v2.h"
#include "mem_mgr/mem_export.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/gpu_fabric_probe.h"

#include "class/cl00f9.h"

typedef struct mem_fabric_importer_info
{
    void   *pOsEvent;
    Memory *pMemory;
} MEM_FABRIC_IMPORTER_INFO;

MAKE_LIST(MemFabricImporterInfoList, MEM_FABRIC_IMPORTER_INFO);

typedef struct mem_fabric_import_descriptor
{
    // Ref-count to keep this descriptor alive
    NvU64 refCount;

    // The list of importers waiting on this object to get ready.
    MemFabricImporterInfoList waitingImportersList;

    //
    // Incremented by importDescriptorGetUnused() and decremented by
    // importDescriptorPutNonBlocking().
    //
    NvU64 extUsageCount;

    // Unique import event ID
    NvU64 importEventId;

    // Export object information associated with this import descriptor.
    NvU16   exportNodeId;
    NvU16   index;
    NvU64   id;
    NvU64   key;

    // Same as packet.uuid, but uses NvUuid type.
    NvUuid           uuid;

    // Set when pMemDesc is installed.
    NvBool bMemdescInstalled;

    // A memdesc that is ready to use, where NULL is a valid value.
    MEMORY_DESCRIPTOR *pMemDesc;

    // Set when unimported.
    NvBool bUnimported;

    NvS32 imexChannel;

    //
    // The lock protects MEM_FABRIC_IMPORT_DESCRIPTOR.
    //
    // The lock should be taken only if the descriptor is safe to access i.e.,
    // holding the module lock or the accessing thread has the descriptor
    // refcounted.
    //
    PORT_RWLOCK *pLock;
} MEM_FABRIC_IMPORT_DESCRIPTOR;

//
// Must be called with the pFabricImportModuleLock RW taken.
// The call returns with pFabricImportDesc->pLock RW acquired if found.
//
static MEM_FABRIC_IMPORT_DESCRIPTOR*
_importDescriptorGetAndLockAcquireWrite
(
    const NvUuid *pExportUuid,
    NvU16         index,
    NvU64         id,
    NvU64        *pKey
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc = NULL;
    NvU64 key = (id << NV00F9_IMPORT_ID_SHIFT) | index;

    if (pKey != NULL)
        *pKey = key;

    pFabricImportDesc = fabricImportCacheGet(pFabric, pExportUuid, key);
    if (pFabricImportDesc == NULL)
        return NULL;

    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    pFabricImportDesc->refCount++;

    return pFabricImportDesc;
}

static void
_initImportFabricEvent
(
    NvU16                        exportNodeId,
    NvUuid                      *pExportUuid,
    NvU16                        index,
    NvU64                        id,
    NvS32                        imexChannel,
    NV00F1_CTRL_FABRIC_EVENT    *pEvent
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    pEvent->imexChannel = imexChannel;
    pEvent->type = NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_IMPORT;
    pEvent->id = fabricGenerateEventId_IMPL(pFabric);
    pEvent->data.import.exportNodeId = exportNodeId;
    pEvent->data.import.index        = index;
    pEvent->data.import.id           = id;
    portMemCopy(pEvent->data.import.exportUuid, NV_MEM_EXPORT_UUID_LEN,
                pExportUuid->uuid,              NV_MEM_EXPORT_UUID_LEN);
}

static void
_initUnimportFabricEvent
(
    NvU64                        importEventId,
    NvU16                        exportNodeId,
    NvS32                        imexChannel,
    NV00F1_CTRL_FABRIC_EVENT    *pEvent
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    pEvent->imexChannel = imexChannel;
    pEvent->type = NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_UNIMPORT;
    pEvent->id = fabricGenerateEventId_IMPL(pFabric);
    pEvent->data.unimport.exportNodeId  = exportNodeId;
    pEvent->data.unimport.importEventId = importEventId;
}

static void
_importDescriptorCleanup
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc
)
{
    if (pFabricImportDesc == NULL)
        return;

    if (pFabricImportDesc->pLock != NULL)
        portSyncRwLockDestroy(pFabricImportDesc->pLock);

    listDestroy(&pFabricImportDesc->waitingImportersList);

    memdescDestroy(pFabricImportDesc->pMemDesc);
    pFabricImportDesc->pMemDesc = NULL;

    portMemFree(pFabricImportDesc);
}

//
// Must be called with the pFabricImportModuleLock RO or RW taken.
//
// pFabricImportModuleLock is needed to synchronize with
// _importDescriptorGetAndLockAcquireWrite(). We don't want to delete
// pFabricImportDesc under while it is being refcounted for the first
// time.
//
// The call returns with pFabricImportDesc->pLock released if found.
//
static void
_importDescriptorPutAndLockReleaseWrite
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc,
    NvBool                        bUnimportEvent
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV00F1_CTRL_FABRIC_EVENT unimportEvent;

    if (pFabricImportDesc == NULL)
        return;

    pFabricImportDesc->refCount--;

    // Cleanup when all the non-FM references are gone (unimport).
    if (!pFabricImportDesc->bUnimported &&
        (pFabricImportDesc->refCount == pFabricImportDesc->extUsageCount))
    {
        //
        // Generate unimport event in the following cases:
        //
        // 1. Import sequence was successful.
        // 2. Import sequence is in progress. This is because, FM must have
        //    ref-counted the fabric memory object already. After import
        //    sequence is successful, an unimport event is needed for FM to
        //    drop ref-count on the fabric memory object. However, there is a
        //    possibility that import sequence could fail in between, and FM
        //    drops ref-count on the fabric memory object already. In such a
        //    case, FM should still complete unimport sequence to unblock the
        //    waiting process.
        //
        // Do not generate unimport event in the following cases:
        //
        // 1. Import was unsuccessful and FM has notified RM about it. This is
        //    because FM must have dropped ref-count on the fabric memory object
        //    already when import failed.
        //
        if (bUnimportEvent &&
            !(pFabricImportDesc->bMemdescInstalled &&
              pFabricImportDesc->pMemDesc == NULL))
        {
            _initUnimportFabricEvent(pFabricImportDesc->importEventId,
                                     pFabricImportDesc->exportNodeId,
                                     pFabricImportDesc->imexChannel,
                                     &unimportEvent);
            NV_CHECK(LEVEL_WARNING,
                     fabricPostEventsV2(pFabric, &unimportEvent, 1) == NV_OK);
        }

        fabricImportCacheDelete(pFabric, &pFabricImportDesc->uuid,
                                pFabricImportDesc->key);

        pFabricImportDesc->bUnimported = NV_TRUE;
    }

    if (pFabricImportDesc->refCount != 0)
    {
        portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);
        return;
    }

    // At this point no one is using pFabricImportDesc, drop the locks...
    portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);

    NV_ASSERT(listCount(&pFabricImportDesc->waitingImportersList) == 0);
    NV_ASSERT(pFabricImportDesc->extUsageCount == 0);
    _importDescriptorCleanup(pFabricImportDesc);
}

MEM_FABRIC_IMPORT_DESCRIPTOR*
importDescriptorGetUnused
(
    const NvUuid *pExportUuid,
    NvU16         index,
    NvU64         id
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc = NULL;

    portSyncRwLockAcquireWrite(pFabric->pFabricImportModuleLock);

    pFabricImportDesc = _importDescriptorGetAndLockAcquireWrite(pExportUuid,
                                                            index, id, NULL);
    if (pFabricImportDesc == NULL)
        goto done;

    //
    // bMemdescInstalled is also checked because the installed pMemDesc
    // could be in use by RM clients.
    //
    if (pFabricImportDesc->bMemdescInstalled ||
        (pFabricImportDesc->extUsageCount > 0))
    {
        _importDescriptorPutAndLockReleaseWrite(pFabricImportDesc, NV_FALSE);
        pFabricImportDesc = NULL;
        goto done;
    }

    pFabricImportDesc->extUsageCount++;
    NV_ASSERT(pFabricImportDesc->extUsageCount == 1);

    portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);

done:
    portSyncRwLockReleaseWrite(pFabric->pFabricImportModuleLock);

    return pFabricImportDesc;
}

void
importDescriptorPutNonBlocking
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    portSyncRwLockAcquireRead(pFabric->pFabricImportModuleLock);

    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    NV_ASSERT(pFabricImportDesc->extUsageCount > 0);

    pFabricImportDesc->extUsageCount--;

    _importDescriptorPutAndLockReleaseWrite(pFabricImportDesc, NV_FALSE);

    portSyncRwLockReleaseRead(pFabric->pFabricImportModuleLock);
}

static void
_importDescriptorFlushImporters
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc
)
{
    MEM_FABRIC_IMPORTER_INFO *pNode;

    if (!pFabricImportDesc->bMemdescInstalled)
        return;

    while ((pNode = listHead(&pFabricImportDesc->waitingImportersList)) != NULL)
    {
        if (pFabricImportDesc->pMemDesc != NULL)
        {
            //
            // This function only initializes pNode->pMemory so it
            // should never fail.
            //
            NV_ASSERT_OK(memConstructCommon(pNode->pMemory,
                         NV_MEMORY_FABRIC_IMPORT_V2,
                         0, pFabricImportDesc->pMemDesc,
                         0, NULL, 0, 0, 0, 0,
                         NVOS32_MEM_TAG_NONE, NULL));
        }

        if (pNode->pOsEvent != NULL)
        {
            osSetEvent(NULL, pNode->pOsEvent);
            NV_ASSERT_OK(osDereferenceObjectCount(pNode->pOsEvent));
        }

        listRemove(&pFabricImportDesc->waitingImportersList, pNode);
    }

    return;
}

NV_STATUS
importDescriptorInstallMemDesc
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc,
    MEMORY_DESCRIPTOR            *pMemDesc
)
{
    NV_STATUS status;

    if (pFabricImportDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    if (pFabricImportDesc->bMemdescInstalled)
    {
        status = NV_ERR_IN_USE;
        goto done;
    }

    pFabricImportDesc->pMemDesc = pMemDesc;
    pFabricImportDesc->bMemdescInstalled = NV_TRUE;

    _importDescriptorFlushImporters(pFabricImportDesc);

    status = NV_OK;

done:
    portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);

    return status;
}

static void
_importDescriptorDequeueWait
(
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc,
    Memory                       *pMemory
)
{
    MEM_FABRIC_IMPORTER_INFO *pNode;

    for (pNode = listHead(&pFabricImportDesc->waitingImportersList);
         pNode != NULL;
         pNode = listNext(&pFabricImportDesc->waitingImportersList, pNode))
    {
        if (pNode->pMemory == pMemory)
        {
            if (pNode->pOsEvent != NULL)
                NV_ASSERT_OK(osDereferenceObjectCount(pNode->pOsEvent));

            listRemove(&pFabricImportDesc->waitingImportersList, pNode);

            break;
        }
    }
}

static NV_STATUS
_importDescriptorEnqueueWait
(
    NvHandle                      hClient,
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc,
    NvP64                         pOsEvent,
    Memory                       *pMemory
)
{
    MEM_FABRIC_IMPORTER_INFO *pNode;
    NV_STATUS status;
    NvP64 pValidatedOsEvent = NULL;

    if (pOsEvent != NULL)
    {
        status = osUserHandleToKernelPtr(hClient, pOsEvent, &pValidatedOsEvent);
        if (status != NV_OK)
        {
            goto fail;
        }
    }

    pNode = listAppendNew(&pFabricImportDesc->waitingImportersList);
    if (pNode == NULL)
    {
        osDereferenceObjectCount(pValidatedOsEvent);
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pNode->pOsEvent = pValidatedOsEvent;
    pNode->pMemory = pMemory;

    // In case the memdesc is ready, get unblocked
    _importDescriptorFlushImporters(pFabricImportDesc);

    return NV_OK;

fail:
    _importDescriptorDequeueWait(pFabricImportDesc, pMemory);

    return status;
}

//
// Returns with pFabricImportModuleLock and pFabricImportDesc->pLock taken in
// RW mode. The caller must drop the locks.
//
static MEM_FABRIC_IMPORT_DESCRIPTOR*
_importDescriptorAlloc
(
    NvU8   *pUuid,
    NvU16   exportNodeId,
    NvU16   index,
    NvU64   id,
    NvS32   imexChannel
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV_STATUS status;
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc;
    NvUuid uuid;
    NvU64 key;

    ct_assert(NV_MEM_EXPORT_UUID_LEN == NV_UUID_LEN);
    portMemCopy(uuid.uuid, NV_UUID_LEN,pUuid, NV_MEM_EXPORT_UUID_LEN);

    //
    // Take pFabricImportModuleLock to synchronize multiple constructors
    // to share the cached pFabricImportDesc. Without this lock, two or
    // more constructors may see the import cache empty at the same time.
    //
    // We want the following sequence happen atomically:
    //
    //    pFabricImportDesc = _importDescriptorGetAndLockAcquireWrite(..);
    //    if (pFabricImportDesc == NULL)
    //    {
    //        pFabricImportDesc = alloc();
    //        fabricImportCacheInsert(pFabricImportDesc);
    //        ...
    //    }
    //
    // Also, it is important to synchronize the constructor with the destructor.
    // _importDescriptorGetAndLockAcquireWrite() looks up and refcounts
    // pFabricImportDesc non-atomically. Thus, pFabricImportDesc may be
    // destroyed before it could be refcounted.
    //
    portSyncRwLockAcquireWrite(pFabric->pFabricImportModuleLock);

    // Nothing to do...
    pFabricImportDesc = _importDescriptorGetAndLockAcquireWrite(&uuid, index,
                                                                id, &key);
    if (pFabricImportDesc != NULL)
        return pFabricImportDesc;

    pFabricImportDesc =
        portMemAllocNonPaged(sizeof(MEM_FABRIC_IMPORT_DESCRIPTOR));
    if (pFabricImportDesc == NULL)
        goto fail;

    portMemSet(pFabricImportDesc, 0, sizeof(MEM_FABRIC_IMPORT_DESCRIPTOR));

    pFabricImportDesc->refCount = 1;

    pFabricImportDesc->index = index;
    pFabricImportDesc->exportNodeId = exportNodeId;
    pFabricImportDesc->uuid = uuid;
    pFabricImportDesc->id = id;
    pFabricImportDesc->key = key;
    pFabricImportDesc->imexChannel = imexChannel;

    listInit(&pFabricImportDesc->waitingImportersList,
             portMemAllocatorGetGlobalNonPaged());

    pFabricImportDesc->pLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pFabricImportDesc->pLock == NULL)
        goto fail;

    status = fabricImportCacheInsert(pFabric, &pFabricImportDesc->uuid, key,
                                     pFabricImportDesc);
    if (status != NV_OK)
        goto fail;

    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    return pFabricImportDesc;

fail:
    _importDescriptorCleanup(pFabricImportDesc);

    portSyncRwLockReleaseWrite(pFabric->pFabricImportModuleLock);

    return NULL;
}

static NV_STATUS
_memoryfabricimportv2Construct
(
    MemoryFabricImportV2         *pMemoryFabricImportV2,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV00F9_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;
    Memory *pMemory = staticCast(pMemoryFabricImportV2, Memory);
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc;
    NV_EXPORT_MEM_PACKET *pExportPacket = &pAllocParams->expPacket;
    NV_STATUS status;
    NvU16 exportNodeId = memoryExportGetNodeId(pExportPacket);
    NvU8 *pUuid = pExportPacket->uuid;
    NV00F1_CTRL_FABRIC_EVENT importEvent;
    RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pMemoryFabricImportV2), RmClient);

    if (pRmClient->imexChannel == -1)
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    // If this a single-node UUID?
    if (exportNodeId == NV_FABRIC_INVALID_NODE_ID)
        return NV_ERR_NOT_SUPPORTED;

    //
    // NV00F9_IMPORT_ID_MAX is reserved for MCFLA case. See
    // mem_multicast_fabric.c
    //
    if (pAllocParams->id >= NV00F9_IMPORT_ID_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    pFabricImportDesc = _importDescriptorAlloc(pUuid,
                                               exportNodeId,
                                               pAllocParams->index,
                                               pAllocParams->id,
                                               pRmClient->imexChannel);
    if (pFabricImportDesc == NULL)
        return NV_ERR_NO_MEMORY;

    //
    // At this point the pFabricImportDesc is refcounted, locked and is
    // safe to access.
    //

    pMemoryFabricImportV2->pFabricImportDesc = pFabricImportDesc;
    pMemoryFabricImportV2->expNodeId = exportNodeId;

    status = _importDescriptorEnqueueWait(pParams->hClient,
                                          pFabricImportDesc,
                                          pAllocParams->pOsEvent,
                                          pMemory);
    if (status != NV_OK)
        goto fail;

    //
    // Validate that the client is allowed to use pre-existing (cached)
    // imported memory.
    //
    if (pFabricImportDesc->imexChannel != pRmClient->imexChannel)
    {
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        goto fail;
    }

    if (pFabricImportDesc->refCount > 1)
        goto unlock;

    _initImportFabricEvent(exportNodeId, &pFabricImportDesc->uuid,
                           pAllocParams->index, pAllocParams->id,
                           pRmClient->imexChannel, &importEvent);

    pFabricImportDesc->importEventId = importEvent.id;

    status = fabricPostEventsV2(pFabric, &importEvent, 1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to notify IMEX daemon of import event\n");
        goto fail;
    }

    //
    // DO NOT FAIL after the call to fabricPostEventsV2.
    // This function is used to signal that the
    // MemoryFabricImportV2 has been successfully constructed.
    // It is important that we do not fail the constructor
    // after the event has been posted.
    //

unlock:
    portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);
    portSyncRwLockReleaseWrite(pFabric->pFabricImportModuleLock);

    return NV_OK;

fail:
    _importDescriptorDequeueWait(pFabricImportDesc, pMemory);

    _importDescriptorPutAndLockReleaseWrite(pFabricImportDesc, NV_FALSE);

    portSyncRwLockReleaseWrite(pFabric->pFabricImportModuleLock);

    pMemoryFabricImportV2->pFabricImportDesc = NULL;

    return status;
}

NV_STATUS
memoryfabricimportv2Construct_IMPL
(
    MemoryFabricImportV2         *pMemoryFabricImportV2,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    if (RS_IS_COPY_CTOR(pParams))
    {
        return memoryfabricimportv2CopyConstruct_IMPL(pMemoryFabricImportV2,
                                                      pCallContext,
                                                      pParams);
    }

    return _memoryfabricimportv2Construct(pMemoryFabricImportV2,
                                          pCallContext, pParams);

    //
    // DO NOT ADD ANY CODE HERE.
    // See _memoryfabricimportv2Construct for details.
    //
}

void
memoryfabricimportv2Destruct_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    Memory *pMemory = staticCast(pMemoryFabricImportV2, Memory);
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc =
                                    pMemoryFabricImportV2->pFabricImportDesc;

    portSyncRwLockAcquireRead(pFabric->pFabricImportModuleLock);

    //
    // Destruct pMemory here, under pFabricImportDesc->pLock. We want to skip
    // the default memory destructor as it calls memdescDestroy(), which
    // we don't want.
    //
    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    memDestructCommon(pMemory);

    _importDescriptorDequeueWait(pFabricImportDesc, pMemory);

    _importDescriptorPutAndLockReleaseWrite(pFabricImportDesc, NV_TRUE);

    portSyncRwLockReleaseRead(pFabric->pFabricImportModuleLock);
}

NvBool
memoryfabricimportv2CanCopy_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2
)
{
    return NV_TRUE;
}

NV_STATUS
memoryfabricimportv2IsReady_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2,
    NvBool                bCopyConstructorContext
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc =
                                    pMemoryFabricImportV2->pFabricImportDesc;
    NV_STATUS status;

    // If the IMEX daemon is down, bail out.
    if (fabricGetNodeId(pFabric) == NV_FABRIC_INVALID_NODE_ID)
        return NV_ERR_INVALID_STATE;

    portSyncRwLockAcquireRead(pFabricImportDesc->pLock);

    if (!pFabricImportDesc->bMemdescInstalled)
        status = NV_ERR_NOT_READY;
    else if (pFabricImportDesc->pMemDesc == NULL)
        status = NV_ERR_INVALID_OBJECT;
    else
        status = NV_OK;

    portSyncRwLockReleaseRead(pFabricImportDesc->pLock);

    return status;
}

NV_STATUS
memoryfabricimportv2CopyConstruct_IMPL
(
    MemoryFabricImportV2         *pMemoryFabricImportV2,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc;
    Memory *pMemory = staticCast(pMemoryFabricImportV2, Memory);
    NV_STATUS status;

    MemoryFabricImportV2 *pSourceMemoryFabricImportV2 =
        dynamicCast(pParams->pSrcRef->pResource, MemoryFabricImportV2);

    pFabricImportDesc = pSourceMemoryFabricImportV2->pFabricImportDesc;

    portSyncRwLockAcquireWrite(pFabricImportDesc->pLock);

    status = _importDescriptorEnqueueWait(pParams->hClient,
                                          pFabricImportDesc,
                                          NULL,
                                          pMemory);
    if (status == NV_OK)
    {
        pFabricImportDesc->refCount++;
        pMemoryFabricImportV2->pFabricImportDesc = pFabricImportDesc;
        pMemoryFabricImportV2->expNodeId = pFabricImportDesc->exportNodeId;
    }

    portSyncRwLockReleaseWrite(pFabricImportDesc->pLock);

    return status;
}

NV_STATUS
memoryfabricimportv2Control_IMPL
(
    MemoryFabricImportV2           *pMemoryFabricImportV2,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            memoryfabricimportv2IsReady(pMemoryFabricImportV2, NV_FALSE));

    return resControl_IMPL(staticCast(pMemoryFabricImportV2, RsResource),
                           pCallContext, pParams);
}

NV_STATUS
memoryfabricimportv2CtrlGetInfo_IMPL
(
    MemoryFabricImportV2        *pMemoryFabricImportV2,
    NV00F9_CTRL_GET_INFO_PARAMS *pParams
)
{
    FABRIC_IMPORT_MEMDESC_DATA *pMemdescData;
    Memory *pMemory = staticCast(pMemoryFabricImportV2, Memory);
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc;

    pFabricImportDesc = pMemoryFabricImportV2->pFabricImportDesc;

    portSyncRwLockAcquireRead(pFabricImportDesc->pLock);

    pMemdescData =
        (FABRIC_IMPORT_MEMDESC_DATA *)memdescGetMemData(pMemory->pMemDesc);

    pParams->memFlags = pMemdescData->memFlags;
    pParams->physAttrs = pMemdescData->physAttrs;
    pParams->size = memdescGetSize(pMemory->pMemDesc);
    pParams->pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);

    portSyncRwLockReleaseRead(pFabricImportDesc->pLock);

    return NV_OK;
}

NV_STATUS
memoryfabricimportv2GetMapAddrSpace_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2,
    CALL_CONTEXT         *pCallContext,
    NvU32                 mapFlags,
    NV_ADDRESS_SPACE     *pAddrSpace
)
{
    *pAddrSpace = ADDR_FABRIC_V2;
    return NV_OK;
}

void
memoryfabricimportv2RemoveFromCache_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc;

    pFabricImportDesc = pMemoryFabricImportV2->pFabricImportDesc;

    portSyncRwLockAcquireRead(pFabricImportDesc->pLock);

    fabricImportCacheDelete(pFabric, &pFabricImportDesc->uuid,
                            pFabricImportDesc->key);

    portSyncRwLockReleaseRead(pFabricImportDesc->pLock);
}

NvBool
memoryfabricimportv2IsExportAllowed_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2
)
{
    return NV_FALSE;
}

NvBool
memoryfabricimportv2IsGpuMapAllowed_IMPL
(
    MemoryFabricImportV2 *pMemoryFabricImportV2,
    OBJGPU               *pGpu
)
{
    Memory *pMemory = staticCast(pMemoryFabricImportV2, Memory);
    FABRIC_IMPORT_MEMDESC_DATA *pMemdescData;
    NvU32 cliqueId;
    NV_STATUS status;

    if (!gpuFabricProbeIsSupported(pGpu))
        return NV_TRUE;

    pMemdescData =
        (FABRIC_IMPORT_MEMDESC_DATA *)memdescGetMemData(pMemory->pMemDesc);

    status = gpuFabricProbeGetFabricCliqueId(pGpu->pGpuFabricProbeInfoKernel,
                                             &cliqueId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to query cliqueId 0x%x\n", status);
        return NV_FALSE;
    }

    return (cliqueId == pMemdescData->cliqueId);
}
