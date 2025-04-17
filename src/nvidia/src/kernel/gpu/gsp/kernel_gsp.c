/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "virtualization/hypervisor/hypervisor.h"
#include "virtualization/vgpuconfigapi.h"
#include "kernel/gpu/disp/kern_disp.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/device/device.h"
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
#include "gpu/rpc/objrpc.h"
#include "gpu/timer/objtmr.h"
#include "os/os.h"
#include "rmgspseq.h"
#include "gpu/disp/dispsw.h"
#include "kernel/gpu/timed_sema.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/pmu/kern_pmu.h"
#include "gpu/perf/kern_perf.h"
#include "core/locks.h"
#include "kernel/gpu/intr/intr.h"
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

#include "gpu/gsp/message_queue_priv.h"

#include "gpu/conf_compute/conf_compute.h"

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
#include "diagnostics/code_coverage_mgr.h"
#endif

#define RPC_HDR  ((rpc_message_header_v*)(pRpc->message_buffer))

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

static NV_STATUS _kgspRpcSendMessage(OBJGPU *, OBJRPC *);
static NV_STATUS _kgspRpcRecvPoll(OBJGPU *, OBJRPC *, NvU32);
static NV_STATUS _kgspRpcDrainEvents(OBJGPU *, KernelGsp *, NvU32, KernelGspRpcEventHandlerContext);
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
static void _kgspRpcGspEventHandleFecsBufferError(NvU32, void *);

static NV_STATUS _kgspRpcGspEventRecoveryAction(OBJGPU *, OBJRPC *);

static void _kgspInitGpuProperties(OBJGPU *);
static NV_STATUS _kgspDumpEngineFunc(OBJGPU*, PRB_ENCODER*, NVD_STATE*, void*);

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
            RPC_PARAMS(gsp_rm_control, _v03_00);
            *data0 = rpc_params->cmd;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC:
        {
            RPC_PARAMS(gsp_rm_alloc, _v03_00);
            *data0 = rpc_params->hClass;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_FREE:
        {
            RPC_PARAMS(free, _v03_00);
            *data0 = rpc_params->params.hObjectOld;
            *data1 = rpc_params->params.hObjectParent;
            break;
        }

        // Events (CPU <- GSP)
        case NV_VGPU_MSG_EVENT_GSP_RUN_CPU_SEQUENCER:
        {
            RPC_PARAMS(run_cpu_sequencer, _v17_00);
            *data0 = rpc_params->cmdIndex;
            *data1 = rpc_params->bufferSizeDWord;
            break;
        }
        case NV_VGPU_MSG_EVENT_POST_EVENT:
        {
            RPC_PARAMS(post_event, _v17_00);
            *data0 = rpc_params->notifyIndex;
            *data1 = rpc_params->data;
            break;
        }
        case NV_VGPU_MSG_EVENT_RC_TRIGGERED:
        {
            RPC_PARAMS(rc_triggered, _v17_02);
            *data0 = rpc_params->nv2080EngineType;
            *data1 = rpc_params->exceptType;
            break;
        }
        case NV_VGPU_MSG_EVENT_VGPU_GSP_PLUGIN_TRIGGERED:
        {
            RPC_PARAMS(vgpu_gsp_plugin_triggered, _v17_00);
            *data0 = rpc_params->gfid;
            *data1 = rpc_params->notifyIndex;
            break;
        }
        case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
        {
            RPC_PARAMS(gsp_lockdown_notice, _v17_00);
            *data0 = rpc_params->bLockdownEngaging;
            *data1 = 0;
            break;
        }
        case NV_VGPU_MSG_EVENT_GSP_POST_NOCAT_RECORD:
        {
            RPC_PARAMS(gsp_post_nocat_record, _v01_00);
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
    NvU32 *pCurrent
)
{
    NvU32 func = RPC_HDR->function;
    NvU32 entry;

    entry = *pCurrent = (*pCurrent + 1) % RPC_HISTORY_DEPTH;

    portMemSet(&pHistory[entry], 0, sizeof(pHistory[0]));
    pHistory[entry].function = func;
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
static NV_STATUS
_kgspRpcSendMessage
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS nvStatus;
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NvU32 gpuMaskUnused;

    NV_ASSERT(rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskUnused));

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, _kgspRpcSanityCheck(pGpu, pKernelGsp, pRpc));

    nvStatus = GspMsgQueueSendCommand(pRpc->pMessageQueueInfo, pGpu);
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

    kgspSetCmdQueueHead_HAL(pGpu, pKernelGsp, pRpc->pMessageQueueInfo->queueIdx, 0);

    _kgspAddRpcHistoryEntry(pRpc, pRpc->rpcHistory, &pRpc->rpcHistoryCurrent);

    return NV_OK;
}

static NV_STATUS
_kgspRpcRunCpuSequencer
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(run_cpu_sequencer, _v17_00);
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    return kgspExecuteSequencerBuffer(pGpu, pKernelGsp, rpc_params);
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
    RPC_PARAMS(post_event, _v17_00);
    PEVENTNOTIFICATION pNotifyList  = NULL;
    PEVENTNOTIFICATION pNotifyEvent = NULL;
    Event             *pEvent       = NULL;
    NV_STATUS          nvStatus     = NV_OK;

    // Get the notification list that contains this event.
    NV_ASSERT_OR_RETURN(CliGetEventInfo(rpc_params->hClient,
        rpc_params->hEvent, &pEvent), NV_ERR_OBJECT_NOT_FOUND);

    if (pEvent->pNotifierShare != NULL)
        pNotifyList = pEvent->pNotifierShare->pEventList;

    NV_ASSERT_OR_RETURN(pNotifyList != NULL, NV_ERR_INVALID_POINTER);

    switch (rpc_params->notifyIndex)
    {
        case NV2080_NOTIFIERS_ECC_DBE:
            _kgspProcessEccNotifier(pGpu, rpc_params->eventData);
            break;
    }

    // Send the event.
    if (rpc_params->bNotifyList)
    {
        // Send notification to all matching events on the list.
        {
            gpuNotifySubDeviceEvent(pGpu, rpc_params->notifyIndex,
                rpc_params->eventData, rpc_params->eventDataSize,
                rpc_params->data, rpc_params->info16);
        }
    }
    else
    {
        // Send event to a specific hEvent.  Find hEvent in the notification list.
        for (pNotifyEvent = pNotifyList; pNotifyEvent; pNotifyEvent = pNotifyEvent->Next)
        {
            if (pNotifyEvent->hEvent == rpc_params->hEvent)
            {
                nvStatus = osNotifyEvent(pGpu, pNotifyEvent, 0,
                                         rpc_params->data, rpc_params->status);
                break;
            }
        }
        NV_ASSERT_OR_RETURN(pNotifyEvent != NULL, NV_ERR_OBJECT_NOT_FOUND);
    }

    return nvStatus;
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
    RPC_PARAMS(rc_triggered, _v17_02);

    KernelRc      *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    KernelChannel *pKernelChannel = NULL;
    KernelFifo    *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR      *pChidMgr;
    NvU32          status = NV_OK;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(rpc_params->nv2080EngineType);
    NvBool         bIsCcEnabled = NV_FALSE;

    // check if there's a PCI-E error pending either in device status or in AER
    krcCheckBusError_HAL(pGpu, pKernelRc);

    //
    // If we have received a special msg from GSP then ack back immediately
    // that we are done writing notifiers since we would have already processed the
    // other RC msgs that trigger notifier writes before this one.
    //
    if (rpc_params->exceptType == ROBUST_CHANNEL_FAST_PATH_ERROR)
    {
        NV_RM_RPC_ECC_NOTIFIER_WRITE_ACK(pGpu, status);
        NV_ASSERT_OK(status);
        return status;
    }

    status = kfifoGetChidMgrFromType(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                     (NvU32)rmEngineType,
                                     &pChidMgr);
    if (status != NV_OK)
        return status;

    if (IS_GFID_PF(rpc_params->gfid))
    {
        pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                      pChidMgr,
                                                      rpc_params->chid);
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
        NvU32 rcDiagRecStart = pRcDB->RcErrRptNextIdx;
        NvU32 rcDiagRecEnd;
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
                NV_PRINTF(LEVEL_INFO, "     %d: offset=0x08%x tag=0x08%x value=0x08%x attribute=0x08%x\n",
                          j, pRcDiagRecord->data[j].offset, pRcDiagRecord->data[j].tag,
                          pRcDiagRecord->data[j].value, pRcDiagRecord->data[j].attribute);
            }
