/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/rc/kernel_rc.h"

#include "kernel/gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/mem_mgr/context_dma.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator_common.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "nverror.h"
#include "nvstatus.h"

static NV_STATUS
_krcErrorWriteNotifierCpuMemHelper
(
    OBJGPU    *pGpu,
    Memory    *pMemory,
    Device    *pDevice,
    NvU32      exceptType,
    RM_ENGINE_TYPE localRmEngineType,
    NV_STATUS  notifierStatus
)
{
    NV_STATUS status = NV_OK;

    switch (memdescGetAddressSpace(pMemory->pMemDesc))
    {
        case ADDR_VIRTUAL:
            notifyFillNotifierGPUVA(pGpu,
                pDevice,
                RES_GET_HANDLE(pMemory),
                memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU_VA, 0),
                exceptType,
                (NvU16)gpuGetNv2080EngineType(localRmEngineType),
                notifierStatus,
                0 /* Index */);
            break;

        case ADDR_SYSMEM:
        case ADDR_FBMEM:
            notifyFillNotifierMemory(pGpu,
                                     pMemory,
                                     exceptType,
                                     (NvU16)gpuGetNv2080EngineType(localRmEngineType),
                                     notifierStatus,
                                     0 /* Index */ );
            break;

        default:
            status = NV_ERR_NOT_SUPPORTED;
    }
    return status;
}


/*!
 * This path is called in GSP_CLIENT, vGPU and MONOLITHIC cases, except in the
 * GSP_CLIENT path where GSP has already written to the notifiers.
 * In that case, rcErrorSendEventNotifications is called which
 * only sends the notifications without actually writing to the error notifiers.
 */
NV_STATUS
krcErrorWriteNotifier_CPU
(
    OBJGPU        *pGpu,
    KernelRc      *pKernelRc,
    KernelChannel *pKernelChannel,
    NvU32          exceptType,
    RM_ENGINE_TYPE localRmEngineType,
    NV_STATUS      notifierStatus,
    NvU32         *pFlushFlags
)
{
    NV_STATUS   status = NV_OK;
    ContextDma *pContextDma;
    Memory     *pMemory;
    Device     *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);
    //
    // Update the ECC error notifier for exceptTypes related to
    // CONTAINED/UNCONTAINED/DBE errors
    //
    NvBool bUpdateEccNotifier = (
        exceptType == ROBUST_CHANNEL_GPU_ECC_DBE ||
        exceptType == ROBUST_CHANNEL_UNCONTAINED_ERROR ||
        exceptType == ROBUST_CHANNEL_CONTAINED_ERROR);

    if (hypervisorIsVgxHyper() && bUpdateEccNotifier &&
        pKernelChannel->hEccErrorContext != NV01_NULL_OBJECT)
    {
        //
        // Attempt to use a context DMA notification with an event that
        // wakes up the listener. If a context DMA is not found, fall back
        // to a memory-based notifier without an event.
        //
        if (ctxdmaGetByHandle(RES_GET_CLIENT(pKernelChannel),
                              pKernelChannel->hEccErrorContext,
                              &pContextDma) == NV_OK)
        {
            notifyFillNotifier(pGpu,
                               pContextDma,
                               exceptType,
                               (NvU16)gpuGetNv2080EngineType(localRmEngineType),
                               notifierStatus);
        }
        else if ((status = memGetByHandleAndDevice(
                      RES_GET_CLIENT(pKernelChannel),
                      pKernelChannel->hEccErrorContext,
                      RES_GET_HANDLE(pDevice),
                      &pMemory)) == NV_OK)
        {
            status = _krcErrorWriteNotifierCpuMemHelper(pGpu,
                pMemory,
                pDevice,
                exceptType,
                localRmEngineType,
                notifierStatus);
        }

        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "notified (ECC) channel %d\n",
                      kchannelGetDebugTag(pKernelChannel));
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                "No valid error notifier found for ECC error context 0x%x\n"
                "Skipping notification update\n",
                pKernelChannel->hEccErrorContext);
        }
    }

    if (ctxdmaGetByHandle(RES_GET_CLIENT(pKernelChannel),
                          pKernelChannel->hErrorContext,
                          &pContextDma) == NV_OK)
    {
        PEVENTNOTIFICATION pEventNotifications = inotifyGetNotificationList(
            staticCast(pContextDma, INotifier));
        notifyFillNotifier(pGpu,
                           pContextDma,
                           exceptType,
                           (NvU16)gpuGetNv2080EngineType(localRmEngineType),
                           notifierStatus);
        NV_PRINTF(LEVEL_INFO, "notified channel %d\n",
                  kchannelGetDebugTag(pKernelChannel));
        if (pEventNotifications)
        {
            notifyEvents(pGpu,
                         pEventNotifications,
                         0,
                         0,
                         RES_GET_HANDLE(pContextDma),
                         status,
                         NV_OS_WRITE_THEN_AWAKEN);
        }
    }
    else if ((status = memGetByHandleAndDevice(RES_GET_CLIENT(pKernelChannel),
                                               pKernelChannel->hErrorContext,
                                               RES_GET_HANDLE(pDevice),
                                               &pMemory)) == NV_OK)
    {
        status = _krcErrorWriteNotifierCpuMemHelper(pGpu,
            pMemory,
            pDevice,
            exceptType,
            localRmEngineType,
            notifierStatus);
        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "notified channel %d\n",
                      kchannelGetDebugTag(pKernelChannel));
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                "No valid error notifier found for error context 0x%x\n"
                "Skipping notification update\n",
                pKernelChannel->hErrorContext);
        }
    }
    return status;
}


