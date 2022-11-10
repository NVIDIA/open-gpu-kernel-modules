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

#ifndef __NVKMS_HEADSURFACE_3D_H__
#define __NVKMS_HEADSURFACE_3D_H__

#include "nvkms-types.h"
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-headsurface-config.h"

NvBool nvHs3dAllocDevice(NVHsDeviceEvoPtr pHsDevice);

void nvHs3dFreeDevice(NVHsDeviceEvoPtr pHsDevice);

NvBool nvHs3dAllocChannel(NVHsChannelEvoPtr pHsChannel);

void nvHs3dFreeChannel(NVHsChannelEvoPtr pHsChannel);

void nvHs3dClearSurface(
    NVHsChannelEvoPtr pHsChannel,
    const NVHsSurfaceRec *pHsSurface,
    const struct NvKmsRect surfaceRect,
    NvBool yuv420);

void nvHs3dSetConfig(NVHsChannelEvoPtr pHsChannel);

NvBool nvHs3dRenderFrame(
    NVHsChannelEvoPtr pHsChannel,
    const NvHsNextFrameRequestType requestType,
    const NvBool honorSwapGroupClipList,
    const NvU8 dstEye,
    const NvU8 dstBufferIndex,
    const enum NvKmsPixelShiftMode pixelShift,
    const struct NvKmsRect destRect,
    const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD]);

void nvHs3dReleaseSemaphore(
    NVHsChannelEvoPtr pHsChannel,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const enum NvKmsNIsoFormat nIsoFormat,
    const NvU16 offsetInWords,
    const NvU32 payload,
    const NvBool allPreceedingReads);

NvU32 nvHs3dLastRenderedOffset(NVHsChannelEvoPtr pHsChannel);

void nvHs3dPushPendingViewportFlip(NVHsChannelEvoPtr pHsChannel);

#endif /* __NVKMS_HEADSURFACE_3D_H__ */