#endif
            if (rcdbAddRcDiagRecFromGsp(pGpu, pRcDB, pCommonRecord, pRcDiagRecord) == NULL)
            {
                NV_PRINTF(LEVEL_WARNING, "Lost RC diagnostic record coming from GPU%d GSP: type=0x%x stateMask=0x%llx\n",
                          gpuGetInstance(pGpu), pRcDiagRecord->type, pCommonRecord->stateMask);
            }
        }

        rcDiagRecEnd = pRcDB->RcErrRptNextIdx - 1;

        // Update records to have the correct PID associated with the channel
        if (rcDiagRecStart != rcDiagRecEnd)
        {
            rcdbUpdateRcDiagRecContext(pRcDB,
                                       rcDiagRecStart,
                                       rcDiagRecEnd,
                                       processId,
                                       owner);
        }
    }

    bIsCcEnabled = gpuIsCCFeatureEnabled(pGpu);

    // With CC enabled, CPU-RM needs to write error notifiers
    if (bIsCcEnabled && pKernelChannel != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(krcErrorSetNotifier(pGpu, pKernelRc,
                                                   pKernelChannel,
                                                   rpc_params->exceptType,
                                                   rmEngineType,
                                                   rpc_params->scope));
    }

    return krcErrorSendEventNotifications_HAL(pGpu, pKernelRc,
        pKernelChannel,
        rmEngineType,            // unused on kernel side
        rpc_params->exceptLevel, // unused on kernel side
        rpc_params->exceptType,
        rpc_params->scope,
        rpc_params->partitionAttributionId,
        NV_FALSE                 // unused on kernel side
        );
}

/*!
 * This function is called on critical FW crash to RC and notify an error code to
 * all user mode channels, allowing the user mode apps to fail deterministically.
 *
 * @param[in] pGpu                 GPU object pointer
 * @param[in] pKernelGsp           KernelGsp object pointer
 * @param[in] exceptType           Error code to send to the RC notifiers
 * @param[in] bSkipKernelChannels  Don't RC and notify kernel channels
 *
 */
void
kgspRcAndNotifyAllChannels_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      exceptType,
    NvBool     bSkipKernelChannels
)
{
    //
    // Note Bug 4503046: UVM currently attributes all errors as global and fails
    // operations on all GPUs, in addition to the current failing GPU. Right now, the only
    // case where we shouldn't skip kernel channels is when the GPU has fallen off the bus.
    //

    KernelRc         *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    KernelChannel    *pKernelChannel;
    KernelFifo       *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHANNEL_ITERATOR  chanIt;
    RMTIMEOUT         timeout;

    NV_PRINTF(LEVEL_ERROR, "RC all %schannels for critical error %d.\n",
              bSkipKernelChannels ? MAKE_NV_PRINTF_STR("user ") : MAKE_NV_PRINTF_STR(""),
              exceptType);

    // Pass 1: halt all channels.
    kfifoGetChannelIterator(pGpu, pKernelFifo, &chanIt, INVALID_RUNLIST_ID);
    while (kfifoGetNextKernelChannel(pGpu, pKernelFifo, &chanIt, &pKernelChannel) == NV_OK)
    {
        if (kchannelCheckIsKernel(pKernelChannel) && bSkipKernelChannels)
        {
            continue;
        }

        kfifoStartChannelHalt(pGpu, pKernelFifo, pKernelChannel);
    }

    //
    // Pass 2: Wait for the halts to complete, and RC notify the channels.
    // The channel halts require a preemption, which may not be able to complete
    // since the GSP is no longer servicing interrupts. Wait for up to the
    // default GPU timeout value for the preemptions to complete.
    //
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    kfifoGetChannelIterator(pGpu, pKernelFifo, &chanIt, INVALID_RUNLIST_ID);
    while (kfifoGetNextKernelChannel(pGpu, pKernelFifo, &chanIt, &pKernelChannel) == NV_OK)
    {
        if (kchannelCheckIsKernel(pKernelChannel) && bSkipKernelChannels)
        {
            continue;
        }

        kfifoCompleteChannelHalt(pGpu, pKernelFifo, pKernelChannel, &timeout);

        NV_ASSERT_OK(
            krcErrorSetNotifier(pGpu, pKernelRc,
                                pKernelChannel,
                                exceptType,
                                kchannelGetEngineType(pKernelChannel),
                                RC_NOTIFIER_SCOPE_CHANNEL));

        NV_ASSERT_OK(
            krcErrorSendEventNotifications_HAL(pGpu, pKernelRc,
                                               pKernelChannel,
                                               kchannelGetEngineType(pKernelChannel),
                                               0,
                                               exceptType,
                                               RC_NOTIFIER_SCOPE_CHANNEL,
                                               0,
                                               NV_FALSE));
    }
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
    RPC_PARAMS(os_error_log, _v17_00);

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
    RPC_PARAMS(gpuacct_perfmon_util_samples, _v1F_0E);

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

    RPC_PARAMS(perf_gpu_boost_sync_limits_callback, _v17_00);

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
    RPC_PARAMS(perf_bridgeless_info_update, _v17_00);

    kPerfGpuBoostSyncBridgelessUpdateInfo(pGpu, rpc_params->bBridgeless);
}

static void
_kgspRpcNvlinkFaultUpCallback
(
    OBJGPU  *pGpu,
    OBJRPC  *pRpc
)
{
    RPC_PARAMS(nvlink_fault_up, _v17_00);

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
    RPC_PARAMS(nvlink_inband_received_data_256, _v17_00);

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
    RPC_PARAMS(nvlink_inband_received_data_512, _v17_00);

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
    RPC_PARAMS(nvlink_inband_received_data_1024, _v17_00);

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
    RPC_PARAMS(nvlink_inband_received_data_2048, _v17_00);

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
    RPC_PARAMS(nvlink_inband_received_data_4096, _v17_00);

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
    RPC_PARAMS(nvlink_is_gpu_degraded, _v17_00);
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
    RPC_PARAMS(nvlink_fatal_error_recovery, _v17_00);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_FATAL_ERROR_RECOVERY_PARAMS_v17_00 *pDest = &rpc_params->params;
    NV_ASSERT_OK(knvlinkFatalErrorRecovery(pGpu, pKernelNvlink, pDest->bRecoverable, pDest->bLazy));
}

/*!
 * Receive MMU fault queue notification from GSP-RM.
 *
 * Non-replayable fault handling is split between GSP-RM and the UVM driver.
 * GSP-RM copies designated faults to the UVM driver's shadow buffer,
 * and sends a notification.  CPU-RM, in turn, needs to notify the UVM
 * driver (schedule the UVM ISR to be run).
 */
static NV_STATUS
_kgspRpcMMUFaultQueued(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    osQueueMMUFaultHandler(pGpu);

    return NV_OK;
}

static NV_STATUS
_kgspRpcSimRead
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(sim_read, _v1E_01);
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
    RPC_PARAMS(sim_write, _v1E_01);
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
    RPC_PARAMS(semaphore_schedule_callback, _v17_00);
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
    RPC_PARAMS(timed_semaphore_release, _v01_00);
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
    RPC_PARAMS(ucode_libos_print, _v1E_08);

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
_kgspRpcVgpuGspPluginTriggered
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(vgpu_gsp_plugin_triggered, _v17_00);

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    gpuGspPluginTriggeredEvent(pGpu, rpc_params->gfid, rpc_params->notifyIndex);
    return NV_OK;
}

