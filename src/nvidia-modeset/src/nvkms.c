/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms.h"
#include "nvkms-private.h"
#include "nvkms-api.h"

#include "nvkms-types.h"
#include "nvkms-utils.h"
#include "nvkms-console-restore.h"
#include "nvkms-dpy.h"
#include "nvkms-dma.h"
#include "nvkms-evo.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-modepool.h"
#include "nvkms-modeset.h"
#include "nvkms-attributes.h"
#include "nvkms-framelock.h"
#include "nvkms-surface.h"
#include "nvkms-3dvision.h"
#include "nvkms-ioctl.h"
#include "nvkms-cursor.h" /* nvSetCursorImage, nvEvoMoveCursor */
#include "nvkms-flip.h" /* nvFlipEvo */
#include "nvkms-vrr.h"

#include "dp/nvdp-connector.h"

#include "nvUnixVersion.h" /* NV_VERSION_STRING */
#include <class/cl0000.h> /* NV01_NULL_OBJECT/NV01_ROOT */

#include "nv_list.h"


/*! \file
 *
 * This source file implements the API of NVKMS, built around open,
 * close, and ioctl file operations.
 *
 * An NvKmsPerOpen is stored "per-open"; all API handles are specific
 * to a per-open instance.  The NvKmsPerOpen is allocated during each
 * nvKmsOpen() call, and freed during the corresponding nvKmsClose()
 * call.
 *
 * An NvKmsPerOpenDev stores the API handles for the device and all
 * the disps and connectors on the device.  It is allocated during
 * nvKmsIoctl(ALLOC_DEVICE), and freed during nvKmsIoctl(FREE_DEVICE).
 */


/*
 * When the NVKMS device file is opened, the per-open structure could
 * be used for one of several actions, denoted by its "type".  The
 * per-open type starts as Undefined.  The per-open's first use
 * defines its type.  Once the type transitions from Undefined to
 * anything, it can never transition to any other type.
 */
enum NvKmsPerOpenType {
    /*
     * The per-open is used for making ioctl calls to make requests of
     * NVKMS.
     */
    NvKmsPerOpenTypeIoctl,

    /*
     * The per-open is used for granting access to a NVKMS registered
     * surface.
     */
    NvKmsPerOpenTypeGrantSurface,

    /*
     * The per-open is used for granting permissions.
     */
    NvKmsPerOpenTypeGrantPermissions,

    /*
     * The per-open is used for granting access to a swap group
     */
    NvKmsPerOpenTypeGrantSwapGroup,

    /*
     * The per-open is used to unicast a specific event.
     */
    NvKmsPerOpenTypeUnicastEvent,

    /*
     * The per-open is currently undefined (this is the initial
     * state).
     */
    NvKmsPerOpenTypeUndefined,
};

struct NvKmsPerOpenConnector {
    NVConnectorEvoPtr            pConnectorEvo;
    NvKmsConnectorHandle         nvKmsApiHandle;
};

struct NvKmsPerOpenFrameLock {
    NVFrameLockEvoPtr            pFrameLockEvo;
    int                          refCnt;
    NvKmsFrameLockHandle         nvKmsApiHandle;
};

struct NvKmsPerOpenDisp {
    NVDispEvoPtr                 pDispEvo;
    NvKmsDispHandle              nvKmsApiHandle;
    NvKmsFrameLockHandle         frameLockHandle;
    NVEvoApiHandlesRec           connectorHandles;
    struct NvKmsPerOpenConnector connector[NVKMS_MAX_CONNECTORS_PER_DISP];
    NVEvoApiHandlesRec           vblankSyncObjectHandles[NVKMS_MAX_HEADS_PER_DISP];
};

struct NvKmsPerOpenDev {
    NVDevEvoPtr                  pDevEvo;
    NvKmsDeviceHandle            nvKmsApiHandle;
    NVEvoApiHandlesRec           dispHandles;
    NVEvoApiHandlesRec           surfaceHandles;
    struct NvKmsFlipPermissions  flipPermissions;
    struct NvKmsModesetPermissions modesetPermissions;
    struct NvKmsPerOpenDisp      disp[NVKMS_MAX_SUBDEVICES];
    NvBool                       isPrivileged;
    NVEvoApiHandlesRec           deferredRequestFifoHandles;
};

struct NvKmsPerOpenEventListEntry {
    NVListRec                    eventListEntry;
    struct NvKmsEvent            event;
};

struct NvKmsPerOpen {
    nvkms_per_open_handle_t     *pOpenKernel;
    NvU32                        pid;
    enum NvKmsClientType         clientType;
    NVListRec                    perOpenListEntry;
    NVListRec                    perOpenIoctlListEntry;
    enum NvKmsPerOpenType        type;

    union {
        struct {
            NVListRec            eventList;
            NvU32                eventInterestMask;
            NVEvoApiHandlesRec   devHandles;
            NVEvoApiHandlesRec   frameLockHandles;
        } ioctl;

        struct {
            NVSurfaceEvoPtr      pSurfaceEvo;
        } grantSurface;

        struct {
            NVDevEvoPtr          pDevEvo;
            NVSwapGroupPtr       pSwapGroup;
        } grantSwapGroup;

        struct {
            NVDevEvoPtr          pDevEvo;
            struct NvKmsPermissions permissions;
        } grantPermissions;

        struct {
            /*
             * A unicast event NvKmsPerOpen is assigned to an object, so that
             * that object can generate events on the unicast event.  Store a
             * pointer to that object, so that we can clear the pointer when the
             * unicast event NvKmsPerOpen is closed.
             *
             * So far, deferred request fifos with swap groups are the only
             * users of unicast events.  When we add more users, we can add an
             * enum or similar to know which object type is using this unicast
             * event.
             */
            NVDeferredRequestFifoPtr pDeferredRequestFifo;
        } unicastEvent;
    };
};

static void AllocSurfaceCtxDmasForAllOpens(NVDevEvoRec *pDevEvo);
static void FreeSurfaceCtxDmasForAllOpens(NVDevEvoRec *pDevEvo);

static NVListRec perOpenList = NV_LIST_INIT(&perOpenList);
static NVListRec perOpenIoctlList = NV_LIST_INIT(&perOpenIoctlList);

/*!
 * Check if there is an NvKmsPerOpenDev on this NvKmsPerOpen that has
 * the specified deviceId.
 */
static NvBool DeviceIdAlreadyPresent(struct NvKmsPerOpen *pOpen, NvU32 deviceId)
{
    struct NvKmsPerOpenDev *pOpenDev;
    NvKmsGenericHandle dev;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                        pOpenDev, dev) {
        if (pOpenDev->pDevEvo->usesTegraDevice &&
            (deviceId == NVKMS_DEVICE_ID_TEGRA)) {
            return TRUE;
        } else if (pOpenDev->pDevEvo->deviceId == deviceId) {
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * Get the NvKmsPerOpenDev described by NvKmsPerOpen + deviceHandle.
 */
static struct NvKmsPerOpenDev *GetPerOpenDev(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle)
{
    if (pOpen == NULL) {
        return NULL;
    }

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    return nvEvoGetPointerFromApiHandle(&pOpen->ioctl.devHandles, deviceHandle);
}


/*!
 * Get the NvKmsPerOpenDev and NvKmsPerOpenDisp described by
 * NvKmsPerOpen + deviceHandle + dispHandle.
 */
static NvBool GetPerOpenDevAndDisp(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle,
    const NvKmsDispHandle dispHandle,
    struct NvKmsPerOpenDev **ppOpenDev,
    struct NvKmsPerOpenDisp **ppOpenDisp)
{
    struct NvKmsPerOpenDev *pOpenDev;
    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDev = GetPerOpenDev(pOpen, deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    pOpenDisp = nvEvoGetPointerFromApiHandle(&pOpenDev->dispHandles,
                                             dispHandle);

    if (pOpenDisp == NULL) {
        return FALSE;
    }

    *ppOpenDev = pOpenDev;
    *ppOpenDisp = pOpenDisp;

    return TRUE;
}


/*!
 * Get the NvKmsPerOpenDisp described by NvKmsPerOpen + deviceHandle +
 * dispHandle.
 */
static struct NvKmsPerOpenDisp *GetPerOpenDisp(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle,
    const NvKmsDispHandle dispHandle)
{
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, deviceHandle);

    if (pOpenDev == NULL) {
        return NULL;
    }

    return nvEvoGetPointerFromApiHandle(&pOpenDev->dispHandles, dispHandle);
}


/*!
 * Get the NvKmsPerOpenConnector described by NvKmsPerOpen +
 * deviceHandle + dispHandle + connectorHandle.
 */
static struct NvKmsPerOpenConnector *GetPerOpenConnector(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle,
    const NvKmsDispHandle dispHandle,
    const NvKmsConnectorHandle connectorHandle)
{
    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDisp = GetPerOpenDisp(pOpen, deviceHandle, dispHandle);

    if (pOpenDisp == NULL) {
        return NULL;
    }

    return nvEvoGetPointerFromApiHandle(&pOpenDisp->connectorHandles,
                                        connectorHandle);
}


/*!
 * Get the NVDpyEvoRec described by NvKmsPerOpen + deviceHandle +
 * dispHandle + dpyId.
 */
static NVDpyEvoRec *GetPerOpenDpy(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle,
    const NvKmsDispHandle dispHandle,
    const NVDpyId dpyId)
{
    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDisp = GetPerOpenDisp(pOpen, deviceHandle, dispHandle);

    if (pOpenDisp == NULL) {
        return NULL;
    }

    return nvGetDpyEvoFromDispEvo(pOpenDisp->pDispEvo, dpyId);
}


/*!
 * Get the NvKmsPerOpenFrameLock described by pOpen + frameLockHandle.
 */
static struct NvKmsPerOpenFrameLock *GetPerOpenFrameLock(
    const struct NvKmsPerOpen *pOpen,
    NvKmsFrameLockHandle frameLockHandle)
{
    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    return nvEvoGetPointerFromApiHandle(&pOpen->ioctl.frameLockHandles,
                                        frameLockHandle);
}


/*!
 * Free the NvKmsPerOpenFrameLock associated with this NvKmsPerOpenDisp.
 *
 * Multiple disps can be assigned to the same framelock object, so
 * NvKmsPerOpenFrameLock is reference counted: the object is freed
 * once all NvKmsPerOpenDisps remove their reference to it.
 *
 * \param[in,out]  pOpen      The per-open data, to which the
 *                            NvKmsPerOpenFrameLock is assigned.
 * \param[in,out]  pOpenDisp  The NvKmsPerOpenDisp whose corresponding
 *                            NvKmsPerOpenFrameLock should be freed.
 */
static void FreePerOpenFrameLock(struct NvKmsPerOpen *pOpen,
                                 struct NvKmsPerOpenDisp *pOpenDisp)
{
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    pOpenFrameLock =
        nvEvoGetPointerFromApiHandle(&pOpen->ioctl.frameLockHandles,
                                     pOpenDisp->frameLockHandle);
    if (pOpenFrameLock == NULL) {
        return;
    }

    pOpenDisp->frameLockHandle = 0;

    pOpenFrameLock->refCnt--;

    if (pOpenFrameLock->refCnt != 0) {
        return;
    }

    nvEvoDestroyApiHandle(&pOpen->ioctl.frameLockHandles,
                          pOpenFrameLock->nvKmsApiHandle);
    nvFree(pOpenFrameLock);
}


/*!
 * Allocate and initialize an NvKmsPerOpenFrameLock.
 *
 * If the disp described by the specified NvKmsPerOpenDisp has a
 * framelock object, allocate an NvKmsPerOpenFrameLock for it.
 *
 * Multiple disps can be assigned to the same framelock object, so
 * NvKmsPerOpenFrameLock is reference counted: we first look to see if
 * an NvKmsPerOpenFrameLock for this disp's framelock object already
 * exists.  If so, we increment its reference count.  Otherwise, we
 * allocate a new NvKmsPerOpenFrameLock.
 *
 * \param[in,out]  pOpen      The per-open data, to which the
 *                            new NvKmsPerOpenFrameLock should be assigned.
 * \param[in,out]  pOpenDisp  The NvKmsPerOpenDisp whose corresponding
 *                            NvKmsPerOpenFrameLock should be allocated.
 */
static NvBool AllocPerOpenFrameLock(
    struct NvKmsPerOpen *pOpen,
    struct NvKmsPerOpenDisp *pOpenDisp)
{
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    NVDispEvoPtr pDispEvo = pOpenDisp->pDispEvo;
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NvKmsGenericHandle handle;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (pFrameLockEvo == NULL) {
        return TRUE;
    }

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.frameLockHandles,
                                        pOpenFrameLock, handle) {
        if (pOpenFrameLock->pFrameLockEvo == pFrameLockEvo) {
            goto done;
        }
    }

    pOpenFrameLock = nvCalloc(1, sizeof(*pOpenFrameLock));

    if (pOpenFrameLock == NULL) {
        return FALSE;
    }

    pOpenFrameLock->pFrameLockEvo = pFrameLockEvo;
    pOpenFrameLock->nvKmsApiHandle =
        nvEvoCreateApiHandle(&pOpen->ioctl.frameLockHandles, pOpenFrameLock);

    if (pOpenFrameLock->nvKmsApiHandle == 0) {
        nvFree(pOpenFrameLock);
        return FALSE;
    }

done:
    pOpenDisp->frameLockHandle = pOpenFrameLock->nvKmsApiHandle;
    pOpenFrameLock->refCnt++;
    return TRUE;
}


/*!
 * Get the NvKmsConnectorHandle that corresponds to the given
 * NVConnectorEvoRec on the NvKmsPerOpen + deviceHandle + dispHandle.
 */
static NvKmsConnectorHandle ConnectorEvoToConnectorHandle(
    const struct NvKmsPerOpen *pOpen,
    const NvKmsDeviceHandle deviceHandle,
    const NvKmsDispHandle dispHandle,
    const NVConnectorEvoRec *pConnectorEvo)
{
    struct NvKmsPerOpenDisp *pOpenDisp;
    struct NvKmsPerOpenConnector *pOpenConnector;
    NvKmsGenericHandle connector;

    pOpenDisp = GetPerOpenDisp(pOpen, deviceHandle, dispHandle);

    if (pOpenDisp == NULL) {
        return 0;
    }

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDisp->connectorHandles,
                                        pOpenConnector, connector) {
        if (pOpenConnector->pConnectorEvo == pConnectorEvo) {
            return pOpenConnector->nvKmsApiHandle;
        }
    }

    return 0;
}


/*!
 * Get the NvKmsDeviceHandle and NvKmsDispHandle that corresponds to
 * the given NVDispEvoRec on the NvKmsPerOpen.
 */
static NvBool DispEvoToDevAndDispHandles(
    const struct NvKmsPerOpen *pOpen,
    const NVDispEvoRec *pDispEvo,
    NvKmsDeviceHandle *pDeviceHandle,
    NvKmsDispHandle *pDispHandle)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    struct NvKmsPerOpenDev *pOpenDev;
    NvKmsGenericHandle dev;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                        pOpenDev, dev) {

        struct NvKmsPerOpenDisp *pOpenDisp;
        NvKmsGenericHandle disp;

        if (pOpenDev->pDevEvo != pDevEvo) {
            continue;
        }

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->dispHandles,
                                            pOpenDisp, disp) {
            if (pOpenDisp->pDispEvo != pDispEvo) {
                continue;
            }

            *pDeviceHandle = pOpenDev->nvKmsApiHandle;
            *pDispHandle = pOpenDisp->nvKmsApiHandle;

            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * Get the NvKmsPerOpenDev that corresponds to the given NVDevEvoRec
 * on the NvKmsPerOpen.
 */
static struct NvKmsPerOpenDev *DevEvoToOpenDev(
    const struct NvKmsPerOpen *pOpen,
    const NVDevEvoRec *pDevEvo)
{
    struct NvKmsPerOpenDev *pOpenDev;
    NvKmsGenericHandle dev;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                        pOpenDev, dev) {
        if (pOpenDev->pDevEvo == pDevEvo) {
            return pOpenDev;
        }
    }

    return NULL;
}


/*!
 * Get the NvKmsFrameLockHandle that corresponds to the given
 * NVFrameLockEvoRec on the NvKmsPerOpen.
 */
static NvBool FrameLockEvoToFrameLockHandle(
    const struct NvKmsPerOpen *pOpen,
    const NVFrameLockEvoRec *pFrameLockEvo,
    NvKmsFrameLockHandle *pFrameLockHandle)
{
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    NvKmsGenericHandle handle;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.frameLockHandles,
                                        pOpenFrameLock, handle) {

        if (pOpenFrameLock->pFrameLockEvo == pFrameLockEvo) {
            *pFrameLockHandle = pOpenFrameLock->nvKmsApiHandle;
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * Clear the specified NvKmsPerOpenConnector.
 *
 * \param[in,out]  pOpenDisp       The NvKmsPerOpenDisp to which the
 *                                 NvKmsPerOpenConnector is assigned.
 * \param[in,out]  pOpenConnector  The NvKmsPerOpenConnector to be cleared.
 */
static void ClearPerOpenConnector(
    struct NvKmsPerOpenDisp *pOpenDisp,
    struct NvKmsPerOpenConnector *pOpenConnector)
{
    nvEvoDestroyApiHandle(&pOpenDisp->connectorHandles,
                          pOpenConnector->nvKmsApiHandle);
    nvkms_memset(pOpenConnector, 0, sizeof(*pOpenConnector));
}


/*!
 * Initialize an NvKmsPerOpenConnector.
 *
 * \param[in,out]  pOpenDisp       The NvKmsPerOpenDisp to which the
 *                                 NvKmsPerOpenConnector is assigned.
 * \param[in,out]  pOpenConnector  The NvKmsPerOpenConnector to initialize.
 * \param[in]      pConnectorEvo   The connector that the NvKmsPerOpenConnector
 *                                 corresponds to.
 *
 * \return  If the NvKmsPerOpenConnector is successfully initialized,
 *          return TRUE.  Otherwise, return FALSE.
 */
static NvBool InitPerOpenConnector(
    struct NvKmsPerOpenDisp *pOpenDisp,
    struct NvKmsPerOpenConnector *pOpenConnector,
    NVConnectorEvoPtr pConnectorEvo)
{
    pOpenConnector->nvKmsApiHandle =
        nvEvoCreateApiHandle(&pOpenDisp->connectorHandles, pOpenConnector);

    if (pOpenConnector->nvKmsApiHandle == 0) {
        goto fail;
    }

    pOpenConnector->pConnectorEvo = pConnectorEvo;

    return TRUE;

fail:
    ClearPerOpenConnector(pOpenDisp, pOpenConnector);
    return FALSE;
}

/*!
 * Clear the specified NvKmsPerOpenDisp.
 *
 * \param[in,out]  pOpenDev   The NvKmsPerOpenDev to which the NvKmsPerOpenDisp
 *                            is assigned.
 * \param[in,out]  pDispEvo   The NvKmsPerOpenDisp to be cleared.
 */
static void ClearPerOpenDisp(
    struct NvKmsPerOpen *pOpen,
    struct NvKmsPerOpenDev *pOpenDev,
    struct NvKmsPerOpenDisp *pOpenDisp)
{
    struct NvKmsPerOpenConnector *pOpenConnector;
    NvKmsGenericHandle connector;

    FreePerOpenFrameLock(pOpen, pOpenDisp);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDisp->connectorHandles,
                                        pOpenConnector, connector) {
        ClearPerOpenConnector(pOpenDisp, pOpenConnector);
    }

    /* Destroy the API handle structures. */
    nvEvoDestroyApiHandles(&pOpenDisp->connectorHandles);

    for (NvU32 i = 0; i < NVKMS_MAX_HEADS_PER_DISP; i++) {
        nvEvoDestroyApiHandles(&pOpenDisp->vblankSyncObjectHandles[i]);
    }

    nvEvoDestroyApiHandle(&pOpenDev->dispHandles, pOpenDisp->nvKmsApiHandle);

    nvkms_memset(pOpenDisp, 0, sizeof(*pOpenDisp));
}


/*!
 * Initialize an NvKmsPerOpenDisp.
 *
 * \param[in,out]  pOpenDev   The NvKmsPerOpenDev to which the NvKmsPerOpenDisp
 *                            is assigned.
 * \param[in,out]  pOpenDisp  The NvKmsPerOpenDisp to initialize.
 * \param[in]      pDispEvo   The disp that the NvKmsPerOpenDisp corresponds to.
 *
 * \return  If the NvKmsPerOpenDisp is successfully initialized, return TRUE.
 *          Otherwise, return FALSE.
 */
static NvBool InitPerOpenDisp(
    struct NvKmsPerOpen *pOpen,
    struct NvKmsPerOpenDev *pOpenDev,
    struct NvKmsPerOpenDisp *pOpenDisp,
    NVDispEvoPtr pDispEvo)
{
    NVConnectorEvoPtr pConnectorEvo;
    NvU32 connector;

    pOpenDisp->nvKmsApiHandle =
        nvEvoCreateApiHandle(&pOpenDev->dispHandles, pOpenDisp);

    if (pOpenDisp->nvKmsApiHandle == 0) {
        goto fail;
    }

    pOpenDisp->pDispEvo = pDispEvo;

    if (nvListCount(&pDispEvo->connectorList) >=
        ARRAY_LEN(pOpenDisp->connector)) {
        nvAssert(!"More connectors on this disp than NVKMS can handle.");
        goto fail;
    }

    if (!nvEvoInitApiHandles(&pOpenDisp->connectorHandles,
                             ARRAY_LEN(pOpenDisp->connector))) {
        goto fail;
    }

    connector = 0;
    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (!InitPerOpenConnector(pOpenDisp, &pOpenDisp->connector[connector],
                                  pConnectorEvo)) {
            goto fail;
        }
        connector++;
    }

    /* Initialize the vblankSyncObjectHandles for each head. */
    for (NvU32 i = 0; i < NVKMS_MAX_HEADS_PER_DISP; i++) {
        if (!nvEvoInitApiHandles(&pOpenDisp->vblankSyncObjectHandles[i],
                                 NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD)) {
            goto fail;
        }
    }

    if (!AllocPerOpenFrameLock(pOpen, pOpenDisp)) {
        goto fail;
    }

    return TRUE;

fail:
    ClearPerOpenDisp(pOpen, pOpenDev, pOpenDisp);
    return FALSE;
}

