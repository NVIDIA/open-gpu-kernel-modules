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

static NvBool _nvlink_lib_is_device_registered(nvlink_device *);
static NvBool _nvlink_lib_is_link_registered(nvlink_device *, nvlink_link *);

/**
 * Associates device with the NVLink Core Library
 *
 * @param[in]  dev    NVLink Device pointer
 *
 * return  NVL_SUCCESS if the device is registered successfully
 */
NvlStatus
nvlink_lib_register_device
(
    nvlink_device *dev
)
{
    NvlStatus lock_status = NVL_SUCCESS;
    NvlStatus result      = NVL_SUCCESS;

    if (dev == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad device pointer\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    // Acquire top-level lock
    lock_status = nvlink_lib_top_lock_acquire();
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return lock_status;
    }

    // Top-level lock is now acquired

    // Assign the deviceId for the device
    dev->deviceId = (NvU64)(NvUPtr)dev;

    // Assign fabric node id to the device object
    dev->nodeId = nvlinkLibCtx.nodeId;

    // Register the device if not yet registered
    if (!_nvlink_lib_is_device_registered(dev))
    {
        // Initialize the node and link list for the device
        nvListInit(&dev->link_list);
        nvListInit(&dev->node);

        // Add the device to the list of devices
        nvListAppend(&dev->node, &nvlinkLibCtx.nv_devicelist_head.node);

        result = NVL_SUCCESS;
    }
    else
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: %s is already registered in nvlink core\n",
            __FUNCTION__, dev->deviceName));

        result = NVL_ERR_GENERIC;
    }

    // Release top-level lock
    nvlink_lib_top_lock_release();

    return result;
}

/**
 * Unassociates device from the NVLink Core
 * Includes removing any links related to the device if still registered
 *
 * @param[in]  dev    NVLink Device pointer
 *
 * return  NVL_SUCCESS if the device is un-registered successfully
 */
NvlStatus
nvlink_lib_unregister_device
(
    nvlink_device *dev
)
{
    NvBool                bConnected  = NV_FALSE;
    nvlink_intranode_conn *intra_conn = NULL;
    nvlink_internode_conn *inter_conn = NULL;
    NvlStatus             lock_status = NVL_SUCCESS;
    NvU32                 numLinks    = 0;
    nvlink_link           *curLink    = NULL;
    nvlink_link           *nextLink   = NULL;

    if (dev == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad device pointer\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    // Acquire top-level lock
    lock_status = nvlink_lib_top_lock_acquire();
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return lock_status;
    }

    // Top-level lock is now acquired

    // Loop to unregister each link from the device
    FOR_EACH_LINK_REGISTERED_SAFE(curLink, nextLink, dev, node)
    {
        // Reset the variables specific to each link
        bConnected = NV_FALSE;
        intra_conn = NULL;
        inter_conn = NULL;
        numLinks   = 0;

        // We will use at most 2 links in this function - the link and it's partner
        nvlink_link *links[2] = {0};

        links[numLinks] = curLink;
        numLinks++;

        // Check if there's an intranode connection present
        nvlink_core_get_intranode_conn(curLink, &intra_conn);
        if (intra_conn != NULL)
        {
            // Mark the endpoint as connected
            bConnected = NV_TRUE;

            if (intra_conn->end0 == curLink)
            {
                links[numLinks] = intra_conn->end1;
            }
            else
            {
                links[numLinks] = intra_conn->end0;
            }
            numLinks++;
        }

        //
        // Check if there's an internode connection present
        // Only the local end required for internode connection
        // (which is registered above) so just detect this for now
        //
        nvlink_core_get_internode_conn(curLink, &inter_conn);

        // Acquire per-link lock
        lock_status = nvlink_lib_link_locks_acquire(links, numLinks);
        if (lock_status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Failed to acquire per-link locks\n",
                __FUNCTION__));
            nvlink_lib_top_lock_release();
            return lock_status;
        }

        if (intra_conn != NULL)
        {
            // Remove the associated intranode connection with this link from the list
            nvlink_core_remove_intranode_conn(intra_conn);
        }

        if (inter_conn != NULL)
        {
            // Remove the associated internode connection with this link from the list
            nvlink_core_remove_internode_conn(curLink);
        }

        // Remove the link from the link list for the device
        nvListDel(&curLink->node);

        // Release and free the link locks
        nvlink_lib_link_locks_release(links, numLinks);
        nvlink_lib_link_lock_free(curLink);

        curLink->link_handlers->remove(curLink);

        // If the endpoint was not connected
        nvlinkLibCtx.notConnectedEndpoints = ( bConnected ?
                                               nvlinkLibCtx.notConnectedEndpoints :
                                               nvlinkLibCtx.notConnectedEndpoints - 1 );

        // Update count of registered endpoints
        nvlinkLibCtx.registeredEndpoints--;
    }

    nvListDel(&dev->node);

    // Release top-level lock
    nvlink_lib_top_lock_release();

    return NVL_SUCCESS;
}

/**
 * Associates link with a device in the NVLink Core library
 *
 * @param[in]  dev     NVLink Device pointer
 * @param[in]  link  NVLink Link pointer
 *
 * return NVL_SUCCESS if the link is registered successfully
 */