static NV_STATUS
_kgspRpcGspVgpuConfig
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(vgpu_config_event, _v17_00);

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
    RPC_PARAMS(extdev_intr_service, _v17_00);

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

    RPC_PARAMS(vgpu_gsp_mig_ci_config, _v21_03);

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
    status = osQueueWorkItemWithFlags(pGpu,
                                      _kgspRpcMigCiConfigUpdateCallback,
                                      (void *)pParams,
                                      OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW | OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS);
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
    RPC_PARAMS(update_gsp_trace, _v01_00);
    NvU32 i;
    NV_RATS_GSP_TRACE_RECORD *GspTraceRecords = (NV_RATS_GSP_TRACE_RECORD*) (&rpc_params->data);
    for (i = 0; i < rpc_params->records; i++)
    {
        gspTraceEventBufferLogRecord(pGpu, &GspTraceRecords[i]);
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
    RPC_PARAMS(gsp_post_nocat_record, _v01_00);

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

    RPC_PARAMS(pfm_req_hndlr_state_sync_callback, _v21_04);

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
    RPC_PARAMS(gsp_lockdown_notice, _v17_00);

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
    rpc_message_header_v *pMsgHdr = RPC_HDR;
    NV_STATUS nvStatus = NV_OK;
    NvU32 event = pMsgHdr->function;

    NV_PRINTF(LEVEL_INFO, "received event from GPU%d: 0x%x (%s) status: 0x%x size: %d\n",
              gpuGetInstance(pGpu), event, _getRpcName(event), pMsgHdr->rpc_result, pMsgHdr->length);

    _kgspAddRpcHistoryEntry(pRpc, pRpc->rpcEventHistory, &pRpc->rpcEventHistoryCurrent);

    /*
     * Shortlist of RPC's that have been manually screened to be safe without the API lock
     * that are called during GSP bootup
     */
    if ((rpcHandlerContext == KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL_BOOTUP) &&
        (!rmapiLockIsOwner()))
    {
        switch(pMsgHdr->function)
        {
            case NV_VGPU_MSG_EVENT_GSP_RUN_CPU_SEQUENCER:
            case NV_VGPU_MSG_EVENT_UCODE_LIBOS_PRINT:
            case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
            case NV_VGPU_MSG_EVENT_GSP_POST_NOCAT_RECORD:
            case NV_VGPU_MSG_EVENT_GSP_INIT_DONE:
            case NV_VGPU_MSG_EVENT_OS_ERROR_LOG:
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
        case NV_VGPU_MSG_EVENT_GSP_RUN_CPU_SEQUENCER:
            nvStatus = _kgspRpcRunCpuSequencer(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_POST_EVENT:
            nvStatus = _kgspRpcPostEvent(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_RC_TRIGGERED:
            nvStatus = _kgspRpcRCTriggered(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_MMU_FAULT_QUEUED:
            nvStatus = _kgspRpcMMUFaultQueued(pGpu, pRpc);
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

        case NV_VGPU_MSG_EVENT_UCODE_LIBOS_PRINT:
            nvStatus = _kgspRpcUcodeLibosPrint(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_VGPU_GSP_PLUGIN_TRIGGERED:
            nvStatus = _kgspRpcVgpuGspPluginTriggered(pGpu, pRpc);
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

        case NV_VGPU_MSG_EVENT_RECOVERY_ACTION:
            nvStatus = _kgspRpcGspEventRecoveryAction(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_GSP_INIT_DONE:   // Handled by _kgspRpcRecvPoll.
        default:
            //
            // Log, but otherwise ignore unexpected events.
            //
            // We will get here if the previous RPC timed out.  The response
            // eventually comes in as an unexpected event.  The error handling
            // for the timeout should have already happened.
            //
            NV_PRINTF(LEVEL_ERROR, "Unexpected RPC event from GPU%d: 0x%x (%s)\n",
                      gpuGetInstance(pGpu), event, _getRpcName(event));
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
    RPC_PARAMS(fecs_error, _v26_02);

    switch (rpc_params->error_type)
    {
        case FECS_ERROR_EVENT_TYPE_BUFFER_RESET_REQUIRED:
        case FECS_ERROR_EVENT_TYPE_BUFFER_FULL:
        {
            FECS_ERROR_REPORT *pErrorReport = portMemAllocNonPaged(sizeof(*pErrorReport));
            NV_ASSERT_OR_RETURN(pErrorReport != NULL, NV_ERR_NO_MEMORY);

            pErrorReport->grIdx = rpc_params->grIdx;
            pErrorReport->errorType = rpc_params->error_type;
            status = osQueueWorkItemWithFlags(pGpu,
                                              _kgspRpcGspEventHandleFecsBufferError,
                                              pErrorReport,
                                              OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RO |
                                              OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE);

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
    RPC_PARAMS(recovery_action, _v28_01);

    switch (rpc_params->type)
    {
        case GPU_RECOVERY_EVENT_TYPE_REFRESH:
            gpuRefreshRecoveryAction(pGpu, NV_FALSE);
            break;
        case GPU_RECOVERY_EVENT_TYPE_GPU_DRAIN_P2P:
            gpuSetRecoveryDrainP2P(pGpu, rpc_params->value);
            break;
        case GPU_RECOVERY_EVENT_TYPE_GPU_REBOOT:
            gpuSetRecoveryRebootRequired(pGpu, rpc_params->value, pGpu->bBlockNewWorkload);
            break;
        case GPU_RECOVERY_EVENT_TYPE_SYS_REBOOT:
            sysSetRecoveryRebootRequired(pSys, rpc_params->value);
            break;

        default:
            status = NV_ERR_INVALID_PARAMETER;
            break;
    }

    return status;
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
    KernelGspRpcEventHandlerContext rpcHandlerContext
)
{
    NV_STATUS nvStatus;

    // Issue a memory barrier to ensure we see any queue updates.
    // Note: Without the fence, the CPU may get stuck in an infinite loop
    //       waiting for a message that has already arrived.
    portAtomicMemoryFenceFull();

    nvStatus = GspMsgQueueReceiveStatus(pRpc->pMessageQueueInfo, pGpu);

    if (nvStatus == NV_OK)
    {
        rpc_message_header_v *pMsgHdr = RPC_HDR;

        if (pMsgHdr->function == expectedFunc)
            return NV_WARN_MORE_PROCESSING_REQUIRED;

        _kgspProcessRpcEvent(pGpu, pRpc, rpcHandlerContext);
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
    KernelGspRpcEventHandlerContext rpcHandlerContext
)
{
    NV_STATUS nvStatus = NV_OK;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    while (nvStatus == NV_OK)
    {
        nvStatus = _kgspRpcDrainOneEvent(pGpu, pRpc, expectedFunc, rpcHandlerContext);
        kgspDumpGspLogs(pKernelGsp, NV_FALSE);
    }

    // If GSP-RM has died, the GPU will need to be reset
    if (!kgspHealthCheck_HAL(pGpu, pKernelGsp))
        return NV_ERR_RESET_REQUIRED;

    if (nvStatus == NV_WARN_NOTHING_TO_DO)
        nvStatus = NV_OK;

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
                              "    %c%-4d %-4d %-21.21s 0x%016llx 0x%016llx 0x%016llx 0x%016llx %6llu%cs %c\n",
                              ((historyIndex == 0) ? ' ' : '-'),
                              historyIndex,
                              pEntry->function,
                              _getRpcName(pEntry->function),
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
                              "    %c%-4d %-4d %-21.21s 0x%016llx 0x%016llx 0x%016llx 0x%016llx          %c\n",
                              ((historyIndex == 0) ? ' ' : '-'),
                              historyIndex,
                              pEntry->function,
                              _getRpcName(pEntry->function),
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
    const rpc_message_header_v *pMsgHdr = RPC_HDR;
    NvU64  data[2];
    NV_STATUS status = NV_OK;
    NvU32  historyIndex;
    NvU32  historyEntry;
    RpcHistoryEntry *pEntry = NULL;
    const NvU32 rpcEntriesToLog = (RPC_HISTORY_DEPTH > 8) ? 8 : RPC_HISTORY_DEPTH;

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
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA0, pEntry->data[0]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA1, pEntry->data[1]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_STARTTIMESTAMP, pEntry->ts_start);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_ENDTIMESTAMP, pEntry->ts_end);
                if (pEntry->ts_end > pEntry->ts_start)
                {
                    prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DURATION, pEntry->ts_end > pEntry->ts_start);
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
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA0, pEntry->data[0]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DATA1, pEntry->data[1]);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_STARTTIMESTAMP, pEntry->ts_start);
                prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_ENDTIMESTAMP, pEntry->ts_end);
                if (pEntry->ts_end > pEntry->ts_start)
                {
                    prbEncAddUInt64(pProtobufData, GSP_RPCENTRY_DURATION, pEntry->ts_end - pEntry->ts_start);
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
    const rpc_message_header_v *pMsgHdr = RPC_HDR;
    NvU32  historyIndex;
    NvU32  historyEntry;
    NvU64  activeData[2];
    const NvU32 rpcEntriesToLog = (RPC_HISTORY_DEPTH > 8) ? 8 : RPC_HISTORY_DEPTH;

    _kgspGetActiveRpcDebugData(pRpc, pMsgHdr->function,
                               &activeData[0], &activeData[1]);
    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "GPU%d GSP RPC buffer contains function %d (%s) and data 0x%016llx 0x%016llx.\n",
                      gpuGetInstance(pGpu),
                      pMsgHdr->function, _getRpcName(pMsgHdr->function),
                      activeData[0], activeData[1]);

    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "GPU%d RPC history (CPU -> GSP):\n",
                      gpuGetInstance(pGpu));
    NV_ERROR_LOG_DATA(pGpu, errorNum,
                      "    entry function                   data0              data1              ts_start           ts_end             duration actively_polling\n");
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
                      "    entry function                   data0              data1              ts_start           ts_end             duration during_incomplete_rpc\n");
    for (historyIndex = 0; historyIndex < rpcEntriesToLog; historyIndex++)
    {
        historyEntry = (pRpc->rpcEventHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
        _kgspLogRpcHistoryEntry(pGpu, errorNum, historyIndex, &pRpc->rpcEventHistory[historyEntry],
                                _kgspIsTimestampDuringRecentRpc(pRpc,
                                                                pRpc->rpcEventHistory[historyEntry].ts_start,
                                                                NV_TRUE/*bCheckIncompleteRpcsOnly*/));
    }
}

/*!
 * Log Xid 119 - GSP RPC Timeout
 */
static void
_kgspLogXid119
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 expectedFunc
)
{
    RpcHistoryEntry *pHistoryEntry = &pRpc->rpcHistory[pRpc->rpcHistoryCurrent];
    NvU64 ts_end = osGetTimestamp();
    NvU64 duration;
    char  durationUnitsChar;
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    if (pRpc->timeoutCount == 1)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "********************************* GSP Timeout **********************************\n");
        NV_PRINTF(LEVEL_ERROR,
                  "Note: Please also check logs above.\n");
    }

    NV_ASSERT(expectedFunc == pHistoryEntry->function);

    NV_ASSERT(ts_end > pHistoryEntry->ts_start);
    duration = _tsDiffToDuration(ts_end - pHistoryEntry->ts_start, &durationUnitsChar);

    NV_ERROR_LOG(pGpu, GSP_RPC_TIMEOUT,
                 "Timeout after %llus of waiting for RPC response from GPU%d GSP! Expected function %d (%s) (0x%llx 0x%llx).",
                 (durationUnitsChar == 'm' ? duration / 1000 : duration),
                 gpuGetInstance(pGpu),
                 expectedFunc,
                 _getRpcName(expectedFunc),
                 pHistoryEntry->data[0],
                 pHistoryEntry->data[1]);

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_GPU_UNAVAILABLE, NULL, 0, GSP_RPC_TIMEOUT, 0);

    if (pRpc->timeoutCount == 1)
    {
        kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_GSP_RPC_TIMEOUT);
        prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_XID, 119);
        kgspLogRpcDebugInfoToProtobuf(pGpu, pRpc, pKernelGsp, &pKernelGsp->nocatData.nocatBuffer);
        kgspPostNocatData(pGpu, pKernelGsp, osGetTimestamp());

        kgspLogRpcDebugInfo(pGpu, pRpc, GSP_RPC_TIMEOUT, NV_TRUE/*bPollingForRpcResponse*/);
        osAssertFailed();

        NV_PRINTF(LEVEL_ERROR,
                  "********************************************************************************\n");
    }
}

