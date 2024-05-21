/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_FLIP_WORKAREA_H__
#define __NVKMS_FLIP_WORKAREA_H__

#include "nvkms-types.h"

typedef struct {
    struct {
        enum NvKmsOutputTf tf;
        NVDpyAttributeColor dpyColor;
        NvBool infoFrameOverride;
        NvU32 staticMetadataLayerMask;
    } hdr;

    struct NvKmsPoint viewPortPointIn;
    struct NvKmsSetLutCommonParams lut;

    struct {
        NvU32 viewPortPointIn  : 1;
        NvU32 hdr              : 1;
    } dirty;
} NVProposedFlipStateOneApiHead;

struct NvKmsFlipWorkArea {
    struct {
        NvBool changed;
        struct {
            /*
             * Pre flip usage bounds are the union of current and new
             * usable usage bounds: the unioned usage bounds have to
             * allow both the current state and the state being flipped to.
             * This field is set and used by PreFlipIMP() and its
             * helper functions.
             */
            struct NvKmsUsageBounds preFlipUsage;

            NVFlipEvoHwState newState;
            NVFlipEvoHwState oldState;
        } head[NVKMS_MAX_HEADS_PER_DISP];
    } sd[NVKMS_MAX_SUBDEVICES];

    struct {
        struct {
            NVProposedFlipStateOneApiHead proposedFlipState;
        } apiHead[NVKMS_MAX_HEADS_PER_DISP];
    } disp[NVKMS_MAX_SUBDEVICES];

    NVEvoUpdateState updateState;
};

#endif /* __NVKMS_FLIP_WORKAREA_H__ */
