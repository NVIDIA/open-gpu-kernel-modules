/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * For a given link, return the associated intranode connection
 *
 * @param[in]  endpoint  NVLink Link pointer
 * @param[out] conn      Connection associated with the link
 */
void
nvlink_core_get_intranode_conn
(
    nvlink_link            *endpoint,
    nvlink_intranode_conn **conn
)
{
    nvlink_intranode_conn *tmpConn = NULL;

    if ((endpoint == NULL) || (conn == NULL))
    {
        return;
    }

    FOR_EACH_CONNECTION(tmpConn, nvlinkLibCtx.nv_intraconn_head, node)
    {
        if (tmpConn->end0 == endpoint || tmpConn->end1 == endpoint)
        {
            *conn = tmpConn;
            break;
        }
    }
}

/**
 * For a given local link, return the associated internode connection
 *
 * @param[in]  localLink  NVLink Link pointer
 * @param[out] conn       Connection associated with the link
 */
void
nvlink_core_get_internode_conn
(
    nvlink_link            *localLink,
    nvlink_internode_conn **conn
)
{
    nvlink_internode_conn *tmpConn = NULL;

    if ((localLink == NULL) || (conn == NULL))
    {
        return;
    }

    FOR_EACH_CONNECTION(tmpConn, nvlinkLibCtx.nv_interconn_head, node)
    {
        if (tmpConn->local_end == localLink)
        {
            *conn = tmpConn;
            break;
        }
    }
}

/**
 * Add a new intranode connection to the list of connections
 *
 * @param[in] end0  NVLink Link pointer for end0
 * @param[in] end1  NVLink Link pointer for end1
 *
 * return NVL_SUCCESS if the conn was added successfully
 */
NvlStatus
nvlink_core_add_intranode_conn
(
    nvlink_link *end0,
    nvlink_link *end1
)
{
    nvlink_intranode_conn *conn = NULL;

    if ((end0 == NULL) || (end1 == NULL))
    {
        return NVL_BAD_ARGS;
    }

    // don't do anything if we have an intranode connecction
    nvlink_core_get_intranode_conn(end0, &conn);

    if (conn != NULL)
    {
        // Verify that the other end of the connection is indeed end1
        conn->end0 == end0 ?
            nvlink_assert(conn->end1 == end1) :
            nvlink_assert(conn->end0 == end1);
        return NVL_SUCCESS;
    }

    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
        "Adding new NVLink intranode connection between %s:%s and %s:%s\n",
        end0->dev->deviceName, end0->linkName,
        end1->dev->deviceName, end1->linkName));

    // create a new intranode connection object
    conn = (nvlink_intranode_conn*)nvlink_malloc(sizeof(nvlink_intranode_conn));
    if (conn == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "Adding NVLink intranode connection failed "
            "due to memory allocation error\n"));
        return NVL_NO_MEM;
    }

    nvlink_memset(conn, 0, sizeof(nvlink_intranode_conn));

    // Initialize the node for the connection
    nvListInit(&conn->node);

    // Initialize the connection endpoints
    conn->end0 = end0;
    conn->end1 = end1;

    // Add the connection to the list of connections
    nvListAppend(&conn->node, &nvlinkLibCtx.nv_intraconn_head.node);

    //
    // Update the count of connected endpoints
    // Loopback link, increment by 1
    // Non loopback link, increment by 2
    //
    nvlinkLibCtx.connectedEndpoints = ( end0 == end1 ?
                           nvlinkLibCtx.connectedEndpoints + 1:
                           nvlinkLibCtx.connectedEndpoints + 2 );

    return NVL_SUCCESS;
}

/**
 * Add a new internode connection to the list of internode connections
 *
 * Note: As of now, no stats/count for internode connections.
 *
 * @param[in] localLink       NVLink Link pointer for one end
 * @param[in] remoteEndPoint  Remote endpoint
 *
 * return NVL_SUCCESS if the conn was added succesfully
 */