static void
_kgspLogRpcSanityCheckFailure
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 rpcStatus,
    NvU32 expectedFunc
)
{
    RpcHistoryEntry *pHistoryEntry = &pRpc->rpcHistory[pRpc->rpcHistoryCurrent];

    NV_ASSERT(expectedFunc == pHistoryEntry->function);

    NV_PRINTF(LEVEL_ERROR,
              "GPU%d sanity check failed 0x%x waiting for RPC response from GSP. Expected function %d (%s) (0x%llx 0x%llx).\n",
              gpuGetInstance(pGpu),
              rpcStatus,
              expectedFunc,
              _getRpcName(expectedFunc),
              pHistoryEntry->data[0],
              pHistoryEntry->data[1]);

    kgspLogRpcDebugInfo(pGpu, pRpc, GSP_RPC_TIMEOUT, NV_TRUE/*bPollingForRpcResponse*/);
    osAssertFailed();
}

static void
_kgspRpcIncrementTimeoutCountAndRateLimitPrints
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    pRpc->timeoutCount++;

    if ((pRpc->timeoutCount == (RPC_TIMEOUT_GPU_RESET_THRESHOLD + 1)) &&
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

    pRpc->bQuietPrints = ((pRpc->timeoutCount > RPC_TIMEOUT_GPU_RESET_THRESHOLD) &&
                          ((pRpc->timeoutCount % (RPC_TIMEOUT_PRINT_RATE_SKIP + 1)) != 0));
}

/*!
 * GSP client RM RPC poll routine
 */
static NV_STATUS
_kgspRpcRecvPoll
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32   expectedFunc
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NV_STATUS  rpcStatus = NV_OK;
    NV_STATUS  timeoutStatus = NV_OK;
    RMTIMEOUT  timeout;
    NvU32      timeoutUs;
    NvU32      timeoutFlags;
    NvBool     bSlowGspRpc = IS_EMULATION(pGpu) || IS_SIMULATION(pGpu);
    NvU32      gpuMaskUnused;

#if defined(GSPRM_HWASAN_ENABLE)
    //
    // TODO HWASAN is SLOW. More investigation is required on where the
    // bottlenecks are and how to uncork them.
    //
    bSlowGspRpc = bSlowGspRpc
        || (expectedFunc == NV_VGPU_MSG_EVENT_GSP_INIT_DONE)
        || (expectedFunc == NV_VGPU_MSG_FUNCTION_UNLOADING_GUEST_DRIVER);