/*!
 * Check that the NvKmsPermissions make sense.
 */
static NvBool ValidateNvKmsPermissions(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsPermissions *pPermissions)
{
    if (pPermissions->type == NV_KMS_PERMISSIONS_TYPE_FLIPPING) {
        NvU32 d, h;

        for (d = 0; d < ARRAY_LEN(pPermissions->flip.disp); d++) {
            for (h = 0; h < ARRAY_LEN(pPermissions->flip.disp[d].head); h++) {

                NvU8 layerMask = pPermissions->flip.disp[d].head[h].layerMask;

                if (layerMask == 0) {
                    continue;
                }

                if (nvHasBitAboveMax(layerMask, pDevEvo->head[h].numLayers)) {
                    return FALSE;
                }

                /*
                 * If the above blocks didn't 'continue', then there
                 * are permissions specified for this disp+head.  Is
                 * the specified disp+head in range for the current
                 * configuration?
                 */
                if (d >= pDevEvo->nDispEvo) {
                    return FALSE;
                }

                if (h >= pDevEvo->numHeads) {
                    return FALSE;
                }
            }
        }
    } else if (pPermissions->type == NV_KMS_PERMISSIONS_TYPE_MODESET) {
        NvU32 d, h;

        for (d = 0; d < ARRAY_LEN(pPermissions->flip.disp); d++) {
            for (h = 0; h < ARRAY_LEN(pPermissions->flip.disp[d].head); h++) {

                NVDpyIdList dpyIdList =
                    pPermissions->modeset.disp[d].head[h].dpyIdList;

                if (nvDpyIdListIsEmpty(dpyIdList)) {
                    continue;
                }

                /*
                 * If the above blocks didn't 'continue', then there
                 * are permissions specified for this disp+head.  Is
                 * the specified disp+head in range for the current
                 * configuration?
                 */
                if (d >= pDevEvo->nDispEvo) {
                    return FALSE;
                }

                if (h >= pDevEvo->numHeads) {
                    return FALSE;
                }
            }
        }
    } else {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Assign pPermissions with the maximum permissions possible for
 * the pDevEvo.
 */
static void AssignFullNvKmsFlipPermissions(
    const NVDevEvoRec *pDevEvo,
    struct NvKmsFlipPermissions *pPermissions)
{
    NvU32 dispIndex, head;

    nvkms_memset(pPermissions, 0, sizeof(*pPermissions));

    for (dispIndex = 0; dispIndex < pDevEvo->nDispEvo; dispIndex++) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            pPermissions->disp[dispIndex].head[head].layerMask =
                NVBIT(pDevEvo->head[head].numLayers) - 1;
        }
    }
}

static void AssignFullNvKmsModesetPermissions(
    const NVDevEvoRec *pDevEvo,
    struct NvKmsModesetPermissions *pPermissions)
{
    NvU32 dispIndex, head;

    nvkms_memset(pPermissions, 0, sizeof(*pPermissions));

    for (dispIndex = 0; dispIndex < pDevEvo->nDispEvo; dispIndex++) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            pPermissions->disp[dispIndex].head[head].dpyIdList =
                nvAllDpyIdList();
        }
    }
}

/*!
 * Set the modeset owner to pOpenDev
 *
 * \param   pOpenDev    The per-open device structure for the new modeset owner.
 * \return  FALSE if there was already a modeset owner. TRUE otherwise.
 */
static NvBool GrabModesetOwnership(struct NvKmsPerOpenDev *pOpenDev)
{
    NVDevEvoPtr pDevEvo = pOpenDev->pDevEvo;

    if (pDevEvo->modesetOwner == pOpenDev) {
        return TRUE;
    }

    if (pDevEvo->modesetOwner != NULL) {
        return FALSE;
    }

    /*
     * If claiming modeset ownership, undo any SST forcing imposed by
     * console restore.
     */
    if (pOpenDev != pDevEvo->pNvKmsOpenDev) {
        nvDPSetAllowMultiStreaming(pDevEvo, TRUE /* allowMST */);
    }

    pDevEvo->modesetOwner = pOpenDev;

    AssignFullNvKmsFlipPermissions(pDevEvo, &pOpenDev->flipPermissions);
    AssignFullNvKmsModesetPermissions(pDevEvo, &pOpenDev->modesetPermissions);

    pDevEvo->modesetOwnerChanged = TRUE;

    return TRUE;
}


/*!
 * Clear permissions on the specified device for all NvKmsPerOpens.
 *
 * For NvKmsPerOpen::type==Ioctl, clear the permissions, except for the
 * specified pOpenDevExclude.
 *
 * For NvKmsPerOpen::type==GrantPermissions, clear
 * NvKmsPerOpen::grantPermissions and reset NvKmsPerOpen::type to
 * Undefined.
 */
static void RevokePermissionsInternal(
    const NvU32 typeBitmask,
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsPerOpenDev *pOpenDevExclude)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenList, perOpenListEntry) {

        if ((pOpen->type == NvKmsPerOpenTypeGrantPermissions) &&
            (pOpen->grantPermissions.pDevEvo == pDevEvo) &&
            (typeBitmask & NVBIT(pOpen->grantPermissions.permissions.type))) {
            nvkms_memset(&pOpen->grantPermissions, 0,
                         sizeof(pOpen->grantPermissions));
            pOpen->type = NvKmsPerOpenTypeUndefined;
        }

        if (pOpen->type == NvKmsPerOpenTypeIoctl) {

            struct NvKmsPerOpenDev *pOpenDev =
                DevEvoToOpenDev(pOpen, pDevEvo);

            if (pOpenDev == NULL) {
                continue;
            }

            if (pOpenDev == pOpenDevExclude || pOpenDev->isPrivileged) {
                continue;
            }

            if (typeBitmask & NVBIT(NV_KMS_PERMISSIONS_TYPE_FLIPPING)) {
                nvkms_memset(&pOpenDev->flipPermissions, 0,
                             sizeof(pOpenDev->flipPermissions));
            }

            if (typeBitmask & NVBIT(NV_KMS_PERMISSIONS_TYPE_MODESET)) {
                nvkms_memset(&pOpenDev->modesetPermissions, 0,
                             sizeof(pOpenDev->modesetPermissions));
            }
        }
    }
}

static void ReallocCoreChannel(NVDevEvoRec *pDevEvo)
{
    if (nvAllocCoreChannelEvo(pDevEvo)) {
        nvDPSetAllowMultiStreaming(pDevEvo, TRUE /* allowMST */);
        AllocSurfaceCtxDmasForAllOpens(pDevEvo);
    }
}

static void RestoreConsole(NVDevEvoPtr pDevEvo)
{
    pDevEvo->modesetOwnerChanged = TRUE;

    // Try to issue a modeset and flip to the framebuffer console surface.
    if (!nvEvoRestoreConsole(pDevEvo, TRUE /* allowMST */)) {
        // If that didn't work, free the core channel to trigger RM's console
        // restore code.
        FreeSurfaceCtxDmasForAllOpens(pDevEvo);
        nvFreeCoreChannelEvo(pDevEvo);

        // Reallocate the core channel right after freeing it. This makes sure
        // that it's allocated and ready right away if another NVKMS client is
        // started.
        ReallocCoreChannel(pDevEvo);
    }
}

/*!
 * Release modeset ownership previously set by GrabModesetOwnership
 *
 * \param   pOpenDev    The per-open device structure relinquishing modeset
 *                      ownership.
 * \return  FALSE if pOpenDev is not the modeset owner, TRUE otherwise.
 */
static NvBool ReleaseModesetOwnership(struct NvKmsPerOpenDev *pOpenDev)
{
    NVDevEvoPtr pDevEvo = pOpenDev->pDevEvo;

    if (pDevEvo->modesetOwner != pOpenDev) {
        // Only the current owner can release ownership.
        return FALSE;
    }

    pDevEvo->modesetOwner = NULL;
    pDevEvo->handleConsoleHotplugs = TRUE;

    RestoreConsole(pDevEvo);
    RevokePermissionsInternal(NVBIT(NV_KMS_PERMISSIONS_TYPE_FLIPPING) |
                              NVBIT(NV_KMS_PERMISSIONS_TYPE_MODESET),
                              pDevEvo, NULL /* pOpenDevExclude */);
    return TRUE;
}

/*!
 * Free the specified NvKmsPerOpenDev.
 *
 * \param[in,out]  pOpen     The per-open data, to which the
 *                           NvKmsPerOpenDev is assigned.
 * \param[in,out]  pOpenDev  The NvKmsPerOpenDev to free.
 */
void nvFreePerOpenDev(struct NvKmsPerOpen *pOpen,
                      struct NvKmsPerOpenDev *pOpenDev)
{
    struct NvKmsPerOpenDisp *pOpenDisp;
    NvKmsGenericHandle disp;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (pOpenDev == NULL) {
        return;
    }

    nvEvoDestroyApiHandles(&pOpenDev->surfaceHandles);

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->dispHandles,
                                        pOpenDisp, disp) {
        ClearPerOpenDisp(pOpen, pOpenDev, pOpenDisp);
    }

    nvEvoDestroyApiHandles(&pOpenDev->dispHandles);

    nvEvoDestroyApiHandle(&pOpen->ioctl.devHandles, pOpenDev->nvKmsApiHandle);

    nvEvoDestroyApiHandles(&pOpenDev->deferredRequestFifoHandles);

    nvFree(pOpenDev);
}


/*!
 * Allocate and initialize an NvKmsPerOpenDev.
 *
 * \param[in,out]  pOpen        The per-open data, to which the
 *                              new NvKmsPerOpenDev should be assigned.
 * \param[in]      pDevEvo      The device to which the new NvKmsPerOpenDev
 *                              corresponds.
 * \param[in]      isPrivileged The NvKmsPerOpenDev is privileged which can
 *                              do modeset anytime.
 *
 * \return  On success, return a pointer to the new NvKmsPerOpenDev.
 *          On failure, return NULL.
 */
struct NvKmsPerOpenDev *nvAllocPerOpenDev(struct NvKmsPerOpen *pOpen,
                                          NVDevEvoPtr pDevEvo, NvBool isPrivileged)
{
    struct NvKmsPerOpenDev *pOpenDev = nvCalloc(1, sizeof(*pOpenDev));
    NVDispEvoPtr pDispEvo;
    NvU32 disp;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (pOpenDev == NULL) {
        goto fail;
    }

    pOpenDev->nvKmsApiHandle =
        nvEvoCreateApiHandle(&pOpen->ioctl.devHandles, pOpenDev);

    if (pOpenDev->nvKmsApiHandle == 0) {
        goto fail;
    }

    pOpenDev->pDevEvo = pDevEvo;

    if (!nvEvoInitApiHandles(&pOpenDev->dispHandles,
                             ARRAY_LEN(pOpenDev->disp))) {
        goto fail;
    }

    if (pDevEvo->nDispEvo > ARRAY_LEN(pOpenDev->disp)) {
        nvAssert(!"More disps on this device than NVKMS can handle.");
        goto fail;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, disp, pDevEvo) {
        if (!InitPerOpenDisp(pOpen, pOpenDev, &pOpenDev->disp[disp], pDispEvo)) {
            goto fail;
        }
    }

    if (!nvEvoInitApiHandles(&pOpenDev->surfaceHandles, 32)) {
        goto fail;
    }

    pOpenDev->isPrivileged = isPrivileged;
    if (pOpenDev->isPrivileged) {
        AssignFullNvKmsFlipPermissions(pDevEvo,
                                       &pOpenDev->flipPermissions);
        AssignFullNvKmsModesetPermissions(pOpenDev->pDevEvo,
                                          &pOpenDev->modesetPermissions);
    }

    if (!nvEvoInitApiHandles(&pOpenDev->deferredRequestFifoHandles, 4)) {
        goto fail;
    }

    return pOpenDev;

fail:
    nvFreePerOpenDev(pOpen, pOpenDev);
    return NULL;
}


/*!
 * Assign NvKmsPerOpen::type.
 *
 * This succeeds only if NvKmsPerOpen::type is Undefined, or already
 * has the requested type and allowRedundantAssignment is TRUE.
 */
static NvBool AssignNvKmsPerOpenType(struct NvKmsPerOpen *pOpen,
                                     enum NvKmsPerOpenType type,
                                     NvBool allowRedundantAssignment)
{
    if ((pOpen->type == type) && allowRedundantAssignment) {
        return TRUE;
    }

    if (pOpen->type != NvKmsPerOpenTypeUndefined) {
        return FALSE;
    }

    switch (type) {
    case NvKmsPerOpenTypeIoctl:
        nvListInit(&pOpen->ioctl.eventList);

        if (!nvEvoInitApiHandles(&pOpen->ioctl.devHandles, NV_MAX_DEVICES)) {
            return FALSE;
        }

        if (!nvEvoInitApiHandles(&pOpen->ioctl.frameLockHandles, 4)) {
            nvEvoDestroyApiHandles(&pOpen->ioctl.devHandles);
            return FALSE;
        }

        nvListAppend(&pOpen->perOpenIoctlListEntry, &perOpenIoctlList);
        break;

    case NvKmsPerOpenTypeGrantSurface:
        /* Nothing to do, here. */
        break;

    case NvKmsPerOpenTypeGrantSwapGroup:
        /* Nothing to do, here. */
        break;

    case NvKmsPerOpenTypeGrantPermissions:
        /* Nothing to do, here. */
        break;

    case NvKmsPerOpenTypeUnicastEvent:
        /* Nothing to do, here. */
        break;

    case NvKmsPerOpenTypeUndefined:
        nvAssert(!"unexpected NvKmsPerOpenType");
        break;
    }

    pOpen->type = type;
    return TRUE;
}

/*!
 * Allocate the specified device.
 */
static NvBool AllocDevice(struct NvKmsPerOpen *pOpen,
                          void *pParamsVoid)
{
    struct NvKmsAllocDeviceParams *pParams = pParamsVoid;
    NVDevEvoPtr pDevEvo;
    struct NvKmsPerOpenDev *pOpenDev;
    NvU32 disp, head;
    NvU8 layer;

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    if (nvkms_strcmp(pParams->request.versionString, NV_VERSION_STRING) != 0) {
        pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_VERSION_MISMATCH;
        return FALSE;
    }

    /*
     * It is an error to call NVKMS_IOCTL_ALLOC_DEVICE multiple times
     * on the same device with the same fd.
     */
    if (DeviceIdAlreadyPresent(pOpen, pParams->request.deviceId)) {
        pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_BAD_REQUEST;
        return FALSE;
    }

    pDevEvo = nvFindDevEvoByDeviceId(pParams->request.deviceId);

    if (pDevEvo == NULL) {
        pDevEvo = nvAllocDevEvo(&pParams->request, &pParams->reply.status);
        if (pDevEvo == NULL) {
            return FALSE;
        }
    } else {
        if (!pParams->request.tryInferSliMosaicFromExistingDevice &&
            (pDevEvo->sli.mosaic != pParams->request.sliMosaic)) {
            pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_BAD_REQUEST;
            return FALSE;
        }

        if (pDevEvo->usesTegraDevice &&
            (pParams->request.deviceId != NVKMS_DEVICE_ID_TEGRA)) {
            pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_BAD_REQUEST;
            return FALSE;
        }
        pDevEvo->allocRefCnt++;
    }

    pOpenDev = nvAllocPerOpenDev(pOpen, pDevEvo, FALSE /* isPrivileged */);

    if (pOpenDev == NULL) {
        nvFreeDevEvo(pDevEvo);
        pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        return FALSE;
    }

    /* Beyond this point, the function cannot fail. */

    if (pParams->request.enableConsoleHotplugHandling) {
        pDevEvo->handleConsoleHotplugs = TRUE;
    }

    pParams->reply.deviceHandle = pOpenDev->nvKmsApiHandle;
    pParams->reply.subDeviceMask =
        NV_TWO_N_MINUS_ONE(pDevEvo->numSubDevices);
    pParams->reply.numHeads = pDevEvo->numHeads;
    pParams->reply.numDisps = pDevEvo->nDispEvo;

    ct_assert(ARRAY_LEN(pParams->reply.dispHandles) ==
              ARRAY_LEN(pOpenDev->disp));

    for (disp = 0; disp < ARRAY_LEN(pParams->reply.dispHandles); disp++) {
        pParams->reply.dispHandles[disp] = pOpenDev->disp[disp].nvKmsApiHandle;
    }

    pParams->reply.inputLutAppliesToBase = pDevEvo->caps.inputLutAppliesToBase;

    ct_assert(ARRAY_LEN(pParams->reply.layerCaps) ==
              ARRAY_LEN(pDevEvo->caps.layerCaps));

    for (head = 0; head < pDevEvo->numHeads; head++) {
        pParams->reply.numLayers[head] = pDevEvo->head[head].numLayers;
    }

    for (layer = 0;
         layer < ARRAY_LEN(pParams->reply.layerCaps);
         layer++) {
        pParams->reply.layerCaps[layer] = pDevEvo->caps.layerCaps[layer];
    }

    pParams->reply.surfaceAlignment  = NV_EVO_SURFACE_ALIGNMENT;
    pParams->reply.requiresVrrSemaphores = !pDevEvo->hal->caps.supportsDisplayRate;

    pParams->reply.nIsoSurfacesInVidmemOnly =
        !!NV5070_CTRL_SYSTEM_GET_CAP(pDevEvo->capsBits,
            NV5070_CTRL_SYSTEM_CAPS_BUG_644815_DNISO_VIDMEM_ONLY);

    pParams->reply.requiresAllAllocationsInSysmem =
        pDevEvo->requiresAllAllocationsInSysmem;
    pParams->reply.supportsHeadSurface = pDevEvo->isHeadSurfaceSupported;

    pParams->reply.validNIsoFormatMask = pDevEvo->caps.validNIsoFormatMask;

    pParams->reply.maxWidthInBytes   = pDevEvo->caps.maxWidthInBytes;
    pParams->reply.maxWidthInPixels  = pDevEvo->caps.maxWidthInPixels;
    pParams->reply.maxHeightInPixels = pDevEvo->caps.maxHeight;
    pParams->reply.cursorCompositionCaps = pDevEvo->caps.cursorCompositionCaps;
    pParams->reply.genericPageKind   = pDevEvo->caps.genericPageKind;

    pParams->reply.maxCursorSize     = pDevEvo->cursorHal->caps.maxSize;

    pParams->reply.validLayerRRTransforms = pDevEvo->caps.validLayerRRTransforms;

    pParams->reply.isoIOCoherencyModes = pDevEvo->isoIOCoherencyModes;
    pParams->reply.nisoIOCoherencyModes = pDevEvo->nisoIOCoherencyModes;

    pParams->reply.supportsSyncpts = pDevEvo->supportsSyncpts;

    pParams->reply.supportsIndependentAcqRelSemaphore =
        pDevEvo->hal->caps.supportsIndependentAcqRelSemaphore;

    pParams->reply.supportsVblankSyncObjects =
        pDevEvo->hal->caps.supportsVblankSyncObjects;

    pParams->reply.status = NVKMS_ALLOC_DEVICE_STATUS_SUCCESS;

    return TRUE;
}