NvlStatus
nvlink_lib_register_link
(
    nvlink_device *dev,
    nvlink_link   *link
)
{
    NvlStatus lock_status = NVL_SUCCESS;
    NvlStatus result      = NVL_SUCCESS;

    if (dev == NULL || link == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad device or link pointer\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    // Allocate per-link lock for the link to be registered
    lock_status = nvlink_lib_link_lock_alloc(link);
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to alloc per-link lock\n",
            __FUNCTION__));

        return lock_status;
    }

    // Acquire top-level lock
    lock_status = nvlink_lib_top_lock_acquire();
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        //
        // Since the per-link lock will be allocated when this function
        // is run again. Free the unused allocated lock.
        //
        nvlink_lib_link_lock_free(link);

        return lock_status;
    }

    // Top-level lock is now acquired

    // Assign the linkId for the device
    link->linkId = (NvU64)(NvUPtr) link;

    // Register the link if not yet registered
    if (!_nvlink_lib_is_link_registered(dev, link))
    {
        // Initialize the node for the link
        nvListInit(&link->node);

        // Generate token for this link
        link->token = (NvU64)(NvUPtr) link;

        // Add the link to the list of links for the device
        nvListAppend(&link->node, &dev->link_list);
        link->link_handlers->add(link);

        // Initialize training parameters
        link->safe_retries             = 0;
        link->packet_injection_retries = 0;

        // Update count of registered endpoints
        nvlinkLibCtx.registeredEndpoints++;

        // Indicate that a new endpoint is registered
        nvlinkLibCtx.bNewEndpoints = NV_TRUE;

        result = NVL_SUCCESS;
    }
    else
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: %s: %s is already registered in nvlink core\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        result = NVL_ERR_GENERIC;

        // Free per-link lock since we don't have a new link
        nvlink_lib_link_lock_free(link); 
    }

    // Release top-level lock
    nvlink_lib_top_lock_release(); 

    return result;
}

/**
 * Unassociates link from a device in the NVLink Core library
 *
 * @param[in]  link NVLink Link pointer
 *
 * return NVL_SUCCESS if the link is un-registered successfully
 */
NvlStatus
nvlink_lib_unregister_link
(
    nvlink_link *link
)
{
    NvBool                bConnected  = NV_FALSE;
    nvlink_intranode_conn *intra_conn = NULL;
    nvlink_internode_conn *inter_conn = NULL;
    NvlStatus             lock_status = NVL_SUCCESS;
    NvU32                 numLinks    = 0;

    // We will use at most 2 links in this function - the link and it's partner
    nvlink_link           *links[2] = {0};

    if (link == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad link pointer\n",
            __FUNCTION__));
        return NVL_ERR_GENERIC;
    }

    // Acquire top-level lock
    lock_status = nvlink_lib_top_lock_acquire();
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return lock_status;
    }

    // Top-level lock is now acquired

    links[numLinks] = link;
    numLinks++;

    // Check if there's an intranode connection present
    nvlink_core_get_intranode_conn(link, &intra_conn);
    if (intra_conn != NULL)
    {
        // Mark the endpoint as connected
        bConnected = NV_TRUE;

        if (intra_conn->end0 == link)
        {
            links[numLinks] = intra_conn->end1;
        }
        else
        {
            links[numLinks] = intra_conn->end0;
        }
        numLinks++;
    }

    //
    // Check if there's an internode connection present
    // Only the local end required for internode connection
    // (which is registered above) so just detect this for now
    //
    nvlink_core_get_internode_conn(link, &inter_conn);

    // Acquire per-link lock
    lock_status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));
        nvlink_lib_top_lock_release();
        return lock_status;
    }

    if (intra_conn != NULL)
    {
        // Remove the associated intranode connection with this link from the list
        nvlink_core_remove_intranode_conn(intra_conn);
    }

    if (inter_conn != NULL)
    {
        // Remove the associated internode connection with this link from the list 
        nvlink_core_remove_internode_conn(link);
    }

    // Remove the link from the link list for the device
    nvListDel(&link->node);

    // Release and free the locks
    nvlink_lib_link_locks_release(links, numLinks); 
    nvlink_lib_link_lock_free(link);

    link->link_handlers->remove(link);

    // If the endpoint was not connected
    nvlinkLibCtx.notConnectedEndpoints = ( bConnected ?
                                           nvlinkLibCtx.notConnectedEndpoints :
                                           nvlinkLibCtx.notConnectedEndpoints - 1 );

    // Update count of registered endpoints
    nvlinkLibCtx.registeredEndpoints--;

    // Release top-level lock
    nvlink_lib_top_lock_release(); 

    return NVL_SUCCESS;
}

/**
 * Check if the nvlink device is already registered in the core library
 *
 * @param[in]  dev    NVLink Device pointer
 *
 * return NV_TRUE if the device is already registered
 */
static NvBool
_nvlink_lib_is_device_registered
(
    nvlink_device *dev
)
{
    nvlink_device *tmpDev = NULL;

    FOR_EACH_DEVICE_REGISTERED(tmpDev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        if (dev->deviceId == tmpDev->deviceId)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/**
 * Check if the nvlink link is already registered in the core library
 *
 * @param[in]  dev     NVLink Device pointer
 * @param[in]  link  NVLink Link pointer
 *
 * return NV_TRUE if the link is already registered for the device
 */
static NvBool
_nvlink_lib_is_link_registered
(
    nvlink_device *dev,
    nvlink_link   *link
)
{
    nvlink_link *tmpLink = NULL;

    FOR_EACH_LINK_REGISTERED(tmpLink, dev, node)
    {
        if (link->linkId == tmpLink->linkId)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}
