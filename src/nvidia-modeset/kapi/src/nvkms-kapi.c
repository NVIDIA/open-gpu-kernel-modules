/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvUnixVersion.h"

#include "nvidia-modeset-os-interface.h"

#include "nvkms-api.h"
#include "nvkms-rmapi.h"
#include "nvkms-vrr.h"

#include "nvkms-kapi.h"
#include "nvkms-kapi-private.h"
#include "nvkms-kapi-internal.h"
#include "nvkms-kapi-notifiers.h"

#include <class/cl0000.h> /* NV01_ROOT/NV01_NULL_OBJECT */
#include <class/cl003e.h> /* NV01_MEMORY_SYSTEM */
#include <class/cl0080.h> /* NV01_DEVICE */
#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */
#include <class/cl0071.h> /* NV01_MEMORY_SYSTEM_OS_DESCRIPTOR */
#include <class/cl2080.h> /* NV20_SUBDEVICE_0 */

#include <ctrl/ctrl0000/ctrl0000gpu.h> /* NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2 */
#include <ctrl/ctrl0000/ctrl0000unix.h> /* NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD */
#include <ctrl/ctrl0000/ctrl0000client.h> /* NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM */
#include <ctrl/ctrl0080/ctrl0080gpu.h> /* NV0080_CTRL_CMD_GPU_GET_NUM_SUBDEVICES */
#include <ctrl/ctrl0080/ctrl0080fb.h> /* NV0080_CTRL_CMD_FB_GET_CAPS_V2 */
#include <ctrl/ctrl2080/ctrl2080unix.h> /* NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT */

#include "ctrl/ctrl003e.h" /* NV003E_CTRL_CMD_GET_SURFACE_PHYS_PAGES */
#include "ctrl/ctrl0041.h" /* NV0041_CTRL_SURFACE_INFO */


ct_assert(NVKMS_KAPI_LAYER_PRIMARY_IDX == NVKMS_MAIN_LAYER);
ct_assert(NVKMS_KAPI_LAYER_MAX == NVKMS_MAX_LAYERS_PER_HEAD);

/* XXX Move to NVKMS */
#define NV_EVO_PITCH_ALIGNMENT 0x100

#define NVKMS_KAPI_SUPPORTED_EVENTS_MASK             \
    ((1 << NVKMS_EVENT_TYPE_DPY_CHANGED) |           \
     (1 << NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED) | \
     (1 << NVKMS_EVENT_TYPE_FLIP_OCCURRED))

static NvU32 EnumerateGpus(nv_gpu_info_t *gpuInfo)
{
    return nvkms_enumerate_gpus(gpuInfo);
}

/*
 * Helper function to free RM objects allocated for NvKmsKapiDevice.
 */
static void RmFreeDevice(struct NvKmsKapiDevice *device)
{
    if (device->hRmSubDevice != 0x0) {
        nvRmApiFree(device->hRmClient,
                    device->hRmDevice,
                    device->hRmSubDevice);
        nvKmsKapiFreeRmHandle(device, device->hRmSubDevice);
        device->hRmSubDevice = 0x0;
    }

    /* Free RM device object */

    if (device->hRmDevice != 0x0) {
        nvRmApiFree(device->hRmClient,
                    device->hRmClient,
                    device->hRmDevice);
        nvKmsKapiFreeRmHandle(device, device->hRmDevice);

        device->hRmDevice = 0x0;
    }

    nvTearDownUnixRmHandleAllocator(&device->handleAllocator);

    device->deviceInstance = 0;

    /* Free RM client */

    if (device->hRmClient != 0x0) {
        nvRmApiFree(device->hRmClient,
                    device->hRmClient,
                    device->hRmClient);

        device->hRmClient = 0x0;
    }
}

/*
 * Helper function to allocate RM objects for NvKmsKapiDevice.
 */
static NvBool RmAllocateDevice(struct NvKmsKapiDevice *device)
{
    NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS getNumSubDevicesParams = { 0 };
    NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS idInfoParams = { };
    NV2080_ALLOC_PARAMETERS subdevAllocParams = { 0 };
    NV0080_ALLOC_PARAMETERS allocParams = { };
    NV0080_CTRL_FB_GET_CAPS_V2_PARAMS fbCapsParams = { 0 };

    NvU32 hRmDevice, hRmSubDevice;
    NvBool supportsGenericPageKind;
    NvU32 ret;

    /* Allocate RM client */

    ret = nvRmApiAlloc(NV01_NULL_OBJECT,
                       NV01_NULL_OBJECT,
                       NV01_NULL_OBJECT,
                       NV01_ROOT,
                       &device->hRmClient);

    if (ret != NVOS_STATUS_SUCCESS || device->hRmClient == 0x0) {
        nvKmsKapiLogDeviceDebug(device, "Failed to allocate RM client");
        goto failed;
    }

    /* Query device instance */

    idInfoParams.gpuId = device->gpuId;

    ret = nvRmApiControl(device->hRmClient,
                         device->hRmClient,
                         NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2,
                         &idInfoParams,
                         sizeof(idInfoParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(device, "Failed to query device instance");
        goto failed;
    }

    device->deviceInstance = idInfoParams.deviceInstance;
    device->isSOC =
        FLD_TEST_DRF(0000, _CTRL_GPU_ID_INFO, _SOC, _TRUE,
                     idInfoParams.gpuFlags);

    /* Initialize RM handle allocator */

    if (!nvInitUnixRmHandleAllocator(&device->handleAllocator,
                                     device->hRmClient,
                                     device->deviceInstance + 1)) {
        nvKmsKapiLogDeviceDebug(device, "Failed to initialize RM handle allocator");
        goto failed;
    }

    /* Allocate RM device object */

    hRmDevice = nvKmsKapiGenerateRmHandle(device);

    if (hRmDevice == 0x0) {
        nvKmsKapiLogDeviceDebug(device, "Failed to allocate RM handle");
        goto failed;
    }

    allocParams.deviceId = device->deviceInstance;

    allocParams.hClientShare = device->hRmClient;

    ret = nvRmApiAlloc(device->hRmClient,
                       device->hRmClient,
                       hRmDevice,
                       NV01_DEVICE_0,
                       &allocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(device, "Failed to allocate RM device object");
        nvKmsKapiFreeRmHandle(device, hRmDevice);
        goto failed;
    }

    device->hRmDevice = hRmDevice;

    ret = nvRmApiControl(device->hRmClient,
                         device->hRmDevice,
                         NV0080_CTRL_CMD_GPU_GET_NUM_SUBDEVICES,
                         &getNumSubDevicesParams,
                         sizeof(getNumSubDevicesParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(device, "Failed to determine number of GPUs");
        goto failed;
    }

    if (getNumSubDevicesParams.numSubDevices != 1) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Unsupported number of GPUs: %d",
            getNumSubDevicesParams.numSubDevices);
        goto failed;
    }

    hRmSubDevice = nvKmsKapiGenerateRmHandle(device);

    if (hRmDevice == 0x0) {
        nvKmsKapiLogDeviceDebug(device, "Failed to allocate RM handle");
        goto failed;
    }

    subdevAllocParams.subDeviceId = 0;

    ret = nvRmApiAlloc(device->hRmClient,
                       device->hRmDevice,
                       hRmSubDevice,
                       NV20_SUBDEVICE_0,
                       &subdevAllocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(device, "Failed to initialize subDevice");
        nvKmsKapiFreeRmHandle(device, hRmSubDevice);
        goto failed;
    }

    device->hRmSubDevice = hRmSubDevice;

    if (device->isSOC) {
        /* NVKMS is only used on T23X and later chips,
         * which all support generic memory. */
        supportsGenericPageKind = NV_TRUE;
    } else {
        ret = nvRmApiControl(device->hRmClient,
                             device->hRmDevice,
                             NV0080_CTRL_CMD_FB_GET_CAPS_V2,
                             &fbCapsParams,
                             sizeof (fbCapsParams));
        if (ret != NVOS_STATUS_SUCCESS) {
            nvKmsKapiLogDeviceDebug(device, "Failed to query framebuffer capabilities");
            goto failed;
        }
        supportsGenericPageKind =
            NV0080_CTRL_FB_GET_CAP(fbCapsParams.capsTbl,
                                   NV0080_CTRL_FB_CAPS_GENERIC_PAGE_KIND);
    }

    device->caps.genericPageKind = 
        supportsGenericPageKind ?
        0x06 /* NV_MMU_PTE_KIND_GENERIC_MEMORY */ :
        0xfe /* NV_MMU_PTE_KIND_GENERIC_16BX2 */;

    return NV_TRUE;

failed:

    RmFreeDevice(device);

    return NV_FALSE;
}

/*
 * Helper function to free NVKMS objects allocated for NvKmsKapiDevice.
 */
static void KmsFreeDevice(struct NvKmsKapiDevice *device)
{
    /* Free notifier memory */

    nvKmsKapiFreeNotifiers(device);

    /* Free NVKMS device */

    if (device->hKmsDevice != 0x0)  {
        struct NvKmsFreeDeviceParams paramsFree = { };

        paramsFree.request.deviceHandle = device->hKmsDevice;

        nvkms_ioctl_from_kapi(device->pKmsOpen,
                              NVKMS_IOCTL_FREE_DEVICE,
                              &paramsFree, sizeof(paramsFree));

        device->hKmsDevice = device->hKmsDisp = 0x0;
    }

    /* Close NVKMS */

    if (device->pKmsOpen != NULL) {
        nvkms_close_from_kapi(device->pKmsOpen);
        device->pKmsOpen = NULL;
    }
}

/*
 * Helper function to allocate NVKMS objects for NvKmsKapiDevice.
 */
static NvBool KmsAllocateDevice(struct NvKmsKapiDevice *device)
{
    struct NvKmsAllocDeviceParams *paramsAlloc;
    NvBool status;
    NvBool inVideoMemory = FALSE;
    NvU32 head;
    NvBool ret = FALSE;
    NvU32 layer;

    paramsAlloc = nvKmsKapiCalloc(1, sizeof(*paramsAlloc));
    if (paramsAlloc == NULL) {
        return FALSE;
    }

    /* Open NVKMS */

    device->pKmsOpen = nvkms_open_from_kapi(device);

    if (device->pKmsOpen == NULL) {
        nvKmsKapiLogDeviceDebug(device, "Failed to Open NVKMS");
        goto done;
    }

    /* Allocate NVKMS device */

    nvkms_strncpy(
        paramsAlloc->request.versionString,
        NV_VERSION_STRING,
        sizeof(paramsAlloc->request.versionString));

    if (device->isSOC) {
        paramsAlloc->request.deviceId = NVKMS_DEVICE_ID_TEGRA;
    } else {
        paramsAlloc->request.deviceId = device->deviceInstance;
    }
    paramsAlloc->request.sliMosaic = NV_FALSE;
    paramsAlloc->request.enableConsoleHotplugHandling = NV_TRUE;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_ALLOC_DEVICE,
                                   paramsAlloc, sizeof(*paramsAlloc));

    if (!status ||
        paramsAlloc->reply.status != NVKMS_ALLOC_DEVICE_STATUS_SUCCESS) {

        if (paramsAlloc->reply.status ==
            NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE) {
            nvKmsKapiLogDeviceDebug(
                device,
                "Display hardware is not available; falling back to "
                "displayless mode");

            ret = TRUE;
            goto done;
        }

        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to NVKM device %u(%u): %d %d\n",
             device->gpuId,
             paramsAlloc->request.deviceId,
             status,
             paramsAlloc->reply.status);

        goto done;
    }

    device->hKmsDevice = paramsAlloc->reply.deviceHandle;

    device->caps.cursorCompositionCaps =
        paramsAlloc->reply.cursorCompositionCaps;

    device->caps.overlayCompositionCaps =
        paramsAlloc->reply.layerCaps[NVKMS_OVERLAY_LAYER].composition;

    device->caps.validLayerRRTransforms =
        paramsAlloc->reply.validLayerRRTransforms;

    device->caps.maxWidthInPixels      = paramsAlloc->reply.maxWidthInPixels;
    device->caps.maxHeightInPixels     = paramsAlloc->reply.maxHeightInPixels;
    device->caps.maxCursorSizeInPixels = paramsAlloc->reply.maxCursorSize;
    device->caps.requiresVrrSemaphores = paramsAlloc->reply.requiresVrrSemaphores;
    /* The generic page kind was determined during RM device allocation,
     * but it should match what NVKMS reports */
    nvAssert(device->caps.genericPageKind == paramsAlloc->reply.genericPageKind);

    /* XXX Add LUT support */

    device->numHeads = paramsAlloc->reply.numHeads;

    for (head = 0; head < device->numHeads; head++) {
        if (paramsAlloc->reply.numLayers[head] < 2) {
            goto done;
        }
        device->numLayers[head] = paramsAlloc->reply.numLayers[head];
    }

    for (layer = 0; layer < NVKMS_KAPI_LAYER_MAX; layer++) {
        device->supportedSurfaceMemoryFormats[layer] =
            paramsAlloc->reply.layerCaps[layer].supportedSurfaceMemoryFormats;
        device->supportsHDR[layer] = paramsAlloc->reply.layerCaps[layer].supportsHDR;
    }

    if (paramsAlloc->reply.validNIsoFormatMask &
        (1 << NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY)) {
        device->notifier.format = NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY;
    } else if (paramsAlloc->reply.validNIsoFormatMask &
               (1 << NVKMS_NISO_FORMAT_FOUR_WORD)) {
        device->notifier.format = NVKMS_NISO_FORMAT_FOUR_WORD;
    } else {
        nvAssert(paramsAlloc->reply.validNIsoFormatMask &
                 (1 << NVKMS_NISO_FORMAT_LEGACY));
        device->notifier.format = NVKMS_NISO_FORMAT_LEGACY;
    }

    /* XXX Add support for SLI/multiple display engines per device */
    if (paramsAlloc->reply.numDisps != 1)
    {
        nvKmsKapiLogDeviceDebug(device, "Found unsupported SLI configuration");
        goto done;
    }

    device->hKmsDisp = paramsAlloc->reply.dispHandles[0];
    device->dispIdx  = 0;

    device->subDeviceMask = paramsAlloc->reply.subDeviceMask;

    device->isoIOCoherencyModes = paramsAlloc->reply.isoIOCoherencyModes;
    device->nisoIOCoherencyModes = paramsAlloc->reply.nisoIOCoherencyModes;

    device->supportsSyncpts = paramsAlloc->reply.supportsSyncpts;

    if (paramsAlloc->reply.nIsoSurfacesInVidmemOnly) {
        inVideoMemory = TRUE;
    }

    /* Allocate notifier memory */
    if (!nvKmsKapiAllocateNotifiers(device, inVideoMemory)) {
        nvKmsKapiLogDebug(
            "Failed to allocate Notifier objects for GPU ID 0x%08x",
            device->gpuId);
        goto done;
    }

    ret = NV_TRUE;