NvlStatus
nvlink_core_add_internode_conn
(
    nvlink_link                 *localLink,
    nvlink_remote_endpoint_info *remoteEndPoint
)
{
    nvlink_internode_conn *conn = NULL;

    if ((localLink == NULL) || (remoteEndPoint == NULL))
    {
        return NVL_BAD_ARGS;
    }

    // Don't do anything if we have an internode connecction for local link
    nvlink_core_get_internode_conn(localLink, &conn);
    if (conn != NULL)
    {
        return NVL_SUCCESS;
    }

    // create a new connection
    conn = (nvlink_internode_conn *)nvlink_malloc(sizeof(nvlink_internode_conn));
    if (conn == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "Adding nvlink internode connection failed"
            " due to memory allocation error\n"));
        return NVL_NO_MEM;
    }

    nvlink_memset(conn, 0, sizeof(nvlink_internode_conn));

    // initialize the node for the connection list
    nvListInit(&conn->node);

    // copy/assign the connection endpoints information
    conn->local_end = localLink;
    nvlink_memcpy(&conn->remote_end,
                  remoteEndPoint,
                  sizeof(nvlink_remote_endpoint_info));

    // add the connection to the list of internode connections
    nvListAppend(&conn->node, &nvlinkLibCtx.nv_interconn_head.node);

    return NVL_SUCCESS;
}

/**
 * Remove the connection from the list of intranode connections
 *
 * @param[in]  conn  NVLink connection pointer
 */
void
nvlink_core_remove_intranode_conn
(
    nvlink_intranode_conn *conn
)
{
    if (conn == NULL)
        return;

    // Remove the connection from the list of connections
    nvListDel(&conn->node);

    //
    // Update the count of connected endpoints
    // Loopback link, decrement by 1
    // Non loopback link, decrement by 2
    //
    nvlinkLibCtx.connectedEndpoints = ( conn->end0 == conn->end1 ?
                           nvlinkLibCtx.connectedEndpoints - 1:
                           nvlinkLibCtx.connectedEndpoints - 2 );

    //
    // Update the count of notConnected endpoints
    // Loopback link, do nothing
    // Non-loopback link, increment by 1
    //
    nvlinkLibCtx.notConnectedEndpoints = ( conn->end0 != conn->end1 ?
                              nvlinkLibCtx.notConnectedEndpoints + 1:
                              nvlinkLibCtx.notConnectedEndpoints );

    nvlink_free((void *)conn);
}

/**
 * Remove the connection from the list of internode connections
 *
 * @param[in]  localLink  NVLink link pointer
 */
void
nvlink_core_remove_internode_conn
(
    nvlink_link *localLink
)
{
    nvlink_internode_conn *conn = NULL;

    if (localLink == NULL)
        return;

    nvlink_core_get_internode_conn(localLink, &conn);

    if (conn != NULL)
    {
        nvListDel(&conn->node);
        nvlink_free((void *)conn);
    }
}

/**
 * Check if the given intranode connection is in the specified mode
 *
 * @param[in]  conn      NVLink Connection pointer
 * @param[in]  linkMode  Link mode
 *
 * return NVL_SUCCESS if the conn is in the given state
 */
