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
 * TODO: Rework this function to acquire locks and update callers
 *
 * Check if the device has no links registered
 *
 * @param[in]  dev  NVLink Device pointer
 *
 * return NV_TRUE if the device has no links registered
 */
NvBool
nvlink_lib_is_link_list_empty
(
    nvlink_device *dev
)
{
    NvBool isEmpty = NV_TRUE;

    isEmpty = nvListIsEmpty(&dev->link_list);

    return isEmpty;
}

/**
 * Get the link associated with the given link id.
 *
 * @param[in]   device   NVLink Device Pointer
 * @param[in]   link_id  Link Id of the given link
 * @param[out]  link     NVLink Link pointer
 *
 * return NVL_SUCCESS on success
 */
NvlStatus
nvlink_lib_get_link
(
    nvlink_device  *device,
    NvU32           link_id,
    nvlink_link   **link
)
{
    nvlink_link *cur    = NULL;
    NvlStatus    status = -NVL_NOT_FOUND;

    if (device == NULL || link == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    *link = NULL;

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
    // Top-level lock is now acquired. Proceed to traversing the 
    // link list for the device
    //

    // Reset status to -NVL_NOT_FOUND
    status = -NVL_NOT_FOUND;

    FOR_EACH_LINK_REGISTERED(cur, device, node)
    {
        if (cur->linkNumber == link_id)
        {
            *link  = cur;
            status = NVL_SUCCESS;
            break;
        }
    }

    // Release the top level-lock
    nvlink_lib_top_lock_release();

    return status;
}

/**
 * Set the given link as the link master.
 *   This requires that the remote end of the link is known, and that it
 *   hasn't set itself to be the master.
 *
 *   Note: This function is used by RM to set master attribute to a link
 *         in order to handle GPU lock inversion problem while servicing
 *         link interrupts(re-training). With external fabric management
 *         enabled, we don't have the issue. Also we don't have to worry
 *         about the inter-node connections which are managed by FM.
 *
 * @param[in]  link  NVLink Link pointer
 *
 * return NVL_SUCCESS if the master was set
 */
NvlStatus
nvlink_lib_set_link_master
(
    nvlink_link *link
)
{
    nvlink_link           *remote_end = NULL;
    NvlStatus              status     = NVL_SUCCESS;
    nvlink_intranode_conn *conn       = NULL;
    nvlink_link           *links[2]   = {0};
    NvU32                  numLinks   = 0;

    if (link == NULL)
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
    // Top-level lock is now acquired. Proceed to traversing the 
    // connection list
    //

    links[numLinks] = link;
    numLinks++;

    nvlink_core_get_intranode_conn(link, &conn);
    if (conn != NULL)
    {
        remote_end      = (conn->end0 == link ? conn->end1 : conn->end0);
        links[numLinks] = remote_end;
        numLinks++;
    }

    // Acquire the per-link locks for all links captured
    status = nvlink_lib_link_locks_acquire(links, numLinks);
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
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Early return if we've already done this
    if (link->master)
    {
        status = NVL_SUCCESS;
    }
    else
    {
        // Make sure the remote end exists and hasn't claimed the master yet
        if (remote_end == NULL || remote_end->master)
        {
            status = NVL_ERR_INVALID_STATE;
        }
        else
        {
            link->master = NV_TRUE;
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    return status;
}

/**
 * Get the link master associated with the given link.
 *   This may be the given link, or it may be the remote end. In the case
 *   when no master is assigned or the remote end is not known, this will
 *   return an error.
 *
 * @param[in]  link    NVLink Link pointer
 * @param[out] master  Master endpoint for the link
 *
 * return NVL_SUCCESS if the master was found
 */
NvlStatus
nvlink_lib_get_link_master
(
    nvlink_link  *link,
    nvlink_link **master
)
{
    nvlink_link           *remote_end = NULL;
    nvlink_intranode_conn *conn       = NULL;
    NvlStatus              status     = NVL_SUCCESS;
    nvlink_link           *links[2]   = {0};
    NvU32                  numLinks   = 0;

    if (link == NULL || master == NULL)
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
    // Top-level lock is now acquired. Proceed to traversing the 
    // connection list
    //

    links[numLinks] = link;
    numLinks++;

    nvlink_core_get_intranode_conn(link, &conn);
    if (conn != NULL)
    {
        remote_end      = (conn->end0 == link ? conn->end1 : conn->end0);
        links[numLinks] = remote_end;
        numLinks++;
    }

    // Acquire the per-link locks for all links captured
    status = nvlink_lib_link_locks_acquire(links, numLinks);
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
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    if (link->master)
    {
        *master = link;
    }
    else
    {
        // Make sure the remote end exists and hasn't claimed the master yet
        if (remote_end == NULL)
        {
            status = NVL_ERR_INVALID_STATE;
        }

        *master = remote_end;
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    return status;
}

/**
 * Set whether the link is using ALI for training.
 * 
 * @param[in]  link       NVLink Link pointer
 * @param[in]  enableALI  Boolean for whether the link is using
 *                        ALI to train the link
 *
 * return NvlSuccess if setting the variable was successful.
 */
NvlStatus
nvlink_lib_link_set_training_mode
(
    nvlink_link  *link,
    NvBool        enableALI
)
{
    NvlStatus status = NVL_SUCCESS;
    if (link == NULL)
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


    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&link, 1);
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
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // TODO: Add Setter for per-link enableALI state variable

    // Release the per-link lock
    nvlink_lib_link_locks_release(&link, 1);

    return status;
}
