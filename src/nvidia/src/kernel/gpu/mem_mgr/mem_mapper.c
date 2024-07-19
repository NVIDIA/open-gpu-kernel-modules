/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/mem_mapper.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/sem_surf.h"
#include "mem_mgr/vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/virtual_mem.h"
#include "mem_mgr/mem.h"
#include "rmapi/rs_utils.h"
#include "rmapi/mapping_list.h"
#include "containers/btree.h"

#include "class/cl0080.h"
#include "class/cl2080.h"

static NV_STATUS
memmapperExecuteMap
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_OPERATION_MAP            *pMap,
    RM_API                               *pRmApi
)
{
    RsClient *pClient = RES_GET_CLIENT(pMemoryMapper);

    RsResourceRef *pVirtualResourceRef;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, clientGetResourceRef(pClient, pMap->hVirtualMemory, &pVirtualResourceRef));
    VirtualMemory *pVirtualMemory = dynamicCast(pVirtualResourceRef->pResource, VirtualMemory);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pVirtualMemory != NULL, NV_ERR_INVALID_ARGUMENT);
    NvU64 baseVirtAddr = memdescGetPhysAddr(staticCast(pVirtualMemory, Memory)->pMemDesc, AT_GPU_VA, 0);

    NV_PRINTF(LEVEL_INFO, "map virt:(0x%x:0x%llx)  phys:(0x%x:0x%llx) size:0x%llx dmaFlags:0x%x\n",
        pMap->hVirtualMemory, pMap->virtualOffset, pMap->hPhysicalMemory, pMap->physicalOffset,
        pMap->size, pMap->dmaFlags);

    NvU64 dmaOffset = baseVirtAddr + pMap->virtualOffset;
    NvU64 newDmaOffset = dmaOffset;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->MapWithSecInfo(pRmApi,
                               RES_GET_CLIENT_HANDLE(pMemoryMapper),
                               RES_GET_PARENT_HANDLE(pMemoryMapper->pSubdevice),
                               pMap->hVirtualMemory,
                               pMap->hPhysicalMemory,
                               pMap->physicalOffset,
                               pMap->size,
                               pMap->dmaFlags | DRF_DEF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE),
                               &newDmaOffset,
                               &pMemoryMapper->secInfo));
    NV_ASSERT_OR_RETURN(newDmaOffset == dmaOffset, NV_ERR_INVALID_STATE);

    return NV_OK;
}

static NV_STATUS
memmapperExecuteUnmap
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_OPERATION_UNMAP          *pUnmap,
    RM_API                               *pRmApi
)
{
    RsClient *pClient      = RES_GET_CLIENT(pMemoryMapper);

    RsResourceRef *pVirtualResourceRef;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, clientGetResourceRef(pClient, pUnmap->hVirtualMemory, &pVirtualResourceRef));
    VirtualMemory *pVirtualMemory = dynamicCast(pVirtualResourceRef->pResource, VirtualMemory);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pVirtualMemory != NULL, NV_ERR_INVALID_ARGUMENT);
    NvU64 baseVirtAddr = memdescGetPhysAddr(staticCast(pVirtualMemory, Memory)->pMemDesc, AT_GPU_VA, 0);

    NV_PRINTF(LEVEL_INFO, "unmap virt:(0x%x:0x%llx) size:0x%llx dmaFlags:0x%x\n",
        pUnmap->hVirtualMemory, pUnmap->virtualOffset, pUnmap->size, pUnmap->dmaFlags);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->UnmapWithSecInfo(pRmApi,
                                 RES_GET_CLIENT_HANDLE(pMemoryMapper),
                                 RES_GET_PARENT_HANDLE(pMemoryMapper->pSubdevice),
                                 pUnmap->hVirtualMemory,
                                 pUnmap->dmaFlags,
                                 baseVirtAddr + pUnmap->virtualOffset,
                                 pUnmap->size,
                                 &pMemoryMapper->secInfo));

    return NV_OK;
}

static NV_STATUS
memmapperExecuteSemaphoreWait
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_OPERATION_SEMAPHORE      *pWait
)
{
    NvU64 semVal = semsurfGetValue(pMemoryMapper->pSemSurf, pWait->index);

    NV_PRINTF(LEVEL_INFO, "wait index:0x%x val:0x%llx cur_var:0x%llx\n",
              pWait->index, pWait->value, semVal);

    return (semVal >= pWait->value) ? NV_OK : NV_ERR_BUSY_RETRY;
}

