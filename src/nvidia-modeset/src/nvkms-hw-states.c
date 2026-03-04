/*
 * SPDX-FileCopyrightText: Copyright (c) 2009 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/*
 * nvkms-hw-states.c - Defines how to set up EVO hardware for the given usage.
 * Used by the EVO state machines in nv_evo_states.c.
 */

#include "nvkms-types.h"
#include "nvkms-framelock.h"
#include "nvkms-evo-states.h"

/*
 * Listed below are the different locking topologies for scan lock
 *
 * ------ (raster lock)
 * ====== (frame lock)
 *
 * Config NoLock: No locking relationship between the heads
 *
 *  +--------+                  +------------+
 *  | Head A |                  | Heads B... |
 *  +--------+                  +------------+
 *
 *
 * Config LockHeads: Supports raster lock across two or more heads.
 *
 *  +--------+                    +--------+
 *  | Head A | -- Internal -+-->  | Head B |
 *  +--------+              |     +--------+
 *                          |
 *                          |     +--------+
 *                          +-->  | Head C |
 *                          |     +--------+
 *                          .    ...
 *
 * Config FrameLockClient: Supports frame lock clients across GPUs/systems,
 * one head per GPU
 *
 *  +--------+                  +-------------+
 *  | Gsync  | ==============>  | Head A/B... |
 *  +--------+                  +-------------+
 *      ^                            |
 *      +-------- External ----------+
 *
 *
 * Config FrameLockServer: Same as above, but generates timing for the
 * frame lock network
 *
 *  +--------+                 +-------------+
 *  | Gsync  |                 | Head A/B... |
 *  +--------+                 +-------------+
 *      ^                           |
 *      +-------------- External ---+
 *
 *
 * Config FrameLockClientManyHeads: Support frame lock across GPUs/systems,
 * two or more heads per GPU
 *
 *      +-------- External ---------------------------+
 *      V                                             |
 *  +--------+        +--------+                   +--------+
 *  | Gsync  | =====> | Head A | == Internal =+==> | Head B |
 *  +--------+        +--------+              |    +--------+
 *                                            |
 *                                            |    +--------+
 *                                            +==> | Head C |
 *                                            |    +--------+
 *                                            .   ...
 *
 * Config FrameLockServerManyHeads: Same as above, only this head
 * is driving timing for the frame lock network.
 *
 *      +-------- External ---------------------------+
 *      V                                             |
 *  +--------+        +--------+                   +--------+
 *  | Gsync  |        | Head A | == Internal =+==> | Head B |
 *  +--------+        +--------+              |    +--------+
 *                                            |
 *                                            |    +--------+
 *                                            +==> | Head C |
 *                                            |    +--------+
 *                                            .   ...
 *
 * Config LockHeadsFrameLockClient: Frame lock enabled on one head of a
 * GPU where two or more heads are raster-locked.
 * Config LockHeadsFrameLockClientManyHeads: Same, but two or more heads are
 * enabled.
 *
 *      +-------- External ---------------------------+
 *      V                                             |
 *  +--------+        +--------+                   +--------+
 *  | Gsync  | =====> | Head A | -- Internal -+--> | Head B |
 *  +--------+        +--------+              |    +--------+
 *                                            |
 *                                            |    +--------+
 *                                            +--> | Head C |
 *                                            |    +--------+
 *                                            .   ...
 *
 * Config LockHeadsFrameLockServer: Frame lock enabled on one head of a GPU
 * where two or more heads are raster-locked, and this head is driving timing
 * for the frame lock network.
 * Config LockHeadsFrameLockServerManyHeads: Same, but one head is frame
 * lock server and the others are frame lock clients.
 *
 *      +-------- External ---------------------------+
 *      V                                             |
 *  +--------+        +--------+                   +--------+
 *  | Gsync  |        | Head A | -- Internal -+--> | Head B |
 *  +--------+        +--------+              |    +--------+
 *                                            |
 *                                            |    +--------+
 *                                            +--> | Head C |
 *                                            |    +--------+
 *                                            .   ...
 *
 * Configs SliPrimary, SliSecondary, SliLastSecondary: Supports SLI.
 *
 *                      +-----------------+
 *                 +--- | Head A, subdev0 |
 *                 |    +-----------------+
 *             External
 *                 |    +-----------------+
 *                 +--> | Head A, subdev1 |
 *                 |    +-----------------+
 *                 |    +-----------------+
 *                 +--> | Head A, subdev2 |
 *                 |    +-----------------+
 *                 .            ...
 *
 *
 * Config LockHeadsSli{Primary,Secondary,LastSecondary}: Supports SLI with two
 * or more heads rasterlocked (primary or any secondary, independently).
 *
 *                  +-----------------+                 +---------------------+
 *             +--- | Head A, subdev0 | -- Internal --> | Heads B..., subdev0 |
 *             |    +-----------------+                 +---------------------+
 *         External
 *             |    +-----------------+                 +---------------------+
 *             +--> | Head A, subdev1 | -- Internal --> | Heads B..., subdev1 |
 *             |    +-----------------+                 +---------------------+
 *             |    +-----------------+
 *             +--> | Head A, subdev2 |
 *             |    +-----------------+
 *             .            ...
 *
 * The SliSecondary states also come in a FrameLockClient variant; this means that
 * they have framelock enabled in the RM (for reporting purposes; they still
 * get their sync from the SLI primary).
 *
 *
 * Config SliPrimaryFrameLockClient: Supports frame lock across GPU
 * groups/systems with SLI
 *
 *
 *       +===============================+
 *       I                               V
 *   +-------+                  +-----------------+
 *   | Gsync | <-----------+--- | Head A, subdev0 |
 *   +-------+             |    +-----------------+
 *                     External
 *                         |    +-----------------+
 *                         +--> | Head A, subdev1 |
 *                         |    +-----------------+
 *                         |    +-----------------+
 *                         +--> | Head A, subdev2 |
 *                         |    +-----------------+
 *                         .            ...
 *
 *
 * Config SliPrimaryFrameLockServer: Same as above, only this SLI head drives
 * timing for the frame lock network.
 *
 *   +-------+                  +-----------------+
 *   | Gsync | <-----------+--- | Head A, subdev0 |
 *   +-------+             |    +-----------------+
 *                     External
 *                         |    +-----------------+
 *                         +--> | Head A, subdev1 |
 *                         |    +-----------------+
 *                         |    +-----------------+
 *                         +--> | Head A, subdev2 |
 *                         |    +-----------------+
 *                         .            ...
 *
 *
 * Config SliPrimaryLockHeadsFrameLockClient: Supports frame lock across GPU
 * groups/systems with SLI, with two or more heads on a GPU rasterlocked
 * together.
 *
 *
 *     +======================+
 *     I                      V
 * +-------+         +-----------------+                 +---------------------+
 * | Gsync | <--+--- | Head A, subdev0 | -- Internal --> | Heads B..., subdev0 |
 * +-------+    |    +-----------------+                 +---------------------+
 *            External
 *              |    +-----------------+                 +---------------------+
 *              +--> | Head A, subdev1 | -- Internal --> | Heads B..., subdev1 |
 *              |    +-----------------+                 +---------------------+
 *              |    +-----------------+
 *              +--> | Head A, subdev2 |
 *              |    +-----------------+
 *              .            ...
 *
 *
 * Config SliPrimaryLockHeadsFrameLockServer: Same as above, only this SLI head
 * drives timing for the frame lock network.
 *
 * +-------+         +-----------------+                 +---------------------+
 * | Gsync | <--+--- | Head A, subdev0 | -- Internal --> | Heads B..., subdev0 |
 * +-------+    |    +-----------------+                 +---------------------+
 *            External
 *              |    +-----------------+                 +---------------------+
 *              +--> | Head A, subdev1 | -- Internal --> | Heads B..., subdev1 |
 *              |    +-----------------+                 +---------------------+
 *              |    +-----------------+
 *              +--> | Head A, subdev2 |
 *              |    +-----------------+
 *              .            ...
 *
 *
 * Note that for the SLI and framelock topologies we set the external fliplock
 * pin.  Changing the pin causes a raster reset for some reason, so we want to
 * change the pin here, prior to enabling flip lock.
 */

