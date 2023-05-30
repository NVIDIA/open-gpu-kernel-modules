/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0080/ctrl0080dma.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/* NV01_DEVICE_XX/NV03_DEVICE dma control commands and parameters */

/*
 * NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK
 *
 *    This parameter returns the parameters specific to a PTE as follows:
 *       pageSize
 *           GET: This parameter returns the page size of the PTE information
 *                being returned.  If 0, then this pteBlock[] array entry is
 *                invalid or not used.  (pteBlock[0] is always used.)
 *           SET: This parameter specifies the page size of the PTE information
 *                to be set.  If 0, then this pteBlock[] array entry is invalid
 *                or not used.  (pteBlock[0] is always used.)
 *       pteEntrySize
 *           GET: This parameter returns the size of the PTE in bytes for this GPU.
 *           SET: N/A
 *       comptagLine
 *           GET: This parameter returns the comptagline field of the corresponding PTE.
 *           SET: This parameter sets the comptagline field of the corresponding PTE.
 *                Incorrect values may lead to dire consequences.
 *       kind
 *           GET: This parameter returns the kind field of the corresponding PTE.
 *           SET: This parameter sets the kind field of the corresponding PTE.
 *                Incorrect values may lead to undesirable consequences.
 *       pteFlags
 *           This parameter returns various fields from the PTE, these are:
 *           FLAGS_VALID:
 *               GET: This flag returns the valid bit of the PTE.
 *               SET: This flag sets the valid bit of the PTE.
 *           FLAGS_ENCRYPTED:
 *               GET: This flag returns the encrypted bit of the PTE. Not all GPUs
 *                  support encryption. If not supported, this flag will be set to
 *                  NOT_SUPPORTED.
 *               SET: This flag sets the encrypted bit of the PTE.
 *           FLAGS_APERTURE:
 *               GET: This flag returns the aperture field of the PTE. See
 *                    NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS_FLAGS_APERTURE_* for values.
 *               SET: This flag sets the aperture field of the PTE.  See
 *                    NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS_FLAGS_APERTURE_* for values.
 *           FLAGS_COMPTAGS:
 *               GET: This flag returns the comptags field of the PTE.  (Not used on Fermi)
 *               SET: N/A
 *           FLAGS_GPU_CACHED:
 *               GET: This flag returns the GPU cacheable bit of the PTE. GPU caching of
 *                    sysmem was added in iGT21a and Fermi. If not supported, this flag
 *                    will be set to NOT_SUPPORTED.
 *               SET: N/A for specific chips, e.g., GF100
 *           FLAGS_SHADER_ACCESS:
 *               GET: This flag returns the shader access control of the PTE. This feature
 *                    was introduced in Kepler.  If not supported, this flag will be set to
 *                    NOT_SUPPORTED.
 *               SET: N/A
 */

typedef struct NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK {
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);
    NV_DECLARE_ALIGNED(NvU64 pteEntrySize, 8);
    NvU32 comptagLine;
    NvU32 kind;
    NvU32 pteFlags;
} NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK;

