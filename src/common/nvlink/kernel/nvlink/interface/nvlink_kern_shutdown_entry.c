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
#include "nvlink_lock.h"

/**
 * [CLEAN SHUTDOWN]
 *
 * Shutdown given links of a device from active to L2 state
 *
 * param[in]  dev       NVLink Device pointer
 * param[in]  linkMask  Mask of links to be shutdown
 * param[in]  flags     Flags to track if the transition is sync/async
 *
 * return NVL_SUCCESS if the links transition to L2
 */
NvlStatus
nvlink_lib_powerdown_links_from_active_to_L2
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
    NvU32                    lockLinkCount = 0;
    nvlink_link            **lockLinks = NULL;


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

        nvlink_free((void *)lockLinks);
        return NVL_ERR_GENERIC;
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

        goto nvlink_lib_powerdown_links_from_active_to_L2_end;
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
            goto nvlink_lib_powerdown_links_from_active_to_L2_end;
        }
        else if ((numLinks + 1) >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock
            nvlink_lib_top_lock_release();
            status = NVL_ERR_INVALID_STATE;
            goto nvlink_lib_powerdown_links_from_active_to_L2_end;
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

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_powerdown_links_from_active_to_L2_end;
    }

    // Filter the connections which are already in SLEEP
    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (!(linkMask & (1 << link->linkNumber)))
        {
            continue;
        }

        // If the link received a L2 exit request, but never exited L2
        if (link->powerStateTransitionStatus == nvlink_power_state_exiting_L2)
        {
            // Update the power state transition status
            link->powerStateTransitionStatus = nvlink_power_state_in_L2;
            continue;
        }

        // Get the connection associated with the link
        conn = NULL;
        nvlink_core_get_intranode_conn(link, &conn);

        // Check the connection state to verify if the link is already in SLEEP
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SLEEP);
        if (status == NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Link is already in sleep %s: %s.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName));
            continue;
        }

        // Link is not in SLEEP. Update power state transition status for the link
        link->powerStateTransitionStatus = nvlink_power_state_entering_L2;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (!(linkMask & (1 << link->linkNumber)))
        {
            continue;
        }

        // Check if the link desires to enter SLEEP
        if (link->powerStateTransitionStatus == nvlink_power_state_entering_L2)
        {
            // Get the connection associated with the link
            conn = NULL;
            nvlink_core_get_intranode_conn(link, &conn);

            // The connection will enter SLEEP only when both its endpoints desire to enter SLEEP
            if ((conn->end0->powerStateTransitionStatus == nvlink_power_state_entering_L2) &&
                (conn->end1->powerStateTransitionStatus == nvlink_power_state_entering_L2))
            {
                // Increment the #connections considered for entering L2
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
        status = nvlink_core_powerdown_intranode_conns_from_active_to_L2(conns, connCount, flags);
    }

    if (status == NVL_SUCCESS)
    {
        //
        // If some links are waiting on the remote end to request sleep,
        // update status to NVL_MORE_PROCESSING_REQUIRED
        //
        status = (numLinks != numConns ? NVL_MORE_PROCESSING_REQUIRED : NVL_SUCCESS);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_powerdown_links_from_active_to_L2_end:

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
 * [PSEUDO-CLEAN SHUTDOWN]
 *
 * Shutdown the given array of links from ACTIVE to OFF state
 *
 * param[in]  links     Array of links to shutdown
 * param[in]  numLinks  Number of links to be shutdown
 * param[in]  flags     Flags to track if the transition is sync/async
 *
 * return NVL_SUCCESS if the pseudo-clean shutdown is successful
 */
NvlStatus
nvlink_lib_powerdown_links_from_active_to_off
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvlStatus               status   = NVL_SUCCESS;
    nvlink_intranode_conn **conns    = NULL;
    nvlink_intranode_conn  *conn     = NULL;
    NvU32                   numConns = 0;
    NvU32                   i,j;
    NvU32                   lockLinkCount = 0;
    nvlink_link           **lockLinks = NULL;
    NvBool                  bIsAlreadyPresent = NV_FALSE;

    if ((links == NULL) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to shutdown\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    lockLinks = (nvlink_link **)nvlink_malloc( sizeof(nvlink_link *) * (2 * numLinks));
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * numLinks);

    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        nvlink_free((void *)lockLinks);
        return NVL_ERR_GENERIC;
    }

    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * numLinks);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_powerdown_links_from_active_to_off_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    //
    // Get the array of both local and remote endpoints whose lock needs
    // to be acquired
    //
    for (i = 0; i < numLinks; i++)
    {
        conn = NULL;
        nvlink_core_get_intranode_conn(links[i], &conn);

        if (conn == NULL)
        {
            continue;
        }

        // Capture both the link and its end-point
        lockLinks[lockLinkCount] = conn->end0;
        lockLinkCount++;

        lockLinks[lockLinkCount] = conn->end1;
        lockLinkCount++;
    }

    if (lockLinkCount == 0)
    {
        if (conns != NULL)
            nvlink_free((void *)conns);

        if (lockLinks != NULL)
            nvlink_free((void *)lockLinks);

         // Release the top-level lock
        nvlink_lib_top_lock_release();
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No conns were found\n", __FUNCTION__));
        return NVL_NOT_FOUND;
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

        goto nvlink_lib_powerdown_links_from_active_to_off_end;
    }

    // Sanity checking if the link is already in OFF/RESET state
    for (i = 0; i < numLinks; i++)
    {
        conn = NULL;
        nvlink_core_get_intranode_conn(links[i], &conn);

        if (conn == NULL)
        {
            continue;
        }

        // Check if both ends of the connection are in L2
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SLEEP);
        if (status == NVL_SUCCESS)
        {
            continue;
        }

        // Check if both ends and their sublinks are in OFF mode
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_OFF);
        if (status == NVL_SUCCESS)
        {
            continue;
        }

        // Check if both ends are in RESET
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_RESET);
        if (status == NVL_SUCCESS)
        {
            continue;
        }

        //
        // If device is using ALI based link training, it is possible
        // for links to be still transitioning to active when a request to shutdown
        // is made. Ensure that all connections transiton successfully to HS or fault
        // before continuining to shutdown
        //
        if(links[0]->dev->enableALI)
        {

            status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_ACTIVE_PENDING);

            if (status == NVL_SUCCESS)
            {
                status = nvlink_core_poll_link_state(conn->end0,
                                             NVLINK_LINKSTATE_HS,
                                             NVLINK_TRANSITION_ACTIVE_PENDING);

                if (status != NVL_SUCCESS &&
                    nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_FAULT) != NVL_SUCCESS)
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Connection between %s: %s and %s: %s is not ready for shutdown (link state is no in HS or FAULT). Soldiering on...\n",
                        __FUNCTION__, conn->end0->dev->deviceName, conn->end0->linkName,
                        conn->end1->dev->deviceName, conn->end1->linkName));
                }
            }
        }

        bIsAlreadyPresent = NV_FALSE;
        // Check if the the connection is already included in the list
        for (j = 0; j < numConns; j++)
        {
            if (conns[j] == conn)
            {
                bIsAlreadyPresent = NV_TRUE;
                break;
            }
        }

        // If this is a new connection, add it to the list
        if (!bIsAlreadyPresent)
        {
            conns[numConns] = conn;
            numConns++;
        }
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Reset status to NVL_SUCCESS
    status = NVL_SUCCESS;

    if (numConns > 0)
    {
        status = nvlink_core_powerdown_intranode_conns_from_active_to_off(conns,
                                                                          numConns,
                                                                          flags);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_powerdown_links_from_active_to_off_end:

    if (conns != NULL)
    {
        nvlink_free((void *)conns);
    }

    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }
    //
    // Squash status. If any side of link doesn not respond the link is
    // shutdown unilaterally
    //
    return NVL_SUCCESS;
}

