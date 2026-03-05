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

#ifndef __gb100_dev_ram_h__
#define __gb100_dev_ram_h__
#define NV_RAMFC                                                    /* ----G */
#define NV_RAMFC_PB_SEGMENT_EXTENDED_BASE        (23*32+31):(23*32+0) /* RW-UF */
#define NV_RAMFC_GP_BASE                         (36*32+31):(36*32+0) /* RW-UF */
#define NV_RAMFC_GP_BASE_HI                      (37*32+31):(37*32+0) /* RW-UF */
#define NV_RAMFC_GP_GET                          (38*32+31):(38*32+0) /* RW-UF */
#define NV_RAMFC_MISC_FETCH_STATE                (45*32+31):(45*32+0) /* RW-UF */
#define NV_RAMFC_DEBUG_STATE(i)        (((i)+48)*32+31):(((i)+48)*32+0) /* RW-UF */
#define NV_RAMFC_DEBUG_STATE__SIZE_1   4 /*       */
#define NV_RAMFC_DEBUG_STATE_RAMFC_INDEX_gp_put                         0x00000000 /*       */
#define NV_RAMFC_DEBUG_STATE_RAMFC_INDEX_gp_peek                        0x00000001 /*       */
#define NV_RAMFC_DEBUG_STATE_RAMFC_INDEX_gp_fetch                       0x00000002 /*       */
#define NV_RAMFC_DEBUG_STATE_RAMFC_INDEX_hdr_shadow                     0x00000003 /*       */
#endif // __gb100_dev_ram_h__
