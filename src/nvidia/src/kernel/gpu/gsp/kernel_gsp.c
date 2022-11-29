/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gsp/kernel_gsp.h"

#include "kernel/core/thread_state.h"
#include "kernel/core/locks.h"
#include "kernel/diagnostics/gpu_acct.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "virtualization/vgpuconfigapi.h"
#include "kernel/gpu/disp/kern_disp.h"
#include "gpu/external_device/external_device.h"

#include "class/cl2080.h" // NV20_SUBDEVICE_0

#include "liblogdecode.h"
#include "libelf.h"
#include "nverror.h"
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

#define RPC_HDR  ((rpc_message_header_v*)(pRpc->message_buffer))

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

static NV_STATUS _kgspRpcSendMessage(OBJGPU *, OBJRPC *);
static NV_STATUS _kgspRpcRecvPoll(OBJGPU *, OBJRPC *, NvU32);
static NV_STATUS _kgspRpcDrainEvents(OBJGPU *, KernelGsp *, NvU32);

static NV_STATUS _kgspAllocSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);
static void _kgspFreeSimAccessBuffer(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static void _kgspStopLogPolling(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static void _kgspFreeBootBinaryImage(OBJGPU *pGpu, KernelGsp *pKernelGsp);

static NV_STATUS _kgspPrepareGspRmBinaryImage(OBJGPU *pGpu, KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static NV_STATUS _kgspCreateRadix3(OBJGPU *pGpu, MEMORY_DESCRIPTOR **ppMemdescRadix3,
                                   MEMORY_DESCRIPTOR *pMemdescData, const void *pData, NvU64 size);

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
            rpc_gsp_rm_control_v03_00 *rpc_params = &rpc_message->gsp_rm_control_v03_00;
            *data0 = rpc_params->cmd;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC:
        {
            rpc_gsp_rm_alloc_v03_00 *rpc_params = &rpc_message->gsp_rm_alloc_v03_00;
            *data0 = rpc_params->hClass;
            *data1 = rpc_params->paramsSize;
            break;
        }
        case NV_VGPU_MSG_FUNCTION_FREE:
        {
            rpc_free_v03_00 *rpc_params = &rpc_message->free_v03_00;
            *data0 = rpc_params->params.hObjectOld;
            *data1 = rpc_params->params.hObjectParent;
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

    NV_ASSERT_OR_RETURN(!osIsGpuShutdown(pGpu), NV_ERR_GPU_IS_LOST);

    nvStatus = GspMsgQueueSendCommand(pRpc->pMessageQueueInfo, pGpu);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GspMsgQueueSendCommand failed: 0x%x\n", nvStatus);
        return nvStatus;
    }

    // GSPRM TODO: Use this call to pass the actual index.
    kgspSetCmdQueueHead_HAL(pGpu, pKernelGsp, 0, 0);

    // Add RPC history entry
    {
        NvU32 func = vgpu_rpc_message_header_v->function;
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
        nvStatus = osEventNotification(pGpu, pNotifyList, rpc_params->notifyIndex,
            rpc_params->eventData, rpc_params->eventDataSize);
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
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJOS *pOS = SYS_GET_OS(pSys);

        NV_ASSERT_OR_RETURN(rpc_params->count <= sizeof(pKernelGsp->pSimAccessBuf->data), NV_ERR_BUFFER_TOO_SMALL);

        for (i = rpc_params->index; i < count; i++)
        {
            NvU32 data;
            pOS->osSimEscapeRead(pGpu, rpc_params->path, i, 4, &data);
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
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJOS *pOS = SYS_GET_OS(pSys);

        pOS->osSimEscapeWrite(pGpu, rpc_params->path, rpc_params->index, rpc_params->count, rpc_params->data);
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

    return dispswReleaseSemaphoreAndNotifierFill(pGpu,
                                                 rpc_params->GPUVA,
                                                 rpc_params->hVASpace,
                                                 rpc_params->ReleaseValue,
                                                 rpc_params->Flags,
                                                 rpc_params->completionStatus,
                                                 rpc_params->hClient,
                                                 rpc_params->hEvent);
}

static NV_STATUS
_kgspRpcTimedSemaphoreRelease(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    RPC_PARAMS(timed_semaphore_release, _v01_00);

    return tsemaRelease_HAL(pGpu,
                            rpc_params->semaphoreVA,
                            rpc_params->notifierVA,
                            rpc_params->hVASpace,
                            rpc_params->releaseValue,
                            rpc_params->completionStatus,
                            rpc_params->hClient);
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
    return NV_OK;
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

    NV_PRINTF(LEVEL_INFO, "received event: 0x%x (%s) status: 0x%x size: %d\n",
              pMsgHdr->function, _getRpcName(pMsgHdr->function), pMsgHdr->rpc_result, pMsgHdr->length);

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

        case NV_VGPU_MSG_EVENT_GSP_INIT_DONE:   // Handled by _kgspRpcRecvPoll.
        default:
            //
            // We will get here if the previous RPC timed out.  The response
            // eventually comes in as an unexpected event.  The error handling
            // for the timeout has already happened, and returning an error here
            // causes subsequent messages to fail.  So return NV_OK.
            //
            NV_PRINTF(LEVEL_ERROR, "Unexpected RPC event 0x%x (%s)\n",
                      pMsgHdr->function, _getRpcName(pMsgHdr->function));
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

    nvStatus = GspMsgQueueReceiveStatus(pRpc->pMessageQueueInfo);

    if (nvStatus == NV_OK)
    {
        rpc_message_header_v *pMsgHdr = RPC_HDR;
        if (pMsgHdr->function == expectedFunc)
            return NV_WARN_MORE_PROCESSING_REQUIRED;

        nvStatus = _kgspProcessRpcEvent(pGpu, pRpc);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to process received event 0x%x (%s): status=0x%x\n",
                      pMsgHdr->function, _getRpcName(pMsgHdr->function), nvStatus);
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
        kgspDumpGspLogs(pGpu, pKernelGsp, NV_FALSE);
    }

    kgspHealthCheck_HAL(pGpu, pKernelGsp);

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
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NvU32 historyEntry = pRpc->rpcHistoryCurrent;
    NvU32 activeData[2];

    if (!pKernelGsp->bXid119Printed)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "********************************* GSP Failure **********************************\n");
    }

    NV_ASSERT(expectedFunc == pRpc->rpcHistory[historyEntry].function);

    _kgspGetActiveRpcDebugData(pRpc, expectedFunc,
                               &activeData[0], &activeData[1]);

    nvErrorLog_va((void*)pGpu, GSP_RPC_TIMEOUT,
                  "Timeout waiting for RPC from GSP! Expected function %d (%s) (0x%x 0x%x).",
                  expectedFunc,
                  _getRpcName(expectedFunc),
                  pRpc->rpcHistory[historyEntry].data[0],
                  pRpc->rpcHistory[historyEntry].data[1]);

    if (!pKernelGsp->bXid119Printed)
    {
        NvU32 historyIndex;

        if ((expectedFunc != vgpu_rpc_message_header_v->function)     ||
            (pRpc->rpcHistory[historyEntry].data[0] != activeData[0]) ||
            (pRpc->rpcHistory[historyEntry].data[1] != activeData[1]))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Current RPC function %d (%s) or data (0x%x 0x%x) does not match expected function %d (%s) or data (0x%x 0x%x).\n",
                      vgpu_rpc_message_header_v->function, _getRpcName(vgpu_rpc_message_header_v->function),
                      activeData[0], activeData[1],
                      expectedFunc, _getRpcName(expectedFunc),
                      pRpc->rpcHistory[historyEntry].data[0],
                      pRpc->rpcHistory[historyEntry].data[1]);
        }

        NV_PRINTF(LEVEL_ERROR, "RPC history (CPU -> GSP):\n");
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

        NV_PRINTF(LEVEL_ERROR, "Dumping stack:\n");
        osAssertFailed();

        NV_PRINTF(LEVEL_NOTICE,
                  "********************************************************************************\n");
    }

    pKernelGsp->bXid119Printed = NV_TRUE;
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
    NV_STATUS  nvStatus;
    RMTIMEOUT  timeout;
    NvU32      timeoutUs = GPU_TIMEOUT_DEFAULT;
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
        // We should only ever timeout this when GSP is in really bad state, so if it just
        // happens to timeout on default timeout it should be OK for us to give it a little
        // more time - make this timeout 1.5 of the default to allow some leeway.
        NvU32 defaultus = pGpu->timeoutData.defaultus;

        timeoutUs = defaultus + defaultus / 2;
    }

    NV_ASSERT(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));
    gpuSetTimeout(pGpu, timeoutUs, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    for (;;)
    {
        nvStatus = _kgspRpcDrainEvents(pGpu, pKernelGsp, expectedFunc);

        switch (nvStatus) {
            case NV_WARN_MORE_PROCESSING_REQUIRED:
                nvStatus = NV_OK;
                goto done;
            case NV_OK:
                // Check timeout and continue outer loop.
                break;
            default:
                goto done;
        }

        osSpinLoop();

        nvStatus = gpuCheckTimeout(pGpu, &timeout);
        if (nvStatus == NV_ERR_TIMEOUT)
        {
            _kgspLogXid119(pGpu, pRpc, expectedFunc);
            goto done;
        }

        if (osIsGpuShutdown(pGpu))
        {
            nvStatus = NV_ERR_GPU_IS_LOST;
            goto done;
        }
    }

done:
    pKernelGsp->bPollingForRpcResponse = NV_FALSE;

    if (bSlowGspRpc)
    {
        // Avoid cumulative timeout due to slow RPC
        threadStateResetTimeout(pGpu);
    }

    return nvStatus;
}