/**
 * Power down the given array of links from ACTIVE to SWCFG state
 *
 * param[in]  links     Array of links to shutdown
 * param[in]  numLinks  Number of links to be shutdown
 * param[in]  flags     Flags to track if the transition is sync/async
 *
 * return NVL_SUCCESS if the transitions were successful
 */
NvlStatus
nvlink_lib_powerdown_links_from_active_to_swcfg
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvlStatus               status   = NVL_SUCCESS;
    nvlink_intranode_conn **conns    = NULL;
    nvlink_intranode_conn  *conn     = NULL;
    NvU32                   numConns = 0;
    NvU32                   i;
    NvU32                   lockLinkCount = 0;
    nvlink_link           **lockLinks = NULL;


    if ((links == NULL) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to shutdown\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    // Allocate the link locks
    lockLinks = (nvlink_link **)nvlink_malloc(sizeof(nvlink_link *) * (2 * numLinks));
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * numLinks);

    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        nvlink_free((void *)lockLinks);
        return NVL_ERR_GENERIC;
    }

    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * numLinks);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_powerdown_links_from_active_to_swcfg_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    //
    // Get the array of both local and remote endpoints whose lock needs
    // to be acquired
    //
    for (i = 0; i < numLinks; i++)
    {
        conn = NULL;
        nvlink_core_get_intranode_conn(links[i], &conn);

        if (conn == NULL)
        {
            continue;
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

        goto nvlink_lib_powerdown_links_from_active_to_swcfg_end;
    }

    // Sanity checking of links; if already in swfg state, skip it
    for (i = 0; i < numLinks; i++)
    {
        nvlink_intranode_conn *conn = NULL;

        nvlink_core_get_intranode_conn(links[i], &conn);
        if (conn == NULL)
        {
            continue;
        }

        // Check if both ends and their sublinks are in SAFE mode
        status = nvlink_core_check_intranode_conn_state(conn, NVLINK_LINKSTATE_SAFE);
        if (status == NVL_SUCCESS)
        {
            continue;
        }

        conns[numConns] = conn;
        numConns++;
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Reset status to NVL_SUCCESS
    status = NVL_SUCCESS;

    if (numConns > 0)
    {
        status = nvlink_core_powerdown_intranode_conns_from_active_to_swcfg(conns,
                                                                            numConns,
                                                                            flags);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_powerdown_links_from_active_to_swcfg_end:

    if (conns != NULL)
    {
        nvlink_free((void *)conns);
    }

    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }
    //
    // Squash status. If any side of link doesn not respond the link is
    // shutdown unilaterally
    //
    return NVL_SUCCESS;
}

/**
 * Reset the given array of links
 *
 * param[in]  links     Array of links to be reset
 * param[in]  numLinks  Number of links to be shutdown
 * param[in]  flags     Flags to track if the transition is sync/async
 *
 * return NVL_SUCCESS if the links were reset successfully
 */
NvlStatus
nvlink_lib_reset_links
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvlStatus               status   = NVL_SUCCESS;
    nvlink_intranode_conn **conns    = NULL;
    nvlink_intranode_conn  *conn     = NULL;
    NvU32                   numConns = 0;
    NvU32                   i;
    NvU32                   lockLinkCount = 0;
    nvlink_link           **lockLinks = NULL;


    if ((links == NULL) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to reset\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC;
    }

    // Allocate space for the link locks
    lockLinks = (nvlink_link **)nvlink_malloc( sizeof(nvlink_link *) * (2 * numLinks));
    if (lockLinks == NULL)
    {
        return NVL_NO_MEM;
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * numLinks);

    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        nvlink_free((void *)lockLinks);
        return NVL_ERR_GENERIC;
    }

    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * numLinks);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_reset_links_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    // Sanity checking if the link is already in OFF/RESET state
    for (i = 0; i < numLinks; i++)
    {
        conn = NULL;

        nvlink_core_get_intranode_conn(links[i], &conn);
        if (conn == NULL)
        {
            continue;
        }

        // Capture both the link and its end-point
        lockLinks[lockLinkCount] = conn->end0;
        lockLinkCount++;

        lockLinks[lockLinkCount] = conn->end1;
        lockLinkCount++;

        conns[numConns] = conn;
        numConns++;
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

        goto nvlink_lib_reset_links_end;
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    if (numConns > 0)
    {
        status = nvlink_core_reset_intranode_conns(conns, numConns, flags);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(lockLinks, lockLinkCount);

nvlink_lib_reset_links_end:

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


NvlStatus
nvlink_lib_powerdown_floorswept_links_to_off
(
    nvlink_device *dev
)
{
    NvlStatus                status    = NVL_SUCCESS;
    nvlink_link             *link      = NULL;
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

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));
        goto nvlink_core_powerdown_floorswept_conns_to_off_end;
    }

    //
    // If the device has less than or equal links in the IP then
    // can be active, then skip floorsweeping
    //
    if (dev->numActiveLinksPerIoctrl >= dev->numLinksPerIoctrl)
    {
        nvlink_lib_top_lock_release();
        goto nvlink_core_powerdown_floorswept_conns_to_off_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists and connection lists
    //

    // Get the array of link endpoints whose lock needs to be acquired
    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if(link == NULL)
        {
            continue;
        }

        lockLinks[lockLinkCount] = link;
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

        goto nvlink_core_powerdown_floorswept_conns_to_off_end;
    }

    //
    // All the required per-link locks are successfully acquired
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    status = nvlink_core_powerdown_floorswept_conns_to_off(lockLinks, lockLinkCount, dev->numIoctrls,
                        dev->numLinksPerIoctrl, dev->numActiveLinksPerIoctrl);

    if (status == NVL_BAD_ARGS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Bad args passed in for floorsweeping. Chip might not support the feature\n",
            __FUNCTION__));
    }

nvlink_core_powerdown_floorswept_conns_to_off_end:
    if (lockLinks != NULL)
    {
        nvlink_free((void *)lockLinks);
    }

    return status;
}
