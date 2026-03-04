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

static void _nvlink_core_set_sublink_pre_hs_settings(nvlink_link *, NvU32);
static void _nvlink_core_set_link_pre_active_settings(nvlink_link *, NvU32);
static void _nvlink_core_set_link_post_active_settings(nvlink_link *, NvU32);

NvlStatus
nvlink_core_train_check_link_ready_ALI
(
    nvlink_link **links,
    NvU32         linkCount
)
{
    NvU32     i      = 0;
    NvlStatus status = NVL_SUCCESS;

    if (links == NULL)
    {
        return NVL_BAD_ARGS;
    }

    for (i = 0; i < linkCount; i++)
    {
        if (links[i] == NULL)
            continue;

        if (links[i]->bCciManaged)
        {
            continue;
        }

        if (!nvlink_core_check_link_state(links[i], NVLINK_LINKSTATE_ALI))
        {
            // If link is not in active, update status to be error and continue
            status = NVL_ERR_GENERIC;
            continue;
        }

        links[i]->link_handlers->training_complete(links[i]);
    }

    return status;
}

/**
 * Link training
 *   Train the internode connection link from SWCFG to ACTIVE
 *
 * @param[in]  conn         NVLink connection pointer
 * @param[in]  isMasterEnd  Is this the master end of the connection
 * @param[in]  flags        Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the link trains successfully
 */
NvlStatus
nvlink_core_train_internode_conns_from_swcfg_to_active
(
    nvlink_internode_conn **conns,
    NvU32                   connCount,
    NvU32                  *isMasterEnd,
    NvU32                   flags
)
{
    NvlStatus status     = NVL_SUCCESS;
    NvU32     i;
    NvBool    skipConn[NVLINK_MAX_SYSTEM_LINK_NUM] = {0};

    if ((conns == NULL) || (connCount == 0) || (isMasterEnd == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to train to ACTIVE\n",
            __FUNCTION__));

        return NVL_BAD_ARGS;
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        // Don't do anything if the link is already at HS.
        if ((nvlink_core_check_link_state(conns[i]->local_end, NVLINK_LINKSTATE_HS)) &&
            (nvlink_core_check_tx_sublink_state(conns[i]->local_end,
                                                NVLINK_SUBLINK_STATE_TX_HS)) &&
            (nvlink_core_check_rx_sublink_state(conns[i]->local_end,
                                                NVLINK_SUBLINK_STATE_RX_HS)))
        {
            //
            // Note: On NVLink version < 3.0, bufferready is set prior to link state
            //       change to ACTIVE. So, return early. For NVLink version >= 3.0, 
            //       bufferready is only set after link is ACTIVE. Hence, proceed to
            //       the subsequent code
            //
            if (conns[i]->local_end->version < NVLINK_DEVICE_VERSION_30)
            {
                skipConn[i] = NV_TRUE;
            }
        }

        //
        // For NVLink version < 3.0, we can train link to ACTIVE only when link is at
        // SWCFG and sublink are at HS
        //
        if (conns[i]->local_end->version < NVLINK_DEVICE_VERSION_30)
        {
            if (!(nvlink_core_check_link_state(conns[i]->local_end, NVLINK_LINKSTATE_SAFE)) ||
                !(nvlink_core_check_tx_sublink_state(conns[i]->local_end,
                                                     NVLINK_SUBLINK_STATE_TX_HS)) ||
                !(nvlink_core_check_rx_sublink_state(conns[i]->local_end,
                                                     NVLINK_SUBLINK_STATE_RX_HS)))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Invalid link/sublink mode while training link to HIGH SPEED"
                    " %s:%s \n",
                    __FUNCTION__,
                conns[i]->local_end->dev->deviceName, conns[i]->local_end->linkName));
                nvlink_core_print_link_state(conns[i]->local_end);
                skipConn[i] = NV_TRUE;
            }
        }
    }

    for (i = 0; i < connCount; i++)
    {
        if ((conns[i] == NULL) || skipConn[i])
        {
            continue;
        }

        _nvlink_core_set_link_pre_active_settings(conns[i]->local_end, flags);

        // Change mode for master link. The other link end should transition to active.
        if (isMasterEnd[i] == NV_TRUE)
        {
            conns[i]->local_end->link_handlers->set_dl_link_mode(conns[i]->local_end,
                                                             NVLINK_LINKSTATE_HS,
                                                             flags);
        }
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        // Wait for the link state to change.
        status = nvlink_core_poll_link_state(conns[i]->local_end,
                                             NVLINK_LINKSTATE_HS,
                                             NVLINK_TRANSITION_HS_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set link state to ACTIVE for link"
                " %s:%s \n",
                __FUNCTION__,
                conns[i]->local_end->dev->deviceName, conns[i]->local_end->linkName));
        }
        else
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_SETUP,
                "%s: Successfully able to set link state to ACTIVE for link"
                " %s:%s \n",
                __FUNCTION__,
                conns[i]->local_end->dev->deviceName, conns[i]->local_end->linkName));
        }

        // Do all the miscellaneous settings once the link is trained to ACTIVE.
        _nvlink_core_set_link_post_active_settings(conns[i]->local_end, flags);
    }

    //
    // Always return success to FM on training failures
    // FM will read link states to determine sucessfull training
    //
    return NVL_SUCCESS;
}