#endif

    KernelGspRpcEventHandlerContext rpcHandlerContext = KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL;
    if (expectedFunc == NV_VGPU_MSG_EVENT_GSP_INIT_DONE)
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

        rpcStatus = _kgspRpcDrainEvents(pGpu, pKernelGsp, expectedFunc, rpcHandlerContext);

        switch (rpcStatus) {
            case NV_WARN_MORE_PROCESSING_REQUIRED:
                // The synchronous RPC response we were waiting for is here
                _kgspCompleteRpcHistoryEntry(pRpc->rpcHistory, pRpc->rpcHistoryCurrent);
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
                _kgspLogRpcSanityCheckFailure(pGpu, pRpc, rpcStatus, expectedFunc);
                pRpc->bQuietPrints = NV_TRUE;
            }
            goto done;
        }

        if (timeoutStatus == NV_ERR_TIMEOUT)
        {
            rpcStatus = timeoutStatus;

            _kgspRpcIncrementTimeoutCountAndRateLimitPrints(pGpu, pRpc);

            if (!pRpc->bQuietPrints)
            {
                _kgspLogXid119(pGpu, pRpc, expectedFunc);
            }

            // Detect for 3 back to back GSP RPC timeout
            if (pRpc->timeoutCount == RPC_TIMEOUT_GPU_RESET_THRESHOLD)
            {
                // GSP is completely stalled and cannot be recovered. Mark the GPU for reset.
                NV_ASSERT_FAILED("Back to back GSP RPC timeout detected! GPU marked for reset");
                gpuMarkDeviceForReset(pGpu);

                // For Windows, if TDR is supported, trigger TDR to recover the system.
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_TDR_EVENT))
                {
                    NV_ASSERT_FAILED("Triggering TDR to recover from GSP hang");
                    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_UCODE_RESET, NULL, 0, 0, 0);
                }
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

        osSpinLoop();
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

    pRpc->message_buffer  = (NvU32 *)pRpc->pMessageQueueInfo->pRpcMsgBuf;
    pRpc->maxRpcSize      = GSP_MSG_QUEUE_RPC_SIZE_MAX;

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
    NvU32 gfid,
    const char *szMemoryId,
    const char *szPrefix,
    const char *elfSectionName,
    NvU64 bufOffset,
    NvU64 bufSize,
    RM_LIBOS_LOG_MEM *taskLogArr
)
{
    NV_STATUS nvStatus = NV_OK;
    RM_LIBOS_LOG_MEM *pTaskLog = NULL;
    char vm_string[8];

    pTaskLog = &taskLogArr[gfid - 1];
    NvP64 pVa = NvP64_NULL;

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pTaskLog->pTaskLogDescriptor,
                        pGpu,
                        bufSize,
                        RM_PAGE_SIZE,
                        NV_TRUE, ADDR_FBMEM, NV_MEMORY_CACHED,
                        MEMDESC_FLAGS_NONE),
        exit);

    memdescDescribe(pTaskLog->pTaskLogDescriptor, ADDR_FBMEM, bufOffset,  bufSize);

    pVa = memdescMapInternal(pGpu, pTaskLog->pTaskLogDescriptor, TRANSFER_FLAGS_NONE);
    if (pVa != NvP64_NULL)
    {
        pTaskLog->pTaskLogBuffer = pVa;
        portMemSet(pTaskLog->pTaskLogBuffer, 0, bufSize);

        pTaskLog->id8 = _kgspGenerateInitArgId(szMemoryId);

        nvDbgSnprintf(vm_string, sizeof(vm_string), "%s%d", szPrefix, gfid);

        libosLogAddLogEx(&pKernelGsp->logDecodeVgpuPartition[gfid - 1],
            pTaskLog->pTaskLogBuffer,
            memdescGetSize(pTaskLog->pTaskLogDescriptor),
            pGpu->gpuInstance,
            (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
            gpuGetChipImpl(pGpu),
            vm_string,
            elfSectionName,
            0,
            pKernelGsp->pBuildIdSection);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to map memory for %s task log buffer for vGPU partition \n", szPrefix);
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
    NvU64 kernelLogBuffSize,
    NvBool *pPreserveLogBufferFull
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
    NvBool bPreserveLogBufferFull = NV_FALSE;

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
        if (!bPreserveLogBufferFull)
        {
            bPreserveLogBufferFull = isLibosPreserveLogBufferFull(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], pGpu->gpuInstance);
        }

        const nv_firmware_task_log_info_t* pInfo = nv_firmware_log_info_for_task(logInitValues[i].taskId);

        if (pInfo == NULL)
        {
            nvStatus = NV_ERR_INVALID_STATE;
            goto exit;
        }

        nvStatus = _setupLogBufferVgpu(
            pGpu,
            pKernelGsp,
            gfid,
            pInfo->memory_id,
            pInfo->prefix,
            pInfo->elf_section_name,
            logInitValues[i].bufOffset,
            logInitValues[i].bufSize,
            logInitValues[i].taskLogArr
        );

        if (nvStatus != NV_OK)
            goto exit;
    }

    if (!bPreserveLogBufferFull)
    {
        bPreserveLogBufferFull = isLibosPreserveLogBufferFull(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], pGpu->gpuInstance);
    }

    const nv_firmware_kernel_log_info_t *pInfo = nv_firmware_kernel_log_info_for_gpu(gpuGetChipArch(pGpu), gpuGetChipImpl(pGpu));

    if (pInfo->elf_section_name == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "Unknown chip for libos kernel logging\n");

        nvStatus = NV_ERR_INVALID_STATE;
        goto exit;
    }

    nvStatus = _setupLogBufferVgpu(
                pGpu,
                pKernelGsp,
                gfid,
                NV_FIRMWARE_KERNEL_LOG_MEMORY_ID,
                NV_FIRMWARE_KERNEL_LOG_PREFIX,
                pInfo->elf_section_name,
                kernelLogBuffOffset,
                kernelLogBuffSize,
                pKernelGsp->libosKernelLogMem
        );

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

    *pPreserveLogBufferFull = bPreserveLogBufferFull;

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
    libosPreserveLogs(&pKernelGsp->logDecodeVgpuPartition[gfid - 1]);

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
                         NV_TRUE, osGetCurrentProcess(),
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

    // Pass the PTE table for the log buffer in the log buffer, after the put pointer.
    memdescGetPhysAddrs(pLog->pTaskLogDescriptor,
                        AT_GPU,
                        0,
                        RM_PAGE_SIZE,
                        NV_CEIL(memdescGetSize(pLog->pTaskLogDescriptor), RM_PAGE_SIZE),
                        &pLog->pTaskLogBuffer[1]);

    pLog->id8 = _kgspGenerateInitArgId(szMemoryId);

    NvU32 libosLogFlags = (bEnableNvlog ? 0 : LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED);

    ct_assert(NV_FIRMWARE_GPU_ARCH_SHIFT == GPU_ARCH_SHIFT);

    libosLogAddLogEx(&pKernelGsp->logDecode,
        pLog->pTaskLogBuffer,
        memdescGetSize(pLog->pTaskLogDescriptor),
        pGpu->gpuInstance,
        (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
        gpuGetChipImpl(pGpu),
        szPrefix,
        elfSectionName,
        libosLogFlags,
        pKernelGsp->pBuildIdSection);

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
    static const struct
    {
        nv_firmware_task_t taskId;
        NvU32       size;
        NvBool      bEnableNvlog;
    } logInitValues[] =
    {
        {NV_FIRMWARE_TASK_INIT,  0x10000, NV_TRUE},  // 64KB for stack traces
#if defined(DEVELOP) || defined(DEBUG)
        {NV_FIRMWARE_TASK_INTR,  0x40000, NV_TRUE},  // 256KB ISR debug log on develop/debug builds
        {NV_FIRMWARE_TASK_RM,    0x40000, NV_TRUE},  // 256KB RM debug log on develop/debug builds
#else
        {NV_FIRMWARE_TASK_INTR,  0x10000, NV_TRUE},  // 64KB ISR debug log on develop/debug builds
        {NV_FIRMWARE_TASK_RM,    0x10000, NV_TRUE},  // 64KB RM debug log on release builds
#endif
        {NV_FIRMWARE_TASK_MNOC,  0x10000, NV_TRUE},  // 64KB MNOC debug log on release builds
        {NV_FIRMWARE_TASK_DEBUG, 0x10000, NV_FALSE}, // 64KB task debugger debug log
        {NV_FIRMWARE_TASK_ROOT,  0x1000,  NV_TRUE},
    };

    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) + 1 /* LOGKRNL */ <= LOGIDX_SIZE);

    NV_STATUS nvStatus = NV_OK;
    NvU32      registeredIdx = 0;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    libosLogCreate(&pKernelGsp->logDecode);

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

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
    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

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
        memdescFree(pKernelGsp->pNotifyOpSurfMemDesc);
        memdescDestroy(pKernelGsp->pNotifyOpSurfMemDesc);
    }

    pKernelGsp->pNotifyOpSurfMemDesc = NULL;
    pKernelGsp->pNotifyOpSurf         = NULL;
    pKernelGsp->pNotifyOpSurfPriv     = NULL;
}

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
/*!
 * Free LIBOS task coverage structures
 */
static void
_kgspFreeTaskRMCoverageStructure
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{

    RM_LIBOS_COVERAGE_MEM *pCoverage = &pKernelGsp->taskRmCoverage;

    // release coverage memory
    if (pCoverage->pTaskCoverageBuffer != NULL)
    {
        memdescUnmap(pCoverage->pTaskCoverageDescriptor,
                        NV_TRUE, osGetCurrentProcess(),
                        (void *)pCoverage->pTaskCoverageBuffer,
                        pCoverage->pTaskCoverageMappingPriv);
        pCoverage->pTaskCoverageBuffer = NULL;
        pCoverage->pTaskCoverageMappingPriv = NULL;
    }

    if (pCoverage->pTaskCoverageDescriptor != NULL)
    {
        memdescFree(pCoverage->pTaskCoverageDescriptor);
        memdescDestroy(pCoverage->pTaskCoverageDescriptor);
        pCoverage->pTaskCoverageDescriptor = NULL;
    }

}

/*
 * Init task_rm coverage
 */
static NV_STATUS
_kgspSetupTaskRMCoverageStructure (
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    ENGDESCRIPTOR engDesc
)
{
    NV_STATUS nvStatus = NV_OK;
    RM_LIBOS_COVERAGE_MEM *pRmCov = &pKernelGsp->taskRmCoverage;
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescCreate(&pRmCov->pTaskCoverageDescriptor,
                        pGpu,
                        BULLSEYE_GSP_RM_COVERAGE_SIZE,
                        RM_PAGE_SIZE,
                        NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                        MEMDESC_FLAGS_NONE), done);

    memdescTagAlloc(nvStatus,
                    NV_FB_ALLOC_RM_INTERNAL_OWNER_COV_TASK_DESCRIPTOR, pRmCov->pTaskCoverageDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus, done);
    NvP64 covPva = NvP64_NULL;
    NvP64 covPpriv = NvP64_NULL;
    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescMap(pRmCov->pTaskCoverageDescriptor, 0,
                    memdescGetSize(pRmCov->pTaskCoverageDescriptor),
                    NV_TRUE, NV_PROTECT_READ_WRITE,
                    &covPva, &covPpriv),
        done);
    pRmCov->pTaskCoverageBuffer = covPva;
    pRmCov->pTaskCoverageMappingPriv = covPpriv;
    pRmCov->id8 = _kgspGenerateInitArgId("RMCOV");
