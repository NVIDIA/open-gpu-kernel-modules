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

#include "class/cl0000.h"
#include "class/cl0005.h"
#include "ctrl/ctrl00da.h"

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

struct NvKmsKapiSemaphoreSurface {
    NvHandle hSemaphoreSurface;

    NvHandle hSemaphoreMem;
    NvHandle hMaxSubmittedMem;
};

struct NvKmsKapiSemaphoreSurface*
nvKmsKapiImportSemaphoreSurface
(
    struct NvKmsKapiDevice *device,
    NvU64 nvKmsParamsUser,
    NvU64 nvKmsParamsSize,
    void **pSemaphoreMap,
    void **pMaxSubmittedMap
)
{
    struct NvKmsKapiSemaphoreSurface *ss = NULL;
    struct NvKmsKapiPrivImportSemaphoreSurfaceParams p, *pHeap;
    NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS refParams = {};
    NvU32 ret;
    int status;

    /* Verify the driver-private params size and copy it in from userspace */

    if (nvKmsParamsSize != sizeof(p)) {
        nvKmsKapiLogDebug(
            "NVKMS semaphore surface import parameter size mismatch - expected: 0x%llx, caller specified: 0x%llx",
            (NvU64)sizeof(p), nvKmsParamsSize);
        goto fail;
    }

    /*
     * Use a heap allocation as the destination pointer passed to
     * nvkms_copyin; stack allocations created within core NVKMS may not
     * be recognizable to the Linux kernel's CONFIG_HARDENED_USERCOPY
     * checker, triggering false errors.  But then save the result to a
     * variable on the stack, so that we can free the heap memory
     * immediately and not worry about its lifetime.
     */

    pHeap = nvKmsKapiCalloc(1, sizeof(*pHeap));
    if (pHeap == NULL) {
        nvKmsKapiLogDebug(
            "NVKMS failed to allocate semaphore surface parameter struct of size: %ld",
            (long)sizeof(*pHeap));
        goto fail;
    }

    status = nvkms_copyin(pHeap, nvKmsParamsUser, sizeof(*pHeap));

    p = *pHeap;
    nvKmsKapiFree(pHeap);

    if (status != 0) {
        nvKmsKapiLogDebug(
            "NVKMS semaphore surface import parameters could not be read from userspace");
        goto fail;
    }

    ss = nvKmsKapiCalloc(1, sizeof(*ss));
    if (ss == NULL) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to allocate memory for NVKMS semaphore surface while importing (0x%08x, 0x%08x)",
            p.hClient, p.hSemaphoreSurface);
        goto fail;
    }

    ret = nvRmApiDupObject2(device->hRmClient,
                            device->hRmSubDevice,
                            &ss->hSemaphoreSurface,
                            p.hClient,
                            p.hSemaphoreSurface,
                            0);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to dup RM semaphore surface object (0x%08x, 0x%08x)",
            p.hClient, p.hSemaphoreSurface);
        goto fail;
    }

    ret = nvRmApiControl(device->hRmClient,
                         ss->hSemaphoreSurface,
                         NV_SEMAPHORE_SURFACE_CTRL_CMD_REF_MEMORY,
                         &refParams,
                         sizeof(refParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to ref RM semaphore surface memory objects (0x%08x, 0x%08x)",
            p.hClient, p.hSemaphoreSurface);
        goto fail;
    }

    ss->hSemaphoreMem = refParams.hSemaphoreMem;
    ss->hMaxSubmittedMem = refParams.hMaxSubmittedMem;

    ret = nvRmApiMapMemory(device->hRmClient,
                           device->hRmDevice,
                           ss->hSemaphoreMem,
                           0,
                           p.semaphoreSurfaceSize,
                           pSemaphoreMap,
                           0);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to map RM semaphore surface semaphore memory (0x%08x, 0x%08x)",
            p.hClient, p.hSemaphoreSurface);
        goto fail;
    }

    if (ss->hMaxSubmittedMem != NV01_NULL_OBJECT) {
        if (ss->hMaxSubmittedMem != ss->hSemaphoreMem) {
            ret = nvRmApiMapMemory(device->hRmClient,
                                   device->hRmDevice,
                                   ss->hMaxSubmittedMem,
                                   0,
                                   p.semaphoreSurfaceSize,
                                   pMaxSubmittedMap,
                                   0);
            if (ret != NVOS_STATUS_SUCCESS) {
                nvKmsKapiLogDeviceDebug(
                    device,
                    "Failed to map RM semaphore surface max submitted memory (0x%08x, 0x%08x)",
                    p.hClient, p.hSemaphoreSurface);
                goto fail;
            }
        } else {
            *pMaxSubmittedMap = *pSemaphoreMap;
        }
    } else {
        *pMaxSubmittedMap = NULL;
    }

    return ss;

