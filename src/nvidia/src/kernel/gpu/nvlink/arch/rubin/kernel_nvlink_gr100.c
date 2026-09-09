/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/diagnostics/nv_debug_dump.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/bus/p2p_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/gpu_fabric_probe.h"
#include "rmapi/rs_utils.h"

#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"

static void _knvlinkABMLinkMaskUpdate_GR100_WORKITEM(NvU32 gpuInstance, void *pArgs);
/*!
 * Retrieve list of supported BW modes
 */
 NV_STATUS
 knvlinkGetSupportedBwMode_GR100
 (
     OBJGPU *pGpu,
     KernelNvlink *pKernelNvlink,
     NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams
 )
 {
     NvU32 i;
     NvU32 numModes = 0;
     NVLINK_BIT_VECTOR supportedBwModes = { 0 };

     (void)gpuFabricProbeGetSupportedBwModes(pGpu->pGpuFabricProbeInfoKernel, &supportedBwModes);

     pKernelNvlink->totalRbmModes = pKernelNvlink->maxRbmLinks;

     // Save off the latest supported BW modes from probe response
     if (!bitVectorTestAllCleared(&supportedBwModes))
     {
        FOR_EACH_IN_BITVECTOR(&supportedBwModes, i)
        {
            pKernelNvlink->hshubSupportedRbmModesList[numModes] = DRF_NUM(0000_CTRL_CMD_GPU, _NVLINK_BW_MODE_SETTING, _LINK_COUNT, i);
            numModes++;
        }
        FOR_EACH_IN_BITVECTOR_END();
     }

     numModes = NV_MIN(numModes, NV2080_CTRL_NVLINK_SUPPORTED_MAX_BW_MODE_COUNT);
     for (i = 0; i < numModes; i++)
     {
        pParams->rbmModesList[i] = pKernelNvlink->hshubSupportedRbmModesList[i];
     }

     pParams->rbmTotalModes = numModes;

     return NV_OK;
 }


 /*!
 * Check if requested RBM mode is supported by GPU
 */
NvBool
knvlinkIsBwModeSupported_GR100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU16    mode
)
{
    NvU8 linkCount = DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, mode);
    NVLINK_BIT_VECTOR supportedBwModes = { 0 };

    // Legacy BW modes are not supported except _FULL
    if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_LINK_COUNT) &&
        (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_FULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Legacy BW modes are not supported on this platform.\n");
        return NV_FALSE;
    }

    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL)
    {
        // Requesting full bandwidth on GPU
        return NV_TRUE;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuFabricProbeGetSupportedBwModes(pGpu->pGpuFabricProbeInfoKernel, &supportedBwModes));
    if (bitVectorTest(&supportedBwModes, linkCount))
    {
        return NV_TRUE;
    }

    NV_PRINTF(LEVEL_ERROR, "RBM requested is not supported. LinkCount: %d\n",
              linkCount);
    return NV_FALSE;
}

static NV_STATUS 
_knvlinkAbmRetryTimerCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    NV_STATUS status = NV_OK;
    NVLINK_RESILIENCY_INFO *pResiliencyInfo;

    pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    if (pResiliencyInfo == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    status =  osQueueWorkItem(pGpu,
        _knvlinkABMLinkMaskUpdate_GR100_WORKITEM,
            NULL,
            (OsQueueWorkItemFlags){
                    .bLockSema = NV_TRUE,
                    .bLockGpus = NV_TRUE,
                    .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                    .bDontFreeParams = NV_TRUE
                });

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to queue ABMLinkMaskUpdate workitem (retry_count=%d) status=0x%x\n", pResiliencyInfo->abmRetryCount, status);
        portAtomicSetU32(&pResiliencyInfo->bPendingAbmLinkMaskUpdate, 0);
    }

    return status;
}

