/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Description:
//       This module implements event ring buffers and
//       the vGPU interrupt handler.
//
//******************************************************************************

#include "vgpu/vgpu_events.h"

#include "kernel/core/core.h"
#include "kernel/core/locks.h"
#include "kernel/core/system.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/rmapi/event.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/gpu_fabric_probe.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/device/device.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/timer/objtmr.h"
#include "kernel/gpu/gsp/gsp_trace_rats_macro.h"

#include "Nvcm.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "vgpu/dev_vgpu.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_util.h"

#ifndef RM_PAGE_SIZE
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#endif

#include "gpu/bus/kern_bus.h"

// workitem callback for pstate change event
static void
_rmPstateEventCallback(NvU32 gpuInstance, void *pPstateParams);

static inline NvU32 _readEventBufPut(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspResponseBuf->v1.putEventBuf;
}

static inline NvU32 _readEventBufGet(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspCtrlBuf->v1.getEventBuf;
}

static inline void _writeEventBufGet(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU32 val)
{
    pVGpu->gspCtrlBuf->v1.getEventBuf = val;
}

// This function is called when the first device is allocated.
// Different address space configs on different arch
// AD10x
//      - Memory is always allocated on FBMEM
// GH100+
//      - Memory is allocated on SYSMEM when BAR2 is in physical mode
//      - memory is allocated on FBMEM when BAR2 switches virtual mode
// GH180
//      - Memory is allocated on SYSMEM initially
//      - Memory is allocated on FBMEM after C2C mapping is completed
//
NV_STATUS _setupGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NV_ADDRESS_SPACE addressSpace = ADDR_FBMEM;
    NvU32 memFlags = 0;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    if (!kbusIsBar2Initialized(pKernelBus) || (pVGpu->bAllocGspBufferInSysmem))
        addressSpace = ADDR_SYSMEM;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              addressSpace,
                              memFlags,
                              &pVGpu->eventRing.mem.pMemDesc,
                              (void**)&pVGpu->eventRing.mem.pMemory,
                              (void**)&pVGpu->eventRing.mem.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "_setupGspEventInfrastructure: GSP Event buf memory setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->eventRing.mem.pfn = memdescGetPte(pVGpu->eventRing.mem.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    portMemSet(pVGpu->eventRing.mem.pMemory, 0, memdescGetSize(pVGpu->eventRing.mem.pMemDesc));

    return NV_OK;
}

// This function is called when the device is freed.
void _teardownGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pVGpu->eventRing.mem.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->eventRing.mem.pMemDesc,
                    (void**)&pVGpu->eventRing.mem.pMemory,
                    (void**)&pVGpu->eventRing.mem.pPriv);
}

// Check if a VGPU event is pending
NvBool
vgpuGetPendingEvent(OBJGPU *pGpu, THREAD_STATE_NODE *pThreadState)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    if (pVGpu->bGspPlugin)
    {
        if (_readEventBufPut(pGpu, pVGpu) != _readEventBufGet(pGpu, pVGpu))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

static void
vgpuRcErrorRecovery
(
    OBJGPU *pGpu,
    NvU32   chID,
    NvU32   exceptType,
    RM_ENGINE_TYPE rmEngineType
)
{
    CHID_MGR                *pChidMgr = NULL;
    KernelChannel           *pKernelChannel;
    KernelFifo              *pKernelFifo      = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS                status           = NV_OK;
    FIFO_MMU_EXCEPTION_DATA  mmuExceptionData = {0};

    status = kfifoGetChidMgrFromType(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                     (NvU32)rmEngineType, &pChidMgr);
    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo, pChidMgr,
                                                  chID);
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pKernelChannel != NULL);

    //
    // In case of SRIOV, the notifier is updated before invoking the RPC for RC
    // in case of an MMU fault. So skip notifying here if the exception type is
    // 31 and SRIOV is enabled
    //
    if (exceptType != ROBUST_CHANNEL_FIFO_ERROR_MMU_ERR_FLT ||
        (exceptType == ROBUST_CHANNEL_FIFO_ERROR_MMU_ERR_FLT &&
         IS_VIRTUAL_WITHOUT_SRIOV(pGpu)))
    {
        // send the error notifier to clients
        krcErrorSetNotifier(pGpu, GPU_GET_KERNEL_RC(pGpu),
                            pKernelChannel,
                            exceptType,
                            rmEngineType,
                            RC_NOTIFIER_SCOPE_TSG);
    }

    // call back to KMD in case win7
    krcErrorInvokeCallback(pGpu, GPU_GET_KERNEL_RC(pGpu),
                           pKernelChannel,
                           &mmuExceptionData,
                           exceptType,
                           ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_FATAL,
                           rmEngineType,
                           INVALID_RCDB_RCDIAG_INDEX);
}

