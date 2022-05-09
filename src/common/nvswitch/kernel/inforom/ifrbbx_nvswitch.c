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
#include "regkey_nvswitch.h"
#include "nvVer.h"
#include "inforom/inforom_nvswitch.h"

void
nvswitch_bbx_collect_current_time
(
    nvswitch_device     *device,
    void                *pBbxState
)
{
    return;
}

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
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_inforom_bbx_unload
(
    nvswitch_device *device
)
{
    return;
}

NvlStatus
nvswitch_inforom_bbx_load
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_bbx_get_sxid
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
