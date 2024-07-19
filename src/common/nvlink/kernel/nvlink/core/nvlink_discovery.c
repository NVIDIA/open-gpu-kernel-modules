/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink.h"
#include "nvlink_export.h"
#include "nvlink_os.h"
#include "../nvlink_ctx.h"
#include "../nvlink_helper.h"

static NvBool _nvlink_core_all_links_initialized(void);
static void   _nvlink_core_discover_topology(void);

/**
 * Get the remote end of the link
 *
 *   For a given end of a link, returns the other end its connected to.
 *
 * Note: This function shouldn't be called when external fabric management is
 *       enabled in the endpoint drivers. Unfortunately, there is no graceful
 *       way to know that in the NVLink driver beforehand (during module load).
 *
 * @param[in]  end         NVLink Link pointer
 * @param[out] remote_end  Remote endpoint of the connection
 * @param[in]  flags       Flags
 */
void
nvlink_core_discover_and_get_remote_end
(
    nvlink_link  *end,
    nvlink_link **remote_end,
    NvU32         flags,
    NvBool        bForceDiscovery
)
{
    nvlink_intranode_conn *conn      = NULL;
    nvlink_device         *dev       = NULL;
    nvlink_link           *link      = NULL;
    NvU32                  linkCount = 0;

    if ((end == NULL) || (remote_end == NULL))
    {
        return;
    }

    nvlink_link   **pLinks = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (pLinks == NULL)
    {
        return;
    }

    if (nvlinkLibCtx.bNewEndpoints || bForceDiscovery)
    {
        if (!_nvlink_core_all_links_initialized())
        {
            // Initialize the links to SWCFG mode
            FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
            {
                FOR_EACH_LINK_REGISTERED(link, dev, node)
                {
                    if (linkCount >= NVLINK_MAX_SYSTEM_LINK_NUM)
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                            "%s: linkCount >= NVLINK_MAX_SYSTEM_LINK_NUM",
                            __FUNCTION__));

                        nvlink_assert(0);
                        nvlink_free((void *)pLinks);
                        return;
                    }

                    pLinks[linkCount++] = link;
                }
            }

            if (pLinks[0]->version >= NVLINK_DEVICE_VERSION_40)
            {
                if (!pLinks[0]->dev->enableALI)
                {
                    nvlink_core_init_links_from_off_to_swcfg_non_ALI(pLinks, linkCount, flags);
                }
            }
            else
            {
                nvlink_core_init_links_from_off_to_swcfg(pLinks, linkCount, flags);
            }
        }

        // Re-discover the nvlink topology
        _nvlink_core_discover_topology();
    }

    // Get the connection for the endpoint
    nvlink_core_get_intranode_conn(end, &conn);

    if (conn != NULL)
    {
        *remote_end = (conn->end0 == end ? conn->end1 : conn->end0);
    }

    if (pLinks != NULL)
    {
        nvlink_free((void *) pLinks);
    }
}

/**
 * Discovery process to determine topology
 *
 *    Involves sending and reading back AN0 packets/SID values
 */
