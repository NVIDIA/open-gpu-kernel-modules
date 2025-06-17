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

#include "nvkms-surface.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h"
#include "nvkms-flip.h"
#include "nvkms-private.h"
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-swapgroup.h"
#include "nvos.h"

// NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD
#include "ctrl/ctrl0000/ctrl0000unix.h"
// NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM
#include "ctrl/ctrl0000/ctrl0000client.h"

/* NV0041_CTRL_SURFACE_INFO */
#include "ctrl/ctrl0041.h"

/* NV01_MEMORY_SYSTEM_OS_DESCRIPTOR */
#include "class/cl0071.h"

static void CpuUnmapSurface(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo)
{
    const NvU32 planeIndex = 0;
    NvU32 sd;

    if (pSurfaceEvo->planes[planeIndex].rmHandle == 0) {
        return;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        if (pSurfaceEvo->cpuAddress[sd] != NULL) {
            nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[sd]->handle,
                               pSurfaceEvo->planes[planeIndex].rmHandle,
                               pSurfaceEvo->cpuAddress[sd],
                               0);
            pSurfaceEvo->cpuAddress[sd] = NULL;
        }
    }
}

NvBool nvEvoCpuMapSurface(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo)
{
    const NvU32 planeIndex = 0;
    NvU32 sd;

    /*
     * We should only be called here with surfaces that contain a single plane.
     */
    nvAssert(nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvo->format)->numPlanes == 1);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        NvU32 result = nvRmApiMapMemory(
                        nvEvoGlobal.clientHandle,
                        pDevEvo->pSubDevices[sd]->handle,
                        pSurfaceEvo->planes[planeIndex].rmHandle,
                        0,
                        pSurfaceEvo->planes[planeIndex].rmObjectSizeInBytes,
                        (void **) &pSurfaceEvo->cpuAddress[sd],
                        0);

        if (result != NVOS_STATUS_SUCCESS) {
            CpuUnmapSurface(pDevEvo, pSurfaceEvo);
            return FALSE;
        }
    }

    return TRUE;
}

static void FreeSurfaceEvoStruct(NVSurfaceEvoPtr pSurfaceEvo)
{
    if (pSurfaceEvo == NULL) {
        return;
    }

    nvAssert(!nvSurfaceEvoInAnyOpens(pSurfaceEvo));

    nvAssert(pSurfaceEvo->structRefCnt == 0);
    nvAssert(pSurfaceEvo->rmRefCnt == 0);

    nvFree(pSurfaceEvo);
}

static void FreeSurfaceEvoRm(NVDevEvoPtr pDevEvo, NVSurfaceEvoPtr pSurfaceEvo)
{
    NvU64 structRefCnt;
    NvU8 planeIndex;

    if ((pDevEvo == NULL) || (pSurfaceEvo == NULL)) {
        return;
    }

    nvAssert(pSurfaceEvo->rmRefCnt == 0);

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {
        pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                            nvEvoGlobal.clientHandle,
                                            &pSurfaceEvo->planes[planeIndex].surfaceDesc);
    }

    CpuUnmapSurface(pDevEvo, pSurfaceEvo);

    if (pSurfaceEvo->planes[0].rmHandle != 0) {
        nvHsUnmapSurfaceFromDevice(pDevEvo,
                                   pSurfaceEvo->planes[0].rmHandle,
                                   pSurfaceEvo->gpuAddress);
    }

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {

        if (pSurfaceEvo->planes[planeIndex].rmHandle == 0) {
            break;
        }

        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pSurfaceEvo->planes[planeIndex].rmHandle);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pSurfaceEvo->planes[planeIndex].rmHandle);

        pSurfaceEvo->planes[planeIndex].rmHandle = 0;
    }

    /*
     * The surface is now an orphan: clear the pSurfaceEvo, for
     * everything other than its structRefCnt.  The only operation
     * that can be done on it is unregistration.
     */
    structRefCnt = pSurfaceEvo->structRefCnt;
    nvkms_memset(pSurfaceEvo, 0, sizeof(*pSurfaceEvo));
    pSurfaceEvo->structRefCnt = structRefCnt;
}

void nvEvoIncrementSurfaceStructRefCnt(NVSurfaceEvoPtr pSurfaceEvo)
{
    nvAssert(!nvEvoSurfaceRefCntsTooLarge(pSurfaceEvo));

    pSurfaceEvo->structRefCnt++;
}

void nvEvoDecrementSurfaceStructRefCnt(NVSurfaceEvoPtr pSurfaceEvo)
{
    nvAssert(pSurfaceEvo->structRefCnt >= 1);
    pSurfaceEvo->structRefCnt--;

    if (pSurfaceEvo->structRefCnt == 0) {
        FreeSurfaceEvoStruct(pSurfaceEvo);
    }
}

