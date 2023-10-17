/*******************************************************************************
    Copyright (c) 2023 NVidia Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*******************************************************************************/

#include "nvlink.h"
#include "nvlink_export.h"
#include "nvlink_os.h"
#include "../nvlink_ctx.h"
#include "../nvlink_helper.h"

#include "nvlink_lock.h"

/**
 * Update UUID and deviceName in the Nvlink Core library
 *
 * @param[in]  devInfo    NVLink Device Info pointer
 * @param[in]  uuid       Device UUID pointer
 * @param[in]  deviceName DeviceName pointer
 *
 * return NVL_SUCCESS if the update was successful
 */
NvlStatus
nvlink_lib_update_uuid_and_device_name
(
    nvlink_device_info *devInfo,
    NvU8 *uuid,
    char *deviceName
)
{
    NvlStatus lock_status = NVL_SUCCESS;
    nvlink_device *dev = NULL;

    if (devInfo == NULL || uuid == NULL || deviceName == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad input arguments!\n",
            __FUNCTION__));
        return NVL_BAD_ARGS;        
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

    devInfo->nodeId = nvlinkLibCtx.nodeId;

    // Get the Corelib device pointer and store it in nvlink_device struct
    nvlink_core_get_device_by_devinfo(devInfo, &dev);
    if (dev == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
        "%s: Device not registered in corelib\n", __FUNCTION__));

        // Release top-level lock
        nvlink_lib_top_lock_release();

        return NVL_NOT_FOUND;
    }

    // Update the UUID and device name cached in Corelib for the correct device
    dev->uuid = uuid;
    dev->deviceName = deviceName;

    // Release top-level lock
    nvlink_lib_top_lock_release(); 

    return NVL_SUCCESS;
}
