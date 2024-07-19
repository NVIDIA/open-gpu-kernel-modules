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

#ifndef __v04_00_dev_disp_h__
#define __v04_00_dev_disp_h__

#define NV_PDISP_VGA_WORKSPACE_BASE                                                           0x00625F04 /* RW-4R */
#define NV_PDISP_VGA_WORKSPACE_BASE_STATUS                                                           3:3 /* RWIVF */
#define NV_PDISP_VGA_WORKSPACE_BASE_STATUS_VALID                                              0x00000001 /* RW--V */
#define NV_PDISP_VGA_WORKSPACE_BASE_ADDR                                                            31:8 /* RWIVF */
#define NV_PDISP_FE_CORE_HEAD_STATE(i)                                             (0x00612078+(i)*2048) /* R--4A */
#define NV_PDISP_FE_CORE_HEAD_STATE_OPERATING_MODE                                                   9:8 /* R--VF */
#define NV_PDISP_FE_CORE_HEAD_STATE_OPERATING_MODE_SLEEP                                      0x00000000 /* R---V */
#define NV_PDISP_FE_CORE_HEAD_STATE_OPERATING_MODE_SNOOZE                                     0x00000001 /* R---V */
#define NV_PDISP_FE_CORE_HEAD_STATE_OPERATING_MODE_AWAKE                                      0x00000002 /* R---V */
#define NV_PDISP_FE_RM_INTR_EN_HEAD_TIMING(i)                                         (0x00611D80+(i)*4) /* RW-4A */
#define NV_PDISP_FE_RM_INTR_EN_HEAD_TIMING_LAST_DATA                                                 1:1 /* RWIVF */
#define NV_PDISP_FE_RM_INTR_EN_HEAD_TIMING_LAST_DATA_INIT                                     0x00000000 /* RWI-V */
#define NV_PDISP_FE_RM_INTR_EN_HEAD_TIMING_LAST_DATA_DISABLE                                  0x00000000 /* RW--V */
#define NV_PDISP_FE_RM_INTR_EN_HEAD_TIMING_LAST_DATA_ENABLE                                   0x00000001 /* RW--V */
#endif // __v04_00_dev_disp_h__