static NvBool ValidatePlaneProperties(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsRegisterSurfaceRequest *pRequest)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(pRequest->format);
    NvU8 planeIndex;

    /*
     * Reject all registration requests for multi-planar NISO surfaces.
     * This is a non-sensical request.
     */
    if ((pRequest->isoType == NVKMS_MEMORY_NISO) &&
        (pFormatInfo->numPlanes > 1)) {
        return FALSE;
    }

    for (planeIndex = 0; planeIndex < pFormatInfo->numPlanes; planeIndex++) {

        const NvU64 planeOffset = pRequest->planes[planeIndex].offset;
        NvU64 planePitch = pRequest->planes[planeIndex].pitch;
        NvU64 rmObjectSizeInBytes =
            pRequest->planes[planeIndex].rmObjectSizeInBytes;
        NvU64 widthInBytes;
        NvU64 planeSizeInBytes;
        NvU32 planeEffectiveLines = pRequest->heightInPixels;
        NvU32 widthInPixels = pRequest->widthInPixels;

        if ((planePitch == 0U) || (rmObjectSizeInBytes == 0U))
        {
            nvEvoLog(EVO_LOG_ERROR, "Invalid request parameters, planePitch or rmObjectSizeInBytes, passed during surface registration");
            return FALSE;
        }

        if ((pRequest->isoType == NVKMS_MEMORY_ISO) &&
            ((planeEffectiveLines == 0U) || (widthInPixels == 0U)))
        {
            nvEvoLog(EVO_LOG_ERROR, "Invalid request parameters, heightInPixels or widthInPixels, passed during surface registration for ISO surfaces");
            return FALSE;
        }

        /* The offset must be 1KB-aligned. */
        if ((planeOffset &
            ((1 << NV_SURFACE_OFFSET_ALIGNMENT_SHIFT) - 1)) != 0) {
            return FALSE;
        }

        /*
         * Convert planePitch to units of bytes if it's currently specified in
         * units of blocks. Each block is 64-bytes wide.
         */
        if (pRequest->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
            planePitch <<= NVKMS_BLOCK_LINEAR_LOG_GOB_WIDTH;
        }

        /*
         * Convert width to bytes.
         */
        widthInBytes = widthInPixels;

        if (pFormatInfo->isYUV) {
            NvU8 divisor = 1;
            NvU8 bytesPerBlock = pFormatInfo->yuv.storageBitsPerComponent >> 3;

            switch (pFormatInfo->numPlanes) {
            case 3:
                /* planar */
                if (planeIndex > 0) {
                    divisor = pFormatInfo->yuv.horizChromaDecimationFactor;
                }
                break;

            case 2:
                /* semi-planar */
                if (planeIndex > 0) {
                    divisor = pFormatInfo->yuv.horizChromaDecimationFactor;
                    bytesPerBlock *= 2;
                }
                break;

            case 1:
                /* 4:2:2 packed */
                bytesPerBlock *= 2;
            }

            widthInBytes *= bytesPerBlock;
            /* Dimensions of decimated planes of odd-width YUV surfaces are
             * supposed to be rounded up */
            widthInBytes = (widthInBytes + (divisor - 1)) / divisor;
        } else {
            widthInBytes *= pFormatInfo->rgb.bytesPerPixel;
        }

        /*
         * Check that an entire line of pixels will fit in the pitch value
         * specified.
         */
        if (widthInBytes > planePitch) {
            return FALSE;
        }

        /*
         * Check that the entire memory region occupied by this plane falls
         * within the size of the underlying memory allocation.
         *
         * Force planeEffectiveLines to be even before dividing by
         * vertChromaDecimationFactor. The height of the source fetch rectangle
         * must be even anyways if there's vertical decimation.
         */
        if (planeIndex != 0 && pFormatInfo->isYUV &&
            pFormatInfo->yuv.vertChromaDecimationFactor > 1) {
            planeEffectiveLines = planeEffectiveLines & ~(0x1);
            planeEffectiveLines /= pFormatInfo->yuv.vertChromaDecimationFactor;
        }

        planeSizeInBytes = planeEffectiveLines * planePitch;

        if ((pRequest->isoType == NVKMS_MEMORY_ISO) &&
            (planeSizeInBytes == 0U))
        {
            nvEvoLog(EVO_LOG_ERROR, "Plane size calculated during ISO surface registration is 0");
            return FALSE;
        }

        if ((planeSizeInBytes > rmObjectSizeInBytes) ||
            (planeOffset > (rmObjectSizeInBytes - planeSizeInBytes))) {
            return FALSE;
        }
    }

    return TRUE;
}

static NvBool ValidateRegisterSurfaceRequest(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsRegisterSurfaceRequest *pRequest)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(pRequest->format);

    /*
     * The purpose of this check is to make sure the given format is valid and not
     * some garbage number. It exists to check for format validity in the case
     * where noDisplayHardWareAccess is TRUE.
     */
    if (pFormatInfo->depth == 0) {
        return FALSE;
    }

    /*
     * NvKmsSurfaceMemoryFormat has a few formats that we will never display.
     * Head surface has several formats it wants to texture from but we won't
     * (and can't) display surfaces with those formats. We should reject any
     * attempt to register a surface that is marked for display and uses one of
     * those formats.
     */
    if (!pRequest->noDisplayHardwareAccess) {
        /*
         * This isn't a perfect check since we can't predict which channel this
         * surface will be used on, but we should definitely reject a format if
         * it isn't usable on any channel.
         */
        NvBool usableOnAnyChannel = FALSE;
        NvU8 layer;

        for (layer = 0;
             layer < ARRAY_LEN(pDevEvo->caps.layerCaps);
             layer++) {

            if (NVBIT64(pRequest->format) &
                pDevEvo->caps.layerCaps[layer].supportedSurfaceMemoryFormats) {
                usableOnAnyChannel = TRUE;
                break;
            }
        }

        if (!usableOnAnyChannel) {
            return FALSE;
        }

        if (!pDevEvo->hal->ValidateWindowFormat(pRequest->format, NULL, NULL)) {
            return FALSE;
        }
    }

    if (!ValidatePlaneProperties(pDevEvo, pRequest)) {
        return FALSE;
    }

    /* XXX Validate surface properties. */

    return TRUE;
}