void vgpuServiceEventGuestAllocated(OBJGPU *pGpu, OBJVGPU *pVGpu, VGPU_EVENT_BUF_ENTRY *pEventEntry)
{
    NV_STATUS status = NV_OK;
    PEVENTNOTIFICATION *ppEventNotification;
    NvHandle hClient = pEventEntry->hClient;
    NvHandle hObject = pEventEntry->hObject;
    NvU32 notifyIdx  = pEventEntry->notifyIndex;

    // Find the PEVENTNOTIFICATION list of this object
    status = CliGetEventNotificationList(hClient, hObject, NULL, &ppEventNotification);
    if (status == NV_OK)
    {
        // Wake up all clients registered to listen for this event
        notifyEvents(pGpu, *ppEventNotification,
                     notifyIdx, 0, 0,
                     NV_OK, NV_OS_WRITE_THEN_AWAKEN);
    }
}

void vgpuServiceEventRC(OBJGPU *pGpu, OBJVGPU *pVGpu, VGPU_EVENT_BUF_ENTRY *pEventEntry)
{
    NV_STATUS status = NV_OK;
    NvHandle hClient = pEventEntry->hClient;
    NvHandle hObject = pEventEntry->hObject;
    NvU32 exceptType = pEventEntry->info32;
    NvU32 nv2080EngineID = pEventEntry->info16 & DRF_SHIFTMASK(NV_VGPU_EV_NOTIFIER_INFO16_VALUE);
    NvU32 chID       = pEventEntry->rcChid;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(nv2080EngineID);

    NV_PRINTF(LEVEL_ERROR,
              "ROBUST_CHANNEL error occurred (hClient = 0x%x hFifo = 0x%x chID = %d exceptType = %d engineID = 0x%x (0x%x)) ...\n",
              hClient, hObject, chID, exceptType, nv2080EngineID, rmEngineType);

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        RsClient         *pClient           = NULL;
        Device           *pDevice           = NULL;
        MIG_INSTANCE_REF  ref;

        NV_ASSERT_OK_OR_ELSE(status,
            serverGetClientUnderLock(&g_resServ, hClient, &pClient), return);

        NV_ASSERT_OK_OR_ELSE(status,
            deviceGetByGpu(pClient, pGpu, NV_TRUE, &pDevice), return);

        NV_ASSERT_OK_OR_ELSE(status,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref),
            return);

        NV_ASSERT_OK_OR_ELSE(status,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType, &rmEngineType),
            return);
    }

    vgpuRcErrorRecovery(pGpu, chID, exceptType, rmEngineType);
}

void vgpuServiceEventVnc(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status = NV_OK;

    pVGpu->bVncConnected = !!(*(NvU32 *)(pVGpu->shared_memory +
                            (NV_VGPU_SHARED_MEMORY_POINTER_VNC / sizeof(NvU32))));

    if (pVGpu->bVncConnected)
    {

        /* Set surface property without comparison with cached, when console vnc connects */
        NV_RM_RPC_SET_SURFACE_PROPERTIES(pGpu, pVGpu->last_surface_info.hClient,
                                         &(pVGpu->last_surface_info.last_surface), NV_TRUE, status);
        if (status != NV_OK) {
            NV_PRINTF(LEVEL_ERROR,
                      "SET_SURFACE_PROPERTY RPC failed with error : 0x%x\n",
                      status);
        }
    }
}

void vgpuServiceEventPstate(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status = NV_OK;
    NvU32 *pCurrPstate;

    pCurrPstate = (pVGpu->shared_memory + (NV_VGPU_SHARED_MEMORY_POINTER_CURRENT_PSTATE / sizeof(NvU32)));

    //Schedule OS workitem to call pstate change notifier
    status = osQueueWorkItemWithFlags(pGpu,
                                      _rmPstateEventCallback,
                                      (void *)pCurrPstate,
                                      OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE |
                                      OS_QUEUE_WORKITEM_FLAGS_DONT_FREE_PARAMS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to schedule Pstate callback! 0x%x\n",
                  status);
    }
}

