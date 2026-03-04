/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-framelock.h"
#include "nvkms-dpy.h"
#include "nvkms-utils.h"
#include "nvkms-evo.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"

#include "nvkms-private.h" /* nvSendDpyAttributeChangedEventEvo() */

#include <class/cl30f1.h>
#include <ctrl/ctrl0000/ctrl0000gsync.h> /* NV0000_CTRL_CMD_GSYNC_GET_ATTACHED_IDS */
#include <ctrl/ctrl30f1.h>
#include "nvos.h"

static NvBool FrameLockUseHouseSyncGetSupport(NVFrameLockEvoPtr pFrameLockEvo,
                                              NvU32 *val);
static NvBool FrameLockSetPolarity(
    NVFrameLockEvoPtr pFrameLockEvo,
    enum NvKmsFrameLockAttributePolarityValue val);
static NvBool HouseSyncOutputModeUsable(const NVFrameLockEvoRec *pFrameLockEvo);

/*!
 * Handle framelock sync gain/loss events triggered from resman.
 *
 * When RM sends an event notification that's handled by FrameLockEvent,
 * that function schedules a timer to service that event notification.
 * These timers are serviced out of order, though; we may receive a
 * SYNC_LOSS event followed by a SYNC_GAIN event, but our scheduled
 * callbacks may be called in the reverse order.
 *
 * Since we can't trust that events were serviced in order, this function
 * responds to every sync gain or loss event by querying the actual
 * sync status across all GPUs from RM and updating our cached sync status
 * and notifying clients if necessary.
 */
static void
FrameLockHandleSyncEvent(void *dataPtr, NvU32 dataU32)
{
    NVDispEvoPtr pDispEvo = dataPtr;
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NvU32 connectorIndex = pDispEvo->framelock.connectorIndex;
    NvBool syncReadyCurrent = FALSE;
    NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS statusParams = { 0 };

    statusParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SYNC,
                       &statusParams,
                       sizeof(statusParams)) != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to query gsync status after event");
    } else {
        if (statusParams.bTiming && statusParams.bSyncReady) {
            syncReadyCurrent = TRUE;
        }
    }

    // Update syncReadyGpuMask for consistency with non-NVKMS path, although
    // it is currently unused.
    if (syncReadyCurrent) {
        pFrameLockEvo->syncReadyGpuMask |= (1 << connectorIndex);
    } else {
        pFrameLockEvo->syncReadyGpuMask &= ~(1 << connectorIndex);
    }

    if (syncReadyCurrent != pFrameLockEvo->syncReadyLast) {
        pFrameLockEvo->syncReadyLast = syncReadyCurrent;
        nvSendFrameLockAttributeChangedEventEvo(
            pFrameLockEvo,
            NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY,
            pFrameLockEvo->syncReadyLast);
    }
}

/*!
 * Receive framelock events from resman.
 *
 * This function is registered as a kernel callback function from
 * resman.
 *
 * However, it is called with resman's context (resman locks held, etc).
 * Schedule deferred work, so that we can process the event without resman's
 * encumbrances.
 */
static void FrameLockEvent(void *arg, void *pEventDataVoid,
                           NvU32 hEvent,
                           NvU32 Data, NV_STATUS Status)
{
    static nvkms_timer_proc_t *callbackTable[] = {
        [NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(0)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(1)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(2)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(3)] = FrameLockHandleSyncEvent,

        [NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(0)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(1)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(2)] = FrameLockHandleSyncEvent,
        [NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(3)] = FrameLockHandleSyncEvent,
    };

    const NvNotification *pNotifyData = pEventDataVoid;
    NvU32 notifyIndex;

    /* callbackTable[] assumes at most four connectors per gsync */
    ct_assert(NV30F1_GSYNC_CONNECTOR_COUNT == 4);

    if (pNotifyData == NULL) {
        nvAssert(!"Invalid pNotifyData from resman");
        return;
    }

    notifyIndex = pNotifyData->info32;

    if ((notifyIndex >= ARRAY_LEN(callbackTable)) ||
        (callbackTable[notifyIndex] == NULL)) {
        nvAssert(!"Invalid notifyIndex from resman");
        return;
    }

    (void) nvkms_alloc_timer_with_ref_ptr(
        callbackTable[notifyIndex], /* callback */
        arg, /* argument (this is a ref_ptr to a pDispEvo) */
        0,   /* unused */
        0);  /* timeout (i.e., service as soon as possible) */
}

/*!
 * Free all events and handles allocated in FrameLockCreateEvents().
 */
static void FrameLockDestroyEvents(NVDispEvoPtr pDispEvo)
{
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    unsigned int i;

    if (pFrameLockEvo == NULL) {
        return;
    }

    for (i = 0; i < NV_FRAMELOCK_NUM_EVENTS; i++) {
        if (pDispEvo->framelock.gsyncEvent[i].handle) {
            nvRmApiFree(nvEvoGlobal.clientHandle,
                        pFrameLockEvo->device,
                        pDispEvo->framelock.gsyncEvent[i].handle);
            nvFreeUnixRmHandle(&pDispEvo->pDevEvo->handleAllocator,
                               pDispEvo->framelock.gsyncEvent[i].handle);
            pDispEvo->framelock.gsyncEvent[i].handle = 0;
        }
    }
}

/*!
 * Allocate and configure all events and handles associated with them.
 */
static NvBool FrameLockCreateEvents(NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    const NvU32 connectorIndex = pDispEvo->framelock.connectorIndex;
    unsigned int i;

    if (pDispEvo->pFrameLockEvo == NULL) {
        return TRUE;
    }

    nvAssert(connectorIndex < NV30F1_GSYNC_CONNECTOR_COUNT);

    /* We should only get here on hardware that has per-connector events */
    nvAssert(!(pFrameLockEvo->caps &
        NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ONLY_PRIMARY_CONNECTOR_EVENT));

    for (i = 0; i < NV_FRAMELOCK_NUM_EVENTS; i++) {
        NvU32 notifier;
        NvBool ret;

        switch (i) {
            case NV_FRAMELOCK_SYNC_LOSS:
                notifier = NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(connectorIndex);
                break;
            case NV_FRAMELOCK_SYNC_GAIN:
                notifier = NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(connectorIndex);
                break;
            default:
                nvAssert(!"Unknown gsync event index");
                continue;
        }

        pDispEvo->framelock.gsyncEvent[i].handle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        ret = TRUE;

        if (!nvRmRegisterCallback(pDevEvo,
                                  &pDispEvo->framelock.gsyncEvent[i].callback,
                                  pDispEvo->ref_ptr,
                                  pFrameLockEvo->device,
                                  pDispEvo->framelock.gsyncEvent[i].handle,
                                  FrameLockEvent,
                                  notifier)) {
            ret = FALSE;
        }

        if (!ret) {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                              "Failed to register for framelock event %d", i);
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pDispEvo->framelock.gsyncEvent[i].handle);
            pDispEvo->framelock.gsyncEvent[i].handle = 0;
            goto noEvents;
        }
    }

    return TRUE;

noEvents:

    nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                 "Failed to register for framelock events");

    FrameLockDestroyEvents(pDispEvo);

    return FALSE;
}

/*!
 * Bind a pSubDev to a pFrameLock.
 */
static void BindGpuToFrameLock(NVDevEvoPtr pDevEvo,
                               const NvU32 gpuId,
                               NVFrameLockEvoPtr pFrameLockEvo,
                               NvU32 connectorIndex)
{
    NVDispEvoPtr pDispEvo;
    unsigned int dispIndex;

    if (pFrameLockEvo->nGpuIds >= ARRAY_LEN(pFrameLockEvo->gpuIds)) {
        return;
    }

    pFrameLockEvo->gpuIds[pFrameLockEvo->nGpuIds] = gpuId;
    pFrameLockEvo->nGpuIds++;

    /*
     * If a disp exists for this subdevice, wire it up.
     * Note that this should not happen for SLI non-display-owners.
     */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        if (nvGpuIdOfDispEvo(pDispEvo) != gpuId) {
            continue;
        }

        pDispEvo->pFrameLockEvo = pFrameLockEvo;

        pDispEvo->framelock.connectorIndex = connectorIndex;

        pFrameLockEvo->connectedGpuMask |= (1 << connectorIndex);
        pFrameLockEvo->syncReadyGpuMask &= ~(1 << connectorIndex);

        /* Set up stereo synchronization events */
        FrameLockCreateEvents(pDispEvo);
    }
}