static NvBool ValidateSurfaceAddressSpace(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsRegisterSurfaceRequest *pRequest,
    NvU32 rmHandle)
{
    NV0041_CTRL_GET_SURFACE_INFO_PARAMS surfaceInfoParams = {};
    NV0041_CTRL_SURFACE_INFO surfaceInfo = {};
    NV_STATUS status;

    /*
     * Don't do these checks on tegra. Tegra has different capabilities.
     * Here we always say display is possible so we never fail framebuffer
     * creation.
     */
    if (pDevEvo->isSOCDisplay) {
        return TRUE;
    }

    /*
     * Don't do these checks for surfaces that do not need access to display
     * hardware.
     */
    if (pRequest->noDisplayHardwareAccess) {
        return TRUE;
    }

    /*
     * If the memory is not isochronous, the memory will not be scanned out to a
     * display. The checks are not needed for such memory types.
     */
    if (pRequest->isoType != NVKMS_MEMORY_ISO) {
        return TRUE;
    }

    /*
     * Check if the memory we are registering this surface with is valid. We
     * cannot scan out sysmem or compressed buffers.
     *
     * If we cannot use this memory for display it may be resident in sysmem
     * or may belong to another GPU.
     */
    surfaceInfo.index = NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE;

    surfaceInfoParams.surfaceInfoListSize = 1;
    surfaceInfoParams.surfaceInfoList = (NvP64)&surfaceInfo;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            rmHandle,
                            NV0041_CTRL_CMD_GET_SURFACE_INFO,
                            &surfaceInfoParams,
                            sizeof(surfaceInfoParams));
    if (status != NV_OK) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                          "Failed to get memory location of RM memory object 0x%x",
                          rmHandle);
        return FALSE;
    }

    if (surfaceInfo.data != NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Memory used for surface not appropriate for scanout");
        return FALSE;
    }

    return TRUE;
}

void nvEvoRegisterSurface(NVDevEvoPtr pDevEvo,
                          struct NvKmsPerOpenDev *pOpenDev,
                          struct NvKmsRegisterSurfaceParams *pParams,
                          enum NvHsMapPermissions hsMapPermissions)
{
    NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDev(pOpenDev);
    const struct NvKmsRegisterSurfaceRequest *pRequest = &pParams->request;
    NVSurfaceEvoPtr pSurfaceEvo = NULL;
    NvKmsSurfaceHandle surfaceHandle = 0;
    NvU32 result;
    NvU8 planeIndex;
    NvBool nisoMemory = (pRequest->isoType == NVKMS_MEMORY_NISO);

    /*
     * HeadSurface needs a CPU mapping of surfaces containing semaphores, in
     * order to check, from the CPU, if a semaphore-interlocked flip is ready.
     */
    const NvBool needCpuMapping = nisoMemory && pDevEvo->isHeadSurfaceSupported;

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    if (!ValidateRegisterSurfaceRequest(pDevEvo, pRequest)) {
        goto fail;
    }

    pSurfaceEvo = nvCalloc(1, sizeof(*pSurfaceEvo));

    if (pSurfaceEvo == NULL) {
        goto fail;
    }

    pSurfaceEvo->format = pRequest->format;

    surfaceHandle = nvEvoCreateApiHandle(pOpenDevSurfaceHandles, pSurfaceEvo);

    if (surfaceHandle == 0) {
        goto fail;
    }

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {

        const NvU32 planeRmHandle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (planeRmHandle == 0) {
            goto fail;
        }

        pSurfaceEvo->planes[planeIndex].rmHandle = planeRmHandle;

        if (pRequest->useFd) {
             /*
              * On T234, the 'fd' provided is allocated outside of RM whereas on
              * dGPU it is allocated by RM. So we check whether the fd is associated
              * with an nvidia character device, and if it is, then we consider that
              * it belongs to RM. Based on whether it belongs to RM or not we need
              * to call different mechanisms to import it.
              */
            if (nvkms_fd_is_nvidia_chardev(pRequest->planes[planeIndex].u.fd)) {
                NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS importParams = { };
                importParams.fd = pRequest->planes[planeIndex].u.fd;
                importParams.object.type = NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM;
                importParams.object.data.rmObject.hDevice = pDevEvo->deviceHandle;
                importParams.object.data.rmObject.hParent = pDevEvo->deviceHandle;
                importParams.object.data.rmObject.hObject = planeRmHandle;

                result = nvRmApiControl(nvEvoGlobal.clientHandle,
                                        nvEvoGlobal.clientHandle,
                                        NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD,
                                        &importParams,
                                        sizeof(importParams));
            } else {
                /*
                 * If 'fd' doesn't belongs to resman assume that it is allocated by
                 * some other dmabuf allocator (like nvmap).
                 */
                NV_OS_DESC_MEMORY_ALLOCATION_PARAMS allocParams = { };

                allocParams.type = NVOS32_TYPE_IMAGE;
                allocParams.descriptor =
                    (NvP64)(NvU64)(pRequest->planes[planeIndex].u.fd);
                allocParams.descriptorType = NVOS32_DESCRIPTOR_TYPE_OS_FILE_HANDLE;
                allocParams.limit = pRequest->planes[planeIndex].rmObjectSizeInBytes - 1;

                allocParams.attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _PCI,
                                               allocParams.attr);
                allocParams.attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE,
                                                _NO, allocParams.attr2);

                /*
                 * The NVKMS client performing the import doesn't know what the original
                 * CPU cache attributes are, so assume WRITE_BACK since we only need RM to
                 * IOVA map the memory into display's address space and the CPU cache
                 * attributes shouldn't really matter in this case.
                 */
                allocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY,
                                               _WRITE_BACK, allocParams.attr);
                allocParams.flags = NVOS32_ALLOC_FLAGS_MAP_NOT_REQUIRED;

                switch (pRequest->layout) {
                    case NvKmsSurfaceMemoryLayoutBlockLinear:
                        allocParams.attr =
                            FLD_SET_DRF(OS32, _ATTR, _FORMAT, _BLOCK_LINEAR,
                                        allocParams.attr);
                        break;

                    case NvKmsSurfaceMemoryLayoutPitch:
                        allocParams.attr =
                            FLD_SET_DRF(OS32, _ATTR, _FORMAT, _PITCH,
                                        allocParams.attr);
                        break;

                    default:
                        nvEvoLogDevDebug(pDevEvo, EVO_LOG_INFO, "Unknown layout");
                        goto fail;
                }

