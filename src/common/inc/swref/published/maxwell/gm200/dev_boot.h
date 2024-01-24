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

#ifndef __gm200_dev_boot_h__
#define __gm200_dev_boot_h__
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
#endif // __gm200_dev_boot_h__
