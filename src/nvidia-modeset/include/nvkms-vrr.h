/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_VRR_H__
#define __NVKMS_VRR_H__

#include "nvkms-types.h"
#include "nvkms-modeset-types.h"

#ifdef __cplusplus
extern "C" {
#endif

void nvAllocVrrEvo(NVDevEvoPtr pDevEvo);
void nvFreeVrrEvo(NVDevEvoPtr pDevEvo);
void nvDisableVrr(NVDevEvoPtr pDevEvo);
void nvEnableVrr(NVDevEvoPtr pDevEvo,
                 const struct NvKmsSetModeRequest *pRequest);
void nvCancelVrrFrameReleaseTimers(NVDevEvoPtr pDevEvo);
void nvSetVrrActive(NVDevEvoPtr pDevEvo, NvBool active);
void nvApplyVrrBaseFlipOverrides(const NVDispEvoRec *pDispEvo, NvU32 head,
                                 const NVFlipChannelEvoHwState *pOld,
                                 NVFlipChannelEvoHwState *pNew);
void nvSetNextVrrFlipTypeAndIndex(NVDevEvoPtr pDevEvo,
                                  struct NvKmsFlipReply *reply);
void nvTriggerVrrUnstallMoveCursor(NVDispEvoPtr pDispEvo);
void nvTriggerVrrUnstallSetCursorImage(NVDispEvoPtr pDispEvo,
                                       NvBool ctxDmaChanged);
void nvGetDpyMinRefreshRateValidValues(
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyVRRType vrrType,
    const NvU32 edidTimeoutMicroseconds,
    NvU32 *minMinRefreshRate,
    NvU32 *maxMinRefreshRate);

NvBool nvDispSupportsVrr(const NVDispEvoRec *pDispEvo);

NvBool nvExportVrrSemaphoreSurface(const NVDevEvoRec *pDevEvo, int fd);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_VRR_H__ */
