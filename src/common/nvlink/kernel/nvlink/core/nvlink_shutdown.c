/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

static void _nvlink_core_clear_link_state(nvlink_link *);

/**
 * [CLEAN SHUTDOWN]
 *
 * Shutdown given intranode connections from active to L2 state
 *
 * @param[in]  conns      Array of connections to transition to L2
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if training is sync/async
 *
 * return NVL_SUCCESS if the connections transitioned to L2 successfully
 */
NvlStatus
nvlink_core_powerdown_intranode_conns_from_active_to_L2
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus status   = NVL_SUCCESS;
    NvU64     linkMode = NVLINK_LINKSTATE_OFF;
    NvU32     version;
    NvU32     i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to exit L2\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    if (conns[0] == NULL)
        return NVL_ERR_GENERIC;

    // Set the version. Currently, only one version is supported on a chip
    version = conns[0]->end0->version;

    /**************** Start the L2 entry sequence for the connections ***************/

    // NVLink 3.0 and beyond, link needs to be ACTIVE before it can be transitioned to L2
    if ((version >= NVLINK_DEVICE_VERSION_30) && (connCount > 0))
    {
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            status = nvlink_core_check_intranode_conn_state(conns[i], NVLINK_LINKSTATE_HS);
            if ((status == NVL_SUCCESS) || (status == NVL_ERR_INVALID_STATE))
            {
                continue;
            }

            // We can train connections to HS only when they are already in SAFE
            status = nvlink_core_check_intranode_conn_state(conns[i], NVLINK_LINKSTATE_SAFE);
            if (status != NVL_SUCCESS)
            {
                continue;
            }

            if (nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(&conns[i], 1, flags))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Failed to train connection to ACTIVE.\n",
                    __FUNCTION__));

                return NVL_ERR_GENERIC;
            }
        }
    }

    // STEP 0: Disable HeartBeat on the endpoints of all connections
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_DISABLE_HEARTBEAT,
                                                        flags);

        // Only send if not in loopback
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_DISABLE_HEARTBEAT,
                                                        flags);
        }
    }

    // STEP 1: Disable PM on the endpoints of all connections
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_DISABLE_PM,
                                                        flags);

        // Only send if not in loopback
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_DISABLE_PM,
                                                        flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_LINKSTATE_DISABLE_PM completes
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
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is not in good state after sending DISABLE PM\n",
                    __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is not in good state after sending DISABLE PM\n",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
        }
    }

    // Check for each connection, if both the ends and their sublinks are in HS mode
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        status = nvlink_core_check_intranode_conn_state(conns[i], NVLINK_LINKSTATE_HS);
        if (status == NVL_ERR_INVALID_STATE)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link %s:%s - Link %s:%s is not in good state\n",
                __FUNCTION__,
                conns[i]->end0->dev->deviceName, conns[i]->end0->linkName,
                conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
        }
        else if (status == NVL_SUCCESS)
        {
            if (version <= NVLINK_DEVICE_VERSION_40)
            {
                // STEP 2: Change link state from ACTIVE to SWCFG on all endpoints
                conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                                NVLINK_LINKSTATE_SAFE,
                                                                flags);
                // Only send if not in loopback
                if (conns[i]->end0 != conns[i]->end1)
                {
                    conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                                NVLINK_LINKSTATE_SAFE,
                                                                flags);
                }
            }
        }
    }

    if (version <= NVLINK_DEVICE_VERSION_40)
    {
        //
        // All the endpoints should now either be in SWCFG or transitioning to SWCFG. Poll for all
        // endpoints to reach SWCFG. If any endpoint does not transition to SWCFG, return error
        //
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            // Wait for the end0 to go to SWCFG
            status = nvlink_core_poll_link_state(conns[i]->end0,
                                                NVLINK_LINKSTATE_SAFE,
                                                NVLINK_TRANSITION_SAFE_TIMEOUT);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to set endpoint %s:%s in SWCFG\n",
                    __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
            }

            // Wait for the end1 to go to SWCFG
            status = nvlink_core_poll_link_state(conns[i]->end1,
                                                NVLINK_LINKSTATE_SAFE,
                                                NVLINK_TRANSITION_SAFE_TIMEOUT);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to set endpoint %s:%s in SWCFG\n",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
        }

        // STEP 3: Change sub-link state to SAFE on all endpoints
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_TX_SAFE,
                                                    flags);

            // Only send if not in loopback
            if (conns[i]->end0 != conns[i]->end1)
            {
                conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_TX_SAFE,
                                                    flags);
            }
        }

        // Poll for all endpoints sub-link state to reach SAFE
        for (i = 0; i < connCount; i++)
        {
            if (conns[i] == NULL)
                continue;

            // Wait for sublinks to go to SAFE
            status = nvlink_core_poll_sublink_state(conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_TX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                    conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_RX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                    NVLINK_TRANSITION_SAFE_TIMEOUT);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to set sublinks to SAFE\n",
                    __FUNCTION__));
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
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to set sublinks to SAFE\n",
                    __FUNCTION__));
            }
        }
    }

    // STEP 4: Save link state on all the endpoints
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (!conns[i]->end0->bStateSaved)
        {
            conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                            NVLINK_LINKSTATE_SAVE_STATE,
                                                            flags);
        }

        if (!conns[i]->end1->bStateSaved)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_SAVE_STATE,
                                                            flags);
        }
    }

    // Get link state on all endpoints. This ensures that NVLINK_LINKSTATE_SAVE_STATE completes
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
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is not in good state after sending SAVESTATE command\n",
                    __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
            }

            status = conns[i]->end1->link_handlers->get_dl_link_mode(conns[i]->end1, &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is not in good state after sending SAVESTATE command\n",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
        }
    }

    // STEP 5: Trigger the sleep request on all the endpoints
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        //
        // Send SLEEP request on one end of connection if not in loopback.
        // Don' poll, since transition will happen when both ends get the request
        //
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end0->link_handlers->set_tl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_SLEEP,
                                                        NVLINK_STATE_CHANGE_ASYNC);
        }

        // Send SLEEP request on both ends and poll for completion
        conns[i]->end1->link_handlers->set_tl_link_mode(conns[i]->end1,
                                                        NVLINK_LINKSTATE_SLEEP,
                                                        NVLINK_STATE_CHANGE_SYNC);
        conns[i]->end0->link_handlers->set_tl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_SLEEP,
                                                        NVLINK_STATE_CHANGE_SYNC);
    }

    // Finally check the connection states
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        status = nvlink_core_check_intranode_conn_state(conns[i], NVLINK_LINKSTATE_SLEEP);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link %s:%s - Link %s:%s is not in good state after sending SLEEP request\n",
                __FUNCTION__,
                conns[i]->end0->dev->deviceName, conns[i]->end0->linkName,
                conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
        }

        // Update the link and sublink states in the core library
        conns[i]->end0->state            = NVLINK_LINKSTATE_SLEEP;
        conns[i]->end1->state            = NVLINK_LINKSTATE_SLEEP;
        conns[i]->end0->tx_sublink_state = NVLINK_SUBLINK_STATE_TX_OFF;
        conns[i]->end1->tx_sublink_state = NVLINK_SUBLINK_STATE_TX_OFF;
        conns[i]->end0->rx_sublink_state = NVLINK_SUBLINK_STATE_RX_OFF;
        conns[i]->end1->rx_sublink_state = NVLINK_SUBLINK_STATE_RX_OFF;

        // Update power state transition status for the connection
        conns[i]->end0->powerStateTransitionStatus = nvlink_power_state_in_L2;
        conns[i]->end1->powerStateTransitionStatus = nvlink_power_state_in_L2;
    }

    /***************** End of L2 entry sequence for the connections ****************/

    //
    // Note that status is squashed, since the expectation is that we soldier on if any link fails
    // during the transition to L2 state
    //
    return NVL_SUCCESS;
}

