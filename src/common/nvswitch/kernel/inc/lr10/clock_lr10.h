/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _CLOCK_LR10_H_
#define _CLOCK_LR10_H_

NvlStatus
nvswitch_init_pll_config_lr10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_init_pll_lr10
(
    nvswitch_device *device
);

void
nvswitch_init_hw_counter_lr10
(
    nvswitch_device *device
);

void
nvswitch_hw_counter_shutdown_lr10
(
    nvswitch_device *device
);

NvU64
nvswitch_hw_counter_read_counter_lr10
(
    nvswitch_device *device
);

void
nvswitch_init_clock_gating_lr10
(
    nvswitch_device *device
);

#endif //_CLOCK_LR10_H_
