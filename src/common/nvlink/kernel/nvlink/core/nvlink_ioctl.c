/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvVer.h"
#include "nvlink_os.h"
#include "nvlink_lib_ctrl.h"
#include "../nvlink_ctx.h"
#include "../nvlink_helper.h"

static nvlink_device_type
_nvlink_core_map_device_type
(
    NvU64 type
)
{
    nvlink_device_type devType;

    switch (type)
    {
        case NVLINK_DEVICE_TYPE_IBMNPU:
            devType = nvlink_device_type_ibmnpu;
            break;
        case NVLINK_DEVICE_TYPE_GPU:
            devType = nvlink_device_type_gpu;
            break;
        case NVLINK_DEVICE_TYPE_NVSWITCH:
            devType = nvlink_device_type_nvswitch;
            break;
        default:
            devType = nvlink_device_type_unknown;
            break;
    }

    return devType;
}

static nvlink_link_mode
_nvlink_core_map_link_state
(
    NvU64 dlState,
    NvU64 tlState
)
{
    nvlink_link_mode mode;

    //
    // If TL has entered contain, return contain.
    // Otherwise, return DL state
    //
    if (tlState == NVLINK_LINKSTATE_CONTAIN)
    {
        mode = nvlink_link_mode_contain;
        return mode;
    }

    switch (dlState)
    {
        case NVLINK_LINKSTATE_OFF:
            mode = nvlink_link_mode_off;
            break;
        case NVLINK_LINKSTATE_HS:
            mode = nvlink_link_mode_active;
            break;
        case NVLINK_LINKSTATE_SAFE:
            mode = nvlink_link_mode_swcfg;
            break;
        case NVLINK_LINKSTATE_FAULT:
            mode = nvlink_link_mode_fault;
            break;
        case NVLINK_LINKSTATE_RECOVERY:
            mode = nvlink_link_mode_recovery;
            break;
        case NVLINK_LINKSTATE_FAIL:
            mode = nvlink_link_mode_fail;
            break;
        case NVLINK_LINKSTATE_DETECT:
            mode = nvlink_link_mode_detect;
            break;
        case NVLINK_LINKSTATE_RESET:
            mode = nvlink_link_mode_reset;
            break;
        case NVLINK_LINKSTATE_ENABLE_PM:
            mode = nvlink_link_mode_enable_pm;
            break;
        case NVLINK_LINKSTATE_DISABLE_PM:
            mode = nvlink_link_mode_disable_pm;
            break;
        case NVLINK_LINKSTATE_TRAFFIC_SETUP:
            mode = nvlink_link_mode_traffic_setup;
            break;
        default:
            mode = nvlink_link_mode_unknown;
            break;
    }

    return mode;
}

static nvlink_tx_sublink_mode
_nvlink_core_map_tx_sublink_state
(
    NvU64 state
)
{
    nvlink_tx_sublink_mode mode;

    switch (state)
    {
        case NVLINK_SUBLINK_STATE_TX_HS:
            mode = nvlink_tx_sublink_mode_hs;
            break;
        case NVLINK_SUBLINK_STATE_TX_SINGLE_LANE:
            mode = nvlink_tx_sublink_mode_single_lane;
            break;
        case NVLINK_SUBLINK_STATE_TX_TRAIN:
            mode = nvlink_tx_sublink_mode_train;
            break;
        case NVLINK_SUBLINK_STATE_TX_SAFE:
            mode = nvlink_tx_sublink_mode_safe;
            break;
        case NVLINK_SUBLINK_STATE_TX_OFF:
            mode = nvlink_tx_sublink_mode_off;
            break;
        case NVLINK_SUBLINK_STATE_TX_COMMON_MODE:
            mode = nvlink_tx_sublink_mode_common_mode;
            break;
        case NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE:
            mode = nvlink_tx_sublink_mode_common_mode_disable;
            break;
        case NVLINK_SUBLINK_STATE_TX_DATA_READY:
            mode = nvlink_tx_sublink_mode_data_ready;
            break;
        case NVLINK_SUBLINK_STATE_TX_EQ:
            mode = nvlink_tx_sublink_mode_tx_eq;
            break;
        case NVLINK_SUBLINK_STATE_TX_PRBS_EN:
            mode = nvlink_tx_sublink_mode_pbrs_en;
            break;
        case NVLINK_SUBLINK_STATE_TX_POST_HS:
            mode = nvlink_tx_sublink_mode_post_hs;
            break;
        default:
            mode = nvlink_tx_sublink_mode_unknown;
            break;
    }

    return mode;
}

