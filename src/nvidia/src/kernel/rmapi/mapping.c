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
#include "core/core.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/device/device.h"
#include "kernel/mem_mgr/virtual_mem.h"
#include "class/cl0000.h" // NV01_NULL_OBJECT

#include "rmapi/rs_utils.h"

#include "entry_points.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/mem_mgr.h"

static NvU64
_getMappingPageSize
(
    RsResourceRef *pMappableRef
)
{
    Memory *pMemory = dynamicCast(pMappableRef->pResource, Memory);
    if (pMemory != NULL)
    {
        return memdescGetPageSize(pMemory->pMemDesc, AT_GPU);
    }
    return RM_PAGE_SIZE;
}

NV_STATUS
serverInterMap_Prologue
(
    RsServer *pServer,
    RsResourceRef *pMapperRef,
    RsResourceRef *pMappableRef,
    RS_INTER_MAP_PARAMS *pParams,
    NvU32 *pReleaseFlags
)
{
    OBJGPU     *pGpu;
    Device     *pDevice;
    Subdevice  *pSubdevice;
    NV_STATUS   rmStatus = NV_OK;
    NvU64       offset = pParams->offset;
    NvU64       length = pParams->length;

    MEMORY_DESCRIPTOR *pSrcMemDesc = NULL;
    NvHandle    hBroadcastDevice;
    NvBool      bSubdeviceHandleProvided;

    CALL_CONTEXT  *pCallContext = resservGetTlsCallContext();
    RsResourceRef *pDeviceRef = pCallContext->pContextRef;
    RS_INTER_MAP_PRIVATE *pPrivate = pParams->pPrivate;

    NV_ASSERT_OR_RETURN(pPrivate != NULL, NV_ERR_INVALID_ARGUMENT);

    // Get pGpu, assuming user passed in either a device or subdevice handle.
    pDevice = dynamicCast(pDeviceRef->pResource, Device);
    if (pDevice == NULL)
    {
        pSubdevice = dynamicCast(pDeviceRef->pResource, Subdevice);
        if (pSubdevice == NULL)
            return NV_ERR_INVALID_OBJECT;

        pGpu = GPU_RES_GET_GPU(pSubdevice);
        pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

        hBroadcastDevice = RES_GET_HANDLE(pSubdevice->pDevice);
        bSubdeviceHandleProvided = NV_TRUE;
        pPrivate->gpuMask = NVBIT(gpuGetInstance(pGpu));
    }
    else
    {
        pGpu = GPU_RES_GET_GPU(pDevice);
        GPU_RES_SET_THREAD_BC_STATE(pDevice);

        hBroadcastDevice = pParams->hDevice;
        bSubdeviceHandleProvided = NV_FALSE;
        pPrivate->gpuMask = gpumgrGetGpuMask(pGpu);
    }

    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    // For non-memory/dma objects, below call simply returns
    if (memmgrIsPmaInitialized(pMemoryManager) &&
            memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        VirtualMemory *pVirtualMemory;

        pVirtualMemory = dynamicCast(pMapperRef->pResource, VirtualMemory);

        if (pVirtualMemory != NULL)
        {
            NvU64 pageSize = RM_PAGE_SIZE;

            if (pVirtualMemory->bOptimizePageTableMempoolUsage)
            {
                pageSize = _getMappingPageSize(pMappableRef);
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
               virtmemReserveMempool(pVirtualMemory, pGpu, pDevice,
                                     pParams->length, pageSize));
        }
    }

    rmStatus = serverResLock_Prologue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, pReleaseFlags);
    if (rmStatus != NV_OK)
        return rmStatus;

    pPrivate->pGpu = pGpu;

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_TRUE, NV_FALSE);

    // Use virtual GetMemInterMapParams to get information needed for mapping from pMappableRef->pResource
    RMRES_MEM_INTER_MAP_PARAMS memInterMapParams;
    portMemSet(&memInterMapParams, 0, sizeof(memInterMapParams));

    memInterMapParams.pGpu = pGpu;
    memInterMapParams.pMemoryRef = pMappableRef;
    memInterMapParams.bSubdeviceHandleProvided = bSubdeviceHandleProvided;

    rmStatus = rmresGetMemInterMapParams(dynamicCast(pMappableRef->pResource, RmResource), &memInterMapParams);
    if (rmStatus != NV_OK)
        return rmStatus;

    pSrcMemDesc = memInterMapParams.pSrcMemDesc;
    NV_ASSERT_OR_RETURN(pSrcMemDesc != NULL, NV_ERR_INVALID_OBJECT_HANDLE);

    pPrivate->pSrcGpu = memInterMapParams.pSrcGpu;
    pPrivate->hMemoryDevice = memInterMapParams.hMemoryDevice;
    pPrivate->bFlaMapping   = memInterMapParams.bFlaMapping;

    // Check length for overflow and against the physical memory size.
    if (((offset + length) < offset) ||
        ((offset + length) > pSrcMemDesc->Size))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Mapping offset 0x%llX or length 0x%llX out of bounds!\n",
                  offset, length);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_LIMIT;
    }

    if (memdescGetFlag(memdescGetMemDescFromGpu(pSrcMemDesc, pGpu), MEMDESC_FLAGS_DEVICE_READ_ONLY) &&
        !FLD_TEST_DRF(OS46, _FLAGS, _ACCESS, _READ_ONLY, pParams->flags))
    {
        NV_PRINTF(LEVEL_ERROR, "Attempting to map READ_ONLY surface as READ_WRITE / WRITE_ONLY!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pPrivate->hBroadcastDevice = hBroadcastDevice;
    pPrivate->pSrcMemDesc = pSrcMemDesc;
    pPrivate->bSubdeviceHandleProvided = bSubdeviceHandleProvided;

    return NV_OK;
}

void
serverInterMap_Epilogue
(
    RsServer *pServer,
    RS_INTER_MAP_PARAMS *pParams,
    NvU32 *pReleaseFlags
)
{
    serverResLock_Epilogue(pServer, LOCK_ACCESS_WRITE, pParams->pLockInfo, pReleaseFlags);
}

NV_STATUS
serverInterUnmap_Prologue
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    OBJGPU       *pGpu        = NULL;
    Device       *pDevice     = NULL;
    Subdevice    *pSubdevice  = NULL;

    CALL_CONTEXT  *pCallContext = resservGetTlsCallContext();
    RsResourceRef *pDeviceRef = pCallContext->pContextRef;

    RS_INTER_UNMAP_PRIVATE *pPrivate = pParams->pPrivate;

    // Alloc pPrivate if not set, Unmap does not require any input into Prologue
    if (pPrivate == NULL)
    {
        pPrivate = portMemAllocNonPaged(sizeof(*pPrivate));
        if (pPrivate == NULL)
            return NV_ERR_NO_MEMORY;

        portMemSet(pPrivate, 0, sizeof(*pPrivate));
        pParams->pPrivate = pPrivate;
        pPrivate->bAllocated = NV_TRUE;
    }

    // Set subdevice or device context.
    pDevice = dynamicCast(pDeviceRef->pResource, Device);
    if (pDevice == NULL)
    {
        pSubdevice = dynamicCast(pDeviceRef->pResource, Subdevice);
        if (pSubdevice == NULL)
            return NV_ERR_INVALID_OBJECT;

        pGpu = GPU_RES_GET_GPU(pSubdevice);
        pPrivate->bcState = gpumgrGetBcEnabledStatus(pGpu);
        GPU_RES_SET_THREAD_BC_STATE(pSubdevice);
        pPrivate->hBroadcastDevice = RES_GET_HANDLE(pSubdevice->pDevice);
        pPrivate->bSubdeviceHandleProvided = NV_TRUE;
        pPrivate->gpuMask = NVBIT(gpuGetInstance(pGpu));
    }
    else
    {
        pGpu = GPU_RES_GET_GPU(pDevice);
        pPrivate->bcState = gpumgrGetBcEnabledStatus(pGpu);
        GPU_RES_SET_THREAD_BC_STATE(pDevice);
        pPrivate->hBroadcastDevice = RES_GET_HANDLE(pDevice);
        pPrivate->bSubdeviceHandleProvided = NV_FALSE;
        pPrivate->gpuMask = gpumgrGetGpuMask(pGpu);
    }

    pPrivate->pGpu = pGpu;

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_FALSE, NV_FALSE);

    return NV_OK;
}

