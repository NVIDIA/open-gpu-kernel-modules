/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvlink_lock.h"


/**
 * Check whether a group of links have completed training
 *
 * @param[in]  links      List of NVLink Link pointers
 * @param[in]  linkCount  Count of #links
 *
 * return NL_SUCCESS if all links transitioned to Active
 */
NvlStatus
nvlink_lib_check_training_complete
(
    nvlink_link **links,
    NvU32 linkCount
)
{
    NvlStatus     status = NVL_SUCCESS;
    nvlink_link **lockLinks           = NULL;
    NvU32         lockLinkCount = 0;
    NvU32         i;

    if (links == NULL || linkCount == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad link pointer or linkCount!\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    lockLinks = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));
        goto nvlink_lib_check_training_complete_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //
    //
    // Get the array of both local and remote endpoints whose lock needs
    // to be acquired
    //
    for (i = 0; i < linkCount; i++)
    {
        if ((lockLinkCount + 1) >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: lockLinkCount >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock
            nvlink_lib_top_lock_release();
            status = NVL_ERR_INVALID_STATE;
            goto nvlink_lib_check_training_complete_end;
        }

        // Capture both the link and its end-point
        lockLinks[lockLinkCount] = links[i];
        lockLinkCount++;
    }
    // Acquire the per-link locks for all links captured
    status = nvlink_lib_link_locks_acquire(lockLinks, lockLinkCount);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));
        // Release the top-level lock
        nvlink_lib_top_lock_release();
        goto nvlink_lib_check_training_complete_end;
    }

    nvlink_lib_top_lock_release();

    // Only run the check if ALI is enabled
    if(links[0]->dev->enableALI)
    {
        //
        // This will be the returned back to the caller, the core function
        // will return early with an error status if a link is not Active
        //
        status = nvlink_core_train_check_link_ready_ALI(lockLinks, lockLinkCount);
    }
    else
    {
        // If ALI is not enabled, return error
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: ALI is not enabled! Cannot check training status, please use non-ALI or ALT training to get links to active\n",
            __FUNCTION__));
        status = NVL_ERR_GENERIC;
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);
nvlink_lib_check_training_complete_end:
    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }
    return status;
}


/**
 * Train a given set of links from SWCFG to ACTIVE state
 *
 * Note: For training the links one by one - its the responsibility of
 *       the caller to call this function every time for each link
 *
 * @param[in]  links      List of NVLink Link pointers
 * @param[in]  linkCount  Count of #links
 * @param[in]  flags      Flag to track whether training is sync/async
 *
 * return NL_SUCCESS if the link state transition was a success
 */
