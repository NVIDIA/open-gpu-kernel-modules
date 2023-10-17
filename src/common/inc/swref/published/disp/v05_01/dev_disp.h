/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __v05_01_dev_disp_h__
#define __v05_01_dev_disp_h__
#define NV_PDISP_POSTCOMP_LOADV_COUNTER(i)                    (0x0061A11C+(i)*1024) /* RW-4A */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER__SIZE_1                                   8 /*       */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER_VALUE                                  31:0 /* RWIUF */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER_VALUE_INIT                       0x00000000 /* RWI-V */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER_VALUE_ZERO                       0x00000000 /* RW--V */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER_VALUE_HW                         0x00000000 /* R---V */
#define NV_PDISP_POSTCOMP_LOADV_COUNTER_VALUE_SW                         0x00000000 /* -W--V */
#endif // __v05_01_dev_disp_h__
