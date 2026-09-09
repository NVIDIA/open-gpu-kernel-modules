/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSP_FW_WPR_META_V1_H_
#define GSP_FW_WPR_META_V1_H_

#include "gsp/gsp_fw_wpr_meta.h"

/*!
 * GSP firmware WPR metadata, V1. Used by Booter (Turing through Ada).
 *
 * Byte layout matches the original GspFwWprMeta so the signed Booter binary
 * keeps reading/writing the same offsets; fields that don't apply to
 * Turing-Ada are reserved padding.
 */
typedef struct
{
    NvU64 magic;
    NvU64 revision;

    // ---- Members regarding data in SYSMEM ----------------------------
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

    NvU64 fbSize;

    NvU64 vgaWorkspaceOffset;
    NvU64 vgaWorkspaceSize;

    NvU64 bootCount;

    NvU8  reserved1[16];

    // CrashCat (contiguous) buffer size/location.
    NvU64 sysmemAddrOfCrashReportQueue;
    NvU32 sizeOfCrashReportQueue;

    NvU32 reserved2;

    // Number of VF partitions allocating sub-heaps from the WPR heap
    // Used during boot to ensure the heap is adequately sized
    NvU8 gspFwHeapVfPartitionCount;

    // Flags to help decide GSP-FW flow.
    // See GSP_FW_FLAGS_* in gsp_fw_wpr_meta.h
    NvU8 flags;

    // NV_REG_STR_RM_GSP_PAGING_CONFIG bitmask, passed from kernel RM
    NvU16 pagingConfig;

    NvU32 reserved3;

    // BL to use for verification (i.e. Booter says OK to boot)
    NvU64 verified;  // 0x0 -> unverified, 0xa0a0a0a0a0a0a0a0 -> verified

    // No more padding available. Structure is exactly 256 bytes.
    // Any new fields require increasing the structure size and bumping revision.
} GspFwWprMetaV1;

#endif // GSP_FW_WPR_META_V1_H_
