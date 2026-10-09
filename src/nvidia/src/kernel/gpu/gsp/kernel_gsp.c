/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "resserv/rs_server.h"

#include "gpu/gsp/kernel_gsp.h"
#include "gpu/falcon/kernel_falcon.h"

#include "kernel/core/thread_state.h"
#include "kernel/core/locks.h"
#include "kernel/diagnostics/gpu_acct.h"
#include "kernel/diagnostics/journal.h"
#include "kernel/diagnostics/nv_debug_dump.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/gsp/gsp_trace_rats_macro.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "kernel/griddisplayless/objgriddisplayless.h"
#include "virtualization/vgpuconfigapi.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "kernel/gpu/disp/kern_disp.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "gpu/external_device/external_device.h"
#include "kernel/platform/platform_request_handler.h"
#include "class/cl2080.h" // NV20_SUBDEVICE_0
#include "ctrl/ctrl2080/ctrl2080nvd.h"
#include "liblogdecode.h"
#include "libelf.h"
#include "nverror.h"
#include "nvrm_registry.h"
#include "nv-firmware.h"
#include "nv-firmware-chip-family-select.h"
#include "nvtypes.h"
#include "nvVer.h"
#include "nvBldVer.h"
#include "gpu/rpc/objrpc.h"
#include "gpu/timer/objtmr.h"
#include "os/os.h"
#include "gpu/disp/dispsw.h"
#include "kernel/gpu/timed_sema.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_version.h"
#include "kernel/gpu/pmu/kern_pmu.h"
#include "gpu/perf/kern_perf.h"
#include "core/locks.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/gr/fecs_event_list.h"
#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"
#include "gpu/fsp/kern_fsp.h"
#include "g_all_dcl_pb.h"
#include "lib/protobuf/prb.h"

#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

#define RPC_MESSAGE_STRUCTURES
#define RPC_MESSAGE_GENERIC_UNION
#include "g_rpc-message-header.h"
#undef RPC_MESSAGE_STRUCTURES
#undef RPC_MESSAGE_GENERIC_UNION

#include "nvdm_format.h"
#include "kernel/core/gmcapi_impl.h"
#include "gmcapi/gmcapi_gsp.h"
#include "gmcapi/gmcapi_engine_types.h"
#include "gmcapi/gmcapi_system_info.h"
#include "gmcapi/gmcapi_system_info_unstable.h"
#include "gpu/gsp/message_queue_priv.h"
#include "cper/gpu_cper.h"
#include "diagnostics/op_event_log.h"
#include "events/gpu/bus/bus_events.h"
#include "events/gpu/gsp/gsp_events.h"
#include "nvoc/event_bus.h"
#include "nvkv/nvkv.h"

#include "gpu/conf_compute/conf_compute.h"

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
#include "diagnostics/instrumentation_manager.h"
#include "ctrl/ctrl208f/ctrl208fucodeinstrumentation.h"
#endif

#include "crashcat/crashcat_report.h"

#include "kernel/gpu/oob/kernel_oob.h"

#include "events/gpu/gsp/gsp_events.h"
#include "nvoc/event_bus.h"

typedef enum
{
    GSP_ERROR_TYPE_NO_ERROR,
    GSP_ERROR_TYPE_RM_HEARTBEAT_TIMEOUT,
    GSP_ERROR_TYPE_LIBOS_HEARTBEAT_TIMEOUT,
    GSP_ERROR_TYPE_RPC_TIMEOUT,
    GSP_ERROR_TYPE_MEMORY_SUBSYSTEM_ERROR,
    GSP_ERROR_TYPE_GPU_CONTAINMENT_ERROR,
    GSP_ERROR_TYPE_COUNT
} GSP_ERROR_TYPE;


struct MIG_CI_UPDATE_CALLBACK_PARAMS
{
    NvU32 execPartCount;
    NvU32 execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NvU32 gfid;
    NvBool bDelete;
};

typedef struct
{
    NvU32 grIdx;
    FECS_ERROR_EVENT_TYPE errorType;
} FECS_ERROR_REPORT;

typedef struct
{
    NvU32 gfid;
    const char *szMemoryId;
    const char *szPrefix;
    const char *elfSectionName;
    NvU64 bufOffset;
    NvU64 bufSize;
    RM_LIBOS_LOG_MEM *taskLogArr;
    NvBool bMergeNvlog;
} LOG_VGPU_SETUP_PARAMS;

typedef struct
{
    NvU32 expectedFunc;
    NvU32 expectedSequence;
    OBJRPC *pRpc;
    KernelGsp *pKernelGsp;
    NvU32 timeoutUs;
    NvBool bIsFullReport;
    NvBool bIsFatalTimeout;
    NvU32 errorNum;
    GSP_ERROR_TYPE errorType;
    NvU32 errorSubCode;
} GSP_RPC_TIMEOUT_LOG_DATA;

typedef struct
{
    NvU32 expectedFunc;
    NvU32 expectedSequence;
    OBJRPC *pRpc;
    NvU32 timeoutUs;
} GSP_RPC_RPC_DATA;

typedef enum
{
    GSP_TIMEOUT_CHECK_ORIGIN_NONE,
    GSP_TIMEOUT_CHECK_ORIGIN_RPC,
    GSP_TIMEOUT_CHECK_ORIGIN_PERIODIC_HEALTHCHECK,
    GSP_TIMEOUT_CHECK_ORIGIN_COUNT
} GSP_TIMEOUT_CHECK_ORIGIN;


//
// RPC_PARAMS defines the rpc_params pointer and initializes it to the correct
// sub-structure.
//
// RPC_PARAMS intentionally assigns the the latest version structure to the
// versioned rpc_params pointer.  With the -Werror=incompatible-pointer-types
// compiler flag, this checks for mismatched structure versions at compile time.
//
// For example:
//   RPC_PARAMS(free, _v03_00);
// expands to
//   rpc_free_v03_00 *rpc_params = &RPC_HDR->rpc_message_data->free_v;
//
#define RPC_PARAMS(r, v) rpc_##r##v *rpc_params = &RPC_HDR->rpc_message_data->r##_v

static NV_STATUS _kgspInitRpcInfrastructure(OBJGPU *, KernelGsp *);
static void _kgspFreeRpcInfrastructure(OBJGPU *, KernelGsp *);

static NV_STATUS _kgspConstructRpcObject(OBJGPU *, KernelGsp *, MESSAGE_QUEUE_INFO *, OBJRPC **);

static NV_STATUS _kgspRpcSendMessage(OBJGPU *, OBJRPC *, NvU32 *);
static NV_STATUS _kgspRpcRecvPoll(OBJGPU *, OBJRPC *, NvU32, NvU32);
static NV_STATUS _kgspRpcDrainEvents(OBJGPU *, KernelGsp *, NvU32, NvU32, KernelGspRpcEventHandlerContext,
                                     GpuWaitConditionFunc *pConditionFunc, void *pConditionData);
static void      _kgspRpcIncrementTimeoutCountAndRateLimitPrints(OBJGPU *, OBJRPC *);

static NV_STATUS _kgspAllocSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);
static void _kgspFreeSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static NV_STATUS _kgspAllocNotifyOpSharedSurface(OBJGPU *pGpu, KernelGsp *pKernelGsp);
static void _kgspFreeNotifyOpSharedSurface(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static void _kgspStopLogPolling(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static void _kgspFreeBootBinaryImage(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static NV_STATUS _kgspPrepareGspRmBinaryImage(OBJGPU *pGpu, KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static NV_STATUS _kgspCreateSignatureMemdesc(OBJGPU *pGpu, KernelGsp *pKernelGsp,
                                             GSP_FIRMWARE *pGspFw);

static NV_STATUS _kgspFwContainerVerifyVersion(OBJGPU *pGpu, KernelGsp *pKernelGsp,
                                               const void *pElfData, NvU64 elfDataSize,
                                               const char *pNameInMsg);

static NV_STATUS _kgspFwContainerGetSection(OBJGPU *pGpu, KernelGsp *pKernelGsp,
                                            const void *pElfData, NvU64 elfDataSize,
                                            const char *pSectionName,
                                            const void **ppSectionData, NvU64 *pSectionSize);

static NV_STATUS _kgspGetSectionNameForPrefix(OBJGPU *pGpu, KernelGsp *pKernelGsp,
                                              char *pSectionNameBuf, NvLength sectionNameBufSize,
                                              const char *pSectionPrefix);

static NV_STATUS _kgspRpcGspEventFecsError(OBJGPU *, OBJRPC *);

static void _kgspRpcGspEventPmuHalted(OBJGPU *);

static void _kgspRpcGspEventHandleFecsBufferError(NvU32, void *);

static NV_STATUS _kgspRpcGspEventRecoveryAction(OBJGPU *, OBJRPC *);

static NV_STATUS _kgspRpcGspTriggerBugcheck(OBJGPU *, OBJRPC *);

static void _kgspRpcGspUpdateGridDisplaylessParams(OBJGPU *, OBJRPC *);

static void _kgspRpcGspForcedDriverShutdown(OBJGPU *);

static void _kgspDumpRmState(OBJGPU *, KernelGsp *);

static void _kgspInitGpuProperties(OBJGPU *);
static NV_STATUS _kgspDumpEngineFunc(OBJGPU*, PRB_ENCODER*, NVD_STATE*, void*);

static NV_STATUS _kgspDecodeGspStaticInfo(OBJGPU *pGpu, NvU64 *pKVData, NvU64 kvLimit);
static NV_STATUS _kgspEncodeSystemInfo(OBJGPU *pGpu, KernelGsp *pKernelGsp,
                                       NvU64 *pKVData, NvU32 kvLimit, NvU32 *pOutCount);

static NvBool _kgspIsRpcTimeoutFatal(KernelGsp *, OBJRPC *, NvU32);

static NvBool _kgspIsTimeoutClassificationActive(KernelGsp *pKernelGsp);

static void _kgspLogGspTimeout(OBJGPU *, NvU32, NvBool, GSP_TIMEOUT_CHECK_ORIGIN, GSP_ERROR_TYPE, NvU32, void *);

static void _kgspLogRpcSpecificTimeout(OBJGPU *, GSP_RPC_TIMEOUT_LOG_DATA *);

static void _kgspLogHeartbeatSpecificTimeout(OBJGPU *, GSP_ERROR_TYPE, NvBool);

static void _kgspLogGpuContainmentSpecificTimeout(OBJGPU *, NvBool, NvU32);

static void _kgspLogMemorySubsystemSpecificTimeout(OBJGPU *);

static NvBool _kgspLogClassifiedTimeout(OBJGPU *, NvU32, GSP_ERROR_TYPE, NvBool, NvU32);

static void _kgspHandleFatalTimeout(OBJGPU *pGpu, KernelGsp *pKernelGsp, NvU32 errorNum);

static NvU32 _kgspCheckAndLogTimeout(OBJGPU *, KernelGsp *, GSP_TIMEOUT_CHECK_ORIGIN, NvBool *, void *);

static void
_kgspGetActiveRpcDebugData
(
    OBJRPC *pRpc,
    NvU32 function,
    NvU64 *data0,
    NvU64 *data1
)
{
    switch (function)
    {
        // Functions (CPU -> GSP)
        case NV_VGPU_MSG_FUNCTION_GSP_RM_CONTROL:
        {
            rpc_gsp_rm_control_v03_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_rm_control_v;
            *data0 = rpc_params->cmd;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC:
        {
            rpc_gsp_rm_alloc_v03_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_rm_alloc_v;
            *data0 = rpc_params->hClass;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_FREE:
        {
            rpc_free_v03_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->free_v;
            *data0 = rpc_params->params.hObjectOld;
            *data1 = rpc_params->params.hObjectParent;
            break;
        }

        // Events (CPU <- GSP)
        case NV_VGPU_MSG_EVENT_POST_EVENT:
        {
            rpc_post_event_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->post_event_v;
            *data0 = rpc_params->notifyIndex;
            *data1 = rpc_params->data;
            break;
        }
        case NV_VGPU_MSG_EVENT_RC_TRIGGERED:
        {
            rpc_rc_triggered_v17_02 *rpc_params = &rpcGetVgpuMessageData(pRpc)->rc_triggered_v;
            *data0 = rpc_params->nv2080EngineType;
            *data1 = rpc_params->exceptType;
            break;
        }
        case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
        {
            rpc_gsp_lockdown_notice_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_lockdown_notice_v;
            *data0 = rpc_params->bLockdownEngaging;
            *data1 = 0;
            break;
        }
        case NV_VGPU_MSG_EVENT_GSP_POST_NOCAT_RECORD:
        {
            rpc_gsp_post_nocat_record_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_post_nocat_record_v;
            const NV2080CtrlNocatJournalInsertRecord *pRecord =
                (const NV2080CtrlNocatJournalInsertRecord *)&rpc_params->data;
            *data0 = pRecord->recType;
            *data1 = pRecord->errorCode;
            break;
        }

        default:
        {
            *data0 = 0;
            *data1 = 0;
            break;
        }
    }
}

static NV_STATUS
_kgspRpcSanityCheck(OBJGPU *pGpu, KernelGsp *pKernelGsp, OBJRPC *pRpc)
{
    if (pKernelGsp->bFatalError)
    {
        NV_PRINTF(LEVEL_INFO, "GSP crashed, skipping RPC\n");
        //
        // In case of a fatal GSP error, if there was an outstanding RPC at the
        // time, we should have already printed the error for that, so this is a
        // new RPC call...from now on don't bother printing RPC errors anymore,
        // as it can be too noisy and overrun logs.
        //
        pRpc->bQuietPrints = NV_TRUE;
        return NV_ERR_RESET_REQUIRED;
    }
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "GPU in reset, skipping RPC\n");
        //
        // Skipping RPCs leads to numerous asserts and error messages.
        //
        pRpc->bQuietPrints = NV_TRUE;
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }
    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    {
        NV_PRINTF(LEVEL_INFO, "GPU lost, skipping RPC\n");
        return NV_ERR_GPU_IS_LOST;
    }
    if (osIsGpuShutdown(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "GPU shutdown, skipping RPC\n");
        return NV_ERR_GPU_IS_LOST;
    }
    if (!gpuIsGpuFullPowerForPmResume(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "GPU not full power, skipping RPC\n");
        return NV_ERR_GPU_NOT_FULL_POWER;
    }
    if (!gpuCheckSysmemAccess(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "GPU has no sysmem access, skipping RPC\n");
        return NV_ERR_INVALID_ACCESS_TYPE;
    }
    return NV_OK;
}

static void
_kgspAddRpcHistoryEntry
(
    OBJRPC *pRpc,
    RpcHistoryEntry *pHistory,
    NvU32 *pCurrent,
    NvU32 func,
    NvU32 sequence
)
{
    NvU32 entry = *pCurrent = (*pCurrent + 1) % RPC_HISTORY_DEPTH;

    portMemSet(&pHistory[entry], 0, sizeof(pHistory[0]));
    pHistory[entry].function = func;
    pHistory[entry].sequence = sequence;
    pHistory[entry].ts_start = osGetTimestamp();

    _kgspGetActiveRpcDebugData(pRpc, func,
                               &pHistory[entry].data[0],
                               &pHistory[entry].data[1]);
}

static void
_kgspCompleteRpcHistoryEntry
(
    RpcHistoryEntry *pHistory,
    NvU32 current
)
{
    NvU32 historyIndex;
    NvU32 historyEntry;

    // Complete the current entry (it should be active)
    // TODO: assert that ts_end == 0 here when continuation record timestamps are fixed
    NV_ASSERT_OR_RETURN_VOID(pHistory[current].ts_start != 0);

    pHistory[current].ts_end = osGetTimestamp();

    //
    // Complete any previous entries that aren't marked complete yet, using the same timestamp
    // (we may not have explicitly waited for them)
    //
    for (historyIndex = 1; historyIndex < RPC_HISTORY_DEPTH; historyIndex++)
    {
        historyEntry = (current + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
        if (pHistory[historyEntry].ts_start != 0 &&
            pHistory[historyEntry].ts_end   == 0)
        {
            pHistory[historyEntry].ts_end = pHistory[current].ts_end;
        }
        else
        {
            break;
        }
    }
}

/*!
 * GSP client RM RPC send routine
 */
NV_STATUS kgspSendRpc_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 nvdmType,
    NvU32 size,
    NvBool bSync
)
{
    NV_STATUS nvStatus;
    OBJRPC *pRpc = pKernelGsp->pRpc;
    NvU32 gpuMaskUnused;

    NV_ASSERT(rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskUnused));

    pKernelGsp->bWatchdogReported = NV_FALSE;

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, _kgspRpcSanityCheck(pGpu, pKernelGsp, pRpc));

    NvU32 func;
    NvU32 sequence;
    if (nvdmType == NVDM_TYPE_RM_RPC)
    {
        rpc_message_header_v *pMsgHdr = rpcGetVgpuMessageHeader(pRpc);
        func = pMsgHdr->function;
        sequence = pMsgHdr->sequence;
    }
    else
    {
        GMCAPI_HEADER *pGmcHdr = rpcGetGmcMessageHeader(pRpc);
        func = GMCAPI_HEADER_COMMAND_ID(pGmcHdr->command);
        sequence = pGmcHdr->sequence;
    }

    nvStatus = GspMsgQueueSendCommand(pRpc->pMessageQueueInfo, pGpu, nvdmType, size);
    if (nvStatus != NV_OK)
    {
        if (nvStatus == NV_ERR_TIMEOUT ||
            nvStatus == NV_ERR_BUSY_RETRY)
        {
            _kgspRpcIncrementTimeoutCountAndRateLimitPrints(pGpu, pRpc);
        }
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
                       "GspMsgQueueSendCommand failed on GPU%d: 0x%x\n",
                       gpuGetInstance(pGpu), nvStatus);
        return nvStatus;
    }

    _kgspAddRpcHistoryEntry(pRpc, pRpc->rpcHistory, &pRpc->rpcHistoryCurrent, func, sequence);

    return bSync ? _kgspRpcRecvPoll(pGpu, pRpc, func, sequence) : NV_OK;
}

static NV_STATUS
_kgspRpcSendMessage
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 *pSequence
)
{
    rpc_message_header_v *vgpuHeader = rpcGetVgpuMessageHeader(pRpc);

    if (pSequence)
        vgpuHeader->sequence = *pSequence = pRpc->sequence++;
    else
        vgpuHeader->sequence = 0;

    return kgspSendRpc(pGpu, GPU_GET_KERNEL_GSP(pGpu), NVDM_TYPE_RM_RPC, vgpuHeader->length, NV_FALSE);
}

NV_STATUS gmcapiLoadAndExecGenericBootloader(GMCAPI_CONTEXT *pCtx)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pCtx->pGpu);
    GspLoadExecGenericBootloaderParams *pParams = (void*)pCtx->pInParams;
    return kgspLoadAndExecuteGenericBootloader_HAL(pCtx->pGpu, pKernelGsp, pParams);
}

NV_STATUS gmcapiLoadAndExecHsBinary(GMCAPI_CONTEXT *pCtx)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pCtx->pGpu);
    GspLoadExecHsBinaryParams *pParams = (void*)pCtx->pInParams;
    return kgspLoadAndExecuteHsBinary_HAL(pCtx->pGpu, pKernelGsp, pParams);
}

static void
_kgspRpcNvlinkAbmFabricHealthMaskUpdateCallback
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelNvlink *pKernelNvlink;

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_ASSERT_OR_RETURN_VOID(pKernelNvlink != NULL);

    NV_ASSERT_OK(knvlinkAbmFabricHealthMaskUpdate(pGpu, pKernelNvlink));
}

static NV_STATUS
_kgspRpcNvlinkAbmFabricHealthMaskUpdate
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    // The callback RPCs to GSP, so it must be done with osQueueWorkItem
    return osQueueWorkItem(pGpu,
                           _kgspRpcNvlinkAbmFabricHealthMaskUpdateCallback,
                           NULL,
                           (OsQueueWorkItemFlags){
                               .bLockSema = NV_TRUE,
                               .bLockGpus = NV_TRUE,
                               .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY
                           });
}

static void
_kgspProcessEccNotifier
(
    OBJGPU *pGpu,
    void   *eventData
)
{
    NV_STATUS          nvStatus     = NV_OK;
    MemoryManager     *pMemoryMgr   = GPU_GET_MEMORY_MANAGER(pGpu);

    if (pMemoryMgr->bEnableDynamicPageOfflining)
    {
        Nv2080EccDbeNotification *pParams = (Nv2080EccDbeNotification*)eventData;
        if ((nvStatus = heapStorePendingBlackList(pGpu, GPU_GET_HEAP(pGpu), pParams->physAddress ,
                                                  pParams->physAddress)) != NV_OK)
        {
            if (nvStatus == NV_ERR_RESET_REQUIRED)
            {
                NV_PRINTF(LEVEL_INFO, "Since we hit the DED on the reserved region, nothing to handle in this code path... \n");
                NV_PRINTF(LEVEL_INFO, "Relying on FBHUB interrupt to kill all the channels and force reset the GPU..\n");
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Dynamically blacklisting the DED page offset failed with, status: %x\n", nvStatus);
                DBG_BREAKPOINT();
            }
        }

    }
}

/*!
 * Receive an event notification from GSP-RM.
 *
 * When an event fires in GSP-RM, osNotifyEvent and osEventNotification check
 * whether the event was originally allocated from client-RM.  If so, they post
 * it to the event queue and take no further action.  Client RM picks up the
 * event here and handles it.
 */
static NV_STATUS
_kgspRpcPostEvent
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_post_event_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->post_event_v;
    EVENTNOTIFICATION *pNotifyList  = NULL;
    EVENTNOTIFICATION *pNotifyEvent = NULL;
    EventApi *pEvent = NULL;
    NvU32 notifyClassId;

    // Get the notification list that contains this event.
    NV_ASSERT_OR_RETURN(CliGetEventInfo(rpc_params->hClient,
        rpc_params->hEvent, &pEvent), NV_ERR_OBJECT_NOT_FOUND);

    NV_ASSERT_OR_RETURN(pEvent->pNotifierShare != NULL, NV_ERR_INVALID_POINTER);

    pNotifyList = pEvent->pNotifierShare->pEventList;
    notifyClassId = objGetClassId(pEvent->pNotifierShare->pNotifier);

    switch (notifyClassId)
    {
        case classId(Subdevice):
        {
            switch (rpc_params->notifyIndex)
            {
                case NV2080_NOTIFIERS_ECC_DBE:
                {
                    _kgspProcessEccNotifier(pGpu, rpc_params->eventData);
                    break;
                }
            }
            break;
        }
    }

    // Send the event.
    if (rpc_params->bNotifyList)
    {
        // Send notification to all matching events on the list.
        if (notifyClassId == classId(Subdevice))
        {
            // Subdevice notifications might require additional handling
            gpuNotifySubDeviceEvent(pGpu, rpc_params->notifyIndex,
                rpc_params->eventData, rpc_params->eventDataSize,
                rpc_params->data, rpc_params->info16);
        }
        else
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                osEventNotificationWithInfo(pGpu,
                    pNotifyList,
                    rpc_params->notifyIndex,
                    rpc_params->data,
                    rpc_params->info16,
                    rpc_params->eventData,
                    rpc_params->eventDataSize));
        }
    }
    else
    {
        // Send event to a specific hEvent.  Find hEvent in the notification list.
        for (pNotifyEvent = pNotifyList; pNotifyEvent; pNotifyEvent = pNotifyEvent->Next)
        {
            if (pNotifyEvent->hEvent == rpc_params->hEvent)
            {
                NV_ASSERT_OK_OR_RETURN(osNotifyEvent(pGpu,
                    pNotifyEvent, 0, rpc_params->data, rpc_params->status, NV_TRUE));
                break;
            }
        }
        NV_ASSERT_OR_RETURN(pNotifyEvent != NULL, NV_ERR_OBJECT_NOT_FOUND);
    }

    return NV_OK;
}

/*!
 * Receive RC notification from GSP-RM.
 *
 * RC error handling ("Channel Teardown sequence") is executed in GSP-RM.
 * Client notifications, OS interaction etc happen in CPU-RM (Kernel RM).
 */
static NV_STATUS
_kgspRpcRCTriggered
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_rc_triggered_v17_02 *rpc_params = &rpcGetVgpuMessageData(pRpc)->rc_triggered_v;

    KernelRc              *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    KernelChannel         *pKernelChannel = NULL;
    KernelFifo            *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32                  status = NV_OK;
    RM_ENGINE_TYPE         rmEngineType = gpuGetRmEngineType(rpc_params->nv2080EngineType);

    // check if there's a PCI-E error pending either in device status or in AER
    krcCheckBusError_HAL(pGpu, pKernelRc);

    if (IS_GFID_PF(rpc_params->gfid))
    {
        CHID_MGR *pChidMgr;

        status = kfifoGetChidMgrFromType(pGpu, pKernelFifo,
                                 ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                 (NvU32)rmEngineType,
                                 &pChidMgr);
        if (status != NV_OK)
            return status;


        if (rpc_params->rcChannelInfoType == RC_CHANNEL_INFO_TYPE_CHANNEL_INFO)
        {
            pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                          pChidMgr,
                                                          rpc_params->chid);

            NV_ASSERT(pKernelChannel == NULL || pKernelChannel->ChID == rpc_params->chid);
        }
        else
        {
            INST_BLOCK_DESC instblk = {0};
            instblk.address  = ((NvU64)rpc_params->instblkAddrHi << 32) |
                                rpc_params->instblkAddrLo;
            instblk.aperture = rpc_params->instblkAperture;
            instblk.gfid     = rpc_params->gfid;

            kfifoConvertInstToKernelChannel_HAL(pGpu, pKernelFifo,
                                                &instblk, &pKernelChannel);
        }

        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                           pKernelChannel != NULL,
                           NV_ERR_INVALID_CHANNEL);
    }

    // Add the RcDiag records we received from GSP-RM to our system wide journal
    {
        OBJSYS   *pSys = SYS_GET_INSTANCE();
        Journal  *pRcDB = SYS_GET_RCDB(pSys);
        RmClient *pClient;

        NvU32 recordSize = rcdbGetOcaRecordSizeWithHeader(pRcDB, RmRcDiagReport);
        NvU32 processId = 0;
        NvU32 owner = RCDB_RCDIAG_DEFAULT_OWNER;

        if (pKernelChannel != NULL)
        {
            pClient = dynamicCast(RES_GET_CLIENT(pKernelChannel), RmClient);
            NV_ASSERT(pClient != NULL);
            if (pClient != NULL)
                processId = pClient->ProcID;
        }

        for (NvU32 i = 0; i < rpc_params->rcJournalBufferSize / recordSize; i++)
        {
            RmRCCommonJournal_RECORD *pCommonRecord =
                (RmRCCommonJournal_RECORD *)((NvU8*)&rpc_params->rcJournalBuffer + i * recordSize);
            RmRcDiag_RECORD *pRcDiagRecord =
                (RmRcDiag_RECORD *)&pCommonRecord[1];

#if defined(DEBUG)
            NV_PRINTF(LEVEL_INFO, "%d: GPUTag=0x%x CPUTag=0x%llx timestamp=0x%llx stateMask=0x%llx\n",
                      i, pCommonRecord->GPUTag, pCommonRecord->CPUTag, pCommonRecord->timeStamp,
                      pCommonRecord->stateMask);
            NV_PRINTF(LEVEL_INFO, "   idx=%d timeStamp=0x%x type=0x%x flags=0x%x count=%d owner=0x%x processId=0x%x\n",
                      pRcDiagRecord->idx, pRcDiagRecord->timeStamp, pRcDiagRecord->type, pRcDiagRecord->flags,
                      pRcDiagRecord->count, pRcDiagRecord->owner, processId);
            for (NvU32 j = 0; j < pRcDiagRecord->count; j++)
            {
                NV_PRINTF(LEVEL_INFO, "     %d: offset=0x%08x tag=0x%08x value=0x%08x attribute=0x%08x\n",
                          j, pRcDiagRecord->data[j].offset, pRcDiagRecord->data[j].tag,
                          pRcDiagRecord->data[j].value, pRcDiagRecord->data[j].attribute);
            }
#endif
            if (rcdbAddRcDiagRecFromGsp(pGpu, pRcDB, pCommonRecord, pRcDiagRecord,
                                        processId, owner) == NULL)
            {
                NV_PRINTF(LEVEL_WARNING, "Lost RC diagnostic record coming from GPU%d GSP: type=0x%x stateMask=0x%llx\n",
                          gpuGetInstance(pGpu), pRcDiagRecord->type, pCommonRecord->stateMask);
            }
        }
    }

    //
    // CPU-RM writes error notifiers for PF channels.
    // GSP-RM may skip notifier writes when KernelChannel is unavailable
    // on the GSP side (channelless path), so CPU-RM handles it here.
    //
    if (pKernelChannel != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(krcErrorSetNotifier(pGpu, pKernelRc,
                                                   pKernelChannel,
                                                   rpc_params->exceptType,
                                                   rmEngineType,
                                                   rpc_params->scope));

        return krcErrorNotifyClients_HAL(pGpu, pKernelRc,
            pKernelChannel,
            rpc_params->exceptType,
            rpc_params->scope,
            rpc_params->partitionAttributionId);
    }

    return NV_OK;
}

/*!
 * Receive Xid notification from GSP-RM
 *
 * Passes Xid errors that are triggered on GSP-RM to nvErrorLog for OS interactions
 * (logging and OS notifications).
 */
static void
_kgspRpcOsErrorLog
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_os_error_log_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->os_error_log_v;

    KernelRc      *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    KernelChannel *pKernelChannel = NULL;
    KernelFifo    *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR      *pChidMgr;

    if (rpc_params->chid != INVALID_CHID)
    {
        pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, rpc_params->runlistId);
        if (pChidMgr != NULL)
        {
            pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                          pChidMgr,
                                                          rpc_params->chid);
        }
    }

    XidContext context = {
        .xid       = rpc_params->exceptType,
        .rootCause = {
            .preemptiveRemovalPreviousXid =
                rpc_params->preemptiveRemovalPreviousXid
        }
    };
    pKernelRc->pPreviousChannelInError = pKernelChannel;
    // Since this is an XID message passed from GSP-RM, don't send the message
    // to OOB again as the GSP has done so already.
    nvErrorLog2_va(pGpu, context, /* oobLogging */ NV_FALSE, "%s", rpc_params->errString);
    pKernelRc->pPreviousChannelInError = NULL;
}

/*!
 * Receives RPC events containing periodic perfmon utilization samples, passing them
 * to GPUACCT for processing.
 */
static void
_kgspRpcGpuacctPerfmonUtilSamples
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];
    rpc_gpuacct_perfmon_util_samples_v1F_0E *rpc_params = &rpcGetVgpuMessageData(pRpc)->gpuacct_perfmon_util_samples_v;

    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS_v1F_0E *src = &rpc_params->params;
    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS        *dest;
    NvU32 i;

    dest = pGpuInstanceInfo->pSamplesParams;
    if (dest == NULL)
    {
        // This RPC event can be received even when the RM hasn't fully started.
        // For instance, CPU RM can take longer than usual to initialize,
        // but the GSP RM sampling timer (a 1 sec interval) is about to tick.
        // In that case, pSamplesParams can not even be allocated by that time.
        // Ignore this RPC event if pSamplesParams has not been allocated yet.
        // See GPUSWSEC-1543 for more info.
        return;
    }

    portMemSet(dest, 0, sizeof(*dest));
    dest->type    = src->type;
    dest->bufSize = src->bufSize;
    dest->count   = src->count;
    dest->tracker = src->tracker;

    for (i = 0; i < NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL_v1F_0E; i++)
    {
        dest->samples[i].base.timeStamp     = src->samples[i].timeStamp;

        dest->samples[i].fb.util            = src->samples[i].fb.util;
        dest->samples[i].fb.procId          = src->samples[i].fb.procId;
        dest->samples[i].fb.subProcessID    = src->samples[i].fb.subProcessID;

        dest->samples[i].gr.util            = src->samples[i].gr.util;
        dest->samples[i].gr.procId          = src->samples[i].gr.procId;
        dest->samples[i].gr.subProcessID    = src->samples[i].gr.subProcessID;

        dest->samples[i].nvenc.util         = src->samples[i].nvenc.util;
        dest->samples[i].nvenc.procId       = src->samples[i].nvenc.procId;
        dest->samples[i].nvenc.subProcessID = src->samples[i].nvenc.subProcessID;

        dest->samples[i].nvdec.util         = src->samples[i].nvdec.util;
        dest->samples[i].nvdec.procId       = src->samples[i].nvdec.procId;
        dest->samples[i].nvdec.subProcessID = src->samples[i].nvdec.subProcessID;
    }

    gpuacctProcessGpuUtil(pGpuInstanceInfo, &dest->samples[0]);
}

/*!
 * Receives RPC events containing current GPU Boost synchronization limits
 * that should be cached and considered in the GPU Boost algorithm and runs
 * the algorithm.
 */
static void
_kgspRpcPerfGpuBoostSyncLimitsCallback
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    KernelPerf *pKernelPerf = GPU_GET_KERNEL_PERF(pGpu);

    rpc_perf_gpu_boost_sync_limits_callback_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->perf_gpu_boost_sync_limits_callback_v;

    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS_v17_00  *src = &rpc_params->params;
    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS          dest;
    NvU32 i;

    dest.flags        = src->flags;
    dest.bBridgeless  = src->bBridgeless;

    for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
    {
        dest.currLimits[i] = src->currLimits[i];
    }

    kperfDoSyncGpuBoostLimits(pGpu, pKernelPerf, &dest);

}

/*!
 * Recieves RPC events containing latest change of bridgeless information
 */
static void
_kgspRpcPerfBridgelessInfoUpdate
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_perf_bridgeless_info_update_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->perf_bridgeless_info_update_v;

    kPerfGpuBoostSyncBridgelessUpdateInfo(pGpu, rpc_params->bBridgeless);
}

static void
_kgspRpcNvlinkFaultUpCallback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_fault_up_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_fault_up_v;

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    knvlinkHandleFaultUpInterrupt_HAL(pGpu, pKernelNvlink, rpc_params->linkId);
}

static void
_kgspRpcNvlinkInbandReceivedData256Callback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_inband_received_data_256_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_inband_received_data_256_v;

    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_256_PARAMS_v17_00 *dest = &rpc_params->params;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(NV_OK == knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvlink, dest->dataSize, dest->data));
}

static void
_kgspRpcNvlinkInbandReceivedData512Callback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_inband_received_data_512_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_inband_received_data_512_v;

    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_512_PARAMS_v17_00 *dest = &rpc_params->params;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(NV_OK == knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvlink, dest->dataSize, dest->data));
}

static void
_kgspRpcNvlinkInbandReceivedData1024Callback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_inband_received_data_1024_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_inband_received_data_1024_v;

    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_1024_PARAMS_v17_00 *dest = &rpc_params->params;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(NV_OK == knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvlink, dest->dataSize, dest->data));
}

static void
_kgspRpcNvlinkInbandReceivedData2048Callback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_inband_received_data_2048_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_inband_received_data_2048_v;

    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_2048_PARAMS_v17_00 *dest = &rpc_params->params;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(NV_OK == knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvlink, dest->dataSize, dest->data));
}

static void
_kgspRpcNvlinkInbandReceivedData4096Callback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_inband_received_data_4096_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_inband_received_data_4096_v;

    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_4096_PARAMS_v17_00 *dest = &rpc_params->params;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(NV_OK == knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvlink, dest->dataSize, dest->data));
}

/*!
 * CPU-RM: Receive GPU Degraded status from GSP
 */
static void
_kgspRpcEventIsGpuDegradedCallback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_is_gpu_degraded_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_is_gpu_degraded_v;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS_v17_00 *dest = &rpc_params->params;

    if(dest->bIsGpuDegraded)
    {
        knvlinkSetDegradedMode(pGpu, pKernelNvlink, dest->linkId);
    }
}

