/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/fifo/kernel_channel.h"
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
#include "gpu/external_device/external_device.h"
#include "kernel/platform/platform_request_handler.h"
#include "class/cl2080.h" // NV20_SUBDEVICE_0
#include "liblogdecode.h"
#include "libelf.h"
#include "nverror.h"
#include "nvrm_registry.h"
#include "nv-firmware.h"
#include "nv-firmware-chip-family-select.h"
#include "nvtypes.h"
#include "nvVer.h"
#include "objrpc.h"
#include "objtmr.h"
#include "os/os.h"
#include "rmgspseq.h"
#include "sweng/dispsw.h"
#include "kernel/gpu/timed_sema.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/pmu/kern_pmu.h"
#include "gpu/perf/kern_perf.h"
#include "core/locks.h"

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

#define RPC_HDR  ((rpc_message_header_v*)(pRpc->message_buffer))

struct MIG_CI_UPDATE_CALLBACK_PARAMS
{
    NvU32 execPartCount;
    NvU32 execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NvU32 gfid;
    NvBool bDelete;
};

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
static NV_STATUS _kgspRpcDrainEvents(OBJGPU *, KernelGsp *, NvU32);
static void      _kgspRpcIncrementTimeoutCountAndRateLimitPrints(OBJGPU *, OBJRPC *);

static NV_STATUS _kgspAllocSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);
static void _kgspFreeSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);

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

static void
_kgspGetActiveRpcDebugData
(
    OBJRPC *pRpc,
    NvU32 function,
    NvU32 *data0,
    NvU32 *data1
)
{
    switch (function)
    {
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
        case NV_VGPU_MSG_EVENT_GSP_LOCKDOWN_NOTICE:
        {
            RPC_PARAMS(gsp_lockdown_notice, _v17_00);
            *data0 = rpc_params->bLockdownEngaging;
            *data1 = 0;
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
_kgspRpcSanityCheck(OBJGPU *pGpu)
{
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU in reset, skipping RPC\n");
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }
    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU lost, skipping RPC\n");
        return NV_ERR_GPU_IS_LOST;
    }
    if (osIsGpuShutdown(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU shutdown, skipping RPC\n");
        return NV_ERR_GPU_IS_LOST;
    }
    if (!gpuIsGpuFullPowerForPmResume(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU not full power, skipping RPC\n");
        return NV_ERR_GPU_NOT_FULL_POWER;
    }
    if (!gpuCheckSysmemAccess(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU has no sysmem access, skipping RPC\n");
        return NV_ERR_INVALID_ACCESS_TYPE;
    }
    return NV_OK;
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

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, _kgspRpcSanityCheck(pGpu));

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

    // Add RPC history entry
    {
        NvU32 func = RPC_HDR->function;
        NvU32 entry;

        entry = pRpc->rpcHistoryCurrent = (pRpc->rpcHistoryCurrent + 1) % RPC_HISTORY_DEPTH;

        portMemSet(&pRpc->rpcHistory[entry], 0, sizeof(pRpc->rpcHistory[0]));
        pRpc->rpcHistory[entry].function = func;

        _kgspGetActiveRpcDebugData(pRpc, func,
                                   &pRpc->rpcHistory[entry].data[0],
                                   &pRpc->rpcHistory[entry].data[1]);
    }

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
        nvStatus = osEventNotificationWithInfo(pGpu, pNotifyList, rpc_params->notifyIndex,
                       rpc_params->data, rpc_params->info16, rpc_params->eventData, rpc_params->eventDataSize);
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
    KernelChannel *pKernelChannel;
    KernelFifo    *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR      *pChidMgr;
    NvU32          status = NV_OK;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(rpc_params->nv2080EngineType);
    NvBool         bIsCcEnabled = NV_FALSE;

    // check if there's a PCI-E error pending either in device status or in AER
    krcCheckBusError_HAL(pGpu, pKernelRc);

    status = kfifoGetChidMgrFromType(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                     (NvU32)rmEngineType,
                                     &pChidMgr);
    if (status != NV_OK)
        return status;

    pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                  pChidMgr,
                                                  rpc_params->chid);
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pKernelChannel != NULL,
                       NV_ERR_INVALID_CHANNEL);

    bIsCcEnabled = gpuIsCCFeatureEnabled(pGpu);

    // With CC enabled, CPU-RM needs to write error notifiers
    if (bIsCcEnabled)
    {
        NV_ASSERT_OK_OR_RETURN(krcErrorSetNotifier(pGpu, pKernelRc,
                                                   pKernelChannel,
                                                   rpc_params->exceptType,
                                                   rmEngineType,
                                                   rpc_params->scope));
    }

    return krcErrorSendEventNotifications_HAL(pGpu, pKernelRc,
        pKernelChannel,
        rmEngineType,           // unused on kernel side
        rpc_params->exceptType,
        rpc_params->scope,
        rpc_params->partitionAttributionId);
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

    pKernelRc->pPreviousChannelInError = pKernelChannel;
    nvErrorLog_va(pGpu, rpc_params->exceptType, "%s", rpc_params->errString);
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
    RPC_PARAMS(gpuacct_perfmon_util_samples, _v17_00);

    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS_v17_00 *src = &rpc_params->params;
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

    for (i = 0; i < NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL; i++)
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

static void _kgspRpcMigCiConfigUpdateCallback
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
                                      OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW | OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS_RW);
    if (status != NV_OK)
    {
        portMemFree(pParams);
    }

    return status;
}

