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

#ifndef __NVKMS_MODEPOOL_H__
#define __NVKMS_MODEPOOL_H__

#include "nvkms-types.h"
#include "nvkms-utils.h" /* NVEvoLogType */

#ifdef __cplusplus
extern "C" {
#endif

void
nvValidateModeIndex(NVDpyEvoPtr pDpyEvo,
                    const struct NvKmsValidateModeIndexRequest *pRequest,
                    struct NvKmsValidateModeIndexReply *pReply);
void
nvValidateModeEvo(NVDpyEvoPtr pDpyEvo,
                  const struct NvKmsValidateModeRequest *pRequest,
                  struct NvKmsValidateModeReply *pReply);

void nvEvoLogModeValidationModeTimings(NVEvoInfoStringPtr
                                       pInfoString,
                                       const NvModeTimings *pModeTimings);

NvBool nvValidateModeForModeset(NVDpyEvoRec *pDpyEvo,
                                const struct NvKmsModeValidationParams *pParams,
                                const struct NvKmsMode *pKmsMode,
                                const struct NvKmsSize *pViewPortSizeIn,
                                const struct NvKmsRect *pViewPortOut,
                                NVDpyAttributeColor *pDpyColor,
                                NVHwModeTimingsEvo *pTimingsEvo,
                                NVT_VIDEO_INFOFRAME_CTRL *pInfoFrameCtrl);

const NVT_TIMING *nvFindEdidNVT_TIMING(
    const NVDpyEvoRec *pDpyEvo,
    const NvModeTimings *pModeTimings,
    const struct NvKmsModeValidationParams *pParams);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_MODEPOOL_H__ */
