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
// Source file:      ctrl/ctrl00f8.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl90f1.h"
#include "mmu_fmt_types.h"

#define NV00F8_CTRL_CMD(cat,idx)       NVXXXX_CTRL_CMD(0x00f8, NV00F8_CTRL_##cat, idx)

/* NV00F8 command categories (6bits) */
#define NV00F8_CTRL_RESERVED (0x00U)
#define NV00F8_CTRL_FABRIC   (0x01U)

/*
 * NV00F8_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00F8_CTRL_CMD_NULL (0xf80000U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV00F8_CTRL_CMD_GET_INFO
 *
 * Queries memory allocation attributes.
 *
 *  size [OUT]
 *    Size of the allocation.
 *
 *  pageSize [OUT]
 *    Page size of the allocation.
 *
 *  allocFlags [OUT]
 *    Flags passed during the allocation.
 *
 *  physAttrs [OUT]
 *    Physical attributes associated with memory allocation.
 *    For flexible mappings, it is not possible to retrieve this information,
 *    behavior is undefined (returns all zeros).
 */
#define NV00F8_CTRL_CMD_GET_INFO (0xf80101U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

/*
 * addressSpace
 *     Same as NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE.
 *
 * compressionCoverage
 *     Same as NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE.
 */
typedef struct NV_PHYSICAL_MEMORY_ATTRS {
    NvU32 addressSpace;
    NvU32 compressionCoverage;
} NV_PHYSICAL_MEMORY_ATTRS;

#define NV00F8_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00F8_CTRL_GET_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);
    NvU32                    allocFlags;
    NV_PHYSICAL_MEMORY_ATTRS physAttrs;
} NV00F8_CTRL_GET_INFO_PARAMS;

/*
 * NV00F8_CTRL_CMD_DESCRIBE
 *
 * Queries the physical attributes of the fabric memory allocation.
 *
 *  offset [IN]
 *    Offset into memory allocation to query physical addresses for.
 *
 *  totalPfns [OUT]
 *    Number of PFNs in memory allocation.
 *
 *  pfnArray [OUT]
 *    Array of PFNs in memory allocation (2MB page size shifted).
 *
 *  numPfns [OUT]
 *    Number of valid entries in pfnArray.
 *
 *  attrs [OUT]
 *    Attributes associated with memory allocation.
 *
 *  physAttrs [OUT]
 *    Physical attributes associated with memory allocation.
 *    For flexible mappings, it is not possible to retrieve this information,
 *    behavior is undefined (returns all zeros).
 *
 *  memFlags [OUT]
 *    Flags associated with memory allocation.
 *
 * Note: This control call is only available for privileged clients.
 */

#define NV00F8_CTRL_CMD_DESCRIBE            (0xf80102) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_DESCRIBE_PARAMS_MESSAGE_ID" */

#define NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE 512

/*
 *  kind
 *    Kind of memory allocation.
 *
 *  pageSize
 *    Page size of memory allocation.
 *
 *  size
 *    Size of memory allocation
 *
 *  cliqueId
 *    Clique ID of the owner GPU
 */
typedef struct NV_FABRIC_MEMORY_ATTRS {
    NvU32 kind;
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 cliqueId;
} NV_FABRIC_MEMORY_ATTRS;

#define NV00F8_CTRL_DESCRIBE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00F8_CTRL_DESCRIBE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 totalPfns, 8);
    NvU32                    pfnArray[NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE];
    NvU32                    numPfns;
    NV_DECLARE_ALIGNED(NV_FABRIC_MEMORY_ATTRS attrs, 8);
    NV_PHYSICAL_MEMORY_ATTRS physAttrs;
    NvU32                    memFlags;
} NV00F8_CTRL_DESCRIBE_PARAMS;

/*
 *  hMemory
 *    Physical memory handle to be attached.
 *
 *  offset
 *    Offset into the fabric object.
 *    Must be physical memory pagesize aligned (at least).
 *
 *  mapOffSet
 *    Offset into the physical memory descriptor.
 *    Must be physical memory pagesize aligned.
 *
 *  mapLength
 *    Length of physical memory handle to be mapped.
 *    Must be physical memory pagesize aligned and less than or equal to
 *    fabric alloc size.
 */
typedef struct NV00F8_CTRL_ATTACH_MEM_INFO {
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 mapOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 mapLength, 8);
} NV00F8_CTRL_ATTACH_MEM_INFO;