/**
 * [PSEUDO-CLEAN SHUTDOWN]
 *
 * Shutdown the given array of intranode connections from ACTIVE to OFF state
 *
 * @param[in]  conns      Array of connections to shutdown
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if shutdown is sync/async
 *
 * return NVL_SUCCESS if the connections shutdown successfully
 */
NvlStatus
nvlink_core_powerdown_intranode_conns_from_active_to_off
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus  status = NVL_SUCCESS;
    NvU32      i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to shutdown\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        // Disable Power Management before moving link out of Active
        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_DISABLE_PM,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_DISABLE_PM,
                                                            flags);
        }

        // Move both ends to SWCFG
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

    // Poll for links to reach SWCFG & initiate sublinks to SAFE state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        // Wait for the end0 to go to SWCFG
        status = nvlink_core_poll_link_state(conns[i]->end0,
                                             NVLINK_LINKSTATE_SAFE,
                                             NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set endpoint %s:%s in SWCFG",
                __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
                
            // to track Failure
            conns[i]->end0->inSWCFG = NV_FALSE;        
	    }
        else
        {
            conns[i]->end0->inSWCFG = NV_TRUE;
        }

        // Wait for the end1 to go to SWCFG
        status = nvlink_core_poll_link_state(conns[i]->end1,
                                             NVLINK_LINKSTATE_SAFE,
                                             NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set endpoint %s:%s in SWCFG\n",
                __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));

            // to track Failure
            conns[i]->end1->inSWCFG = NV_FALSE; 
	    }
        else
        {
            conns[i]->end1->inSWCFG = NV_TRUE;
        }

        // Change each sublink state to SAFE
	    if(conns[i]->end0->inSWCFG == NV_TRUE)
        {
            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_TX_SAFE,
                                                       flags);
        }

        if (conns[i]->end0 != conns[i]->end1  && conns[i]->end1->inSWCFG == NV_TRUE)
        {
            conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                       NVLINK_SUBLINK_STATE_TX_SAFE,
                                                       flags);
        }
    }

    // Poll for sublinks to reach SAFE state
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        // Wait for sublinks to go to SAFE
        if(conns[i]->end0->inSWCFG == NV_TRUE)
        {
            status = nvlink_core_poll_sublink_state(conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_TX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                    conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_RX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                    NVLINK_TRANSITION_SAFE_TIMEOUT);
        }
        if (status != NVL_SUCCESS || conns[i]->end0->inSWCFG == NV_FALSE)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks to SAFE",
                __FUNCTION__));
        }

        if(conns[i]->end1->inSWCFG == NV_TRUE)
        {
            status = nvlink_core_poll_sublink_state(conns[i]->end1,
                                                    NVLINK_SUBLINK_STATE_TX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                                    conns[i]->end0,
                                                    NVLINK_SUBLINK_STATE_RX_SAFE,
                                                    NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                                    NVLINK_TRANSITION_SAFE_TIMEOUT);
        }
        if (status != NVL_SUCCESS || conns[i]->end1->inSWCFG == NV_FALSE)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks to SAFE",
                __FUNCTION__));
        }

        //
        // Disable error detect on both sides of the link
        //
        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_DISABLE_ERR_DETECT,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_DISABLE_ERR_DETECT,
                                                            flags);
        }

        //
        // Disable Lanes on both sides of the link
        //
        status = conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                                 NVLINK_LINKSTATE_LANE_DISABLE,
                                                                 flags);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to disable lanes for link %s:%s\n",
                __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
        }

        if (conns[i]->end0 != conns[i]->end1)
        {
            status = conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                                     NVLINK_LINKSTATE_LANE_DISABLE,
                                                                     flags);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to disable lanes for link %s:%s\n",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
        }

        //
        // Shutdown Lanes on both sides of the link
        //
        status = conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                                 NVLINK_LINKSTATE_LANE_SHUTDOWN,
                                                                 flags);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to shutdown lanes for link %s:%s\n",
                __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
        }

        if (conns[i]->end0 != conns[i]->end1)
        {
            status = conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                                     NVLINK_LINKSTATE_LANE_SHUTDOWN,
                                                                     flags);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to shutdown lanes for link %s:%s\n",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
        }

        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0, NVLINK_LINKSTATE_OFF, flags);

        // Link becomes in-accessible after its turned off. Check if this is a loopback connection
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1, NVLINK_LINKSTATE_OFF, flags);
        }

        _nvlink_core_clear_link_state(conns[i]->end0);
        _nvlink_core_clear_link_state(conns[i]->end1);
    }

    //
    // Squash status. If any side of link doesn not respond the link is
    // shutdown unilaterally
    //
    return NVL_SUCCESS;
}

