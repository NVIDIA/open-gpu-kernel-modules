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

#ifndef __ga100_dev_ram_h__
#define __ga100_dev_ram_h__
#define NV_RAMIN_ALLOC_SIZE                                    4096 /*       */
#define NV_RAMRL_ENTRY_CHAN_USERD_PTR_LO         (31+0*32):(8+0*32) /* RWXUF */
#define NV_RAMRL_ENTRY_CHAN_USERD_PTR_HI_HW       (7+1*32):(0+1*32) /* RWXUF */
#define NV_RAMRL_ENTRY_BASE_SHIFT                                12 /*       */
#define NV_RAMUSERD_PUT                        (16*32+31):(16*32+0) /* RW-UF */
#define NV_RAMUSERD_GET                        (17*32+31):(17*32+0) /* RW-UF */
#define NV_RAMUSERD_REF                        (18*32+31):(18*32+0) /* RW-UF */
#define NV_RAMUSERD_PUT_HI                     (19*32+31):(19*32+0) /* RW-UF */
#define NV_RAMUSERD_TOP_LEVEL_GET              (22*32+31):(22*32+0) /* RW-UF */
#define NV_RAMUSERD_TOP_LEVEL_GET_HI           (23*32+31):(23*32+0) /* RW-UF */
#define NV_RAMUSERD_GET_HI                     (24*32+31):(24*32+0) /* RW-UF */
#define NV_RAMUSERD_GP_GET                     (34*32+31):(34*32+0) /* RW-UF */
#define NV_RAMUSERD_GP_PUT                     (35*32+31):(35*32+0) /* RW-UF */
#endif // __ga100_dev_ram_h__
