/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl9074_h_
#define _cl9074_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define GF100_TIMED_SEMAPHORE_SW                                   (0x00009074)

/* NvNotification[] fields and values */
#define NV9074_NOTIFICATION_STATUS_PENDING                         (0x8000)
#define NV9074_NOTIFICATION_STATUS_DONE_FLUSHED                    (0x0001)
#define NV9074_NOTIFICATION_STATUS_DONE                            (0x0000)

#define NV9074_SET_NOTIFIER_HI_V                                            7:0

#define NV9074_SET_SEMAPHORE_HI_V                                           7:0

#define NV9074_SCHEDULE_SEMAPHORE_RELEASE_NOTIFY                            1:0

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl9074_h_ */