void
serverInterUnmap_Epilogue
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    RS_INTER_UNMAP_PRIVATE *pPrivate = pParams->pPrivate;
    OBJGPU *pGpu;

    if (pPrivate == NULL)
        return;

    pGpu = pPrivate->pGpu;

    if (pGpu != NULL)
    {
        gpumgrSetBcEnabledStatus(pGpu, pPrivate->bcState);
    }

    if (pPrivate->bAllocated)
    {
        portMemFree(pPrivate);
        pParams->pPrivate = NULL;
    }
}

static NV_STATUS
_rmapiRmUnmapMemoryDma
(
    NvHandle            hClient,
    NvHandle            hDevice,
    NvHandle            hMemCtx,
    NvU32               flags,
    NvU64               dmaOffset,
    NvU64               size,
    RS_LOCK_INFO       *pLockInfo,
    API_SECURITY_INFO  *pSecInfo
)
{
    RsClient           *pRsClient   = NULL;

    RS_INTER_UNMAP_PARAMS params;
    RS_INTER_UNMAP_PRIVATE private;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pRsClient));

    portMemSet(&params, 0, sizeof(params));
    params.hClient = hClient;
    params.hMapper = hMemCtx;
    params.hDevice = hDevice;
    params.flags = flags;
    params.dmaOffset = dmaOffset;
    params.pLockInfo = pLockInfo;
    params.pSecInfo = pSecInfo;

    params.size = size;

    portMemSet(&private, 0, sizeof(private));
    params.pPrivate = &private;

    return serverInterUnmap(&g_resServ, &params);
}