/**
 * Power down the given array of intranode connections from ACTIVE to SWCFG state
 *
 * @param[in]  conns      Array of connections to shutdown
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags to track if shutdown is sync/async
 *
 * return NVL_SUCCESS if the connections shutdown successfully
 */
NvlStatus
nvlink_core_powerdown_intranode_conns_from_active_to_swcfg
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvlStatus  status = NVL_SUCCESS;
    NvU32      i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to shutdown\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        // Disable Power Management before moving link out of Active
        conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                        NVLINK_LINKSTATE_DISABLE_PM,
                                                        flags);
        if (conns[i]->end0 != conns[i]->end1)
        {
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_DISABLE_PM,
                                                            flags);
        }

        // Move both ends to SWCFG
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

    //
    // Poll _SAFE state for connections and set corresponding sublinks to _SAFE
    //
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        // Wait for the end0 to go to SWCFG
        status = nvlink_core_poll_link_state(conns[i]->end0,
                                             NVLINK_LINKSTATE_SAFE,
                                             NVLINK_TRANSITION_SAFE_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set endpoint %s:%s in SWCFG",
                __FUNCTION__, conns[i]->end0->dev->deviceName, conns[i]->end0->linkName));
        }
        else
        {
            // Change each sublink state to SAFE
            conns[i]->end0->link_handlers->set_tx_mode(conns[i]->end0,
                                                       NVLINK_SUBLINK_STATE_TX_SAFE,
                                                       flags);
        }

        if (conns[i]->end0 != conns[i]->end1)
        {
            // Wait for the end1 to go to SWCFG
            status = nvlink_core_poll_link_state(conns[i]->end1,
                                                 NVLINK_LINKSTATE_SAFE,
                                                 NVLINK_TRANSITION_SAFE_TIMEOUT);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to set endpoint %s:%s in SWCFG",
                    __FUNCTION__, conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
            }
            else
            {
                // Change each sublink state to SAFE
                conns[i]->end1->link_handlers->set_tx_mode(conns[i]->end1,
                                                           NVLINK_SUBLINK_STATE_TX_SAFE,
                                                           flags);
            }
        }
    }

    // Wait for sublinks to go to SAFE
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
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
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks to SAFE (TX:RX)",
                __FUNCTION__));
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: sublinks (%s:%s)  (%s:%s)",
                __FUNCTION__, 
                conns[i]->end0->dev->deviceName, conns[i]->end0->linkName,
                conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
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
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set sublinks to SAFE (RX:TX)",
                __FUNCTION__));
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: sublinks (%s:%s)  (%s:%s)",
                __FUNCTION__, 
                conns[i]->end0->dev->deviceName, conns[i]->end0->linkName,
                conns[i]->end1->dev->deviceName, conns[i]->end1->linkName));
        }
    }

    // Update tracking info
    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;

        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Connection is in SAFE mode. ",
            __FUNCTION__));
        nvlink_core_print_intranode_conn(conns[i]);
    }

    //
    // Squash status. If any side of link doesn not respond the link is
    // shutdown unilaterally
    //
    return NVL_SUCCESS;
}

