/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu_mgr/gpu_db.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "nvctassert.h"

#include "gpu/mig_mgr/kernel_mig_manager.h"

static NV_STATUS _gpushareddataInitGsp(OBJGPU *pGpu);
static void _gpushareddataDestroyGsp(OBJGPU *pGpu);
static NV_STATUS _gpushareddataSendDataPollRpc(OBJGPU *pGpu, NvU64 polledDataMask, NvU32 pollingIntervalMs);
static inline void _gpushareddataUpdateSeqOpen(volatile NvU64 *pSeq);
static inline void _gpushareddataUpdateSeqClose(volatile NvU64 *pSeq);
static NV_STATUS _handlePollMaskHelper(OBJGPU *, NvBool, NvBool);

//
// A subtle difference between vGPU and MODS/CC is that we create RUSD memory for vGPU
// but not for MODS/CC.
//
static inline NvBool
_rusdSupported
(
    OBJGPU *pGpu
)
{

    if (IS_VIRTUAL(pGpu))
        return NV_FALSE;

    // RUSD is not yet supported when CPU CC is enabled. See bug 4148522.
    if ((sysGetStaticConfig(SYS_GET_INSTANCE()))->bOsCCEnabled)
        return NV_FALSE;

    return NV_TRUE;
}

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
    return _rusdSupported(pGpu) &&
            (pGpu->userSharedData.pollingRegistryOverride != NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_DISABLE) &&
            (IS_GSP_CLIENT(pGpu) ||
                (pGpu->userSharedData.pollingRegistryOverride == NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE) ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_RUSD_POLLING_SUPPORT_MONOLITHIC));
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

    if (!_rusdSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    if (!_rusdPollingSupported(pGpu) && (pAllocParams->polledDataMask != 0U))
        return NV_ERR_NOT_SUPPORTED;

    pData->pollingIntervalMs = pGpu->userSharedData.defaultPollingIntervalMs;

    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    if (*ppMemDesc == NULL)
        return NV_ERR_NOT_SUPPORTED;

    // TODO: no need to iterate through all clients for adding a new mask
    if (pAllocParams->polledDataMask != 0U)
    {
        pData->polledDataMask = pAllocParams->polledDataMask;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _handlePollMaskHelper(pGpu, NV_FALSE, NV_FALSE));
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

    // On object destruction, reset to default and re-trigger polling param calculation.
    pData->polledDataMask = 0U;
    pData->pollingIntervalMs = pGpu->userSharedData.defaultPollingIntervalMs;
    NV_CHECK(LEVEL_ERROR, _handlePollMaskHelper(pGpu, NV_FALSE, NV_TRUE) == NV_OK);

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

    // TODO: JIRA CORERM-7317 this runs only on Kernel RM. Need to support Monolithic RM..
    if (pGpu->userSharedData.pollingRegistryOverride ==
        NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE)
    {
        // Value should already be initialized by registry override
        if (pGpu->userSharedData.pollingIntervalMs == 0)
        {
            NV_ASSERT(0);
            // as a safety measure, initialize to default value.
            if (gpuIsTeslaBranded(pGpu))
            {
                pGpu->userSharedData.pollingIntervalMs = NV_REG_STR_RM_RUSD_POLLING_INTERVAL_TESLA;
            }
            else if (RMCFG_FEATURE_PLATFORM_WINDOWS && IS_GSP_CLIENT(pGpu))
            {
                pGpu->userSharedData.pollingIntervalMs = NV_REG_STR_RM_RUSD_POLLING_INTERVAL_WINDOWS_GSP;
            }
            else
            {
                pGpu->userSharedData.pollingIntervalMs = NV_REG_STR_RM_RUSD_POLLING_INTERVAL_DEFAULT;
            }
        }

        // If polling is forced always on, start polling during init and never stop
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _gpushareddataSendDataPollRpc(pGpu, ~0ULL, pGpu->userSharedData.pollingIntervalMs));
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
        if (!pGpu->userSharedData.bPollIntervalOverridden && gpuIsTeslaBranded(pGpu))
        {
            pGpu->userSharedData.pollingIntervalMs = NV_REG_STR_RM_RUSD_POLLING_INTERVAL_TESLA;
        }

        pGpu->userSharedData.defaultPollingIntervalMs = pGpu->userSharedData.pollingIntervalMs;

        NV_PRINTF(LEVEL_INFO, "Default RUSD polling frequency: %d ms\n", pGpu->userSharedData.defaultPollingIntervalMs);

        params.polledDataMask = pGpu->userSharedData.lastPolledDataMask;
        params.pollIntervalMs = pGpu->userSharedData.pollingIntervalMs;

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

    ct_assert(NV00DE_RUSD_POLLING_INTERVAL_MIN == NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MIN);

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
    NvU64 polledDataMask,
    NvU32 pollingIntervalMs
)
{
    NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS params;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    if (pollingIntervalMs == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(pollingIntervalMs >= NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MIN);
    NV_ASSERT(pollingIntervalMs <= pGpu->userSharedData.defaultPollingIntervalMs);

    //
    // If the registry is set, override the polling mask to all data.
    // With the registry set, non-poll interval request should return in the below check.
    //
    if (pGpu->userSharedData.pollingRegistryOverride ==
            NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE)
    {
        polledDataMask = ~0ULL;
    }

    if (polledDataMask == pGpu->userSharedData.lastPolledDataMask &&
        pollingIntervalMs == pGpu->userSharedData.pollingIntervalMs)
    {
        return NV_OK; // Nothing to do
    }

    NV_PRINTF(LEVEL_INFO, "RUSD polledDataMask: 0x%llx, pollingIntervalMs: %u\n",
              polledDataMask, pollingIntervalMs);

    portMemSet(&params, 0, sizeof(params));

    params.polledDataMask = polledDataMask;
    params.pollIntervalMs = pollingIntervalMs;

    // Send updated data request to GSP
    status = pRmApi->Control(pRmApi, pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL,
                             &params, sizeof(params));
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                      (status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET),
                       status);
    if (status == NV_ERR_GPU_IN_FULLCHIP_RESET)
        return status;
    pGpu->userSharedData.lastPolledDataMask = polledDataMask;
    pGpu->userSharedData.pollingIntervalMs = pollingIntervalMs;

    return NV_OK;
}