static void
_nvlink_core_discover_topology(void)
{
    nvlink_device         *dev0         = NULL;
    nvlink_device         *dev1         = NULL;
    nvlink_link           *end0         = NULL;
    nvlink_link           *end1         = NULL;
    nvlink_intranode_conn *conn         = NULL;
    NvU64                  linkMode     = NVLINK_LINKSTATE_OFF;
    NvBool                 isTokenFound = NV_FALSE;
    NvU64                  token        = 0;

    nvlinkLibCtx.notConnectedEndpoints = 0;

    FOR_EACH_DEVICE_REGISTERED(dev0, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(end0, dev0, node)
        {
            //
            // If receiver detect failed for the link or if clocks could not be set
            // up for the link, then move to next link
            //
            if (!end0->bRxDetected || end0->bTxCommonModeFail)
                continue;

            conn = NULL;
            nvlink_core_get_intranode_conn(end0, &conn);
            if (conn != NULL)
            {
                continue;
            }

            if (end0->packet_injection_retries > NVLINK_MAX_NUM_PACKET_INJECTION_RETRIES)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Packet injection retries reached for %s:%s.\n",
                    __FUNCTION__, end0->dev->deviceName, end0->linkName));
                nvlinkLibCtx.notConnectedEndpoints++;
                continue;
            }

            end0->link_handlers->get_dl_link_mode(end0, &linkMode);

            // Packet injection can only happen on links that are in SAFE or ACTIVE
            if (!((linkMode == NVLINK_LINKSTATE_SAFE) || (linkMode == NVLINK_LINKSTATE_HS)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Packet injection only works for links in SAFE or HS %s:%s.\n",
                    __FUNCTION__, end0->dev->deviceName, end0->linkName));
                nvlinkLibCtx.notConnectedEndpoints++;
                continue;
            }

            //
            // Send the AN0 packet
            // For Nvlink3.0, token mechanism is handled by Minion.
            // SW gets Sids values and so write_disocvery_token is Stubbed for Nvlink 3.0
            // We use the return value of write_discovery_token to shift between 
            // Nvlink2.0 and NvLink3.0
            //
            if ((end0->version < NVLINK_DEVICE_VERSION_30) ||
                ((end0->localSid == 0) || (end0->remoteSid == 0)))
            {
                end0->link_handlers->write_discovery_token(end0, end0->token);
            }
            end0->packet_injection_retries++;
            isTokenFound = NV_FALSE;

            FOR_EACH_DEVICE_REGISTERED(dev1, nvlinkLibCtx.nv_devicelist_head, node)
            {
                FOR_EACH_LINK_REGISTERED(end1, dev1, node)
                {
                    //
                    // If receiver detect failed for the link or if clocks could not be
                    // set up for the link, then move to next link
                    //
                    if (!end1->bRxDetected || end1->bTxCommonModeFail)
                        continue;

                    token = 0;

                    if ((end0->version >= NVLINK_DEVICE_VERSION_30) &&
                        (end0->localSid != 0) && (end0->remoteSid != 0))
                    {
                        if ((end0->remoteSid    == end1->localSid) &&
                            (end0->remoteLinkId == end1->linkNumber))
                        {
                            // Make sure the below token check passes.
                            token = end0->token;
                        }
                    }
                    else
                    {
                        // Read the RX sublink for the AN0 packet
                        end1->link_handlers->read_discovery_token(end1, (NvU64 *) &token);
                    }

                    // If token matches, establish the connection
                    if (token == end0->token)
                    {
                        isTokenFound = NV_TRUE;

                        //
                        // If R4 tokens were used for NVLink3.0+, then mark initnegotiate
                        // passed, since ALT training won't get kicked off without it.
                        //
                        if ((end0->version >= NVLINK_DEVICE_VERSION_30) &&
                            ((end0->localSid == 0) || (end0->remoteSid == 0)))
                        {
                            end0->bInitnegotiateConfigGood = NV_TRUE;
                            end1->bInitnegotiateConfigGood = NV_TRUE;
                        }

                        // Add to the connections list
                        nvlink_core_add_intranode_conn(end0, end1);
                        break;
                    }
                }

                if (isTokenFound) break;
            }

            if (nvlinkLibCtx.connectedEndpoints ==
               (nvlinkLibCtx.registeredEndpoints - nvlinkLibCtx.notConnectedEndpoints))
            {
                break;
            }
        }

        if (nvlinkLibCtx.connectedEndpoints ==
           (nvlinkLibCtx.registeredEndpoints - nvlinkLibCtx.notConnectedEndpoints))
        {
            break;
        }
    }
}

/**
 * Are all links trained or is there a need to re-attempt training ?
 *
 *   Returns true if all links trained and no need to re-attempt training
 *   Returns false otherwise
 */
