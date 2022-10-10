/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_timer_h__
#define __gh100_dev_timer_h__

#define NV_PTIMER_TIME_PRIV_LEVEL_MASK                                            0x00009430 /* RW-4R */
#define NV_PTIMER_TIME_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0                           4:4 /*       */
#define NV_PTIMER_TIME_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_ENABLE             0x00000001
#define NV_PTIMER_TIME_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_DISABLE            0x00000000
#define NV_PTIMER_TIME_0_NSEC                                                           31:5 /* R-XUF */

#endif // __gh100_dev_timer_h__
