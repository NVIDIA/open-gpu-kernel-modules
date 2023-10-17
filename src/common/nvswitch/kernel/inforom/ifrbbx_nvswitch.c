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

#include "common_nvswitch.h"
#include "inforom/inforom_nvswitch.h"


NvlStatus
nvswitch_inforom_bbx_add_sxid
(
    nvswitch_device *device,
    NvU32            exceptionType,
    NvU32            data0,
    NvU32            data1,
    NvU32            data2
)
{
    NvlStatus status;

    status = device->hal.nvswitch_bbx_add_sxid(device, exceptionType, data0, data1, data2);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_inforom_bbx_add_sxid failed, status=%d\n", status);
    }

    return status;
}

void
nvswitch_inforom_bbx_unload
(
    nvswitch_device *device
)
{
    NvlStatus status;

    status = device->hal.nvswitch_bbx_unload(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_inforom_bbx_unload failed, status=%d\n", status);
    }

    return;
}

NvlStatus
nvswitch_inforom_bbx_load
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU64 time_ns = 0;
    NvU32     majorVer;
    NvU32     minorVer;
    NvU32     buildNum;
    NvU8        osType;
    NvU32       osVersion;

    osType = INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_UNIX;

    status = nvswitch_os_get_os_version(&majorVer, &minorVer, &buildNum);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get OS version, status=%d\n",
            __FUNCTION__, status);
        return status;
    }

    if ((majorVer > 0xff) || (minorVer > 0xff) || (buildNum > 0xffff))
    {
        NVSWITCH_PRINT(device, ERROR,
            "Unexpected OS versions found. majorVer: 0x%x minorVer: 0x%x buildNum: 0x%x\n",
            majorVer, minorVer, buildNum);
        return -NVL_ERR_NOT_SUPPORTED;;
    }

    osVersion =
        REF_NUM(INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_MAJOR, majorVer) |
        REF_NUM(INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_MINOR, minorVer) |
        REF_NUM(INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_BUILD, buildNum);

    time_ns = nvswitch_os_get_platform_time_epoch();

    status = device->hal.nvswitch_bbx_load(device, time_ns, osType, osVersion);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_inforom_bbx_load failed, status=%d\n", status);
    }

    return status;
}

NvlStatus
nvswitch_inforom_bbx_get_sxid
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    NvlStatus status;

    status = device->hal.nvswitch_bbx_get_sxid(device, params);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_inforom_bbx_load failed, status=%d\n", status);
    }

    return status;
}

NvlStatus
nvswitch_inforom_bbx_get_data
(
    nvswitch_device *device,
    NvU8 dataType,
    void *params
)
{
    NvlStatus status;

    status = device->hal.nvswitch_bbx_get_data(device, dataType, params);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: (type=%d) failed, status=%d\n", __FUNCTION__, dataType, status);
    }

    return status;
}
