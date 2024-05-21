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

#ifndef GSP_FW_WPR_META_H_
#define GSP_FW_WPR_META_H_

/*!
 * GSP firmware WPR metadata
 *
 * Initialized by CPU-RM and DMA'd to FB, at the end of what will be WPR2.
 * Verified, and locked in WPR2 by Booter.
 *
 * Firmware scrubs the last 256mb of FB, no memory outside of this region
 * may be used until the FW RM has scrubbed the remainder of memory.
 *
 *   ---------------------------- <- fbSize (end of FB, 1M aligned)
 *   | VGA WORKSPACE            |
 *   ---------------------------- <- vbiosReservedOffset  (64K? aligned)
 *   | (potential align. gap)   |
 *   ---------------------------- <- gspFwWprEnd + frtsSize + pmuReservedSize
 *   | PMU mem reservation      |
 *   ---------------------------- <- gspFwWprEnd (128K aligned) + frtsSize
 *   | FRTS data                |    (frtsSize is 0 on GA100)
 *   | ------------------------ | <- frtsOffset
 *   | BOOT BIN (e.g. SK + BL)  |
 *   ---------------------------- <- bootBinOffset
 *   | GSP FW ELF               |
 *   ---------------------------- <- gspFwOffset
 *   | GSP FW (WPR) HEAP        |
 *   ---------------------------- <- gspFwHeapOffset
 *   | Booter-placed metadata   |
 *   | (struct GspFwWprMeta)    |
 *   ---------------------------- <- gspFwWprStart (128K aligned)
 *   | GSP FW (non-WPR) HEAP    |
 *   ---------------------------- <- nonWprHeapOffset, gspFwRsvdStart
 *                                   (GSP_CARVEOUT_SIZE bytes from end of FB)
 */
