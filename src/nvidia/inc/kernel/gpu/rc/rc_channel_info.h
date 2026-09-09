/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RC_CHANNEL_INFO_H
#define RC_CHANNEL_INFO_H

#include "kernel/gpu/fifo/kernel_fifo.h"

typedef enum
{
    RC_CHANNEL_INFO_TYPE_CHANNEL_INFO,
    RC_CHANNEL_INFO_TYPE_INSTBLK,
} RC_CHANNEL_INFO_TYPE;

typedef struct RC_CHANNEL_INFO
{
    RC_CHANNEL_INFO_TYPE type;
    NvU32 gfid;

    union
    {
        FIFO_CHANNEL_INFO channelInfo; // RC_CHANNEL_INFO_TYPE_CHANNEL_INFO

        struct                         // RC_CHANNEL_INFO_TYPE_INSTBLK
        {
            NvU64   address;        // Physical address or IOVA (unshifted)
            NvU32   aperture;       // INST_BLOCK_APERTURE
        } instblk;
    };
} RC_CHANNEL_INFO;

#endif // RC_CHANNEL_INFO_H