NvlStatus
nvlink_lib_train_links_from_swcfg_to_active
(
    nvlink_link **links,
    NvU32         linkCount,
    NvU32         flags
)
{
    nvlink_intranode_conn **conns     = NULL;
    nvlink_intranode_conn  *conn      = NULL;
    NvlStatus               status    = NVL_SUCCESS;
    NvU32                   connCount = 0;
    NvU32                   i, j;
    nvlink_link           **lockLinks = NULL;
    NvU32                   lockLinkCount = 0;

    if (links == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad link pointer\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    lockLinks = (nvlink_link **)nvlink_malloc( sizeof(nvlink_link *) * (2 * linkCount));
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * linkCount);

    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_train_links_from_swcfg_to_active_end;
    }

    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * linkCount);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_train_links_from_swcfg_to_active_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    //
    // Get the array of both local and remote endpoints whose lock needs
    // to be acquired
    //
    for (i = 0; i < linkCount; i++)
    {
        conn = NULL;
        nvlink_core_get_intranode_conn(links[i], &conn);

        if (!conn)
        {
            //
            // Could not find the connection for the link. Release the
            // top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_ERR_GENERIC;
            goto nvlink_lib_train_links_from_swcfg_to_active_end;
        }

        // Capture both the link and its end-point
        lockLinks[lockLinkCount] = conn->end0;
        lockLinkCount++;

        lockLinks[lockLinkCount] = conn->end1;
        lockLinkCount++;
    }

    // Acquire the per-link locks for all links captured
    status = nvlink_lib_link_locks_acquire(lockLinks, lockLinkCount);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        goto nvlink_lib_train_links_from_swcfg_to_active_end;
    }

    // Check all the connections which need to be trained
    for (i = 0; i < linkCount; i++)
    {
        conn = NULL;
        nvlink_core_get_intranode_conn(links[i], &conn);

        // Don't train links that didn't receive CONFIG_GOOD (NVLINK3+)
        if (((conn->end0->version >= NVLINK_DEVICE_VERSION_30) ||
             (conn->end1->version >= NVLINK_DEVICE_VERSION_30)) &&
             (!links[i]->bInitnegotiateConfigGood))
        {
            continue;
        }

        // Check if the link is already in ACTIVE
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_HS);
        if ((status == NVL_SUCCESS) || (status == NVL_ERR_INVALID_STATE))
        {
            continue;
        }

        // We can train connections to HS only when they are already in SAFE
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SAFE);
        if (status != NVL_SUCCESS)
        {
            continue;
        }

        // Check if the connection is not already considered
        for (j = 0; j < connCount; j++)
        {
            if (conns[j] == conn)
                break;
        }

        // If this is a new connection, add it to the list
        if (j == connCount)
        {
            conns[connCount] = conn;
            connCount++;
        }
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    if (connCount > 0)
    {
        if ((conn->end0->version >= NVLINK_DEVICE_VERSION_40) ||
            (conn->end1->version >= NVLINK_DEVICE_VERSION_40))
        {
            if (!conn->end0->dev->enableALI)
            {
                status = nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI(conns,
                                                                                connCount,
                                                                                flags);
            }
        }
        // For NVLink3+, use ALT sequence
        else if ((conn->end0->version >= NVLINK_DEVICE_VERSION_30) ||
            (conn->end1->version >= NVLINK_DEVICE_VERSION_30))
        {
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(conns,
                                                                                connCount,
                                                                                flags);
        }
        else
        {
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy(conns,
                                                                                connCount,
                                                                                flags);
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_train_links_from_swcfg_to_active_end:

    if (conns != NULL)
    {
        nvlink_free((void *)conns);
    }

    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }

    return status;
}

/**
 * Train a given set of links of a device from L2 to ACTIVE state
 *
 * param[in]  dev       NVLink Device pointer
 * param[in]  linkMask  Mask of links to be trained
 * param[in]  flags     Flags to track if the transition is sync/async
 *
 * return NVL_SUCCESS if the links train to ACTIVE
 */