static void
_kgspRpcNvlinkFatalErrorRecoveryCallback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_fatal_error_recovery_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_fatal_error_recovery_v;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_FATAL_ERROR_RECOVERY_PARAMS_v17_00 *pDest = &rpc_params->params;
    NV_ASSERT_OK(knvlinkFatalErrorRecovery(pGpu, pKernelNvlink, pDest->bRecoverable, pDest->bLazy));
}

/*!
 * CPU-RM: Receive GFM state change from GSP
 */
static void
_kgspRpcEventGfmStateChangeCallback
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_gpu_gfm_state_change_v17_00 *rpc_params =
        &rpcGetVgpuMessageData(pRpc)->gpu_gfm_state_change_v;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink != NULL)
    {
        knvlinkHandleGfmStateChange(pGpu, pKernelNvlink,
            rpc_params->params.gfmState);
    }
}

//
// If we have received a special msg from GSP then ack back immediately
// that we are done writing notifiers since we would have already processed the
// other RC msgs that trigger notifier writes before this one.
//
static NV_STATUS
_kgspRpcEccPossibleErrFastPath
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NvU32 status = NV_OK;
    NV_RM_RPC_ECC_NOTIFIER_WRITE_ACK(pGpu, status);
    NV_ASSERT_OK(status);
    return status;
}

static void
_kgspRpcNvlinkTrafficQuiesceNotifyCallback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    rpc_nvlink_traffic_quiesce_notify_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->nvlink_traffic_quiesce_notify_v;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_TRAFFIC_QUIESCE_NOTIFY_PARAMS_v17_00 *pDest = &rpc_params->params;
    NV_ASSERT_OK(knvlinkTrafficQuiesceAction(pGpu, pKernelNvlink, pDest->action));
}

/*!
 * Receive MMU fault queue notification from GSP-RM.
 *
 * Non-replayable fault handling is split between GSP-RM and the UVM driver.
 * GSP-RM copies designated faults to the UVM driver's shadow buffer,
 * and sends a notification.  CPU-RM, in turn, needs to notify the UVM
 * driver (schedule the UVM ISR to be run).
 */
NV_STATUS gmcapiMmuFaultQueued(GMCAPI_CONTEXT *pCtx)
{
    osQueueMMUFaultHandler(pCtx->pGpu);

    return NV_OK;
}

static NV_STATUS
_kgspRpcSimRead
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_sim_read_v1E_01 *rpc_params = &rpcGetVgpuMessageData(pRpc)->sim_read_v;
    if (IS_SIMULATION(pGpu))
    {
        const NvU32 count = rpc_params->index + (rpc_params->count / sizeof(NvU32));
        NvU32 i;

        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        NV_ASSERT_OR_RETURN(rpc_params->count <= sizeof(pKernelGsp->pSimAccessBuf->data), NV_ERR_BUFFER_TOO_SMALL);

        for (i = rpc_params->index; i < count; i++)
        {
            NvU32 data;
            gpuSimEscapeRead(pGpu, rpc_params->path, i, 4, &data);
            pKernelGsp->pSimAccessBuf->data[i] = data;
        }

        pKernelGsp->pSimAccessBuf->seq++;
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS
_kgspRpcSimWrite
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_sim_write_v1E_01 *rpc_params = &rpcGetVgpuMessageData(pRpc)->sim_write_v;
    if (IS_SIMULATION(pGpu))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        gpuSimEscapeWrite(pGpu, rpc_params->path, rpc_params->index, rpc_params->count, rpc_params->data);
        pKernelGsp->pSimAccessBuf->seq++;
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS
_kgspRpcSemaphoreScheduleCallback(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_semaphore_schedule_callback_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->semaphore_schedule_callback_v;
    NV_STATUS status;
    RsClient *pClient;
    Device *pDevice;

    status = serverGetClientUnderLock(&g_resServ, rpc_params->hClient, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, rpc_params->hEvent, &pDevice);
    if (status != NV_OK)
        return status;

    return dispswReleaseSemaphoreAndNotifierFill(pGpu,
                                                 rpc_params->GPUVA,
                                                 rpc_params->hVASpace,
                                                 rpc_params->ReleaseValue,
                                                 rpc_params->Flags,
                                                 rpc_params->completionStatus,
                                                 pDevice);
}

static NV_STATUS
_kgspRpcTimedSemaphoreRelease(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_timed_semaphore_release_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->timed_semaphore_release_v;
    NV_STATUS status;
    RsClient *pClient;
    Device *pDevice;

    status = serverGetClientUnderLock(&g_resServ, rpc_params->hClient, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, rpc_params->hDevice, &pDevice);
    if (status != NV_OK)
        return status;

    return tsemaRelease_HAL(pGpu,
                            rpc_params->semaphoreVA,
                            rpc_params->notifierVA,
                            rpc_params->hVASpace,
                            rpc_params->releaseValue,
                            rpc_params->completionStatus,
                            pDevice);
}


static NV_STATUS
_kgspRpcUcodeLibosPrint
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_ucode_libos_print_v1E_08 *rpc_params = &rpcGetVgpuMessageData(pRpc)->ucode_libos_print_v;

    // Check ucodes registered with the libos print mechanism
    switch (rpc_params->ucodeEngDesc)
    {
        case ENG_PMU:
        {
            KernelPmu *pKernelPmu = GPU_GET_KERNEL_PMU(pGpu);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelPmu != NULL, NV_ERR_OBJECT_NOT_FOUND);

            kpmuLogBuf(pGpu, pKernelPmu,
                       rpc_params->libosPrintBuf, rpc_params->libosPrintBufSize);

            return NV_OK;
        }
        default:
            NV_ASSERT_FAILED("Attempting to use libos prints with an unsupported ucode!\n");
            return NV_ERR_NOT_SUPPORTED;
    }
}

static NV_STATUS
_kgspRpcGspVgpuConfig
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_vgpu_config_event_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->vgpu_config_event_v;

    NV_ASSERT_OR_RETURN(rpc_params->notifyIndex < NVA081_NOTIFIERS_MAXCOUNT,
                        NV_ERR_INVALID_ARGUMENT);

    CliNotifyVgpuConfigEvent(pGpu, rpc_params->notifyIndex);

    return NV_OK;
}

static NV_STATUS
_kgspRpcGspExtdevIntrService
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_extdev_intr_service_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->extdev_intr_service_v;

    extdevGsyncService(pGpu, rpc_params->lossRegStatus, rpc_params->gainRegStatus, rpc_params->miscRegStatus, rpc_params->rmStatus);

    return NV_OK;
}

static void
_kgspRpcMigCiConfigUpdateCallback
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    struct MIG_CI_UPDATE_CALLBACK_PARAMS * pParams = (struct MIG_CI_UPDATE_CALLBACK_PARAMS *)pArgs;

    kmigmgrUpdateCiConfigForVgpu(pGpu, pKernelMIGManager,
                                 pParams->execPartCount, pParams->execPartId,
                                 pParams->gfid, pParams->bDelete);

    return;
}

static NV_STATUS
_kgspRpcMigCiConfigUpdate
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status;
    struct MIG_CI_UPDATE_CALLBACK_PARAMS *pParams;

    rpc_vgpu_gsp_mig_ci_config_v21_03 *rpc_params = &rpcGetVgpuMessageData(pRpc)->vgpu_gsp_mig_ci_config_v;

    NV_ASSERT_OR_RETURN(rpc_params->execPartCount <= NVC637_CTRL_MAX_EXEC_PARTITIONS,
                        NV_ERR_INVALID_ARGUMENT);

    pParams = portMemAllocNonPaged(sizeof(struct MIG_CI_UPDATE_CALLBACK_PARAMS));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pParams->execPartCount = rpc_params->execPartCount;
    portMemCopy(pParams->execPartId, (sizeof(NvU32) * rpc_params->execPartCount),
                rpc_params->execPartId, (sizeof(NvU32) * rpc_params->execPartCount));
    pParams->gfid = rpc_params->gfid;
    pParams->bDelete = rpc_params->bDelete;

    status = osQueueWorkItem(pGpu,
                             _kgspRpcMigCiConfigUpdateCallback,
                             (void *)pParams,
                             (OsQueueWorkItemFlags){
                                 .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                 .bLockGpus = NV_TRUE});
    if (status != NV_OK)
    {
        portMemFree(pParams);
    }

    return status;
}

static void
_kgspRpcGspUpdateTrace
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
#if KERNEL_GSP_TRACING_RATS_ENABLED
    rpc_update_gsp_trace_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->update_gsp_trace_v;
    NvU32 i;
    NV_RATS_RECORD *pRatsRecords = (NV_RATS_RECORD*) (&rpc_params->data);
    for (i = 0; i < rpc_params->records; i++)
    {
        gspRatsEventBufferLogRecord(pGpu, &pRatsRecords[i]);
    }
#endif
}

static void
_kgspRpcGspPostNocatRecord
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    OBJSYS                  *pSys = SYS_GET_INSTANCE();
    Journal                 *pRcdb = SYS_GET_RCDB(pSys);
    NOCAT_JOURNAL_PARAMS    newEntry;
    const NV2080CtrlNocatJournalInsertRecord *pRecord = NULL;
    rpc_gsp_post_nocat_record_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_post_nocat_record_v;

    // make a pointer to the record.
    pRecord = (const NV2080CtrlNocatJournalInsertRecord *)&rpc_params->data;

    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.timestamp          = pRecord->timestamp;
    newEntry.recType            = pRecord->recType;
    newEntry.bugcheck           = pRecord->bugcheck;
    newEntry.pSource            = pRecord->source;
    newEntry.subsystem          = pRecord->subsystem;
    newEntry.errorCode          = pRecord->errorCode;
    newEntry.diagBufferLen      = pRecord->diagBufferLen;
    newEntry.pDiagBuffer        = pRecord->diagBuffer;
    newEntry.pFaultingEngine    = pRecord->faultingEngine;
    newEntry.tdrReason          = pRecord->tdrReason;

    (void)rcdbNocatInsertNocatError(pGpu, &newEntry);
    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RPC_INSERT_RECORDS_IDX]++;
}

static NV_STATUS
_kgspRpcEventPlatformRequestHandlerStateSyncCallback
(
    OBJGPU* pGpu,
    OBJRPC* pRpc
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    PlatformRequestHandler* pPlatformRequestHandler
                 = SYS_GET_PFM_REQ_HNDLR(pSys);

    rpc_pfm_req_hndlr_state_sync_callback_v21_04 *rpc_params = &rpcGetVgpuMessageData(pRpc)->pfm_req_hndlr_state_sync_callback_v;

    NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS_v21_04  *src = &rpc_params->params;
    NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS          dst = { 0 };

    dst.flags         = src->flags;
    dst.syncData.type = src->syncData.type;

    // Copy in the rpc data
    switch (src->syncData.type)
    {
        case NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_TYPE_SMBPBI:
        {
            dst.syncData.data.smbpbi.sensorId =
                src->syncData.data.smbpbi.sensorId;
            dst.syncData.data.smbpbi.limit =
                src->syncData.data.smbpbi.limit;
            break;
        }
        default:
        {
            // Nothing for now
            break;
        }
    }

    pfmreqhndlrStateSync(pPlatformRequestHandler, pGpu, &dst);
    return NV_OK;
}

static void
_kgspRpcGspLockdownNotice
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    rpc_gsp_lockdown_notice_v17_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->gsp_lockdown_notice_v;

    //
    // While the GSP is in lockdown, we cannot access some of its registers,
    // including interrupt status and control. We shouldn't receive any more
    // SWGEN0 interrupts while the core is in lockdown.
    //
    pKernelGsp->bInLockdown = rpc_params->bLockdownEngaging;

    NV_PRINTF(LEVEL_INFO, "GSP lockdown %s\n",
              pKernelGsp->bInLockdown ? "engaged" : "disengaged");
}

static
const char *_getRpcName
(
    NvU32 id
)
{
    static const char *rpcName[] =
        {
            #define X(UNIT, a, VAL) #a,
            #define E(a, VAL) #a,
            #undef _RPC_GLOBAL_ENUMS_H_
            #include "vgpu/rpc_global_enums.h"
            #undef X
            #undef E
        };

    if (id < NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS)
    {
        return rpcName[id];
    }
    else if ((id > NV_VGPU_MSG_EVENT_FIRST_EVENT) && (id < NV_VGPU_MSG_EVENT_NUM_EVENTS))
    {
        NvU32 index = id - (NV_VGPU_MSG_EVENT_FIRST_EVENT - NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS) + 1;
        return rpcName[index];
    }

    return "Unknown";
}

/*!
 * GSP client process RPC events
 */
static void
_kgspProcessRpcEvent
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    KernelGspRpcEventHandlerContext rpcHandlerContext
)
{
    rpc_message_header_v *pMsgHdr = rpcGetVgpuMessageHeader(pRpc);
    NV_STATUS nvStatus = NV_OK;
    NvU32 event = pMsgHdr->function;

    NV_PRINTF(LEVEL_INFO, "received event from GPU%d: 0x%x (%s) status: 0x%x size: %d\n",
              gpuGetInstance(pGpu), event, _getRpcName(event), pMsgHdr->rpc_result, pMsgHdr->length);

    _kgspAddRpcHistoryEntry(pRpc, pRpc->rpcEventHistory, &pRpc->rpcEventHistoryCurrent, pMsgHdr->function, pMsgHdr->sequence);

    /*
     * Shortlist of RPC's that have been manually screened to be safe without the API lock
     * that are called during GSP bootup
     */
    if ((rpcHandlerContext == KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL_BOOTUP) &&
        (!rmapiLockIsOwner()))
    {
        switch(pMsgHdr->function)
        {
            case NV_VGPU_MSG_EVENT_UCODE_LIBOS_PRINT:
            case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
            case NV_VGPU_MSG_EVENT_GSP_POST_NOCAT_RECORD:
            case NV_VGPU_MSG_EVENT_OS_ERROR_LOG:
            case NV_VGPU_MSG_EVENT_PFM_REQ_HNDLR_STATE_SYNC_CALLBACK:
                break;
            default:
                NV_PRINTF(LEVEL_ERROR, "Attempted to process RPC event from GPU%d: 0x%x (%s) during bootup without API lock\n",
                        gpuGetInstance(pGpu), event, _getRpcName(event));
                NV_ASSERT(0);
                goto done;
        }
    }

    switch(event)
    {
        case NV_VGPU_MSG_EVENT_POST_EVENT:
            nvStatus = _kgspRpcPostEvent(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_RC_TRIGGERED:
            nvStatus = _kgspRpcRCTriggered(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_ECC_POSSIBLE_ERR_FAST_PATH:
            nvStatus = _kgspRpcEccPossibleErrFastPath(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_SIM_READ:
            nvStatus = _kgspRpcSimRead(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_SIM_WRITE:
            nvStatus = _kgspRpcSimWrite(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_OS_ERROR_LOG:
            _kgspRpcOsErrorLog(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_GPUACCT_PERFMON_UTIL_SAMPLES:
            _kgspRpcGpuacctPerfmonUtilSamples(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_PERF_GPU_BOOST_SYNC_LIMITS_CALLBACK:
            _kgspRpcPerfGpuBoostSyncLimitsCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_PERF_BRIDGELESS_INFO_UPDATE:
            _kgspRpcPerfBridgelessInfoUpdate(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_SEMAPHORE_SCHEDULE_CALLBACK:
            _kgspRpcSemaphoreScheduleCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_TIMED_SEMAPHORE_RELEASE:
            _kgspRpcTimedSemaphoreRelease(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_FAULT_UP:
            _kgspRpcNvlinkFaultUpCallback(pGpu, pRpc);
             break;

        case NV_VGPU_MSG_EVENT_NVLINK_INBAND_RECEIVED_DATA_256:
            _kgspRpcNvlinkInbandReceivedData256Callback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_INBAND_RECEIVED_DATA_512:
            _kgspRpcNvlinkInbandReceivedData512Callback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_INBAND_RECEIVED_DATA_1024:
            _kgspRpcNvlinkInbandReceivedData1024Callback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_INBAND_RECEIVED_DATA_2048:
            _kgspRpcNvlinkInbandReceivedData2048Callback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_INBAND_RECEIVED_DATA_4096:
            _kgspRpcNvlinkInbandReceivedData4096Callback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_FATAL_ERROR_RECOVERY:
            _kgspRpcNvlinkFatalErrorRecoveryCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_IS_GPU_DEGRADED :
            _kgspRpcEventIsGpuDegradedCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_TRAFFIC_QUIESCE_NOTIFY:
            _kgspRpcNvlinkTrafficQuiesceNotifyCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_GPU_GFM_STATE_CHANGE:
            _kgspRpcEventGfmStateChangeCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_UCODE_LIBOS_PRINT:
            nvStatus = _kgspRpcUcodeLibosPrint(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_VGPU_CONFIG:
            nvStatus = _kgspRpcGspVgpuConfig(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_EXTDEV_INTR_SERVICE:
            nvStatus = _kgspRpcGspExtdevIntrService(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_PFM_REQ_HNDLR_STATE_SYNC_CALLBACK:
            nvStatus = _kgspRpcEventPlatformRequestHandlerStateSyncCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_MIG_CI_CONFIG_UPDATE:
            nvStatus = _kgspRpcMigCiConfigUpdate(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
            _kgspRpcGspLockdownNotice(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_UPDATE_GSP_TRACE:
            _kgspRpcGspUpdateTrace(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_GSP_POST_NOCAT_RECORD:
            _kgspRpcGspPostNocatRecord(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_FECS_ERROR:
            nvStatus = _kgspRpcGspEventFecsError(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_PMU_HALTED:
            _kgspRpcGspEventPmuHalted(pGpu);
            break;

        case NV_VGPU_MSG_EVENT_RECOVERY_ACTION:
            nvStatus = _kgspRpcGspEventRecoveryAction(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_TRIGGER_BUGCHECK:
            nvStatus = _kgspRpcGspTriggerBugcheck(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_UPDATE_GRID_DISPLAYLESS_PARAMS:
            _kgspRpcGspUpdateGridDisplaylessParams(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_FORCED_DRIVER_SHUTDOWN:
            _kgspRpcGspForcedDriverShutdown(pGpu);
            break;

        case NV_VGPU_MSG_EVENT_NVLINK_ABM_FABRIC_HEALTH_MASK_UPDATE:
            nvStatus = _kgspRpcNvlinkAbmFabricHealthMaskUpdate(pGpu, pRpc);
            break;

        default:
            //
            // Log, but otherwise ignore unexpected events.
            //
            // We will get here if the previous RPC timed out.  The response
            // eventually comes in as an unexpected event.  The error handling
            // for the timeout should have already happened.
            //
            NV_PRINTF(LEVEL_ERROR, "Unexpected RPC event from GPU%d: 0x%x (%s), sequence: %u\n",
                      gpuGetInstance(pGpu), event, _getRpcName(event), pMsgHdr->sequence);
            break;
    }

    if (nvStatus != NV_OK)
    {
        //
        // Failing to properly handle a specific event does not mean we should stop
        // processing events/RPCs, so print the error and soldier on.
        //
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to process received event 0x%x (%s) from GPU%d: status=0x%x\n",
                  event, _getRpcName(event), gpuGetInstance(pGpu), nvStatus);
    }

done:
    _kgspCompleteRpcHistoryEntry(pRpc->rpcEventHistory, pRpc->rpcEventHistoryCurrent);
}

/*!
 * Receive FECS error notification from GSP
 *
 * FECS error interrupt goes to GSP, but Kernel needs
 * to take action
 */
static NV_STATUS
_kgspRpcGspEventFecsError
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status;
    rpc_fecs_error_v26_02 *rpc_params = &rpcGetVgpuMessageData(pRpc)->fecs_error_v;

    switch (rpc_params->error_type)
    {
        case FECS_ERROR_EVENT_TYPE_BUFFER_RESET_REQUIRED:
        case FECS_ERROR_EVENT_TYPE_BUFFER_FULL:
        {
            FECS_ERROR_REPORT *pErrorReport = portMemAllocNonPaged(sizeof(*pErrorReport));
            NV_ASSERT_OR_RETURN(pErrorReport != NULL, NV_ERR_NO_MEMORY);

            pErrorReport->grIdx = rpc_params->grIdx;
            pErrorReport->errorType = rpc_params->error_type;

            status = osQueueWorkItem(pGpu,
                _kgspRpcGspEventHandleFecsBufferError,
                pErrorReport,
                (OsQueueWorkItemFlags) {
                    .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY,
                    .bLockGpuGroupSubdevice = NV_TRUE});

            if (status != NV_OK)
                portMemFree(pErrorReport);

            break;
        }
        default:
        {
            status = NV_ERR_INVALID_PARAMETER;
            break;
        }
    }

    return status;
}

static void
_kgspRpcGspEventPmuHalted(OBJGPU *pGpu)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    //
    // We received a PMU halted event from GSP-RM.
    // PMU cannot be recovered at this point and the GPU was marked for reset,
    // don't send any more RPCs to avoid future PMU and GSP errors.
    //
    NV_PRINTF(LEVEL_ERROR, "Received signal from GSP that PMU has halted.\n");
    pKernelGsp->bFatalError = NV_TRUE;
}

/*!
 * Receive GPU recovery action notification from GSP
 *
 */
static NV_STATUS
_kgspRpcGspEventRecoveryAction
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status = NV_OK;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    rpc_recovery_action_v28_01 *rpc_params = &rpcGetVgpuMessageData(pRpc)->recovery_action_v;

    switch (rpc_params->type)
    {
        case GPU_RECOVERY_EVENT_TYPE_REFRESH:
            gpuRefreshRecoveryAction(pGpu, NV_FALSE);
            break;
        case GPU_RECOVERY_EVENT_TYPE_GPU_DRAIN_P2P:
            gpuSetRecoveryDrainP2P(pGpu, rpc_params->value);
            break;
        case GPU_RECOVERY_EVENT_TYPE_GPU_BUS_RESET:
            gpuSetRecoveryBusReset(pGpu, rpc_params->value, pGpu->bBlockNewWorkload);
            break;
        case GPU_RECOVERY_EVENT_TYPE_SYS_OS_REBOOT:
            sysSetRecoveryOsRebootRequired(pSys, rpc_params->value);
            break;
        case GPU_RECOVERY_EVENT_TYPE_SYS_SYSTEM_REBOOT:
            sysSetRecoverySystemRebootRequired(pSys, rpc_params->value);
            break;

        default:
            status = NV_ERR_INVALID_PARAMETER;
            break;
    }

    return status;
}

static void
_kgspRpcGspForcedDriverShutdown(OBJGPU *pGpu)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    //
    // We received a forced driver shutdown event from GSP-RM.
    // This means that GSP will be shutting down after a GSP RM error injection test.
    // Do not attempt sending RPCs to GSP after this point, and mark the GPU for reset.
    //
    NV_PRINTF(LEVEL_ERROR,
        "Forcing driver shutdown by error injection test, marking GPU%d for reset!\n",
        gpuGetInstance(pGpu));
    gpuMarkDeviceForReset(pGpu);
    pKernelGsp->bFatalError = NV_TRUE;
}

static NV_STATUS
_kgspRpcGspTriggerBugcheck
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status = NV_OK;
    rpc_trigger_bugcheck_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->trigger_bugcheck_v;
    NV_PRINTF(LEVEL_ERROR,
              "Received signal from GSP to trigger bugcheck! BugCode=0x%x\n",
              rpc_params->bugCode);

    osBugCheck(rpc_params->bugCode);
    return status;
}

static void
_kgspRpcGspUpdateGridDisplaylessParams
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_update_grid_displayless_params_v01_00 *rpc_params = &rpcGetVgpuMessageData(pRpc)->update_grid_displayless_params_v;
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);
    Nv2080HotplugNotification hotplugNotificationParams = { 0 };

    pGridDisplayless->heads.numHeads               = rpc_params->numHeads;
    pGridDisplayless->maxResolution.maxHResolution = rpc_params->maxHResolution;
    pGridDisplayless->maxResolution.maxVResolution = rpc_params->maxVResolution;
    pGridDisplayless->maxPixels                    = rpc_params->maxPixels;

    // Notify NVKMS of the updated displayless params
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_HOTPLUG,
                            &hotplugNotificationParams,
                            sizeof(hotplugNotificationParams), 0, 0);
}

/*!
 * Processes the callback for the FECS buffer error notifications
 *
 * The callback RPCs to GSP, so it must be done with osQueueWorkItem
 */
static void
_kgspRpcGspEventHandleFecsBufferError
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    FECS_ERROR_REPORT *pErrorReport = (FECS_ERROR_REPORT *)pArgs;

    fecsHandleFecsLoggingError(pGpu, pErrorReport->grIdx, pErrorReport->errorType);
}

/*!
 * Handle a single RPC event from GSP unless the event is [an RPC return for] expectedFunc,
 * or there are no events available in the buffer.
 *
 * @return
 *   NV_OK                              if the event is successfully handled.
 *   NV_WARN_NOTHING_TO_DO              if there are no events available.
 *   NV_WARN_MORE_PROCESSING_REQUIRED   if the event is expectedFunc: it is unhandled and in the staging area.
 *   (Another status)                   if event reading fails.
 */
static NV_STATUS
_kgspRpcDrainOneEvent
(
    OBJGPU          *pGpu,
    OBJRPC          *pRpc,
    NvU32            expectedFunc,
    NvU32            expectedSequence,
    KernelGspRpcEventHandlerContext rpcHandlerContext,
    GpuWaitConditionFunc *pConditionFunc,
    void                 *pConditionData
)
{
    NV_STATUS nvStatus;
    NvU32 nvdmType;

    // Issue a memory barrier to ensure we see any queue updates.
    // Note: Without the fence, the CPU may get stuck in an infinite loop
    //       waiting for a message that has already arrived.
    portAtomicMemoryFenceFull();

    nvStatus = GspMsgQueueReceiveStatus(pRpc->pMessageQueueInfo, pGpu, &nvdmType);

    if (nvStatus == NV_OK && nvdmType == NVDM_TYPE_RM_RPC)
    {
        rpc_message_header_v *pMsgHdr = rpcGetVgpuMessageHeader(pRpc);

        if (pMsgHdr->function == expectedFunc &&
            pMsgHdr->sequence == expectedSequence)
        {
            return NV_WARN_MORE_PROCESSING_REQUIRED;
        }

        _kgspProcessRpcEvent(pGpu, pRpc, rpcHandlerContext);
    }
    else if (nvStatus == NV_OK && nvdmType == NVDM_TYPE_GMCAPI)
    {
        GMCAPI_HEADER *pGmcHdr = rpcGetGmcMessageHeader(pRpc);
        const NvU32 cmd = GMCAPI_HEADER_COMMAND_ID(pGmcHdr->command);

        // Don't allow a match if in the case where the expectedFunc
        // is zero.  It's never a valid GMC command ID.
        if (expectedFunc != 0 &&
            cmd == expectedFunc &&
            pGmcHdr->sequence == expectedSequence)
        {
            NV_ASSERT(pGmcHdr->command & GMCAPI_HEADER_COMMAND_FLAGS_RESPONSE);
            return NV_WARN_MORE_PROCESSING_REQUIRED;
        }

        NV_PRINTF(LEVEL_INFO, "Received GMCAPI event 0x%08x with %u bytes\n", cmd, pGmcHdr->size);
        _kgspAddRpcHistoryEntry(pRpc, pRpc->rpcEventHistory, &pRpc->rpcEventHistoryCurrent, cmd, pGmcHdr->sequence);
        nvStatus = gmcapiCommand(pGpu, cmd, pGmcHdr->data, pGmcHdr->size, NULL, NULL);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "GMCAPI event 0x%08x failed, status=0x%08x\n", cmd, nvStatus);
        }
        _kgspCompleteRpcHistoryEntry(pRpc->rpcEventHistory, pRpc->rpcEventHistoryCurrent);

        if(pConditionFunc != NULL)
        {
            // If the callback returns NV_TRUE, then we need to return NV_WARN_MORE_PROCESSING_REQUIRED to break
            // out of the polling loop.
            if(pConditionFunc(pGpu, pConditionData))
            {
                return NV_WARN_MORE_PROCESSING_REQUIRED;
            }
        }
        return NV_OK;
    }

    //
    // We don't expect NV_WARN_MORE_PROCESSING_REQUIRED here.
    // If we get it we need to suppress it to avoid confusing our caller, for whom it has special meaning.
    //
    NV_ASSERT_OR_ELSE(nvStatus != NV_WARN_MORE_PROCESSING_REQUIRED,
        nvStatus = NV_ERR_GENERIC);

    return nvStatus;
}

/*!
 * Handle RPC events from GSP until the event is [an RPC return for] expectedFunc,
 * or there are no events available in the buffer.
 *
 * Also dump GSP logs, and check for severe errors coming from GSP.
 *
 * @return
 *   NV_OK                              if one or more events are handled and there are none left.
 *   NV_WARN_MORE_PROCESSING_REQUIRED   if an expectedFunc event is found: it is unhandled and in the staging area.
 *                                        (Zero or more preceding events were successfully handled.)
 *   (Another status)                   if event reading or processing fails.
 */
static NV_STATUS
_kgspRpcDrainEvents
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      expectedFunc,
    NvU32      expectedSequence,
    KernelGspRpcEventHandlerContext rpcHandlerContext,
    GpuWaitConditionFunc *pConditionFunc,
    void                 *pConditionData
)
{
    NV_STATUS nvStatus = NV_OK;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    while (nvStatus == NV_OK)
    {
        nvStatus = _kgspRpcDrainOneEvent(pGpu, pRpc, expectedFunc, expectedSequence, rpcHandlerContext,
                                         pConditionFunc, pConditionData);
        kgspDumpGspLogs(pKernelGsp, NV_FALSE);
    }

    // If GSP-RM has died, the GPU will need to be reset
    if (!kgspHealthCheck_HAL(pGpu, pKernelGsp))
        return NV_ERR_RESET_REQUIRED;

    if (nvStatus == NV_WARN_NOTHING_TO_DO)
        nvStatus = NV_OK;

    //
    // Re-evaluate the caller-supplied wait condition once per drain cycle.
    // Some conditions (e.g. polling for a HW state bit like the processor
    // suspended mailbox value) are not driven by RPC event arrival, so they
    // must be checked here even when no events were processed.
    //
    if ((nvStatus == NV_OK) && (pConditionFunc != NULL))
    {
        if (pConditionFunc(pGpu, pConditionData))
            nvStatus = NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    return nvStatus;
}

static NvU64
_tsDiffToDuration
(
    NvU64 duration,
    char *pDurationUnitsChar
)
{
    const NvU64 tsFreqUs = osGetTimestampFreq() / 1000000;

    *pDurationUnitsChar = 'u';

    NV_ASSERT_OR_RETURN(tsFreqUs > 0, 0);

    duration /= tsFreqUs;

    // 999999us then 1000ms
    if (duration >= 1000000)
    {
        duration /= 1000;
        *pDurationUnitsChar = 'm';

        // 9999ms then 10s
        if (duration >= 10000)
        {
            duration /= 1000;
            *pDurationUnitsChar = ' '; // so caller can always just append 's'
        }
    }

    return duration;
}

static NvBool
_kgspIsTimestampDuringRecentRpc
(
    OBJRPC *pRpc,
    NvU64 timestamp,
    NvBool bCheckIncompleteRpcsOnly
)
{
    NvU32 historyIndex;
    NvU32 historyEntry;

    for (historyIndex = 0; historyIndex < RPC_HISTORY_DEPTH; historyIndex++)
    {
        historyEntry = (pRpc->rpcHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
        if (pRpc->rpcHistory[historyEntry].function != 0)
        {
            if ((timestamp >= pRpc->rpcHistory[historyEntry].ts_start) &&
                ((pRpc->rpcHistory[historyEntry].ts_end == 0) ||
                 (!bCheckIncompleteRpcsOnly && (timestamp <= pRpc->rpcHistory[historyEntry].ts_end))))
            {
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

static void
_kgspLogRpcHistoryEntry
(
    OBJGPU *pGpu,
    NvU32 errorNum,
    NvU32 historyIndex,
    RpcHistoryEntry *pEntry,
    NvBool lastColumnCondition
)
{
    NvU64 duration;
    char  durationUnitsChar;

    if (pEntry->function != 0)
    {
        duration = (pEntry->ts_end > pEntry->ts_start) ? (pEntry->ts_end - pEntry->ts_start) : 0;
        if (duration)
        {
            duration = _tsDiffToDuration(duration, &durationUnitsChar);

            NV_ERROR_LOG_DATA(pGpu, errorNum,
                              "    %c%-4d %-4d %-21.21s %10u 0x%016llx 0x%016llx 0x%016llx 0x%016llx %6llu%cs %c\n",
                              ((historyIndex == 0) ? ' ' : '-'),
                              historyIndex,
                              pEntry->function,
                              _getRpcName(pEntry->function),
                              pEntry->sequence,
                              pEntry->data[0],
                              pEntry->data[1],
                              pEntry->ts_start,
                              pEntry->ts_end,
                              duration, durationUnitsChar,
                              (lastColumnCondition ? 'y' : ' '));
        }
        else
        {
            NV_ERROR_LOG_DATA(pGpu, errorNum,
                              "    %c%-4d %-4d %-21.21s %10u 0x%016llx 0x%016llx 0x%016llx 0x%016llx          %c\n",
                              ((historyIndex == 0) ? ' ' : '-'),
                              historyIndex,
                              pEntry->function,
                              _getRpcName(pEntry->function),
                              pEntry->sequence,
                              pEntry->data[0],
                              pEntry->data[1],
                              pEntry->ts_start,
                              pEntry->ts_end,
                              (lastColumnCondition ? 'y' : ' '));
        }
    }
}

void
kgspLogRpcDebugInfoToProtobuf
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    KernelGsp *pKernelGsp,
    PRB_ENCODER *pProtobufData
)
{
    const rpc_message_header_v *pMsgHdr = rpcGetVgpuMessageHeader(pRpc);
    NvU64  data[2];
    NV_STATUS status = NV_OK;
    NvU32  historyIndex;
    NvU32  historyEntry;
    RpcHistoryEntry *pEntry = NULL;
    const NvU32 rpcEntriesToLog = (RPC_HISTORY_DEPTH > 8) ? 8 : RPC_HISTORY_DEPTH;
    const NvU64 tsFreqUs = osGetTimestampFreq() / 1000000;

    NV_ASSERT_OR_RETURN_VOID(tsFreqUs > 0);

    prbEncAddUInt32(pProtobufData, GSP_XIDREPORT_GPUINSTANCE, gpuGetInstance(pGpu));

    status = prbEncNestedStart(pProtobufData, GSP_XIDREPORT_RPCDEBUGINFO);

    if (status != NV_OK)
        return;

    _kgspGetActiveRpcDebugData(pRpc, pMsgHdr->function,
                               &data[0], &data[1]);

    pKernelGsp->nocatData.errorCode = data[0];

    status = prbEncNestedStart(pProtobufData, GSP_RPCDEBUGINFO_ACTIVERPC);
    if (status == NV_OK)
    {
        prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_FUNCTION, pMsgHdr->function);
        prbEncAddString(pProtobufData, GSP_RPCENTRY_RPCNAME, _getRpcName(pMsgHdr->function));
        prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA0, data[0]);
        prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA1, data[1]);
        prbEncNestedEnd(pProtobufData);
    }

    status = prbEncNestedStart(pProtobufData, GSP_RPCDEBUGINFO_RPCHISTORYCPUTOGSP);

    if (status == NV_OK)
    {
        for (historyIndex = 0; historyIndex < rpcEntriesToLog; historyIndex++)
        {
            historyEntry = (pRpc->rpcHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
            pEntry = &pRpc->rpcHistory[historyEntry];
            status = prbEncNestedStart(pProtobufData, GSP_RPCHISTORYCPUTOGSP_RPCENTRY);
            if (status == NV_OK)
            {
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_HISTORYINDEX, historyIndex);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_FUNCTION, pEntry->function);
                prbEncAddString(pProtobufData, GSP_RPCENTRY_RPCNAME, _getRpcName(pEntry->function));
                prbEncAddUInt32(pProtobufData, GSP_RPCENTRY_SEQUENCE, pEntry->sequence);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA0, pEntry->data[0]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA1, pEntry->data[1]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_STARTTIMESTAMP, pEntry->ts_start);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_ENDTIMESTAMP, pEntry->ts_end);
                if (pEntry->ts_end > pEntry->ts_start)
                {
                    prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DURATION, (pEntry->ts_end - pEntry->ts_start) / tsFreqUs);
                }
                prbEncNestedEnd(pProtobufData);
            }
        }
        prbEncNestedEnd(pProtobufData);
    }

    status = prbEncNestedStart(pProtobufData, GSP_RPCDEBUGINFO_RPCHISTORYGSPTOCPU);
    if (status == NV_OK)
    {
        for (historyIndex = 0; historyIndex < rpcEntriesToLog; historyIndex++)
        {
            historyEntry = (pRpc->rpcEventHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
            pEntry = &pRpc->rpcEventHistory[historyEntry];
            status = prbEncNestedStart(pProtobufData, GSP_RPCHISTORYGSPTOCPU_RPCENTRY);
            if (status == NV_OK)
            {
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_HISTORYINDEX, historyIndex);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_FUNCTION, pEntry->function);
                prbEncAddString(pProtobufData, GSP_RPCENTRY_RPCNAME, _getRpcName(pEntry->function));
                prbEncAddUInt32(pProtobufData, GSP_RPCENTRY_SEQUENCE, pEntry->sequence);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA0, pEntry->data[0]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA1, pEntry->data[1]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_STARTTIMESTAMP, pEntry->ts_start);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_ENDTIMESTAMP, pEntry->ts_end);
                if (pEntry->ts_end > pEntry->ts_start)
                {
                    prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DURATION, (pEntry->ts_end - pEntry->ts_start) / tsFreqUs);
                }
                prbEncNestedEnd(pProtobufData);
            }
        }
        prbEncNestedEnd(pProtobufData);
    }
    prbEncNestedEnd(pProtobufData);
}

void
kgspLogRpcDebugInfo
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32   errorNum,
    NvBool  bPollingForRpcResponse
)
{
    const rpc_message_header_v *pMsgHdr = rpcGetVgpuMessageHeader(pRpc);
    NvU32  historyIndex;
    NvU32  historyEntry;
    NvU64  activeData[2];
    const NvU32 rpcEntriesToLog = (RPC_HISTORY_DEPTH > 8) ? 8 : RPC_HISTORY_DEPTH;

    _kgspGetActiveRpcDebugData(pRpc, pMsgHdr->function,
                               &activeData[0], &activeData[1]);
    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "GPU%d GSP RPC buffer contains function %d (%s) sequence %u and data 0x%016llx 0x%016llx.\n",
                      gpuGetInstance(pGpu),
                      pMsgHdr->function, _getRpcName(pMsgHdr->function), pMsgHdr->sequence,
                      activeData[0], activeData[1]);

    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "GPU%d RPC history (CPU -> GSP):\n",
                      gpuGetInstance(pGpu));
    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "    entry function                     sequence data0              data1              ts_start           ts_end             duration actively_polling\n");
    for (historyIndex = 0; historyIndex < rpcEntriesToLog; historyIndex++)
    {
        historyEntry = (pRpc->rpcHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
        _kgspLogRpcHistoryEntry(pGpu, errorNum, historyIndex, &pRpc->rpcHistory[historyEntry],
                                ((historyIndex == 0) && bPollingForRpcResponse));
    }

    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "GPU%d RPC event history (CPU <- GSP):\n",
                      gpuGetInstance(pGpu));
    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "    entry function                     sequence data0              data1              ts_start           ts_end             duration during_incomplete_rpc\n");
    for (historyIndex = 0; historyIndex < rpcEntriesToLog; historyIndex++)
    {
        historyEntry = (pRpc->rpcEventHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
        _kgspLogRpcHistoryEntry(pGpu, errorNum, historyIndex, &pRpc->rpcEventHistory[historyEntry],
                                _kgspIsTimestampDuringRecentRpc(pRpc,
                                                                pRpc->rpcEventHistory[historyEntry].ts_start,
                                                                NV_TRUE/*bCheckIncompleteRpcsOnly*/));
    }
}