fail:
    if (ss && ss->hSemaphoreSurface) {
        if ((ss->hMaxSubmittedMem != NV01_NULL_OBJECT) &&
            (ss->hMaxSubmittedMem != ss->hSemaphoreMem)) {
            nvRmApiFree(device->hRmClient,
                        device->hRmDevice,
                        ss->hMaxSubmittedMem);
        }

        if (ss->hSemaphoreMem != NV01_NULL_OBJECT) {
            nvRmApiFree(device->hRmClient,
                        device->hRmDevice,
                        ss->hSemaphoreMem);
        }

        nvRmApiFree(device->hRmClient,
                    device->hRmDevice,
                    ss->hSemaphoreSurface);
    }
    nvKmsKapiFree(ss);
    return NULL;
}

void nvKmsKapiFreeSemaphoreSurface
(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *ss
)
{
    if (device == NULL || ss == NULL) {
        return;
    }

    if ((ss->hMaxSubmittedMem != NV01_NULL_OBJECT) &&
        (ss->hMaxSubmittedMem != ss->hSemaphoreMem)) {
        nvRmApiFree(device->hRmClient,
                    device->hRmDevice,
                    ss->hMaxSubmittedMem);
    }

    nvRmApiFree(device->hRmClient,
                device->hRmDevice,
                ss->hSemaphoreMem);

    nvRmApiFree(device->hRmClient,
                device->hRmSubDevice,
                ss->hSemaphoreSurface);

    nvKmsKapiFree(ss);
}

struct NvKmsKapiSemaphoreSurfaceCallback {
    NvKmsSemaphoreSurfaceCallbackProc *pCallback;
    void *pData;

    NVOS10_EVENT_KERNEL_CALLBACK_EX rmCallback;
};

static void SemaphoreSurfaceKapiCallback(void *arg1, void *arg2, NvHandle hEvent,
                                         NvU32 data, NvU32 status)
{
    struct NvKmsKapiSemaphoreSurfaceCallback *cb = arg1;
    cb->pCallback(cb->pData);
    nvKmsKapiFree(cb);
}

NvKmsKapiRegisterWaiterResult
nvKmsKapiRegisterSemaphoreSurfaceCallback(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvKmsSemaphoreSurfaceCallbackProc *pCallback,
    void *pData,
    NvU64 index,
    NvU64 wait_value,
    NvU64 new_value,
    struct NvKmsKapiSemaphoreSurfaceCallback **pCallbackHandle)
{
    NvU32 ret;

    struct NvKmsKapiSemaphoreSurfaceCallback *cb = NULL;
    NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS waiterParams = { };

    if (device == NULL) {
        nvKmsKapiLogDebug(
            "Invalid device while registering semaphore surface callback");
        goto fail;
    }