done:
    if (!ret) {
        KmsFreeDevice(device);
    }

    nvKmsKapiFree(paramsAlloc);

    return ret;
}

static void FreeDevice(struct NvKmsKapiDevice *device)
{
    /* Free NVKMS objects allocated for NvKmsKapiDevice */

    KmsFreeDevice(device);

    /* Free RM objects allocated for NvKmsKapiDevice */

    RmFreeDevice(device);

    /* Lower the reference count of gpu. */

    nvkms_close_gpu(device->gpuId);

    if (device->pSema != NULL) {
        nvkms_sema_free(device->pSema);
    }

    nvKmsKapiFree(device);
}

NvBool nvKmsKapiAllocateSystemMemory(struct NvKmsKapiDevice *device,
                                     NvU32 hRmHandle,
                                     enum NvKmsSurfaceMemoryLayout layout,
                                     NvU64 size,
                                     enum NvKmsKapiAllocationType type,
                                     NvU8 *compressible)
{
    NvU32 ret;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    const NvKmsDispIOCoherencyModes *pIOCoherencyModes = NULL;

    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.size = size;

    switch (layout) {
        case NvKmsSurfaceMemoryLayoutBlockLinear:
            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _FORMAT, _BLOCK_LINEAR,
                            memAllocParams.attr);
            if (*compressible) {
                /*
                 * RM will choose a compressed page kind and hence allocate
                 * comptags for color surfaces >= 32bpp.  The actual kind
                 * chosen isn't important, as it can be overridden by creating
                 * a virtual alloc with a different kind when mapping the
                 * memory into the GPU.
                 */
                memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _32,
                                                  memAllocParams.attr);
                memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _ANY,
                                                  memAllocParams.attr);
            } else {
                memAllocParams.attr =
                    FLD_SET_DRF(OS32, _ATTR, _DEPTH, _UNKNOWN,
                                memAllocParams.attr);
            }
            break;

        case NvKmsSurfaceMemoryLayoutPitch:
            memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _FORMAT, _PITCH,
                                              memAllocParams.attr);
            break;

        default:
            nvKmsKapiLogDeviceDebug(device, "Unknown Memory Layout");
            return NV_FALSE;
    }

    switch (type) {
        case NVKMS_KAPI_ALLOCATION_TYPE_SCANOUT:
            /* XXX Note compression and scanout do not work together on
             * any current GPUs.  However, some use cases do involve scanning
             * out a compression-capable surface:
             *
             * 1) Mapping the compressible surface as non-compressed when
             *    generating its content.
             *
             * 2) Using decompress-in-place to decompress the surface content
             *    before scanning it out.
             *
             * Hence creating compressed allocations of TYPE_SCANOUT is allowed.
             */

            pIOCoherencyModes = &device->isoIOCoherencyModes;

            break;
        case NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER:
            if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
                nvKmsKapiLogDeviceDebug(device,
                        "Attempting creation of BlockLinear notifier memory");
                return NV_FALSE;
            }

            memAllocParams.attr2 = FLD_SET_DRF(OS32, _ATTR2, _NISO_DISPLAY,
                                               _YES, memAllocParams.attr2);

            pIOCoherencyModes = &device->nisoIOCoherencyModes;

            break;
        case NVKMS_KAPI_ALLOCATION_TYPE_OFFSCREEN:
            memAllocParams.flags |= NVOS32_ALLOC_FLAGS_NO_SCANOUT;
            break;
        default:
            nvKmsKapiLogDeviceDebug(device, "Unknown Allocation Type");
            return NV_FALSE;
    }

    memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _PCI,
                                      memAllocParams.attr);
    memAllocParams.attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO,
                                       memAllocParams.attr2);

    if (pIOCoherencyModes == NULL || !pIOCoherencyModes->coherent) {
        memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY,
                                          _WRITE_COMBINE, memAllocParams.attr);
    } else {
        memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY,
                                          _WRITE_BACK, memAllocParams.attr);
    }

    memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS,
                                      memAllocParams.attr);

    ret = nvRmApiAlloc(device->hRmClient,
                       device->hRmDevice,
                       hRmHandle,
                       NV01_MEMORY_SYSTEM,
                       &memAllocParams);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
                device,
                "nvRmApiAlloc failed with error code 0x%08x",
                ret);

        return NV_FALSE;
    }

    if (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE,
                     memAllocParams.attr)) {
        *compressible = 0;
    } else {
        *compressible = 1;
    }

    return TRUE;
}

NvBool nvKmsKapiAllocateVideoMemory(struct NvKmsKapiDevice *device,
                                    NvU32 hRmHandle,
                                    enum NvKmsSurfaceMemoryLayout layout,
                                    NvU64 size,
                                    enum NvKmsKapiAllocationType type,
                                    NvU8 *compressible)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    NvU32 ret;

    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.size = size;

    switch (layout) {
        case NvKmsSurfaceMemoryLayoutBlockLinear:
            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _FORMAT, _BLOCK_LINEAR,
                            memAllocParams.attr);

            if (*compressible) {
                /*
                 * RM will choose a compressed page kind and hence allocate
                 * comptags for color surfaces >= 32bpp.  The actual kind
                 * chosen isn't important, as it can be overridden by creating
                 * a virtual alloc with a different kind when mapping the
                 * memory into the GPU.
                 */
                memAllocParams.attr =
                    FLD_SET_DRF(OS32, _ATTR, _DEPTH, _32,
                                memAllocParams.attr);
                memAllocParams.attr =
                    FLD_SET_DRF(OS32, _ATTR, _COMPR, _ANY,
                                memAllocParams.attr);
            } else {
                memAllocParams.attr =
                    FLD_SET_DRF(OS32, _ATTR, _DEPTH, _UNKNOWN,
                                memAllocParams.attr);
            }
            break;

        case NvKmsSurfaceMemoryLayoutPitch:
            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _FORMAT, _PITCH,
                            memAllocParams.attr);
            break;

        default:
            nvKmsKapiLogDeviceDebug(device, "Unknown Memory Layout");
            return NV_FALSE;
    }


    memAllocParams.attr =
        FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM,
                    memAllocParams.attr);
    memAllocParams.attr2 =
        FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO,
                    memAllocParams.attr2);

    switch (type) {
        case NVKMS_KAPI_ALLOCATION_TYPE_SCANOUT:
            /* XXX [JRJ] Not quite right.  This can also be used to allocate
             * cursor images.  The stuff RM does with this field is kind of
             * black magic, and I can't tell if it actually matters.
             */
            memAllocParams.type = NVOS32_TYPE_PRIMARY;

            memAllocParams.alignment = NV_EVO_SURFACE_ALIGNMENT;
            memAllocParams.flags |=
                NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE |   /* Pick up above EVO alignment */
                NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP; /* X sets this for cursors */
                memAllocParams.attr =
                    FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS,
                                memAllocParams.attr);

            /* XXX [JRJ] Note compression and scanout do not work together on
             * any current GPUs.  However, some use cases do involve scanning
             * out a compression-capable surface:
             *
             * 1) Mapping the compressible surface as non-compressed when
             *    generating its content.
             *
             * 2) Using decompress-in-place to decompress the surface content
             *    before scanning it out.
             *
             * Hence creating compressed allocations of TYPE_SCANOUT is allowed.
             */

            break;
        case NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER:
            if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
                nvKmsKapiLogDeviceDebug(device,
                    "Attempting creation of BlockLinear notifier memory");
                return NV_FALSE;
            }

            memAllocParams.type = NVOS32_TYPE_DMA;

            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB,
                            memAllocParams.attr);
            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _UNCACHED,
                            memAllocParams.attr);

            break;
        case NVKMS_KAPI_ALLOCATION_TYPE_OFFSCREEN:
            memAllocParams.type = NVOS32_TYPE_IMAGE;
            memAllocParams.flags |=
                NVOS32_ALLOC_FLAGS_NO_SCANOUT |
                NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP;
            memAllocParams.attr =
                FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS,
                            memAllocParams.attr);
            break;
        default:
            nvKmsKapiLogDeviceDebug(device, "Unknown Allocation Type");
            return NV_FALSE;
    }

    ret = nvRmApiAlloc(device->hRmClient,
                       device->hRmDevice,
                       hRmHandle,
                       NV01_MEMORY_LOCAL_USER,
                       &memAllocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "VidHeapControl failed with error code 0x%08x",
            ret);

        return NV_FALSE;
    }

    if (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE,
                     memAllocParams.attr)) {
        *compressible = 0;
    } else {
        *compressible = 1;
    }

    return NV_TRUE;
}

static struct NvKmsKapiDevice* AllocateDevice
(
    const struct NvKmsKapiAllocateDeviceParams *params
)
{
    struct NvKmsKapiDevice *device = NULL;

    device = nvKmsKapiCalloc(1, sizeof(*device));

    if (device == NULL) {
        nvKmsKapiLogDebug(
            "Failed to allocate memory for NvKmsKapiDevice of GPU ID 0x%08x",
            params->gpuId);
        goto failed;
    }

    device->pSema = nvkms_sema_alloc();

    if (device->pSema == NULL) {
        nvKmsKapiLogDebug(
            "Failed to allocate semaphore for NvKmsKapiDevice of GPU ID 0x%08x",
            params->gpuId);
        goto failed;
    }

    /* Raise the reference count of gpu. */

    if (!nvkms_open_gpu(params->gpuId)) {
        nvKmsKapiLogDebug("Failed to open GPU ID 0x%08x", params->gpuId);
        goto failed;
    }

    device->gpuId = params->gpuId;

    nvKmsKapiLogDebug(
        "Allocating NvKmsKapiDevice 0x%p for GPU ID 0x%08x",
        device,
        device->gpuId);

    /* Allocate RM object for NvKmsKapiDevice */

    if (!RmAllocateDevice(device)) {
        nvKmsKapiLogDebug(
            "Failed to allocate RM objects for GPU ID 0x%08x",
            device->gpuId);
        goto failed;
    }

    /* Allocate NVKMS objects for NvKmsKapiDevice */

    if (!KmsAllocateDevice(device)) {
        nvKmsKapiLogDebug(
            "Failed to allocate NVKMS objects for GPU ID 0x%08x",
            device->gpuId);
        goto failed;
    }

    device->privateData = params->privateData;
    device->eventCallback = params->eventCallback;

    return device;

failed:

    FreeDevice(device);

    return NULL;
}

static NvBool GrabOwnership(struct NvKmsKapiDevice *device)
{
    struct NvKmsGrabOwnershipParams paramsGrab = { };

    if (device->hKmsDevice == 0x0) {
        return NV_TRUE;
    }

    paramsGrab.request.deviceHandle = device->hKmsDevice;

    return nvkms_ioctl_from_kapi(device->pKmsOpen,
                                 NVKMS_IOCTL_GRAB_OWNERSHIP,
                                 &paramsGrab, sizeof(paramsGrab));

}

