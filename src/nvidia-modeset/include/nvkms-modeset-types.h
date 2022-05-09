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

#ifndef __NVKMS_MODESET_TYPES_H__
#define __NVKMS_MODESET_TYPES_H__

/* This header file defines types used internally by the modeset path. */

#include "nvkms-types.h"

typedef struct {
    NVHwModeTimingsEvo timings;
    NVDpyIdList dpyIdList;
    NVConnectorEvoRec *pConnectorEvo;
    NvU32 activeRmId;
    struct NvKmsSetLutCommonParams lut;
    NvU8 allowFlipLockGroup;
    enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace;
    enum NvKmsDpyAttributeColorRangeValue colorRange;
    struct NvKmsModeValidationParams modeValidationParams;
    NvBool changed                       : 1;
    NvBool allowGsync                    : 1;
    NvBool hs10bpcHint                   : 1;
    enum NvKmsAllowAdaptiveSync allowAdaptiveSync;
    NvU32 vrrOverrideMinRefreshRate;
    NVDPLibModesetStatePtr pDpLibModesetState;
    NVDispHeadAudioStateEvoRec audio;
} NVProposedModeSetHwStateOneHead;

typedef struct {
    NVProposedModeSetHwStateOneHead head[NVKMS_MAX_HEADS_PER_DISP];
} NVProposedModeSetHwStateOneDisp;

typedef struct {
    struct {
        NVFlipEvoHwState flip;
    } head[NVKMS_MAX_HEADS_PER_DISP];
} NVProposedModeSetHwStateOneSubDev;

typedef struct {
    NVProposedModeSetHwStateOneDisp disp[NVKMS_MAX_SUBDEVICES];
    NVProposedModeSetHwStateOneSubDev sd[NVKMS_MAX_SUBDEVICES];
    NvBool allowHeadSurfaceInNvKms       : 1;
} NVProposedModeSetHwState;

struct _NVEvoModesetUpdateState {
    NVEvoUpdateState updateState;
    NVDpyIdList connectorIds;
    const NVDPLibModesetStateRec
        *pDpLibModesetState[NVKMS_MAX_HEADS_PER_DISP];
    NvBool windowMappingChanged;
};

#endif /* __NVKMS_MODESET_TYPES_H__ */