NvBool nvEvoLockHWStateNoLock(NVDispEvoPtr pDispEvo, NVEvoSubDevPtr pEvoSubDev,
                              const NvU32 *pHeads)
{
    unsigned int i;

    nvAssert(pHeads != NULL && pHeads[0] != NV_INVALID_HEAD);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (pHC->mergeMode) {
            /* MergeMode is currently handled outside of the scanlock state
             * machine, so don't touch the HeadControl state for heads with
             * mergeMode enabled.  (The state machine will be transitioned to
             * the 'ProhibitLock' state to prevent other states from being
             * reached, so this should be the only HWState function that needs
             * this special case.) */
            continue;
        }

        /* Disable scan lock on this head */
        pHC->serverLock = NV_EVO_NO_LOCK;
        pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->clientLock = NV_EVO_NO_LOCK;
        pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->clientLockoutWindow = 0;
        pHC->setLockOffsetX = FALSE;
        pHC->useStallLockPin = FALSE;
        pHC->stallLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->crashLockUnstallMode = FALSE;

        /* Reset the flip lock pin to internal, if not needed for SLI */
        if (!HEAD_MASK_QUERY(pEvoSubDev->flipLockPinSetForSliHeadMask, head)) {
            pHC->flipLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        }
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_UNSET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);

        /* Disable framelock */
        pEvoSubDev->frameLockServerMaskAssy &= ~(1 << head);
        pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
        pEvoSubDev->frameLockExtRefClkMaskAssy &= ~(1 << head);

        /* Reset SLI state */
        pEvoSubDev->sliRasterLockServerMask &= ~(1 << head);
        pEvoSubDev->sliRasterLockClientMask &= ~(1 << head);

        pHC->lockChainPosition = 0;
    }

    pEvoSubDev->frameLockHouseSync = FALSE;

    return TRUE;
}