static void ReleaseOwnership(struct NvKmsKapiDevice *device)
{
    struct NvKmsReleaseOwnershipParams paramsRelease = { };

    if (device->hKmsDevice == 0x0) {
        return;
    }

    paramsRelease.request.deviceHandle = device->hKmsDevice;

    nvkms_ioctl_from_kapi(device->pKmsOpen,
                          NVKMS_IOCTL_RELEASE_OWNERSHIP,
                          &paramsRelease, sizeof(paramsRelease));
}

static NvBool GrantPermissions
(
    NvS32 fd, 
    struct NvKmsKapiDevice *device,
    NvU32 head,
    NvKmsKapiDisplay display
)
{
    struct NvKmsGrantPermissionsParams paramsGrant = { };
    struct NvKmsPermissions *perm = &paramsGrant.request.permissions;
    NvU32 dispIdx = device->dispIdx;

    if (dispIdx >= ARRAY_LEN(perm->modeset.disp) ||
        head >= ARRAY_LEN(perm->modeset.disp[0].head) || device == NULL) {
        return NV_FALSE;
    }

    if (device->hKmsDevice == 0x0) {
        return NV_TRUE;
    }

    perm->type = NV_KMS_PERMISSIONS_TYPE_MODESET;
    perm->modeset.disp[dispIdx].head[head].dpyIdList =
        nvAddDpyIdToEmptyDpyIdList(nvNvU32ToDpyId(display));

    paramsGrant.request.fd = fd;
    paramsGrant.request.deviceHandle = device->hKmsDevice;

    return nvkms_ioctl_from_kapi(device->pKmsOpen,
                                 NVKMS_IOCTL_GRANT_PERMISSIONS, &paramsGrant,
                                 sizeof(paramsGrant));
}

static NvBool RevokePermissions
(
    struct NvKmsKapiDevice *device,
    NvU32 head,
    NvKmsKapiDisplay display
)
{
    struct NvKmsRevokePermissionsParams paramsRevoke = { };
    struct NvKmsPermissions *perm = &paramsRevoke.request.permissions;
    NvU32 dispIdx = device->dispIdx;


    if (dispIdx >= ARRAY_LEN(perm->modeset.disp) ||
        head >= ARRAY_LEN(perm->modeset.disp[0].head) || device == NULL) {
        return NV_FALSE;
    }

    if (device->hKmsDevice == 0x0) {
        return NV_TRUE;
    }

    perm->type = NV_KMS_PERMISSIONS_TYPE_MODESET;
    perm->modeset.disp[dispIdx].head[head].dpyIdList =
        nvAddDpyIdToEmptyDpyIdList(nvNvU32ToDpyId(display));

    paramsRevoke.request.deviceHandle = device->hKmsDevice;

    return nvkms_ioctl_from_kapi(device->pKmsOpen,
                                 NVKMS_IOCTL_REVOKE_PERMISSIONS, &paramsRevoke,
                                 sizeof(paramsRevoke));
}

static NvBool DeclareEventInterest
(
    const struct NvKmsKapiDevice *device,
    const NvU32 interestMask
)
{
    struct NvKmsDeclareEventInterestParams kmsEventParams = { };

    if (device->hKmsDevice == 0x0 || device->eventCallback == NULL) {
        return NV_TRUE;
    }

    kmsEventParams.request.interestMask =
                   interestMask & NVKMS_KAPI_SUPPORTED_EVENTS_MASK;

    return nvkms_ioctl_from_kapi(device->pKmsOpen,
                                 NVKMS_IOCTL_DECLARE_EVENT_INTEREST,
                                 &kmsEventParams, sizeof(kmsEventParams));
}

static NvBool GetDeviceResourcesInfo
(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiDeviceResourcesInfo *info
)
{
    struct NvKmsQueryDispParams paramsDisp = { };
    NvBool status = NV_FALSE;

    NvU32 i;

    nvkms_memset(info, 0, sizeof(*info));

    info->caps.hasVideoMemory = !device->isSOC;
    info->caps.genericPageKind = device->caps.genericPageKind;

    if (device->hKmsDevice == 0x0) {
        info->caps.pitchAlignment = 0x1;
        return NV_TRUE;
    }

    paramsDisp.request.deviceHandle = device->hKmsDevice;
    paramsDisp.request.dispHandle   = device->hKmsDisp;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_DISP,
                                   &paramsDisp, sizeof(paramsDisp));

    if (!status)
    {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to query display engine information");

        goto done;
    }

    info->numHeads = device->numHeads;

    ct_assert(sizeof(info->numLayers) == sizeof(device->numLayers));
    nvkms_memcpy(info->numLayers, device->numLayers, sizeof(device->numLayers));

    ct_assert(ARRAY_LEN(info->connectorHandles) >=
              ARRAY_LEN(paramsDisp.reply.connectorHandles));

    info->numConnectors = paramsDisp.reply.numConnectors;

    for (i = 0; i < paramsDisp.reply.numConnectors; i++) {
        info->connectorHandles[i] = paramsDisp.reply.connectorHandles[i];
    }

    {
        const struct NvKmsCompositionCapabilities *pCaps =
            &device->caps.cursorCompositionCaps;

        info->caps.validCursorCompositionModes =
            pCaps->colorKeySelect[NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE].
                supportedBlendModes[1];
    }

    for (i = 0; i < NVKMS_KAPI_LAYER_MAX; i++) {
        if (i == NVKMS_KAPI_LAYER_PRIMARY_IDX) {
            info->caps.layer[i].validCompositionModes =
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE);
        } else {
            const struct NvKmsCompositionCapabilities *pCaps =
                &device->caps.overlayCompositionCaps;

            info->caps.layer[i].validCompositionModes =
                pCaps->colorKeySelect[NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE].
                    supportedBlendModes[1];
        }
    }

    for (i = 0; i < NVKMS_KAPI_LAYER_MAX; i++) {
        info->caps.layer[i].validRRTransforms =
            device->caps.validLayerRRTransforms;
    }

    info->caps.maxWidthInPixels      = device->caps.maxWidthInPixels;
    info->caps.maxHeightInPixels     = device->caps.maxHeightInPixels;
    info->caps.maxCursorSizeInPixels = device->caps.maxCursorSizeInPixels;

    info->caps.pitchAlignment = NV_EVO_PITCH_ALIGNMENT;

    info->caps.supportsSyncpts = device->supportsSyncpts;

    info->caps.supportedCursorSurfaceMemoryFormats =
        NVBIT(NvKmsSurfaceMemoryFormatA8R8G8B8);

    ct_assert(sizeof(info->supportedSurfaceMemoryFormats) ==
              sizeof(device->supportedSurfaceMemoryFormats));

    nvkms_memcpy(info->supportedSurfaceMemoryFormats,
                 device->supportedSurfaceMemoryFormats,
                 sizeof(device->supportedSurfaceMemoryFormats));

    ct_assert(sizeof(info->supportsHDR) ==
              sizeof(device->supportsHDR));

    nvkms_memcpy(info->supportsHDR,
                 device->supportsHDR,
                 sizeof(device->supportsHDR));
done:

    return status;
}

/*
 * XXX Make it per-connector, query valid dpyId list as dynamic data of
 * connector.
 */
static NvBool GetDisplays
(
    struct NvKmsKapiDevice *device,
    NvU32 *numDisplays, NvKmsKapiDisplay *displayHandles
)
{
    struct NvKmsQueryDispParams paramsDisp = { };
    NvBool status = NV_FALSE;

    NVDpyId dpyId;

    if (device->hKmsDevice == 0x0) {
        *numDisplays = 0;
        return NV_TRUE;
    }

    paramsDisp.request.deviceHandle = device->hKmsDevice;
    paramsDisp.request.dispHandle   = device->hKmsDisp;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_DISP,
                                   &paramsDisp, sizeof(paramsDisp));

    if (!status)
    {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to query display engine information");

        return NV_FALSE;
    }

    if (*numDisplays == 0) {
        goto done;
    }

    if (*numDisplays < nvCountDpyIdsInDpyIdList(paramsDisp.reply.validDpys)) {
        nvKmsKapiLogDebug(
            "Size of display handle array is less than number of displays");
        goto done;
    }

    FOR_ALL_DPY_IDS(dpyId, paramsDisp.reply.validDpys) {
        *(displayHandles++) = nvDpyIdToNvU32(dpyId);
    }

done:

    *numDisplays = nvCountDpyIdsInDpyIdList(paramsDisp.reply.validDpys);

    return NV_TRUE;
}

static NvBool GetConnectorInfo
(
    struct NvKmsKapiDevice *device,
    NvKmsKapiConnector connector, struct NvKmsKapiConnectorInfo *info
)
{
    struct NvKmsQueryConnectorStaticDataParams paramsConnector = { };
    NvBool status = NV_FALSE;

    if (device == NULL || info == NULL) {
        goto done;
    }

    paramsConnector.request.deviceHandle    = device->hKmsDevice;
    paramsConnector.request.dispHandle      = device->hKmsDisp;
    paramsConnector.request.connectorHandle = connector;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_CONNECTOR_STATIC_DATA,
                                   &paramsConnector, sizeof(paramsConnector));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to query static data of connector 0x%08x",
            connector);

        goto done;
    }

    info->handle = connector;

    info->physicalIndex = paramsConnector.reply.physicalIndex;

    info->signalFormat = paramsConnector.reply.signalFormat;

    info->type = paramsConnector.reply.type;

done:

    return status;
}

static NvBool GetStaticDisplayInfo
(
    struct NvKmsKapiDevice *device,
    NvKmsKapiDisplay display, struct NvKmsKapiStaticDisplayInfo *info
)
{
    struct NvKmsQueryDpyStaticDataParams   paramsDpyStatic = { };
    NvBool status = NV_FALSE;

    if (device == NULL || info == NULL) {
        goto done;
    }

    /* Query static data of display */

    paramsDpyStatic.request.deviceHandle = device->hKmsDevice;
    paramsDpyStatic.request.dispHandle   = device->hKmsDisp;

    paramsDpyStatic.request.dpyId = nvNvU32ToDpyId(display);

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_DPY_STATIC_DATA,
                                   &paramsDpyStatic, sizeof(paramsDpyStatic));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to query static data of dpy 0x%08x",
            display);

        goto done;
    }

    info->handle = display;

    info->connectorHandle = paramsDpyStatic.reply.connectorHandle;

    ct_assert(sizeof(info->dpAddress) ==
              sizeof(paramsDpyStatic.reply.dpAddress));

    nvkms_memcpy(info->dpAddress,
                 paramsDpyStatic.reply.dpAddress,
                 sizeof(paramsDpyStatic.reply.dpAddress));
    info->dpAddress[sizeof(paramsDpyStatic.reply.dpAddress) - 1] = '\0';

    info->internal = paramsDpyStatic.reply.mobileInternal;
    info->headMask = paramsDpyStatic.reply.headMask;
done:

    return status;
}

static NvBool GetDynamicDisplayInfo(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiDynamicDisplayParams *params)
{
    struct NvKmsQueryDpyDynamicDataParams *pParamsDpyDynamic = NULL;
    NvBool status = NV_FALSE;

    if (device == NULL || params == NULL) {
        goto done;
    }

    pParamsDpyDynamic = nvKmsKapiCalloc(1, sizeof(*pParamsDpyDynamic));

    if (pParamsDpyDynamic == NULL) {
        goto done;
    }

    pParamsDpyDynamic->request.deviceHandle = device->hKmsDevice;
    pParamsDpyDynamic->request.dispHandle   = device->hKmsDisp;

    pParamsDpyDynamic->request.dpyId = nvNvU32ToDpyId(params->handle);

    if (params->overrideEdid) {
        ct_assert(sizeof(params->edid.buffer) ==
                  sizeof(pParamsDpyDynamic->reply.edid.buffer));
        nvkms_memcpy(
            pParamsDpyDynamic->request.edid.buffer,
            params->edid.buffer,
            sizeof(pParamsDpyDynamic->request.edid.buffer));

        pParamsDpyDynamic->request.edid.bufferSize = params->edid.bufferSize;

        pParamsDpyDynamic->request.overrideEdid = NV_TRUE;
    }

    pParamsDpyDynamic->request.forceConnected = params->forceConnected;

    pParamsDpyDynamic->request.forceDisconnected = params->forceDisconnected;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_DPY_DYNAMIC_DATA,
                                   pParamsDpyDynamic, sizeof(*pParamsDpyDynamic));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to query dynamic data of dpy 0x%08x",
            params->handle);

        goto done;
    }

    params->connected = pParamsDpyDynamic->reply.connected;

    if (pParamsDpyDynamic->reply.connected && !params->overrideEdid) {
        NvBool vrrSupported =
            (pParamsDpyDynamic->reply.vrrType != NVKMS_DPY_VRR_TYPE_NONE) ? NV_TRUE : NV_FALSE;

        nvkms_memcpy(
            params->edid.buffer,
            pParamsDpyDynamic->reply.edid.buffer,
            sizeof(params->edid.buffer));

        params->edid.bufferSize = pParamsDpyDynamic->reply.edid.bufferSize;
        params->vrrSupported = (vrrSupported && !device->caps.requiresVrrSemaphores) ? NV_TRUE : NV_FALSE;
    }