/*!
 * Break the binding of pSubDev and pDisp to pFrameLock that we
 * created in BindGpuToFrameLock().
 */
static void UnbindGpuFromFrameLock(NVDevEvoPtr pDevEvo,
                                   const NvU32 gpuId,
                                   NVFrameLockEvoPtr pFrameLockEvo)
{
    NVDispEvoPtr pDispEvo;
    unsigned int dispIndex;
    unsigned int gpu, j;

    for (gpu = 0; gpu < pFrameLockEvo->nGpuIds; gpu++) {
        if (pFrameLockEvo->gpuIds[gpu] == gpuId) {
            break;
        }
    }

    if (gpu == pFrameLockEvo->nGpuIds) {
        return;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        const NvU32 connectorIndex = pDispEvo->framelock.connectorIndex;

        if (nvGpuIdOfDispEvo(pDispEvo) != gpuId) {
            continue;
        }

        FrameLockDestroyEvents(pDispEvo);

        pFrameLockEvo->connectedGpuMask &= ~(1 << connectorIndex);
        pFrameLockEvo->syncReadyGpuMask &= ~(1 << connectorIndex);

        pDispEvo->framelock.connectorIndex = 0;

        pDispEvo->pFrameLockEvo = NULL;
    }

    for (j = gpu; j < (pFrameLockEvo->nGpuIds - 1); j++) {
        pFrameLockEvo->gpuIds[j] = pFrameLockEvo->gpuIds[j+1];
    }

    pFrameLockEvo->nGpuIds--;
}

/*!
 * Find the NVFrameLockEvoPtr with the specified gsyncId.
 */
static NVFrameLockEvoPtr FindFrameLock(NvU32 gsyncId)
{
    NVFrameLockEvoPtr pFrameLockEvo;

    FOR_ALL_EVO_FRAMELOCKS(pFrameLockEvo) {
        if (pFrameLockEvo->gsyncId == gsyncId) {
            return pFrameLockEvo;
        }
    }

    return NULL;
}

/*!
 * Return whether the NVDevEvoPtr contains a GPU with the specified gpuId.
 */
static NvBool GpuIdInDevEvo(NVDevEvoPtr pDevEvo, NvU32 gpuId)
{
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (pDevEvo->pSubDevices[sd]->gpuId == gpuId) {
            return TRUE;
        }
    }

    return FALSE;
}

/*!
 * Free the pFrameLock object.
 */
static void FreeFrameLockEvo(NVFrameLockEvoPtr pFrameLockEvo)
{
    if (pFrameLockEvo == NULL) {
        return;
    }

    if (pFrameLockEvo->device != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    nvEvoGlobal.clientHandle,
                    pFrameLockEvo->device);

        nvFreeUnixRmHandle(&pFrameLockEvo->handleAllocator,
                           pFrameLockEvo->device);
        pFrameLockEvo->device = 0;
    }

    nvAssert(pFrameLockEvo->nGpuIds == 0);

    nvTearDownUnixRmHandleAllocator(&pFrameLockEvo->handleAllocator);

    nvListDel(&pFrameLockEvo->frameLockListEntry);

    nvFree(pFrameLockEvo);
}

/*!
 * Allocate and initialize a new pFrameLock object.
 */
static NVFrameLockEvoPtr AllocFrameLockEvo(int instance, NvU32 gsyncId,
                                           NvBool *pBadFirmware)
{
    NV30F1_ALLOC_PARAMETERS gsyncAllocParams = { 0 };
    NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS gsyncGetCapsParams = { 0 };
    NVFrameLockEvoPtr pFrameLockEvo;

    nvAssert(FindFrameLock(gsyncId) == NULL);

    *pBadFirmware = FALSE;

    pFrameLockEvo = nvCalloc(1, sizeof(NVFrameLockEvoRec));

    if (pFrameLockEvo == NULL) {
        return NULL;
    }

    nvListInit(&pFrameLockEvo->frameLockListEntry);

    if (!nvInitUnixRmHandleAllocator(
            &pFrameLockEvo->handleAllocator,
            nvEvoGlobal.clientHandle,
            NVKMS_RM_HANDLE_SPACE_FRAMELOCK(instance))) {
        nvEvoLog(EVO_LOG_ERROR, "Failed to initialize framelock handles");
        goto fail;
    }

    pFrameLockEvo->device =
        nvGenerateUnixRmHandle(&pFrameLockEvo->handleAllocator);

    gsyncAllocParams.gsyncInstance = instance;

    /* allocate a framelock object for the framelock device */
    if (nvRmApiAlloc(nvEvoGlobal.clientHandle,
                     nvEvoGlobal.clientHandle,
                     pFrameLockEvo->device,
                     NV30_GSYNC,
                     &gsyncAllocParams) != NVOS_STATUS_SUCCESS) {
        pFrameLockEvo->device = 0;
        goto fail;
    }

    /* Store unique frame lock device ID */
    pFrameLockEvo->gsyncId = gsyncId;
    pFrameLockEvo->houseSyncUseable = 0;
    pFrameLockEvo->nGpuIds = 0;

    /* Initialize the state for the framelock board */
    pFrameLockEvo->polarity = NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_FALLING_EDGE;
    pFrameLockEvo->syncDelay = 0;
    pFrameLockEvo->syncInterval = 0;
    pFrameLockEvo->videoMode =
        NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_AUTO;
    pFrameLockEvo->testMode = FALSE;
    pFrameLockEvo->houseSyncMode =
        NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_DISABLED;
    pFrameLockEvo->mulDivValue = 1;
    pFrameLockEvo->mulDivMode = NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE_MULTIPLY;

    /* Query the framelock revision information */
    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GSYNC_GET_CAPS,
                       &gsyncGetCapsParams,
                       sizeof(gsyncGetCapsParams))
        != NVOS_STATUS_SUCCESS) {
        goto fail;
    }

    /* Check if the Quadro Sync card has a firmware
     * version compatible with the GPUs connected to it.
     */
    if (gsyncGetCapsParams.isFirmwareRevMismatch) {
        *pBadFirmware = TRUE;
        goto fail;
    }

    /* gsyncGetCapsParams.revId has the framelock board id in the high 4 bits
     * and the FPGA revision in the low 4 bits.  This is preserved here for
     * legacy clients, but we expose the full board ID (e.g. 0x358, 0x2060,
     * 0x2061) and firmware version individually, so clients can more easily
     * distinguish P2061 ("Quadro Sync II") from P2060 and P358
     * ("Quadro Sync").
     */

    pFrameLockEvo->fpgaIdAndRevision = gsyncGetCapsParams.revId;
    pFrameLockEvo->boardId = gsyncGetCapsParams.boardId;
    pFrameLockEvo->firmwareMajorVersion = gsyncGetCapsParams.revision;
    pFrameLockEvo->firmwareMinorVersion = gsyncGetCapsParams.extendedRevision;
    pFrameLockEvo->caps = gsyncGetCapsParams.capFlags;
    pFrameLockEvo->maxSyncSkew = gsyncGetCapsParams.maxSyncSkew;
    pFrameLockEvo->syncSkewResolution = gsyncGetCapsParams.syncSkewResolution;
    pFrameLockEvo->maxSyncInterval = gsyncGetCapsParams.maxSyncInterval;
    pFrameLockEvo->videoModeReadOnly = !!(gsyncGetCapsParams.capFlags &
        NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ONLY_GET_VIDEO_MODE);
    pFrameLockEvo->mulDivSupported = !!(gsyncGetCapsParams.capFlags &
        NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_MULTIPLY_DIVIDE_SYNC);
    pFrameLockEvo->maxMulDivValue = gsyncGetCapsParams.maxMulDivValue;

    /* Determine if house sync is selectable on this frame lock device */
    if (!FrameLockUseHouseSyncGetSupport(pFrameLockEvo,
                                         &pFrameLockEvo->houseSyncUseable)) {
        pFrameLockEvo->houseSyncUseable = FALSE;
    }

    pFrameLockEvo->houseSyncModeValidValues =
        (1 << NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_DISABLED);

    if (pFrameLockEvo->houseSyncUseable) {
        pFrameLockEvo->houseSyncModeValidValues |=
            (1 << NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_INPUT);
    }

    if (HouseSyncOutputModeUsable(pFrameLockEvo)) {
        pFrameLockEvo->houseSyncModeValidValues |=
            (1 << NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_OUTPUT);
    }

    /* Add frame lock device to global list. */
    nvListAppend(&pFrameLockEvo->frameLockListEntry, &nvEvoGlobal.frameLockList);

    return pFrameLockEvo;