static NV_STATUS
memmapperExecuteSemaphoreSignal
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_OPERATION_SEMAPHORE      *pSignal,
    RM_API                               *pRmApi
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(GPU_RES_GET_GPU(pMemoryMapper));
    NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS params = {0};

    params.index    = pSignal->index;
    params.newValue = pSignal->value;

    NV_PRINTF(LEVEL_INFO, "signal index:0x%x val:0x%llx\n",
              pSignal->index, pSignal->value);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pMemoryManager->hClient,
                        pMemoryMapper->hInternalSemaphoreSurface,
                        NV_SEMAPHORE_SURFACE_CTRL_CMD_SET_VALUE,
                        &params,
                        sizeof(params)));
    return NV_OK;
}

static void
memmapperSetError
(
    MemoryMapper                         *pMemoryMapper,
    NV_STATUS                             errorStatus
)
{
    NV_ASSERT_OR_RETURN_VOID(errorStatus != NV_OK);

    if (!pMemoryMapper->bError)
    {
        pMemoryMapper->pNotification->status = errorStatus;
        pMemoryMapper->bError = NV_TRUE;
    }
}

static void
memmapperProcessWork
(
    MemoryMapper *pMemoryMapper
)
{
    NV_PRINTF(LEVEL_INFO, "processing MemoryMapper work\n");

    if (pMemoryMapper->bError)
    {
        NV_PRINTF(LEVEL_INFO, "return from MemoryMapper worker (error)\n");
        return;
    }

    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    while (pMemoryMapper->operationQueueGet != pMemoryMapper->operationQueuePut)
    {
        NV00FE_CTRL_OPERATION *pOperation = &pMemoryMapper->pOperationQueue[pMemoryMapper->operationQueueGet];
        NV_STATUS status = NV_OK;

        switch (pOperation->type)
        {
            case NV00FE_CTRL_OPERATION_TYPE_NOP:
                break;
            case NV00FE_CTRL_OPERATION_TYPE_MAP:
                NV_CHECK_OK(status, LEVEL_ERROR, memmapperExecuteMap(pMemoryMapper, &pOperation->data.map, pRmApi));
                break;
            case NV00FE_CTRL_OPERATION_TYPE_UNMAP:
                NV_CHECK_OK(status, LEVEL_ERROR, memmapperExecuteUnmap(pMemoryMapper, &pOperation->data.unmap, pRmApi));
                break;
            case NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_WAIT:
                status = memmapperExecuteSemaphoreWait(pMemoryMapper, &pOperation->data.semaphore);
                NV_CHECK(LEVEL_ERROR, (status == NV_OK) || (status == NV_ERR_BUSY_RETRY));
                break;
            case NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_SIGNAL:
                status = memmapperExecuteSemaphoreSignal(pMemoryMapper, &pOperation->data.semaphore, pRmApi);
                break;
            default:
                NV_CHECK_OK(status, LEVEL_ERROR, NV_ERR_INVALID_ARGUMENT);
                break;
        }

        if (status != NV_OK)
        {
            if (status != NV_ERR_BUSY_RETRY)
            {
                memmapperSetError(pMemoryMapper, status);
            }
            break;
        }

        pMemoryMapper->operationQueueGet = (pMemoryMapper->operationQueueGet + 1) % pMemoryMapper->operationQueueLen;
    }

    NV_PRINTF(LEVEL_INFO, "return from MemoryMapper worker\n");
}

static void
memoryMapperWorker
(
    NvU32 gpuInstance,
    void *pArg
)
{
    MemoryMapperWorkerParams *pWorkerParams = pArg;
    MemoryMapper *pMemoryMapper = pWorkerParams->pMemoryMapper;

    if (pMemoryMapper != NULL)
    {
        memmapperProcessWork(pMemoryMapper);
    }

    if (--pWorkerParams->numRefs == 0)
    {
        portMemFree(pWorkerParams);
    }
}

static void
memoryMapperQueueWork
(
    MemoryMapper *pMemoryMapper
)
{
    pMemoryMapper->pWorkerParams->numRefs++;

    // Queue work at passive IRQL
    NV_ASSERT_OK(osQueueWorkItemWithFlags(GPU_RES_GET_GPU(pMemoryMapper),
                                          memoryMapperWorker,
                                          pMemoryMapper->pWorkerParams,
                                          OS_QUEUE_WORKITEM_FLAGS_DONT_FREE_PARAMS
                                          | OS_QUEUE_WORKITEM_FLAGS_FALLBACK_TO_DPC
                                          | OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW
                                          | OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS));
}

