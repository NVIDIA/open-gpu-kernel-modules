/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_boot_h__
#define __ga100_dev_boot_h__
#define NV_PMC                                0x00000fff:0x00000000 /* RW--D */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_DEVICE(i)                             (i):(i) /*       */
#define NV_PMC_ENABLE_DEVICE__SIZE_1                             32 /*       */
#define NV_PMC_ENABLE_DEVICE_DISABLE                     0x00000000 /*       */
#define NV_PMC_ENABLE_DEVICE_ENABLE                      0x00000001 /*       */
#define NV_PMC_ENABLE_NVDEC                                   15:15 /*       */
#define NV_PMC_ENABLE_NVDEC_DISABLED                     0x00000000 /*       */
#define NV_PMC_ENABLE_NVDEC_ENABLED                      0x00000001 /*       */
#define NV_PMC_ENABLE_PERFMON                                 28:28 /* RWIVF */
#define NV_PMC_ENABLE_PERFMON_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PERFMON_ENABLED                    0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PDISP                                   30:30 /* RWIVF */
#define NV_PMC_ENABLE_PDISP_DISABLED                     0x00000000 /* RW--V */
#define NV_PMC_ENABLE_PDISP_ENABLED                      0x00000001 /* RWI-V */
#define NV_PMC_DEVICE_ENABLE(i)                                 (0x000000600+(i)*4) /* RW-4A */
#define NV_PMC_DEVICE_ENABLE__SIZE_1              1 /*       */
#define NV_PMC_DEVICE_ENABLE__PRIV_LEVEL_MASK  0x00000084 /*       */
#define NV_PMC_DEVICE_ENABLE_STATUS                                            31:0 /* RWIVF */
#define NV_PMC_DEVICE_ENABLE_STATUS_DISABLE_ALL                          0x00000000 /* RWI-V */
#define NV_PMC_DEVICE_ENABLE_STATUS_BIT(i)                                  (i):(i) /*       */
#define NV_PMC_DEVICE_ENABLE_STATUS_BIT__SIZE_1   32 /*       */
#define NV_PMC_DEVICE_ENABLE_STATUS_BIT_DISABLE                          0x00000000 /*       */
#define NV_PMC_DEVICE_ENABLE_STATUS_BIT_ENABLE                           0x00000001 /*       */
#endif // __ga100_dev_boot_h__