static void _gpushareddataGetPollDataUnion
(
    OBJGPU *pGpu,
    NvU64 *pPolledDataMask,
    NvU32 *pPollingIntervalMs
)
{
    RmClient **ppClient;

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
                // poll data mask is the union of mask requested by all RUSD clients
                (*pPolledDataMask) |= pIterData->polledDataMask;

                NV_ASSERT(pIterData->pollingIntervalMs >= NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MIN);
                NV_ASSERT(pIterData->pollingIntervalMs <= pGpu->userSharedData.defaultPollingIntervalMs);

                // polling interval is the minimum of all RUSD clients
                if (pIterData->pollingIntervalMs < *pPollingIntervalMs)
                {
                    *pPollingIntervalMs = pIterData->pollingIntervalMs;
                }
            }
        }
    }
}

/*
 * bPermanentRequest:
 *   NV_TRUE  : if this is a permanent polling request that comes from
 *              NV2080_CTRL_CMD_GPU_RUSD_SET_FEATURES or the kernel channel ctor/dtor
 *   NV_FALSE : if this is NOT a permanent polling request, i.e., it's a polling
 *              request for the RUSD clients.
 *
 * bPollingIntervalRequested:
 *   NV_TRUE: if the caller requests polling intertval change.
 *   NV_FALSE: if the caller does not request polling interval change.
 */