static void
_knvlinkABMLinkMaskUpdate_GR100_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelNvlink *pKernelNvlink;
    NVLINK_RESILIENCY_INFO *pResiliencyInfo;
    NV_STATUS status = NV_OK;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid GPU pointer\n");
        return;
    }

    pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    if (pResiliencyInfo == NULL)
    {
        return;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    if (pKernelNvlink == NULL)
    {
        portAtomicSetU32(&pResiliencyInfo->bPendingAbmLinkMaskUpdate, 0);
        return;
    }

   status = knvlinkResiliencyEntryFunction(pGpu, pKernelNvlink, pResiliencyInfo, NV_FALSE, NV_FALSE);

   // If there is an existing resiliency flow running, schedule a timer event to retry
   if (status == NV_ERR_BUSY_RETRY && 
        pResiliencyInfo->abmRetryCount < KNVLINK_TRAFFIC_QUIESCE_ACTION_RETRY_COUNT_MAX)
   {
        pResiliencyInfo->abmRetryCount++;
        NV_PRINTF(LEVEL_INFO, "Existing traffic quiesce flow is running, scheduling timer event to retry (retry_count=%d)\n", pResiliencyInfo->abmRetryCount);
        status = tmrEventScheduleRelSec(GPU_GET_TIMER(pGpu), pResiliencyInfo->pAbmRetryEvent, 1U);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to schedule ABM retry (retry_count=%d) status=0x%x\n", pResiliencyInfo->abmRetryCount, status);
            goto exit;
        }

        return;
    }

exit:
   if (status != NV_OK)
   {
        NV_PRINTF(LEVEL_ERROR, "Failed to update ABMLinkMask status=0x%x after %d retries (max %d retries)\n", status,
                    pResiliencyInfo->abmRetryCount, KNVLINK_TRAFFIC_QUIESCE_ACTION_RETRY_COUNT_MAX);
        knvlinkSetAmapUpdateStatus(pGpu,
            GPU_GET_KERNEL_NVLINK(pGpu),
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
   }

    pResiliencyInfo->abmRetryCount = 0;
    portAtomicSetU32(&pResiliencyInfo->bPendingAbmLinkMaskUpdate, 0);
}

NV_STATUS
knvlinkABMLinkMaskUpdate_GR100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool bNeedsRCRecovery
)
{
    NVLINK_RESILIENCY_INFO *pResiliencyInfo;
    NV_STATUS status = NV_OK;

    //
    //  If traffic quiesce is not enabled OR
    // There are no P2P objects & no need to RC channels
    // then use the GB100 implementation
    //
    if (!pKernelNvlink->bTrafficQuiesceEnable ||
        (!knvlinkIsP2PActive_IMPL(pGpu, pKernelNvlink)) ||
        pKernelNvlink->bDisableNonDisruptiveLinkMask)
    {

        return knvlinkABMLinkMaskUpdate_GB100(pGpu, pKernelNvlink, bNeedsRCRecovery);
    }

    pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    if (pResiliencyInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (portAtomicCompareAndSwapU32(&pResiliencyInfo->bPendingAbmLinkMaskUpdate, 1, 0) == 0)
    {
        NV_PRINTF(LEVEL_INFO, "NVLINK ABM link mask update is already in progress, skipping\n");
        return NV_ERR_INVALID_STATE;
    }

    pResiliencyInfo->abmRetryCount = 0;

    // Queue a work item to check P2P with proper locks.
    status = osQueueWorkItem(pGpu,
        _knvlinkABMLinkMaskUpdate_GR100_WORKITEM,
            NULL,
            (OsQueueWorkItemFlags){
                .bLockSema = NV_TRUE,
                .bLockGpus = NV_TRUE,
                .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                .bDontFreeParams = NV_TRUE
            });

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to queue ABMLinkMaskUpdate workitem status=0x%x\n", status);
        portAtomicSetU32(&pResiliencyInfo->bPendingAbmLinkMaskUpdate, 0);
        return status;
    }

    return NV_OK;
}

