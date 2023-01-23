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

#ifndef _PMGR_LS10_H_
#define _PMGR_LS10_H_

#include "ls10.h"

void
nvswitch_init_pmgr_ls10
(
    nvswitch_device *device
);

void
nvswitch_init_pmgr_devices_ls10
(
    nvswitch_device *device
);

NvU32
nvswitch_read_physical_id_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_get_rom_info_ls10
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom
);

void
nvswitch_i2c_set_hw_speed_mode_ls10
(
    nvswitch_device *device,
    NvU32 port,
    NvU32 speedMode
);

NvBool
nvswitch_is_i2c_supported_ls10
(
    nvswitch_device *device
);

#endif //_PMGR_LS10_H_
