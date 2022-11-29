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

#ifndef __ls10_dev_ctrl_ip_addendum_h__
#define __ls10_dev_ctrl_ip_addendum_h__
//
// GIN _LEAF() mapping to IP blocks
// Refer to "Table 74. Laguna interrupt sources" in Laguna Seca IAS
//
#define NV_CTRL_CPU_INTR_NVLW_FATAL_IDX                       0
#define NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX                   1
#define NV_CTRL_CPU_INTR_NVLW_CORRECTABLE_IDX                 2
#define NV_CTRL_CPU_INTR_NPG_FATAL_IDX                        5
#define NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX                    6
#define NV_CTRL_CPU_INTR_NPG_CORRECTABLE_IDX                  7
#define NV_CTRL_CPU_INTR_NXBAR_FATAL_IDX                      8
#define NV_CTRL_CPU_INTR_UNITS_IDX                            9
#define NV_CTRL_CPU_INTR_NVLW_FATAL                           NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_FATAL_IDX)
#define NV_CTRL_CPU_INTR_NVLW_FATAL_MASK                      15:0
#define NV_CTRL_CPU_INTR_NVLW_NON_FATAL                       NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX)
#define NV_CTRL_CPU_INTR_NVLW_NON_FATAL_MASK                  15:0
#define NV_CTRL_CPU_INTR_NVLW_CORRECTABLE                     NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_CORRECTABLE_IDX)
#define NV_CTRL_CPU_INTR_NVLW_CORRECTABLE_MASK                15:0
#define NV_CTRL_CPU_INTR_NPG_FATAL                            NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NPG_FATAL_IDX)
#define NV_CTRL_CPU_INTR_NPG_FATAL_MASK                       15:0
#define NV_CTRL_CPU_INTR_NPG_NON_FATAL                        NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX)
#define NV_CTRL_CPU_INTR_NPG_NON_FATAL_MASK                   15:0
#define NV_CTRL_CPU_INTR_NPG_CORRECTABLE                      NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NPG_CORRECTABLE_IDX)
#define NV_CTRL_CPU_INTR_NPG_CORRECTABLE_MASK                 15:0
#define NV_CTRL_CPU_INTR_NXBAR_FATAL                          NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NXBAR_FATAL_IDX)
#define NV_CTRL_CPU_INTR_NXBAR_FATAL_MASK                     2:0
#define NV_CTRL_CPU_INTR_UNITS                                NV_CTRL_CPU_INTR_LEAF(NV_CTRL_CPU_INTR_UNITS_IDX)
#define NV_CTRL_CPU_INTR_UNITS_PMGR_HOST                      0:0
#define NV_CTRL_CPU_INTR_UNITS_PTIMER                         1:1
#define NV_CTRL_CPU_INTR_UNITS_PTIMER_ALARM                   2:2
#define NV_CTRL_CPU_INTR_UNITS_SEC0_INTR0_0                   7:7
#define NV_CTRL_CPU_INTR_UNITS_SEC0_NOTIFY_0                  8:8
#define NV_CTRL_CPU_INTR_UNITS_SOE_SHIM_ILLEGAL_OP            11:11
#define NV_CTRL_CPU_INTR_UNITS_SOE_SHIM_FLUSH                 12:12
#define NV_CTRL_CPU_INTR_UNITS_XTL_CPU                        13:13
#define NV_CTRL_CPU_INTR_UNITS_XAL_EP                         14:14
#define NV_CTRL_CPU_INTR_UNITS_PRIV_RING                      15:15
#define NV_CTRL_CPU_INTR_UNITS_FSP                            16:16

#endif // __ls10_dev_ctrl_ip_addendum_h__
