/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_user_shared_data.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "class/cl00de.h"
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM

#include "gpu/mig_mgr/kernel_mig_manager.h"

static NV_STATUS _gpushareddataInitGsp(GpuUserSharedData *pData, NvBool bInit);
static NV_STATUS _gpushareddataRequestDataPoll(GpuUserSharedData *pData, NvU64 polledDataMask);

NV_STATUS
gpushareddataConstruct_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS            status   = NV_OK;
    Memory              *pMemory  = staticCast(pData, Memory);
    OBJGPU              *pGpu     = pMemory->pGpu; // pGpu is initialized in the Memory class constructor
    MEMORY_DESCRIPTOR  **ppMemDesc = &(pGpu->userSharedData.pMemDesc);
    NV00DE_ALLOC_PARAMETERS *pAllocParams = (NV00DE_ALLOC_PARAMETERS*)(pParams->pAllocParams);

    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (RS_IS_COPY_CTOR(pParams))
    {
        return NV_OK;
    }

    if (*ppMemDesc == NULL)
    {
        OBJSYS    *pSys = SYS_GET_INSTANCE();

        // RUSD is not yet supported when CPU CC is enabled. See bug 4148522.
        if ((sysGetStaticConfig(pSys))->bOsCCEnabled)
            return NV_ERR_NOT_SUPPORTED;

        // Create a kernel-side mapping for writing the data if one is not already present
        NV_ASSERT_OK_OR_RETURN(memdescCreate(ppMemDesc, pGpu, sizeof(NV00DE_SHARED_DATA), 0, NV_TRUE,
                               ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_USER_READ_ONLY));

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_35, 
                    (*ppMemDesc));
        NV_ASSERT_OK_OR_GOTO(status, status, err);


        NV_ASSERT_OK_OR_GOTO(status,
            memdescMap(*ppMemDesc, 0, (*ppMemDesc)->Size,
                        NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pGpu->userSharedData.pMapBuffer,
                        &pGpu->userSharedData.pMapBufferPriv),
                        err);

        portMemSet(pGpu->userSharedData.pMapBuffer, 0, sizeof(NV00DE_SHARED_DATA));

        // Initial write from cached data
        gpuUpdateUserSharedData(pGpu);

        if (IS_GSP_CLIENT(pGpu))
        {
           // Init system memdesc on GSP
           _gpushareddataInitGsp(pData, NV_TRUE);
        }
    }

    if (pAllocParams->polledDataMask != 0U)
    {
        NV_ASSERT_OK_OR_RETURN(_gpushareddataRequestDataPoll(pData, pAllocParams->polledDataMask));
    }

    NV_ASSERT_OK_OR_RETURN(memConstructCommon(pMemory,
                NV01_MEMORY_SYSTEM, 0, *ppMemDesc, 0, NULL, 0, 0, 0, 0,
                NVOS32_MEM_TAG_NONE, NULL));
    memdescAddRef(pGpu->userSharedData.pMemDesc);

    return NV_OK;

err: // Only for global memdesc construct fail cleanup
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
    *ppMemDesc = NULL;
    return status;
}

void
gpushareddataDestruct_IMPL(GpuUserSharedData *pData)
{
    Memory *pMemory = staticCast(pData, Memory);
    OBJGPU *pGpu = pMemory->pGpu;

    NV_ASSERT_OR_RETURN_VOID(!RMCFG_FEATURE_PLATFORM_GSP);

    if (!pMemory->bConstructed || (pMemory->pMemDesc == NULL))
    {
        return;
    }

    _gpushareddataRequestDataPoll(pData, 0U);

    memdescRemoveRef(pGpu->userSharedData.pMemDesc);
    memDestructCommon(pMemory);

    if (pGpu->userSharedData.pMemDesc->RefCount <= 1)
    {
        NV_ASSERT(pGpu->userSharedData.pMemDesc->RefCount == 1);

        if (IS_GSP_CLIENT(pGpu))
        {
           _gpushareddataInitGsp(pData, NV_FALSE);
        }

        gpushareddataDestroy(pGpu);
    }
}

NvBool
gpushareddataCanCopy_IMPL(GpuUserSharedData *pData)
{
    return NV_TRUE;
}

