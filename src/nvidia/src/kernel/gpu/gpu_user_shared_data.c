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

#include "gpu/gpu_user_shared_data.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/subdevice/subdevice.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "nvrm_registry.h"
#include "class/cl00de.h"
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl00de.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"

#include "gpu/mig_mgr/kernel_mig_manager.h"

static NV_STATUS _gpushareddataInitGsp(OBJGPU *pGpu);
static void _gpushareddataDestroyGsp(OBJGPU *pGpu);
static NV_STATUS _gpushareddataSendDataPollRpc(OBJGPU *pGpu, NvU64 polledDataMask);
static NV_STATUS _gpushareddataRequestDataPoll(GpuUserSharedData *pData, NvU64 polledDataMask);
static inline void _gpushareddataUpdateSeqOpen(volatile NvU64 *pSeq);
static inline void _gpushareddataUpdateSeqClose(volatile NvU64 *pSeq);

static inline
NvBool
_rusdPollingSupported
(
    OBJGPU *pGpu
)
{
    //
    // RUSD polling is disabled on non-GSP for pre-GA102 due to collisions
    // with VSYNC interrupt on high refresh rate monitors. See Bug 4432698.
    // For GA102+, the RPC to PMU are replaced by PMUMON RMCTRLs.
    //
    return ((!IS_VIRTUAL(pGpu)) && 
            (pGpu->userSharedData.pollingRegistryOverride != NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_DISABLE) &&
            (IS_GSP_CLIENT(pGpu) ||
                (pGpu->userSharedData.pollingRegistryOverride == NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE) ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_RUSD_POLLING_SUPPORT_MONOLITHIC)));
}

