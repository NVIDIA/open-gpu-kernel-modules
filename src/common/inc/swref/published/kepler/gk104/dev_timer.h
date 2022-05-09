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

#ifndef __gk104_dev_timer_h__
#define __gk104_dev_timer_h__
#define NV_PTIMER_INTR_0                                 0x00009100 /* RW-4R */
#define NV_PTIMER_INTR_0_ALARM                                  0:0 /* RWXVF */
#define NV_PTIMER_INTR_0_ALARM_NOT_PENDING               0x00000000 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_PENDING                   0x00000001 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_RESET                     0x00000001 /* -W--V */
#define NV_PTIMER_INTR_EN_0                              0x00009140 /* RW-4R */
#define NV_PTIMER_INTR_EN_0_ALARM                               0:0 /* RWIVF */
#define NV_PTIMER_INTR_EN_0_ALARM_DISABLED               0x00000000 /* RWI-V */
#define NV_PTIMER_INTR_EN_0_ALARM_ENABLED                0x00000001 /* RW--V */
#define NV_PTIMER_TIME_0                                 0x00009400 /* RW-4R */
#define NV_PTIMER_TIME_0_NSEC                                  31:5 /* RWXUF */
#define NV_PTIMER_TIME_1                                 0x00009410 /* RW-4R */
#define NV_PTIMER_TIME_1_NSEC                                  28:0 /* RWXUF */
#define NV_PTIMER_ALARM_0                                0x00009420 /* RW-4R */
#define NV_PTIMER_ALARM_0_NSEC                                 31:5 /* RWXUF */
#endif // __gk104_dev_timer_h__
