/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing memory export class
 *
 *****************************************************************************/

#define NVOC_MEM_EXPORT_H_PRIVATE_ACCESS_ALLOWED

/*
 * Lock ordering
 *
 * RMAPI Lock (can be removed once RM-core allows it, for now RO-only)
 * |_External Client Lock (e.g. CUDA client)
 *   |_Export Module Lock
 *     |_Export Descriptor Lock
 *       |_GPU(s) Lock as needed
 */

#include "os/os.h"
#include "core/locks.h"
#include "os/capability.h"
#include "core/system.h"
#include "core/thread_state.h"
#include "mem_mgr/mem_export.h"
#include "resserv/rs_resource.h"
#include "rmapi/client.h"
#include "compute/fabric.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "containers/list.h"

typedef struct mem_export_uuid
{
    NvU64 expId;
    NvU16 nodeId;
    NvU8  unused[6];
} MEM_EXPORT_UUID;

typedef struct attached_parent_info
{
    void     *pGpuOsInfo;
    OBJGPU   *pGpu;
    NvU64     refCount;
    NvHandle  hClient;
    NvHandle  hParent; // hDevice or hSubdevice

    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
} ATTACHED_PARENT_INFO;

MAKE_LIST(ATTACHED_PARENT_INFO_LIST, ATTACHED_PARENT_INFO);

typedef struct attached_mem_info
{
    ATTACHED_PARENT_INFO *pParentInfo;

    NvHandle hDupedMem;
    NvU8     addressSpace;
} ATTACHED_MEM_INFO;

typedef struct mem_export_info
{
    NV00E0_ALLOCATION_PARAMETERS cachedParams;

    NvU64           refCount;
    MEM_EXPORT_UUID uuid;

    NvS32 imexChannel;

    struct
    {
        NvU32 gpu;
        NvU32 migGi[KMIGMGR_MAX_GPU_SWIZZID];
    } attachedUsageCount[NV_MAX_DEVICES];

    ATTACHED_PARENT_INFO_LIST parentInfoList;

    // Temp stash before committing the change.
    ATTACHED_MEM_INFO stashMemInfos[NV00E0_MAX_EXPORT_HANDLES];

    //
    // The lock protects MEM_EXPORT_INFO, the mem export descriptor.
    //
    // The lock should be taken only if a mem export descriptor is safe
    // to access i.e., holding the module lock or the accessing thread
    // has the mem export descriptor refcounted.
    //
    PORT_RWLOCK *pLock;

    // Should be last
    ATTACHED_MEM_INFO memInfos[0];
} MEM_EXPORT_INFO;

NvU16
memoryExportGetNodeId
(
    NV_EXPORT_MEM_PACKET *pExportPacket
)
{
    ct_assert(sizeof(MEM_EXPORT_UUID) == sizeof(pExportPacket->uuid));

    MEM_EXPORT_UUID *pExportUuid = (MEM_EXPORT_UUID*)pExportPacket->uuid;

    return pExportUuid->nodeId;
}

