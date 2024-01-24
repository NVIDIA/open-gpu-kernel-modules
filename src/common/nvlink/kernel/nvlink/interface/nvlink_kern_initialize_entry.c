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
 * Re-Initialize a given link from OFF to SWCFG
 *
 * @param[in]  link   NVLink Link pointer
 * @param[in]  flags  Flag to track if the initialization is aync/async
 *
 * return NVL_SUCCESS if the initialization was successful
 */
NvlStatus
nvlink_lib_reinit_link_from_off_to_swcfg
(
    nvlink_link *link,
    NvU32        flags
)
{
    NvlStatus              status   = NVL_SUCCESS;
    nvlink_intranode_conn *conn     = NULL;
    nvlink_link           *links[2] = {0};

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
    // Top-level lock is now acquired. Proceed to traversing the 
    // connection list
    //

    nvlink_core_get_intranode_conn(link, &conn);
    if (!conn)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No connection was found for this link.\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return NVL_ERR_GENERIC;
    }

    links[0] = conn->end0;
    links[1] = conn->end1;

    // Acquire the per-link locks for all links captured
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
    // The connection list traversal is also complete now
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    if (conn->end0->version >= NVLINK_DEVICE_VERSION_40 ||
        conn->end1->version >= NVLINK_DEVICE_VERSION_40)
    {
        if (!conn->end0->dev->enableALI)
        {
            nvlink_core_init_links_from_off_to_swcfg_non_ALI(links, 2, flags);
        }
    } 
    else
    {
        nvlink_core_init_links_from_off_to_swcfg(links, 2, flags);
    }
    // Release the per-link locks
    nvlink_lib_link_locks_release(links, 2);

    return NVL_SUCCESS;
}