/**
 * Link training
 *   Train the internode connection sublink to enter high speed
 *
 * @param[in]  conn   NVLink connection pointer
 * @param[in]  flags  Flags to track if the training is sync/async
 *
 * return NVL_SUCCESS if the sublink trained successfully
 */
NvlStatus
nvlink_core_train_internode_conn_sublink_from_safe_to_hs
(
    nvlink_internode_conn *conn,
    NvU32                  flags
)
{
    NvlStatus status = NVL_SUCCESS;

    if (conn == NULL)
    {
        return NVL_BAD_ARGS;
    }

    // NVLink 3.0 onwards this is handled through INITOPTIMIZE, return error
    if (conn->local_end->version >= NVLINK_DEVICE_VERSION_30)
    {
        return NVL_ERR_NOT_SUPPORTED;
    }

    _nvlink_core_set_sublink_pre_hs_settings(conn->local_end, flags);

    // don't do anything if the link is already at HS.
    if ((nvlink_core_check_link_state(conn->local_end, NVLINK_LINKSTATE_HS)) &&
        (nvlink_core_check_tx_sublink_state(conn->local_end,
                                            NVLINK_SUBLINK_STATE_TX_HS)) &&
        (nvlink_core_check_rx_sublink_state(conn->local_end,
                                            NVLINK_SUBLINK_STATE_RX_HS)))
    {
        // both link and sublinks are at HS. don't do anything.
        return NVL_SUCCESS;
    }

    // we can train sublink to HS only when link is at SWCFG.
    if (!nvlink_core_check_link_state(conn->local_end, NVLINK_LINKSTATE_SAFE))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Invalid link mode while training sublink to HIGH SPEED"
            " %s:%s \n",
            __FUNCTION__,
            conn->local_end->dev->deviceName, conn->local_end->linkName));
        nvlink_core_print_link_state(conn->local_end);
        return NVL_ERR_INVALID_STATE;
    }

    // tx sublink state must be in SAFE as well.
    if (!nvlink_core_check_tx_sublink_state(conn->local_end,
                                            NVLINK_SUBLINK_STATE_TX_SAFE))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Invalid tx sublink mode while training sublink to HIGH SPEED"
            " %s:%s \n",
            __FUNCTION__,
            conn->local_end->dev->deviceName, conn->local_end->linkName));
        nvlink_core_print_link_state(conn->local_end);
        return NVL_ERR_INVALID_STATE;
    }

    //
    // rx sublink state may be in SAFE mode or in HS, if the other end of the
    // connection already toggled tx sublink mode to HS.
    //
    if (!((nvlink_core_check_rx_sublink_state(conn->local_end,
                                              NVLINK_SUBLINK_STATE_RX_SAFE)) ||
          (nvlink_core_check_rx_sublink_state(conn->local_end,
                                              NVLINK_SUBLINK_STATE_RX_HS))))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Invalid rx sublink mode while training sublink to HIGH SPEED"
            " %s:%s \n",
            __FUNCTION__,
            conn->local_end->dev->deviceName, conn->local_end->linkName));
        nvlink_core_print_link_state(conn->local_end);
        return NVL_ERR_INVALID_STATE;
    }

    // Put TX sublink in HS
    conn->local_end->link_handlers->set_tx_mode(conn->local_end,
                                                NVLINK_SUBLINK_STATE_TX_HS,
                                                flags);

    // Wait for sublink to go in HS.
    status = nvlink_core_poll_tx_sublink_state(conn->local_end,
                                               NVLINK_SUBLINK_STATE_TX_HS,
                                               NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                               NVLINK_TRANSITION_HS_TIMEOUT);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to set sublink state to HIGH SPEED for link"
            " %s:%s \n",
            __FUNCTION__,
            conn->local_end->dev->deviceName, conn->local_end->linkName));
        return status;
    }

    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_SETUP,
        "%s:Successfully able to set sublink state to HIGH SPEED for link"
        " %s:%s \n",
        __FUNCTION__,
        conn->local_end->dev->deviceName, conn->local_end->linkName));

    return status;
}

