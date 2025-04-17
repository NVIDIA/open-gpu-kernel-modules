/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES
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
#ifndef __gb20b_dev_boot_h__
#define __gb20b_dev_boot_h__

#define NV_PMC_SCRATCH_RESET_PLUS_2                      0x000005e0 /* RW-4R */
#define NV_PMC_SCRATCH_RESET_PLUS_2__SAFETY               "parity"  /*       */
#define NV_PMC_SCRATCH_RESET_PLUS_2__PRIV_LEVEL_MASK     0x000005e4 /*       */
#define NV_PMC_SCRATCH_RESET_PLUS_2_VALUE                            31:0 /* RWBVF */
#define NV_PMC_SCRATCH_RESET_PLUS_2_VALUE_INIT                          0 /* RWB-V */
#define NV_SYSCTRL_SEC_FAULT                                                                                                                                                  0x3:0x0 /* R---M */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SEC2_DCLS                                                                                                                               2:2 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SEC2_L5_WDT                                                                                                                             3:3 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GSP_DCLS                                                                                                                                4:4 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GSP_L5_WDT                                                                                                                              5:5 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_PMU_DCLS                                                                                                                                6:6 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_PMU_L5_WDT                                                                                                                              7:7 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GPMVDD_VMON                                                                                                                             8:8 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GPCVDD_VMON                                                                                                                             9:9 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SOC2GPU_SEC_FAULT_FUNCTION_LOCKDOWN_REQ                                                                                               10:10 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_FUNCTION_LOCKDOWN                                                                                                                     11:11 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_DEVICE_LOCKDOWN                                                                                                                       12:12 /* R-XUF */

#endif // __gb20b_dev_boot_h__