static NV_STATUS
_memoryexportDup
(
    MemoryExport                 *pMemoryExport,
    NV00E0_ALLOCATION_PARAMETERS *pAllocParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    SysMemExportCacheEntry *pEntry;
    MEM_EXPORT_INFO *pExportInfo;
    MEM_EXPORT_UUID uuid;
    RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pMemoryExport), RmClient);
    NV_STATUS status = NV_OK;
    NvBool bImexDaemon;
    NvS32 imexChannel = pAllocParams->imexChannel;

    ct_assert(sizeof(NV_EXPORT_MEM_PACKET) == NV_MEM_EXPORT_PACKET_LEN);
    ct_assert(sizeof(MEM_EXPORT_UUID) == NV_MEM_EXPORT_UUID_LEN);

    portMemCopy(&uuid, NV_MEM_EXPORT_UUID_LEN,
                pAllocParams->packet.uuid, NV_MEM_EXPORT_UUID_LEN);

    //
    // Keep the module lock throughout so that the destructor can't delete
    // pExportInfo underneath us until we refcount it.
    //
    portSyncRwLockAcquireRead(pSys->pSysMemExportModuleLock);

    pEntry = multimapFindItem(&pSys->sysMemExportCache, uuid.nodeId,
                              uuid.expId);
    if (pEntry == NULL)
    {
        portSyncRwLockReleaseRead(pSys->pSysMemExportModuleLock);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pExportInfo = pEntry->pData;

    portSyncRwLockAcquireWrite(pExportInfo->pLock);

    //
    // If the client is IMEX daemon, we need to trust the channel
    // provided by it.
    //
    bImexDaemon = rmclientIsCapable(pRmClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT);

    if (bImexDaemon && (pExportInfo->imexChannel != imexChannel))
    {
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
    else if (!bImexDaemon && (pExportInfo->imexChannel != pRmClient->imexChannel))
    {
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
    else
    {
        *pAllocParams = pExportInfo->cachedParams;

        pExportInfo->refCount++;

        pMemoryExport->pExportInfo = pExportInfo;
    }

    portSyncRwLockReleaseWrite(pExportInfo->pLock);

    portSyncRwLockReleaseRead(pSys->pSysMemExportModuleLock);

    return status;
}

static void
_memoryexportGenerateUuid
(
    MEM_EXPORT_UUID *pUuid
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);

    ct_assert(sizeof(MEM_EXPORT_UUID) == NV_MEM_EXPORT_UUID_LEN);

    pUuid->nodeId = fabricGetNodeId(pFabric);
    pUuid->expId = portAtomicExIncrementU64(&pSys->sysExportObjectCounter);
}

static NV_STATUS
_memoryexportConstruct
(
    MemoryExport                 *pMemoryExport,
    NV00E0_ALLOCATION_PARAMETERS *pAllocParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    MEM_EXPORT_INFO *pExportInfo;
    NvU64 size;
    NV_STATUS status;
    SysMemExportCacheEntry *pEntry;
    SYS_MEM_EXPORT_CACHESubmap *pSubmap = NULL;
    RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pMemoryExport), RmClient);

    if (pRmClient->imexChannel == -1)
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    if (pAllocParams->numMaxHandles == 0)
        return NV_ERR_INVALID_ARGUMENT;

    size = sizeof(*pExportInfo) + (pAllocParams->numMaxHandles *
           sizeof(pExportInfo->memInfos[0]));

    pExportInfo = portMemAllocNonPaged(size);
    if (pExportInfo == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pExportInfo, 0, size);

    pExportInfo->imexChannel = pRmClient->imexChannel;

    _memoryexportGenerateUuid(&pExportInfo->uuid);

    pExportInfo->pLock = portSyncRwLockCreate(
                            portMemAllocatorGetGlobalNonPaged());
    if (pExportInfo->pLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto free_mem;
    }

    portMemCopy(pAllocParams->packet.uuid, NV_MEM_EXPORT_UUID_LEN,
                &pExportInfo->uuid, NV_MEM_EXPORT_UUID_LEN);

    pExportInfo->cachedParams.packet = pAllocParams->packet;
    pExportInfo->cachedParams.numMaxHandles = pAllocParams->numMaxHandles;
    pExportInfo->cachedParams.flags = pAllocParams->flags;

    portMemCopy(pExportInfo->cachedParams.metadata,
                NV_MEM_EXPORT_METADATA_LEN,
                pAllocParams->metadata,
                NV_MEM_EXPORT_METADATA_LEN);

    listInit(&pExportInfo->parentInfoList, portMemAllocatorGetGlobalNonPaged());

    pExportInfo->refCount = 1;

    pMemoryExport->pExportInfo = pExportInfo;

    // Cache entry now...
    portSyncRwLockAcquireWrite(pSys->pSysMemExportModuleLock);

    if (multimapFindSubmap(&pSys->sysMemExportCache,
                           pExportInfo->uuid.nodeId) == NULL)
    {
        if ((pSubmap = multimapInsertSubmap(&pSys->sysMemExportCache,
                           pExportInfo->uuid.nodeId)) == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto fail;
        }
    }

    pEntry = multimapInsertItemNew(&pSys->sysMemExportCache,
                                   pExportInfo->uuid.nodeId,
                                   pExportInfo->uuid.expId);
    if (pEntry == NULL)
    {
        status = NV_ERR_INSERT_DUPLICATE_NAME;
        goto fail;
    }

    pEntry->pData = pExportInfo;

    portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);

    return NV_OK;

fail:
    listDestroy(&pExportInfo->parentInfoList);

    if (pSubmap != NULL)
        multimapRemoveSubmap(&pSys->sysMemExportCache, pSubmap);

    portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);

    portSyncRwLockDestroy(pExportInfo->pLock);
    pExportInfo->pLock = NULL;

free_mem:
    portMemFree(pExportInfo);

    return status;
}

NV_STATUS
memoryexportConstruct_IMPL
(
    MemoryExport                 *pMemoryExport,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status;
    NV00E0_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;

    if (pAllocParams->flags & NV_MEM_EXPORT_FLAGS_DUP_BY_UUID)
        status = _memoryexportDup(pMemoryExport, pAllocParams);
    else
        status = _memoryexportConstruct(pMemoryExport, pAllocParams);

    return status;
}

