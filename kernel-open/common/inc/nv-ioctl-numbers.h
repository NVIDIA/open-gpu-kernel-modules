/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef NV_IOCTL_NUMBERS_H
#define NV_IOCTL_NUMBERS_H

/* NOTE: using an ioctl() number > 55 will overflow! */
#define NV_IOCTL_MAGIC      'F'
#define NV_IOCTL_BASE       200
#define NV_ESC_CARD_INFO             (NV_IOCTL_BASE + 0)
#define NV_ESC_REGISTER_FD           (NV_IOCTL_BASE + 1)
#define NV_ESC_ALLOC_OS_EVENT        (NV_IOCTL_BASE + 6)
#define NV_ESC_FREE_OS_EVENT         (NV_IOCTL_BASE + 7)
#define NV_ESC_STATUS_CODE           (NV_IOCTL_BASE + 9)
#define NV_ESC_CHECK_VERSION_STR     (NV_IOCTL_BASE + 10)
#define NV_ESC_IOCTL_XFER_CMD        (NV_IOCTL_BASE + 11)
#define NV_ESC_ATTACH_GPUS_TO_FD     (NV_IOCTL_BASE + 12)
#define NV_ESC_QUERY_DEVICE_INTR     (NV_IOCTL_BASE + 13)
#define NV_ESC_SYS_PARAMS            (NV_IOCTL_BASE + 14)
#define NV_ESC_EXPORT_TO_DMABUF_FD   (NV_IOCTL_BASE + 17)
#define NV_ESC_WAIT_OPEN_COMPLETE    (NV_IOCTL_BASE + 18)

#endif
