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

#ifndef __gb10b_dev_boot_h__
#define __gb10b_dev_boot_h__

#define NV_SYSCTRL_SEC_FAULT                                                                                                                                        0x3:0x0 /* R---M */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_FUSE_POD                                                                                                                      0:0 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SEC2_SCPM                                                                                                                     1:1 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SEC2_DCLS                                                                                                                     2:2 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_SEC2_L5_WDT                                                                                                                   3:3 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GSP_DCLS                                                                                                                      4:4 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_GSP_L5_WDT                                                                                                                    5:5 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_PMU_DCLS                                                                                                                      6:6 /* R-XUF */
#define NV_SYSCTRL_SEC_FAULT_BIT_POSITION_PMU_L5_WDT                                                                                                                    7:7 /* R-XUF */

#endif // __gb10b_dev_boot_h__
