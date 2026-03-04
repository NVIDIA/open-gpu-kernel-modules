/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "rmapi/rmapi.h"
#include "core/locks.h"
#include "gpu/device/device.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

NV_STATUS
kfifoIdleChannelsPerDevice_KERNEL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvHandle   *phClients,
    NvHandle   *phDevices,
    NvHandle   *phChannels,
    NvU32       numChannels,
    NvU32       flags,
    NvU32       timeout
)
{
    NV_STATUS rmStatus = NV_OK;
    NV_RM_RPC_IDLE_CHANNELS(pGpu, phClients, phDevices, phChannels,
                            numChannels, flags, timeout, rmStatus);
    return rmStatus;
}


NV_STATUS
RmIdleChannels
(
    NvHandle    hClient,
    NvHandle    hDevice,
    NvHandle    hChannel,
    NvU32       numChannels,
    NvP64       clients,
    NvP64       devices,
    NvP64       channels,
    NvU32       flags,
    NvU32       timeout,
    NvBool      bUserModeArgs
)
{
    OBJGPU          *pGpu;
    KernelFifo      *pKernelFifo;
    NV_STATUS        rmStatus = NV_OK;
    RMAPI_PARAM_COPY paramCopyClients;
    RMAPI_PARAM_COPY paramCopyDevices;
    RMAPI_PARAM_COPY paramCopyChannels;
    NvU32            gpuIdx, chanIdx;
    NvHandle        *phClients            = NULL;
    NvHandle        *phDevices            = NULL;
    NvHandle        *phChannels           = NULL;
    NvU32            numChannelsPerGpu[NV_MAX_DEVICES] = {0};
    NvBool           isGpuGrpLockAcquired = NV_FALSE;
    NvU32            gpuLockMask          = 0;

    NV_PRINTF(LEVEL_INFO, "hChannel: 0x%x, numChannels: %u\n", hChannel,
              numChannels);

    LOCK_METER_DATA(IDLE_CHANNELS, flags, numChannels, 0);

    switch(DRF_VAL(OS30, _FLAGS, _CHANNEL, flags))
    {
        case NVOS30_FLAGS_CHANNEL_SINGLE:
            numChannels = 1;
            phClients  = &hClient;
            phDevices  = &hDevice;
            phChannels = &hChannel;
            break;

        case NVOS30_FLAGS_CHANNEL_LIST:

            if (numChannels == 0)
            {
                return NV_OK;
            }

            // setup for access to client's parameters
            RMAPI_PARAM_COPY_INIT(paramCopyClients,
                                  phClients,
                                  clients,
                                  numChannels, sizeof(NvU32));

            rmStatus = rmapiParamsAcquire(&paramCopyClients, bUserModeArgs);
            if (rmStatus != NV_OK)
                goto done;

            RMAPI_PARAM_COPY_INIT(paramCopyDevices,
                                  phDevices,
                                  devices,
                                  numChannels, sizeof(NvU32));

            rmStatus = rmapiParamsAcquire(&paramCopyDevices, bUserModeArgs);
            if (rmStatus != NV_OK)
                    goto done;

            RMAPI_PARAM_COPY_INIT(paramCopyChannels,
                                  phChannels,
                                  channels,
                                  numChannels, sizeof(NvU32));

            rmStatus = rmapiParamsAcquire(&paramCopyChannels, bUserModeArgs);
            if (rmStatus != NV_OK)
                goto done;

            break;
        default:
            return NV_ERR_INVALID_FLAGS;
    }

    // This loop converts subdevice handles to device handles (IMO it's hopeless
    // to try to idle a channel on just one subdevice, if it's in use on
    // both).
    for (chanIdx = 0; chanIdx < numChannels; chanIdx++)
    {
        NvU32        gpuInst;
        RsClient    *pClient;
        GpuResource *pGpuResource;

        //
        // Don't allow other clients' resources to be accessed/modified by this
        // control call.
        //
        if (hClient != phClients[chanIdx])
        {
            rmStatus = NV_ERR_INSUFFICIENT_PERMISSIONS;
            goto done;
        }

        rmStatus = serverGetClientUnderLock(&g_resServ, phClients[chanIdx],
                                            &pClient);
        if (rmStatus != NV_OK)
            goto done;

        rmStatus = gpuresGetByDeviceOrSubdeviceHandle(pClient,
                                                      phDevices[chanIdx],
                                                      &pGpuResource);
        if (rmStatus != NV_OK)
            goto done;

        pGpu = GPU_RES_GET_GPU(pGpuResource);

        GPU_RES_SET_THREAD_BC_STATE(pGpuResource);

        // Update hDevice if it was originally a hSubdevice
        phDevices[chanIdx] = RES_GET_HANDLE(GPU_RES_GET_DEVICE(pGpuResource));

        // Update lock mask
        gpuInst = gpuGetInstance(pGpu);
        numChannelsPerGpu[gpuInst] += 1;
        gpuLockMask |= gpumgrGetGrpMaskFromGpuInst(gpuInst);
    }

    // This acquire locks(If not already acquired) in ascending order of
    // gpus we're trying to idle.
    if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuLockMask))
    {
        // LOCK: acquire Device lock
        rmStatus = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                         GPUS_LOCK_FLAGS_NONE,
                                         RM_LOCK_MODULES_FIFO, &gpuLockMask);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to acquire Device lock, error 0x%x\n", rmStatus);
            goto done;
        }
        isGpuGrpLockAcquired = NV_TRUE;
    }

    // Loop over all devices, idling those necessary given this channel list
    for (gpuIdx = 0; gpuIdx < NV_MAX_DEVICES; ++gpuIdx)
    {
        NvHandle *pPerGpuClients;
        NvHandle *pPerGpuDevices;
        NvHandle *pPerGpuChannels;
        NvU32     perGpuIdx = 0;

        if (numChannelsPerGpu[gpuIdx] == 0)
        {
            continue;
        }

        pGpu = gpumgrGetGpu(gpuIdx);
        pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

        pPerGpuClients  = portMemAllocNonPaged((sizeof *pPerGpuClients) *
                                               numChannelsPerGpu[gpuIdx]);
        pPerGpuDevices  = portMemAllocNonPaged((sizeof *pPerGpuDevices) *
                                               numChannelsPerGpu[gpuIdx]);
        pPerGpuChannels = portMemAllocNonPaged((sizeof *pPerGpuChannels) *
                                               numChannelsPerGpu[gpuIdx]);
        NV_CHECK_OR_ELSE(LEVEL_ERROR,
                         ((pPerGpuClients  != NULL) &&
                          (pPerGpuDevices  != NULL) &&
                          (pPerGpuChannels != NULL)),
                         rmStatus = NV_ERR_NO_MEMORY; goto done);

        for (chanIdx = 0;
             chanIdx < numChannels && perGpuIdx < numChannelsPerGpu[gpuIdx];
             chanIdx++)
        {
            RsClient    *pClient;
            GpuResource *pGpuResource;

            //
            // This does occasionally fail when the client was asynchronously
            // killed
            // NOTE: We may not hold the GPU lock here.
            //
            if (serverGetClientUnderLock(&g_resServ,
                                         phClients[chanIdx],
                                         &pClient) == NV_OK &&
                gpuresGetByDeviceOrSubdeviceHandle(pClient,
                                                   phDevices[chanIdx],
                                                   &pGpuResource) == NV_OK &&
                gpuGetInstance(GPU_RES_GET_GPU(pGpuResource)) == gpuIdx)
            {
                pPerGpuClients[perGpuIdx]  = phClients[chanIdx];
                pPerGpuDevices[perGpuIdx]  = phDevices[chanIdx];
                pPerGpuChannels[perGpuIdx] = phChannels[chanIdx];
                perGpuIdx++;
            }
        }

        rmStatus = kfifoIdleChannelsPerDevice_HAL(pGpu, pKernelFifo, pPerGpuClients, pPerGpuDevices, pPerGpuChannels,
                                                  numChannelsPerGpu[gpuIdx], flags, timeout);
        portMemFree(pPerGpuClients);
        portMemFree(pPerGpuDevices);
        portMemFree(pPerGpuChannels);

        if (rmStatus != NV_OK)
        {
            goto done;
        }
    }

done:

    if (isGpuGrpLockAcquired)
    {
        //UNLOCK: release Device lock
        rmGpuGroupLockRelease(gpuLockMask, GPUS_LOCK_FLAGS_NONE);
    }

    // paramCopy structs not initialized for CHANNEL_SINGLE
    if (DRF_VAL(OS30, _FLAGS, _CHANNEL, flags) == NVOS30_FLAGS_CHANNEL_LIST)
    {
        // No need to copy these back out
        if (phClients != NULL)
        {
            paramCopyClients.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            (void) rmapiParamsRelease(&paramCopyClients);
        }

        if (phDevices != NULL)
        {
            paramCopyDevices.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            (void) rmapiParamsRelease(&paramCopyDevices);
        }

        if (phChannels != NULL)
        {
            paramCopyChannels.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            (void) rmapiParamsRelease(&paramCopyChannels);
        }
    }

    NV_PRINTF(LEVEL_INFO,
              "DONE. hChannel: 0x%x, numChannels: %u, rmStatus: 0x%x\n",
              hChannel, numChannels, rmStatus);

    return rmStatus;
}
