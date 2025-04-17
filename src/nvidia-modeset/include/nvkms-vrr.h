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

#ifdef __cplusplus
extern "C" {
#endif

enum NvKmsDpyVRRType
nvGetAllowedDpyVrrType(const NVDpyEvoRec *pDpyEvo,
                       const NvModeTimings *pTimings,
                       enum NvKmsStereoMode stereoMode,
                       const NvBool allowGsync,
                       const enum NvKmsAllowAdaptiveSync allowAdaptiveSync);
void nvAdjustHwModeTimingsForVrrEvo(
    const NVDpyEvoRec *pDpyEvo,
    const enum NvKmsDpyVRRType vrrType,
    const NvU32 vrrOverrideMinRefreshRate,
    NVHwModeTimingsEvoPtr pTimings);

void nvAllocVrrEvo(NVDevEvoPtr pDevEvo);
void nvFreeVrrEvo(NVDevEvoPtr pDevEvo);
void nvDisableVrr(NVDevEvoPtr pDevEvo);
void nvEnableVrr(NVDevEvoPtr pDevEvo);
void nvCancelVrrFrameReleaseTimers(NVDevEvoPtr pDevEvo,
                                   const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES]);
void nvSetVrrActive(NVDevEvoPtr pDevEvo, 
                    const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES], 
                    const NvU32 vrrActiveApiHeadMasks[NVKMS_MAX_SUBDEVICES]);
void nvApplyVrrBaseFlipOverrides(const NVDispEvoRec *pDispEvo, NvU32 head,
                                 const NVFlipChannelEvoHwState *pOld,
                                 NVFlipChannelEvoHwState *pNew);
enum NvKmsVrrFlipType nvGetActiveVrrType(const NVDevEvoRec *pDevEvo);
NvS32 nvIncVrrSemaphoreIndex(NVDevEvoPtr pDevEvo,
                             const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES]);
void nvTriggerVrrUnstallMoveCursor(NVDispEvoPtr pDispEvo);
void nvTriggerVrrUnstallSetCursorImage(NVDispEvoPtr pDispEvo,
                                       NvBool elvReleased);
void nvGetDpyMinRefreshRateValidValues(
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyVRRType vrrType,
    const NvU32 edidTimeoutMicroseconds,
    NvU32 *minMinRefreshRate,
    NvU32 *maxMinRefreshRate);

NvBool nvDispSupportsVrr(const NVDispEvoRec *pDispEvo);

NvBool nvExportVrrSemaphoreSurface(const NVDevEvoRec *pDevEvo, int fd);

void nvVrrSignalSemaphore(NVDevEvoPtr pDevEvo, NvS32 vrrSemaphoreIndex);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_VRR_H__ */
