/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvidia-modeset-os-interface.h"

#include "nvkms-rmapi.h"

#include "nvkms-kapi.h"
#include "nvkms-kapi-private.h"
#include "nvkms-kapi-internal.h"

#include "class/cl0005.h"

struct NvKmsKapiChannelEvent {
    struct NvKmsKapiDevice *device;

    NvKmsChannelEventProc *proc;
    void *data;

    struct NvKmsKapiPrivAllocateChannelEventParams nvKmsParams;

    NvHandle hCallbacks[NVKMS_KAPI_MAX_EVENT_CHANNELS];
    NVOS10_EVENT_KERNEL_CALLBACK_EX rmCallback;
};

static void ChannelEventHandler(void *arg1, void *arg2, NvHandle hEvent,
                                 NvU32 data, NvU32 status)
{
    struct NvKmsKapiChannelEvent *cb = arg1;
    cb->proc(cb->data, 0);
}

void nvKmsKapiFreeChannelEvent
(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiChannelEvent *cb
)
{
    int i;

    if (device == NULL || cb == NULL) {
        return;
    }

    for (i = 0; i < NVKMS_KAPI_MAX_EVENT_CHANNELS; ++i) {
        if (!cb->hCallbacks[i]) {
            continue;
        }

        nvRmApiFree(device->hRmClient,
                    device->hRmClient,
                    cb->hCallbacks[i]);

        nvFreeUnixRmHandle(&device->handleAllocator,
                           cb->hCallbacks[i]);
    }

    nvKmsKapiFree(cb);
}

struct NvKmsKapiChannelEvent* nvKmsKapiAllocateChannelEvent
(
    struct NvKmsKapiDevice *device,
    NvKmsChannelEventProc *proc,
    void *data,
    NvU64 nvKmsParamsUser,
    NvU64 nvKmsParamsSize
)
{
    int status, i;
    struct NvKmsKapiChannelEvent *cb = NULL;

    if (device == NULL || proc == NULL) {
        goto fail;
    }

    cb = nvKmsKapiCalloc(1, sizeof(*cb));
    if (cb == NULL) {
        goto fail;
    }

    /* Verify the driver-private params size and copy it in from userspace */

    if (nvKmsParamsSize != sizeof(cb->nvKmsParams)) {
        nvKmsKapiLogDebug(
            "NVKMS private memory import parameter size mismatch - "
            "expected: 0x%llx, caller specified: 0x%llx",
            (NvU64)sizeof(cb->nvKmsParams), nvKmsParamsSize);
        goto fail;
    }

    status = nvkms_copyin(&cb->nvKmsParams,
                          nvKmsParamsUser, sizeof(cb->nvKmsParams));
    if (status != 0) {
        nvKmsKapiLogDebug(
            "NVKMS private memory import parameters could not be read from "
            "userspace");
        goto fail;
    }

    cb->device = device;

    cb->proc = proc;
    cb->data = data;

    cb->rmCallback.func = ChannelEventHandler;
    cb->rmCallback.arg = cb;

    for (i = 0; i < NVKMS_KAPI_MAX_EVENT_CHANNELS; ++i) {
        NV0005_ALLOC_PARAMETERS eventParams = { };
        NvU32 ret;

        if (!cb->nvKmsParams.hChannels[i]) {
            continue;
        }

        cb->hCallbacks[i] = nvGenerateUnixRmHandle(&device->handleAllocator);
        if (cb->hCallbacks[i] == 0x0) {
            nvKmsKapiLogDeviceDebug(device,
                                    "Failed to allocate event callback handle for channel 0x%x",
                                    cb->nvKmsParams.hChannels[i]);
            goto fail;
        }

        eventParams.hParentClient = cb->nvKmsParams.hClient;
        eventParams.hClass = NV01_EVENT_KERNEL_CALLBACK_EX;
        eventParams.notifyIndex = 0;
        eventParams.data = NV_PTR_TO_NvP64(&cb->rmCallback);

        ret = nvRmApiAlloc(device->hRmClient,
                           cb->nvKmsParams.hChannels[i],
                           cb->hCallbacks[i],
                           NV01_EVENT_KERNEL_CALLBACK_EX,
                           &eventParams);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvKmsKapiLogDeviceDebug(device,
                                    "Failed to allocate event callback for channel 0x%x",
                                    cb->nvKmsParams.hChannels[i]);
            nvFreeUnixRmHandle(&device->handleAllocator, cb->hCallbacks[i]);
            cb->hCallbacks[i] = 0;
            goto fail;
        }
    }

    return cb;

fail:
    nvKmsKapiFreeChannelEvent(device, cb);
    return NULL;
}
