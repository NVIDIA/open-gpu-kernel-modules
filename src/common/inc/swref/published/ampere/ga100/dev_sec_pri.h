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

#ifndef __ga100_dev_sec_pri_h__
#define __ga100_dev_sec_pri_h__

#define NV_PSEC                                0x843fff:0x840000 /* RW--D */
#define NV_PSEC_FALCON_ENGINE                                                                            0x008403c0     /* RW-4R */
#define NV_PSEC_FALCON_ENGINE_RESET                                                                      0:0            /* RWIVF */
#define NV_PSEC_FALCON_ENGINE_RESET_TRUE                                                                 0x00000001     /* RW--V */
#define NV_PSEC_FALCON_ENGINE_RESET_FALSE                                                                0x00000000     /* RWI-V */

#define NV_PSEC_MAILBOX(i)                                                                               (0x00840804+(i)*4) /* RW-4A */
#define NV_PSEC_MAILBOX__SIZE_1                                                                          4              /*       */
#define NV_PSEC_MAILBOX_DATA                                                                             31:0           /* RWIVF */
#define NV_PSEC_MAILBOX_DATA_INIT                                                                        0x00000000     /* RWI-V */

#endif // __ga100_dev_sec_pri_h__
