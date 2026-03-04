/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _SMBPBI_LS10_H_
#define _SMBPBI_LS10_H_

NvlStatus
nvswitch_smbpbi_alloc_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_smbpbi_post_init_hal_ls10
(
    nvswitch_device *device
);

void
nvswitch_smbpbi_destroy_hal_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_smbpbi_get_dem_num_messages_ls10
(
    nvswitch_device *device,
    NvU8            *pMsgCount
);

NvlStatus
nvswitch_inforom_dem_load_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_smbpbi_dem_load_ls10
(
    nvswitch_device *device
);

void
nvswitch_smbpbi_send_unload_ls10
(
    nvswitch_device *device
);

void
nvswitch_smbpbi_dem_flush_ls10
(
    nvswitch_device *device
);

void
nvswitch_smbpbi_log_message_ls10
(
    nvswitch_device *device,
    NvU32           num,
    NvU32           msglen,
    NvU8            *osErrorString
);

NvlStatus
nvswitch_smbpbi_send_init_data_ls10
(
    nvswitch_device *device
);

#endif //_SMBPBI_LS10_H_