static nvlink_rx_sublink_mode
_nvlink_core_map_rx_sublink_state
(
    NvU64 state
)
{
    nvlink_rx_sublink_mode mode;

    switch (state)
    {
        case NVLINK_SUBLINK_STATE_RX_HS:
            mode = nvlink_rx_sublink_mode_hs;
            break;
        case NVLINK_SUBLINK_STATE_RX_SINGLE_LANE:
            mode = nvlink_rx_sublink_mode_single_lane;
            break;
        case NVLINK_SUBLINK_STATE_RX_TRAIN:
            mode = nvlink_rx_sublink_mode_train;
            break;
        case NVLINK_SUBLINK_STATE_RX_SAFE:
            mode = nvlink_rx_sublink_mode_safe;
            break;
        case NVLINK_SUBLINK_STATE_RX_OFF:
            mode = nvlink_rx_sublink_mode_off;
            break;
        case NVLINK_SUBLINK_STATE_RX_RXCAL:
            mode = nvlink_rx_sublink_mode_rxcal;
            break;
        default:
            mode = nvlink_rx_sublink_mode_unknown;
            break;
    }

    return mode;
}

/**
 * Check if the link is already initialized
 *
 * Note: A link is initialized if it is in SWCFG or ACTIVE state
 *
 * @param[in]  linkMode   Link state
 * 
 * NvBool
 */
static NvBool
_nvlink_core_is_link_initialized
(
    NvU64 linkMode
)
{
    if ((linkMode == NVLINK_LINKSTATE_SAFE) ||
        (linkMode == NVLINK_LINKSTATE_HS))
    {
        return NV_TRUE;
    }
    else
    {
        return NV_FALSE;
    }
}

/**
 * Get the mask of enabled links for the device
 *
 * @param[in]  dev   nvlink_device pointer
 * 
 * NvU64
 */
static NvU64
_nvlink_core_get_enabled_link_mask
(
    nvlink_device *dev
)
{
    NvU64        linkMask = 0x0;
    nvlink_link *link     = NULL;

    nvListForEachEntry(link, &dev->link_list, node)
    {
        linkMask |= NVBIT64(link->linkNumber);
    }

    return linkMask;
}

/**
 * Check if the device type is supported
 *
 * @param[in]  devType   Device type
 * 
 * NvBool
 */
NvBool
nvlink_core_is_supported_device_type
(
    NvU32 devType
)
{
    if ((devType == nvlink_device_type_ibmnpu) ||
        (devType == nvlink_device_type_gpu)    ||
        (devType == nvlink_device_type_nvswitch))
    {
        return NV_TRUE;
    }
    else
    {
        return NV_FALSE;
    }
}

/**
 * Get the link and sublink states for the endpoint
 *
 * @param[in]  link      nvlink_device *
 * @param[out] linkState nvlink_link_state *
 */
void
nvlink_core_get_endpoint_state
(
    nvlink_link       *link,
    nvlink_link_state *linkState
)
{
    NvlStatus status            = NVL_SUCCESS;
    NvU32     txSubLinkSubstate = NVLINK_SUBLINK_SUBSTATE_TX_STABLE;
    NvU32     rxSubLinkSubState = NVLINK_SUBLINK_SUBSTATE_RX_STABLE;
    NvU64     state = NVLINK_LINKSTATE_INVALID;
    NvU64     dlState = NVLINK_LINKSTATE_INVALID;
    NvU64     tlState = NVLINK_LINKSTATE_INVALID;
    NvU64     cciState = NVLINK_LINKSTATE_INVALID;
    if ((link == NULL) || (linkState == NULL))
    {
        return;
    }

    //
    // This is a best case effort to return the current state of the link
    // to user as part of the ioctl call. Typically, this call should succeed
    // unless the corresponding HAL/Callbacks are not registered, which can
    // happen during early development cycle. Adding an assert to catch that
    // in debug builds.
    //    

    status = link->link_handlers->get_dl_link_mode(link, &dlState);
    nvlink_assert(status == NVL_SUCCESS);

    link->link_handlers->get_tl_link_mode(link, &tlState);

    linkState->linkMode = _nvlink_core_map_link_state(dlState, tlState);

    // CCI link training in progress
    link->link_handlers->get_cci_link_mode(link, &cciState);
    if (cciState == NVLINK_LINKSTATE_TRAINING_CCI)
    {
        linkState->linkMode = nvlink_link_mode_training_cci;
    }

    status = link->link_handlers->get_tx_mode(link,
                                              &state,
                                              &txSubLinkSubstate);
    nvlink_assert(status == NVL_SUCCESS);

    linkState->txSubLinkMode = _nvlink_core_map_tx_sublink_state(state);

    status = link->link_handlers->get_rx_mode(link,
                                              &state,
                                              &rxSubLinkSubState);
    nvlink_assert(status == NVL_SUCCESS);

    linkState->rxSubLinkMode = _nvlink_core_map_rx_sublink_state(state);
}

