/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gb202_dev_runlist_h__
#define __gb202_dev_runlist_h__
#define NV_CHRAM_CHANNEL(i)                              (0x000+(i)*4) /* RW-4A */
#define NV_CHRAM_CHANNEL__SIZE_1      2048 /*       */
#define NV_CHRAM_CHANNEL_WRITE_CONTROL                             0:0 /* -WIVF */
#define NV_CHRAM_CHANNEL_WRITE_CONTROL_ONES_SET_BITS        0x00000000 /* -WI-V */
#define NV_CHRAM_CHANNEL_WRITE_CONTROL_ONES_CLEAR_BITS      0x00000001 /* -W--V */
#define NV_CHRAM_CHANNEL_ENABLE                                    1:1 /* RWIVF */
#define NV_CHRAM_CHANNEL_ENABLE_NOT_IN_USE                  0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_ENABLE_IN_USE                      0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_NEXT                                      2:2 /* RWIVF */
#define NV_CHRAM_CHANNEL_NEXT_FALSE                         0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_NEXT_TRUE                          0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_BUSY                                      3:3 /* R-IVF */
#define NV_CHRAM_CHANNEL_BUSY_FALSE                         0x00000000 /* R-I-V */
#define NV_CHRAM_CHANNEL_BUSY_TRUE                          0x00000001 /* R---V */
#define NV_CHRAM_CHANNEL_PBDMA_FAULTED                             4:4 /* RWIVF */
#define NV_CHRAM_CHANNEL_PBDMA_FAULTED_FALSE                0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_PBDMA_FAULTED_TRUE                 0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_ENG_FAULTED                               5:5 /* RWIVF */
#define NV_CHRAM_CHANNEL_ENG_FAULTED_FALSE                  0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_ENG_FAULTED_TRUE                   0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_ON_PBDMA                                  6:6 /* R-IVF */
#define NV_CHRAM_CHANNEL_ON_PBDMA_FALSE                     0x00000000 /* R-I-V */
#define NV_CHRAM_CHANNEL_ON_PBDMA_TRUE                      0x00000001 /* R---V */
#define NV_CHRAM_CHANNEL_ON_ENG                                    7:7 /* R-IVF */
#define NV_CHRAM_CHANNEL_ON_ENG_FALSE                       0x00000000 /* R-I-V */
#define NV_CHRAM_CHANNEL_ON_ENG_TRUE                        0x00000001 /* R---V */
#define NV_CHRAM_CHANNEL_PENDING                                   8:8 /* RWIVF */
#define NV_CHRAM_CHANNEL_PENDING_FALSE                      0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_PENDING_TRUE                       0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_CTX_RELOAD                                9:9 /* RWIVF */
#define NV_CHRAM_CHANNEL_CTX_RELOAD_FALSE                   0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_CTX_RELOAD_TRUE                    0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_PBDMA_BUSY                              10:10 /* R-IVF */
#define NV_CHRAM_CHANNEL_PBDMA_BUSY_FALSE                   0x00000000 /* R-I-V */
#define NV_CHRAM_CHANNEL_PBDMA_BUSY_TRUE                    0x00000001 /* R---V */
#define NV_CHRAM_CHANNEL_ENG_BUSY                                11:11 /* R-IVF */
#define NV_CHRAM_CHANNEL_ENG_BUSY_FALSE                     0x00000000 /* R-I-V */
#define NV_CHRAM_CHANNEL_ENG_BUSY_TRUE                      0x00000001 /* R---V */
#define NV_CHRAM_CHANNEL_ACQUIRE_FAIL                            12:12 /* RWIVF */
#define NV_CHRAM_CHANNEL_ACQUIRE_FAIL_FALSE                 0x00000000 /* RWI-V */
#define NV_CHRAM_CHANNEL_ACQUIRE_FAIL_TRUE                  0x00000001 /* RW--V */
#define NV_CHRAM_CHANNEL_UPDATE                                31:0 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_ENABLE_CHANNEL           0x00000002 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_DISABLE_CHANNEL          0x00000003 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_FORCE_CTX_RELOAD         0x00000200 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_RESET_PBDMA_FAULTED      0x00000011 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_RESET_ENG_FAULTED        0x00000021 /*       */
#define NV_CHRAM_CHANNEL_UPDATE_CLEAR_CHANNEL            0xFFFFFFFF /*       */
#define NV_RUNLIST_PREEMPT                                     0x014 /* RW-4R */
#define NV_RUNLIST_PREEMPT_ID                                    11:0 /*       */
#define NV_RUNLIST_PREEMPT_ID_HW                   10:0 /* RWIUF */
#define NV_RUNLIST_PREEMPT_ID_HW_NULL                      0x00000000 /* RWI-V */
#define NV_RUNLIST_PREEMPT_TSG_PREEMPT_PENDING                  20:20 /* R-IVF */
#define NV_RUNLIST_PREEMPT_TSG_PREEMPT_PENDING_FALSE       0x00000000 /* R-I-V */
#define NV_RUNLIST_PREEMPT_TSG_PREEMPT_PENDING_TRUE        0x00000001 /* R---V */
#define NV_RUNLIST_PREEMPT_RUNLIST_PREEMPT_PENDING              21:21 /* R-IVF */
#define NV_RUNLIST_PREEMPT_RUNLIST_PREEMPT_PENDING_FALSE   0x00000000 /* R-I-V */
#define NV_RUNLIST_PREEMPT_RUNLIST_PREEMPT_PENDING_TRUE    0x00000001 /* R---V */
#define NV_RUNLIST_PREEMPT_TYPE                                 25:24 /* RWIVF */
#define NV_RUNLIST_PREEMPT_TYPE_RUNLIST                    0x00000000 /* RWI-V */
#define NV_RUNLIST_PREEMPT_TYPE_TSG                        0x00000001 /* RW--V */
#endif // __gb202_dev_runlist_h__