static void
_memoryexportDetachParent
(
    MEM_EXPORT_INFO      *pExportInfo,
    ATTACHED_PARENT_INFO *pParentInfo
)
{
    THREAD_STATE_NODE *pThreadNode;
    THREAD_STATE_FREE_CALLBACK freeCallback;
    NvU32 deviceInstance;

    NV_ASSERT_OR_RETURN_VOID(pParentInfo->refCount == 0);

    if (pParentInfo->pGpuOsInfo != NULL)
    {
        deviceInstance = gpuGetDeviceInstance(pParentInfo->pGpu);

        if (pParentInfo->pKernelMIGGpuInstance != NULL)
        {
            KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pParentInfo->pKernelMIGGpuInstance;
            NvU32 swizzId = pKernelMIGGpuInstance->swizzId;

            pExportInfo->attachedUsageCount[deviceInstance].migGi[swizzId]--;

            // Update giIdMasks if the MIG instance is no more used..
            if (pExportInfo->attachedUsageCount[deviceInstance].migGi[swizzId] == 0)
                pExportInfo->cachedParams.giIdMasks[deviceInstance] &= !NVBIT(swizzId);

            // Drop refcount on GPU instance..
            NV_ASSERT_OK(kmigmgrDecRefCount(pKernelMIGGpuInstance->pShare));
        }

        // Drop refcount on GPU
        NV_ASSERT_OR_RETURN_VOID(threadStateGetCurrent(&pThreadNode,
                                                       NULL) == NV_OK);

        freeCallback.pCb = osReleaseGpuOsInfo;
        freeCallback.pCbData = (void *)pParentInfo->pGpuOsInfo;

        NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode,
                                                      &freeCallback));

        pExportInfo->attachedUsageCount[deviceInstance].gpu--;

        // Update deviceInstanceMask if the GPU is no more used..
        if (pExportInfo->attachedUsageCount[deviceInstance].gpu == 0)
            pExportInfo->cachedParams.deviceInstanceMask &= !NVBIT(deviceInstance);
    }

    listRemove(&pExportInfo->parentInfoList, pParentInfo);
}

static void
_memoryexportUndupMem
(
    ATTACHED_PARENT_INFO *pParentInfo,
    ATTACHED_MEM_INFO    *pMemInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OK(pRmApi->Free(pRmApi, pParentInfo->hClient,
                              pMemInfo->hDupedMem));

    pMemInfo->hDupedMem = 0;
}

static void
_memoryexportDetachMemAndParent
(
    MEM_EXPORT_INFO    *pExportInfo,
    ATTACHED_MEM_INFO  *pMemInfo
)
{
    ATTACHED_PARENT_INFO *pParentInfo = pMemInfo->pParentInfo;

    _memoryexportUndupMem(pParentInfo, pMemInfo);

    pParentInfo->refCount--;
    pMemInfo->pParentInfo = NULL;

    pExportInfo->cachedParams.numCurHandles--;

    // If parent info is unused, drop it.
    if (pParentInfo->refCount == 0)
        _memoryexportDetachParent(pExportInfo, pParentInfo);
}

void
memoryexportDestruct_IMPL
(
    MemoryExport *pMemoryExport
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    MEM_EXPORT_INFO *pExportInfo = pMemoryExport->pExportInfo;
    SYS_MEM_EXPORT_CACHESubmap *pSubmap;
    NvU16 i, j;
    NvU32 gpuMask = GPUS_LOCK_ALL;

    if (pExportInfo == NULL)
        return;

    pMemoryExport->pExportInfo = NULL;

    //
    // Take pSysMemExportModuleLock to synchronize with _memoryexportDup().
    // We don't want to delete pExportInfo under it.
    //
    portSyncRwLockAcquireWrite(pSys->pSysMemExportModuleLock);
    portSyncRwLockAcquireWrite(pExportInfo->pLock);

    pExportInfo->refCount--;

    if (pExportInfo->refCount > 0)
    {
        portSyncRwLockReleaseWrite(pExportInfo->pLock);
        portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);

        return;
    }

    //
    // Continue to hold pSysMemExportModuleLock to update the
    // sysMemExportCache and may undup/detach memory from
    // hSysMemExportClient
    //

    // Empty caches so new calls to _memoryexportDup() fail.
    multimapRemoveItemByKey(&pSys->sysMemExportCache,
                            pExportInfo->uuid.nodeId,
                            pExportInfo->uuid.expId);

    pSubmap = multimapFindSubmap(&pSys->sysMemExportCache,
                                 pExportInfo->uuid.nodeId);
    if ((pSubmap != NULL) &&
        (multimapCountSubmapItems(&pSys->sysMemExportCache, pSubmap) == 0))
        multimapRemoveSubmap(&pSys->sysMemExportCache, pSubmap);

    // Now I am the only one holding the pExportInfo, drop the lock.
    portSyncRwLockReleaseWrite(pExportInfo->pLock);

    NV_ASSERT_OK(rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_MEM,
                                       &gpuMask));

    for (i = 0; i < pExportInfo->cachedParams.numMaxHandles; i++)
    {
        if (pExportInfo->memInfos[i].hDupedMem != 0)
        {
            _memoryexportDetachMemAndParent(pExportInfo,
                                            &pExportInfo->memInfos[i]);
        }
    }

    // Drop rest of the locks as memory undup/detach is done..
    rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);

    for (j = 0; j < NV_MAX_DEVICES; j++)
    {
        NV_ASSERT(pExportInfo->attachedUsageCount[j].gpu == 0);

        for (i = 0; i < KMIGMGR_MAX_GPU_SWIZZID; i++)
            NV_ASSERT(pExportInfo->attachedUsageCount[j].migGi[i] == 0);
        NV_ASSERT(pExportInfo->cachedParams.giIdMasks[j] == 0);
    }

    NV_ASSERT(pExportInfo->cachedParams.numCurHandles == 0);
    NV_ASSERT(pExportInfo->cachedParams.deviceInstanceMask == 0);

    NV_ASSERT(listCount(&pExportInfo->parentInfoList) == 0);
    listDestroy(&pExportInfo->parentInfoList);

    portSyncRwLockDestroy(pExportInfo->pLock);
    pExportInfo->pLock = NULL;

    portMemFree(pExportInfo);
    pMemoryExport->pExportInfo = NULL;
}