/*!
 * Update a channel's error notifier with exception type, engine id, and an
 * error code
 *
 * Supported notifier types are: CTXDMA, GPU virtual memory, system memory,
 * VIDMEM
 *
 * @param[in] pKernelChannel    Channel
 * @param[in] exceptType        Exception type written to notifier
 * @param[in] rmEngineType      RM Engine ID written to notifier
 * @param[in] scope             If we should notify every channel in the TSG
 *
 * @returns NV_OK                  if successful
 *          NV_ERR_INVALID_CHANNEL if the specified channel does not exist
 *          NV_ERR_INVALID_DEVICE  if the channel's device cannot be found
 *          NV_ERR_NOT_SUPPORTED   if the notifier type is not supported
 */
NV_STATUS krcErrorSetNotifier_IMPL
(
    OBJGPU            *pGpu,
    KernelRc          *pKernelRc,
    KernelChannel     *pKernelChannel,
    NvU32              exceptType,
    RM_ENGINE_TYPE     rmEngineType,
    RC_NOTIFIER_SCOPE  scope
)
{
    KernelFifo   *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    OBJSYS       *pSys            = SYS_GET_INSTANCE();
    NvU32         status          = NV_OK;
    NvU32         flushFlags      = 0;
    NvBool        bNewListCreated = NV_FALSE;
    CHANNEL_NODE *pChanNode;
    CHANNEL_LIST *pChanList;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    //
    // WAR bug 4503046: mark reboot required when any UVM channels receive an
    // error.
    //
    if (pKernelChannel->bUvmOwned)
    {
        sysSetRecoveryRebootRequired(pSys, NV_TRUE);
    }

    //
    // WAR bug 200326278, 200474671
    //
    // Notifying TSG wide causes vGPU failures, so avoid TSG wide notification
    // when GPU is Virtualized.
    //
    if (scope == RC_NOTIFIER_SCOPE_TSG &&
        kfifoIsSubcontextSupported(pKernelFifo) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
    {
        pChanList = (pKernelChannel->pKernelChannelGroupApi
                     ->pKernelChannelGroup->pChanList);
    }
    else
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             kfifoChannelListCreate(pGpu, pKernelFifo, &pChanList),
                             Error);
        bNewListCreated = NV_TRUE;

        NV_ASSERT_OK_OR_GOTO(status,
            kfifoChannelListAppend(pGpu, pKernelFifo, pKernelChannel, pChanList),
            Error);
    }

    for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
    {
        RM_ENGINE_TYPE localRmEngineType;
        KernelChannel *pKernelChannel = pChanNode->pKernelChannel;

        //
        // If MIG is enabled, RM need to notify partition local engineId.
        // Convert global ID to partition local if client has filled proper
        // engineIDs
        //
        localRmEngineType = rmEngineType;
        if (IS_MIG_IN_USE(pGpu) &&
            RM_ENGINE_TYPE_IS_VALID(rmEngineType))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            MIG_INSTANCE_REF ref;

            status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                     GPU_RES_GET_DEVICE(pKernelChannel),
                                                     &ref);
            if (status != NV_OK)
                goto Error;

            if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Notifier requested for an unsupported engine 0x%x (0x%x)\n",
                    gpuGetNv2080EngineType(rmEngineType), rmEngineType);
                status = NV_ERR_INVALID_ARGUMENT;
                goto Error;
            }

            // Override the engine type with the local engine idx
            status = kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager,
                                                       ref,
                                                       rmEngineType,
                                                       &localRmEngineType);
            if (status != NV_OK)
                goto Error;
        }

        NV_ASSERT_OK_OR_GOTO(status,
            krcErrorWriteNotifier_HAL(pGpu, pKernelRc,
                                      pKernelChannel,
                                      exceptType,
                                      localRmEngineType,
                                      0xffff /* notifierStatus */,
                                      &flushFlags),
            Error)
    }

    if (flushFlags != 0) {
        kbusFlush_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), flushFlags);
    }