NvlStatus
nvlink_lib_train_links_from_L2_to_active
(
    nvlink_device *dev,
    NvU32          linkMask,
    NvU32          flags
)
{
    NvlStatus                status    = NVL_SUCCESS;
    nvlink_link             *link      = NULL;
    nvlink_intranode_conn  **conns     = NULL;
    nvlink_intranode_conn   *conn      = NULL;
    NvU32                    numLinks  = 0;
    NvU32                    numConns  = 0;
    NvU32                    connCount = 0;
    NvU32                    i;

    nvlink_link            **lockLinks = NULL;
    NvU32                    lockLinkCount = 0;


    if (dev == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad device pointer specified.\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    lockLinks = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * NVLINK_MAX_SYSTEM_LINK_NUM);

    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_train_links_from_L2_to_active_end;
    }

    // Initialize the list of links
    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * 32);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_train_links_from_L2_to_active_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    // Get the array of link endpoints whose lock needs to be acquired
    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (!(linkMask & (1 << link->linkNumber)))
        {
            continue;
        }

        // Get the connection associated with the link
        conn = NULL;
        nvlink_core_get_intranode_conn(link, &conn);

        if (conn == NULL)
        {
            //
            // Could not find the connection for the link. Release the
            // top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_ERR_GENERIC;
            goto nvlink_lib_train_links_from_L2_to_active_end;
        }

        lockLinks[lockLinkCount] = conn->end0;
        lockLinkCount++;

        lockLinks[lockLinkCount] = conn->end1;
        lockLinkCount++;
    }

    // Acquire the per-link locks for all links captured
    status = nvlink_lib_link_locks_acquire(lockLinks, lockLinkCount);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        goto nvlink_lib_train_links_from_L2_to_active_end;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (!(linkMask & (1 << link->linkNumber)))
        {
            continue;
        }

        // If the link received a L2 entry request, but never entered L2
        if (link->powerStateTransitionStatus == nvlink_power_state_entering_L2)
        {
            // Update the power state transition status
            link->powerStateTransitionStatus = nvlink_power_state_in_L0;
            continue;
        }

        // Get the connection associated with the link
        conn = NULL;
        nvlink_core_get_intranode_conn(link, &conn);

        // Check the connection state to verify if the link is already in HS
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_HS);
        if (status == NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Link is not in sleep %s: %s.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName));
            continue;
        }

        // Check the connection state to verify if the link is already in SAFE
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SAFE);
        if (status == NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link is not in sleep %s: %s.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName));
            continue;
        }

        // Mark the power state transition for the link
        link->powerStateTransitionStatus = nvlink_power_state_exiting_L2;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (!(linkMask & (1 << link->linkNumber)))
        {
            continue;
        }

        if (link->powerStateTransitionStatus == nvlink_power_state_exiting_L2)
        {
            // Get the connection associated with the link
            conn = NULL;
            nvlink_core_get_intranode_conn(link, &conn);

            // Verify if both the endpoints desire to exit SLEEP
            if ((conn->end0->powerStateTransitionStatus == nvlink_power_state_exiting_L2) &&
                (conn->end1->powerStateTransitionStatus == nvlink_power_state_exiting_L2))
            {
                // Increment the #connections considered for exiting L2
                numConns++;

                // Check if the the connection is already included in the list
                for (i = 0; i < connCount; i++)
                {
                    if (conns[i] == conn)
                        break;
                }

                // If this is a new connection, add it to the list
                if (i == connCount)
                {
                    conns[connCount] = conn;
                    connCount++;
                }
            }

            // Increment the #links considered for exiting L2
            numLinks++;
        }
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Clear the status variable
    status = NVL_SUCCESS;

    if (connCount > 0)
    {
        status = nvlink_core_train_intranode_conns_from_from_L2_to_active(conns, connCount, flags);
    }

    if (status == NVL_SUCCESS)
    {
        //
        // If some links are waiting on the remote end to exit sleep,
        // update status to NVL_MORE_PROCESSING_REQUIRED
        //
        status = (numLinks != numConns ? NVL_MORE_PROCESSING_REQUIRED : NVL_SUCCESS);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_train_links_from_L2_to_active_end:

    if (conns != NULL)
    {
        nvlink_free((void *)conns);
    }

    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }

    return status;
}

/**
 * Retrain a given link from SWCFG to ACTIVE
 *
 * @param[in]  link   NVLink Link pointer
 * @param[in]  flags  Flag to track if the training is aync/async
 *
 * return NVL_SUCCESS if the training was successful
 */