NvBool
memoryexportCanCopy_IMPL
(
    MemoryExport *pMemoryExport
)
{
    return NV_FALSE;
}

static NV_STATUS
_memoryexportVerifyMem
(
    OBJGPU                  *pGpu,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    RsClient                *pClient,
    NvHandle                 hSrcHandle,
    NvU8                    *pAddrSpace
)
{
    NV_STATUS status;
    RsResourceRef *pSrcMemoryRef;
    Memory *pSrcMemory;
    MEMORY_DESCRIPTOR *pMemDesc;
    CALL_CONTEXT callContext;
    NvU32 addrSpace;
    NvU32 mapFlags;

    //
    // Don't not use memGetByHandle() or access pSrcMemory->pMemDesc here.
    // There are certain memory types like MCFLA which might not be ready
    // at the time of attachment, and hence could cause memGetByHandle()
    // fail with NV_ERR_NOT_READY, which we don't want in this case.
    //
    status = clientGetResourceRef(pClient, hSrcHandle, &pSrcMemoryRef);
    if (status != NV_OK)
        return status;

    pSrcMemory = dynamicCast(pSrcMemoryRef->pResource, Memory);
    if (pSrcMemory == NULL)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    if (!memIsExportAllowed(pSrcMemory))
        return NV_ERR_NOT_SUPPORTED;

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pClient = pClient;
    callContext.pResourceRef = pSrcMemoryRef;

    //
    // rmApiGetEffectiveAddrSpace expect mapping flags to be set as DIRECT for
    // GPU cacheable Sysmem for Pre-Ampere chips. We are not doing any mapping
    // here, so passing this as workaround to get the expected address space.
    //
    mapFlags = FLD_SET_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, 0);

    status = memGetMapAddrSpace(pSrcMemory, &callContext, mapFlags, &addrSpace);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to query address space: 0x%x\n", status);
        return status;
    }

    switch(addrSpace)
    {
        case ADDR_SYSMEM:
            *pAddrSpace = NV00E0_ADDR_SPACE_TYPE_SYSMEM;
            break;
        case ADDR_FBMEM:
            *pAddrSpace = NV00E0_ADDR_SPACE_TYPE_VIDMEM;
            break;
        case ADDR_FABRIC_V2:
            *pAddrSpace = NV00E0_ADDR_SPACE_TYPE_FABRIC;
            break;
        case ADDR_FABRIC_MC:
            *pAddrSpace = NV00E0_ADDR_SPACE_TYPE_FABRIC_MC;
            break;
        default:
            return NV_ERR_NOT_SUPPORTED;
    }

    // No need to inspect parent GPU if a deviceless object
    if (pGpu == NULL)
        return NV_OK;

    // MIG is about vidmem partitioning, so limit the check.
    if ((pKernelMIGGpuInstance != NULL) && (addrSpace == ADDR_FBMEM))
    {
        if ((pKernelMIGGpuInstance->pMemoryPartitionHeap != pSrcMemory->pHeap))
            return NV_ERR_INVALID_OBJECT_PARENT;
    }

    // Check if hMemory belongs to the same pGpu
    pMemDesc = pSrcMemory->pMemDesc;
    if ((pSrcMemory->pGpu != pGpu) && (pMemDesc->pGpu != pGpu))
        return NV_ERR_INVALID_OBJECT_PARENT;

    return NV_OK;
}