/*!
 * Initialize stripped down version of RPC infra init for GSP clients.
 */
static NV_STATUS
_kgspInitRpcInfrastructure
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS nvStatus = NV_OK;

    pKernelGsp->pRpc = initRpcObject(pGpu);
    if (pKernelGsp->pRpc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "initRpcObject failed\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    OBJRPC *pRpc = pKernelGsp->pRpc;

    portMemSet(&pRpc->rpcHistory, 0, sizeof(pRpc->rpcHistory));
    pRpc->rpcHistoryCurrent   = RPC_HISTORY_DEPTH - 1;

    pRpc->pMessageQueueInfo   = NULL;

    nvStatus = GspMsgQueueInit(pGpu, &pRpc->pMessageQueueInfo);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GspMsgQueueInit failed\n");
        _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
        return nvStatus;
    }

    pRpc->messageQueuePhysMem = pRpc->pMessageQueueInfo->sharedMemPA;
    pRpc->message_buffer      = (NvU32 *)pRpc->pMessageQueueInfo->pRpcMsgBuf;
    pRpc->maxRpcSize          = GSP_MSG_QUEUE_RPC_SIZE_MAX;
    pRpc->init_msg_buf        = (NvU32 *)pRpc->pMessageQueueInfo->pInitMsgBuf;
    pRpc->init_msg_buf_pa     = pRpc->pMessageQueueInfo->initMsgBufPA;

    portMemSet(&pKernelGsp->gspStaticInfo, 0,
               sizeof(pKernelGsp->gspStaticInfo));

    rpcSendMessage_FNPTR(pKernelGsp->pRpc) = _kgspRpcSendMessage;
    rpcRecvPoll_FNPTR(pKernelGsp->pRpc)    = _kgspRpcRecvPoll;

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
        GspMsgQueueCleanup(&pKernelGsp->pRpc->pMessageQueueInfo);
        rpcDestroy(pGpu, pKernelGsp->pRpc);
        portMemFree(pKernelGsp->pRpc);
        pKernelGsp->pRpc = NULL;
    }
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
    kgspDumpGspLogs(pGpu, pKernelGsp, NV_FALSE);

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