static void
_kgspLogRatsCrashBuffer
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
#if KERNEL_GSP_TRACING_RATS_ENABLED
    NV_RATS_RECORD *buffer = pKernelGsp->pRatsCrashBufferRaw;
    NvU32 bufferLength = pKernelGsp->ratsCrashBufferSize;

    if (buffer == NULL)
    {
        NV_PRINTF(LEVEL_INFO,"Gsp Trace Crash Buffer is NULL\n");
        return;
    }

    NV_PRINTF(LEVEL_ERROR, "Gsp Trace Crash Buffer\n");
    for (NvU32 i = 0; i < bufferLength; i++)
    {
        NV_PRINTF(LEVEL_ERROR, "%llu,%u,%u,%u,%u,%llu\n",
                  buffer[i].gspRmTrace.timeStamp,
                  buffer[i].gspRmTrace.traceType,
                  buffer[i].seqNo,
                  buffer[i].gspSeqNo,
                  buffer[i].gspRmTrace.threadId,
                  buffer[i].gspRmTrace.info64);
    }
#endif
}

/*!
 * Log slow RPC to nvlog and nocat
 */
static void
_kgspCheckSlowRpc
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RpcHistoryEntry *pHistoryEntry = &pRpc->rpcHistory[pRpc->rpcHistoryCurrent];
    NvU64 duration;
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    const NvU64 tsFreqUs = osGetTimestampFreq() / 1000000;

    NV_ASSERT_OR_RETURN_VOID(tsFreqUs > 0);

    duration = (pHistoryEntry->ts_end - pHistoryEntry->ts_start) / tsFreqUs;

    if (duration > SLOW_RPC_THRESHOLD_US)
    {
        NV_PRINTF(LEVEL_WARNING, "Slow RPC response from GPU%d GSP (%lluus). Function %d (%s) sequence %u (0x%llx 0x%llx).\n",
                    gpuGetInstance(pGpu),
                    duration,
                    pHistoryEntry->function,
                    _getRpcName(pHistoryEntry->function),
                    pHistoryEntry->sequence,
                    pHistoryEntry->data[0],
                    pHistoryEntry->data[1]);

        kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_GSP_RPC_PERF);
        prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCPERF_GPUINSTANCE, gpuGetInstance(pGpu));
        prbEncAddString(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCPERF_MSG, "Slow RPC response from GSP!");

        prbEncNestedStart(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCPERF_ACTIVERPC);
        prbEncAddUInt64(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_FUNCTION, pHistoryEntry->function);
        prbEncAddString(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_RPCNAME, _getRpcName(pHistoryEntry->function));
        prbEncAddUInt64(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_DURATION, duration);
        prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_SEQUENCE, pHistoryEntry->sequence);
        prbEncAddUInt64(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_DATA0, pHistoryEntry->data[0]);
        prbEncAddUInt64(&pKernelGsp->nocatData.nocatBuffer, GSP_RPCENTRY_DATA1, pHistoryEntry->data[1]);
        prbEncNestedEnd(&pKernelGsp->nocatData.nocatBuffer);

        kgspPostNocatData(pGpu, pKernelGsp, osGetTimestamp());
    }
}

static NvBool
_kgspHeartbeatIsInitialized(KernelGsp *pKernelGsp)
{
    return (pKernelGsp->gspRmHeartbeatTimeoutMs != 0) &&
           (pKernelGsp->libosHeartbeatTimeoutMs != 0);
}

static void
_kgspHeartbeatInit(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NV_ASSERT_OR_RETURN_VOID(kgspIsHeartbeatSupported(pGpu));

    NvU64 defaultTimeoutMs = pGpu->timeoutData.defaultus / 1000;

    // Set the GSP-RM heartbeat timeout to 2x the default timeout.
    // This is to ensure that GSP-RM is confidently hung and that there are no slow RPCs.
    // If an RPC times out and the timeout is not detected, the periodic health check will detect it.
    if (pKernelGsp->gspRmHeartbeatTimeoutMs == 0)
    {
        pKernelGsp->gspRmHeartbeatTimeoutMs = defaultTimeoutMs * 2;
    }

    // Libos normally runs at 1ms interval, but there is no timeout on handling DMA request.
    // Since GSP-RM requests the DMA, and GSP-RM should be confined to default timeout, also
    // set libos heartbeat timeout to 1.25 times the default timeout.
    pKernelGsp->libosHeartbeatTimeoutMs = defaultTimeoutMs + ((defaultTimeoutMs / 10) * 3);
}

static void
_kgspHeartbeatDisable(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // Disable heartbeat checking for GSP-RM and Libos
    pKernelGsp->gspRmHeartbeatTimeoutMs = 0;
    pKernelGsp->libosHeartbeatTimeoutMs = 0;
}

static void
_kgspHealthPeriodicHealthCheck(OBJGPU *pGpu, void *pData)
{
    KernelGsp *pKernelGsp = (KernelGsp *)pData;
    NvBool bFatal = NV_FALSE;
    GSP_ERROR_TYPE errorType = GSP_ERROR_TYPE_NO_ERROR ;

    // If there is an active RPC polling or if the GPU
    // is marked for reset, skip this check. The active RPC polling
    // will check for errors appropriately.
    if (pKernelGsp->bPollingForRpcResponse || pKernelGsp->bFatalError)
    {
        return;
    }

    //
    // Skip the heartbeat check while the GPU is in a PM transition
    // (suspend or resume) or after GSP-RM has been unloaded. The mailbox
    // heartbeat values are not advancing in these states, so reading them
    // would falsely trip the timeout logic.
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH) ||
        pKernelGsp->bGspRmUnloaded ||
        pKernelGsp->bGspRmForceUnloaded)
    {
        return;
    }

    NV_PRINTF(LEVEL_INFO, "Performing periodic health check\n");
    // check and handle for any errors
    errorType = _kgspCheckAndLogTimeout(pGpu,
                       pKernelGsp,
                       GSP_TIMEOUT_CHECK_ORIGIN_PERIODIC_HEALTHCHECK,
                       &bFatal, // pFatal
                       NULL); // pData

    if (errorType == GSP_ERROR_TYPE_NO_ERROR)
    {
        pKernelGsp->bHeartbeatTimeoutDumped = NV_FALSE;
    }

    // generate a fatal event if the error is fatal
    if (bFatal && pKernelGsp->gspStallDetection == NV_REG_STR_RM_GSP_STALL_DETECTION_ENABLE)
    {
        _kgspHandleFatalTimeout(pGpu, pKernelGsp, errorType);
    }
}

static NV_STATUS
_kgspRegisterHealthCheckCallback(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // schedule the callback for heartbeat timeout checks
    NV_STATUS status = osSchedule1HzCallback(pGpu, _kgspHealthPeriodicHealthCheck, pKernelGsp, NV_OS_1HZ_REPEAT);
    if (status != NV_OK)
        NV_PRINTF(LEVEL_ERROR, "Failed to schedule heartbeat callback: %d\n", status);

    return status;
}

static NvBool
_kgspIsHeartbeatTimedOut(OBJGPU *pGpu, KernelGsp *pKernelGsp, NvU32 heartbeatType)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    NvU64 currentTimeNs;
    NvU32 currentTimeMs;
    NvU32 heartbeatWithOffsetMs;
    NvBool isTimedOut;
    NvU32 heartbeat;
    NvU32 timeoutMs;
    const char *pSource;

    if (!_kgspHeartbeatIsInitialized(pKernelGsp))
    {
        return NV_FALSE;
    }

    if (heartbeatType == NV_PGSP_MAILBOX_REGISTER_LIBOS_HEARTBEAT)
    {
        pSource = "LibOS";
        timeoutMs = pKernelGsp->libosHeartbeatTimeoutMs;
    }
    else
    {
        pSource = "GSP-RM";
        timeoutMs = pKernelGsp->gspRmHeartbeatTimeoutMs;
    }

    heartbeat = kgspReadMailbox(pGpu, pKernelGsp, heartbeatType);
    PORT_UNREFERENCED_VARIABLE(pSource);

    // Manually calculate system time from PTIMER instead of using actual system time to avoid issues with drift.
    currentTimeNs = tmrGetTime(pGpu, pTmr) + tmrGetPtimerOffsetNs(pGpu, pTmr);
    if (pKernelFsp != NULL && kfspRequiresBug3957833WAR_HAL(pGpu, pKernelFsp))
    {
        currentTimeNs += pTmr->sysTimerOffsetNs;
    }

    // Convert to 32-bit millisecond timestamp to match heartbeat format.
    currentTimeMs = (NvU32)(currentTimeNs / 1000000);
    // Manually account for the system time offset since Libos heartbeat only knows GPU time.
    // GSP-RM heartbeat uses GPU time as well for consistency.
    heartbeatWithOffsetMs = (NvU32)(heartbeat + (pTmr->sysTimerOffsetNs / 1000000));

    // No need to account for overflow when subtracting unsigned integers
    isTimedOut = (currentTimeMs - heartbeatWithOffsetMs) > timeoutMs;
    if (isTimedOut && !pKernelGsp->bHeartbeatTimeoutDumped)
    {
        NV_PRINTF(LEVEL_ERROR, "%s heartbeat timed out, currentTimeMs %u heartbeat %u heartbeatWithOffsetMs %u diff %u timeout %u\n",
            pSource, currentTimeMs, heartbeat, heartbeatWithOffsetMs, currentTimeMs - heartbeatWithOffsetMs, timeoutMs);
    }

    return isTimedOut;
}

NvBool
kgspHeartbeatIsGspRmHeartbeatTimedOut_IMPL(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    return _kgspIsHeartbeatTimedOut(pGpu, pKernelGsp,
                                    NV_PGSP_MAILBOX_REGISTER_GSPRM_HEARTBEAT);
}

NvBool
kgspHeartbeatIsLibosHeartbeatTimedOut_IMPL(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    return _kgspIsHeartbeatTimedOut(pGpu, pKernelGsp,
                                    NV_PGSP_MAILBOX_REGISTER_LIBOS_HEARTBEAT);
}

/*!
 * Determine if timeout classification is enabled and active
 */
static NvBool
_kgspIsTimeoutClassificationActive(KernelGsp *pKernelGsp)
{
    return (pKernelGsp->gspTimeoutClassification == NV_REG_STR_RM_GSP_TIMEOUT_CLASSIFICATION_ENABLE);
}


static void
_kgspLogRpcSpecificTimeout
(
    OBJGPU *pGpu,
    GSP_RPC_TIMEOUT_LOG_DATA *pRpcTimeoutLogData
)
{
    OBJRPC *pRpc = pRpcTimeoutLogData->pRpc;
    RpcHistoryEntry *pHistoryEntry = &pRpc->rpcHistory[pRpc->rpcHistoryCurrent];
    NvU64 ts_end = osGetTimestamp();
    NvU64 duration;
    char  durationUnitsChar;
    NvU32 expectedFunc = pRpcTimeoutLogData->expectedFunc;
    NvU32 expectedSequence = pRpcTimeoutLogData->expectedSequence;
    KernelGsp *pKernelGsp = pRpcTimeoutLogData->pKernelGsp;
    const char *pRpcName;
    NvU64 timestampDelta;
    NvU64 timestampFreq;
    NvU64 elapsedNs;
    NvU64 waitedSeconds;

    NV_ASSERT(expectedFunc == pHistoryEntry->function);

    NV_ASSERT(ts_end > pHistoryEntry->ts_start);
    timestampDelta = ts_end - pHistoryEntry->ts_start;
    timestampFreq = osGetTimestampFreq();
    NV_ASSERT_OR_RETURN_VOID(timestampFreq > 0);
    NV_ASSERT_OR_RETURN_VOID(portSafeMulU64(timestampDelta, 1000000000ULL, &elapsedNs));
    elapsedNs /= timestampFreq;
    duration = _tsDiffToDuration(timestampDelta, &durationUnitsChar);
    waitedSeconds = (durationUnitsChar == 'm') ? (duration / 1000ULL) : duration;
    pRpcName = _getRpcName(expectedFunc);

    if (!_kgspLogClassifiedTimeout(pGpu, pRpcTimeoutLogData->errorNum,
                                   pRpcTimeoutLogData->errorType,
                                   pRpcTimeoutLogData->bIsFatalTimeout,
                                   pRpcTimeoutLogData->errorSubCode))
    {
        if (pRpcTimeoutLogData->errorNum == GSP_RPC_TIMEOUT)
        {
            eventEmit(GspRpcTimeout,
                      pKernelGsp,
                      pRpcTimeoutLogData->bIsFatalTimeout ?
                          OPERATIONAL_EVENT_SEVERITY_FATAL :
                          OPERATIONAL_EVENT_SEVERITY_RECOVERABLE,
                      (NvU64)pRpcTimeoutLogData->timeoutUs * 1000ULL,
                      elapsedNs,
                      gpuGetInstance(pGpu),
                      expectedFunc,
                      pRpcName,
                      expectedSequence,
                      pHistoryEntry->data[0],
                      pHistoryEntry->data[1],
                      waitedSeconds);
        }
    }
    eventbusFlush(pGpu->pEventBus);

    if (!pRpcTimeoutLogData->bIsFullReport)
    {
        return;
    }

    kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_GSP_RPC_TIMEOUT);
    prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_XID, pRpcTimeoutLogData->errorNum);
    kgspLogRpcDebugInfoToProtobuf(pGpu, pRpc, pKernelGsp, &pKernelGsp->nocatData.nocatBuffer);
    kgspPostNocatData(pGpu, pKernelGsp, osGetTimestamp());

    kgspLogRpcDebugInfo(pGpu, pRpc, pRpcTimeoutLogData->errorNum, NV_TRUE);
    osAssertFailed();
}

static void
_kgspLogGpuContainmentSpecificTimeout
(
    OBJGPU *pGpu,
    NvBool bIsFatalTimeout,
    NvU32 containmentCode
)
{
    eventEmit(BusC2cContainment,
              pGpu,
              bIsFatalTimeout ?
                  OPERATIONAL_EVENT_SEVERITY_FATAL :
                  OPERATIONAL_EVENT_SEVERITY_RECOVERABLE,
              containmentCode);
}

static void
_kgspLogMemorySubsystemSpecificTimeout
(
    OBJGPU *pGpu
)
{
    // Xid 175 is already emitted and flushed by MemSysTimeout in
    // kgmmuCheckMemSubsysError_IMPL before this function is reached.
    PORT_UNREFERENCED_VARIABLE(pGpu);
}

static void
_kgspLogHeartbeatSpecificTimeout
(
    OBJGPU *pGpu,
    GSP_ERROR_TYPE errorType,
    NvBool bIsFatalTimeout
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    if (_kgspIsTimeoutClassificationActive(pKernelGsp))
    {
        NvU32 severity = bIsFatalTimeout ?
            OPERATIONAL_EVENT_SEVERITY_FATAL :
            OPERATIONAL_EVENT_SEVERITY_RECOVERABLE;

        if (errorType == GSP_ERROR_TYPE_LIBOS_HEARTBEAT_TIMEOUT)
        {
            eventEmit(GspHeartbeatTimeout,
                      pKernelGsp,
                      GSP_OP_EVENT_CODE_LIBOS_HEARTBEAT_TIMEOUT,
                      "LibOS",
                      severity,
                      (NvU64)pKernelGsp->libosHeartbeatTimeoutMs * 1000000ULL,
                      0);
        }
        else
        {
            eventEmit(GspHeartbeatTimeout,
                      pKernelGsp,
                      GSP_OP_EVENT_CODE_GSP_RM_HEARTBEAT_TIMEOUT,
                      "GSP-RM",
                      severity,
                      (NvU64)pKernelGsp->gspRmHeartbeatTimeoutMs * 1000000ULL,
                      0);
        }
    }
    else
    {
        const char *pSource = (errorType == GSP_ERROR_TYPE_LIBOS_HEARTBEAT_TIMEOUT) ? "LibOS" : "GSP-RM";
        NV_PRINTF(LEVEL_ERROR,
            "GPU%d: GSP ERROR: %s Heartbeat Timeout detected.\n", gpuGetInstance(pGpu), pSource);
        PORT_UNREFERENCED_VARIABLE(pSource);
    }
}

static NvBool
_kgspLogClassifiedTimeout
(
    OBJGPU *pGpu,
    NvU32 errorNum,
    GSP_ERROR_TYPE errorType,
    NvBool bIsFatalTimeout,
    NvU32 errorSubCode
)
{
    switch (errorNum)
    {
        case GPU_CONTAINMENT_ERROR:
            _kgspLogGpuContainmentSpecificTimeout(pGpu, bIsFatalTimeout, errorSubCode);
            return NV_TRUE;
        case MEMORY_SUBSYSTEM_ERROR:
            _kgspLogMemorySubsystemSpecificTimeout(pGpu);
            return NV_TRUE;
        case GSP_ERROR:
            _kgspLogHeartbeatSpecificTimeout(pGpu, errorType, bIsFatalTimeout);
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

/*
 * Purpose:
 *   _kgspLogGspTimeout is responsible for logging and reporting timeout events detected on the GSP (GPU System Processor).
 *   It is the single entry point for both RPC-originated timeouts and timeouts detected by the periodic health check.
 *   Based on the timeout origin, it picks an appropriate specialized logger (RPC, memory subsystem, or heartbeat) and
 *   then orchestrates the shared "full report" path (build info, RM state dump, core dumps, etc.) that aids root cause
 *   analysis. For RPC origins, repeat timeouts are abbreviated to avoid spamming the logs.
 *
 *
 */
static void
_kgspLogGspTimeout
(
    OBJGPU *pGpu,
    NvU32 errorNum,
    NvBool bIsFatalTimeout,
    GSP_TIMEOUT_CHECK_ORIGIN origin,
    GSP_ERROR_TYPE errorType,
    NvU32 errorSubCode,
    void *pData
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    KernelFalcon *pKernelFlcn = staticCast(pKernelGsp, KernelFalcon);
    GSP_RPC_RPC_DATA *pRpcData = NULL;
    NvBool bFullReport;

    //
    // Derive bFullReport based on origin. For RPC, abbreviate after the first
    // timeout to keep the logs from being spammed with the same information.
    // For the periodic health check, produce a full report for fatal timeouts
    // unconditionally. For non-fatal heartbeat timeouts (classification disabled),
    // dump only on the first detection; back-to-back detections are suppressed.
    // The flag resets when the heartbeat recovers.
    //
    if (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC)
    {
        pRpcData = (GSP_RPC_RPC_DATA *)pData;
        bFullReport = bIsFatalTimeout || (pRpcData->pRpc->timeoutCount == 1);
    }
    else
    {
        if (bIsFatalTimeout)
        {
            bFullReport = NV_TRUE;
        }
        else if (errorNum == GSP_ERROR)
        {
            bFullReport = !pKernelGsp->bHeartbeatTimeoutDumped;
            pKernelGsp->bHeartbeatTimeoutDumped = NV_TRUE;
        }
        else
        {
            bFullReport = NV_TRUE;
        }
    }

    if (bFullReport)
    {
        const char *pClassification;
        switch (errorNum)
        {
            case GPU_CONTAINMENT_ERROR:
                pClassification = "GPU containment error";
                break;
            case MEMORY_SUBSYSTEM_ERROR:
                pClassification = "Memory subsystem error";
                break;
            case GSP_ERROR:
                pClassification = "GSP heartbeat timeout";
                break;
            case GSP_RPC_TIMEOUT:
                pClassification = "GSP-RM is slow";
                break;
            default:
                pClassification = "Unknown";
                break;
        }
        PORT_UNREFERENCED_VARIABLE(pClassification);
        NV_PRINTF(LEVEL_ERROR,
                  "*** GSP Timeout: %s (origin=%s) ***\n",
                  pClassification,
                  (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC) ? "RPC" : "HealthCheck");

        kgspPrintGspBinBuildId(pGpu, pKernelGsp);
    }

    gpuReportGspFmcErrorCode_HAL(pGpu);

    //
    // There are 2 places we check for errors: the RPC handler and the periodic
    // healthcheck. RPC-originated timeouts must be logged via the RPC-specific
    // logger; otherwise we log the appropriate timeout based on the error number.
    //
    if (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC)
    {
        GSP_RPC_TIMEOUT_LOG_DATA rpcTimeoutLogData;

        rpcTimeoutLogData.pRpc             = pRpcData->pRpc;
        rpcTimeoutLogData.expectedFunc     = pRpcData->expectedFunc;
        rpcTimeoutLogData.expectedSequence = pRpcData->expectedSequence;
        rpcTimeoutLogData.pKernelGsp       = pKernelGsp;
        rpcTimeoutLogData.timeoutUs        = pRpcData->timeoutUs;
        rpcTimeoutLogData.errorNum         = errorNum;
        rpcTimeoutLogData.errorType        = errorType;
        rpcTimeoutLogData.bIsFullReport    = bFullReport;
        rpcTimeoutLogData.bIsFatalTimeout  = bIsFatalTimeout;
        rpcTimeoutLogData.errorSubCode     = errorSubCode;

        _kgspLogRpcSpecificTimeout(pGpu, &rpcTimeoutLogData);
    }
    else
    {
        if (bFullReport)
        {
            if (!_kgspLogClassifiedTimeout(pGpu, errorNum, errorType,
                                           bIsFatalTimeout, errorSubCode))
            {
                NV_PRINTF(LEVEL_ERROR, "GSP ERROR: Unknown error number %d\n", errorNum);
            }
            eventbusFlush(pGpu->pEventBus);
        }
    }

    if (!bFullReport)
        return;

    //
    // Always dump RPC/event history regardless of origin. For RPC-originated
    // timeouts this is also logged by _kgspLogRpcSpecificTimeout, but the
    // periodic health check path was missing it entirely (Bug 6242724).
    //
    if (origin != GSP_TIMEOUT_CHECK_ORIGIN_RPC)
    {
        kgspLogRpcDebugInfo(pGpu, pKernelGsp->pRpc, errorNum,
                            pKernelGsp->bPollingForRpcResponse);
    }

    _kgspDumpRmState(pGpu, pKernelGsp);

    _kgspLogRatsCrashBuffer(pGpu, pKernelGsp);
    kflcnCoreDumpNondestructive(pGpu, pKernelFlcn, 2);
    kgspDumpMailbox_HAL(pGpu, pKernelGsp);

    //
    // Dump registers / core state
    // On production boards, ICD dump cannot be done because halt is final.
    //
    if (bIsFatalTimeout)
    {
        if (pKernelGsp->gspStallDetection == NV_REG_STR_RM_GSP_STALL_DETECTION_ENABLE)
            kflcnCoreDumpDestructive(pGpu, pKernelFlcn);
    }

    NV_PRINTF(LEVEL_ERROR, "*** End GSP Timeout ***\n");
}

static void
_kgspLogRpcSanityCheckFailure
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 rpcStatus,
    NvU32 expectedFunc,
    NvU32 expectedSequence
)
{
    RpcHistoryEntry *pHistoryEntry = &pRpc->rpcHistory[pRpc->rpcHistoryCurrent];

    NV_ASSERT(expectedFunc == pHistoryEntry->function);

    NV_PRINTF(LEVEL_ERROR,
              "GPU%d sanity check failed 0x%x waiting for RPC response from GSP. Expected function %d (%s) sequence %u (0x%llx 0x%llx).\n",
              gpuGetInstance(pGpu),
              rpcStatus,
              expectedFunc,
              _getRpcName(expectedFunc),
              expectedSequence,
              pHistoryEntry->data[0],
              pHistoryEntry->data[1]);

    kgspLogRpcDebugInfo(pGpu, pRpc, GSP_RPC_TIMEOUT, NV_TRUE);
    osAssertFailed();
}

static void
_kgspRpcIncrementTimeoutCountAndRateLimitPrints
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    pRpc->timeoutCount++;

    if ((pRpc->timeoutCount == (pKernelGsp->gspRpcTimeoutGpuResetThreshold + 1)) &&
        (RPC_TIMEOUT_PRINT_RATE_SKIP > 0))
    {
        // make sure we warn Xid and NV_PRINTF/NVLOG consumers that we are rate limiting prints
        if (GPU_GET_KERNEL_RC(pGpu)->bLogEvents)
        {
            portDbgPrintf(
                "NVRM: Rate limiting GSP RPC error prints for GPU at PCI:%04x:%02x:%02x (printing 1 of every %d).  The GPU likely needs to be reset.\n",
                gpuGetDomain(pGpu),
                gpuGetBus(pGpu),
                gpuGetDevice(pGpu),
                RPC_TIMEOUT_PRINT_RATE_SKIP + 1);
        }
        NV_PRINTF(LEVEL_WARNING,
                  "Rate limiting GSP RPC error prints (printing 1 of every %d)\n",
                  RPC_TIMEOUT_PRINT_RATE_SKIP + 1);
    }

    pRpc->bQuietPrints = ((pRpc->timeoutCount > pKernelGsp->gspRpcTimeoutGpuResetThreshold) &&
                          ((pRpc->timeoutCount % (RPC_TIMEOUT_PRINT_RATE_SKIP + 1)) != 0));
}

/*!
 * Log Xid 175 - GSP timeout due to memory subsystem error
 */
static NV_STATUS
_kgspCheckIfMemsysError
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS   status;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    //
    // kgmmuCheckMemSubsysHang_HAL checks if the memory HW is 'stuck' anywhere,
    // by probing it through TLB invalidate, sys membars and check if they timeout.
    //
    status = kgmmuCheckMemSubsysError_HAL(pGpu, pKernelGmmu);

    // ignore memory error print as it will be reported out later
    if (status != NV_OK && status != NV_ERR_MEMORY_ERROR)
    {
        NV_PRINTF(LEVEL_ERROR, "Unexpected error when checking for memory subsystem error: 0x%x \n", status);
    }

    return status;
}

/*
 * GSP Timeout Decision Tree
 * ================================
 *
 *                GSP Timeout/RPC Timeout
 *                         |
 *                         v
 *             +---------------------------+
 *             | HW ERROR?                 |
 *             +---------------------------+
 *                 |                  |
 *            YES  |                  | NO
 *                 v                  v
 *          +-----------+     +---------------------+
 *          | Memory HW |     | LibOS Heartbeat     |
 *          | Error     |     | Timed Out?          |
 *          |           |     +---------------------+
 *          | errorNum= |        |              |
 *          | HW ERROR  |    YES |              | NO
 *          |           |        |              |
 *          |           |        |              |
 *          +-----------+        v              v
 *                       +-----------------+  +---------------------+
 *                       | GSP core        |  | GSP-RM Heartbeat    |
 *                       | unresponsive    |  | Timed Out?          |
 *                       |                 |  +---------------------+
 *                       | errorNum =      |     |              |
 *                       | GSP_ERROR       | YES |              | NO
 *                       +-----------------+     |              |
 *                                               v              v
 *                                       +-----------------+  +-----------------+
 *                                       | GSP-RM          |  | GSP-RM is slow  |
 *                                       | unresponsive    |  |                 |
 *                                       |                 |  | errorNum =      |
 *                                       | errorNum =      |  | GSP_RPC_TIMEOUT |
 *                                       | GSP_ERROR       |  +-----------------+
 *                                       +-----------------+
 */

/*
 * _kgspCheckAndLogTimeout
 *
 * Purpose:
 *   This function is responsible for diagnosing and classifying errors or timeouts related to GSP (GPU System Processor)
 *   operation. When a periodic health check or an RPC-related timeout occurs (e.g. missed heartbeat, unresponsive GSP, memory errors),
 *   this function is called to analyze the GPU state, determine the specific error and its severity, and log information needed
 *   for further handling. It also updates the out-parameters to indicate whether the error is fatal, which is used elsewhere to
 *   trigger GPU recovery flows.
 *
 * Flow Path Context:
 *   This function is called from timeout handling paths such as the periodic health check callback
 *   (_kgspHealthPeriodicHealthCheck), or when an RPC or heartbeat timeout is detected. If it detects an error,
 *   it will then call _kgspLogGspTimeout to log the error and return the error.
 */
static NvU32
_kgspCheckAndLogTimeout
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_TIMEOUT_CHECK_ORIGIN origin,
    NvBool *pFatal,
    void *pData
)
{
    NvU32 errorNum = GSP_ERROR_TYPE_NO_ERROR;
    GSP_ERROR_TYPE errorType = GSP_ERROR_TYPE_NO_ERROR;
    NvU32 intrStatus;
    NvU32 containmentCode = 0;
    KernelFalcon *pKernelFalcon;

    *pFatal = NV_FALSE;

    //
    // Special case: GSP fatal HW error (e.g. poison). We handle this block separately and return
    // immediately because it performs its own side effects (ECC log, gpuMarkDeviceForReset) and
    // must not be subject to the normal timeout classification override below.
    //
    pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    intrStatus = kflcnGetPendingHostInterrupts(pGpu, pKernelFalcon);

    if (kgspCheckGspPoisonError_HAL(pGpu, pKernelGsp, intrStatus))
    {
        //
        // gpuCheckEccCounts_HAL should be able to detect and log Xid 140 (UNRECOVERABLE_ECC_ERROR_ESCAPE)
        // but the functions it uses to get ecc error counts aren't wired to the right HAL for GB100+,
        // bug 5816620 will fix it. Revisit this function and remove nvErrorLog_va once bug 5816620 is fixed.
        //
        gpuCheckEccCounts_HAL(pGpu, OPERATIONAL_EVENT_SEVERITY_FATAL);
        eventEmit(GspPoison, pGpu);
        eventbusFlush(pGpu->pEventBus);
        *pFatal = NV_TRUE;

        return UNRECOVERABLE_ECC_ERROR_ESCAPE;
    }
    if (kgspCheckGpuContainmentError_HAL(pGpu, pKernelGsp, &containmentCode) != NV_OK)
    {
        *pFatal = NV_TRUE;
        errorNum = GPU_CONTAINMENT_ERROR;
        errorType = GSP_ERROR_TYPE_GPU_CONTAINMENT_ERROR;
    }
    else if (_kgspCheckIfMemsysError(pGpu, pKernelGsp) == NV_ERR_MEMORY_ERROR)
    {
        *pFatal = NV_TRUE;
        errorNum = MEMORY_SUBSYSTEM_ERROR;
        errorType = GSP_ERROR_TYPE_MEMORY_SUBSYSTEM_ERROR;
    }
    else if (kgspHeartbeatIsLibosHeartbeatTimedOut(pGpu, pKernelGsp))
    {
        errorNum = GSP_ERROR;
        errorType = GSP_ERROR_TYPE_LIBOS_HEARTBEAT_TIMEOUT;
    }
    else if (kgspHeartbeatIsGspRmHeartbeatTimedOut(pGpu, pKernelGsp))
    {
        errorNum = GSP_ERROR;
        errorType = GSP_ERROR_TYPE_RM_HEARTBEAT_TIMEOUT;
    }
    else if (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC)
    {
        errorNum = GSP_RPC_TIMEOUT;
        errorType = GSP_ERROR_TYPE_RPC_TIMEOUT;
    }
    else
    {
        return GSP_ERROR_TYPE_NO_ERROR;
    }

    //
    // Decide fatality if not already done above. Every timeout except rate-limited RPC timeouts
    // are logged by the unified logger (_kgspLogGspTimeout) below.
    //
    if (*pFatal)
    {
        if (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC)
        {
            GSP_RPC_RPC_DATA *pRpcData = (GSP_RPC_RPC_DATA *)pData;

            // Honor quiet-prints rate limiting before dispatching to the unified logger.
            if (pRpcData->pRpc->bQuietPrints)
                return errorNum;
        }
    }
    else if (origin == GSP_TIMEOUT_CHECK_ORIGIN_RPC)
    {
        GSP_RPC_RPC_DATA *pRpcData = (GSP_RPC_RPC_DATA *)pData;

        *pFatal = _kgspIsRpcTimeoutFatal(pKernelGsp, pRpcData->pRpc, errorNum);

        // If the timeout is not fatal, override errorNum to RPC timeout.
        if (!*pFatal)
            errorNum = GSP_RPC_TIMEOUT;

        // Honor quiet-prints rate limiting before dispatching to the unified logger.
        if (pRpcData->pRpc->bQuietPrints)
            return errorNum;
    }
    else // GSP_TIMEOUT_CHECK_ORIGIN_PERIODIC_HEALTHCHECK
    {
        //
        // Any errors detected during the periodic healthcheck are fatal, except
        // that the regkey can override heartbeat failures to be non-fatal.
        //
        if (errorNum == GSP_ERROR && !_kgspIsTimeoutClassificationActive(pKernelGsp))
            *pFatal = NV_FALSE;
        else
            *pFatal = NV_TRUE;
    }

    _kgspLogGspTimeout(pGpu, errorNum, *pFatal, origin, errorType, containmentCode, pData);

    return errorNum;
}

