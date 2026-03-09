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

#ifndef __gb100_dev_esched_pbdma_h__
#define __gb100_dev_esched_pbdma_h__
#define NV_PBDMA       0x000007ff:0x00000000 /* RW--D */
#define NV_PBDMA_GP_BASE                                     0x090 /* RW-4R */
#define NV_PBDMA_GP_BASE_RSVD                                  2:0 /* RWXUF */
#define NV_PBDMA_GP_BASE_RSVD_ZERO                      0x00000000 /* RW--V */
#define NV_PBDMA_GP_BASE_HI                                  0x094 /* RW-4R */
#define NV_PBDMA_GP_BASE_HI_OFFSET                            24:0 /* RWXUF */
#define NV_PBDMA_GP_BASE_HI_OFFSET_ZERO                 0x00000000 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE                            0x0b4 /* RW-4R */
#define NV_PBDMA_MISC_FETCH_STATE_GP_INFO_LIMIT2                      28:24 /* RWXUF */
#define NV_PBDMA_MISC_FETCH_STATE_GP_INFO_LIMIT2_ZERO            0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE                    0x05C /* RW-4R */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_VALUE               24:8 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_VALUE_ZERO    0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD               31:25 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD_ZERO     0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD2                7:0 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD2_ZERO    0x00000000 /* RW--V */
#endif // __gb100_dev_esched_pbdma_h__
