/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0080/ctrl0080fb.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/* NV01_DEVICE_XX/NV03_DEVICE fb control commands and parameters */

/**
 * NV0080_CTRL_CMD_FB_GET_CAPS
 *
 * This command returns the set of framebuffer capabilities for the device
 * in the form of an array of unsigned bytes.  Framebuffer capabilities
 * include supported features and required workarounds for the framebuffer
 * engine(s) within the device, each represented by a byte offset into the
 * table and a bit position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0080_CTRL_FB_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the framebuffer caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_POINTER
 */
#define NV0080_CTRL_CMD_FB_GET_CAPS (0x801301) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FB_INTERFACE_ID << 8) | NV0080_CTRL_FB_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FB_GET_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0080_CTRL_FB_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV0080_CTRL_FB_GET_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV0080_CTRL_FB_GET_CAP(tbl,c)              (((NvU8)tbl[(1?c)]) & (0?c))

/* caps format is byte_index:bit_mask */
#define NV0080_CTRL_FB_CAPS_SUPPORT_RENDER_TO_SYSMEM                                0:0x01
#define NV0080_CTRL_FB_CAPS_BLOCKLINEAR                                             0:0x02
#define NV0080_CTRL_FB_CAPS_SUPPORT_SCANOUT_FROM_SYSMEM                             0:0x04
#define NV0080_CTRL_FB_CAPS_SUPPORT_CACHED_SYSMEM                                   0:0x08
#define NV0080_CTRL_FB_CAPS_SUPPORT_C24_COMPRESSION                                 0:0x10 // Deprecated
#define NV0080_CTRL_FB_CAPS_SUPPORT_SYSMEM_COMPRESSION                              0:0x20
#define NV0080_CTRL_FB_CAPS_NISO_CFG0_BUG_534680                                    0:0x40 // Deprecated
#define NV0080_CTRL_FB_CAPS_ISO_FETCH_ALIGN_BUG_561630                              0:0x80 // Deprecated

#define NV0080_CTRL_FB_CAPS_BLOCKLINEAR_GOBS_512                                    1:0x01
#define NV0080_CTRL_FB_CAPS_L2_TAG_BUG_632241                                       1:0x02
#define NV0080_CTRL_FB_CAPS_SINGLE_FB_UNIT                                          1:0x04 // Deprecated
#define NV0080_CTRL_FB_CAPS_CE_RMW_DISABLE_BUG_897745                               1:0x08 // Deprecated
#define NV0080_CTRL_FB_CAPS_OS_OWNS_HEAP_NEED_ECC_SCRUB                             1:0x10
#define NV0080_CTRL_FB_CAPS_ASYNC_CE_L2_BYPASS_SET                                  1:0x20 // Deprecated
#define NV0080_CTRL_FB_CAPS_DISABLE_TILED_CACHING_INVALIDATES_WITH_ECC_BUG_1521641  1:0x40
#define NV0080_CTRL_FB_CAPS_GENERIC_PAGE_KIND                                       1:0x80

#define NV0080_CTRL_FB_CAPS_DISABLE_MSCG_WITH_VR_BUG_1681803                        2:0x01
#define NV0080_CTRL_FB_CAPS_VIDMEM_ALLOCS_ARE_CLEARED                               2:0x02
#define NV0080_CTRL_FB_CAPS_DISABLE_PLC_GLOBALLY                                    2:0x04
#define NV0080_CTRL_FB_CAPS_PLC_BUG_3046774                                         2:0x08
#define NV0080_CTRL_FB_CAPS_PARTIAL_UNMAP                                           2:0x10


/* size in bytes of fb caps table */
#define NV0080_CTRL_FB_CAPS_TBL_SIZE         3



/*!
 * NV0080_CTRL_CMD_FB_COMPBIT_STORE_GET_INFO
 *
 * This command returns compbit backing store-related information.
 *
 *   size
 *     [out] Size of compbit store, in bytes
 *   address
 *     [out] Address of compbit store
 *   addressSpace
 *     [out] Address space of compbit store (corresponds to type NV_ADDRESS_SPACE in nvrm.h)
 *   maxCompbitLine
 *     [out] Maximum compbitline possible, determined based on size
 *   comptagsPerCacheLine
 *     [out] Number of compression tags per compression cache line, across all
 *           L2 slices.
 *   cacheLineSize
 *     [out] Size of compression cache line, across all L2 slices. (bytes)
 *   cacheLineSizePerSlice
 *     [out] Size of the compression cache line per slice (bytes)
 *   cacheLineFetchAlignment
 *     [out] Alignment used while fetching the compression cacheline range in FB.
 *           If start offset of compcacheline in FB is S and end offset is E, then
 *           the range to fetch to ensure entire compcacheline data is extracted is:
 *           (align_down(S) , align_up(E))
 *           This is needed in GM20X+ because of interleaving of data in Linear FB space.
 *           Example - In GM204 every other 1K FB chunk of data is offset by 16K.
 *   backingStoreBase
 *     [out] Address of start of Backing Store in linear FB Physical Addr space.
 *           This is the actual offset in FB which HW starts using as the Backing Store and
 *           in general will be different from the start of the region that driver allocates
 *           as the backing store. This address is expected to be 2K aligned.
 *   gobsPerComptagPerSlice
 *     [out] (Only on Pascal) Number of GOBS(512 bytes of surface PA) that correspond to one 64KB comptgaline, per slice.
 *           One GOB stores 1 byte of compression bits.
 *           0 value means this field is not applicable for the current architecture.
 *   backingStoreCbcBase
 *     [out] 2KB aligned base address of CBC (post divide address)
 *   comptaglineAllocationPolicy
 *     [out] Policy used to allocate comptagline from CBC for the device
 *   privRegionStartOffset
 *     [out] Starting offset for any priv region allocated by clients. only used by MODS
 *   Possible status values returned are:
 *   NV_OK
 */
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO (0x801306) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FB_INTERFACE_ID << 8) | NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 Size, 8);
    NV_DECLARE_ALIGNED(NvU64 Address, 8);
    NvU32 AddressSpace;
    NvU32 MaxCompbitLine;
    NvU32 comptagsPerCacheLine;
    NvU32 cacheLineSize;
    NvU32 cacheLineSizePerSlice;
    NvU32 cacheLineFetchAlignment;
    NV_DECLARE_ALIGNED(NvU64 backingStoreBase, 8);
    NvU32 gobsPerComptagPerSlice;
    NvU32 backingStoreCbcBase;
    NvU32 comptaglineAllocationPolicy;
    NV_DECLARE_ALIGNED(NvU64 privRegionStartOffset, 8);
    NvU32 cbcCoveragePerSlice;
} NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS;

