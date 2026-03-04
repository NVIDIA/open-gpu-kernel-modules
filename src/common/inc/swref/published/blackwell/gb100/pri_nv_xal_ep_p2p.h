/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_pri_nv_xal_ep_p2p_h__
#define __gb100_pri_nv_xal_ep_p2p_h__

#define NV_XAL_EP_P2P                                               0x00827fff:0x00826000 /* RW--D */
#define NV_XAL_EP_P2P_REG_STRIDE                                                       64
#define NV_XAL_EP_P2P_MAX_READ_SIZE_IN_DW                                              64
#define NV_XAL_EP_P2P_WMBOX_ADDR(i)                                   (0x00826024+(i)*64) /* RW-4A */
#define NV_XAL_EP_P2P_WMBOX_ADDR__SIZE_1                                                8 /*       */
#define NV_XAL_EP_P2P_WMBOX_ADDR__PRIV_LEVEL_MASK                              0x00826900 /*       */
#define NV_XAL_EP_P2P_WMBOX_ADDR_DIS                                                  0:0 /* RWIUF */
#define NV_XAL_EP_P2P_WMBOX_ADDR_DIS_DISABLED                                         0x1 /* RWI-V */
#define NV_XAL_EP_P2P_WMBOX_ADDR_DIS_ENABLED                                          0x0 /* RW--V */
#define NV_XAL_EP_P2P_WMBOX_ADDR_ADDR                                                23:1 /* RWIUF */
#define NV_XAL_EP_P2P_WMBOX_ADDR_ADDR_INIT                                       0x03ffff /* RWI-V */
#define NV_XAL_EP_P2P_WREQMB_L(i)                                     (0x00826200+(i)*64) /* R--4A */
#define NV_XAL_EP_P2P_WREQMB_L__SIZE_1                                                  8 /*       */
#define NV_XAL_EP_P2P_WREQMB_L__PRIV_LEVEL_MASK                                0x00826900 /*       */
#define NV_XAL_EP_P2P_WREQMB_L_PAGE_ADDR                                             23:0 /* R-IUF */
#define NV_XAL_EP_P2P_WREQMB_L_PAGE_ADDR_INIT                                    0x000000 /* R-I-V */
#define NV_XAL_EP_P2P_WREQMB_H(i)                                     (0x00826204+(i)*64) /* R--4A */
#define NV_XAL_EP_P2P_WREQMB_H__SIZE_1                                                  8 /*       */
#define NV_XAL_EP_P2P_WREQMB_H__PRIV_LEVEL_MASK                                0x00826900 /*       */
#define NV_XAL_EP_P2P_WREQMB_H_REQ_ATTR                                               4:0 /* R-IUF */
#define NV_XAL_EP_P2P_WREQMB_H_REQ_ATTR_INIT                                         0x00 /* R-I-V */

#endif // __gb100_pri_nv_xal_ep_p2p_h__