static void
memmapperSemaphoreEventCallback
(
    void        *pArg,
    void        *pData,
    NvHandle     hEvent,
    NvU32        data,
    NvU32        status
)
{
    memoryMapperQueueWork(pArg);
}

NV_STATUS
memmapperConstruct_IMPL
(
    MemoryMapper *pMemoryMapper,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU                             *pGpu           = GPU_RES_GET_GPU(pMemoryMapper);
    MemoryManager                      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    RM_API                             *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsResourceRef                      *pResourceRef   = pCallContext->pResourceRef;
    RsResourceRef                      *pParentRef     = pResourceRef->pParentRef;
    NV_MEMORY_MAPPER_ALLOCATION_PARAMS *pAllocParams   = pParams->pAllocParams;
    NV_STATUS                           status;

    // Store secInfo used at MemoryMapper creation for RMAPI calls on behalf of the client
    pMemoryMapper->secInfo = pCallContext->secInfo;
    pMemoryMapper->secInfo.paramLocation = PARAM_LOCATION_KERNEL;

    pMemoryMapper->pSubdevice = dynamicCast(pParentRef->pResource, Subdevice);
    NV_ASSERT_OR_RETURN(pMemoryMapper->pSubdevice != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(pAllocParams->maxQueueSize != 0, NV_ERR_INVALID_ARGUMENT);
    pMemoryMapper->operationQueueLen = pAllocParams->maxQueueSize;
    pMemoryMapper->pOperationQueue =
        portMemAllocNonPaged(pMemoryMapper->operationQueueLen * sizeof(*pMemoryMapper->pOperationQueue));
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pMemoryMapper->pOperationQueue != NULL, NV_ERR_NO_MEMORY, failed);

    RsResourceRef *pNotificationMemoryRef;
    NV_ASSERT_OK_OR_GOTO(status,
        clientGetResourceRef(pCallContext->pClient, pAllocParams->hNotificationMemory, &pNotificationMemoryRef),
        failed);
    pMemoryMapper->pNotificationMemory = dynamicCast(pNotificationMemoryRef->pResource, Memory);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR,
        pMemoryMapper->pNotificationMemory != NULL, NV_ERR_INVALID_ARGUMENT, failed);
    pMemoryMapper->notificationSurface.pMemDesc = pMemoryMapper->pNotificationMemory->pMemDesc;
    pMemoryMapper->notificationSurface.offset = pAllocParams->notificationOffset;
    pMemoryMapper->pNotification = (NV_MEMORY_MAPPER_NOTIFICATION *)memmgrMemBeginTransfer(
        pMemoryManager, &pMemoryMapper->notificationSurface,
        sizeof (NV_MEMORY_MAPPER_NOTIFICATION), TRANSFER_FLAGS_USE_BAR1);
    NV_ASSERT_TRUE_OR_GOTO(status, pMemoryMapper->pNotification != NULL, NV_ERR_NO_MEMORY, failed);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->DupObject(pRmApi,
                          pMemoryManager->hClient,
                          pMemoryManager->hSubdevice,
                          &pMemoryMapper->hInternalSemaphoreSurface,
                          RES_GET_CLIENT_HANDLE(pMemoryMapper),
                          pAllocParams->hSemaphoreSurface,
                          NV04_DUP_HANDLE_FLAGS_NONE),
        failed);

    RmClient *pInternalClient = serverutilGetClientUnderLock(pMemoryManager->hClient);
    NV_ASSERT_TRUE_OR_GOTO(status, pInternalClient != NULL, NV_ERR_INVALID_STATE, failed);
    RsClient *pInternalRsClient = staticCast(pInternalClient, RsClient);
    RsResourceRef  *pSemSurfRef;
    NV_ASSERT_OK_OR_GOTO(status,
        clientGetResourceRef(pInternalRsClient, pMemoryMapper->hInternalSemaphoreSurface, &pSemSurfRef),
        failed);
    pMemoryMapper->pSemSurf = dynamicCast(pSemSurfRef->pResource, SemaphoreSurface);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pMemoryMapper->pSemSurf != NULL, NV_ERR_INVALID_ARGUMENT, failed);

    pMemoryMapper->pWorkerParams = portMemAllocNonPaged(sizeof(*pMemoryMapper->pWorkerParams));
    pMemoryMapper->pWorkerParams->pMemoryMapper = pMemoryMapper;
    pMemoryMapper->pWorkerParams->numRefs = 1;
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pMemoryMapper->pWorkerParams != NULL, NV_ERR_NO_MEMORY, failed);

    pMemoryMapper->semaphoreCallback.func = memmapperSemaphoreEventCallback;
    pMemoryMapper->semaphoreCallback.arg  = pMemoryMapper;

    NV_ASSERT_OK_OR_GOTO(status, refAddDependant(pNotificationMemoryRef, RES_GET_REF(pMemoryMapper)), failed);

