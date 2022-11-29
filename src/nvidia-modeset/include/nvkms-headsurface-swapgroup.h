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

#ifndef __NVKMS_HEADSURFACE_SWAPGROUP_H__
#define __NVKMS_HEADSURFACE_SWAPGROUP_H__

#include "nvkms-types.h"

NvBool nvHsSwapGroupIsHeadSurfaceNeeded(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead);

void nvHsSwapGroupRelease(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup);

NVSwapGroupRec* nvHsAllocSwapGroup(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsAllocSwapGroupRequest *pRequest);

NVSwapGroupRec *nvHsGetSwapGroupStruct(
    const NVEvoApiHandlesRec *pEvoApiHandles,
    NvKmsSwapGroupHandle handle);

NVSwapGroupRec *nvHsGetSwapGroup(
    const NVEvoApiHandlesRec *pEvoApiHandles,
    NvKmsSwapGroupHandle handle);

void nvHsDecrementSwapGroupRefCnt(NVSwapGroupPtr pSwapGroup);

NvBool nvHsIncrementSwapGroupRefCnt(NVSwapGroupPtr pSwapGroup);

void nvHsFreeSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup);

typedef struct _NVHsJoinSwapGroupWorkArea {
    NVDevEvoPtr pDevEvo;
    NVSwapGroupRec *pSwapGroup;
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    struct NvKmsPerOpen *pEventOpenFd;
    NvBool enabledHeadSurface;
} NVHsJoinSwapGroupWorkArea;

NvBool nvHsJoinSwapGroup(
    NVHsJoinSwapGroupWorkArea *joinSwapGroupWorkArea,
    NvU32 numHandles,
    NvBool pendingJoin);

void nvHsLeaveSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo,
    NvBool teardown);

NvBool nvHsSetSwapGroupClipList(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup,
    const NvU16 nClips,
    struct NvKmsRect *pClipList);

void nvHsSwapGroupReady(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo,
    const NvU32 request);

NvBool nvHsSwapGroupGetPerEyeStereo(
    const NVSwapGroupRec *pSwapGroup);

#endif /* __NVKMS_HEADSURFACE_SWAPGROUP_H__ */
