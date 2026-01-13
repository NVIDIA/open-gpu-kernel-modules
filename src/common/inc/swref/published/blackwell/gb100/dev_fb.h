/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_fb_h_
#define __gb100_dev_fb_h_

#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE                           0x001FA3E0 /* RW-4R */
#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE__PRIV_LEVEL_MASK          0x001FA7C4 /*       */
#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE_LOWER_SCALE                      3:0 /* RWEVF */
#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE_LOWER_SCALE_INIT          0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE_LOWER_MAG                       27:4 /* RWEVF */
#define NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE_LOWER_MAG_INIT            0x00000000 /* RWE-V */

#endif // __gb100_dev_fb_h_
