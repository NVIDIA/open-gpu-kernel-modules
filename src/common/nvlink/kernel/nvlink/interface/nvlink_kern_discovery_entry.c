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
 * Get the connected remote endpoint information
 *
 *   For a given link, return the remote endpoint details it is connected to.
 *   If there is no connection associated with the specified link, then, the
 *   conn_info.connected member will be NV_FALSE.
 *
 *   Note: This routine will not initiate any link initialization or topology
 *   discovery.
 *
 * @param[in]  link       NVLink Link pointer
 * @param[out] conn_info  Details of remote endpoint
 */
NvlStatus
nvlink_lib_get_remote_conn_info
(
    nvlink_link      *link,
    nvlink_conn_info *conn_info
)
{
    NvlStatus              status    = NVL_SUCCESS;
    nvlink_link           *remoteEnd = NULL;
    nvlink_intranode_conn *intraConn = NULL;
    nvlink_internode_conn *interConn = NULL;
    NvU32                  numLinks  = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Initialize connected state to false
    conn_info->bConnected = NV_FALSE;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the 
    // connnection list
    //

    // Find the associated intranode connection with this link
    nvlink_core_get_intranode_conn(link, &intraConn);
    if (intraConn != NULL)
    {
        // Get the required remote endpoint of the connection
        remoteEnd = (intraConn->end0 == link ?
                     intraConn->end1 : intraConn->end0);

        // Mark the connected state
        conn_info->bConnected = NV_TRUE;

        if ((numLinks+1) >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = link;
        numLinks++;

        links[numLinks] = remoteEnd;
        numLinks++;
    }

    //
    // On multi-node systems, check the internode connection
    // list as well to return the connection information
    //
    nvlink_core_get_internode_conn(link, &interConn);
    if (interConn != NULL)
    {
        // Mark the connected state
        conn_info->bConnected = NV_TRUE;

        links[numLinks] = link;
        numLinks++;
    }

    // Acquire per-link lock
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        nvlink_lib_top_lock_release();
        nvlink_free((void *)links);
        return status;
    }

    if (intraConn != NULL)
    {
        nvlink_core_copy_intranode_conn_info(remoteEnd, conn_info);
    }
    else
    {
        if (interConn != NULL)
        {
            nvlink_core_copy_internode_conn_info(&interConn->remote_end,
                                                conn_info);
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks); 

    // Release top-level lock
    nvlink_lib_top_lock_release();

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Get the connected remote endpoint information
 *
 *   For a given link, return the remote endpoint it is connected to.
 *
 *   Note: This routine triggers topology discovery on the set of
 *         links registered in the core library
 *
 * @param[in]  end        NVLink Link pointer
 * @param[out] conn_info  Details of remote endpoint
 * @param[in]  flags      Flags
 */
NvlStatus
nvlink_lib_discover_and_get_remote_conn_info
(
    nvlink_link      *end,
    nvlink_conn_info *conn_info,
    NvU32             flags,
    NvBool            bForceDiscovery
)
{
    NvlStatus      status                            = NVL_SUCCESS;
    nvlink_link   *link                              = NULL;
    nvlink_link   *remote_end                        = NULL;
    nvlink_device *dev                               = NULL;
    NvU32          numLinks                          = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
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

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(link, dev, node)
        {
            if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                    __FUNCTION__));

                nvlink_assert(0);

                // Release the top-level lock and free links
                nvlink_lib_top_lock_release();
                nvlink_free((void *)links);
                return NVL_ERR_INVALID_STATE;
            }

            links[numLinks] = link;
            numLinks++;
        }
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    // Initialize connected state to false
    conn_info->bConnected = NV_FALSE;

    // Get the remote_end of the link
    nvlink_core_discover_and_get_remote_end(end, &remote_end, flags,
                                            bForceDiscovery);

    if (remote_end)
    {
        // mark the connected state
        conn_info->bConnected = NV_TRUE;
        nvlink_core_copy_intranode_conn_info(remote_end, conn_info);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}