static void UnregisterDeferredRequestFifos(struct NvKmsPerOpenDev *pOpenDev)
{
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    NvKmsGenericHandle handle;

    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->deferredRequestFifoHandles,
                                        pDeferredRequestFifo,
                                        handle) {

        nvEvoDestroyApiHandle(&pOpenDev->deferredRequestFifoHandles, handle);

        nvEvoUnregisterDeferredRequestFifo(pOpenDev->pDevEvo,
                                           pDeferredRequestFifo);
    }
}

/*
 * Forward declaration since this function is used by
 * DisableRemainingVblankSyncObjects().
 */
static void DisableAndCleanVblankSyncObject(struct NvKmsPerOpenDisp *pOpenDisp,
                                            NvU32 head,
                                            NVVblankSyncObjectRec *pVblankSyncObject,
                                            NVEvoUpdateState *pUpdateState,
                                            NvKmsVblankSyncObjectHandle handle);

static void DisableRemainingVblankSyncObjects(struct NvKmsPerOpen *pOpen,
                                              struct NvKmsPerOpenDev *pOpenDev)
{
    struct NvKmsPerOpenDisp *pOpenDisp;
    NvKmsGenericHandle disp;
    NVVblankSyncObjectRec *pVblankSyncObject;
    NvKmsVblankSyncObjectHandle handle;
    NvU32 head = 0;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (pOpenDev == NULL) {
        return;
    }

    /* For each pOpenDisp: */
    FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->dispHandles,
                                        pOpenDisp, disp) {
        /*
         * A single update state can handle changes across multiple heads on a
         * given Disp.
         */
        NVEvoUpdateState updateState = { };

        /* For each head: */
        for (head = 0; head < ARRAY_LEN(pOpenDisp->vblankSyncObjectHandles); head++) {
            NVEvoApiHandlesRec *pHandles =
                &pOpenDisp->vblankSyncObjectHandles[head];

            /* For each still-active vblank sync object: */
            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(pHandles,
                                                pVblankSyncObject, handle) {
                DisableAndCleanVblankSyncObject(pOpenDisp, head,
                                                pVblankSyncObject,
                                                &updateState,
                                                handle);
            }
        }

        if (!nvIsUpdateStateEmpty(pOpenDisp->pDispEvo->pDevEvo, &updateState)) {
            /*
             * Instruct hardware to execute the staged commands from the
             * ConfigureVblankSyncObject() calls (inherent in
             * DisableAndCleanVblankSyncObject()) above. This will set up
             * and wait for a notification that the hardware execution
             * has completed.
             */
            nvEvoUpdateAndKickOff(pOpenDisp->pDispEvo, TRUE, &updateState,
                                  TRUE);
        }
    }
}

static void FreeDeviceReference(struct NvKmsPerOpen *pOpen,
                                struct NvKmsPerOpenDev *pOpenDev)
{
    /* Disable all client-owned vblank sync objects that still exist. */
    DisableRemainingVblankSyncObjects(pOpen, pOpenDev);

    UnregisterDeferredRequestFifos(pOpenDev);

    nvEvoFreeClientSurfaces(pOpenDev->pDevEvo, pOpenDev,
                            &pOpenDev->surfaceHandles);

    if (!nvFreeDevEvo(pOpenDev->pDevEvo)) {
        // If this pOpenDev is the modeset owner, implicitly release it.  Does
        // nothing if this pOpenDev is not the modeset owner.
        //
        // If nvFreeDevEvo() freed the device, then it also implicitly released
        // ownership.
        ReleaseModesetOwnership(pOpenDev);

        nvAssert(pOpenDev->pDevEvo->modesetOwner != pOpenDev);
    }

    nvFreePerOpenDev(pOpen, pOpenDev);
}

/*!
 * Free the specified device.
 */
static NvBool FreeDevice(struct NvKmsPerOpen *pOpen,
                         void *pParamsVoid)
{
    struct NvKmsFreeDeviceParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    FreeDeviceReference(pOpen, pOpenDev);

    return TRUE;
}


/*!
 * Get the disp data.  This information should remain static for the
 * lifetime of the disp.
 */
static NvBool QueryDisp(struct NvKmsPerOpen *pOpen,
                        void *pParamsVoid)
{
    struct NvKmsQueryDispParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp *pOpenDisp;
    const NVEvoSubDeviceRec *pSubDevice;
    NVDispEvoPtr pDispEvo;
    NvU32 connector;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pDispEvo = pOpenDisp->pDispEvo;

    pParams->reply.displayOwner    = pDispEvo->displayOwner;
    pParams->reply.subDeviceMask   = nvDispSubDevMaskEvo(pDispEvo);
    // Don't include dynamic displays in validDpys.  The data returned here is
    // supposed to be static for the lifetime of the pDispEvo.
    pParams->reply.validDpys       =
        nvDpyIdListMinusDpyIdList(pDispEvo->validDisplays,
                                  pDispEvo->dynamicDpyIds);
    pParams->reply.bootDpys        = pDispEvo->bootDisplays;
    pParams->reply.muxDpys         = pDispEvo->muxDisplays;
    pParams->reply.frameLockHandle = pOpenDisp->frameLockHandle;
    pParams->reply.numConnectors   = nvListCount(&pDispEvo->connectorList);

    ct_assert(ARRAY_LEN(pParams->reply.connectorHandles) ==
              ARRAY_LEN(pOpenDisp->connector));

    for (connector = 0; connector < ARRAY_LEN(pParams->reply.connectorHandles);
         connector++) {
        pParams->reply.connectorHandles[connector] =
            pOpenDisp->connector[connector].nvKmsApiHandle;
    }

    pSubDevice = pDispEvo->pDevEvo->pSubDevices[pDispEvo->displayOwner];
    if (pSubDevice != NULL) {
        ct_assert(sizeof(pParams->reply.gpuString) >=
                  sizeof(pSubDevice->gpuString));
        nvkms_memcpy(pParams->reply.gpuString, pSubDevice->gpuString,
                     sizeof(pSubDevice->gpuString));
    }

    return TRUE;
}


/*!
 * Get the connector static data.  This information should remain static for the
 * lifetime of the connector.
 */
static NvBool QueryConnectorStaticData(struct NvKmsPerOpen *pOpen,
                                       void *pParamsVoid)
{
    struct NvKmsQueryConnectorStaticDataParams *pParams = pParamsVoid;
    struct NvKmsPerOpenConnector *pOpenConnector;
    NVConnectorEvoPtr pConnectorEvo;

    pOpenConnector = GetPerOpenConnector(pOpen,
                                         pParams->request.deviceHandle,
                                         pParams->request.dispHandle,
                                         pParams->request.connectorHandle);
    if (pOpenConnector == NULL) {
        return FALSE;
    }

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pConnectorEvo = pOpenConnector->pConnectorEvo;

    pParams->reply.dpyId            = pConnectorEvo->displayId;
    pParams->reply.isDP             = nvConnectorUsesDPLib(pConnectorEvo) ||
                                      nvConnectorIsDPSerializer(pConnectorEvo);
    pParams->reply.legacyTypeIndex  = pConnectorEvo->legacyTypeIndex;
    pParams->reply.type             = pConnectorEvo->type;
    pParams->reply.typeIndex        = pConnectorEvo->typeIndex;
    pParams->reply.signalFormat     = pConnectorEvo->signalFormat;
    pParams->reply.physicalIndex    = pConnectorEvo->physicalIndex;
    pParams->reply.physicalLocation = pConnectorEvo->physicalLocation;
    pParams->reply.headMask         = pConnectorEvo->validHeadMask;

    pParams->reply.isLvds =
        (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) &&
        (pConnectorEvo->or.protocol ==
         NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM);

    pParams->reply.locationOnChip = (pConnectorEvo->or.location ==
                                     NV0073_CTRL_SPECIFIC_OR_LOCATION_CHIP);
    return TRUE;
}


/*!
 * Get the connector dynamic data.  This information should reflects changes to
 * the connector over time (e.g. for DisplayPort MST devices).
 */
static NvBool QueryConnectorDynamicData(struct NvKmsPerOpen *pOpen,
                                        void *pParamsVoid)
{
    struct NvKmsQueryConnectorDynamicDataParams *pParams = pParamsVoid;
    struct NvKmsPerOpenConnector *pOpenConnector;
    NVConnectorEvoPtr pConnectorEvo;
    NVDispEvoPtr pDispEvo;
    NVDpyEvoPtr pDpyEvo;

    pOpenConnector = GetPerOpenConnector(pOpen,
                                         pParams->request.deviceHandle,
                                         pParams->request.dispHandle,
                                         pParams->request.connectorHandle);
    if (pOpenConnector == NULL) {
        return FALSE;
    }

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pConnectorEvo = pOpenConnector->pConnectorEvo;
    pDispEvo = pConnectorEvo->pDispEvo;

    if (nvConnectorUsesDPLib(pConnectorEvo)) {
        pParams->reply.detectComplete = pConnectorEvo->detectComplete;
    } else {
        pParams->reply.detectComplete = TRUE;
    }

    // Find the dynamic dpys on this connector.
    pParams->reply.dynamicDpyIdList = nvEmptyDpyIdList();
    FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->dynamicDpyIds, pDispEvo) {
        if (pDpyEvo->pConnectorEvo == pConnectorEvo) {
            pParams->reply.dynamicDpyIdList =
                nvAddDpyIdToDpyIdList(pDpyEvo->id,
                                      pParams->reply.dynamicDpyIdList);
        }
    }

    return TRUE;
}


/*!
 * Get the static data for the specified dpy.  This information should
 * remain static for the lifetime of the dpy.
 */
static NvBool QueryDpyStaticData(struct NvKmsPerOpen *pOpen,
                                 void *pParamsVoid)
{
    struct NvKmsQueryDpyStaticDataParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pParams->reply.connectorHandle =
        ConnectorEvoToConnectorHandle(pOpen,
                                      pParams->request.deviceHandle,
                                      pParams->request.dispHandle,
                                      pDpyEvo->pConnectorEvo);
    /*
     * All pConnectorEvos should have corresponding pOpenConnectors,
     * so we should always be able to find the NvKmsConnectorHandle.
     */
    nvAssert(pParams->reply.connectorHandle != 0);

    pParams->reply.type = pDpyEvo->pConnectorEvo->legacyType;

    if (pDpyEvo->dp.addressString != NULL) {
        const size_t len = nvkms_strlen(pDpyEvo->dp.addressString) + 1;
        nvkms_memcpy(pParams->reply.dpAddress, pDpyEvo->dp.addressString,
                     NV_MIN(sizeof(pParams->reply.dpAddress), len));
        pParams->reply.dpAddress[sizeof(pParams->reply.dpAddress) - 1] = '\0';
    }

    pParams->reply.mobileInternal = pDpyEvo->internal;
    pParams->reply.isDpMST = nvDpyEvoIsDPMST(pDpyEvo);

    return TRUE;
}


/*!
 * Get the dynamic data for the specified dpy.  This information can
 * change when a hotplug occurs.
 */
static NvBool QueryDpyDynamicData(struct NvKmsPerOpen *pOpen,
                                  void *pParamsVoid)
{
    struct NvKmsQueryDpyDynamicDataParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    return nvDpyGetDynamicData(pDpyEvo, pParams);
}

/* Store a copy of the user's infoString pointer, so we can copy out to it when
 * we're done. */
struct InfoStringExtraUserStateCommon
{
    NvU64 userInfoString;
};

/*
 * Allocate a kernel buffer to populate the infoString which will be copied out
 * to userspace upon completion.
 */
static NvBool InfoStringPrepUserCommon(
    NvU32 infoStringSize,
    NvU64 *ppInfoString,
    struct InfoStringExtraUserStateCommon *pExtra)
{
    char *kernelInfoString = NULL;

    if (infoStringSize == 0) {
        *ppInfoString = 0;
        return TRUE;
    }

    if (!nvKmsNvU64AddressIsSafe(*ppInfoString)) {
        return FALSE;
    }

    if (infoStringSize > NVKMS_MODE_VALIDATION_MAX_INFO_STRING_LENGTH) {
        return FALSE;
    }

    kernelInfoString = nvCalloc(1, infoStringSize);
    if (kernelInfoString == NULL) {
        return FALSE;
    }

    pExtra->userInfoString = *ppInfoString;
    *ppInfoString = nvKmsPointerToNvU64(kernelInfoString);

    return TRUE;
}

/*
 * Copy the infoString out to userspace and free the kernel-internal buffer.
 */
static NvBool InfoStringDoneUserCommon(
    NvU32 infoStringSize,
    NvU64 pInfoString,
    NvU32 *infoStringLenWritten,
    struct InfoStringExtraUserStateCommon *pExtra)
{
    char *kernelInfoString = nvKmsNvU64ToPointer(pInfoString);
    int status;
    NvBool ret;

    if ((infoStringSize == 0) || (*infoStringLenWritten == 0)) {
        ret = TRUE;
        goto done;
    }

    nvAssert(*infoStringLenWritten <= infoStringSize);

    status = nvkms_copyout(pExtra->userInfoString,
                           kernelInfoString,
                           *infoStringLenWritten);
    if (status == 0) {
        ret = TRUE;
    } else {
        ret = FALSE;
        *infoStringLenWritten = 0;
    }

done:
    nvFree(kernelInfoString);

    return ret;
}

struct NvKmsValidateModeIndexExtraUserState
{
    struct InfoStringExtraUserStateCommon common;
};

static NvBool ValidateModeIndexPrepUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsValidateModeIndexParams *pParams = pParamsVoid;
    struct NvKmsValidateModeIndexExtraUserState *pExtra = pExtraUserStateVoid;

    return InfoStringPrepUserCommon(
        pParams->request.infoStringSize,
        &pParams->request.pInfoString,
        &pExtra->common);
}

static NvBool ValidateModeIndexDoneUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsValidateModeIndexParams *pParams = pParamsVoid;
    struct NvKmsValidateModeIndexExtraUserState *pExtra = pExtraUserStateVoid;

    return InfoStringDoneUserCommon(
        pParams->request.infoStringSize,
        pParams->request.pInfoString,
        &pParams->reply.infoStringLenWritten,
        &pExtra->common);
}

/*!
 * Validate the requested mode.
 */
static NvBool ValidateModeIndex(struct NvKmsPerOpen *pOpen,
                                void *pParamsVoid)
{
    struct NvKmsValidateModeIndexParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    nvValidateModeIndex(pDpyEvo, &pParams->request, &pParams->reply);

    return TRUE;
}

struct NvKmsValidateModeExtraUserState
{
    struct InfoStringExtraUserStateCommon common;
};

static NvBool ValidateModePrepUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsValidateModeParams *pParams = pParamsVoid;
    struct NvKmsValidateModeExtraUserState *pExtra = pExtraUserStateVoid;

    return InfoStringPrepUserCommon(
        pParams->request.infoStringSize,
        &pParams->request.pInfoString,
        &pExtra->common);
}

static NvBool ValidateModeDoneUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsValidateModeParams *pParams = pParamsVoid;
    struct NvKmsValidateModeExtraUserState *pExtra = pExtraUserStateVoid;

    return InfoStringDoneUserCommon(
        pParams->request.infoStringSize,
        pParams->request.pInfoString,
        &pParams->reply.infoStringLenWritten,
        &pExtra->common);
}

/*!
 * Validate the requested mode.
 */
static NvBool ValidateMode(struct NvKmsPerOpen *pOpen,
                           void *pParamsVoid)
{
    struct NvKmsValidateModeParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    nvValidateModeEvo(pDpyEvo, &pParams->request, &pParams->reply);

    return TRUE;
}

static NvBool
CopyInOneLut(NvU64 pRampsUser, struct NvKmsLutRamps **ppRampsKernel)
{
    struct NvKmsLutRamps *pRampsKernel = NULL;
    int status;

    if (pRampsUser == 0) {
        return TRUE;
    }

    if (!nvKmsNvU64AddressIsSafe(pRampsUser)) {
        return FALSE;
    }

    pRampsKernel = nvAlloc(sizeof(*pRampsKernel));
    if (!pRampsKernel) {
        return FALSE;
    }

    status = nvkms_copyin((char *)pRampsKernel, pRampsUser,
                          sizeof(*pRampsKernel));
    if (status != 0) {
        nvFree(pRampsKernel);
        return FALSE;
    }

    *ppRampsKernel = pRampsKernel;

    return TRUE;
}

static NvBool
CopyInLutParams(struct NvKmsSetLutCommonParams *pCommonLutParams)
{
    struct NvKmsLutRamps *pInputRamps = NULL;
    struct NvKmsLutRamps *pOutputRamps = NULL;

    if (!CopyInOneLut(pCommonLutParams->input.pRamps, &pInputRamps)) {
        goto fail;
    }
    if (!CopyInOneLut(pCommonLutParams->output.pRamps, &pOutputRamps)) {
        goto fail;
    }

    pCommonLutParams->input.pRamps = nvKmsPointerToNvU64(pInputRamps);
    pCommonLutParams->output.pRamps = nvKmsPointerToNvU64(pOutputRamps);

    return TRUE;

fail:
    nvFree(pInputRamps);
    nvFree(pOutputRamps);
    return FALSE;
}

