/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CE_UTILS_SIZES_H
#define _CE_UTILS_SIZES_H

#define NUM_COPY_BLOCKS                       4096
#define CHANNEL_HOST_SEMAPHORE_SIZE           4
#define CHANNEL_ENGINE_SEMAPHORE_SIZE         4
#define GPFIFO_SIZE                           NV906F_GP_ENTRY__SIZE * NUM_COPY_BLOCKS
#define CHANNEL_NOTIFIER_SIZE                 (sizeof(NvNotification) *                 \
                                              NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1)

#define CE_MAX_BYTES_PER_LINE                 0xffffffffULL
#define CE_METHOD_SIZE_PER_BLOCK              0x64
#define FAST_SCRUBBER_METHOD_SIZE_PER_BLOCK   0x94

// number of bytes per sec2 method-stream (including host methods)
#define SEC2_METHOD_SIZE_PER_BLOCK            0x94
#define SEC2_AUTH_TAG_BUF_SEMAPHORE_SIZE      4

#endif //  _CE_UTILS_SIZES_H