NvBool nvEvoLockHWStateLockHeads(NVDispEvoPtr pDispEvo,
                                 NVEvoSubDevPtr pEvoSubDev,
                                 const NvU32 *pHeads)
{
    unsigned int i, serverHead = 0;

    nvAssert(pHeads != NULL &&
             pHeads[0] != NV_INVALID_HEAD &&
             pHeads[1] != NV_INVALID_HEAD);

    /* First, disable all scan locking */
    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /* Make the first head a raster lock server on the internal pin */
            pHC->serverLock = NV_EVO_RASTER_LOCK;
            pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(head);
            serverHead = head;
        } else {
            /* Make all the other heads raster lock clients on the internal pin */
            pHC->clientLock = NV_EVO_RASTER_LOCK;
            pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(serverHead);
            pHC->clientLockoutWindow = 2;
        }
    }

    return TRUE;
}

NvBool nvEvoLockHWStateFrameLockClient(NVDispEvoPtr pDispEvo,
                                       NVEvoSubDevPtr pEvoSubDev,
                                       const NvU32 *pHeads)

{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_FRAME_LOCK);

    if (pin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* Set up for the FRAME_LOCK_SERVER state */
    if (!nvEvoLockHWStateFrameLockServer(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    nvAssert(pHeads != NULL && pHeads[0] != NV_INVALID_HEAD);

    /* Additionally enable the first head as a frame lock client */
    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    pHC->clientLock = NV_EVO_FRAME_LOCK;
    pHC->clientLockPin = pin;

    pEvoSubDev->frameLockServerMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockClientMaskAssy |= 1 << head;

    return TRUE;
}

NvBool nvEvoLockHWStateFrameLockServer(NVDispEvoPtr pDispEvo,
                                       NVEvoSubDevPtr pEvoSubDev,
                                       const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_RASTER_LOCK);
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* disable all scan locking */
    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    nvAssert(pHeads != NULL && pHeads[0] != NV_INVALID_HEAD);

    /* Enable the first head as a raster lock server on the external pin */
    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    pHC->serverLock = NV_EVO_RASTER_LOCK;
    pHC->serverLockPin = pin;

    /* Set up the first head to use the external flip lock pin */
    pHC->flipLockPin = flPin;
    pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
        HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);

    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;

    return TRUE;
}

