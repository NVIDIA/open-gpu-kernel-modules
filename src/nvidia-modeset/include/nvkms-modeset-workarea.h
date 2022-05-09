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

#ifndef __NVKMS_MODESET_WORKAREA_H__
#define __NVKMS_MODESET_WORKAREA_H__

typedef struct {
    struct {
        struct {
            NVFlipEvoHwState newState;
            NVFlipEvoHwState oldState;
            NvU32 oldActiveRmId;
        } head[NVKMS_MAX_HEADS_PER_DISP];

        NVDpyIdList changedDpyIdList;

        NVDpyIdList sorAssignedConnectorsList;
        NvU32 assignedSorMask;

    } sd[NVKMS_MAX_SUBDEVICES];
    NVEvoUpdateState earlyUpdateState;
    NVEvoModesetUpdateState modesetUpdateState;

    /*
     * The display bandwidth values that NVKMS needs to allocate after the
     * modeset is complete.
     */
    NvU32 postModesetIsoBandwidthKBPS;
    NvU32 postModesetDramFloorKBPS;
} NVModeSetWorkArea;

struct NvKmsVrrTimings {
    struct {
        struct {
            NVHwModeTimingsEvo timings;
            NvBool adjusted;
        } head[NVKMS_MAX_HEADS_PER_DISP];
    } disp[NVKMS_MAX_SUBDEVICES];
};

#endif /* __NVKMS_MODESET_WORKAREA_H__ */
