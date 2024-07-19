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

#ifndef __NVKMS_DPY_H__
#define __NVKMS_DPY_H__

#include "nvkms-types.h"

#ifdef __cplusplus
extern "C" {
#endif

void nvDpyProbeMaxPixelClock(NVDpyEvoPtr pDpyEvo);
NVEvoPassiveDpDongleType nvDpyGetPassiveDpDongleType(
    const NVDpyEvoRec *pDpyEvo,
    NvU32 *passiveDpDongleMaxPclkKHz);
void nvDpySetValidSyncsEvo(const NVDpyEvoRec *pDpyEvo,
                           struct NvKmsModeValidationValidSyncs *pValidSyncs);
NVDpyEvoPtr nvAllocDpyEvo(NVDispEvoPtr pDispEvo,
                          NVConnectorEvoPtr pConnectorEvo,
                          NVDpyId dpyId, const char *dpAddress);
void nvFreeDpyEvo(NVDispEvoPtr pDispEvo, NVDpyEvoPtr pDpyEvo);
NVConnectorEvoPtr nvGetConnectorFromDisp(NVDispEvoPtr pDispEvo, NVDpyId dpyId);

void nvDpyAssignSDRInfoFramePayload(NVT_HDR_INFOFRAME_PAYLOAD *pPayload);
void nvCancelSDRTransitionTimer(NVDpyEvoRec *pDpyEvo);
void nvUpdateInfoFrames(NVDpyEvoRec *pDpyEvo);

NvBool nvDpyRequiresDualLinkEvo(const NVDpyEvoRec *pDpyEvo,
                                const NVHwModeTimingsEvo *pTimings);

NVDpyEvoPtr nvGetDpyEvoFromDispEvo(const NVDispEvoRec *pDispEvo, NVDpyId dpyId);

NVDpyEvoPtr nvGetDPMSTDpyEvo(NVConnectorEvoPtr pConnectorEvo,
                             const char *address, NvBool *pDynamicDpyCreated);

typedef enum {
    NVKMS_EDID_READ_MODE_DEFAULT,
    NVKMS_EDID_READ_MODE_ACPI,
} NvKmsEdidReadMode;

NvBool nvDpyReadAndParseEdidEvo(
    const NVDpyEvoRec *pDpyEvo,
    const struct NvKmsQueryDpyDynamicDataRequest *pRequest,
    NvKmsEdidReadMode readMode,
    NVEdidRec *pEdid,
    NVParsedEdidEvoPtr *ppParsedEdid,
    NVEvoInfoStringPtr pInfoString);

char *nvGetDpyIdListStringEvo(NVDispEvoPtr pDispEvo,
                              const NVDpyIdList dpyIdList);

NvBool nvDpyGetDynamicData(
    NVDpyEvoPtr pDpyEvo,
    struct NvKmsQueryDpyDynamicDataParams *pParams);

void nvDpyUpdateCurrentAttributes(NVDpyEvoRec *pDpyEvo);

NvBool nvDpyIsAdaptiveSync(const NVDpyEvoRec *pDpyEvo);

NvBool nvDpyIsAdaptiveSyncDefaultlisted(const NVDpyEvoRec *pDpyEvo);

enum NvKmsDpyAttributeDigitalSignalValue
nvGetDefaultDpyAttributeDigitalSignalValue(const NVConnectorEvoRec *pConnectorEvo);

NvKmsDpyOutputColorFormatInfo nvDpyGetOutputColorFormatInfo(
    const NVDpyEvoRec *pDpyEvo);

NvU32 nvDpyGetPossibleApiHeadsMask(const NVDpyEvoRec *pDpyEvo);

NvBool nvDpyIsHDRCapable(const NVDpyEvoRec *pDpyEvo);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_DPY_H__ */