done:
    if (nvStatus != NV_OK)
    {
        _kgspFreeTaskRMCoverageStructure(pGpu, pKernelGsp);
    }
    return nvStatus;
}
#endif

static void
_kgspReadRegkeyOverrides(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    (void)osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_WPR_END_MARGIN, &pKernelGsp->wprEndMarginOverride);

    if (osReadRegistryDword(pGpu, NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB, &pKernelGsp->heapSizeMBOverride) != NV_OK)
    {
        pKernelGsp->heapSizeMBOverride = 0;
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

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    nvStatus = _kgspSetupTaskRMCoverageStructure(pGpu, pKernelGsp, engDesc);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init task_rm coverage structure failed");
        goto done;
    }
#endif

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
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
        _kgspFreeTaskRMCoverageStructure(pGpu, pKernelGsp);
#endif
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
_kgspPrepareScrubberImageIfNeeded(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // Prepare Scrubber ucode image if pre-scrubbed memory is insufficient
    NvU64 neededSize = pKernelGsp->pWprMeta->fbSize - pKernelGsp->pWprMeta->gspFwRsvdStart;
    NvU64 prescrubbedSize = kgspGetPrescrubbedTopFbSize(pGpu, pKernelGsp);
    NV_PRINTF(LEVEL_INFO, "pre-scrubbed memory: 0x%llx bytes, needed: 0x%llx bytes\n",
              prescrubbedSize, neededSize);

    // WAR for Bug 5016200 - Always run scrubber from kernel RM for ADA config
    if ((neededSize > prescrubbedSize) || kgspIsScrubberImageSupported(pGpu, pKernelGsp))
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgspAllocateScrubberUcodeImage(pGpu, pKernelGsp, &pKernelGsp->pScrubberUcode));

    return NV_OK;
}

/*!
 * Prepare and place RPCs in message queue that GSP-RM will process
 * in early boot before OBJGPU is created.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelGsp  KernelGsp object pointer
 *
 * @return NV_OK if RPCs queued successfully.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspQueueAsyncInitRpcs_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status = NV_OK;

    NV_RM_RPC_GSP_SET_SYSTEM_INFO(pGpu, status);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("NV_RM_RPC_GSP_SET_SYSTEM_INFO", status);
        return status;
    }

    NV_RM_RPC_SET_REGISTRY(pGpu, status);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("NV_RM_RPC_SET_REGISTRY", status);
        return status;
    }

    return NV_OK;
}

static void
_kgspSetFwWprLayoutOffset
(
    OBJGPU *pGpu
)
{
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_FW_WPR_OFFSET_SET_BY_ACR))
    {
        return;
    }

    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    KernelGsp      *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    GspFwWprMeta     *pWprMeta = pKernelGsp->pWprMeta;

    pWprMeta->nonWprHeapOffset   = pGSCI->fwWprLayoutOffset.nonWprHeapOffset;
    pWprMeta->frtsOffset         = pGSCI->fwWprLayoutOffset.frtsOffset;
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
    NV_STATUS status;
    NvBool bEccDisabled = !kmemsysCheckReadoutEccEnablement(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    NV_ASSERT_OR_RETURN(pbRetry != NULL, NV_ERR_INVALID_ARGUMENT);
    *pbRetry = NV_FALSE;

    // Fail early if WPR2 is up
    if (kgspIsWpr2Up_HAL(pGpu, pKernelGsp))
    {
        NV_PRINTF(LEVEL_ERROR, "unexpected WPR2 already up, cannot proceed with booting GSP\n");
        NV_PRINTF(LEVEL_ERROR, "(the GPU is likely in a bad state and may need to be reset)\n");
        return NV_ERR_INVALID_STATE;
    }

    // Populate WPR meta structure (requires knowing FB size on dGPU, which depends on GFW_BOOT)
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgspPopulateWprMeta_HAL(pGpu, pKernelGsp, pGspFw));

    // If the new FB layout requires a scrubber ucode to scrub additional space, prepare it now
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kgspPrepareScrubberImageIfNeeded(pGpu, pKernelGsp));

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
            pKernelGsp->pWprMeta->flags |= GSP_FW_FLAGS_SCAN_RECOVERY_MARGIN;
    }

    // Proceed with GSP boot
    status = kgspBootstrap_HAL(pGpu, pKernelGsp, KGSP_BOOT_MODE_NORMAL);

    if (status != NV_OK)
    {
        // Increment the bootAttempt counter only on failure to boot GSP
        pKernelGsp->bootAttempts++;
        if (gpuCheckEccCounts_HAL(pGpu) || (bEccDisabled && !hypervisorIsVgxHyper()))
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

    // Initialize libos init args list
    kgspSetupLibosInitArgs(pGpu, pKernelGsp);

    // Fill in the GSP-RM message queue init parameters
    kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, NULL);

    NvBool bDelayInitRpcs = NV_FALSE;
    //
    // If SPDM is enabled, RPC traffic may be encrypted. Since we
    // can't encrypt until GSP boots and session is established, we must send
    // these messages later (kgspBootstrap_HAL).
    //
    Spdm *pSpdm = GPU_GET_SPDM(pGpu);
    if (pSpdm != NULL && pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
    {
        bDelayInitRpcs = NV_TRUE;
    }

    if (!bDelayInitRpcs)
    {
        //
        // Stuff the message queue with async init messages that will be run
        // before OBJGPU is created.
        //
        status = kgspQueueAsyncInitRpcs(pGpu, pKernelGsp);
        if (status != NV_OK)
        {
            goto done;
        }
    }

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
        NV_PRINTF(LEVEL_ERROR, "Initial shift, %d, is larger than max allowed [0, %d]. Modulo applied\n",
                  pKernelGsp->bootAttempts, maxGspBootAttempts - 1);
        pKernelGsp->bootAttempts = pKernelGsp->bootAttempts % maxGspBootAttempts;
    }

    NvBool bRetry = NV_FALSE;
    do
    {
        // Reset the thread state timeout after failed attempts to prevent premature timeouts.
        if (status != NV_OK)
            threadStateResetTimeout(pGpu);

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
            NV_PRINTF(LEVEL_ERROR, "Max GSP-RM boot attempts exceeded: %d/%d\n",
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

    // at this point we should be able to exchange RPCs with RM offload task
    NV_RM_RPC_SET_GUEST_SYSTEM_INFO(pGpu, status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SET_GUEST_SYSTEM_INFO failed: 0x%x\n", status);
        goto done;
    }

    NV_RM_RPC_GET_GSP_STATIC_INFO(pGpu, status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GET_GSP_STATIC_INFO failed: 0x%x\n", status);
        goto done;
    }

    // Set PDB properties as per data from GSP.
    _kgspInitGpuProperties(pGpu);

    // Set FW WPR layout offset as per data from GSP.
    _kgspSetFwWprLayoutOffset(pGpu);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspStartLogPolling(pGpu, pKernelGsp), done);

done:
    pKernelGsp->bInInit = NV_FALSE;

    if (status != NV_OK)
    {
        KernelPmu *pKernelPmu = GPU_GET_KERNEL_PMU(pGpu);

        // Force sync GSP logs
        kgspDumpGspLogs(pKernelGsp, NV_TRUE);

        // Preserve any captured GSP-RM logs
        libosPreserveLogs(&pKernelGsp->logDecode);

        if (pKernelPmu != NULL)
        {
            // If PMU init fails, kgsp init will also fail
            libosPreserveLogs(&pKernelPmu->logDecode);
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
    NvBool bInPmTransition = (unloadMode != KGSP_UNLOAD_MODE_NORMAL);
    NvBool bGc6Entering = (unloadMode == KGSP_UNLOAD_MODE_GC6_ENTER);

    NV_PRINTF(LEVEL_INFO, "unloading GSP-RM\n");
    NV_RM_RPC_UNLOADING_GUEST_DRIVER(pGpu, rpcStatus, bInPmTransition, bGc6Entering, newPmLevel);

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        // FIPS: If CC enabled, we need to confirm GSP-RM was able to teardown CC state.
        kgspCheckGspRmCcCleanup_HAL(pGpu, pKernelGsp);
    }

    // Wait for GSP-RM processor to suspend
    kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp);

    // Dump GSP-RM logs and reset before proceeding with the rest of teardown
    kgspDumpGspLogs(pKernelGsp, NV_FALSE);

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    kgspCollectGspCoverage(pGpu, pKernelGsp);
#endif

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
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    _kgspFreeTaskRMCoverageStructure(pGpu, pKernelGsp);
#endif
    _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    _kgspFreeBootBinaryImage(pGpu, pKernelGsp);
    _kgspFreeSimAccessBuffer(pGpu, pKernelGsp);
    _kgspFreeNotifyOpSharedSurface(pGpu, pKernelGsp);

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
    if (pKernelGsp->bInInit || pKernelGsp->pLogElf || bSyncNvLog
      || pKernelGsp->bHasVgpuLogs
    )
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
kgspCollectGspCoverage_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU8 *pSysmemBuffer = (NvU8*) pKernelGsp->taskRmCoverage.pTaskCoverageBuffer;
    codecovmgrMergeCoverage(pSys->pCodeCovMgr, 0, pGpu->gpuInstance, pSysmemBuffer);
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
    MESSAGE_QUEUE_COLLECTION *pMQCollection   = pKernelGsp->pMQCollection;
    GSP_SR_INIT_ARGUMENTS *pSrInitArgs =  &pGspArgs->srInitArguments;

    // Setup the message queue arguments
    pMQInitArgs->sharedMemPhysAddr      = pMQCollection->sharedMemPA;
    pMQInitArgs->pageTableEntryCount    = pMQCollection->pageTableEntryCount;
    pMQInitArgs->cmdQueueOffset         = pMQCollection->pageTableSize;
    pMQInitArgs->statQueueOffset        = pMQInitArgs->cmdQueueOffset + pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX].commandQueueSize;

    if (pGspInitArgs == NULL)
    {
        NvU32 stackReg;

        // For normal boot, determine whether to use dmem stack
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_STACK_PLACEMENT, &stackReg) != NV_OK)
        {
            stackReg = NV_REG_STR_RM_GSP_STACK_PLACEMENT_DEFAULT;
        }

        pGspArgs->bDmemStack = ((stackReg == NV_REG_STR_RM_GSP_STACK_PLACEMENT_DEFAULT) ||
                                (stackReg == NV_REG_STR_RM_GSP_STACK_PLACEMENT_DMEM));

        pSrInitArgs->bInPMTransition     = NV_FALSE;
        pSrInitArgs->oldLevel            = 0;
        pSrInitArgs->flags               = 0;
    }
    else
    {
        pSrInitArgs->bInPMTransition     = NV_TRUE;
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

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        pGspArgs->sysmemHeapArgs.pa = memdescGetPhysAddr(pKernelGsp->pSysmemHeapDescriptor, AT_GPU, 0);
        pGspArgs->sysmemHeapArgs.size = pKernelGsp->pSysmemHeapDescriptor->Size;
    }
    else
    {
        pGspArgs->sysmemHeapArgs.pa = 0;
        pGspArgs->sysmemHeapArgs.size = 0;
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

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

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
                     NV_TRUE, osGetCurrentProcess(),
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

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

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
    NV_ASSERT_OR_RETURN(pChipFamilyName != NULL, NV_ERR_INVALID_STATE);

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

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgspFwContainerVerifyVersion(pGpu, pKernelGsp,
            pGspFw->pBuf,
            pGspFw->size,
            "GSP firmware image"));

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

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

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
    memdescUnmap(*ppMemdescRadix3, NV_TRUE, osGetCurrentProcess(),
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
    portMemSet(pLibosInitArgs, 0, LIBOS_INIT_ARGUMENTS_SIZE);


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
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    pLibosInitArgs[idx].kind = LIBOS_MEMORY_REGION_CONTIGUOUS;
    pLibosInitArgs[idx].loc  = LIBOS_MEMORY_REGION_LOC_SYSMEM;
    pLibosInitArgs[idx].id8  = pKernelGsp->taskRmCoverage.id8;
    pLibosInitArgs[idx].pa   = memdescGetPhysAddr(pKernelGsp->taskRmCoverage.pTaskCoverageDescriptor, AT_GPU, 0);
    pLibosInitArgs[idx].size = memdescGetSize(pKernelGsp->taskRmCoverage.pTaskCoverageDescriptor);
    ++idx;
#endif

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
    NV_ASSERT_OK(_kgspRpcDrainEvents(pGpu, pKernelGsp, NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS, KGSP_RPC_EVENT_HANDLER_CONTEXT_INTERRUPT));
}

/*!
 * Wait for GSP-RM initialization to complete.
 */
