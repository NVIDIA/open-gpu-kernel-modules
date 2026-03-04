/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef ga100_dev_nv_bus_addendum_h
#define ga100_dev_nv_bus_addendum_h

#define NV_PBUS_SW_SCRATCH1_SMC_MODE                               15:15
#define NV_PBUS_SW_SCRATCH1_SMC_MODE_OFF                           0x00000000
#define NV_PBUS_SW_SCRATCH1_SMC_MODE_ON                            0x00000001

//
// This bit is set by RM to indicate that a GPU reset is recommended after
// work is drained
//
#define NV_PBUS_SW_SCRATCH30_GPU_DRAIN_AND_RESET_RECOMMENDED       1:1
#define NV_PBUS_SW_SCRATCH30_GPU_DRAIN_AND_RESET_RECOMMENDED_NO    0x00000000
#define NV_PBUS_SW_SCRATCH30_GPU_DRAIN_AND_RESET_RECOMMENDED_YES   0x00000001

#endif // ga100_dev_nv_bus_addendum_h