/*
 * NV00F8_CTRL_CMD_ATTACH_MEM
 *
 * Attaches physical memory info to the fabric object.
 *
 *  memInfos [IN]
 *    Memory infos to be attached.
 *
 *  numMemInfos [IN]
 *    Number of memory infos to be attached.
 *
 *  flags [IN]
 *    For future use only. Must be zero for now.
 *
 *  numAttached [OUT]
 *    Successful attach count (returns a valid value on error too)
 *
 *  Restrictions:
 *  a. Physical memory with 2MB pagesize is allowed
 *  b. vidmem and sysmem with fixed node ID can be attached.
 *     sysmem with fixed node ID is supported only if EGM is enabled.
 *  c. Supported only for flexible fabric objects.
 */
#define NV00F8_CTRL_CMD_ATTACH_MEM      (0xf80103) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_ATTACH_MEM_PARAMS_MESSAGE_ID" */

#define NV00F8_MAX_ATTACHABLE_MEM_INFOS 64

#define NV00F8_CTRL_ATTACH_MEM_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV00F8_CTRL_ATTACH_MEM_PARAMS {
    NV_DECLARE_ALIGNED(NV00F8_CTRL_ATTACH_MEM_INFO memInfos[NV00F8_MAX_ATTACHABLE_MEM_INFOS], 8);
    NvU16 numMemInfos;
    NvU32 flags;
    NvU16 numAttached;
} NV00F8_CTRL_ATTACH_MEM_PARAMS;

/*
 * NV00F8_CTRL_CMD_DETACH_MEM
 *
 * Detaches physical memory handle from the fabric object.
 *
 *  offsets [IN]
 *    Offsets at which memory was attached.
 *
 *  numOffsets [IN]
 *    Number of offsets to be detached.
 *
 *  flags [IN]
 *    For future use only. Must be zero for now.
 *
 *  numDetached [OUT]
 *    Successful detach count (returns a valid value on error too)
 */
#define NV00F8_CTRL_CMD_DETACH_MEM    (0xf80104) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_DETACH_MEM_PARAMS_MESSAGE_ID" */

#define NV00F8_MAX_DETACHABLE_OFFSETS 64

#define NV00F8_CTRL_DETACH_MEM_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV00F8_CTRL_DETACH_MEM_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offsets[NV00F8_MAX_DETACHABLE_OFFSETS], 8);
    NvU16 numOffsets;
    NvU32 flags;
    NvU16 numDetached;
} NV00F8_CTRL_DETACH_MEM_PARAMS;

/*
 * NV00F8_CTRL_CMD_GET_NUM_ATTACHED_MEM
 *
 * Returns number of attached physical memory info to the fabric object in
 * a given offset range.
 *
 *  offsetStart [IN]
 *    Offsets at which memory was attached.
 *
 *  offsetEnd [IN]
 *    Offsets at which memory was attached.
 *
 *  numMemInfos [OUT]
 *    Number of memory infos.
 */
#define NV00F8_CTRL_CMD_GET_NUM_ATTACHED_MEM (0xf80105) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS_MESSAGE_ID" */

#define NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offsetStart, 8);
    NV_DECLARE_ALIGNED(NvU64 offsetEnd, 8);
    NvU16 numMemInfos;
} NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS;

/*
 * NV00F8_CTRL_CMD_GET_PAGE_LEVEL_INFO
 *
 * Queries page table  information for a specific memory fabric address. This
 * call is only supported for Verif platforms. This will return the same info
 * as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 *
 *  offset [IN]
 *    Memory fabric Offset from the base address for which page table
 *    information is queried. This offset should be aligned to physical page
 *    size.
 *
 *  numLevels [OUT]
 *    Number of levels populated.
 *
 *  levels [OUT]
 *    Per-level information.
 *
 *  pFmt
 *    Same as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 *
 *  levelFmt
 *    Same as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 * 
 *  sublevelFmt
 *    Same as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 *
 *  aperture
 *    Same as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 *
 *  size
 *    Same as NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS.
 */
#define NV00F8_CTRL_CMD_GET_PAGE_LEVEL_INFO (0xf80107U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS_MESSAGE_ID" */

#define NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NvU32 numLevels;
    NV_DECLARE_ALIGNED(NV_CTRL_VASPACE_PAGE_LEVEL levels[GMMU_FMT_MAX_LEVELS], 8);
} NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS;

/* _ctrl00f8_h_ */