/**
 * Train a given set of intranode connections from L2 to ACTIVE state
 *
 * @param[in]  conns      Array of connections to train
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the connections train successfully
 */
NvlStatus
nvlink_core_train_intranode_conns_from_from_L2_to_active
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus status   = NVL_SUCCESS;
    NvU64     linkMode = NVLINK_LINKSTATE_OFF;
    NvU32     i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to exit L2\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    /**************** Start the L2 exit sequence for the connections ***************/

    // STEP 1: Reset all endpoints of the links. This clears any link state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_RESET,
                                                        flags);
        conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_RESET,
                                                        flags);
    }

    // STEP 2: NVLink 3 and beyond, we also need to perform INITPHASE1
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_INITPHASE1,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_INITPHASE1,
                                                            flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_LINKSTATE_INITPHASE1 completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }
        }
    }

    // Verify that all the endpoints are now in INIT state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        status = nvlink_core_check_intranode_conn_state(conns[i], NVLINK_LINKSTATE_OFF);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Connection did not transition to INIT. ",
                __FUNCTION__));
            nvlink_core_print_intranode_conn(conns[i]);

            return status;
        }
    }

    // STEP 3: Restore all end point state saved while entering SLEEP state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bStateSaved)
        {
            conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                            NVLINK_LINKSTATE_RESTORE_STATE,
                                                            flags);
        }

        if (conns[i]->end1->bStateSaved)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_RESTORE_STATE,
                                                            flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_LINKSTATE_RESTORE_STATE completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }
        }
    }

    // STEP 4: Initialize RX Termination on all end points
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_rx_mode(conns[i]->end0,
                                                   NVLINK_SUBLINK_STATE_RX_INIT_TERM,
                                                   flags);
        conns[i]->end1->link_handlers->set_rx_mode(conns[i]->end1,
                                                   NVLINK_SUBLINK_STATE_RX_INIT_TERM,
                                                   flags);
    }

    // Get link state on all endpoints. This ensures that NVLINK_SUBLINK_STATE_RX_INIT_TERM completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }
        }
    }

    // STEP 5: Enable Common mode on Tx's of all endpoints
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (!((conns[i]->end0->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE) ||
              (conns[i]->end0->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE) ||
              (conns[i]->end0->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_DATA_READY)))
        {
            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_TX_COMMON_MODE,
                                                       flags);
        }
        if (!((conns[i]->end1->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE) ||
              (conns[i]->end1->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE) ||
              (conns[i]->end1->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_DATA_READY)))
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_COMMON_MODE,
                                                       flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_SUBLINK_STATE_TX_COMMON_MODE completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }
        }
    }

    // STEP 6: Put all Rx's in RXCAL
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->rx_sublink_state != NVLINK_SUBLINK_STATE_RX_RXCAL)
        {
            conns[i]->end0->link_handlers->set_rx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_RX_RXCAL,
                                                       flags);
        }
        if (conns[i]->end1->rx_sublink_state != NVLINK_SUBLINK_STATE_RX_RXCAL)
        {
            conns[i]->end1->link_handlers->set_rx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_RX_RXCAL,
                                                       flags);
        }
    }

    // STEP 7: Disable Tx common mode
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (!((conns[i]->end0->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE) ||
              (conns[i]->end0->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_DATA_READY)))
        {
            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE,
                                                       flags);
        }
        if (!((conns[i]->end1->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE) ||
              (conns[i]->end1->tx_sublink_state == NVLINK_SUBLINK_STATE_TX_DATA_READY)))
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE,
                                                       flags);
        }
    }

    // STEP 8: Set Data Ready and Enable
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->tx_sublink_state != NVLINK_SUBLINK_STATE_TX_DATA_READY)
        {
            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_TX_DATA_READY,
                                                       flags);
        }
        if (conns[i]->end1->tx_sublink_state != NVLINK_SUBLINK_STATE_TX_DATA_READY)
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_DATA_READY,
                                                       flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_SUBLINK_STATE_TX_DATA_READY completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                return status;
            }
        }
    }

    // STEP 9: Set link mode to SAFE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_SAFE,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_SAFE,
                                                            flags);
        }
    }

    // Verify all the endpoints link state now reflect SAFE state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        status = nvlink_core_poll_link_state(conns[i]->end0,
                                             NVLINK_LINKSTATE_SAFE,
                                             NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        status = nvlink_core_poll_link_state(conns[i]->end1,
                                             NVLINK_LINKSTATE_SAFE,
                                             NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        status = nvlink_core_poll_sublink_state(conns[i]->end0,
                                                NVLINK_SUBLINK_STATE_TX_SAFE,
                                                NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                conns[i]->end1,
                                                NVLINK_SUBLINK_STATE_RX_SAFE,
                                                NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        status = nvlink_core_poll_sublink_state(conns[i]->end1,
                                                NVLINK_SUBLINK_STATE_TX_SAFE,
                                                NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                conns[i]->end0,
                                                NVLINK_SUBLINK_STATE_RX_SAFE,
                                                NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
    }

    // STEP 9: Set INITNEOGOTIATE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_INITNEGOTIATE,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_INITNEGOTIATE,
                                                            flags);
        }
    }

    // Bug 2398907 mentioned that a link pair can take upto 125us for DL stat to have CONFIG_GOOD.
    nvlink_sleep(1);

    // STEP 8: Set POST_INITNEGOTIATE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_POST_INITNEGOTIATE,
                                                        flags);
        conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_POST_INITNEGOTIATE,
                                                        flags);
    }

    if (connCount != 0)
    {
        // STEP 11: Train the sublinks to HS and links to ACTIVE
        if (conns[0]->end0->version >= NVLINK_DEVICE_VERSION_30)
        {
            // NVLink 3.0+ ALT training
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(conns,
                                                                                connCount,
                                                                                flags);
        }
        else
        {
            // Legacy training
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy(conns,
                                                                                connCount,
                                                                                flags);
        }

        if (status != NVL_SUCCESS)
        {
            return status;
        }

        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            // Update the power state transition status of the link
            conns[i]->end0->powerStateTransitionStatus = nvlink_power_state_in_L0;
            conns[i]->end1->powerStateTransitionStatus = nvlink_power_state_in_L0;
        }
    }

    /***************** End of L2 exit sequence for the connections *****************/

    return status;
}