static void
FreeCopiedInLutParams(struct NvKmsSetLutCommonParams *pCommonLutParams)
{
    struct NvKmsLutRamps *pInputRamps =
        nvKmsNvU64ToPointer(pCommonLutParams->input.pRamps);
    struct NvKmsLutRamps *pOutputRamps =
        nvKmsNvU64ToPointer(pCommonLutParams->output.pRamps);

    nvFree(pInputRamps);
    nvFree(pOutputRamps);
}

/* No extra user state needed for SetMode; although we lose the user pointers
 * for the LUT ramps after copying them in, that's okay because we don't need
 * to copy them back out again. */
struct NvKmsSetModeExtraUserState
{
};

/*!
 * Copy in any data referenced by pointer for the SetMode request.  Currently
 * this is only the LUT ramps.
 */
static NvBool SetModePrepUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsSetModeParams *pParams = pParamsVoid;
    struct NvKmsSetModeRequest *pReq = &pParams->request;
    NvU32 disp, head, dispFailed, headFailed;

    /* Iterate over all of the common LUT ramp pointers embedded in the SetMode
     * request, and copy in each one. */
    for (disp = 0; disp < ARRAY_LEN(pReq->disp); disp++) {
        for (head = 0; head < ARRAY_LEN(pReq->disp[disp].head); head++) {
            struct NvKmsSetLutCommonParams *pCommonLutParams =
                &pReq->disp[disp].head[head].lut;

            if (!CopyInLutParams(pCommonLutParams)) {
                /* Remember how far we got through these loops before we
                 * failed, so that we can undo everything up to this point. */
                dispFailed = disp;
                headFailed = head;
                goto fail;
            }
        }
    }

    return TRUE;

fail:
    for (disp = 0; disp < ARRAY_LEN(pReq->disp); disp++) {
        for (head = 0; head < ARRAY_LEN(pReq->disp[disp].head); head++) {
            struct NvKmsSetLutCommonParams *pCommonLutParams =
                &pReq->disp[disp].head[head].lut;

            if (disp > dispFailed ||
                (disp == dispFailed && head >= headFailed)) {
                break;
            }

            FreeCopiedInLutParams(pCommonLutParams);
        }
    }

    return FALSE;
}

/*!
 * Free buffers allocated in SetModePrepUser.
 */
static NvBool SetModeDoneUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsSetModeParams *pParams = pParamsVoid;
    struct NvKmsSetModeRequest *pReq = &pParams->request;
    NvU32 disp, head;

    for (disp = 0; disp < ARRAY_LEN(pReq->disp); disp++) {
        for (head = 0; head < ARRAY_LEN(pReq->disp[disp].head); head++) {
            struct NvKmsSetLutCommonParams *pCommonLutParams =
                &pReq->disp[disp].head[head].lut;

            FreeCopiedInLutParams(pCommonLutParams);
        }
    }

    return TRUE;
}

/*!
 * Perform a modeset on the device.
 */
static NvBool SetMode(struct NvKmsPerOpen *pOpen,
                      void *pParamsVoid)
{
    struct NvKmsSetModeParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    return nvSetDispModeEvo(pOpenDev->pDevEvo, pOpenDev,
                            &pParams->request, &pParams->reply,
                            FALSE /* bypassComposition */,
                            TRUE /* doRasterLock */);
}

static inline NvBool nvHsIoctlSetCursorImage(
    NVDispEvoPtr pDispEvo,
    const struct NvKmsPerOpenDev *pOpenDevice,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvU32 head,
    const struct NvKmsSetCursorImageCommonParams *pParams)
{
    return nvSetCursorImage(pDispEvo,
                            pOpenDevice,
                            pOpenDevSurfaceHandles,
                            head,
                            pParams);
}

/*!
 * Set the cursor image.
 */
static NvBool SetCursorImage(struct NvKmsPerOpen *pOpen,
                             void *pParamsVoid)
{
    struct NvKmsSetCursorImageParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    struct NvKmsPerOpenDisp *pOpenDisp;
    NVDispEvoPtr pDispEvo;

    if (!GetPerOpenDevAndDisp(pOpen,
                              pParams->request.deviceHandle,
                              pParams->request.dispHandle,
                              &pOpenDev,
                              &pOpenDisp)) {
        return FALSE;
    }

    pDispEvo = pOpenDisp->pDispEvo;

    if (!nvHeadIsActive(pDispEvo, pParams->request.head)) {
        return FALSE;
    }

    return nvHsIoctlSetCursorImage(pDispEvo,
                                   pOpenDev,
                                   &pOpenDev->surfaceHandles,
                                   pParams->request.head,
                                   &pParams->request.common);
}

static inline NvBool nvHsIoctlMoveCursor(
    NVDispEvoPtr pDispEvo,
    NvU32 head,
    const struct NvKmsMoveCursorCommonParams *pParams)
{
    nvEvoMoveCursor(pDispEvo, head, pParams);
    return TRUE;
}

/*!
 * Change the cursor position.
 */
static NvBool MoveCursor(struct NvKmsPerOpen *pOpen,
                         void *pParamsVoid)
{
    struct NvKmsMoveCursorParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp *pOpenDisp;
    NVDispEvoPtr pDispEvo;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    pDispEvo = pOpenDisp->pDispEvo;

    if (!nvHeadIsActive(pDispEvo, pParams->request.head)) {
        return FALSE;
    }

    return nvHsIoctlMoveCursor(pDispEvo,
                               pParams->request.head,
                               &pParams->request.common);
}

/* No extra user state needed for SetLut; although we lose the user pointers
 * for the LUT ramps after copying them in, that's okay because we don't need
 * to copy them back out again. */
struct NvKmsSetLutExtraUserState
{
};

/*!
 * Copy in any data referenced by pointer for the SetLut request.  Currently
 * this is only the LUT ramps.
 */
static NvBool SetLutPrepUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsSetLutParams *pParams = pParamsVoid;
    struct NvKmsSetLutCommonParams *pCommonLutParams = &pParams->request.common;

    return CopyInLutParams(pCommonLutParams);
}

/*!
 * Free buffers allocated in SetLutPrepUser.
 */
static NvBool SetLutDoneUser(
    void *pParamsVoid,
    void *pExtraUserStateVoid)
{
    struct NvKmsSetLutParams *pParams = pParamsVoid;
    struct NvKmsSetLutCommonParams *pCommonLutParams = &pParams->request.common;

    FreeCopiedInLutParams(pCommonLutParams);

    return TRUE;
}

/*!
 * Set the LUT on the specified head.
 */
static NvBool SetLut(struct NvKmsPerOpen *pOpen,
                     void *pParamsVoid)
{
    struct NvKmsSetLutParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp *pOpenDisp;
    NVDispEvoPtr pDispEvo;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    pDispEvo = pOpenDisp->pDispEvo;

    if (!nvHeadIsActive(pDispEvo, pParams->request.head)) {
        return FALSE;
    }

    if (!nvValidateSetLutCommonParams(pDispEvo->pDevEvo,
                                      &pParams->request.common)) {
        return FALSE;
    }

    nvEvoSetLut(pDispEvo,
                pParams->request.head, TRUE /* kickoff */,
                &pParams->request.common);

    return TRUE;
}


/*!
 * Return whether the specified head is idle.
 */
static NvBool IdleBaseChannelCheckIdleOneHead(
    NVDispEvoPtr pDispEvo,
    NvU32 head)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    {
        NVEvoChannelPtr pMainLayerChannel =
            pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
        NvBool isMethodPending = FALSE;
        NvBool ret;

        ret = pDevEvo->hal->IsChannelMethodPending(pDevEvo, pMainLayerChannel,
                                                   pDispEvo->displayOwner, &isMethodPending);
        return !ret || !isMethodPending;
    }
}

/*!
 * Return whether all heads described in pRequest are idle.
 *
 * Note that we loop over all requested heads, rather than return FALSE once we
 * find the first non-idle head, because checking for idle has side effects: in
 * headSurface, checking for idle gives the headSurface flip queue the
 * opportunity to proceed another frame.
 */
static NvBool IdleBaseChannelCheckIdle(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsIdleBaseChannelRequest *pRequest,
    struct NvKmsIdleBaseChannelReply *pReply)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;
    NvBool allIdle = TRUE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        for (head = 0; head < pDevEvo->numHeads; head++) {

            NvBool idle;

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            if ((pRequest->subDevicesPerHead[head] & NVBIT(sd)) == 0) {
                continue;
            }

            idle = IdleBaseChannelCheckIdleOneHead(pDispEvo, head);

            if (!idle) {
                pReply->stopSubDevicesPerHead[head] |= NVBIT(sd);
            }
            allIdle = allIdle && idle;
        }
    }

    return allIdle;
}

/*!
 * Idle all requested heads.
 *
 * First, wait for the heads to idle naturally.  If a timeout is exceeded, then
 * force the non-idle heads to idle, and record these in pReply.
 */
static NvBool IdleBaseChannelAll(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsIdleBaseChannelRequest *pRequest,
    struct NvKmsIdleBaseChannelReply *pReply)
{
    NvU64 startTime = 0;

    /*
     * Each element in subDevicesPerHead[] must be large enough to hold one bit
     * per subdevice.
     */
    ct_assert(NVKMS_MAX_SUBDEVICES <=
              (sizeof(pRequest->subDevicesPerHead[0]) * 8));

    /* Loop until all head,sd pairs are idle, or we time out. */
    do {
        const NvU32 timeout = 2000000; /* 2 seconds */


        /*
         * Clear the pReply data,
         * IdleBaseChannelCheckIdle() will fill it afresh.
         */
        nvkms_memset(pReply, 0, sizeof(*pReply));

        /* If all heads are idle, we are done. */
        if (IdleBaseChannelCheckIdle(pDevEvo, pRequest, pReply)) {
            return TRUE;
        }

        /* Break out of the loop if we exceed the timeout. */
        if (nvExceedsTimeoutUSec(&startTime, timeout)) {
            break;
        }

        /* At least one head is not idle; yield, and try again. */
        nvkms_yield();

    } while (TRUE);

    return TRUE;
}


/*!
 * Wait for the requested base channels to be idle, returning whether
 * stopping the base channels was necessary.
 */
static NvBool IdleBaseChannel(struct NvKmsPerOpen *pOpen,
                              void *pParamsVoid)
{
    struct NvKmsIdleBaseChannelParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    /* Only the modesetOwner can idle base. */

    if (pOpenDev->pDevEvo->modesetOwner != pOpenDev) {
        return FALSE;
    }

    return IdleBaseChannelAll(pOpenDev->pDevEvo,
                              &pParams->request, &pParams->reply);
}


static inline NvBool nvHsIoctlFlip(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsPerOpenDev *pOpenDev,
    const struct NvKmsFlipRequest *pRequest,
    struct NvKmsFlipReply *pReply)
{
    return nvFlipEvo(pOpenDev->pDevEvo,
                     pOpenDev,
                     pRequest,
                     pReply,
                     FALSE /* skipUpdate */,
                     TRUE /* allowFlipLock */);
}

/*!
 * Flip the specified head.
 */
static NvBool Flip(struct NvKmsPerOpen *pOpen,
                   void *pParamsVoid)
{
    struct NvKmsFlipParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    return nvHsIoctlFlip(pOpenDev->pDevEvo, pOpenDev,
                         &pParams->request, &pParams->reply);
}


/*!
 * Record whether this client is interested in the specified dynamic
 * dpy.
 */
static NvBool DeclareDynamicDpyInterest(struct NvKmsPerOpen *pOpen,
                                        void *pParamsVoid)
{
    /* XXX NVKMS TODO: implement me. */

    return TRUE;
}


/*!
 * Register a surface with the specified per-open + device.
 */
static NvBool RegisterSurface(struct NvKmsPerOpen *pOpen,
                              void *pParamsVoid)
{
    struct NvKmsRegisterSurfaceParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    /*
     * Only allow userspace clients to specify memory objects by FD.
     * This prevents clients from specifying (hClient, hObject) tuples that
     * really belong to other clients.
     */
    if (pOpen->clientType == NVKMS_CLIENT_USER_SPACE &&
        !pParams->request.useFd) {
        return FALSE;
    }

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    nvEvoRegisterSurface(pOpenDev->pDevEvo, pOpenDev, pParams,
                         NvHsMapPermissionsReadOnly);
    return TRUE;
}


/*!
 * Unregister a surface from the specified per-open + device.
 */
static NvBool UnregisterSurface(struct NvKmsPerOpen *pOpen,
                                void *pParamsVoid)
{
    struct NvKmsUnregisterSurfaceParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    nvEvoUnregisterSurface(pOpenDev->pDevEvo, pOpenDev,
                           pParams->request.surfaceHandle,
                           FALSE /* skipUpdate */);
    return TRUE;
}


/*!
 * Associate a surface with the NvKmsPerOpen specified by
 * NvKmsGrantSurfaceParams::request::fd.
 */
static NvBool GrantSurface(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsGrantSurfaceParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    NVSurfaceEvoPtr pSurfaceEvo;
    struct NvKmsPerOpen *pOpenFd;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    pSurfaceEvo =
        nvEvoGetSurfaceFromHandleNoCtxDmaOk(pOpenDev->pDevEvo,
                                            &pOpenDev->surfaceHandles,
                                            pParams->request.surfaceHandle);
    if (pSurfaceEvo == NULL) {
        return FALSE;
    }

    if (nvEvoSurfaceRefCntsTooLarge(pSurfaceEvo)) {
        return FALSE;
    }

    /* Only the owner of the surface can grant it to other clients. */

    if (!nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev,
                             pParams->request.surfaceHandle)) {
        return FALSE;
    }

    pOpenFd = nvkms_get_per_open_data(pParams->request.fd);

    if (pOpenFd == NULL) {
        return FALSE;
    }

    if (!AssignNvKmsPerOpenType(
            pOpenFd, NvKmsPerOpenTypeGrantSurface, FALSE)) {
        return FALSE;
    }

    nvEvoIncrementSurfaceStructRefCnt(pSurfaceEvo);
    pOpenFd->grantSurface.pSurfaceEvo = pSurfaceEvo;

    return TRUE;
}


/*!
 * Retrieve the surface and device associated with
 * NvKmsAcquireSurfaceParams::request::fd, and give the client an
 * NvKmsSurfaceHandle to the surface.
 */
static NvBool AcquireSurface(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsAcquireSurfaceParams *pParams = pParamsVoid;
    struct NvKmsPerOpen *pOpenFd;
    struct NvKmsPerOpenDev *pOpenDev;
    NvKmsSurfaceHandle surfaceHandle = 0;

    pOpenFd = nvkms_get_per_open_data(pParams->request.fd);

    if (pOpenFd == NULL) {
        return FALSE;
    }

    if (pOpenFd->type != NvKmsPerOpenTypeGrantSurface) {
        return FALSE;
    }

    nvAssert(pOpenFd->grantSurface.pSurfaceEvo != NULL);

    if (pOpenFd->grantSurface.pSurfaceEvo->rmRefCnt == 0) { /* orphan */
        return FALSE;
    }

    if (nvEvoSurfaceRefCntsTooLarge(pOpenFd->grantSurface.pSurfaceEvo)) {
        return FALSE;
    }

    /* Since the surface isn't orphaned, it should have an owner, with a
     * pOpenDev and a pDevEvo.  Get the pOpenDev for the acquiring client that
     * matches the owner's pDevEvo. */
    nvAssert(pOpenFd->grantSurface.pSurfaceEvo->owner.pOpenDev->pDevEvo != NULL);
    pOpenDev = DevEvoToOpenDev(pOpen,
        pOpenFd->grantSurface.pSurfaceEvo->owner.pOpenDev->pDevEvo);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    surfaceHandle =
        nvEvoCreateApiHandle(&pOpenDev->surfaceHandles,
                             pOpenFd->grantSurface.pSurfaceEvo);

    if (surfaceHandle == 0) {
        return FALSE;
    }

    nvEvoIncrementSurfaceStructRefCnt(pOpenFd->grantSurface.pSurfaceEvo);

    pParams->reply.deviceHandle = pOpenDev->nvKmsApiHandle;
    pParams->reply.surfaceHandle = surfaceHandle;

    return TRUE;
}

static NvBool ReleaseSurface(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsReleaseSurfaceParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    nvEvoReleaseSurface(pOpenDev->pDevEvo, pOpenDev,
                        pParams->request.surfaceHandle);
    return TRUE;
}


/*!
 * Change the value of the specified attribute.
 */
static NvBool SetDpyAttribute(struct NvKmsPerOpen *pOpen,
                              void *pParamsVoid)
{
    struct NvKmsSetDpyAttributeParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    return nvSetDpyAttributeEvo(pDpyEvo, pParams);
}


/*!
 * Get the value of the specified attribute.
 */
static NvBool GetDpyAttribute(struct NvKmsPerOpen *pOpen,
                              void *pParamsVoid)
{
    struct NvKmsGetDpyAttributeParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    return nvGetDpyAttributeEvo(pDpyEvo, pParams);
}


/*!
 * Get the valid values of the specified attribute.
 */
static NvBool GetDpyAttributeValidValues(struct NvKmsPerOpen *pOpen,
                                         void *pParamsVoid)
{
    struct NvKmsGetDpyAttributeValidValuesParams *pParams = pParamsVoid;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen,
                            pParams->request.deviceHandle,
                            pParams->request.dispHandle,
                            pParams->request.dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    return nvGetDpyAttributeValidValuesEvo(pDpyEvo, pParams);
}


/*!
 * Set the value of the specified attribute.
 */
static NvBool SetDispAttribute(struct NvKmsPerOpen *pOpen,
                               void *pParamsVoid)
{
    struct NvKmsSetDispAttributeParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    return nvSetDispAttributeEvo(pOpenDisp->pDispEvo, pParams);
}


/*!
 * Get the value of the specified attribute.
 */
static NvBool GetDispAttribute(struct NvKmsPerOpen *pOpen,
                               void *pParamsVoid)
{
    struct NvKmsGetDispAttributeParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    return nvGetDispAttributeEvo(pOpenDisp->pDispEvo, pParams);
}


/*!
 * Get the valid values of the specified attribute.
 */
static NvBool GetDispAttributeValidValues(struct NvKmsPerOpen *pOpen,
                                          void *pParamsVoid)
{
    struct NvKmsGetDispAttributeValidValuesParams *pParams = pParamsVoid;

    struct NvKmsPerOpenDisp *pOpenDisp;

    pOpenDisp = GetPerOpenDisp(pOpen,
                               pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        return FALSE;
    }

    return nvGetDispAttributeValidValuesEvo(pOpenDisp->pDispEvo, pParams);
}


/*!
 * Get information about the specified framelock device.
 */
static NvBool QueryFrameLock(struct NvKmsPerOpen *pOpen,
                             void *pParamsVoid)
{
    struct NvKmsQueryFrameLockParams *pParams = pParamsVoid;
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    const NVFrameLockEvoRec *pFrameLockEvo;
    NvU32 gpu;

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pOpenFrameLock =
        GetPerOpenFrameLock(pOpen, pParams->request.frameLockHandle);

    if (pOpenFrameLock == NULL) {
        return FALSE;
    }

    pFrameLockEvo = pOpenFrameLock->pFrameLockEvo;

    ct_assert(ARRAY_LEN(pFrameLockEvo->gpuIds) <=
              ARRAY_LEN(pParams->reply.gpuIds));

    for (gpu = 0; gpu < pFrameLockEvo->nGpuIds; gpu++) {
        pParams->reply.gpuIds[gpu] = pFrameLockEvo->gpuIds[gpu];
    }

    return TRUE;
}