/*!
 * Initialize LIBOS task logging structures
 */
static NV_STATUS
_kgspInitLibosLoggingStructures
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_FIRMWARE *pGspFw
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
        {"LOGINIT", "INIT", 0x10000, ".fwlogging_init"},    // 64KB for stack traces
#if defined(DEVELOP) || defined(DEBUG)
        {"LOGVGPU", "VGPU", 0x40000, ".fwlogging_vgpu"},    // 256KB VGPU debug log on develop/debug builds
        {"LOGRM",   "RM",   0x40000, ".fwlogging_rm"}       // 256KB RM debug log on develop/debug builds
#else
        {"LOGVGPU", "VGPU", 0x10000, ".fwlogging_vgpu"},    // 64KB VGPU debug log on release builds
        {"LOGRM",   "RM",   0x10000, ".fwlogging_rm"}       // 64KB RM debug log on release builds
#endif
    };
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) <= LIBOS_LOG_MAX_LOGS);
    ct_assert(NV_ARRAY_ELEMENTS(logInitValues) == LOGIDX_SIZE);

    NV_STATUS nvStatus = NV_OK;
    NvU8      idx;

    libosLogCreate(&pKernelGsp->logDecode);

    for (idx = 0; idx < LOGIDX_SIZE; idx++)
    {
        RM_LIBOS_LOG_MEM *pLog = &pKernelGsp->rmLibosLogMem[idx];
        NvP64 pVa = NvP64_NULL;
        NvP64 pPriv = NvP64_NULL;

        //
        // Setup logging memory for each task.
        // Use MEMDESC_FLAGS_CPU_ONLY -- too early to call memdescMapIommu.
        //
        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescCreate(&pLog->pTaskLogDescriptor,
                          pGpu,
                          logInitValues[idx].size,
                          RM_PAGE_SIZE,
                          NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                          MEMDESC_FLAGS_NONE),
            error_cleanup);

        NV_ASSERT_OK_OR_GOTO(nvStatus,
            memdescAlloc(pLog->pTaskLogDescriptor),
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

    // Setup symbol decoder
    if (pGspFw->pLogElf)
    {
        const void *pLogData = NULL;
        NvU64 logSize = 0;

        NV_ASSERT_OK_OR_GOTO(
            nvStatus,
            _kgspFwContainerVerifyVersion(pGpu, pKernelGsp,
                pGspFw->pLogElf,
                pGspFw->logElfSize,
                "GSP firmware log"),
            error_cleanup);

        NV_ASSERT_OK_OR_GOTO(
            nvStatus,
            _kgspFwContainerGetSection(pGpu, pKernelGsp,
                pGspFw->pLogElf,
                pGspFw->logElfSize,
                GSP_LOGGING_SECTION_NAME,
                &pLogData,
                &logSize),
            error_cleanup);

        pKernelGsp->pLogElf = portMemAllocNonPaged(logSize);
        if (pKernelGsp->pLogElf == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for log elf");
            nvStatus = NV_ERR_NO_MEMORY;
            goto error_cleanup;
        }
        portMemCopy(pKernelGsp->pLogElf, logSize, pLogData, logSize);

        if (pKernelGsp->pLogElf)
        {
            libosLogInit(&pKernelGsp->logDecode, pKernelGsp->pLogElf, logSize);
        }
    }

error_cleanup:
    if (nvStatus != NV_OK)
        _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);

    return nvStatus;
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

    NV_ASSERT_OK_OR_GOTO(nvStatus,
        memdescAlloc(pKernelGsp->pMemDesc_simAccessBuf),
        error_cleanup);

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

    nvStatus = _kgspInitRpcInfrastructure(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init RPC infrastructure failed\n");
        return nvStatus;
    }

    nvStatus = kgspAllocBootArgs_HAL(pGpu, pKernelGsp);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "boot arg alloc failed: 0x%x\n", nvStatus);
        _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
        return nvStatus;
    }

    if (IS_SIMULATION(pGpu))
    {
        nvStatus = _kgspAllocSimAccessBuffer(pGpu, pKernelGsp);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "sim access buffer alloc failed: 0x%x\n", nvStatus);
            kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
            _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
            return nvStatus;
        }
    }

    return NV_OK;
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

        // Try and extract a VBIOS image.
        status = kgspExtractVbiosFromRom_HAL(pGpu, pKernelGsp, &pVbiosImg);

        if (status == NV_OK)
        {
            // Got a VBIOS image, now parse it for FWSEC.
            status = kgspParseFwsecUcodeFromVbiosImg(pGpu, pKernelGsp, pVbiosImg,
                                                        &pKernelGsp->pFwsecUcode);
            kgspFreeVbiosImg(pVbiosImg);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to parse FWSEC ucode from VBIOS image: 0x%x\n",
                            status);
                goto done;
            }
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

        // execute Booter Unload if needed to reset from unclean shutdown
        kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp);
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

    status = kgspCalculateFbLayout(pGpu, pKernelGsp, pGspFw);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error calculating FB layout\n");
        goto done;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, kgspInitLogging(pGpu, pKernelGsp, pGspFw), done);

    // Wait for GFW_BOOT OK status
    kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp);

    // bring up ucode with RM offload task
    status = kgspBootstrapRiscvOSEarly_HAL(pGpu, pKernelGsp, pGspFw);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "cannot bootstrap riscv/gsp: 0x%x\n", status);
        kgspHealthCheck_HAL(pGpu, pKernelGsp);
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
        // Preserve any captured gsp-rm logs
        libosPreserveLogs(&pKernelGsp->logDecode);
    }

    if (gpusLockedMask != 0)
    {
        rmGpuGroupLockRelease(gpusLockedMask, GPUS_LOCK_FLAGS_NONE);
    }

    return status;
}