NV_STATUS
kgspWaitForRmInitDone_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    OBJRPC *pRpc = pKernelGsp->pRpc;

    //
    // Kernel RM can timeout when GSP-RM has an error condition.  Give GSP-RM
    // a chance to report the error before we pull the rug out from under it.
    //
    threadStateResetTimeout(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rpcRecvPoll(pGpu, pRpc, NV_VGPU_MSG_EVENT_GSP_INIT_DONE));

    //
    // Now check if RPC really succeeded (NV_VGPU_MSG_RESULT_* are defined to
    // equivalent NV_STATUS codes in RM).
    //
    NV_ASSERT_OK_OR_RETURN(RPC_HDR->rpc_result);

    pGpu->gspRmInitialized = NV_TRUE;
    if (hypervisorIsVgxHyper() && pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX))
    {
        // Decrease timeout values for VGX driver
        timeoutInitializeGpuDefault(&pGpu->timeoutData, pGpu);
    }

    return NV_OK;
}

/*!
 * Execute a sequencer buffer coming from GSP
 *
 * @param[in]      pGpu             GPU object pointer
 * @param[in]      pKernelGsp       KernelGsp object pointer
 * @param[in]      pRunCpuSeqParams Sequence buffer RPC parameters
 *
 * @return NV_OK if the GSP sequencer buffer has been executed successfully
 *         NV_ERR_INVALID_STATE if the sequencer buffer is not allocated
 *         NV_ERR_INVALID_DATA is the sequencer buffer is malformed
 */
