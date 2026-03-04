/*
 * SPDX-FileCopyrightText: Copyright (c) 20202025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/sysmem_scrub.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "core/locks.h"
#include "os/os.h"
#include "gpu/mem_mgr/ce_utils.h"
#include "nvrm_registry.h"

void
_sysmemscrubFreeWorkerParams
(
    SysmemScrubberWorkerParams *pWorkerParams
)
{
    if (pWorkerParams->pSpinlock != NULL)
        portSyncSpinlockDestroy(pWorkerParams->pSpinlock);
    portMemFree(pWorkerParams);
}

NV_STATUS
sysmemscrubConstruct_IMPL
(
    SysmemScrubber *pSysmemScrubber,
    OBJGPU *pGpu
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV0050_ALLOCATION_PARAMETERS ceUtilsAllocParams = {0};
    SysmemScrubberWorkerParams *pWorkerParams;
    NV_STATUS status;
    NvU32 data32;

    NV_ASSERT_OR_RETURN(pMemoryManager->bFastScrubberSupportsSysmem, NV_ERR_NOT_SUPPORTED);

    pSysmemScrubber->pGpu = pGpu;

    // Disable by default until locking issues are addressed
    pSysmemScrubber->bAsync = NV_TRUE;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_ASYNC_SYSMEM_SCRUB, &data32) == NV_OK)
    {
        pSysmemScrubber->bAsync = !data32;
    }

    pWorkerParams = portMemAllocNonPaged(sizeof (*pWorkerParams));
    NV_ASSERT_OR_RETURN(pWorkerParams != NULL, NV_ERR_NO_MEMORY);
    pWorkerParams->pSpinlock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    NV_ASSERT_TRUE_OR_GOTO(status, pWorkerParams->pSpinlock != NULL, NV_ERR_NO_MEMORY, failed);
    pWorkerParams->pSysmemScrubber = pSysmemScrubber;
    pWorkerParams->refCount = 1;
    pSysmemScrubber->pWorkerParams = pWorkerParams;

    listInitIntrusive(&pSysmemScrubber->asyncScrubList);

    ceUtilsAllocParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _ENABLE_COMPLETION_CB, _TRUE);
    NV_ASSERT_OK_OR_GOTO(status,
        objCreate(&pSysmemScrubber->pCeUtils, pSysmemScrubber, CeUtils, pGpu, NULL, &ceUtilsAllocParams),
        failed);

failed:
    if (status != NV_OK)
    {
        _sysmemscrubFreeWorkerParams(pWorkerParams);
    }

    return status;
}

static void
_sysmemscrubProcessCompletedEntries
(
    SysmemScrubber *pSysmemScrubber,
    SysmemScrubberWorkerParams *pWorkerParams
)
{
    SysScrubEntry *pEntry;
    SysScrubList freeList;

    //
    // Destructor sets pWorkerParams->pSysmemScrubber to NULL
    // After that the workers need to return early
    // Destructor is responsible for draining all the work iself
    // This is done as destructor can't flush all the pending workers
    //

    listInitIntrusive(&freeList);

    portSyncSpinlockAcquire(pWorkerParams->pSpinlock);

    if (pSysmemScrubber == NULL)
    {
        // Destructor passes pSysmemScrubber directly, as pWorkerParams->pSysmemScrubber is NULL by then (see below)
        pSysmemScrubber = pWorkerParams->pSysmemScrubber;
    }

    if (pSysmemScrubber != NULL)
    {
        // ceutilsDestruct() ensures that the work is completed
        NvU64 lastCompleted = (pSysmemScrubber->pCeUtils == NULL) ?
            NV_U64_MAX : ceutilsUpdateProgress(pSysmemScrubber->pCeUtils);

        while ((pEntry = listHead(&pSysmemScrubber->asyncScrubList)) != NULL)
        {
            if (pEntry->semaphoreValue > lastCompleted)
                break;

            listRemove(&pSysmemScrubber->asyncScrubList, pEntry);
            listAppendExisting(&freeList, pEntry);
        }
    }

    portSyncSpinlockRelease(pWorkerParams->pSpinlock);

    while ((pEntry = listHead(&freeList)) != NULL)
    {
        NV_PRINTF(LEVEL_INFO, "freeing scrubbed pMemDesc=%p RefCount=%u DupCount=%u\n",
            pEntry->pMemDesc, pEntry->pMemDesc->RefCount, pEntry->pMemDesc->DupCount);

        memdescFree(pEntry->pMemDesc);
        memdescDestroy(pEntry->pMemDesc);

        listRemove(&freeList, pEntry);
        portMemFree(pEntry);
    }

    listDestroy(&freeList);
}

static void
_sysmemscrubProcessCompletedEntriesCb
(
    NvU32 gpuInstance,
    void *pArg
)
{
    SysmemScrubberWorkerParams *pWorkerParams = pArg;

    NV_PRINTF(LEVEL_SILENT, "processing completed scrub work in deferred work item\n");

    portAtomicSetU32(&pWorkerParams->bWorkerQueued, NV_FALSE);

    _sysmemscrubProcessCompletedEntries(NULL, pWorkerParams);

    if (portAtomicDecrementU32(&pWorkerParams->refCount) == 0)
    {
        _sysmemscrubFreeWorkerParams(pWorkerParams);
    }
}

static NvBool
_sysmemscrubIsWorkPending
(
    SysmemScrubberWorkerParams *pWorkerParams
)
{
    // TODO: remove this function when CeUtils migrates to SemaphoreSurface
    SysmemScrubber *pSysmemScrubber;
    SysScrubEntry *pEntry;
    NvBool bWorkPending = NV_FALSE;

    portSyncSpinlockAcquire(pWorkerParams->pSpinlock);
    pSysmemScrubber = pWorkerParams->pSysmemScrubber;
    if (pSysmemScrubber != NULL)
    {
        pEntry = listHead(&pSysmemScrubber->asyncScrubList);
        bWorkPending = pEntry != NULL && pEntry->semaphoreValue <= ceutilsUpdateProgress(pSysmemScrubber->pCeUtils);
    }
    portSyncSpinlockRelease(pWorkerParams->pSpinlock);

    return bWorkPending;
}


static void
_sysmemscrubQueueProcessCompletedEntries(void *pArg)
{
    // The event handler can't get called after destructor, as the event gets deregistered
    SysmemScrubber *pSysmemScrubber = pArg;
    SysmemScrubberWorkerParams *pWorkerParams = pSysmemScrubber->pWorkerParams;

    NV_PRINTF(LEVEL_SILENT, "scrub completed callback\n");

    if (portAtomicAddU32(&pWorkerParams->bWorkerQueued, 0) ||
        !_sysmemscrubIsWorkPending(pWorkerParams))
    {
        return;
    }

    // queue work to run it outside interrupt context
    NV_ASSERT_OR_RETURN_VOID(
        osQueueWorkItem(pSysmemScrubber->pGpu,
                        _sysmemscrubProcessCompletedEntriesCb,
                        pWorkerParams,
                        (OsQueueWorkItemFlags){
                            .bDontFreeParams = NV_TRUE,
                            .bFallbackToDpc = NV_TRUE,
                            .bLockGpuGroupDevice = NV_TRUE,
                            .bFullGpuSanity = NV_TRUE}) == NV_OK);

    portAtomicSetU32(&pWorkerParams->bWorkerQueued, NV_TRUE);
    portAtomicIncrementU32(&pWorkerParams->refCount);
}

static NV_STATUS
_sysmemscrubScrubAndFreeAsync
(
    SysmemScrubber *pSysmemScrubber,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    CEUTILS_MEMSET_PARAMS params =
    {
        .pMemDesc = pMemDesc,
        .length = pMemDesc->Size,
        .flags = NV0050_CTRL_MEMSET_FLAGS_PIPELINED | NV0050_CTRL_MEMSET_FLAGS_ASYNC,
        .pCompletionCallback = _sysmemscrubQueueProcessCompletedEntries,
        .pCompletionCallbackArg = pSysmemScrubber
    };
    SysmemScrubberWorkerParams *pWorkerParams = pSysmemScrubber->pWorkerParams;
    SysScrubEntry *pEntry = portMemAllocNonPaged(sizeof (*pEntry));
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_NO_MEMORY);

    portSyncSpinlockAcquire(pWorkerParams->pSpinlock);
    listAppendExisting(&pSysmemScrubber->asyncScrubList, pEntry);

    //
    // RM might be holding memory references despite memory is freed by the user
    // This should not happen for compressed allocations, so don't handle it and clear memdesc anyway
    // Such references should be replaced by refAddDependant()
    //
    NV_ASSERT(pMemDesc->RefCount == 1);
    NV_ASSERT(!memdescIsSubMemoryMemDesc(pMemDesc));

    memdescAddRef(pMemDesc);
    pMemDesc->Allocated++;

    status = ceutilsMemset(pSysmemScrubber->pCeUtils, &params);
    NV_ASSERT_OK(status);

    if (status == NV_OK)
    {
        pEntry->pMemDesc = pMemDesc;
        pEntry->semaphoreValue = params.submittedWorkId;
    }
    else
    {
        listRemove(&pSysmemScrubber->asyncScrubList, pEntry);
        portMemFree(pEntry);
    }
    portSyncSpinlockRelease(pWorkerParams->pSpinlock);

    return status;
}

static NV_STATUS
_sysmemscrubScrubAndFreeSync
(
    SysmemScrubber *pSysmemScrubber,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    CEUTILS_MEMSET_PARAMS params =
    {
        .pMemDesc = pMemDesc,
        .length = pMemDesc->Size,
        .flags = NV0050_CTRL_MEMSET_FLAGS_PIPELINED
    };

    return ceutilsMemset(pSysmemScrubber->pCeUtils, &params);
}

NV_STATUS
sysmemscrubScrubAndFree_IMPL
(
    SysmemScrubber *pSysmemScrubber,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pSysmemScrubber->pGpu->gpuInstance) || rmGpuLockIsOwner(), NV_ERR_INVALID_STATE);

    NV_ASSERT(pMemDesc->Size == pMemDesc->ActualSize);

    // WAR: currently queuing work out of ISR can fail, clean it up here
    _sysmemscrubProcessCompletedEntries(NULL, pSysmemScrubber->pWorkerParams);

    if (pSysmemScrubber->bAsync &&
        _sysmemscrubScrubAndFreeAsync(pSysmemScrubber, pMemDesc) == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "pMemDesc=%p RefCount=%u DupCount=%u\n",
            pMemDesc, pMemDesc->RefCount, pMemDesc->DupCount);
        return NV_OK;
    }

    return _sysmemscrubScrubAndFreeSync(pSysmemScrubber, pMemDesc);
}

void
sysmemscrubDestruct_IMPL
(
    SysmemScrubber *pSysmemScrubber
)
{
    SysmemScrubberWorkerParams *pWorkerParams = pSysmemScrubber->pWorkerParams;

    portSyncSpinlockAcquire(pWorkerParams->pSpinlock);
    pWorkerParams->pSysmemScrubber = NULL;
    portSyncSpinlockRelease(pWorkerParams->pSpinlock);

    objDelete(pSysmemScrubber->pCeUtils);
    pSysmemScrubber->pCeUtils = NULL;

    // pWorkerParams->pSysmemScrubber is NULL, so wokers won't run at this point
    _sysmemscrubProcessCompletedEntries(pSysmemScrubber, pWorkerParams);

    if (portAtomicDecrementU32(&pWorkerParams->refCount) == 0)
        _sysmemscrubFreeWorkerParams(pWorkerParams);

    NV_ASSERT(listCount(&pSysmemScrubber->asyncScrubList) == 0);
    listDestroy(&pSysmemScrubber->asyncScrubList);
}