fail:

    FreeFrameLockEvo(pFrameLockEvo);
    return NULL;
}


static void BindFrameLockToDevEvo(NVFrameLockEvoPtr pFrameLockEvo,
                                  NVDevEvoPtr pDevEvo)
{
    NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS gsyncTopologyParams = { };
    int i;

    /* find out which gpus are attached to which connectors */

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GET_GSYNC_GPU_TOPOLOGY,
                       &gsyncTopologyParams,
                       sizeof(gsyncTopologyParams))
        != NVOS_STATUS_SUCCESS) {
        return;
    }

    /* Bind corresponding GPUs to the Frame Lock device */
    for (i = 0; i < ARRAY_LEN(gsyncTopologyParams.gpus); i++) {

        NvU32 connectorIndex;
        const NvU32 gpuId = gsyncTopologyParams.gpus[i].gpuId;

        if (gpuId == NV30F1_CTRL_GPU_INVALID_ID) {
            continue;
        }

        if (!GpuIdInDevEvo(pDevEvo, gpuId)) {
            continue;
        }

        /*
         * Connector type of _NONE means we sync through a proxy GPU,
         * which we do not support.
         */
        if (gsyncTopologyParams.gpus[i].connector ==
                NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_NONE) {
            continue;
        }
        /*
         * gsyncTopologyParams.gpus[i].connector is an enumerated
         * type; convert it to a 0-based index
         */
        nvAssert(gsyncTopologyParams.gpus[i].connector <
                 (NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_ONE +
                  NV30F1_GSYNC_CONNECTOR_COUNT));
        connectorIndex = gsyncTopologyParams.gpus[i].connector -
            NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_ONE;

        BindGpuToFrameLock(pDevEvo, gpuId, pFrameLockEvo, connectorIndex);
    }
}

static void UnBindFrameLockFromDevEvo(NVFrameLockEvoPtr pFrameLockEvo,
                                      NVDevEvoPtr pDevEvo)
{
    int i;

    /*
     * Loop through GPUs from highest to lowest, because
     * UnbindGpuFromFrameLock() may remove gpuIds[i].
     */
    for (i = pFrameLockEvo->nGpuIds - 1; i >= 0; i--) {
        const NvU32 gpuId = pFrameLockEvo->gpuIds[i];

        if (!GpuIdInDevEvo(pDevEvo, gpuId)) {
            continue;
        }

        UnbindGpuFromFrameLock(pDevEvo, gpuId, pFrameLockEvo);
    }
}


/*!
 * Find all of the available framelock devices.
 *
 * Framelock devices can only be recognized by resman after an RM
 * client has attached a GPU that the framelock device is connected
 * to.  So, subsequent calls to this function may find additional
 * framelock devices.
 *
 * Allocate framelock objects for all the newly found framelock devices.
 */
void nvAllocFrameLocksEvo(NVDevEvoPtr pDevEvo)
{
    NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS attachedGsyncParams = { };
    int i;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       nvEvoGlobal.clientHandle,
                       NV0000_CTRL_CMD_GSYNC_GET_ATTACHED_IDS,
                       &attachedGsyncParams, sizeof(attachedGsyncParams))
        != NVOS_STATUS_SUCCESS) {
        return;
    }

    for (i = 0; i < ARRAY_LEN(attachedGsyncParams.gsyncIds); i++) {
        NVFrameLockEvoPtr pFrameLockEvo;
        NvBool badFirmware = FALSE;

        if (attachedGsyncParams.gsyncIds[i] == NV0000_CTRL_GSYNC_INVALID_ID) {
            continue;
        }

        pFrameLockEvo = FindFrameLock(attachedGsyncParams.gsyncIds[i]);

        if (pFrameLockEvo == NULL) {
            pFrameLockEvo = AllocFrameLockEvo(i,
                                              attachedGsyncParams.gsyncIds[i],
                                              &badFirmware);
        }

        if (pFrameLockEvo == NULL) {
            if (badFirmware) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "The firmware on this Quadro Sync card is not compatible "
                    "with the GPUs connected to it.  Please visit "
                    "<https://www.nvidia.com/object/quadro-sync.html> "
                    "for instructions on installing the correct firmware.");
                pDevEvo->badFramelockFirmware = TRUE;
            }
            continue;
        }

        BindFrameLockToDevEvo(pFrameLockEvo, pDevEvo);
    }
}

/*!
 * Free any framelock devices connected to any GPU on this pDevEvo.
 */

void nvFreeFrameLocksEvo(NVDevEvoPtr pDevEvo)
{
    NVFrameLockEvoPtr pFrameLockEvo, pFrameLockEvoTmp;

    /* Destroy the pFrameLockEvos */
    nvListForEachEntry_safe(pFrameLockEvo, pFrameLockEvoTmp,
                            &nvEvoGlobal.frameLockList, frameLockListEntry) {

        UnBindFrameLockFromDevEvo(pFrameLockEvo, pDevEvo);

        if (pFrameLockEvo->nGpuIds == 0) {
            FreeFrameLockEvo(pFrameLockEvo);
        }
    }
}

/*!
 * Determine if this framelock device supports user selection of house
 * sync.  assign val appropriately.  Returns TRUE if the attribute was
 * successfully queried.
 */
static NvBool FrameLockUseHouseSyncGetSupport(NVFrameLockEvoPtr pFrameLockEvo,
                                              NvU32 *val)
{
    NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS
        gsyncGetControlParamsParams = { 0 };
    NvU32 ret;

    if (!val) return FALSE;

    gsyncGetControlParamsParams.which = 
        NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_USE_HOUSE;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_PARAMS,
                         &gsyncGetControlParamsParams,
                         sizeof(gsyncGetControlParamsParams));

    /* If we can query Use House Sync, then it is available */
    *val = (ret == NVOS_STATUS_SUCCESS) ? TRUE : FALSE;

    return *val;
}


/*!
 * Return whether or not this framelock device supports house sync mode.
 *
 * House sync mode is currently only available on P2061 (Quadro Sync II).
 */
static NvBool HouseSyncOutputModeUsable(const NVFrameLockEvoRec *pFrameLockEvo)
{
    return (pFrameLockEvo->houseSyncUseable &&
            (pFrameLockEvo->boardId ==
             NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2061));
}


/*!
 * Enable or disable house sync output mode in the framelock board.
 */
static NvBool FrameLockSetHouseSyncOutputMode(NVFrameLockEvoPtr pFrameLockEvo,
                                              NvBool enable)
{
    NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS
        gsyncSetHouseSyncModeParams = { 0 };
    NvU32 ret;
    NvU8 houseSyncMode = enable ? NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_OUTPUT :
                                  NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_INPUT;

    nvAssert(HouseSyncOutputModeUsable(pFrameLockEvo));

    gsyncSetHouseSyncModeParams.houseSyncMode = houseSyncMode;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_HOUSE_SYNC_MODE,
                         &gsyncSetHouseSyncModeParams,
                         sizeof(gsyncSetHouseSyncModeParams));

    return (ret == NVOS_STATUS_SUCCESS);
}


/*!
 * Set the framelock to use the house sync if val is TRUE, otherwise
 * set the framelock to use external sync.  Returns FALSE if the
 * assignment failed.
 */
NvBool nvFrameLockSetUseHouseSyncEvo(NVFrameLockEvoPtr pFrameLockEvo, NvU32 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;
    NvBool houseSyncOutputMode = FALSE;

    gsyncSetControlParamsParams.which = 
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_USE_HOUSE;
    
    gsyncSetControlParamsParams.useHouseSync = val;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    if (HouseSyncOutputModeUsable(pFrameLockEvo)) {

        NvS64 houseSyncInputPresent;
        NvBool allowHouseSyncOutput = FALSE;

        if (nvFrameLockGetStatusEvo(pFrameLockEvo,
                                    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS,
                                    &houseSyncInputPresent)) {
            if (houseSyncInputPresent == 0) {
                allowHouseSyncOutput = TRUE;
            }
        }

        if (!val && allowHouseSyncOutput &&
            (pFrameLockEvo->houseSyncMode ==
             NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_OUTPUT)) {

            houseSyncOutputMode = TRUE;
        }

        if (!FrameLockSetHouseSyncOutputMode(pFrameLockEvo, houseSyncOutputMode)) {
            return FALSE;
        }
    }

    /*
     * House sync polarity is required to be rising edge if house sync is not
     * in use.
     *
     * In addition, house sync polarity has no effect when house sync output
     * mode is in use.
     */
    if (val && !houseSyncOutputMode) {
        return FrameLockSetPolarity(pFrameLockEvo, pFrameLockEvo->polarity);
    } else {
        return FrameLockSetPolarity(pFrameLockEvo,
                                    NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_RISING_EDGE);
    }
}

