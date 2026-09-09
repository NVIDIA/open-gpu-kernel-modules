/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/memacct.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "rmapi/client.h"
#include "gpu/gpu.h"

typedef struct ClientGroupLimits
{
    NvLength SoftLimit;
    NvLength HardLimit;
    NvLength Available;
} ClientGroupLimits;

// map from cgroup id to the limits/stats for that specific group
MAKE_MAP(ClientGroupMapType, ClientGroupLimits);

typedef struct GpuRegion
{
    ClientGroupMapType clientGroupMap;
    struct {
        void *region;
        void *prechargePool;
        NvU64 precharge;
    } os;
} GpuRegion;

// map region id (gpuId) to region's group limit map
MAKE_MAP(GpuRegionMapType, GpuRegion);

static struct {
    GpuRegionMapType GpuRegionMap;
    NvCgroupImpl impl;
    PORT_MUTEX *mutex;
} g_memacct;

struct MemoryCharge
{
    NvU32 gpuId;
    NvU32 pid;
    ClientGroupID cligrp;
    NvLength size;
    void *osPool;
    int refCount;
};

static NV_STATUS memacctTryChargeOs(void *osRegion, ClientGroupID cligrp, NvLength size, MemoryCharge **ppCharge)
{
    MemoryCharge *pCharge;
    void *pPool;
    NV_STATUS status;

    *ppCharge = NULL;

    status = osMemacctTryCharge(osRegion, size, &pPool, NULL);
    if (status == NV_ERR_RESOURCE_ACCOUNTING_HARD_LIMIT_EXCEEDED)
    {
        NV_PRINTF(LEVEL_INFO, "charge %llu osRegion 0x%016llx rejected\n", size, (NvU64)osRegion);
        return NV_ERR_RESOURCE_ACCOUNTING_HARD_LIMIT_EXCEEDED;
    }
    else if (status != NV_OK)
    {
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "charge %llu for osRegion 0x%016llx pool 0x%016llx ok\n", size, (NvU64)osRegion, (NvU64)pPool);

    pCharge = portMemAllocNonPaged(sizeof *pCharge);
    if (pCharge == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "release %llu for pool 0x%016llx (charge structure allocation failure)\n",
            size, (NvU64)pPool);
        osMemacctReleaseCharge(pPool, size);
        return NV_ERR_NO_MEMORY;
    }

    pCharge->size = size;
    pCharge->osPool = pPool;
    pCharge->refCount = 1;
    *ppCharge = pCharge;

    if ((osCgroupParent(cligrp) != NULL) && osCgroupCanEvict(NULL, pPool))
        return NV_WARN_RESOURCE_ACCOUNTING_SOFT_LIMIT_EXCEEDED;

    return NV_OK;
}

static ClientGroupLimits *memacctLimitsForGroupLocked(GpuRegion *pRegion, ClientGroupID *cligrp)
{
    ClientGroupLimits *pLimits = NULL;
    ClientGroupID limitingGroup = *cligrp;
    while (limitingGroup != NULL)
    {
        pLimits = mapFind(&pRegion->clientGroupMap, (NvU64)limitingGroup);
        if (pLimits != NULL)
        {
            *cligrp = limitingGroup;
            break;
        }

        limitingGroup = osCgroupParent(limitingGroup);
    }
    return pLimits;
}

static NV_STATUS memacctTryChargeInternalLocked(GpuRegion *pRegion, ClientGroupID cligrp, NvU32 gpuId, NvLength size, MemoryCharge *pCharge)
{
    NvLength current;
    ClientGroupLimits *pLimits;

    pLimits = memacctLimitsForGroupLocked(pRegion, &cligrp);
    if (pLimits == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "region %x no limits set for group %016llx\n", gpuId,
            (NvU64)cligrp);
        return NV_ERR_INVALID_LIMIT;
    }

    if (size > pLimits->Available)
    {
        NV_PRINTF(LEVEL_INFO, "region %x group %016llx request %llu over limit, available %llu/%llu\n",
            gpuId, (NvU64)cligrp, size, pLimits->Available, pLimits->HardLimit);
        return NV_ERR_RESOURCE_ACCOUNTING_HARD_LIMIT_EXCEEDED;
    }

    pCharge->size = size;
    pCharge->cligrp = cligrp;
    pCharge->refCount = 1;
    pLimits->Available -= size;

    current = pLimits->HardLimit - pLimits->Available;
    NV_PRINTF(LEVEL_INFO, "region %x group %016llx charged %llu total %llu/%llu\n", gpuId,
        (NvU64)cligrp, size, current, pLimits->HardLimit);

    if (current > pLimits->SoftLimit)
        return NV_WARN_RESOURCE_ACCOUNTING_SOFT_LIMIT_EXCEEDED;

    return NV_OK;
}