static NV_STATUS
_knvlinkSerializeResiliencyTimingLogEntry
(
    PRB_ENCODER *pPrbEnc,
    const PRB_FIELD_DESC *pLogFieldDesc,
    const KNVLINK_RESILIENCY_TIMING_LOG_ENTRY *pEntry
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, prbEncNestedStart(pPrbEnc, pLogFieldDesc));
    prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_TIMESTAMP_NS, pEntry->timestampNs);
    prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_ELAPSED_NS, pEntry->elapsedNs);
    prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_TIMEOUT_NS, pEntry->timeoutNs);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_SEQ_ID, pEntry->seqId);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_FLOW, pEntry->flow);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_EVENT, pEntry->event);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_STATUS, pEntry->status);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_QUIESCE_STATE, pEntry->quiesceState);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_UVM_IDLE, pEntry->uvmIdle);
    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCYTIMINGLOG_FLAGS, pEntry->flags);
    return prbEncNestedEnd(pPrbEnc);
}

static NvU32
_knvlinkGetResiliencyRingbufferEntries
(
    KernelNvlink *pKernelNvlink,
    KNVLINK_RESILIENCY_TIMING_LOG_ENTRY *pEntries,
    NvU32 maxEntries,
    NvU32 *pDropCount
)
{
    NvU32 count = 0U;
    NvU64 idx;
    KNVLINK_RESILIENCY_TIMING_LOG_ENTRY *pEntry = NULL;
    NvU64 writeCount;
    NvU64 ringDepth;

    NV_ASSERT_OR_RETURN(pKernelNvlink != NULL, 0U);
    NV_ASSERT_OR_RETURN((pEntries != NULL) && (pDropCount != NULL), 0U);

    writeCount = portAtomicOrU32(&pKernelNvlink->resiliencyTimingWriteCount, 0U);
    ringDepth = ringbufCurrentSize(&pKernelNvlink->resiliencyTimingLog);

    *pDropCount = (writeCount > ringDepth) ? (NvU32)(writeCount - ringDepth) : 0U;

    FOR_EACH_IN_RINGBUF(&pKernelNvlink->resiliencyTimingLog, idx, pEntry)
    {
        if (count >= maxEntries)
        {
            break;
        }

        pEntries[count++] = *pEntry;
    }
    FOR_EACH_END_RINGBUF();

    return count;
}

static
NV_STATUS
_knvlinkDumpEngineFunc
(
    OBJGPU *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE *pNvDumpState,
    void *pvData
)
{
    KNVLINK_RESILIENCY_TIMING_LOG_ENTRY *pEntries = NULL;
    NvU32 entryCount = 0;
    NvU32 dropCount = 0;
    NvU32 i;
    NV_STATUS status = NV_OK;
    NvU8 startingDepth = prbEncNestingLevel(pPrbEnc);
    KernelNvlink *pKernelNvlink = (KernelNvlink *)pvData;

    NV_ASSERT_OR_RETURN((pGpu != NULL) && (pKernelNvlink != NULL), NV_ERR_INVALID_ARGUMENT);

    if (!pKernelNvlink->bTrafficQuiesceEnable)
    {
        return NV_OK;
    }

    pEntries = portMemAllocNonPaged(sizeof(KNVLINK_RESILIENCY_TIMING_LOG_ENTRY) * KNVLINK_RESILIENCY_TIMING_LOG_MAX_ENTRIES);
    if (pEntries == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pEntries, 0, sizeof(KNVLINK_RESILIENCY_TIMING_LOG_ENTRY) * KNVLINK_RESILIENCY_TIMING_LOG_MAX_ENTRIES);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, prbEncNestedStart(pPrbEnc, NVDEBUG_GPUINFO_ENG_KERNEL_NVLINK));

    entryCount = _knvlinkGetResiliencyRingbufferEntries(pKernelNvlink,
        pEntries,
        KNVLINK_RESILIENCY_TIMING_LOG_MAX_ENTRIES,
        &dropCount);

    prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_UNCONTAINED_ERROR_ABORT_TIMEOUT_NS, pKernelNvlink->uncontainedErrorAbortTimeoutNs);
    prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_TRAFFIC_QUIESCE_ABORT_TIMEOUT_NS, pKernelNvlink->trafficQuiesceAbortTimeoutNs);

    for (i = 0; i < entryCount; i++)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
        _knvlinkSerializeResiliencyTimingLogEntry(pPrbEnc,
            NVDEBUG_ENG_KERNELNVLINK_RESILIENCY_TIMING_LOG,
            &pEntries[i]));
    }

    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KERNELNVLINK_RESILIENCY_TIMING_LOG_DROPS, dropCount);

    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    // Squash status to ensure remainder of dump can be collected
    status = NV_OK;

    portMemFree(pEntries);
    return status;
}

