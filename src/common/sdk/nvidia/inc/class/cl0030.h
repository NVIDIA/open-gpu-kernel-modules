/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl0030_h_
#define _cl0030_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  NV01_NULL                                                 (0x00000030)
/* NvNotification[] fields and values */
#define NV030_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct _cl0030_tag0 {
 NvV32 Reserved00[0x7c0];
} Nv030Typedef, Nv01Null;
#define NV030_TYPEDEF                                              Nv01Null
/* obsolete stuff */
#define NV1_NULL                                                   (0x00000030)
#define Nv1Null                                                    Nv01Null
#define nv1Null                                                    Nv01Null
#define nv01Null                                                   Nv01Null

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl0030_h_ */
