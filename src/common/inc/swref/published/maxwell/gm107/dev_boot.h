/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gm107_dev_boot_h__
#define __gm107_dev_boot_h__
#define NV_PMC                                0x00000FFF:0x00000000 /* RW--D */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_INTR(i)                           (0x00000100+(i)*4) /* RW-4A */
#define NV_PMC_INTR__SIZE_1             3 /*       */
#define NV_PMC_INTR_DEVICE(i)                               (i):(i) /*       */
#define NV_PMC_INTR_DEVICE__SIZE_1                               31 /*       */
#define NV_PMC_INTR_DEVICE_NOT_PENDING                   0x00000000 /*       */
#define NV_PMC_INTR_DEVICE_PENDING                       0x00000001 /*       */
#define NV_PMC_INTR_EN(i)                        (0x00000140+(i)*4) /* RW-4A */
#define NV_PMC_INTR_EN__SIZE_1          3 /*       */
#define NV_PMC_INTR_0                                    0x00000100 /*       */
#define NV_PMC_INTR_1                                    0x00000104 /*       */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_DEVICE(i)                             (i):(i) /*       */
#define NV_PMC_ENABLE_DEVICE__SIZE_1                             32 /*       */
#define NV_PMC_ENABLE_DEVICE_DISABLE                     0x00000000 /*       */
#define NV_PMC_ENABLE_DEVICE_ENABLE                      0x00000001 /*       */
#endif // __gm107_dev_boot_h__
