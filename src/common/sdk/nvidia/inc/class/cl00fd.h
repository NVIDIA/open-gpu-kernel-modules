/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * Class definition for allocating a contiguous or discontiguous Multicast FLA.
 */

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl00fd.finn
//

#include "cl00e0.h"



#define NV_MEMORY_MULTICAST_FABRIC                               (0xfdU) /* finn: Evaluated from "NV00FD_ALLOCATION_PARAMETERS_MESSAGE_ID" */

/*
 *  expPacket [IN]
 *    Export object (see cl00e0.h) identifier from which memory will be imported.
 *
 *  index [IN]
 *    Index of the export object to which the memory object is attached.
 */


/*
 *  alignment [IN]
 *    Alignment for the allocation.
 *    Should be at least the requested page size.
 *
 *  allocSize [IN]
 *    Size of the Multicast FLA VA.
 *
 *  pageSize [IN]
 *    Requested page size. Can be any of the NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_*
 *
 *  allocFlags [IN]
 *     Should be one of NV_MEMORY_MULTICAST_FABRIC_ALLOC_FLAGS_*
 *
 *  numGpus [IN]
 *    Number of unique GPUs to be attached.
 *
 *  pOsEvent [IN]
 *    Optional OS event handle created with NvRmAllocOsEvent().
 */

#define NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M                0x20000000

/*
 * This flag must be passed if the object is created using export packet. Note
 * when this flag is provided, the input params: alignment, allocSize,
 * numGpus, pageSize won't be honored.
 */
#define NV_MEMORY_MULTICAST_FABRIC_ALLOC_FLAGS_USE_EXPORT_PACKET 0x00000001



#define NV00FD_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00fdU)

typedef struct NV00FD_ALLOCATION_PARAMETERS {
    NV_EXPORT_MEM_PACKET expPacket;
    NvU16                index;


    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NV_DECLARE_ALIGNED(NvU64 allocSize, 8);
    NvU32                pageSize;
    NvU32                allocFlags;
    NvU32                numGpus;
    NV_DECLARE_ALIGNED(NvP64 pOsEvent, 8);
} NV00FD_ALLOCATION_PARAMETERS;

