/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_SURFACE_H__
#define __NVKMS_SURFACE_H__

#include "nvkms-types.h"

void nvEvoRegisterSurface(NVDevEvoPtr pDevEvo,
                          struct NvKmsPerOpenDev *pOpenDev,
                          struct NvKmsRegisterSurfaceParams *pParams,
                          enum NvHsMapPermissions hsMapPermissions);

void nvEvoUnregisterSurface(NVDevEvoPtr pDevEvo,
                            struct NvKmsPerOpenDev *pOpenDev,
                            NvKmsSurfaceHandle surfaceHandle,
                            NvBool skipUpdate);
void nvEvoReleaseSurface(NVDevEvoPtr pDevEvo,
                         struct NvKmsPerOpenDev *pOpenDev,
                         NvKmsSurfaceHandle surfaceHandle);

void nvEvoFreeClientSurfaces(NVDevEvoPtr pDevEvo,
                             struct NvKmsPerOpenDev *pOpenDev,
                             NVEvoApiHandlesRec *pOpenDevSurfaceHandles);

void nvEvoIncrementSurfaceStructRefCnt(NVSurfaceEvoPtr pSurfaceEvo);
void nvEvoDecrementSurfaceStructRefCnt(NVSurfaceEvoPtr pSurfaceEvo);

void nvEvoIncrementSurfaceRefCnts(NVSurfaceEvoPtr pSurfaceEvo);
void nvEvoDecrementSurfaceRefCnts(NVDevEvoPtr pDevEvo,
                                  NVSurfaceEvoPtr pSurfaceEvo);

NvBool nvEvoSurfaceRefCntsTooLarge(const NVSurfaceEvoRec *pSurfaceEvo);

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandle(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandle,
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel);

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandleNoDispHWAccessOk(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvKmsSurfaceHandle surfaceHandle);

NVSurfaceEvoPtr nvEvoGetSurfaceFromHandleNoHWAccess(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvKmsSurfaceHandle surfaceHandle);

NVDeferredRequestFifoRec *nvEvoRegisterDeferredRequestFifo(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo);

void nvEvoUnregisterDeferredRequestFifo(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo);

NVVblankSemControl *nvEvoEnableVblankSemControl(
    NVDevEvoRec *pDevEvo,
    NVDispEvoRec *pDispEvo,
    NvU32 apiHeadMask,
    NVSurfaceEvoRec *pSurfaceEvo,
    NvU64 surfaceOffset);

NvBool nvEvoDisableVblankSemControl(
    NVDevEvoRec *pDevEvo,
    NVVblankSemControl *pVblankSemControl);

NvBool nvEvoAccelVblankSemControls(
    NVDevEvoPtr pDevEvo,
    NvU32 dispIndex,
    NvU32 hwHeadMask);

static inline NvBool nvEvoIsSurfaceOwner(const NVSurfaceEvoRec *pSurfaceEvo,
                                         const struct NvKmsPerOpenDev *pOpenDev,
                                         NvKmsSurfaceHandle surfaceHandle)
{
    return ((pSurfaceEvo->owner.pOpenDev == pOpenDev) &&
            (pSurfaceEvo->owner.surfaceHandle == surfaceHandle));
}

#define ASSERT_EYES_MATCH(_arr, _field)                                 \
    nvAssert((_arr)[NVKMS_RIGHT] == NULL ||                             \
             (_arr)[NVKMS_LEFT]->_field ==  (_arr)[NVKMS_RIGHT]->_field);

ct_assert((NVKMS_RIGHT - NVKMS_LEFT) == 1);

#define FOR_ALL_EYES(_eye) \
    for ((_eye) = NVKMS_LEFT; (_eye) <= NVKMS_RIGHT; (_eye)++)

#define FOR_ALL_VALID_PLANES(_planeIndex, _pSurface) \
    for ((_planeIndex) = 0; \
         (_planeIndex) < \
         (nvKmsGetSurfaceMemoryFormatInfo((_pSurface)->format))->numPlanes; \
         (_planeIndex)++)

#endif /* __NVKMS_SURFACE_H__ */