static NV_STATUS
_memoryexportValidateAndDupMem
(
    RsClient             *pSrcClient,
    NvHandle              hSrcMem,
    ATTACHED_PARENT_INFO *pDestParentInfo,
    ATTACHED_MEM_INFO    *pMemInfo
)
{
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    status = _memoryexportVerifyMem(pDestParentInfo->pGpu,
                                    pDestParentInfo->pKernelMIGGpuInstance,
                                    pSrcClient, hSrcMem,
                                    &pMemInfo->addressSpace);
    if (status != NV_OK)
        return status;

    status = pRmApi->DupObject(pRmApi, pDestParentInfo->hClient,
                               pDestParentInfo->hParent,
                               &pMemInfo->hDupedMem,
                               pSrcClient->hClient, hSrcMem, 0);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

//
// This function is called without holding GPU lock, so don't access GPU until
// validated.
//
static NV_STATUS
_memoryexportValidateParent
(
    NV00E0_CTRL_EXPORT_MEM_PARAMS  *pExportParams,
    void                          **ppGpuOsInfo,
    OBJGPU                        **ppGpu,
    Device                        **ppDevice,
    Subdevice                     **ppSubdevice
)
{
    NV_STATUS status;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams = pCallContext->pControlParams;
    RsClient *pClient = pCallContext->pClient;

    *ppGpuOsInfo = NULL;
    *ppGpu = NULL;
    *ppDevice = NULL;
    *ppSubdevice = NULL;

    // Non-device parent, nothing to do..
    if (pClient->hClient == pExportParams->hParent)
        return NV_OK;

    //
    // Validate the parent subdevice/device is same as that of ref-counted by
    // the OS layer.
    //
    status = deviceGetByHandle(pClient, pExportParams->hParent, ppDevice);
    if (status != NV_OK)
    {
        status = subdeviceGetByHandle(pClient, pExportParams->hParent,
                                      ppSubdevice);
        if (status != NV_OK)
            return status;

        *ppGpu = GPU_RES_GET_GPU(*ppSubdevice);
    }
    else
    {
        *ppGpu = GPU_RES_GET_GPU(*ppDevice);
    }

    if(!osMatchGpuOsInfo(*ppGpu, pParams->secInfo.gpuOsInfo))
        return NV_ERR_INVALID_DEVICE;

    *ppGpuOsInfo = pParams->secInfo.gpuOsInfo;

    return NV_OK;
}

static NV_STATUS
_memoryexportGetParentHandles
(
    OBJGPU                   *pGpu,
    Device                   *pDevice,
    Subdevice                *pSubdevice,
    KERNEL_MIG_GPU_INSTANCE **ppKernelMIGGpuInstance,
    NvHandle                 *phParentClient,
    NvHandle                 *phParentObject
)
{
    NV_STATUS status;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    *phParentClient = pSys->hSysMemExportClient;
    *phParentObject = pSys->hSysMemExportClient;
    *ppKernelMIGGpuInstance = NULL;

    // Non-device parent, nothing to do..
    if (pGpu == NULL)
        return NV_OK;

    if (IS_MIG_ENABLED(pGpu))
    {
        MIG_INSTANCE_REF ref;
        Device *pTempDevice = (pSubdevice != NULL) ? GPU_RES_GET_DEVICE(pSubdevice) : pDevice;

        status = kmigmgrGetInstanceRefFromDevice(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu),
                                                 pTempDevice, &ref);
        if (status != NV_OK)
            return status;

        if (!kmigmgrIsMIGReferenceValid(&ref))
            return NV_ERR_INVALID_STATE;

        status = kmigmgrIncRefCount(ref.pKernelMIGGpuInstance->pShare);
        if (status != NV_OK)
            return status;

        *ppKernelMIGGpuInstance = ref.pKernelMIGGpuInstance;

        if (pSubdevice != NULL)
        {
            *phParentClient = ref.pKernelMIGGpuInstance->instanceHandles.hClient;
            *phParentObject = ref.pKernelMIGGpuInstance->instanceHandles.hSubdevice;
        }
        else
        {
            *phParentClient = ref.pKernelMIGGpuInstance->instanceHandles.hClient;
            *phParentObject = ref.pKernelMIGGpuInstance->instanceHandles.hDevice;
        }
    }
    else
    {
        if (pSubdevice != NULL)
        {
            *phParentClient = GPU_GET_MEMORY_MANAGER(pGpu)->hClient;
            *phParentObject = GPU_GET_MEMORY_MANAGER(pGpu)->hSubdevice;
        }
        else
        {
            *phParentClient = GPU_GET_MEMORY_MANAGER(pGpu)->hClient;
            *phParentObject = GPU_GET_MEMORY_MANAGER(pGpu)->hDevice;
        }
    }

    return NV_OK;
}

