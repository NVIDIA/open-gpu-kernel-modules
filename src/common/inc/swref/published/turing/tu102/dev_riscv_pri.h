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

#ifndef __tu102_dev_riscv_pri_h__
#define __tu102_dev_riscv_pri_h__

#define NV_FALCON2_GSP_BASE 0x00111000
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS                                                       0x00000240     /* R-I4R */
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS_ACTIVE_STAT                                           0:0            /* R-IVF */
#define NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS_ACTIVE_STAT_ACTIVE                                    0x00000001     /* R---V */
#define NV_PRISCV_RISCV_CPUCTL                                                                         0x00000268     /* RWI4R */
#define NV_PRISCV_RISCV_IRQMASK                                                                        0x000002b4     /* R-I4R */
#define NV_PRISCV_RISCV_IRQDEST                                                                        0x000002b8     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_CMD                                                                        0x00000300     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_CMD_OPC                                                                    4:0            /* RW-VF */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_STOP                                                               0x00000000     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_RREG                                                               0x00000008     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_RDM                                                                0x0000000a     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_RSTAT                                                              0x0000000e     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_RCSR                                                               0x00000010     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_OPC_RPC                                                                0x00000012     /* RW--V */
#define NV_PRISCV_RISCV_ICD_CMD_SZ                                                                     7:6            /* RW-VF */
#define NV_PRISCV_RISCV_ICD_CMD_IDX                                                                    12:8           /* RW-VF */
#define NV_PRISCV_RISCV_ICD_CMD_ERROR                                                                  14:14          /* R-IVF */
#define NV_PRISCV_RISCV_ICD_CMD_ERROR_TRUE                                                             0x00000001     /* R---V */
#define NV_PRISCV_RISCV_ICD_CMD_ERROR_FALSE                                                            0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_ICD_CMD_BUSY                                                                   15:15          /* R-IVF */
#define NV_PRISCV_RISCV_ICD_CMD_BUSY_FALSE                                                             0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_ICD_CMD_BUSY_TRUE                                                              0x00000001     /* R---V */
#define NV_PRISCV_RISCV_ICD_CMD_PARM                                                                   31:16          /* RW-VF */
#define NV_PRISCV_RISCV_ICD_RDATA0                                                                     0x0000030c     /* R--4R */
#define NV_PRISCV_RISCV_ICD_RDATA1                                                                     0x00000318     /* R--4R */
#define NV_PRISCV_RISCV_ICD_ADDR0                                                                      0x00000304     /* RW-4R */
#define NV_PRISCV_RISCV_ICD_ADDR1                                                                      0x00000310     /* RW-4R */
#define NV_PRISCV_RISCV_TRACECTL                                                                       0x00000344     /* RW-4R */
#define NV_PRISCV_RISCV_TRACECTL_FULL                                                                  30:30          /* RWIVF */
#define NV_PRISCV_RISCV_TRACE_RDIDX                                                                    0x00000348     /* RW-4R */
#define NV_PRISCV_RISCV_TRACE_RDIDX_RDIDX                                                              7:0            /* RWIVF */
#define NV_PRISCV_RISCV_TRACE_RDIDX_MAXIDX                                                             23:16          /* R-IVF */
#define NV_PRISCV_RISCV_TRACE_WTIDX                                                                    0x0000034c     /* RW-4R */
#define NV_PRISCV_RISCV_TRACE_WTIDX_WTIDX                                                              31:24          /* RWIVF */
#define NV_PRISCV_RISCV_TRACEPC_HI                                                                     0x00000350     /* RW-4R */
#define NV_PRISCV_RISCV_TRACEPC_LO                                                                     0x00000354     /* RW-4R */
#define NV_PRISCV_RISCV_PRIV_ERR_STAT                                                                  0x00000360     /* RWI4R */
#define NV_PRISCV_RISCV_PRIV_ERR_INFO                                                                  0x00000364     /* R-I4R */
#define NV_PRISCV_RISCV_PRIV_ERR_ADDR                                                                  0x00000368     /* R-I4R */
#define NV_PRISCV_RISCV_HUB_ERR_STAT                                                                   0x0000036c     /* RWI4R */

#endif // __tu102_dev_riscv_pri_h__
