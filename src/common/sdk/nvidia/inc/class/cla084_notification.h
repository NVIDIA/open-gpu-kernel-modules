/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _cla084_notification_h_
#define _cla084_notification_h_

#ifdef __cplusplus
extern "C" {
#endif

//
// @todo: We will define the actual event values later based on the use case.
// These event values are only for Test purpose.
//
/* event values */
#define NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_BOOTLOADED      (0)
#define NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_UNLOADED        (1)
#define NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_CRASHED         (2)
#define NVA084_NOTIFIERS_EVENT_GUEST_DRIVER_LOADED              (3)
#define NVA084_NOTIFIERS_EVENT_GUEST_DRIVER_UNLOADED            (4)
#define NVA084_NOTIFIERS_EVENT_PRINT_ERROR_MESSAGE              (5)
#define NVA084_NOTIFIERS_EVENT_GUEST_LICENSE_STATE_CHANGED      (6)
#define NVA084_NOTIFIERS_MAXCOUNT                               (7)

#define NVA084_NOTIFICATION_STATUS_IN_PROGRESS              (0x8000)
#define NVA084_NOTIFICATION_STATUS_BAD_ARGUMENT             (0x4000)
#define NVA084_NOTIFICATION_STATUS_ERROR_INVALID_STATE      (0x2000)
#define NVA084_NOTIFICATION_STATUS_ERROR_STATE_IN_USE       (0x1000)
#define NVA084_NOTIFICATION_STATUS_DONE_SUCCESS             (0x0000)

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _cla084_notification_h_