NV_STATUS
memoryexportCtrlExportMem_IMPL
(
    MemoryExport                  *pMemoryExport,
    NV00E0_CTRL_EXPORT_MEM_PARAMS *pParams
)
{
    NV_STATUS status;
    NvU16 result;
    NvU16 i = 0, j;
    MEM_EXPORT_INFO *pExportInfo = pMemoryExport->pExportInfo;
    ATTACHED_PARENT_INFO *pParentInfo;
    ATTACHED_MEM_INFO *pMemInfo;
    OBJGPU *pGpu;
    void *pGpuOsInfo;
    Device *pDevice;
    Subdevice *pSubdevice;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32 gpuMask = GPUS_LOCK_ALL;
    NvBool bModuleLockAcquired = NV_FALSE;
    NvBool bAllGpuLockAcquired = NV_FALSE;

    if ((pParams->numHandles == 0) ||
        (pParams->numHandles > NV00E0_MAX_EXPORT_HANDLES))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // pExportInfo->cachedParams.numMaxHandles is an immutable attribute,
    // can be queried outside of the locks.
    //
    if ((!portSafeAddU16(pParams->index, pParams->numHandles, &result)) ||
        (result > pExportInfo->cachedParams.numMaxHandles))
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    status = _memoryexportValidateParent(pParams, &pGpuOsInfo, &pGpu, &pDevice, &pSubdevice);
    if (status != NV_OK)
    {
        return status;
    }

    //
    // During device-less memory dup, take module lock to protect
    // pSys->hSysMemExportClient. RM-core expects client locking to
    // be managed by the caller in this case, rather than relying
    // on dual client locking.
    //
    if (pGpu == NULL)
    {
        portSyncRwLockAcquireWrite(pSys->pSysMemExportModuleLock);
        bModuleLockAcquired = NV_TRUE;
    }

    portSyncRwLockAcquireWrite(pExportInfo->pLock);

    pParentInfo = listAppendNew(&pExportInfo->parentInfoList);
    if (pParentInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pParentInfo->pGpuOsInfo = pGpuOsInfo;
    pParentInfo->pGpu = pGpu;
    pParentInfo->refCount = 0;
    pParentInfo->hClient = 0;
    pParentInfo->hParent = 0;
    pParentInfo->pKernelMIGGpuInstance = NULL;

    status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                   GPUS_LOCK_FLAGS_NONE,
                                   RM_LOCK_MODULES_MEM,
                                   &gpuMask);
    if (status != NV_OK)
    {
        NV_ASSERT(0);
        goto fail;
    }

    bAllGpuLockAcquired = NV_TRUE;

    status = _memoryexportGetParentHandles(pGpu, pDevice, pSubdevice,
                                           &pParentInfo->pKernelMIGGpuInstance,
                                           &pParentInfo->hClient,
                                           &pParentInfo->hParent);
    if (status != NV_OK)
        goto fail;

    // Duped memory and store it in the stash
    for (i = 0; i < pParams->numHandles; i++)
    {
        if (pParams->handles[i] == 0)
            continue;

        pMemInfo = &pExportInfo->stashMemInfos[i];

        status = _memoryexportValidateAndDupMem(RES_GET_CLIENT(pMemoryExport),
                                                pParams->handles[i],
                                                pParentInfo, pMemInfo);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to duping 0x%x\n", status);
            goto fail;
        }
    }

    // Start committing now.

    // !!! Don't expect any failures from this point onward !!!

    for (i = 0; i < pParams->numHandles; i++)
    {
        pMemInfo = &pExportInfo->memInfos[i + pParams->index];

        // If the handle already exists in this position, detach it
        if (pMemInfo->hDupedMem != 0)
        {
            _memoryexportDetachMemAndParent(pExportInfo, pMemInfo);

            NV_ASSERT(pMemInfo->hDupedMem == 0);
        }

        // Nothing to attach, continue..
        if (pExportInfo->stashMemInfos[i].hDupedMem == 0)
            continue;

        // Attach successful
        *pMemInfo = pExportInfo->stashMemInfos[i];

        // Ref-count parent and cache for future use.
        pParentInfo->refCount++;
        pMemInfo->pParentInfo = pParentInfo;

        pExportInfo->cachedParams.numCurHandles++;

        // Clear stash
        pExportInfo->stashMemInfos[i].hDupedMem = 0;
    }

    // Attach GPU if applicable...
    if (pGpu != NULL)
    {
        NvU32 deviceInstance = gpuGetDeviceInstance(pGpu);

        pExportInfo->attachedUsageCount[deviceInstance].gpu++;
        pExportInfo->cachedParams.deviceInstanceMask |= NVBIT(deviceInstance);

        if (pParentInfo->pKernelMIGGpuInstance != NULL)
        {
            NvU32 swizzId = pParentInfo->pKernelMIGGpuInstance->swizzId;

            pExportInfo->attachedUsageCount[deviceInstance].migGi[swizzId]++;
            pExportInfo->cachedParams.giIdMasks[deviceInstance] |= NVBIT(swizzId);
        }
    }

    //
    // If this was mem detach only call, detach the new parent info which is
    // unused.
    //
    if (pParentInfo->refCount == 0)
        _memoryexportDetachParent(pExportInfo, pParentInfo);

    goto done;

fail:
    for (j = 0; j < i; j++)
    {
        pMemInfo = &pExportInfo->stashMemInfos[j];
        if (pMemInfo->hDupedMem != 0)
            _memoryexportUndupMem(pParentInfo, pMemInfo);
    }

    if (pParentInfo->pKernelMIGGpuInstance != NULL)
        NV_ASSERT_OK(kmigmgrDecRefCount(pParentInfo->pKernelMIGGpuInstance->pShare));

    listRemove(&pExportInfo->parentInfoList, pParentInfo);