done:

    if (pParamsDpyDynamic != NULL) {
        nvKmsKapiFree(pParamsDpyDynamic);
    }

    return status;
}

static void FreeMemory
(
    struct NvKmsKapiDevice *device, struct NvKmsKapiMemory *memory
)
{
    if (device == NULL || memory == NULL) {
        return;
    }

    if (memory->hRmHandle != 0x0) {
        NvU32 ret;

        ret = nvRmApiFree(device->hRmClient,
                          device->hRmDevice,
                          memory->hRmHandle);

        if (ret != NVOS_STATUS_SUCCESS) {
            nvKmsKapiLogDeviceDebug(
                device,
                "Failed to free RM memory object 0x%08x allocated for "
                "NvKmsKapiMemory 0x%p",
                memory->hRmHandle, memory);
        }

        nvKmsKapiFreeRmHandle(device, memory->hRmHandle);
    }

    nvKmsKapiFree(memory);
}

static struct NvKmsKapiMemory *AllocMemoryObjectAndHandle(
    struct NvKmsKapiDevice *device,
    NvU32 *handleOut
)
{
    struct NvKmsKapiMemory *memory;

    /* Allocate the container object */

    memory = nvKmsKapiCalloc(1, sizeof(*memory));

    if (memory == NULL) {
        nvKmsKapiLogDebug(
            "Failed to allocate memory for NVKMS memory object on "
            "NvKmsKapiDevice 0x%p",
            device);
        return NULL;
    }

    /* Generate RM handle for memory object */

    *handleOut = nvKmsKapiGenerateRmHandle(device);

    if (*handleOut == 0x0) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to allocate RM handle for memory");
        nvKmsKapiFree(memory);
        return NULL;
    }

    return memory;
}

static struct NvKmsKapiMemory* AllocateVideoMemory
(
    struct NvKmsKapiDevice *device,
    enum NvKmsSurfaceMemoryLayout layout,
    enum NvKmsKapiAllocationType type,
    NvU64 size,
    NvU8 *compressible
)
{
    struct NvKmsKapiMemory *memory = NULL;
    NvU32 hRmHandle;

    memory = AllocMemoryObjectAndHandle(device, &hRmHandle);

    if (!memory) {
        return NULL;
    }

    if (!nvKmsKapiAllocateVideoMemory(device,
                                      hRmHandle,
                                      layout,
                                      size,
                                      type,
                                      compressible)) {
        nvKmsKapiFreeRmHandle(device, hRmHandle);
        FreeMemory(device, memory);
        return NULL;
    }

    memory->hRmHandle = hRmHandle;
    memory->size = size;
    memory->surfaceParams.layout = layout;

    if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        memory->surfaceParams.blockLinear.genericMemory = NV_TRUE;
    }

    return memory;
}

static struct NvKmsKapiMemory* AllocateSystemMemory
(
    struct NvKmsKapiDevice *device,
    enum NvKmsSurfaceMemoryLayout layout,
    enum NvKmsKapiAllocationType type,
    NvU64 size,
    NvU8 *compressible
)
{
    struct NvKmsKapiMemory *memory = NULL;
    NvU32 hRmHandle;

    memory = AllocMemoryObjectAndHandle(device, &hRmHandle);

    if (!memory) {
        return NULL;
    }

    if (!nvKmsKapiAllocateSystemMemory(device,
                                       hRmHandle,
                                       layout,
                                       size,
                                       type,
                                       compressible)) {
        nvKmsKapiFreeRmHandle(device, hRmHandle);
        FreeMemory(device, memory);
        return NULL;
    }

    memory->hRmHandle = hRmHandle;
    memory->size = size;
    memory->surfaceParams.layout = layout;

    if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        memory->surfaceParams.blockLinear.genericMemory = NV_TRUE;
    }

    return memory;
}

static struct NvKmsKapiMemory* ImportMemory
(
    struct NvKmsKapiDevice *device,
    NvU64 memorySize,
    NvU64 nvKmsParamsUser,
    NvU64 nvKmsParamsSize
)
{
    struct NvKmsKapiPrivImportMemoryParams nvKmsParams, *pNvKmsParams = NULL;
    NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS importParams = { };
    struct NvKmsKapiMemory *memory = NULL;
    NvU32 hMemory;
    NvU32 ret;
    int status;

    /* Verify the driver-private params size and copy it in from userspace */

    if (nvKmsParamsSize != sizeof(nvKmsParams)) {
        nvKmsKapiLogDebug(
            "NVKMS private memory import parameter size mismatch - "
            "expected: 0x%llx, caller specified: 0x%llx",
            (NvU64)sizeof(nvKmsParams), nvKmsParamsSize);
        return NULL;
    }

    /*
     * Use a heap allocation as the destination pointer passed to
     * nvkms_copyin; stack allocations created within core NVKMS may not
     * be recognizable to the Linux kernel's CONFIG_HARDENED_USERCOPY
     * checker, triggering false errors.  But then save the result to a
     * variable on the stack, so that we can free the heap memory
     * immediately and not worry about its lifetime.
     */

    pNvKmsParams = nvKmsKapiCalloc(1, sizeof(*pNvKmsParams));

    if (pNvKmsParams == NULL) {
        nvKmsKapiLogDebug("Failed to allocate memory for ImportMemory");
        return NULL;
    }

    status = nvkms_copyin(pNvKmsParams, nvKmsParamsUser, sizeof(*pNvKmsParams));

    nvKmsParams = *pNvKmsParams;

    nvKmsKapiFree(pNvKmsParams);

    if (status != 0) {
        nvKmsKapiLogDebug(
            "NVKMS private memory import parameters could not be read from "
            "userspace");
        return NULL;
    }

    memory = AllocMemoryObjectAndHandle(device, &hMemory);

    if (!memory) {
        return NULL;
    }

    importParams.fd = nvKmsParams.memFd;
    importParams.object.type = NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM;
    importParams.object.data.rmObject.hDevice = device->hRmDevice;
    importParams.object.data.rmObject.hParent = device->hRmDevice;
    importParams.object.data.rmObject.hObject = hMemory;

    ret = nvRmApiControl(device->hRmClient,
                         device->hRmClient,
                         NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD,
                         &importParams,
                         sizeof(importParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to import RM memory object (%d) of size %llu bytes",
            nvKmsParams.memFd, memorySize);

        nvKmsKapiFreeRmHandle(device, hMemory);
        goto failed;
    }

    memory->hRmHandle = hMemory;
    memory->size = memorySize;
    memory->surfaceParams = nvKmsParams.surfaceParams;

    return memory;

failed:

    FreeMemory(device, memory);

    return NULL;
}

static struct NvKmsKapiMemory* DupMemory
(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiDevice *srcDevice,
    const struct NvKmsKapiMemory *srcMemory
)
{
    struct NvKmsKapiMemory *memory;
    NvU32 hMemory;
    NvU32 ret;

    memory = AllocMemoryObjectAndHandle(device, &hMemory);

    if (!memory) {
        return NULL;
    }

    ret = nvRmApiDupObject(device->hRmClient,
                           device->hRmDevice,
                           hMemory,
                           srcDevice->hRmClient,
                           srcMemory->hRmHandle,
                           0);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to dup NVKMS memory object 0x%p (0x%08x, 0x%08x) "
            "of size %llu bytes",
            srcMemory, srcDevice->hRmClient, srcMemory->hRmHandle,
            srcMemory->size);

        nvKmsKapiFreeRmHandle(device, hMemory);
        goto failed;
    }

    memory->hRmHandle = hMemory;
    memory->size = srcMemory->size;
    memory->surfaceParams = srcMemory->surfaceParams;

    return memory;

failed:
    FreeMemory(device, memory);

    return NULL;
}

static NvBool ExportMemory
(
    const struct NvKmsKapiDevice *device,
    const struct NvKmsKapiMemory *memory,
    NvU64 nvKmsParamsUser,
    NvU64 nvKmsParamsSize
)
{
    struct NvKmsKapiPrivExportMemoryParams nvKmsParams, *pNvKmsParams = NULL;
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS exportParams = { };
    int status;
    NvU32 ret;

    if (device == NULL || memory == NULL) {
        nvKmsKapiLogDebug(
            "Invalid device or memory parameter while exporting memory");
        return NV_FALSE;
    }

    /* Verify the driver-private params size and copy it in from userspace */

    if (nvKmsParamsSize != sizeof(nvKmsParams)) {
        nvKmsKapiLogDebug(
            "NVKMS private memory export parameter size mismatch - "
            "expected: 0x%llx, caller specified: 0x%llx",
            (NvU64)sizeof(nvKmsParams), nvKmsParamsSize);
        return NV_FALSE;
    }

    /*
     * Use a heap allocation as the destination pointer passed to
     * nvkms_copyin; stack allocations created within core NVKMS may not
     * be recognizable to the Linux kernel's CONFIG_HARDENED_USERCOPY
     * checker, triggering false errors.  But then save the result to a
     * variable on the stack, so that we can free the heap memory
     * immediately and not worry about its lifetime.
     */

    pNvKmsParams = nvKmsKapiCalloc(1, sizeof(*pNvKmsParams));

    if (pNvKmsParams == NULL) {
        nvKmsKapiLogDebug("Failed to allocate scratch memory for ExportMemory");
        return NV_FALSE;
    }

    status = nvkms_copyin(pNvKmsParams, nvKmsParamsUser, sizeof(*pNvKmsParams));

    nvKmsParams = *pNvKmsParams;
    nvKmsKapiFree(pNvKmsParams);

    if (status != 0) {
        nvKmsKapiLogDebug(
            "NVKMS private memory export parameters could not be read from "
            "userspace");
        return NV_FALSE;
    }

    exportParams.fd = nvKmsParams.memFd;
    exportParams.object.type = NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM;
    exportParams.object.data.rmObject.hDevice = device->hRmDevice;
    exportParams.object.data.rmObject.hParent = device->hRmDevice;
    exportParams.object.data.rmObject.hObject = memory->hRmHandle;

    ret = nvRmApiControl(device->hRmClient,
                         device->hRmClient,
                         NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECT_TO_FD,
                         &exportParams,
                         sizeof(exportParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to export RM memory object of size %llu bytes "
            "to (%d)", memory->size, nvKmsParams.memFd);
        return NV_FALSE;
    }

    return NV_TRUE;
}

static struct NvKmsKapiMemory*
GetSystemMemoryHandleFromDmaBufSgtHelper(struct NvKmsKapiDevice *device,
                                         NvU32 descriptorType,
                                         NvP64 descriptor,
                                         NvU32 limit)
{
    NvU32 ret;
    NV_OS_DESC_MEMORY_ALLOCATION_PARAMS memAllocParams = {0};
    struct NvKmsKapiMemory *memory = NULL;
    NvU32 hRmHandle;

    memory = AllocMemoryObjectAndHandle(device, &hRmHandle);

    if (!memory) {
        return NULL;
    }

    memAllocParams.type = NVOS32_TYPE_PRIMARY;
    memAllocParams.descriptorType = descriptorType;
    memAllocParams.descriptor = descriptor;
    memAllocParams.limit = limit;

    memAllocParams.attr =
        FLD_SET_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr);

    memAllocParams.attr2 =
        FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, memAllocParams.attr2);

    /* dmabuf import is currently only used for ISO memory. */
    if (!device->isoIOCoherencyModes.coherent) {
        memAllocParams.attr =
            FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_COMBINE,
                        memAllocParams.attr);
    } else {
        memAllocParams.attr =
            FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_BACK,
                        memAllocParams.attr);
    }

    ret = nvRmApiAlloc(device->hRmClient,
                       device->hRmDevice,
                       hRmHandle,
                       NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,
                       &memAllocParams);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvKmsKapiLogDeviceDebug(
                device,
                "nvRmApiAlloc failed with error code 0x%08x",
                ret);
        nvKmsKapiFreeRmHandle(device, hRmHandle);
        FreeMemory(device, memory);
        return NULL;
    }

    memory->hRmHandle = hRmHandle;
    memory->size = limit + 1;
    memory->surfaceParams.layout = NvKmsSurfaceMemoryLayoutPitch;

    return memory;
}

static struct NvKmsKapiMemory*
GetSystemMemoryHandleFromSgt(struct NvKmsKapiDevice *device,
                             NvP64 sgt,
                             NvP64 gem,
                             NvU32 limit)
{
    NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR_PARAMETERS params = {
        .sgt = sgt,
        .gem = gem
    };

    return GetSystemMemoryHandleFromDmaBufSgtHelper(
        device, NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR, &params, limit);
}

static struct NvKmsKapiMemory*
GetSystemMemoryHandleFromDmaBuf(struct NvKmsKapiDevice *device,
                                NvP64 dmaBuf,
                                NvU32 limit)
{
    return GetSystemMemoryHandleFromDmaBufSgtHelper(
        device, NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR, dmaBuf, limit);
}

