/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_HEADSURFACE_H__
#define __NVKMS_HEADSURFACE_H__

#include "nvkms-types.h"

typedef struct _NVHsLayerRequestedFlipStateRec {
    NVSurfaceEvoPtr pSurfaceEvo[NVKMS_MAX_EYES];
    NVFlipCompletionNotifierEvoHwState completionNotifier;
    NVFlipSyncObjectEvoHwState syncObject;
    NvBool perEyeStereoFlip;
    NvU8 minPresentInterval;
} NVHsLayerRequestedFlipState;

NvU64 nvHsMapSurfaceToDevice(
    const NVDevEvoRec *pDevEvo,
    const NvU32 rmHandle,
    const NvU64 sizeInBytes,
    enum NvHsMapPermissions hsMapPermissions);

void nvHsUnmapSurfaceFromDevice(
    const NVDevEvoRec *pDevEvo,
    const NvU32 rmHandle,
    const NvU64 gpuAddress);

NVHsSurfacePtr nvHsAllocSurface(
    NVDevEvoRec *pDevEvo,
    const NvBool requireDisplayHardwareAccess,
    const enum NvKmsSurfaceMemoryFormat format,
    const NvU32 widthInPixels,
    const NvU32 heightInPixels);

void nvHsFreeSurface(
    NVDevEvoRec *pDevEvo,
    NVHsSurfacePtr pHsSurface);

NvBool nvHsAllocDevice(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsAllocDeviceRequest *pRequest);

void nvHsFreeDevice(NVDevEvoRec *pDevEvo);

NVHsChannelEvoPtr nvHsAllocChannel(NVDispEvoRec *pDispEvo, NvU32 apiHead);

void nvHsFreeChannel(NVHsChannelEvoPtr pHsChannel);

void nvHsPushFlipQueueEntry(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 layer,
    const NVHsLayerRequestedFlipState *pHwState);

void nvHsDrainFlipQueue(
    NVHsChannelEvoPtr pHsChannel);

NvBool nvHsIdleFlipQueue(
    NVHsChannelEvoPtr pHsChannel,
    NvBool force);

void nvHsInitNotifiers(
    NVHsDeviceEvoPtr pHsDevice,
    NVHsChannelEvoPtr pHsChannel);

void nvHsFlip(
    NVHsDeviceEvoPtr pHsDevice,
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 eyeMask,
    const NvBool perEyeStereoFlip,
    const NvU8 currentIndex,
    const NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps,
    const NvBool isFirstFlip,
    const NvBool allowFlipLock);

typedef enum {
    NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME,
    NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK,
    NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY,
} NvHsNextFrameRequestType;

void nvHsNextFrame(
    NVHsDeviceEvoPtr pHsDevice,
    NVHsChannelEvoPtr pHsChannel,
    const NvHsNextFrameRequestType requestType);

void nvHsAddVBlankCallback(NVHsChannelEvoPtr pHsChannel);

void nvHsRemoveVBlankCallback(NVHsChannelEvoPtr pHsChannel);

void nvHsAddRgLine1Callback(NVHsChannelEvoPtr pHsChannel);

void nvHsRemoveRgLine1Callback(NVHsChannelEvoPtr pHsChannel);

void nvHsAllocStatistics(
    NVHsChannelEvoPtr pHsChannel);

void nvHsFreeStatistics(
    NVHsChannelEvoPtr pHsChannel);

void nvHsProcessPendingViewportFlips(NVDevEvoPtr pDevEvo);

NVSurfaceEvoRec *nvHsGetNvKmsSurface(const NVDevEvoRec *pDevEvo,
                                     NvKmsSurfaceHandle surfaceHandle,
                                     const NvBool requireDisplayHardwareAccess);

#if NVKMS_PROCFS_ENABLE
void nvHsProcFs(
    NVEvoInfoStringRec *pInfoString,
    NVDevEvoRec *pDevEvo,
    NvU32 dispIndex,
    NvU32 apiHead);
#endif

#endif /* __NVKMS_HEADSURFACE_H__ */
