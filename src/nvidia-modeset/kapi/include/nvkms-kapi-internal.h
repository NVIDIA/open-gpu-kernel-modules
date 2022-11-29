/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_KAPI_INTERNAL_H__

#define __NVKMS_KAPI_INTERNAL_H__

#include "unix_rm_handle.h"

#include "nvkms-utils.h"
#include "nvkms-kapi-private.h"

//XXX Decouple functions like nvEvoLog used for logging from NVKMS

#define nvKmsKapiLogDebug(__format...) \
    nvEvoLogDebug(EVO_LOG_INFO, "[kapi] "__format)

#define nvKmsKapiLogDeviceDebug(__device, __format, ...)          \
    nvEvoLogDebug(EVO_LOG_INFO, "[kapi][GPU Id 0x%08x] "__format, \
                  device->gpuId, ##__VA_ARGS__)

struct NvKmsKapiDevice {

    NvU32 gpuId;

    nvkms_sema_handle_t *pSema;

    /* RM handles */

    NvU32 hRmClient;
    NvU32 hRmDevice, hRmSubDevice;
    NvU32 deviceInstance;

    NVUnixRmHandleAllocatorRec handleAllocator;

    /* NVKMS handles */

    struct nvkms_per_open *pKmsOpen;

    NvKmsDeviceHandle hKmsDevice;
    NvKmsDispHandle hKmsDisp;
    NvU32 dispIdx;

    NvU32 subDeviceMask;

    NvBool isSOC;
    NvKmsDispIOCoherencyModes isoIOCoherencyModes;
    NvKmsDispIOCoherencyModes nisoIOCoherencyModes;
    NvBool supportsSyncpts;

    /* Device capabilities */

    struct {
        struct NvKmsCompositionCapabilities cursorCompositionCaps;
        struct NvKmsCompositionCapabilities overlayCompositionCaps;

        NvU16 validLayerRRTransforms;

        NvU32 maxWidthInPixels;
        NvU32 maxHeightInPixels;
        NvU32 maxCursorSizeInPixels;

        NvU8  genericPageKind;
        NvBool requiresVrrSemaphores;
    } caps;

    NvU64 supportedSurfaceMemoryFormats[NVKMS_KAPI_LAYER_MAX];
    NvBool supportsHDR[NVKMS_KAPI_LAYER_MAX];

    NvU32 numHeads;
    NvU32 numLayers[NVKMS_KAPI_MAX_HEADS];

    struct {
        NvU32 hRmHandle;
        NvKmsSurfaceHandle hKmsHandle;

        NvBool mapped;
        void *pLinearAddress;

        enum NvKmsNIsoFormat format;
    } notifier;

    struct {
        NvU32 currFlipNotifierIndex;
    } layerState[NVKMS_KAPI_MAX_HEADS][NVKMS_MAX_LAYERS_PER_HEAD];

    void *privateData;

    void (*eventCallback)(const struct NvKmsKapiEvent *event);
};

struct NvKmsKapiMemory {
    NvU32 hRmHandle;
    NvU64 size;

    struct NvKmsKapiPrivSurfaceParams surfaceParams;
};

struct NvKmsKapiSurface {
    NvKmsSurfaceHandle hKmsHandle;
};

static inline void *nvKmsKapiCalloc(size_t nmem, size_t size)
{
    return nvInternalAlloc(nmem * size, NV_TRUE);
}

static inline void nvKmsKapiFree(void *ptr)
{
    return nvInternalFree(ptr);
}

static inline NvU32 nvKmsKapiGenerateRmHandle(struct NvKmsKapiDevice *device)
{
    NvU32 handle;

    nvkms_sema_down(device->pSema);
    handle = nvGenerateUnixRmHandle(&device->handleAllocator);
    nvkms_sema_up(device->pSema);

    return handle;
}

static inline void nvKmsKapiFreeRmHandle(struct NvKmsKapiDevice *device,
                                         NvU32 handle)
{
    nvkms_sema_down(device->pSema);
    nvFreeUnixRmHandle(&device->handleAllocator, handle);
    nvkms_sema_up(device->pSema);
}

NvBool nvKmsKapiAllocateVideoMemory(struct NvKmsKapiDevice *device,
                                    NvU32 hRmHandle,
                                    enum NvKmsSurfaceMemoryLayout layout,
                                    NvU64 size,
                                    enum NvKmsKapiAllocationType type,
                                    NvU8 *compressible);

NvBool nvKmsKapiAllocateSystemMemory(struct NvKmsKapiDevice *device,
                                     NvU32 hRmHandle,
                                     enum NvKmsSurfaceMemoryLayout layout,
                                     NvU64 size,
                                     enum NvKmsKapiAllocationType type,
                                     NvU8 *compressible);

struct NvKmsKapiChannelEvent*
nvKmsKapiAllocateChannelEvent(struct NvKmsKapiDevice *device,
                              NvKmsChannelEventProc *proc,
                              void *data,
                              NvU64 nvKmsParamsUser,
                              NvU64 nvKmsParamsSize);

void
nvKmsKapiFreeChannelEvent(struct NvKmsKapiDevice *device,
                          struct NvKmsKapiChannelEvent *cb);

#endif /* __NVKMS_KAPI_INTERNAL_H__ */