NV_STATUS
gpushareddataConstruct_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory              *pMemory  = staticCast(pData, Memory);
    OBJGPU              *pGpu     = pMemory->pGpu; // pGpu is initialized in the Memory class constructor
    MEMORY_DESCRIPTOR  **ppMemDesc = &(pGpu->userSharedData.pMemDesc);
    NV00DE_ALLOC_PARAMETERS *pAllocParams = (NV00DE_ALLOC_PARAMETERS*)(pParams->pAllocParams);

    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    if (IS_VIRTUAL(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    if (!_rusdPollingSupported(pGpu) && (pAllocParams->polledDataMask != 0U))
        return NV_ERR_NOT_SUPPORTED;

    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    if (*ppMemDesc == NULL)
        return NV_ERR_NOT_SUPPORTED;

    if (pAllocParams->polledDataMask != 0U)
    {
        NV_ASSERT_OK_OR_RETURN(_gpushareddataRequestDataPoll(pData, pAllocParams->polledDataMask));
    }

    NV_ASSERT_OK_OR_RETURN(memConstructCommon(pMemory,
                NV01_MEMORY_SYSTEM, 0, *ppMemDesc, 0, NULL, 0, 0, 0, 0,
                NVOS32_MEM_TAG_NONE, NULL));
    memdescAddRef(pGpu->userSharedData.pMemDesc);

    return NV_OK;
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
}

// Called before starting a non-polled data write, changes seq valid->invalid
static inline void
_gpushareddataUpdateSeqOpen
(
    volatile NvU64 *pSeq
)
{
    NvU64 seqVal;

    // Initialize seq to RUSD_SEQ_START at first write. If never written before, seq is treated as an invalid timestamp
    if (MEM_RD64(pSeq) == 0LLU)
    {
        portAtomicExSetU64(pSeq, RUSD_SEQ_START + 1);
    }
    else
    {
        portAtomicExIncrementU64(pSeq);
    }

    portAtomicMemoryFenceStore();

    seqVal = MEM_RD64(pSeq);

    NV_ASSERT(!RUSD_SEQ_DATA_VALID(seqVal));
}

// Called after finishing a non-polled data write, changes seq invalid->valid
static inline void
_gpushareddataUpdateSeqClose
(
    volatile NvU64 *pSeq
)
{
    NvU64 seqVal;

    portAtomicExIncrementU64(pSeq);
    portAtomicMemoryFenceStore();

    seqVal = MEM_RD64(pSeq);

    NV_ASSERT(RUSD_SEQ_DATA_VALID(seqVal));
}


NvBool
gpushareddataCanCopy_IMPL(GpuUserSharedData *pData)
{
    return NV_TRUE;
}

NV00DE_SHARED_DATA * gpushareddataWriteStart_INTERNAL(OBJGPU *pGpu, NvU64 offset)
{
    NV00DE_SHARED_DATA *pSharedData = (NV00DE_SHARED_DATA *) pGpu->userSharedData.pMapBuffer;

    if (pSharedData == NULL)
    {
        pSharedData = &pGpu->userSharedData.data;
    }
    
    _gpushareddataUpdateSeqOpen((volatile NvU64*)(((NvU8*)pSharedData) + offset));

    return pSharedData;
}

void gpushareddataWriteFinish_INTERNAL(OBJGPU *pGpu, NvU64 offset)
{
    NV00DE_SHARED_DATA *pSharedData = (NV00DE_SHARED_DATA *) pGpu->userSharedData.pMapBuffer;

    if (pSharedData == NULL)
    {
        pSharedData = &pGpu->userSharedData.data;
    }

    _gpushareddataUpdateSeqClose((volatile NvU64*)(((NvU8*)pSharedData) + offset));
}
static void
_gpushareddataDestroyGsp
(
    OBJGPU *pGpu
)
{
    NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS params = { 0 };
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    params.bInit = NV_FALSE;

    // Free Memdesc on GSP-side
    NV_CHECK_OK(status, LEVEL_ERROR,
                pRmApi->Control(pRmApi, pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_INIT_USER_SHARED_DATA,
                               &params, sizeof(params)));
}

static NV_STATUS
_gpushareddataInitGsp
(
    OBJGPU *pGpu
)
{
    NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS params = { 0 };
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    params.bInit = NV_TRUE;
    params.physAddr = memdescGetPhysAddr(pGpu->userSharedData.pMemDesc, AT_GPU, 0);

    // Link up Memdesc on GSP-side
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_INIT_USER_SHARED_DATA,
                                           &params, sizeof(params)));

    if (pGpu->userSharedData.pollingRegistryOverride == NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE)
    {
        // If polling is forced always on, start polling during init and never stop
        return _gpushareddataSendDataPollRpc(pGpu, ~0ULL);
    }

    return NV_OK;
}

static NV_STATUS
_gpushareddataInitPollingFrequency
(
    OBJGPU *pGpu
)
{
    NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS params = { 0 };
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    //
    // Let Kernel RM decide the polling frequency.
    // RUSD currently does not support VGPU, skip the RMCTRL on vGPU.
    //
    if (!RMCFG_FEATURE_PLATFORM_GSP && !IS_VIRTUAL(pGpu))
    {
        if (!pGpu->userSharedData.bPollFrequencyOverridden && gpuIsTeslaBranded(pGpu))
            pGpu->userSharedData.pollingFrequencyMs = NV_REG_STR_RM_RUSD_POLLING_INTERVAL_TESLA;

        params.polledDataMask = pGpu->userSharedData.lastPolledDataMask;
        params.pollFrequencyMs = pGpu->userSharedData.pollingFrequencyMs;

        NV_ASSERT_OK_OR_RETURN(
            pRmApi->Control(pRmApi, pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL,
                            &params, sizeof(params)));
    }

    return NV_OK;
}