NvlStatus
nvlink_core_check_intranode_conn_state
(
    nvlink_intranode_conn *conn,
    NvU64                  linkMode
)
{
    if (conn == NULL)
    {
        return NVL_BAD_ARGS;
    }

    switch (linkMode)
    {
        case NVLINK_LINKSTATE_OFF:
        {
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_OFF)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_OFF)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Link already in OFF state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_SUCCESS;
            }

            // Check if only one end of connection is OFF
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_OFF)) ||
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_OFF)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link is in bad state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_ERR_INVALID_STATE;
            }

            return NVL_ERR_GENERIC;
        }

        case NVLINK_LINKSTATE_RESET:
        {
            if (conn->end0->version >= NVLINK_DEVICE_VERSION_50)
                return NVL_ERR_GENERIC;

            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_RESET)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_RESET)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Link already in RESET state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_SUCCESS;
            }

            // Check if only one end of connection is RESET
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_RESET)) ||
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_RESET)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link is in bad state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_ERR_INVALID_STATE;
            }

            return NVL_ERR_GENERIC;
        }

        case NVLINK_LINKSTATE_SAFE:
        {
            if (conn->end0->version >= NVLINK_DEVICE_VERSION_50)
                return NVL_SUCCESS;

            // Check if both ends and their sublinks are already in SAFE mode
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_SAFE)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_SAFE)))
            {
                if ((nvlink_core_check_tx_sublink_state(conn->end0,
                                                        NVLINK_SUBLINK_STATE_TX_OFF)) &&
                    (nvlink_core_check_tx_sublink_state(conn->end1,
                                                        NVLINK_SUBLINK_STATE_TX_OFF)) &&
                    (nvlink_core_check_rx_sublink_state(conn->end0,
                                                        NVLINK_SUBLINK_STATE_RX_OFF)) &&
                    (nvlink_core_check_rx_sublink_state(conn->end1,
                                                        NVLINK_SUBLINK_STATE_RX_OFF)))
                {
                    //
                    // If links are in safe, check if sublinks are in off
                    // if so, we had performed pseudo-clean shutdown
                    //
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                        "%s: Link is not in SAFE mode. ",
                        __FUNCTION__));
                    nvlink_core_print_intranode_conn(conn);
                    return NVL_ERR_GENERIC;
                }
                else if (!((nvlink_core_check_tx_sublink_state(conn->end0,
                                                         NVLINK_SUBLINK_STATE_TX_SAFE)) &&
                           (nvlink_core_check_tx_sublink_state(conn->end1,
                                                         NVLINK_SUBLINK_STATE_TX_SAFE)) &&
                           (nvlink_core_check_rx_sublink_state(conn->end0,
                                                         NVLINK_SUBLINK_STATE_RX_SAFE)) &&
                           (nvlink_core_check_rx_sublink_state(conn->end1,
                                                         NVLINK_SUBLINK_STATE_RX_SAFE))))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Sublinks are in bad state. ",
                        __FUNCTION__));
                    nvlink_core_print_intranode_conn(conn);
                    return NVL_ERR_INVALID_STATE;
                }

                return NVL_SUCCESS;
            }

            // Check if only one end of connection is in SAFE mode
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_SAFE)) ||
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_SAFE)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link is in bad state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_ERR_INVALID_STATE;
            }

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Link is not in SAFE mode. ",
                __FUNCTION__));
            nvlink_core_print_intranode_conn(conn);
            return NVL_ERR_GENERIC;
        }

        case NVLINK_LINKSTATE_HS:
        {
            // Check if both ends and their sublinks are already in HS mode
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_HS)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_HS)))
            {
                // In NVLINK4.0, corelib doesn't control sublink state transitions
                if (conn->end0->version < NVLINK_DEVICE_VERSION_40 &&
                    !((nvlink_core_check_tx_sublink_state(conn->end0,
                                                         NVLINK_SUBLINK_STATE_TX_HS)) &&
                      (nvlink_core_check_tx_sublink_state(conn->end1,
                                                         NVLINK_SUBLINK_STATE_TX_HS)) &&
                      (nvlink_core_check_rx_sublink_state(conn->end0,
                                                         NVLINK_SUBLINK_STATE_RX_HS)) &&
                      (nvlink_core_check_rx_sublink_state(conn->end1,
                                                         NVLINK_SUBLINK_STATE_RX_HS))))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Sublinks are in bad state. ",
                        __FUNCTION__));
                    nvlink_core_print_intranode_conn(conn);
                    return NVL_ERR_INVALID_STATE;
                }

                return NVL_SUCCESS;
            }

            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_HS)) ||
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_HS)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link is in bad state. ",
                    __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);
                return NVL_ERR_INVALID_STATE;
            }

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Link is not in HIGH SPEED mode. ",
                __FUNCTION__));
            nvlink_core_print_intranode_conn(conn);
            return NVL_ERR_GENERIC;
        }

        case NVLINK_LINKSTATE_SLEEP:
        {
            // Check if both ends of connection are already in SLEEP mode
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_SLEEP)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_SLEEP)))
            {
                return NVL_SUCCESS;
            }

            // Check if only one end of connection is in SLEEP mode
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_SLEEP)) ||
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_SLEEP)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link is in bad state. ",
                    __FUNCTION__));
                    nvlink_core_print_intranode_conn(conn);
                return NVL_ERR_INVALID_STATE;
            }

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Link is not in SLEEP mode. ",
                __FUNCTION__));
                nvlink_core_print_intranode_conn(conn);

            return NVL_ERR_GENERIC;
        }
        case NVLINK_LINKSTATE_ACTIVE_PENDING:
        {
            // Check if both ends of connection are already in ACTIVE_PENDING
            if ((nvlink_core_check_link_state(conn->end0, NVLINK_LINKSTATE_ACTIVE_PENDING)) &&
                (nvlink_core_check_link_state(conn->end1, NVLINK_LINKSTATE_ACTIVE_PENDING)))
            {
                return NVL_SUCCESS;
            }
            break;
        }
    }

    return NVL_SUCCESS;
}

