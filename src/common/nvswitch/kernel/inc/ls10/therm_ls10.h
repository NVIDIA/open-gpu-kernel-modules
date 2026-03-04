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

#ifndef _THERM_LS10_H_
#define _THERM_LS10_H_

NvlStatus
nvswitch_init_thermal_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_ctrl_therm_read_temperature_ls10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info
);

NvlStatus
nvswitch_ctrl_therm_get_temperature_limit_ls10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS *info
);

void
nvswitch_monitor_thermal_alert_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_ctrl_therm_read_voltage_ls10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_VOLTAGE_PARAMS *info
);

NvlStatus
nvswitch_ctrl_therm_read_power_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_POWER_PARAMS *info
);
#endif //_THERM_LS10_H_
