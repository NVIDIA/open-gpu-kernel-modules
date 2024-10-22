/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_riscv_pri_h__
#define __gb100_dev_riscv_pri_h__

#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM                               2:2            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM_FAULTED                       0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM_NO_FAULT                      0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM                               3:3            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM_FAULTED                       0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM_NO_FAULT                      0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE                             4:4            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE_FAULTED                     0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE_NO_FAULT                    0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE                             5:5            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE_FAULTED                     0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE_NO_FAULT                    0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE                             6:6            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE_FAULTED                     0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE_NO_FAULT                    0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG                                7:7            /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG_FAULTED                        0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG_NO_FAULT                       0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM                             12:12          /* R-IVF */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM_FAULTED                     0x00000001     /* R---V */
#define NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM_NO_FAULT                    0x00000000     /* R-I-V */

#endif // __gb100_dev_riscv_pri_h__
