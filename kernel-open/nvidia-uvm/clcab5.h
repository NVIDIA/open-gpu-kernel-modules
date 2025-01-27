/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clcab5_h_
#define _clcab5_h_

#define BLACKWELL_DMA_COPY_B                                                            (0x0000CAB5)

#define NVCAB5_LAUNCH_DMA                                                               (0x00000300)
#define NVCAB5_LAUNCH_DMA_DATA_TRANSFER_TYPE                                            1:0
#define NVCAB5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NONE                                       (0x00000000)
#define NVCAB5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PIPELINED                                  (0x00000001)
#define NVCAB5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NON_PIPELINED                              (0x00000002)
#define NVCAB5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PREFETCH                                   (0x00000003)

#define NVCAB5_REQ_ATTR                                                                 (0x00000754)
#define NVCAB5_REQ_ATTR_PREFETCH_L2_CLASS                                               1:0
#define NVCAB5_REQ_ATTR_PREFETCH_L2_CLASS_EVICT_FIRST                                   (0x00000000)
#define NVCAB5_REQ_ATTR_PREFETCH_L2_CLASS_EVICT_NORMAL                                  (0x00000001)
#define NVCAB5_REQ_ATTR_PREFETCH_L2_CLASS_EVICT_LAST                                    (0x00000002)
#define NVCAB5_REQ_ATTR_PREFETCH_L2_CLASS_EVICT_DEMOTE                                  (0x00000003)

#endif /* _clcab5_h_ */