/**
 * Copy the intranode connection's remote endpoint information into
 * the nvlink_conn_info structure passed in
 *
 * @param[in]  remote_end  NVLink Link pointer
 * @param[in]  conn_info   Details of remote endpoint
 */
void
nvlink_core_copy_intranode_conn_info
(
    nvlink_link      *remote_end,
    nvlink_conn_info *conn_info
)
{
    if ((remote_end == NULL) || (conn_info == NULL))
    {
        return;
    }

    // copy the remote device pci information
    conn_info->domain      = remote_end->dev->pciInfo.domain;
    conn_info->bus         = remote_end->dev->pciInfo.bus;
    conn_info->device      = remote_end->dev->pciInfo.device;
    conn_info->function    = remote_end->dev->pciInfo.function;
    conn_info->pciDeviceId = remote_end->dev->pciInfo.pciDeviceId;
    conn_info->chipSid     = remote_end->localSid;

    // copy the device type
    conn_info->deviceType = remote_end->dev->type;

    // copy the remote device uuid
    if (remote_end->dev->uuid != NULL)
    {
        nvlink_memcpy(conn_info->devUuid, remote_end->dev->uuid, NV_UUID_LEN);
    }

    // copy the remote link number
    conn_info->linkNumber = remote_end->linkNumber;
}

/**
 * Copy the internode connection's remote endpoint information into
 * the nvlink_conn_info structure passed in
 *
 * @param[in]  remote_end  NVLink Link pointer
 * @param[in]  conn_info   Details of remote endpoint
 */
void
nvlink_core_copy_internode_conn_info
(
    nvlink_remote_endpoint_info *remote_end,
    nvlink_conn_info            *conn_info
)
{
    if ((remote_end == NULL) || (conn_info == NULL))
    {
        return;
    }

    // copy the remote device pci information
    conn_info->domain      = remote_end->pciInfo.domain;
    conn_info->bus         = remote_end->pciInfo.bus;
    conn_info->device      = remote_end->pciInfo.device;
    conn_info->function    = remote_end->pciInfo.function;
    conn_info->pciDeviceId = 0;

    // copy the device type
    conn_info->deviceType = remote_end->devType;

    // copy the remote device uuid
    nvlink_memcpy(conn_info->devUuid, remote_end->devUuid, NV_UUID_LEN);

    // copy the remote link number
    conn_info->linkNumber = remote_end->linkIndex;
}