static NvBool
_nvlink_core_all_links_initialized(void)
{
    nvlink_device *dev    = NULL;
    nvlink_link   *link   = NULL;
    NvU64          linkMode, txMode, rxMode;
    NvU32          txSubMode, rxSubMode;

    if (nvlinkLibCtx.registeredEndpoints == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: No links registered with nvlink core! Sleeping.\n",
            __FUNCTION__));
        return NV_TRUE;
    }

    nvlinkLibCtx.endpointsInFail   = 0;
    nvlinkLibCtx.endpointsInSafe   = 0;
    nvlinkLibCtx.endpointsInActive = 0;

    //
    // Get the current state of all endpoints. This determines
    // if some of the endpoints are still not trained to SAFE
    //
    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(link, dev, node)
        {
            if (nvlinkLibCtx.bNewEndpoints)
            {
                link->safe_retries             = 0;
                link->packet_injection_retries = 0;
            }

            if (link->state == NVLINK_LINKSTATE_FAIL)
            {
                if (nvlinkLibCtx.bNewEndpoints)
                {
                    //
                    // New endpoints were detected. There may be a chance that
                    // endpoints that failed previously may transition to safe
                    //
                    link->state = NVLINK_LINKSTATE_OFF;
                }
                else
                {
                    nvlinkLibCtx.endpointsInFail++;
                }
                continue;
            }

            linkMode = NVLINK_LINKSTATE_OFF;

            if (link->link_handlers->get_dl_link_mode(link, &linkMode))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get link mode for %s:%s",
                    __FUNCTION__, link->dev->deviceName, link->linkName));
                continue;
            }

            if (linkMode == NVLINK_LINKSTATE_SAFE)
            {

                //
                // Link is only truly in SAFE mode if link state and sublink state
                // is in SAFE/SWCFG.
                // After pseudo-clean shutdown, sublinks are in OFF, so they
                // need to be retrained to SAFE
                //
                if (link->link_handlers->get_tx_mode(link, &txMode, &txSubMode))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Unable to get tx sublink mode for %s:%s",
                        __FUNCTION__, link->dev->deviceName, link->linkName));
                }
                if (link->link_handlers->get_rx_mode(link, &rxMode, &rxSubMode))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Unable to get rx sublink mode for %s:%s",
                        __FUNCTION__, link->dev->deviceName, link->linkName));
                }

                if ((txMode == NVLINK_SUBLINK_STATE_TX_OFF) ||
                    (rxMode == NVLINK_SUBLINK_STATE_RX_OFF))
                {
                    continue;
                }

                link->bRxDetected = NV_TRUE;
                nvlinkLibCtx.endpointsInSafe++;
                continue;
            }

            if (linkMode == NVLINK_LINKSTATE_HS)
            {
                link->bRxDetected = NV_TRUE;
                nvlinkLibCtx.endpointsInActive++;
                continue;
            }
        }
    }

    // New endpoints have been considered
    nvlinkLibCtx.bNewEndpoints = NV_FALSE;

    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
        "%s: Registered Links = %d, nvlinkLibCtx.endpointsInSafe = %d, "
        " nvlinkLibCtx.endpointsInFail = %d, nvlinkLibCtx.endpointsInActive = %d\n",
        __FUNCTION__,
        nvlinkLibCtx.registeredEndpoints, nvlinkLibCtx.endpointsInSafe,
        nvlinkLibCtx.endpointsInFail, nvlinkLibCtx.endpointsInActive));

    // Determine if all links are currently trained
    if ((nvlinkLibCtx.registeredEndpoints - nvlinkLibCtx.endpointsInFail -
         nvlinkLibCtx.endpointsInSafe - nvlinkLibCtx.endpointsInActive) == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: All connected links are in trained\n",
            __FUNCTION__));
        return NV_TRUE;
    }

    return NV_FALSE;
}