NvBool nvEvoLockHWStateFrameLockServerHouseSync(NVDispEvoPtr pDispEvo,
                                                NVEvoSubDevPtr pEvoSubDev,
                                                const NvU32 *pHeads)
{
    /* As far as EVO is concerned, House Sync means FL client */
    if (!nvEvoLockHWStateFrameLockClient(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    const int head = pHeads[0];
    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockHouseSync = TRUE;

    return TRUE;
}

NvBool nvEvoLockHWStateFrameLockClientManyHeads(NVDispEvoPtr pDispEvo,
                                                NVEvoSubDevPtr pEvoSubDev,
                                                const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_FRAME_LOCK);

    if (pin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* Set up as a frame lock server with two heads */
    if (!nvEvoLockHWStateFrameLockServerManyHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    /* Additionally enable the first head as a frame lock client */
    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    pHC->clientLock = NV_EVO_FRAME_LOCK;
    pHC->clientLockPin = pin;

    pEvoSubDev->frameLockServerMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockClientMaskAssy |= 1 << head;

    return TRUE;
}

NvBool nvEvoLockHWStateFrameLockServerManyHeads(NVDispEvoPtr pDispEvo,
                                                NVEvoSubDevPtr pEvoSubDev,
                                                const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_RASTER_LOCK);
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i, serverHead = 0;

    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* Disable all scan lock */
    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /* Make the first head a frame lock server on the internal pin.
             * The first head is guaranteed to be framelock server or one of
             * the requested framelock clients here
             */
            nvAssert(nvIsFramelockableHead(pDispEvo, head));

            pHC->serverLock = NV_EVO_FRAME_LOCK;
            pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(head);
            serverHead = head;

            pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
        } else {
            /* If two or more heads are framelocked, but at least one head
             * cannot be framelocked with the others, that head will be in
             * the list of pDpys, but must not be framelocked, so skip it.
             */

            if (!nvIsFramelockableHead(pDispEvo, head)) {
                continue;
            }
            if (i == 1) {
                /* Make the second head a raster lock server on the external pin */
                pHC->serverLock = NV_EVO_RASTER_LOCK;
                pHC->serverLockPin = pin;
            }
            /* Make all nonzero heads a frame lock client on the internal pin */
            pHC->clientLock = NV_EVO_FRAME_LOCK;
            pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(serverHead);

            pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        }

        /* Set up all heads to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);

        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;
    }

     return TRUE;
}
NvBool nvEvoLockHWStateFrameLockServerHouseSyncManyHeads(NVDispEvoPtr pDispEvo,
                                                         NVEvoSubDevPtr pEvoSubDev,
                                                         const NvU32 *pHeads)
{
    /* As far as EVO is concerned, House Sync means FL client */
    if (!nvEvoLockHWStateFrameLockClientManyHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    const int head = pHeads[0];
    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockHouseSync = TRUE;

    return TRUE;
}

NvBool nvEvoLockHWStateLockHeadsFrameLockServer(NVDispEvoPtr pDispEvo,
                                                NVEvoSubDevPtr pEvoSubDev,
                                                const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_RASTER_LOCK);
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i, serverHead = 0;

    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* Disable all scan lock */
    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        /* Make the first head a raster lock server on the internal pin */
        if (i == 0) {
            pHC->serverLock = NV_EVO_RASTER_LOCK;
            pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(head);
            serverHead = head;

            pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
        } else {
            if (i == 1) {
                /* Make the second head a raster lock server on the external pin */
                pHC->serverLock = NV_EVO_RASTER_LOCK;
                pHC->serverLockPin = pin;
            }

            /* Make all nonzero heads raster lock clients on the internal pin */
            pHC->clientLock = NV_EVO_RASTER_LOCK;
            pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(serverHead);
            pHC->clientLockoutWindow = 2;

            pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        }

        /* Set up all heads to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);

        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;
    }

     return TRUE;
}

NvBool nvEvoLockHWStateLockHeadsFrameLockServerHouseSync(NVDispEvoPtr pDispEvo,
                                                         NVEvoSubDevPtr pEvoSubDev,
                                                         const NvU32 *pHeads)
{
    /* As far as EVO is concerned, House Sync means FL client */
    if (!nvEvoLockHWStateLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    const int head = pHeads[0];
    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockHouseSync = TRUE;

    return TRUE;
}

NvBool nvEvoLockHWStateLockHeadsFrameLockClient(NVDispEvoPtr pDispEvo,
                                                NVEvoSubDevPtr pEvoSubDev,
                                                const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_FRAME_LOCK);

    if (pin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* Set up for the LOCK_HEADS_FRAME_LOCK_SERVER state */
    if (!nvEvoLockHWStateLockHeadsFrameLockServer(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    /* Additionally, enable the first head as a frame lock client */
    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    pHC->clientLock = NV_EVO_FRAME_LOCK;
    pHC->clientLockPin = pin;

    pEvoSubDev->frameLockServerMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockClientMaskAssy |= 1 << head;

    return TRUE;
}

static void SetLockChainPosition(NVDispEvoPtr pDispEvo,
                                 NVEvoSubDevPtr pEvoSubDev,
                                 NVEvoHeadControlPtr pHC)
{
    if (pDispEvo->displayOwner == pEvoSubDev->subDeviceInstance) {
        /*
         * When we own display (even if subDeviceInstance != 0), set
         * lockChainPosition of 0, since we are actually scanning out pixels
         * (this is the case for all SLI Mosaic and non-Mosaic display owners).
         */
        pHC->lockChainPosition = 0;
    } else {
        /*
         * If we don't own display, just assume the video bridge chain is
         * linear
         */
        pHC->lockChainPosition = pEvoSubDev->subDeviceInstance;
    }
}

NvBool nvEvoLockHWStateSliPrimary(NVDispEvoPtr pDispEvo,
                                  NVEvoSubDevPtr pEvoSubDev,
                                  const NvU32 *pHeads)
{
    NVEvoLockPin pin = pEvoSubDev->sliServerLockPin;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    nvAssert(pHeads[0] != NV_INVALID_HEAD);
    nvAssert(pHeads[1] == NV_INVALID_HEAD);

    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    pHC->serverLock = NV_EVO_RASTER_LOCK;
    pHC->serverLockPin = pin;

    pEvoSubDev->sliRasterLockServerMask |= 1 << head;

    SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryLockHeads(NVDispEvoPtr pDispEvo,
                                           NVEvoSubDevPtr pEvoSubDev,
                                           const NvU32 *pHeads)
{
    NVEvoLockPin pin = pEvoSubDev->sliServerLockPin;
    unsigned int i;

    if (pin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    /* LockHeads sets up server lock on the first head, client lock on the rest */
    nvEvoLockHWStateLockHeads(pDispEvo, pEvoSubDev, pHeads);

    nvAssert(pHeads != NULL &&
             pHeads[0] != NV_INVALID_HEAD &&
             pHeads[1] != NV_INVALID_HEAD);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /*
             * The first head is configured as rasterlock server on its
             * internal pin.  It serves as the server for everything else on
             * this GPU, as well as (indirectly though another head) everything
             * in the SLI group.
             */
            pEvoSubDev->sliRasterLockServerMask |= 1 << head;
        } else {
            if (i == 1) {
                /*
                 * The first rasterlock client on this GPU also serves as server
                 * for the rest of the SLI device
                 */
                pHC->serverLock = NV_EVO_RASTER_LOCK;
                pHC->serverLockPin = pin;
            }

            /* All of these heads should inherit extrefclk from the server */
            pEvoSubDev->sliRasterLockClientMask |= 1 << head;
        }

        SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliSecondary(NVDispEvoPtr pDispEvo,
                                    NVEvoSubDevPtr pEvoSubDev,
                                    const NvU32 *pHeads)
{
    NVEvoLockPin serverPin = pEvoSubDev->sliServerLockPin;
    NVEvoLockPin clientPin = pEvoSubDev->sliClientLockPin;
    const NvU32 clientLockoutWindow = 2;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

    if (clientPin == NV_EVO_LOCK_PIN_ERROR ||
        serverPin == NV_EVO_LOCK_PIN_ERROR ||
        flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    nvAssert(pHeads[0] != NV_INVALID_HEAD);
    nvAssert(pHeads[1] == NV_INVALID_HEAD);

    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    /* Server lock to be consumed by GPUs further down the chain */
    pHC->serverLock = NV_EVO_RASTER_LOCK;
    pHC->serverLockPin = serverPin;

    /* Client lock to sync to GPUs further up the chain */
    pHC->clientLock = NV_EVO_RASTER_LOCK;
    pHC->clientLockPin = clientPin;
    pHC->clientLockoutWindow = clientLockoutWindow;

    pEvoSubDev->sliRasterLockClientMask |= 1 << head;

    SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);

    return TRUE;
}

NvBool nvEvoLockHWStateSliSecondaryFrameLockClient(NVDispEvoPtr pDispEvo,
                                                   NVEvoSubDevPtr pEvoSubDev,
                                                   const NvU32 *pHeads)
{
    if (!nvEvoLockHWStateSliSecondary(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    nvAssert(pHeads[0] != NV_INVALID_HEAD);
    nvAssert(pHeads[1] == NV_INVALID_HEAD);

    pEvoSubDev->frameLockClientMaskAssy |= 1 << pHeads[0];

    return TRUE;
}

NvBool nvEvoLockHWStateSliLastSecondary(NVDispEvoPtr pDispEvo,
                                        NVEvoSubDevPtr pEvoSubDev,
                                        const NvU32 *pHeads)
{
    NVEvoLockPin clientPin = pEvoSubDev->sliClientLockPin;
    const NvU32 clientLockoutWindow = 2;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

    if (clientPin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    nvAssert(pHeads[0] != NV_INVALID_HEAD);
    nvAssert(pHeads[1] == NV_INVALID_HEAD);

    const int head = pHeads[0];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

    /* Only set up client lock; no more GPUs to consume server lock */
    pHC->clientLock = NV_EVO_RASTER_LOCK;
    pHC->clientLockPin = clientPin;
    pHC->clientLockoutWindow = clientLockoutWindow;

    pEvoSubDev->sliRasterLockClientMask |= 1 << head;

    SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);

    return TRUE;
}

NvBool nvEvoLockHWStateSliLastSecondaryFrameLockClient(NVDispEvoPtr pDispEvo,
                                                       NVEvoSubDevPtr pEvoSubDev,
                                                       const NvU32 *pHeads)
{
    if (!nvEvoLockHWStateSliLastSecondary(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    nvAssert(pHeads[0] != NV_INVALID_HEAD);
    nvAssert(pHeads[1] == NV_INVALID_HEAD);

    const int head = pHeads[0];
    pEvoSubDev->frameLockClientMaskAssy |= 1 << head;

    return TRUE;
}

NvBool nvEvoLockHWStateSliSecondaryLockHeads(NVDispEvoPtr pDispEvo,
                                             NVEvoSubDevPtr pEvoSubDev,
                                             const NvU32 *pHeads)
{
    NVEvoLockPin serverPin = pEvoSubDev->sliServerLockPin;
    NVEvoLockPin clientPin = pEvoSubDev->sliClientLockPin;
    const NvU32 clientLockoutWindow = 2;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i;

    if (clientPin == NV_EVO_LOCK_PIN_ERROR || serverPin == NV_EVO_LOCK_PIN_ERROR ||
        flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /*
             * first head (chosen arbitrarily): server lock to be consumed by
             * GPUs further down the chain
             */
            pHC->serverLock = NV_EVO_RASTER_LOCK;
            pHC->serverLockPin = serverPin;
        }

        /*
         * Client lock all heads to the external SLI pin.  Note that we cannot
         * client lock one head and set up internal locking for the other
         * because of bug 405996.
         */
        pHC->clientLock = NV_EVO_RASTER_LOCK;
        pHC->clientLockPin = clientPin;
        pHC->clientLockoutWindow = clientLockoutWindow;

        pEvoSubDev->sliRasterLockClientMask |= 1 << head;

        SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliSecondaryLockHeadsFrameLockClient(NVDispEvoPtr pDispEvo,
                                                            NVEvoSubDevPtr pEvoSubDev,
                                                            const NvU32 *pHeads)
{
    unsigned int i;

    if (!nvEvoLockHWStateSliSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        pEvoSubDev->frameLockClientMaskAssy |= 1 << pHeads[i];
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliLastSecondaryLockHeads(NVDispEvoPtr pDispEvo,
                                                 NVEvoSubDevPtr pEvoSubDev,
                                                 const NvU32 *pHeads)
{
    NVEvoLockPin clientPin = pEvoSubDev->sliClientLockPin;
    const NvU32 clientLockoutWindow = 2;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i;

    if (clientPin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    nvEvoLockHWStateNoLock(pDispEvo, pEvoSubDev, pHeads);

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        /*
         * Client lock all heads to the external SLI pin.  Note that we cannot
         * client lock one head and set up internal locking for the other
         * because of bug 405996.
         */
        pHC->clientLock = NV_EVO_RASTER_LOCK;
        pHC->clientLockPin = clientPin;
        pHC->clientLockoutWindow = clientLockoutWindow;

        pEvoSubDev->sliRasterLockClientMask |= 1 << head;

        SetLockChainPosition(pDispEvo, pEvoSubDev, pHC);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliLastSecondaryLockHeadsFrameLockClient(NVDispEvoPtr pDispEvo,
                                                                NVEvoSubDevPtr pEvoSubDev,
                                                                const NvU32 *pHeads)
{
    unsigned int i;

    if (!nvEvoLockHWStateSliLastSecondaryLockHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        pEvoSubDev->frameLockClientMaskAssy |= 1 << pHeads[i];
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryFrameLockServer(NVDispEvoPtr pDispEvo,
                                                 NVEvoSubDevPtr pEvoSubDev,
                                                 const NvU32 *pHeads)
{
    unsigned int i;
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

    if (flPin == NV_EVO_LOCK_PIN_ERROR ||
        !nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];

        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
        } else {
            pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        }

        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;

        /* Set up this head to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryFrameLockServerHouseSync(NVDispEvoPtr pDispEvo,
                                                          NVEvoSubDevPtr pEvoSubDev,
                                                          const NvU32 *pHeads)
{
    /* As far as EVO is concerned, House Sync means FL client */
    if (!nvEvoLockHWStateSliPrimaryFrameLockClient(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    const int head = pHeads[0];
    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockHouseSync = TRUE;

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryFrameLockClient(NVDispEvoPtr pDispEvo,
                                                 NVEvoSubDevPtr pEvoSubDev,
                                                 const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_FRAME_LOCK);
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i;


    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    if (!nvEvoLockHWStateSliPrimary(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /* Enable first head as framelock client */
            pHC->clientLock = NV_EVO_FRAME_LOCK;
            pHC->clientLockPin = pin;
        }

        pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;

        /* Set up this head to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServer(NVDispEvoPtr pDispEvo,
                                                          NVEvoSubDevPtr pEvoSubDev,
                                                          const NvU32 *pHeads)
{
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i;

    if (flPin == NV_EVO_LOCK_PIN_ERROR ||
        !nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
        } else {
            pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        }

        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;

        /* Set up this head to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);
    }

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockServerHouseSync(NVDispEvoPtr pDispEvo,
                                                                   NVEvoSubDevPtr pEvoSubDev,
                                                                   const NvU32 *pHeads)
{
    /* As far as EVO is concerned, House Sync means FL client */
    if (!nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    const int head = pHeads[0];
    pEvoSubDev->frameLockServerMaskAssy |= 1 << head;
    pEvoSubDev->frameLockClientMaskAssy &= ~(1 << head);
    pEvoSubDev->frameLockHouseSync = TRUE;

    return TRUE;
}

NvBool nvEvoLockHWStateSliPrimaryLockHeadsFrameLockClient(NVDispEvoPtr pDispEvo,
                                                          NVEvoSubDevPtr pEvoSubDev,
                                                          const NvU32 *pHeads)
{
    NVEvoLockPin pin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                            NV_EVO_LOCK_SIGNAL_FRAME_LOCK);
    NVEvoLockPin flPin = nvEvoGetPinForSignal(pDispEvo, pEvoSubDev,
                                              NV_EVO_LOCK_SIGNAL_FLIP_LOCK);
    unsigned int i;

    if (pin == NV_EVO_LOCK_PIN_ERROR || flPin == NV_EVO_LOCK_PIN_ERROR) {
        return FALSE;
    }

    if (!nvEvoLockHWStateSliPrimaryLockHeads(pDispEvo, pEvoSubDev, pHeads)) {
        return FALSE;
    }

    for (i = 0; pHeads[i] != NV_INVALID_HEAD; i++) {
        const int head = pHeads[i];
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];

        if (i == 0) {
            /* Enable first head as framelock client */
            pHC->clientLock = NV_EVO_FRAME_LOCK;
            pHC->clientLockPin = pin;
        }

        pEvoSubDev->frameLockClientMaskAssy |= 1 << head;
        pEvoSubDev->frameLockExtRefClkMaskAssy |= 1 << head;

        /* Set up this head to use the external flip lock pin */
        pHC->flipLockPin = flPin;
        pEvoSubDev->flipLockPinSetForFrameLockHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockPinSetForFrameLockHeadMask, head);
    }

    return TRUE;
}