#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_ADDRESS_SPACE_UNKNOWN 0 // ADDR_UNKNOWN
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_ADDRESS_SPACE_SYSMEM  1 // ADDR_SYSMEM
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_ADDRESS_SPACE_FBMEM   2 // ADDR_FBMEM

// Policy used to allocate comptaglines
/**
 * Legacy mode allocates a comptagline for 64kb page. This mode will always allocate
 * contiguous comptaglines from a ctag heap.
 */
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_POLICY_LEGACY         0
/**
 * 1TO1 mode allocates a comptagline for 64kb page. This mode will calculate
 * comptagline offset based on physical address. This mode will allocate
 * contiguous comptaglines if the surface is contiguous and non-contiguous
 * comptaglines for non-contiguous surfaces.
 */
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_POLICY_1TO1           1
/**
 * 1TO4_Heap mode allocates a comptagline for 256kb page granularity. This mode
 * will allocate comptagline from a heap. This mode will align the surface allocations
 * to 256kb before allocating comptaglines. The comptaglines allocated will always be
 * contiguous here.
 * TODO: For GA10x, this mode will support < 256kb surface allocations, by sharing
 * a comptagline with at most 3 different 64Kb allocations. This will result in
 * miixed-contiguity config where comptaglines will be allocated contiguously as well
 * as non-contiguous when shared with other allocations.
 */
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_POLICY_1TO4           2
/**
 * Rawmode will transfer allocation of comptaglines to HW, where HW manages
 * comptaglines based on physical offset. The comptaglines are cleared when SW
 * issues physical/virtual scrub to the surface before reuse.
 */
#define NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO_POLICY_RAWMODE        3

/**
 * NV0080_CTRL_CMD_FB_GET_CAPS_V2
 *
 * This command returns the same set of framebuffer capabilities for the
 * device as @ref NV0080_CTRL_CMD_FB_GET_CAPS. The difference is in the structure
 * NV0080_CTRL_FB_GET_CAPS_V2_PARAMS, which contains a statically sized array,
 * rather than a caps table pointer and a caps table size in
 * NV0080_CTRL_FB_GET_CAPS_PARAMS.
 *
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the framebuffer caps bits will be written by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_POINTER
 */
#define NV0080_CTRL_CMD_FB_GET_CAPS_V2                                  (0x801307) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FB_INTERFACE_ID << 8) | NV0080_CTRL_FB_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FB_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0080_CTRL_FB_GET_CAPS_V2_PARAMS {
    NvU8 capsTbl[NV0080_CTRL_FB_CAPS_TBL_SIZE];
} NV0080_CTRL_FB_GET_CAPS_V2_PARAMS;

/**
 * NV0080_CTRL_CMD_FB_SET_DEFAULT_VIDMEM_PHYSICALITY
 *
 * When clients allocate video memory specifying _DEFAULT (0) for
 * NVOS32_ATTR_PHYSICALITY, RM will usually allocate contiguous memory.
 * Clients can change that behavior with this command so that _DEFAULT maps to
 * another value.
 *
 * The expectation is that clients currently implicitly rely on the default,
 * but can be incrementally updated to explicitly specify _CONTIGUOUS where
 * necessary and change the default for their allocations to _NONCONTIGUOUS or
 * _ALLOW_NONCONTIGUOUS.
 *
 * In the future RM may be updated to globally default to _NONCONTIGUOUS or
 * _ALLOW_NONCONTIGUOUS, and at that point this can be removed.
 */
#define NV0080_CTRL_CMD_FB_SET_DEFAULT_VIDMEM_PHYSICALITY (0x801308) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FB_INTERFACE_ID << 8) | NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS {
    NvU32 value;
} NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS;

typedef enum NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY {
    NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_DEFAULT = 0,
    NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_NONCONTIGUOUS = 1,
    NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_CONTIGUOUS = 2,
    NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_ALLOW_NONCONTIGUOUS = 3,
} NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY;


/* _ctrl0080fb_h_ */