void memacctIncrementChargeRefCount(MemoryCharge *pCharge)
{
    if (pCharge == NULL)
        return;

    pCharge->refCount++;
}

NV_STATUS memacctTryCharge(RmClient *pRmClient, NvU32 gpuId, NvLength size, MemoryCharge **ppCharge)
{
    NV_STATUS status;
    ClientGroupID cligrp;
    GpuRegion *pRegion;

    if (g_memacct.impl == CGROUP_IMPL_NONE)
        return NV_OK;

    cligrp = osClientGroupID(pRmClient->ProcID, pRmClient->pOsPidInfo);
    if (cligrp == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "no cgroup found for pid %d\n", pRmClient->ProcID);
        return NV_OK;
    }

    // attempt to pre-allocate charge structure outside the lock for the fallback/misc cgroup case
    if (g_memacct.impl == CGROUP_IMPL_FALLBACK)
    {
        *ppCharge = portMemAllocNonPaged(sizeof **ppCharge);
        if (*ppCharge == NULL)
        {
            NV_PRINTF(LEVEL_INFO, "region %x group %016llx request %llu charge structure allocation failure\n",
                gpuId, (NvU64)cligrp, size);
            return NV_ERR_NO_MEMORY;
        }
    }

    portSyncMutexAcquire(g_memacct.mutex);
    pRegion = mapFind(&g_memacct.GpuRegionMap, gpuId);
    if (pRegion == NULL)
    {
        portSyncMutexRelease(g_memacct.mutex);
        NV_PRINTF(LEVEL_INFO, "region %x not found\n", gpuId);
        portMemFree(*ppCharge);
        *ppCharge = NULL;
        return NV_OK;
    }

    if (g_memacct.impl == CGROUP_IMPL_OS)
    {
        void *osRegion = pRegion->os.region;
        portSyncMutexRelease(g_memacct.mutex);
        status = memacctTryChargeOs(osRegion, cligrp, size, ppCharge);
    }
    else
    {
        status = memacctTryChargeInternalLocked(pRegion, cligrp, gpuId, size, *ppCharge);
        portSyncMutexRelease(g_memacct.mutex);
        if ((status != NV_OK) && (status != NV_WARN_RESOURCE_ACCOUNTING_SOFT_LIMIT_EXCEEDED))
        {
            // not really an error case, just need to clean up the pre-allocation
            if (status == NV_ERR_INVALID_LIMIT)
                status = NV_OK;

            portMemFree(*ppCharge);
            *ppCharge = NULL;
        }
    }

    if (*ppCharge != NULL)
    {
        (*ppCharge)->gpuId = gpuId;
        (*ppCharge)->pid = pRmClient->ProcID;
    }

    if (status == NV_WARN_RESOURCE_ACCOUNTING_SOFT_LIMIT_EXCEEDED)
    {
        gpuNotifySubDeviceEvent(gpumgrGetGpuFromId(gpuId), NV2080_NOTIFIERS_MEMACCT_SOFT_LIMIT_EXCEEDED,
            NULL, 0, pRmClient->ProcID, 0);

        status = NV_OK;
    }

    return status;
}

static NvBool memacctReleaseChargeInternal(MemoryCharge *pCharge)
{
    NvBool notify = NV_FALSE;
    GpuRegion *pRegion;
    ClientGroupLimits *pLimits;
    NvLength before;
    NvLength after;

    portSyncMutexAcquire(g_memacct.mutex);
    pRegion = mapFind(&g_memacct.GpuRegionMap, pCharge->gpuId);
    if (pRegion == NULL)
        goto done;

    pLimits = mapFind(&pRegion->clientGroupMap, (NvU64)pCharge->cligrp);
    if (pLimits == NULL)
        goto done;

    // uncharge
    before = pLimits->HardLimit - pLimits->Available;
    pLimits->Available += pCharge->size;
    after = pLimits->HardLimit - pLimits->Available;

    NV_PRINTF(LEVEL_INFO, "region %x group %016llx released %llu total %llu/%llu\n",
        pCharge->gpuId, (NvU64)pCharge->cligrp, pCharge->size,
        after, pLimits->HardLimit);

    if ((before > pLimits->SoftLimit) && (after <= pLimits->SoftLimit))
        notify = NV_TRUE;

done:
    portSyncMutexRelease(g_memacct.mutex);
    return notify;
}

