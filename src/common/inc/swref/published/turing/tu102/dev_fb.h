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

#ifndef __tu102_dev_fb_h__
#define __tu102_dev_fb_h__

#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO                               0x00100A18 /* R--4R */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL                                 0:0 /* R-IVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL_FALSE                           0x0 /* R-I-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL_TRUE                            0x1 /* R---V */
#define NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_REPLAYABLE                                64 /* R---V */
#define NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_NON_REPLAYABLE                        132 /* R---V */

#define NV_PFB_PRI_MMU_WPR2_ADDR_LO                                            0x001FA824 /* RW-4R */
#define NV_PFB_PRI_MMU_WPR2_ADDR_LO_VAL                                              31:4 /* RWEVF */
#define NV_PFB_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT                                  0x0000000c /*       */
#define NV_PFB_PRI_MMU_WPR2_ADDR_HI                                            0x001FA828 /* RW-4R */
#define NV_PFB_PRI_MMU_WPR2_ADDR_HI_VAL                                              31:4 /* RWEVF */
#define NV_PFB_PRI_MMU_WPR2_ADDR_HI_ALIGNMENT                                  0x0000000c /*       */

#endif // __tu102_dev_fb_h__