NV_STATUS
gpuCreateRusdMemory_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS            status   = NV_OK;
    MEMORY_DESCRIPTOR  **ppMemDesc = &(pGpu->userSharedData.pMemDesc);

    // RUSD is not yet supported when CPU CC is enabled. See bug 4148522.
    if ((sysGetStaticConfig(SYS_GET_INSTANCE()))->bOsCCEnabled)
        return NV_OK;

    // Create a kernel-side mapping for writing RUSD data
    NV_ASSERT_OK_OR_RETURN(memdescCreate(ppMemDesc, pGpu, sizeof(NV00DE_SHARED_DATA), 0, NV_TRUE,
                           ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_USER_READ_ONLY));

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_RUSD_BUFFER, (*ppMemDesc));
    NV_ASSERT_OK_OR_GOTO(status, status, err);

    pGpu->userSharedData.pMapBuffer = memdescMapInternal(pGpu, *ppMemDesc, TRANSFER_FLAGS_NONE);
    if (pGpu->userSharedData.pMapBuffer == NULL)
    {
        status = NV_ERR_MEMORY_ERROR;
        goto err;
    }

    portMemSet(pGpu->userSharedData.pMapBuffer, 0, sizeof(NV00DE_SHARED_DATA));

    if (IS_GSP_CLIENT(pGpu))
    {
       // Init system memdesc on GSP
       _gpushareddataInitGsp(pGpu);
    }

    _gpushareddataInitPollingFrequency(pGpu);

    return NV_OK;

err: // Only for global memdesc construct fail cleanup
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
    *ppMemDesc = NULL;
    return status;
}

void
gpuDestroyRusdMemory_IMPL
(
    OBJGPU *pGpu
)
{
    GpuSharedDataMap *pData = &pGpu->userSharedData;

    if (pData->pMemDesc == NULL)
        return;

    if (IS_GSP_CLIENT(pGpu))
    {
       // Destroy system memdesc on GSP
       _gpushareddataDestroyGsp(pGpu);
    }

    NV_ASSERT(pGpu->userSharedData.pMemDesc->RefCount == 1);

    memdescUnmapInternal(pGpu, pData->pMemDesc, TRANSFER_FLAGS_NONE);
    memdescFree(pData->pMemDesc);
    memdescDestroy(pData->pMemDesc);
    pData->pMemDesc = NULL;
    pData->pMapBuffer = NULL;
}

static NV_STATUS
_gpushareddataSendDataPollRpc
(
    OBJGPU *pGpu,
    NvU64 polledDataMask
)
{
    NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS params;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    if (polledDataMask == pGpu->userSharedData.lastPolledDataMask)
        return NV_OK; // Nothing to do

    portMemSet(&params, 0, sizeof(params));

    params.polledDataMask = polledDataMask;

    // Send updated data request to GSP
    status = pRmApi->Control(pRmApi, pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL,
                             &params, sizeof(params));
    NV_ASSERT_OR_RETURN((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET), status);
    if (status == NV_ERR_GPU_IN_FULLCHIP_RESET)
        return status;
    pGpu->userSharedData.lastPolledDataMask = polledDataMask;

    return NV_OK;
}

static NV_STATUS
_gpushareddataRequestDataPoll
(
    GpuUserSharedData *pData,
    NvU64 polledDataMask
)
{
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;
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

    return _gpushareddataSendDataPollRpc(pGpu, polledDataUnion);
}

NV_STATUS
gpushareddataCtrlCmdRequestDataPoll_IMPL
(
    GpuUserSharedData *pData,
    NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams
)
{
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;

    // Polling is always forced on, no point routing to GSP because we will never change state
    if (pGpu->userSharedData.pollingRegistryOverride == NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE)
        return NV_OK;

    if (!_rusdPollingSupported(pGpu) && (pParams->polledDataMask != 0U))
        return NV_ERR_NOT_SUPPORTED;

    return _gpushareddataRequestDataPoll(pData, pParams->polledDataMask);
}

NV_STATUS
subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams
)
{
    POBJGPU pGpu = GPU_RES_GET_GPU(pSubdevice);

    pParams->supportedFeatures = 0;

    if (!IS_VIRTUAL(pGpu))
    {
        pParams->supportedFeatures |= RUSD_FEATURE_NON_POLLING;
    }

    if (_rusdPollingSupported(pGpu))
    {
        pParams->supportedFeatures |= RUSD_FEATURE_POLLING;
    }

    return NV_OK;
}