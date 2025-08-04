/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb20b_dev_gsp_h__
#define __gb20b_dev_gsp_h__

#define NV_PGSP                               0x113fff:0x110000 /* RW--D */
#define NV_PGSP_MAILBOX(i)                                             (0x00110804+(i)*4) /* RW-4A */
#define NV_PGSP_MAILBOX__SIZE_1                                                         4 /*       */
#define NV_PGSP_MAILBOX_DATA                                                         31:0 /* RWIVF */
#define NV_PGSP_MAILBOX_DATA_INIT                                              0x00000000 /* RWI-V */

#define NV_PGSP_QUEUE_HEAD(i)                                          (0x00110c00+(i)*8) /* RW-4A */
#define NV_PGSP_QUEUE_HEAD__SIZE_1                                                      8 /*       */
#define NV_PGSP_QUEUE_TAIL(i)                                          (0x00110c04+(i)*8) /* RW-4A */
#define NV_PGSP_QUEUE_TAIL__SIZE_1                                                      8 /*       */

#endif // __gb20b_dev_gsp_h__
