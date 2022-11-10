/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_HDMI_H__
#define __NVKMS_HDMI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvkms-types.h"

void nvUpdateHdmiInfoFrames(const NVDispEvoRec *pDispEvo,
                            const NvU32 head,
                            const NVAttributesSetEvoRec *pAttributesSet,
                            const NVDispHeadInfoFrameStateEvoRec *pInfoFrameState,
                            NVDpyEvoRec *pDpyEvo);

void nvDpyUpdateHdmiPreModesetEvo(NVDpyEvoPtr pDpyEvo);
void nvDpyUpdateHdmiVRRCaps(NVDpyEvoPtr pDpyEvo);
void nvUpdateHdmiCaps(NVDpyEvoPtr pDpyEvo);

void nvLogEdidCea861InfoEvo(NVDpyEvoPtr pDpyEvo,
                            NVEvoInfoStringPtr pInfoString);
NvBool nvDpyIsHdmiEvo(const NVDpyEvoRec *pDpyEvo);

NvBool nvHdmi204k60HzRGB444Allowed(const NVDpyEvoRec *pDpyEvo,
                                   const struct NvKmsModeValidationParams *pParams,
                                   const NVT_TIMING *pTiming);

void nvHdmiDpEnableDisableAudio(const NVDispEvoRec *pDispEvo,
                                const NvU32 head, const NvBool enable);

void nvRemoveUnusedHdmiDpAudioDevice(const NVDispEvoRec *pDispEvo);

void nvHdmiSetVRR(NVDispEvoPtr pDispEvo, NvU32 head, NvBool enable);

NvBool nvInitHdmiLibrary(NVDevEvoRec *pDevEvo);
void nvTeardownHdmiLibrary(NVDevEvoRec *pDevEvo);

NvBool nvHdmiFrlAssessLink(NVDpyEvoPtr pDpyEvo);
NvBool nvHdmiFrlQueryConfig(const NVDpyEvoRec *pDpyEvo,
                            const NvModeTimings *pModeTimings,
                            NVHwModeTimingsEvo *pTimings,
                            const struct NvKmsModeValidationParams *pParams);
void nvHdmiFrlClearConfig(NVDispEvoRec *pDispEvo, NvU32 activeRmId);
void nvHdmiFrlSetConfig(NVDispEvoRec *pDispEvo, NvU32 head);

void nvHdmiDpConstructHeadAudioState(const NvU32 displayId,
                                     const NVDpyEvoRec *pDpyEvo,
                                     NVDispHeadAudioStateEvoRec *pAudioState);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_HDMI_H__ */