static NvBool RmGc6BlockerRefCntAction(const struct NvKmsKapiDevice *device,
                                       NvU32 action)
{
    NV_STATUS status;
    NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS params = { 0 };

    nvAssert((action == NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_INC) ||
             (action == NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_DEC));

    params.action = action;

    status = nvRmApiControl(device->hRmClient,
                            device->hRmSubDevice,
                            NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT,
                            &params,
                            sizeof(params));
    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to modify GC6 blocker refcount for 0x%x, status: 0x%x",
            device->hRmSubDevice, status);
        return NV_FALSE;
    }

    return NV_TRUE;
}

static NvBool RmGc6BlockerRefCntInc(const struct NvKmsKapiDevice *device)
{
    return RmGc6BlockerRefCntAction(
        device,
        NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_INC);
}

static NvBool RmGc6BlockerRefCntDec(const struct NvKmsKapiDevice *device)
{
    return RmGc6BlockerRefCntAction(
        device,
        NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_DEC);
}

static NvBool GetMemoryPages
(
    const struct NvKmsKapiDevice *device,
    const struct NvKmsKapiMemory *memory,
    NvU64 **pPages,
    NvU32 *pNumPages
)
{
    NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS paramsGetNumPages = {};
    NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS paramsGetPages = {};
    NvU64 *pages;
    NV_STATUS status;

    if (device == NULL || memory == NULL) {
        return NV_FALSE;
    }

    status = nvRmApiControl(device->hRmClient,
                            memory->hRmHandle,
                            NV003E_CTRL_CMD_GET_SURFACE_NUM_PHYS_PAGES,
                            &paramsGetNumPages,
                            sizeof(paramsGetNumPages));
    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(device,
                "Failed to get number of physical allocation pages for RM"
                "memory object 0x%x", memory->hRmHandle);
        return NV_FALSE;
    }

    if (!paramsGetNumPages.numPages) {
        return NV_FALSE;
    }

    pages = nvKmsKapiCalloc(paramsGetNumPages.numPages, sizeof(pages));
    if (!pages) {
        nvKmsKapiLogDeviceDebug(device, "Failed to allocate memory");
        return NV_FALSE;
    }

    paramsGetPages.pPages = NV_PTR_TO_NvP64(pages);
    paramsGetPages.numPages = paramsGetNumPages.numPages;

    status = nvRmApiControl(device->hRmClient,
                            memory->hRmHandle,
                            NV003E_CTRL_CMD_GET_SURFACE_PHYS_PAGES,
                            &paramsGetPages,
                            sizeof(paramsGetPages));
    if (status != NV_OK) {
        nvKmsKapiFree(pages);
        nvKmsKapiLogDeviceDebug(device,
            "Failed to get physical allocation pages for RM"
            "memory object 0x%x", memory->hRmHandle);
        return NV_FALSE;
    }

    nvAssert(paramsGetPages.numPages == paramsGetNumPages.numPages);

    *pPages = pages;
    *pNumPages = paramsGetPages.numPages;

    return NV_TRUE;
}

/*
 * Check if the memory we are creating this framebuffer with is valid. We
 * cannot scan out sysmem or compressed buffers.
 *
 * If we cannot use this memory for display it may be resident in sysmem
 * or may belong to another GPU.
 */
static NvBool IsMemoryValidForDisplay
(
    const struct NvKmsKapiDevice *device,
    const struct NvKmsKapiMemory *memory
)
{
    NV_STATUS status;
    NV0041_CTRL_SURFACE_INFO surfaceInfo = {};
    NV0041_CTRL_GET_SURFACE_INFO_PARAMS surfaceInfoParams = {};

    if (device == NULL || memory == NULL) {
        return NV_FALSE;
    }

    /*
     * Don't do these checks on tegra. Tegra has different capabilities.
     * Here we always say display is possible so we never fail framebuffer
     * creation.
     */
    if (device->isSOC) {
        return NV_TRUE;
    }

    /* Get the type of address space this memory is in, i.e. vidmem or sysmem */
    surfaceInfo.index = NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE;

    surfaceInfoParams.surfaceInfoListSize = 1;
    surfaceInfoParams.surfaceInfoList = (NvP64)&surfaceInfo;

    status = nvRmApiControl(device->hRmClient,
                            memory->hRmHandle,
                            NV0041_CTRL_CMD_GET_SURFACE_INFO,
                            &surfaceInfoParams,
                            sizeof(surfaceInfoParams));
    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(device,
                "Failed to get memory location of RM memory object 0x%x",
                memory->hRmHandle);
        return NV_FALSE;
    }

    return surfaceInfo.data == NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM;
}

static void FreeMemoryPages
(
    NvU64 *pPages
)
{
    nvKmsKapiFree(pPages);
}

static NvBool MapMemory
(
    const struct NvKmsKapiDevice *device,
    const struct NvKmsKapiMemory *memory, NvKmsKapiMappingType type,
    void **ppLinearAddress
)
{
    NV_STATUS status;
    NvU32 flags = 0;

    if (device == NULL || memory == NULL) {
        return NV_FALSE;
    }

    switch (type) {
        case NVKMS_KAPI_MAPPING_TYPE_USER:
            /*
             * Usermode clients can't be trusted not to access mappings while
             * the GPU is in GC6.
             *
             * TODO: Revoke/restore mappings rather than blocking GC6
             */
            if (!RmGc6BlockerRefCntInc(device)) {
                return NV_FALSE;
            }
            flags |= DRF_DEF(OS33, _FLAGS, _MEM_SPACE, _USER);
            break;
        case NVKMS_KAPI_MAPPING_TYPE_KERNEL:
            /*
             * Kernel clients should ensure on their own that the GPU isn't in
             * GC6 before making accesses to mapped vidmem surfaces.
             */
            break;
    }

    status = nvRmApiMapMemory(
                device->hRmClient,
                device->hRmSubDevice,
                memory->hRmHandle,
                0,
                memory->size,
                ppLinearAddress,
                flags);

    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to Map RM memory object 0x%x allocated for NVKMemory 0x%p",
            memory->hRmHandle, memory);
        if (type == NVKMS_KAPI_MAPPING_TYPE_USER) {
            RmGc6BlockerRefCntDec(device); // XXX Can't handle failure.
        }
        return NV_FALSE;
    }

    return NV_TRUE;
}

static void UnmapMemory
(
    const struct NvKmsKapiDevice *device,
    const struct NvKmsKapiMemory *memory, NvKmsKapiMappingType type,
    const void *pLinearAddress
)
{
    NV_STATUS status;
    NvU32 flags = 0;

    if (device == NULL || memory == NULL) {
        return;
    }

    switch (type) {
        case NVKMS_KAPI_MAPPING_TYPE_USER:
            flags |= DRF_DEF(OS33, _FLAGS, _MEM_SPACE, _USER);
            break;
        case NVKMS_KAPI_MAPPING_TYPE_KERNEL:
            break;
    }

    status =
        nvRmApiUnmapMemory(device->hRmClient,
                           device->hRmSubDevice,
                           memory->hRmHandle,
                           pLinearAddress,
                           flags);

    if (status != NV_OK) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to Ummap RM memory object 0x%x allocated for NVKMemory 0x%p",
            memory->hRmHandle, memory);
    }

    if (type == NVKMS_KAPI_MAPPING_TYPE_USER) {
        RmGc6BlockerRefCntDec(device); // XXX Can't handle failure.
    }
}

static NvBool GetSurfaceParams(
    struct NvKmsKapiCreateSurfaceParams *params,
    NvU32 *pNumPlanes,
    enum NvKmsSurfaceMemoryLayout *pLayout,
    NvU32 *pLog2GobsPerBlockY,
    NvU32 pitch[])
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(params->format);
    enum NvKmsSurfaceMemoryLayout layout = NvKmsSurfaceMemoryLayoutPitch;
    NvU32 log2GobsPerBlockY = 0;
    NvU32 i;

    if (pFormatInfo->numPlanes == 0)
    {
        nvKmsKapiLogDebug("Unknown surface format");
        return NV_FALSE;
    }

    for (i = 0; i < pFormatInfo->numPlanes; i++) {
        struct NvKmsKapiMemory *memory =
                                params->planes[i].memory;

        if (memory == NULL) {
            return FALSE;
        }

        pitch[i] = params->planes[i].pitch;

        if (i == 0) {
            if (params->explicit_layout) {
                layout = params->layout;
            } else {
                layout = memory->surfaceParams.layout;
            }

            switch (layout) {
            case NvKmsSurfaceMemoryLayoutBlockLinear:
                if (params->explicit_layout) {
                    log2GobsPerBlockY = params->log2GobsPerBlockY;
                } else {
                    log2GobsPerBlockY =
                        memory->surfaceParams.blockLinear.log2GobsPerBlock.y;
                }
                break;

            case NvKmsSurfaceMemoryLayoutPitch:
                log2GobsPerBlockY = 0;
                break;

            default:
                nvKmsKapiLogDebug("Invalid surface layout: %u", layout);
                return NV_FALSE;
            }
        } else {
            if (!params->explicit_layout) {
                if (layout != memory->surfaceParams.layout) {
                    nvKmsKapiLogDebug("All planes are not of same layout");
                    return FALSE;
                }

                if (layout == NvKmsSurfaceMemoryLayoutBlockLinear &&
                    log2GobsPerBlockY !=
                    memory->surfaceParams.blockLinear.log2GobsPerBlock.y) {

                    nvKmsKapiLogDebug(
                        "All planes do not have the same blocklinear parameters");
                    return FALSE;
                }
            }
        }

        if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
            if (params->explicit_layout) {
                if (pitch[i] & 63) {
                    nvKmsKapiLogDebug(
                        "Invalid block-linear pitch alignment: %u", pitch[i]);
                    return NV_FALSE;
                }

                pitch[i] = pitch[i] >> 6;
            } else {
                /*
                 * The caller (nvidia-drm) is not blocklinear-aware, so the
                 * passed-in pitch cannot accurately reflect block information.
                 * Override the pitch with what was specified when the surface
                 * was imported.
                 */
                pitch[i] = memory->surfaceParams.blockLinear.pitchInBlocks;
            }
        } else {
            pitch[i] = params->planes[i].pitch;
        }

    }

    *pNumPlanes = pFormatInfo->numPlanes;
    *pLayout = layout;
    *pLog2GobsPerBlockY = log2GobsPerBlockY;

    return NV_TRUE;
}
static struct NvKmsKapiSurface* CreateSurface
(
    struct NvKmsKapiDevice *device,
    struct NvKmsKapiCreateSurfaceParams *params
)
{
    struct NvKmsRegisterSurfaceParams paramsReg = { };
    NvBool status;

    struct NvKmsKapiSurface *surface = NULL;

    enum NvKmsSurfaceMemoryLayout layout = NvKmsSurfaceMemoryLayoutPitch;
    NvU32 log2GobsPerBlockY = 0;
    NvU32 numPlanes = 0;
    NvU32 pitch[NVKMS_MAX_PLANES_PER_SURFACE] = { 0 };
    NvU32 i;

    if (!GetSurfaceParams(params,
                          &numPlanes,
                          &layout,
                          &log2GobsPerBlockY,
                          pitch))
    {
        goto failed;
    }

    surface = nvKmsKapiCalloc(1, sizeof(*surface));

    if (surface == NULL) {
        nvKmsKapiLogDebug(
            "Failed to allocate memory for NVKMS surface object on "
            "NvKmsKapiDevice 0x%p",
            device);
        goto failed;
    }

    if (device->hKmsDevice == 0x0) {
        goto done;
    }

    /* Create NVKMS surface */

    paramsReg.request.deviceHandle = device->hKmsDevice;

    paramsReg.request.useFd        = FALSE;
    paramsReg.request.rmClient     = device->hRmClient;

    paramsReg.request.widthInPixels  = params->width;
    paramsReg.request.heightInPixels = params->height;

    paramsReg.request.format = params->format;

    paramsReg.request.layout = layout;
    paramsReg.request.log2GobsPerBlockY = log2GobsPerBlockY;

    for (i = 0; i < numPlanes; i++) {
        struct NvKmsKapiMemory *memory =
                                params->planes[i].memory;

        paramsReg.request.planes[i].u.rmObject = memory->hRmHandle;
        paramsReg.request.planes[i].rmObjectSizeInBytes = memory->size;
        paramsReg.request.planes[i].offset = params->planes[i].offset;
        paramsReg.request.planes[i].pitch = pitch[i];
    }

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_REGISTER_SURFACE,
                                   &paramsReg, sizeof(paramsReg));
    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to register NVKMS surface of dimensions %ux%u "
            "and %s format",
            params->width,
            params->height,
            nvKmsSurfaceMemoryFormatToString(params->format));

        goto failed;
    }

    surface->hKmsHandle = paramsReg.reply.surfaceHandle;