    if ((semaphoreSurface == NULL) ||
        ((pCallback == NULL) && (new_value == 0))) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Invalid parameter while registering semaphore surface callback");
        goto fail;
    }

    waiterParams.index = index;
    waiterParams.waitValue = wait_value;
    waiterParams.newValue = new_value;

    if (pCallback) {
        cb = nvKmsKapiCalloc(1, sizeof(*cb));
        if (cb == NULL) {
            nvKmsKapiLogDeviceDebug(
                device,
                "Failed to allocate memory for semaphore surface (0x%08x, 0x%08x) callback on index %" NvU64_fmtu " for value %" NvU64_fmtx,
                device->hRmClient, semaphoreSurface->hSemaphoreSurface,
                index, wait_value);
            goto fail;
        }

        cb->pCallback = pCallback;
        cb->pData = pData;
        cb->rmCallback.func = SemaphoreSurfaceKapiCallback;
        cb->rmCallback.arg = cb;

        waiterParams.notificationHandle = (NvUPtr)&cb->rmCallback;
    }

    ret = nvRmApiControl(device->hRmClient,
                         semaphoreSurface->hSemaphoreSurface,
                         NV_SEMAPHORE_SURFACE_CTRL_CMD_REGISTER_WAITER,
                         &waiterParams,
                         sizeof(waiterParams));

    switch (ret) {
    case NVOS_STATUS_SUCCESS:
        if (pCallback) {
            *pCallbackHandle = cb;
        }
        return NVKMS_KAPI_REG_WAITER_SUCCESS;
    case NVOS_STATUS_ERROR_ALREADY_SIGNALLED:
        return NVKMS_KAPI_REG_WAITER_ALREADY_SIGNALLED;
    default:
        break;
    }

fail:
    nvKmsKapiFree(cb);
    return NVKMS_KAPI_REG_WAITER_FAILED;
}

NvBool
nvKmsKapiUnregisterSemaphoreSurfaceCallback(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvU64 index,
    NvU64 wait_value,
    struct NvKmsKapiSemaphoreSurfaceCallback *callbackHandle)
{
    NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS waiterParams = { };
    NvU32 ret;


    if (device == NULL) {
        nvKmsKapiLogDebug(
            "Invalid device while unregistering semaphore surface callback");
        return NV_FALSE;
    }

    if ((semaphoreSurface == NULL) || (callbackHandle == NULL) ||
        (wait_value == 0)) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Invalid parameter while unregistering semaphore surface callback");
        return NV_FALSE;
    }

    /*
     * Note this function does not actually dereference callbackHandle before
     * making the RM control call. This is important, as there may exist a race
     * such that the client is calling this function while another thread is
     * running the callback and freeing its handle.
     *
     * The existance of this race seems to imply there is an additional hazard
     * where a new callback may be registered against the same wait value during
     * the first race, which this call would then mistakenly delete. That is
     * impossible because the RM semaphore surface code would detect that such a
     * waiter is already signaled and return without adding it to the waiter
     * list.
     */
    waiterParams.index = index;
    waiterParams.waitValue = wait_value;

    /*
     * Manually perform the equivalent of &callbackHandle->rmCallback, but with
     * semantics that make it clearer there is no access of the memory pointed
     * to by callbackHandle.
     */
    waiterParams.notificationHandle = (NvUPtr)callbackHandle +
        offsetof(struct NvKmsKapiSemaphoreSurfaceCallback, rmCallback);

    ret = nvRmApiControl(device->hRmClient,
                         semaphoreSurface->hSemaphoreSurface,
                         NV_SEMAPHORE_SURFACE_CTRL_CMD_UNREGISTER_WAITER,
                         &waiterParams,
                         sizeof(waiterParams));

    switch (ret) {
    case NVOS_STATUS_SUCCESS:
        /*
         * The callback was successfully unregistered, and will never run. Free
         * its associated data.
         */
        nvKmsKapiFree(callbackHandle);
        return NV_TRUE;

    default:
        /*
         * This code must assume failure to unregister for any reason indicates
         * the callback is being run right now, or is on a list of pending
         * callbacks which will be run in finite time. Do not free its data.
         */
        return NV_FALSE;
    }
}

NvBool
nvKmsKapiSetSemaphoreSurfaceValue(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvU64 index,
    NvU64 new_value)
{
    NvU32 ret;

    NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS setParams = { };

    if (device == NULL) {
        nvKmsKapiLogDebug(
            "Invalid device used to set semaphore surface value");
        return NV_FALSE;
    }

    if (semaphoreSurface == NULL) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Attempt to set value on Invalid semaphore surface");
        return NV_FALSE;
    }

    setParams.index = index;
    setParams.newValue = new_value;

    ret = nvRmApiControl(device->hRmClient,
                         semaphoreSurface->hSemaphoreSurface,
                         NV_SEMAPHORE_SURFACE_CTRL_CMD_SET_VALUE,
                         &setParams,
                         sizeof(setParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return NV_FALSE;
    }

    return NV_TRUE;
}