NV_STATUS
kgspInitLogging_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    NV_STATUS nvStatus;

    nvStatus = _kgspInitLibosLoggingStructures(pGpu, pKernelGsp, pGspFw);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "init LIBOS logging structures failed: 0x%x\n", nvStatus);
        return nvStatus;
    }

    return nvStatus;
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
    kgspDumpGspLogs(pGpu, pKernelGsp, NV_FALSE);

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
        status = kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp);
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

    kgspFreeFlcnUcode(pKernelGsp->pFwsecUcode);
    pKernelGsp->pFwsecUcode = NULL;

    kgspFreeFlcnUcode(pKernelGsp->pBooterLoadUcode);
    pKernelGsp->pBooterLoadUcode = NULL;

    kgspFreeFlcnUcode(pKernelGsp->pBooterUnloadUcode);
    pKernelGsp->pBooterUnloadUcode = NULL;

    kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
    _kgspFreeLibosLoggingStructures(pGpu, pKernelGsp);
    _kgspFreeRpcInfrastructure(pGpu, pKernelGsp);
    _kgspFreeBootBinaryImage(pGpu, pKernelGsp);
    _kgspFreeSimAccessBuffer(pGpu, pKernelGsp);
}

/*!
 * Dump logs coming from GSP-RM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelGsp    KernelGsp pointer
 * @param[in] bSyncNvLog    NV_TRUE: Copy a snapshot of the libos logs
 *                          into the nvLog wrap buffers.
 */