typedef struct
{
    // Magic
    // BL to use for verification (i.e. Booter locked it in WPR2)
    NvU64 magic; // = 0xdc3aae21371a60b3;

    // Revision number of Booter-BL-Sequencer handoff interface
    // Bumped up when we change this interface so it is not backward compatible.
    // Bumped up when we revoke GSP-RM ucode
    NvU64 revision; // = 1;

    // ---- Members regarding data in SYSMEM ----------------------------
    // Consumed by Booter for DMA

    NvU64 sysmemAddrOfRadix3Elf;
    NvU64 sizeOfRadix3Elf;

    NvU64 sysmemAddrOfBootloader;
    NvU64 sizeOfBootloader;

    // Offsets inside bootloader image needed by Booter
    NvU64 bootloaderCodeOffset;
    NvU64 bootloaderDataOffset;
    NvU64 bootloaderManifestOffset;

    union
    {
        // Used only at initial boot
        struct
        {
            NvU64 sysmemAddrOfSignature;
            NvU64 sizeOfSignature;
        };

        //
        // Used at suspend/resume to read GspFwHeapFreeList
        // Offset relative to GspFwWprMeta FBMEM PA (gspFwWprStart)
        //
        struct
        {
            NvU32 gspFwHeapFreeListWprOffset;
            NvU32 unused0;
            NvU64 unused1;
        };
    };

    // ---- Members describing FB layout --------------------------------
    NvU64 gspFwRsvdStart;

    NvU64 nonWprHeapOffset;
    NvU64 nonWprHeapSize;

    NvU64 gspFwWprStart;

    // GSP-RM to use to setup heap.
    NvU64 gspFwHeapOffset;
    NvU64 gspFwHeapSize;

    // BL to use to find ELF for jump
    NvU64 gspFwOffset;
    // Size is sizeOfRadix3Elf above.

    NvU64 bootBinOffset;
    // Size is sizeOfBootloader above.

    NvU64 frtsOffset;
    NvU64 frtsSize;

    NvU64 gspFwWprEnd;

    // GSP-RM to use for fbRegionInfo?
    NvU64 fbSize;

    // ---- Other members -----------------------------------------------

    // GSP-RM to use for fbRegionInfo?
    NvU64 vgaWorkspaceOffset;
    NvU64 vgaWorkspaceSize;

    // Boot count.  Used to determine whether to load the firmware image.
    NvU64 bootCount;

    // This union is organized the way it is to start at an 8-byte boundary and achieve natural
    // packing of the internal struct fields.
    union
    {
        struct
        {
            // TODO: the partitionRpc* fields below do not really belong in this
            //       structure. The values are patched in by the partition bootstrapper
            //       when GSP-RM is booted in a partition, and this structure was a
            //       convenient place for the bootstrapper to access them. These should
            //       be moved to a different comm. mechanism between the bootstrapper
            //       and the GSP-RM tasks.

            // Shared partition RPC memory (physical address)
            NvU64 partitionRpcAddr;

            // Offsets relative to partitionRpcAddr
            NvU16 partitionRpcRequestOffset;
            NvU16 partitionRpcReplyOffset;

            // Code section and dataSection offset and size.
            NvU32 elfCodeOffset;
            NvU32 elfDataOffset;
            NvU32 elfCodeSize;
            NvU32 elfDataSize;

            // Used during GSP-RM resume to check for revocation
            NvU32 lsUcodeVersion;
        };

        struct
        {
            // Pad for the partitionRpc* fields, plus 4 bytes
            NvU32 partitionRpcPadding[4];

            // CrashCat (contiguous) buffer size/location - occupies same bytes as the
            // elf(Code|Data)(Offset|Size) fields above.
            // TODO: move to GSP_FMC_INIT_PARAMS
            NvU64 sysmemAddrOfCrashReportQueue;
            NvU32 sizeOfCrashReportQueue;

            // Pad for the lsUcodeVersion field
            NvU32 lsUcodeVersionPadding[1];
        };
    };

    // Number of VF partitions allocating sub-heaps from the WPR heap
    // Used during boot to ensure the heap is adequately sized
    NvU8 gspFwHeapVfPartitionCount;

    // Flags to help decide GSP-FW flow.
    NvU8 flags;

    // Pad structure to exactly 256 bytes.  Can replace padding with additional
    // fields without incrementing revision.  Padding initialized to 0.
    NvU8 padding[2];

    //
    // Starts at gspFwWprEnd+frtsSize b/c FRTS is positioned
    // to end where this allocation starts (when RM requests FSP to create
    // FRTS).
    //
    NvU32 pmuReservedSize;

    // BL to use for verification (i.e. Booter says OK to boot)
    NvU64 verified;  // 0x0 -> unverified, 0xa0a0a0a0a0a0a0a0 -> verified
} GspFwWprMeta;

#define GSP_FW_WPR_META_VERIFIED  0xa0a0a0a0a0a0a0a0ULL
#define GSP_FW_WPR_META_REVISION  1
#define GSP_FW_WPR_META_MAGIC     0xdc3aae21371a60b3ULL

#define GSP_FW_WPR_HEAP_FREE_REGION_COUNT 128

typedef struct
{
    //
    // offset relative to GspFwWprMeta FBMEM PA
    // describes a region at [offs, offs + length)
    //
    NvU32 offs; // start, inclusive
    NvU32 length;
} GspFwHeapFreeRegion;

typedef struct
{
    NvU64 magic;
    NvU32 nregions;
    GspFwHeapFreeRegion regions[GSP_FW_WPR_HEAP_FREE_REGION_COUNT];
} GspFwHeapFreeList;

#define GSP_FW_HEAP_FREE_LIST_MAGIC 0x4845415046524545ULL

#define GSP_FW_FLAGS                            8:0
#define GSP_FW_FLAGS_CLOCK_BOOST                NVBIT(0)
#define GSP_FW_FLAGS_RECOVERY_MARGIN_PRESENT    NVBIT(1)

#endif // GSP_FW_WPR_META_H_
