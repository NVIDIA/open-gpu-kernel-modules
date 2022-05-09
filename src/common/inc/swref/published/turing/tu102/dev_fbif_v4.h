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

#ifndef __tu102_dev_fbif_v4_h__
#define __tu102_dev_fbif_v4_h__

#define NV_PFALCON_FBIF_TRANSCFG(i)                                                                    (0x00000000+(i)*4) /* RW-4A */
#define NV_PFALCON_FBIF_TRANSCFG__SIZE_1                                                               8              /*       */
#define NV_PFALCON_FBIF_TRANSCFG_TARGET                                                                1:0            /* RWIVF */
#define NV_PFALCON_FBIF_TRANSCFG_TARGET_COHERENT_SYSMEM                                                0x00000001     /* R---V */
#define NV_PFALCON_FBIF_TRANSCFG_MEM_TYPE                                                              2:2            /* RWIVF */
#define NV_PFALCON_FBIF_TRANSCFG_MEM_TYPE_PHYSICAL                                                     0x00000001     /* R---V */
#define NV_PFALCON_FBIF_CTL                                                                            0x00000024     /* RW-4R */
#define NV_PFALCON_FBIF_CTL_ALLOW_PHYS_NO_CTX                                                          7:7            /* RWIVF */
#define NV_PFALCON_FBIF_CTL_ALLOW_PHYS_NO_CTX_ALLOW                                                    0x00000001     /* RW--V */

#endif // __tu102_dev_fbif_v4_h__
