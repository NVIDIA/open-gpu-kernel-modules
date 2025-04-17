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

#include "nv_smg.h"

//XXX Decouple functions like nvEvoLog used for logging from NVKMS

#define nvKmsKapiLogDebug(__format...) \
    nvEvoLogDebug(EVO_LOG_INFO, "[kapi] "__format)

#define nvKmsKapiLogDeviceDebug(__device, __format, ...)          \
    nvEvoLogDebug(EVO_LOG_INFO, "[kapi][GPU Id 0x%08x] "__format, \
                  device->gpuId, ##__VA_ARGS__)

/*
 * Semaphore values used when using semaphore-based synchronization between
 * userspace rendering and flips.
 */
enum NvKmsKapiSemaphoreValues {
    /*
     * Initial state on driver init, and the value written by the hardware when
     * it has completed processing of a frame using this semaphore.
     */
    NVKMS_KAPI_SEMAPHORE_VALUE_DONE = 0xd00dd00d,

    /*
     * Value of the semaphore when a flip is pending in the display pushbuffer,
     * but userspace rendering is not yet complete.
     */
    NVKMS_KAPI_SEMAPHORE_VALUE_NOT_READY = 0x13371337,

    /*
     * Value of the semaphore when userspace rendering is complete and the
     * pending flip may proceed.
     */
    NVKMS_KAPI_SEMAPHORE_VALUE_READY = 0xf473f473,
};

struct NvKmsKapiNisoSurface {
    NvU32 hRmHandle;
    NvKmsSurfaceHandle hKmsHandle;

    NvBool mapped;
    void *pLinearAddress;

    enum NvKmsNIsoFormat format;

};

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

    /* SMG state */

    MIGDeviceId migDevice;
    NvU32 smgGpuInstSubscriptionHdl;
    NvU32 smgComputeInstSubscriptionHdl;

    nvRMContext rmSmgContext;

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

        NvBool supportsInputColorSpace;
        NvBool supportsInputColorRange;
    } caps;

    NvU64 supportedSurfaceMemoryFormats[NVKMS_KAPI_LAYER_MAX];
    NvBool supportsICtCp[NVKMS_KAPI_LAYER_MAX];

    struct NvKmsKapiLutCaps lutCaps;

    NvU32 numHeads;
    NvU32 numLayers[NVKMS_KAPI_MAX_HEADS];

    struct NvKmsKapiNisoSurface notifier;
    struct NvKmsKapiNisoSurface semaphore;

    NvU32 numDisplaySemaphores;

    struct {
        NvU32 currFlipNotifierIndex;
    } layerState[NVKMS_KAPI_MAX_HEADS][NVKMS_MAX_LAYERS_PER_HEAD];

    void *privateData;

    void (*eventCallback)(const struct NvKmsKapiEvent *event);

    NvU64 vtFbBaseAddress;
    NvU64 vtFbSize;
};

struct NvKmsKapiMemory {
    NvU32 hRmHandle;
    NvU64 size;

    struct NvKmsKapiPrivSurfaceParams surfaceParams;

    NvBool isVidmem;
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

struct NvKmsKapiSemaphoreSurface*
nvKmsKapiImportSemaphoreSurface(struct NvKmsKapiDevice *device,
                                NvU64 nvKmsParamsUser,
                                NvU64 nvKmsParamsSize,
                                void **pSemaphoreMap,
                                void **pMaxSubmittedMap);

void
nvKmsKapiFreeSemaphoreSurface(struct NvKmsKapiDevice *device,
                              struct NvKmsKapiSemaphoreSurface *ss);

NvKmsKapiRegisterWaiterResult
nvKmsKapiRegisterSemaphoreSurfaceCallback(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvKmsSemaphoreSurfaceCallbackProc *pCallback,
    void *pData,
    NvU64 index,
    NvU64 wait_value,
    NvU64 new_value,
    struct NvKmsKapiSemaphoreSurfaceCallback **pCallbackHandle);

NvBool
nvKmsKapiUnregisterSemaphoreSurfaceCallback(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvU64 index,
    NvU64 wait_value,
    struct NvKmsKapiSemaphoreSurfaceCallback *callbackHandle);

NvBool
nvKmsKapiSetSemaphoreSurfaceValue(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
    NvU64 index,
    NvU64 new_value);

#endif /* __NVKMS_KAPI_INTERNAL_H__ */