done:
    return surface;

failed:
    nvKmsKapiFree(surface);

    return NULL;
}

static void DestroySurface
(
    struct NvKmsKapiDevice *device, struct NvKmsKapiSurface *surface
)
{
    struct NvKmsUnregisterSurfaceParams paramsUnreg = { };
    NvBool status;

    if (device->hKmsDevice == 0x0) {
        goto done;
    }

    paramsUnreg.request.deviceHandle  = device->hKmsDevice;
    paramsUnreg.request.surfaceHandle = surface->hKmsHandle;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_UNREGISTER_SURFACE,
                                   &paramsUnreg, sizeof(paramsUnreg));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to unregister NVKMS surface registered for "
            "NvKmsKapiSurface 0x%p",
            surface);
    }

done:
    nvKmsKapiFree(surface);
}

/*
 * Helper function to convert NvKmsMode to NvKmsKapiDisplayMode.
 */
static void NvKmsModeToKapi
(
    const struct NvKmsMode *kmsMode,
    struct NvKmsKapiDisplayMode *mode
)
{
    const NvModeTimings *timings = &kmsMode->timings;

    nvkms_memset(mode, 0, sizeof(*mode));

    mode->timings.refreshRate   = timings->RRx1k;
    mode->timings.pixelClockHz  = timings->pixelClockHz;
    mode->timings.hVisible      = timings->hVisible;
    mode->timings.hSyncStart    = timings->hSyncStart;
    mode->timings.hSyncEnd      = timings->hSyncEnd;
    mode->timings.hTotal        = timings->hTotal;
    mode->timings.hSkew         = timings->hSkew;
    mode->timings.vVisible      = timings->vVisible;
    mode->timings.vSyncStart    = timings->vSyncStart;
    mode->timings.vSyncEnd      = timings->vSyncEnd;
    mode->timings.vTotal        = timings->vTotal;

    mode->timings.flags.interlaced = timings->interlaced;
    mode->timings.flags.doubleScan = timings->doubleScan;
    mode->timings.flags.hSyncPos   = timings->hSyncPos;
    mode->timings.flags.hSyncNeg   = timings->hSyncNeg;
    mode->timings.flags.vSyncPos   = timings->vSyncPos;
    mode->timings.flags.vSyncNeg   = timings->vSyncNeg;

    mode->timings.widthMM  = timings->sizeMM.w;
    mode->timings.heightMM = timings->sizeMM.h;

    ct_assert(sizeof(mode->name) == sizeof(kmsMode->name));

    nvkms_memcpy(mode->name, kmsMode->name, sizeof(mode->name));
}

static void InitNvKmsModeValidationParams(
    const struct NvKmsKapiDevice *device,
    struct NvKmsModeValidationParams *params)
{
    /*
     * Mode timings structures of KAPI clients may not have field like
     * RRx1k, it does not guarantee that computed RRx1k value during
     * conversion from -
     *     KAPI client's mode-timings structure
     *         -> NvKmsKapiDisplayMode -> NvModeTimings
     * is same as what we get from edid, this may cause mode-set to fail.
     *
     * The RRx1k filed don't impact hardware modetiming values, therefore
     * override RRx1k check.
     *
     * XXX NVKMS TODO: Bug 200156338 is filed to delete NvModeTimings::RRx1k
     * if possible.
     */
    params->overrides = NVKMS_MODE_VALIDATION_NO_RRX1K_CHECK;
}

static int GetDisplayMode
(
    struct NvKmsKapiDevice *device,
    NvKmsKapiDisplay display, NvU32 modeIndex,
    struct NvKmsKapiDisplayMode *mode, NvBool *valid,
    NvBool *preferredMode
)
{
    struct NvKmsValidateModeIndexParams paramsValidate = { };
    NvBool status;

    if (device == NULL) {
        return -1;
    }

    paramsValidate.request.deviceHandle = device->hKmsDevice;
    paramsValidate.request.dispHandle   = device->hKmsDisp;

    paramsValidate.request.dpyId = nvNvU32ToDpyId(display);

    InitNvKmsModeValidationParams(device,
                                  &paramsValidate.request.modeValidation);

    paramsValidate.request.modeIndex = modeIndex;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_VALIDATE_MODE_INDEX,
                                   &paramsValidate, sizeof(paramsValidate));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to get validated mode index 0x%x for NvKmsKapiDisplay 0x%08x",
            modeIndex, display);
        return -1;
    }

    if (mode != NULL) {
        NvKmsModeToKapi(&paramsValidate.reply.mode, mode);
    }


    if (valid != NULL) {
        *valid = paramsValidate.reply.valid;
    }

    if (preferredMode != NULL) {
        *preferredMode = paramsValidate.reply.preferredMode;
    }

    return paramsValidate.reply.end ? 0 : 1;
}

/*
 * Helper function to convert NvKmsKapiDisplayMode to NvKmsMode.
 */
static void NvKmsKapiDisplayModeToKapi
(
    const struct NvKmsKapiDisplayMode *mode,
    struct NvKmsMode *kmsMode
)
{
    NvModeTimings *timings = &kmsMode->timings;

    nvkms_memset(kmsMode, 0, sizeof(*kmsMode));

    nvkms_memcpy(kmsMode->name, mode->name, sizeof(mode->name));

    timings->RRx1k         = mode->timings.refreshRate;
    timings->pixelClockHz  = mode->timings.pixelClockHz;
    timings->hVisible      = mode->timings.hVisible;
    timings->hSyncStart    = mode->timings.hSyncStart;
    timings->hSyncEnd      = mode->timings.hSyncEnd;
    timings->hTotal        = mode->timings.hTotal;
    timings->hSkew         = mode->timings.hSkew;
    timings->vVisible      = mode->timings.vVisible;
    timings->vSyncStart    = mode->timings.vSyncStart;
    timings->vSyncEnd      = mode->timings.vSyncEnd;
    timings->vTotal        = mode->timings.vTotal;

    timings->interlaced    = mode->timings.flags.interlaced;
    timings->doubleScan    = mode->timings.flags.doubleScan;
    timings->hSyncPos      = mode->timings.flags.hSyncPos;
    timings->hSyncNeg      = mode->timings.flags.hSyncNeg;
    timings->vSyncPos      = mode->timings.flags.vSyncPos;
    timings->vSyncNeg      = mode->timings.flags.vSyncNeg;

    timings->sizeMM.w = mode->timings.widthMM;
    timings->sizeMM.h = mode->timings.heightMM;
}

static NvBool ValidateDisplayMode
(
    struct NvKmsKapiDevice *device,
    NvKmsKapiDisplay display, const struct NvKmsKapiDisplayMode *mode
)
{
    struct NvKmsValidateModeParams paramsValidate;
    NvBool status;

    if (device == NULL) {
        return NV_FALSE;
    }

    nvkms_memset(&paramsValidate, 0, sizeof(paramsValidate));

    paramsValidate.request.deviceHandle = device->hKmsDevice;
    paramsValidate.request.dispHandle   = device->hKmsDisp;

    paramsValidate.request.dpyId = nvNvU32ToDpyId(display);

    InitNvKmsModeValidationParams(device,
                                  &paramsValidate.request.modeValidation);


    NvKmsKapiDisplayModeToKapi(mode, &paramsValidate.request.mode);

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_VALIDATE_MODE,
                                   &paramsValidate, sizeof(paramsValidate));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "Failed to get validated mode %ux%u@%uHz for NvKmsKapiDisplay 0x%08x of "
            "NvKmsKapiDevice 0x%p",
            mode->timings.hVisible, mode->timings.vVisible,
            mode->timings.refreshRate/1000, display,
            device);
        return NV_FALSE;
    }

    return paramsValidate.reply.valid;
}

static NvBool AssignSyncObjectConfig(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiLayerConfig *pLayerConfig,
    struct NvKmsChannelSyncObjects *pSyncObject)
{
    if (!device->supportsSyncpts) {
        if (pLayerConfig->syncptParams.preSyncptSpecified ||
            pLayerConfig->syncptParams.postSyncptRequested) {
            return NV_FALSE;
        }
    }

    pSyncObject->useSyncpt = FALSE;

    if (pLayerConfig->syncptParams.preSyncptSpecified) {
        pSyncObject->useSyncpt = TRUE;

        pSyncObject->u.syncpts.pre.type = NVKMS_SYNCPT_TYPE_RAW;
        pSyncObject->u.syncpts.pre.u.raw.id = pLayerConfig->syncptParams.preSyncptId;
        pSyncObject->u.syncpts.pre.u.raw.value = pLayerConfig->syncptParams.preSyncptValue;
    }

    if (pLayerConfig->syncptParams.postSyncptRequested) {
        pSyncObject->useSyncpt = TRUE;

        pSyncObject->u.syncpts.requestedPostType = NVKMS_SYNCPT_TYPE_FD;
    }
    return NV_TRUE;
}

static void AssignHDRMetadataConfig(
    const struct NvKmsKapiLayerConfig *layerConfig,
    const NvU32 layer,
    struct NvKmsFlipCommonParams *params)
{
    if (layerConfig->hdrMetadataSpecified) {
        params->layer[layer].hdr.enabled = TRUE;
        params->layer[layer].hdr.specified = TRUE;
        params->layer[layer].hdr.staticMetadata = layerConfig->hdrMetadata;
    } else {
        params->layer[layer].hdr.enabled = FALSE;
        params->layer[layer].hdr.specified = TRUE;
    }
}

static void NvKmsKapiCursorConfigToKms(
    const struct NvKmsKapiCursorRequestedConfig *requestedConfig,
    struct NvKmsFlipCommonParams *params,
    NvBool bFromKmsSetMode)
{
    if (requestedConfig->flags.surfaceChanged || bFromKmsSetMode) {
        params->cursor.imageSpecified = NV_TRUE;

        if (requestedConfig->surface != NULL) {
            params->cursor.image.surfaceHandle[NVKMS_LEFT] =
                requestedConfig->surface->hKmsHandle;
        }

        params->cursor.image.cursorCompParams.colorKeySelect =
            NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE;
        params->cursor.image.cursorCompParams.blendingMode[1] =
            requestedConfig->compParams.compMode;
        params->cursor.image.cursorCompParams.surfaceAlpha =
            requestedConfig->compParams.surfaceAlpha;
    }

    if (requestedConfig->flags.dstXYChanged || bFromKmsSetMode) {
        params->cursor.position.x = requestedConfig->dstX;
        params->cursor.position.y = requestedConfig->dstY;

        params->cursor.positionSpecified = NV_TRUE;
    }
}

static NvBool NvKmsKapiOverlayLayerConfigToKms(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiLayerRequestedConfig *layerRequestedConfig,
    const NvU32 layer,
    const NvU32 head,
    struct NvKmsFlipCommonParams *params,
    NvBool commit,
    NvBool bFromKmsSetMode)
{
    NvBool ret = NV_FALSE;
    const struct NvKmsKapiLayerConfig *layerConfig =
        &layerRequestedConfig->config;

    if (layerRequestedConfig->flags.surfaceChanged || bFromKmsSetMode) {
       params->layer[layer].syncObjects.specified = NV_TRUE;
       params->layer[layer].completionNotifier.specified = NV_TRUE;
       params->layer[layer].surface.specified = NV_TRUE;

        if (layerConfig->surface != NULL) {
            params->layer[layer].surface.handle[NVKMS_LEFT] =
                layerConfig->surface->hKmsHandle;
        }

        params->layer[layer].surface.rrParams =
            layerConfig->rrParams;

        params->layer[layer].compositionParams.val.colorKeySelect =
            NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE;
        params->layer[layer].compositionParams.val.blendingMode[1] =
            layerConfig->compParams.compMode;
        params->layer[layer].compositionParams.val.surfaceAlpha =
            layerConfig->compParams.surfaceAlpha;
        params->layer[layer].compositionParams.specified = TRUE;
        params->layer[layer].minPresentInterval =
            layerConfig->minPresentInterval;
    }

    params->layer[layer].sizeIn.val.width = layerConfig->srcWidth;
    params->layer[layer].sizeIn.val.height = layerConfig->srcHeight;
    params->layer[layer].sizeIn.specified = TRUE;

    params->layer[layer].sizeOut.val.width = layerConfig->dstWidth;
    params->layer[layer].sizeOut.val.height = layerConfig->dstHeight;
    params->layer[layer].sizeOut.specified = TRUE;

    if (layerRequestedConfig->flags.dstXYChanged || bFromKmsSetMode) {
        params->layer[layer].outputPosition.val.x = layerConfig->dstX;
        params->layer[layer].outputPosition.val.y = layerConfig->dstY;

        params->layer[layer].outputPosition.specified = NV_TRUE;
    }

    params->layer[layer].colorspace.val = layerConfig->inputColorSpace;
    params->layer[layer].colorspace.specified = TRUE;

    AssignHDRMetadataConfig(layerConfig, layer, params);

    if (commit) {
        NvU32 nextIndex = NVKMS_KAPI_INC_NOTIFIER_INDEX(
                device->layerState[head][layer].
                currFlipNotifierIndex);

        if (layerConfig->surface != NULL) {
            NvU32 nextIndexOffsetInBytes =
                NVKMS_KAPI_NOTIFIER_OFFSET(head,
                        layer, nextIndex);

            params->layer[layer].completionNotifier.val.
                surface.surfaceHandle = device->notifier.hKmsHandle;

            params->layer[layer].completionNotifier.val.
                surface.format = device->notifier.format;

            params->layer[layer].completionNotifier.val.
                surface.offsetInWords = nextIndexOffsetInBytes >> 2;

            params->layer[layer].completionNotifier.val.awaken = NV_TRUE;
        }

        ret = AssignSyncObjectConfig(device,
                                     layerConfig,
                                     &params->layer[layer].syncObjects.val);
        if (ret == NV_FALSE) {
            return ret;
        }

        /*
         * XXX Should this be done after commit?
         * What if commit fail?
         *
         * It is not expected to fail any commit in KAPI layer,
         * only validated configuration is expected
         * to commit.
         */
        device->layerState[head][layer].
            currFlipNotifierIndex = nextIndex;
    }

    return NV_TRUE;
}

