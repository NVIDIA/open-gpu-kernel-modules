/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_riscv_pri_h__
#define __ga100_dev_riscv_pri_h__

#define NV_FALCON2_GSP_BASE 0x00111000
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS                                                       0x00000240     /* R-I4R */
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS_ACTIVE_STAT                                           0:0            /* R-IVF */
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS_ACTIVE_STAT_ACTIVE                                    0x00000001     /* R---V */
#define NV_PRISCV_RISCV_IRQMASK                                                                        0x000002b4     /* R-I4R */
#define NV_PRISCV_RISCV_IRQDEST                                                                        0x000002b8     /* RW-4R */

#endif // __ga100_dev_riscv_pri_h__
