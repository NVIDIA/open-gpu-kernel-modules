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

void nvKmsKapiFreeNisoSurface(struct NvKmsKapiDevice *device,
                              struct NvKmsKapiNisoSurface *surface)
{
    if (surface->hKmsHandle != 0) {
        struct NvKmsUnregisterSurfaceParams paramsUnreg = { };
        NvBool status;

        paramsUnreg.request.deviceHandle  = device->hKmsDevice;
        paramsUnreg.request.surfaceHandle = surface->hKmsHandle;

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

    if (surface->mapped) {
        NV_STATUS status;

        status = nvRmApiUnmapMemory(device->hRmClient,
                                    device->hRmSubDevice,
                                    surface->hRmHandle,
                                    surface->pLinearAddress,
                                    0);

        if (status != NV_OK) {
            nvKmsKapiLogDeviceDebug(
                device,
                "UnmapMemory failed with error code 0x%08x",
                status);
        }

        device->notifier.mapped = NV_FALSE;
    }

    if (surface->hRmHandle != 0) {
        NvU32 status;

        status = nvRmApiFree(device->hRmClient,
                             device->hRmDevice,
                             surface->hRmHandle);

        if (status != NVOS_STATUS_SUCCESS) {
            nvKmsKapiLogDeviceDebug(
                device,
                "RmFree failed with error code 0x%08x",
                status);
        }

        nvFreeUnixRmHandle(&device->handleAllocator, surface->hRmHandle);
        surface->hRmHandle = 0;
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

static NvBool AllocateNisoSurface(struct NvKmsKapiDevice *device,
                                  struct NvKmsKapiNisoSurface *surface,
                                  NvU64 size,
                                  NvBool inVideoMemory)
{
    struct NvKmsRegisterSurfaceParams surfParams = {};
    NV_STATUS status = 0;
    NvU8 compressible = 0;
    NvBool ret;

    surface->hRmHandle =
        nvGenerateUnixRmHandle(&device->handleAllocator);

    if (surface->hRmHandle == 0x0) {
        nvKmsKapiLogDeviceDebug(
            device,
            "nvGenerateUnixRmHandle() failed");
        return NV_FALSE;
    }

    if (inVideoMemory) {
        ret = nvKmsKapiAllocateVideoMemory(device,
                                           surface->hRmHandle,
                                           NvKmsSurfaceMemoryLayoutPitch,
                                           size,
                                           NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER,
                                           &compressible);
    } else {
        ret = nvKmsKapiAllocateSystemMemory(device,
                                            surface->hRmHandle,
                                            NvKmsSurfaceMemoryLayoutPitch,
                                            size,
                                            NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER,
                                            &compressible);
    }

    if (!ret) {
        nvFreeUnixRmHandle(&device->handleAllocator, surface->hRmHandle);
        surface->hRmHandle = 0x0;
        goto failed;
    }

    status = nvRmApiMapMemory(device->hRmClient,
                              device->hRmSubDevice,
                              surface->hRmHandle,
                              0,
                              size,
                              &surface->pLinearAddress,
                              0);

    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(
            device,
            "MapMemory failed with error code 0x%08x",
            status);
        goto failed;
    }

    surface->mapped = NV_TRUE;

    surfParams.request.deviceHandle      = device->hKmsDevice;
    surfParams.request.useFd             = FALSE;
    surfParams.request.rmClient          = device->hRmClient;

    surfParams.request.widthInPixels     = size;
    surfParams.request.heightInPixels    = 1;
    surfParams.request.layout            = NvKmsSurfaceMemoryLayoutPitch;
    surfParams.request.format            = NvKmsSurfaceMemoryFormatI8;
    surfParams.request.log2GobsPerBlockY = 0;
    surfParams.request.isoType           = NVKMS_MEMORY_NISO;

    surfParams.request.planes[0].u.rmObject = surface->hRmHandle;
    surfParams.request.planes[0].pitch = size;
    surfParams.request.planes[0].rmObjectSizeInBytes = size;

    if (!nvkms_ioctl_from_kapi(device->pKmsOpen,
                               NVKMS_IOCTL_REGISTER_SURFACE,
                               &surfParams, sizeof(surfParams))) {
        nvKmsKapiLogDeviceDebug(
            device,
            "NVKMS_IOCTL_REGISTER_SURFACE failed");
        goto failed;
    }

    surface->hKmsHandle = surfParams.reply.surfaceHandle;

    return NV_TRUE;

failed:

    nvKmsKapiFreeNisoSurface(device, surface);

    return NV_FALSE;
}

#define NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE 0x1000

NvBool nvKmsKapiAllocateNotifiers(struct NvKmsKapiDevice *device,
                                  NvBool inVideoMemory)
{
    ct_assert((NVKMS_KAPI_MAX_NOTIFIERS * NVKMS_KAPI_NOTIFIER_SIZE) <=
              NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE);

    ct_assert(NVKMS_KAPI_NOTIFIER_SIZE >= sizeof(NvNotification));
    nvAssert(NVKMS_KAPI_NOTIFIER_SIZE >=
             nvKmsSizeOfNotifier(device->notifier.format, TRUE /* overlay */));
    nvAssert(NVKMS_KAPI_NOTIFIER_SIZE >=
             nvKmsSizeOfNotifier(device->notifier.format, FALSE /* overlay */));

    if (!AllocateNisoSurface(device,
                             &device->notifier,
                             NVKMS_KAPI_NOTIFIERS_SURFACE_SIZE,
                             inVideoMemory)) {
        return NV_FALSE;
    }

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
}

static void ResetSemaphore(struct NvKmsKapiDevice *device,
                          NvU32 index,
                          NvU32 payload)
{
    nvKmsResetSemaphore(device->semaphore.format,
                        index,
                        device->semaphore.pLinearAddress,
                        payload);
}

#define NVKMS_KAPI_SEMAPHORE_SURFACE_SIZE 0x1000

NvBool nvKmsKapiAllocateSemaphores(struct NvKmsKapiDevice *device,
                                   NvBool inVideoMemory)
{
    NvU32 index;

    if (!AllocateNisoSurface(device,
                             &device->semaphore,
                             NVKMS_KAPI_SEMAPHORE_SURFACE_SIZE,
                             inVideoMemory)) {
        return NV_FALSE;
    }

    /* Init Semaphores */

    device->numDisplaySemaphores = NVKMS_KAPI_SEMAPHORE_SURFACE_SIZE /
        nvKmsSizeOfSemaphore(device->semaphore.format);

    /*
     * See the comment in nvKmsKapiSignalDisplaySemaphore() for the full
     * justification of this requirement. The current implementation requires
     * only 16 semaphores (2 per window) given a maximum of one outstanding
     * flip, but this value allows up to 32 outstanding flips, as recommended
     * by the architecture team in an old hardware bug.
     */
    nvAssert(device->numDisplaySemaphores >= 256);

    for (index = 0; index < device->numDisplaySemaphores; index++) {
        ResetSemaphore(device, index, NVKMS_KAPI_SEMAPHORE_VALUE_DONE);
    }

    return NV_TRUE;
}

NvBool nvKmsKapiResetDisplaySemaphore(struct NvKmsKapiDevice *device,
                                      NvU32 index)
{
    struct nvKmsParsedSemaphore semParsed;

    nvKmsParseSemaphore(device->semaphore.format,
                        index,
                        device->semaphore.pLinearAddress,
                        &semParsed);

    if (semParsed.payload != NVKMS_KAPI_SEMAPHORE_VALUE_DONE) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Attempt to reuse semaphore at index %u with pending status 0x%08x",
            index,
            semParsed.payload);

        return NV_FALSE;
    }

