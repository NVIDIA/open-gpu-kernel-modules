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

// This file implements the EVO RM interface used by the DisplayPort library.

#include "nvkms-utils.h"

#include "nvdp-evo-interface.hpp"

#include "nvkms-rmapi.h"

namespace nvkmsDisplayPort {

EvoInterface::EvoInterface(NVConnectorEvoPtr pConnectorEvo)
    : pConnectorEvo(pConnectorEvo)
{
}

NvU32 EvoInterface::rmControl0073(NvU32 command, void * params,
                                  NvU32 paramSize)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;

    return nvRmApiControl(nvEvoGlobal.clientHandle,
                          pDevEvo->displayCommonHandle,
                          command,
                          params,
                          paramSize);
}

NvU32 EvoInterface::rmControl5070(NvU32 command, void * params,
                                  NvU32 paramSize)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;

    return nvRmApiControl(nvEvoGlobal.clientHandle,
                          pDevEvo->displayHandle,
                          command,
                          params,
                          paramSize);
}

/*!
 * Look up the value of a particular key in the DisplayPort-specific registry
 * corresponding to this connector.  These values are provided at device
 * allocation time, copied from the client request during nvAllocDevEvo().
 *
 * \param[in]     key       The name of the key to look up.
 *
 * \return        The unsigned 32-bit value set for the key, or 0 if the key is
 *                not set.
 */
NvU32 EvoInterface::getRegkeyValue(const char *key)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NvU32 val;
    NvBool found = nvGetRegkeyValue(pDevEvo, key, &val);

    if (found) {
        return val;
    } else {
        return 0;
    }
}

bool EvoInterface::isInbandStereoSignalingSupported()
{

    return FALSE;
}

NvU32 EvoInterface::getSubdeviceIndex()
{
    return pConnectorEvo->pDispEvo->displayOwner;
}

NvU32 EvoInterface::getDisplayId()
{
    return nvDpyIdToNvU32(pConnectorEvo->displayId);
}

NvU32 EvoInterface::getSorIndex()
{
    return nvEvoConnectorGetPrimaryOr(pConnectorEvo);
}

NvU32 EvoInterface::getLinkIndex()
{
    switch (pConnectorEvo->or.protocol) {
    case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A:
        return 0;
    case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B:
        return 1;
    }

    nvAssert(!"Unrecognized DP protocol");
    return -1;
}

NvU32 EvoInterface::monitorDenylistInfo(
    NvU32 manufId, NvU32 productId,
    DisplayPort::DpMonitorDenylistData *pDenylistData)
{
    //
    // WAR for Toshiba/Dell internal(eDP) panel Sharp , overriding
    // optimal link configuration to HBR2.
    //
    // HBR2 is required to drive 4K resolution, which is supported on DP1.2
    // onward specifications. Panel advertises itself as DP1.2 capable, but
    // does not have ESI address space, this is violation the specification
    // and hence inside DP library we downgrade the DPCD revision to 1.1.
    // With this downgrade in DPCD version, link rate also gets downgraded
    // to HBR.
    //
    if (manufId == 0x104d &&
        (productId == 0x1414 || productId == 0x1430)) {

        NvU32 warFlags = DisplayPort::DP_MONITOR_CAPABILITY_DP_OVERRIDE_OPTIMAL_LINK_CONFIG;

        pDenylistData->dpOverrideOptimalLinkConfig.linkRate  = 0x14; // HBR2
        pDenylistData->dpOverrideOptimalLinkConfig.laneCount = laneCount_4; // 4 lanes

        return warFlags;
    }

    return 0;
}

}; // namespace nvkmsDisplayPort