/**
 * Reset the given array of intranode connections
 *
 * @param[in]  conns      Array of connections to reset
 * @param[in]  connCount  Number of connections in the array
 * @param[in]  flags      Flags
 *
 * return NVL_SUCCESS if the connections reset successfully
 */
NvlStatus
nvlink_core_reset_intranode_conns
(
    nvlink_intranode_conn **conns,
    NvU32                   connCount,
    NvU32                   flags
)
{
    NvU32  i;

    if ((conns == NULL) || (connCount == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connections to shutdown\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    for (i = 0; i < connCount; i++)
    {
        if (conns[i] == NULL)
            continue;
        
        if (conns[i]->end0->bCciManaged || 
            conns[i]->end1->bCciManaged)
        {
            continue;
        }

        //
        // Reset both ends of this connection.
        // This path should enable/init those link endpoints as well.
        //
        // NVLink3.0 + uses the TL link reset
        //
        if (conns[i]->end0->version >= NVLINK_DEVICE_VERSION_30)
        {
            conns[i]->end0->link_handlers->set_tl_link_mode(conns[i]->end0,
                                                            NVLINK_LINKSTATE_RESET,
                                                            flags);
            if (conns[i]->end0 != conns[i]->end1)
            {
                conns[i]->end1->link_handlers->set_tl_link_mode(conns[i]->end1,
                                                                NVLINK_LINKSTATE_RESET,
                                                                flags);
            }
        }
        else
        {
            conns[i]->end0->link_handlers->set_dl_link_mode(conns[i]->end0,
                                                            NVLINK_LINKSTATE_RESET,
                                                            flags);
            conns[i]->end1->link_handlers->set_dl_link_mode(conns[i]->end1,
                                                            NVLINK_LINKSTATE_RESET,
                                                            flags);
        }

        _nvlink_core_clear_link_state(conns[i]->end0);
        _nvlink_core_clear_link_state(conns[i]->end1);
    }

    return NVL_SUCCESS;
}

/**
 * Clears Core Library State
 *
 * @param[in]  link  NVLink Link pointer
 */
static void
_nvlink_core_clear_link_state
(
    nvlink_link *link
)
{
    if (link == NULL)
        return;

    // Receiver Detect needs to happen again
    link->bRxDetected = NV_FALSE;

    // INITNEGOTIATE needs to happen again
    link->bInitnegotiateConfigGood = NV_FALSE;

    // TxCommonMode needs to happen again
    link->bTxCommonModeFail = NV_FALSE;

    // SAFE transition needs to happen again
    link->bSafeTransitionFail = NV_FALSE;

    // Reset the SW state tracking the link and sublink states
    link->state            = NVLINK_LINKSTATE_OFF;
    link->tx_sublink_state = NVLINK_SUBLINK_STATE_TX_OFF;
    link->rx_sublink_state = NVLINK_SUBLINK_STATE_RX_OFF;
}

static NvBool
_nvlink_core_check_if_conn_in_array
(
    nvlink_intranode_conn **connArray,
    NvU32                  connArrayLength,
    nvlink_intranode_conn *conn
)
{
    NvU32 i;

    for (i = 0; i < connArrayLength; ++i)
    {
        if (conn == connArray[i])
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

NvlStatus
nvlink_core_powerdown_floorswept_conns_to_off
(
    nvlink_link           **links,
    NvU32                   linkCount,
    NvU32                   numIoctrls,
    NvU32                   numLinksPerIoctrl,
    NvU32                   numActiveLinksPerIoctrl
)
{
    NvU32 i,j;
    nvlink_intranode_conn **connsToShutdown = NULL;
    nvlink_intranode_conn **visitedConns    = NULL;
    nvlink_intranode_conn *conn;
    NvU32 connCount;
    NvU32 numConnsToShutdown;

    if (linkCount == 0 || numIoctrls == 0 || numLinksPerIoctrl == 0 ||
        numActiveLinksPerIoctrl == 0)
    {
        return NVL_BAD_ARGS;
    }

    connsToShutdown = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * NVLINK_MAX_SYSTEM_LINK_NUM);

    if (connsToShutdown == NULL)
    {
        return NVL_NO_MEM;
    }

    visitedConns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * NVLINK_MAX_SYSTEM_LINK_NUM);

    if (visitedConns == NULL)
    {
        nvlink_free(connsToShutdown);
        return NVL_NO_MEM;
    }


    //
    // For each IOCTRL find the total # of connections and shutdown
    // any connections over the number of active links per IOCTRL
    //
    for (i=0; i<numIoctrls; i++)
    {
        connCount = 0;
        numConnsToShutdown = 0;
        for (j = 0; j < linkCount; j++)
        {
            //
            // If the link is associated with the current IOCTRL
            // then retrieve its connection. If there is no
            // active connection associated with the link then continue
            // If the connection is not active, shutdown the links but don't
            // increment connCount as this is not an active connection to be used for P2P
            // If the number of connections found so far is greater
            // then the number of active links allowed, mark the connection
            // as being needed to shutdown. Increment the total connection count
            //
            if (links[j]->linkNumber >= numLinksPerIoctrl*i &&
                links[j]->linkNumber < numLinksPerIoctrl*(i+1))
            {
                conn = NULL;
                nvlink_core_get_intranode_conn(links[j], &(conn));
                if (conn == NULL ||
                    _nvlink_core_check_if_conn_in_array(visitedConns, connCount, conn) ||
                    (conn->end0 == NULL || conn->end1 == NULL))
                {
                    continue;
                }
                else if(nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_OFF) ==
                            NVL_SUCCESS)
                {
                    continue;
                }
                else if ((nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_HS) != NVL_SUCCESS  &&
                          nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SAFE) != NVL_SUCCESS) &&
                        (!_nvlink_core_check_if_conn_in_array(connsToShutdown, numConnsToShutdown, conn)))
                {
                    // If link is not in SAFE or HS and not currently in our connsToShutdown array then add it
                    connsToShutdown[numConnsToShutdown++] = conn;
                }
                else
                {
                    visitedConns[connCount++] = conn;
                    if (connCount > numActiveLinksPerIoctrl &&
                        !_nvlink_core_check_if_conn_in_array(connsToShutdown, numConnsToShutdown, conn))
                    {
                        connsToShutdown[numConnsToShutdown++] = conn;
                    }
                }

                //
                // If the #of conns found == #of links per IOCTRl then
                // bail early since we know none of the other links can be
                // part of this IOCTRL
                //
                if (connCount == numLinksPerIoctrl)
                {
                    connCount = 0;
                    break;
                }
            }
        }

        //
        // If the number of shutdown is non-zero then shutdown the connections
        // and remove the connection from the corelib since all endpoints
        // will query the corelib for topology and this connection should no longer
        // be reported
        //
        if (numConnsToShutdown != 0)
        {
            nvlink_core_powerdown_intranode_conns_from_active_to_off(connsToShutdown, numConnsToShutdown, 0);
            nvlink_core_reset_intranode_conns(connsToShutdown, numConnsToShutdown, NVLINK_STATE_CHANGE_SYNC);

            for (j = 0; j < numConnsToShutdown; ++j)
            {
                nvlink_core_remove_intranode_conn(connsToShutdown[j]);
            }
        }
    }

    nvlink_free(visitedConns);
    nvlink_free(connsToShutdown);

    return NVL_SUCCESS;
}