Error:
    if (bNewListCreated)
    {
        kfifoChannelListDestroy(pGpu, pKernelFifo, pChanList);
    }
    return status;
}


/*! Send notifications for notifiers what were already written to.
 *
 * GSP writes to notifiers to avoid a race condition between the time when an
 * error is handled and when the client receives notifications. However, GSP
 * doesn't actually send those notification events to the client in some cases
 * as that requires OS interaction.
 *
 * This function actually sends those notifications to the client.
 *
 */
NV_STATUS
krcErrorSendEventNotificationsCtxDma_FWCLIENT
(
    OBJGPU           *pGpu,
    KernelRc         *pKernelRc,
    KernelChannel    *pKernelChannel,
    RC_NOTIFIER_SCOPE scope
)
{
    NV_STATUS          status          = NV_OK;
    KernelFifo        *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    NvBool             bNewListCreated = NV_FALSE;
    CHANNEL_NODE      *pChanNode;
    CHANNEL_LIST      *pChanList;

    //
    // This function should only be called on paths where GSP has already
    // written to notifiers
    //
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_INVALID_STATE);

    // WAR bug 200326278, 200474671
    if (scope == RC_NOTIFIER_SCOPE_TSG &&
        kfifoIsSubcontextSupported(pKernelFifo))
    {
        // Notify all channels in the TSG
        pChanList = (pKernelChannel->pKernelChannelGroupApi
                     ->pKernelChannelGroup->pChanList);
    }
    else
    {
        // Setup a fake list for the channel to be notified
        status = kfifoChannelListCreate(pGpu, pKernelFifo, &pChanList);
        if (status != NV_OK) {
            NV_PRINTF(LEVEL_ERROR, "failed to create notification list\n");
            goto Error;
        }
        bNewListCreated = NV_TRUE;

        status = kfifoChannelListAppend(pGpu, pKernelFifo, pKernelChannel, pChanList);
        if (status != NV_OK) {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to insert channel into notification list\n");
            goto Error;
        }
    }

    for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
    {
        ContextDma *pContextDma;

        if (ctxdmaGetByHandle(RES_GET_CLIENT(pChanNode->pKernelChannel),
                              pChanNode->pKernelChannel->hErrorContext,
                              &pContextDma) == NV_OK)
        {
            EVENTNOTIFICATION *pEventNotifications = inotifyGetNotificationList(
                staticCast(pContextDma, INotifier));
            if (pEventNotifications != NULL)
            {
                notifyEvents(pGpu, pEventNotifications, 0, 0,
                             RES_GET_HANDLE(pContextDma), status,
                             NV_OS_WRITE_THEN_AWAKEN);
            }
        }
    }

Error:
    if (bNewListCreated)
    {
        kfifoChannelListDestroy(pGpu, pKernelFifo, pChanList);
    }
    return status;
}


NV_STATUS
krcErrorSendEventNotifications_KERNEL
(
    OBJGPU            *pGpu,
    KernelRc          *pKernelRc,
    KernelChannel     *pKernelChannel,
    RM_ENGINE_TYPE     rmEngineType,       // unused
    NvU32              exceptLevel,        // unused
    NvU32              exceptType,
    RC_NOTIFIER_SCOPE  scope,
    NvU16              partitionAttributionId,
    NvBool             bOsRcCallbackNeeded // unused
)
{
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (pKernelChannel != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(
            krcErrorSendEventNotificationsCtxDma_HAL(pGpu, pKernelRc,
                                                     pKernelChannel,
                                                     scope));
    }

    gpuNotifySubDeviceEvent(pGpu,
                            NV2080_NOTIFIERS_RC_ERROR,
                            NULL,
                            0,
                            exceptType,
                            partitionAttributionId);
    return NV_OK;
}
