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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl00f8.finn
//



#include "nvcfg_sdk.h"


/*
 * Class definition for allocating a contiguous or discontiguous FLA.
 */

#define NV_MEMORY_FABRIC                (0xf8U) /* finn: Evaluated from "NV00F8_ALLOCATION_PARAMETERS_MESSAGE_ID" */

/*
 *  alignment [IN]
 *    Alignment for the allocation.
 *    Should be at least the requested page size.
 *
 *  allocSize [IN]
 *    Size of the FLA VA.
 *
 *  pageSize [IN]
 *    Requested page size. Can be any of the NV_MEMORY_FABRIC_PAGE_SIZE_*
 *
 *  allocFlags [IN]
 *    Can be any of the NV00F8_ALLOC_FLAGS_*
 *        DEFAULT (sticky)
 *            The FLA -> PA mappings will be stuck to the object, i.e, once the mapping is created
 *            there is no way to unmap it explicitly.
 *            The FLA object must be destroyed to release the mappings.
 *            The FLA object can't be duped or exported until it has a mapping associated with it.
 *            Partial FLA->PA mappings will NOT be allowed.
 *        FLEXIBLE_FLA
 *            The FLA -> PA mappings can be modified anytime irrespective of the FLA object is duped
 *            or exported.
 *            Partial FLA mappings are allowed.
 *        FORCE_NONCONTIGUOUS
 *            The allocator may pick contiguous memory whenever possible. This flag forces the
 *            allocator to always allocate noncontiguous memory. This flag is mainly used for
 *            testing purpose. So, use with caution.
 *        FORCE_CONTIGUOUS
 *            This flag forces the allocator to always allocate contiguous memory.
 *        READ_ONLY
 *            The FLA -> PA mappings will be created read-only. This option is only available on
 *            debug/develop builds due to security concerns. The security concerns are due to the
 *            fact that FLA access errors (a.k.a PRIV errors) are not aways context attributable.
 *
 *  map.offset [IN]
 *    Offset into the physical memory descriptor.
 *    Must be physical memory page size aligned.
 *
 *  map.hVidMem [IN]
 *    - Handle to the physical memory.
 *    - Must be passed so that the FLA -> GPA mapping can happen during object creation.
 *    - For sticky allocations, physical memory being mapped should be large enough
 *      (accounting map.offset) to cover the whole fabric object allocation size.
 *    - For flexible allocations, physical memory handle should be zero.
 *    - Phys memory with 2MB and 512MB pages is supported.
 *    - hVidMem should belong the same device and client which is allocating FLA.
 *
 *  map.flags [IN]
 *    Reserved for future use.
 *    Clients should pass 0 as of now.
 */

#define NV_MEMORY_FABRIC_PAGE_SIZE_2M   0x200000
#define NV_MEMORY_FABRIC_PAGE_SIZE_512M 0x20000000
#define NV_MEMORY_FABRIC_PAGE_SIZE_256G 0x4000000000

#define NV00F8_ALLOC_FLAGS_DEFAULT      0
#define NV00F8_ALLOC_FLAGS_FLEXIBLE_FLA        NVBIT(0)
#define NV00F8_ALLOC_FLAGS_FORCE_NONCONTIGUOUS NVBIT(1)
#define NV00F8_ALLOC_FLAGS_FORCE_CONTIGUOUS    NVBIT(2)
#define NV00F8_ALLOC_FLAGS_READ_ONLY           NVBIT(3)

#define NV00F8_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00f8U)

typedef struct NV00F8_ALLOCATION_PARAMETERS {

    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NV_DECLARE_ALIGNED(NvU64 allocSize, 8);

    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);
    NvU32 allocFlags;

    struct {
        NV_DECLARE_ALIGNED(NvU64 offset, 8);

        NvHandle hVidMem;
        NvU32    flags;
    } map;
} NV00F8_ALLOCATION_PARAMETERS;

