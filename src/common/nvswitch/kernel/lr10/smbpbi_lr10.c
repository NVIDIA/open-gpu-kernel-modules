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
#include "lr10/lr10.h"
#include "lr10/smbpbi_lr10.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_nvlsaw_ip_addendum.h"


NvlStatus
nvswitch_smbpbi_get_dem_num_messages_lr10
(
    nvswitch_device *device,
    NvU8            *pMsgCount
)
{
    NvU32 reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_12);

    *pMsgCount = DRF_VAL(_NVLSAW_SW, _SCRATCH_12, _EVENT_MESSAGE_COUNT, reg);

    return NVL_SUCCESS;
}