static NvBool SetFrameLockAttribute(struct NvKmsPerOpen *pOpen,
                                    void *pParamsVoid)
{
    struct NvKmsSetFrameLockAttributeParams *pParams = pParamsVoid;
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    NVFrameLockEvoRec *pFrameLockEvo;

    pOpenFrameLock =
        GetPerOpenFrameLock(pOpen, pParams->request.frameLockHandle);

    if (pOpenFrameLock == NULL) {
        return FALSE;
    }

    pFrameLockEvo = pOpenFrameLock->pFrameLockEvo;

    return nvSetFrameLockAttributeEvo(pFrameLockEvo, pParams);
}


static NvBool GetFrameLockAttribute(struct NvKmsPerOpen *pOpen,
                                    void *pParamsVoid)
{
    struct NvKmsGetFrameLockAttributeParams *pParams = pParamsVoid;
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    const NVFrameLockEvoRec *pFrameLockEvo;

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pOpenFrameLock =
        GetPerOpenFrameLock(pOpen, pParams->request.frameLockHandle);

    if (pOpenFrameLock == NULL) {
        return FALSE;
    }

    pFrameLockEvo = pOpenFrameLock->pFrameLockEvo;

    return nvGetFrameLockAttributeEvo(pFrameLockEvo, pParams);
}


static NvBool GetFrameLockAttributeValidValues(struct NvKmsPerOpen *pOpen,
                                               void *pParamsVoid)
{
    struct NvKmsGetFrameLockAttributeValidValuesParams *pParams = pParamsVoid;
    struct NvKmsPerOpenFrameLock *pOpenFrameLock;
    const NVFrameLockEvoRec *pFrameLockEvo;

    nvkms_memset(&pParams->reply, 0, sizeof(pParams->reply));

    pOpenFrameLock =
        GetPerOpenFrameLock(pOpen, pParams->request.frameLockHandle);

    if (pOpenFrameLock == NULL) {
        return FALSE;
    }

    pFrameLockEvo = pOpenFrameLock->pFrameLockEvo;

    return nvGetFrameLockAttributeValidValuesEvo(pFrameLockEvo, pParams);
}


/*!
 * Pop the next event off of the client's event queue.
 */
static NvBool GetNextEvent(struct NvKmsPerOpen *pOpen,
                           void *pParamsVoid)
{
    struct NvKmsGetNextEventParams *pParams = pParamsVoid;
    struct NvKmsPerOpenEventListEntry *pEntry;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (nvListIsEmpty(&pOpen->ioctl.eventList)) {
        pParams->reply.valid = FALSE;
        return TRUE;
    }

    pEntry = nvListFirstEntry(&pOpen->ioctl.eventList,
                              struct NvKmsPerOpenEventListEntry,
                              eventListEntry);

    pParams->reply.valid = TRUE;
    pParams->reply.event = pEntry->event;

    nvListDel(&pEntry->eventListEntry);

    nvFree(pEntry);

    if (nvListIsEmpty(&pOpen->ioctl.eventList)) {
        nvkms_event_queue_changed(pOpen->pOpenKernel, FALSE);
    }

    return TRUE;
}


/*!
 * Record the client's event interest for the specified device.
 */
static NvBool DeclareEventInterest(struct NvKmsPerOpen *pOpen,
                                   void *pParamsVoid)
{
    struct NvKmsDeclareEventInterestParams *pParams = pParamsVoid;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    pOpen->ioctl.eventInterestMask = pParams->request.interestMask;

    return TRUE;
}

static NvBool ClearUnicastEvent(struct NvKmsPerOpen *pOpen,
                                void *pParamsVoid)
{
    struct NvKmsClearUnicastEventParams *pParams = pParamsVoid;
    struct NvKmsPerOpen *pOpenFd = NULL;

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    pOpenFd = nvkms_get_per_open_data(pParams->request.unicastEventFd);

    if (pOpenFd == NULL) {
        return FALSE;
    }

    if (pOpenFd->type != NvKmsPerOpenTypeUnicastEvent) {
        return FALSE;
    }

    nvkms_event_queue_changed(pOpenFd->pOpenKernel, FALSE);

    return TRUE;
}

static NvBool SetLayerPosition(struct NvKmsPerOpen *pOpen,
                                 void *pParamsVoid)
{
    struct NvKmsSetLayerPositionParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    /* XXX NVKMS HEADSURFACE TODO: intercept */

    return nvLayerSetPositionEvo(pOpenDev->pDevEvo, &pParams->request);
}

static NvBool GrabOwnership(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsGrabOwnershipParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    // The only kind of ownership right now is modeset ownership.
    return GrabModesetOwnership(pOpenDev);
}

static NvBool ReleaseOwnership(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsReleaseOwnershipParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    // The only kind of ownership right now is modeset ownership.
    return ReleaseModesetOwnership(pOpenDev);
}

static NvBool GrantPermissions(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsGrantPermissionsParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    struct NvKmsPerOpen *pOpenFd;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    /* Only the modesetOwner can grant permissions. */

    if (pOpenDev->pDevEvo->modesetOwner != pOpenDev) {
        return FALSE;
    }

    if (!ValidateNvKmsPermissions(pOpenDev->pDevEvo,
                                  &pParams->request.permissions)) {
        return FALSE;
    }

    pOpenFd = nvkms_get_per_open_data(pParams->request.fd);

    if (pOpenFd == NULL) {
        return FALSE;
    }

    if (!AssignNvKmsPerOpenType(
            pOpenFd, NvKmsPerOpenTypeGrantPermissions, FALSE)) {
        return FALSE;
    }

    pOpenFd->grantPermissions.permissions = pParams->request.permissions;

    pOpenFd->grantPermissions.pDevEvo = pOpenDev->pDevEvo;

    return TRUE;
}

static NvBool AcquirePermissions(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsAcquirePermissionsParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    struct NvKmsPerOpen *pOpenFd;
    const struct NvKmsPermissions *pPermissionsNew;
    enum NvKmsPermissionsType type;

    pOpenFd = nvkms_get_per_open_data(pParams->request.fd);

    if (pOpenFd == NULL) {
        return FALSE;
    }

    if (pOpenFd->type != NvKmsPerOpenTypeGrantPermissions) {
        return FALSE;
    }

    pOpenDev = DevEvoToOpenDev(pOpen, pOpenFd->grantPermissions.pDevEvo);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    type = pOpenFd->grantPermissions.permissions.type;

    pPermissionsNew = &pOpenFd->grantPermissions.permissions;

    if (type == NV_KMS_PERMISSIONS_TYPE_FLIPPING) {
        NvU32 d, h;

        for (d = 0; d < ARRAY_LEN(pOpenDev->flipPermissions.disp); d++) {
            for (h = 0; h < ARRAY_LEN(pOpenDev->flipPermissions.
                                      disp[d].head); h++) {
                pOpenDev->flipPermissions.disp[d].head[h].layerMask |=
                    pPermissionsNew->flip.disp[d].head[h].layerMask;
            }
        }

        pParams->reply.permissions.flip = pOpenDev->flipPermissions;

    } else if (type == NV_KMS_PERMISSIONS_TYPE_MODESET) {
        NvU32 d, h;

        for (d = 0; d < ARRAY_LEN(pOpenDev->modesetPermissions.disp); d++) {
            for (h = 0; h < ARRAY_LEN(pOpenDev->modesetPermissions.
                                      disp[d].head); h++) {
                pOpenDev->modesetPermissions.disp[d].head[h].dpyIdList =
                    nvAddDpyIdListToDpyIdList(
                        pOpenDev->modesetPermissions.disp[d].head[h].dpyIdList,
                        pPermissionsNew->modeset.disp[d].head[h].dpyIdList);
            }
        }

        pParams->reply.permissions.modeset = pOpenDev->modesetPermissions;

    } else {
        /*
         * GrantPermissions() should ensure that
         * pOpenFd->grantPermissions.permissions.type is always valid.
         */
        nvAssert(!"AcquirePermissions validation failure");
        return FALSE;
    }

    pParams->reply.permissions.type = type;
    pParams->reply.deviceHandle = pOpenDev->nvKmsApiHandle;

    return TRUE;
}

static NvBool RevokePermissions(struct NvKmsPerOpen *pOpen, void *pParamsVoid)
{
    struct NvKmsRevokePermissionsParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev =
        GetPerOpenDev(pOpen, pParams->request.deviceHandle);
    const NvU32 validBitmask =
        NVBIT(NV_KMS_PERMISSIONS_TYPE_FLIPPING) |
        NVBIT(NV_KMS_PERMISSIONS_TYPE_MODESET);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    /* Only the modeset owner can revoke permissions. */
    if (pOpenDev->pDevEvo->modesetOwner != pOpenDev) {
        return FALSE;
    }

    /* Reject invalid bitmasks. */

    if ((pParams->request.permissionsTypeBitmask & ~validBitmask) != 0) {
        return FALSE;
    }

    /* Revoke permissions for everyone except the caller. */

    RevokePermissionsInternal(pParams->request.permissionsTypeBitmask,
                              pOpenDev->pDevEvo,
                              pOpenDev /* pOpenDevExclude */);
    return TRUE;
}

static NvBool RegisterDeferredRequestFifo(struct NvKmsPerOpen *pOpen,
                                          void *pParamsVoid)
{
    struct NvKmsRegisterDeferredRequestFifoParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    NVSurfaceEvoPtr pSurfaceEvo;
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    NvKmsDeferredRequestFifoHandle handle;

    pOpenDev = GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    pSurfaceEvo = nvEvoGetSurfaceFromHandleNoCtxDmaOk(
        pOpenDev->pDevEvo,
        &pOpenDev->surfaceHandles,
        pParams->request.surfaceHandle);

    if (pSurfaceEvo == NULL) {
        return FALSE;
    }

    /*
     * WAR Bug 2050970: If a surface is unregistered and it wasn't registered
     * with NvKmsRegisterSurfaceRequest::noDisplayHardwareAccess, then the call
     * to nvRMSyncEvoChannel() in nvEvoDecrementSurfaceRefCnts() may hang
     * if any flips in flight acquire on semaphore releases that haven't
     * occurred yet.
     *
     * Since a ctxdma is not necessary for the deferred request fifo surface,
     * we work around this by forcing all surfaces that will be registered as
     * a deferred request fifo to be registered with
     * noDisplayHardwareAccess==TRUE, then skip the idle in
     * nvEvoDecrementSurfaceRefCnts() for these surfaces.
     */
    if (pSurfaceEvo->requireCtxDma) {
        return FALSE;
    }

    pDeferredRequestFifo =
        nvEvoRegisterDeferredRequestFifo(pOpenDev->pDevEvo, pSurfaceEvo);

    if (pDeferredRequestFifo == NULL) {
        return FALSE;
    }

    handle = nvEvoCreateApiHandle(&pOpenDev->deferredRequestFifoHandles,
                                  pDeferredRequestFifo);

    if (handle == 0) {
        nvEvoUnregisterDeferredRequestFifo(pOpenDev->pDevEvo,
                                           pDeferredRequestFifo);
        return FALSE;
    }

    pParams->reply.deferredRequestFifoHandle = handle;

    return TRUE;
}

static NvBool UnregisterDeferredRequestFifo(struct NvKmsPerOpen *pOpen,
                                            void *pParamsVoid)
{
    struct NvKmsUnregisterDeferredRequestFifoParams *pParams = pParamsVoid;
    NvKmsDeferredRequestFifoHandle handle =
        pParams->request.deferredRequestFifoHandle;
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    struct NvKmsPerOpenDev *pOpenDev =
        GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    pDeferredRequestFifo =
        nvEvoGetPointerFromApiHandle(
            &pOpenDev->deferredRequestFifoHandles, handle);

    if (pDeferredRequestFifo == NULL) {
        return FALSE;
    }

    nvEvoDestroyApiHandle(&pOpenDev->deferredRequestFifoHandles, handle);

    nvEvoUnregisterDeferredRequestFifo(pOpenDev->pDevEvo, pDeferredRequestFifo);

    return TRUE;
}

/*!
 * Get the CRC32 data for the specified dpy.
 */
static NvBool QueryDpyCRC32(struct NvKmsPerOpen *pOpen,
                            void *pParamsVoid)
{
    struct NvKmsQueryDpyCRC32Params *pParams = pParamsVoid;
    struct NvKmsPerOpenDev *pOpenDev;
    struct NvKmsPerOpenDisp *pOpenDisp;
    NVDispEvoPtr pDispEvo;
    CRC32NotifierCrcOut crcOut;

    if (!GetPerOpenDevAndDisp(pOpen,
                              pParams->request.deviceHandle,
                              pParams->request.dispHandle,
                              &pOpenDev,
                              &pOpenDisp)) {
        return FALSE;
    }

    if (pOpenDev->pDevEvo->modesetOwner != pOpenDev) {
        // Only the current owner can query CRC32 values.
        return FALSE;
    }

    pDispEvo = pOpenDisp->pDispEvo;

    if (!nvHeadIsActive(pDispEvo, pParams->request.head)) {
        return FALSE;
    }

    nvkms_memset(&(pParams->reply), 0, sizeof(pParams->reply));

    // Since will only read 1 frame of CRCs, point to single reply struct vals
    crcOut.rasterGeneratorCrc32 = &(pParams->reply.rasterGeneratorCrc32);
    crcOut.compositorCrc32 = &(pParams->reply.compositorCrc32);
    crcOut.outputCrc32 = &(pParams->reply.outputCrc32);

    if (!nvReadCRC32Evo(pDispEvo, pParams->request.head, &crcOut)) {
        return FALSE;
    }

    return TRUE;
}

static NvBool SwitchMux(
    struct NvKmsPerOpen *pOpen,
    void *pParamsVoid)
{
    struct NvKmsSwitchMuxParams *pParams = pParamsVoid;
    const struct NvKmsSwitchMuxRequest *r = &pParams->request;
    NVDpyEvoPtr pDpyEvo;
    NVDevEvoPtr pDevEvo;

    pDpyEvo = GetPerOpenDpy(pOpen, r->deviceHandle, r->dispHandle, r->dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    if (pDevEvo->modesetOwner != GetPerOpenDev(pOpen, r->deviceHandle)) {
        return FALSE;
    }

    switch (pParams->request.operation) {
        case NVKMS_SWITCH_MUX_PRE:
            return nvRmMuxPre(pDpyEvo, r->state);
        case NVKMS_SWITCH_MUX:
            return nvRmMuxSwitch(pDpyEvo, r->state);
        case NVKMS_SWITCH_MUX_POST:
            return nvRmMuxPost(pDpyEvo, r->state);
        default:
            return FALSE;
    }
}

static NvBool GetMuxState(
    struct NvKmsPerOpen *pOpen,
    void *pParamsVoid)
{
    struct NvKmsGetMuxStateParams *pParams = pParamsVoid;
    const struct NvKmsGetMuxStateRequest *r = &pParams->request;
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = GetPerOpenDpy(pOpen, r->deviceHandle, r->dispHandle, r->dpyId);
    if (pDpyEvo == NULL) {
        return FALSE;
    }

    pParams->reply.state = nvRmMuxState(pDpyEvo);

    return pParams->reply.state != MUX_STATE_GET;
}

static NvBool ExportVrrSemaphoreSurface(
    struct NvKmsPerOpen *pOpen,
    void *pParamsVoid)
{
    struct NvKmsExportVrrSemaphoreSurfaceParams *pParams = pParamsVoid;
    const struct NvKmsExportVrrSemaphoreSurfaceRequest *req = &pParams->request;
    const struct NvKmsPerOpenDev *pOpenDev =
        GetPerOpenDev(pOpen, pParams->request.deviceHandle);

    if (pOpenDev == NULL) {
        return FALSE;
    }

    return nvExportVrrSemaphoreSurface(pOpenDev->pDevEvo, req->memFd);
}

static NvBool EnableVblankSyncObject(
    struct NvKmsPerOpen *pOpen,
    void *pParamsVoid)
{
    struct NvKmsEnableVblankSyncObjectParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp* pOpenDisp = NULL;
    NVDispHeadStateEvoRec *pHeadState = NULL;
    NVDevEvoPtr pDevEvo = NULL;
    NvKmsVblankSyncObjectHandle vblankHandle = 0;
    int freeVblankSyncObjectIdx = 0;
    NvU32 head = pParams->request.head;
    NVVblankSyncObjectRec *vblankSyncObjects = NULL;
    NVDispEvoPtr pDispEvo = NULL;
    NVEvoUpdateState updateState = { };

    /* Obtain the Head State. */
    pOpenDisp = GetPerOpenDisp(pOpen, pParams->request.deviceHandle,
                               pParams->request.dispHandle);
    if (pOpenDisp == NULL) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Unable to GetPerOpenDisp.");
        return FALSE;
    }

    pDispEvo = pOpenDisp->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    /* Ensure Vblank Sync Object API is supported on this chip. */
    if (!pDevEvo->supportsSyncpts ||
        !pDevEvo->hal->caps.supportsVblankSyncObjects) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Vblank Sync Object functionality is not "
                      "supported on this chip.");
        return FALSE;
    }

    /* Validate requested head because it comes from user input. */
    if (head >= ARRAY_LEN(pDispEvo->headState)) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Invalid head requested, head=%d.", head);
        return FALSE;
    }
    pHeadState = &pDispEvo->headState[head];
    vblankSyncObjects = pHeadState->vblankSyncObjects;
    pDevEvo = pDispEvo->pDevEvo;

    /*
     * Find the available sync object. Sync Objects with handle=0 are not in
     * use.
     */
    for (freeVblankSyncObjectIdx = 0;
         freeVblankSyncObjectIdx < pHeadState->numVblankSyncObjectsCreated;
         freeVblankSyncObjectIdx++) {
        if (!vblankSyncObjects[freeVblankSyncObjectIdx].inUse) {
            break;
        }
    }
    if (freeVblankSyncObjectIdx == pHeadState->numVblankSyncObjectsCreated) {
        return FALSE;
    }

    /* Save the created vblank handle if it is valid. */
    vblankHandle =
        nvEvoCreateApiHandle(&pOpenDisp->vblankSyncObjectHandles[head],
                             &vblankSyncObjects[freeVblankSyncObjectIdx]);
    if (vblankHandle == 0) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Unable to create vblank handle.");
        return FALSE;
    }

    if (nvHeadIsActive(pDispEvo, head)) {
        /*
         * Instruct the hardware to enable a semaphore corresponding to this
         * syncpt. The Update State will be populated.
         */
        pDevEvo->hal->ConfigureVblankSyncObject(
                    pDevEvo,
                    pHeadState->timings.rasterBlankStart.y,
                    head,
                    freeVblankSyncObjectIdx,
                    vblankSyncObjects[freeVblankSyncObjectIdx].evoSyncpt.hCtxDma,
                    &updateState);

        /*
         * Instruct hardware to execute the staged commands from the
         * ConfigureVblankSyncObject() call above. This will set up and wait for a
         * notification that the hardware execution actually completed.
         */
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState, TRUE);

        vblankSyncObjects[freeVblankSyncObjectIdx].enabled = TRUE;
    }

    /* Populate the vblankSyncObjects array. */
    vblankSyncObjects[freeVblankSyncObjectIdx].inUse = TRUE;

    /* Populate the reply field. */
    pParams->reply.vblankHandle = vblankHandle;
    /* Note: the syncpt ID is NOT the same as the vblank handle. */
    pParams->reply.syncptId =
        pHeadState->vblankSyncObjects[freeVblankSyncObjectIdx].evoSyncpt.id;

    return TRUE;
}

