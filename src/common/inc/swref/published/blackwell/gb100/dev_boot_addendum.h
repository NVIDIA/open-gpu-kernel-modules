/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef gb100_dev_boot_addendum_h
#define gb100_dev_boot_addendum_h

#define NV_PMC_SCRATCH_RESET_2_CC                      NV_PMC_SCRATCH_RESET_2(4)
#define NV_PMC_SCRATCH_RESET_2_CC_MODE_ENABLED         0:0
#define NV_PMC_SCRATCH_RESET_2_CC_MODE_ENABLED_TRUE    0x1
#define NV_PMC_SCRATCH_RESET_2_CC_MODE_ENABLED_FALSE   0x0
#define NV_PMC_SCRATCH_RESET_2_CC_DEV_ENABLED          1:1
#define NV_PMC_SCRATCH_RESET_2_CC_DEV_ENABLED_TRUE     0x1
#define NV_PMC_SCRATCH_RESET_2_CC_DEV_ENABLED_FALSE    0x0

#endif // gb100_dev_boot_addendum_h