static NV_STATUS
_kgspRpcRgLineIntr
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(rg_line_intr, _v17_00);

    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelDisplay != NULL, NV_ERR_OBJECT_NOT_FOUND);

    kdispInvokeRgLineCallback(pKernelDisplay, rpc_params->head, rpc_params->rgIntr, NV_FALSE);

    return NV_OK;
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
            #define X(UNIT, a) #a,
            #define E(a) #a,
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
static NV_STATUS
_kgspProcessRpcEvent
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    rpc_message_header_v *pMsgHdr = RPC_HDR;
    NV_STATUS nvStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "received event from GPU%d: 0x%x (%s) status: 0x%x size: %d\n",
              gpuGetInstance(pGpu), pMsgHdr->function, _getRpcName(pMsgHdr->function), pMsgHdr->rpc_result, pMsgHdr->length);

    switch(pMsgHdr->function)
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

        case NV_VGPU_MSG_EVENT_NVLINK_IS_GPU_DEGRADED :
            _kgspRpcEventIsGpuDegradedCallback(pGpu, pRpc);
            break;

        case NV_VGPU_MSG_EVENT_RG_LINE_INTR:
            _kgspRpcRgLineIntr(pGpu, pRpc);
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

        case NV_VGPU_MSG_EVENT_GSP_INIT_DONE:   // Handled by _kgspRpcRecvPoll.
        default:
            //
            // We will get here if the previous RPC timed out.  The response
            // eventually comes in as an unexpected event.  The error handling
            // for the timeout has already happened, and returning an error here
            // causes subsequent messages to fail.  So return NV_OK.
            //
            NV_PRINTF(LEVEL_ERROR, "Unexpected RPC event from GPU%d: 0x%x (%s)\n",
                      gpuGetInstance(pGpu), pMsgHdr->function, _getRpcName(pMsgHdr->function));
            break;
    }

    return nvStatus;
}

/*!
 * Handle a single RPC event from GSP unless the event is [an RPC return for] expectedFunc,
 * or there are no events available in the buffer.
 *
 * @return
 *   NV_OK                              if the event is successfully handled.
 *   NV_WARN_NOTHING_TO_DO              if there are no events available.
 *   NV_WARN_MORE_PROCESSING_REQUIRED   if the event is expectedFunc: it is unhandled and in the staging area.
 *   (Another status)                   if event reading or processing fails.
 */
