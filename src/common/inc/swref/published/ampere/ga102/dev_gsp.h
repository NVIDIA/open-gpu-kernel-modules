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
#ifndef __ga102_dev_gsp_h__
#define __ga102_dev_gsp_h__

#define NV_PGSP                                0x113fff:0x110000 /* RW--D */
#define NV_PGSP_FALCON_MAILBOX0                                                                          0x110040       /* RW-4R */
#define NV_PGSP_FALCON_MAILBOX0_DATA                                                                     31:0           /* RWIVF */
#define NV_PGSP_FALCON_MAILBOX1                                                                          0x110044       /* RW-4R */
#define NV_PGSP_FALCON_MAILBOX1_DATA                                                                     31:0           /* RWIVF */
#define NV_PGSP_FALCON_ENGINE                                                                            0x1103c0       /* RW-4R */
#define NV_PGSP_FALCON_ENGINE_RESET                                                                      0:0            /* RWIVF */
#define NV_PGSP_FALCON_ENGINE_RESET_TRUE                                                                 0x00000001     /* RW--V */
#define NV_PGSP_FALCON_ENGINE_RESET_FALSE                                                                0x00000000     /* RWI-V */
#define NV_PGSP_MAILBOX(i)                                                                               (0x110804+(i)*4) /* RW-4A */
#define NV_PGSP_MAILBOX__SIZE_1                                                                          4              /*       */
#define NV_PGSP_MAILBOX_DATA                                                                             31:0           /* RWIVF */
#define NV_PGSP_QUEUE_HEAD(i)                                                                            (0x110c00+(i)*8) /* RW-4A */
#define NV_PGSP_QUEUE_HEAD__SIZE_1                                                                       8              /*       */
#define NV_PGSP_QUEUE_HEAD_ADDRESS                                                                       31:0           /* RWIVF */

#endif // __ga102_dev_gsp_h__
