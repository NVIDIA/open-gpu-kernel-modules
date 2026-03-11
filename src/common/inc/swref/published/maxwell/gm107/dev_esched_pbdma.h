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

#ifndef __gm107_dev_pbdma_zero__
#define __gm107_dev_pbdma_zero__
#define NV_PBDMA                                                         0x0001ffff:0x00000000 /* RW--D */
#define NV_PBDMA_GP_BASE_RSVD                                                              2:0 /* RW-UF */
#define NV_PBDMA_GP_BASE_RSVD_ZERO                                                  0x00000000 /* RW--V */
#define NV_PBDMA_GP_BASE_HI(i)                                           (0x0000004C+(i)*8192) /* RW-4A */
#define NV_PBDMA_GP_BASE_HI__SIZE_1                                                          2 /*       */
#define NV_PBDMA_GP_BASE_HI_OFFSET                                                         7:0 /* RW-UF */
#define NV_PBDMA_GP_BASE_HI_OFFSET_ZERO                                             0x00000000 /* RW--V */
#define NV_PBDMA_GP_BASE_HI_LIMIT2                                                       20:16 /* RW-UF */
#define NV_PBDMA_GP_BASE_HI_LIMIT2_ZERO                                             0x00000000 /* RW--V */
#endif // __gm107_dev_pbdma_zero__