    ResetSemaphore(device, index, NVKMS_KAPI_SEMAPHORE_VALUE_NOT_READY);

    return NV_TRUE;
}

void nvKmsKapiSignalDisplaySemaphore(struct NvKmsKapiDevice *device,
                                     NvU32 index)
{
    /*
     * Note most users of semaphores use a "ready" value that varies from
     * frame to frame, citing bug 194936. However, this "bug" simply
     * notes that the hardware may read ahead and grab semaphore values for
     * pending semaphore acquires such that two pending frames using the
     * same semaphore might be signaled "ready" by the same semaphore write.
     * Given this implementation cycles through at least 256 semaphores,
     * meaning if all 8 hardware windows were programmed in every flip, there
     * could be at least 32 frames in-flight without re-using a semaphore
     * slot, and the code above that initializes a semaphore for each frame
     * first ensures the prior frame using that semaphore has completed,
     * this approach is not necessary here. There should be no opportunity
     * for the hardware to "pre-fetch" a prior frame's semaphore acquire
     * value from the semaphore here, and hence a constant value is sufficient.
     */
    ResetSemaphore(device, index, NVKMS_KAPI_SEMAPHORE_VALUE_READY);
}

void nvKmsKapiCancelDisplaySemaphore(struct NvKmsKapiDevice *device,
                                     NvU32 index)
{
    struct nvKmsParsedSemaphore semParsed;

    nvKmsParseSemaphore(device->semaphore.format,
                        index,
                        device->semaphore.pLinearAddress,
                        &semParsed);

    if (semParsed.payload != NVKMS_KAPI_SEMAPHORE_VALUE_DONE) {
        nvAssert(semParsed.payload == NVKMS_KAPI_SEMAPHORE_VALUE_NOT_READY);
        ResetSemaphore(device, index, NVKMS_KAPI_SEMAPHORE_VALUE_DONE);
    }
}

NvU32 nvKmsKapiGetDisplaySemaphoreOffset(struct NvKmsKapiDevice *device,
                                         NvU32 index)
{
    return nvKmsSizeOfSemaphore(device->semaphore.format) * index;
}
