/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl0005_h_
#define _cl0005_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  NV01_EVENT                                                (0x00000005)
/* NvNotification[] fields and values */
#define NV003_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct _cl0005_tag0 {
    NvV32 Reserved00[0x7c0];
} Nv005Typedef, Nv01Event;
#define NV005_TYPEDEF                                              Nv01Event
/* obsolete stuff */
#define NV1_TIMER                                                  (0x00000004)
#define Nv1Event                                                   Nv01Event
#define nv1Event                                                   Nv01Event
#define nv01Event                                                  Nv01Event

/* NvRmAlloc() parameters */
typedef struct {
    NvHandle hParentClient;
    NvHandle hSrcResource;

    NvV32    hClass;
    NvV32    notifyIndex;
    NvP64    data NV_ALIGN_BYTES(8);
} NV0005_ALLOC_PARAMETERS;

/* NV0005_ALLOC_PARAMETERS's notifyIndex field is overloaded to contain the
 * notifyIndex value itself, plus flags, and optionally a subdevice field if
 * flags contains NV01_EVENT_SUBDEVICE_SPECIFIC. Note that NV01_EVENT_*
 * contain the full 32-bit flag value that is OR'd into notifyIndex, not the
 * contents of the FLAGS field (i.e. NV01_EVENT_* are pre-shifted into place).
 */
#define NV0005_NOTIFY_INDEX_INDEX     15:0
#define NV0005_NOTIFY_INDEX_SUBDEVICE 23:16
#define NV0005_NOTIFY_INDEX_FLAGS     31:24

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl0005_h_ */