static void DisableAndCleanVblankSyncObject(struct NvKmsPerOpenDisp *pOpenDisp,
                                            NvU32 head,
                                            NVVblankSyncObjectRec *pVblankSyncObject,
                                            NVEvoUpdateState *pUpdateState,
                                            NvKmsVblankSyncObjectHandle handle)
{
    NVDispEvoPtr pDispEvo = pOpenDisp->pDispEvo;

    if (nvHeadIsActive(pDispEvo, head)) {
        /*
         * Instruct the hardware to disable the semaphore corresponding to this
         * syncpt. The Update State will be populated.
         *
         * Note: Using dummy zero value for rasterLine because the disable
         * codepath in ConfigureVblankSyncObject() does not use that argument.
         */
        pDispEvo->pDevEvo->hal->ConfigureVblankSyncObject(pDispEvo->pDevEvo,
                                                          0, /* rasterLine */
                                                          head,
                                                          pVblankSyncObject->index,
                                                          0, /* hCtxDma */
                                                          pUpdateState);
        /*
         * Note: it is the caller's responsibility to call
         * nvEvoUpdateAndKickOff().
         */
    }

    pVblankSyncObject->inUse = FALSE;
    pVblankSyncObject->enabled = FALSE;

    /* Remove the handle from the map. */
    nvEvoDestroyApiHandle(&pOpenDisp->vblankSyncObjectHandles[head], handle);
}

static NvBool DisableVblankSyncObject(
    struct NvKmsPerOpen *pOpen,
    void *pParamsVoid)
{
    struct NvKmsDisableVblankSyncObjectParams *pParams = pParamsVoid;
    struct NvKmsPerOpenDisp* pOpenDisp =
        GetPerOpenDisp(pOpen, pParams->request.deviceHandle,
                       pParams->request.dispHandle);
    NVVblankSyncObjectRec *pVblankSyncObject = NULL;
    NvU32 head = pParams->request.head;
    NVDevEvoPtr pDevEvo = NULL;
    NVEvoUpdateState updateState = { };

    if (pOpenDisp == NULL) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Unable to GetPerOpenDisp.");
        return FALSE;
    }

    pDevEvo = pOpenDisp->pDispEvo->pDevEvo;

    /* Ensure Vblank Sync Object API is supported on this chip. */
    if (!pDevEvo->supportsSyncpts ||
        !pDevEvo->hal->caps.supportsVblankSyncObjects) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Vblank Sync Object functionality is not "
                      "supported on this chip.");
        return FALSE;
    }

    /* Validate requested head because it comes from user input. */
    if (head >= ARRAY_LEN(pOpenDisp->pDispEvo->headState)) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Invalid head requested, head=%d.", head);
        return FALSE;
    }

    /* Mark the indicated object as free. */
    pVblankSyncObject =
        nvEvoGetPointerFromApiHandle(&pOpenDisp->vblankSyncObjectHandles[head],
                                     pParams->request.vblankHandle);
    if (pVblankSyncObject == NULL) {
        nvEvoLogDebug(EVO_LOG_ERROR, "unable to find object with provided "
                      "handle.");
        return FALSE;
    }

    DisableAndCleanVblankSyncObject(pOpenDisp, head, pVblankSyncObject,
                                    &updateState, pParams->request.vblankHandle);

    if (!nvIsUpdateStateEmpty(pOpenDisp->pDispEvo->pDevEvo, &updateState)) {
        /*
         * Instruct hardware to execute the staged commands from the
         * ConfigureVblankSyncObject() call inside of the
         * DisableAndCleanVblankSyncObject() call above. This will set up and
         * wait for a notification that the hardware execution has completed.
         */
        nvEvoUpdateAndKickOff(pOpenDisp->pDispEvo, TRUE, &updateState, TRUE);
    }

    return TRUE;
}

/*!
 * Perform the ioctl operation requested by the client.
 *
 * \param[in,out]  pOpenVoid           The per-open data, allocated by
 *                                     nvKmsOpen().
 * \param[in]      cmdOpaque           The NVKMS_IOCTL_ operation to perform.
 * \param[in,out]  paramsAddress       A pointer, in the client process's
 *                                     address space, to the parameter
 *                                     structure.  This is cmd-specific.
 * \param[in]      paramSize           The client-specified size of the params.
 *
 * \return  Return TRUE if the ioctl operation was successfully
 *          performed.  Otherwise, return FALSE.
 */
NvBool nvKmsIoctl(
    void *pOpenVoid,
    const NvU32 cmdOpaque,
    const NvU64 paramsAddress,
    const size_t paramSize)
{
    static const struct {

        NvBool (*proc)(struct NvKmsPerOpen *pOpen, void *pParamsVoid);
        NvBool (*prepUser)(void *pParamsVoid, void *pExtraStateVoid);
        NvBool (*doneUser)(void *pParamsVoid, void *pExtraStateVoid);
        const size_t paramSize;
        /* Size of extra state tracked for user parameters */
        const size_t extraSize;

        const size_t requestSize;
        const size_t requestOffset;

        const size_t replySize;
        const size_t replyOffset;

    } dispatch[] = {

#define _ENTRY_WITH_USER(_cmd, _func, _prepUser, _doneUser, _extraSize)      \
        [_cmd] = {                                                           \
            .proc          = _func,                                          \
            .prepUser      = _prepUser,                                      \
            .doneUser      = _doneUser,                                      \
            .paramSize     = sizeof(struct NvKms##_func##Params),            \
            .requestSize   = sizeof(struct NvKms##_func##Request),           \
            .requestOffset = offsetof(struct NvKms##_func##Params, request), \
            .replySize     = sizeof(struct NvKms##_func##Reply),             \
            .replyOffset   = offsetof(struct NvKms##_func##Params, reply),   \
            .extraSize     = _extraSize,                                     \
        }

#define ENTRY(_cmd, _func)                                                   \
        _ENTRY_WITH_USER(_cmd, _func, NULL, NULL, 0)

#define ENTRY_CUSTOM_USER(_cmd, _func)                                       \
        _ENTRY_WITH_USER(_cmd, _func,                                        \
                         _func##PrepUser, _func##DoneUser,                   \
                         sizeof(struct NvKms##_func##ExtraUserState))

        ENTRY(NVKMS_IOCTL_ALLOC_DEVICE, AllocDevice),
        ENTRY(NVKMS_IOCTL_FREE_DEVICE, FreeDevice),
        ENTRY(NVKMS_IOCTL_QUERY_DISP, QueryDisp),
        ENTRY(NVKMS_IOCTL_QUERY_CONNECTOR_STATIC_DATA, QueryConnectorStaticData),
        ENTRY(NVKMS_IOCTL_QUERY_CONNECTOR_DYNAMIC_DATA, QueryConnectorDynamicData),
        ENTRY(NVKMS_IOCTL_QUERY_DPY_STATIC_DATA, QueryDpyStaticData),
        ENTRY(NVKMS_IOCTL_QUERY_DPY_DYNAMIC_DATA, QueryDpyDynamicData),
        ENTRY_CUSTOM_USER(NVKMS_IOCTL_VALIDATE_MODE_INDEX, ValidateModeIndex),
        ENTRY_CUSTOM_USER(NVKMS_IOCTL_VALIDATE_MODE, ValidateMode),
        ENTRY_CUSTOM_USER(NVKMS_IOCTL_SET_MODE, SetMode),
        ENTRY(NVKMS_IOCTL_SET_CURSOR_IMAGE, SetCursorImage),
        ENTRY(NVKMS_IOCTL_MOVE_CURSOR, MoveCursor),
        ENTRY_CUSTOM_USER(NVKMS_IOCTL_SET_LUT, SetLut),
        ENTRY(NVKMS_IOCTL_IDLE_BASE_CHANNEL, IdleBaseChannel),
        ENTRY(NVKMS_IOCTL_FLIP, Flip),
        ENTRY(NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST,
              DeclareDynamicDpyInterest),
        ENTRY(NVKMS_IOCTL_REGISTER_SURFACE, RegisterSurface),
        ENTRY(NVKMS_IOCTL_UNREGISTER_SURFACE, UnregisterSurface),
        ENTRY(NVKMS_IOCTL_GRANT_SURFACE, GrantSurface),
        ENTRY(NVKMS_IOCTL_ACQUIRE_SURFACE, AcquireSurface),
        ENTRY(NVKMS_IOCTL_RELEASE_SURFACE, ReleaseSurface),
        ENTRY(NVKMS_IOCTL_SET_DPY_ATTRIBUTE, SetDpyAttribute),
        ENTRY(NVKMS_IOCTL_GET_DPY_ATTRIBUTE, GetDpyAttribute),
        ENTRY(NVKMS_IOCTL_GET_DPY_ATTRIBUTE_VALID_VALUES,
              GetDpyAttributeValidValues),
        ENTRY(NVKMS_IOCTL_SET_DISP_ATTRIBUTE, SetDispAttribute),
        ENTRY(NVKMS_IOCTL_GET_DISP_ATTRIBUTE, GetDispAttribute),
        ENTRY(NVKMS_IOCTL_GET_DISP_ATTRIBUTE_VALID_VALUES,
              GetDispAttributeValidValues),
        ENTRY(NVKMS_IOCTL_QUERY_FRAMELOCK, QueryFrameLock),
        ENTRY(NVKMS_IOCTL_SET_FRAMELOCK_ATTRIBUTE, SetFrameLockAttribute),
        ENTRY(NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE, GetFrameLockAttribute),
        ENTRY(NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE_VALID_VALUES,
              GetFrameLockAttributeValidValues),
        ENTRY(NVKMS_IOCTL_GET_NEXT_EVENT, GetNextEvent),
        ENTRY(NVKMS_IOCTL_DECLARE_EVENT_INTEREST, DeclareEventInterest),
        ENTRY(NVKMS_IOCTL_CLEAR_UNICAST_EVENT, ClearUnicastEvent),
        ENTRY(NVKMS_IOCTL_SET_LAYER_POSITION, SetLayerPosition),
        ENTRY(NVKMS_IOCTL_GRAB_OWNERSHIP, GrabOwnership),
        ENTRY(NVKMS_IOCTL_RELEASE_OWNERSHIP, ReleaseOwnership),
        ENTRY(NVKMS_IOCTL_GRANT_PERMISSIONS, GrantPermissions),
        ENTRY(NVKMS_IOCTL_ACQUIRE_PERMISSIONS, AcquirePermissions),
        ENTRY(NVKMS_IOCTL_REVOKE_PERMISSIONS, RevokePermissions),
        ENTRY(NVKMS_IOCTL_QUERY_DPY_CRC32, QueryDpyCRC32),
        ENTRY(NVKMS_IOCTL_REGISTER_DEFERRED_REQUEST_FIFO,
              RegisterDeferredRequestFifo),
        ENTRY(NVKMS_IOCTL_UNREGISTER_DEFERRED_REQUEST_FIFO,
              UnregisterDeferredRequestFifo),
        ENTRY(NVKMS_IOCTL_SWITCH_MUX, SwitchMux),
        ENTRY(NVKMS_IOCTL_GET_MUX_STATE, GetMuxState),
        ENTRY(NVKMS_IOCTL_EXPORT_VRR_SEMAPHORE_SURFACE, ExportVrrSemaphoreSurface),
        ENTRY(NVKMS_IOCTL_ENABLE_VBLANK_SYNC_OBJECT, EnableVblankSyncObject),
        ENTRY(NVKMS_IOCTL_DISABLE_VBLANK_SYNC_OBJECT, DisableVblankSyncObject),
    };

    struct NvKmsPerOpen *pOpen = pOpenVoid;
    void *pParamsKernelPointer;
    NvBool ret;
    enum NvKmsIoctlCommand cmd = cmdOpaque;
    void *pExtraUserState = NULL;

    if (!AssignNvKmsPerOpenType(pOpen, NvKmsPerOpenTypeIoctl, TRUE)) {
        return FALSE;
    }

    if (cmd >= ARRAY_LEN(dispatch)) {
        return FALSE;
    }

    if (dispatch[cmd].proc == NULL) {
        return FALSE;
    }

    if (paramSize != dispatch[cmd].paramSize) {
        return FALSE;
    }

    if (pOpen->clientType == NVKMS_CLIENT_USER_SPACE) {
        pParamsKernelPointer = nvCalloc(1, paramSize + dispatch[cmd].extraSize);
        if (pParamsKernelPointer == NULL) {
            return FALSE;
        }

        if (dispatch[cmd].requestSize > 0) {
            int status =
                nvkms_copyin((char *) pParamsKernelPointer +
                             dispatch[cmd].requestOffset,
                             paramsAddress + dispatch[cmd].requestOffset,
                             dispatch[cmd].requestSize);
            if (status != 0) {
                nvFree(pParamsKernelPointer);
                return FALSE;
            }
        }

        if (dispatch[cmd].prepUser) {
            pExtraUserState = (char *)pParamsKernelPointer + paramSize;

            if (!dispatch[cmd].prepUser(pParamsKernelPointer,
                                        pExtraUserState)) {
                nvFree(pParamsKernelPointer);
                return FALSE;
            }
        }
    } else {
        pParamsKernelPointer = nvKmsNvU64ToPointer(paramsAddress);
    }

    ret = dispatch[cmd].proc(pOpen, pParamsKernelPointer);

    if (pOpen->clientType == NVKMS_CLIENT_USER_SPACE) {

        if (dispatch[cmd].doneUser) {
            pExtraUserState = (char *)pParamsKernelPointer + paramSize;

            if (!dispatch[cmd].doneUser(pParamsKernelPointer,
                                        pExtraUserState)) {
                ret = FALSE;
            }
        }

        if (dispatch[cmd].replySize > 0) {
            int status =
                nvkms_copyout(paramsAddress + dispatch[cmd].replyOffset,
                              (char *) pParamsKernelPointer +
                              dispatch[cmd].replyOffset,
                              dispatch[cmd].replySize);
            if (status != 0) {
                ret = FALSE;
            }
        }

        nvFree(pParamsKernelPointer);
    }

    return ret;
}


/*!
 * Close callback.
 *
 * \param[in,out]  pOpenVoid    The per-open data, allocated by nvKmsOpen().
 */
void nvKmsClose(void *pOpenVoid)
{
    struct NvKmsPerOpen *pOpen = pOpenVoid;

    if (pOpen == NULL) {
        return;
    }

    /*
     * First remove the pOpen from global tracking.  Otherwise, assertions can
     * fail in the free paths below -- the assertions check that the object
     * being freed is not tracked by any pOpen.
     */
    nvListDel(&pOpen->perOpenListEntry);

    if (pOpen->type == NvKmsPerOpenTypeIoctl) {

        struct NvKmsPerOpenEventListEntry *pEntry, *pEntryTmp;
        struct NvKmsPerOpenDev *pOpenDev;
        NvKmsGenericHandle dev;

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                            pOpenDev, dev) {
            FreeDeviceReference(pOpen, pOpenDev);
        }

        nvEvoDestroyApiHandles(&pOpen->ioctl.frameLockHandles);

        nvEvoDestroyApiHandles(&pOpen->ioctl.devHandles);

        nvListForEachEntry_safe(pEntry, pEntryTmp,
                                &pOpen->ioctl.eventList, eventListEntry) {
            nvListDel(&pEntry->eventListEntry);
            nvFree(pEntry);
        }

        nvListDel(&pOpen->perOpenIoctlListEntry);
    }

    if (pOpen->type == NvKmsPerOpenTypeGrantSurface) {
        nvAssert(pOpen->grantSurface.pSurfaceEvo != NULL);
        nvEvoDecrementSurfaceStructRefCnt(pOpen->grantSurface.pSurfaceEvo);
    }

    if (pOpen->type == NvKmsPerOpenTypeGrantSwapGroup) {
        nvAssert(pOpen->grantSwapGroup.pSwapGroup != NULL);
    }

    if (pOpen->type == NvKmsPerOpenTypeUnicastEvent) {
        nvRemoveUnicastEvent(pOpen);
    }

    nvFree(pOpen);
}


/*!
 * Open callback.
 *
 * Allocate, initialize, and return an opaque pointer to an NvKmsPerOpen.
 *
 * \return  If successful, return an NvKmsPerOpen pointer.  Otherwise,
 *          return NULL.
 */
void *nvKmsOpen(
    NvU32 pid,
    enum NvKmsClientType clientType,
    nvkms_per_open_handle_t *pOpenKernel)
{
    struct NvKmsPerOpen *pOpen = nvCalloc(1, sizeof(*pOpen));

    if (pOpen == NULL) {
        goto fail;
    }

    pOpen->pid = pid;
    pOpen->clientType = clientType;
    pOpen->type = NvKmsPerOpenTypeUndefined;
    pOpen->pOpenKernel = pOpenKernel;

    nvListAppend(&pOpen->perOpenListEntry, &perOpenList);

    return pOpen;

fail:
    nvKmsClose(pOpen);
    return NULL;
}

extern const char *const pNV_KMS_ID;

#if NVKMS_PROCFS_ENABLE

static const char *ProcFsPerOpenTypeString(
    enum NvKmsPerOpenType type)
{
    switch (type) {
    case NvKmsPerOpenTypeIoctl:            return "ioctl";
    case NvKmsPerOpenTypeGrantSurface:     return "grantSurface";
    case NvKmsPerOpenTypeGrantSwapGroup:   return "grantSwapGroup";
    case NvKmsPerOpenTypeGrantPermissions: return "grantPermissions";
    case NvKmsPerOpenTypeUnicastEvent:     return "unicastEvent";
    case NvKmsPerOpenTypeUndefined:        return "undefined";
    }

    return "unknown";
}

static const char *ProcFsPerOpenClientTypeString(
    enum NvKmsClientType clientType)
{
    switch (clientType) {
    case NVKMS_CLIENT_USER_SPACE:   return "user-space";
    case NVKMS_CLIENT_KERNEL_SPACE: return "kernel-space";
    }

    return "unknown";
}

static const char *ProcFsPermissionsTypeString(
    enum NvKmsPermissionsType permissionsType)
{
    switch (permissionsType) {
    case NV_KMS_PERMISSIONS_TYPE_FLIPPING: return "flipping";
    case NV_KMS_PERMISSIONS_TYPE_MODESET:  return "modeset";
    }

    return "unknown";
}

static void
ProcFsPrintClients(
    void *data,
    char *buffer,
    size_t size,
    nvkms_procfs_out_string_func_t *outString)
{
    struct NvKmsPerOpen *pOpen;
    NVEvoInfoStringRec infoString;

    nvListForEachEntry(pOpen, &perOpenList, perOpenListEntry) {

        const char *extra = "";

        nvInitInfoString(&infoString, buffer, size);

        if (pOpen == nvEvoGlobal.nvKmsPerOpen) {
            extra = " (NVKMS-internal client)";
        }

        nvEvoLogInfoString(&infoString,
            "Client (pOpen)              : %p", pOpen);
        nvEvoLogInfoString(&infoString,
            "  pid                       : %d%s", pOpen->pid, extra);
        nvEvoLogInfoString(&infoString,
            "  clientType                : %s",
            ProcFsPerOpenClientTypeString(pOpen->clientType));
        nvEvoLogInfoString(&infoString,
            "  type                      : %s",
            ProcFsPerOpenTypeString(pOpen->type));

        if (pOpen->type == NvKmsPerOpenTypeIoctl) {

            NvKmsGenericHandle deviceHandle;
            struct NvKmsPerOpenDev *pOpenDev;

            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                                pOpenDev, deviceHandle) {
                NVDevEvoPtr pDevEvo = pOpenDev->pDevEvo;

                nvEvoLogInfoString(&infoString,
                    "  pDevEvo (deviceId:%02d)     : %p",
                    pDevEvo->deviceId, pDevEvo);
                nvEvoLogInfoString(&infoString,
                    "    NvKmsDeviceHandle       : %d", deviceHandle);
            }

        } else if (pOpen->type == NvKmsPerOpenTypeGrantSurface) {

            NVSurfaceEvoPtr pSurfaceEvo = pOpen->grantSurface.pSurfaceEvo;

            nvEvoLogInfoString(&infoString,
                "  pSurfaceEvo               : %p", pSurfaceEvo);

        } else if (pOpen->type == NvKmsPerOpenTypeGrantPermissions) {

            NVDevEvoPtr pDevEvo = pOpen->grantPermissions.pDevEvo;
            const struct NvKmsPermissions *pPerms =
                &pOpen->grantPermissions.permissions;

            nvEvoLogInfoString(&infoString,
                "  pDevEvo (deviceId:%02d)     : %p",
                pDevEvo->deviceId, pDevEvo);

            nvEvoLogInfoString(&infoString,
                "  PermissionsType            : %s",
                ProcFsPermissionsTypeString(pPerms->type));

            if (pPerms->type == NV_KMS_PERMISSIONS_TYPE_FLIPPING) {
                NvU32 d, h;

                for (d = 0; d < ARRAY_LEN(pPerms->flip.disp); d++) {
                    for (h = 0; h < ARRAY_LEN(pPerms->flip.disp[d].head); h++) {

                        const NvU8 layerMask =
                            pPerms->flip.disp[d].head[h].layerMask;

                        if (layerMask == 0) {
                            continue;
                        }

                        nvEvoLogInfoString(&infoString,
                            "    disp:%02d, head:%02d        : 0x%08x", d, h,
                            layerMask);
                    }
                }
            } else if (pPerms->type == NV_KMS_PERMISSIONS_TYPE_MODESET) {
                NvU32 d, h;

                for (d = 0; d < ARRAY_LEN(pPerms->flip.disp); d++) {
                    for (h = 0; h < ARRAY_LEN(pPerms->flip.disp[d].head); h++) {

                        NVDpyIdList dpyIdList =
                            pPerms->modeset.disp[d].head[h].dpyIdList;
                        NVDispEvoPtr pDispEvo;
                        char *dpys;

                        if (nvDpyIdListIsEmpty(dpyIdList)) {
                            continue;
                        }

                        pDispEvo = pDevEvo->pDispEvo[d];

                        dpys = nvGetDpyIdListStringEvo(pDispEvo, dpyIdList);

                        if (dpys == NULL) {
                            continue;
                        }

                        nvEvoLogInfoString(&infoString,
                            "    disp:%02d, head:%02d        : %s", d, h, dpys);

                        nvFree(dpys);
                    }
                }
            }
        } else if (pOpen->type == NvKmsPerOpenTypeGrantSwapGroup) {

            NVDevEvoPtr pDevEvo = pOpen->grantSwapGroup.pDevEvo;

            nvEvoLogInfoString(&infoString,
                "  pDevEvo (deviceId:%02d)     : %p",
                pDevEvo->deviceId, pDevEvo);
            nvEvoLogInfoString(&infoString,
                "  pSwapGroup                : %p",
                pOpen->grantSwapGroup.pSwapGroup);

        } else if (pOpen->type == NvKmsPerOpenTypeUnicastEvent) {

            nvEvoLogInfoString(&infoString,
                "  pDeferredRequestFifo      : %p",
                pOpen->unicastEvent.pDeferredRequestFifo);
        }

        nvEvoLogInfoString(&infoString, "");
        outString(data, buffer);
    }
}

static void PrintSurfacePlanes(
    NVEvoInfoStringRec *pInfoString,
    const NVSurfaceEvoRec *pSurfaceEvo)
{
    NvU8 planeIndex;

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {
        nvEvoLogInfoString(pInfoString,
            "plane[%u] disp ctxDma:0x%08x pitch:%u offset:%" NvU64_fmtu
            " rmObjectSizeInBytes:%" NvU64_fmtu,
            planeIndex,
            pSurfaceEvo->planes[planeIndex].ctxDma,
            pSurfaceEvo->planes[planeIndex].pitch,
            pSurfaceEvo->planes[planeIndex].offset,
            pSurfaceEvo->planes[planeIndex].rmObjectSizeInBytes);
    }
}

static void PrintSurfaceClients(
    NVEvoInfoStringRec *pInfoString,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const NVDevEvoRec *pDevEvo)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {
        NvKmsGenericHandle deviceHandle;
        struct NvKmsPerOpenDev *pOpenDev;

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                            pOpenDev, deviceHandle) {
            NvKmsGenericHandle surfaceHandle;
            NVSurfaceEvoPtr pTmpSurfaceEvo;

            if (pOpenDev->pDevEvo != pDevEvo) {
                continue;
            }

            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->surfaceHandles,
                                                pTmpSurfaceEvo, surfaceHandle) {
                if (pTmpSurfaceEvo != pSurfaceEvo) {
                    continue;
                }

                nvEvoLogInfoString(pInfoString,
                    "   pOpen                    : %p", pOpen);
                nvEvoLogInfoString(pInfoString,
                    "    pOpenDev                : %p", pOpenDev);
                nvEvoLogInfoString(pInfoString,
                    "    NvKmsSurfaceHandle      : %d", surfaceHandle);
            }
        }
    }
}

static void PrintSurface(
    NVEvoInfoStringRec *pInfoString,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const NVDevEvoRec *pDevEvo)
{
    NvU32 sd;

    nvEvoLogInfoString(pInfoString,
        "pSurfaceEvo                 : %p", pSurfaceEvo);
    nvEvoLogInfoString(pInfoString,
        "  pDevEvo (deviceId:%02d)     : %p", pDevEvo->deviceId, pDevEvo);
    nvEvoLogInfoString(pInfoString,
        "  owner                     : "
        "pOpenDev:%p, NvKmsSurfaceHandle:%d",
        pSurfaceEvo->owner.pOpenDev,
        pSurfaceEvo->owner.surfaceHandle);
    nvEvoLogInfoString(pInfoString,
        "  {width,height}InPixels    : %d x %d",
        pSurfaceEvo->widthInPixels,
        pSurfaceEvo->heightInPixels);
    nvEvoLogInfoString(pInfoString,
        "  misc                      : "
        "log2GobsPerBlockY:%d",
        pSurfaceEvo->log2GobsPerBlockY);
    nvEvoLogInfoString(pInfoString,
        "  memory                    : layout:%s format:%s",
        NvKmsSurfaceMemoryLayoutToString(pSurfaceEvo->layout),
        nvKmsSurfaceMemoryFormatToString(pSurfaceEvo->format));
    nvEvoLogInfoString(pInfoString,
        "  refCnts                   : "
        "rmRefCnt:%" NvU64_fmtx" structRefCnt:%" NvU64_fmtx,
        pSurfaceEvo->rmRefCnt,
        pSurfaceEvo->structRefCnt);

    PrintSurfacePlanes(pInfoString, pSurfaceEvo);

    nvEvoLogInfoString(pInfoString,
        "  clients                   :");

    PrintSurfaceClients(pInfoString, pSurfaceEvo, pDevEvo);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (pSurfaceEvo->cpuAddress[sd] != NULL) {
            nvEvoLogInfoString(pInfoString,
                "  cpuAddress[%02d]            : %p",
                sd, pSurfaceEvo->cpuAddress[sd]);
        }
    }

    nvEvoLogInfoString(pInfoString, "");
}

static void
ProcFsPrintSurfaces(
    void *data,
    char *buffer,
    size_t size,
    nvkms_procfs_out_string_func_t *outString)
{
    struct NvKmsPerOpen *pOpen;
    NVEvoInfoStringRec infoString;
    NvU32 i;

    for (i = 0; i < 2; i++) {

        nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {
            NvKmsGenericHandle deviceHandle;
            struct NvKmsPerOpenDev *pOpenDev;

            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                                pOpenDev, deviceHandle) {

                NvKmsGenericHandle surfaceHandle;
                NVSurfaceEvoPtr pSurfaceEvo;

                FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->surfaceHandles,
                                                    pSurfaceEvo,
                                                    surfaceHandle) {
                    /*
                     * Because clients can grant surfaces between each
                     * other, a pSurfaceEvo could be in multiple clients'
                     * lists.  So, we loop over all surfaces on all clients
                     * twice: the first time we print unique surfaces and set
                     * 'procFsFlag' to recognize duplicates.  The second time,
                     * we clear 'procFsFlag'.
                     */
                    if (i == 0) {
                        if (pSurfaceEvo->procFsFlag) {
                            continue;
                        }

                        nvInitInfoString(&infoString, buffer, size);
                        PrintSurface(&infoString, pSurfaceEvo,
                                     pOpenDev->pDevEvo);
                        outString(data, buffer);

                        pSurfaceEvo->procFsFlag = TRUE;
                    } else {
                        pSurfaceEvo->procFsFlag = FALSE;
                    }
                }
            }
        }
    }
}