/**
 * Train intranode connections associated with a list of links to HS
 * using non-ALI sequence
 *
 * @param[in]  conns      Array of connections to train
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the connections train successfully
 */
NvlStatus
nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus status     = NVL_SUCCESS;
    NvlStatus pollStatus = NVL_SUCCESS;
    NvU32     i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to train to ACTIVE\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    // Trigger INITOPTIMIZE on both ends of the connection
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_INITOPTIMIZE,
                                                        flags);

        // On loopback, only send INITOPTIMIZE to one side.
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_INITOPTIMIZE,
                                                            flags);
        }
    }

    // Trigger POST_INITOPTIMIZE (Checks INITOPTIMIZE was successful) on both ends of the connection
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_POST_INITOPTIMIZE,
                                                        flags);

        // On loopback, only send POST_INITOPTIMIZE to one side.
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_POST_INITOPTIMIZE,
                                                            flags);
        }
    }

    // Set link modes to ACTIVE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        // Some settings required before moving to ACTIVE
        _nvlink_core_set_link_pre_active_settings(conns[i]->end0, flags);
        _nvlink_core_set_link_pre_active_settings(conns[i]->end1, flags);

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_HS,
                                                        flags);

        // If not in loopback send the dl link mode
        if (conns[i]->end0 != conns[i]->end1)
        {

            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_HS,
                                                        flags);
        }

    }

    // Verify link mode HS on the endpoints
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        pollStatus = nvlink_core_poll_link_state(conns[i]->end0,
                                                NVLINK_LINKSTATE_HS,
                                                NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus != NVL_SUCCESS)
        {
            status = pollStatus;
        }

        pollStatus = nvlink_core_poll_link_state(conns[i]->end1,
                                                NVLINK_LINKSTATE_HS,
                                                NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus != NVL_SUCCESS)
        {
            status = pollStatus;
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_INITTL,
                                                        flags);

        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_INITTL,
                                                        flags);
        }

        conns[i]->end0->link_handlers->training_complete(conns[i]->end0);

        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->training_complete(conns[i]->end1);
        }

        conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                   NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                   flags);
        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                       flags);
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                        flags);
        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                            flags);
        }
    }

    return status;
}