static void
_kgspHandleFatalTimeout(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 errorNum
)
{
    pKernelGsp->bFatalError = NV_TRUE;
    NV_ASSERT_FAILED("GPU marked for reset");
    gpuMarkDeviceForReset(pGpu);

    // For Windows, if TDR is supported, trigger TDR to recover the system.
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_TDR_EVENT))
    {
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_UCODE_RESET, NULL, 0, 0, 0);
    }
    else
    {
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_GPU_UNAVAILABLE, NULL, 0, errorNum, 0);
    }
}

/*!
 * Determine if the timeout is fatal based on the value of NV_REG_STR_RM_GSP_TIMEOUT_CLASSIFICATION
 * and the RPC timeout count. Heartbeat GSP_ERROR is non-fatal when classification is inactive
 * (caller downgrades to GSP_RPC_TIMEOUT), consistent with the periodic healthcheck path.
 */
static NvBool
_kgspIsRpcTimeoutFatal(
    KernelGsp *pKernelGsp,
    OBJRPC *pRpc,
    NvU32 errorNum
)
{
    // Timeout threshold reached - always fatal
    if (pRpc->timeoutCount == pKernelGsp->gspRpcTimeoutGpuResetThreshold)
    {
        return NV_TRUE;
    }

    // Heartbeat timeout (GSP_ERROR): non-fatal when timeout classification is inactive, so the
    // caller can downgrade to GSP_RPC_TIMEOUT (matches the periodic healthcheck path).
    if (errorNum == GSP_ERROR && !_kgspIsTimeoutClassificationActive(pKernelGsp))
    {
        return NV_FALSE;
    }

    // all other errors would be fatal, such as HW errors
    if (errorNum != GSP_RPC_TIMEOUT)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

/*!
 * Thunk to deal with the difference between the OBJRPC recvPoll function pointer
 * our exposed kgspRecvPoll() function.
 */
static NV_STATUS
_kgspRpcRecvPoll
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32   expectedFunc,
    NvU32   expectedSequence
)
{
    return kgspRecvPoll_IMPL(pGpu, GPU_GET_KERNEL_GSP(pGpu), expectedFunc, expectedSequence, NULL, NULL);
}

/*!
 * GSP client RM RPC poll routine
 */
 NV_STATUS
kgspRecvPoll_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      expectedFunc,
    NvU32      expectedSequence,
    GpuWaitConditionFunc *pConditionFunc,
    void                 *pConditionData
)
{
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_STATUS  rpcStatus = NV_OK;
    NV_STATUS  timeoutStatus = NV_OK;
    RMTIMEOUT  timeout;
    NvU32      timeoutUs;
    NvU32      timeoutFlags;
    NvBool     bSlowGspRpc = !IS_SILICON(pGpu);
    NvU32      gpuMaskUnused;

#if defined(GSPRM_HWASAN_ENABLE)
    //
    // TODO HWASAN is SLOW. More investigation is required on where the
    // bottlenecks are and how to uncork them.
    //
    bSlowGspRpc = bSlowGspRpc
        || (expectedFunc == GMCAPI_CMD_GSP_RESUME_DONE)
        || (expectedFunc == GMCAPI_CMD_GSP_INIT);
#endif

    KernelGspRpcEventHandlerContext rpcHandlerContext = KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL;
    if (expectedFunc == GMCAPI_CMD_GSP_RESUME_DONE ||
        expectedFunc == GMCAPI_CMD_GSP_INIT)
    {
        // special case for bootup path without API lock
        rpcHandlerContext = KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL_BOOTUP;
    }
    //
    // We do not allow recursive polling. This can happen if e.g.
    //    1. CPU-RM issues RPC-A to GSP and polls waiting for it to finish
    //    2. While servicing RPC-A, GSP emits an async event back to CPU-RM
    //    3. CPU-RM services the async event and sends another synchronous RPC-B
    //    4. RPC-A response will come first, but CPU-RM is now waiting on RPC-B
    //
    // We don't have a good way to handle this and should just be deferring the
    // second RPC until the first one is done, via e.g. osQueueWorkItem().
    // This assert is meant to catch and loudly fail such cases.
    //
    NV_ASSERT_OR_RETURN(!pKernelGsp->bPollingForRpcResponse, NV_ERR_INVALID_STATE);
    pKernelGsp->bPollingForRpcResponse = NV_TRUE;

    //
    // GSP-RM init in emulation/simulation environment is extremely slow,
    // so need to increment timeout.
    // Apply the timeout extension to other RPCs as well, mostly so that
    // we'll reset the thread state after each RPC, not just while waiting
    // for the INIT_DONE event.
    //
    if (bSlowGspRpc)
    {
        NvU32 timeoutResult;

        // On slow Apollo emulators, GSP-RM init could take more than an hour
        NV_ASSERT(portSafeMulU32(GSP_SCALE_TIMEOUT_EMU_SIM, 1500000, &timeoutResult));
        timeoutUs = timeoutResult;
    }
    else
    {
        NvU32 defaultus = pGpu->timeoutData.defaultus;

        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            // Ensure at least 3.1s for vGPU-GSP before adding leeway (Bug 3928607)
            timeoutUs = NV_MAX(3100 * 1000, defaultus) + (defaultus / 2);
        }
        else
        {
            //
            // We should only ever timeout this when GSP is in really bad state, so if it just
            // happens to timeout on default timeout it should be OK for us to give it a little
            // more time - make this timeout 1.5 of the default to allow some leeway.
            //
            timeoutUs = defaultus + defaultus / 2;
        }
    }

    NV_ASSERT(rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskUnused));

    timeoutFlags = GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE;
    if (pRpc->bQuietPrints)
        timeoutFlags |= GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG;

    gpuSetTimeout(pGpu, timeoutUs, &timeout, timeoutFlags);

    for (;;)
    {
        //
        // Check for GPU timeout, save that information, and then verify if the RPC is completed.
        // Otherwise if the CPU thread goes to sleep immediately after the RPC check, it may result in hitting a timeout.
        //
        timeoutStatus = gpuCheckTimeout(pGpu, &timeout);

        rpcStatus = _kgspRpcDrainEvents(pGpu, pKernelGsp, expectedFunc, expectedSequence, rpcHandlerContext,
                                        pConditionFunc, pConditionData);

        switch (rpcStatus) {
            case NV_WARN_MORE_PROCESSING_REQUIRED:
                // The synchronous RPC response we were waiting for is here
                _kgspCompleteRpcHistoryEntry(pRpc->rpcHistory, pRpc->rpcHistoryCurrent);
                if (!bSlowGspRpc)
                {
                    _kgspCheckSlowRpc(pGpu, pRpc);
                }
                rpcStatus = NV_OK;
                goto done;
            case NV_OK:
                // Check timeout and continue outer loop.
                break;
            default:
                goto done;
        }

        rpcStatus = _kgspRpcSanityCheck(pGpu, pKernelGsp, pRpc);
        if (rpcStatus != NV_OK)
        {
            if (!pRpc->bQuietPrints)
            {
                _kgspLogRpcSanityCheckFailure(pGpu, pRpc, rpcStatus, expectedFunc, expectedSequence);
                pRpc->bQuietPrints = NV_TRUE;
            }
            goto done;
        }

        //
        // Today, we will soldier on if GSP times out. This can cause future issues if the action
        // requested never actually occurs.
        //
        if (timeoutStatus == NV_ERR_TIMEOUT)
        {
            NvU32 errorNum;
            NvBool bIsFatalTimeout;
            rpcStatus = timeoutStatus;
            GSP_RPC_RPC_DATA rpcTimeoutLogData;
            rpcTimeoutLogData.pRpc = pRpc;
            rpcTimeoutLogData.expectedFunc = expectedFunc;
            rpcTimeoutLogData.expectedSequence = expectedSequence;
            rpcTimeoutLogData.timeoutUs = timeoutUs;

            _kgspRpcIncrementTimeoutCountAndRateLimitPrints(pGpu, pRpc);

            errorNum = _kgspCheckAndLogTimeout(pGpu,
                                          pKernelGsp,
                                          GSP_TIMEOUT_CHECK_ORIGIN_RPC,
                                          &bIsFatalTimeout,
                                          &rpcTimeoutLogData);

            if (bIsFatalTimeout && pKernelGsp->gspStallDetection == NV_REG_STR_RM_GSP_STALL_DETECTION_ENABLE)
            {
                _kgspHandleFatalTimeout(pGpu, pKernelGsp, errorNum);
            }

            goto done;
        }
        else if (timeoutStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "gpuCheckTimeout() returned unexpected error (0x%08x)\n",
                      timeoutStatus);
            rpcStatus = timeoutStatus;
            goto done;
        }


        //
        // Check if Gsync requires display interrupt to be serviced inline to
        // reduce stutter.
        // If we have a separate interrupt line, the low latency interrupts will
        // always all get serviced in the top half, and there's no need to
        // check them here.
        //
        if ((pKernelDisplay != NULL) &&
            pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE) &&
            !pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE))
        {
            kdispApplyAggressiveVblankHandlingWar(pGpu, pKernelDisplay);
        }

        {
            osSpinLoop();
        }
    }

    pRpc->timeoutCount = 0;

done:
    pKernelGsp->bPollingForRpcResponse = NV_FALSE;

    if (bSlowGspRpc)
    {
        // Avoid cumulative timeout due to slow RPC
        threadStateResetTimeout(pGpu);
    }

    return rpcStatus;
}

/*!
 * Initialize RPC objects required for interfacing with GSP.
 */
static NV_STATUS
_kgspInitRpcInfrastructure
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS nvStatus = NV_OK;
    MESSAGE_QUEUE_COLLECTION *pMQCollection = NULL;

    nvStatus = GspMsgQueuesInit(pGpu, &pMQCollection);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GspMsgQueueInit failed\n");
        goto done;
    }

    pKernelGsp->pMQCollection = pMQCollection;

    // Init RM RPC object
    nvStatus = _kgspConstructRpcObject(pGpu, pKernelGsp,
                                       &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX],
                                       &pKernelGsp->pRpc);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init task RM RPC infrastructure failed\n");
        goto done;
    }

done:
    if (nvStatus != NV_OK)
    {
        _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    }

    return nvStatus;
}


/*!
 * Initialize stripped down version of RPC infra init for GSP clients.
 */
static NV_STATUS
_kgspConstructRpcObject
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    MESSAGE_QUEUE_INFO *pMQI,
    OBJRPC **ppRpc
)
{
    OBJRPC *pRpc;

    NV_ASSERT_OR_RETURN(pMQI != NULL, NV_ERR_INVALID_ARGUMENT);

    pRpc = initRpcObject(pGpu);
    if (pRpc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "initRpcObject failed\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pRpc->pMessageQueueInfo = pMQI;

    portMemSet(&pRpc->rpcHistory, 0, sizeof(pRpc->rpcHistory));
    pRpc->rpcHistoryCurrent = RPC_HISTORY_DEPTH - 1;
    portMemSet(&pRpc->rpcEventHistory, 0, sizeof(pRpc->rpcEventHistory));
    pRpc->rpcEventHistoryCurrent = RPC_HISTORY_DEPTH - 1;

    pRpc->message_buffer = (NvU32 *)pRpc->pMessageQueueInfo->pRpcMsgBuf;
    pRpc->maxRpcSize = gspMsgQueueGetMaxRpcSize(pRpc->pMessageQueueInfo->queueElementSizeMax,
                                                pRpc->pMessageQueueInfo->queueElementHdrSize);

    rpcSendMessage_FNPTR(pRpc) = _kgspRpcSendMessage;
    rpcRecvPoll_FNPTR(pRpc)    = _kgspRpcRecvPoll;

    *ppRpc = pRpc;

    return NV_OK;
}

static void
_kgspFreeRpcInfrastructure
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    if (pKernelGsp->pRpc != NULL)
    {
        rpcDestroy(pGpu, pKernelGsp->pRpc);
        portMemFree(pKernelGsp->pRpc);
        pKernelGsp->pRpc = NULL;
    }
    GspMsgQueuesCleanup(&pKernelGsp->pMQCollection);
}

/*!
 * Convert init arg name to 64bit id value.
 *
 * @param[in]      name  String representing name of init arg
 */
static NvU64
_kgspGenerateInitArgId(const char *name)
{
    NvU64 id = 0;
    NvU8 c;
    NvU32 i;

    // Convert at most 8 characters from name into id.
    for (i = 0; i < (sizeof(NvU64) / sizeof(NvU8)); ++i)
    {
        c = (NvU8)*name++;
        if (c == '\0')
        {
            break;
        }
        id = (id << 8) | c;
    }

    return id;
}

static void
_kgspUnmapTaskLogBuf(OBJGPU *pGpu, RM_LIBOS_LOG_MEM *pLog)
{
    // release log memory for this task.
    if (pLog->pTaskLogBuffer != NULL)
    {
        memdescUnmapInternal(pGpu, pLog->pTaskLogDescriptor, TRANSFER_FLAGS_NONE);
        pLog->pTaskLogBuffer = NULL;
    }

    if (pLog->pTaskLogDescriptor != NULL)
    {
        memdescFree(pLog->pTaskLogDescriptor);
        memdescDestroy(pLog->pTaskLogDescriptor);
        pLog->pTaskLogDescriptor = NULL;
    }
}

/*!
 * Free vgpu partition LIBOS task logging structures
 */
static void
_kgspFreeLibosVgpuPartitionLoggingStructures
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 gfid
)
{
    RM_LIBOS_LOG_MEM *vgpuLogBuffers[] =
    {
        pKernelGsp->gspPluginInitTaskLogMem,
        pKernelGsp->gspPluginVgpuTaskLogMem,
        pKernelGsp->libosKernelLogMem
    };

    libosLogDestroy(&pKernelGsp->logDecodeVgpuPartition[gfid - 1]);

    // release all the vgpu tasks' log buffer memory
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(vgpuLogBuffers); ++i)
    {
        RM_LIBOS_LOG_MEM *pTaskLog = &vgpuLogBuffers[i][gfid - 1];
        _kgspUnmapTaskLogBuf(pGpu, pTaskLog);
    }
}

/*!
 * Free vgpu partition LIBOS task logging structures
 */
NV_STATUS
kgspFreeVgpuPartitionLogging_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 gfid
)
{
    if (gfid > MAX_PARTITIONS_WITH_GFID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    else
    {
        // Make sure there is no lingering debug output.
        kgspDumpGspLogs(pKernelGsp, NV_FALSE);

        while (!portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 1, 0))
            osSpinLoop();

        _kgspFreeLibosVgpuPartitionLoggingStructures(pGpu, pKernelGsp, gfid);

        portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 0, 1);

        return NV_OK;
    }
}

static NV_STATUS
_setupLogBufferVgpu
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    LOG_VGPU_SETUP_PARAMS logVgpuSetupParams
)
{
    NV_STATUS nvStatus = NV_OK;
    RM_LIBOS_LOG_MEM *pTaskLog = NULL;
    char vm_string[8];

    pTaskLog = &logVgpuSetupParams.taskLogArr[logVgpuSetupParams.gfid - 1];
    NvP64 pVa = NvP64_NULL;

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pTaskLog->pTaskLogDescriptor,
                        pGpu,
                        logVgpuSetupParams.bufSize,
                        RM_PAGE_SIZE,
                        NV_TRUE, ADDR_FBMEM, NV_MEMORY_CACHED,
                        MEMDESC_FLAGS_NONE),
        exit);

    memdescDescribe(pTaskLog->pTaskLogDescriptor, ADDR_FBMEM, logVgpuSetupParams.bufOffset, logVgpuSetupParams.bufSize);

    pVa = memdescMapInternal(pGpu, pTaskLog->pTaskLogDescriptor, TRANSFER_FLAGS_NONE);
    if (pVa != NvP64_NULL)
    {
        pTaskLog->pTaskLogBuffer = pVa;
        portMemSet(pTaskLog->pTaskLogBuffer, 0, logVgpuSetupParams.bufSize);

        pTaskLog->id8 = _kgspGenerateInitArgId(logVgpuSetupParams.szMemoryId);

        nvDbgSnprintf(vm_string, sizeof(vm_string), "%s%d", logVgpuSetupParams.szPrefix, logVgpuSetupParams.gfid);

        NvU32 libosLogFlags = 0;

        libosLogFlags |= LIBOS_LOG_NVLOG_BUFFER_FLAG_PACKED_METADATA;

        if (logVgpuSetupParams.bMergeNvlog)
            libosLogFlags |= LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER;

        libosLogAddLogEx(&pKernelGsp->logDecodeVgpuPartition[logVgpuSetupParams.gfid - 1],
            pTaskLog->pTaskLogBuffer,
            memdescGetSize(pTaskLog->pTaskLogDescriptor),
            pGpu->gpuInstance,
            (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
            gpuGetChipImpl(pGpu),
            vm_string,
            logVgpuSetupParams.elfSectionName,
            0,
            pKernelGsp->pBuildIdSection,
            LIBOS_LOG_NVLOG_BUFFER_VERSION,
            libosLogFlags);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to map memory for %s task log buffer for vGPU partition \n", logVgpuSetupParams.szPrefix);
        nvStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        goto exit;
    }

exit:
    return nvStatus;
}

/*!
 * Initialize vgpu partition LIBOS task logging structures
 */
NV_STATUS
kgspInitVgpuPartitionLogging_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 gfid,
    NvU64 initTaskLogBUffOffset,
    NvU64 initTaskLogBUffSize,
    NvU64 vgpuTaskLogBUffOffset,
    NvU64 vgpuTaskLogBuffSize,
    NvU64 kernelLogBuffOffset,
    NvU64 kernelLogBuffSize
)
{
    struct
    {
        nv_firmware_task_t taskId;
        NvU64             bufOffset;
        NvU64             bufSize;
        RM_LIBOS_LOG_MEM *taskLogArr;
    } logInitValues[] =
    {
        {NV_FIRMWARE_TASK_INIT, initTaskLogBUffOffset, initTaskLogBUffSize, pKernelGsp->gspPluginInitTaskLogMem},
        {NV_FIRMWARE_TASK_VGPU, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, pKernelGsp->gspPluginVgpuTaskLogMem}
    };
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);

    NV_STATUS nvStatus = NV_OK;
    char sourceName[SOURCE_NAME_MAX_LENGTH];
    NvBool bMergeNvlog = NV_TRUE;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    if (gfid > MAX_PARTITIONS_WITH_GFID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    while (!portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 1, 0))
        osSpinLoop();

    // Source name is used to generate a tag that is a unique identifier for nvlog buffers.
    // As the source name 'GSP' is already in use, we will need a custom source name.
    nvDbgSnprintf(sourceName, SOURCE_NAME_MAX_LENGTH, "V%02d", gfid);
    libosLogCreateEx(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], sourceName);

    // Setup logging for each task in vgpu partition
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(logInitValues); ++i)
    {
        const nv_firmware_task_log_info_t* pInfo = nv_firmware_log_info_for_task(logInitValues[i].taskId);

        if (pInfo == NULL)
        {
            nvStatus = NV_ERR_INVALID_STATE;
            goto exit;
        }

        LOG_VGPU_SETUP_PARAMS logVgpuSetupParams = {gfid, pInfo->memory_id, pInfo->prefix, pInfo->elf_section_name,
                                                    logInitValues[i].bufOffset, logInitValues[i].bufSize,
                                                    logInitValues[i].taskLogArr, bMergeNvlog};

        nvStatus = _setupLogBufferVgpu(
            pGpu,
            pKernelGsp,
            logVgpuSetupParams
        );

        if (nvStatus != NV_OK)
            goto exit;
    }

    const nv_firmware_kernel_log_info_t *pInfo = nv_firmware_kernel_log_info_for_gpu(gpuGetChipArch(pGpu), gpuGetChipImpl(pGpu));

    if (pInfo->elf_section_name == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "Unknown chip for libos kernel logging\n");

        nvStatus = NV_ERR_INVALID_STATE;
        goto exit;
    }

    LOG_VGPU_SETUP_PARAMS logVgpuSetupParams = {gfid, NV_FIRMWARE_KERNEL_LOG_MEMORY_ID, NV_FIRMWARE_KERNEL_LOG_PREFIX,
                                                pInfo->elf_section_name, kernelLogBuffOffset, kernelLogBuffSize,
                                                pKernelGsp->libosKernelLogMem, bMergeNvlog};

    nvStatus = _setupLogBufferVgpu(
                pGpu,
                pKernelGsp,
                logVgpuSetupParams
            );

    if (bMergeNvlog)
    {
        char vmMergedLogString[8];
        nvDbgSnprintf(vmMergedLogString, sizeof(vmMergedLogString), "%s%d", "VGPU", gfid);
        libosLogSetupMergedNvlog(
            &pKernelGsp->logDecodeVgpuPartition[gfid - 1],
            pGpu->gpuInstance,
            initTaskLogBUffSize + vgpuTaskLogBuffSize + kernelLogBuffSize,
            vmMergedLogString,
            (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
            gpuGetChipImpl(pGpu),
            pKernelGsp->pBuildIdSection
        );
    }

    if (nvStatus != NV_OK)
        goto exit;

    {
        libosLogInit(&pKernelGsp->logDecodeVgpuPartition[gfid - 1],
                     pKernelGsp->pLogElf,
                     pKernelGsp->logElfDataSize);
        // nvlog buffers are now setup using the appropriate sourceName to avoid tag-value clash.
        // Now sourceName can be modified to preserve the 'GSP-VGPUx' logging convention.
        portStringCopy(pKernelGsp->logDecodeVgpuPartition[gfid - 1].sourceName,
                       SOURCE_NAME_MAX_LENGTH,
                       "GSP", SOURCE_NAME_MAX_LENGTH);
    }

    pKernelGsp->bHasVgpuLogs = NV_TRUE;

    libosLogUpdateTimerDelta(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], pTmr->sysTimerOffsetNs);

exit:
    if (nvStatus != NV_OK)
        _kgspFreeLibosVgpuPartitionLoggingStructures(pGpu, pKernelGsp, gfid);

    portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 0, 1);

    return nvStatus;
}

/*!
 * Preserve vGPU Partition log buffers between VM reboots
 */
NV_STATUS
kgspPreserveVgpuPartitionLogging_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 gfid
)
{
    if ((gfid == 0) || (gfid > MAX_PARTITIONS_WITH_GFID))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Make sure this this NvLog buffer is pushed
    kgspDumpGspLogs(pKernelGsp, NV_FALSE);

    // Preserve any captured vGPU Partition logs
    libosPreserveLogs(&pKernelGsp->logDecodeVgpuPartition[gfid - 1],
                      /* bPreserveNoWrap = */ NV_TRUE,
                      /* bPreserveWrap = */   NV_FALSE);

    return NV_OK;
}

void kgspNvlogFlushCb(void *pKernelGsp)
{
    if (pKernelGsp != NULL)
        kgspDumpGspLogs((KernelGsp*)pKernelGsp, NV_TRUE);
}

/*!
 * Free LIBOS task logging structures
 */
static void
_kgspFreeLibosLoggingStructures
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU8 idx;

    _kgspStopLogPolling(pGpu, pKernelGsp);

    // Make sure there is no lingering debug output.
    kgspDumpGspLogs(pKernelGsp, NV_FALSE);

    nvlogDeregisterFlushCb(kgspNvlogFlushCb, pKernelGsp);

    libosLogDestroy(&pKernelGsp->logDecode);

    for (idx = 0; idx < kgspGetLogCount(pKernelGsp); idx++)
    {
        RM_LIBOS_LOG_MEM *pLog = &pKernelGsp->rmLibosLogMem[idx];

        // release log memory for each task.
        if (pLog->pTaskLogBuffer != NULL)
        {
            memdescUnmap(pLog->pTaskLogDescriptor,
                         NV_TRUE,
                         (void *)pLog->pTaskLogBuffer,
                         pLog->pTaskLogMappingPriv);
            pLog->pTaskLogBuffer = NULL;
            pLog->pTaskLogMappingPriv = NULL;
        }

        if (pLog->pTaskLogDescriptor != NULL)
        {
            memdescFree(pLog->pTaskLogDescriptor);
            memdescDestroy(pLog->pTaskLogDescriptor);
            pLog->pTaskLogDescriptor = NULL;
        }
    }

    portMemFree(pKernelGsp->pLogElf);
    pKernelGsp->pLogElf = NULL;

    portMemFree(pKernelGsp->pBuildIdSection);
    pKernelGsp->pBuildIdSection = NULL;
}

static NV_STATUS
_setupLogBufferBaremetal
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 idx,
    const char *szMemoryId,
    const char *szPrefix,
    NvU32 size,
    const char *elfSectionName,
    NvBool bEnableNvlog,
    NvU64 flags
)
{
    NV_STATUS nvStatus = NV_OK;

    RM_LIBOS_LOG_MEM *pLog = &pKernelGsp->rmLibosLogMem[idx];
    NvP64 pVa = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;

    // Setup logging memory for each task.
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pLog->pTaskLogDescriptor,
                        pGpu,
                        size,
                        RM_PAGE_SIZE,
                        NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                        flags),
        exit);

    memdescTagAlloc(nvStatus,
                    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_12, pLog->pTaskLogDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
        exit);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pLog->pTaskLogDescriptor, 0,
                    memdescGetSize(pLog->pTaskLogDescriptor),
                    NV_TRUE, NV_PROTECT_READ_WRITE,
                    &pVa, &pPriv),
        exit);

    pLog->pTaskLogBuffer = pVa;
    pLog->pTaskLogMappingPriv = pPriv;
    portMemSet(pLog->pTaskLogBuffer, 0, memdescGetSize(pLog->pTaskLogDescriptor));

    // Pass the GPA for the log buffer in the log buffer, after the put pointer.
    pLog->pTaskLogBuffer[1] = memdescGetPhysAddr(pLog->pTaskLogDescriptor, AT_GPU, 0);

    pLog->id8 = _kgspGenerateInitArgId(szMemoryId);

    NvU32 libosLogDecodeFlags = (bEnableNvlog ? 0 : LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED);
    NvU32 libosLogFlags = LIBOS_LOG_NVLOG_BUFFER_FLAG_PACKED_METADATA;

    ct_assert(NV_FIRMWARE_GPU_ARCH_SHIFT == GPU_ARCH_SHIFT);

    libosLogAddLogEx(&pKernelGsp->logDecode,
        pLog->pTaskLogBuffer,
        memdescGetSize(pLog->pTaskLogDescriptor),
        pGpu->gpuInstance,
        (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
        gpuGetChipImpl(pGpu),
        szPrefix,
        elfSectionName,
        libosLogDecodeFlags,
        pKernelGsp->pBuildIdSection,
        LIBOS_LOG_NVLOG_BUFFER_VERSION,
        libosLogFlags);

exit:
    return nvStatus;
}


/*!
 * Initialize LIBOS task logging structures
 */
static NV_STATUS
_kgspInitLibosLoggingStructures
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 taskInitLogSize;
    NvU32 taskIntrLogSize;
    NvU32 taskRmLogSize;
    NvU32 taskMnocLogSize;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_INIT, &taskInitLogSize) != NV_OK)
    {
        taskInitLogSize = NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_INIT_DEFAULT;
    }
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_INTR, &taskIntrLogSize) != NV_OK)
    {
        taskIntrLogSize = NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_INTR_DEFAULT;
    }
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_RM, &taskRmLogSize) != NV_OK)
    {
        taskRmLogSize = NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_RM_DEFAULT;
    }
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_MNOC, &taskMnocLogSize) != NV_OK)
    {
        taskMnocLogSize = NV_REG_STR_RM_GSP_LOG_BUFFER_SIZE_TASK_MNOC_DEFAULT;
    }

    const struct
    {
        nv_firmware_task_t taskId;
        NvU32       size;
        NvBool      bEnableNvlog;
    } logInitValues[] =
    {
        {NV_FIRMWARE_TASK_INIT,  taskInitLogSize, NV_TRUE},
        {NV_FIRMWARE_TASK_INTR,  taskIntrLogSize, NV_TRUE},
        {NV_FIRMWARE_TASK_RM,    taskRmLogSize, NV_TRUE},
        {NV_FIRMWARE_TASK_MNOC,  taskMnocLogSize, NV_TRUE},
        {NV_FIRMWARE_TASK_DEBUG, 0x10000, NV_FALSE}, // 64KB task debugger debug log
        {NV_FIRMWARE_TASK_ROOT,             0x1000,  NV_TRUE},
        {NV_FIRMWARE_TASK_RM_STATE_MONITOR, 0x1000,  NV_TRUE},  // 64KB RM state monitor log on release builds
    };

    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) + 1 /* LOGKRNL */ <= LOGIDX_SIZE);

    NV_STATUS nvStatus = NV_OK;
    NvU32      registeredIdx = 0;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    libosLogCreate(&pKernelGsp->logDecode);

    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }

    for (NvU32 idx = 0; idx < NV_ARRAY_ELEMENTS(logInitValues); idx++)
    {
        const nv_firmware_task_log_info_t* pInfo = nv_firmware_log_info_for_task(logInitValues[idx].taskId);

        if (pInfo == NULL)
        {
            nvStatus = NV_ERR_INVALID_STATE;
            goto exit;
        }

        // skip LIBOS2 mappings on non-libos2 chips
        if (!(pInfo->supported_os & NV_FIRMWARE_LIBOS2_SUPPORTED) && kgspGetLibosVersion(pKernelGsp) == KGSP_LIBOS_VERSION_2)
            continue;

        // skip LIBOS3 mappings on non-libos3 chips
        if (!(pInfo->supported_os & NV_FIRMWARE_LIBOS3_SUPPORTED) && kgspGetLibosVersion(pKernelGsp) == KGSP_LIBOS_VERSION_3)
            continue;

        nvStatus = _setupLogBufferBaremetal(
            pGpu,
            pKernelGsp,
            registeredIdx,
            pInfo->memory_id,
            pInfo->prefix,
            logInitValues[idx].size,
            pInfo->elf_section_name,
            logInitValues[idx].bEnableNvlog,
            flags
        );

        registeredIdx++;
        if(nvStatus != NV_OK)
            goto exit;
    }

    // Determine which kernel is online, and add the according buffer
    if (kgspHasLibosKernelLogging_HAL(pGpu))
    {
        const nv_firmware_kernel_log_info_t *pInfo = nv_firmware_kernel_log_info_for_gpu(gpuGetChipArch(pGpu), gpuGetChipImpl(pGpu));

        if (pInfo->elf_section_name != NULL)
        {
            nvStatus = _setupLogBufferBaremetal(
                    pGpu,
                    pKernelGsp,
                    registeredIdx,
                    NV_FIRMWARE_KERNEL_LOG_MEMORY_ID,
                    NV_FIRMWARE_KERNEL_LOG_PREFIX,
                    0x10000,
                    pInfo->elf_section_name,
                    NV_TRUE,
                    flags
                );

            registeredIdx++;
            if (nvStatus != NV_OK)
                goto exit;
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING, "Unknown chip for libos kernel logging (non-fatal)\n");
        }
    }

exit:
    if (nvStatus != NV_OK)
        _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);

    return nvStatus;
}

static NV_STATUS
_kgspInitLibosLogDecoder
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    // If there's no log ELF or it's already been wired, skip wiring it now
    if ((pGspFw->pLogElf == NULL) || (pKernelGsp->pLogElf != NULL))
        return NV_OK;

    // Setup symbol decoder
    const void *pLogData = NULL;
    NvU64 logSize = 0;

    NV_ASSERT_OK_OR_RETURN(
        _kgspFwContainerVerifyVersion(pGpu, pKernelGsp,
            pGspFw->pLogElf,
            pGspFw->logElfSize,
            "GSP firmware log"));

    NV_ASSERT_OK_OR_RETURN(
        _kgspFwContainerGetSection(pGpu, pKernelGsp,
            pGspFw->pLogElf,
            pGspFw->logElfSize,
            GSP_LOGGING_SECTION_NAME,
            &pLogData,
            &logSize));

    pKernelGsp->pLogElf = portMemAllocNonPaged(logSize);
    pKernelGsp->logElfDataSize = logSize;

    NV_ASSERT_OR_RETURN(pKernelGsp->pLogElf != NULL, NV_ERR_NO_MEMORY);

    portMemCopy(pKernelGsp->pLogElf, logSize, pLogData, logSize);

    libosLogInit(&pKernelGsp->logDecode, pKernelGsp->pLogElf, logSize);

    return NV_OK;
}

static NV_STATUS
_kgspAllocSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NvP64 pVa   = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NV_STATUS nvStatus;

    if (!IS_SIMULATION(pGpu))
    {
        pKernelGsp->pMemDesc_simAccessBuf = NULL;
        pKernelGsp->pSimAccessBuf         = NULL;
        pKernelGsp->pSimAccessBufPriv     = NULL;
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pKernelGsp->pMemDesc_simAccessBuf,
                      pGpu,
                      sizeof(SimAccessBuffer),
                      RM_PAGE_SIZE,
                      NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                      MEMDESC_FLAGS_NONE),
        error_cleanup);

        memdescTagAlloc(nvStatus,
                NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_13, pKernelGsp->pMemDesc_simAccessBuf);
        NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus, error_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pKernelGsp->pMemDesc_simAccessBuf, 0,
                   memdescGetSize(pKernelGsp->pMemDesc_simAccessBuf),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pVa, &pPriv),
        error_cleanup);

    pKernelGsp->pSimAccessBuf = (SimAccessBuffer*)pVa;
    pKernelGsp->pSimAccessBufPriv = pPriv;

    portMemSet(pKernelGsp->pSimAccessBuf, 0, memdescGetSize(pKernelGsp->pMemDesc_simAccessBuf));

error_cleanup:
    if (nvStatus != NV_OK)
        _kgspFreeSimAccessBuffer(pGpu, pKernelGsp);

    return nvStatus;
}

static void kgspFreeRatsCrashBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
#if KERNEL_GSP_TRACING_RATS_ENABLED
    if (pKernelGsp->pRatsCrashBufferRaw != NULL)
    {
        memdescUnmap(pKernelGsp->pRatsCrashBufferRawMemDesc,
                     NV_TRUE,
                     pKernelGsp->pRatsCrashBufferRaw,
                     pKernelGsp->pRatsCrashBufferRawMemDescPriv);
        pKernelGsp->pRatsCrashBufferRaw = NULL;
    }

    if (pKernelGsp->pRatsCrashBufferRawMemDesc != NULL)
    {
        memdescFree(pKernelGsp->pRatsCrashBufferRawMemDesc);
        memdescDestroy(pKernelGsp->pRatsCrashBufferRawMemDesc);
        pKernelGsp->pRatsCrashBufferRawMemDesc = NULL;
    }
#endif
}