static NV_STATUS
_kgspRpcDrainOneEvent
(
    OBJGPU          *pGpu,
    OBJRPC          *pRpc,
    NvU32            expectedFunc
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

        nvStatus = _kgspProcessRpcEvent(pGpu, pRpc);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to process received event 0x%x (%s) from GPU%d: status=0x%x\n",
                      pMsgHdr->function, _getRpcName(pMsgHdr->function), gpuGetInstance(pGpu), nvStatus);
        }
    }

    //
    // We don't expect the NV_WARN_MORE_PROCESSING_REQUIRED from either called function.
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
    NvU32      expectedFunc
)
{
    NV_STATUS nvStatus = NV_OK;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    while (nvStatus == NV_OK)
    {
        nvStatus = _kgspRpcDrainOneEvent(pGpu, pRpc, expectedFunc);
        kgspDumpGspLogs(pKernelGsp, NV_FALSE);
    }

    // If GSP-RM has died, 
    if (!kgspHealthCheck_HAL(pGpu, pKernelGsp))
        return NV_ERR_RESET_REQUIRED;

    if (nvStatus == NV_WARN_NOTHING_TO_DO)
        nvStatus = NV_OK;

    return nvStatus;
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
    NvU32 historyEntry = pRpc->rpcHistoryCurrent;

    if (pRpc->timeoutCount == 1)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "********************************* GSP Failure **********************************\n");
    }

    NV_ASSERT(expectedFunc == pRpc->rpcHistory[historyEntry].function);

    nvErrorLog_va((void*)pGpu, GSP_RPC_TIMEOUT,
                  "Timeout waiting for RPC from GSP%d! Expected function %d (%s) (0x%x 0x%x).",
                  gpuGetInstance(pGpu),
                  expectedFunc,
                  _getRpcName(expectedFunc),
                  pRpc->rpcHistory[historyEntry].data[0],
                  pRpc->rpcHistory[historyEntry].data[1]);
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, LEVEL_ERROR, NV_PRINTF_ADD_PREFIX
                 ("Timeout waiting for RPC from GSP%d! Expected function %d (0x%x 0x%x)"),
                 gpuGetInstance(pGpu),
                 expectedFunc,
                 pRpc->rpcHistory[historyEntry].data[0],
                 pRpc->rpcHistory[historyEntry].data[1]);

    if (pRpc->timeoutCount == 1)
    {
        NvU32 activeData[2];
        NvU32 historyIndex;
        rpc_message_header_v *pMsgHdr = RPC_HDR;

        _kgspGetActiveRpcDebugData(pRpc, pMsgHdr->function,
                                   &activeData[0], &activeData[1]);

        if ((expectedFunc != pMsgHdr->function) ||
            (pRpc->rpcHistory[historyEntry].data[0] != activeData[0]) ||
            (pRpc->rpcHistory[historyEntry].data[1] != activeData[1]))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Current RPC function %d (%s) or data (0x%x 0x%x) does not match expected function %d (%s) or data (0x%x 0x%x).\n",
                      pMsgHdr->function, _getRpcName(pMsgHdr->function),
                      activeData[0], activeData[1],
                      expectedFunc, _getRpcName(expectedFunc),
                      pRpc->rpcHistory[historyEntry].data[0],
                      pRpc->rpcHistory[historyEntry].data[1]);
        }

        NV_PRINTF(LEVEL_ERROR, "RPC history (CPU -> GSP%d):\n", gpuGetInstance(pGpu));
        NV_PRINTF(LEVEL_ERROR, "\tentry\tfunc\t\t\t\tdata\n");
        for (historyIndex = 0; historyIndex < RPC_HISTORY_DEPTH; historyIndex++)
        {
            historyEntry = (pRpc->rpcHistoryCurrent + RPC_HISTORY_DEPTH - historyIndex) % RPC_HISTORY_DEPTH;
            NV_PRINTF(LEVEL_ERROR, "\t%c%-2d\t%2d %-22s\t0x%08x 0x%08x\n",
                      ((historyIndex == 0) ? ' ' : '-'),
                      historyIndex,
                      pRpc->rpcHistory[historyEntry].function,
                      _getRpcName(pRpc->rpcHistory[historyEntry].function),
                      pRpc->rpcHistory[historyEntry].data[0],
                      pRpc->rpcHistory[historyEntry].data[1]);
        }

        osAssertFailed();

        NV_PRINTF(LEVEL_ERROR,
                  "********************************************************************************\n");
    }
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

    if (pKernelGsp->bFatalError)
    {
        // in case of a fatal GSP error, don't bother printing RPC errors at all
        pRpc->bQuietPrints = NV_TRUE;
        return;
    }

    if ((pRpc->timeoutCount == (RPC_TIMEOUT_LIMIT_PRINT_RATE_THRESH + 1)) &&
        (RPC_TIMEOUT_LIMIT_PRINT_RATE_SKIP > 0))
    {
        // make sure we warn Xid and NV_PRINTF/NVLOG consumers that we are rate limiting prints
        if (GPU_GET_KERNEL_RC(pGpu)->bLogEvents)
        {
            portDbgPrintf(
                "NVRM: Rate limiting GSP RPC error prints for GPU at PCI:%04x:%02x:%02x (printing 1 of every %d).  The GPU likely needs to be reset.\n",
                gpuGetDomain(pGpu),
                gpuGetBus(pGpu),
                gpuGetDevice(pGpu),
                RPC_TIMEOUT_LIMIT_PRINT_RATE_SKIP + 1);
        }
        NV_PRINTF(LEVEL_WARNING,
                  "Rate limiting GSP RPC error prints (printing 1 of every %d)\n",
                  RPC_TIMEOUT_LIMIT_PRINT_RATE_SKIP + 1);
    }

    pRpc->bQuietPrints = ((pRpc->timeoutCount > RPC_TIMEOUT_LIMIT_PRINT_RATE_THRESH) &&
                          ((pRpc->timeoutCount % (RPC_TIMEOUT_LIMIT_PRINT_RATE_SKIP + 1)) != 0));
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
            // We should only ever timeout this when GSP is in really bad state, so if it just
            // happens to timeout on default timeout it should be OK for us to give it a little
            // more time - make this timeout 1.5 of the default to allow some leeway.
            timeoutUs = defaultus + defaultus / 2;
        }
    }

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

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

        rpcStatus = _kgspRpcDrainEvents(pGpu, pKernelGsp, expectedFunc);

        switch (rpcStatus) {
            case NV_WARN_MORE_PROCESSING_REQUIRED:
                rpcStatus = NV_OK;
                goto done;
            case NV_OK:
                // Check timeout and continue outer loop.
                break;
            default:
                goto done;
        }

        NV_CHECK_OK_OR_GOTO(rpcStatus, LEVEL_SILENT, _kgspRpcSanityCheck(pGpu), done);

        if (timeoutStatus == NV_ERR_TIMEOUT)
        {
            rpcStatus = timeoutStatus;

            _kgspRpcIncrementTimeoutCountAndRateLimitPrints(pGpu, pRpc);

            if (!pRpc->bQuietPrints)
            {
                _kgspLogXid119(pGpu, pRpc, expectedFunc);
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

    // Init task_isr RPC object
    if (pKernelGsp->bIsTaskIsrQueueRequired)
    {
        nvStatus = _kgspConstructRpcObject(pGpu, pKernelGsp,
                                           &pMQCollection->rpcQueues[RPC_TASK_ISR_QUEUE_IDX],
                                           &pKernelGsp->pLocklessRpc);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "init task ISR RPC infrastructure failed\n");
            goto done;
        }
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
    pRpc->rpcHistoryCurrent   = RPC_HISTORY_DEPTH - 1;
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
    if (pKernelGsp->pLocklessRpc != NULL)
    {
        rpcDestroy(pGpu, pKernelGsp->pLocklessRpc);
        portMemFree(pKernelGsp->pLocklessRpc);
        pKernelGsp->pLocklessRpc = NULL;
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
        pKernelGsp->gspPluginVgpuTaskLogMem
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

        _kgspFreeLibosVgpuPartitionLoggingStructures(pGpu, pKernelGsp, gfid);
        return NV_OK;
    }
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
    NvU64 vgpuTaskLogBuffSize
)
{
    struct
    {
        const char       *szMemoryId;
        const char       *szPrefix;
        const char       *elfSectionName;
        NvU64             bufOffset;
        NvU64             bufSize;
        RM_LIBOS_LOG_MEM *taskLogArr;
    } logInitValues[] =
    {
        {"LOGINIT", "INIT", ".fwlogging_init", initTaskLogBUffOffset, initTaskLogBUffSize, pKernelGsp->gspPluginInitTaskLogMem},
        {"LOGVGPU", "VGPU", ".fwlogging_vgpu", vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, pKernelGsp->gspPluginVgpuTaskLogMem}
    };
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);

    NV_STATUS nvStatus = NV_OK;
    RM_LIBOS_LOG_MEM *pTaskLog = NULL;
    char vm_string[8], sourceName[SOURCE_NAME_MAX_LENGTH];

    if (gfid > MAX_PARTITIONS_WITH_GFID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncMutexAcquire(pKernelGsp->pNvlogFlushMtx);

    // Source name is used to generate a tag that is a unique identifier for nvlog buffers.
    // As the source name 'GSP' is already in use, we will need a custom source name.
    nvDbgSnprintf(sourceName, SOURCE_NAME_MAX_LENGTH, "V%02d", gfid);
    libosLogCreateEx(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], sourceName);

    // Setup logging for each task in vgpu partition
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(logInitValues); ++i)
    {
        pTaskLog = &logInitValues[i].taskLogArr[gfid - 1];
        NvP64 pVa = NvP64_NULL;

        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescCreate(&pTaskLog->pTaskLogDescriptor,
                          pGpu,
                          logInitValues[i].bufSize,
                          RM_PAGE_SIZE,
                          NV_TRUE, ADDR_FBMEM, NV_MEMORY_CACHED,
                          MEMDESC_FLAGS_NONE),
            error_cleanup);

        memdescDescribe(pTaskLog->pTaskLogDescriptor, ADDR_FBMEM, logInitValues[i].bufOffset,  logInitValues[i].bufSize);

        pVa = memdescMapInternal(pGpu, pTaskLog->pTaskLogDescriptor, TRANSFER_FLAGS_NONE);
        if (pVa != NvP64_NULL)
        {
            pTaskLog->pTaskLogBuffer = pVa;
            portMemSet(pTaskLog->pTaskLogBuffer, 0, logInitValues[i].bufSize);

            pTaskLog->id8 = _kgspGenerateInitArgId(logInitValues[i].szMemoryId);

            nvDbgSnprintf(vm_string, sizeof(vm_string), "%s%d", logInitValues[i].szPrefix, gfid);

            libosLogAddLogEx(&pKernelGsp->logDecodeVgpuPartition[gfid - 1],
                pTaskLog->pTaskLogBuffer,
                memdescGetSize(pTaskLog->pTaskLogDescriptor),
                pGpu->gpuInstance,
                (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
                gpuGetChipImpl(pGpu),
                vm_string,
                logInitValues[i].elfSectionName);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to map memory for %s task log buffer for vGPU partition \n", logInitValues[i].szPrefix);
            nvStatus = NV_ERR_INSUFFICIENT_RESOURCES;
            goto error_cleanup;
        }
    }

    {
        libosLogInit(&pKernelGsp->logDecodeVgpuPartition[gfid - 1], pKernelGsp->pLogElf, pKernelGsp->logElfDataSize);
        // nvlog buffers are now setup using the appropriate sourceName to avoid tag-value clash.
        // Now sourceName can be modified to preserve the 'GSP-VGPUx' logging convention.
        portStringCopy(pKernelGsp->logDecodeVgpuPartition[gfid - 1].sourceName,
                       SOURCE_NAME_MAX_LENGTH,
                       "GSP", SOURCE_NAME_MAX_LENGTH);
    }

    pKernelGsp->bHasVgpuLogs = NV_TRUE;

