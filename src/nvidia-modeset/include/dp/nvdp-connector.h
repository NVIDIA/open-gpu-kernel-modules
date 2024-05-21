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

#ifndef __NVKMS_DP_NVDP_CONNECTOR_H__
#define __NVKMS_DP_NVDP_CONNECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvkms-types.h"

NVDPLibConnectorPtr nvDPCreateConnector(NVConnectorEvoPtr pConnectorEvo);

void nvDPNotifyLongPulse(NVConnectorEvoPtr pConnectorEvo,
                         NvBool connected);

void nvDPNotifyShortPulse(NVDPLibConnectorPtr pNVDpLibConnector);

void nvDPDestroyConnector(NVDPLibConnectorPtr pNVDpLibConnector);

NvBool nvDPIsLinkAwaitingTransition(NVConnectorEvoPtr pConnectorEvo);

NVDPLibModesetStatePtr nvDPLibCreateModesetState(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvU32 displayId,
    const NVDpyIdList dpyIdList,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc,
    const NVHwModeTimingsEvo *pTimings,
    const NVDscInfoEvoRec *pDscInfo);

void nvDPLibFreeModesetState(NVDPLibModesetStatePtr pDpLibModesetState);

NvBool nvDPLibIsModePossible(const NVDPLibConnectorRec *pDpLibConnector,
                             const NVDpLibIsModePossibleParamsRec *pParams,
                             NvU32 *pFailedHeadMask);

NvBool nvDPValidateModeForDpyEvo(
    const NVDpyEvoRec *pDpyEvo,
    const NVDpyAttributeColor *pDpyColor,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    const NVHwModeTimingsEvo *pTimings,
    const NvBool b2Heads1Or,
    NVDscInfoEvoRec *pDscInfo);

void nvDPPreSetMode(NVDPLibConnectorPtr pDpLibConnector,
                    const NVEvoModesetUpdateState *pModesetUpdateState);

void nvDPPostSetMode(NVDPLibConnectorPtr pDpLibConnector,
                     const NVEvoModesetUpdateState *pModesetUpdateState);

void nvDPPause(NVDPLibConnectorPtr pNVDpLibConnector);

NvBool nvDPResume(NVDPLibConnectorPtr pNVDpLibConnector, NvBool plugged);

void nvDPSetAllowMultiStreamingOneConnector(
    NVDPLibConnectorPtr pDpLibConnector,
    NvBool allowMST);

void nvDPSetAllowMultiStreaming(NVDevEvoPtr pDevEvo, NvBool allowMST);

enum NVDpLinkMode {
    NV_DP_LINK_MODE_OFF,
    NV_DP_LINK_MODE_SST,
    NV_DP_LINK_MODE_MST,
};

enum NVDpLinkMode nvDPGetActiveLinkMode(NVDPLibConnectorPtr pDpLibConnector);

void nvDPSetLinkHandoff(NVDPLibConnectorPtr pDpLibConnector, NvBool enable);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_DP_NVDP_CONNECTOR_H__ */