/*!
 * Set the polarity according to val; val is interpreted as an
 * NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY value.  Returns FALSE if the
 * assignment failed.
 */
static NvBool FrameLockSetPolarity(
    NVFrameLockEvoPtr pFrameLockEvo,
    enum NvKmsFrameLockAttributePolarityValue val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;
    NvU32 polarity;

    gsyncSetControlParamsParams.which = 
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY;

    switch (val) {
    case NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_RISING_EDGE:
        polarity = NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_RISING_EDGE;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_FALLING_EDGE:
        polarity = NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_FALLING_EDGE;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_BOTH_EDGES:
        polarity = NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_BOTH_EDGES;
        break;

    default:
        return FALSE;
    }
        
    gsyncSetControlParamsParams.syncPolarity = polarity;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    return TRUE;
}

/*!
 * Set the sync delay to the value given in val.  Returns FALSE if the
 * assignment failed.  Assigns pFrameLockEvo->syncDelay upon success.
 */
static NvBool FrameLockSetSyncDelay(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;

    if (val > pFrameLockEvo->maxSyncSkew) return FALSE;

    gsyncSetControlParamsParams.which =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_SKEW;

    gsyncSetControlParamsParams.syncSkew = val;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->syncDelay = val;

    return TRUE;
}

/*!
 * Set the sync multiply/divide value given in val.  Returns FALSE if the
 * assignment failed.  Assigns pFrameLockEvo->mulDivValue upon success.
 */
static NvBool SetFrameLockMulDivVal(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;

    if (!pFrameLockEvo->mulDivSupported ||
        (val > pFrameLockEvo->maxMulDivValue)) {
        return FALSE;
    }

    gsyncSetControlParamsParams.which =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MULTIPLY_DIVIDE;

    gsyncSetControlParamsParams.syncMulDiv.multiplyDivideValue = val;
    gsyncSetControlParamsParams.syncMulDiv.multiplyDivideMode = pFrameLockEvo->mulDivMode;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->mulDivValue = val;

    return TRUE;
}

/*!
 * Set the sync multiply/divide mode given in val.  Returns FALSE if the
 * assignment failed.  Assigns pFrameLockEvo->mulDivMode upon success.
 */
static NvBool SetFrameLockMulDivMode(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;

    if (!pFrameLockEvo->mulDivSupported ||
        ((val != NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE_MULTIPLY) &&
         (val != NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE_DIVIDE))) {
        return FALSE;
    }

    gsyncSetControlParamsParams.which =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MULTIPLY_DIVIDE;

    gsyncSetControlParamsParams.syncMulDiv.multiplyDivideValue = pFrameLockEvo->mulDivValue;
    gsyncSetControlParamsParams.syncMulDiv.multiplyDivideMode = val;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->mulDivMode = val;

    return TRUE;
}
/*!
 * Set the sync interval to the value given in val.  Returns FALSE if
 * the assignment failed.  Assigns pFrameLockEvo->syncInterval upon
 * success.
 */
static NvBool FrameLockSetSyncInterval(NVFrameLockEvoPtr pFrameLockEvo,
                                       NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;

    gsyncSetControlParamsParams.which =
        NV30F1_CTRL_GSYNC_SET_CONTROL_NSYNC;

    gsyncSetControlParamsParams.nSync = val;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->syncInterval = val;

    return TRUE;
}

/*!
 * Query the status of the values that are acquired through the
 * GET_STATUS_SYNC command, and assign the value to val.  Returns
 * FALSE if the query failed or if attr is not one of the currently
 * handled attributes.
 */
static NvBool FrameLockGetStatusSync(const NVDispEvoRec *pDispEvo, NvS64 *val,
                                     enum NvKmsDispAttribute nvKmsAttribute)
{
    NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS gsyncGetStatusSyncParams = { 0 };
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;

    gsyncGetStatusSyncParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SYNC,
                       &gsyncGetStatusSyncParams,
                       sizeof(gsyncGetStatusSyncParams))
            != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    switch (nvKmsAttribute)
    {

    case NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_STEREO_SYNC:
        *val = (gsyncGetStatusSyncParams.bTiming &&
                gsyncGetStatusSyncParams.bStereoSync &&
                gsyncGetStatusSyncParams.bSyncReady);
        break;

    case NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TIMING:
        *val = gsyncGetStatusSyncParams.bTiming ? TRUE : FALSE;
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

/*!
 * Return the sync rate.
 */
static NvS64 FrameLockInterpretSyncRate(const NVFrameLockEvoRec *pFrameLockEvo,
                                        NvS64 val)
{
    /* Only show decimal places if they are accurate. The queried
       value provides 4 decimal places */
    if (pFrameLockEvo->caps & 
        NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_2DPS) {
        // only two are valid
        val -= (val % 100);
    } else if (pFrameLockEvo->caps &
               NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_3DPS) {
        // only three are valid
        val -= (val % 10);
    } else if (pFrameLockEvo->caps &
               NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_4DPS) {
        // all four are valid, nothing to do
    }
    return val;
}

/*!
 * Query the status of one of the values that are acquired through the
 * GET_STATUS command, and assign the value to val.  Returns FALSE if
 * the query failed or if attr is not one of the currently handled
 * attributes.
 */
NvBool nvFrameLockGetStatusEvo(const NVFrameLockEvoRec *pFrameLockEvo,
                               enum NvKmsFrameLockAttribute attribute,
                               NvS64 *val)
{
    NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS gsyncGetStatusParams = { 0 };

    switch (attribute) {

    case NV_KMS_FRAMELOCK_ATTRIBUTE_PORT0_STATUS:
        gsyncGetStatusParams.which = NV30F1_CTRL_GSYNC_GET_STATUS_PORT0_INPUT;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_PORT1_STATUS:
        gsyncGetStatusParams.which = NV30F1_CTRL_GSYNC_GET_STATUS_PORT1_INPUT;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS:
        gsyncGetStatusParams.which = NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_INCOMING_HOUSE_SYNC_RATE:
        gsyncGetStatusParams.which =
            NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC_INCOMING;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY:
        gsyncGetStatusParams.which = NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_READY;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED:
        gsyncGetStatusParams.which =
            NV30F1_CTRL_GSYNC_GET_STATUS_PORT0_ETHERNET |
            NV30F1_CTRL_GSYNC_GET_STATUS_PORT1_ETHERNET;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE:
    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE_4:
        gsyncGetStatusParams.which = NV30F1_CTRL_GSYNC_GET_STATUS_REFRESH;
        break;

    default:
        return FALSE;
    }

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GSYNC_GET_STATUS,
                       &gsyncGetStatusParams,
                       sizeof(gsyncGetStatusParams))
            != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    switch (attribute) {

    case NV_KMS_FRAMELOCK_ATTRIBUTE_PORT0_STATUS:
        *val = gsyncGetStatusParams.bPort0Input ?
            NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_INPUT :
            NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_OUTPUT;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_PORT1_STATUS:
        *val = gsyncGetStatusParams.bPort1Input ?
            NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_INPUT :
            NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_OUTPUT;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS:
        *val = gsyncGetStatusParams.bHouseSync;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY:
        *val = gsyncGetStatusParams.bSyncReady;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED:
        *val = 0x0;
        if (gsyncGetStatusParams.bPort0Ethernet)
            *val |= NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED_PORT0;
        if (gsyncGetStatusParams.bPort1Ethernet)
            *val |= NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED_PORT1;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_INCOMING_HOUSE_SYNC_RATE:
        *val =
            FrameLockInterpretSyncRate(pFrameLockEvo,
                                       gsyncGetStatusParams.houseSyncIncoming);
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE:
    case NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE_4:
        *val = FrameLockInterpretSyncRate(pFrameLockEvo,
                                          gsyncGetStatusParams.refresh);
        if (attribute == NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE) {
            /* _STATUS_REFRESH is in Hz/10000, _SYNC_RATE is Hz/1000 */
            *val /= 10;
        }
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

/*!
 * [en|dis]able syncing of the GPU to the FrameLock board for the
 * display mask associated with that gpu. val controls whether we are
 * enabling or disabling.
 */
static NvBool FrameLockSetEnable(NVDispEvoPtr pDispEvo, NvS64 val)
{
    if (val) {

        /* XXX NVKMS TODO: address the following:

           In Xinerama a single app has a channel on each gpu. Before
           framelock is enabled the first time per X server, vblanks
           are not synchronized, so if a swap groupped app is started
           before framelock is enabled the channels get unstalled at
           different times, and it's likely that one display will be
           armed while the other is not. When framelock is enabled in
           this state, we'll deadlock because suddenly the armed display
           is waiting on the unarmed display to unstall, and the unarmed
           display cannot arm. Prevent this by idling all channels */

        return nvEnableFrameLockEvo(pDispEvo);
    } else {
        return nvDisableFrameLockEvo(pDispEvo);
    }
}

static NvBool FrameLockSetWatchdog(NVFrameLockEvoPtr pFrameLockEvo, NvU32 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS
        gsyncSetControlWatchdogParams = { 0 };
    NvU32 ret;

    gsyncSetControlWatchdogParams.enable = val;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_WATCHDOG,
                         &gsyncSetControlWatchdogParams,
                         sizeof(gsyncSetControlWatchdogParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    return TRUE;
}


/*!
 * For the given display, determine if it can be set as a frame lock
 * server
 */
static NvBool FrameLockDpyCanBeServer(const NVDpyEvoRec *pDpyEvo)
{
    NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS gsyncGetControlSyncParams = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);
    const NVDispHeadStateEvoRec *pHeadState;
    NvU32 ret;

    nvAssert(head != NV_INVALID_HEAD);
    nvAssert(pDispEvo);
    nvAssert(pDispEvo->pFrameLockEvo);

    pHeadState = &pDispEvo->headState[head];
    nvAssert(pHeadState->activeRmId);

    /* If already a server, assume it can be a server. */
    if (nvDpyIdsAreEqual(pDispEvo->framelock.server, pDpyEvo->id)) {
        return TRUE;
    }

    gsyncGetControlSyncParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);
    gsyncGetControlSyncParams.displays = pHeadState->activeRmId;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SYNC,
                         &gsyncGetControlSyncParams,
                         sizeof(gsyncGetControlSyncParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    if (gsyncGetControlSyncParams.master &&
        nvFrameLockServerPossibleEvo(pDpyEvo)) {
        return TRUE;
    }

    return FALSE;
}


/*!
 * For the given display, determine if it can be set as a frame lock
 * client.
 */
static NvBool FrameLockDpyCanBeClient(const NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoPtr pDispEvo;

    nvAssert(pDpyEvo->pDispEvo);
    nvAssert(pDpyEvo->pDispEvo->pFrameLockEvo);
    nvAssert(nvDpyEvoIsActive(pDpyEvo));

    pDispEvo = pDpyEvo->pDispEvo;

    /* If already a client, assume it can be a client. */
    if (nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->framelock.clients)) {
        return TRUE;
    }

    /* Otherwise, see if we can make it a client. */
    return nvFrameLockClientPossibleEvo(pDpyEvo);
}


/*!
 * [en|dis]able test mode (based on the value of val).  Returns FALSE
 * if changing the test mode failed.  Assigns pFrameLockEvo->testMode
 * upon success.
 */
static NvBool FrameLockSetTestMode(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS
        gsyncSetControlTestingParams = { 0 };
    NvU32 ret;

    gsyncSetControlTestingParams.bEmitTestSignal = (val == TRUE);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_TESTING,
                         &gsyncSetControlTestingParams,
                         sizeof(gsyncSetControlTestingParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->testMode = val;

    return TRUE;
}


/*!
 * Set the video mode according to val; returns FALSE if the
 * assignment failed.  Assigns pFrameLockEvo->videoMode upon success.
 */
static NvBool FrameLockSetVideoMode(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS
        gsyncSetControlParamsParams = { 0 };
    NvU32 ret;

    gsyncSetControlParamsParams.which =
        NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE;

    switch (val) {

    case NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_AUTO:
        gsyncSetControlParamsParams.syncVideoMode =
            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NONE;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_TTL:
        gsyncSetControlParamsParams.syncVideoMode =
            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_TTL;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_BI_LEVEL:
        gsyncSetControlParamsParams.syncVideoMode =
            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NTSCPALSECAM;
        break;

    case NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_TRI_LEVEL:
        gsyncSetControlParamsParams.syncVideoMode =
            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_HDTV;
        break;

    default:
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS,
                         &gsyncSetControlParamsParams,
                         sizeof(gsyncSetControlParamsParams));

    if (ret != NVOS_STATUS_SUCCESS) return FALSE;

    pFrameLockEvo->videoMode = val;

    return TRUE;
}


/*!
 * Enable or disable the swap ready connection through the gsync
 * connector. This should be called when we bind the swap barrier.
 */
static NvBool SetSwapBarrier(NVDispEvoPtr pDispEvo, NvS64 val)
{
    NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS
        gsyncSetSwapBarrierParams = { 0 };
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NvU32 ret;
    NvBool enable = !!val;

    if (!pFrameLockEvo) return FALSE;

    nvSetSwapBarrierNotifyEvo(pDispEvo, enable, TRUE /* isPre */);

    gsyncSetSwapBarrierParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);
    gsyncSetSwapBarrierParams.enable = enable;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SWAP_BARRIER,
                         &gsyncSetSwapBarrierParams,
                         sizeof(gsyncSetSwapBarrierParams));

    nvSetSwapBarrierNotifyEvo(pDispEvo, enable, FALSE /* isPre */);

    return (ret == NVOS_STATUS_SUCCESS);
}


/*!
 * Flush all of our known framelock SW state out to the HW, to make
 * sure both are in sync.  This should be called any time we get the
 * HW back from outside control (e.g., starting X or coming back from
 * a VT switch).
 */
static NvBool ResetHardwareOneDisp(NVDispEvoPtr pDispEvo, NvS64 value)
{
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NvU32 activeHeadsMask;
    NvBool ret = TRUE;

    if (!pDispEvo->pFrameLockEvo || !value) {
        /* Nothing to do */
        return TRUE;
    }

    /* We should never get here when framelock is enabled */
    if (pDispEvo->framelock.syncEnabled) {
        nvAssert(!"Attempted to reset framelock HW while framelock is enabled");
        return FALSE;
    }

    /* (Re-)set the HW state to match the SW state */
    if (!nvFrameLockSetUseHouseSyncEvo(pFrameLockEvo,
                                       pFrameLockEvo->houseSyncArmed)) {
        ret = FALSE;
    }
    if (!FrameLockSetSyncDelay(pFrameLockEvo, pFrameLockEvo->syncDelay)) {
        ret = FALSE;
    }
    if (!FrameLockSetSyncInterval(pFrameLockEvo, pFrameLockEvo->syncInterval)) {
        ret = FALSE;
    }
    if (!FrameLockSetVideoMode(pFrameLockEvo, pFrameLockEvo->videoMode)) {
        ret = FALSE;
    }
    if (!FrameLockSetTestMode(pFrameLockEvo, pFrameLockEvo->testMode)) {
        ret = FALSE;
    }
    if (!SetFrameLockMulDivVal(pFrameLockEvo, pFrameLockEvo->mulDivValue)) {
        ret = FALSE;
    }
    if (!SetFrameLockMulDivMode(pFrameLockEvo, pFrameLockEvo->mulDivMode)) {
        ret = FALSE;
    }

    /* Since (we think) sync is disabled, these should always be disabled */
    if (!FrameLockSetWatchdog(pFrameLockEvo, FALSE)) {
        ret = FALSE;
    }
    if (!SetSwapBarrier(pDispEvo, FALSE)) {
        ret = FALSE;
    }

    /* Disable both server and client lock for all heads */
    activeHeadsMask = nvGetActiveHeadMask(pDispEvo);

    if (!nvFramelockSetControlUnsyncEvo(pDispEvo, activeHeadsMask, TRUE)) {
        ret = FALSE;
    }
    if (!nvFramelockSetControlUnsyncEvo(pDispEvo, activeHeadsMask, FALSE)) {
        ret = FALSE;
    }

    return ret;
}