void memacctReleaseCharge(MemoryCharge *pCharge)
{
    NvBool notify;

    if (g_memacct.impl == CGROUP_IMPL_NONE)
        return;

    if (pCharge == NULL)
        return;

    if (--pCharge->refCount)
        return;

    notify = NV_FALSE;
    if (g_memacct.impl == CGROUP_IMPL_OS)
    {
        NvBool overSoftLimitBefore;
        NvBool overSoftLimitAfter;

        NV_PRINTF(LEVEL_INFO, "release %llu for pool 0x%016llx\n", pCharge->size,
            (NvU64)pCharge->osPool);

        overSoftLimitBefore = osCgroupCanEvict(NULL, pCharge->osPool);
        osMemacctReleaseCharge(pCharge->osPool, pCharge->size);
        overSoftLimitAfter = osCgroupCanEvict(NULL, pCharge->osPool);

        if (overSoftLimitBefore && !overSoftLimitAfter)
            notify = NV_TRUE;
    }
    else
    {
        notify = memacctReleaseChargeInternal(pCharge);
    }

    if (notify)
    {
        gpuNotifySubDeviceEvent(gpumgrGetGpuFromId(pCharge->gpuId), NV2080_NOTIFIERS_MEMACCT_RETURNED_BELOW_SOFT_LIMIT,
            NULL, 0, pCharge->pid, 0);
    }

    portMemFree(pCharge);
}

NV_STATUS memacctGetLimits(ClientGroupID cligrp, NvU32 gpuId, NvLength *pSoftLimit, NvLength *pHardLimit, NvLength *pCurrent)
{
    GpuRegion *pRegion;
    ClientGroupLimits *pLimits;
    NV_STATUS status = NV_OK;

    if (g_memacct.impl != CGROUP_IMPL_FALLBACK)
        return NV_ERR_NOT_SUPPORTED;

    portSyncMutexAcquire(g_memacct.mutex);
    pRegion = mapFind(&g_memacct.GpuRegionMap, gpuId);
    if (pRegion == NULL)
    {
        // device doesn't exist - might be a bad argument or maybe was skipped as zerofb, for example
        status = NV_ERR_INVALID_LIMIT;
        goto cleanup;
    }

    pLimits = memacctLimitsForGroupLocked(pRegion, &cligrp);
    if (pLimits == NULL)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup;
    }

    *pSoftLimit = pLimits->SoftLimit;
    *pHardLimit = pLimits->HardLimit;
    *pCurrent = pLimits->HardLimit - pLimits->Available;

cleanup:
    portSyncMutexRelease(g_memacct.mutex);
    return status;
}

NV_STATUS memacctSetLimits(ClientGroupID cligrp, NvU32 gpuId, NvLength softlimit, NvLength hardlimit)
{
    NV_STATUS status;
    GpuRegion *pRegion;
    ClientGroupLimits *pLimits;

    if (g_memacct.impl != CGROUP_IMPL_FALLBACK)
        return NV_ERR_NOT_SUPPORTED;

    portSyncMutexAcquire(g_memacct.mutex);
    pRegion = mapFind(&g_memacct.GpuRegionMap, gpuId);
    if (pRegion == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto cleanup;
    }

    // no setting root group limits (to match dmem)
    if (osCgroupParent(cligrp) == NULL)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup;
    }

    pLimits = mapFind(&pRegion->clientGroupMap, (NvU64)cligrp);
    if (pLimits != NULL)
    {
        NvLength used = pLimits->HardLimit - pLimits->Available;
        if (used > hardlimit)
        {
            NV_PRINTF(LEVEL_ERROR, "region %x group %016llx set limit error - requested limit %llu is lower than current allocation %llu\n",
                gpuId, (NvU64)cligrp, hardlimit, used);

            status = NV_ERR_INVALID_LIMIT;
            goto cleanup;
        }
        pLimits->Available = hardlimit - used;
    }
    else
    {
        pLimits = mapInsertNew(&pRegion->clientGroupMap, (NvU64)cligrp);
        if (pLimits == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }
        pLimits->Available = hardlimit;
    }

    pLimits->SoftLimit = softlimit;
    pLimits->HardLimit = hardlimit;
    status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "region %x group %016llx setting limits %llu %llu\n", gpuId,
        (NvU64)cligrp, softlimit, hardlimit);

cleanup:
    portSyncMutexRelease(g_memacct.mutex);
    return status;
}

static void memacctOneTimeSetup(void)
{
    g_memacct.impl = osCgroupImplementation();
    if ((g_memacct.mutex == NULL) && (g_memacct.impl != CGROUP_IMPL_NONE))
    {
        PORT_MEM_ALLOCATOR *allocator = portMemAllocatorGetGlobalNonPaged();
        mapInit(&g_memacct.GpuRegionMap, allocator);
        g_memacct.mutex = portSyncMutexCreate(allocator);
    }
}

