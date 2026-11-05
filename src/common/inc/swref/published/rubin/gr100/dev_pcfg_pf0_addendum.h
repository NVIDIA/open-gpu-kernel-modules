/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gr100_dev_pcfg_pf0_addendum_h__
#define __gr100_dev_pcfg_pf0_addendum_h__

#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_FLR_ALIVE                           22:22
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_CONV_RESET_ALIVE                    23:23
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_FLR_PENDING                         25:25
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_CONV_RESET_PENDING                  26:26
#define NV_PF0_DVSEC0_GPU_BOOT_STATUS                                                                0x00000b08
#define NV_PF0_DVSEC0_GPU_BOOT_STATUS_ILLEGAL_UPHY_ACK                                               8:8

#endif // __gr100_dev_pcfg_pf0_addendum_h__
