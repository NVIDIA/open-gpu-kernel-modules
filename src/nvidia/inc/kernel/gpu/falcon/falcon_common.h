/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef FALCON_COMMON_H
#define FALCON_COMMON_H

/*!
 * Alignment/granularity of falcon memory blocks
 */
#define FLCN_BLK_ALIGNMENT (256)

/*!
 * Address/offset alignment required for falcon IMEM accesses
 */
#define FLCN_IMEM_ACCESS_ALIGNMENT (4)

/*!
 * Address/offset alignment required for falcon DMEM accesses
 */
#define FLCN_DMEM_ACCESS_ALIGNMENT (4)

/*!
 * Falcon IMEM block-size (as a power-of-2)
 */
#define FALCON_IMEM_BLKSIZE2 (8)

/*!
 * Falcon DMEM block-size (as a power-of-2)
 */
#define FALCON_DMEM_BLKSIZE2 (8)

/*!
 * Denotes invalid/absent VA for falcon ucode loading
 */
#define FLCN_DMEM_VA_INVALID 0xffffffff

/*!
 * Default Falcon context buffer size
 */
#define FLCN_CTX_ENG_BUFFER_SIZE_HW      4096

/*!
 * Number of register read needed for reset signal propagation
 */
#define FLCN_RESET_PROPAGATION_DELAY_COUNT    10

/*!
 * Used by FALCON_DMATRFCMD polling functions to wait for _FULL==FALSE or _IDLE==TRUE
 */
typedef enum {
    FLCN_DMA_POLL_QUEUE_NOT_FULL = 0,
    FLCN_DMA_POLL_ENGINE_IDLE = 1
} FlcnDmaPollMode;

#endif  // FALCON_COMMON_H
