/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

// @todo: Remove this once chips_a/drivers/resman/arch/nvalloc/common/inc/riscvifriscv.h has been
//        switched to directly include this file.
#if !defined(RISCVIFRISCV_H) && !defined(LIBOS_INIT_H_)
#define LIBOS_INIT_H_

#define LIBOS_MEMORY_REGION_INIT_ARGUMENTS_MAX 4096

typedef NvU64 LibosAddress;

typedef enum {
    LIBOS_MEMORY_REGION_NONE,
    LIBOS_MEMORY_REGION_CONTIGUOUS,
    LIBOS_MEMORY_REGION_RADIX3
} LibosMemoryRegionKind;

typedef enum {
    LIBOS_MEMORY_REGION_LOC_NONE,
    LIBOS_MEMORY_REGION_LOC_SYSMEM,
    LIBOS_MEMORY_REGION_LOC_FB
} LibosMemoryRegionLoc;

#define LIBOS_MEMORY_REGION_RADIX_PAGE_SIZE 4096
#define LIBOS_MEMORY_REGION_RADIX_PAGE_LOG2 12
typedef struct
{
    LibosAddress          id8;  // Id tag.
    LibosAddress          pa;   // Physical address.
    LibosAddress          size; // Size of memory area.
    NvU8                  kind; // See LibosMemoryRegionKind above.
    NvU8                  loc;  // See LibosMemoryRegionLoc above.
} LibosMemoryRegionInitArgument;

#endif