NV00DE_SHARED_DATA * gpushareddataWriteStart(OBJGPU *pGpu)
{
    return &pGpu->userSharedData.data;
}

void gpushareddataWriteFinish(OBJGPU *pGpu)
{
    gpuUpdateUserSharedData(pGpu);
}

void gpuUpdateUserSharedData_IMPL(OBJGPU *pGpu)
{
    NV00DE_SHARED_DATA *pSharedData = (NV00DE_SHARED_DATA*)(pGpu->userSharedData.pMapBuffer);
    const NvU32 data_offset = sizeof(pSharedData->seq);
    const NvU32 data_size = NV_OFFSETOF(NV00DE_SHARED_DATA, clkPublicDomainInfos) - data_offset;

    if (pSharedData == NULL)
        return;

    portAtomicIncrementU32(&pSharedData->seq);
    portAtomicMemoryFenceStore();

    // Push cached data to mapped buffer
    if (IS_MIG_ENABLED(pGpu))
    {
        // Global Bar1 data is inaccurate if MIG is enabled, invalidate
        portMemSet((NvU8*)pSharedData + data_offset, 0, data_size);
    }
    else
    {
        portMemCopy((NvU8*)pSharedData + data_offset, data_size,
                    (NvU8*)&pGpu->userSharedData.data + data_offset, data_size);
    }

    portAtomicMemoryFenceStore();
    portAtomicIncrementU32(&pSharedData->seq);
}

static NV_STATUS
_gpushareddataInitGsp
(
    GpuUserSharedData *pData,
    NvBool bInit
)
{
    NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS params = { 0 };
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (bInit)
        params.physAddr = memdescGetPhysAddr(pGpu->userSharedData.pMemDesc, AT_GPU, 0);
    else
        params.physAddr = 0ULL;

    // Link up Memdesc on GSP-side
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_INIT_USER_SHARED_DATA,
                                           &params, sizeof(params)));
    return NV_OK;
}

void
gpushareddataDestroy
(
    OBJGPU *pGpu
)
{
    GpuSharedDataMap *pData = &pGpu->userSharedData;

    memdescUnmap(pData->pMemDesc, NV_TRUE, 0,
                 pData->pMapBuffer,
                 pData->pMapBufferPriv);
    memdescFree(pData->pMemDesc);
    memdescDestroy(pData->pMemDesc);
    pData->pMemDesc = NULL;
    pData->pMapBuffer = NULL;
    pData->pMapBufferPriv = NULL;
}

static NV_STATUS
_gpushareddataRequestDataPoll
(
    GpuUserSharedData *pData,
    NvU64 polledDataMask
)
{
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS params;
    NvU64 polledDataUnion = 0U;
    RmClient **ppClient;

    if (polledDataMask == pData->polledDataMask)
        return NV_OK; // Nothing to do

    pData->polledDataMask = polledDataMask;

    // Iterate over all clients to get all RUSD objects
    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient != NULL;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        // Collect mask of all data requested by any existing RUSD objects
        RS_ITERATOR iter = clientRefIter(staticCast(*ppClient, RsClient), NULL,
                                         classId(GpuUserSharedData), RS_ITERATE_DESCENDANTS, NV_TRUE);
        while (clientRefIterNext(iter.pClient, &iter))
        {
            // Ignore RUSD objects on GPUs that don't match the RUSD object this poll request came from
            GpuUserSharedData *pIterData = dynamicCast(iter.pResourceRef->pResource, GpuUserSharedData);
            if ((pIterData != NULL) && (staticCast(pIterData, Memory)->pGpu == pGpu))
            {
                polledDataUnion |= pIterData->polledDataMask;
            }
        }
    }

    if (polledDataUnion == pGpu->userSharedData.lastPolledDataMask)
        return NV_OK; // Nothing to do

    params.polledDataMask = polledDataUnion;

    // Send updated data request to GSP
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL,
                                           &params, sizeof(params)));

    pGpu->userSharedData.lastPolledDataMask = polledDataUnion;

    return NV_OK;
}

NV_STATUS
gpushareddataCtrlCmdRequestDataPoll_IMPL
(
    GpuUserSharedData *pData,
    NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams
)
{
    return _gpushareddataRequestDataPoll(pData, pParams->polledDataMask);
}
