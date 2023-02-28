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
#ifndef __gh100_dev_gsp_h__
#define __gh100_dev_gsp_h__
#define NV_PGSP                                                                                          0x113fff:0x110000 /* RW--D */
#define NV_PGSP_FALCON_ENGINE                                                                            0x1103c0     /* RW-4R */
#define NV_PGSP_FALCON_ENGINE_RESET                                                                      0:0            /* RWEVF */
#define NV_PGSP_FALCON_ENGINE_RESET_DEASSERT                                                             0              /*       */
#define NV_PGSP_FALCON_ENGINE_RESET_ASSERT                                                               1              /*       */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS                                                               10:8           /* R-EVF */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS_ASSERTED                                                      0x00000000     /* R-E-V */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS_DEASSERTED                                                    0x00000002     /* R---V */
#define NV_PGSP_MAILBOX(i)                                                                               (0x110804+(i)*4) /* RW-4A */
#endif // __gh100_dev_gsp_h__
