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

#ifndef __ga102_dev_riscv_pri_h__
#define __ga102_dev_riscv_pri_h__

#define NV_FALCON2_GSP_BASE 0x00111000
#define NV_PRISCV_RISCV_IRQMASK                                                                        0x00000528     /* R-I4R */
#define NV_PRISCV_RISCV_IRQDEST                                                                        0x0000052c     /* RW-4R */
#define NV_PRISCV_RISCV_IRQDELEG                                                                       0x00000534     /* RWI4R */
#define NV_PRISCV_RISCV_RPC                                                                            0x000003ec     /* R--4R */
#define NV_PRISCV_RISCV_CPUCTL                                                                         0x00000388     /* RWI4R */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT                                                             7:7            /* R-IVF */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT_ACTIVE                                                      0x00000001     /* R---V */
#define NV_PRISCV_RISCV_CPUCTL_HALTED                                                                  4:4            /* R-IVF */
#define NV_PRISCV_RISCV_ICD_CMD                                                                        0x000003d0     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_ADDR0                                                                      0x000003d4     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_ADDR1                                                                      0x000003d8     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_RDATA0                                                                     0x000003e4     /* R--4R */
#define NV_PRISCV_RISCV_ICD_RDATA1                                                                     0x000003e8     /* R--4R */
#define NV_PRISCV_RISCV_TRACECTL                                                                       0x00000400     /* RW-4R */
#define NV_PRISCV_RISCV_TRACECTL_FULL                                                                  30:30          /* RWIVF */
#define NV_PRISCV_RISCV_TRACE_RDIDX                                                                    0x00000404     /* RW-4R */
#define NV_PRISCV_RISCV_TRACE_RDIDX_RDIDX                                                              7:0            /* RWIVF */
#define NV_PRISCV_RISCV_TRACE_RDIDX_MAXIDX                                                             23:16          /* R-IVF */
#define NV_PRISCV_RISCV_TRACE_WTIDX                                                                    0x00000408     /* RW-4R */
#define NV_PRISCV_RISCV_TRACE_WTIDX_WTIDX                                                              31:24          /* RWIVF */
#define NV_PRISCV_RISCV_TRACEPC_HI                                                                     0x00000410     /* RW-4R */
#define NV_PRISCV_RISCV_TRACEPC_LO                                                                     0x0000040c     /* RW-4R */
#define NV_PRISCV_RISCV_PRIV_ERR_STAT                                                                  0x00000500     /* RWI4R */
#define NV_PRISCV_RISCV_PRIV_ERR_INFO                                                                  0x00000504     /* R-I4R */
#define NV_PRISCV_RISCV_PRIV_ERR_ADDR                                                                  0x00000508     /* R-I4R */
#define NV_PRISCV_RISCV_PRIV_ERR_ADDR_HI                                                               0x0000050c     /* R-I4R */
#define NV_PRISCV_RISCV_HUB_ERR_STAT                                                                   0x00000510     /* RWI4R */
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
