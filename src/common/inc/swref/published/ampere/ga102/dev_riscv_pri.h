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

#ifndef __ga102_dev_riscv_pri_h__
#define __ga102_dev_riscv_pri_h__

#define NV_FALCON2_GSP_BASE 0x00111000
#define NV_PRISCV_RISCV_IRQMASK                                                                        0x00000528     /* R-I4R */
#define NV_PRISCV_RISCV_IRQDEST                                                                        0x0000052c     /* RW-4R */
#define NV_PRISCV_RISCV_CPUCTL                                                                         0x00000388     /* RWI4R */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT                                                             7:7            /* R-IVF */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT_ACTIVE                                                      0x00000001     /* R---V */
#define NV_PRISCV_RISCV_CPUCTL_HALTED                                                                  4:4            /* R-IVF */
#define NV_PRISCV_RISCV_BCR_CTRL                                                                       0x00000668     /* RWI4R */
#define NV_PRISCV_RISCV_BCR_CTRL_VALID                                                                 0:0            /* R-IVF */
#define NV_PRISCV_RISCV_BCR_CTRL_VALID_TRUE                                                            0x00000001     /* R---V */
#define NV_PRISCV_RISCV_BCR_CTRL_VALID_FALSE                                                           0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_BCR_CTRL_CORE_SELECT                                                           4:4            /* RWIVF */
#define NV_PRISCV_RISCV_BCR_CTRL_CORE_SELECT_FALCON                                                    0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_CTRL_CORE_SELECT_RISCV                                                     0x00000001     /* RW--V */
#define NV_PRISCV_RISCV_BCR_CTRL_BRFETCH                                                               8:8            /* RWIVF */
#define NV_PRISCV_RISCV_BCR_CTRL_BRFETCH_TRUE                                                          0x00000001     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_CTRL_BRFETCH_FALSE                                                         0x00000000     /* RW--V */

#endif // __ga102_dev_riscv_pri_h__