static const char *SwapGroupPerEyeStereoString(const NvU32 request)
{
    const NvU32 value =
        DRF_VAL(KMS, _DEFERRED_REQUEST,
                _SWAP_GROUP_READY_PER_EYE_STEREO, request);

    switch (value) {

    case NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO_PER_PAIR:
        return "PerPair";
    case NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO_PER_EYE:
        return "PerEye";
    }

    return "Unknown";
}

static void ProcFsPrintOneDeferredRequestFifo(
    void *data,
    char *buffer,
    size_t size,
    nvkms_procfs_out_string_func_t *outString,
    const NVDeferredRequestFifoRec *pDeferredRequestFifo,
    const struct NvKmsPerOpen *pOpen,
    const struct NvKmsPerOpenDev *pOpenDev,
    const NvKmsDeferredRequestFifoHandle deferredRequestFifoHandle)
{
    NVEvoInfoStringRec infoString;

    const struct NvKmsDeferredRequestFifo *fifo = pDeferredRequestFifo->fifo;
    NvU32 i, prevI;

    nvInitInfoString(&infoString, buffer, size);

    nvEvoLogInfoString(&infoString,
        "pDeferredRequestFifo        : %p", pDeferredRequestFifo);

    nvEvoLogInfoString(&infoString,
        "  Client (pOpen)            : %p", pOpen);

    nvEvoLogInfoString(&infoString,
        "    pOpenDev                : %p", pOpenDev);

    nvEvoLogInfoString(&infoString,
        "  pSurfaceEvo               : %p", pDeferredRequestFifo->pSurfaceEvo);

    nvEvoLogInfoString(&infoString,
        "  NvKms...RequestFifoHandle : %d", deferredRequestFifoHandle);

    if (pDeferredRequestFifo->swapGroup.pSwapGroup != NULL) {

        nvEvoLogInfoString(&infoString,
            "  swapGroup                 :");
        nvEvoLogInfoString(&infoString,
            "    pSwapGroup              : %p",
            pDeferredRequestFifo->swapGroup.pSwapGroup);
        nvEvoLogInfoString(&infoString,
            "    pOpenUnicastEvent       : %p",
            pDeferredRequestFifo->swapGroup.pOpenUnicastEvent);
        nvEvoLogInfoString(&infoString,
            "    ready                   : %d",
            pDeferredRequestFifo->swapGroup.ready);
        nvEvoLogInfoString(&infoString,
            "    semaphoreIndex          : 0x%02x",
            pDeferredRequestFifo->swapGroup.semaphoreIndex);
    }

    nvEvoLogInfoString(&infoString,
        "  put                       : %d", fifo->put);

    nvEvoLogInfoString(&infoString,
        "  get                       : %d", fifo->get);

    outString(data, buffer);

    for (i = 0; i < ARRAY_LEN(fifo->request); i++) {

        const NvU32 request = fifo->request[i];
        const NvU32 opcode = DRF_VAL(KMS, _DEFERRED_REQUEST, _OPCODE, request);
        const NvU32 semaphoreIndex =
            DRF_VAL(KMS, _DEFERRED_REQUEST, _SEMAPHORE_INDEX, request);

        switch (opcode) {

        case NVKMS_DEFERRED_REQUEST_OPCODE_NOP:
            break;

        case NVKMS_DEFERRED_REQUEST_OPCODE_SWAP_GROUP_READY:
            nvInitInfoString(&infoString, buffer, size);
            nvEvoLogInfoString(&infoString,
                "  request[0x%02x]             : "
                "opcode:SWAP_GROUP_READY, semaphoreIndex:0x%02x, "
                "perEyeStereo:%s",
                i, semaphoreIndex,
                SwapGroupPerEyeStereoString(request));
            outString(data, buffer);
            break;

        default:
            nvInitInfoString(&infoString, buffer, size);
            nvEvoLogInfoString(&infoString,
                "  request[0x%02x]             : opcode:INVALID", i);
            outString(data, buffer);
            break;
        }
    }

    /*
     * Print the fifo->semaphore[] array, but collapse multiple lines with
     * duplicate values.
     *
     * To collapse duplicates, loop over all semaphore[] elements.  If the
     * current element is the same as semaphore[prev], continue.  If they
     * differ, print the value in semaphore[prev .. i-1], and update prev.
     */
    prevI = 0;

    for (i = 1; i <= ARRAY_LEN(fifo->semaphore); i++) {

        const NvU32 prevValue = fifo->semaphore[prevI].data[0];

        if (i != ARRAY_LEN(fifo->semaphore)) {
            const NvU32 currentValue = fifo->semaphore[i].data[0];

            /*
             * If the value in this element matches the previous element, don't
             * print anything, yet.
             */
            if (currentValue == prevValue) {
                continue;
            }
        }

        nvInitInfoString(&infoString, buffer, size);

        if (prevI == (i - 1)) {
            nvEvoLogInfoString(&infoString,
                "  semaphore[0x%02x]           : 0x%08x",
                prevI, prevValue);
        } else {
            nvEvoLogInfoString(&infoString,
                "  semaphore[0x%02x..0x%02x]     : 0x%08x",
                prevI, i - 1, prevValue);
        }

        outString(data, buffer);

        prevI = i;
    }

    nvInitInfoString(&infoString, buffer, size);
    nvEvoLogInfoString(&infoString, "");
    outString(data, buffer);
}

static void
ProcFsPrintDeferredRequestFifos(
    void *data,
    char *buffer,
    size_t size,
    nvkms_procfs_out_string_func_t *outString)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {

        struct NvKmsPerOpenDev *pOpenDev;
        NvKmsGenericHandle devHandle;

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(
            &pOpen->ioctl.devHandles,
            pOpenDev, devHandle) {

            NVDeferredRequestFifoRec *pDeferredRequestFifo;
            NvKmsGenericHandle fifoHandle;

            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(
                &pOpenDev->deferredRequestFifoHandles,
                pDeferredRequestFifo, fifoHandle) {

                ProcFsPrintOneDeferredRequestFifo(
                    data, buffer, size, outString,
                    pDeferredRequestFifo,
                    pOpen,
                    pOpenDev,
                    fifoHandle);
            }
        }
    }
}

static void
ProcFsPrintDpyCrcs(
    void *data,
    char *buffer,
    size_t size,
    nvkms_procfs_out_string_func_t *outString)
{
    NVDevEvoPtr pDevEvo;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex, head;
    NVEvoInfoStringRec infoString;

    FOR_ALL_EVO_DEVS(pDevEvo) {

        nvInitInfoString(&infoString, buffer, size);
        nvEvoLogInfoString(&infoString,
                           "pDevEvo (deviceId:%02d)         : %p",
                           pDevEvo->deviceId, pDevEvo);
        outString(data, buffer);

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

            nvInitInfoString(&infoString, buffer, size);
            nvEvoLogInfoString(&infoString,
                               " pDispEvo (dispIndex:%02d)      : %p",
                               dispIndex, pDispEvo);
            outString(data, buffer);

            for (head = 0; head < pDevEvo->numHeads; head++) {
                const NVDispHeadStateEvoRec *pHeadState =
                                            &pDispEvo->headState[head];
                struct NvKmsDpyCRC32 compCrc;
                struct NvKmsDpyCRC32 rgCrc;
                struct NvKmsDpyCRC32 outputCrc;
                CRC32NotifierCrcOut crcOut;
                crcOut.compositorCrc32 = &compCrc;
                crcOut.rasterGeneratorCrc32 = &rgCrc;
                crcOut.outputCrc32 = &outputCrc;

                if (pHeadState->pConnectorEvo == NULL) {
                    continue;
                }

                nvInitInfoString(&infoString, buffer, size);
                if (nvReadCRC32Evo(pDispEvo, head, &crcOut)) {
                    nvEvoLogInfoString(&infoString,
                                       "  head %d                      :",
                                       head);
                    if (compCrc.supported) {
                        nvEvoLogInfoString(&infoString,
                                           "   compositor CRC             : 0x%08x",
                                           compCrc.value);
                    } else {
                        nvEvoLogInfoString(&infoString,
                                           "   compositor CRC             : unsupported");
                    }
                    if (rgCrc.supported) {
                        nvEvoLogInfoString(&infoString,
                                           "   raster generator CRC       : 0x%08x",
                                           rgCrc.value);
                    } else {
                        nvEvoLogInfoString(&infoString,
                                           "   raster generator CRC       : unsupported");
                    }
                    if (outputCrc.supported) {
                        nvEvoLogInfoString(&infoString,
                                           "   output CRC                 : 0x%08x",
                                           outputCrc.value);
                    } else {
                        nvEvoLogInfoString(&infoString,
                                           "   output CRC                 : unsupported");
                    }
                } else {
                    nvEvoLogInfoString(&infoString,
                                       "  head %d                      : error",
                                       head);
                }
                outString(data, buffer);
            }
        }
    }
}

#endif /* NVKMS_PROCFS_ENABLE */

void nvKmsGetProcFiles(const nvkms_procfs_file_t **ppProcFiles)
{
#if NVKMS_PROCFS_ENABLE
    static const nvkms_procfs_file_t procFiles[] = {
        { "clients",                ProcFsPrintClients },
        { "surfaces",               ProcFsPrintSurfaces },
        { "deferred-request-fifos", ProcFsPrintDeferredRequestFifos },
        { "crcs",                   ProcFsPrintDpyCrcs },
        { NULL, NULL },
    };

    *ppProcFiles = procFiles;
#else
    *ppProcFiles = NULL;
#endif
}

static void FreeGlobalState(void)
{
    nvKmsClose(nvEvoGlobal.nvKmsPerOpen);
    nvEvoGlobal.nvKmsPerOpen = NULL;

    if (nvEvoGlobal.clientHandle != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle, nvEvoGlobal.clientHandle,
                    nvEvoGlobal.clientHandle);
        nvEvoGlobal.clientHandle = 0;
    }
}