/*!
 * Returns the allowable configurations for the given display device.
 * The device must be enabled to advertise server/client
 * configuration.
 */
static unsigned int FrameLockGetValidDpyConfig(const NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoPtr pDispEvo;
    unsigned int valid =
        (1 << (NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_DISABLED));

    if (!pDpyEvo || !nvDpyEvoIsActive(pDpyEvo)) {
        goto done;
    }

    pDispEvo = pDpyEvo->pDispEvo;

    if (!pDispEvo || !pDispEvo->pFrameLockEvo) {
        goto done;
    }

    /* Check if display can be a server */

    if (FrameLockDpyCanBeServer(pDpyEvo)) {
        valid |= (1 << (NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_SERVER));
    }

    /* Check if display can be a client */

    if (FrameLockDpyCanBeClient(pDpyEvo)) {
        valid |= (1 << (NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_CLIENT));
    }

 done:

    return valid;
}

static NvBool GetFrameLock(NVDispEvoPtr pDispEvo, NvS64 *val)
{
    *val = (pDispEvo->pFrameLockEvo) ? 1 : 0;
    return TRUE;
}

static NvBool SetFrameLockPolarity(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    if ((val != NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_RISING_EDGE) &&
        (val != NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_FALLING_EDGE) &&
        (val != NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_BOTH_EDGES)) {
        return FALSE;
    }

    pFrameLockEvo->polarity = val;

    return TRUE;
}

static NvBool GetFrameLockPolarity(const NVFrameLockEvoRec *pFrameLockEvo,
                                   enum NvKmsFrameLockAttribute attribute,
                                   NvS64 *val)
{
    *val = pFrameLockEvo->polarity;

    return TRUE;
}

static NvBool GetFrameLockSyncDelay(const NVFrameLockEvoRec *pFrameLockEvo,
                                    enum NvKmsFrameLockAttribute attribute,
                                    NvS64 *val)
{
    *val = pFrameLockEvo->syncDelay;

    return TRUE;
}

static NvBool GetFrameLockSyncDelayValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min = 0;
    pValidValues->u.range.max = pFrameLockEvo->maxSyncSkew;

    return TRUE;
}

static NvBool GetFrameLockMulDivVal(const NVFrameLockEvoRec *pFrameLockEvo,
                                    enum NvKmsFrameLockAttribute attribute,
                                    NvS64 *val)
{
    if (!pFrameLockEvo->mulDivSupported) {
        return FALSE;
    }

    *val = pFrameLockEvo->mulDivValue;

    return TRUE;
}

static NvBool GetFrameLockMulDivValValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    if (!pFrameLockEvo->mulDivSupported) {
        return FALSE;
    }

    pValidValues->u.range.min = 1;
    pValidValues->u.range.max = pFrameLockEvo->maxMulDivValue;

    return TRUE;
}

static NvBool GetFrameLockMulDivModeValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!pFrameLockEvo->mulDivSupported) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool GetFrameLockMulDivMode(const NVFrameLockEvoRec *pFrameLockEvo,
                                     enum NvKmsFrameLockAttribute attribute,
                                     NvS64 *val)
{
    if (!pFrameLockEvo->mulDivSupported) {
        return FALSE;
    }

    *val = pFrameLockEvo->mulDivMode;

    return TRUE;
}

static NvBool SetHouseSyncMode(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    if ((val < 0) || (val > 31)) {
        return FALSE;
    }

    if ((pFrameLockEvo->houseSyncModeValidValues & NVBIT(val)) == 0) {
        return FALSE;
    }

    pFrameLockEvo->houseSyncMode = val;

    return TRUE;
}

static NvBool GetHouseSyncMode(const NVFrameLockEvoRec *pFrameLockEvo,
                               enum NvKmsFrameLockAttribute attribute,
                               NvS64 *val)
{
    if (!pFrameLockEvo->houseSyncUseable) return FALSE;

    *val = pFrameLockEvo->houseSyncMode;

    return TRUE;
}

static NvBool GetHouseSyncModeValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!pFrameLockEvo->houseSyncUseable) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    pValidValues->u.bits.ints = pFrameLockEvo->houseSyncModeValidValues;

    return TRUE;
}

static NvBool GetFrameLockSyncInterval(const NVFrameLockEvoRec *pFrameLockEvo,
                                       enum NvKmsFrameLockAttribute attribute,
                                       NvS64 *val)
{
    *val = pFrameLockEvo->syncInterval;

    return TRUE;
}

static NvBool GetFrameLockSyncIntervalValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min = 0;
    pValidValues->u.range.max = pFrameLockEvo->maxSyncInterval;

    return TRUE;
}

static NvBool SetFrameLockSync(NVDispEvoRec *pDispEvo, NvS64 val)
{
    NvBool a, b;

    if (!pDispEvo->pFrameLockEvo) return FALSE;

    /* If we are already enabled or already disabled, we're done. */
    if (val == pDispEvo->framelock.syncEnabled) return TRUE;

    /* Something must be set to enable/disable */
    if (nvDpyIdIsInvalid(pDispEvo->framelock.server) &&
        nvDpyIdListIsEmpty(pDispEvo->framelock.clients)) return FALSE;

    /* If we're disabling and test mode is currently enabled, disable it */
    if (!val &&
        !nvDpyIdIsInvalid(pDispEvo->framelock.server) &&
        pDispEvo->pFrameLockEvo->testMode) {

        FrameLockSetTestMode(pDispEvo->pFrameLockEvo, FALSE);
    }

    /*
     * It is important to set syncEnabled before calling FrameLockSetEnable.
     * FrameLockSetEnable may call into GLS which may call back into the
     * driver to query if framelock is enabled, which checks this field.
     */
    pDispEvo->framelock.syncEnabled = val;

    a = FrameLockSetEnable(pDispEvo, val);
    b = FrameLockSetWatchdog(pDispEvo->pFrameLockEvo, val);

    /*
     * Since RM doesn't send a SYNC_READY event on sync disable through nvctrl,
     * send it here.
     */
    if (!val && a && b) {
        nvSendFrameLockAttributeChangedEventEvo(
            pDispEvo->pFrameLockEvo,
            NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY,
            FALSE);
        pDispEvo->pFrameLockEvo->syncReadyLast = val;
    }

    return (a && b);
}

static NvBool GetFrameLockSync(NVDispEvoPtr pDispEvo, NvS64 *val)
{
    if (!pDispEvo->pFrameLockEvo) return FALSE;

    /* return the cached state */

    *val = ((pDispEvo->framelock.currentServerHead != NV_INVALID_HEAD) ||
            (pDispEvo->framelock.currentClientHeadsMask != 0x0));

    return TRUE;
}

static NvBool GetFrameLockSyncReady(const NVFrameLockEvoRec *pFrameLockEvo,
                                    enum NvKmsFrameLockAttribute attribute,
                                    NvS64 *val)
{
    /* return the cached state */

    *val = pFrameLockEvo->syncReadyLast;

    return TRUE;
}

static NvBool GetFrameLockStereoSync(NVDispEvoPtr pDispEvo, NvS64 *val)
{
    if (!pDispEvo->pFrameLockEvo) return FALSE;

    return FrameLockGetStatusSync(pDispEvo, val,
                                  NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_STEREO_SYNC);
}

static NvBool GetFrameLockTiming(NVDispEvoPtr pDispEvo, NvS64 *val)
{
    if (!pDispEvo->pFrameLockEvo) return FALSE;

    return FrameLockGetStatusSync(pDispEvo, val,
                                  NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TIMING);
}

static NvBool SetFrameLockTestSignal(NVDispEvoRec *pDispEvo, NvS64 val)
{
    if (!pDispEvo->pFrameLockEvo) return FALSE;

    /* The test signal can only be emitted if the GPU is the server
     * and framelock is enabled.
     */

    if (!nvDpyIdIsInvalid(pDispEvo->framelock.server) &&
        pDispEvo->framelock.syncEnabled) {
        return FrameLockSetTestMode(pDispEvo->pFrameLockEvo, val);
    }

    return FALSE;
}

static NvBool GetFrameLockTestSignal(NVDispEvoPtr pDispEvo, NvS64 *val)
{
    if (!pDispEvo->pFrameLockEvo ||
        nvDpyIdIsInvalid(pDispEvo->framelock.server)) {
        return FALSE;
    }

    *val = pDispEvo->pFrameLockEvo->testMode;

    return TRUE;
}