failed:
    if (status != NV_OK)
    {
        if (pMemoryMapper->pNotification != NULL)
        {
            memmgrMemEndTransfer(pMemoryManager, &pMemoryMapper->notificationSurface,
                                 sizeof (NV_MEMORY_MAPPER_NOTIFICATION), TRANSFER_FLAGS_USE_BAR1);
        }

        if (pMemoryMapper->hInternalSemaphoreSurface != NV01_NULL_OBJECT)
        {
            pRmApi->Free(pRmApi, pMemoryManager->hClient, pMemoryMapper->hInternalSemaphoreSurface);
        }

        portMemFree(pMemoryMapper->pOperationQueue);
        portMemFree(pMemoryMapper->pWorkerParams);
    }

    return status;
}

void
memmapperDestruct_IMPL
(
    MemoryMapper *pMemoryMapper
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(GPU_RES_GET_GPU(pMemoryMapper));
    RM_API        *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    memmgrMemEndTransfer(pMemoryManager, &pMemoryMapper->notificationSurface,
                         sizeof (NV_MEMORY_MAPPER_NOTIFICATION), TRANSFER_FLAGS_USE_BAR1);

    refRemoveDependant(RES_GET_REF(pMemoryMapper->pNotificationMemory), RES_GET_REF(pMemoryMapper));
    pRmApi->Free(pRmApi, pMemoryManager->hClient, pMemoryMapper->hInternalSemaphoreSurface);
    portMemFree(pMemoryMapper->pOperationQueue);

    pMemoryMapper->pWorkerParams->pMemoryMapper = NULL;

    if (--pMemoryMapper->pWorkerParams->numRefs == 0)
    {
        portMemFree(pMemoryMapper->pWorkerParams);
    }
}

static NV_STATUS
memmapperSubmitSemaphoreWait
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_OPERATION_SEMAPHORE      *pSemaphoreWait
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(GPU_RES_GET_GPU(pMemoryMapper));
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS params = {0};
    params.index              = pSemaphoreWait->index;
    params.waitValue          = pSemaphoreWait->value;
    params.notificationHandle = (NvU64)&pMemoryMapper->semaphoreCallback;

    NV_STATUS status = pRmApi->Control(pRmApi,
                                       pMemoryManager->hClient,
                                       pMemoryMapper->hInternalSemaphoreSurface,
                                       NV_SEMAPHORE_SURFACE_CTRL_CMD_REGISTER_WAITER,
                                       &params,
                                      sizeof params);

    NV_CHECK(LEVEL_ERROR, status == NV_OK || status == NV_ERR_ALREADY_SIGNALLED);
    return status;
}

