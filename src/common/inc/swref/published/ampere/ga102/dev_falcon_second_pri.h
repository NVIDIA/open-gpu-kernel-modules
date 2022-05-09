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
#ifndef __ga102_dev_falcon_second_pri_h__
#define __ga102_dev_falcon_second_pri_h__

#define NV_FALCON2_GSP_BASE 0x00111000
#define NV_FALCON2_NVDEC0_BASE 0x00849c00
#define NV_FALCON2_SEC_BASE 0x00841000
#define NV_PFALCON2_FALCON_MOD_SEL                                                                       0x00000180     /* RWI4R */
#define NV_PFALCON2_FALCON_MOD_SEL_ALGO                                                                  7:0            /* RWIVF */
#define NV_PFALCON2_FALCON_MOD_SEL_ALGO_RSA3K                                                            0x00000001     /* RW--V */
#define NV_PFALCON2_FALCON_BROM_CURR_UCODE_ID                                                            0x00000198     /* RWI4R */
#define NV_PFALCON2_FALCON_BROM_CURR_UCODE_ID_VAL                                                        7:0            /* RWIVF */
#define NV_PFALCON2_FALCON_BROM_ENGIDMASK                                                                0x0000019c     /* RWI4R */
#define NV_PFALCON2_FALCON_BROM_PARAADDR(i)                                                              (0x00000210+(i)*4) /* RWI4A */

#endif // __ga102_dev_falcon_second_pri_h__