NvlStatus
nvlink_lib_retrain_link_from_swcfg_to_active
(
    nvlink_link *link,
    NvU32        flags
)
{
    nvlink_intranode_conn *conn   = NULL;
    NvlStatus              status = NVL_SUCCESS;

    if (!link)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad link pointer specified.\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    // Get the connection associated with the link
    nvlink_core_get_intranode_conn(link, &conn);

    if (!conn)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connection was found for %s: %s.\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        //
        // Could not find the connection for the link. Release the
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_ERR_GENERIC;
    }

    // create array of one conn and two link endpoints
    nvlink_intranode_conn *conns[1] = {conn};
    nvlink_link           *links[2] = {0};

    links[0] = conn->end0;
    links[1] = conn->end1;

    // Acquire the per-link locks for the links
    status = nvlink_lib_link_locks_acquire(links, 2);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return status;
    }

    //
    // All the required per-link locks are successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Check if the link is already in ACTIVE
    status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_HS);
    if ((status == NVL_SUCCESS) || (status == NVL_ERR_INVALID_STATE))
    {
        // Release the per-link locks
        nvlink_lib_link_locks_release(links, 2);

        return status;
    }

    // We can train connections to HS only when they are already in SAFE
    status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SAFE);
    if (status != NVL_SUCCESS)
    {
        // Release the per-link locks
        nvlink_lib_link_locks_release(links, 2);

        return status;
    }
    
    if ((conn->end0->version >= NVLINK_DEVICE_VERSION_40) ||
        (conn->end1->version >= NVLINK_DEVICE_VERSION_40))
    {
        if (!conn->end0->bInitnegotiateConfigGood ||
            !conn->end1->bInitnegotiateConfigGood)
        {
            status = NVL_ERR_GENERIC;
        }
        else if (!conn->end0->dev->enableALI)
        {
            // ALI training for NVLink4.0+
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI(conns, 0x1, flags);
        }
    }
    else if ((conn->end0->version >= NVLINK_DEVICE_VERSION_30) ||
        (conn->end1->version >= NVLINK_DEVICE_VERSION_30))
    {
        if (!conn->end0->bInitnegotiateConfigGood ||
            !conn->end1->bInitnegotiateConfigGood)
        {
            status = NVL_ERR_GENERIC;
        }
        else
        {
            // ALT training for NVLink3.0+
            status = nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(conns, 0x1, flags);
        }
    }
    else
    {
        // Legacy training for pre-NVLink3.0
        status = nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy(conns, 0x1, flags);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, 2);

    return status;
}

/**
 * Save training seeds into the link structure
 *
 * @param[in]  link      NVLink Link pointer
 * @param[in]  seedData  Training seed information
 *
 * return NVL_SUCCESS if the seed saving was successful
 */
NvlStatus
nvlink_lib_save_training_seeds
(
    nvlink_link *link,
    NvU32       *seedData
)
{
    NvlStatus status = NVL_SUCCESS;

    // Check to make sure we are given a buffer of data
    if (seedData == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No seed data given to store %s: %s.\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));
        return NVL_ERR_GENERIC;
    }

    NvU32 size = seedData[0];

    // check to make sure the size is not out of bounds 
    if (size > NVLINK_MAX_SEED_NUM)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad data, size of %d out of bounds %s: %s.\n",
            __FUNCTION__, size, link->dev->deviceName, link->linkName));
        return NVL_ERR_GENERIC;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&link, 1);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link lock\n",
            __FUNCTION__));

        return status;
    }

    //always using corelib defined structures for size
    nvlink_memcpy(link->seedData, seedData, sizeof(link->seedData));

    // Release the per-link locks
    nvlink_lib_link_locks_release(&link, 1);

    return NVL_SUCCESS;
}

/**
 * Copy training seeds from the link structure
 *
 * @param[in]  link      NVLink Link pointer
 * @param[in]  seedData  Training seed information
 *
 * return NVL_SUCCESS if the seed copy was successful
 */
NvlStatus
nvlink_lib_copy_training_seeds
(
    nvlink_link *link,
    NvU32       *seedDataCopy
)
{
    NvlStatus status = NVL_SUCCESS;

    // Check to make sure we are given a buffer to copy into
    if (seedDataCopy == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No seed data structure given to store into %s: %s.\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));
        return NVL_ERR_GENERIC;
    }

    NvU32 size = link->seedData[0];

    // check to make sure the size is not out of bounds
    if (size > NVLINK_MAX_SEED_NUM)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad data, size of %d out of bounds %s: %s.\n",
            __FUNCTION__, size, link->dev->deviceName, link->linkName));
        return NVL_ERR_GENERIC;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&link, 1);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link lock\n",
            __FUNCTION__));

        return status;
    }

    nvlink_memcpy(seedDataCopy, link->seedData, sizeof(link->seedData));

    // Release the per-link locks
    nvlink_lib_link_locks_release(&link, 1);

    return NVL_SUCCESS;
}