void vgpuServiceEventEcc(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NvU32 ecc_type  = * (NvU32 *)(pVGpu->shared_memory + (NV_VGPU_SHARED_MEMORY_POINTER_ECC_TYPE / sizeof(NvU32)));
    NvU32 ecc_count = * (NvU32 *)(pVGpu->shared_memory + (NV_VGPU_SHARED_MEMORY_POINTER_ECC_ERROR_COUNT / sizeof(NvU32)));
    NvU32 ecc_unit  = * (NvU32 *)(pVGpu->shared_memory + (NV_VGPU_SHARED_MEMORY_POINTER_ECC_UNIT / sizeof(NvU32)));

    if (ecc_unit < NV2080_CTRL_GPU_ECC_UNIT_COUNT)
    {
        if (ecc_type == NV2080_NOTIFIERS_ECC_SBE)
        {
            pVSI->eccStatus.units[ecc_unit].sbe.count              += ecc_count;
            pVSI->eccStatus.units[ecc_unit].sbeNonResettable.count += ecc_count;
        }
        else
        {
            pVSI->eccStatus.units[ecc_unit].dbe.count              += ecc_count;
            pVSI->eccStatus.units[ecc_unit].dbeNonResettable.count += ecc_count;
        }

    }

    if (gpuIsGlobalPoisonFuseEnabled(pGpu))
    {
        pVSI->eccStatus.bFatalPoisonError  = !!(* (NvU32 *)(pVGpu->shared_memory +
                                        (NV_VGPU_SHARED_MEMORY_POINTER_ECC_POISON_ERROR / sizeof(NvU32))));
    }

    if (ecc_count)
    {

        gpuNotifySubDeviceEvent(pGpu, ecc_type, NULL, 0,
                                (NvV32) ecc_count, (NvV16) ecc_unit);
    }
}

void vgpuServiceEventNvencReportingState(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pGpu->encSessionStatsReportingState =
        (*(NvU32 *)(pVGpu->shared_memory + (NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE / sizeof(NvU32))));
}

void vgpuServiceEventInbandResponse(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status = NV_OK;

    OBJRPC * pRpc = GPU_GET_RPC(pGpu);
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pData = NULL;
    NvBool more = NV_TRUE;

    if (!pVGpu->bGspPlugin)
        return;

    while (more)
    {
        NvU32 inband_resp_state = (*(volatile NvU32 *)(pVGpu->shared_memory +
                    (NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE / sizeof(NvU32))));

        if (inband_resp_state == NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_NONE)
        {
            break;
        }
        if (pData == NULL)
        {
            pData = portMemAllocNonPaged(sizeof(*pData));
        }
        portMemSet(pData, 0, sizeof(*pData));

        if (FLD_TEST_DRF(_VGPU_SHARED_MEMORY_POINTER, _NVLINK_INBAND_RESPONSE, _PROBE, _PENDING, inband_resp_state))
        {
            status = rpcCtrlNvlinkGetInbandReceivedData_HAL(pGpu, pRpc, pData,
                                                            NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP, &more);
            if (status != NV_OK)
                goto cleanup;

            status = knvlinkInbandMsgCallbackDispatcher(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
                                                        sizeof(*pData), (void*)pData);
            if (status != NV_OK)
                goto cleanup;

            break;
        }
        if (FLD_TEST_DRF(_VGPU_SHARED_MEMORY_POINTER, _NVLINK_INBAND_RESPONSE, _MC_SETUP, _PENDING, inband_resp_state))
        {
            status = rpcCtrlNvlinkGetInbandReceivedData_HAL(pGpu, pRpc, pData,
                                                            NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP, &more);
            if (status != NV_OK)
                goto cleanup;

            status = knvlinkInbandMsgCallbackDispatcher(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
                                                        sizeof(*pData), (void*)pData);
            if (status != NV_OK)
                goto cleanup;
        }
    }
cleanup:
    if (pData != NULL)
        portMemFree(pData);
}

void vgpuServiceEventTracing(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
#if KERNEL_GSP_TRACING_RATS_ENABLED
    gspTraceServiceVgpuEventTracing(pGpu);
#endif
}