NvCgroupImpl memacctActiveImplementation(void)
{
    return g_memacct.impl;
}

NV_STATUS memacctInitGpuInfo(OBJGPU *pGpu)
{
    NvU64 id;
    MemoryManager *pMemoryManager;
    KernelMemorySystem *pKernelMemorySystem;
    KernelMIGManager *pKernelMIGManager;
    NvBool bIsPmaEnabled;
    Heap *pHeap;
    NvU64 size;
    NvU64 freeSize;
    NvU64 precharge;
    void *prechargePool;
    void *osRegion;
    NV_STATUS status;
    GpuRegion *pRegion;

    memacctOneTimeSetup();

    if (g_memacct.impl == CGROUP_IMPL_NONE)
        return NV_OK;

    NV_ASSERT_OR_RETURN(gpumgrIsSafeToReadGpuInfo(), NV_ERR_INVALID_LOCK_STATE);

    id = pGpu->gpuId;
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    bIsPmaEnabled = memmgrIsPmaInitialized(pMemoryManager);
    pHeap = GPU_GET_HEAP(pGpu);

    size = memmgrGetTotalRamSizeBytes(pGpu, pMemoryManager, pKernelMemorySystem, pHeap, NULL,
        bIsPmaEnabled);
    if (size == 0)
    {
        NV_PRINTF(LEVEL_INFO, "region %llx zero size, ignoring\n", id);
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "creating new memacct region %llx size %llu\n", id, size);
    freeSize = memmgrGetHeapFreeBytes(pGpu, pMemoryManager, pKernelMIGManager, pHeap,
        bIsPmaEnabled, NV_FALSE);
    precharge = size - freeSize;
    prechargePool = NULL;

    osRegion = NULL;
    if (g_memacct.impl == CGROUP_IMPL_OS)
    {
        osRegion = osCgroupRegisterRegion(pGpu, size, precharge, &prechargePool);
        if (!osRegion)
            return NV_ERR_OPERATING_SYSTEM;
    }

    portSyncMutexAcquire(g_memacct.mutex);

    status = NV_OK;
    pRegion = mapInsertNew(&g_memacct.GpuRegionMap, id);
    if (pRegion == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        if (osRegion)
            osCgroupUnregisterRegion(osRegion, prechargePool, precharge);
        goto cleanup;
    }

    if (g_memacct.impl == CGROUP_IMPL_OS)
    {
        pRegion->os.region = osRegion;
        pRegion->os.precharge = precharge;
        pRegion->os.prechargePool = prechargePool;
    }
    else
    {
        mapInit(&pRegion->clientGroupMap, portMemAllocatorGetGlobalNonPaged());
    }
cleanup:
    portSyncMutexRelease(g_memacct.mutex);
    return status;
}

void memacctRemoveGpu(OBJGPU *pGpu)
{
    void *osRegion = NULL;
    void *prechargePool = NULL;
    NvU64 precharge = 0;
    GpuRegion *pRegion;
    
    if (g_memacct.impl == CGROUP_IMPL_NONE)
        return;

    if (g_memacct.mutex == NULL)
        return;

    if (pGpu == NULL)
        return;

    NV_ASSERT_OR_RETURN_VOID(gpumgrIsSafeToReadGpuInfo());

    portSyncMutexAcquire(g_memacct.mutex);
    pRegion = mapFind(&g_memacct.GpuRegionMap, pGpu->gpuId);
    if (pRegion == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "region %x not found, ignoring\n", pGpu->gpuId);
        goto cleanup;
    }

    NV_PRINTF(LEVEL_INFO, "removing memacct region %x\n", pGpu->gpuId);

    if (g_memacct.impl == CGROUP_IMPL_OS)
    {
        osRegion = pRegion->os.region;
        precharge = pRegion->os.precharge;
        prechargePool = pRegion->os.prechargePool;
    }
    else
    {
        mapClear(&pRegion->clientGroupMap);
    }
    mapRemove(&g_memacct.GpuRegionMap, pRegion);

cleanup:
    if (mapCount(&g_memacct.GpuRegionMap) == 0)
    {
        mapClear(&g_memacct.GpuRegionMap);
        portSyncMutexRelease(g_memacct.mutex);
        portSyncMutexDestroy(g_memacct.mutex);
        g_memacct.mutex = NULL;
    }
    else
    {
        portSyncMutexRelease(g_memacct.mutex);
    }

    if (osRegion)
    {
        osCgroupUnregisterRegion(osRegion, prechargePool, precharge);
    }

    return;
}