static NV_STATUS
_handlePollMaskHelper
(
    OBJGPU *pGpu,
    NvBool bPermanentRequest,
    NvBool bPollingIntervalRequested
)
{
    GPU_DB_RUSD_SETTINGS gpudbRusd = { 0 };
    NvU64 permanentPolledDataMask = 0;
    NvU64 polledDataMask = 0;
    NvU32 pollingIntervalMs = pGpu->userSharedData.defaultPollingIntervalMs;
    NV_STATUS status;
    NvU32 originalPollingIntervalMs = pGpu->userSharedData.pollingIntervalMs;

    NV_ASSERT(pollingIntervalMs != 0);

    // A permanent request should never request polling interval change.
    NV_ASSERT_OR_RETURN(!(bPermanentRequest && bPollingIntervalRequested),
                        NV_ERR_INVALID_ARGUMENT);

    // Get the permanent mask if the setting is active.
    if (pGpu->numUserKernelChannel > 0)
    {
        if (gpudbGetRusdSettings(pGpu->gpuUuid.uuid, &gpudbRusd) == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "RUSD permanent polled data mask: 0x%llx\n",
                      gpudbRusd.permanentPolledDataMask);
            permanentPolledDataMask = gpudbRusd.permanentPolledDataMask;
        }
    }

    // Skip if the permanent request is the same as lastPolledDataMask
    if (bPermanentRequest &&
        (permanentPolledDataMask == pGpu->userSharedData.lastPolledDataMask))
    {
        return NV_OK;
    }

    // Combine with 00DE objects polling mask
    _gpushareddataGetPollDataUnion(pGpu, &polledDataMask, &pollingIntervalMs);

    polledDataMask |= permanentPolledDataMask;

    //
    // If this is not a polling interval request, the calculated polling
    // interval should be the same as the global polling interval.
    //
    if (!bPollingIntervalRequested)
    {
        NV_ASSERT(pollingIntervalMs == pGpu->userSharedData.pollingIntervalMs);
    }

    // on failed request, global polling interval should not change
    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        _gpushareddataSendDataPollRpc(pGpu, polledDataMask, pollingIntervalMs),
        NV_ASSERT(pGpu->userSharedData.pollingIntervalMs == originalPollingIntervalMs));

    return status;
}

static void
_gpuRusdRequestPermanentDataPollCallback
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    
    if (pGpu == NULL)
        return;

    NV_CHECK(LEVEL_ERROR, _handlePollMaskHelper(pGpu, NV_TRUE, NV_FALSE) == NV_OK);
}

NV_STATUS
gpuRusdRequestPermanentDataPoll_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS status;

    if (!_rusdPollingSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    status = osQueueWorkItem(pGpu,
                             _gpuRusdRequestPermanentDataPollCallback,
                             NULL,
                             (OsQueueWorkItemFlags){
                                 .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                 .bLockGpuGroupDevice = NV_TRUE});

    if (status != NV_OK)
        NV_PRINTF(LEVEL_ERROR, "Fail to queue work _gpuRusdRequestPermanentDataPollCallback\n");

    return status;
}

NV_STATUS
gpushareddataCtrlCmdRequestDataPoll_IMPL
(
    GpuUserSharedData *pData,
    NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams
)
{
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;
    NvU64 originalPolledDataMask;
    NV_STATUS status;

    if (!_rusdPollingSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    // Polling is always forced on, no point routing to GSP because we will never change state
    if (pGpu->userSharedData.pollingRegistryOverride == NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE)
        return NV_OK;

    if (pData->polledDataMask == pParams->polledDataMask)
        return NV_OK;

    originalPolledDataMask = pData->polledDataMask;
    pData->polledDataMask = pParams->polledDataMask;

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        _handlePollMaskHelper(pGpu, NV_FALSE, NV_FALSE),
        pData->polledDataMask = originalPolledDataMask; return status);

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    pParams->supportedFeatures = 0;

    if (!_rusdSupported(pGpu))
        return NV_OK;

    pParams->supportedFeatures |= RUSD_FEATURE_NON_POLLING;

    if (_rusdPollingSupported(pGpu))
    {
        pParams->supportedFeatures |= RUSD_FEATURE_POLLING;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdRusdSetFeatures_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_RUSD_SET_FEATURES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    GPU_DB_RUSD_SETTINGS gpudbRusd = { 0 };
    NV_STATUS status = NV_OK;

    if (!_rusdPollingSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    /* 
     * permanentPolledDataMask is saved in gpudb
     * The permanentPolledDataMask is preserved when GPU is unbind.
     * when GPU is rebind (same gpuId), the permanentPolledDataMask will be applied.
     */
    gpudbRusd.permanentPolledDataMask = pParams->permanentPolledDataMask;

    status = gpudbSetRusdSettings(pGpu->gpuUuid.uuid, &gpudbRusd);

    if (status == NV_OK)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _handlePollMaskHelper(pGpu, NV_TRUE, NV_FALSE)); 
    }

    return status;
}

