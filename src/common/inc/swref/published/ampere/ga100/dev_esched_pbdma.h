/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __ga100_dev_esched_pbdma_h__
#define __ga100_dev_esched_pbdma_h__
#define NV_PBDMA                                             0x000007ff:0x00000000 /* RW--D */
#define NV_PBDMA_INTR_NOTIFY                                 0x0f8 /* RW-4R */
#define NV_PBDMA_INTR_NOTIFY_VECTOR                           11:0 /* RWXUF */
#define NV_PBDMA_INTR_NOTIFY_CTRL_GSP                        30:30 /* RWXUF */
#define NV_PBDMA_INTR_NOTIFY_CTRL_GSP_DISABLE                    0 /* R---V */
#define NV_PBDMA_INTR_NOTIFY_CTRL_GSP_ENABLE                     1 /* R---V */
#define NV_PBDMA_INTR_NOTIFY_CTRL_CPU                        31:31 /* RWXUF */
#define NV_PBDMA_INTR_NOTIFY_CTRL_CPU_DISABLE                    0 /* R---V */
#define NV_PBDMA_INTR_NOTIFY_CTRL_CPU_ENABLE                     1 /* R---V */
#define NV_PBDMA_SET_CHANNEL_INFO_VEID                       ((6-1)+8):8 /* RWXUF */   
#define NV_PBDMA_SET_CHANNEL_INFO_CHID                       27:16 /*       */
#endif // __ga100_dev_esched_pbdma_h__