static NvBool SetFrameLockVideoMode(NVFrameLockEvoPtr pFrameLockEvo, NvS64 val)
{
    if (pFrameLockEvo->videoModeReadOnly) {
        return FALSE;
    }

    return FrameLockSetVideoMode(pFrameLockEvo, val);
}

static NvBool GetFrameLockVideoMode(const NVFrameLockEvoRec *pFrameLockEvo,
                                    enum NvKmsFrameLockAttribute attribute,
                                    NvS64 *val)
{
    *val = pFrameLockEvo->videoMode;

    return TRUE;
}

static NvBool GetFrameLockVideoModeValidValues(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min =
        NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_AUTO;
    pValidValues->u.range.max =
        NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_TRI_LEVEL;

    if (pFrameLockEvo->videoModeReadOnly) {
        pValidValues->writable = FALSE;
    }

    return TRUE;
}

static NvBool GetFrameLockFpgaRevision(const NVFrameLockEvoRec *pFrameLockEvo,
                                       enum NvKmsFrameLockAttribute attribute,
                                       NvS64 *val)
{
    *val = pFrameLockEvo->fpgaIdAndRevision;

    return TRUE;
}

static NvBool GetFrameLockFirmwareMajorVersion(
    const NVFrameLockEvoRec *pFrameLockEvo,
    enum NvKmsFrameLockAttribute attribute,
    NvS64 *val)
{
    *val = pFrameLockEvo->firmwareMajorVersion;

    return TRUE;
}

static NvBool GetFrameLockFirmwareMinorVersion(
    const NVFrameLockEvoRec *pFrameLockEvo,
    enum NvKmsFrameLockAttribute attribute,
    NvS64 *val)
{
    *val = pFrameLockEvo->firmwareMinorVersion;

    return TRUE;
}

static NvBool GetFrameLockBoardId(const NVFrameLockEvoRec *pFrameLockEvo,
                                  enum NvKmsFrameLockAttribute attribute,
                                  NvS64 *val)
{
    *val = pFrameLockEvo->boardId;

    return TRUE;
}

static NvBool GetFrameLockFpgaRevisionUnsupported(
    NVDispEvoPtr pDispEvo,
    NvS64 *val)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    *val = pDevEvo->badFramelockFirmware;

    return TRUE;
}

static NvBool GetFrameLockSyncDelayResolution(
    const NVFrameLockEvoRec *pFrameLockEvo,
    enum NvKmsFrameLockAttribute attribute,
    NvS64 *val)
{
    *val = pFrameLockEvo->syncSkewResolution;

    return TRUE;
}

NvBool nvSetFrameLockDisplayConfigEvo(NVDpyEvoRec *pDpyEvo, NvS64 val)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    unsigned int valid;
    NvBool removeFromClients = FALSE;
    NvBool removeFromServer = FALSE;

    if (!pDispEvo || !pDispEvo->pFrameLockEvo) {
        return FALSE;
    }

    /* Only set the config when framelock is disabled */

    if (pDispEvo->framelock.syncEnabled) {
        return FALSE;
    }

    valid = FrameLockGetValidDpyConfig(pDpyEvo);

    /* Display device cannot be set as such */
    if (!((1<<val) & valid)) {
        return FALSE;
    }

    switch (val) {
    case NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_SERVER:
        /* alert other dpy it is being disabled as server */
        if (!nvDpyIdIsInvalid(pDispEvo->framelock.server) &&
            !nvDpyIdsAreEqual(pDispEvo->framelock.server, pDpyEvo->id)) {
            NVDpyEvoPtr pOtherDpyEvo;

            pOtherDpyEvo =
                nvGetDpyEvoFromDispEvo(pDispEvo, pDispEvo->framelock.server);
            if (pOtherDpyEvo) {
                nvSendDpyAttributeChangedEventEvo(
                    pOtherDpyEvo,
                    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG,
                    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_DISABLED);
            }
        }
        pDispEvo->framelock.server = pDpyEvo->id;
        removeFromClients = TRUE;
        break;

    case NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_CLIENT:
        pDispEvo->framelock.clients =
            nvAddDpyIdToDpyIdList(pDpyEvo->id, pDispEvo->framelock.clients);
        removeFromServer = TRUE;
        break;

    case NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_DISABLED:
        removeFromClients = TRUE;
        removeFromServer = TRUE;
        break;

    default:
        return FALSE;
    }

    if (removeFromClients) {
        if (nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->framelock.clients)) {
            pDispEvo->framelock.clients =
                nvDpyIdListMinusDpyId(pDispEvo->framelock.clients, pDpyEvo->id);
        }
    }

    if (removeFromServer) {
        if (nvDpyIdsAreEqual(pDispEvo->framelock.server, pDpyEvo->id)) {
            pDispEvo->framelock.server = nvInvalidDpyId();
        }
    }

    return TRUE;
}

NvBool nvGetFrameLockDisplayConfigEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *val)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;

    if (!pDispEvo || !pDispEvo->pFrameLockEvo) {
        return FALSE;
    }

    if (nvDpyIdsAreEqual(pDispEvo->framelock.server, pDpyEvo->id)) {
        *val = NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_SERVER;
    } else if (nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->framelock.clients)) {
        *val = NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_CLIENT;
    } else {
        *val = NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_DISABLED;
    }

    return TRUE;
}

NvBool nvGetFrameLockDisplayConfigValidValuesEvo(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (pDpyEvo->pDispEvo->pFrameLockEvo == NULL) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    pValidValues->u.bits.ints = FrameLockGetValidDpyConfig(pDpyEvo);

    return TRUE;
}

static const struct {
    NvBool (*set)(NVDispEvoPtr pDispEvo, NvS64 value);
    NvBool (*get)(NVDispEvoPtr pDispEvo, NvS64 *pValue);
    enum NvKmsAttributeType type;
} DispAttributesDispatchTable[] = {
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK] = {
        .set  = NULL,
        .get  = GetFrameLock,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_SYNC] = {
        .set  = SetFrameLockSync,
        .get  = GetFrameLockSync,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_GPU_FRAMELOCK_FPGA_REVISION_UNSUPPORTED] = {
        .set            = NULL,
        .get            = GetFrameLockFpgaRevisionUnsupported,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_STEREO_SYNC] = {
        .set  = NULL,
        .get  = GetFrameLockStereoSync,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TIMING] = {
        .set  = NULL,
        .get  = GetFrameLockTiming,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TEST_SIGNAL] = {
        .set  = SetFrameLockTestSignal,
        .get  = GetFrameLockTestSignal,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_RESET] = {
        .set  = ResetHardwareOneDisp,
        .get  = NULL,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_SET_SWAP_BARRIER] = {
        .set  = SetSwapBarrier,
        .get  = NULL,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DISP_ATTRIBUTE_QUERY_DP_AUX_LOG] = {
        .set  = NULL,
        .get  = nvRmQueryDpAuxLog,
        .type = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
};


/*!
 * Set pParams->attribute to pParams->value on the given disp.
 */
NvBool nvSetDispAttributeEvo(NVDispEvoPtr pDispEvo,
                             struct NvKmsSetDispAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(DispAttributesDispatchTable)) {
        return FALSE;
    }

    if (DispAttributesDispatchTable[index].set == NULL) {
        return FALSE;
    }

    return DispAttributesDispatchTable[index].set(pDispEvo,
                                                  pParams->request.value);
}


/*!
 * Get the value of pParams->attribute on the given disp.
 */
NvBool nvGetDispAttributeEvo(NVDispEvoPtr pDispEvo,
                             struct NvKmsGetDispAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(DispAttributesDispatchTable)) {
        return FALSE;
    }

    if (DispAttributesDispatchTable[index].get == NULL) {
        return FALSE;
    }

    return DispAttributesDispatchTable[index].get(pDispEvo,
                                                  &pParams->reply.value);
}


/*!
 * Get the valid values of pParams->attribute on the given disp.
 */
