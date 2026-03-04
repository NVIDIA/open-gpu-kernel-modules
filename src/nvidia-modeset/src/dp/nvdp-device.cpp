/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "dp/nvdp-device.h"
#include "nvdp-connector-event-sink.hpp"
#include "dp/nvdp-connector-event-sink.h"

#include "nvkms-types.h"
#include "nvkms-rm.h"
#include "nvkms-dpy.h"

#include "nvctassert.h"

void nvDPDeviceSetPowerState(NVDpyEvoPtr pDpyEvo, NvBool on)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    if (!pDpyEvo->dp.pDpLibDevice) {
        return;
    }

    nvAssert(nvDpyUsesDPLib(pDpyEvo));

    DisplayPort::Device *device = pDpyEvo->dp.pDpLibDevice->device;

    nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);
    device->setPanelPowerParams(on, on);

    /*
     * WAR: Some monitors clear the MSA_TIMING_PAR_IGNORE_EN bit in the
     * DOWNSPREAD_CTRL DPCD register after changing power state, which will
     * cause the monitor to fail to restore the image after powering back on
     * while VRR flipping.  To work around this, re-enable Adaptive-Sync
     * immediately after powering on.  (Bug 200488547)
     */
    if (nvDpyIsAdaptiveSync(pDpyEvo) && on) {
        NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;
        NVDPLibConnectorPtr pDpLibConnector = pConnectorEvo->pDpLibConnector;
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            if (nvDpyIdIsInDpyIdList(pDpyEvo->id,
                                     pDpLibConnector->dpyIdList[head]) &&
                (pDispEvo->headState[head].timings.vrr.type !=
                 NVKMS_DPY_VRR_TYPE_NONE)) {
                nvDPLibSetAdaptiveSync(pDispEvo, head, TRUE);
                break;
            }
        }
    }
}

unsigned int nvDPGetEDIDSize(const NVDpyEvoRec *pDpyEvo)
{
    NVDPLibDevicePtr pDpLibDevice = pDpyEvo->dp.pDpLibDevice;

    nvAssert(nvDpyUsesDPLib(pDpyEvo));

    if (!pDpLibDevice) {
        return 0;
    }

    return pDpLibDevice->device->getEDIDSize();
}

NvBool nvDPGetEDID(const NVDpyEvoRec *pDpyEvo, void *buffer, unsigned int size)
{
    NVDPLibDevicePtr pDpLibDevice = pDpyEvo->dp.pDpLibDevice;

    nvAssert(nvDpyUsesDPLib(pDpyEvo));

    if (!pDpLibDevice) {
        return FALSE;
    }

    return pDpLibDevice->device->getEDID((char *)buffer, size);
}

void nvDPGetDpyGUID(NVDpyEvoPtr pDpyEvo)
{
    NVDPLibDevicePtr pDpLibDevice;
    const char *str;

    nvkms_memset(&pDpyEvo->dp.guid, 0, sizeof(pDpyEvo->dp.guid));

    ct_assert(sizeof(pDpyEvo->dp.guid.buffer) == DPCD_GUID_SIZE);

    if (!nvDpyUsesDPLib(pDpyEvo)) {
        return;
    }

    pDpLibDevice = pDpyEvo->dp.pDpLibDevice;
    if (!pDpLibDevice) {
        return;
    }

    pDpyEvo->dp.guid.valid =
        nvkmsDisplayPort::nvDPGetDeviceGUID(pDpLibDevice->device,
                                        pDpyEvo->dp.guid.buffer) == true;
    if (!pDpyEvo->dp.guid.valid) {
        return;
    }

    str = nvkmsDisplayPort::nvDPGetDeviceGUIDStr(pDpLibDevice->device);
    if (str != NULL) {
        nvkms_strncpy(pDpyEvo->dp.guid.str, str, sizeof(pDpyEvo->dp.guid.str));
    } else {
        pDpyEvo->dp.guid.valid = FALSE;
    }
}

// Perform a fake lostDevice during device teardown.  This function is called by
// DpyFree before it deletes a pDpy.
void nvDPDpyFree(NVDpyEvoPtr pDpyEvo)
{
    if (!nvDpyUsesDPLib(pDpyEvo)) {
        return;
    }

    if (!pDpyEvo->dp.pDpLibDevice) {
        return;
    }

    DisplayPort::Device *device = pDpyEvo->dp.pDpLibDevice->device;

    pDpyEvo->pConnectorEvo->pDpLibConnector->evtSink->lostDevice(device);
}

NvBool nvDPDpyIsDscPossible(const NVDpyEvoRec *pDpyEvo)
{
    if (!nvDpyUsesDPLib(pDpyEvo) ||
            (pDpyEvo->dp.pDpLibDevice == NULL)) {
        return FALSE;
    }
    return pDpyEvo->dp.pDpLibDevice->device->isDSCPossible();
}

NvBool nvDPDpyGetDpcdRevision(const NVDpyEvoRec *pDpyEvo,
                              unsigned int *major,
                              unsigned int *minor)
{
    if (!nvDpyUsesDPLib(pDpyEvo) ||
        (pDpyEvo->dp.pDpLibDevice == NULL)) {
        return FALSE;
    }

    return pDpyEvo->dp.pDpLibDevice->device->getDpcdRevision(major, minor);
}