static NV_STATUS
kgspInitRatsCrashBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NV_STATUS status = NV_OK;
#if KERNEL_GSP_TRACING_RATS_ENABLED
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    void *pMemDescPriv = NULL;
    void *pBuffer = NULL;

    NvU32 gspTraceCrashRegkey;
    NvU32 bufferSizeBytes;
    NvU32 bufferSize = NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_DEFAULT;
    NvBool enableGspTraceCrash = NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_DEFAULT;

    // Feature not supported on CC due to needing shared SYSMEM buffer with GSP
    if (gpuIsCCFeatureEnabled(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "GSP Trace Crash logging is not supported on CC\n");
        enableGspTraceCrash = NV_FALSE;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING, &gspTraceCrashRegkey) == NV_OK)
    {
        enableGspTraceCrash = (gspTraceCrashRegkey == NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_ENABLE);

        // Fail loudly if user attempts to explicitly enable GSP Trace Crash logging on CC
        if (enableGspTraceCrash && gpuIsCCFeatureEnabled(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR, "Attempted to enable GSP Trace Crash logging on CC, which is not supported\n");
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    if (!enableGspTraceCrash)
    {
        NV_PRINTF(LEVEL_INFO, "GSP Trace Crash logging is disabled\n");
        return NV_OK;
    }

    osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE, &bufferSize);
    if (bufferSize > NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP Trace Crash buffer size is too large, clamping to %u\n",
                  NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MAX);
        bufferSize = NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MAX;
    }
    else if (bufferSize < NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MIN)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP Trace Crash buffer size is too small, clamping to %u\n",
                  NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MIN);
        bufferSize = NV_REG_STR_RM_GSP_TRACE_CRASH_LOGGING_BUFFER_SIZE_MIN;
    }

    bufferSizeBytes = sizeof(NV_RATS_RECORD) * bufferSize;
    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(&pMemDesc,
                      pGpu,
                      bufferSizeBytes,
                      RM_PAGE_SIZE,
                      NV_TRUE,
                      ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                      MEMDESC_FLAGS_NONE),
        done);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_GSP_TRACE_CRASH_BUFFER, pMemDesc);
    NV_ASSERT_OK_OR_GOTO(status, status, done);

    NV_ASSERT_OK_OR_GOTO(status,
        memdescMap(pMemDesc, 0,
                   memdescGetSize(pMemDesc),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pBuffer, &pMemDescPriv),
        done);
    portMemSet(pBuffer, 0, bufferSizeBytes);

    NV_RM_RPC_INIT_GSP_TRACE_CRASH_BUFFER(
        pGpu,
        status,
        memdescGetPhysAddr(pMemDesc, AT_GPU, 0),
        memdescGetSize(pMemDesc)
    );
    NV_ASSERT_OK_OR_GOTO(status, status, done);

    pKernelGsp->pRatsCrashBufferRawMemDesc = pMemDesc;
    pKernelGsp->pRatsCrashBufferRawMemDescPriv = pMemDescPriv;
    pKernelGsp->pRatsCrashBufferRaw = (NV_RATS_RECORD *)pBuffer;
    pKernelGsp->ratsCrashBufferSize = bufferSize;

done:
    if (status != NV_OK)
    {
        if (pBuffer != NULL)
        {
            memdescUnmap(pMemDesc,
                         NV_TRUE,
                         pBuffer,
                         pMemDescPriv);
        }

        if (pMemDesc != NULL)
        {
            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
        }
    }

#endif // KERNEL_GSP_TRACING_RATS_ENABLED
    return status;
}

static void kgspFreeRmStateMonitorBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    if (pKernelGsp->pRmStateMonitorBuffer != NULL)
    {
        memdescUnmap(pKernelGsp->pRmStateMonitorBufferMD,
                     NV_TRUE,
                     pKernelGsp->pRmStateMonitorBuffer,
                     pKernelGsp->pRmStateMonitorBufferMDPriv);
        pKernelGsp->pRmStateMonitorBuffer = NULL;
    }

    if (pKernelGsp->pRmStateMonitorBufferMD != NULL)
    {
        memdescFree(pKernelGsp->pRmStateMonitorBufferMD);
        memdescDestroy(pKernelGsp->pRmStateMonitorBufferMD);
        pKernelGsp->pRmStateMonitorBufferMD = NULL;
    }
}

static NV_STATUS
kgspAllocRmStateMonitorBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NV_STATUS status;

    NvU64 bufferSize = RM_PAGE_SIZE; // allocate 4kb

    NvU64 flags = MEMDESC_FLAGS_USER_READ_ONLY;

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(&pKernelGsp->pRmStateMonitorBufferMD,
                      pGpu,
                      bufferSize,
                      RM_PAGE_SIZE,
                      NV_TRUE,
                      ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                      flags),
        done);

    memdescTagAlloc(status,
                NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_14, pKernelGsp->pRmStateMonitorBufferMD);
    NV_ASSERT_OK_OR_GOTO(status, status, done);

    NV_ASSERT_OK_OR_GOTO(status,
                memdescMap(pKernelGsp->pRmStateMonitorBufferMD, 0,
                   memdescGetSize(pKernelGsp->pRmStateMonitorBufferMD),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pKernelGsp->pRmStateMonitorBuffer, &pKernelGsp->pRmStateMonitorBufferMDPriv),
        done);

    NV_PRINTF(LEVEL_INFO, "Mapped rm state monitor buffer: %p\n", pKernelGsp->pRmStateMonitorBuffer);
    NV_PRINTF(LEVEL_INFO, "bufferSize %llu memdesc size %llu\n",
        bufferSize, memdescGetSize(pKernelGsp->pRmStateMonitorBufferMD));

    if (pKernelGsp->pRmStateMonitorBuffer != NULL)
    {
        portMemSet(pKernelGsp->pRmStateMonitorBuffer, 0, memdescGetSize(pKernelGsp->pRmStateMonitorBufferMD));
    }

done:
    if (status != NV_OK)
        kgspFreeRmStateMonitorBuffer(pGpu, pKernelGsp);

    return status;
}

static void _kgspDumpRmState(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // skip dump for CC mode
    if (gpuIsCCFeatureEnabled(pGpu))
        return;

    // if state monitor is disabled via regkey, skip
    if (pKernelGsp->stateMonitorEnabled == NV_REG_STR_RM_ENABLE_STATE_MONITOR_DISABLE)
    {
        return;
    }

    if (pKernelGsp->pRmStateMonitorBuffer == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to dump RM state, RM state monitor buffer not allocated\n");
        return;
    }

    RM_STATE_MONITOR_BUFFER_LAYOUT *pRmStateMonitorBufferLayout = (RM_STATE_MONITOR_BUFFER_LAYOUT *)pKernelGsp->pRmStateMonitorBuffer;

    pRmStateMonitorBufferLayout->requestSequenceNumber++;

    RMTIMEOUT timeout;
    NV_STATUS status = NV_OK;
    NvU32 i = 0;
    NvU64 timeoutUs = pGpu->timeoutData.defaultus / 15; // magic number determined emperically


    gpuSetTimeout(pGpu, timeoutUs, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
    while (pRmStateMonitorBufferLayout->requestSequenceNumber != pRmStateMonitorBufferLayout->responseSequenceNumber)
    {
        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to get RM state, timeout\n");
            return;
        }
        osSpinLoop();
    }

    NV_ERROR_LOG_DATA(pGpu, 0, "******************************RM DUMP REPORT******************************\n");

    // check first to see if the DPC queue is empty
    NvBool dpcQueueEmpty = NV_TRUE;
    for (i = 0; i < NV_ARRAY_ELEMENTS(pRmStateMonitorBufferLayout->data.dpcQueue); i++)
    {
        if (pRmStateMonitorBufferLayout->data.dpcQueue[i][0] != 0)
        {
            dpcQueueEmpty = NV_FALSE;
            break;
        }
    }

    if (!dpcQueueEmpty)
    {
        NV_ERROR_LOG_DATA(pGpu, 0, "    DPC QUEUE: \n");
        for (i = 0; i < NV_ARRAY_ELEMENTS(pRmStateMonitorBufferLayout->data.dpcQueue); i++)
        {
            NvU32 qpcQueueSize = NV_ARRAY_ELEMENTS(pRmStateMonitorBufferLayout->data.dpcQueue[i]);

            for (NvU32 j = 0; j < qpcQueueSize;)
            {
                if (pRmStateMonitorBufferLayout->data.dpcQueue[i][j]   != 0 ||
                    pRmStateMonitorBufferLayout->data.dpcQueue[i][j+1] != 0 ||
                    pRmStateMonitorBufferLayout->data.dpcQueue[i][j+2] != 0 ||
                    pRmStateMonitorBufferLayout->data.dpcQueue[i][j+3] != 0)
                {
                    NV_ERROR_LOG_DATA(pGpu, 0, "DPC QUEUE[%02u]: [%u]: %016llx [%u] %016llx [%u] %016llx [%u] %016llx\n",
                        i,
                        j, pRmStateMonitorBufferLayout->data.dpcQueue[i][j],
                        j+1, pRmStateMonitorBufferLayout->data.dpcQueue[i][j+1],
                        j+2, pRmStateMonitorBufferLayout->data.dpcQueue[i][j+2],
                        j+3, pRmStateMonitorBufferLayout->data.dpcQueue[i][j+3]);
                }

                j = j + 4;
            }
        }
    }

    // check first to see if the work item queue is empty
    NvBool workItemQueueEmpty = NV_TRUE;
    for (i = 0; i < NV_ARRAY_ELEMENTS(pRmStateMonitorBufferLayout->data.workItemsQueue); i++)
    {
        if (pRmStateMonitorBufferLayout->data.workItemsQueue[i] != 0)
        {
            workItemQueueEmpty = NV_FALSE;
            break;
        }
    }

    if (!workItemQueueEmpty)
    {
        NV_ERROR_LOG_DATA(pGpu, 0, "    WORK ITEM QUEUE: \n");

        for (i = 0; i < NV_ARRAY_ELEMENTS(pRmStateMonitorBufferLayout->data.workItemsQueue);)
        {
            if (pRmStateMonitorBufferLayout->data.workItemsQueue[i]   != 0 ||
                pRmStateMonitorBufferLayout->data.workItemsQueue[i+1] != 0 ||
                pRmStateMonitorBufferLayout->data.workItemsQueue[i+2] != 0 ||
                pRmStateMonitorBufferLayout->data.workItemsQueue[i+3] != 0)
            {
                NV_ERROR_LOG_DATA(pGpu, 0, "[%02u]: %016llx [%02u] %016llx [%02u] %016llx [%02u] %016llx\n",
                    i, pRmStateMonitorBufferLayout->data.workItemsQueue[i],
                    i+1, pRmStateMonitorBufferLayout->data.workItemsQueue[i+1],
                    i+2, pRmStateMonitorBufferLayout->data.workItemsQueue[i+2],
                    i+3, pRmStateMonitorBufferLayout->data.workItemsQueue[i+3]);
            }

            i = i + 4;
        }
    }

    NV_ERROR_LOG_DATA(pGpu, 0, "DPC execution time: %llx\n", pRmStateMonitorBufferLayout->data.lastDpcExecutionTime);
    NV_ERROR_LOG_DATA(pGpu, 0, "Total DPC execution time: %llx\n", pRmStateMonitorBufferLayout->data.totalDpcExecutionTime);
    NV_ERROR_LOG_DATA(pGpu, 0, "Work items execution time: %llx\n", pRmStateMonitorBufferLayout->data.lastWorkItemsExecutionTime);
    NV_ERROR_LOG_DATA(pGpu, 0, "Total work items execution time: %llx\n", pRmStateMonitorBufferLayout->data.totalWorkItemsExecutionTime);

    NV_ERROR_LOG_DATA(pGpu, 0, "**************************************************************************\n");
}

static void
_kgspFreeSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    if (!IS_SIMULATION(pGpu))
    {
        return;
    }

    if (pKernelGsp->pMemDesc_simAccessBuf != NULL)
    {
        memdescFree(pKernelGsp->pMemDesc_simAccessBuf);
        memdescDestroy(pKernelGsp->pMemDesc_simAccessBuf);
    }

    pKernelGsp->pMemDesc_simAccessBuf = NULL;
    pKernelGsp->pSimAccessBuf         = NULL;
    pKernelGsp->pSimAccessBufPriv     = NULL;
}

static NV_STATUS
_kgspAllocNotifyOpSharedSurface(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NvP64 pVa   = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NV_STATUS nvStatus;
    NvU64 flags = MEMDESC_FLAGS_NONE;


    //
    // On systems with SEV enabled, the fault buffer flush sequence memory should be allocated
    // in unprotected sysmem as GSP will be writing to this location to let the guest
    // know a the issued notify op has finished as well as the status of the operation.
    //
    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }



    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pKernelGsp->pNotifyOpSurfMemDesc,
                      pGpu,
                      sizeof(NotifyOpSharedSurface),
                      RM_PAGE_SIZE,
                      NV_FALSE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                      flags),
        error_cleanup);

        memdescTagAlloc(nvStatus,
                NV_FB_ALLOC_RM_INTERNAL_OWNER_GSP_NOTIFY_OP_SURFACE, pKernelGsp->pNotifyOpSurfMemDesc);
        NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus, error_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pKernelGsp->pNotifyOpSurfMemDesc, 0,
                   memdescGetSize(pKernelGsp->pNotifyOpSurfMemDesc),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pVa, &pPriv),
        error_cleanup);

    pKernelGsp->pNotifyOpSurf = (NotifyOpSharedSurface*)pVa;
    pKernelGsp->pNotifyOpSurfPriv = pPriv;

    portMemSet(pKernelGsp->pNotifyOpSurf, 0, memdescGetSize(pKernelGsp->pNotifyOpSurfMemDesc));

error_cleanup:
    if (nvStatus != NV_OK)
        _kgspFreeNotifyOpSharedSurface(pGpu, pKernelGsp);

    return nvStatus;
}

static void
_kgspFreeNotifyOpSharedSurface(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    if (pKernelGsp->pNotifyOpSurfMemDesc != NULL)
    {
        memdescUnmap(pKernelGsp->pNotifyOpSurfMemDesc,
                     NV_TRUE,
                     pKernelGsp->pNotifyOpSurf,
                     pKernelGsp->pNotifyOpSurfPriv);
        memdescFree(pKernelGsp->pNotifyOpSurfMemDesc);
        memdescDestroy(pKernelGsp->pNotifyOpSurfMemDesc);
    }

    pKernelGsp->pNotifyOpSurfMemDesc = NULL;
    pKernelGsp->pNotifyOpSurf         = NULL;
    pKernelGsp->pNotifyOpSurfPriv     = NULL;
}

static void
_kgspReadRegkeyOverrides(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NvU32 data32 = 0;

    (void)osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_WPR_END_MARGIN, &pKernelGsp->wprEndMarginOverride);

    if (osReadRegistryDword(pGpu, NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB, &pKernelGsp->heapSizeMBOverride) != NV_OK)
    {
        pKernelGsp->heapSizeMBOverride = 0;
    }

    // GSP stall detection can be disabled for VGPU-GSP via regkey to help debug issues like bug 5487708
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_STALL_DETECTION, &pKernelGsp->gspStallDetection) != NV_OK)
    {
        pKernelGsp->gspStallDetection = NV_REG_STR_RM_GSP_STALL_DETECTION_DEFAULT;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_PERIODIC_HEALTHCHECK, &pKernelGsp->gspPeriodicHealthcheck) != NV_OK)
    {
        pKernelGsp->gspPeriodicHealthcheck = NV_REG_STR_RM_GSP_PERIODIC_HEALTHCHECK_DEFAULT;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_GPU_CONTAINMENT_CHECK, &pKernelGsp->gspGpuContainmentCheck) != NV_OK)
    {
        pKernelGsp->gspGpuContainmentCheck = NV_REG_STR_RM_GSP_GPU_CONTAINMENT_CHECK_DEFAULT;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_TIMEOUT_CLASSIFICATION, &pKernelGsp->gspTimeoutClassification) != NV_OK)
    {
        pKernelGsp->gspTimeoutClassification = NV_REG_STR_RM_GSP_TIMEOUT_CLASSIFICATION_DEFAULT;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_RPC_TIMEOUT_GPU_RESET_THRESHOLD, &pKernelGsp->gspRpcTimeoutGpuResetThreshold) != NV_OK)
    {
        pKernelGsp->gspRpcTimeoutGpuResetThreshold = NV_REG_STR_RM_GSP_RPC_TIMEOUT_GPU_RESET_THRESHOLD_DEFAULT;
    }

    if (NV_OK != osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_STATE_MONITOR, &pKernelGsp->stateMonitorEnabled))
    {
        pKernelGsp->stateMonitorEnabled = NV_REG_STR_RM_ENABLE_STATE_MONITOR_DEFAULT;
    }

    // Single VM optimization regkeys
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_VGPU_GSP_SINGLE_VM_MODE, &data32) == NV_OK)
    {
        pKernelGsp->bVgpuGspSingleVmMode = (data32 == NV_REG_STR_RM_VGPU_GSP_SINGLE_VM_MODE_ENABLED);
    }

    pKernelGsp->singleVmHeapAdjustmentMB = 0;

    // By default, preserve logs on unload is disabled due to noisy logs and perf
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS, &pKernelGsp->preserveLogs) != NV_OK)
    {
        pKernelGsp->preserveLogs = NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS_DEFAULT;
    }

    {
        NvU32 pagingConfig = NV_REG_STR_RM_GSP_PAGING_CONFIG_DEFAULT;
        const NvU32 imemPagingMask =
            DRF_SHIFTMASK(NV_REG_STR_RM_GSP_PAGING_CONFIG_INIT_CODE) |
            DRF_SHIFTMASK(NV_REG_STR_RM_GSP_PAGING_CONFIG_RM_CODE);

        osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_PAGING_CONFIG, &pagingConfig);

        if (!pKernelGsp->bImemPagingSupported && (pagingConfig & imemPagingMask))
        {
            NV_PRINTF(LEVEL_WARNING, "IMEM paging not supported on this GPU, ignoring\n");
            pagingConfig &= ~imemPagingMask;
        }

        if (pagingConfig != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                "RmGspPagingConfig = 0x%x: init_code=%s rm_code=%s init_data=%s rm_data=%s "
                "init_stack=%s rm_stack=%s intr_stack=%s wpr_heap=%s\n", pagingConfig,
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _INIT_CODE,  _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _RM_CODE,    _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _INIT_DATA,  _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _RM_DATA,    _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _INIT_STACK, _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _RM_STACK,   _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _INTR_STACK, _ENABLED, pagingConfig) ? "on" : "off",
                FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _WPR_HEAP,   _ENABLED, pagingConfig) ? "on" : "off");
        }

        pKernelGsp->pagingConfig = pagingConfig;
    }

    // override the heartbeat timeout value, no fallback for default value
    NvU32 gspRmHeartbeatTimeout = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_RM_HEARTBEAT_TIMEOUT, &gspRmHeartbeatTimeout) == NV_OK)
    {
        if (gspRmHeartbeatTimeout >= NV_REG_STR_RM_GSP_RM_HEARTBEAT_TIMEOUT_MIN)
        {
            pKernelGsp->gspRmHeartbeatTimeoutMs = gspRmHeartbeatTimeout;
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Ignoring RmGspRmHeartbeatTimeout=%u (below minimum %u ms)\n",
                      gspRmHeartbeatTimeout,
                      NV_REG_STR_RM_GSP_RM_HEARTBEAT_TIMEOUT_MIN);
        }
    }else
    {
        pKernelGsp->gspRmHeartbeatTimeoutMs = 0x0;
    }

}

/*!
 * Create KernelGsp object and initialize RPC infrastructure
 */
NV_STATUS
kgspConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    ENGDESCRIPTOR engDesc
)
{
    NV_STATUS nvStatus = NV_OK;

    if (!IS_GSP_CLIENT(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    _kgspReadRegkeyOverrides(pGpu, pKernelGsp);

    kgspConfigureFalcon_HAL(pGpu, pKernelGsp);

    // Init RPC objects used to communicate with GSP.
    nvStatus = _kgspInitRpcInfrastructure(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init RPC infrastructure failed\n");
        goto done;
    }

    // Clear out the gspStaticInfo. We will populate this once GSP-RM is up.
    portMemSet(&pKernelGsp->gspStaticInfo, 0,
               sizeof(pKernelGsp->gspStaticInfo));

    nvStatus = kgspAllocBootArgs_HAL(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "boot arg alloc failed: 0x%x\n", nvStatus);
        goto done;
    }

    if (IS_SIMULATION(pGpu))
    {
        nvStatus = _kgspAllocSimAccessBuffer(pGpu, pKernelGsp);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "sim access buffer alloc failed: 0x%x\n", nvStatus);
            goto done;
        }
    }

    nvStatus = _kgspAllocNotifyOpSharedSurface(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "notify operation shared surface alloc failed: 0x%x\n", nvStatus);
        goto done;
    }

done:
    if (nvStatus != NV_OK)
    {
        _kgspFreeSimAccessBuffer(pGpu, pKernelGsp);
        kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
        _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);
        _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    }

    return nvStatus;
}

NV_STATUS kgspStateInitLocked_IMPL(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NvDebugDump *pNvd = GPU_GET_NVD(pGpu);

    if (pNvd != NULL)
    {
        //
        // Register as PRIORITY_CRITICAL so it runs sooner and we get more of the
        // useful RPC history and not too many DUMP_COMPONENT and similar RPCs
        // invoked by other engines' dump functions
        //
        nvdEngineSignUp(pGpu,
                        pNvd,
                        _kgspDumpEngineFunc,
                        NVDUMP_COMPONENT_ENG_KGSP,
                        REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _CRITICAL) |
                        REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _CPU),
                        (void *)pGpu);
    }
    return NV_OK;
}


/*!
 * Convert VBIOS version containing Version and OemVersion packed together to
 * a string representation.
 *
 * Example:
 *   for Version 0x05400001, OemVersion 0x12
 *   input argument vbiosVersionCombined 0x0540000112
 *   output str "5.40.00.01.12"
 */
static void
_kgspVbiosVersionToStr(NvU64 vbiosVersionCombined, char *pVbiosVersionStr, NvU32 size)
{
    nvDbgSnprintf(pVbiosVersionStr, size, "%2X.%02X.%02X.%02X.%02X",
                  (vbiosVersionCombined >> 32) & 0xff,
                  (vbiosVersionCombined >> 24) & 0xff,
                  (vbiosVersionCombined >> 16) & 0xff,
                  (vbiosVersionCombined >> 8) & 0xff,
                  (vbiosVersionCombined) & 0xff);
}

static NV_STATUS
_kgspEncodePackedRegistryKeys(KernelGsp *pKernelGsp, NVKVContext *pKVContext)
{
    PACKED_REGISTRY_TABLE *pRegTable = pKernelGsp->pRegTable;
    NvU8                  *pByte     = (NvU8*)pRegTable;  // Byte version of record pointer.
    NV_STATUS              nvStatus  = NV_OK;
    NvU32                  i;

    NV_PRINTF(LEVEL_INFO, "encoding registry entries\n");

    #define NVKV_PREFIX NVGMC_SI

    // Walk the records and copy the data.
    for (i = 0; i < pRegTable->numEntries; i++)
    {
        PACKED_REGISTRY_ENTRY *pEntry = &pRegTable->entries[i];

        if (pEntry->length == 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                        "Skipping registry write for entry with length 0\n");
            continue;
        }

        NvU32 regKeyNameLength = portStringLength((const char *)&pByte[pEntry->nameOffset]) + 1;
        NV_ASSERT_OR_RETURN(regKeyNameLength <= NVGMC_SI_REGKEY_NAME_MAX_LEN, NV_ERR_INVALID_ARGUMENT);

        switch (pEntry->type)
        {
            case REGISTRY_TABLE_ENTRY_TYPE_DWORD:
                NVKV_SET_STRING8_LEN(pKVContext, 0, REGKEY_NAME, &pByte[pEntry->nameOffset], regKeyNameLength);
                NVKV_SET_SEQ32_1U(pKVContext, 0, REGKEY_VALUE_U32, pEntry->data);
                break;

            case REGISTRY_TABLE_ENTRY_TYPE_BINARY:
                if (pEntry->length > 4096)
                {
                    NV_PRINTF(LEVEL_WARNING, "Large binary regkey %s (length %u) skipped\n",
                        (const char *)&pByte[pEntry->nameOffset], pEntry->length);
                }
                else
                {
                    NVKV_SET_STRING8_LEN(pKVContext, 0, REGKEY_NAME, &pByte[pEntry->nameOffset], regKeyNameLength);
                    NVKV_SET_ARRAY8(pKVContext, 0, REGKEY_VALUE_BINARY, &pByte[pEntry->data], pEntry->length);
                }
                break;

            case REGISTRY_TABLE_ENTRY_TYPE_STRING:
                NVKV_SET_STRING8_LEN(pKVContext, 0, REGKEY_NAME, &pByte[pEntry->nameOffset], regKeyNameLength);
                NVKV_SET_STRING8_LEN(pKVContext, 0, REGKEY_VALUE_STRING, &pByte[pEntry->data], pEntry->length);
                break;

            default:
                // We should never get here.
                NV_PRINTF(LEVEL_ERROR, "    bad type %u\n", pEntry->type);
                nvStatus = NV_ERR_INVALID_STATE;
                break;
        }
        if (nvStatus != NV_OK)
            break;
    }

    #undef NVKV_PREFIX

    return nvStatus;
}

/*!
 * Encode system information into an NVKV buffer for transmission to GSP (GMCAPI path).
 *
 * Subsumes the former split of _kgspPrepareSystemInfo() (populate GspSystemInfo) and
 * _kgspEncodeSystemInfo(pSI). Values are read from GPU/RM state as keys are emitted; only fields
 * that required explicit preparation (PCI mirror via kbif HAL, clSyncWithGsp outputs) use locals
 * above. Emits keys only when the corresponding source is present, matching the old behavior.
 * ACPI substructures use explicit keys; MUX and DOD arrays use indexed keys.
 *
 * @param[in]  pGpu       GPU object (availability of subsystems)
 * @param[in]  pKernelGsp Kernel Gsp (sim/notify surfaces, packed registry table)
 * @param[out] pKVData    Output NVKV-encoded buffer (NvU64-aligned)
 * @param[in]  kvLimit    Max NvU64 entries in pKVData
 * @param[out] pOutCount  Number of NvU64 entries written
 *
 * @return NV_OK or NV_ERR_BUFFER_TOO_SMALL
 */
static NV_STATUS
_kgspEncodeSystemInfo
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU64     *pKVData,
    NvU32      kvLimit,
    NvU32     *pOutCount
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU64 pcieFlags = 0;
    *pOutCount = 0;

    NVKVContext pKVContext = NVKV_BEGIN(pKVData, 0, kvLimit);

    // Cache GPU SSID info
    NVKV_SET_SEQ32_3U(&pKVContext, 0,
        NVGMC_SI_PCI_DEVICE_ID, pGpu->idInfo.PCIDeviceID,
        NVGMC_SI_PCI_SUB_DEVICE_ID, pGpu->idInfo.PCISubDeviceID,
        NVGMC_SI_PCI_REVISION_ID, pGpu->idInfo.PCIRevisionID);

    // Note: Can't use this above because PCI_DEVICE_ID, PCI_SUB_DEVICE_ID, and PCI_REVISION_ID are already macros.
    #define NVKV_PREFIX NVGMC_SI

    /* GPU physical addresses (4 sequential 64-bit) */
    NVKV_SET_SEQ64_4U(&pKVContext, 0, GPU_PHYS_ADDR, pGpu->busInfo.gpuPhysAddr,
                                      GPU_PHYS_FB_ADDR, pGpu->busInfo.gpuPhysFbAddr,
                                      GPU_PHYS_INST_ADDR, pGpu->busInfo.gpuPhysInstAddr,
                                      GPU_PHYS_IO_ADDR, pGpu->busInfo.gpuPhysIoAddr);
    NVKV_SET_SEQ64_1U(&pKVContext, 0, NV_DOMAIN_BUS_DEVICE_FUNC, pGpu->busInfo.nvDomainBusDeviceFunc);
    NVKV_SET_SEQ32_1U(&pKVContext, 0, HOST_ARCH, gmcapiOorArchRmToGmcapi((NvU8)pGpu->busInfo.oorArch));

    NvU32 hostOsType = RMCFG_FEATURE_PLATFORM_WINDOWS ? NVGMC_SI_HOST_OS_TYPE_WINDOWS :
                       RMCFG_FEATURE_PLATFORM_UNIX ? NVGMC_SI_HOST_OS_TYPE_UNIX :
                       NVGMC_SI_HOST_OS_TYPE_OTHER;
    NVKV_SET_SEQ32_1U(&pKVContext, 0, HOST_OS_TYPE, hostOsType);

    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    if (pKernelBif != NULL)
    {
        NvU32 pciConfigMirrorBase = 0;
        NvU32 pciConfigMirrorSize = 0;

        NV_STATUS status = kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif,
                                                    &pciConfigMirrorBase,
                                                    &pciConfigMirrorSize);
        // PCIe config space mirror is removed on Blackwell+ chips, so NV_ERR_NO_SUCH_DOMAIN is expected.
        NV_ASSERT(status == NV_OK ||
                  status == NV_ERR_NO_SUCH_DOMAIN);

        NVKV_SET_SEQ32_2U(&pKVContext, 0, PCI_CONFIG_MIRROR_BASE, pciConfigMirrorBase,
                                          PCI_CONFIG_MIRROR_SIZE, pciConfigMirrorSize);

        pcieFlags |= NVKV_NUM64(PCIE_FLAGS_MNOC_AVAILABLE, !!pKernelBif->bMnocAvailable) |
                     NVKV_NUM64(PCIE_FLAGS_FLR_SUPPORTED, !!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED)) |
                     NVKV_NUM64(PCIE_FLAGS_PCIE_POWER_CONTROL_PRESENT, !!pKernelBif->pciePowerControlInfo.bPciePowerControlPresent) |
                     NVKV_NUM64(PCIE_FLAGS_64B_BAR0_SUPPORTED, !!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED));

        // Cache L1SS enablement from chipset side (kbif)
        NVKV_SET_SEQ32_1U(&pKVContext, 0, CHIPSET_L1SS_ENABLE, pKernelBif->chipsetL1ssEnable);

        // Cache Pcie Power Control Info, NV_PF0_DEVICE_CONTROL_2, and pcie link capabilities from config space
        NVKV_SET_SEQ32_3U(&pKVContext, 0,
            PCIE_POWER_CONTROL_VALUE, pKernelBif->pciePowerControlInfo.pciePowerControlValue,
            PF0_DEVICE_CONTROL2_REG, pKernelBif->pf0DeviceControl2Reg,
            PCIE_LINK_CAP, pKernelBif->pcieConfigReg.linkCap);

        NVKV_SET_SEQ64_1U(&pKVContext, 0, PCIE_ATOMICS_OP_MASK, pKernelBif->osPcieAtomicsOpMask);
        NVKV_SET_SEQ32_1U(&pKVContext, 0, PCIE_ATOMICS_CPL_DEV_CAP_MASK, pKernelBif->pcieAtomicsCplDeviceCapMask);
    }

    if (IS_SIMULATION(pGpu))
    {
        NVKV_SET_SEQ64_1U(&pKVContext, 0, SIM_ACCESS_BUF_PHYS_ADDR,
                          memdescGetPhysAddr(pKernelGsp->pMemDesc_simAccessBuf, AT_GPU, 0));
    }

    if (pKernelGsp->pNotifyOpSurfMemDesc != NULL)
    {
        NVKV_SET_SEQ64_1U(&pKVContext, 0, NOTIFY_OP_SHARED_SURF_PHYS_ADDR,
                          memdescGetPhysAddr(pKernelGsp->pNotifyOpSurfMemDesc, AT_GPU, 0));
    }

    NVKV_SET_SEQ64_2U(&pKVContext, 0, CONSOLE_MEM_SIZE, GPU_GET_MEMORY_MANAGER(pGpu)->Ram.ReservedConsoleDispMemSize,
                                      MAX_USER_VA, osGetMaxUserVa());

    OBJCL *pCl = SYS_GET_CL(SYS_GET_INSTANCE());
    if (pCl != NULL)
    {
        NvU64 clPdbProperties = 0;
        NvU32 chipsetId = 0;
        BUSINFO fhbBusInfo = { 0 };
        BUSINFO chipsetBusInfo = { 0 };

        clSyncWithGsp(pCl, &clPdbProperties, &chipsetId, &fhbBusInfo, &chipsetBusInfo);
        NVKV_SET_SEQ64_1U(&pKVContext, 0, CL_FLAGS, clPdbProperties);
        NVKV_SET_SEQ64_2U(&pKVContext, 0,
            FHB_BUSINFO, NVKV_NUM64(BUSINFO_DEVICE_ID, fhbBusInfo.deviceID) |
                         NVKV_NUM64(BUSINFO_VENDOR_ID, fhbBusInfo.vendorID) |
                         NVKV_NUM64(BUSINFO_SUBDEVICE_ID, fhbBusInfo.subdeviceID) |
                         NVKV_NUM64(BUSINFO_SUBVENDOR_ID, fhbBusInfo.subvendorID),
            CHIPSET_BUSINFO, NVKV_NUM64(BUSINFO_DEVICE_ID, chipsetBusInfo.deviceID) |
                             NVKV_NUM64(BUSINFO_VENDOR_ID, chipsetBusInfo.vendorID) |
                             NVKV_NUM64(BUSINFO_SUBDEVICE_ID, chipsetBusInfo.subdeviceID) |
                             NVKV_NUM64(BUSINFO_SUBVENDOR_ID, chipsetBusInfo.subvendorID));
        NVKV_SET_SEQ32_2U(&pKVContext, 0,
            FHB_BUSINFO_REVISION_ID, (NvU32)fhbBusInfo.revisionID,
            CHIPSET_BUSINFO_REVISION_ID, (NvU32)chipsetBusInfo.revisionID);
        NVKV_SET_SEQ32_1U(&pKVContext, 0, CHIPSET, chipsetId);
    }

    NVKV_SET_SEQ32_1U(&pKVContext, 0, HOST_PAGE_SIZE, (NvU32)osGetPageSize());

    // Fill in the cached ACPI method data.
    // GSP will treat receiving any ACPI key as assuming ACPI data is valid.
    const ACPI_METHOD_DATA *pAcpi = &pGpu->acpiMethodData;
    if (pAcpi->bValid)
    {
        NvU32 i;
        const DOD_METHOD_DATA *pDod = &pAcpi->dodMethodData;
        const JT_METHOD_DATA *pJt = &pAcpi->jtMethodData;
        const MUX_METHOD_DATA *pMux = &pAcpi->muxMethodData;
        const CAPS_METHOD_DATA *pCaps = &pAcpi->capsMethodData;

        NVKV_SET_SEQ32_1U(&pKVContext, 0, ACPI_DOD_STATUS, (NvU32)pDod->status);
        NVKV_SET_ARRAY32(&pKVContext, 0, ACPI_DOD_ACPI_IDS, pDod->acpiIdList, pDod->acpiIdListLen/sizeof(NvU32));

        NVKV_SET_SEQ32_4U(&pKVContext, 0, ACPI_JT_STATUS, (NvU32)pJt->status,
                                          ACPI_JT_CAPS, pJt->jtCaps,
                                          ACPI_JT_REV_ID, (NvU32)pJt->jtRevId,
                                          ACPI_JT_B_SBIOS_CAPS, (NvU32)pJt->bSBIOSCaps);

        NVKV_SET_SEQ32_1U(&pKVContext, 0, ACPI_MUX_TABLE_LEN, pMux->tableLen);
        for (i = 0; i < pMux->tableLen; i++)
        {
            NVKV_SET_SEQ32_3U(&pKVContext, i, ACPI_MUX_MODE_ACPI_ID, pMux->acpiIdMuxModeTable[i].acpiId,
                                              ACPI_MUX_MODE_MODE, pMux->acpiIdMuxModeTable[i].mode,
                                              ACPI_MUX_MODE_STATUS, (NvU32)pMux->acpiIdMuxModeTable[i].status);
            NVKV_SET_SEQ32_3U(&pKVContext, i, ACPI_MUX_PART_ACPI_ID, pMux->acpiIdMuxPartTable[i].acpiId,
                                              ACPI_MUX_PART_MODE, pMux->acpiIdMuxPartTable[i].mode,
                                              ACPI_MUX_PART_STATUS, (NvU32)pMux->acpiIdMuxPartTable[i].status);
            NVKV_SET_SEQ32_3U(&pKVContext, i, ACPI_MUX_STATE_ACPI_ID, pMux->acpiIdMuxStateTable[i].acpiId,
                                              ACPI_MUX_STATE_MODE, pMux->acpiIdMuxStateTable[i].mode,
                                              ACPI_MUX_STATE_STATUS, (NvU32)pMux->acpiIdMuxStateTable[i].status);
        }

        NVKV_SET_SEQ32_2U(&pKVContext, 0, ACPI_CAPS_STATUS, (NvU32)pCaps->status,
                                          ACPI_CAPS_OPTIMUS_CAPS, pCaps->optimusCaps);
    }

    // Hypervisor type and virtual config
    NVKV_SET_SEQ32_2U(&pKVContext, 0, HYPERVISOR_TYPE, hypervisorGetHypervisorType(SYS_GET_HYPERVISOR(pSys)),
                                      VIRTUAL_CONFIG_BITS, (NvU32)pGpu->virtualConfigBits);
    // VF Related GPU Data
    NVKV_SET_SEQ32_2U(&pKVContext, 0, VF_TOTAL_VFS, pGpu->sriovState.totalVFs,
                                      VF_FIRST_VF_OFFSET, pGpu->sriovState.firstVFOffset);
    NVKV_SET_SEQ64_3U(&pKVContext, 0, VF_FIRST_BAR0_ADDRESS, pGpu->sriovState.firstVFBarAddress[0],
                                      VF_FIRST_BAR1_ADDRESS, pGpu->sriovState.firstVFBarAddress[1],
                                      VF_FIRST_BAR2_ADDRESS, pGpu->sriovState.firstVFBarAddress[2]);

    NVKV_SET_SEQ64_1U(&pKVContext, 0, SYS_TIMER_OFFSET_NS, GPU_GET_TIMER(pGpu)->sysTimerOffsetNs);


    NVKV_SET_SEQ32_1U(&pKVContext, 0, GRID_BUILD_CSP, osGetGridCspSupport());

    // Boolean flags packed into 64-bit words
    // PCIe related flags (some were set up above if kernel bif is present)
    pcieFlags |=
        NVKV_NUM64(PCIE_FLAGS_GPU_BEHIND_BRIDGE, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE)) |
        NVKV_NUM64(PCIE_FLAGS_UPSTREAM_L0S_UNSUPPORTED, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED)) |
        NVKV_NUM64(PCIE_FLAGS_UPSTREAM_L1_UNSUPPORTED, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED)) |
        NVKV_NUM64(PCIE_FLAGS_UPSTREAM_L1_POR_SUPPORTED, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED)) |
        NVKV_NUM64(PCIE_FLAGS_UPSTREAM_L1_POR_MOBILE_ONLY, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY)) |
        NVKV_NUM64(PCIE_FLAGS_IS_CXL_DEVICE, !!osGpuIsCxlDevice(pGpu)) |
        NVKV_NUM64(PCIE_FLAGS_UPSTREAM_ADDRESS_VALID, !!pGpu->gpuClData.upstreamPort.addr.valid);

    // Platform / virtualization: system mux, passthru, primary, grid build, S0ix, nocat, page tables, zero-FB region.
    NvU64 platformFlags =
        NVKV_NUM64(PLATFORM_FLAGS_SYSTEM_HAS_MUX, !!pGpu->bSystemHasMux) |
        NVKV_NUM64(PLATFORM_FLAGS_IS_PASSTHRU, !!pGpu->bIsPassthru) |
        NVKV_NUM64(PLATFORM_FLAGS_IS_PRIMARY, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE)) |
        NVKV_NUM64(PLATFORM_FLAGS_S0IX_SUPPORT, !!pSys->getProperty(pSys, PDB_PROP_SYS_SUPPORTS_S0IX)) |
        NVKV_NUM64(PLATFORM_FLAGS_GSP_NOCAT_ENABLED, !!(RMCFG_FEATURE_PLATFORM_WINDOWS)) |
        NVKV_NUM64(PLATFORM_FLAGS_ENABLE_DYN_GRAN_PAGE_ARRAYS, !!pSys->bEnableDynamicGranularityPageArrays) |