NV_STATUS
rmapiMap
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemCtx,
    NvHandle  hMemory,
    NvU64     offset,
    NvU64     length,
    NvU32     flags,
    NvU64    *pDmaOffset
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->MapWithSecInfo(pRmApi, hClient, hDevice, hMemCtx, hMemory, offset,
                                  length, flags, pDmaOffset, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiMapWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvU32              flags,
    NvU64             *pDmaOffset,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;
    RM_API_CONTEXT rmApiContext = {0};
    RS_INTER_MAP_PARAMS params;
    RS_INTER_MAP_PRIVATE private;
    RS_LOCK_INFO lockInfo;

    NV_PRINTF(LEVEL_INFO,
              "Nv04Map: client:0x%x device:0x%x context:0x%x memory:0x%x flags:0x%x\n",
              hClient, hDevice, hMemCtx, hMemory, flags);
    NV_PRINTF(LEVEL_INFO,
              "Nv04Map:  offset:0x%llx length:0x%llx dmaOffset:0x%08llx\n",
              offset, length, *pDmaOffset);

    NV_PRINTF(LEVEL_INFO, "MMU_PROFILER Nv04Map 0x%x\n", flags);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
        return status;

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, NV01_NULL_OBJECT, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return status;
    }

    lockInfo.flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK |
                      RM_LOCK_FLAGS_NO_GPUS_LOCK;

    //
    // In the RTD3 case, the API lock isn't taken since it can be initiated
    // from another thread that holds the API lock and because we now hold
    // the GPU lock.
    //
    if (rmapiInRtd3PmPath())
    {
        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
        lockInfo.state &= ~RM_LOCK_STATES_API_LOCK_ACQUIRED;
    }

    LOCK_METER_DATA(MAPMEM_DMA, flags, 0, 0);


    portMemSet(&params, 0, sizeof(params));
    params.hClient = hClient;
    params.hMapper = hMemCtx;
    params.hDevice = hDevice;
    params.hMappable = hMemory;
    params.offset = offset;
    params.length = length;
    params.flags = flags;
    params.dmaOffset = *pDmaOffset;
    params.pLockInfo = &lockInfo;
    params.pSecInfo = pSecInfo;

    portMemSet(&private, 0, sizeof(private));
    params.pPrivate = &private;

    // map DMA memory
    status = serverInterMap(&g_resServ, &params);

    *pDmaOffset = params.dmaOffset;

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv04Map: map complete\n");
        NV_PRINTF(LEVEL_INFO, "Nv04Map:  dmaOffset: 0x%08llx\n", *pDmaOffset);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Nv04Map: map failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
    }

    return status;
}

NV_STATUS
rmapiMapWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvU32              flags,
    NvU64             *pDmaOffset,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiMapWithSecInfo(pRmApi, hClient, hDevice, hMemCtx, hMemory, offset,
                                 length, flags, pDmaOffset, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
rmapiUnmap
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemCtx,
    NvU32     flags,
    NvU64     dmaOffset,
    NvU64     size
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->UnmapWithSecInfo(pRmApi, hClient, hDevice, hMemCtx,
                                    flags, dmaOffset, size, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiUnmapWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvU32              flags,
    NvU64              dmaOffset,
    NvU64              size,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS                     status;
    RM_API_CONTEXT                rmApiContext   = {0};
    RS_LOCK_INFO                  lockInfo;

    NV_PRINTF(LEVEL_INFO,
              "Nv04Unmap: client:0x%x device:0x%x context:0x%x\n",
              hClient, hDevice, hMemCtx);
    NV_PRINTF(LEVEL_INFO, "Nv04Unmap:  flags:0x%x dmaOffset:0x%08llx size:0x%llx\n",
              flags, dmaOffset, size);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
        return status;

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, NV01_NULL_OBJECT, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return status;
    }
    lockInfo.flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK |
                      RM_LOCK_FLAGS_NO_GPUS_LOCK;

    LOCK_METER_DATA(UNMAPMEM_DMA, flags, 0, 0);

    // Unmap DMA memory
    status = _rmapiRmUnmapMemoryDma(hClient, hDevice, hMemCtx, flags,
                                    dmaOffset, size, &lockInfo, pSecInfo);

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv04Unmap: Unmap complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Nv04Unmap: ummap failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
    }

    return status;
}

NV_STATUS
rmapiUnmapWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvU32              flags,
    NvU64              dmaOffset,
    NvU64              size,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiUnmapWithSecInfo(pRmApi, hClient, hDevice, hMemCtx, flags, dmaOffset, size, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
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
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

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
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_INTER_UNMAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverUpdateLockFlagsForInterAutoUnmap
(
    RsServer *pServer,
    RS_INTER_UNMAP_PARAMS *pParams
)
{
    pParams->pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK |
                                 RM_LOCK_FLAGS_GPU_GROUP_LOCK;

    return NV_OK;
}