done:
    if (bAllGpuLockAcquired)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    portSyncRwLockReleaseWrite(pExportInfo->pLock);

    if (bModuleLockAcquired)
        portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);

    return status;
}

static NV_STATUS
_memoryexportFindImporterMIGParent
(
    RsClient             *pImpClient,
    ATTACHED_PARENT_INFO *pSrcParentInfo,
    NvHandle             *pImpParentHandle
)
{
    NV_STATUS status;
    OBJGPU *pSrcGpu;
    KERNEL_MIG_GPU_INSTANCE *pSrcKernelMIGGpuInstance;
    NvBool bDevice;
    RS_ITERATOR it;

    pSrcGpu = pSrcParentInfo->pGpu;
    NV_ASSERT_OR_RETURN(pSrcGpu != NULL, NV_ERR_INVALID_STATE);

    pSrcKernelMIGGpuInstance = pSrcParentInfo->pKernelMIGGpuInstance;
    NV_ASSERT_OR_RETURN(pSrcKernelMIGGpuInstance != NULL, NV_ERR_INVALID_STATE);

    *pImpParentHandle = 0;

    bDevice = (pSrcKernelMIGGpuInstance->instanceHandles.hDevice  == pSrcParentInfo->hParent);

    // In MIG, multiple devices of same the device instance are feasible.
    it = clientRefIter(pImpClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

    while (clientRefIterNext(pImpClient, &it))
    {
        Device *pImpDevice = dynamicCast(it.pResourceRef->pResource, Device);
        Subdevice *pImpSubdevice;
        MIG_INSTANCE_REF impRef;

        if (pImpDevice == NULL)
            continue;

        status = kmigmgrGetInstanceRefFromDevice(pSrcGpu, GPU_GET_KERNEL_MIG_MANAGER(pSrcGpu),
                                                 pImpDevice, &impRef);
        if (status != NV_OK)
            continue;

        if (pSrcKernelMIGGpuInstance != impRef.pKernelMIGGpuInstance)
            continue;

        if (bDevice)
        {
            *pImpParentHandle = RES_GET_HANDLE(pImpDevice);
        }
        else
        {
            status = subdeviceGetByDeviceAndGpu(pImpClient, pImpDevice, pSrcGpu, &pImpSubdevice);
            if (status != NV_OK)
                continue;

            *pImpParentHandle = RES_GET_HANDLE(pImpSubdevice);
        }

        break;
    }

    return (*pImpParentHandle == 0) ? NV_ERR_OBJECT_NOT_FOUND : NV_OK;
}

static NV_STATUS
_memoryexportFindImporterParent
(
    RsClient             *pImpClient,
    ATTACHED_PARENT_INFO *pSrcParentInfo,
    NvHandle             *pImpParentHandle
)
{
    NV_STATUS status;
    OBJGPU *pSrcGpu = pSrcParentInfo->pGpu;
    NvBool bDevice;
    Device *pImpDevice;
    Subdevice *pImpSubdevice;

    // If device-less memory, use client as importer parent
    if (pSrcGpu == NULL)
    {
        *pImpParentHandle = pImpClient->hClient;
        return NV_OK;
    }

    if (IS_MIG_ENABLED(pSrcGpu))
    {
        return _memoryexportFindImporterMIGParent(pImpClient, pSrcParentInfo,
                                                  pImpParentHandle);
    }

    // If source is device, then return device as parent
    bDevice = (GPU_GET_MEMORY_MANAGER(pSrcGpu)->hDevice == pSrcParentInfo->hParent);

    status = deviceGetByGpu(pImpClient, pSrcGpu, NV_TRUE, &pImpDevice);
    if (status != NV_OK)
        return status;

    if (bDevice)
    {
        *pImpParentHandle = RES_GET_HANDLE(pImpDevice);
        return NV_OK;
    }

    status = subdeviceGetByDeviceAndGpu(pImpClient, pImpDevice, pSrcGpu, &pImpSubdevice);
    if (status != NV_OK)
        return status;

    *pImpParentHandle = RES_GET_HANDLE(pImpSubdevice);

    return NV_OK;
}

NV_STATUS
memoryexportCtrlImportMem_IMPL
(
    MemoryExport                  *pMemoryExport,
    NV00E0_CTRL_IMPORT_MEM_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status;
    NvU16 result;
    NvU16 i = 0, j;
    MEM_EXPORT_INFO *pExportInfo = pMemoryExport->pExportInfo;
    ATTACHED_MEM_INFO *pMemInfo;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle hDstParent;
    NvHandle hDstClient = RES_GET_CLIENT(pMemoryExport)->hClient;
    NvU32 gpuMask = GPUS_LOCK_ALL;
    NvBool bAllGpuLockAcquired = NV_FALSE;

    if ((pParams->numHandles == 0) ||
        (pParams->numHandles > NV00E0_MAX_IMPORT_HANDLES))
        return NV_ERR_INVALID_ARGUMENT;

    //
    // pExportInfo->cachedParams.numMaxHandles is an immutable attribute,
    // can be queried outside of the locks.
    //
    if ((!portSafeAddU16(pParams->index, pParams->numHandles, &result)) ||
        (result > pExportInfo->cachedParams.numMaxHandles))
        return NV_ERR_OUT_OF_RANGE;

    //
    // During import we might read (dup) the hSysMemExportClient's client
    // database to dup from it. So, during device-less memory dup, take module
    // lock to protect pSys->hSysMemExportClient. RM-core expects client
    // locking to be managed by the caller in this case, rather than relying
    // on dual client locking (as there could be a lock inversion issue to
    // lock another client, if one client is already locked).
    //
    portSyncRwLockAcquireRead(pSys->pSysMemExportModuleLock);

    portSyncRwLockAcquireWrite(pExportInfo->pLock);

    status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                   GPUS_LOCK_FLAGS_NONE,
                                   RM_LOCK_MODULES_MEM,
                                   &gpuMask);
    if (status != NV_OK)
    {
        NV_ASSERT(0);
        goto done;
    }

    bAllGpuLockAcquired = NV_TRUE;

    for (i = 0; i < pParams->numHandles; i++)
    {
        pMemInfo = &pExportInfo->memInfos[i + pParams->index];

        // Nothing to import
        if (pMemInfo->hDupedMem == 0)
        {
            portMemSet(&pParams->memInfos[i], 0, sizeof(pParams->memInfos[0]));
            continue;
        }

        if (pParams->handles[i] == 0)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid handle\n");
            status = NV_ERR_INVALID_OBJECT_HANDLE;
            goto fail;
        }

        status = _memoryexportFindImporterParent(RES_GET_CLIENT(pMemoryExport),
                                                 pMemInfo->pParentInfo,
                                                 &hDstParent);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to find parent: 0x%x\n", status);
            goto fail;
        }

        status = pRmApi->DupObject(pRmApi,
                                   hDstClient,
                                   hDstParent,
                                   &pParams->handles[i],
                                   pMemInfo->pParentInfo->hClient,
                                   pMemInfo->hDupedMem, 0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to duping 0x%x\n", status);
            goto fail;
        }

        pParams->memInfos[i].addrSpace = pMemInfo->addressSpace;
        pParams->memInfos[i].hParent = hDstParent;
    }

    goto done;

