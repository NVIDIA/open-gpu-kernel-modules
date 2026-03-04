/*
 * SPDX-FileCopyrightText: Copyright (c) 2012 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _cla081_h_
#define _cla081_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVA081_VGPU_CONFIG                                  (0x0000a081)

/*event values*/
#define NVA081_NOTIFIERS_EVENT_VGPU_GUEST_CREATED               (0)
#define NVA081_NOTIFIERS_EVENT_VGPU_GUEST_INITIALISING          (1)
#define NVA081_NOTIFIERS_EVENT_VGPU_GUEST_DESTROYED             (2)
#define NVA081_NOTIFIERS_MAXCOUNT                               (5)


#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _cla081_h