#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_VALID                                     0:0
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_VALID_FALSE                         (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_VALID_TRUE                          (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ENCRYPTED                                 2:1
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ENCRYPTED_FALSE                     (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ENCRYPTED_TRUE                      (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ENCRYPTED_NOT_SUPPORTED             (0x00000002U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_APERTURE                                  6:3
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_APERTURE_VIDEO_MEMORY               (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_APERTURE_PEER_MEMORY                (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_APERTURE_SYSTEM_COHERENT_MEMORY     (0x00000002U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_APERTURE_SYSTEM_NON_COHERENT_MEMORY (0x00000003U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS                                  10:7
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS_NONE                       (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS_1                          (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS_2                          (0x00000002U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS_4                          (0x00000004U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_GPU_CACHED                                12:11
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_GPU_CACHED_FALSE                    (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_GPU_CACHED_TRUE                     (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_GPU_CACHED_NOT_SUPPORTED            (0x00000002U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_SHADER_ACCESS                             14:13
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_SHADER_ACCESS_READ_WRITE            (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_SHADER_ACCESS_READ_ONLY             (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_SHADER_ACCESS_WRITE_ONLY            (0x00000002U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_SHADER_ACCESS_NOT_SUPPORTED         (0x00000003U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_READ_ONLY                                 15:15
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_READ_ONLY_FALSE                     (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_READ_ONLY_TRUE                      (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ATOMIC                                    16:16
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ATOMIC_DISABLE                      (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ATOMIC_ENABLE                       (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ACCESS_COUNTING                           17:17
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ACCESS_COUNTING_DISABLE             (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_ACCESS_COUNTING_ENABLE              (0x00000001U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_PRIVILEGED                                18:18
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_PRIVILEGED_FALSE                    (0x00000000U)
#define NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_PRIVILEGED_TRUE                     (0x00000001U)

/*
 * NV0080_CTRL_DMA_GET_PTE_INFO
 *
 * This command queries PTE information for the specified GPU virtual address.
 *
 *   gpuAddr
 *      This parameter specifies the GPU virtual address for which PTE
 *      information is to be returned.
 *   skipVASpaceInit
 *      This parameter specifies(true/false) whether the VA Space
 *      initialization should be skipped in this ctrl call.
 *   pteBlocks
 *      This parameter returns the page size-specific attributes of a PTE.
 *      Please see NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK.
 *   hVASpace
 *      handle for the allocated VA space that this control call should operate
 *      on. If it's 0, it assumes to use the implicit allocated VA space.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_GENERIC
 */

#define NV0080_CTRL_CMD_DMA_GET_PTE_INFO                                          (0x801801U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_GET_PTE_INFO_PTE_BLOCKS                                   5U

#define NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 gpuAddr, 8);
    NvU32    subDeviceId;
    NvU8     skipVASpaceInit;
    NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK pteBlocks[NV0080_CTRL_DMA_GET_PTE_INFO_PTE_BLOCKS], 8);
    NvHandle hVASpace;
} NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS;

/*
 * NV0080_CTRL_DMA_SET_PTE_INFO
 *
 * This command sets PTE information for the specified GPU virtual address.
 * Usage of parameter and field definitions is identical to that of
 * NV0080_CTRL_DMA_GET_PTE_INFO, with the following exception:
 *
 * - pteFlags field NV0080_CTRL_DMA_PTE_INFO_PARAMS_FLAGS_COMPTAGS is ignored,
 *   as this setting is specified via the kind specification.
 * - pteEntrySize is ignored, as this setting is read-only in the GET case.
 * - hVASpace
 *    handle for the allocated VA space that this control call should operate
 *    on. If it's 0, it assumes to use the implicit allocated VA space.
 *
 */

#define NV0080_CTRL_CMD_DMA_SET_PTE_INFO        (0x80180aU) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_SET_PTE_INFO_PTE_BLOCKS 5U

#define NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 gpuAddr, 8);
    NvU32    subDeviceId;
    NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK pteBlocks[NV0080_CTRL_DMA_SET_PTE_INFO_PTE_BLOCKS], 8);
    NvHandle hVASpace;
} NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS;


#define NV0080_CTRL_CMD_DMA_FILL_PTE_MEM (0x801802U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS {
    NvU32 pageCount;
    struct {
        NvHandle hClient;
        NvHandle hDevice;
        NvHandle hMemory;
        NvU32    subDeviceId;
    } hwResource;
    struct {
        NvU32 fbKind;
        NvU32 sysKind;
        NvU32 compTagStartOffset;
    } comprInfo;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 gpuAddr, 8);
    NV_DECLARE_ALIGNED(NvP64 pageArray, 8);
    NV_DECLARE_ALIGNED(NvP64 pteMem, 8);
    NvU32    pteMemPfn;
    NvU32    pageSize;
    NvU32    startPageIndex;
    NvU32    flags;
    NvHandle hSrcVASpace;
    NvHandle hTgtVASpace;
    NvU32    peerId;
} NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS;



/*
 * NV0080_CTRL_DMA_FLUSH
 *
 * This command flushes the specified target unit
 *
 *   targetUnit
 *      The unit to flush, either L2 cache or compression tag cache.
 *      This field is a logical OR of the individual fields such as
 *      L2 cache or compression tag cache. Also L2 invalidation for
 *      either SYSMEM/PEERMEM is triggered. But this invalidation is 
 *      for FERMI.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 *
 * See Also:
 *   NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE
 *     Flushes the entire GPU cache or a set of physical addresses (if the
 *     hardware supports it).  Use this call if you want to flush a set of
 *     addresses or the entire GPU cache in unicast mode.
 *   NV0041_CTRL_CMD_SURFACE_FLUSH_GPU_CACHE
 *     Flushes memory associated with a single allocation if the hardware
 *     supports it.  Use this call if you want to flush a single allocation and
 *     you have a memory object describing the physical memory.
 */
#define NV0080_CTRL_CMD_DMA_FLUSH                                                     (0x801805U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_FLUSH_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_FLUSH_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0080_CTRL_DMA_FLUSH_PARAMS {
    NvU32 targetUnit;
} NV0080_CTRL_DMA_FLUSH_PARAMS;

#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2                         0:0
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2_DISABLE            (0x00000000U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2_ENABLE             (0x00000001U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_COMPTAG                    1:1
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_COMPTAG_DISABLE       (0x00000000U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_COMPTAG_ENABLE        (0x00000001U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_FB                         2:2
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_FB_DISABLE            (0x00000000U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_FB_ENABLE             (0x00000001U)

// This is exclusively for Fermi
// The selection of non-zero valued bit-fields avoids the routing 
// into the above cases and vice-versa
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2_INVALIDATE              4:3
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2_INVALIDATE_SYSMEM  (0x00000001U)
#define NV0080_CTRL_DMA_FLUSH_TARGET_UNIT_L2_INVALIDATE_PEERMEM (0x00000002U)


/**
 * NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS
 *
 * This command returns information about the VA caps on the GPU
 *
 *   vaBitCount
 *     Returns number of bits in a virtual address
 *   pdeCoverageBitCount
 *     Returns number of VA bits covered in each PDE.  One PDE covers
 *     2^pdeCoverageBitCount bytes.
 *
 *   bigPageSize
 *     Size of the big page
 *   compressionPageSize
 *     Size of region each compression tag covers
 *   dualPageTableSupported
 *     TRUE if one page table can map with both 4KB and big pages
 *
 *   numPageTableFormats
 *     Returns the number of different page table sizes supported by the RM
 *   pageTableBigFormat
 *   pageTable4KFormat[]
 *     Returns size in bytes and number of VA bits covered by each page table
 *     format.  Up to MAX_NUM_PAGE_TABLE_FORMATS can be returned.  The most
 *     compact format will be pageTableSize[0] and the least compact format
 *     will be last.
 *   hVASpace
 *     handle for the allocated VA space that this control call should operate
 *     on. If it's 0, it assumes to use the implicit allocated VA space.
 *   vaRangeLo 
 *     Indicates the start of usable VA range.
 *
 *   hugePageSize
 *     Size of the huge page if supported, 0 otherwise.
 *
 *   vaSpaceId
 *     Virtual Address Space id assigned by RM.
 *     Only relevant on AMODEL.
 *
 *   pageSize512MB
 *     Size of the 512MB page if supported, 0 otherwise.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */
#define NV0080_CTRL_CMD_DMA_ADV_SCHED_GET_VA_CAPS               (0x801806U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS_MESSAGE_ID" */

typedef struct NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PAGE_TABLE_FORMAT {
    NvU32 pageTableSize;
    NvU32 pageTableCoverage;
} NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PAGE_TABLE_FORMAT;

#define NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_MAX_NUM_PAGE_TABLE_FORMATS (16U)
#define NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS {
    NvU32                                                   vaBitCount;
    NvU32                                                   pdeCoverageBitCount;
    NvU32                                                   num4KPageTableFormats;
    NvU32                                                   bigPageSize;
    NvU32                                                   compressionPageSize;
    NvU32                                                   dualPageTableSupported;
    NvU32                                                   idealVRAMPageSize;
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PAGE_TABLE_FORMAT pageTableBigFormat;
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PAGE_TABLE_FORMAT pageTable4KFormat[NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_MAX_NUM_PAGE_TABLE_FORMATS];
    NvHandle                                                hVASpace;
    NV_DECLARE_ALIGNED(NvU64 vaRangeLo, 8);
    NvU32                                                   vaSpaceId;
    NV_DECLARE_ALIGNED(NvU64 supportedPageSizeMask, 8);
} NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS;

/*
 * Adding a version define to allow clients to access valid
 * parameters based on version.
 */
#define NV0080_CTRL_CMD_DMA_ADV_SCHED_GET_VA_CAPS_WITH_VA_RANGE_LO 0x1U

/*
 * NV0080_CTRL_DMA_GET_PDE_INFO
 *
 * This command queries PDE information for the specified GPU virtual address.
 *
 *   gpuAddr
 *       This parameter specifies the GPU virtual address for which PDE
 *       information is to be returned.
 *   pdeVirtAddr
 *       This parameter returns the GPU virtual address of the PDE.
 *   pdeEntrySize
 *       This parameter returns the size of the PDE in bytes for this GPU.
 *   pdeAddrSpace
 *       This parameter returns the GPU address space of the PDE.
 *   pdeSize
 *       This parameter returns the fractional size of the page table(s) as
 *       actually set in the PDE, FULL, 1/2, 1/4 or 1/8.  (This amount may
 *       differ from that derived from pdeVASpaceSize.)  Intended for VERIF only.
 *   pteBlocks
 *       This parameter returns the page size-specific parameters as follows:
 *       ptePhysAddr
 *           This parameter returns the GPU physical address of the page table.
 *       pteCacheAttrib
 *           This parameter returns the caching attribute of the
 *           GPU physical address of the page table.
 *       pteEntrySize
 *           This parameter returns the size of the PTE in bytes for this GPU.
 *       pageSize
 *           This parameter returns the page size of the page table.
 *           If pageSize == 0, then this PTE block is not valid.
 *       pteAddrSpace
 *           This parameter returns the GPU address space of the page table.
 *       pdeVASpaceSize
 *           This parameter returns the size of the VA space addressable by
 *           the page table if fully used (i.e., if all PTEs marked VALID).
 *   pdbAddr
 *       This parameter returns the PDB address for the PDE.
 *   hVASpace
 *       handle for the allocated VA space that this control call should operate
 *       on. If it's 0, it assumes to use the implicit allocated VA space.
 *   
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */
#define NV0080_CTRL_CMD_DMA_GET_PDE_INFO                           (0x801809U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_MESSAGE_ID" */

typedef struct NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCK {
    NV_DECLARE_ALIGNED(NvU64 ptePhysAddr, 8);
    NvU32 pteCacheAttrib;
    NvU32 pteEntrySize;
    NvU32 pageSize;
    NvU32 pteAddrSpace;
    NvU32 pdeVASpaceSize;
    NvU32 pdeFlags;
} NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCK;

#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_VIDEO_MEMORY               (0x00000000U)
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_SYSTEM_COHERENT_MEMORY     (0x00000001U)
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_SYSTEM_NON_COHERENT_MEMORY (0x00000002U)

#define NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCKS                                           5U

#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 gpuAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 pdeVirtAddr, 8);
    NvU32    pdeEntrySize;
    NvU32    pdeAddrSpace;
    NvU32    pdeSize;
    NvU32    subDeviceId;
    NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCK pteBlocks[NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCKS], 8);
    NV_DECLARE_ALIGNED(NvU64 pdbAddr, 8);
    NvHandle hVASpace;
} NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS;

#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_VIDEO_MEMORY               (0x00000000U)
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_SYSTEM_COHERENT_MEMORY     (0x00000001U)
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_SYSTEM_NON_COHERENT_MEMORY (0x00000002U)
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_SIZE_FULL                             1U
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_SIZE_HALF                             2U
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_SIZE_QUARTER                          3U
#define NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_SIZE_EIGHTH                           4U

/*
 * NV0080_CTRL_CMD_DMA_INVALIDATE_TLB
 *
 * This command invalidates the GPU TLB. This is intended to be used
 * for RM clients that manage their own TLB consistency when updating
 * page tables on their own, or with DEFER_TLB_INVALIDATION options
 * to other RM APIs.
 *
 *   hVASpace
 *     This parameter specifies the VASpace object whose MMU TLB entries
 *     needs to be invalidated, if the flag is set to NV0080_CTRL_DMA_INVALIDATE_TLB_ALL_FALSE.
 *     Specifying a GMMU VASpace object handle will invalidate the GMMU TLB for the particular VASpace.
 *     Specifying a SMMU VASpace object handle will flush the entire SMMU TLB & PTC.
 *
 *   flags
 *     This parameter can be used to specify any flags needed
 *     for the invlalidation request.
 *       NV0080_CTRL_DMA_INVALIDATE_TLB_ALL
 *         When set to TRUE this flag requests a global invalidate.
 *         When set to FALSE this flag requests a chip-specfic
 *         optimization to invalidate only the address space bound
 *         to the associated hDevice.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_TIMEOUT_RETRY
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0080_CTRL_CMD_DMA_INVALIDATE_TLB                                            (0x80180cU) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS {
    NvHandle hVASpace;
    NvU32    flags;
} NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS;

#define NV0080_CTRL_DMA_INVALIDATE_TLB_ALL                     0:0
#define NV0080_CTRL_DMA_INVALIDATE_TLB_ALL_FALSE (0x00000000U)
#define NV0080_CTRL_DMA_INVALIDATE_TLB_ALL_TRUE  (0x00000001U)

/**
 * NV0080_CTRL_CMD_DMA_GET_CAPS
 *
 * This command returns the set of DMA capabilities for the device
 * in the form of an array of unsigned bytes.  DMA capabilities
 * include supported features and required workarounds for address
 * translation system within the device, each represented by a byte
 * offset into the table and a bit position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0080_CTRL_DMA_CAPS_TBL_SIZE.
 *
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the framebuffer caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * 32BIT_POINTER_ENFORCED
 *     If this property is TRUE NVOS32 and NVOS46 calls with
 *     32BIT_POINTER_DISABLED will return addresses above 4GB.
 *
 * SHADER_ACCESS_SUPPORTED
 *     If this property is set, the MMU in the system supports the independent
 *     access bits for the shader.  This is accessed with the following fields:
 *         NVOS46_FLAGS_SHADER_ACCESS
 *         NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS_FLAGS_SHADER_ACCESS
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_DMA_GET_CAPS             (0x80180dU) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_GET_CAPS_PARAMS_MESSAGE_ID" */
/* size in bytes of fb caps table */
#define NV0080_CTRL_DMA_CAPS_TBL_SIZE            8U
#define NV0080_CTRL_DMA_GET_CAPS_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV0080_CTRL_DMA_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NvU8  capsTbl[NV0080_CTRL_DMA_CAPS_TBL_SIZE];
} NV0080_CTRL_DMA_GET_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV0080_CTRL_DMA_GET_CAP(tbl,c)              (((NvU8)tbl[(1?c)]) & (0?c))

/* caps format is byte_index:bit_mask */
#define NV0080_CTRL_DMA_CAPS_32BIT_POINTER_ENFORCED                0:0x01
#define NV0080_CTRL_DMA_CAPS_SHADER_ACCESS_SUPPORTED               0:0x04
#define NV0080_CTRL_DMA_CAPS_SPARSE_VIRTUAL_SUPPORTED              0:0x08
#define NV0080_CTRL_DMA_CAPS_MULTIPLE_VA_SPACES_SUPPORTED          0:0x10

/*
 * NV0080_CTRL_DMA_SET_VA_SPACE_SIZE
 *
 *   Change the size of an existing VA space.
 *   NOTE: Currently this only supports growing the size, not shrinking.
 *
 *   1. Allocate new page directory able to map extended range.
 *   2. Copy existing PDEs from old directory to new directory.
 *   3. Initialize new PDEs to invalid.
 *   4. Update instmem to point to new page directory.
 *   5. Free old page directory.
 *
 *   vaSpaceSize
 *      On input, the requested size of the VA space in bytes.
 *      On output, the actual resulting VA space size.
 *
 *      The actual size will be greater than or equal to the requested size,
 *      unless NV0080_CTRL_DMA_GROW_VA_SPACE_SIZE_MAX is requested, which
 *      requests the maximum available.
 *      
 *      NOTE: Specific size requests (e.g. other than SIZE_MAX) must account
 *            for the VA hole at the beginning of the range which is used to
 *            distinguish NULL pointers. This region is not counted as part
 *            of the vaSpaceSize since it is not allocatable.
 *
 *   hVASpace
 *      handle for the allocated VA space that this control call should operate
 *      on. If it's 0, it assumes to use the implicit allocated VA space
 *      associated with the client/device pair.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NV0080_CTRL_CMD_DMA_SET_VA_SPACE_SIZE (0x80180eU) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 vaSpaceSize, 8);
    NvHandle hVASpace;
} NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS;

#define NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_MAX (0xFFFFFFFFFFFFFFFFULL)

/*
 * NV0080_CTRL_DMA_UPDATE_PDE_2
 *
 * This command updates a single PDE for the given (hClient, hDevice)
 * with specific attributes.
 * This command is only available on Windows and MODS platforms.
 * This command can be called by kernel clients only.
 *
 * The VA range the PDE maps must be contained by a VA allocation marked with
 * NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED.
 * However if the MODS-only FORCE_OVERRIDE flag is set this restriction is relaxed.
 *
 * RM does not track the PDE's attributes in SW - this control simply stuffs
 * the PDE in memory after translating and checking the parameters.
 *
 * Parameters are checked for relative consistency (e.g. valid domains),
 * but it is the client's responsibility to provide correct page table
 * addresses, e.g. global consistency is not checked.
 *
 * It is also the client's responsibility to flush/invalidate the MMU
 * when appropriate, either by setting the _FLUSH_PDE_CACHE flag for this
 * call or by flushing through other APIs.
 * This control does not flush automatically to allow batches of calls
 * to be made before a single flush.
 *
 *   ptParams
 *      Page-size-specific parameters, as follows:
 *
 *      physAddr
 *         Base address of physically contiguous memory of page table.
 *         Must be aligned sufficiently for the PDE address field.
 *      numEntries
 *         Deprecated and ignored.
 *         Use FLAGS_PDE_SIZE that applies to the tables for all page sizes.
 *      aperture
 *         Address space the base address applies to.
 *         Can be left as INVALID to ignore this page table size.
 *
 *   pdeIndex
 *      The PDE index this update applies to.
 *   flags
 *      See NV0080_CTRL_DMA_UPDATE_PDE_FLAGS_*.
 *   hVASpace
 *      handle for the allocated VA space that this control call should operate
 *      on. If it's 0, it assumes to use the implicit allocated VA space
 *      associated with the client/device pair.
 *   pPdeBuffer [out]
 *      Kernel pointer to 64 bit unsigned integer representing a Page Dir Entry
 *      that needs to be updated. It should point to memory as wide as the Page Dir
 *      Entry.
 *
 *      If NULL, Page Dir Entry updates will go to the internally managed Page Dir.
 *      If not NULL, the updates will be written to this buffer.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0080_CTRL_CMD_DMA_UPDATE_PDE_2      (0x80180fU) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS_MESSAGE_ID" */

typedef struct NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 physAddr, 8);
    NvU32 numEntries;   // deprecated
    NvU32 aperture;
} NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS;

#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_INVALID                    (0x00000000U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_VIDEO_MEMORY               (0x00000001U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_SYSTEM_COHERENT_MEMORY     (0x00000002U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_SYSTEM_NON_COHERENT_MEMORY (0x00000003U)

#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX_SMALL                           0U
#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX_BIG                             1U
#define NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE                           2U

#define NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS_MESSAGE_ID (0xFU)

typedef struct NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS {
    NvU32    pdeIndex;
    NvU32    flags;
    NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS ptParams[NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE], 8);
    NvHandle hVASpace;
    NV_DECLARE_ALIGNED(NvP64 pPdeBuffer, 8); // NV_MMU_VER2_DUAL_PDE__SIZE
    NvU32    subDeviceId; // ID+1, 0 for BC
} NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS;

/*!
 * If set a PDE cache flush (MMU invalidate) will be performed.
 */
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FLUSH_PDE_CACHE          0:0
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FLUSH_PDE_CACHE_FALSE (0x00000000U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FLUSH_PDE_CACHE_TRUE  (0x00000001U)

/*!
 * For verification purposes (MODS-only) this flag may be set to modify any PDE
 * in the VA space (RM managed or externally managed).
 * It is up to caller to restore any changes properly (or to expect faults).
 */
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FORCE_OVERRIDE           1:1
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FORCE_OVERRIDE_FALSE  (0x00000000U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_FORCE_OVERRIDE_TRUE   (0x00000001U)

/*!
 * Directly controls the PDE_SIZE field (size of the page tables pointed to by this PDE).
 */
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_PDE_SIZE                 3:2
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_PDE_SIZE_FULL         (0x00000000U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_PDE_SIZE_HALF         (0x00000001U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_PDE_SIZE_QUARTER      (0x00000002U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_PDE_SIZE_EIGHTH       (0x00000003U)

/*! 
 * Used to specify if the allocation is sparse. Applicable only in case of 
 * VA Space managed by OS, as in WDDM2.0
 */
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_SPARSE                   4:4
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_SPARSE_FALSE          (0x00000000U)
#define NV0080_CTRL_DMA_UPDATE_PDE_2_FLAGS_SPARSE_TRUE           (0x00000001U)

/*
 * NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE
 * This interface will create a corresponding privileged
 * kernel address space that will mirror user space allocations in this
 * VASPACE.
 * The user can either pass a FERMI_VASPACE_A handle or RM will use the 
 * vaspace associated with the client/device if hVaspace is passed as 
 * NULL.
 * Once this property is set, the user will not be able to make allocations
 * from  the top most PDE of this address space.
 *
 * The user is expected to call this function as soon as he has created 
 * the device/Vaspace object. If the user has already made VA allocations 
 * in this vaspace then this call will return a failure 
 * (NV_ERR_INVALID_STATE). 
 * The Vaspace should have no VA allocations when this call is made.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
*/
#define NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE                  (0x801810U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS {
    NvHandle hVASpace;
} NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS;

/*
 * NV0080_CTRL_DMA_SET_DEFAULT_VASPACE
 * This is a special control call provided for KMD to use. 
 * It will associate an allocated Address Space Object as the 
 * default address space of the device.
 * 
 * This is added so that the USER can move to using address space objects when they 
 * want to specify the size of the big page size they want to use but still want
 * to use the rest of the relevant RM apis without specifying the hVASpace.
 * 
 * This call will succeed only if there is already no VASPACE associated with the 
 * device. This means the user will have to call this before he has made any allocations
 * on this device/address space.
 *
 * The hVASpace that is passed in to be associated shoould belong to the parent device that
 * this call is made for. This call will fail if we try to associate a VASpace belonging to 
 * some other client/device.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *  
 */
#define NV0080_CTRL_DMA_SET_DEFAULT_VASPACE (0x801812U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS {
    NvHandle hVASpace;
} NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS;

/*!
 * NV0080_CTRL_DMA_SET_PAGE_DIRECTORY
 *
 * Move an existing VA space to an externally-managed top-level page directory.
 * The VA space must have been created in SHARED_MANAGEMENT mode.
 * For lifecycle details, see NV_VASPACE_ALLOCATION_PARAMETERS documentation in nvos.h.
 *
 * RM will propagate the update to all channels using the VA space.
 *
 * NOTE: All channels using this VA space are expected to be idle and unscheduled prior
 *       to and during this control call - it is responsibility of caller to ensure this.
 *
 *   physAddress
 *      Physical address of the new page directory within the aperture specified by flags.
 *   numEntries
 *      Number of entries in the new page directory.
 *      The backing phyical memory must be at least this size (multiplied by entry size).
 *   flags
 *      APERTURE
 *          Specifies which physical aperture the page directory resides.
 *      PRESERVE_PDES
 *          Deprecated - RM will always copy the RM-managed PDEs from the old page directory
 *          to the new page directory.
 *      ALL_CHANNELS
 *          If true, RM will update the instance blocks for all channels using
 *          the VAS and ignore the chId parameter.
 *      EXTEND_VASPACE
 *          If true, RM will use the client VA for client VA requests in VASPACE_SHARED_MANAGEMENT mode
 *          If false, RM will use the internal VA for client VA requests.
 *      IGNORE_CHANNEL_BUSY
 *          If true, RM will ignore the channel busy status during set page
 *          directory operation.
 *   hVASpace
 *      handle for the allocated VA space that this control call should operate
 *      on. If it's 0, it assumes to use the implicit allocated VA space
 *      associated with the client/device pair.
 *   chId
 *      ID of the Channel to be updated.
 *   pasid
 *      PASID (Process Address Space IDentifier) of the process corresponding to
 *      the VA space. Ignored unless the VA space has ATS enabled.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_LIMIT
 *   NV_ERR_GENERIC
 */
#define NV0080_CTRL_CMD_DMA_SET_PAGE_DIRECTORY (0x801813U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 physAddress, 8);
    NvU32    numEntries;
    NvU32    flags;
    NvHandle hVASpace;
    NvU32    chId;
    NvU32    subDeviceId; // ID+1, 0 for BC
    NvU32    pasid;
} NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS;

#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE                  1:0
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_VIDMEM           (0x00000000U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_SYSMEM_COH       (0x00000001U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_SYSMEM_NONCOH    (0x00000002U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_PRESERVE_PDES             2:2
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_PRESERVE_PDES_FALSE       (0x00000000U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_PRESERVE_PDES_TRUE        (0x00000001U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_ALL_CHANNELS              3:3
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_ALL_CHANNELS_FALSE        (0x00000000U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_ALL_CHANNELS_TRUE         (0x00000001U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_IGNORE_CHANNEL_BUSY       4:4
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_IGNORE_CHANNEL_BUSY_FALSE (0x00000000U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_IGNORE_CHANNEL_BUSY_TRUE  (0x00000001U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_EXTEND_VASPACE            5:5
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_EXTEND_VASPACE_FALSE      (0x00000000U)
#define NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_EXTEND_VASPACE_TRUE       (0x00000001U)

/*!
 * NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY
 *
 * Restore an existing VA space to an RM-managed top-level page directory.
 * The VA space must have been created in SHARED_MANAGEMENT mode and
 * previously relocated to an externally-managed page directory with
 * NV0080_CTRL_CMD_DMA_SET_PAGE_DIRECTORY (these two API are symmetric operations).
 * For lifecycle details, see NV_VASPACE_ALLOCATION_PARAMETERS documentation in nvos.h.
 *
 * RM will propagate the update to all channels using the VA space.
 *
 * NOTE: All channels using this VA space are expected to be idle and unscheduled prior
 *       to and during this control call - it is responsibility of caller to ensure this.
 *
 *   hVASpace
 *      handle for the allocated VA space that this control call should operate
 *      on. If it's 0, it assumes to use the implicit allocated VA space
 *      associated with the client/device pair.
 */
#define NV0080_CTRL_CMD_DMA_UNSET_PAGE_DIRECTORY                           (0x801814U) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_DMA_INTERFACE_ID << 8) | NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS {
    NvHandle hVASpace;
    NvU32    subDeviceId; // ID+1, 0 for BC
} NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS;



/* _ctrl0080dma_h_ */