/**
 * Train intranode connections associated with a list of links to HS
 * using non-ALI sequence
 *
 * @param[in]  links      Array of links to train
 * @param[in]  numLinks   Number of links in the array
 *
 * return NVL_SUCCESS if the connections train successfully
 */
NvlStatus
nvlink_core_train_intranode_conns_from_off_to_active_ALI
(
    nvlink_link **pLinks,
    NvU32         numLinks
)
{
    NvlStatus status       = NVL_SUCCESS;
    NvlStatus returnStatus = NVL_SUCCESS;
    NvU32     i;

    if ((pLinks == NULL) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to train to ACTIVE\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    for (i = 0; i < numLinks; ++i)
    {
        if (pLinks[i] == NULL)
            continue;

        if (pLinks[i]->bCciManaged)
        {
            continue;
        }

        status = pLinks[i]->link_handlers->ali_training(pLinks[i]);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: failed to send ALI link training on link 0x%x\n",
            __FUNCTION__, pLinks[i]->linkNumber));
            returnStatus = status;
        }
    }

    return returnStatus;
}

/**
 * Train intranode connections associated with a list of links to HS
 * using ALT sequence
 *
 * @param[in]  conns      Array of connections to train
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the connections train successfully
 */
NvlStatus
nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus status     = NVL_SUCCESS;
    NvlStatus pollStatus = NVL_SUCCESS;
    NvU64     linkMode   = NVLINK_LINKSTATE_OFF;
    NvU32     i;
    NvBool    skipConn[NVLINK_MAX_SYSTEM_LINK_NUM] = {0};

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to train to ACTIVE\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        status = conns[i]->end0->link_handlers->get_dl_link_mode(conns[i]->end0, &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
            continue;
        }

        //
        // Skip training if links are in HS
        // Only checking end0 here because HS implies both sides are HS
        //
        if (linkMode == NVLINK_LINKSTATE_HS)
        {
            skipConn[i] = NV_TRUE;
        }
    }

    // Trigger INITOPTIMIZE on both ends of the connection
    for (i = 0; i < connCount; i++)
    {
        if ((conns[i] == NULL) || skipConn[i])
        {
            continue;
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_INITOPTIMIZE,
                                                        flags);

        // On loopback, only send INITOPTIMIZE to one side.
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_INITOPTIMIZE,
                                                            flags);
        }
    }

    // Trigger POST_INITOPTIMIZE (Checks INITOPTIMIZE was successful) on both ends of the connection
    for (i = 0; i < connCount; i++)
    {
        if ((conns[i] == NULL) || skipConn[i])
        {
            continue;
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_POST_INITOPTIMIZE,
                                                        flags);

        // On loopback, only send POST_INITOPTIMIZE to one side.
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_POST_INITOPTIMIZE,
                                                            flags);
        }
    }

    // Set link modes to ACTIVE
    for (i = 0; i < connCount; i++)
    {
        if ((conns[i] == NULL) || skipConn[i])
        {
            continue;
        }

        // Some settings required before moving to ACTIVE
        _nvlink_core_set_link_pre_active_settings(conns[i]->end0, flags);
        _nvlink_core_set_link_pre_active_settings(conns[i]->end1, flags);

        //
        // Put only end0 in ACTIVE mode. The other end should automatically go to Active.
        // If it does not go to ACTIVE then we need to do fault handling.
        //
        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_HS,
                                                        flags);
    }

    // Verify link mode HS on the endpoints
    for (i = 0; i < connCount; i++)
    {
        if ((conns[i] == NULL) || skipConn[i])
        {
            continue;
        }

        pollStatus = nvlink_core_poll_link_state(conns[i]->end1,
                                                NVLINK_LINKSTATE_HS,
                                                NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus != NVL_SUCCESS)
        {
            status = pollStatus;
        }
        else
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Successfully able to set linkstate to ACTIVE for links"
                " %s:%s<->%s:%s\n",
                __FUNCTION__,
                conns[i]->end0->dev->deviceName, conns[i]->end0->linkName,
                conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
        }

        conns[i]->end0->link_handlers->training_complete(conns[i]->end0);

        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->training_complete(conns[i]->end1);
        }

        conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                   NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                   flags);
        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                       flags);
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                        flags);
        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                            flags);
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_ENABLE_PM,
                                                        flags);
        // On loopback, only send once
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_ENABLE_PM,
                                                            flags);
        }
    }

    return status;
}

