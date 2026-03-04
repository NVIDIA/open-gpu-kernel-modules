/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_FLIP_H__
#define __NVKMS_FLIP_H__


#include "nvkms-types.h"


NvBool nvCheckLayerPermissions(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 apiHead,
    const NvU8 changedLayersMask);

NvBool nvCheckFlipPermissions(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 apiHead,
    const struct NvKmsFlipCommonParams *pParams);

NvBool nvFlipEvo(NVDevEvoPtr pDevEvo,
                 const struct NvKmsPerOpenDev *pOpenDev,
                 const struct NvKmsFlipRequestOneHead *pFlipHead,
                 NvU32 numFlipHeads,
                 NvBool commit,
                 struct NvKmsFlipReply *reply,
                 NvBool skipUpdate,
                 NvBool allowFlipLock);

void nvApiHeadGetLayerSurfaceArray(const NVDispEvoRec *pDispEvo,
                                   const NvU32 apiHead,
                                   const NvU32 layer,
                                   NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES]);

void nvApiHeadGetCursorInfo(const NVDispEvoRec *pDispEvo,
                            const NvU32 apiHead,
                            NVSurfaceEvoPtr *ppSurfaceEvo,
                            NvS16 *x, NvS16 *y);

void nvApiHeadSetViewportPointIn(const NVDispEvoRec *pDispEvo,
                                 const NvU32 apiHead,
                                 const NvU16 x,
                                 const NvU16 y);

NvU32 nvApiHeadGetActiveViewportOffset(NVDispEvoRec *pDispEvo,
                                       NvU32 apiHead);

void nvApiHeadIdleMainLayerChannels(NVDevEvoRec *pDevEvo,
    const NvU32 apiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES]);

void nvApiHeadUpdateFlipLock(NVDevEvoRec *pDevEvo,
                             const NvU32 apiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES],
                             const NvBool enable);

NvBool nvIdleMainLayerChannelCheckIdleOneApiHead(NVDispEvoPtr pDispEvo,
                                                 NvU32 apiHead);

#define NV_SURFACE_USAGE_MASK_CURSOR                                0:0
#define NV_SURFACE_USAGE_MASK_CURSOR_DISABLE                          0
#define NV_SURFACE_USAGE_MASK_CURSOR_ENABLE                           1
#define NV_SURFACE_USAGE_MASK_LAYER(_n)       (3+(3*(_n))):(1+(3*(_n)))

#define NV_SURFACE_USAGE_MASK_LAYER_SEMAPHORE          1:1
#define NV_SURFACE_USAGE_MASK_LAYER_SEMAPHORE_DISABLE    0
#define NV_SURFACE_USAGE_MASK_LAYER_SEMAPHORE_ENABLE     1
#define NV_SURFACE_USAGE_MASK_LAYER_NOTIFIER           2:2
#define NV_SURFACE_USAGE_MASK_LAYER_NOTIFIER_DISABLE     0
#define NV_SURFACE_USAGE_MASK_LAYER_NOTIFIER_ENABLE      1
#define NV_SURFACE_USAGE_MASK_LAYER_SCANOUT            0:0
#define NV_SURFACE_USAGE_MASK_LAYER_SCANOUT_DISABLE      0
#define NV_SURFACE_USAGE_MASK_LAYER_SCANOUT_ENABLE       1

NvU32 nvCollectSurfaceUsageMaskOneApiHead(const NVDispEvoRec *pDispEvo,
                                          const NvU32 apiHead,
                                          NVSurfaceEvoPtr pSurfaceEvo);

void nvIdleLayerChannels(NVDevEvoRec *pDevEvo,
    NvU32 layerMaskPerSdApiHead[NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP]);

void nvEvoClearSurfaceUsage(NVDevEvoRec *pDevEvo,
                            NVSurfaceEvoPtr pSurfaceEvo,
                            const NvBool skipSync);

NvBool nvIdleBaseChannelOneApiHead(NVDispEvoRec *pDispEvo, NvU32 apiHead,
                                   NvBool *pStoppedBase);

#endif /* __NVKMS_FLIP_H__ */