#if defined(NV_UNIX) && !RMCFG_FEATURE_MODS_FEATURES
        NVKV_NUM64(PLATFORM_FLAGS_IS_GRID_BUILD, !!os_is_grid_supported()) |
#endif // defined(NV_UNIX) && !RMCFG_FEATURE_MODS_FEATURES
        NVKV_NUM64(PLATFORM_FLAGS_RESERVE_ZERO_FB_ADDR_AS_REGION, !!GPU_GET_MEMORY_MANAGER(pGpu)->bReserveZeroFbAddressAsRegion);

    // Display, mem policy, TDR, FSP, scheduler: corresponds to KERNEL_DISPLAY / KERNEL_MEMORY_SYSTEM / etc. in prepare.
    NvU64 featureFlags =
        NVKV_NUM64(FEATURE_FLAGS_UNIX_HDMI_FRL_COMPLIANCE, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_UNIX_HDMI_FRL_COMPLIANCE_ENABLED)) |
        NVKV_NUM64(FEATURE_FLAGS_PRESERVE_VIDEO_MEM_ALLOCS, !!GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->bPreserveComptagBackingStoreOnSuspend) |
        // Indicate whether the driver supports NV2080_NOTIFIERS_UCODE_RESET event.
        NVKV_NUM64(FEATURE_FLAGS_TDR_EVENT_SUPPORTED, !!pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_TDR_EVENT));

    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    if (pKernelDisplay != NULL)
    {
        featureFlags |=
            NVKV_NUM64(FEATURE_FLAGS_STRETCH_VBLANK_CAPABLE, !!pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE)) |
            NVKV_NUM64(FEATURE_FLAGS_WINDOW_CHANNEL_ALWAYS_MAPPED, !!pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_WINDOW_CHANNEL_ALWAYS_MAPPED));
    }

    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    if (pKernelFsp != NULL)
    {
        featureFlags |= NVKV_NUM64(FEATURE_FLAGS_CLOCK_BOOST_SUPPORTED, !!pKernelFsp->bClockBoostSupported);
    }

    // Fill in VF related GPU flags (64-bit BAR capability per VF BAR).
    NvU64 vfFlags =
        NVKV_NUM64(VF_FLAGS_64BIT_BAR0, !!pGpu->sriovState.b64bitVFBar0) |
        NVKV_NUM64(VF_FLAGS_64BIT_BAR1, !!pGpu->sriovState.b64bitVFBar1) |
        NVKV_NUM64(VF_FLAGS_64BIT_BAR2, !!pGpu->sriovState.b64bitVFBar2);

    NVKV_SET_SEQ64_4U(&pKVContext, 0, PCIE_FLAGS, pcieFlags,
                                      PLATFORM_FLAGS, platformFlags,
                                      FEATURE_FLAGS, featureFlags,
                                      VF_FLAGS, vfFlags);

    NVKV_SET_STRING8(&pKVContext, 0, KERNEL_DRIVER_NAME, "NVRM");
    NVKV_SET_STRING8(&pKVContext, 0, KERNEL_DRIVER_VERSION_STRING, NV_VERSION_STRING);
    NVKV_SET_SEQ32_1U(&pKVContext, 0, KERNEL_DRIVER_CHANGELIST, NV_BUILD_CHANGELIST_NUM);

    NvU32 unstableInterfaceSupport =
        NVKV_NUM(UNSTABLE_INTERFACE_SUPPORT_RPC, NV_TRUE) |
        NVKV_NUM(UNSTABLE_INTERFACE_SUPPORT_EVENTS, NV_TRUE);
    NVKV_SET_SEQ32_1U(&pKVContext, 0, UNSTABLE_INTERFACE_SUPPORT, unstableInterfaceSupport);

    NV_ASSERT_OK_OR_RETURN(_kgspEncodePackedRegistryKeys(pKernelGsp, &pKVContext));

    NvU64 kvIndex = NVKV_END(&pKVContext);
    *pOutCount = (NvU32)kvIndex;

    NV_ASSERT_OR_RETURN(kvIndex <= kvLimit, NV_ERR_BUFFER_TOO_SMALL);
    #undef NVKV_PREFIX

    return NV_OK;
}


/*!
 * Prepare data for the init RPC GspSetSystemInfo (GMCAPI NVKV path).
 *
 * Allocates the NVKV buffer and calls _kgspEncodeSystemInfo(), which gathers from GPU/RM state and
 * encodes in one pass (no intermediate GspSystemInfo struct).
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelGsp  KernelGsp object pointer
 *
 * @return NV_OK if successful
 */
static NV_STATUS
_kgspPrepareSystemInfo
(
    OBJGPU                *pGpu,
    KernelGsp             *pKernelGsp
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    pKernelGsp->encodedSystemInfoKeyLimit = 65536 / sizeof(NvU64);
    pKernelGsp->pEncodedSystemInfo = portMemAllocPaged(sizeof(NvU64) * pKernelGsp->encodedSystemInfoKeyLimit);
    if (pKernelGsp->pEncodedSystemInfo == NULL)
        return NV_ERR_NO_MEMORY;

    pKernelGsp->encodedSystemInfoCount = 0;

    status = _kgspEncodeSystemInfo(pGpu, pKernelGsp, pKernelGsp->pEncodedSystemInfo,
                                   pKernelGsp->encodedSystemInfoKeyLimit,
                                   &pKernelGsp->encodedSystemInfoCount);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to encode system info: 0x%x\n", status);
        portMemFree(pKernelGsp->pEncodedSystemInfo);
        pKernelGsp->pEncodedSystemInfo = NULL;
        pKernelGsp->encodedSystemInfoCount = 0;
        return status;
    }

    return NV_OK;
}

/*!
 * Prepare data for the init RPC SetRegistry
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelGsp  KernelGsp object pointer
 *
 * @return NV_OK if successful
 */
static NV_STATUS
_kgspPrepareRegistry
(
    OBJGPU                *pGpu,
    KernelGsp             *pKernelGsp
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Compute size of registry table
    status = osPackageRegistry(pGpu, NULL, &pKernelGsp->regTableSize);
    if (status != NV_OK)
        return status;

    pKernelGsp->pRegTable = portMemAllocPaged(pKernelGsp->regTableSize);
    if (pKernelGsp->pRegTable == NULL)
        return NV_ERR_NO_MEMORY;

    status = osPackageRegistry(pGpu, pKernelGsp->pRegTable, &pKernelGsp->regTableSize);
    if (status != NV_OK)
    {
        portMemFree(pKernelGsp->pRegTable);
        pKernelGsp->pRegTable = NULL;
        return status;
    }

    return status;
}

/*!
 * Send init RPCs necessary for GSP-RM early boot before OBJGPU is created.
 * GSP-RM will keep polling until these messages are sent.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelGsp  KernelGsp object pointer
 *
 * @return NV_OK if RPCs queued successfully.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspSendInitRpcs_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status;
    NvU32 maxStaticInfoDataSize = 48 * 1024;
    NvLength responseSize = maxStaticInfoDataSize;
    NvU64 *pKVStaticInfoData = (NvU64 *)portMemAllocNonPaged(maxStaticInfoDataSize);
    NV_ASSERT_OR_RETURN(pKVStaticInfoData != NULL, NV_ERR_NO_MEMORY);

    threadStateResetTimeout(pGpu);

    status = gmcapiCommand(pGpu, GMCAPI_CMD_GSP_INIT,
                           pKernelGsp->pEncodedSystemInfo,
                           sizeof(NvU64) * pKernelGsp->encodedSystemInfoCount,
                           pKVStaticInfoData, &responseSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GMCAPI_CMD_GSP_INIT failed: 0x%x\n", status);
        goto done;
    }

    status = _kgspDecodeGspStaticInfo(pGpu, pKVStaticInfoData,
                                      responseSize / sizeof(NvU64));
    if (status != NV_OK)
        goto done;

    pGpu->gspRmInitialized = NV_TRUE;
    pKernelGsp->bGspRmUnloaded = NV_FALSE;

    if (kgspIsHeartbeatSupported(pGpu))
    {
        _kgspHeartbeatInit(pGpu, pKernelGsp);
    }

    if (pKernelGsp->pExternalBindata != NULL)
    {
        memdescFree(pKernelGsp->pExternalBindata);
        memdescDestroy(pKernelGsp->pExternalBindata);
        pKernelGsp->pExternalBindata = NULL;
    }

done:
    portMemFree(pKVStaticInfoData);
    return status;
}

static NvBool
_kgspShouldRelaxGspInitLocking
(
    OBJGPU *pGpu
)
{
    NvU32 relaxGspInitLockingReg;

    if (!RMCFG_FEATURE_PLATFORM_UNIX)
    {
        return NV_FALSE;
    }

    Spdm *pSpdm = GPU_GET_SPDM(pGpu);
    if (pSpdm != NULL && pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
    {
        return NV_FALSE;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING, &relaxGspInitLockingReg) != NV_OK)
    {
        relaxGspInitLockingReg = NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_DEFAULT;
    }

    if ((relaxGspInitLockingReg == NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_DEFAULT) ||
        (relaxGspInitLockingReg == NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_ENABLE))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NV_STATUS
_kgspBootReacquireLocks(OBJGPU *pGpu, KernelGsp *pKernelGsp, GPU_MASK *pGpusLockedMask)
{
    //
    // To follow lock order constraints, GPU lock needs to be released before acquiring API lock
    // As this path doesn't go through resource server, no client locks should be held at this point.
    // Note: we must not hold any client locks when re-acquiring the API per lock ordering
    //
    rmGpuGroupLockRelease(*pGpusLockedMask, GPUS_LOCK_FLAGS_NONE);
    *pGpusLockedMask = 0;

    //
    // rmapiLockAcquire should never fail on Linux if the API lock and GPU locks are not held.
    // Failure to acquire the API lock means the cleanup sequence will skipped since it is
    // unsafe without the lock.
    //
    NV_ASSERT_OK_OR_RETURN(rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT));

    //
    // This should never fail on Linux due to locks in the Unix layer.
    // This will need to be revisited when parallel init is enabled on other platforms.
    //
    NV_ASSERT_OR_RETURN(gpumgrIsGpuPointerAttached(pGpu), NV_ERR_INVALID_DEVICE);

    // Reqcquire the GPU lock released above.
    NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                                 GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT,
                                                 pGpusLockedMask));

    return NV_OK;
}

static NV_STATUS
_kgspBootGspRm(OBJGPU *pGpu, KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw, GPU_MASK *pGpusLockedMask, NvU8 *pbRetry)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NV_STATUS status;
    NvBool bEccDisabled = !kmemsysCheckReadoutEccEnablement(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    NV_ASSERT_OR_RETURN(pbRetry != NULL, NV_ERR_INVALID_ARGUMENT);
    *pbRetry = NV_FALSE;

    // Fail early if WPR2 is up
    if (kgspIsWpr2Up_HAL(pGpu, pKernelGsp) &&
        (!pGpu->getProperty(pGpu, PDB_PROP_GPU_PREINITIALIZED_WPR_REGION)))
    {
        NV_PRINTF(LEVEL_ERROR, "unexpected WPR2 already up, cannot proceed with booting GSP\n");
        NV_PRINTF(LEVEL_ERROR, "(the GPU is likely in a bad state and may need to be reset)\n");

        if (pKernelBif != NULL)
        {
            kbifCheckResetStatus_HAL(pGpu, pKernelBif);
        }

        return NV_ERR_INVALID_STATE;
    }

    // Populate WPR meta structure (requires knowing FB size on dGPU, which depends on GFW_BOOT)
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgspPopulateWprMeta_HAL(pGpu, pKernelGsp, pGspFw));

    {
        // If the new FB layout requires a scrubber ucode to scrub additional space, prepare it now
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgspPrepareScrubberImageIfNeeded_HAL(pGpu, pKernelGsp));
    }

    // Setup arguments for bootstrapping GSP
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgspPrepareForBootstrap_HAL(pGpu, pKernelGsp, KGSP_BOOT_MODE_NORMAL));

    // Release the API lock if relaxed locking for parallel init is enabled
    NvBool bRelaxedLocking = _kgspShouldRelaxGspInitLocking(pGpu);
    if (bRelaxedLocking)
        rmapiLockRelease();

    if ((pKernelGsp->bootAttempts > 0) && bEccDisabled)
    {
        NvU32 bScanWprEndMargin;
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN, &bScanWprEndMargin) != NV_OK)
            bScanWprEndMargin = NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN_DEFAULT;

        if (bScanWprEndMargin)
        {
            pKernelGsp->pGspArgumentsCached->flags |= GSP_ARGUMENTS_FLAG_SCAN_RECOVERY_MARGIN;
        }
    }

    // Proceed with GSP boot
    status = kgspBootstrap_HAL(pGpu, pKernelGsp, KGSP_BOOT_MODE_NORMAL);

    if (status != NV_OK && !pGpu->pGpuArch->bGpuArchIsZeroFb)
    {
        // Increment the bootAttempt counter only on failure to boot GSP
        pKernelGsp->bootAttempts++;
        if (gpuCheckEccCounts_HAL(pGpu, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE) || bEccDisabled)
        {
            *pbRetry = NV_TRUE;

            // Persistently track next attempt so that subsequent boots can pick up where it left off.
            if (bEccDisabled)
                osWriteRegistryDword(pGpu, NV_REG_STR_RM_GSP_BOOT_INITIAL_SHIFT, pKernelGsp->bootAttempts);
        }
    }

    //
    // The caller will check that both the API lock and the GPU lock will be held upon return from
    // this function, regardless of whether GSP bootstrap succeeded.
    //
    if (bRelaxedLocking)
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              _kgspBootReacquireLocks(pGpu, pKernelGsp, pGpusLockedMask));

    return status;
}

/*!
 * Initialize GSP-RM
 *
 * @param[in]      pGpu          GPU object pointer
 * @param[in]      pKernelGsp    KernelGsp object pointer
 * @param[in]      pGspFw        GSP firmware structure pointer
 *
 * @return NV_OK if GSP fw RM offload successfully initialized.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspInitRm_IMPL
(
    OBJGPU       *pGpu,
    KernelGsp    *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    NV_STATUS  status = NV_OK;
    OBJTMR    *pTmr = GPU_GET_TIMER(pGpu);
    GPU_MASK   gpusLockedMask = 0;
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    if (!IS_GSP_CLIENT(pGpu))
        return NV_OK;

    if ((pGspFw == NULL) || (pGspFw->pBuf == NULL) || (pGspFw->size == 0))
    {
        NV_PRINTF(LEVEL_ERROR, "need firmware to initialize GSP\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKernelGsp->bInInit = NV_TRUE;

    // Need to hold the GPU instance lock in order to write to the RPC queue
    NV_ASSERT_OK_OR_GOTO(status,
        rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                              GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT, &gpusLockedMask),
        done);

    /*
     * For GSP-RM boot, we must trigger FRTS (if it exists for the chip)
     * before loading GSP-RM so that FRTS data and GSP-RM code/data/heap can coexist
     * in WPR2. FRTS is triggered by running a VBIOS-provided ucode called FWSEC.
     *
     * Here, we extract a VBIOS image from ROM, and parse it for FWSEC.
     */
    if (pKernelGsp->pFwsecUcode == NULL)
    {
        KernelGspVbiosImg *pVbiosImg = NULL;

        // Start VBIOS version string as "unknown"
        portStringCopy(pKernelGsp->vbiosVersionStr, sizeof(pKernelGsp->vbiosVersionStr), "unknown", sizeof("unknown"));

        // Try and extract a VBIOS image.
        status = kgspExtractVbiosFromRom_HAL(pGpu, pKernelGsp, &pVbiosImg);

        if (status == NV_OK)
        {
            NvU64 vbiosVersionCombined = 0;

            // Got a VBIOS image, now parse it for FWSEC.
            status = kgspParseFwsecUcodeFromVbiosImg(pGpu, pKernelGsp, pVbiosImg,
                                                     &pKernelGsp->pFwsecUcode, &vbiosVersionCombined);
            kgspFreeVbiosImg(pVbiosImg);

            if (vbiosVersionCombined > 0)
            {
                _kgspVbiosVersionToStr(vbiosVersionCombined, pKernelGsp->vbiosVersionStr, sizeof(pKernelGsp->vbiosVersionStr));
            }

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to parse FWSEC ucode from VBIOS image (VBIOS version %s): 0x%x\n",
                          pKernelGsp->vbiosVersionStr, status);
                goto done;
            }

            NV_PRINTF(LEVEL_INFO, "parsed VBIOS version %s\n", pKernelGsp->vbiosVersionStr);
        }
        else if (status == NV_ERR_NOT_SUPPORTED)
        {
            // Extracting VBIOS image from ROM is not supported.
            status = NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "failed to extract VBIOS image from ROM: 0x%x\n",
                        status);
            goto done;
        }

    }

    /*
     * We use a set of Booter ucodes to boot GSP-RM as well as manage its lifecycle.
     *
     * Booter Load loads, verifies, and boots GSP-RM in WPR2.
     * Booter Unload tears down WPR2 for driver unload.
     *
     * Here we prepare the Booter ucode images in SYSMEM so they may be loaded onto
     * SEC2 (Load / Unload) and NVDEC0 (Unload).
     */
    if (pKernelGsp->bPartitionedFmc)
    {
        //
        // The secure boot ucode is included in the partitioned FMC, no need for
        // separate Booter ucodes.
        //
    }
    else
    {
        if (pKernelGsp->pBooterLoadUcode == NULL)
        {
            status = kgspAllocateBooterLoadUcodeImage(pGpu, pKernelGsp,
                                                      &pKernelGsp->pBooterLoadUcode);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to allocate Booter Load ucode: 0x%x\n", status);
                goto done;
            }
        }

        if (pKernelGsp->pBooterUnloadUcode == NULL)
        {
            status = kgspAllocateBooterUnloadUcodeImage(pGpu, pKernelGsp,
                                                        &pKernelGsp->pBooterUnloadUcode);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to allocate Booter Unload ucode: 0x%x\n", status);
                goto done;
            }
        }
    }

    // Prepare boot binary image.
    status = kgspPrepareBootBinaryImage(pGpu, pKernelGsp);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error preparing boot binary image\n");
        goto done;
    }

    // Prepare GSP-RM image.
    status = _kgspPrepareGspRmBinaryImage(pGpu, pKernelGsp, pGspFw);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error preparing GSP-RM image\n");
        goto done;
    }

    // Init logging memory used by GSP
    status = _kgspInitLibosLoggingStructures(pGpu, pKernelGsp);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init libos logging structures failed: 0x%x\n", status);
        goto done;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _kgspInitLibosLogDecoder(pGpu, pKernelGsp, pGspFw), done);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, nvlogRegisterFlushCb(kgspNvlogFlushCb, pKernelGsp), done);

    // Reset thread state timeout and wait for GFW_BOOT OK status
    threadStateResetTimeout(pGpu);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp), done);

    //
    // Set the GPU time to the wall-clock time after GFW boot is complete
    // (to avoid PLM collisions) but before loading GSP-RM ucode (which
    // consumes the updated GPU time).
    //
    tmrSetCurrentTime_HAL(pGpu, pTmr);

    libosLogUpdateTimerDelta(&pKernelGsp->logDecode, pTmr->sysTimerOffsetNs);

    // Check if FSP has clock boost capability
    if (pKernelFsp != NULL)
    {
        kfspCheckForClockBoostCapability_HAL(pGpu, pKernelFsp);
    }

    // Allocate rm dump buffer
    NV_CHECK_OK(status, LEVEL_ERROR, kgspAllocRmStateMonitorBuffer(pGpu, pKernelGsp));

    // Initialize libos init args list
    kgspSetupLibosInitArgs(pGpu, pKernelGsp);

    // Fill in the GSP-RM message queue init parameters
    kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, NULL);

    // The init calls are to be send right after gsp boots, which may not have rmapi lock due to
    // parallel GSP bootstrapping. The data of the init calls require global data structure
    // so the preparation of the data is done here while still having rmapi lock
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _kgspPrepareRegistry(pGpu, pKernelGsp), done);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _kgspPrepareSystemInfo(pGpu, pKernelGsp), done);

    //
    // Bring up ucode with RM offload task.
    // If an ECC error occurs which results in the failure of the bootstrap, try again.
    // Subsequent attempts will shift the GSP region in an attempt to avoid the
    // unstable memory.
    //
    NvU32 maxGspBootAttempts;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_BOOT_RETRY_ATTEMPTS, &maxGspBootAttempts) != NV_OK)
    {
        maxGspBootAttempts = NV_REG_STR_RM_GSP_BOOT_RETRY_ATTEMPTS_DEFAULT;

    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_BOOT_INITIAL_SHIFT, &pKernelGsp->bootAttempts) != NV_OK)
    {
        pKernelGsp->bootAttempts = NV_REG_STR_RM_GSP_BOOT_INITIAL_SHIFT_DEFAULT;
    }
    else if (pKernelGsp->bootAttempts >= maxGspBootAttempts)
    {
        NV_PRINTF(LEVEL_ERROR, "Initial shift, %u, is larger than max allowed [0, %u]. Modulo applied\n",
                  pKernelGsp->bootAttempts, maxGspBootAttempts - 1);
        pKernelGsp->bootAttempts = pKernelGsp->bootAttempts % maxGspBootAttempts;
    }

    NvBool bRetry = NV_FALSE;
    do
    {
        // Reset the thread state timeout after failed attempts to prevent premature timeouts.
        if (status != NV_OK)
            threadStateResetTimeout(pGpu);

        if (pKernelGsp->bootAttempts > 0)
        {
            NV_PRINTF(LEVEL_WARNING, "GSP-RM boot attempt #%u of %u\n", pKernelGsp->bootAttempts, maxGspBootAttempts);
        }

        //
        // _kgspBootGspRm() will set bRetry to NV_TRUE if an unhandled error
        // results in a failed GSP boot attempt and we are permitted to retry.
        // Depending on where and when the error occurred, subsequent boot
        // attempts may fail.
        //
        status = _kgspBootGspRm(pGpu, pKernelGsp, pGspFw, &gpusLockedMask, &bRetry);

        //
        // _kgspBootGspRm() may temporarily release locks to facilitate parallel GSP bootstrap on
        // other GPUs. It is responsible for reacquiring them in the proper order. If there is a
        // failure to reacquire locks, it is unsafe to continue, regardless of the initialization
        // status - so we return immediately here, rather attempting cleanup.
        //
        // Note: _kgspBootGspRm() is structured such that gpusLockedMask will always be 0 (no GPU
        //       locks held) if the API lock is not held upon return.
        //
        NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && (gpusLockedMask != 0),
                            NV_ERR_INVALID_LOCK_STATE);
    } while (bRetry && (pKernelGsp->bootAttempts < maxGspBootAttempts));

    if (status != NV_OK)
    {
        if (status == NV_ERR_INSUFFICIENT_POWER)
        {
            OBJSYS *pSys = SYS_GET_INSTANCE();
            OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

            pGpuMgr->powerDisconnectedGpuBus[pGpuMgr->powerDisconnectedGpuCount++] = gpuGetBus(pGpu);
        }

        if (pKernelGsp->bootAttempts >= maxGspBootAttempts)
        {
            NV_PRINTF(LEVEL_ERROR, "Max GSP-RM boot attempts exceeded: %u/%u\n",
                      pKernelGsp->bootAttempts, maxGspBootAttempts);
        }

        //
        // Ignore return value - a crash report may have already been consumed,
        // this is just here as a last attempt to report boot issues that might
        // have escaped prior checks.
        //
        (void)kgspHealthCheck_HAL(pGpu, pKernelGsp);
        goto done;
    }

    NV_ASSERT_OK_OR_GOTO(status, kgspInitRatsCrashBuffer(pGpu, pKernelGsp), done);

    // Set PDB properties as per data from GSP.
    _kgspInitGpuProperties(pGpu);

    // Populate srRegionsInfo from per-arch sources (pWprMeta + GSP static info on Hopper+).
    kgspPopulateSrRegionsInfo_HAL(pGpu, pKernelGsp);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspStartLogPolling(pGpu, pKernelGsp), done);

    // schedule the callback for periodic GSP health check when enabled via regkey
    if (pKernelGsp->gspPeriodicHealthcheck == NV_REG_STR_RM_GSP_PERIODIC_HEALTHCHECK_ENABLE)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _kgspRegisterHealthCheckCallback(pGpu, pKernelGsp), done);
    }

done:
    // We don't need to tie up this memory after init is complete, so free it now.
    portMemFree(pKernelGsp->pEncodedSystemInfo);
    pKernelGsp->pEncodedSystemInfo = NULL;

    portMemFree(pKernelGsp->pRegTable);
    pKernelGsp->pRegTable = NULL;

    pKernelGsp->bInInit = NV_FALSE;

    if (status != NV_OK)
    {
        KernelPmu *pKernelPmu = GPU_GET_KERNEL_PMU(pGpu);

        // Force sync GSP logs
        kgspDumpGspLogs(pKernelGsp, NV_TRUE);

        // Preserve any captured GSP-RM logs
        libosPreserveLogs(&pKernelGsp->logDecode,
                          /* bPreserveNoWrap = */ NV_TRUE,
                          /* bPreserveWrap = */   NV_FALSE);

        if (pKernelPmu != NULL)
        {
            // If PMU init fails, kgsp init will also fail
            libosPreserveLogs(&pKernelPmu->logDecode,
                              /* bPreserveNoWrap = */ NV_TRUE,
                              /* bPreserveWrap = */   NV_FALSE);
        }
    }

    if (gpusLockedMask != 0)
    {
        rmGpuGroupLockRelease(gpusLockedMask, GPUS_LOCK_FLAGS_NONE);
    }

    return status;
}

/*!
 * Unload GSP-RM
 */
NV_STATUS
kgspUnloadRm_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspUnloadMode unloadMode,
    NvU32 newPmLevel
)
{
    NV_STATUS rpcStatus = NV_OK;
    NV_STATUS status;

    if (pKernelGsp->bGspRmForceUnloaded)
    {
        NV_PRINTF(LEVEL_ERROR, "skipping attempt to unload GSP-RM after force unload\n");
        return NV_OK;
    }

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    kgspCollectGspInstrumentation(pGpu, pKernelGsp);
#endif

    GmcApiGspSuspendParams suspendReq = { 0 };
    if (unloadMode != KGSP_UNLOAD_MODE_NORMAL)
        suspendReq.flags |= GMCAPI_GSP_SUSPEND_FLAGS_PM_TRANSITION;
    if (unloadMode == KGSP_UNLOAD_MODE_GC6_ENTER)
        suspendReq.flags |= GMCAPI_GSP_SUSPEND_FLAGS_GC6_ENTERING;
    if (newPmLevel & GPU_STATE_FLAGS_FAST_UNLOAD)
        suspendReq.flags |= GMCAPI_GSP_SUSPEND_FLAGS_FAST;
    if (newPmLevel & GPU_STATE_FLAGS_FORCE_GSP_UNLOAD)
        suspendReq.flags |= GMCAPI_GSP_SUSPEND_FLAGS_FORCE;

    //
    // bug 6115674: newPmLevel is overloaded to either GPU_STATE_FLAGS_* or
    // NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_*, both of which occupy the
    // same bit range, and the only way to know which is valid is via unloadMode.
    //
    if (unloadMode != KGSP_UNLOAD_MODE_NORMAL && newPmLevel == NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7)
        suspendReq.flags |= GMCAPI_GSP_SUSPEND_FLAGS_HIBERNATE;

    NV_PRINTF(LEVEL_NOTICE, "unloading GSP-RM (suspend flags 0x%llx)\n", suspendReq.flags);

    // fire-and-forget RPC, control returns immediately
    rpcStatus = gmcapiCommand(pGpu, GMCAPI_CMD_GSP_SUSPEND,
                              &suspendReq, (NvU32)sizeof(suspendReq),
                              NULL, NULL);

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        // FIPS: If CC enabled, we need to confirm GSP-RM was able to teardown CC state.
        kgspCheckGspRmCcCleanup_HAL(pGpu, pKernelGsp);
    }

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    // Bullseye/instrumented builds can add significant overhead to GSP RPC waits.
    (void)threadStateResetTimeout(pGpu);
#endif

    // Wait for GSP-RM processor to suspend
    NV_CHECK_OK(status, LEVEL_WARNING,
        kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp, NV_TRUE));

    //
    // Mark GSP-RM as unloaded before any further teardown so the periodic
    // health check stops touching the GSP mailboxes. Cleared again on
    // resume in kgspWaitForRmResumeDone_IMPL.
    //
    pKernelGsp->bGspRmUnloaded = NV_TRUE;

    // Disable heartbeat checking for GSP-RM and Libos
    _kgspHeartbeatDisable(pGpu, pKernelGsp);

    // Dump GSP-RM logs and reset before proceeding with the rest of teardown
    kgspDumpGspLogs(pKernelGsp, (pKernelGsp->preserveLogs != NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS_DISABLE));

    // Without this call, NVLOG buffers don't persist across GPU destruct, so logs would be lost
    if (pKernelGsp->preserveLogs != NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS_DISABLE)
    {
        //
        // Debug WAR for Bug 5312623
        // Wrap buffers preserved here during unload aren't re-used when NVLOG re-initializes during next load.
        // Instead, a new wrap buffer is allocated and used. If the driver unloads a second/nth time, the old
        // preserved wrap buffer is deleted and the working wrap buffer becomes preserved. Thus, only the
        // wrap buffer from the last unload with NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS set will be preserved.
        //
        libosPreserveLogs(&pKernelGsp->logDecode,
                          ((pKernelGsp->preserveLogs & NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS_ENABLE_NO_WRAP) != 0),
                          ((pKernelGsp->preserveLogs & NV_REG_STR_RM_GSP_PRESERVE_UNLOAD_LOGS_ENABLE_WRAP) != 0));
    }


    // Teardown remaining GSP state
    status = kgspTeardown_HAL(pGpu, pKernelGsp, unloadMode);

    //
    // To fix boot issue after GPU reset on ESXi config:
    // We still do not have root cause but looks like some sanity is failing during boot after reset is done.
    // As temp WAR, add delay of 250 ms after gsp rm unload is done.
    // Limit this to [VGPU-GSP] supported configs only and when we are in GPU RESET path.
    //
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) &&
        gpuIsSriovEnabled(pGpu) &&
        IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        osDelay(250);
    }

    if (rpcStatus != NV_OK)
    {
        return rpcStatus;
    }

    return status;
}

/*!
 * Free RPC infrastructure and KernelGsp object
 */
void
kgspDestruct_IMPL
(
    KernelGsp *pKernelGsp
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelGsp);

    if (!IS_GSP_CLIENT(pGpu))
        return;

    // set VBIOS version string back to "unknown"
    portStringCopy(pKernelGsp->vbiosVersionStr, sizeof(pKernelGsp->vbiosVersionStr), "unknown", sizeof("unknown"));

    kgspFreeFlcnUcode(pKernelGsp->pFwsecUcode);
    pKernelGsp->pFwsecUcode = NULL;

    kgspFreeFlcnUcode(pKernelGsp->pBooterLoadUcode);
    pKernelGsp->pBooterLoadUcode = NULL;

    kgspFreeFlcnUcode(pKernelGsp->pBooterUnloadUcode);
    pKernelGsp->pBooterUnloadUcode = NULL;

    kgspFreeFlcnUcode(pKernelGsp->pScrubberUcode);
    pKernelGsp->pScrubberUcode = NULL;

    kgspFreeBootArgs_HAL(pGpu, pKernelGsp);

    _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);
    _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    _kgspFreeBootBinaryImage(pGpu, pKernelGsp);
    _kgspFreeSimAccessBuffer(pGpu, pKernelGsp);
    _kgspFreeNotifyOpSharedSurface(pGpu, pKernelGsp);

    kgspFreeRatsCrashBuffer(pGpu, pKernelGsp);

    kgspFreeRmStateMonitorBuffer(pGpu, pKernelGsp);

    kgspFreeSuspendResumeData_HAL(pGpu, pKernelGsp);
}