NV_STATUS
gpushareddataCtrlCmdRequestPollInterval_IMPL
(
    GpuUserSharedData *pData,
    NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM *pParams
)
{
    OBJGPU *pGpu = staticCast(pData, Memory)->pGpu;
    NvU32 originalGlobalPollingIntervalMs;
    NvU32 originalClientPollingIntervalMs;
    NvU32 inputIntervalMs;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!_rusdPollingSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    inputIntervalMs = pParams->pollingIntervalMs;

    // 0ms is a special value used to reset the polling interval to the default.
    if (inputIntervalMs < NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MIN && inputIntervalMs != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid input polling interval: %u ms\n", inputIntervalMs);
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // If a client requests a polling interval longer than the default, we
    // do not treat it as an error. The system's default polling is already
    // faster, thus satisfying the client's requirement. This simplifies
    // client logic as they don't need to know the default interval.
    //
    // A special case: a requested interval of 0 resets the client's
    // override, causing it to fall back to the default polling behavior.
    //
    if (inputIntervalMs > pGpu->userSharedData.defaultPollingIntervalMs || inputIntervalMs == 0)
    {
        inputIntervalMs = pGpu->userSharedData.defaultPollingIntervalMs;
    }

    originalGlobalPollingIntervalMs = pGpu->userSharedData.pollingIntervalMs;
    originalClientPollingIntervalMs = pData->pollingIntervalMs;
    pData->pollingIntervalMs = inputIntervalMs;

    //
    // Requested polling interval must be valid at this point. If the requested
    // interval is smaller than the current one, poll at the new interval.
    //
    if (originalGlobalPollingIntervalMs > inputIntervalMs)
    {
        NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
            _gpushareddataSendDataPollRpc(pGpu,
                                          pGpu->userSharedData.lastPolledDataMask,
                                          inputIntervalMs),
            pData->pollingIntervalMs = originalClientPollingIntervalMs);

        return status;
    }

    //
    // If RUSD already uses a shorter interval than the requested, and the
    // current polling interval does not come from this client, the request
    // won't affect the global polling interval.
    // Update the interval for this client and return.
    //
    if (originalGlobalPollingIntervalMs < originalClientPollingIntervalMs &&
        originalGlobalPollingIntervalMs < inputIntervalMs)
    {
        return NV_OK;
    }

    // If we're already polling at the requested polling freq, nothing to do.
    if (originalGlobalPollingIntervalMs == inputIntervalMs)
    {
        return NV_OK;
    }

    //
    // The only case reaches here is when the global polling interval equals
    // the current client specific interval. And the requested interval is longer.
    // In this case, we'll need to iterate through the client list to find
    // the new polling interval.
    //
    NV_ASSERT(originalGlobalPollingIntervalMs == originalClientPollingIntervalMs);
    NV_ASSERT(originalGlobalPollingIntervalMs < inputIntervalMs);

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        _handlePollMaskHelper(pGpu, NV_FALSE, NV_TRUE),
        pData->pollingIntervalMs = originalClientPollingIntervalMs);

    return status;
}