error_cleanup:
    portSyncMutexRelease(pKernelGsp->pNvlogFlushMtx);

    if (nvStatus != NV_OK)
        _kgspFreeLibosVgpuPartitionLoggingStructures(pGpu, pKernelGsp, gfid);

    return nvStatus;
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

    if (pKernelGsp->pNvlogFlushMtx != NULL)
    {
        nvlogDeregisterFlushCb(kgspNvlogFlushCb, pKernelGsp);
        portSyncMutexDestroy(pKernelGsp->pNvlogFlushMtx);

        pKernelGsp->pNvlogFlushMtx = NULL;
    }

    libosLogDestroy(&pKernelGsp->logDecode);

    for (idx = 0; idx < LOGIDX_SIZE; idx++)
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
        const char *szMemoryId;
        const char *szPrefix;
        NvU32       size;
        const char *elfSectionName;
    } logInitValues[] =
    {
        {"LOGINIT", "INIT", 0x10000, ".fwlogging_init"},  // 64KB for stack traces
#if defined(DEVELOP) || defined(DEBUG)
        // The interrupt task is in the rm elf, so they share the same logging elf too
        {"LOGINTR", "INTR", 0x40000, ".fwlogging_rm"},    // 256KB ISR debug log on develop/debug builds
        {"LOGRM",   "RM",   0x40000, ".fwlogging_rm"}     // 256KB RM debug log on develop/debug builds
#else
        // The interrupt task is in the rm elf, so they share the same logging elf too
        {"LOGINTR", "INTR", 0x10000, ".fwlogging_rm"},    // 64KB ISR debug log on develop/debug builds
        {"LOGRM",   "RM",   0x10000, ".fwlogging_rm"}     // 64KB RM debug log on release builds
#endif
    };
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) == LOGIDX_SIZE);

    NV_STATUS nvStatus = NV_OK;
    NvU8      idx;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    pKernelGsp->pNvlogFlushMtx = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pKernelGsp->pNvlogFlushMtx == NULL)
    {
        nvStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        goto error_cleanup;
    }

    libosLogCreate(&pKernelGsp->logDecode);

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    for (idx = 0; idx < LOGIDX_SIZE; idx++)
    {
        RM_LIBOS_LOG_MEM *pLog = &pKernelGsp->rmLibosLogMem[idx];
        NvP64 pVa = NvP64_NULL;
        NvP64 pPriv = NvP64_NULL;

        // Setup logging memory for each task.
        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescCreate(&pLog->pTaskLogDescriptor,
                          pGpu,
                          logInitValues[idx].size,
                          RM_PAGE_SIZE,
                          NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                          flags),
            error_cleanup);

        memdescTagAlloc(nvStatus, 
                      NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_12, pLog->pTaskLogDescriptor);
        NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
            error_cleanup);

        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescMap(pLog->pTaskLogDescriptor, 0,
                       memdescGetSize(pLog->pTaskLogDescriptor),
                       NV_TRUE, NV_PROTECT_READ_WRITE,
                       &pVa, &pPriv),
            error_cleanup);

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

        pLog->id8 = _kgspGenerateInitArgId(logInitValues[idx].szMemoryId);

        libosLogAddLogEx(&pKernelGsp->logDecode,
            pLog->pTaskLogBuffer,
            memdescGetSize(pLog->pTaskLogDescriptor),
            pGpu->gpuInstance,
            (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
            gpuGetChipImpl(pGpu),
            logInitValues[idx].szPrefix,
            logInitValues[idx].elfSectionName);
    }

