/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gb100_dev_boot_zb_h__
#define __gb100_dev_boot_zb_h__

#define NV_PMC_ZB                                    0x00000FFF:0x00000000 /* RW--D */
#define NV_PMC_ZB_SCRATCH_RESET_2(i)                 (0x00000580+(i)*4) /* RW-4A */
#define NV_PMC_ZB_SCRATCH_RESET_2__SIZE_1                            16 /*       */
#define NV_PMC_ZB_SCRATCH_RESET_2_VALUE                            31:0 /* RWBVF */
#define NV_PMC_ZB_SCRATCH_RESET_2_VALUE_INIT                          0 /* RWB-V */
#define NV_PMC_ZB_ENABLE_PERFMON    28:28 /* RWBVF */
#define NV_PMC_ZB_ENABLE_PERFMON_DISABLED                  0x00000000 /* RWB-V */
#define NV_PMC_ZB_ENABLE_PERFMON_ENABLED                   0x00000001 /* RW--V */
#define NV_PMC_ZB_ENABLE_PDISP         30:30 /* RWBVF */
#define NV_PMC_ZB_ENABLE_PDISP_DISABLED                     0x00000000 /* RW--V */
#define NV_PMC_ZB_ENABLE_PDISP_ENABLED                      0x00000001 /* RWB-V */
#endif // __gb100_dev_boot_zb_h__
 