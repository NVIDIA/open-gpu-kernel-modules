/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb202_dev_riscv_pri_h__
#define __gb202_dev_riscv_pri_h__

#define NV_PRISCV_RISCV_CPUCTL                                                                         0x00000388     /* RW-4R */
#define NV_PRISCV_RISCV_RPC                                                                            0x000003ec     /* R--4R */
#define NV_PRISCV_RISCV_IRQDELEG                                                                       0x00000534     /* RW-4R */
#define NV_PRISCV_RISCV_IRQDEST                                                                        0x0000052c     /* RW-4R */
#define NV_PRISCV_RISCV_IRQMASK                                                                        0x00000528     /* R--4R */
#define NV_PRISCV_RISCV_PRIV_ERR_STAT                                                                  0x00000420     /* RW-4R */
#define NV_PRISCV_RISCV_PRIV_ERR_INFO                                                                  0x00000424     /* R--4R */
#define NV_PRISCV_RISCV_PRIV_ERR_ADDR                                                                  0x00000428     /* R--4R */
#define NV_PRISCV_RISCV_PRIV_ERR_ADDR_HI                                                               0x0000042c     /* R--4R */
#define NV_PRISCV_RISCV_HUB_ERR_STAT                                                                   0x00000430     /* RW-4R */

#endif // __gb202_dev_riscv_pri_h__
