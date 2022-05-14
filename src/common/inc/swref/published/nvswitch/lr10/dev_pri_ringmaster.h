/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __lr10_dev_pri_ringmain_h__
#define __lr10_dev_pri_ringmain_h__
/* This file is autogenerated.  Do not edit */
#define NV_PPRIV_MASTER_CG1                                                                              0x001200a8     /* RW-4R */
#define NV_PPRIV_MASTER_CG1_SLCG                                                                         0:0            /* RWBVF */
#define NV_PPRIV_MASTER_CG1_SLCG__PROD                                                                   0x00000000     /* RW--V */
#define NV_PPRIV_MASTER_CG1_SLCG_ENABLED                                                                 0x00000000     /* RW--V */
#define NV_PPRIV_MASTER_CG1_SLCG_DISABLED                                                                0x00000001     /* RWB-V */

#define NV_PPRIV_MASTER_RING_COMMAND                                                                     0x0012004c     /* RW-4R */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD                                                                 5:0            /* RWBVF */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_NO_CMD                                                          0x00000000     /* RWB-V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_START_RING                                                      0x00000001     /* RW--T */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ACK_INTERRUPT                                                   0x00000002     /* RW--T */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS                                              0x00000003     /* RW--T */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_AND_START_RING                                        0x00000004     /* RW--T */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_NO_TAG_ENUMERATE_AND_START_RING                                 0x00000005     /* RW--T */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP                                       9:6            /* RWBVF */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_ALL                                   0x00000000     /* RWB-V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_GPC                                   0x00000001     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_FBP                                   0x00000002     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_SYS                                   0x00000003     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_ROP_L2                                0x00000004     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_NO_TAG_ALL                            0x00000008     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_NO_TAG_GPC                            0x00000009     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_NO_TAG_FBP                            0x0000000a     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_NO_TAG_SYS                            0x0000000b     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_NO_TAG_ROP_L2                         0x0000000c     /* RW--V */
#define NV_PPRIV_MASTER_RING_COMMAND_DATA                                                                0x00120048     /* RW-4R */
#define NV_PPRIV_MASTER_RING_COMMAND_DATA_START_RING_SEED                                                7:0            /* RWBVF */
#define NV_PPRIV_MASTER_RING_COMMAND_DATA_START_RING_SEED_INIT                                           0x00000053     /* RWB-V */

#define NV_PPRIV_MASTER_RING_START_RESULTS                                                               0x00120050     /* R--4R */
#define NV_PPRIV_MASTER_RING_START_RESULTS_CONNECTIVITY                                                  0:0            /* R-BVF */
#define NV_PPRIV_MASTER_RING_START_RESULTS_CONNECTIVITY_PASS                                             0x00000001     /* R---V */
#define NV_PPRIV_MASTER_RING_START_RESULTS_CONNECTIVITY_FAIL                                             0x00000000     /* R-B-V */

#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0                                                           0x00120058     /* R--4R */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GBL_WRITE_ERROR_FBP                                       31:16          /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GBL_WRITE_ERROR_FBP_V                                     0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GBL_WRITE_ERROR_SYS                                       8:8            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GBL_WRITE_ERROR_SYS_V                                     0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_RING_START_CONN_FAULT                                     0:0            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_RING_START_CONN_FAULT_V                                   0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_DISCONNECT_FAULT                                          1:1            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_DISCONNECT_FAULT_V                                        0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_OVERFLOW_FAULT                                            2:2            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_OVERFLOW_FAULT_V                                          0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_RING_ENUMERATION_FAULT                                    3:3            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_RING_ENUMERATION_FAULT_V                                  0x00000000     /* R-B-V */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GPC_RS_MAP_CONFIG_FAULT                                   4:4            /* R-BVF */
#define NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0_GPC_RS_MAP_CONFIG_FAULT_V                                 0x00000000     /* R-B-V */
#endif // __lr10_dev_pri_ringmain_h__