/**
 * Get the nvlink_device * from the PCI DBDF
 *
 * @param[in]  devInfo  PCI Information
 * @param[out] dev      nvlink_device *
 */
void
nvlink_core_get_device_by_devinfo
(
    nvlink_device_info  *devInfo,
    nvlink_device      **dev
)
{
    nvlink_device *tmpDev = NULL;

    if ((devInfo == NULL) || (dev == NULL))
    {
        return;
    }

    FOR_EACH_DEVICE_REGISTERED(tmpDev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        if ( (tmpDev->nodeId           == devInfo->nodeId)         &&
             (tmpDev->pciInfo.domain   == devInfo->pciInfo.domain) &&
             (tmpDev->pciInfo.bus      == devInfo->pciInfo.bus)    &&
             (tmpDev->pciInfo.device   == devInfo->pciInfo.device) &&
             (tmpDev->pciInfo.function == devInfo->pciInfo.function))
        {
            *dev = tmpDev;
            return;
        }
    }

    // not found any matching device
    *dev = NULL;
}

/**
 * Get the nvlink_link * from the PCI DBDF and link#
 *
 * @param[in]  endPoint  PCI Information and link#
 * @param[out] link      nvlink_link *
 */
void
nvlink_core_get_link_by_endpoint
(
    nvlink_endpoint  *endPoint,
    nvlink_link     **link
)
{
    nvlink_device *tmpDev  = NULL;
    nvlink_link   *tmpLink = NULL;

    if ((endPoint == NULL) || (link == NULL))
    {
        return;
    }

    FOR_EACH_DEVICE_REGISTERED(tmpDev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        if ((tmpDev->nodeId           == endPoint->nodeId)         &&
            (tmpDev->pciInfo.domain   == endPoint->pciInfo.domain) &&
            (tmpDev->pciInfo.bus      == endPoint->pciInfo.bus)    &&
            (tmpDev->pciInfo.device   == endPoint->pciInfo.device) &&
            (tmpDev->pciInfo.function == endPoint->pciInfo.function))
        {
            FOR_EACH_LINK_REGISTERED(tmpLink, tmpDev, node)
            {
                if (tmpLink->linkNumber == endPoint->linkIndex)
                {
                    *link = tmpLink;
                    return;
                }
            }
        }
    }

    // not found any matching link
    *link = NULL;
}

/**
 * Given the nvlink_link ptr, copy the endpoint details for the link
 *
 * @param[in]   connLink      nvlink_link *
 * @param[out]  endPointInfo  Endpoint details for the link
 */
void
nvlink_core_copy_endpoint_info
(
    nvlink_link     *connLink, 
    nvlink_endpoint *endPointInfo
)
{
    if ((connLink == NULL) || (endPointInfo == NULL))
    {
        return;
    }

    nvlink_device *dev = connLink->dev;

    endPointInfo->pciInfo.domain   = dev->pciInfo.domain;
    endPointInfo->pciInfo.bus      = dev->pciInfo.bus;
    endPointInfo->pciInfo.device   = dev->pciInfo.device;
    endPointInfo->pciInfo.function = dev->pciInfo.function;
    endPointInfo->nodeId           = dev->nodeId;
    endPointInfo->linkIndex        = connLink->linkNumber;
}

/**
 * Given the nvlink_device ptr, copy the device details
 *
 * @param[in]   tmpDev   nvlink_device *
 * @param[out]  devInfo  Device details
 */
void
nvlink_core_copy_device_info
(
    nvlink_device            *tmpDev, 
    nvlink_detailed_dev_info *devInfo
)
{
    if ((tmpDev == NULL) || (devInfo == NULL))
    {
        return;
    }

    devInfo->pciInfo.domain   = tmpDev->pciInfo.domain;
    devInfo->pciInfo.bus      = tmpDev->pciInfo.bus;
    devInfo->pciInfo.device   = tmpDev->pciInfo.device;
    devInfo->pciInfo.function = tmpDev->pciInfo.function;
    devInfo->numLinks         = nvListCount(&tmpDev->link_list);
    devInfo->devType          = _nvlink_core_map_device_type(tmpDev->type);
    devInfo->enabledLinkMask  = _nvlink_core_get_enabled_link_mask(tmpDev);
    devInfo->bEnableAli       = tmpDev->enableALI;

    // copy device uuid information if available
    if (tmpDev->uuid != NULL)
    {
        nvlink_memcpy(devInfo->devUuid, tmpDev->uuid, NVLINK_UUID_LEN);
    }

    // copy device name information if available
    if (tmpDev->deviceName != NULL)
    {
        int nameLen = nvlink_strlen(tmpDev->deviceName);
        int copyLen = 0;
        copyLen     = (nameLen > NVLINK_DEVICE_NAME_LEN_MAX) ? NVLINK_DEVICE_NAME_LEN_MAX : nameLen;
        nvlink_memcpy(devInfo->deviceName, tmpDev->deviceName, copyLen);
    }
}

