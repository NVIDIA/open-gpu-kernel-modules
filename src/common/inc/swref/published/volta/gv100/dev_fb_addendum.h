/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef gv100_dev_fb_addendum_h
#define gv100_dev_fb_addendum_h

#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_NOTIFY                                         0:0 /* -WIVF */
#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_NOTIFY_INIT                                    0x0 /* -WI-V */
#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_NOTIFY_SET                                     0x1 /* -W--V */
#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_ERROR                                          1:1 /* -WIVF */
#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_ERROR_INIT                                     0x0 /* -WI-V */
#define NV_PFB_NISO_INTR_EN_SET_PERF_ALIAS_HUB_ACCESS_COUNTER_ERROR_SET                                      0x1 /* -W--V */

#endif // gv100_dev_fb_addendum_h
