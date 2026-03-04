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

#ifndef __gh100_dev_fuse_h__
#define __gh100_dev_fuse_h__

#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS                                           0x0082074C /* RW-4R */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA                                             0:0 /* RWIVF */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA_NO                                   0x00000000 /* RW--V */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA_YES                                  0x00000001 /* RW--V */
#define NV_FUSE_SPARE_BIT_0                                                        0x00824E04 /* RW-4R */
#define NV_FUSE_SPARE_BIT_0_DATA                                                          0:0 /* RWIVF */
#define NV_FUSE_SPARE_BIT_0_DATA_INIT                                              0x00000000 /* RWI-V */
#define NV_FUSE_SPARE_BIT_0_DATA_DISABLE                                           0x00000000 /* RW--V */
#define NV_FUSE_SPARE_BIT_0_DATA_ENABLE                                            0x00000001 /* RW--V */
#define NV_FUSE_SPARE_BIT_1                                                        0x00824E08 /* RW-4R */
#define NV_FUSE_SPARE_BIT_1_DATA                                                          0:0 /* RWIVF */
#define NV_FUSE_SPARE_BIT_1_DATA_INIT                                              0x00000000 /* RWI-V */
#define NV_FUSE_SPARE_BIT_1_DATA_DISABLE                                           0x00000000 /* RW--V */
#define NV_FUSE_SPARE_BIT_1_DATA_ENABLE                                            0x00000001 /* RW--V */
#define NV_FUSE_SPARE_BIT_2                                                        0x00824E0C /* RW-4R */
#define NV_FUSE_SPARE_BIT_2_DATA                                                          0:0 /* RWIVF */
#define NV_FUSE_SPARE_BIT_2_DATA_INIT                                              0x00000000 /* RWI-V */
#define NV_FUSE_SPARE_BIT_2_DATA_DISABLE                                           0x00000000 /* RW--V */
#define NV_FUSE_SPARE_BIT_2_DATA_ENABLE                                            0x00000001 /* RW--V */
#define NV_FUSE_SPARE_BIT_3                                                        0x00824E10 /* RW-4R */
#define NV_FUSE_SPARE_BIT_3_DATA                                                          0:0 /* RWIVF */
#define NV_FUSE_SPARE_BIT_3_DATA_INIT                                              0x00000000 /* RWI-V */
#define NV_FUSE_SPARE_BIT_3_DATA_DISABLE                                           0x00000000 /* RW--V */
#define NV_FUSE_SPARE_BIT_3_DATA_ENABLE                                            0x00000001 /* RW--V */

#endif // __gh100_dev_fuse_h__
