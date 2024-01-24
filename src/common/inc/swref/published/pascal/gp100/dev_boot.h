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

#ifndef __gp100_dev_boot_h__
#define __gp100_dev_boot_h__
#define NV_PMC_INTR(i)                           (0x00000100+(i)*4) /* R--4A */
#define NV_PMC_INTR_EN(i)                      (0x00000140+(i)*4) /* R--4A */
#define NV_PMC_INTR_EN__SIZE_1    2 /*       */
#define NV_PMC_INTR_EN_DEVICE(i)                          (i):(i) /*       */
#define NV_PMC_INTR_EN_DEVICE__SIZE_1                          32 /*       */
#define NV_PMC_INTR_EN_DEVICE_DISABLED                 0x00000000 /*       */
#define NV_PMC_INTR_EN_DEVICE_ENABLED                  0x00000001 /*       */
#define NV_PMC_INTR_EN_VALUE                                 31:0 /* R-IVF */
#define NV_PMC_INTR_EN_VALUE_INIT                      0x00000000 /* R-I-V */
#define NV_PMC_INTR_EN_SET(i)                  (0x00000160+(i)*4) /* -W-4A */
#define NV_PMC_INTR_EN_SET__SIZE_1 2 /*      */
#define NV_PMC_INTR_EN_SET_DEVICE(i)                      (i):(i) /*       */
#define NV_PMC_INTR_EN_SET_DEVICE__SIZE_1                      32 /*       */
#define NV_PMC_INTR_EN_SET_DEVICE_SET                  0x00000001 /*       */
#define NV_PMC_INTR_EN_SET_VALUE                             31:0 /* -W-VF */
#define NV_PMC_INTR_EN_CLEAR(i)                (0x00000180+(i)*4) /* -W-4A */
#define NV_PMC_INTR_EN_CLEAR__SIZE_1 2 /*    */
#define NV_PMC_INTR_EN_CLEAR_DEVICE(i)                    (i):(i) /*       */
#define NV_PMC_INTR_EN_CLEAR_DEVICE__SIZE_1                    32 /*       */
#define NV_PMC_INTR_EN_CLEAR_DEVICE_SET                0x00000001 /*       */
#define NV_PMC_INTR_EN_CLEAR_VALUE                           31:0 /* -W-VF */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_PMEDIA                                    4:4 /*       */
#define NV_PMC_ENABLE_PMEDIA_DISABLED                    0x00000000 /*       */
#define NV_PMC_ENABLE_PMEDIA_ENABLED                     0x00000001 /*       */
#define NV_PMC_ENABLE_PFIFO                                     8:8 /* RWIVF */
#define NV_PMC_ENABLE_PFIFO_DISABLED                     0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PFIFO_ENABLED                      0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PWR                                     13:13 /*       */
#define NV_PMC_ENABLE_PWR_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_PWR_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_PGRAPH                                  12:12 /*       */
#define NV_PMC_ENABLE_PGRAPH_DISABLED                    0x00000000 /*       */
#define NV_PMC_ENABLE_PGRAPH_ENABLED                     0x00000001 /*       */
#define NV_PMC_ENABLE_SEC                                     14:14 /*       */
#define NV_PMC_ENABLE_SEC_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_SEC_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE0                                       6:6 /*       */
#define NV_PMC_ENABLE_CE0_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE0_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE1                                       7:7 /*       */
#define NV_PMC_ENABLE_CE1_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE1_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE2                                     21:21 /*       */
#define NV_PMC_ENABLE_CE2_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE2_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE3                                     22:22 /*       */
#define NV_PMC_ENABLE_CE3_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE3_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE4                                     23:23 /*       */
#define NV_PMC_ENABLE_CE4_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE4_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_CE5                                     24:24 /*       */
#define NV_PMC_ENABLE_CE5_DISABLED                       0x00000000 /*       */
#define NV_PMC_ENABLE_CE5_ENABLED                        0x00000001 /*       */
#define NV_PMC_ENABLE_NVDEC                                   15:15 /*       */
#define NV_PMC_ENABLE_NVDEC_DISABLED                     0x00000000 /*       */
#define NV_PMC_ENABLE_NVDEC_ENABLED                      0x00000001 /*       */
#define NV_PMC_ENABLE_PDISP                                   30:30 /* RWIVF */
#define NV_PMC_ENABLE_PDISP_DISABLED                     0x00000000 /* RW--V */
#define NV_PMC_ENABLE_PDISP_ENABLED                      0x00000001 /* RWI-V */
#define NV_PMC_ENABLE_NVENC0                                  18:18 /*       */
#define NV_PMC_ENABLE_NVENC0_DISABLED                    0x00000000 /*       */
#define NV_PMC_ENABLE_NVENC0_ENABLED                     0x00000001 /*       */
#define NV_PMC_ENABLE_NVENC1                                  19:19 /*       */
#define NV_PMC_ENABLE_NVENC1_DISABLED                    0x00000000 /*       */
#define NV_PMC_ENABLE_NVENC1_ENABLED                     0x00000001 /*       */
#define NV_PMC_ENABLE_NVENC2                                    4:4 /*       */
#define NV_PMC_ENABLE_NVENC2_DISABLED                    0x00000000 /*       */
#define NV_PMC_ENABLE_NVENC2_ENABLED                     0x00000001 /*       */
#endif // __gp100_dev_boot_h__
