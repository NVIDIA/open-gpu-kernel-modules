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

#ifndef __NVKMS_FLIP_UTILS_H__
#define __NVKMS_FLIP_UTILS_H__


#include "nvkms-types.h"

NvBool nvAssignSurfaceArray(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES],
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel,
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES]);

NvBool nvAssignNIsoEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsNIsoSurface *pParamsNIso,
    const NvBool notifier, /* TRUE=notifier; FALSE=semaphore */
    const NvU32 layer,
    NVFlipNIsoSurfaceEvoHwState *pNIsoState);

NvBool nvAssignCompletionNotifierEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsCompletionNotifierDescription *pParamsNotif,
    const NvU32 layer,
    NVFlipCompletionNotifierEvoHwState *pNotif);

NvBool nvAssignSemaphoreEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvU32 layer,
    const NvU32 sd,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject);

NvBool nvValidatePerLayerCompParams(
    const struct NvKmsCompositionParams *pCompParams,
    const struct NvKmsCompositionCapabilities *pCaps,
    NVSurfaceEvoPtr pSurfaceEvo);

NvBool
nvAssignCursorSurface(const struct NvKmsPerOpenDev *pOpenDev,
                      const NVDevEvoRec *pDevEvo,
                      const struct NvKmsSetCursorImageCommonParams *pImgParams,
                      NVSurfaceEvoPtr *pSurfaceEvo);


#endif /* __NVKMS_FLIP_UTILS_H__ */