error_cleanup:
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

    kgspConfigureFalcon_HAL(pGpu, pKernelGsp);

    // Init RPC objects used to communicate with GSP.
    nvStatus = _kgspInitRpcInfrastructure(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init RPC infrastructure failed\n");
        goto done;
    }

    // Init logging memory used by GSP
    nvStatus = _kgspInitLibosLoggingStructures(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init libos logging structures failed: 0x%x\n", nvStatus);
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

done:
    if (nvStatus != NV_OK)
    {
        kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
        _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);
        _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    }

    return nvStatus;
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

    // Set the GPU time to the wall-clock time before loading GSP ucode.
    tmrSetCurrentTime_HAL(pGpu, pTmr);

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
            //
            // Extracting VBIOS image from ROM is not supported.
            // Sanity check we don't depend on it for FRTS, and proceed without FWSEC.
            //
            NV_ASSERT_OR_GOTO(kgspGetFrtsSize(pGpu, pKernelGsp) == 0, done);
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

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _kgspInitLibosLogDecoder(pGpu, pKernelGsp, pGspFw), done);

    // If live decoding is enabled, do not register flush callback to avoid racing with ioctl
    if (pKernelGsp->pLogElf == NULL)
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, nvlogRegisterFlushCb(kgspNvlogFlushCb, pKernelGsp), done);

    // Reset thread state timeout and wait for GFW_BOOT OK status
    threadStateResetTimeout(pGpu);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp), done);

    // Fail early if WPR2 is up
    if (kgspIsWpr2Up_HAL(pGpu, pKernelGsp))
    {
        NV_PRINTF(LEVEL_ERROR, "unexpected WPR2 already up, cannot proceed with booting gsp\n");
        NV_PRINTF(LEVEL_ERROR, "(the GPU is likely in a bad state and may need to be reset)\n");
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    // Calculate FB layout (requires knowing FB size which depends on GFW_BOOT)
    status = kgspCalculateFbLayout_HAL(pGpu, pKernelGsp, pGspFw);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error calculating FB layout\n");
        goto done;
    }

    // Prepare Scrubber ucode image if pre-scrubbed memory is insufficient
    if (pKernelGsp->pScrubberUcode == NULL)
    {
        NvU64 neededSize = pKernelGsp->pWprMeta->fbSize - pKernelGsp->pWprMeta->gspFwRsvdStart;
        NvU64 prescrubbedSize = kgspGetPrescrubbedTopFbSize(pGpu, pKernelGsp);

        if (neededSize > prescrubbedSize)
        {
            NV_PRINTF(LEVEL_INFO,
                      "allocating Scrubber ucode as pre-scrubbed memory (0x%llx bytes) is insufficient (0x%llx bytes needed)\n",
                      prescrubbedSize, neededSize);

            status = kgspAllocateScrubberUcodeImage(pGpu, pKernelGsp,
                                                    &pKernelGsp->pScrubberUcode);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to allocate Scrubber ucode: 0x%x\n", status);
                goto done;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "skipping allocating Scrubber ucode as pre-scrubbed memory (0x%llx bytes) is sufficient (0x%llx bytes needed)\n",
                      prescrubbedSize, neededSize);
        }
    }

    // bring up ucode with RM offload task
    status = kgspBootstrapRiscvOSEarly_HAL(pGpu, pKernelGsp, pGspFw);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "cannot bootstrap riscv/gsp: 0x%x\n", status);

        //
        // Ignore return value - a crash report may have already been consumed,
        // this is just here as a last attempt to report boot issues that might
        // escaped prior checks.
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

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspStartLogPolling(pGpu, pKernelGsp), done);

