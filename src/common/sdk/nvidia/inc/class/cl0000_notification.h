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

#ifndef _cl0000_notification_h_
#define _cl0000_notification_h_

#ifdef __cplusplus
extern "C" {
#endif

/*event values*/
#define NV0000_NOTIFIERS_DISPLAY_CHANGE                            (0)
#define NV0000_NOTIFIERS_EVENT_NONE_PENDING                        (1)
#define NV0000_NOTIFIERS_GPU_UNBIND_EVENT                          (2)
#define NV0000_NOTIFIERS_GPU_BIND_EVENT                            (3)
#define NV0000_NOTIFIERS_NVTELEMETRY_REPORT_EVENT                  (4)
#define NV0000_NOTIFIERS_MAXCOUNT                                  (5)

/*Status definitions for NV0000_NOTIFIERS_DISPLAY_CHANGE event*/

#define NV0000_NOTIFIERS_STATUS_ACPI_DISPLAY_DEVICE_CYCLE          (0)

//---------------------------------------------------------------------------

/* NvNotification[] fields and values */
#define NV000_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)


/* pio method data structure */
typedef volatile struct _cl0000_tag0 {
    NvV32 Reserved00[0x7c0];
} Nv000Typedef, Nv01Root;

/* obsolete aliases */
#define NV000_TYPEDEF                                              Nv01Root
#define Nv1Root                                                    Nv01Root
#define nv1Root                                                    Nv01Root
#define nv01Root                                                   Nv01Root

/*event values*/
#define NV0000_NOTIFIERS_ENABLE_CPU_UTIL_CTRL                      (1) 

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl0000_notification_h_ */