void vgpuServiceEvents(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    VGPU_EVENT_BUF_ENTRY *pEventEntry;
    NvU32 flags;

    pVGpu->eventRing.get = _readEventBufGet(pGpu, pVGpu);
    pVGpu->eventRing.put = _readEventBufPut(pGpu, pVGpu);

    // process all pending events
    while (pVGpu->eventRing.get != pVGpu->eventRing.put)
    {
        pEventEntry = ((VGPU_EVENT_BUF_ENTRY*) pVGpu->eventRing.mem.pMemory) + pVGpu->eventRing.get;

        flags = pEventEntry->flags;

        if (FLD_TEST_DRF(_VGPU, _EV_FLAGS, _ALLOCATED, _GUEST, flags))
        {
            vgpuServiceEventGuestAllocated(pGpu, pVGpu, pEventEntry);
        }
        else if (FLD_TEST_DRF(_VGPU, _EV_FLAGS, _ALLOCATED, _PLUGIN, flags))
        {
            // Plugin allocated events may/may not have guest-side equivalents
            switch (DRF_VAL(_VGPU, _EV_FLAGS, _TYPE, flags))
            {
                case NV_VGPU_EV_FLAGS_TYPE_ROBUST_CHANNEL_ERROR:
                    vgpuServiceEventRC(pGpu, pVGpu, pEventEntry);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_VNC:
                    vgpuServiceEventVnc(pGpu, pVGpu);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_PSTATE:
                    vgpuServiceEventPstate(pGpu, pVGpu);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_ECC:
                    vgpuServiceEventEcc(pGpu, pVGpu);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_NVENC_REPORTING_STATE:
                    vgpuServiceEventNvencReportingState(pGpu, pVGpu);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_INBAND_RESPONSE:
                    vgpuServiceEventInbandResponse(pGpu, pVGpu);
                    break;

                case NV_VGPU_EV_FLAGS_TYPE_TRACING:
                    vgpuServiceEventTracing(pGpu,pVGpu);
                    break;

                default:
                    NV_PRINTF(LEVEL_ERROR, "Unsupported vgpu event type %d\n",
                              DRF_VAL(_VGPU, _EV_FLAGS, _TYPE, flags));
                    break;
            }
        }

        // update the get/put pointers
        pVGpu->eventRing.get = (pVGpu->eventRing.get + 1) % VGPU_EVENT_BUF_ENTRY_COUNT;
        _writeEventBufGet(pGpu, pVGpu, pVGpu->eventRing.get);
        pVGpu->eventRing.put = _readEventBufPut(pGpu, pVGpu);
    }
}

void vgpuServiceGspPlugin(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    vgpuServiceEvents(pGpu, pVGpu);
}

// Service a VGPU event (bottom half/DPC)
void vgpuService(OBJGPU *pGpu)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    if (pVGpu->bGspPlugin)
        vgpuServiceGspPlugin(pGpu, pVGpu);
}

/*
 *  Workitem callback for pstate change event
 *
 *   To run at PASSIVE_LEVEL, we queue a workitem for
 *   gpuNotifySubDeviceEvent().
 *   So this callback will notify all the guest rmclients
 *   registered for NV2080_NOTIFIERS_PSTATE_CHANGE event.
 */
static void
_rmPstateEventCallback
(
    NvU32 gpuInstance,
    void *pCurrPstate
)
{
    OBJGPU     *pGpu = gpumgrGetGpu(gpuInstance);
    OBJTMR     *pTmr = GPU_GET_TIMER(pGpu);
    NvU64       startTime = 0;
    NvU32       currPstate = *(NvU32 *)pCurrPstate;
    Nv2080PStateChangeNotification pstateParams;

    tmrGetCurrentTime(pTmr, &startTime);

    if (FULL_GPU_SANITY_CHECK(pGpu))
    {
        pstateParams.timeStamp.nanoseconds[0] = NvU64_HI32(startTime);
        pstateParams.timeStamp.nanoseconds[1] = NvU64_LO32(startTime);
        pstateParams.NewPstate = currPstate;

        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_PSTATE_CHANGE,
                &pstateParams, sizeof(pstateParams), currPstate, 0);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU sanity check failed! gpuInstance = 0x%x.\n",
                  gpuInstance);
    }
}
