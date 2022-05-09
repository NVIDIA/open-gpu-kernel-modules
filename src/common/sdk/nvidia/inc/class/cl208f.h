/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2006 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl208f_h_
#define _cl208f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/* Class within the subdevice used for diagnostic purpose*/
#define  NV20_SUBDEVICE_DIAG                                       (0x0000208f)

/* event values */
#define NV208F_NOTIFIERS_SW                                        (0)
#define NV208F_NOTIFIERS_MAXCOUNT                                  (1)

/* NvNotification[] fields and values */
#define NV208f_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT          (0x4000)
/* pio method data structure */
typedef volatile struct _cl208f_tag0 {
 NvV32 Reserved00[0x7c0];
} Nv208fTypedef, Nv20SubdeviceDiag;
#define  NV208f_TYPEDEF                                            Nv20SubdeviceDiag

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl208f_h_ */