NV_STATUS
kgspExecuteSequencerBuffer_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    void      *pRunCpuSeqParams
)
{
    rpc_run_cpu_sequencer_v17_00 *pParams = (rpc_run_cpu_sequencer_v17_00 *)pRunCpuSeqParams;
    NvU32 *pCmd = pParams->commandBuffer;
    NvU32 buffer_end = pParams->cmdIndex;
    NvU32 current_cmd_index = 0;
    NV_STATUS nvStatus = NV_OK;
    NvU32 payloadSize;

    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN((pParams->bufferSizeDWord != 0), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(buffer_end < pParams->bufferSizeDWord, NV_ERR_INVALID_DATA);

    while (current_cmd_index < buffer_end)
    {
        NvU32 opCode = pCmd[current_cmd_index++];
        payloadSize = GSP_SEQUENCER_PAYLOAD_SIZE_DWORDS(opCode);

        NV_ASSERT_OR_RETURN(current_cmd_index + payloadSize <= buffer_end, NV_ERR_INVALID_DATA);

        //
        // Handling of sequencer commands is split between those commands
        // that are common to all architectures (handled directly here) and
        // those commands that are arch-specific and are handled via the
        // kgspExecuteSequencerCommand_HAL() call below.
        //
        switch (opCode)
        {
            // 2 arguments
            case GSP_SEQ_BUF_OPCODE_REG_WRITE:
            {
                GSP_SEQ_BUF_PAYLOAD_REG_WRITE regWrite;
                portMemCopy(&regWrite, sizeof(GSP_SEQ_BUF_PAYLOAD_REG_WRITE), &pCmd[current_cmd_index], sizeof(GSP_SEQ_BUF_PAYLOAD_REG_WRITE));

                GPU_REG_WR32(pGpu, regWrite.addr, regWrite.val);
                break;
            }

            // 3 arguments
            case GSP_SEQ_BUF_OPCODE_REG_MODIFY:
            {
                GSP_SEQ_BUF_PAYLOAD_REG_MODIFY regModify;
                NvU32 regVal;

                portMemCopy(&regModify, sizeof(GSP_SEQ_BUF_PAYLOAD_REG_MODIFY), &pCmd[current_cmd_index], sizeof(GSP_SEQ_BUF_PAYLOAD_REG_MODIFY));

                regVal = GPU_REG_RD32(pGpu, regModify.addr);
                regVal = regVal & ~regModify.mask;
                regVal = regVal | regModify.val;
                GPU_REG_WR32(pGpu, regModify.addr, regVal);
                break;
            }

            // 5 arguments
            case GSP_SEQ_BUF_OPCODE_REG_POLL:
            {
                GSP_SEQ_BUF_PAYLOAD_REG_POLL regPoll;
                NvU32 regval;
                RMTIMEOUT timeout;

                portMemCopy(&regPoll, sizeof(GSP_SEQ_BUF_PAYLOAD_REG_POLL), &pCmd[current_cmd_index], sizeof(GSP_SEQ_BUF_PAYLOAD_REG_POLL));

                regval = GPU_REG_RD32(pGpu, regPoll.addr);

                gpuSetTimeout(pGpu, regPoll.timeout, &timeout, 0);
                while ((regval & regPoll.mask) != regPoll.val)
                {
                    nvStatus = gpuCheckTimeout(pGpu, &timeout);
                    if (nvStatus == NV_ERR_TIMEOUT)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for register to settle, value = 0x%x, err_code = 0x%x\n",
                            regval, regPoll.error);
                        DBG_BREAKPOINT();
                        return nvStatus;
                    }
                    osSpinLoop();
                    regval = GPU_REG_RD32(pGpu, regPoll.addr);
                }
                break;
            }

            case GSP_SEQ_BUF_OPCODE_DELAY_US:
            {
                GSP_SEQ_BUF_PAYLOAD_DELAY_US delayUs;
                portMemCopy(&delayUs, sizeof(GSP_SEQ_BUF_PAYLOAD_DELAY_US), &pCmd[current_cmd_index], sizeof(GSP_SEQ_BUF_PAYLOAD_DELAY_US));

                osDelayUs(delayUs.val);
                break;
            }

            case GSP_SEQ_BUF_OPCODE_REG_STORE:
            {
                GSP_SEQ_BUF_PAYLOAD_REG_STORE regStore;
                portMemCopy(&regStore, sizeof(GSP_SEQ_BUF_PAYLOAD_REG_STORE), &pCmd[current_cmd_index], sizeof(GSP_SEQ_BUF_PAYLOAD_REG_STORE));

                NV_ASSERT_OR_RETURN(regStore.index < GSP_SEQ_BUF_REG_SAVE_SIZE, NV_ERR_INVALID_ARGUMENT);

                pParams->regSaveArea[regStore.index] = GPU_REG_RD32(pGpu, regStore.addr);
                break;
            }

            case GSP_SEQ_BUF_OPCODE_CORE_RESET:
            {
                NV_ASSERT_OR_RETURN(payloadSize == 0, NV_ERR_INVALID_ARGUMENT);

                NV_ASSERT_OK_OR_RETURN(kflcnReset_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon)));
                kflcnDisableCtxReq_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
                break;
            }

            case GSP_SEQ_BUF_OPCODE_CORE_START:
            {
                NV_ASSERT_OR_RETURN(payloadSize == 0, NV_ERR_INVALID_ARGUMENT);

                kflcnStartCpu_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
                break;
            }

            case GSP_SEQ_BUF_OPCODE_CORE_WAIT_FOR_HALT:
            {
                NV_ASSERT_OR_RETURN(payloadSize == 0, NV_ERR_INVALID_ARGUMENT);

                NV_ASSERT_OK_OR_RETURN(kflcnWaitForHalt_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon), GPU_TIMEOUT_DEFAULT, 0));
                break;
            }

            default:
                //
                // Route this command to the arch-specific handler.
                //
                NV_ASSERT_OK_OR_RETURN(kgspExecuteSequencerCommand_HAL(pGpu, pKernelGsp, opCode, &pCmd[current_cmd_index], payloadSize * sizeof (*pCmd)));
                break;
        }
        current_cmd_index += payloadSize;
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
        return GSP_FW_HEAP_SIZE_VGPU_DEFAULT;

    //
    // The baremetal heap calculation is a function of the architecture, FB
    // size, and a chunk for backing client allocations (pre-calibrated for the
    // architecture through rough profiling).
    //
    pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
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

    //
    // Reclaimable binary data will end up padding the heap (in some cases,
    // significantly), but due to memory fragmentation we can't rely on it to
    // linearly reduce the amount needed in the primary heap, so it is not a
    // factor here. Instead, it's just extra margin to keep us from exhausting
    // the heap at runtime.
    //
    NvU64 heapSize = kgspGetFwHeapParamOsCarveoutSize_HAL(pGpu, pKernelGsp) +
                     pKernelGsp->fwHeapParamBaseSize +
                     NV_ALIGN_UP(GSP_FW_HEAP_PARAM_SIZE_PER_GB * memSizeGB, 1 << 20) +
                     NV_ALIGN_UP(GSP_FW_HEAP_PARAM_CLIENT_ALLOC_SIZE, 1 << 20);

    // Clamp to the minimum, even if the calculations say we can do with less
    const NvU32 minGspFwHeapSizeMB = kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
    heapSize = NV_MAX(heapSize, (NvU64)minGspFwHeapSizeMB << 20);

    // Clamp to the maximum heap size, if necessary
    heapSize = NV_MIN(heapSize, (NvU64)maxGspFwHeapSizeMB << 20);

    NV_PRINTF(LEVEL_INFO, "GSP FW heap %lluMB of %uGB\n",
              heapSize >> 20, memSizeGB);

    return heapSize;
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

NvU64 kgspGetWprEndMargin_IMPL(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    NvU64 wprEndMargin;
    GspFwWprMeta *pWprMeta = pKernelGsp->pWprMeta;

    wprEndMargin = ((NvU64)DRF_VAL(_REG, _RM_GSP_WPR_END_MARGIN, _MB, pKernelGsp->wprEndMarginOverride)) << 20;
    if (wprEndMargin == 0)
    {
        // Calculate the default margin size based on the WPR size
        const GspFwWprMeta *pWprMeta = pKernelGsp->pWprMeta;

        //
        // This needs to be called after pWprMeta->sizeOfRadix3Elf has been initialized,
        // in order to estimate the default WPR size.
        //
        NV_ASSERT(pWprMeta->sizeOfRadix3Elf > 0);

        //
        // If the bounds are encoded in GspFwWprMeta from a prior attempt, use them.
        // Otherwise, estimate the WPR size by the sizes of the elements in the layout.
        //
        if (pWprMeta->gspFwWprEnd > pWprMeta->nonWprHeapOffset)
        {
            // Turing...Ada - Path taken after WPR meta populated.
            wprEndMargin = pWprMeta->gspFwWprEnd - pWprMeta->nonWprHeapOffset;
        }
        else
        {
            //
            // Turing...Ada - Path taken before WPR meta populated.
            // Hopper+ path - ACR does not write computed bounds back into Kernel struct.
            //
            KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
            if (pKernelFsp != NULL)
                wprEndMargin += kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp);

            wprEndMargin += kpmuReservedMemorySizeGet(GPU_GET_KERNEL_PMU(pGpu));
            wprEndMargin += kgspGetFrtsSize_HAL(pGpu, pKernelGsp);
            wprEndMargin += pKernelGsp->gspRmBootUcodeSize;
            wprEndMargin += pWprMeta->sizeOfRadix3Elf;
            wprEndMargin += kgspGetFwHeapSize(pGpu, pKernelGsp, 0, 0);
            wprEndMargin += kgspGetNonWprHeapSize(pGpu, pKernelGsp);
        }

        if (pKernelGsp->bootAttempts > 0)
            wprEndMargin *= pKernelGsp->bootAttempts;
    }

    if (FLD_TEST_DRF(_REG, _RM_GSP_WPR_END_MARGIN, _APPLY, _ALWAYS, pKernelGsp->wprEndMarginOverride) ||
        (pKernelGsp->bootAttempts > 0))
    {
        NV_PRINTF(LEVEL_WARNING, "Adding margin of 0x%llx bytes after the end of WPR2\n",
                  wprEndMargin);
        pWprMeta->flags |= GSP_FW_FLAGS_RECOVERY_MARGIN_PRESENT;
        return wprEndMargin;
    }

    // Normal boot path
    pWprMeta->flags &= ~GSP_FW_FLAGS_RECOVERY_MARGIN_PRESENT;
    return 0;
}

static void
_kgspInitGpuProperties(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_MOBILE, pGSCI->bIsMobile);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED, pGSCI->bIsGc6Rtd3Allowed);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC8_SUPPORTED, pGSCI->bIsGc8Rtd3Allowed);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED, pGSCI->bIsGcOffRtd3Allowed);
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
* @param pKernelGsp                 Pointer to KernelGsp object
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