void
kgspDumpGspLogs_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvBool bSyncNvLog
)
{
    if (!IS_GSP_CLIENT(pGpu))
    {
        return;
    }

    if (pKernelGsp->bInInit || pKernelGsp->pLogElf || bSyncNvLog)
    {
        libosExtractLogs(&pKernelGsp->logDecode, bSyncNvLog);
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
    MESSAGE_QUEUE_INFO *pMQI = pKernelGsp->pRpc->pMessageQueueInfo;
    GSP_SR_INIT_ARGUMENTS *pSrInitArgs =  &pGspArgs->srInitArguments;

    // Setup the message queue arguments
    pMQInitArgs->sharedMemPhysAddr   = pKernelGsp->pRpc->messageQueuePhysMem;
    pMQInitArgs->pageTableEntryCount = pMQI->pageTableEntryCount;
    pMQInitArgs->cmdQueueOffset      = pMQI->pageTableSize;
    pMQInitArgs->statQueueOffset     = pMQInitArgs->cmdQueueOffset + pMQI->commandQueueSize;

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

    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeImage == NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeDesc  == NULL, NV_ERR_INVALID_STATE);

    // get the bindata storage for the image/descriptor
    kgspGetGspRmBootUcodeStorage_HAL(pGpu, pKernelGsp, &pBinStorageImage, &pBinStorageDesc);

    // copy the image to sysmem
    bufSize = bindataGetBufferSize(pBinStorageImage);
    bufSizeAligned = NV_ALIGN_UP(bufSize, 0x1000);

    NV_ASSERT_OK_OR_GOTO(status,
                        memdescCreate(&pKernelGsp->pGspRmBootUcodeMemdesc,
                                pGpu,
                                bufSizeAligned,
                                RM_PAGE_SIZE,
                                NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                MEMDESC_FLAGS_NONE),
                        fail);

    NV_ASSERT_OK_OR_GOTO(status,
                         memdescAlloc(pKernelGsp->pGspRmBootUcodeMemdesc),
                         fail);

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

    // NOTE: align to 256 because that's the alignment needed for Booter DMA
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pKernelGsp->pSignatureMemdesc, pGpu,
            NV_ALIGN_UP(pGspFw->signatureSize, 256), 256,
            NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_NONE));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        memdescAlloc(pKernelGsp->pSignatureMemdesc), fail_create);

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
        _kgspCreateRadix3(pGpu, &pKernelGsp->pGspUCodeRadix3Descriptor,
            NULL, pGspFw->pImageData, pGspFw->imageSize));

    return NV_OK;
}

