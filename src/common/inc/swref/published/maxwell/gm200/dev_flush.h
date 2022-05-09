/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gm200_dev_flush_h__
#define __gm200_dev_flush_h__
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE                   0x00070004 /* RW-4R */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_PENDING                  0:0 /* RWIUF */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_PENDING_EMPTY     0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_PENDING_BUSY      0x00000001 /* R---V */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_OUTSTANDING              1:1 /* R-IUF */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_OUTSTANDING_FALSE 0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_SYSMEM_INVALIDATE_OUTSTANDING_TRUE  0x00000001 /* R---V */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE                   0x00070008 /* RW-4R */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_PENDING                  0:0 /* RWIUF */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_PENDING_EMPTY     0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_PENDING_BUSY      0x00000001 /* R---V */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_OUTSTANDING              1:1 /* R-IUF */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_OUTSTANDING_FALSE 0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_PEERMEM_INVALIDATE_OUTSTANDING_TRUE  0x00000001 /* R---V */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS                   0x0007000c /* RW-4R */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_PENDING                  0:0 /* RWIUF */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_PENDING_EMPTY     0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_PENDING_BUSY      0x00000001 /* R---V */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_OUTSTANDING              1:1 /* R-IUF */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_OUTSTANDING_FALSE 0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_CLEAN_COMPTAGS_OUTSTANDING_TRUE  0x00000001 /* R---V */
#define NV_UFLUSH_L2_FLUSH_DIRTY                   0x00070010 /* RW-4R */
#define NV_UFLUSH_L2_FLUSH_DIRTY_PENDING                  0:0 /* RWIUF */
#define NV_UFLUSH_L2_FLUSH_DIRTY_PENDING_EMPTY     0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_FLUSH_DIRTY_PENDING_BUSY      0x00000001 /* R---V */
#define NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING              1:1 /* R-IUF */
#define NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING_FALSE 0x00000000 /* R-I-V */
#define NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING_TRUE  0x00000001 /* R---V */
#endif // __gm200_dev_flush_h__
