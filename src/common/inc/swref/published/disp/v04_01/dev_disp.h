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

#ifndef __v04_01_dev_disp_h__
#define __v04_01_dev_disp_h__
#define NV_PDISP                                                                       0x006F1FFF:0x00610000 /* RW--D */

#define NV_PDISP_FE_EVT_DISPATCH                                                              0x00611A00 /* R--4R */
#define NV_PDISP_FE_EVT_DISPATCH_SEM_WIN                                                           26:26 /* R--VF */
#define NV_PDISP_FE_EVT_DISPATCH_SEM_WIN_NOT_PENDING                                          0x00000000 /* R---V */
#define NV_PDISP_FE_EVT_DISPATCH_SEM_WIN_PENDING                                              0x00000001 /* R---V */

#define NV_PDISP_FE_EVT_STAT_SEM_WIN                                                          0x00611868 /* RW-4R */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH(i)                                               (0+(i)):(0+(i)) /*       */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH__SIZE_1                                                       32 /*       */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH_INIT                                                  0x00000000 /*       */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH_NOT_PENDING                                           0x00000000 /*       */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH_PENDING                                               0x00000001 /*       */
#define NV_PDISP_FE_EVT_STAT_SEM_WIN_CH_RESET                                                 0x00000001 /*       */

#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP                                                    0x00611C30 /* R--4R */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_WIN_SEM                                                   9:9 /* R-IVF */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_WIN_SEM_NOT_PENDING                                0x00000000 /* R-I-V */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_WIN_SEM_PENDING                                    0x00000001 /* R---V */

#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING(i)                                       (0x00611C00+(i)*4) /* R--4A */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING__SIZE_1                                                   8 /*       */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING(i)                                           (0x00611800+(i)*4) /* RW-4A */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING__SIZE_1                                                       8 /*       */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM(i)                                     (16+(i)):(16+(i)) /*       */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM__SIZE_1                                                6 /*       */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM_INIT                                          0x00000000 /*       */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM_NOT_PENDING                                   0x00000000 /*       */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM_PENDING                                       0x00000001 /*       */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_SEM_RESET                                         0x00000001 /*       */

#endif // __v04_01_dev_disp_h__