/**
 * Transition to SWCFG on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_link_init_async
(
    nvlink_link **links,
    NvU32         numLinks
)
{
    NvU32 i;

    // Sanity check the links array for non-zero links
    if ((links == NULL) || (numLinks == 0))
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail)
        {
            // link did not pass RXDET or failed in common mode, don't do anything
            continue;
        }

        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        // TODO : Handle fault checking
        if (_nvlink_core_is_link_initialized(linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

            // link already in higher state. don't do anything
            continue;
        }

        // Put the link in SAFE state
        links[i]->link_handlers->set_dl_link_mode(links[i],
                                                  NVLINK_LINKSTATE_SAFE,
                                                  NVLINK_STATE_CHANGE_SYNC);
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Generate a discovery token for the given link
 *
 * @param[in]  link  nvlink_link pointer
 *
 * return NvU64
 */
NvU64
nvlink_core_get_link_discovery_token
(
    nvlink_link *link
)
{
    NvU64 token = 0;

    if (link == NULL)
        return token;

    //
    // generate a unique token value for discovering connections.
    // link->token is the memory address of the allocated link object,
    // which is unique within a node. Adding fabric node id
    // to make it unique across different nodes.
    //

    token = link->token & ~((NvU64)NVLINK_FABRIC_NODE_ID_MASK << NVLINK_FABRIC_NODE_ID_POS);
    token = token | ((NvU64)link->dev->nodeId << NVLINK_FABRIC_NODE_ID_POS);
    return token;
}

/**
 * Write the dicovery token for the given link
 *
 * @param[in]  link   nvlink_link pointer
 * @param[in]  token  Discovery token to write
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_write_link_discovery_token
(
    nvlink_link *link,
    NvU64        token
)
{
    NvlStatus status   = NVL_SUCCESS;
    NvU64     linkMode = NVLINK_LINKSTATE_OFF;

    if (link == NULL)
    {
        return NVL_BAD_ARGS;
    }

    // Packet injection can only happen if link is in SWCFG/ACTIVE
    status = link->link_handlers->get_dl_link_mode(link, &linkMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get link mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        return status;
    }

    if (!_nvlink_core_is_link_initialized(linkMode))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Packet injection only works for links in SAFE or HS %s:%s.\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        return NVL_ERR_INVALID_STATE;
    }

    // Send the token (AN0 packet)
    link->link_handlers->write_discovery_token(link, token);

    return NVL_SUCCESS;
}

/**
 * Read the dicovery token for the given link
 *
 * @param[in]  link   nvlink_link pointer
 *
 * return NvU64
 */
NvU64
nvlink_core_read_link_discovery_token
(
    nvlink_link *link
)
{ 
    NvU64     token    = 0;
    NvlStatus status   = NVL_SUCCESS;
    NvU64     linkMode = NVLINK_LINKSTATE_OFF;

    if (link == NULL)
    {
        return 0;
    }

    status = link->link_handlers->get_dl_link_mode(link, &linkMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get link mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        return 0;
    }

    if (!_nvlink_core_is_link_initialized(linkMode))
    {
        return 0;
    }

    // query discovery token from the link
    link->link_handlers->read_discovery_token(link, (NvU64 *) &token);

    return token;
}

/**
 * Detect the connection by correlating the tokens
 *
 * @param[in]  srcLink        nvlink_link pointer
 * @param[in]  writeToken     write token
 * @param[in]  skipReadToken  token vs sid for connection detection
 *
 */
void
nvlink_core_correlate_conn_by_token
(
    nvlink_link *srcLink,
    NvU64        writeToken,
    NvBool       skipReadToken
)
{
    nvlink_device *dev       = NULL;
    nvlink_link   *dstLink   = NULL;
    NvU64          readToken = 0;

    if (srcLink == NULL)
    {
        return;
    }

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(dstLink, dev, node)
        {
            nvlink_intranode_conn *conn = NULL;

            nvlink_core_get_intranode_conn(dstLink, &conn);
            if (conn != NULL)
            {
                // skip token read if we already have a connection for the dstLink
                continue;
            }

            if (skipReadToken)
            {
                if ((srcLink->remoteSid == dstLink->localSid) &&
                    (srcLink->remoteLinkId == dstLink->linkNumber))
                {
                    readToken = writeToken;
                }
            }
            else
            {
                // Read the RX sublink for the AN0 packet
                readToken = nvlink_core_read_link_discovery_token(dstLink);
            }

            // If token matches, establish the connection
            if (writeToken == readToken)
            {
                // Add to the connections list
                nvlink_core_add_intranode_conn(srcLink, dstLink);
                return;
            }
        }
    }
}
