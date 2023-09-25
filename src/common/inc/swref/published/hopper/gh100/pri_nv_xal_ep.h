/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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
#ifndef __gh100_pri_nv_xal_ep_h__
#define __gh100_pri_nv_xal_ep_h__
#define NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT                                         0x000010
#define NV_XAL_EP_BAR0_WINDOW_BASE                                                   21:0
#define NV_XAL_EP_BAR0_WINDOW                                                  0x0010fd40
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY                                        0x0010f810
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_TOKEN                                        30:0
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED                              0x0010f814
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_TOKEN                              30:0
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_STATUS                            31:31
#define NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED_STATUS_BUSY                         0x1
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED                           0x0010f80c
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_TOKEN                           30:0
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_STATUS                         31:31
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED_STATUS_BUSY                      0x1
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS                                     0x0010f808
#define NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_TOKEN                                     30:0
#define NV_XAL_EP_INTR_0                                                       0x0010f100
#define NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT                                               5:5
#define NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT_PENDING                                       0x1
#define NV_XAL_EP_INTR_0_PRI_FECSERR                                                  1:1
#define NV_XAL_EP_INTR_0_PRI_FECSERR_PENDING                                          0x1
#define NV_XAL_EP_INTR_0_PRI_REQ_TIMEOUT                                              2:2
#define NV_XAL_EP_INTR_0_PRI_REQ_TIMEOUT_PENDING                                      0x1
#define NV_XAL_EP_INTR_0_PRI_RSP_TIMEOUT                                              3:3
#define NV_XAL_EP_INTR_0_PRI_RSP_TIMEOUT_PENDING                                      0x1
#define NV_XAL_EP_SCPM_PRI_DUMMY_DATA_PATTERN_INIT                             0xbadf0200

#define NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT                            0x0010f364 /* RW-4R */
#define NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT_TOTAL                            15:0 /* RWIUF */
#define NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT_TOTAL_INIT                     0x0000 /* RWI-V */
#define NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT_UNIQUE                          31:16 /* RWIUF */
#define NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT_UNIQUE_INIT                    0x0000 /* RWI-V */

#define NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT                             0x0010f37c /* RW-4R */
#define NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT_TOTAL                             15:0 /* RWIUF */
#define NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT_TOTAL_INIT                      0x0000 /* RWI-V */
#define NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT_UNIQUE                           31:16 /* RWIUF */
#define NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT_UNIQUE_INIT                     0x0000 /* RWI-V */
#endif // __gh100_pri_nv_xal_ep_h__

