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

#ifndef __gp100_dev_ctxsw_prog_h__
#define __gp100_dev_ctxsw_prog_h__
#define NV_CTXSW_TIMESTAMP_BUFFER_RD_WR_POINTER                            30:0  /*     */
#define NV_CTXSW_TIMESTAMP_BUFFER_MAILBOX1_TRACE_FEATURE                  31:31  /*     */
#define NV_CTXSW_TIMESTAMP_BUFFER_MAILBOX1_TRACE_FEATURE_ENABLED            0x1  /*     */
#define NV_CTXSW_TIMESTAMP_BUFFER_MAILBOX1_TRACE_FEATURE_DISABLED           0x0  /*     */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_PREEMPT_OFFSET                   0x00000000 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SPILL_OFFSET                     0x00000004 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_CB_OFFSET                        0x00000008 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_PAGEPOOL_OFFSET                  0x0000000c /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SLICE_STRIDE                     0x00000010 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GLOBAL_BETA_SIZE                 0x00000014 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GLOBAL_ALPHA_SIZE                0x00000018 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GLOBAL_PAGEPOOL_SIZE             0x0000001c /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GFXP_BETA_SIZE                   0x00000020 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GFXP_ALPHA_SIZE                  0x00000024 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GFXP_PAGEPOOL_SIZE               0x00000028 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GFXP_SPILL_SIZE                  0x0000002c /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_NUM_SLICES                       0x00000030 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SLICE_ARRAY1                     0x00000034 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SLICE_ARRAY2                     0x00000038 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SLICE_ARRAY3                     0x0000003c /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_SLICE_ARRAY4                     0x00000040 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_RM_SLICE_ARRAY1                  0x00000044 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_RM_SLICE_ARRAY2                  0x00000048 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_RM_SLICE_ARRAY3                  0x0000004c /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_RM_SLICE_ARRAY4                  0x00000050 /* RW-4R */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_MAX_SLICES                       0x00000054 /* RW-4R */
#endif // __gp100_dev_ctxsw_prog_h__