/**
 * Train a single intranode connection associated with a list of links to HS
 * using legacy pre-Ampere sequence
 *
 * @param[in]  conns      Array of connections to train
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the connections train successfully
 */
NvlStatus
nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus status     = NVL_SUCCESS;
    NvlStatus pollStatus = NVL_SUCCESS;
    NvU32     i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to train to ACTIVE\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    // Enable PRBS generator on both ends of the link
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        _nvlink_core_set_sublink_pre_hs_settings(conns[i]->end0, flags);
        _nvlink_core_set_sublink_pre_hs_settings(conns[i]->end1, flags);
    }

    // Put TX sublink on both ends in High Speed
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                   NVLINK_SUBLINK_STATE_TX_HS,
                                                   flags);
        conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                   NVLINK_SUBLINK_STATE_TX_HS,
                                                   flags);
    }

    // Wait for sublinks to go in High Speed.
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        pollStatus = nvlink_core_poll_sublink_state(conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_TX_HS,
                                                    NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                    conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_RX_HS,
                                                    NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                    NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks in High Speed mode",
                __FUNCTION__));

            status = pollStatus;
        }

        pollStatus = nvlink_core_poll_sublink_state(conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_TX_HS,
                                                    NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                    conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_RX_HS,
                                                    NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                    NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks in High Speed mode",
                __FUNCTION__));

            status = pollStatus;
        }
    }

    // Some settings required before moving to ACTIVE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        _nvlink_core_set_link_pre_active_settings(conns[i]->end0, flags);
        _nvlink_core_set_link_pre_active_settings(conns[i]->end1, flags);

        //
        // Put only end0 in ACTIVE mode. The other end should automatically go to Active.
        // If it does not go to ACTIVE then we need to do fault handling.
        //
        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_HS,
                                                        flags);
    }

    // Verify link mode HS on the endpoints
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        pollStatus = nvlink_core_poll_link_state(conns[i]->end1,
                                                 NVLINK_LINKSTATE_HS,
                                                 NVLINK_TRANSITION_HS_TIMEOUT);
        if (pollStatus == NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Successfully able to train an intranode connection to Active",
                __FUNCTION__));
            nvlink_core_print_intranode_conn(conns[i]);
        }
        else
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to train an intranode connection to Active",
                __FUNCTION__));

            nvlink_core_print_intranode_conn(conns[i]);
            status = NVL_ERR_GENERIC;
        }

        conns[i]->end0->link_handlers->training_complete(conns[i]->end0);
        conns[i]->end1->link_handlers->training_complete(conns[i]->end1);

        conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                   NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                   flags);
        conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                   NVLINK_SUBLINK_STATE_TX_POST_HS,
                                                   flags);

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                        flags);
        conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_TRAFFIC_SETUP,
                                                        flags);

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_ENABLE_PM,
                                                        flags);
        conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_ENABLE_PM,
                                                        flags);
    }

    return status;
}

