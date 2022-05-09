/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl906f_sw_h_
#define _cl906f_sw_h_

/* NvNotification[] elements */
#define NV906F_NOTIFIERS_RC                                         (0)
#define NV906F_NOTIFIERS_REFCNT                                     (1)
#define NV906F_NOTIFIERS_NONSTALL                                   (2)
#define NV906F_NOTIFIERS_EVENTBUFFER                                (3)
#define NV906F_NOTIFIERS_IDLECHANNEL                                (4)
#define NV906F_NOTIFIERS_ENDCTX                                     (5)
#define NV906F_NOTIFIERS_SW                                         (6)
#define NV906F_NOTIFIERS_GR_DEBUG_INTR                              (7)
#define NV906F_NOTIFIERS_MAXCOUNT                                   (8)

/* NvNotification[] fields and values */
#define NV906f_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT              (0x2000)
#define NV906f_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT          (0x4000)

#endif /* _cl906f_sw_h_ */
