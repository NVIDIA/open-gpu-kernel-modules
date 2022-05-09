/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gm200_dev_timer_h__
#define __gm200_dev_timer_h__
#define NV_PTIMER_INTR_0                                 0x00009100 /* RW-4R */
#define NV_PTIMER_INTR_0_TIMER                                  1:1 /* RWXVF */
#define NV_PTIMER_INTR_0_TIMER_NOT_PENDING               0x00000000 /* R---V */
#define NV_PTIMER_INTR_0_TIMER_PENDING                   0x00000001 /* R---V */
#define NV_PTIMER_INTR_0_TIMER_RESET                     0x00000001 /* -W--C */
#define NV_PTIMER_INTR_EN_0                              0x00009140 /* RW-4R */
#define NV_PTIMER_INTR_EN_0_TIMER                               1:1 /* RWIVF */
#define NV_PTIMER_INTR_EN_0_TIMER_DISABLED               0x00000000 /* RWI-V */
#define NV_PTIMER_INTR_EN_0_TIMER_ENABLED                0x00000001 /* RW--V */
#define NV_PTIMER_TIMER_0                                0x00009428 /* RW-4R */
#endif // __gm200_dev_timer_h__