                if (!pRequest->noDisplayHardwareAccess) {
                    if (nisoMemory) {
                        allocParams.attr2 =
                            FLD_SET_DRF(OS32, _ATTR2, _NISO_DISPLAY, _YES,
                                        allocParams.attr2);
                    } else {
                        allocParams.attr2 =
                            FLD_SET_DRF(OS32, _ATTR2, _ISO, _YES,
                                        allocParams.attr2);
                    }
                }

                result = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                                      pDevEvo->deviceHandle,
                                      planeRmHandle,
                                      NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,
                                      &allocParams);

                /*
                 * Bug 200614156. RM doesn't support mapping osdesc objects into CPU’s
                 * address space.
                 */
                nvAssert(!needCpuMapping);
            }
        } else {
            /*
             * If 'useFd' is not specified, the (rmClient, rmObject) tuple from
             * the request is an object in the caller's RM client space.
             * Call RM to dup the memory into nvkms's RM client.
             */
            result = nvRmApiDupObject(nvEvoGlobal.clientHandle,
                                      pDevEvo->deviceHandle,
                                      planeRmHandle,
                                      pRequest->rmClient,
                                      pRequest->planes[planeIndex].u.rmObject,
                                      0);
        }

        if (result != NVOS_STATUS_SUCCESS) {
            goto fail;
        }

        if (!ValidateSurfaceAddressSpace(pDevEvo, pRequest, planeRmHandle)) {
            goto fail;
        }

        /* XXX Validate sizeInBytes: can we query the surface size from RM? */

        if (!pRequest->noDisplayHardwareAccess) {
            NvU32 ret =
                nvRmAllocAndBindSurfaceDescriptor(
                        pDevEvo,
                        planeRmHandle,
                        pRequest->layout,
                        pRequest->planes[planeIndex].rmObjectSizeInBytes - 1,
                        &pSurfaceEvo->planes[planeIndex].surfaceDesc);
            if (ret != NVOS_STATUS_SUCCESS) {
                goto fail;
            }
        }

        pSurfaceEvo->planes[planeIndex].pitch =
                            pRequest->planes[planeIndex].pitch;
        pSurfaceEvo->planes[planeIndex].offset =
                            pRequest->planes[planeIndex].offset;
        pSurfaceEvo->planes[planeIndex].rmObjectSizeInBytes =
                            pRequest->planes[planeIndex].rmObjectSizeInBytes;
    }

    pSurfaceEvo->requireDisplayHardwareAccess = !pRequest->noDisplayHardwareAccess;
    pSurfaceEvo->noDisplayCaching = pRequest->noDisplayCaching;

    /*
     * Map the surface into the GPU's virtual address space, for use with
     * headSurface.  If the surface may be used for semaphores, headSurface will
     * need to write to it through the graphics channel.  Force a writable GPU
     * mapping.
     *
     * Map the first plane of the surface only into the GPU's address space.
     * We would have already rejected multi-planar semaphore requests earlier.
     */
    if (nisoMemory) {
        hsMapPermissions = NvHsMapPermissionsReadWrite;
    }

    pSurfaceEvo->gpuAddress = nvHsMapSurfaceToDevice(
                    pDevEvo,
                    pSurfaceEvo->planes[0].rmHandle,
                    pRequest->planes[0].rmObjectSizeInBytes,
                    hsMapPermissions);

    if (pSurfaceEvo->gpuAddress == NV_HS_BAD_GPU_ADDRESS) {
        goto fail;
    }

    /*
     * Map the first plane of the surface only into the CPU's address space.
     * This is the only valid plane since we would have already rejected
     * multi-planar NISO surface requests earlier in
     *
     * nvEvoRegisterSurface() => ValidateRegisterSurfaceRequest() =>
     * ValidatePlaneProperties().
     */
    if (needCpuMapping) {
        if (!nvEvoCpuMapSurface(pDevEvo, pSurfaceEvo)) {
            goto fail;
        }
    }

    pSurfaceEvo->widthInPixels        = pRequest->widthInPixels;
    pSurfaceEvo->heightInPixels       = pRequest->heightInPixels;
    pSurfaceEvo->layout               = pRequest->layout;
    pSurfaceEvo->log2GobsPerBlockY    = pRequest->log2GobsPerBlockY;
    pSurfaceEvo->isoType              = pRequest->isoType;

    pSurfaceEvo->rmRefCnt = 1;
    pSurfaceEvo->structRefCnt = 1;

    pSurfaceEvo->owner.pOpenDev = pOpenDev;
    pSurfaceEvo->owner.surfaceHandle = surfaceHandle;

    pParams->reply.surfaceHandle = surfaceHandle;

    return;

fail:
    nvEvoDestroyApiHandle(pOpenDevSurfaceHandles, surfaceHandle);

    FreeSurfaceEvoRm(pDevEvo, pSurfaceEvo);
    FreeSurfaceEvoStruct(pSurfaceEvo);
}

/* Temporary storage used by ClearSurfaceUsage{Collect,Apply}. */
struct ClearSurfaceUsageCache {
    struct {
        struct {
            NvBool flipToNull           : 1;
            NvBool flipSemaphoreToNull  : 1;

            NvBool needToIdle           : 1;
        } layer[NVKMS_MAX_LAYERS_PER_HEAD];

        NvBool flipCursorToNull         : 1;
    } apiHead[NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP];
};

