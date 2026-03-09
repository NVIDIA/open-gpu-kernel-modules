/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb10b_dev_nv_xal_ep_zb__h__
#define __gb10b_dev_nv_xal_ep_zb__h__

#define NV_XAL_EP_ZB                               0x00000FFF:0x00000000 /* RW--D */
#define NV_XAL_EP_ZB_BAR0_WINDOW                                                  0x00000d40 /* RW-4R */
#define NV_XAL_EP_ZB_BAR0_WINDOW_BASE                                                   24:0 /* RWIUF */
#define NV_XAL_EP_BAR0_WINDOW_BASE_0                                            0x0000000 /* RWI-V */
#define NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT                                        0x0000010 /*       */

#endif // __gb10b_dev_nv_xal_ep_zb__h__
