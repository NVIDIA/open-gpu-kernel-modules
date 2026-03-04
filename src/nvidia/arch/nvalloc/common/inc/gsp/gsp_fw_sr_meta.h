/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#ifndef GSP_FW_SR_META_H_
#define GSP_FW_SR_META_H_

#define GSP_FW_SR_META_MAGIC     0x8a3bb9e6c6c39d93ULL
#define GSP_FW_SR_META_REVISION  2

#define GSP_FW_SR_META_INTERNAL_SIZE  128

/*!
 * GSP firmware SR metadata
 *
 * Initialized by CPU-RM and kept in Sysmem.
 * Consumed by secure ucode (Booter or ACR_RISCV).
 */
typedef struct
{
    // Magic for verification by secure ucode
    NvU64 magic;  // = GSP_FW_SR_META_MAGIC;

    //
    // Revision number
    // Bumped up when we change this interface so it is not backward compatible.
    //
    NvU64 revision;  // = GSP_FW_SR_META_MAGIC_REVISION;

    // Members regarding data in SYSMEM
    NvU64 sysmemAddrOfSuspendResumeData;
    NvU64 sizeOfSuspendResumeData;

    //
    // Internal members for use by secure ucode
    // Must be exactly GSP_FW_SR_META_INTERNAL_SIZE bytes.
    //
    NvU32 internal[32];

    // Same as flags of GspFwWprMeta
    NvU32 flags;

    // Subrevision number used by secure ucode
    NvU32 subrevision;

    //
    // Pad structure to exactly 256 bytes (1 DMA chunk).
    // Padding initialized to zero.
    //
    NvU32 padding[22];
} GspFwSRMeta;

#endif // GSP_FW_SR_META_H_
