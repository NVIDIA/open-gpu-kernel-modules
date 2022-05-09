/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl007d_h_
#define _cl007d_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define NV04_SOFTWARE_TEST                                         (0x0000007D)
#define NV07D                                             0x00001fff:0x00000000
/* NvNotification[] elements */
#define NV07D_NOTIFIERS_NOTIFY                                     (0)
#define NV07D_NOTIFIERS_MAXCOUNT                                   (1)
/* NvNotification[] fields and values */
#define NV07D_NOTIFICATION_STATUS_IN_PROGRESS                      (0x8000)
#define NV07D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
#define NV07D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT               (0x2000)
#define NV07D_NOTIFICATION_STATUS_ERROR_INVALID_STATE              (0x1000)
#define NV07D_NOTIFICATION_STATUS_ERROR_STATE_IN_USE               (0x0800)
#define NV07D_NOTIFICATION_STATUS_DONE_SUCCESS                     (0x0000)

/* pio method data structures */
typedef volatile struct _cl007d_tag0 {
 NvV32 NoOperation;             /* ignored                          0100-0103*/
 NvV32 Notify;                  /* NV07D_NOTIFY_*                   0104-0107*/
 NvV32 Reserved0104[0x78/4];
 NvV32 SetContextDmaNotifies;   /* NV01_CONTEXT_DMA                 0180-0183*/
 NvV32 Reserved0184[0x1f7c/4];
} Nv07dTypedef, Nv04SoftwareTest;

#define NV07D_TYPEDEF                                          Nv04SoftwareTest
/* dma method offsets, fields, and values */
#define NV07D_SET_OBJECT                                           (0x00000000)
#define NV07D_NO_OPERATION                                         (0x00000100)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl007d_h_ */
