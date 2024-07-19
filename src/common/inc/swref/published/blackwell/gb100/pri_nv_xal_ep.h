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

#ifndef __gb100_pri_nv_xal_ep_h__
#define __gb100_pri_nv_xal_ep_h__

#define NV_XAL_EP_INTR_0                                                       0x0010f100 /* RW-4R */
#define NV_XAL_EP_INTR_0_PRI_FECSERR                                                  1:1 /* RWIVF */
#define NV_XAL_EP_INTR_0_PRI_FECSERR_PENDING                                          0x1 /* R---V */
#define NV_XAL_EP_INTR_0_PRI_REQ_TIMEOUT                                              2:2 /* RWIVF */
#define NV_XAL_EP_INTR_0_PRI_REQ_TIMEOUT_PENDING                                      0x1 /* R---V */
#define NV_XAL_EP_INTR_0_PRI_RSP_TIMEOUT                                              3:3 /* RWIVF */
#define NV_XAL_EP_INTR_0_PRI_RSP_TIMEOUT_PENDING                                      0x1 /* R---V */
#define NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT                                               5:5 /* RWIVF */
#define NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT_PENDING                                       0x1 /* R---V */
#define NV_XAL_EP_INTR_0_TRS_TIMEOUT                                                24:24 /* RWIVF */
#define NV_XAL_EP_INTR_0_TRS_TIMEOUT_PENDING                                          0x1 /* R---V */
#define NV_XAL_EP_SCPM_PRI_DUMMY_DATA_PATTERN_INIT                             0xbadf0200 /* RWI-V */

#define NV_XAL_EP_BAR0_WINDOW                                                  0x0010fd40 /* RW-4R */
#define NV_XAL_EP_BAR0_WINDOW_BASE                                                   22:0 /* RWIUF */
#define NV_XAL_EP_BAR0_WINDOW_BASE_0                                             0x000000 /* RWI-V */
#define NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT                                         0x000010 /*       */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS                                     0x0010f808 /* R--4R */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_TOKEN                                     30:0 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_TOKEN_INIT                          0x00000000 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED                           0x0010f80c /* R--4R */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_TOKEN                           30:0 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_TOKEN_INIT                0x00000000 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_STATUS                         31:31 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_STATUS_IDLE                      0x0 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_STATUS_BUSY                      0x1 /* R---V */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY                                        0x0010f810 /* R--4R */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_TOKEN                                        30:0 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_TOKEN_INIT                             0x00000000 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED                              0x0010f814 /* R--4R */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_TOKEN                              30:0 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_TOKEN_INIT                   0x00000000 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_STATUS                            31:31 /* R-IUF */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_STATUS_IDLE                         0x0 /* R-I-V */
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_STATUS_BUSY                         0x1 /* R---V */

#endif // __gb100_pri_nv_xal_ep_h__