/*
 * Search for heads where the surfaces are used, and populate the structure
 * pointed to by 'pCache' to indicate which channels need to be updated.
 */
static void
ClearSurfaceUsageCollect(NVDevEvoPtr pDevEvo,
                         NVSurfaceEvoPtr pSurfaceEvo,
                         struct ClearSurfaceUsageCache *pCache)
{
    NVDispEvoPtr pDispEvo;
    NvU32 apiHead, sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NvU32 usageMaskOneHead = nvCollectSurfaceUsageMaskOneApiHead(pDispEvo,
                apiHead, pSurfaceEvo);
            NvU32 usageMaskMainLayer = DRF_IDX_VAL(_SURFACE,
                _USAGE_MASK, _LAYER, NVKMS_MAIN_LAYER, usageMaskOneHead);
            NvU32 layer;

            /*
             * XXX NVKMS TODO: flip across heads/subdevices for all scenarios
             * that are flip locked.
             */

            if (FLD_TEST_DRF(_SURFACE, _USAGE_MASK_LAYER, _SEMAPHORE,
                    _ENABLE, usageMaskMainLayer)) {
                pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].
                    flipSemaphoreToNull = TRUE;
            }

            if (FLD_TEST_DRF(_SURFACE, _USAGE_MASK_LAYER, _NOTIFIER,
                    _ENABLE, usageMaskMainLayer) ||
                    FLD_TEST_DRF(_SURFACE, _USAGE_MASK_LAYER, _SCANOUT,
                        _ENABLE, usageMaskMainLayer)) {
                pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].
                    flipToNull = TRUE;
            }

            for (layer = 0;
                 layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                NvU32 usageMaskOneLayer = DRF_IDX_VAL(_SURFACE,
                    _USAGE_MASK, _LAYER, layer, usageMaskOneHead);

                if (layer == NVKMS_MAIN_LAYER) {
                    continue;
                }

                if (usageMaskOneLayer != 0x0) {
                    pCache->apiHead[sd][apiHead].layer[layer].
                        flipToNull = TRUE;
                } if (pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].
                        flipToNull) {
                    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES] = { };
                    /*
                     * EVO requires that, when flipping the base channel
                     * (aka main layer) to NULL, overlay channel is also
                     * flipped to NULL.
                     */
                    if ((pSurfaceEvos[NVKMS_LEFT] != NULL) ||
                            (pSurfaceEvos[NVKMS_RIGHT] != NULL)) {
                        pCache->apiHead[sd][apiHead].layer[layer].
                            flipToNull = TRUE;
                     }
                }
            }

            if (FLD_TEST_DRF(_SURFACE, _USAGE_MASK, _CURSOR,
                    _ENABLE, usageMaskOneHead) != 0x0) {
                pCache->apiHead[sd][apiHead].flipCursorToNull = TRUE;
            }
        }
    }
}

/*
 * Do the hard work to babysit the hardware to ensure that any channels which
 * need clearing have actually done so before proceeding to free memory and
 * remove ctxdmas from the hash table.
 *
 * This is achieved in several steps:
 * 1. Issue a flip of any overlay layer to NULL -- these are processed
 *    separately since using one Flip request would interlock them, potentially
 *    exacerbating stuck channels by getting other channels stuck too.
 *    Pre-NVDisplay requires that, when flipping the core channel to NULL,
 *    all satellite channels are also flipped to NULL. The EVO2 hal takes care
 *    to enable/disable the core surface along with the base surface,
 *    therefore flip overlay to NULL before base.
 * 2. Issue a flip of any main layer to NULL
 * 3. Wait for any base/overlay layer that we expect to be idle to actually
 *    be idle.  If they don't idle in a timely fashion, apply accelerators to
 *    forcibly idle any problematic channels.
 * 4. Issue a flip of any core channels to NULL.
 */