/**
 * Miscellaneous pre High Speed settings.
 *   Do all the sublink specific settings before it is trained to HS mode
 *
 * @param[in]  link   NVLink Link pointer
 * @param[in]  flags  Flags to track if the step is sync/async
 */
static void
_nvlink_core_set_sublink_pre_hs_settings
(
    nvlink_link *link,
    NvU32        flags
)
{
    if (link == NULL)
        return;

    //
    // Before training the sublinks to HS, the PROD values must be loaded.
    // On Volta/NVSwitch, the PROD values get loaded by UCODE during DLPL Init.
    // So, this PRBS setting is not a prerequisite for High Speed transition.
    // However, for GP100, these values and several other RX end parameters get
    // loaded as part of PRBS enable. If these values are not initialized, then
    // the RX end of the link won't transition to High Speed.
    //

    // Enable PRBS generator
    link->link_handlers->set_tx_mode(link, NVLINK_SUBLINK_STATE_TX_PRBS_EN, flags);
}

/**
 * Miscellaneous pre Active mode settings
 *   Do all the link specific settings before it is trained to Active mode.
 *
 * @param[in]  link   NVLink Link pointer
 * @param[in]  flags  Flags to track if the step is sync/async
 */
static void
_nvlink_core_set_link_pre_active_settings
(
    nvlink_link *link,
    NvU32        flags
)
{
    if (link == NULL)
        return;

    // Some settings required before moving to ACTIVE
    link->link_handlers->set_dl_link_mode(link, NVLINK_LINKSTATE_PRE_HS, flags);
}

/**
 * Miscellaneous post Active mode settings
 *   Do all the link specific settings once it is trained to Active mode.
 *
 * @param[in]  link   NVLink Link pointer
 * @param[in]  flags  Flags to track if the step is sync/async
 */
static void
_nvlink_core_set_link_post_active_settings
(
    nvlink_link *link,
    NvU32        flags
)
{
    if (link == NULL)
        return;

    link->link_handlers->training_complete(link);

    link->link_handlers->set_tx_mode(link, NVLINK_SUBLINK_STATE_TX_POST_HS, flags);

    link->link_handlers->set_dl_link_mode(link, NVLINK_LINKSTATE_TRAFFIC_SETUP, flags);

    link->link_handlers->set_dl_link_mode(link, NVLINK_LINKSTATE_ENABLE_PM, flags);
}
