/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080gsp.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX GSP control commands and parameters */

/*
 * NV2080_CTRL_CMD_GSP_GET_FEATURES
 *
 * This command is used to determine which GSP features are
 * supported on this GPU.
 *
 *   gspFeatures
 *     Bit mask that specifies GSP features supported.
 *   bValid
 *     If this field is set to NV_TRUE, then above bit mask is
 *     considered valid. Otherwise, bit mask should be ignored
 *     as invalid. bValid will be set to NV_TRUE when RM is a
 *     GSP client with GPU support offloaded to GSP firmware.
 *   bDefaultGspRmGpu
 *     If this field is set to NV_TRUE, it indicates that the
 *     underlying GPU has GSP-RM enabled by default. If set to NV_FALSE,
 *     it indicates that the GPU has GSP-RM disabled by default.
 *   firmwareVersion
 *     This field contains the buffer into which the firmware build version
 *     should be returned, if GPU is offloaded. Otherwise, the buffer
 *     will remain untouched.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GSP_GET_FEATURES    (0x20803601) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GSP_INTERFACE_ID << 8) | NV2080_CTRL_GSP_GET_FEATURES_PARAMS_MESSAGE_ID" */

#define NV2080_GSP_MAX_BUILD_VERSION_LENGTH (0x0000040)

#define NV2080_CTRL_GSP_GET_FEATURES_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_GSP_GET_FEATURES_PARAMS {
    NvU32  gspFeatures;
    NvBool bValid;
    NvBool bDefaultGspRmGpu;
    NvU8   firmwareVersion[NV2080_GSP_MAX_BUILD_VERSION_LENGTH];
} NV2080_CTRL_GSP_GET_FEATURES_PARAMS;

/* Valid feature values */
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED 0:0
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED_FALSE                      (0x00000000)
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED_TRUE                       (0x00000001)
#define NV2080_CTRL_GSP_GET_FEATURES_VGPU_GSP_MIG_REFACTORING_ENABLED 1:1
#define NV2080_CTRL_GSP_GET_FEATURES_VGPU_GSP_MIG_REFACTORING_ENABLED_FALSE (0x00000000)
#define NV2080_CTRL_GSP_GET_FEATURES_VGPU_GSP_MIG_REFACTORING_ENABLED_TRUE  (0x00000001)

/*
 * NV2080_CTRL_CMD_GSP_GET_RM_HEAP_STATS
 *
 * This command reports the current GSP-RM heap usage statistics.
 *
 *  gfid
 *    The gfid that's under query: When gfid = 0, it will report the stats of PF.
 *    Otherwise, it will report stats for RM task's memory consumption associated
 *    with a given gfid.
 *  managedSize
 *    The total size in bytes of the underlying heap. Note that not all memory
 *    will be allocatable, due to fragmentation and memory allocator/tracking
 *    overhead.
 *  current
 *    An NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT record corresponding to
 *    GSP-RM heap usage at the time this command is called.
 *  peak
 *    An NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT record corresponding to
 *    the "high water mark" of heap usage since GSP-RM was started.
 */
#define NV2080_CTRL_CMD_GSP_GET_RM_HEAP_STATS                               (0x20803602) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GSP_INTERFACE_ID << 8) | NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT
 *
 * This record represents a set of heap measurements at a given point in time.
 *
 *  allocatedSize
 *    Allocated memory size, in bytes. This value does not include overhead used
 *    by the underlying allocator for padding/metadata, but does include the
 *    NvPort memory tracking overhead.
 *  usableSize
 *    Allocated memory size excluding all metadata, in bytes. This value does
 *    not include the NvPort memory tracking overhead.
 *  memTrackOverhead
 *    Allocated memory size used for NvPort memory tracking.
 */
typedef struct NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT {
    NV_DECLARE_ALIGNED(NvU64 allocatedSize, 8);
    NV_DECLARE_ALIGNED(NvU64 usableSize, 8);
    NV_DECLARE_ALIGNED(NvU64 memTrackOverhead, 8);
    NvU32 allocationCount;
} NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT;

#define NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS {
    NvU32 gfid;
    NV_DECLARE_ALIGNED(NvU64 managedSize, 8);
    NV_DECLARE_ALIGNED(NvU64 largestFreeChunkSize, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT current, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GSP_RM_HEAP_STATS_SNAPSHOT peak, 8);
} NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS;

/*
 * NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS
 *
 * This command reports the current partition's VGPU-GSP plugin's heap usage statistics.
 *
 *  managedSize
 *    The total size in bytes of the underlying heap. Note that not all memory
 *    will be allocatable, due to fragmentation and memory allocator/tracking
 *    overhead.
 *  allocatedSize
 *    Allocated memory size, in bytes. This value does not include overhead used
 *    by the underlying allocator for padding/metadata.
 *  allocationCount
 *    The number of active allocations. This count reflects the current number of
 *    memory blocks that have been allocated but not yet freed.
 *  peakAllocatedSize
 *    The highest recorded allocated memory size, in bytes. This value represents the
 *    maximum amount of memory that has been allocated at any point in time. When a new
 *    highest allocated size is recorded, the peakAllocatedSize is updated.
 *  peakAllocationCount
 *    The number of active allocations corresponding to the highest recorded peakAllocatedSize.
 */

#define NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS (0x20803603) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GSP_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 allocatedSize, 8);
    NV_DECLARE_ALIGNED(NvU64 peakAllocatedSize, 8);
    NV_DECLARE_ALIGNED(NvU64 managedSize, 8);
    NvU32 allocationCount;
    NvU32 peakAllocationCount;
    NV_DECLARE_ALIGNED(NvU64 largestFreeChunkSize, 8);
} NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS;

#define NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS (0x20803604) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GSP_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_GSP_LIBOS_POOL_COUNT_MAX     64

typedef struct NV2080_CTRL_GSP_LIBOS_POOL_STATS {
    NvU32 allocations;
    NvU32 peakAllocations;
    NV_DECLARE_ALIGNED(NvU64 objectSize, 8);
} NV2080_CTRL_GSP_LIBOS_POOL_STATS;


#define NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GSP_LIBOS_POOL_STATS poolStats[NV2080_CTRL_GSP_LIBOS_POOL_COUNT_MAX], 8);
    NV_DECLARE_ALIGNED(NvU64 totalHeapSize, 8);
    NvU8 poolCount;
} NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS;

// _ctrl2080gsp_h_