static void
ClearSurfaceUsageApply(NVDevEvoPtr pDevEvo,
                       struct ClearSurfaceUsageCache *pCache,
                       NvBool skipUpdate)
{
    NVDispEvoPtr pDispEvo;
    NvU32 apiHead, sd;
    const NvU32 maxApiHeads = pDevEvo->numApiHeads * pDevEvo->numSubDevices;
    struct NvKmsFlipRequestOneHead *pFlipApiHead =
        nvCalloc(1, sizeof(*pFlipApiHead) * maxApiHeads);
    NvU32 numFlipApiHeads = 0;

    if (pFlipApiHead == NULL) {
        nvAssert(!"Failed to allocate memory");
        return;
    }

    /* 1. Issue a flip of any overlay layer to NULL */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            struct NvKmsFlipCommonParams *pRequestOneApiHead =
                &pFlipApiHead[numFlipApiHeads].flip;
            NvU32 layer;
            NvBool found = FALSE;

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            for (layer = 0;
                 layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {

                if (layer == NVKMS_MAIN_LAYER) {
                    continue;
                }

                if (pCache->apiHead[sd][apiHead].layer[layer].flipToNull) {
                    pRequestOneApiHead->layer[layer].surface.specified = TRUE;
                    // No need to specify sizeIn/sizeOut as we are flipping NULL surface.
                    pRequestOneApiHead->layer[layer].compositionParams.specified = TRUE;
                    pRequestOneApiHead->layer[layer].syncObjects.specified = TRUE;
                    pRequestOneApiHead->layer[layer].completionNotifier.specified = TRUE;

                    found = TRUE;

                    pCache->apiHead[sd][apiHead].layer[layer].needToIdle = TRUE;
                }
            }

            if (found) {
                pFlipApiHead[numFlipApiHeads].sd = sd;
                pFlipApiHead[numFlipApiHeads].head = apiHead;
                numFlipApiHeads++;
                nvAssert(numFlipApiHeads <= maxApiHeads);
            }
        }
    }

    if (numFlipApiHeads > 0) {
        nvFlipEvo(pDevEvo, pDevEvo->pNvKmsOpenDev,
                  pFlipApiHead,
                  numFlipApiHeads,
                  TRUE  /* commit */,
                  NULL  /* pReply */,
                  skipUpdate,
                  FALSE /* allowFlipLock */);

        nvkms_memset(pFlipApiHead, 0,
            sizeof(pFlipApiHead[0]) * numFlipApiHeads);
        numFlipApiHeads = 0;
    }

    /*
     * No need to idle the overlay layer before flipping the main channel to
     * NULL, because the FlipOverlay90() function in the EVO2 hal makes sure
     * that the overlay's flip to NULL is always interlocked with the core
     * channel and the base (main layer) channel's flip to NULL can proceed only
     * after completion of the overlay's flip to NULL (the base channel's flip
     * to NULL interlocks with the core channel's flip to NULL).
     */

    /* 2. Issue a flip of any main layer to NULL */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            struct NvKmsFlipCommonParams *pRequestOneApiHead =
                &pFlipApiHead[numFlipApiHeads].flip;
            NvBool found = FALSE;

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            if (pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].flipToNull ||
                pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].flipSemaphoreToNull) {

                if (pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].flipToNull) {
                    pRequestOneApiHead->layer[NVKMS_MAIN_LAYER].surface.specified = TRUE;
                    // No need to specify sizeIn/sizeOut as we are flipping NULL surface.
                    pRequestOneApiHead->layer[NVKMS_MAIN_LAYER].completionNotifier.specified = TRUE;

                    pCache->apiHead[sd][apiHead].layer[NVKMS_MAIN_LAYER].needToIdle = TRUE;
                }

                /* XXX arguably we should also idle for this case, but we
                 * don't currently have a way to do so without also
                 * clearing the ISO surface */
                pRequestOneApiHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.useSyncpt = FALSE;
                pRequestOneApiHead->layer[NVKMS_MAIN_LAYER].syncObjects.specified = TRUE;

                found = TRUE;
            }

            if (found) {
                pFlipApiHead[numFlipApiHeads].sd = sd;
                pFlipApiHead[numFlipApiHeads].head = apiHead;
                numFlipApiHeads++;
                nvAssert(numFlipApiHeads <= maxApiHeads);
            }
        }
    }

    if (numFlipApiHeads > 0) {
        nvFlipEvo(pDevEvo, pDevEvo->pNvKmsOpenDev,
                  pFlipApiHead,
                  numFlipApiHeads,
                  TRUE  /* commit */,
                  NULL  /* pReply */,
                  skipUpdate,
                  FALSE /* allowFlipLock */);

        nvkms_memset(pFlipApiHead, 0,
            sizeof(pFlipApiHead[0]) * numFlipApiHeads);
        numFlipApiHeads = 0;
    }

    /*
     * 3. Wait for any base/overlay layer that we expect to be idle to actually
     *    be idle.  If they don't idle in a timely fashion, apply accelerators to
     *    forcibly idle any problematic channels.
     */
    if (!skipUpdate) {
        NvU32 layerMaskPerSdApiHead[NVKMS_MAX_SUBDEVICES]
            [NVKMS_MAX_HEADS_PER_DISP] = { };
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                for (NvU32 layer = 0;
                     layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                    if (pCache->apiHead[sd][apiHead].layer[layer].needToIdle) {
                        layerMaskPerSdApiHead[sd][apiHead] |= NVBIT(layer);
                    }
                }
            }
        }
        nvIdleLayerChannels(pDevEvo, layerMaskPerSdApiHead);
    }

    /* 4. Issue a flip of any core channels to NULL */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            if (pCache->apiHead[sd][apiHead].flipCursorToNull) {
                pFlipApiHead[numFlipApiHeads].flip.cursor.imageSpecified = TRUE;
                pFlipApiHead[numFlipApiHeads].sd = sd;
                pFlipApiHead[numFlipApiHeads].head = apiHead;
                numFlipApiHeads++;
                nvAssert(numFlipApiHeads <= maxApiHeads);
            }
        }
    }

    if (numFlipApiHeads > 0) {
        nvFlipEvo(pDevEvo, pDevEvo->pNvKmsOpenDev,
                  pFlipApiHead,
                  numFlipApiHeads,
                  TRUE  /* commit */,
                  NULL  /* pReply */,
                  skipUpdate,
                  FALSE /* allowFlipLock */);
    }

    nvFree(pFlipApiHead);
}

/*
 * This function unregisters/releases all of the surface handles remaining for
 * the given pOpenDev.
 *
 * It duplicates some functionality of nvEvoUnregisterSurface() and
 * nvEvoReleaseSurface(), but with an important difference: it processes the
 * "clear surface usage" step for all surfaces up front, and only once that is
 * complete it proceeds with freeing the surfaces.
 *
 * In practice, this makes teardown much smoother than invoking those functions
 * individually for each surface, particularly in the case that the hardware is
 * stuck and needs accelerators.  Consider the case where a client has
 * registered several surfaces, and is flipping between two of them, and the
 * hardware is stuck on a semaphore acquire that will never complete with
 * several frames pending in the pushbuffer.  If the first surface processed
 * by nvEvoUnregisterSurface() happens to be the current "back buffer" (i.e.,
 * not the most recently pushed surface to be displayed), then
 * nvEvoUnregisterSurface() will call ClearSurfaceUsage(), but it will find no
 * channels to clear, and will proceed with nvEvoDecrementSurfaceRefCnts()
 * which will call nvRMSyncEvoChannel() to drain any outstanding methods.  Due
 * to the stalled semaphore, nvRMSyncEvoChannel() will stall for 2 seconds,
 * time out along with a nasty message to the kernel log, then we'll free the
 * surface and remove its entry from the display hash table anyway.  And that
 * may happen several times until we finally call nvEvoUnregisterSurface() on
 * the surface which is the most recently requested flip, where
 * ClearSurfaceUsage() will finally get a chance to tear down the channel
 * forcefully by using accelerators to skip the semaphore acquire.  But, some
 * of the methods which were outstanding and now get processed may reference a
 * ctxdma which was already freed, triggering nasty Xid messages.
 *
 * By gathering up all the channels we can to find which ones to clear first,
 * we have a much higher chance of avoiding these timeouts.
 */
