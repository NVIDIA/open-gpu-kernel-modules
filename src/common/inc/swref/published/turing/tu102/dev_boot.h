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

#ifndef __tu102_dev_boot_h__
#define __tu102_dev_boot_h__
#define NV_PMC_INTR(i)                           (0x00000100+(i)*4) /* RW-4A */
#define NV_PMC_INTR_EN(i)                      (0x00000140+(i)*4) /* R--4A */
#define NV_PMC_INTR_EN__SIZE_1    2 /*       */
#define NV_PMC_INTR_EN_DEVICE(i)                          (i):(i) /*       */
#define NV_PMC_INTR_EN_DEVICE__SIZE_1                          32 /*       */
#define NV_PMC_INTR_EN_DEVICE_DISABLED                 0x00000000 /*       */
#define NV_PMC_INTR_EN_DEVICE_ENABLED                  0x00000001 /*       */
#define NV_PMC_INTR_EN_VALUE                                 31:0 /* R-IVF */
#define NV_PMC_INTR_EN_VALUE_INIT                      0x00000000 /* R-I-V */
#define NV_PMC_INTR_EN_SET(i)                  (0x00000160+(i)*4) /* -W-4A */
#define NV_PMC_INTR_EN_SET__SIZE_1 2 /*       */
#define NV_PMC_INTR_EN_SET_DEVICE(i)                      (i):(i) /*       */
#define NV_PMC_INTR_EN_SET_DEVICE__SIZE_1                      32 /*       */
#define NV_PMC_INTR_EN_SET_DEVICE_SET                  0x00000001 /*       */
#define NV_PMC_INTR_EN_SET_VALUE                             31:0 /* -W-VF */
#define NV_PMC_INTR_EN_CLEAR(i)                (0x00000180+(i)*4) /* -W-4A */
#define NV_PMC_INTR_EN_CLEAR__SIZE_1 2 /*       */
#define NV_PMC_INTR_EN_CLEAR_DEVICE(i)                    (i):(i) /*       */
#define NV_PMC_INTR_EN_CLEAR_DEVICE__SIZE_1                    32 /*       */
#define NV_PMC_INTR_EN_CLEAR_DEVICE_SET                0x00000001 /*       */
#define NV_PMC_INTR_EN_CLEAR_VALUE                           31:0 /* -W-VF */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_DEVICE(i)                             (i):(i) /*       */
#define NV_PMC_ENABLE_DEVICE__SIZE_1                             32 /*       */
#define NV_PMC_ENABLE_DEVICE_DISABLE                     0x00000000 /*       */
#define NV_PMC_ENABLE_DEVICE_ENABLE                      0x00000001 /*       */
#define NV_PMC_ENABLE_NVDEC                                   15:15 /*       */
#define NV_PMC_ENABLE_NVDEC_DISABLED                     0x00000000 /*       */
#define NV_PMC_ENABLE_NVDEC_ENABLED                      0x00000001 /*       */
#endif // __tu102_dev_boot_h__
