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

#ifndef __gh100_dev_nv_xpl_h_
#define __gh100_dev_nv_xpl_h_
#define NV_XPL_DL_ERR_COUNT_RBUF                                               0x00000a54 /* R--4R */
#define NV_XPL_DL_ERR_COUNT_RBUF__PRIV_LEVEL_MASK                              0x00000b08 /*       */
#define NV_XPL_DL_ERR_COUNT_RBUF_CORR_ERR                                            15:0 /* R-EVF */
#define NV_XPL_DL_ERR_COUNT_RBUF_CORR_ERR_INIT                                     0x0000 /* R-E-V */
#define NV_XPL_DL_ERR_COUNT_RBUF_UNCORR_ERR                                         31:16 /* R-EVF */
#define NV_XPL_DL_ERR_COUNT_RBUF_UNCORR_ERR_INIT                                   0x0000 /* R-E-V */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT                                            0x00000a58 /* R--4R */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT__PRIV_LEVEL_MASK                           0x00000b08 /*       */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT_CORR_ERR                                         15:0 /* R-EVF */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT_CORR_ERR_INIT                                  0x0000 /* R-E-V */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT_UNCORR_ERR                                      31:16 /* R-EVF */
#define NV_XPL_DL_ERR_COUNT_SEQ_LUT_UNCORR_ERR_INIT                                0x0000 /* R-E-V */

#define NV_XPL_DL_ERR_RESET                                                    0x00000a5c /* RW-4R */
#define NV_XPL_DL_ERR_RESET_RBUF_CORR_ERR_COUNT                                       0:0 /* RWCVF */
#define NV_XPL_DL_ERR_RESET_RBUF_CORR_ERR_COUNT_DONE                                  0x0 /* RWC-V */
#define NV_XPL_DL_ERR_RESET_RBUF_CORR_ERR_COUNT_PENDING                               0x1 /* -W--T */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_CORR_ERR_COUNT                                    1:1 /* RWCVF */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_CORR_ERR_COUNT_DONE                               0x0 /* RWC-V */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_CORR_ERR_COUNT_PENDING                            0x1 /* -W--T */
#define NV_XPL_DL_ERR_RESET_RBUF_UNCORR_ERR_COUNT                                   16:16 /* RWCVF */
#define NV_XPL_DL_ERR_RESET_RBUF_UNCORR_ERR_COUNT_DONE                                0x0 /* RWC-V */
#define NV_XPL_DL_ERR_RESET_RBUF_UNCORR_ERR_COUNT_PENDING                             0x1 /* -W--T */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_UNCORR_ERR_COUNT                                17:17 /* RWCVF */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_UNCORR_ERR_COUNT_DONE                             0x0 /* RWC-V */
#define NV_XPL_DL_ERR_RESET_SEQ_LUT_UNCORR_ERR_COUNT_PENDING                          0x1 /* -W--T */
#endif // __gh100_dev_nv_xpl_h__
