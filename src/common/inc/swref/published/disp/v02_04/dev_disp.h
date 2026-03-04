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

#ifndef __v02_04_dev_disp_h__
#define __v02_04_dev_disp_h__

#define NV_PDISP_DSI_RM_INTR_DISPATCH                                        0x00610088 /* R--4R */
#define NV_PDISP_DSI_RM_INTR_DISPATCH_HEAD(i)                         (24+(i)):(24+(i)) /* R--VF */
#define NV_PDISP_DSI_RM_INTR_DISPATCH_HEAD__SIZE_1                                    4 /*       */
#define NV_PDISP_DSI_RM_INTR_DISPATCH_HEAD_NOT_PENDING                       0x00000000 /*       */
#define NV_PDISP_DSI_RM_INTR_DISPATCH_HEAD_PENDING                           0x00000001 /*       */

#define NV_PDISP_DSI_RM_INTR_HEAD(i)                              (0x006100BC+(i)*2048) /* RW-4A */
#define NV_PDISP_DSI_RM_INTR_HEAD__SIZE_1                                             4 /*       */
#define NV_PDISP_DSI_RM_INTR_HEAD_VBLANK                                            0:0 /* RWIVF */
#define NV_PDISP_DSI_RM_INTR_HEAD_VBLANK_INIT                                0x00000000 /* R-I-V */
#define NV_PDISP_DSI_RM_INTR_HEAD_VBLANK_NOT_PENDING                         0x00000000 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_VBLANK_PENDING                             0x00000001 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_VBLANK_RESET                               0x00000001 /* -W--V */

#define NV_PDISP_DSI_RM_INTR_HEAD_PMU_RG_LINE                                     29:29 /* RWIVF */
#define NV_PDISP_DSI_RM_INTR_HEAD_PMU_RG_LINE_INIT                           0x00000000 /* R-I-V */
#define NV_PDISP_DSI_RM_INTR_HEAD_PMU_RG_LINE_NOT_PENDING                    0x00000000 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_PMU_RG_LINE_PENDING                        0x00000001 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_PMU_RG_LINE_RESET                          0x00000001 /* -W--V */

#define NV_PDISP_DSI_RM_INTR_HEAD_RM_RG_LINE                                      31:31 /* RWIVF */
#define NV_PDISP_DSI_RM_INTR_HEAD_RM_RG_LINE_INIT                            0x00000000 /* R-I-V */
#define NV_PDISP_DSI_RM_INTR_HEAD_RM_RG_LINE_NOT_PENDING                     0x00000000 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_RM_RG_LINE_PENDING                         0x00000001 /* R---V */
#define NV_PDISP_DSI_RM_INTR_HEAD_RM_RG_LINE_RESET                           0x00000001 /* -W--V */

#define NV_PDISP_PIPE_IN_LOADV_COUNTER(i)                         (0x00616118+(i)*2048) /* RW-4A */
#define NV_PDISP_PIPE_IN_LOADV_COUNTER__SIZE_1                                        4 /*       */
#define NV_PDISP_PIPE_IN_LOADV_COUNTER_VALUE                                       31:0 /* RWIUF */
#define NV_PDISP_PIPE_IN_LOADV_COUNTER_VALUE_INIT                            0x00000000 /* RWI-V */
#define NV_PDISP_PIPE_IN_LOADV_COUNTER_VALUE_ZERO                            0x00000000 /* RW--V */
#endif // __v02_04_dev_disp_h__