static NvBool NvKmsKapiPrimaryLayerConfigToKms(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiLayerRequestedConfig *layerRequestedConfig,
    const NvU32 head,
    struct NvKmsFlipCommonParams *params,
    NvBool commit,
    NvBool bFromKmsSetMode)
{
    NvBool ret = NV_FALSE;
    const struct NvKmsKapiLayerConfig *layerConfig =
        &layerRequestedConfig->config;

    NvBool changed = FALSE;

    if (layerRequestedConfig->flags.surfaceChanged || bFromKmsSetMode) {
        params->layer[NVKMS_MAIN_LAYER].surface.specified = NV_TRUE;
        params->layer[NVKMS_MAIN_LAYER].completionNotifier.specified = NV_TRUE;
        params->layer[NVKMS_MAIN_LAYER].syncObjects.specified = NV_TRUE;


        params->layer[NVKMS_MAIN_LAYER].minPresentInterval =
            layerConfig->minPresentInterval;
        params->layer[NVKMS_MAIN_LAYER].tearing = layerConfig->tearing;
        params->layer[NVKMS_MAIN_LAYER].surface.rrParams = layerConfig->rrParams;

        if (layerConfig->surface != NULL) {
            params->layer[NVKMS_MAIN_LAYER].surface.handle[0] =
                layerConfig->surface->hKmsHandle;

            if (params->layer[NVKMS_MAIN_LAYER].surface.handle[0] != 0) {
                params->layer[NVKMS_MAIN_LAYER].sizeIn.val.width = layerConfig->srcWidth;
                params->layer[NVKMS_MAIN_LAYER].sizeIn.val.height = layerConfig->srcHeight;
                params->layer[NVKMS_MAIN_LAYER].sizeIn.specified = TRUE;

                params->layer[NVKMS_MAIN_LAYER].sizeOut.val.width = layerConfig->dstWidth;
                params->layer[NVKMS_MAIN_LAYER].sizeOut.val.height = layerConfig->dstHeight;
                params->layer[NVKMS_MAIN_LAYER].sizeOut.specified = TRUE;
            }
        }

        changed = TRUE;
    }

    if (layerRequestedConfig->flags.srcXYChanged || bFromKmsSetMode) {
        params->viewPortIn.point.x = layerConfig->srcX;
        params->viewPortIn.point.y = layerConfig->srcY;
        params->viewPortIn.specified = NV_TRUE;

        changed = TRUE;
    }

    params->layer[NVKMS_MAIN_LAYER].colorspace.val = layerConfig->inputColorSpace;
    params->layer[NVKMS_MAIN_LAYER].colorspace.specified = TRUE;

    AssignHDRMetadataConfig(layerConfig, NVKMS_MAIN_LAYER, params);

    if (commit && changed) {
        NvU32 nextIndex = NVKMS_KAPI_INC_NOTIFIER_INDEX(
            device->layerState[head][NVKMS_MAIN_LAYER].
            currFlipNotifierIndex);

        if (layerConfig->surface != NULL) {
            NvU32 nextIndexOffsetInBytes =
                NVKMS_KAPI_NOTIFIER_OFFSET(head,
                                           NVKMS_MAIN_LAYER, nextIndex);

            params->layer[NVKMS_MAIN_LAYER].completionNotifier.
                val.surface.surfaceHandle = device->notifier.hKmsHandle;

            params->layer[NVKMS_MAIN_LAYER].completionNotifier.
                val.surface.format = device->notifier.format;

            params->layer[NVKMS_MAIN_LAYER].completionNotifier.
                val.surface.offsetInWords = nextIndexOffsetInBytes >> 2;

            params->layer[NVKMS_MAIN_LAYER].completionNotifier.val.awaken = NV_TRUE;
        }

        ret = AssignSyncObjectConfig(device,
                                     layerConfig,
                                     &params->layer[NVKMS_MAIN_LAYER].syncObjects.val);
        if (ret == NV_FALSE) {
            return ret;
        }

        /*
         * XXX Should this be done after commit?
         * What if commit fail?
         *
         * It is not expected to fail any commit in KAPI layer,
         * only validated configuration is expected
         * to commit.
         */
        device->layerState[head][NVKMS_MAIN_LAYER].
            currFlipNotifierIndex = nextIndex;
    }

    return NV_TRUE;
}

static NvBool NvKmsKapiLayerConfigToKms(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiLayerRequestedConfig *layerRequestedConfig,
    const NvU32 layer,
    const NvU32 head,
    struct NvKmsFlipCommonParams *params,
    NvBool commit,
    NvBool bFromKmsSetMode)
{
    if (layer == NVKMS_KAPI_LAYER_PRIMARY_IDX) {
        return NvKmsKapiPrimaryLayerConfigToKms(device,
                                                layerRequestedConfig,
                                                head,
                                                params,
                                                commit,
                                                bFromKmsSetMode);

    }

    return NvKmsKapiOverlayLayerConfigToKms(device,
                                            layerRequestedConfig,
                                            layer,
                                            head,
                                            params,
                                            commit,
                                            bFromKmsSetMode);
}

static NvBool GetOutputTransferFunction(
    const struct NvKmsKapiHeadRequestedConfig *headRequestedConfig,
    enum NvKmsOutputTf *tf)
{
    NvBool found = NV_FALSE;
    NvU32 layer;

    *tf = NVKMS_OUTPUT_TF_NONE;

    for (layer = 0;
         layer < ARRAY_LEN(headRequestedConfig->layerRequestedConfig);
         layer++) {
        const struct NvKmsKapiLayerRequestedConfig *layerRequestedConfig =
            &headRequestedConfig->layerRequestedConfig[layer];
        const struct NvKmsKapiLayerConfig *layerConfig =
            &layerRequestedConfig->config;

        if (layerConfig->hdrMetadataSpecified) {
            if (!found) {
                *tf = layerConfig->tf;
                found = NV_TRUE;
            } else if (*tf != layerConfig->tf) {
                nvKmsKapiLogDebug(
                    "Output transfer function should be the same for all layers on a head");
                return NV_FALSE;
            }
        }
    }

    return NV_TRUE;
}

/*
 * Helper function to convert NvKmsKapiRequestedModeSetConfig
 * to NvKmsSetModeParams.
 */
static NvBool NvKmsKapiRequestedModeSetConfigToKms(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
    struct NvKmsSetModeParams *params,
    NvBool commit)
{
    NvU32 dispIdx = device->dispIdx;
    NvU32 head;

    nvkms_memset(params, 0, sizeof(*params));

    params->request.commit = commit;
    params->request.deviceHandle = device->hKmsDevice;
    params->request.requestedDispsBitMask = 1 << dispIdx;

    for (head = 0;
         head < ARRAY_LEN(requestedConfig->headRequestedConfig); head++) {

        const struct NvKmsKapiHeadRequestedConfig *headRequestedConfig =
            &requestedConfig->headRequestedConfig[head];
        const struct NvKmsKapiHeadModeSetConfig *headModeSetConfig =
            &headRequestedConfig->modeSetConfig;
        struct NvKmsSetModeOneHeadRequest *paramsHead;
        enum NvKmsOutputTf tf;
        NvU32 layer;
        NvU32 i;

        if ((requestedConfig->headsMask & (1 << head)) == 0x0) {
            continue;
        }

        params->request.disp[dispIdx].requestedHeadsBitMask |= 1 << head;

        if (headModeSetConfig->numDisplays == 0) {
            continue;
        }

        if (params->request.commit && !headModeSetConfig->bActive) {
            continue;
        }

        paramsHead = &params->request.disp[dispIdx].head[head];

        InitNvKmsModeValidationParams(device,
                                      &paramsHead->modeValidationParams);

        for (i = 0; i < headModeSetConfig->numDisplays; i++) {
            paramsHead->dpyIdList = nvAddDpyIdToDpyIdList(
                    nvNvU32ToDpyId(headModeSetConfig->displays[i]),
                    paramsHead->dpyIdList);
        }

        NvKmsKapiDisplayModeToKapi(&headModeSetConfig->mode, &paramsHead->mode);

        NvKmsKapiCursorConfigToKms(&headRequestedConfig->cursorRequestedConfig,
                                   &paramsHead->flip,
                                   NV_TRUE /* bFromKmsSetMode */);
        for (layer = 0;
             layer < ARRAY_LEN(headRequestedConfig->layerRequestedConfig);
             layer++) {

            const struct NvKmsKapiLayerRequestedConfig *layerRequestedConfig =
                &headRequestedConfig->layerRequestedConfig[layer];

            if (!NvKmsKapiLayerConfigToKms(device,
                                           layerRequestedConfig,
                                           layer,
                                           head,
                                           &paramsHead->flip,
                                           commit,
                                           NV_TRUE /* bFromKmsSetMode */)) {
                return NV_FALSE;
            }
        }

        if (!GetOutputTransferFunction(headRequestedConfig, &tf)) {
            return NV_FALSE;
        }

        paramsHead->flip.tf.val = tf;
        paramsHead->flip.tf.specified = NV_TRUE;

        paramsHead->viewPortSizeIn.width =
            headModeSetConfig->mode.timings.hVisible;
        paramsHead->viewPortSizeIn.height =
            headModeSetConfig->mode.timings.vVisible;

        if (device->caps.requiresVrrSemaphores) {
            paramsHead->allowGsync = NV_FALSE;
            paramsHead->allowAdaptiveSync = NVKMS_ALLOW_ADAPTIVE_SYNC_DISABLED;
        } else {
            paramsHead->allowGsync = NV_TRUE;
            paramsHead->allowAdaptiveSync = NVKMS_ALLOW_ADAPTIVE_SYNC_ALL;
        }
    }

    return NV_TRUE;
}


static NvBool KmsSetMode(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
    const NvBool commit)
{
    struct NvKmsSetModeParams *params = NULL;
    NvBool status = NV_FALSE;

    params = nvKmsKapiCalloc(1, sizeof(*params));

    if (params == NULL) {
        goto done;
    }

    if (!NvKmsKapiRequestedModeSetConfigToKms(device,
                                              requestedConfig,
                                              params,
                                              commit)) {
        goto done;
    }

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_SET_MODE,
                                   params, sizeof(*params));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "NVKMS_IOCTL_SET_MODE ioctl failed");
        goto done;
    }

    if (params->reply.status != NVKMS_SET_MODE_STATUS_SUCCESS)
    {
        int i;

        nvKmsKapiLogDeviceDebug(
            device,
            "NVKMS_IOCTL_SET_MODE failed!  Status:\n");

        nvKmsKapiLogDeviceDebug(
            device,
            "  top-level status: %d\n", params->reply.status);

        nvKmsKapiLogDeviceDebug(
            device,
            "  disp0 status: %d\n", params->reply.disp[0].status);

        for (i = 0; i < ARRAY_LEN(params->reply.disp[0].head); i++)
        {
            nvKmsKapiLogDeviceDebug(
                device,
                "    head%d status: %d\n",
                i, params->reply.disp[0].head[i].status);
        }

        status = NV_FALSE;
    }

done:

    if (params != NULL) {
        nvKmsKapiFree(params);
    }

    return status;
}

static NvBool IsHeadConfigValid(
    const struct NvKmsFlipParams *params,
    const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
    const struct NvKmsKapiHeadModeSetConfig *headModeSetConfig,
    NvU32 head)
{
    if ((requestedConfig->headsMask & (1 << head)) == 0x0) {
        return NV_FALSE;
    }

    if (headModeSetConfig->numDisplays == 0) {
        return NV_FALSE;
    }

    if (params->request.commit && !headModeSetConfig->bActive) {
        return NV_FALSE;
    }
    return NV_TRUE;
}

