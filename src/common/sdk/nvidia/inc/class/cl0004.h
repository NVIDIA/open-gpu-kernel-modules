/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl0004_h_
#define _cl0004_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  NV01_TIMER                                                (0x00000004)
/* NvNotification[] elements */
#define NV004_NOTIFIERS_SET_ALARM_NOTIFY                           (0)
#define NV004_NOTIFIERS_MAXCOUNT                                   (1)

/* mapped timer registers */
typedef volatile struct _Nv01TimerMapTypedef {
    NvU32 Reserved00[0x100];
    NvU32 PTimerTime0;       /* 0x00009400 */
    NvU32 Reserved01[0x3];
    NvU32 PTimerTime1;       /* 0x00009410 */
} Nv01TimerMap;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl0004_h_ */
