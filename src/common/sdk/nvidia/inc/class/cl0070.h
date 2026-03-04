/*
 * Copyright (c) 2001-2022, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl0070.finn
//

#define NV01_MEMORY_VIRTUAL        (0x70U) /* finn: Evaluated from "NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS_MESSAGE_ID" */
#define NV01_MEMORY_SYSTEM_DYNAMIC (0x70U) /* finn: Evaluated from "NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS_MESSAGE_ID" */

/*
 * NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS
 *
 * Allocation params for NV01_MEMORY_VIRTUAL.
 *
 * NV01_MEMORY_SYSTEM_DYNAMIC is an alias for NV01_MEMORY_VIRTUAL.  This
 * was traditionally allocated with RmAllocMemory64(). The default GPU
 * virtual address space is used, and the limit of this address space is
 * returned in limit.  The NV01_MEMORY_SYSTEM_DYNAMIC handle can be
 * passed to RmAllocContextDma2() with an offset/limit.  The context dma
 * handle can then be used as the hDma handle for RmMapMemoryDma.
 *
 * This behavior is maintained in the RM compatibility shim.
 *
 * NV01_MEMORY_VIRTUAL replaces this behavior with a single object.
 *
 * hVASpace - if hVASpace is NV01_NULL_OBJECT the default GPU VA space is
 *      selected.  Alternatively a FERMI_VASPACE_A handle may be specified.
 *
 *      The NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE is used for by the
 *      compatibility layer to emulate NV01_MEMORY_SYSTEM_DYNAMIC semantics.
 *
 * offset - An offset into the virtual address space may be specified.  This
 *      will limit range of the GPU VA returned by RmMapMemoryDma to be
 *      above offset.
 *
 * limit - When limit is zero the maximum limit used.  If a non-zero limit
 *      is specified then it will be used.  The final limit is returned.
 */
#define NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS_MESSAGE_ID (0x0070U)

typedef struct NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offset, 8);   // [IN] - offset into address space
    NV_DECLARE_ALIGNED(NvU64 limit, 8);    // [IN/OUT] - limit of address space
    NvHandle hVASpace;                     // [IN] - Address space handle
} NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS;

#define NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE 0xffffffffU

