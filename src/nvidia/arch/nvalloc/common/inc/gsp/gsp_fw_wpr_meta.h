/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *   ---------------------------- <- gspFwWprEnd (128K aligned)
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

    NvU64 sysmemAddrOfSignature;
    NvU64 sizeOfSignature;

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

    // Bit 0 is used to check if [VGPU-GSP] mode is active in init partition
    NvU8 driverModel;

    // Pad structure to exactly 256 bytes.  Can replace padding with additional
    // fields without incrementing revision.  Padding initialized to 0.
    NvU8 padding[11];

    // BL to use for verification (i.e. Booter says OK to boot)
    NvU64 verified;  // 0x0 -> unverified, 0xa0a0a0a0a0a0a0a0 -> verified
} GspFwWprMeta;

#define GSP_FW_WPR_META_VERIFIED  0xa0a0a0a0a0a0a0a0ULL
#define GSP_FW_WPR_META_REVISION  1
#define GSP_FW_WPR_META_MAGIC     0xdc3aae21371a60b3ULL

// Bit 0 is used to check if [VGPU-GSP] mode is active in init partition
#define DRIVERMODEL_VGPU 0

#endif // GSP_FW_WPR_META_H_