/*!
 * @brief Registers Callback to the existing dump framework
 */
 void
 knvlinkDumpCallbackRegister_GR100
 (
     OBJGPU *pGpu,
     KernelNvlink *pKernelNvlink
 )
 {
    NvDebugDump         *pNvd               = GPU_GET_NVD(pGpu);
    NVD_ENGINE_CALLBACK *pEngineCallback    = NULL;

    // Return if pNvd is NULL
    if (pNvd == NULL)
        return;

    // See if already registered (can be if have multiple)
    if ((nvdFindEngine(pGpu, pNvd, NVDUMP_COMPONENT_ENG_KERNEL_NVLINK, &pEngineCallback) == NV_OK) &&
        (pEngineCallback != NULL))
        return;

    // Required to sign up for nvlink callback in Kernel RM in order to dump contents in GSP RM
    nvdEngineSignUp(pGpu,
                    pNvd,
                    _knvlinkDumpEngineFunc,
                    NVDUMP_COMPONENT_ENG_KERNEL_NVLINK,
                    REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _MED) |
                    REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _CPU),
                    (void *)pKernelNvlink);

    // Register the dump callback for the GB100 implementation
    knvlinkDumpCallbackRegister_GB100(pGpu, pKernelNvlink);
 }

 NV_STATUS
 knvlinkSetupResiliencyCallbacks_GR100
 (
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NVLINK_RESILIENCY_INFO *pResiliencyInfo
 )
 {
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pTmr != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pResiliencyInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    status = tmrEventCreate(pTmr,
                            &pResiliencyInfo->pAbmRetryEvent,
                            _knvlinkAbmRetryTimerCallback,
                            NULL,
                            TMR_FLAGS_NONE);     // one-shot schedule
    if (status != NV_OK)
    {
        pResiliencyInfo->pAbmRetryEvent = NULL;
        return status;
    }

    status = tmrEventCreate(pTmr,
                            &pResiliencyInfo->pLfmRetryEvent,
                            knvlinkLfmQuiesceRetryTimerCallback,
                            NULL,
                            TMR_FLAGS_NONE);
    if (status != NV_OK)
    {
        tmrEventDestroy(pTmr, pResiliencyInfo->pAbmRetryEvent);
        pResiliencyInfo->pAbmRetryEvent = NULL;
        pResiliencyInfo->pLfmRetryEvent = NULL;
        return status;
    }
    return NV_OK;
 }

 void
 knvlinkDestroyResiliencyCallbacks_GR100
 (
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NVLINK_RESILIENCY_INFO *pResiliencyInfo
 )
 {
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    if ((pTmr == NULL) || (pResiliencyInfo == NULL))
    {
        return;
    }

    if (pResiliencyInfo->pAbmRetryEvent != NULL)
    {
        tmrEventDestroy(pTmr, pResiliencyInfo->pAbmRetryEvent);
        pResiliencyInfo->pAbmRetryEvent = NULL;
    }

    if (pResiliencyInfo->pLfmRetryEvent != NULL)
    {
        tmrEventDestroy(pTmr, pResiliencyInfo->pLfmRetryEvent);
        pResiliencyInfo->pLfmRetryEvent = NULL;
    }
 }
