/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_gc6_island_addendum_h__
#define __gh100_dev_gc6_island_addendum_h__

#define NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K                        0x100

#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC                               NV_PGC6_AON_SECURE_SCRATCH_GROUP_20
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MODE_ENABLED                  0:0
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MODE_ENABLED_TRUE             0x1
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MODE_ENABLED_FALSE            0x0
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_DEV_ENABLED                   1:1
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_DEV_ENABLED_TRUE              0x1
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_DEV_ENABLED_FALSE             0x0
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MULTI_GPU_MODE                6:5
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MULTI_GPU_MODE_NONE           0x0
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_CC_MULTI_GPU_MODE_PROTECTED_PCIE 0x1

#endif // __gh100_dev_gc6_island_addendum_h__