static NvBool KmsFlip(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
    struct NvKmsKapiModeSetReplyConfig *replyConfig,
    const NvBool commit)
{
    struct NvKmsFlipParams *params = NULL;
    struct NvKmsFlipRequestOneHead *pFlipHead = NULL;
    NvBool status = NV_TRUE;
    NvU32 i, head;

    /* Allocate space for the params structure, plus space for each possible
     * head. */
    params = nvKmsKapiCalloc(1,
            sizeof(*params) + sizeof(pFlipHead[0]) * NVKMS_KAPI_MAX_HEADS);

    if (params == NULL) {
        return NV_FALSE;
    }

    /* The flipHead array was allocated in the same block above. */
    pFlipHead = (struct NvKmsFlipRequestOneHead *)(params + 1);

    params->request.deviceHandle = device->hKmsDevice;
    params->request.commit = commit;
    params->request.allowVrr = NV_FALSE;
    params->request.pFlipHead = nvKmsPointerToNvU64(pFlipHead);
    params->request.numFlipHeads = 0;
    for (head = 0;
         head < ARRAY_LEN(requestedConfig->headRequestedConfig); head++) {

        const struct NvKmsKapiHeadRequestedConfig *headRequestedConfig =
            &requestedConfig->headRequestedConfig[head];
        const struct NvKmsKapiHeadModeSetConfig *headModeSetConfig =
            &headRequestedConfig->modeSetConfig;
        enum NvKmsOutputTf tf;

        struct NvKmsFlipCommonParams *flipParams = NULL;

        NvU32 layer;

        if (!IsHeadConfigValid(params, requestedConfig, headModeSetConfig, head)) {
            continue;
        }

        pFlipHead[params->request.numFlipHeads].sd = 0;
        pFlipHead[params->request.numFlipHeads].head = head;
        flipParams = &pFlipHead[params->request.numFlipHeads].flip;
        params->request.numFlipHeads++;

        NvKmsKapiCursorConfigToKms(&headRequestedConfig->cursorRequestedConfig,
                                   flipParams,
                                   NV_FALSE /* bFromKmsSetMode */);

        for (layer = 0;
             layer < ARRAY_LEN(headRequestedConfig->layerRequestedConfig);
             layer++) {

            const struct NvKmsKapiLayerRequestedConfig
                *layerRequestedConfig =
                 &headRequestedConfig->layerRequestedConfig[layer];

            status = NvKmsKapiLayerConfigToKms(device,
                                               layerRequestedConfig,
                                               layer,
                                               head,
                                               flipParams,
                                               commit,
                                               NV_FALSE /* bFromKmsSetMode */);

            if (status != NV_TRUE) {
                goto done;
            }
        }

        status = GetOutputTransferFunction(headRequestedConfig, &tf);
        if (status != NV_TRUE) {
            goto done;
        }

        flipParams->tf.val = tf;
        flipParams->tf.specified = NV_TRUE;

        if (headModeSetConfig->vrrEnabled) {
            params->request.allowVrr = NV_TRUE;
        }
    }

    if (params->request.numFlipHeads == 0) {
        goto done;
    }

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_FLIP,
                                   params, sizeof(*params));

    if (!status) {
        nvKmsKapiLogDeviceDebug(
            device,
            "NVKMS_IOCTL_FLIP ioctl failed");
        goto done;
    }

    if (!commit) {
        goto done;
    }

    /*! fill back flip reply */
    for (i = 0; i < params->request.numFlipHeads; i++) {
         const struct NvKmsKapiHeadRequestedConfig *headRequestedConfig =
            &requestedConfig->headRequestedConfig[pFlipHead[i].head];

         struct NvKmsKapiHeadReplyConfig *headReplyConfig =
            &replyConfig->headReplyConfig[pFlipHead[i].head];

         const struct NvKmsKapiHeadModeSetConfig *headModeSetConfig =
            &headRequestedConfig->modeSetConfig;

         struct NvKmsFlipCommonReplyOneHead *flipParams = &params->reply.flipHead[i];

         NvU32 layer;

         if (!IsHeadConfigValid(params, requestedConfig, headModeSetConfig, pFlipHead[i].head)) {
            continue;
         }

         for (layer = 0;
              layer < ARRAY_LEN(headRequestedConfig->layerRequestedConfig);
              layer++) {

              const struct NvKmsKapiLayerConfig *layerRequestedConfig =
                  &headRequestedConfig->layerRequestedConfig[layer].config;

              struct NvKmsKapiLayerReplyConfig *layerReplyConfig =
                  &headReplyConfig->layerReplyConfig[layer];

              /*! initialize explicitly to -1 as 0 is valid file descriptor */
              layerReplyConfig->postSyncptFd = -1;
              if (layerRequestedConfig->syncptParams.postSyncptRequested) {
                 layerReplyConfig->postSyncptFd =
                     flipParams->layer[layer].postSyncpt.u.fd;
              }
          }
      }

done:

    nvKmsKapiFree(params);

    return status;
}

static NvBool ApplyModeSetConfig(
    struct NvKmsKapiDevice *device,
    const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
    struct NvKmsKapiModeSetReplyConfig *replyConfig,
    const NvBool commit)
{
    NvBool bRequiredModeset = NV_FALSE;
    NvU32  head;

    if (device == NULL || requestedConfig == NULL) {
        return NV_FALSE;
    }

    for (head = 0;
         head < ARRAY_LEN(requestedConfig->headRequestedConfig); head++) {

        const struct NvKmsKapiHeadRequestedConfig *headRequestedConfig =
            &requestedConfig->headRequestedConfig[head];
        const struct NvKmsKapiHeadModeSetConfig *headModeSetConfig =
            &headRequestedConfig->modeSetConfig;

        if ((requestedConfig->headsMask & (1 << head)) == 0x0) {
            continue;
        }

        bRequiredModeset =
            headRequestedConfig->flags.activeChanged   ||
            headRequestedConfig->flags.displaysChanged ||
            headRequestedConfig->flags.modeChanged;

        /*
         * NVKMS flip ioctl could not validate flip configuration for an
         * inactive head, therefore use modeset ioctl if configuration contain
         * any such head.
         */
        if (!commit &&
            headModeSetConfig->numDisplays != 0 && !headModeSetConfig->bActive) {
            bRequiredModeset = TRUE;
        }

        if (bRequiredModeset) {
            break;
        }
    }

    if (bRequiredModeset) {
        return KmsSetMode(device, requestedConfig, commit);
    }

    return KmsFlip(device, requestedConfig, replyConfig, commit);
}

void nvKmsKapiHandleEventQueueChange
(
    struct NvKmsKapiDevice *device
)
{
    if (device == NULL) {
        return;
    }

    /*
     * If the callback is NULL, event interest declaration should be
     * rejected, and no events would be reported.
     */
    nvAssert(device->eventCallback != NULL);

    do
    {
        struct NvKmsGetNextEventParams kmsEventParams = { };
        struct NvKmsKapiEvent kapiEvent = { };
        NvBool err = NV_FALSE;

        if (!nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_GET_NEXT_EVENT,
                                   &kmsEventParams, sizeof(kmsEventParams))) {
            break;
        }

        if (!kmsEventParams.reply.valid) {
            break;
        }

        kapiEvent.type = kmsEventParams.reply.event.eventType;

        kapiEvent.device = device;
        kapiEvent.privateData = device->privateData;

        switch (kmsEventParams.reply.event.eventType) {
            case NVKMS_EVENT_TYPE_DPY_CHANGED:
                kapiEvent.u.displayChanged.display =
                    nvDpyIdToNvU32(kmsEventParams.
                                   reply.event.u.dpyChanged.dpyId);
                break;
            case NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED:
                kapiEvent.u.dynamicDisplayConnected.display =
                    nvDpyIdToNvU32(kmsEventParams.
                                   reply.event.u.dynamicDpyConnected.dpyId);
                break;
            case NVKMS_EVENT_TYPE_FLIP_OCCURRED:
                kapiEvent.u.flipOccurred.head =
                    kmsEventParams.reply.event.u.flipOccurred.head;
                kapiEvent.u.flipOccurred.layer =
                    kmsEventParams.reply.event.u.flipOccurred.layer;
                break;
            default:
                continue;
        }

        if (err) {
            nvKmsKapiLogDeviceDebug(
                device,
                "Error in conversion from "
                "NvKmsGetNextEventParams to NvKmsKapiEvent");
            continue;
        }

        device->eventCallback(&kapiEvent);

    } while(1);
}

/*
 * Helper function to convert NvKmsQueryDpyCRC32Reply to NvKmsKapiDpyCRC32.
 */
static void NvKmsCrcsToKapi
(
    const struct NvKmsQueryDpyCRC32Reply *crcs,
    struct NvKmsKapiCrcs *kmsCrcs
)
{
    kmsCrcs->outputCrc32.value = crcs->outputCrc32.value;
    kmsCrcs->outputCrc32.supported = crcs->outputCrc32.supported;
    kmsCrcs->rasterGeneratorCrc32.value = crcs->rasterGeneratorCrc32.value;
    kmsCrcs->rasterGeneratorCrc32.supported = crcs->rasterGeneratorCrc32.supported;
    kmsCrcs->compositorCrc32.value = crcs->compositorCrc32.value;
    kmsCrcs->compositorCrc32.supported = crcs->compositorCrc32.supported;
}

static NvBool GetCRC32
(
    struct NvKmsKapiDevice *device,
    NvU32 head,
    struct NvKmsKapiCrcs *crc32
)
{
    struct NvKmsQueryDpyCRC32Params params = { };
    NvBool status;

    if (device->hKmsDevice == 0x0) {
        return NV_TRUE;
    }

    params.request.deviceHandle = device->hKmsDevice;
    params.request.dispHandle   = device->hKmsDisp;
    params.request.head         = head;

    status = nvkms_ioctl_from_kapi(device->pKmsOpen,
                                   NVKMS_IOCTL_QUERY_DPY_CRC32,
                                   &params, sizeof(params));

    if (!status) {
        nvKmsKapiLogDeviceDebug(device, "NVKMS QueryDpyCRC32Data failed.");
        return NV_FALSE;
    }
    NvKmsCrcsToKapi(&params.reply, crc32);
    return NV_TRUE;
}

NvBool nvKmsKapiGetFunctionsTableInternal
(
    struct NvKmsKapiFunctionsTable *funcsTable
)
{
    if (funcsTable == NULL) {
        return NV_FALSE;
    }

    if (nvkms_strcmp(funcsTable->versionString, NV_VERSION_STRING) != 0) {
        funcsTable->versionString = NV_VERSION_STRING;
        return NV_FALSE;
    }

    funcsTable->systemInfo.bAllowWriteCombining =
        nvkms_allow_write_combining();

    funcsTable->enumerateGpus = EnumerateGpus;

    funcsTable->allocateDevice = AllocateDevice;
    funcsTable->freeDevice     = FreeDevice;

    funcsTable->grabOwnership    = GrabOwnership;
    funcsTable->releaseOwnership = ReleaseOwnership;

    funcsTable->grantPermissions     = GrantPermissions;
    funcsTable->revokePermissions    = RevokePermissions;

    funcsTable->declareEventInterest = DeclareEventInterest;

    funcsTable->getDeviceResourcesInfo = GetDeviceResourcesInfo;
    funcsTable->getDisplays            = GetDisplays;
    funcsTable->getConnectorInfo       = GetConnectorInfo;

    funcsTable->getStaticDisplayInfo   = GetStaticDisplayInfo;
    funcsTable->getDynamicDisplayInfo  = GetDynamicDisplayInfo;

    funcsTable->allocateVideoMemory  = AllocateVideoMemory;
    funcsTable->allocateSystemMemory = AllocateSystemMemory;
    funcsTable->importMemory         = ImportMemory;
    funcsTable->dupMemory            = DupMemory;
    funcsTable->exportMemory         = ExportMemory;
    funcsTable->freeMemory           = FreeMemory;
    funcsTable->getSystemMemoryHandleFromSgt = GetSystemMemoryHandleFromSgt;
    funcsTable->getSystemMemoryHandleFromDmaBuf =
        GetSystemMemoryHandleFromDmaBuf;

    funcsTable->mapMemory   = MapMemory;
    funcsTable->unmapMemory = UnmapMemory;

    funcsTable->createSurface  = CreateSurface;
    funcsTable->destroySurface = DestroySurface;

    funcsTable->getDisplayMode      = GetDisplayMode;
    funcsTable->validateDisplayMode = ValidateDisplayMode;

    funcsTable->applyModeSetConfig   = ApplyModeSetConfig;

    funcsTable->allocateChannelEvent = nvKmsKapiAllocateChannelEvent;
    funcsTable->freeChannelEvent = nvKmsKapiFreeChannelEvent;

    funcsTable->getCRC32 = GetCRC32;

    funcsTable->getMemoryPages = GetMemoryPages;
    funcsTable->freeMemoryPages = FreeMemoryPages;

    funcsTable->isMemoryValidForDisplay = IsMemoryValidForDisplay;

    return NV_TRUE;
}
