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

void nvClearFlipEvoHwState(
    NVFlipEvoHwState *pFlipState);

void nvInitFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState);

NvBool nvAssignSurfaceArray(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES],
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel,
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES]);

NvBool
nvAssignCursorSurface(const struct NvKmsPerOpenDev *pOpenDev,
                      const NVDevEvoRec *pDevEvo,
                      const struct NvKmsSetCursorImageCommonParams *pImgParams,
                      NVSurfaceEvoPtr *pSurfaceEvo);

NvBool nvUpdateFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState,
    NvBool allowVrr);

void
nvOverrideScalingUsageBounds(const NVDevEvoRec *pDevEvo,
                             NvU32 head,
                             NVFlipEvoHwState *pFlipState,
                             const struct NvKmsUsageBounds *pPossibleUsage);

NvBool nvValidateFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVHwModeTimingsEvo *pTimings,
    const NVFlipEvoHwState *pFlipState);

void
nvUpdateSurfacesFlipRefCount(
    NVDevEvoPtr pDevEvo,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState,
    NvBool increase);

void nvFlipEvoOneHead(
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NVFlipEvoHwState *pFlipState,
    NvBool allowFlipLock,
    NVEvoUpdateState *updateState);

void nvEvoCancelPostFlipIMPTimer(
    NVDevEvoPtr pDevEvo);

void nvFillPostSyncptReplyOneChannel(
    NVEvoChannel *pChannel,
    enum NvKmsSyncptType postType,
    struct NvKmsSyncpt *postSyncpt,
    const NVFlipSyncObjectEvoHwState *pHwSyncObject);

NvBool nvFlipEvo(NVDevEvoPtr pDevEvo,
                 const struct NvKmsPerOpenDev *pOpenDev,
                 const struct NvKmsFlipRequest *request,
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

#endif /* __NVKMS_FLIP_H__ */
