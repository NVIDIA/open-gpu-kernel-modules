/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvos.h"
#include "dp/nvdp-connector.h"
#include "nvkms-event.h"
#include "nvkms-rm.h"
#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h"
#include "nvkms-private.h"
#include "nvkms-evo.h"

/*
 * Handle a display device hotplug event.
 *
 * What "hotplug" means is unclear, but it could mean any of the following:
 *   - A display device is plugged in.
 *   - A display device is unlugged.
 *   - A display device was unplugged and then plugged back in.
 *   - A display device was plugged in and then unplugged.
 *   - An already connected display device is turned on.
 *   - An already connected display device is turned off.
 *   - A DisplayPort device needs its link status and RX Capabilities fields
 *     read and may need to be retrained ("long" hotplug event, > 2ms).
 *
 * DisplayPort "short" hotplug events, which are between 0.25ms and 2ms, are
 * handled separately by nvHandleDPIRQEventDeferredWork below.
 */

void
nvHandleHotplugEventDeferredWork(void *dataPtr, NvU32 dataU32)
{
    NVDispEvoPtr pDispEvo = dataPtr;
    NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS hotplugParams = { 0 };
    NvU32 ret;
    NVDpyIdList hotplugged, unplugged, tmpUnplugged, changed;
    NVDpyIdList connectedDisplays;
    NVDpyEvoPtr pDpyEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    // Get the hotplug state.
    hotplugParams.subDeviceInstance = pDispEvo->displayOwner;

    if ((ret = nvRmApiControl(
                nvEvoGlobal.clientHandle,
                pDevEvo->displayCommonHandle,
                NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_UNPLUG_STATE,
                &hotplugParams,
                sizeof(hotplugParams)))
            != NVOS_STATUS_SUCCESS) {

        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN, "Failed to determine which "
                     "devices were hotplugged: 0x%x\n", ret);
        return;
    }

    /*
     * Work around an RM bug in hotplug notification when the GPU is in
     * GC6.  In this case, the RM will notify us of a hotplug event, but
     * NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_UNPLUG_STATE returns both
     * hotPlugMask and hotUnplugMask as 0.
     * Bug 200528641 tracks finding a root cause.  Until that bug is
     * fixed, call NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE to get the
     * full list of connected dpys and construct hotplugged and
     * unplugged lists from that if we encounter this case.
     */
    if ((hotplugParams.hotPlugMask == 0) &&
        (hotplugParams.hotUnplugMask == 0)) {
        const NVDpyIdList updatedDisplayList = nvRmGetConnectedDpys(pDispEvo,
                                                  pDispEvo->connectorIds);
        hotplugged = nvDpyIdListMinusDpyIdList(updatedDisplayList,
                                               pDispEvo->connectedDisplays);
        unplugged = nvDpyIdListMinusDpyIdList(pDispEvo->connectedDisplays,
                                              updatedDisplayList);
    } else {
        hotplugged = nvNvU32ToDpyIdList(hotplugParams.hotPlugMask);
        unplugged = nvNvU32ToDpyIdList(hotplugParams.hotUnplugMask);
    }

    // The RM only reports the latest plug/unplug status of each dpy.
    nvAssert(nvDpyIdListIsEmpty(nvIntersectDpyIdListAndDpyIdList(hotplugged,
                                                                 unplugged)));
    nvAssert(nvDpyIdListIsASubSetofDpyIdList(hotplugged,
                                             pDispEvo->connectorIds));
    nvAssert(nvDpyIdListIsASubSetofDpyIdList(unplugged,
                                             pDispEvo->connectorIds));

    connectedDisplays = pDispEvo->connectedDisplays;

    // Ignore non-DP devices that were reported as unplugged while already
    // disconnected.
    tmpUnplugged = nvEmptyDpyIdList();
    FOR_ALL_EVO_DPYS(pDpyEvo, unplugged, pDispEvo) {
        NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;

        if (nvConnectorUsesDPLib(pConnectorEvo) ||
            nvDpyIdIsInDpyIdList(pConnectorEvo->displayId, connectedDisplays)) {

            tmpUnplugged =
                nvAddDpyIdToDpyIdList(pConnectorEvo->displayId, tmpUnplugged);
        }
    }
    unplugged = tmpUnplugged;

    // Non-DP devices that were disconnected and connected again should generate an
    // unplug / plug pair.
    FOR_ALL_EVO_DPYS(pDpyEvo, hotplugged, pDispEvo) {
        NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;

        if (!nvConnectorUsesDPLib(pConnectorEvo) &&
            nvDpyIdIsInDpyIdList(pConnectorEvo->displayId, connectedDisplays)) {

            unplugged = nvAddDpyIdToDpyIdList(pConnectorEvo->displayId, unplugged);
        }
    }

#if defined(DEBUG)
    if (!nvDpyIdListIsEmpty(hotplugged)) {
        char *str = nvGetDpyIdListStringEvo(pDispEvo, hotplugged);
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_INFO,
                          "Received display hotplug event: %s",
                          nvSafeString(str, "unknown"));
        nvFree(str);
    }
    if (!nvDpyIdListIsEmpty(unplugged)) {
        char *str = nvGetDpyIdListStringEvo(pDispEvo, unplugged);
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_INFO,
                          "Received display unplug event: %s",
                          nvSafeString(str, "unknown"));
        nvFree(str);
    }
#endif /* DEBUG */

    // First, the OR configuration of the connector should not change, but
    // re-query it to make sure.
    changed = nvAddDpyIdListToDpyIdList(hotplugged, unplugged);
    FOR_ALL_EVO_DPYS(pDpyEvo, changed, pDispEvo) {
        nvRmGetConnectorORInfo(pDpyEvo->pConnectorEvo, TRUE);
    }

    // Next, disconnect devices that are in the unplug mask.
    FOR_ALL_EVO_DPYS(pDpyEvo, unplugged, pDispEvo) {
        NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;

        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            nvDPNotifyLongPulse(pConnectorEvo, FALSE);
        } else {
            nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DPY_CHANGED);
        }
    }

    // Finally, connect devices that are in the plug mask.
    FOR_ALL_EVO_DPYS(pDpyEvo, hotplugged, pDispEvo) {
        NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;

        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            nvDPNotifyLongPulse(pConnectorEvo, TRUE);
        } else {
            nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DPY_CHANGED);
        }
    }
}

void
nvHandleDPIRQEventDeferredWork(void *dataPtr, NvU32 dataU32)
{
    NVDispEvoPtr pDispEvo = dataPtr;

    // XXX[AGP]: ReceiveDPIRQEvent throws away the DisplayID of the device that
    // caused the event, so for now we have to poll all of the connected DP
    // devices to see which ones need attention.  When RM is fixed, this can be
    // improved.

    NVConnectorEvoPtr pConnectorEvo;

    // Notify all connectors which are using DP lib. For DP Serializer connector,
    // HPD_IRQ indicates loss of clock/sync, so re-train the link.
    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            nvDPNotifyShortPulse(pConnectorEvo->pDpLibConnector);
        } else if (nvConnectorIsDPSerializer(pConnectorEvo)) {
            nvDPSerializerHandleDPIRQ(pDispEvo, pConnectorEvo);
        }
    }
}