void nvEvoFreeClientSurfaces(NVDevEvoPtr pDevEvo,
                             struct NvKmsPerOpenDev *pOpenDev,
                             NVEvoApiHandlesRec *pOpenDevSurfaceHandles)
{
    NvKmsGenericHandle surfaceHandle;
    NVSurfaceEvoPtr pSurfaceEvo;
    struct ClearSurfaceUsageCache cache = { };
    NvBool needApply = FALSE;

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(pOpenDevSurfaceHandles,
                                        pSurfaceEvo, surfaceHandle) {

        if (nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle)) {
            /*
             * If something besides the owner has an rmRefCnt reference,
             * the surface might be in use by EVO; flip to NULL to attempt
             * to free it.
             */
            if (pSurfaceEvo->rmRefCnt > 1) {
                ClearSurfaceUsageCollect(pDevEvo, pSurfaceEvo, &cache);
                needApply = TRUE;
            }
        }
    }

    if (needApply) {
        ClearSurfaceUsageApply(pDevEvo, &cache, FALSE);
    }

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(pOpenDevSurfaceHandles,
                                        pSurfaceEvo, surfaceHandle) {
        const NvBool isOwner =
            nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle);

        /* Remove the handle from the calling client's namespace. */
        nvEvoDestroyApiHandle(pOpenDevSurfaceHandles, surfaceHandle);

        if (isOwner) {
            nvEvoDecrementSurfaceRefCnts(pDevEvo, pSurfaceEvo);
        } else {
            nvEvoDecrementSurfaceStructRefCnt(pSurfaceEvo);
        }
    }

}

void nvEvoUnregisterSurface(NVDevEvoPtr pDevEvo,
                            struct NvKmsPerOpenDev *pOpenDev,
                            NvKmsSurfaceHandle surfaceHandle,
                            NvBool skipUpdate,
                            NvBool skipSync)
{
    NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDev(pOpenDev);
    NVSurfaceEvoPtr pSurfaceEvo;

    pSurfaceEvo = nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles,
                                               surfaceHandle);
    if (pSurfaceEvo == NULL) {
        return;
    }

    if (!nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle)) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_WARN,
                         "Surface unregister attempted by non-owner; "
                         "non-owners must release the surface.");
        return;
    }

    /*
     * If something besides the owner has an rmRefCnt reference,
     * the surface might be in use by EVO; flip to NULL to attempt
     * to free it.
     */
    if (pSurfaceEvo->rmRefCnt > 1) {
        struct ClearSurfaceUsageCache cache = { };

        ClearSurfaceUsageCollect(pDevEvo, pSurfaceEvo, &cache);
        ClearSurfaceUsageApply(pDevEvo, &cache, skipUpdate);
    }

    /* Remove the handle from the calling client's namespace. */
    nvEvoDestroyApiHandle(pOpenDevSurfaceHandles, surfaceHandle);

    nvEvoDecrementSurfaceRefCntsWithSync(pDevEvo, pSurfaceEvo, skipSync);
}

void nvEvoReleaseSurface(NVDevEvoPtr pDevEvo,
                         struct NvKmsPerOpenDev *pOpenDev,
                         NvKmsSurfaceHandle surfaceHandle)
{
    NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDev(pOpenDev);
    NVSurfaceEvoPtr pSurfaceEvo;

    pSurfaceEvo = nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles,
                                               surfaceHandle);
    if (pSurfaceEvo == NULL) {
        return;
    }

    if (nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle)) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_WARN,
                         "Surface release attempted by owner; "
                         "owners must unregister the surface.");
        return;
    }

    /* Remove the handle from the calling client's namespace. */
    nvEvoDestroyApiHandle(pOpenDevSurfaceHandles, surfaceHandle);

    nvEvoDecrementSurfaceStructRefCnt(pSurfaceEvo);
}

void nvEvoIncrementSurfaceRefCnts(NVSurfaceEvoPtr pSurfaceEvo)
{
    nvAssert(!nvEvoSurfaceRefCntsTooLarge(pSurfaceEvo));

    pSurfaceEvo->rmRefCnt++;
    pSurfaceEvo->structRefCnt++;
}

void nvEvoDecrementSurfaceRefCnts(NVDevEvoPtr pDevEvo,
                                  NVSurfaceEvoPtr pSurfaceEvo)
{
    nvEvoDecrementSurfaceRefCntsWithSync(pDevEvo, pSurfaceEvo, NV_FALSE);
}

void nvEvoDecrementSurfaceRefCntsWithSync(NVDevEvoPtr pDevEvo,
                                          NVSurfaceEvoPtr pSurfaceEvo,
                                          NvBool skipSync)
{
    nvAssert(pSurfaceEvo->rmRefCnt >= 1);
    pSurfaceEvo->rmRefCnt--;

    if (pSurfaceEvo->rmRefCnt == 0) {
        /*
         * Don't clear usage/sync if this surface was registered as not
         * requiring display hardware access, to WAR timeouts that result from
         * OGL unregistering a deferred request fifo causing a sync here that
         * may timeout if GLS hasn't had the opportunity to release semaphores
         * with pending flips. (Bug 2050970)
         */
        if (pSurfaceEvo->requireDisplayHardwareAccess) {
            nvEvoClearSurfaceUsage(pDevEvo, pSurfaceEvo, skipSync);
        }

        FreeSurfaceEvoRm(pDevEvo, pSurfaceEvo);
    }

    nvEvoDecrementSurfaceStructRefCnt(pSurfaceEvo);
}