done:
    pKernelGsp->bInInit = NV_FALSE;

    if (status != NV_OK)
    {
        KernelPmu *pKernelPmu = GPU_GET_KERNEL_PMU(pGpu);

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
    KernelGsp *pKernelGsp
)
{
    NV_STATUS rpcStatus = NV_OK;
    NV_STATUS status;

    NV_PRINTF(LEVEL_INFO, "unloading GSP-RM\n");
    NV_RM_RPC_UNLOADING_GUEST_DRIVER(pGpu, rpcStatus, NV_FALSE, NV_FALSE, 0);

    // Wait for GSP-RM processor to suspend
    kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp);

    // Dump GSP-RM logs and reset before invoking FWSEC-SB
    kgspDumpGspLogs(pKernelGsp, NV_FALSE);

    //
    // Avoid cascading timeouts when attempting to invoke the below ucodes if
    // we are unloading due to a GSP-RM timeout.
    //
    threadStateResetTimeout(pGpu);

    // Because of COT, RM cannot reset GSP-RISCV and FSP has exclusive access to reset and reboot GSP for next run.
    if(!(pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_COT_ENABLED)))
    {
        kflcnReset_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
    }

    // Invoke FWSEC-SB to put back PreOsApps during driver unload
    status = kgspExecuteFwsecSb_HAL(pGpu, pKernelGsp, pKernelGsp->pFwsecUcode);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC-SB for PreOsApps during driver unload: 0x%x\n", status);
        NV_ASSERT(0);
    }

    if (pKernelGsp->bPartitionedFmc)
    {
        //
        // GSP-RM invokes the partitioned FMC to unload directly as part of the
        // NV_RM_RPC_UNLOADING_GUEST_DRIVER call above.
        //
        status = rpcStatus;
    }
    else
    {
        // After instructing GSP-RM to unload itself, run Booter Unload to teardown WPR2
        status = kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp, 0);
    }

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

    kgspFreeSuspendResumeData_HAL(pGpu, pKernelGsp);
}