fail:
    for (j = 0; j < i; j++)
    {
        if (pParams->memInfos[j].hParent != 0)
            pRmApi->Free(pRmApi, hDstClient, pParams->handles[j]);
    }

done:
    if (bAllGpuLockAcquired)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    portSyncRwLockReleaseWrite(pExportInfo->pLock);

    portSyncRwLockReleaseRead(pSys->pSysMemExportModuleLock);

    return status;
}

NV_STATUS
memoryexportCtrlGetInfo_IMPL
(
    MemoryExport                *pMemoryExport,
    NV00E0_CTRL_GET_INFO_PARAMS *pParams
)
{
    MEM_EXPORT_INFO *pExportInfo = pMemoryExport->pExportInfo;

    portSyncRwLockAcquireRead(pExportInfo->pLock);

    pParams->info = pMemoryExport->pExportInfo->cachedParams;

    portSyncRwLockReleaseRead(pExportInfo->pLock);

    return NV_OK;
}

NV_STATUS
memoryexportControl_IMPL
(
    MemoryExport                   *pMemoryExport,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    //
    // Note: GPU lock(s) is required for some control calls. Thus, it is
    // incorrect to take the leaf lock here. resControl_IMPL() attempts to
    // acquire the GPU locks before it calls the control call body.
    //
    return resControl_IMPL(staticCast(pMemoryExport, RsResource),
                           pCallContext, pParams);
}

void
memoryexportClearCache
(
    NvU16 nodeId
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    SYS_MEM_EXPORT_CACHESubmap *pSubmap;

    portSyncRwLockAcquireWrite(pSys->pSysMemExportModuleLock);

    pSubmap = multimapFindSubmap(&pSys->sysMemExportCache, nodeId);

    if (pSubmap != NULL)
    {
        SYS_MEM_EXPORT_CACHEIter it =
                multimapSubmapIterItems(&pSys->sysMemExportCache, pSubmap);

        while (multimapItemIterNext(&it) != 0)
            multimapRemoveItem(&pSys->sysMemExportCache, it.pValue);

        multimapRemoveSubmap(&pSys->sysMemExportCache, pSubmap);
    }

    portSyncRwLockReleaseWrite(pSys->pSysMemExportModuleLock);
}
