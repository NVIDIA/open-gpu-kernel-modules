/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gm200_dev_nv_p2p_h__
#define __gm200_dev_nv_p2p_h__
#define NV_P2P                               0x0013AFFF:0x00139000 /* RW--D */
#define NV_P2P_WMBOX_ADDR_ADDR                                   18:1 /* RWIUF */
#define NV_P2P_WREQMB_L(i)                        (0x00139068+(i)*64) /* R--4A */
#define NV_P2P_WREQMB_L__SIZE_1                                     8 /*       */
#define NV_P2P_WREQMB_L_PAGE_ADDR                                20:0 /* R-IUF */
#define NV_P2P_WREQMB_L_PAGE_ADDR_INIT                     0x00000000 /* R-I-V */
#define NV_P2P_WREQMB_H(i)                        (0x0013906c+(i)*64) /* R--4A */
#define NV_P2P_WREQMB_H__SIZE_1                                     8 /*       */
#define NV_P2P_WREQMB_H_KIND                                      7:0 /* R-IUF */
#define NV_P2P_WREQMB_H_KIND_INIT                          0x00000000 /* R-I-V */
#define NV_P2P_WREQMB_H_COMPTAGLINE                              24:8 /* R-IUF */
#define NV_P2P_WREQMB_H_COMPTAGLINE_INIT                   0x00000000 /* R-I-V */
#define NV_P2P_WREQMB_H_PAGE_SIZE                               25:25 /* R-IUF */
#define NV_P2P_WREQMB_H_PAGE_SIZE_INIT                     0x00000000 /* R-I-V */
#endif // __gm200_dev_nv_p2p_h__
