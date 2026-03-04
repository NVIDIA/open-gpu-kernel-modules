/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_STATES_H__
#define __NVKMS_STATES_H__

#include "nvkms-types.h"

#include "g_nvkms-evo-states.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum NVEvoLockSignal {
    NV_EVO_LOCK_SIGNAL_FLIP_LOCK,
    NV_EVO_LOCK_SIGNAL_FRAME_LOCK,
    NV_EVO_LOCK_SIGNAL_RASTER_LOCK,
    NV_EVO_LOCK_SIGNAL_STEREO,
} NVEvoLockSignal;

typedef enum NVEvoLockAction {
    NV_EVO_PROHIBIT_LOCK,
    NV_EVO_PROHIBIT_LOCK_DISABLE,
    NV_EVO_LOCK_HEADS,
    NV_EVO_UNLOCK_HEADS,
    NV_EVO_ADD_FRAME_LOCK_SERVER,
    NV_EVO_REM_FRAME_LOCK_SERVER,
    NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC,
    NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC,
    NV_EVO_ADD_FRAME_LOCK_CLIENT,
    NV_EVO_REM_FRAME_LOCK_CLIENT,
    NV_EVO_ADD_FRAME_LOCK_REF,
    NV_EVO_REM_FRAME_LOCK_REF,
    NV_EVO_ADD_SLI_SECONDARY,
    NV_EVO_ADD_SLI_LAST_SECONDARY,
    NV_EVO_ADD_SLI_PRIMARY,
    NV_EVO_REM_SLI,
} NVEvoLockAction;

/* nv_evo.c */

NVEvoLockPin nvEvoGetPinForSignal(const NVDispEvoRec *,
                                  NVEvoSubDevPtr,
                                  NVEvoLockSignal);
NvBool nvEvoRefFrameLockSli(NVDispEvoPtr pDispEvo,
                            NVEvoSubDevPtr pEvoSubDev,
                            const NvU32 *pHeads);
NvBool nvEvoUnRefFrameLockSli(NVDispEvoPtr pDispEvo,
                              NVEvoSubDevPtr pEvoSubDev,
                              const NvU32 *pHeads);

/* nvkms-hw-states.c */

NvBool nvEvoLockHWStateNoLock(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockServerManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateLockHeadsFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimary(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliSecondary(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliLastSecondary(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliSecondaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliLastSecondaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliSecondaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliLastSecondaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliSecondaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliLastSecondaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);
NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, const NvU32 *pHeads);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_STATES_H__ */
