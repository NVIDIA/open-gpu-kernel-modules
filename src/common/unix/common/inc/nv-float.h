/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(NV_FLOAT_H)
#define NV_FLOAT_H

/* Floating-point constants, expressed as integer constants */

#define NV_FLOAT_NEG_ONE      0xBF800000 /*    -1.00f */
#define NV_FLOAT_NEG_QUARTER  0xBE800000 /*    -0.25f */
#define NV_FLOAT_ZERO         0x00000000 /*     0.00f */
#define NV_FLOAT_QUARTER      0x3E800000 /*     0.25f */
#define NV_FLOAT_HALF         0x3F000000 /*     0.50f */
#define NV_FLOAT_ONE          0x3F800000 /*     1.00f */
#define NV_FLOAT_TWO          0x40000000 /*     2.00f */
#define NV_FLOAT_255          0x437F0000 /*   255.00f */
#define NV_FLOAT_1024         0x44800000 /*  1024.00f */
#define NV_FLOAT_65536        0x47800000 /* 65536.00f */

#endif
