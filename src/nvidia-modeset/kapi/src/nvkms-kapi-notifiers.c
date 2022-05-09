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

#include "nvkms-api.h"
#include "nvkms-sync.h"
#include "nvkms-rmapi.h"
#include "nvkms-kapi-notifiers.h"

#define NVKMS_KAPI_MAX_NOTIFIERS        \
    (NVKMS_KAPI_MAX_HEADS *             \
     NVKMS_MAX_LAYERS_PER_HEAD *        \
     NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER)

void nvKmsKapiFreeNotifiers(struct NvKmsKapiDevice *device)
{
    if (device->notifier.hKmsHandle != 0) {
        struct NvKmsUnregisterSurfaceParams paramsUnreg = { };
        NvBool status;

        paramsUnreg.request.deviceHandle  = device->hKmsDevice;
        paramsUnreg.request.surfaceHandle = device->notifier.hKmsHandle;

        status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                       NVKMS_IOCTL_UNREGISTER_SURFACE,
                                       &paramsUnreg, sizeof(paramsUnreg));

        if (!status) {
            nvKmsKapiLogDeviceDebug(
                device,
                "NVKMS_IOCTL_UNREGISTER_SURFACE failed");
        }

        device->notifier.hKmsHandle = 0;
    }

    if (device->notifier.mapped) {
        NV_STATUS status;

        status = nvRmApiUnmapMemory(device->hRmClient,
                                    device->hRmSubDevice,
                                    device->notifier.hRmHandle,
                                    device->notifier.pLinearAddress,
                                    0);

        if (status != NV_OK) {
            nvKmsKapiLogDeviceDebug(
                device,
                "UnmapMemory failed with error code 0x%08x",
                status);
        }

        device->notifier.mapped = NV_FALSE;
    }

    if (device->notifier.hRmHandle != 0) {
        NvU32 status;

        status = nvRmApiFree(device->hRmClient,
                             device->hRmDevice,
                             device->notifier.hRmHandle);

        if (status != NVOS_STATUS_SUCCESS) {
            nvKmsKapiLogDeviceDebug(
                device,
                "RmFree failed with error code 0x%08x",
                status);
        }

        nvFreeUnixRmHandle(&device->handleAllocator, device->notifier.hRmHandle);
        device->notifier.hRmHandle = 0;
    }
}

static void InitNotifier(struct NvKmsKapiDevice *device,
                         NvU32 head, NvU32 layer, NvU32 index)
{
    nvKmsResetNotifier(device->notifier.format,
                       (layer == NVKMS_OVERLAY_LAYER),
                       NVKMS_KAPI_NOTIFIER_INDEX(head, layer, index),
                       device->notifier.pLinearAddress);
}

#define NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE 0x1000

NvBool nvKmsKapiAllocateNotifiers(struct NvKmsKapiDevice *device,
                                  NvBool inVideoMemory)
{
    struct NvKmsRegisterSurfaceParams surfParams = {};
    NV_STATUS status = 0;
    NvU8 compressible = 0;
    NvBool ret;

    ct_assert((NVKMS_KAPI_MAX_NOTIFIERS * NVKMS_KAPI_NOTIFIER_SIZE) <=
              (NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE));

    ct_assert(NVKMS_KAPI_NOTIFIER_SIZE >= sizeof(NvNotification));
    nvAssert(NVKMS_KAPI_NOTIFIER_SIZE >=
             nvKmsSizeOfNotifier(device->notifier.format, TRUE /* overlay */));
    nvAssert(NVKMS_KAPI_NOTIFIER_SIZE >=
             nvKmsSizeOfNotifier(device->notifier.format, FALSE /* overlay */));

    device->notifier.hRmHandle =
        nvGenerateUnixRmHandle(&device->handleAllocator);

    if (device->notifier.hRmHandle == 0x0) {
        nvKmsKapiLogDeviceDebug(
            device,
            "nvGenerateUnixRmHandle() failed");
        return NV_FALSE;
    }

    if (inVideoMemory) {
        ret = nvKmsKapiAllocateVideoMemory(device,
                                           device->notifier.hRmHandle,
                                           NvKmsSurfaceMemoryLayoutPitch,
                                           NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE,
                                           NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER,
                                           &compressible);
    } else {
        ret = nvKmsKapiAllocateSystemMemory(device,
                                            device->notifier.hRmHandle,
                                           NvKmsSurfaceMemoryLayoutPitch,
                                            NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE,
                                            NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER,
                                            &compressible);
    }

    if (!ret) {
        nvFreeUnixRmHandle(&device->handleAllocator, device->notifier.hRmHandle);
        device->notifier.hRmHandle = 0x0;
        goto failed;
    }

    status = nvRmApiMapMemory(device->hRmClient,
                              device->hRmSubDevice,
                              device->notifier.hRmHandle,
                              0,
                              NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE,
                              &device->notifier.pLinearAddress,
                              0);

    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(
            device,
            "MapMemory failed with error code 0x%08x",
            status);
        goto failed;
    }

    device->notifier.mapped = NV_TRUE;

    surfParams.request.deviceHandle      = device->hKmsDevice;
    surfParams.request.useFd             = FALSE;
    surfParams.request.rmClient          = device->hRmClient;

    surfParams.request.widthInPixels     = NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE;
    surfParams.request.heightInPixels    = 1;
    surfParams.request.layout            = NvKmsSurfaceMemoryLayoutPitch;
    surfParams.request.format            = NvKmsSurfaceMemoryFormatI8;
    surfParams.request.log2GobsPerBlockY = 0;
    surfParams.request.isoType           = NVKMS_MEMORY_NISO;

    surfParams.request.planes[0].u.rmObject = device->notifier.hRmHandle;
    surfParams.request.planes[0].pitch = NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE;
    surfParams.request.planes[0].rmObjectSizeInBytes =
        NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE;

    if (!nvkms_ioctl_from_kapi(device->pKmsOpen,
                               NVKMS_IOCTL_REGISTER_SURFACE,
                               &surfParams, sizeof(surfParams))) {
        nvKmsKapiLogDeviceDebug(
            device,
            "NVKMS_IOCTL_REGISTER_SURFACE failed");
        goto failed;
    }

    device->notifier.hKmsHandle = surfParams.reply.surfaceHandle;

    /* Init Notifiers */

    {
        NvU32 head;

        for (head = 0; head < device->numHeads; head++) {
            NvU32 layer;

            for (layer = 0; layer < NVKMS_MAX_LAYERS_PER_HEAD; layer++) {
                NvU32 index;

                for (index = 0;
                     index < NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER; index++) {
                    InitNotifier(device, head, layer, index);
                }
            }
        }
    }

    return NV_TRUE;

failed:

    nvKmsKapiFreeNotifiers(device);

    return NV_FALSE;
}
