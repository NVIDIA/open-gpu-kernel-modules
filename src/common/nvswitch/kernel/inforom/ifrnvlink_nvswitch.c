/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "error_nvswitch.h"

#include "inforom/inforom_nvswitch.h"

NvlStatus
nvswitch_inforom_nvlink_flush
(
    struct nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvlink_load
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_inforom_nvlink_unload
(
    nvswitch_device *device
)
{
    return;
}

NvlStatus
nvswitch_inforom_nvlink_get_minion_data
(
    nvswitch_device *device,
    NvU8             linkId,
    NvU32           *seedData
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvlink_set_minion_data
(
    nvswitch_device *device,
    NvU8             linkId,
    NvU32           *seedData,
    NvU32            size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvlink_log_error_event
(
    nvswitch_device            *device,
    void                       *error_event
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvlink_get_max_correctable_error_rate
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvlink_get_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