static void
_kgspDumpGspLogsUnlocked
(
    KernelGsp *pKernelGsp,
    NvBool bSyncNvLog
)
{
    libosExtractLogs(&pKernelGsp->logDecode, bSyncNvLog);

    if (pKernelGsp->bHasVgpuLogs)
    {
        // Dump logs from vGPU partition
        for (NvU32 i = 0; i < MAX_PARTITIONS_WITH_GFID; i++)
        {
            libosExtractLogs(&pKernelGsp->logDecodeVgpuPartition[i], bSyncNvLog);
        }
    }
}

/*!
 * Dump logs coming from GSP-RM
 *
 * @param[in] pKernelGsp    KernelGsp pointer
 * @param[in] bSyncNvLog    NV_TRUE: Copy a snapshot of the libos logs
 *                          into the nvLog wrap buffers.
 */
void
kgspDumpGspLogs_IMPL
(
    KernelGsp *pKernelGsp,
    NvBool bSyncNvLog
)
{
    if (RMCFG_FEATURE_RM_NEW_TRACER_ETW ||
        pKernelGsp->bInInit || pKernelGsp->pLogElf || bSyncNvLog ||
        pKernelGsp->bHasVgpuLogs)
    {
        while (!portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 1, 0))
        {
            if (osIsRaisedIRQL())
            {
                // called at DPC/ISR and there is contention, just bail
                return;
            }

            osSpinLoop();
        }

        _kgspDumpGspLogsUnlocked(pKernelGsp, bSyncNvLog);

        portAtomicCompareAndSwapS32(&pKernelGsp->logDumpLock, 0, 1);
    }
}

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
/*!
 * Dump coverage coming from GSP-RM
 *
 * @param[in] pKernelGsp    KernelGsp pointer
 *
 */
void
kgspCollectGspInstrumentation_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status;
    NV2080_CTRL_INTERNAL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pParams;
    NvU32 offset = 0;
    NvU32 chunkCount = 0;

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for coverage collection\n");
        return;
    }

    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->ucode = NV208F_UCODE_INSTRUMENTATION_GSP_TASK_RM;
    pParams->gfid = GFID_TASK_RM;
    pParams->instrumentationType = NV208F_BULLSEYE_COVERAGE_TYPE;

    do
    {
        pParams->offset = offset;
        pParams->dataSize = 0;
        pParams->bComplete = NV_FALSE;

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_UCODE_INSTRUMENTATION_GET_DATA,
                                 pParams,
                                 sizeof(*pParams));

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to collect coverage chunk at offset 0x%x: 0x%x\n",
                      offset, status);
            break;
        }

        if (pParams->dataSize == 0)
        {
            break;
        }

        // Merge this chunk into the instrumentation buffer
        instrumentationmanagerMerge(pSys->pInstrumentationManager,
                                    GFID_TASK_RM,
                                    pGpu->gpuInstance,
                                    pParams->data,
                                    offset,
                                    pParams->dataSize);

        offset += pParams->dataSize;
        chunkCount++;

    } while (!pParams->bComplete);

    portMemFree(pParams);
}
#endif

/*!
 * Populate GSP-RM init arguments.
 */
void
kgspPopulateGspRmInitArgs_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    GSP_SR_INIT_ARGUMENTS *pGspInitArgs
)
{
    GSP_ARGUMENTS_CACHED *pGspArgs = pKernelGsp->pGspArgumentsCached;
    MESSAGE_QUEUE_INIT_ARGUMENTS *pMQInitArgs = &pGspArgs->messageQueueInitArguments;
    MESSAGE_QUEUE_COLLECTION *pMQCollection = pKernelGsp->pMQCollection;
    MESSAGE_QUEUE_INFO *pMQInfo = &pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX];
    GSP_SR_INIT_ARGUMENTS *pSrInitArgs = &pGspArgs->srInitArguments;

    portMemSet(pGspArgs, 0, sizeof(*pGspArgs));
    pGspArgs->magic = GSP_ARGUMENTS_MAGIC_VALUE;
    pGspArgs->size = sizeof(*pGspArgs);

    // Setup the message queue arguments
    pMQInitArgs->sharedMemPhysAddr      = pMQCollection->sharedMemPA;
    pMQInitArgs->pageTableEntryCount    = pMQCollection->pageTableEntryCount;
    pMQInitArgs->cmdQueueOffset         = pMQCollection->pageTableSize;
    pMQInitArgs->statQueueOffset        = pMQInitArgs->cmdQueueOffset + pMQInfo->commandQueueSize;
    pMQInitArgs->queueElementHdrSize    = pMQInfo->queueElementHdrSize;
    pMQInitArgs->queueElementSizeMin    = pMQInfo->queueElementSizeMin;
    pMQInitArgs->queueElementSizeMax    = pMQInfo->queueElementSizeMax;
    pMQInitArgs->queueHeaderAlign       = pMQInfo->queueHeaderAlign;
    pMQInitArgs->queueElementAlign      = pMQInfo->queueElementAlign;
    if (pMQInfo->bEncryptionEnabled)
        pMQInitArgs->flags |= MESSAGE_QUEUE_INIT_FLAG_ENCRYPTION;

    if (pGspInitArgs == NULL)
    {
        NvU32 stackReg;

        // For normal boot, determine whether to use dmem stack
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_STACK_PLACEMENT, &stackReg) != NV_OK)
        {
            stackReg = NV_REG_STR_RM_GSP_STACK_PLACEMENT_DEFAULT;
        }

        if (((stackReg == NV_REG_STR_RM_GSP_STACK_PLACEMENT_DEFAULT) ||
             (stackReg == NV_REG_STR_RM_GSP_STACK_PLACEMENT_DMEM)) &&
            !FLD_TEST_DRF(_REG_STR_RM, _GSP_PAGING_CONFIG, _RM_STACK, _ENABLED,
                          pKernelGsp->pagingConfig))
        {
            pGspArgs->flags |= GSP_ARGUMENTS_FLAG_STACK_IN_DMEM;
        }

        pSrInitArgs->oldLevel            = 0;
        pSrInitArgs->flags               = 0;
    }
    else
    {
        pGspArgs->flags                 |= GSP_ARGUMENTS_FLAG_IN_PM_TRANSITION;
        pSrInitArgs->oldLevel            = pGspInitArgs->oldLevel;
        pSrInitArgs->flags               = pGspInitArgs->flags;
    }

    pGspArgs->gpuInstance = pGpu->gpuInstance;

    portMemSet(&pGspArgs->profilerArgs, 0, sizeof(pGspArgs->profilerArgs));

    if (pKernelGsp->pProfilerSamples != NULL &&
        pKernelGsp->pProfilerSamplesMD != NULL)
    {
        pGspArgs->profilerArgs.pa = memdescGetPhysAddr(pKernelGsp->pProfilerSamplesMD, AT_GPU, 0);
        pGspArgs->profilerArgs.size = memdescGetSize(pKernelGsp->pProfilerSamplesMD);
    }

    if (pGpu->pGpuArch->bGpuArchIsZeroFb)
    {
        pGspArgs->sysmemHeapArgs.pa = memdescGetPhysAddr(pKernelGsp->pSysmemHeapDescriptor, AT_GPU, 0);
        pGspArgs->sysmemHeapArgs.size = pKernelGsp->pSysmemHeapDescriptor->Size;
    }
    else
    {
        pGspArgs->sysmemHeapArgs.pa = 0;
        pGspArgs->sysmemHeapArgs.size = 0;
    }

    // Setup the rm state monitor buffer arguments
    portMemSet(&pGspArgs->rmStateMonitorBufferArgs, 0, sizeof(pGspArgs->rmStateMonitorBufferArgs));

    if (pKernelGsp->pRmStateMonitorBuffer != NULL &&
        pKernelGsp->pRmStateMonitorBufferMD != NULL)
    {
        pGspArgs->rmStateMonitorBufferArgs.pa = memdescGetPhysAddr(pKernelGsp->pRmStateMonitorBufferMD, AT_GPU, 0);
        pGspArgs->rmStateMonitorBufferArgs.size = memdescGetSize(pKernelGsp->pRmStateMonitorBufferMD);
    }

    if (pKernelGsp->pExternalBindata != NULL)
    {
        pGspArgs->bindataArgs.radix3 = memdescGetPhysAddr(pKernelGsp->pExternalBindata, AT_GPU, 0);
        pGspArgs->bindataArgs.size = pKernelGsp->externalBindataSize;
    }
}

/*!
 * Prepare boot binary image for GSP-RM boot.
 *
 * @return NV_OK if boot binary image prepared successfully.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspPrepareBootBinaryImage_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status;
    BINDATA_STORAGE *pBinStorageImage;
    BINDATA_STORAGE *pBinStorageDesc;
    NvU32 bufSize;
    NvU32 bufSizeAligned;
    RM_RISCV_UCODE_DESC *pDesc = NULL;
    NvP64 pVa = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeImage == NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeDesc  == NULL, NV_ERR_INVALID_STATE);

    // get the bindata storage for the image/descriptor
    kgspGetGspRmBootUcodeStorage_HAL(pGpu, pKernelGsp, &pBinStorageImage, &pBinStorageDesc);

    // copy the image to sysmem
    bufSize = bindataGetBufferSize(pBinStorageImage);
    bufSizeAligned = NV_ALIGN_UP(bufSize, 0x1000);

    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }

    NV_ASSERT_OK_OR_GOTO(status,
                        memdescCreate(&pKernelGsp->pGspRmBootUcodeMemdesc,
                                pGpu,
                                bufSizeAligned,
                                RM_PAGE_SIZE,
                                NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                flags),
                        fail);

    memdescTagAlloc(status,
            NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_15, pKernelGsp->pGspRmBootUcodeMemdesc);
    NV_ASSERT_OK_OR_GOTO(status, status, fail);

    NV_ASSERT_OK_OR_GOTO(status,
                        memdescMap(pKernelGsp->pGspRmBootUcodeMemdesc, 0,
                                memdescGetSize(pKernelGsp->pGspRmBootUcodeMemdesc),
                                NV_TRUE, NV_PROTECT_READ_WRITE,
                                &pVa, &pPriv),
                        fail);

    pKernelGsp->gspRmBootUcodeSize   = bufSize;
    pKernelGsp->pGspRmBootUcodeImage = (NvU8 *)NvP64_VALUE(pVa);;
    pKernelGsp->pGspRmBootUcodeMemdescPriv = pPriv;

    NV_ASSERT_OK_OR_GOTO(status,
                        bindataWriteToBuffer(pBinStorageImage,
                               pKernelGsp->pGspRmBootUcodeImage,
                               bufSize),
                        fail);

    // get the image descriptor
    NV_ASSERT_OK_OR_GOTO(status,
                         bindataStorageAcquireData(pBinStorageDesc, (const void**)&pDesc),
                         fail);
    pKernelGsp->pGspRmBootUcodeDesc = pDesc;

    return status;

fail:
    _kgspFreeBootBinaryImage(pGpu, pKernelGsp);
    return status;
}

static void
_kgspFreeBootBinaryImage
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    bindataStorageReleaseData(pKernelGsp->pGspRmBootUcodeDesc);
    pKernelGsp->pGspRmBootUcodeDesc  = NULL;

    if (pKernelGsp->pGspRmBootUcodeImage != NULL)
    {
        memdescUnmap(pKernelGsp->pGspRmBootUcodeMemdesc,
                     NV_TRUE,
                     (void *)pKernelGsp->pGspRmBootUcodeImage,
                     pKernelGsp->pGspRmBootUcodeMemdescPriv);
        pKernelGsp->pGspRmBootUcodeImage = NULL;
        pKernelGsp->pGspRmBootUcodeMemdescPriv = NULL;
    }
    if (pKernelGsp->pGspRmBootUcodeMemdesc != NULL)
    {
        memdescFree(pKernelGsp->pGspRmBootUcodeMemdesc);
        memdescDestroy(pKernelGsp->pGspRmBootUcodeMemdesc);
        pKernelGsp->pGspRmBootUcodeMemdesc = NULL;
    }

    pKernelGsp->gspRmBootUcodeSize   = 0;
}

static NV_STATUS
_kgspCreateSignatureMemdesc
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    NV_STATUS status = NV_OK;
    NvU8 *pSignatureVa = NULL;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }

    // NOTE: align to 256 because that's the alignment needed for Booter DMA
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pKernelGsp->pSignatureMemdesc, pGpu,
            NV_ALIGN_UP(pGspFw->signatureSize, 256), 256,
            NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED, flags));

    memdescTagAlloc(status,
            NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_16, pKernelGsp->pSignatureMemdesc);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, fail_create);

    pSignatureVa = memdescMapInternal(pGpu, pKernelGsp->pSignatureMemdesc, TRANSFER_FLAGS_NONE);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        (pSignatureVa != NULL) ? NV_OK : NV_ERR_INSUFFICIENT_RESOURCES,
        fail_alloc);

    portMemCopy(pSignatureVa, memdescGetSize(pKernelGsp->pSignatureMemdesc),
        pGspFw->pSignatureData, pGspFw->signatureSize);

    memdescUnmapInternal(pGpu, pKernelGsp->pSignatureMemdesc, 0);
    pSignatureVa = NULL;

    return status;

fail_alloc:
    memdescFree(pKernelGsp->pSignatureMemdesc);

fail_create:
    memdescDestroy(pKernelGsp->pSignatureMemdesc);
    pKernelGsp->pSignatureMemdesc = NULL;

    return status;
}

/*!
 * Verify that the version embedded in the .fwversion section of the ELF given
 * by pElfData and elfDataSize matches our NV_VERSION_STRING.
 */
static NV_STATUS
_kgspFwContainerVerifyVersion
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const void *pElfData,
    NvU64 elfDataSize,
    const char *pNameInMsg
)
{
    const char *pFwversion;
    NvU64 fwversionSize;
    NvBool bIsVersionValid;

    {
        const void *pFwversionRaw;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _kgspFwContainerGetSection(pGpu, pKernelGsp,
                pElfData,
                elfDataSize,
                GSP_VERSION_SECTION_NAME,
                &pFwversionRaw,
                &fwversionSize));

        pFwversion = (const char *) pFwversionRaw;
    }

    // Sanity check .fwversion before attempting to print it
    bIsVersionValid =
        ((fwversionSize > 0) &&
         (fwversionSize < 64) &&
         (pFwversion[fwversionSize - 1] == '\0'));

    NvU64 expectedVersionLength = portStringLength(NV_VERSION_STRING);

    // Check that text in .fwversion section of ELF matches our NV_VERSION_STRING
    if ((fwversionSize != expectedVersionLength + 1) ||
        (portStringCompare(pFwversion, NV_VERSION_STRING, expectedVersionLength) != 0))
    {
        if (bIsVersionValid)
        {
            NV_PRINTF(LEVEL_ERROR, "%s version mismatch: got version %s, expected version %s\n",
                      pNameInMsg, pFwversion, NV_VERSION_STRING);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "%s version unknown or malformed, expected version %s\n",
                      pNameInMsg, NV_VERSION_STRING);
        }

        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

/*!
 * Get the name of the section corresponding to the given section name
 * prefix and the current chip.
 */
static NV_STATUS
_kgspGetSectionNameForPrefix
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    char *pSectionNameBuf,  // out
    NvLength sectionNameBufSize,
    const char *pSectionPrefix
)
{
    NvLength sectionPrefixLength;

    nv_firmware_chip_family_t chipFamily;
    const char *pChipFamilyName;
    NvLength chipFamilyNameLength;

    NvLength totalSize;

    NV_ASSERT_OR_RETURN(pSectionNameBuf != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(sectionNameBufSize > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSectionPrefix != NULL, NV_ERR_INVALID_ARGUMENT);

    chipFamily = nv_firmware_get_chip_family(gpuGetChipArch(pGpu),
                                             gpuGetChipImpl(pGpu));
    NV_ASSERT_OR_RETURN(chipFamily != NV_FIRMWARE_CHIP_FAMILY_NULL,
                        NV_ERR_INVALID_STATE);

    pChipFamilyName = nv_firmware_chip_family_to_string(chipFamily);
    NV_ASSERT_OR_RETURN(portStringLength(pChipFamilyName) != 0, NV_ERR_INVALID_STATE);

    sectionPrefixLength = portStringLength(pSectionPrefix);
    chipFamilyNameLength = portStringLength(pChipFamilyName);

    totalSize = sectionPrefixLength + chipFamilyNameLength + 1;
    NV_ASSERT_OR_RETURN(sectionNameBufSize >= sectionPrefixLength + 1,
                        NV_ERR_BUFFER_TOO_SMALL);
    NV_ASSERT_OR_RETURN(sectionNameBufSize >= totalSize,
                        NV_ERR_BUFFER_TOO_SMALL);

    portStringCopy(pSectionNameBuf, sectionNameBufSize,
                   pSectionPrefix, sectionPrefixLength + 1);
    portStringCat(pSectionNameBuf, sectionNameBufSize,
                  pChipFamilyName, chipFamilyNameLength + 1);

    return NV_OK;
}

static NV_STATUS
_kgspPrepareGspRmBinaryImage
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    char signatureSectionName[32];
    const void *pFwVersionSectionData = NULL;
    NvU64 fwVersionSectionSize = 0;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspFwContainerVerifyVersion(pGpu, pKernelGsp,
            pGspFw->pBuf,
            pGspFw->size,
            "GSP firmware image"));

    portMemSet(pKernelGsp->gspRmFwVersionStr, 0,
               sizeof(pKernelGsp->gspRmFwVersionStr));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspFwContainerGetSection(pGpu, pKernelGsp,
            pGspFw->pBuf,
            pGspFw->size,
            GSP_VERSION_SECTION_NAME,
            &pFwVersionSectionData,
            &fwVersionSectionSize));

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (fwVersionSectionSize > 0) &&
        (((const char *)pFwVersionSectionData)[fwVersionSectionSize - 1] == '\0'),
        NV_ERR_INVALID_DATA);

    portStringCopy(pKernelGsp->gspRmFwVersionStr,
                   sizeof(pKernelGsp->gspRmFwVersionStr),
                   (const char *)pFwVersionSectionData,
                   (NvLength)fwVersionSectionSize);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspFwContainerGetSection(pGpu, pKernelGsp,
            pGspFw->pBuf,
            pGspFw->size,
            GSP_IMAGE_SECTION_NAME,
            &pGspFw->pImageData,
            &pGspFw->imageSize));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspGetSectionNameForPrefix(pGpu, pKernelGsp,
            signatureSectionName, sizeof(signatureSectionName),
            kgspGetSignatureSectionNamePrefix_HAL(pGpu, pKernelGsp)));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspFwContainerGetSection(pGpu, pKernelGsp,
            pGspFw->pBuf,
            pGspFw->size,
            signatureSectionName,
            &pGspFw->pSignatureData,
            &pGspFw->signatureSize));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspCreateSignatureMemdesc(pGpu, pKernelGsp,
            pGspFw));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgspCreateRadix3(pGpu, pKernelGsp, &pKernelGsp->pGspUCodeRadix3Descriptor,
            NULL, pGspFw->pImageData, pGspFw->imageSize));

    if (pGspFw->pUcodesBin)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgspCreateRadix3(pGpu, pKernelGsp, &pKernelGsp->pExternalBindata,
                NULL, pGspFw->pUcodesBin, pGspFw->ucodesBinSize));

        pKernelGsp->externalBindataSize = pGspFw->ucodesBinSize;
    }

    {
        // read build-id
        const void *pBuildIdSectionData = NULL;
        NvU64 buildIdSectionSize = 0;

        NV_ASSERT_OK_OR_RETURN(
            _kgspFwContainerGetSection(pGpu, pKernelGsp,
                pGspFw->pBuf,
                pGspFw->size,
                GSP_BUILD_ID_SECTION_NAME,
                &pBuildIdSectionData,
                &buildIdSectionSize));

        pKernelGsp->pBuildIdSection = portMemAllocNonPaged(buildIdSectionSize);

        NV_ASSERT_OR_RETURN(pKernelGsp->pBuildIdSection != NULL, NV_ERR_NO_MEMORY);

        pKernelGsp->buildIdSectionSize = buildIdSectionSize;

        portMemCopy(pKernelGsp->pBuildIdSection, buildIdSectionSize, pBuildIdSectionData, buildIdSectionSize);
    }

    return NV_OK;
}

NV_STATUS
kgspCreateRadix3_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    MEMORY_DESCRIPTOR **ppMemdescRadix3,
    MEMORY_DESCRIPTOR *pMemdescData,
    const void *pData,
    NvU64 size
)
{
    const NvU64 entriesLog2 = LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2 - 3;
    NvU8 *pRadix3Buf;
    NvP64 pVaKernel;
    NvP64 pPrivKernel;
    NvU64 ptSize;
    NvU64 allocSize;
    NvU64 nPages = 0;
    NvU64 dataOffset = 0;
    NvU32 i;
    NV_STATUS status = NV_OK;
    NvU64 flags = MEMDESC_FLAGS_KERNEL_MODE;

    // radix3 working array.
    struct
    {
        NvU64  nPages;
        NvU64  offset;
    } radix3[4];

    NV_ASSERT_OR_RETURN(ppMemdescRadix3 != NULL, NV_ERR_INVALID_PARAMETER);
    NV_ASSERT_OR_ELSE_STR(!((pMemdescData != NULL) && (pData != NULL)),
                          "Specify pMemdescData or pData, or none, but not both",
                          return NV_ERR_INVALID_PARAMETER);

    // If the size is not specified, get it from the memory descriptor.
    if ((size == 0) && (pMemdescData != NULL))
        size = memdescGetSize(pMemdescData);
    NV_ASSERT_OR_RETURN(size > 0, NV_ERR_OUT_OF_RANGE);

    // Clear working structure.
    portMemSet(radix3, 0, sizeof radix3);

    // Populate npages, high to low.
    i = NV_ARRAY_ELEMENTS(radix3) - 1;
    radix3[i].nPages = (size + LIBOS_MEMORY_REGION_RADIX_PAGE_SIZE - 1) >>
                       LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2;
    for (; i > 0; i--)
        radix3[i - 1].nPages = ((radix3[i].nPages - 1) >> entriesLog2) + 1;

    // Populate offset, low to high.
    for (i = 1; i < NV_ARRAY_ELEMENTS(radix3); i++)
    {
        nPages += radix3[i - 1].nPages;
        radix3[i].offset = nPages << LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2;
    }

    NV_ASSERT_OR_RETURN(radix3[0].nPages == 1, NV_ERR_OUT_OF_RANGE);

    // Allocate space for PTEs and PDEs.
    ptSize = nPages << LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2;
    allocSize = ptSize;

    if (pMemdescData == NULL)
    {
        // We don't have a separate descriptor for the data.  We need PTEs,
        // so include space for data in the new descriptor.
        allocSize += radix3[3].nPages << LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2;
    }

    if (confComputeForceUnprotAlloc(pGpu))
    {
        flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(ppMemdescRadix3, pGpu, allocSize,
            LIBOS_MEMORY_REGION_RADIX_PAGE_SIZE,
            NV_MEMORY_NONCONTIGUOUS,
            ADDR_SYSMEM,
            NV_MEMORY_CACHED,
            flags),
        done);

    memdescSetPageSize(*ppMemdescRadix3, AT_GPU, RM_PAGE_SIZE_HUGE);
    memdescTagAlloc(status,
            NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_17, (*ppMemdescRadix3));
    if (status == NV_ERR_NO_MEMORY)
    {
        // TODO: Bug 5299603
        NV_PRINTF(LEVEL_INFO, "memdescTagAllocate failed for huge pages, trying again with regular ones\n");
        memdescSetPageSize(*ppMemdescRadix3, AT_GPU, RM_PAGE_SIZE);
        memdescTagAlloc(status,
                NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_17, (*ppMemdescRadix3));
    }
    NV_ASSERT_OK_OR_GOTO(status, status, error_ret);

    // Create kernel mapping.
    NV_ASSERT_OK_OR_GOTO(status,
        memdescMap(*ppMemdescRadix3, 0, allocSize, NV_TRUE, NV_PROTECT_WRITEABLE,
            &pVaKernel, &pPrivKernel),
        error_ret);

    if (pVaKernel == NvP64_NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "VA error for radix3 shared buffer\n");
        status = NV_ERR_NO_MEMORY;
        goto error_ret;
    }

    pRadix3Buf = KERNEL_POINTER_FROM_NvP64(NvU8 *, pVaKernel);

    // Zap out page table.
    portMemSet(pRadix3Buf, 0, ptSize);

    // Fill in PDEs.
    for (i = 0; i < NV_ARRAY_ELEMENTS(radix3) - 2; i++)
    {
        memdescGetPhysAddrs(*ppMemdescRadix3,
            AT_GPU,                     // addressTranslation
            radix3[i + 1].offset,       // offset
            RM_PAGE_SIZE,               // stride
            radix3[i + 1].nPages,       // count
            (RmPhysAddr *)(pRadix3Buf + radix3[i].offset)); // physical address table
    }

    dataOffset = radix3[3].offset;

    if (pData != NULL)
    {
        // Optionally copy data into the radix3 buffer.
        portMemCopy(pRadix3Buf + dataOffset, size, pData, size);

        // If we only have part of the last page, clear the rest.
        NvU32 clearSize = allocSize - dataOffset - size;
        if (clearSize != 0)
            portMemSet(pRadix3Buf + dataOffset + size, 0, clearSize);

        pMemdescData = *ppMemdescRadix3;
    }

    memdescGetPhysAddrs(*ppMemdescRadix3,
        AT_GPU,                     // addressTranslation
        dataOffset,                 // offset
        RM_PAGE_SIZE,               // stride
        radix3[3].nPages,           // count
        (RmPhysAddr *)(pRadix3Buf + radix3[2].offset));  // physical address table

    //
    // No reason to keep this memory mapped on the CPU side.  Only GSP will
    // access it after this point.
    //
    memdescUnmap(*ppMemdescRadix3, NV_TRUE,
                  pVaKernel, pPrivKernel);
done:
    return status;

error_ret:
    if (*ppMemdescRadix3 != NULL)
    {
        memdescFree(*ppMemdescRadix3);
        memdescDestroy(*ppMemdescRadix3);
        *ppMemdescRadix3 = NULL;
    }

    return status;
}

static NV_STATUS
_kgspFwContainerGetSection
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const void *pElfData,
    NvU64 elfDataSize,
    const char *pSectionName,
    const void **ppSectionData,
    NvU64 *pSectionSize
)
{
    const NvU8 *pGspBuf = pElfData;
    const LibosElf64Header *pElfHeader;
    const LibosElf64SectionHeader *pElfSectionHeader;
    NvU64 elfSectionHeaderTableLength;
    NvU64 elfSectionHeaderMaxIdx;
    NvU64 elfSectionNamesTableOffset;
    NvU64 elfSectionNamesTableSize;
    NvU64 elfSectionNamesTableMaxIdx;
    static const NvU32 elfMagicNumber = 0x464C457F;
    static const NvU8 elfClass64 = 0x2;
    static const NvU8 elfLittleEndian = 0x1;
    const char *pCurrentSectionName;
    NvLength sectionNameLength;
    NvS16 idx;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pElfData != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, elfDataSize > 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pSectionName != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, ppSectionData != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pSectionSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, elfDataSize >= sizeof(LibosElf64Header), NV_ERR_INVALID_DATA);

    sectionNameLength = portStringLength(pSectionName);

    pElfHeader = (const LibosElf64Header*) pGspBuf;

    // Check for the elf identifier at the beginning of the file
    NV_CHECK_OR_RETURN(LEVEL_ERROR, *(NvU32*)&pElfHeader->ident == elfMagicNumber, NV_ERR_INVALID_DATA);
    // Make sure the data is formatted as little endian
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pElfHeader->ident[5] == elfLittleEndian, NV_ERR_INVALID_DATA);
    // Check the class type, only ELFCLASS64 is supported
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pElfHeader->ident[4] == elfClass64, NV_ERR_INVALID_DATA);

    // Make sure that the elf section header table is valid
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pElfHeader->shentsize == sizeof(LibosElf64SectionHeader), NV_ERR_INVALID_DATA);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeMulU64(pElfHeader->shentsize, pElfHeader->shnum, &elfSectionHeaderTableLength), NV_ERR_INVALID_DATA);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(pElfHeader->shoff, elfSectionHeaderTableLength - 1, &elfSectionHeaderMaxIdx), NV_ERR_INVALID_DATA);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, elfDataSize >= elfSectionHeaderMaxIdx, NV_ERR_INVALID_DATA);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pElfHeader->shstrndx <= pElfHeader->shnum, NV_ERR_INVALID_DATA);

    // Get the offset and size of the table that holds the section names and make sure they are valid
    pElfSectionHeader = (const LibosElf64SectionHeader*) &pGspBuf[pElfHeader->shoff + (pElfHeader->shstrndx * pElfHeader->shentsize)];
    elfSectionNamesTableOffset = pElfSectionHeader->offset;
    elfSectionNamesTableSize = pElfSectionHeader->size;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(elfSectionNamesTableOffset, elfSectionNamesTableSize - 1, &elfSectionNamesTableMaxIdx), NV_ERR_INVALID_DATA);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, elfDataSize >= elfSectionNamesTableMaxIdx, NV_ERR_INVALID_DATA);

    // Iterate through all of the section headers to find the signatures
    pElfSectionHeader = (const LibosElf64SectionHeader*) &pGspBuf[elfSectionHeaderMaxIdx + 1 - sizeof(*pElfSectionHeader)];

    for (idx = pElfHeader->shnum - 1; idx >= 0; idx--, pElfSectionHeader--)
    {
        NvU64 currentSectionNameMaxLength;
        NvU64 elfSectionMaxIdx;

        // Make sure the header name index fits within the section names table
        NV_CHECK_OR_RETURN(LEVEL_ERROR, elfSectionNamesTableSize - 1 >= pElfSectionHeader->name, NV_ERR_INVALID_DATA);
        currentSectionNameMaxLength = elfSectionNamesTableSize - pElfSectionHeader->name - 1;
        pCurrentSectionName = (const char *) &pGspBuf[elfSectionNamesTableOffset + pElfSectionHeader->name];

        // Make sure the elf section size and offset are valid
        if (pElfSectionHeader->size > 0)
        {
            NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(pElfSectionHeader->offset, pElfSectionHeader->size - 1, &elfSectionMaxIdx), NV_ERR_INVALID_DATA);
        }
        else
        {
            elfSectionMaxIdx = pElfSectionHeader->offset;
        }
        NV_CHECK_OR_RETURN(LEVEL_ERROR, elfDataSize >= elfSectionMaxIdx, NV_ERR_INVALID_DATA);

        // Check whether the section name matches the expected section name
        if ((sectionNameLength <= currentSectionNameMaxLength) &&
            (portStringCompare(pCurrentSectionName, pSectionName, sectionNameLength) == 0) &&
            (pCurrentSectionName[sectionNameLength] == '\0'))
        {
            *ppSectionData = &pGspBuf[pElfSectionHeader->offset];
            *pSectionSize = pElfSectionHeader->size;

            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * Allocate the chip-agnostic boot-args buffers shared by all per-arch
 * kgspAllocBootArgs HAL implementations:
 *   - LibosMemoryRegionInitArgument[] page (libos init args)
 *   - GSP_ARGUMENTS_CACHED page
 *   - sysmem heap (only on zero-FB platforms)
 */
NV_STATUS
kgspAllocBootArgsCommon
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU64      memdescFlags
)
{
    NvP64 pVa = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NV_STATUS nvStatus = NV_OK;

    //
    // Setup libos arguments memory.
    //
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pKernelGsp->pLibosInitArgumentsDescriptor,
                      pGpu,
                      LIBOS_MEMORY_REGION_INIT_ARGUMENTS_MAX,
                      LIBOS_MEMORY_REGION_INIT_ARGUMENTS_MAX,
                      NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                      memdescFlags),
                      exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_LIBOS_ARGS,
                    pKernelGsp->pLibosInitArgumentsDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pKernelGsp->pLibosInitArgumentsDescriptor, 0,
                   memdescGetSize(pKernelGsp->pLibosInitArgumentsDescriptor),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pVa, &pPriv),
                   exit_cleanup);

    pKernelGsp->pLibosInitArgumentsCached = (LibosMemoryRegionInitArgument *)NvP64_VALUE(pVa);
    pKernelGsp->pLibosInitArgumentsMappingPriv = pPriv;

    portMemSet(pKernelGsp->pLibosInitArgumentsCached, 0, LIBOS_MEMORY_REGION_INIT_ARGUMENTS_MAX);

    // Setup bootloader arguments memory.
    NV_ASSERT(sizeof(GSP_ARGUMENTS_CACHED) <= 0x1000);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pKernelGsp->pGspArgumentsDescriptor,
                      pGpu, 0x1000, 0x1000,
                      NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                      memdescFlags),
                      exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_BOOTLOADER_ARGS,
                    pKernelGsp->pGspArgumentsDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pKernelGsp->pGspArgumentsDescriptor, 0,
                   memdescGetSize(pKernelGsp->pGspArgumentsDescriptor),
                   NV_TRUE, NV_PROTECT_READ_WRITE,
                   &pVa, &pPriv),
                   exit_cleanup);

    pKernelGsp->pGspArgumentsCached = (GSP_ARGUMENTS_CACHED *)NvP64_VALUE(pVa);
    pKernelGsp->pGspArgumentsMappingPriv = pPriv;

    portMemSet(pKernelGsp->pGspArgumentsCached, 0, sizeof(*pKernelGsp->pGspArgumentsCached));

    if (pGpu->pGpuArch->bGpuArchIsZeroFb)
    {
        NvU32 heapSizeMB = 0;
        if (osReadRegistryDword(pGpu, NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB, &heapSizeMB) != NV_OK)
        {
            heapSizeMB = NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB_DEFAULT;
        }

        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescCreate(&pKernelGsp->pSysmemHeapDescriptor,
                          pGpu, (NvU64)heapSizeMB << 20, 0,
                          NV_FALSE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                          memdescFlags),
                          exit_cleanup);

        memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_BOOTLOADER_ARGS,
                        pKernelGsp->pSysmemHeapDescriptor);
        NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                             exit_cleanup);

        NV_ASSERT_TRUE_OR_GOTO(nvStatus,
                               memdescCheckContiguity(pKernelGsp->pSysmemHeapDescriptor, AT_GPU),
                               NV_ERR_MEMORY_ERROR, exit_cleanup);
    }

    return nvStatus;

exit_cleanup:
    kgspFreeBootArgsCommon(pGpu, pKernelGsp);
    return nvStatus;
}

/*!
 * Free the buffers allocated by kgspAllocBootArgsCommon.
 */
void
kgspFreeBootArgsCommon
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    // release libos init argument resources
    if (pKernelGsp->pLibosInitArgumentsCached != NULL)
    {
        memdescUnmap(pKernelGsp->pLibosInitArgumentsDescriptor,
                     NV_TRUE,
                     (void *)pKernelGsp->pLibosInitArgumentsCached,
                     pKernelGsp->pLibosInitArgumentsMappingPriv);
        pKernelGsp->pLibosInitArgumentsCached = NULL;
        pKernelGsp->pLibosInitArgumentsMappingPriv = NULL;
    }
    if (pKernelGsp->pLibosInitArgumentsDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pLibosInitArgumentsDescriptor);
        memdescDestroy(pKernelGsp->pLibosInitArgumentsDescriptor);
        pKernelGsp->pLibosInitArgumentsDescriptor = NULL;
    }

    // release init argument page resources
    if (pKernelGsp->pGspArgumentsCached != NULL)
    {
        memdescUnmap(pKernelGsp->pGspArgumentsDescriptor,
                     NV_TRUE,
                     (void *)pKernelGsp->pGspArgumentsCached,
                     pKernelGsp->pGspArgumentsMappingPriv);
        pKernelGsp->pGspArgumentsCached = NULL;
        pKernelGsp->pGspArgumentsMappingPriv = NULL;
    }
    if (pKernelGsp->pGspArgumentsDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pGspArgumentsDescriptor);
        memdescDestroy(pKernelGsp->pGspArgumentsDescriptor);
        pKernelGsp->pGspArgumentsDescriptor = NULL;
    }

    // Release sysmem heap memory
    if (pKernelGsp->pSysmemHeapDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pSysmemHeapDescriptor);
        memdescDestroy(pKernelGsp->pSysmemHeapDescriptor);
        pKernelGsp->pSysmemHeapDescriptor = NULL;
    }
}