void
kgspDumpGspLogsUnlocked_IMPL
(
    KernelGsp *pKernelGsp,
    NvBool bSyncNvLog
)
{
    if (pKernelGsp->bInInit || pKernelGsp->pLogElf || bSyncNvLog)
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
    if (pKernelGsp->bInInit || pKernelGsp->pLogElf || bSyncNvLog)
    {
        if (pKernelGsp->pNvlogFlushMtx != NULL)
            portSyncMutexAcquire(pKernelGsp->pNvlogFlushMtx);

        kgspDumpGspLogsUnlocked(pKernelGsp, bSyncNvLog);

        if (pKernelGsp->pNvlogFlushMtx != NULL)
            portSyncMutexRelease(pKernelGsp->pNvlogFlushMtx);
    }
}

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
    if (pKernelGsp->bIsTaskIsrQueueRequired)
    {
        pMQInitArgs->locklessCmdQueueOffset  = pMQInitArgs->statQueueOffset        + pMQCollection->rpcQueues[RPC_TASK_RM_QUEUE_IDX].statusQueueSize;
        pMQInitArgs->locklessStatQueueOffset = pMQInitArgs->locklessCmdQueueOffset + pMQCollection->rpcQueues[RPC_TASK_ISR_QUEUE_IDX].commandQueueSize;
    }
    else
    {
        pMQInitArgs->locklessCmdQueueOffset  = 0;
        pMQInitArgs->locklessStatQueueOffset = 0;
    }

    if (pGspInitArgs == NULL)
    {
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
    NvU8 *pDesc = NULL;
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

    // copy the image descriptor
    bufSize = bindataGetBufferSize(pBinStorageDesc);
    pDesc = portMemAllocNonPaged(bufSize);
    if (pDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate ucode desc buffer\n");
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    pKernelGsp->pGspRmBootUcodeDesc = (RM_RISCV_UCODE_DESC*)pDesc;

    NV_ASSERT_OK_OR_GOTO(status,
                         bindataWriteToBuffer(pBinStorageDesc, pDesc, bufSize),
                         fail);

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
    portMemFree(pKernelGsp->pGspRmBootUcodeDesc);
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
    NvU64 expectedVersionLength = portStringLength(NV_VERSION_STRING);

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

    // Check that text in .fwversion section of ELF matches our NV_VERSION_STRING
    if ((fwversionSize != expectedVersionLength + 1) ||
        (portStringCompare(pFwversion, NV_VERSION_STRING, expectedVersionLength) != 0))
    {
        // Sanity check .fwversion before attempting to print it in the error message
        if ((fwversionSize > 0) &&
            (fwversionSize < 64) &&
            (pFwversion[fwversionSize - 1] == '\0'))
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
    for (idx = 0; idx < LOGIDX_SIZE; idx++)
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
    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));
    //
    // We should never have an event with code NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS.
    // If we do the assert will fail on NV_WARN_MORE_PROCESSING_REQUIRED,
    // in addition to general error codes.
    //
    NV_ASSERT_OK(_kgspRpcDrainEvents(pGpu, pKernelGsp, NV_VGPU_MSG_FUNCTION_NUM_FUNCTIONS));
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

                kflcnReset_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
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
    //
    // Do not take any locks in kgspDumpGspLogs. As this callback only fires when kgspNvlogFlushCb
    // is not registered, there is no possibility of data race.
    //
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    kgspDumpGspLogsUnlocked(pKernelGsp, NV_FALSE);
}

NV_STATUS
kgspStartLogPolling_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status = NV_OK;

    //
    // Only enable the 1 Hz poll if we can live decode logs in dmesg. Else we'll flush it on demand
    // by nvidia-debugdump.
    //
    if (pKernelGsp->pLogElf != NULL)
        status = osSchedule1SecondCallback(pGpu, _kgspLogPollingCallback, NULL, NV_OS_1HZ_REPEAT);
    return status;
}

