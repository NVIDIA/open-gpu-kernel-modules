/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gm107_dev_fifo_h__
#define __gm107_dev_fifo_h__
#define NV_PFIFO_FB_IFACE                                0x000026f0 /* RW-4R */
#define NV_PFIFO_FB_IFACE_CONTROL                               0:0 /* RWIUF */
#define NV_PFIFO_FB_IFACE_CONTROL_DISABLE                0x00000000 /* RW--V */
#define NV_PFIFO_FB_IFACE_CONTROL_ENABLE                 0x00000001 /* RWI-V */
#define NV_PFIFO_FB_IFACE_STATUS                                4:4 /* R-IUF */
#define NV_PFIFO_FB_IFACE_STATUS_DISABLED                0x00000000 /* R---V */
#define NV_PFIFO_FB_IFACE_STATUS_ENABLED                 0x00000001 /* R-I-V */
#endif // __gm107_dev_fifo_h__