/*!
 * Setup libos init arguments.
 */
void
kgspSetupLibosInitArgs_IMPL
(
    OBJGPU         *pGpu,
    KernelGsp *pKernelGsp
)
{
    LibosMemoryRegionInitArgument *pLibosInitArgs = pKernelGsp->pLibosInitArgumentsCached;
    NvU8 idx;
    portMemSet(pLibosInitArgs, 0, LIBOS_MEMORY_REGION_INIT_ARGUMENTS_MAX);


    // Add memory areas for logging each LIBOS task.
    // @note LOGINIT must be first for early init logging to work.
    // @note: These should be switched to radix regions to remove the need
    //        for large apertures in the RM task for logging.
    for (idx = 0; idx < kgspGetLogCount(pKernelGsp); idx++)
    {
        pLibosInitArgs[idx].kind = LIBOS_MEMORY_REGION_CONTIGUOUS;
        pLibosInitArgs[idx].loc  = LIBOS_MEMORY_REGION_LOC_SYSMEM;
        pLibosInitArgs[idx].id8  = pKernelGsp->rmLibosLogMem[idx].id8;
        pLibosInitArgs[idx].pa   = pKernelGsp->rmLibosLogMem[idx].pTaskLogBuffer[1];
        pLibosInitArgs[idx].size = memdescGetSize(pKernelGsp->rmLibosLogMem[idx].pTaskLogDescriptor);
    }

    // insert GSP-RM ELF args address; id must match libos-config.py entry
    pLibosInitArgs[idx].kind = LIBOS_MEMORY_REGION_CONTIGUOUS;
    pLibosInitArgs[idx].loc  = LIBOS_MEMORY_REGION_LOC_SYSMEM;
    pLibosInitArgs[idx].id8  = _kgspGenerateInitArgId("RMARGS");
    pLibosInitArgs[idx].pa   = memdescGetPhysAddr(pKernelGsp->pGspArgumentsDescriptor, AT_GPU, 0);
    pLibosInitArgs[idx].size = memdescGetSize(pKernelGsp->pGspArgumentsDescriptor);
    ++idx;

    portAtomicMemoryFenceFull();
}

/*!
 * Receive and process RPC event from GSP-RM.
 *
 * This function is called from interrupt bottom-half handler (DPC) and
 * would race with normal RPC flow, _kgspRpcRecvPoll().
 * This race is currently avoided only because DPC is executed under
 * gpus lock, so RPC and Bottom-half handler are mutually exclusive
 * control flows.
 */
void
kgspRpcRecvEvents_IMPL
(
    OBJGPU *pGpu,
    KernelGsp  *pKernelGsp
)
{
    NvU32 gpuMaskUnused;
    NV_ASSERT(rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskUnused));
    //
    // We should never have an event with code NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS.
    // If we do the assert will fail on NV_WARN_MORE_PROCESSING_REQUIRED,
    // in addition to general error codes.
    //
    NV_ASSERT_OK(_kgspRpcDrainEvents(pGpu, pKernelGsp, NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS, 0, KGSP_RPC_EVENT_HANDLER_CONTEXT_INTERRUPT, NULL, NULL));
}

/*!
 * Invoked when GSP sends the RESUME_DONE message
 */
NV_STATUS gmcapiGspResumeDone(GMCAPI_CONTEXT *pCtx)
{
    const GmcApiGspResumeDoneParams *pParams = pCtx->pInParams;
    const NvBool bIsD3Hot = !!(pParams->flags & GMCAPI_GSP_RESUME_DONE_FLAGS_D3HOT);

    NV_PRINTF(LEVEL_INFO, "GSP-RM reports bIsD3Hot = 0x%08x\n", bIsD3Hot);

    if (IS_GPU_GC6_STATE_EXITING(pCtx->pGpu))
    {
        // Kernel-RM reports this info in _gpuGc6ExitStateLoad
        pCtx->pGpu->bIsRTD3Gc6D3HotTransition = bIsD3Hot;
    }
    else
    {
        // Kernel-RM reports this info in RmSetPowerStateEx
        pCtx->pGpu->bIsRTD3GcoffD3HotTransition = bIsD3Hot;
    }

    GPU_GET_KERNEL_GSP(pCtx->pGpu)->bResumeDoneEventReceived = NV_TRUE;
    return NV_OK;
}

static NvBool _kgspIsResumeDone(OBJGPU *pGpu, void *unused)
{
    return (GPU_GET_KERNEL_GSP(pGpu)->bResumeDoneEventReceived);
}

/*!
 * Wait for GSP-RM resume to complete.
 */
NV_STATUS
kgspWaitForRmResumeDone_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    pGpu->bIsRTD3Gc6D3HotTransition      = NV_FALSE;
    pGpu->bIsRTD3GcoffD3HotTransition    = NV_FALSE;

    //
    // Kernel RM can timeout when GSP-RM has an error condition.  Give GSP-RM
    // a chance to report the error before we pull the rug out from under it.
    //
    threadStateResetTimeout(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        gpuRpcConditionWait(pGpu, _kgspIsResumeDone, NULL));

    // Reset for next time
    pKernelGsp->bResumeDoneEventReceived = NV_FALSE;

    pKernelGsp->bGspRmUnloaded = NV_FALSE;

    // GSP starts sending heartbeat after rminit, treat heartbeat values as valid at this point
    if (kgspIsHeartbeatSupported(pGpu))
    {
       _kgspHeartbeatInit(pGpu, pKernelGsp);
    }

    return NV_OK;
}

#if LIBOS_LOG_DECODE_ENABLE
static void
_kgspLogPollingCallback
(
    OBJGPU *pGpu,
    void   *data
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    kgspDumpGspLogs(pKernelGsp, NV_FALSE);
}

NV_STATUS
kgspStartLogPolling_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    return osSchedule1HzCallback(pGpu,
                                 _kgspLogPollingCallback,
                                 NULL,
                                 NV_OS_1HZ_REPEAT);
}

static void
_kgspStopLogPolling
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    osRemove1HzCallback(pGpu, _kgspLogPollingCallback, NULL);
}

#else // LIBOS_LOG_DECODE_ENABLE

NV_STATUS
kgspStartLogPolling_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    return NV_OK;
}

static void
_kgspStopLogPolling
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    return;
}
#endif // LIBOS_LOG_DECODE_ENABLE

/*!
 * Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kgspRegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU32 engineIdx = MC_ENGINE_IDX_GSP;

    if (!IS_GSP_CLIENT(pGpu))
        return;

    NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
    pRecords[engineIdx].pInterruptService = staticCast(pKernelGsp, IntrService);
}

/*!
 * Service GSP interrupts.
 *
 * @returns Zero, or any implementation-chosen nonzero value. If the same nonzero value is returned enough
 *          times the interrupt is considered stuck.
 */
NvU32
kgspServiceInterrupt_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, 0);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_GSP, 0);

    return kgspService_HAL(pGpu, pKernelGsp);
}

/*!
 * Calculates the GSP FW heap size based on the GPU's resources.
 */
static NvU64
_kgspCalculateFwHeapSize
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 maxGspFwHeapSizeMB
)
{
    KernelMemorySystem *pKernelMemorySystem;
    NvU32               memSizeGB;

    // For VGPU, use the static pre-calculated size
    if (pGpu->bVgpuGspPluginOffloadEnabled)
        return kgspVgpuFwHeapSize_HAL(pGpu, pKernelGsp);

    //
    // The baremetal heap calculation is a function of the architecture, FB
    // size, and a chunk for backing client allocations (pre-calibrated for the
    // architecture through rough profiling).
    //
    pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if (pGpu->pGpuArch->bGpuArchIsZeroFb)
    {
        // Bug 4898452 - Hardcode this size for now, will come out to 134MB on GB10b
        memSizeGB = 1;
    }
    else
    {
        NvU64 fbSize = 0;
        NV_ASSERT_OK(kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, &fbSize));
        memSizeGB = (NvU32)(NV_ALIGN_UP64(fbSize, 1 << 30) >> 30);
    }

    NvU64 heapSize = kgspGetFwHeapParamOsCarveoutSize_HAL(pGpu, pKernelGsp) +
                     pKernelGsp->fwHeapParamBaseSize +
                     NV_ALIGN_UP(GSP_FW_HEAP_PARAM_SIZE_PER_GB * memSizeGB, 1 << 20);

    {
        heapSize += NV_ALIGN_UP(GSP_FW_HEAP_PARAM_CLIENT_ALLOC_SIZE, 1 << 20);
    }

    // Clamp to the minimum, even if the calculations say we can do with less
    const NvU32 minGspFwHeapSizeMB = kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
    heapSize = NV_MAX(heapSize, (NvU64)minGspFwHeapSizeMB << 20);

    // Clamp to the maximum heap size, if necessary
    heapSize = NV_MIN(heapSize, (NvU64)maxGspFwHeapSizeMB << 20);

    NV_PRINTF(LEVEL_INFO, "GSP FW heap %lluMB of %uGB\n",
              heapSize >> 20, memSizeGB);

    return heapSize;
}

NvU64
kgspGetMinWprHeapSizeMB_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    if (pGpu->bVgpuGspPluginOffloadEnabled)
    {
        if (pKernelGsp->bVgpuGspSingleVmMode)
            return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_1VM_MIN_MB;
        return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_MIN_MB;
    }
    return kgspGetLibos3BaremetalMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
}

NvU64
kgspGetMaxWprHeapSizeMB_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    if (pGpu->bVgpuGspPluginOffloadEnabled)
    {
        if (pKernelGsp->bVgpuGspSingleVmMode)
            return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_1VM_MAX_MB;
        return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_MAX_MB;
    }
    return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_BAREMETAL_MAX_MB;
}

/*!
 * Returns the size in bytes of the GSP FW heap:
 *  - the registry override, if present
 *  - otherwise, calculate the FW heap size for this GPU, limiting it to stay
 *    within the pre-scrubbed area at the end of FB, if needed
 *
 * @param[in] anteriorFbSize  - size in bytes of the memory reserved in front of
 *                              the GSP FW heap, or 0 to disable limiting of the
 *                              heap range to within the pre-scrubbed area at
 *                              the end of FB
 * @param[in] posteriorFbSize - size in bytes of the memory reserved between the
 *                              end of the GSP FW heap and the end of FB, or 0
 *                              to disable limiting of the heap range to within
 *                              the pre-scrubbed area at the end of FB
 */
NvU64
kgspGetFwHeapSize_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU64 anteriorFbSize,
    NvU64 posteriorFbSize
)
{
    NvU32 maxScrubbedHeapSizeMB = NV_U32_MAX;

    //
    // The pre-scrubbed region at the end of FB may limit the heap size, if no
    // scrubber ucode is supported to unlock the rest of memory prior to booting
    // GSP-RM.
    //
    if (!pKernelGsp->bScrubberUcodeSupported && ((anteriorFbSize != 0) || (posteriorFbSize != 0)))
    {
        const NvU64 prescrubbedSize = kgspGetPrescrubbedTopFbSize(pGpu, pKernelGsp);
        if (prescrubbedSize < NV_U64_MAX)
        {
            const NvU64 maxScrubbedHeapSize = prescrubbedSize - (anteriorFbSize + posteriorFbSize);
            maxScrubbedHeapSizeMB = (NvU32)(maxScrubbedHeapSize >> 20);
        }
    }

    // Get the heap size override from the registry, if any
    if (pKernelGsp->heapSizeMBOverride != NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB_DEFAULT)
    {
        const NvU32 minGspFwHeapSizeMB = kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
        const NvU32 maxGspFwHeapSizeMB = NV_MIN(kgspGetMaxWprHeapSizeMB_HAL(pGpu, pKernelGsp),
                                                maxScrubbedHeapSizeMB);

        NV_ASSERT(minGspFwHeapSizeMB < maxGspFwHeapSizeMB);

        if (pKernelGsp->heapSizeMBOverride > maxGspFwHeapSizeMB)
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size clamped to maximum (%uMB)\n",
                      maxGspFwHeapSizeMB);
            pKernelGsp->heapSizeMBOverride = maxGspFwHeapSizeMB;
        }
        else if (pKernelGsp->heapSizeMBOverride < minGspFwHeapSizeMB)
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size clamped to minimum (%uMB)\n",
                      minGspFwHeapSizeMB);
            pKernelGsp->heapSizeMBOverride = minGspFwHeapSizeMB;
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size overridden (%uMB)\n",
                      pKernelGsp->heapSizeMBOverride);
        }

        return ((NvU64)pKernelGsp->heapSizeMBOverride) << 20;
    }

    return _kgspCalculateFwHeapSize(pGpu, pKernelGsp, maxScrubbedHeapSizeMB);
}

static void
_kgspInitGpuProperties(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_MOBILE, pGSCI->bIsMobile);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED, pGSCI->bIsGc6Rtd3Allowed && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_EXTERNAL_GPU));
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC8_SUPPORTED, pGSCI->bIsGc8Rtd3Allowed && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_EXTERNAL_GPU));
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED, pGSCI->bIsGcOffRtd3Allowed && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_EXTERNAL_GPU));
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_UEFI, pGSCI->bIsGpuUefi);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_EFI_INIT, pGSCI->bIsEfiInit);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED, pGSCI->bIsGcoffLegacyAllowed);
}

static NV_STATUS _kgspDumpEngineFunc
(
    OBJGPU *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE *pNvDumpState,
    void *pvData
)
{
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    NV_STATUS nvStatus = NV_OK;
    NvU8 startingDepth = prbEncNestingLevel(pPrbEnc);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_GPUINFO_ENG_KGSP));

    for (NvU32 i = 0; i < RPC_HISTORY_DEPTH; i++)
    {
        NvU32 entryIdx = (pRpc->rpcHistoryCurrent + RPC_HISTORY_DEPTH - i) % RPC_HISTORY_DEPTH;
        RpcHistoryEntry *entry = &pRpc->rpcHistory[entryIdx];

        if (entry->function == 0)
            break;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            prbEncNestedStart(pPrbEnc, NVDEBUG_ENG_KGSP_RPC_HISTORY));

        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_FUNCTION, entry->function);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_SEQUENCE, entry->sequence);
        prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_TS_START, entry->ts_start);
        prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_TS_END, entry->ts_end);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_DATA0, entry->data[0]);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_DATA1, entry->data[1]);

        prbEncNestedEnd(pPrbEnc);
    }

    for (NvU32 i = 0; i < RPC_HISTORY_DEPTH; i++)
    {
        NvU32 entryIdx = (pRpc->rpcEventHistoryCurrent + RPC_HISTORY_DEPTH - i) % RPC_HISTORY_DEPTH;
        RpcHistoryEntry *entry = &pRpc->rpcEventHistory[entryIdx];

        if (entry->function == 0)
            break;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            prbEncNestedStart(pPrbEnc, NVDEBUG_ENG_KGSP_EVENT_HISTORY));

        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_FUNCTION, entry->function);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_SEQUENCE, entry->sequence);
        prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_TS_START, entry->ts_start);
        prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_TS_END, entry->ts_end);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_DATA0, entry->data[0]);
        prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_KGSP_RPCINFO_DATA1, entry->data[1]);

        prbEncNestedEnd(pPrbEnc);
    }

    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return nvStatus;
}

/*!
* @brief initialize the nocat diagnostic buffer to accumulate data in.
*
* @returns                   status of the buffer.
*/
NV_STATUS
kgspInitNocatData_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GspNocatEvent gspNocatEvent
)
{
    NV_STATUS status = NV_OK;
    const PRB_FIELD_DESC *fieldDesc;

    switch (gspNocatEvent)
    {
        case GSP_NOCAT_CRASHCAT_REPORT:
        case GSP_NOCAT_GSP_RPC_HISTORY:
        case GSP_NOCAT_GSP_RPC_TIMEOUT:
            fieldDesc = DCL_DCLMSG_GSP_XIDREPORT;
            break;
        case GSP_NOCAT_GSP_RPC_PERF:
            fieldDesc = DCL_DCLMSG_GSP_RPCPERF;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    // clear all the data
    portMemSet(&pKernelGsp->nocatData, 0, sizeof(pKernelGsp->nocatData));

    pKernelGsp->nocatData.gspNocatEvent = gspNocatEvent;
    pKernelGsp->nocatData.initialized = NV_TRUE;

    // start a message for the data.
    status = prbSetupDclMsg(&pKernelGsp->nocatData.nocatBuffer,
                            NV2080_NOCAT_JOURNAL_MAX_DIAG_BUFFER,
                            fieldDesc);

    if (status != NV_OK)
    {
        pKernelGsp->nocatData.initialized = NV_FALSE;
    }

    return status;
}

/*!
* @brief post any accumulated diagnostic data in the buffer.
*
* @param pKernelGsp                 Pointer to KernelGsp object
*
* @returns                   status of the buffer.
*/
NV_STATUS
kgspPostNocatData_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU64 timestamp
)
{
    NV_STATUS status = NV_OK;
    NvU8 *pBuff = NULL;
    NvU32 len = 0;
    NOCAT_JOURNAL_PARAMS newEntry;

    if (!pKernelGsp->nocatData.initialized)
    {
        return NV_ERR_INVALID_STATE;
    }

    // end the message.
    status = prbEncNestedEnd(&pKernelGsp->nocatData.nocatBuffer);

    // finish the prb buffer.
    len = prbEncFinish(&pKernelGsp->nocatData.nocatBuffer, (void**)&pBuff);

    if (status != NV_OK)
        goto end;

    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.errorCode = pKernelGsp->nocatData.errorCode;
    newEntry.pDiagBuffer = pBuff;
    newEntry.diagBufferLen = len;
    newEntry.timestamp = timestamp;

    switch (pKernelGsp->nocatData.gspNocatEvent)
    {
        case GSP_NOCAT_CRASHCAT_REPORT:
        {
            // post the buffer as terminating event
            newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_TDR;
            newEntry.pSource = GSP_NOCAT_SOURCE_ID;
            break;
        }
        case GSP_NOCAT_GSP_RPC_HISTORY:
        {
            // post the buffer as non-terminating event
            newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE;
            newEntry.pSource = GSP_NOCAT_SOURCE_ID_RPC_HISTORY;
            break;
        }
        case GSP_NOCAT_GSP_RPC_TIMEOUT:
        {
            // post the buffer as terminating event
            newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_TDR;
            newEntry.pSource = GSP_NOCAT_SOURCE_ID_RPC_TIMEOUT;
            break;
        }
        case GSP_NOCAT_GSP_RPC_PERF:
        {
            // post the buffer as non-terminating event
            newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE;
            newEntry.pSource = GSP_NOCAT_SOURCE_ID_RPC_PERF;
            break;
        }
        default:
            newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE;
            newEntry.pSource = GSP_NOCAT_SOURCE_ID;
    }

    rcdbNocatInsertNocatError(pGpu, &newEntry);

end:
    // dispose of the buffer.
    portMemFree(pBuff);

    // clear the initialized flag.
    pKernelGsp->nocatData.initialized = NV_FALSE;

    return status;
}

/*!
* @brief Helper function for logging crashcat report to NOCAT
*
* @param pKernelGsp                 Pointer to KernelGsp object
*/
NV_STATUS
kgspPostCrashcatReportToNocat_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    CrashCatReport *pReport,
    NvU32 xid
)
{
    NV_STATUS status = NV_OK;
    char buildIdString[64];
    LibosElfNoteHeader *pBuildIdNoteHeader = pKernelGsp->pBuildIdSection;

    kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_CRASHCAT_REPORT);

    // Build id string can be used by offline decoder to decode crashcat data/addresses to symbols
    if (pKernelGsp->pBuildIdSection != NULL)
    {
        portStringBufferToHex(buildIdString,
                                sizeof(buildIdString)/sizeof(buildIdString[0]),
                                pBuildIdNoteHeader->data + pBuildIdNoteHeader->namesz,
                                pBuildIdNoteHeader->descsz);

        prbEncAddString(&pKernelGsp->nocatData.nocatBuffer,
                        GSP_XIDREPORT_BUILDID,
                        &buildIdString[0]);
    }

    // ErrorCode of nocat event is used for categorizing GSP crash data collected from the field via nocat
    // Since lowest bit of ra is always empty, we use bit 0 to store the sign bit, for
    // differentiating task crash vs libos crash
    // signbit of ra - 1 bit, 0
    // ra           - (28 - 1) bits, 27:1
    // scause       - 4 bits, 31:28
    // stval        - 32 bits, 63:32
    pKernelGsp->nocatData.errorCode |= (crashcatReportRa_HAL(pReport) >> 63) & 1;
    pKernelGsp->nocatData.errorCode |= crashcatReportRa_HAL(pReport) & 0xFFFFFFE;
    pKernelGsp->nocatData.errorCode |= (crashcatReportXcause_HAL(pReport) & 0xF) << 28;
    pKernelGsp->nocatData.errorCode |= (crashcatReportXtval_HAL(pReport) & 0xFFFFFFFF) << 32;

    prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_XID, xid);
    prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_GPUINSTANCE, gpuGetInstance(pGpu));
    crashcatReportLogToProtobuf_HAL(pReport, &pKernelGsp->nocatData.nocatBuffer);

    status = kgspPostNocatData(pGpu, pKernelGsp, osGetTimestamp());
    return status;
}

/*!
* @brief Helper function for printing GSP bin buildId
*
* @param pKernelGsp                 Pointer to KernelGsp object
*/
void
kgspPrintGspBinBuildId_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    char buildIdString[64];
    LibosElfNoteHeader *pBuildIdNoteHeader;

    if (pKernelGsp->pBuildIdSection != NULL)
    {
        pBuildIdNoteHeader = pKernelGsp->pBuildIdSection;
        portStringBufferToHex(buildIdString,
                                NV_ARRAY_ELEMENTS(buildIdString),
                                pBuildIdNoteHeader->data + pBuildIdNoteHeader->namesz,
                                pBuildIdNoteHeader->descsz);
        NV_PRINTF(LEVEL_ERROR, "GSP bin buildId: %s\n", buildIdString);
    }
}

typedef struct _kgspStaticInfoKeyHandlerContext
{
    OBJGPU *pGpu;
    GspStaticConfigInfo *pSCI;
    NV_STATUS status;
} kgspStaticInfoKeyHandlerContext;

static NvBool
_kgspStaticInfoKeyHandler(const NvU64 keyIndex, const NvU64 key, const NVKVValue *pValue, void *pCtx)
{
    kgspStaticInfoKeyHandlerContext *pHandlerCtx = (kgspStaticInfoKeyHandlerContext *)pCtx;
    GspStaticConfigInfo *pSCI = pHandlerCtx->pSCI;
    OBJGPU *pGpu = pHandlerCtx->pGpu;

    #define NVKV_PREFIX NVGMC_SC

    NVKVDecodeContext ctx = NVKV_DECODE_INIT(keyIndex, key, pValue);
    NVKV_DECODE_SWITCH(&ctx)
    {
        NVKV_CASE_STRING8(&ctx, GPU_NAME_STRING, pSCI->gpuNameString);
        NVKV_CASE_STRING8(&ctx, GPU_SHORT_NAME_STRING, pSCI->gpuShortNameString);
        NVKV_CASE_ARRAY8(&ctx, GPU_GID_SHA1, pSCI->gidInfo.data);

        NVKV_CASE_VAR(&ctx, U32, SKU_BOARD_ID, pSCI->SKUInfo.BoardID);
        NVKV_CASE_VAR(&ctx, U32, SKU_CONFIG_VERSION, pSCI->SKUInfo.skuConfigVersion);
        NVKV_CASE_VAR(&ctx, U32, SKU_BUSINESS_CYCLE, pSCI->SKUInfo.businessCycle);
        NVKV_CASE_VAR(&ctx, U32, CHIP_SUB_REV, pSCI->chipSubRev);
        NVKV_CASE_VAR(&ctx, U32, EMULATION_REV1, pSCI->emulationRev1);

        NVKV_CASE_ARRAY8(&ctx, CHIP_SKU, pSCI->SKUInfo.chipSKU);
        NVKV_CASE_ARRAY8(&ctx, CHIP_SKU_MODIFIER, pSCI->SKUInfo.chipSKUMod);
        NVKV_CASE_ARRAY8(&ctx, PROJECT, pSCI->SKUInfo.project);
        NVKV_CASE_ARRAY8(&ctx, PROJECT_SKU, pSCI->SKUInfo.projectSKU);
        NVKV_CASE_ARRAY8(&ctx, PROJECT_SKU_MODIFIER, pSCI->SKUInfo.projectSKUMod);
        NVKV_CASE_ARRAY8(&ctx, CDP, pSCI->SKUInfo.CDP);

        NVKV_CASE_VAR(&ctx, U32, INTERNAL_CLIENT_HANDLE, pSCI->hInternalClient);
        NVKV_CASE_VAR(&ctx, U32, INTERNAL_DEVICE_HANDLE, pSCI->hInternalDevice);
        NVKV_CASE_VAR(&ctx, U32, INTERNAL_SUBDEVICE_HANDLE, pSCI->hInternalSubdevice);

        NVKV_CASE_MAX(&ctx, U32, FB_REGION_COUNT, pSCI->fbRegionInfoParams.numFBRegions, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES);

        NVKV_CASE_IDX(&ctx, U64, FB_REGION_BASE, pSCI->fbRegionInfoParams.fbRegion[keyIndex].base, pSCI->fbRegionInfoParams.numFBRegions);
        NVKV_CASE_IDX(&ctx, U64, FB_REGION_LIMIT, pSCI->fbRegionInfoParams.fbRegion[keyIndex].limit, pSCI->fbRegionInfoParams.numFBRegions);
        NVKV_CASE_IDX(&ctx, U64, FB_REGION_RESERVED, pSCI->fbRegionInfoParams.fbRegion[keyIndex].reserved, pSCI->fbRegionInfoParams.numFBRegions);
        NVKV_CASE_IDX(&ctx, U32, FB_REGION_PERFORMANCE, pSCI->fbRegionInfoParams.fbRegion[keyIndex].performance, pSCI->fbRegionInfoParams.numFBRegions);
        NVKV_CASE_IDX(&ctx, U32, FB_REGION_TAG, pSCI->fbRegionInfoParams.fbRegion[keyIndex].regionTag, pSCI->fbRegionInfoParams.numFBRegions);

        NVKV_CASE_VAR(&ctx, U32, GC6_RTD3_TOTAL_BOARD_POWER, pSCI->RTD3GC6TotalBoardPower);
        NVKV_CASE_VAR(&ctx, U32, GC6_RTD3_PERST_DELAY, pSCI->RTD3GC6PerstDelay);

        NVKV_CASE_VAR(&ctx, U64, FB_LENGTH, pSCI->fb_length);
        NVKV_CASE_VAR(&ctx, U32, SRIOV_MAX_GFID, pSCI->sriovMaxGfid);
        NVKV_CASE_VAR(&ctx, U64, SRIOV_BAR0_SIZE, pSCI->sriovCaps.bar0Size);
        NVKV_CASE_VAR(&ctx, U64, SRIOV_BAR1_SIZE, pSCI->sriovCaps.bar1Size);
        NVKV_CASE_VAR(&ctx, U64, SRIOV_BAR2_SIZE, pSCI->sriovCaps.bar2Size);
        NVKV_CASE_VAR(&ctx, U64, BAR1_PDE_BASE, pSCI->bar1PdeBase);
        NVKV_CASE_VAR(&ctx, U64, BAR2_PDE_BASE, pSCI->bar2PdeBase);

        NVKV_CASE_ARRAY64(&ctx, ECID_INFO, pSCI->ecidInfo.info);
        NVKV_CASE_VAR(&ctx, U64, VMMU_SEGMENT_SIZE, pGpu->vmmuSegmentSize);

        NVKV_CASE_VAR(&ctx, U64, PDI, pSCI->pdi, { pSCI->bPdiValid = NV_TRUE; });

        NVKV_CASE_VAR(&ctx, U32, VBIOS_SUB_VENDOR, pSCI->vbiosSubVendor, { pSCI->bVbiosValid = NV_TRUE; });
        NVKV_CASE_VAR(&ctx, U32, VBIOS_SUB_DEVICE, pSCI->vbiosSubDevice, { pSCI->bVbiosValid = NV_TRUE; });
        NVKV_CASE_VAR(&ctx, U32, VBIOS_REVISION, pSCI->vbiosRevision, { pSCI->bVbiosValid = NV_TRUE; });
        NVKV_CASE_VAR(&ctx, U32, VBIOS_OEM_REVISION, pSCI->vbiosOemRevision, { pSCI->bVbiosValid = NV_TRUE; });

        NVKV_CASE_VAR(&ctx, U64, WPR_LAYOUT_NON_WPR_HEAP_OFFSET, pSCI->fwWprLayoutOffset.nonWprHeapOffset);
        NVKV_CASE_VAR(&ctx, U64, WPR_LAYOUT_FRTS_OFFSET, pSCI->fwWprLayoutOffset.frtsOffset);

        NVKV_CASE_LOCAL(&ctx, U64, BRANDING, branding,
        {
            pSCI->bIsQuadroGeneric = NVKV_VAL64(BRANDING_IS_QUADRO_GENERIC, branding);
            pSCI->bIsQuadroAd = NVKV_VAL64(BRANDING_IS_QUADRO_AD, branding);
            pSCI->bIsNvidiaNvs = NVKV_VAL64(BRANDING_IS_NVIDIA_NVS, branding);
            pSCI->bIsVgx = NVKV_VAL64(BRANDING_IS_VGX, branding);
            pSCI->bIsTitan = NVKV_VAL64(BRANDING_IS_TITAN, branding);
            pSCI->bIsTesla = NVKV_VAL64(BRANDING_IS_TESLA, branding);
            pSCI->bIsMobile = NVKV_VAL64(BRANDING_IS_MOBILE, branding);
            pSCI->bGeforceSmb = NVKV_VAL64(BRANDING_IS_GEFORCE_SMB, branding);
            pSCI->bIsCmpSku = NVKV_VAL64(BRANDING_IS_CMP_SKU, branding);
        });

        NVKV_CASE_LOCAL(&ctx, U64, POWER_FEATURES, powerFeatures,
        {
            pSCI->bIsGc6Rtd3Allowed = NVKV_VAL64(POWER_FEATURES_IS_GC6_RTD3_ALLOWED, powerFeatures);
            pSCI->bIsGc8Rtd3Allowed = NVKV_VAL64(POWER_FEATURES_IS_GC8_RTD3_ALLOWED, powerFeatures);
            pSCI->bIsGcOffRtd3Allowed = NVKV_VAL64(POWER_FEATURES_IS_GCOFF_RTD3_ALLOWED, powerFeatures);
            pSCI->bIsGcoffLegacyAllowed = NVKV_VAL64(POWER_FEATURES_IS_GCOFF_LEGACY_ALLOWED, powerFeatures);
        });

        NVKV_CASE_LOCAL(&ctx, U64, MISC_FEATURES, miscFeatures,
        {
            pSCI->bIsMigSupported = NVKV_VAL64(MISC_FEATURES_IS_MIG_SUPPORTED, miscFeatures);
            pSCI->bSelfHostedMode = NVKV_VAL64(MISC_FEATURES_IS_SELF_HOSTED, miscFeatures);
            pSCI->bAtsSupported = NVKV_VAL64(MISC_FEATURES_IS_ATS_SUPPORTED, miscFeatures);
            pSCI->bIsGpuUefi = NVKV_VAL64(MISC_FEATURES_IS_GPU_UEFI, miscFeatures);
            pSCI->bIsEfiInit = NVKV_VAL64(MISC_FEATURES_IS_EFI_INIT, miscFeatures);
            pSCI->bBusResetRequired = NVKV_VAL64(MISC_FEATURES_IS_BUS_RESET_REQUIRED, miscFeatures);
            pSCI->bPageRetirementSupported = NVKV_VAL64(MISC_FEATURES_IS_PAGE_RETIREMENT_SUPPORTED, miscFeatures);
            pSCI->bSplitVasBetweenServerClientRm = NVKV_VAL64(MISC_FEATURES_IS_SPLIT_VAS_BETWEEN_SERVER_CLIENT_RM, miscFeatures);
            pSCI->bClRootportNeedsNosnoopWAR = NVKV_VAL64(MISC_FEATURES_IS_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, miscFeatures);
            pSCI->bSysL2CacheCoherentMode = NVKV_VAL64(MISC_FEATURES_IS_SYS_L2_CACHE_COHERENT, miscFeatures);
            pSCI->poisonFuseEnabled = NVKV_VAL64(MISC_FEATURES_IS_POISON_FUSE_ENABLED, miscFeatures);
        });

        NVKV_CASE_IDX_LOCAL(&ctx, U32, FB_REGION_FLAGS, flags, pSCI->fbRegionInfoParams.numFBRegions,
        {
            pSCI->fbRegionInfoParams.fbRegion[keyIndex].supportCompressed = NVKV_VAL(FB_REGION_FLAGS_IS_COMPRESSION_SUPPORTED, flags);
            pSCI->fbRegionInfoParams.fbRegion[keyIndex].supportISO = NVKV_VAL(FB_REGION_FLAGS_IS_ISO_SUPPORTED, flags);
            pSCI->fbRegionInfoParams.fbRegion[keyIndex].bProtected = NVKV_VAL(FB_REGION_FLAGS_IS_PROTECTED, flags);
        });

        NVKV_CASE(&ctx, ENGINE_MASK,
        {
            if (pValue->valueType != NVKV_VALUE_TYPE_U64)
                continue;
            gpuGetRmEngineTypeCapsFromGMCEngineIdMasks(pSCI->engineCaps, pValue->valueData.pU64, keyIndex, pValue->valueCount);
            break;
        });

        NVKV_CASE_LOCAL(&ctx, U32, VGX_VERSION, vgxVersion,
        {
            rpcSetIpVersion(pGpu, GPU_GET_RPC(pGpu), vgxVersion);
        });

        default:
        {
            NV_PRINTF(LEVEL_INFO, "Ignored NVGMC_SC key: 0x%04x index: 0x%04x\n",(NvU32)key, (NvU32)keyIndex );
        }
    }
    NVKV_DECODE_SWITCH_END(&ctx);

    #undef NVKV_PREFIX

    NV_PRINTF(LEVEL_ERROR, "Unexpected NVGMC_SC key 0x%04x: index: 0x%04x, type: %d, count: %d\n", (NvU32)key, (NvU32)keyIndex, (NvU32)pValue->valueType, (NvU32)pValue->valueCount);
    pHandlerCtx->status = NV_ERR_INVALID_ARGUMENT;
    return NV_FALSE;
}

static NV_STATUS _kgspDecodeGspStaticInfo(OBJGPU *pGpu, NvU64 *pKVData, NvU64 kvLimit)
{
    kgspStaticInfoKeyHandlerContext ctx;

    ctx.pGpu = pGpu;
    ctx.pSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    ctx.status = NV_OK;

    portMemSet(ctx.pSCI, 0, sizeof(GspStaticConfigInfo));

    NV_STATUS status = nvkvDecode(_kgspStaticInfoKeyHandler, pKVData, kvLimit, &ctx);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "nvkvDecode failed: 0x%x\n", status);
    }
    if(ctx.status != NV_OK)
    {
        // Logging here is redundant with respect to the one at the end of _kgspStaticInfoKeyHandler
        status = ctx.status;
    }

    return status;
}