static void
_kgspStopLogPolling
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    if (pKernelGsp->pLogElf != NULL)
        osRemove1SecondRepeatingCallback(pGpu, _kgspLogPollingCallback, NULL);
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
    // For VGPU, use the static pre-calculated size
    if (pGpu->bVgpuGspPluginOffloadEnabled)
        return GSP_FW_HEAP_SIZE_VGPU_DEFAULT;

    //
    // The baremetal heap calculation is a function of the architecture, FB
    // size, and a chunk for backing client allocations (pre-calibrated for the
    // architecture through rough profiling).
    //
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU64 fbSize = 0;

    NV_ASSERT_OK(kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, &fbSize));
    const NvU32 fbSizeGB = (NvU32)(NV_ALIGN_UP64(fbSize, 1 << 30) >> 30);

    //
    // Reclaimable binary data will end up padding the heap (in some cases,
    // significantly), but due to memory fragmentation we can't rely on it to
    // linearly reduce the amount needed in the primary heap, so it is not a
    // factor here. Instead, it's just extra margin to keep us from exhausting
    // the heap at runtime.
    //
    NvU64 heapSize = kgspGetFwHeapParamOsCarveoutSize_HAL(pGpu, pKernelGsp) +
                     pKernelGsp->fwHeapParamBaseSize +
                     NV_ALIGN_UP(GSP_FW_HEAP_PARAM_SIZE_PER_GB_FB * fbSizeGB, 1 << 20) +
                     NV_ALIGN_UP(GSP_FW_HEAP_PARAM_CLIENT_ALLOC_SIZE, 1 << 20);

    // Clamp to the minimum, even if the calculations say we can do with less
    const NvU32 minGspFwHeapSizeMB = kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
    heapSize = NV_MAX(heapSize, (NvU64)minGspFwHeapSizeMB << 20);

    // Clamp to the maximum heap size, if necessary
    heapSize = NV_MIN(heapSize, (NvU64)maxGspFwHeapSizeMB << 20);

    NV_PRINTF(LEVEL_INFO, "GSP FW heap %lluMB of %uGB\n",
              heapSize >> 20, fbSizeGB);

    return heapSize;
}

/*!
 * Returns the size in bytes of the GSP FW heap:
 *  - the registry override, if present
 *  - otherwise, calculate the FW heap size for this GPU, limiting it to stay
 *    within the pre-scrubbed area at the end of FB, if needed
 *
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
    NvU64 posteriorFbSize
)
{
    NvU32 maxScrubbedHeapSizeMB = NV_U32_MAX;
    NvU32 heapSizeMB = 0;

    //
    // The pre-scrubbed region at the end of FB may limit the heap size, if no
    // scrubber ucode is supported to unlock the rest of memory prior to booting
    // GSP-RM.
    //
    if (!pKernelGsp->bScrubberUcodeSupported && (posteriorFbSize != 0))
    {
        const NvU64 prescrubbedSize = kgspGetPrescrubbedTopFbSize(pGpu, pKernelGsp);
        if (prescrubbedSize < NV_U64_MAX)
            maxScrubbedHeapSizeMB = (NvU32)((prescrubbedSize - posteriorFbSize) >> 20);
    }

    // Get the heap size override from the registry, if any
    if ((osReadRegistryDword(pGpu, NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB, &heapSizeMB) == NV_OK) &&
        (heapSizeMB != NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB_DEFAULT))
    {
        const NvU32 minGspFwHeapSizeMB = kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp);
        const NvU32 maxGspFwHeapSizeMB = NV_MIN(kgspGetMaxWprHeapSizeMB_HAL(pGpu, pKernelGsp),
                                                maxScrubbedHeapSizeMB);

        NV_ASSERT(minGspFwHeapSizeMB < maxGspFwHeapSizeMB);

        if (heapSizeMB > maxGspFwHeapSizeMB)
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size clamped to maximum (%uMB)\n",
                      maxGspFwHeapSizeMB);
            heapSizeMB = maxGspFwHeapSizeMB;
        }
        else if (heapSizeMB < minGspFwHeapSizeMB)
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size clamped to minimum (%uMB)\n",
                      minGspFwHeapSizeMB);
            heapSizeMB = minGspFwHeapSizeMB;
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING, "Firmware heap size overridden (%uMB)\n",
                      heapSizeMB);
        }

        return ((NvU64)heapSizeMB) << 20;
    }

    return _kgspCalculateFwHeapSize(pGpu, pKernelGsp, maxScrubbedHeapSizeMB);
}