NvBool nvEvoSurfaceRefCntsTooLarge(const NVSurfaceEvoRec *pSurfaceEvo)
{
    return ((pSurfaceEvo->rmRefCnt == NV_U64_MAX) ||
            (pSurfaceEvo->structRefCnt == NV_U64_MAX));
}

static NVSurfaceEvoPtr GetSurfaceFromHandle(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandle,
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel,
    const NvBool requireDisplayHardwareAccess,
    const NvBool maybeUsedBy3d)
{
    NVSurfaceEvoPtr pSurfaceEvo =
        nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles, surfaceHandle);

    nvAssert(requireDisplayHardwareAccess || (!isUsedByCursorChannel && !isUsedByLayerChannel));

    if (pSurfaceEvo == NULL) {
        return NULL;
    }

    if (pSurfaceEvo->rmRefCnt == 0) { /* orphan */
        return NULL;
    }

    if (requireDisplayHardwareAccess && !pSurfaceEvo->requireDisplayHardwareAccess) {
        return NULL;
    }

    /* Validate that the surface can be used as a cursor image */
    if (isUsedByCursorChannel &&
        !pDevEvo->hal->ValidateCursorSurface(pDevEvo, pSurfaceEvo)) {
        return NULL;
    }

    /*
     * XXX If maybeUsedBy3d, the fetched surface may be used as a texture by the
     * 3d engine.  Previously, all surfaces were checked by
     * nvEvoGetHeadSetStoragePitchValue() at registration time, and we don't
     * know if nvEvoGetHeadSetStoragePitchValue() was protecting us from any
     * surface dimensions that could cause trouble for the 3d engine.
     */
    if (isUsedByLayerChannel || maybeUsedBy3d) {
        NvU8 planeIndex;

        FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {
            if (nvEvoGetHeadSetStoragePitchValue(
                                pDevEvo,
                                pSurfaceEvo->layout,
                                pSurfaceEvo->planes[planeIndex].pitch) == 0) {
                return NULL;
            }
        }
    }

    return pSurfaceEvo;
}

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandle(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandle,
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel)
{
    return GetSurfaceFromHandle(pDevEvo,
                                pOpenDevSurfaceHandles,
                                surfaceHandle,
                                isUsedByCursorChannel,
                                isUsedByLayerChannel,
                                TRUE /* requireDisplayHardwareAccess */,
                                TRUE /* maybeUsedBy3d */);
}

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandleNoDispHWAccessOk(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvKmsSurfaceHandle surfaceHandle)
{
    return GetSurfaceFromHandle(pDevEvo,
                                pOpenDevSurfaceHandles,
                                surfaceHandle,
                                FALSE /* isUsedByCursorChannel */,
                                FALSE /* isUsedByLayerChannel */,
                                FALSE /* requireDisplayHardwareAccess */,
                                TRUE /* maybeUsedBy3d */);
}

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandleNoHWAccess(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvKmsSurfaceHandle surfaceHandle)
{
    return GetSurfaceFromHandle(pDevEvo,
                                pOpenDevSurfaceHandles,
                                surfaceHandle,
                                FALSE /* isUsedByCursorChannel */,
                                FALSE /* isUsedByLayerChannel */,
                                FALSE /* requireDisplayHardwareAccess */,
                                FALSE /* maybeUsedBy3d */);
}

/*!
 * Create a deferred request fifo, using the specified pSurfaceEvo.
 */
NVDeferredRequestFifoRec *nvEvoRegisterDeferredRequestFifo(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo)
{
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    NvU32 ret;

    if (pSurfaceEvo->planes[0].rmObjectSizeInBytes <
        sizeof(struct NvKmsDeferredRequestFifo)) {
        return NULL;
    }

    /*
     * XXX validate that the surface is in sysmem; can we query that from
     * resman?
     */

    pDeferredRequestFifo = nvCalloc(1, sizeof(*pDeferredRequestFifo));

    if (pDeferredRequestFifo == NULL) {
        return NULL;
    }

    /* Get a CPU mapping of the surface. */

    ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                           pDevEvo->deviceHandle,
                           pSurfaceEvo->planes[0].rmHandle,
                           0,
                           sizeof(*pDeferredRequestFifo->fifo),
                           (void **) &pDeferredRequestFifo->fifo,
                           0);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFree(pDeferredRequestFifo);
        return NULL;
    }

    pDeferredRequestFifo->pSurfaceEvo = pSurfaceEvo;

    nvEvoIncrementSurfaceRefCnts(pSurfaceEvo);

    return pDeferredRequestFifo;
}

/*!
 * Free the deferred request fifo.
 */
void nvEvoUnregisterDeferredRequestFifo(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo)
{
    nvAssert(pDeferredRequestFifo->fifo != NULL);
    nvAssert(pDeferredRequestFifo->pSurfaceEvo != NULL);

    nvHsLeaveSwapGroup(pDevEvo, pDeferredRequestFifo, FALSE /* teardown */);

    nvRmApiUnmapMemory(
                    nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pDeferredRequestFifo->pSurfaceEvo->planes[0].rmHandle,
                    pDeferredRequestFifo->fifo,
                    0);

    nvEvoDecrementSurfaceRefCnts(pDevEvo, pDeferredRequestFifo->pSurfaceEvo);

    nvFree(pDeferredRequestFifo);
}
