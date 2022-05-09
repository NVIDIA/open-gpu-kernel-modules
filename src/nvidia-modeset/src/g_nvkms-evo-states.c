/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2010 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-evo-states.h"

static NvBool EvoLockStateFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockClientManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockClientPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerHouseSyncManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerHouseSyncManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerHouseSyncPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateFrameLockServerPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockClientManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockClientPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsFrameLockServerPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateLockHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateNoLock(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliLastSecondary(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliLastSecondaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliLastSecondaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliLastSecondaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliLastSecondaryLockHeadsFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimary(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockClientPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockServerHouseSyncPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryFrameLockServerPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServer(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSync(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliSecondary(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliSecondaryFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliSecondaryLockHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliSecondaryLockHeadsFrameLockClient(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateSliSecondaryLockHeadsFrameLockClientManyHeads(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);
static NvBool EvoLockStateVrr(NVDispEvoPtr, NVEvoSubDevPtr, NVEvoLockAction, const NvU32 *pHeads);

static NvBool EvoLockStateFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClientManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockClientManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClientManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockClient;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockClientManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockClientManyHeads(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockClientManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockClientPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockClientPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClientManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockClient;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateNoLock;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServer(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSync;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerHouseSync(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServer;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerHouseSyncManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerHouseSyncManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerHouseSyncPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServer;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServerPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateFrameLockServerPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateFrameLockServer;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_PRIMARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeads;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_LAST_SECONDARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryLockHeads;
        }
        return TRUE;

    case NV_EVO_UNLOCK_HEADS:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServer;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_SECONDARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondaryLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockClientManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockClientManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockClientPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServer(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSync;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerHouseSync(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServer;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerHouseSyncPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServer;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsFrameLockServerPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServer;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateLockHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateLockHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateNoLock(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockClient;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_PRIMARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_LAST_SECONDARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondary;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateFrameLockServer;
        }
        return TRUE;

    case NV_EVO_LOCK_HEADS:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    case NV_EVO_ENABLE_VRR:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateVrr;
        }
        return TRUE;

    case NV_EVO_ADD_SLI_SECONDARY:
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliLastSecondary(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!nvEvoRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliLastSecondaryFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!nvEvoUnRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliLastSecondaryLockHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!nvEvoRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliLastSecondaryLockHeadsFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryLockHeadsFrameLockClientManyHeads;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!nvEvoUnRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliLastSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliLastSecondaryLockHeadsFrameLockClientManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliLastSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliLastSecondaryLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimary(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockClient;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServer;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockClientPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockClient;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockServer(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerHouseSync;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockServerHouseSync(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServer;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockServerHouseSyncPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServer;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryFrameLockServerPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimary;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryFrameLockServer;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServer;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServer(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSync;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSync(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServer;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSync;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeads;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServer;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockClientPlusRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeads;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliPrimaryLockHeadsFrameLockServerPlusRef(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerManyHeadsPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServerHouseSyncPlusRef;
        }
        return TRUE;

    case NV_EVO_ADD_FRAME_LOCK_REF:
        if (!queryOnly) {
            pEvoSubDev->frameLockSliProxyClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_SERVER:
        if (!queryOnly) {
            nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockRef;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_REF:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockSliProxyClients) {
                nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliPrimaryLockHeadsFrameLockServer;
            } else {
                pEvoSubDev->frameLockSliProxyClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliSecondary(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!nvEvoRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondaryFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliSecondaryFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!nvEvoUnRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondary(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondary;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliSecondaryLockHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!nvEvoRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondaryLockHeadsFrameLockClient;
        }
        return TRUE;

    case NV_EVO_REM_SLI:
        if (!queryOnly) {
            nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliSecondaryLockHeadsFrameLockClient(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondaryLockHeadsFrameLockClientManyHeads;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!nvEvoUnRefFrameLockSli(pDispEvo, pEvoSubDev, pHeads)) {
            return FALSE;
        }
        if (!queryOnly) {
            nvEvoLockHWStateSliSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateSliSecondaryLockHeads;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateSliSecondaryLockHeadsFrameLockClientManyHeads(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_ADD_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            pEvoSubDev->frameLockExtraClients++;
        }
        return TRUE;

    case NV_EVO_REM_FRAME_LOCK_CLIENT:
        if (!queryOnly) {
            if (!pEvoSubDev->frameLockExtraClients) {
                nvEvoLockHWStateSliSecondaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads);
                pEvoSubDev->scanLockState = EvoLockStateSliSecondaryLockHeadsFrameLockClient;
            } else {
                pEvoSubDev->frameLockExtraClients--;
            }
        }
        return TRUE;

    default:
        return FALSE;
    }
}

static NvBool EvoLockStateVrr(
    NVDispEvoPtr pDispEvo,
    NVEvoSubDevPtr pEvoSubDev,
    NVEvoLockAction action,
    const NvU32 *pHeads
)
{
    NvBool queryOnly = pHeads == NULL;

    switch (action) {

    case NV_EVO_DISABLE_VRR:
        if (!queryOnly) {
            nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);
            pEvoSubDev->scanLockState = EvoLockStateNoLock;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

void nvEvoStateStartNoLock(
    NVEvoSubDevPtr pEvoSubDev
)
{
    pEvoSubDev->scanLockState = EvoLockStateNoLock;
}

