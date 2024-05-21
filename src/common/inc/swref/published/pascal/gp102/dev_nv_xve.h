/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gp102_dev_nv_xve_h__
#define __gp102_dev_nv_xve_h__
#define NV_XVE_SW_RESET                                          0x00000718 /* RW-4R */
#define NV_XVE_SW_RESET_RESET                                           0:0 /* RWCVF */
#define NV_XVE_SW_RESET_GPU_ON_SW_RESET_INIT                     0x00000001 /* RWC-V */
#define NV_XVE_SW_RESET_GPU_ON_SW_RESET_ENABLE                   0x00000001 /* RW--V */
#define NV_XVE_SW_RESET_GPU_ON_SW_RESET_DISABLE                  0x00000000 /* RW--V */
#define NV_XVE_SW_RESET_GPU_ON_SW_RESET                                 1:1 /* RWCVF */
#define NV_XVE_SW_RESET_COUNTER_EN                                      2:2 /* RWCVF */
#define NV_XVE_SW_RESET_COUNTER_VAL                                    14:4 /* RWCVF */
#define NV_XVE_SW_RESET_CLOCK_ON_SW_RESET                             15:15 /* RWCVF */
#define NV_XVE_SW_RESET_CLOCK_COUNTER_EN                              16:16 /* RWCVF */
#define NV_XVE_SW_RESET_CLOCK_COUNTER_VAL                             27:17 /* RWCVF */
#endif // __gp102_dev_nv_xve_h__
