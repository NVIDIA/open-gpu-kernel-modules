/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * This file defines the internal interfaces to the NUMA allocator component,
 * used by parent module PMA.
 *
 * All interfaces here should only be used on PowerPC 9 systems with NUMA
 * mode enabled, where GPU's framebuffer memory is onlined to the Linux kernel.
 *
 * Therefore, PMA just needs to sub-allocate from the Linux kernel for most
 * allocation requests. However, PMA does keep states in order to support
 * eviction and GPU scrubber. Please see below for more details.
 */

#ifndef NUMA_H
#define NUMA_H

#include "nvport/nvport.h"
#include "phys_mem_allocator.h"
#include "nvmisc.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * @brief Handles NUMA allocation by calling kernel APIs
 *
 * This function will implement a mixture of pass-through memory allocation
 * from the Linux kernel as well as eviction from UVM if the Linux kernel
 * does not have any free memory.
 *
 * From a high level, PMA keep a bitmap for all UVM unpinned (evictable)
 * allocations and PMA will decide to kick off eviction based on the bitmap.
 *
 * Please note that GPU scrubber is used for any memory evicted and CPU scrubber
 * is used for allocations coming from the Linux kernel.
 * The perf implication is under further study. See bug #1999793.
 */
NV_STATUS pmaNumaAllocate(PMA *pPma, NvLength allocationCount, NvU64 pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions, NvU64 *pPages);

/*!
 * @brief Frees pages on a NUMA node.
 * This function implements pass-through free calls to the Linux kernel.
 * For UVM allocations PMA also updates the bitmap used for eviction.
 */
void pmaNumaFreeInternal(PMA *pPma, NvU64 *pPages, NvU64 pageCount, NvU64 size, NvU32 flag);

void pmaNumaSetReclaimSkipThreshold(PMA *pPma, NvU32 skipReclaimPercent);
#ifdef __cplusplus
}
#endif

#endif // NUMA_H
