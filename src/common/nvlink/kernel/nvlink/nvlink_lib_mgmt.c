/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvlink_ctx.h"
#include "nvlink_helper.h"

#include "nvlink_lock.h"

nvlink_lib_context nvlinkLibCtx = {0};

/*
 * Initialize the nvlink core library
 *
 * return NVL_SUCCESS if the library is initialized successfully
 */
NvlStatus
nvlink_lib_initialize(void)
{
    NvlStatus lock_status = NVL_SUCCESS;

    if (nvlinkLibCtx.nv_devicelist_head.initialized == 0)
    {
        // Allocate top-level lock
        lock_status = nvlink_lib_top_lock_alloc();
        if (lock_status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Failed to allocate top-level lock\n",
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

            return lock_status;
         }

        // Top-level lock is now acquired

        // Initialize the device list head
        nvListInit(&nvlinkLibCtx.nv_devicelist_head.link_list);
        nvListInit(&nvlinkLibCtx.nv_devicelist_head.node);
        nvlinkLibCtx.nv_devicelist_head.initialized = 1;

        // Initialize the intranode connection list head
        nvListInit(&nvlinkLibCtx.nv_intraconn_head.node);

        // Initialize the internode connection list head
        nvListInit(&nvlinkLibCtx.nv_interconn_head.node);

        // Initialize registered and connected links to 0
        nvlinkLibCtx.registeredEndpoints   = 0;
        nvlinkLibCtx.connectedEndpoints    = 0;
        nvlinkLibCtx.notConnectedEndpoints = 0;

        //
        // Initialize fabric node id to max value until set
        // by ioctl interface
        //
        nvlinkLibCtx.nodeId = NV_U16_MAX ;

        // Release top-level lock
        nvlink_lib_top_lock_release(); 
    }

    return NVL_SUCCESS;
}

/*
 * Unload the nvlink core library
 *
 * return NVL_SUCCESS if the library is unloaded successfully
 */
NvlStatus
nvlink_lib_unload(void)
{
    NvlStatus lock_status = NVL_SUCCESS;

    if (nvlink_lib_is_initialized())
    {
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

        // Check if there are no devices registered
        if (nvlink_lib_is_device_list_empty())
        {
            nvlinkLibCtx.nv_devicelist_head.initialized = 0;
        }

        // Release and free top-level lock
        nvlink_lib_top_lock_release();
        nvlink_lib_top_lock_free();
    }

    return NVL_SUCCESS;
}

/*
 * Check if the nvlink core library is initialized
 *
 * return NV_TRUE if the core library is already initialized
 */
NvBool
nvlink_lib_is_initialized(void)
{
    return nvlinkLibCtx.nv_devicelist_head.initialized;
}

/*
 * Check if there are any devices registered
 *
 * return NV_TRUE if there are devices registered in the core library
 */
NvBool
nvlink_lib_is_device_list_empty(void)
{
    NvBool isEmpty = NV_TRUE;

    isEmpty = nvListIsEmpty(&nvlinkLibCtx.nv_devicelist_head.node);

    return isEmpty;
}

/*
 * Get if a device registerd to the nvlink corelib has a reduced nvlink config
 *
 * return NV_TRUE if there is a device registered to the core library that is a reduced
 * nvlink config device
 */
NvBool
nvlink_lib_is_registerd_device_with_reduced_config(void)
{
    NvlStatus lock_status           = NVL_SUCCESS;
    nvlink_device *dev              = NULL;
    NvBool         bIsReducedConfig = NV_FALSE;

    // Acquire top-level lock
    lock_status = nvlink_lib_top_lock_acquire();
    if (lock_status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return NV_FALSE;
     }

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        //
        // If the device is a reduced config set bIsReducedConfig to NV_TRUE
        // and break to ensure that top level lock is released below
        //
        if (dev->bReducedNvlinkConfig == NV_TRUE)
        {
            bIsReducedConfig = NV_TRUE;
            break;
        }
    }

    // Release top-level lock
    nvlink_lib_top_lock_release();

    return bIsReducedConfig;
}

/*
* Get the number of devices that have the device type deviceType
*/
NvlStatus
nvlink_lib_return_device_count_by_type
(
    NvU32 deviceType,
    NvU32 *numDevices
)
{
    NvlStatus lock_status = NVL_SUCCESS;
    nvlink_device *dev = NULL;
    NvU32 device_count = 0;

    if (nvlink_lib_is_initialized())
    {
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

        // Loop through device list
        FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
        {
            if (dev->type == deviceType)
            {
                device_count++;
            }
        }

        // Release top-level lock
        nvlink_lib_top_lock_release(); 
    }
    *numDevices = device_count;
    return NVL_SUCCESS;
}