NV_STATUS
memmapperCtrlCmdSubmitOperations_IMPL
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams
)
{
    NV00FE_CTRL_OPERATION        *pOperataionsParams = pParams->pOperations;
    NV_STATUS                     status             = NV_OK;
    CALL_CONTEXT                 *pCallContext       = resservGetTlsCallContext();
    NvU32                         i;

    // Check that the client hasn't dropped privileges
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pCallContext->secInfo.privLevel == pMemoryMapper->secInfo.privLevel,
        NV_ERR_INVALID_STATE);

    // If the queue is not empty, worker from previous commands will do the job
    NvBool bQueueWorker = (pMemoryMapper->operationQueuePut == pMemoryMapper->operationQueueGet);

    for (i = 0; i < pParams->operationsCount; i++)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, !pMemoryMapper->bError, NV_ERR_INVALID_STATE);

        NvU32 nextOperationQueuePut = (pMemoryMapper->operationQueuePut + 1) % pMemoryMapper->operationQueueLen;

        if (nextOperationQueuePut == pMemoryMapper->operationQueueGet)
        {
            status = NV_ERR_NOT_READY;
            break;
        }

        NV00FE_CTRL_OPERATION *pOperation       = &pMemoryMapper->pOperationQueue[pMemoryMapper->operationQueuePut];
        NV00FE_CTRL_OPERATION *pOperationParams = &pOperataionsParams[i];

        *pOperation = *pOperationParams;

        switch (pOperationParams->type)
        {
            case NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_WAIT:
                NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR,
                    semsurfValidateIndex(pMemoryMapper->pSemSurf, pOperation->data.semaphore.index),
                    NV_ERR_INVALID_ARGUMENT, op_failed);
                status = memmapperSubmitSemaphoreWait(pMemoryMapper, &pOperation->data.semaphore);
                if (status == NV_ERR_ALREADY_SIGNALLED)
                {
                    status = NV_OK;
                    // semaphore is already signalled, so skip the operation completely
                    continue;
                }
                else if (status == NV_OK && pMemoryMapper->operationQueuePut == pMemoryMapper->operationQueueGet)
                {
                    // worker will be called by semaphore surface callback
                    bQueueWorker = NV_FALSE;
                }
                NV_CHECK_OR_GOTO(LEVEL_ERROR, status == NV_OK, op_failed);
                break;
            case NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_SIGNAL:
                NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR,
                    semsurfValidateIndex(pMemoryMapper->pSemSurf, pOperation->data.semaphore.index),
                    NV_ERR_INVALID_ARGUMENT, op_failed);
                break;
            case NV00FE_CTRL_OPERATION_TYPE_MAP:
            case NV00FE_CTRL_OPERATION_TYPE_UNMAP:
                break;
            case NV00FE_CTRL_OPERATION_TYPE_NOP:
            default:
                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, NV_ERR_INVALID_ARGUMENT, op_failed);
                break;
        }

op_failed:
        if (status != NV_OK)
        {
            memmapperSetError(pMemoryMapper, status);
            break;
        }

        pMemoryMapper->operationQueuePut = nextOperationQueuePut;
    }

    pParams->operationsProcessedCount = i;

    if (bQueueWorker && pMemoryMapper->operationQueuePut != pMemoryMapper->operationQueueGet)
    {
        // only queue worker when the queue is not empty
        memoryMapperQueueWork(pMemoryMapper);
    }

    return status;
}

NV_STATUS
memmapperCtrlCmdResizeQueue_IMPL
(
    MemoryMapper                         *pMemoryMapper,
    NV00FE_CTRL_RESIZE_QUEUE_PARAMS      *pParams
)
{
    if ((pMemoryMapper->operationQueuePut - pMemoryMapper->operationQueueGet + pMemoryMapper->operationQueueLen)
        % pMemoryMapper->operationQueueLen + 1 > pParams->maxQueueSize)
    {
        // All queued operations need to fit in the queue
        NV_CHECK_FAILED(LEVEL_ERROR, "Queue size too small");
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV00FE_CTRL_OPERATION *pNewOperationQueue =
        portMemAllocNonPaged(pParams->maxQueueSize * sizeof(*pMemoryMapper->pOperationQueue));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pNewOperationQueue != NULL, NV_ERR_NO_MEMORY);

    NvU32 newQueuePut = 0;
    while (pMemoryMapper->operationQueueGet != pMemoryMapper->operationQueuePut)
    {
        pNewOperationQueue[newQueuePut] = pMemoryMapper->pOperationQueue[pMemoryMapper->operationQueueGet];
        pMemoryMapper->operationQueueGet = (pMemoryMapper->operationQueueGet + 1) % pMemoryMapper->operationQueueLen;
        newQueuePut++;
        NV_ASSERT_OR_RETURN(newQueuePut < pParams->maxQueueSize, NV_ERR_INVALID_STATE);
    }

    portMemFree(pMemoryMapper->pOperationQueue);

    pMemoryMapper->pOperationQueue   = pNewOperationQueue;
    pMemoryMapper->operationQueueLen = pParams->maxQueueSize;
    pMemoryMapper->operationQueuePut = newQueuePut;
    pMemoryMapper->operationQueueGet = 0;

    return NV_OK;
}
