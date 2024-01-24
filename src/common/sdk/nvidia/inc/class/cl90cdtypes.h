/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl90cdtypes_h_
#define _cl90cdtypes_h_

#ifdef __cplusplus
extern "C" {
#endif

//
// Legacy values record type values have been kept for backward
// compatibility. New values should be added sequentially.
//
#define NV_EVENT_BUFFER_RECORD_TYPE_INVALID                                   (0)
#define NV_EVENT_BUFFER_RECORD_TYPE_VIDEO_TRACE                               (1)
#define NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH_V2                        (2)
#define NV_EVENT_BUFFER_RECORD_TYPE_NVTELEMETRY_REPORT_EVENT_SYSTEM           (4)
#define NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE                            (8)
#define NV_EVENT_BUFFER_RECORD_TYPE_NVTELEMETRY_REPORT_EVENT_SUBDEVICE        (132)
#define NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH                           (134)


#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _cl90cdtypes_h_