static NV_STATUS
_kgspCreateRadix3
(
    OBJGPU *pGpu,
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

    // radix3 working array.
    struct
    {
        NvU64  nPages;
        NvU64  offset;
    } radix3[4];

    NV_ASSERT_OR_RETURN(ppMemdescRadix3 != NULL, NV_ERR_INVALID_PARAMETER);
    NV_ASSERT_OR_ELSE_STR((pMemdescData != NULL) != (pData != NULL),
                          "Specify pMemdescData or pData, but not both",
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

    if (pData != NULL)
    {
        // We don't have a separate descriptor for the data.  We need PTEs,
        // so include space for data in the new descriptor.
        allocSize += radix3[3].nPages << LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(ppMemdescRadix3, pGpu, allocSize,
            LIBOS_MEMORY_REGION_RADIX_PAGE_SIZE,
            NV_MEMORY_NONCONTIGUOUS,
            ADDR_SYSMEM,
            NV_MEMORY_CACHED,
            MEMDESC_FLAGS_KERNEL_MODE),
        done);

    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(*ppMemdescRadix3), error_ret);

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

    if (pData != NULL)
    {
        dataOffset = radix3[3].offset;

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
    NV_STATUS       nvStatus     = NV_OK;

    //
    // Kernel RM can timeout when GSP-RM has an error condition.  Give GSP-RM
    // a chance to report the error before we pull the rug out from under it.
    //
    threadStateResetTimeout(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rpcRecvPoll(pGpu, pKernelGsp->pRpc, NV_VGPU_MSG_EVENT_GSP_INIT_DONE));

    // Now check if RPC really succeeded
    NV_ASSERT_OK_OR_RETURN(((rpc_message_header_v *)pKernelGsp->pRpc->message_buffer)->rpc_result);
    if (nvStatus != NV_VGPU_MSG_RESULT_SUCCESS)
    {
        NV_ASSERT_OK_FAILED("nvStatus", nvStatus);
    }

    return nvStatus;
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
    kgspDumpGspLogs(pGpu, pKernelGsp, NV_FALSE);
}

NV_STATUS
kgspStartLogPolling_IMPL
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status;
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
