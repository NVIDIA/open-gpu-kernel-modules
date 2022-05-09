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

#ifndef __gm107_dev_timer_h__
#define __gm107_dev_timer_h__
#define NV_PTIMER_TIME_0                                  0x00009400 /* RW-4R */
#define NV_PTIMER_TIME_1                                  0x00009410 /* RW-4R */
#define NV_PTIMER_TIMER_CFG0                              0x00009300 /* RW-4R */
#define NV_PTIMER_TIMER_CFG0_DEN                                 4:0 /* RWIUF */
#define NV_PTIMER_TIMER_CFG0_DEN_108MHZ_REF                     0x1b /* RWI-V */
#define NV_PTIMER_TIMER_CFG0_NUM                               18:16 /* RWIUF */
#define NV_PTIMER_TIMER_CFG0_NUM_108MHZ_REF                      0x7 /* RWI-V */
#define NV_PTIMER_TIMER_CFG1                              0x00009304 /* RW-4R */
#define NV_PTIMER_TIMER_CFG1_INTEGER                             5:0 /* RWIUF */
#define NV_PTIMER_TIMER_CFG1_INTEGER_108MHZ_REF             0x000009 /* RWI-V */
#define NV_PTIMER_GR_TICK_FREQ                           0x00009480 /* RW-4R */
#define NV_PTIMER_GR_TICK_FREQ_SELECT                           2:0 /* RWIUF */
#define NV_PTIMER_GR_TICK_FREQ_SELECT_MAX                0x00000000 /* RW--V */
#define NV_PTIMER_GR_TICK_FREQ_SELECT_DEFAULT            0x00000005 /* RWI-V */
#define NV_PTIMER_GR_TICK_FREQ_SELECT_MIN                0x00000007 /* RW--V */
#define NV_PTIMER_TIME_0_NSEC                                  31:5 /* RWXUF */
#define NV_PTIMER_TIME_1_NSEC                                  28:0 /* RWXUF */

#endif // __gm107_dev_timer_h__