NvBool nvKmsModuleLoad(void)
{
    NvU32 ret = NVOS_STATUS_ERROR_GENERIC;

    nvEvoLog(EVO_LOG_INFO, "Loading %s", pNV_KMS_ID);

    ret = nvRmApiAlloc(NV01_NULL_OBJECT,
                       NV01_NULL_OBJECT,
                       NV01_NULL_OBJECT,
                       NV01_ROOT,
                       &nvEvoGlobal.clientHandle);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLog(EVO_LOG_ERROR, "Failed to initialize client");
        goto fail;
    }

    nvEvoGlobal.nvKmsPerOpen = nvKmsOpen(0, NVKMS_CLIENT_KERNEL_SPACE, NULL);
    if (!nvEvoGlobal.nvKmsPerOpen) {
        nvEvoLog(EVO_LOG_ERROR, "Failed to initialize internal modeset client");
        goto fail;
    }

    if (!AssignNvKmsPerOpenType(nvEvoGlobal.nvKmsPerOpen,
                                NvKmsPerOpenTypeIoctl, FALSE)) {
        goto fail;
    }

    return TRUE;
fail:
    FreeGlobalState();

    return FALSE;
}


void nvKmsModuleUnload(void)
{
    FreeGlobalState();

    nvAssert(nvListIsEmpty(&nvEvoGlobal.frameLockList));
    nvAssert(nvListIsEmpty(&nvEvoGlobal.devList));
#if defined(DEBUG)
    nvReportUnfreedAllocations();
#endif
    nvEvoLog(EVO_LOG_INFO, "Unloading");
}


static void SendEvent(struct NvKmsPerOpen *pOpen,
                      const struct NvKmsEvent *pEvent)
{
    struct NvKmsPerOpenEventListEntry *pEntry = nvAlloc(sizeof(*pEntry));

    nvAssert(pOpen->type == NvKmsPerOpenTypeIoctl);

    if (pEntry == NULL) {
        return;
    }

    pEntry->event = *pEvent;
    nvListAppend(&pEntry->eventListEntry, &pOpen->ioctl.eventList);

    nvkms_event_queue_changed(pOpen->pOpenKernel, TRUE);
}

static void ConsoleRestoreTimerFired(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;

    if (pDevEvo->modesetOwner == NULL && pDevEvo->handleConsoleHotplugs) {
        pDevEvo->skipConsoleRestore = FALSE;
        nvEvoRestoreConsole(pDevEvo, TRUE /* allowMST */);
    }
}

/*!
 * Generate a dpy event.
 *
 * \param[in]  pDpyEvo    The dpy for which the event should be generated.
 * \param[in]  eventType  The NVKMS_EVENT_TYPE_
 * \param[in]  attribute  The NvKmsDpyAttribute; only used for
 *                        NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED.
 * \param[in]  NvS64      The NvKmsDpyAttribute value; only used for
 *                        NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED.
 */
static void SendDpyEventEvo(const NVDpyEvoRec *pDpyEvo,
                            const NvU32 eventType,
                            const enum NvKmsDpyAttribute attribute,
                            const NvS64 value)
{
    struct NvKmsPerOpen *pOpen;
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {

        struct NvKmsEvent event = { 0 };
        NvKmsDeviceHandle deviceHandle;
        NvKmsDispHandle dispHandle;

        if (!DispEvoToDevAndDispHandles(pOpen, pDispEvo,
                                        &deviceHandle, &dispHandle)) {
            continue;
        }

        if ((pOpen->ioctl.eventInterestMask & NVBIT(eventType)) == 0) {
            continue;
        }

        event.eventType = eventType;

        switch (eventType) {

        case NVKMS_EVENT_TYPE_DPY_CHANGED:
            event.u.dpyChanged.deviceHandle = deviceHandle;
            event.u.dpyChanged.dispHandle = dispHandle;
            event.u.dpyChanged.dpyId = pDpyEvo->id;
            break;

        case NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED:
            event.u.dynamicDpyConnected.deviceHandle = deviceHandle;
            event.u.dynamicDpyConnected.dispHandle = dispHandle;
            event.u.dynamicDpyConnected.dpyId = pDpyEvo->id;
            break;

        case NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED:
            event.u.dynamicDpyDisconnected.deviceHandle = deviceHandle;
            event.u.dynamicDpyDisconnected.dispHandle = dispHandle;
            event.u.dynamicDpyDisconnected.dpyId = pDpyEvo->id;
            break;

        case NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED:
            event.u.dpyAttributeChanged.deviceHandle = deviceHandle;
            event.u.dpyAttributeChanged.dispHandle = dispHandle;
            event.u.dpyAttributeChanged.dpyId = pDpyEvo->id;
            event.u.dpyAttributeChanged.attribute = attribute;
            event.u.dpyAttributeChanged.value = value;
            break;

        default:
            nvAssert(!"Bad eventType");
            return;
        }

        SendEvent(pOpen, &event);
    }

    if (eventType == NVKMS_EVENT_TYPE_DPY_CHANGED) {
        NVDevEvoPtr pDevEvo = pDpyEvo->pDispEvo->pDevEvo;

        if (pDevEvo->modesetOwner == NULL && pDevEvo->handleConsoleHotplugs) {
            nvkms_free_timer(pDevEvo->consoleRestoreTimer);
            pDevEvo->consoleRestoreTimer =
                nvkms_alloc_timer(ConsoleRestoreTimerFired, pDevEvo, 0, 500);
        }
    }
}

void nvSendDpyEventEvo(const NVDpyEvoRec *pDpyEvo, const NvU32 eventType)
{
    nvAssert(eventType != NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED);
    SendDpyEventEvo(pDpyEvo, eventType,
                    0 /* attribute (unused) */,
                    0 /* value (unused) */ );
}

void nvSendDpyAttributeChangedEventEvo(const NVDpyEvoRec *pDpyEvo,
                                       const enum NvKmsDpyAttribute attribute,
                                       const NvS64 value)
{
    SendDpyEventEvo(pDpyEvo,
                    NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED,
                    attribute, value);
}

void nvSendFrameLockAttributeChangedEventEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    const enum NvKmsFrameLockAttribute attribute,
    const NvS64 value)
{
    struct NvKmsPerOpen *pOpen;
    const NvU32 eventType = NVKMS_EVENT_TYPE_FRAMELOCK_ATTRIBUTE_CHANGED;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {

        struct NvKmsEvent event = { 0 };
        NvKmsFrameLockHandle frameLockHandle;

        if ((pOpen->ioctl.eventInterestMask & NVBIT(eventType)) == 0) {
            continue;
        }

        if (!FrameLockEvoToFrameLockHandle(pOpen, pFrameLockEvo,
                                           &frameLockHandle)) {
            continue;
        }

        event.eventType = eventType;
        event.u.frameLockAttributeChanged.frameLockHandle = frameLockHandle;
        event.u.frameLockAttributeChanged.attribute = attribute;
        event.u.frameLockAttributeChanged.value = value;

        SendEvent(pOpen, &event);
    }
}


void nvSendFlipOccurredEventEvo(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelMask channelMask)
{
    struct NvKmsPerOpen *pOpen;
    const NvU32 eventType = NVKMS_EVENT_TYPE_FLIP_OCCURRED;
    const NvU32 dispIndex = 0; /* XXX NVKMS TODO: need disp-scope in event */
    const NVDispEvoRec *pDispEvo = pDevEvo->pDispEvo[dispIndex];
    NvU32 head, layer;

    nvAssert(NV_EVO_CHANNEL_MASK_POPCOUNT(channelMask) == 1);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            if (pDevEvo->head[head].layer[layer]->channelMask == channelMask) {
                break;
            }
        }

        if (layer < pDevEvo->head[head].numLayers) {
            break;
        }
    }

    if (head >= pDevEvo->numHeads) {
        nvAssert(!"Bad channelMask");
        return;
    }

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {

        struct NvKmsEvent event = { 0 };
        NvKmsDeviceHandle deviceHandle;
        NvKmsDispHandle dispHandle;

        struct NvKmsPerOpenDev *pOpenDev;
        const struct NvKmsFlipPermissions *pFlipPermissions;

        pOpenDev = DevEvoToOpenDev(pOpen, pDevEvo);

        if (pOpenDev == NULL) {
            continue;
        }

        if ((pOpen->ioctl.eventInterestMask & NVBIT(eventType)) == 0) {
            continue;
        }

        pFlipPermissions = &pOpenDev->flipPermissions;

        if ((pFlipPermissions->disp[dispIndex].head[head].layerMask &
             NVBIT(layer)) == 0x0) {
            continue;
        }

        if (!DispEvoToDevAndDispHandles(pOpen, pDispEvo,
                                        &deviceHandle, &dispHandle)) {
            continue;
        }

        event.eventType = eventType;
        event.u.flipOccurred.deviceHandle = deviceHandle;
        event.u.flipOccurred.dispHandle = dispHandle;
        event.u.flipOccurred.head = head;
        event.u.flipOccurred.layer = layer;

        SendEvent(pOpen, &event);
    }
}

void nvSendUnicastEvent(struct NvKmsPerOpen *pOpen)
{
    if (pOpen == NULL) {
        return;
    }

    nvAssert(pOpen->type == NvKmsPerOpenTypeUnicastEvent);

    nvkms_event_queue_changed(pOpen->pOpenKernel, TRUE);
}

void nvRemoveUnicastEvent(struct NvKmsPerOpen *pOpen)
{
    NVDeferredRequestFifoPtr pDeferredRequestFifo;

    if (pOpen == NULL) {
        return;
    }

    nvAssert(pOpen->type == NvKmsPerOpenTypeUnicastEvent);

    pDeferredRequestFifo = pOpen->unicastEvent.pDeferredRequestFifo;

    if (pDeferredRequestFifo != NULL) {
        pDeferredRequestFifo->swapGroup.pOpenUnicastEvent = NULL;
        pOpen->unicastEvent.pDeferredRequestFifo = NULL;
    }
}

static void AllocSurfaceCtxDmasForAllOpens(NVDevEvoRec *pDevEvo)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {
        struct NvKmsPerOpenDev *pOpenDev = DevEvoToOpenDev(pOpen, pDevEvo);

        NvKmsGenericHandle surfaceHandle;
        NVSurfaceEvoPtr pSurfaceEvo;

        if (pOpenDev == NULL) {
            continue;
        }

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->surfaceHandles,
                                            pSurfaceEvo, surfaceHandle) {

            NvU8 planeIndex;

            if (!nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle)) {
                continue;
            }

            if (!pSurfaceEvo->requireCtxDma) {
                nvAssert(pSurfaceEvo->planes[0].ctxDma == 0);
                continue;
            }

            /*
             * Orphan surfaces should not get this far: they should
             * fail the owner check above.
             */
            nvAssert(pSurfaceEvo->rmRefCnt > 0);

            FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {

                pSurfaceEvo->planes[planeIndex].ctxDma =
                    nvRmEvoAllocateAndBindDispContextDMA(
                    pDevEvo,
                    pSurfaceEvo->planes[planeIndex].rmHandle,
                    pSurfaceEvo->layout,
                    pSurfaceEvo->planes[planeIndex].rmObjectSizeInBytes - 1);
                if (!pSurfaceEvo->planes[planeIndex].ctxDma) {
                    FreeSurfaceCtxDmasForAllOpens(pDevEvo);
                    nvAssert(!"Failed to re-allocate surface ctx dma");
                    return;
                }
            }
        }
    }
}


static void FreeSurfaceCtxDmasForAllOpens(NVDevEvoRec *pDevEvo)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {
        struct NvKmsPerOpenDev *pOpenDev = DevEvoToOpenDev(pOpen, pDevEvo);

        NvKmsGenericHandle surfaceHandle;
        NVSurfaceEvoPtr pSurfaceEvo;

        if (pOpenDev == NULL) {
            continue;
        }

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->surfaceHandles,
                                            pSurfaceEvo, surfaceHandle) {

            NvU8 planeIndex;

            if (!nvEvoIsSurfaceOwner(pSurfaceEvo, pOpenDev, surfaceHandle)) {
                continue;
            }

            /*
             * Orphan surfaces should not get this far: they should
             * fail the owner check above.
             */
            nvAssert(pSurfaceEvo->rmRefCnt > 0);

            if (!pSurfaceEvo->requireCtxDma) {
                nvAssert(pSurfaceEvo->planes[0].ctxDma == 0);
                continue;
            }

            FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {
                nvRmEvoFreeDispContextDMA(
                    pDevEvo,
                    &pSurfaceEvo->planes[planeIndex].ctxDma);
            }
        }
    }
}

#if defined(DEBUG)
NvBool nvSurfaceEvoInAnyOpens(const NVSurfaceEvoRec *pSurfaceEvo)
{
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenList, perOpenListEntry) {

        if (pOpen->type == NvKmsPerOpenTypeIoctl) {
            struct NvKmsPerOpenDev *pOpenDev;
            NvKmsGenericHandle dev;

            FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpen->ioctl.devHandles,
                                                pOpenDev, dev) {

                NvKmsGenericHandle surfaceHandleUnused;
                NVSurfaceEvoPtr pSurfaceEvoTmp;

                if (pOpenDev == NULL) {
                    continue;
                }

                FOR_ALL_POINTERS_IN_EVO_API_HANDLES(&pOpenDev->surfaceHandles,
                                                    pSurfaceEvoTmp,
                                                    surfaceHandleUnused) {
                    if (pSurfaceEvoTmp == pSurfaceEvo) {
                        return TRUE;
                    }
                }
            }
        } else if ((pOpen->type == NvKmsPerOpenTypeGrantSurface) &&
                   (pOpen->grantSurface.pSurfaceEvo == pSurfaceEvo)) {
            return TRUE;
        }
    }

    return FALSE;
}
#endif

NVDevEvoPtr nvGetDevEvoFromOpenDev(
    const struct NvKmsPerOpenDev *pOpenDev)
{
    nvAssert(pOpenDev != NULL);
    return pOpenDev->pDevEvo;
}

const struct NvKmsFlipPermissions *nvGetFlipPermissionsFromOpenDev(
    const struct NvKmsPerOpenDev *pOpenDev)
{
    nvAssert(pOpenDev != NULL);
    return &pOpenDev->flipPermissions;
}

const struct NvKmsModesetPermissions *nvGetModesetPermissionsFromOpenDev(
    const struct NvKmsPerOpenDev *pOpenDev)
{
    nvAssert(pOpenDev != NULL);
    return &pOpenDev->modesetPermissions;
}

NVEvoApiHandlesRec *nvGetSurfaceHandlesFromOpenDev(
    struct NvKmsPerOpenDev *pOpenDev)
{
    if (pOpenDev == NULL) {
        return NULL;
    }

    return &pOpenDev->surfaceHandles;
}

const NVEvoApiHandlesRec *nvGetSurfaceHandlesFromOpenDevConst(
    const struct NvKmsPerOpenDev *pOpenDev)
{
    if (pOpenDev == NULL) {
        return NULL;
    }

    return &pOpenDev->surfaceHandles;
}

static int suspendCounter = 0;

/*
 * Suspend NVKMS.
 *
 * This function is called by RM once per GPU, but NVKMS just counts the number
 * of suspend calls so that it can deallocate the core channels on the first
 * call to suspend(), and reallocate them on the last call to resume().
 */
void nvKmsSuspend(NvU32 gpuId)
{
    if (suspendCounter == 0) {
        NVDevEvoPtr pDevEvo;

        FOR_ALL_EVO_DEVS(pDevEvo) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_INFO, "Suspending");

            /*
             * Shut down all heads and skip console restore.
             *
             * This works around an RM bug where it fails to train DisplayPort
             * links during resume if the system was suspended while heads were
             * active.
             *
             * XXX TODO bug 1850734: In addition to fixing the above
             * RM bug, NVKMS should clear pDispEvo head and connector state
             * that becomes stale after suspend. Shutting the heads down here
             * clears the relevant state explicitly.
             */
            nvShutDownHeads(pDevEvo,
                            NULL /* pTestFunc, shut down all heads */);
            pDevEvo->skipConsoleRestore = TRUE;

            FreeSurfaceCtxDmasForAllOpens(pDevEvo);

            nvFreeCoreChannelEvo(pDevEvo);
        }
    }

    suspendCounter++;
}

void nvKmsResume(NvU32 gpuId)
{
    suspendCounter--;

    if (suspendCounter == 0) {
        NVDevEvoPtr pDevEvo;

        FOR_ALL_EVO_DEVS(pDevEvo) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_INFO, "Resuming");

            nvRestoreSORAssigmentsEvo(pDevEvo);

            ReallocCoreChannel(pDevEvo);

            if (pDevEvo->modesetOwner == NULL) {
                // Hardware state was lost, so we need to force a console
                // restore.
                pDevEvo->skipConsoleRestore = FALSE;
                RestoreConsole(pDevEvo);
            }
        }
    }
}

static void ServiceOneDeferredRequestFifo(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo)
{
    struct NvKmsDeferredRequestFifo *fifo = pDeferredRequestFifo->fifo;
    NvU32 get, put;

    nvAssert(fifo != NULL);

    get = fifo->get;
    put = fifo->put;

    if (put == get) {
        return;
    }

    if ((get >= ARRAY_LEN(fifo->request)) ||
        (put >= ARRAY_LEN(fifo->request))) {
        return;
    }

    while (get != put) {

        const NvU32 request = fifo->request[get];
        const NvU32 opcode =
            DRF_VAL(KMS, _DEFERRED_REQUEST, _OPCODE, request);

        switch (opcode) {

        case NVKMS_DEFERRED_REQUEST_OPCODE_NOP:
            break;

        default:
            nvAssert(!"Invalid NVKMS deferred request opcode");
            break;
        }

        get = (get + 1) % ARRAY_LEN(fifo->request);
    }

    fifo->get = put;
}

/*!
 * Respond to a non-stall interrupt.
 */
void nvKmsServiceNonStallInterrupt(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;
    struct NvKmsPerOpen *pOpen;

    nvListForEachEntry(pOpen, &perOpenIoctlList, perOpenIoctlListEntry) {

        struct NvKmsPerOpenDev *pOpenDev = DevEvoToOpenDev(pOpen, pDevEvo);
        NVDeferredRequestFifoRec *pDeferredRequestFifo;
        NvKmsGenericHandle handle;

        if (pOpenDev == NULL) {
            continue;
        }

        FOR_ALL_POINTERS_IN_EVO_API_HANDLES(
            &pOpenDev->deferredRequestFifoHandles,
            pDeferredRequestFifo,
            handle) {

            ServiceOneDeferredRequestFifo(pDevEvo, pDeferredRequestFifo);
        }
    }

}

NvBool nvKmsGetBacklight(NvU32 display_id, void *drv_priv, NvU32 *brightness)
{
    NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS params = { 0 };
    NV_STATUS status = NV_ERR_INVALID_STATE;
    NVDispEvoRec *pDispEvo = drv_priv;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = display_id;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            pDevEvo->displayCommonHandle,
                            NV0073_CTRL_CMD_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS,
                            &params, sizeof(params));

    if (status == NV_OK) {
        *brightness = params.brightness;
    }

    return status == NV_OK;
}

NvBool nvKmsSetBacklight(NvU32 display_id, void *drv_priv, NvU32 brightness)
{
    NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS params = { 0 };
    NV_STATUS status = NV_ERR_INVALID_STATE;
    NVDispEvoRec *pDispEvo = drv_priv;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId  = display_id;
    params.brightness = brightness;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            pDevEvo->displayCommonHandle,
                            NV0073_CTRL_CMD_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS,
                            &params, sizeof(params));

    return status == NV_OK;
}