NvBool nvGetDispAttributeValidValuesEvo(
    const NVDispEvoRec *pDispEvo,
    struct NvKmsGetDispAttributeValidValuesParams *pParams)
{
    struct NvKmsAttributeValidValuesCommonReply *pReply =
        &pParams->reply.common;
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(DispAttributesDispatchTable)) {
        return FALSE;
    }

    /*
     * FRAMELOCK and GPU_FRAMELOCK_FPGA_REVISION_UNSUPPORTED
     * can be queried without a pFrameLockEvo; all other
     * attributes require a pFrameLockEvo.
     */
    if (((pParams->request.attribute != NV_KMS_DISP_ATTRIBUTE_FRAMELOCK) &&
         (pParams->request.attribute !=
          NV_KMS_DISP_ATTRIBUTE_GPU_FRAMELOCK_FPGA_REVISION_UNSUPPORTED)) &&
        (pDispEvo->pFrameLockEvo == NULL)) {
        return FALSE;
    }

    nvkms_memset(pReply, 0, sizeof(*pReply));

    pReply->readable = (DispAttributesDispatchTable[index].get != NULL);
    pReply->writable = (DispAttributesDispatchTable[index].set != NULL);

    pReply->type = DispAttributesDispatchTable[index].type;

    return TRUE;
}


static const struct {
    NvBool (*set)(NVFrameLockEvoPtr pFrameLockEvo, NvS64 value);
    NvBool (*get)(const NVFrameLockEvoRec *pFrameLockEvo,
                  enum NvKmsFrameLockAttribute attribute, NvS64 *pValue);
    NvBool (*getValidValues)(
        const NVFrameLockEvoRec *pFrameLockEvo,
        struct NvKmsAttributeValidValuesCommonReply *pValidValues);
    enum NvKmsAttributeType type;
} FrameLockAttributesDispatchTable[] = {
    [NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY] = {
        .set            = SetFrameLockPolarity,
        .get            = GetFrameLockPolarity,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BITMASK,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_DELAY] = {
        .set            = FrameLockSetSyncDelay,
        .get            = GetFrameLockSyncDelay,
        .getValidValues = GetFrameLockSyncDelayValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE] = {
        .set            = SetHouseSyncMode,
        .get            = GetHouseSyncMode,
        .getValidValues = GetHouseSyncModeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_INTERVAL] = {
        .set            = FrameLockSetSyncInterval,
        .get            = GetFrameLockSyncInterval,
        .getValidValues = GetFrameLockSyncIntervalValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY] = {
        .set            = NULL,
        .get            = GetFrameLockSyncReady,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE] = {
        .set            = SetFrameLockVideoMode,
        .get            = GetFrameLockVideoMode,
        .getValidValues = GetFrameLockVideoModeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_FPGA_REVISION] = {
        .set            = NULL,
        .get            = GetFrameLockFpgaRevision,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_FIRMWARE_MAJOR_VERSION] = {
        .set            = NULL,
        .get            = GetFrameLockFirmwareMajorVersion,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_FIRMWARE_MINOR_VERSION] = {
        .set            = NULL,
        .get            = GetFrameLockFirmwareMinorVersion,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_BOARD_ID] = {
        .set            = NULL,
        .get            = GetFrameLockBoardId,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_DELAY_RESOLUTION] = {
        .set            = NULL,
        .get            = GetFrameLockSyncDelayResolution,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_PORT0_STATUS] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_PORT1_STATUS] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BITMASK,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE_4] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_INCOMING_HOUSE_SYNC_RATE] = {
        .set            = NULL,
        .get            = nvFrameLockGetStatusEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_VALUE] = {
        .set            = SetFrameLockMulDivVal,
        .get            = GetFrameLockMulDivVal,
        .getValidValues = GetFrameLockMulDivValValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE] = {
        .set            = SetFrameLockMulDivMode,
        .get            = GetFrameLockMulDivMode,
        .getValidValues = GetFrameLockMulDivModeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
};

NvBool nvSetFrameLockAttributeEvo(
    NVFrameLockEvoRec *pFrameLockEvo,
    const struct NvKmsSetFrameLockAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(FrameLockAttributesDispatchTable)) {
        return FALSE;
    }

    if (FrameLockAttributesDispatchTable[index].set == NULL) {
        return FALSE;
    }

    if ((FrameLockAttributesDispatchTable[index].type ==
         NV_KMS_ATTRIBUTE_TYPE_BOOLEAN) &&
        (pParams->request.value != TRUE) &&
        (pParams->request.value != FALSE)) {
        return FALSE;
    }

    return FrameLockAttributesDispatchTable[index].set(pFrameLockEvo,
                                                       pParams->request.value);
}

NvBool nvGetFrameLockAttributeEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsGetFrameLockAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(FrameLockAttributesDispatchTable)) {
        return FALSE;
    }

    if (FrameLockAttributesDispatchTable[index].get == NULL) {
        return FALSE;
    }

    return FrameLockAttributesDispatchTable[index].get(pFrameLockEvo,
                                                       pParams->request.attribute,
                                                       &pParams->reply.value);
}

NvBool nvGetFrameLockAttributeValidValuesEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsGetFrameLockAttributeValidValuesParams *pParams)
{
    struct NvKmsAttributeValidValuesCommonReply *pReply =
        &pParams->reply.common;
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(FrameLockAttributesDispatchTable)) {
        return FALSE;
    }

    nvkms_memset(pReply, 0, sizeof(*pReply));

    pReply->readable = (FrameLockAttributesDispatchTable[index].get != NULL);
    pReply->writable = (FrameLockAttributesDispatchTable[index].set != NULL);

    pReply->type = FrameLockAttributesDispatchTable[index].type;

    /*
     * The getValidValues function provides two important things:
     * - If type==Range, then assigns reply::u::range.
     * - If the attribute is not currently available, returns FALSE.
     * If the getValidValues function is NULL, assume the attribute is
     * available.  The type must not be something requires assigning
     * to reply::u.
     */
    if (FrameLockAttributesDispatchTable[index].getValidValues == NULL) {
        nvAssert(pReply->type != NV_KMS_ATTRIBUTE_TYPE_RANGE);
        return TRUE;
    }

    return FrameLockAttributesDispatchTable[index].getValidValues(
            pFrameLockEvo, pReply);
}

NvU32 nvGetFramelockServerHead(const NVDispEvoRec *pDispEvo)
{
    const NVDpyEvoRec *pDpyEvo =
        nvGetDpyEvoFromDispEvo(pDispEvo, pDispEvo->framelock.server);
    return (pDpyEvo != NULL) ? nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead) :
                NV_INVALID_HEAD;
}

NvU32 nvGetFramelockClientHeadsMask(const NVDispEvoRec *pDispEvo)
{
    NvU32 headsMask = 0x0;
    const NVDpyEvoRec *pServerDpyEvo, *pClientDpyEvo;

    pServerDpyEvo = nvGetDpyEvoFromDispEvo(pDispEvo,
                                           pDispEvo->framelock.server);
    if ((pServerDpyEvo != NULL) &&
            (pServerDpyEvo->apiHead != NV_INVALID_HEAD)) {
        const NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[pServerDpyEvo->apiHead];
        NvU32 primaryHead = nvGetPrimaryHwHead(pDispEvo,
                                               pServerDpyEvo->apiHead);

        nvAssert(primaryHead != NV_INVALID_HEAD);

        /*
         * The secondary hardware-head of the server dpy are client of the
         * primary head.
         */
        headsMask |= pApiHeadState->hwHeadsMask;
        headsMask &= ~NVBIT(primaryHead);
    }

    FOR_ALL_EVO_DPYS(pClientDpyEvo, pDispEvo->framelock.clients, pDispEvo) {
        if (pClientDpyEvo->apiHead == NV_INVALID_HEAD) {
            continue;
        }
        const NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[pClientDpyEvo->apiHead];
        headsMask |= pApiHeadState->hwHeadsMask;
    }

    return headsMask;
}

void nvUpdateGLSFramelock(const NVDispEvoRec *pDispEvo, const NvU32 head,
                          const NvBool enable, const NvBool server)
{
    NVDpyEvoRec *pDpyEvo;
    NvS64 value = enable | (server << 1);

    /*
     * XXX[2Heads1OR] Optimize this loop in follow on code change when
     * apiHead -> pDpyEvo mapping will get implemented.
     */
    FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->validDisplays, pDispEvo) {
        /*
         * XXX[2Heads1OR] Framelock is currently not supported with
         * 2Heads1OR, the api head is expected to be mapped onto a single
         * hardware head which is the primary hardware head.
         */
        if ((pDpyEvo->apiHead == NV_INVALID_HEAD) ||
                (nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead) != head)) {
            continue;
        }

        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_UPDATE_GLS_FRAMELOCK,
            value);
    }
}
