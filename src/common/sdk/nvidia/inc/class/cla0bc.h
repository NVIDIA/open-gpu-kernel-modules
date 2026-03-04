/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cla0bc.finn
//

#define NVENC_SW_SESSION                       (0xa0bcU) /* finn: Evaluated from "NVA0BC_ALLOC_PARAMETERS_MESSAGE_ID" */

/*
 * NVENC_SESSION_INFO_REGION_MAX_COUNT_V1
 *   Number of regions.
 *   Currently we have two regions.
 *   +---------+   <== Region 1 Start
 *   | Entry 1 |
 *   | Entry 2 |
 *   |   ...   |
 *   | Entry N |
 *   +---------+   <== Region 1 End, Region 2 Start.
 *   | Entry 1 |
 *   | Entry 2 |
 *   |   ...   |
 *   | Entry N |
 *   +---------+   <== Region 2 End
 *   Region 1 : Contains GPU timestamp of each frame when frame was submitted
 *              to encode by UMD.
 *   Region 2 : Two entries in this region for each frame. Start and end GPU 
 *              timestamps of when GPU started and ended encoding a frame.
 */
#define NVENC_SESSION_INFO_REGION_MAX_COUNT_V1 2

/*
 * NVENC_SESSION_INFO_ENTRY_V1
 *   This structure represents a single timestamp entry for a frame.
 *   frameId
 *     Frame id of the frame being submitted for encoding by UMD.
 *   reserved
 *     This is a reserved field. Unused.
 *   timestamp
 *     GPU timestamp.
 */

typedef struct NVENC_SESSION_INFO_ENTRY_V1 {
    NvU32 frameId;
    NvU32 reserved;
    NV_DECLARE_ALIGNED(NvU64 timestamp, 8);
} NVENC_SESSION_INFO_ENTRY_V1;

/*
 * NVENC_SESSION_INFO_REGION_1_ENTRY_V1
 *   This structure represents an entry in Region 1.
 *   submissionTSEntry
 *     Frame id and GPU timestamp of the time when the frame was submitted to 
 *     encode by UMD.
 * 
 * NVENC_SESSION_INFO_REGION_1_ENTRY_SIZE_V1
 *   Size of each entry in region 1.
 * 
 * NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1
 *   No of entries in region 1.
 *
 * NVENC_SESSION_INFO_REGION_1_V1
 *   This structure represents Region 1.
 *
 * NVENC_SESSION_INFO_REGION_1_SIZE_V1
 *   Size of region 1.
 * 
 * NVENC_SESSION_INFO_REGION_1_OFFSET_V1
 *   First region, so offset is 0.
 */

typedef struct NVENC_SESSION_INFO_REGION_1_ENTRY_V1 {
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_ENTRY_V1 submissionTSEntry, 8);
} NVENC_SESSION_INFO_REGION_1_ENTRY_V1;

#define NVENC_SESSION_INFO_REGION_1_ENTRY_SIZE_V1          sizeof(NVENC_SESSION_INFO_REGION_1_ENTRY_V1)

#define NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1 256

typedef struct NVENC_SESSION_INFO_REGION_1_V1 {
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_REGION_1_ENTRY_V1 frameInfo[NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1], 8);
} NVENC_SESSION_INFO_REGION_1_V1;

#define NVENC_SESSION_INFO_REGION_1_SIZE_V1                sizeof(NVENC_SESSION_INFO_REGION_1_V1)

#define NVENC_SESSION_INFO_REGION_1_OFFSET_V1 0

/*
 * NVENC_SESSION_INFO_REGION_2_ENTRY_V1
 *   This structure represents a complete entry in Region 2.
 *   startTSEntry
 *     Frame id and GPU timestamp of the time when frame encoding started.
 *   endTSEntry
 *     Frame id and GPU timestamp of the time when frame encoding ended.
 *
 * NVENC_SESSION_INFO_REGION_2_ENTRY_SIZE_V1
 *   Size of each entry in region 2.
 * 
 * NVENC_SESSION_INFO_REGION_2_MAX_ENTRIES_COUNT_V1
 *   No of entries in region 2.
 *
 * NVENC_SESSION_INFO_REGION_2_V1
 *   This structure represents Region 2.
 *
 * NVENC_SESSION_INFO_REGION_2_SIZE_V1
 *   Size of region 2.
 * 
 * NVENC_SESSION_INFO_REGION_2_OFFSET_V1
 *   Offset of region 2 from base.
 */

typedef struct NVENC_SESSION_INFO_REGION_2_ENTRY_V1 {
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_ENTRY_V1 startTSEntry, 8);
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_ENTRY_V1 endTSEntry, 8);
} NVENC_SESSION_INFO_REGION_2_ENTRY_V1;

#define NVENC_SESSION_INFO_REGION_2_ENTRY_SIZE_V1          sizeof(NVENC_SESSION_INFO_REGION_2_ENTRY_V1)

#define NVENC_SESSION_INFO_REGION_2_MAX_ENTRIES_COUNT_V1 NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1

typedef struct NVENC_SESSION_INFO_REGION_2_V1 {
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_REGION_2_ENTRY_V1 frameInfo[NVENC_SESSION_INFO_REGION_2_MAX_ENTRIES_COUNT_V1], 8);
} NVENC_SESSION_INFO_REGION_2_V1;

#define NVENC_SESSION_INFO_REGION_2_SIZE_V1                sizeof(NVENC_SESSION_INFO_REGION_2_V1)

#define NVENC_SESSION_INFO_REGION_2_OFFSET_V1              (NVENC_SESSION_INFO_REGION_1_OFFSET_V1 + \
                                                            NVENC_SESSION_INFO_REGION_1_SIZE_V1)

/*
 * NVENC_SESSION_INFO_V1
 *   This structure represents the complete memory allocated to store the per 
 *   frame submission-start-end timestamps data.
 *
 * NVENC_SESSION_INFO_SIZE_V1
 *   Size of complete memory.
 */

typedef struct NVENC_SESSION_INFO_V1 {
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_REGION_1_V1 region1, 8);
    NV_DECLARE_ALIGNED(NVENC_SESSION_INFO_REGION_2_V1 region2, 8);
} NVENC_SESSION_INFO_V1;

#define NVENC_SESSION_INFO_SIZE_V1                         sizeof(NVENC_SESSION_INFO_V1)

/*
 * NVA0BC_ALLOC_PARAMETERS
 *
 * This structure represents NVENC SW session allocation parameters.
 *
 *   codecType
 *     Codec type to be used to do the encoding.
 *   hResolution
 *     Width of frames to be encoded.
 *   vResolution
 *     Height of frames to be encoded.
 *   version
 *     Adding version to handle any future changes to struct.
 *     In future we can extend this struct to notify RM that UMD needs to send
 *     other data. Versioning will help in identifying the difference in structs.
 *     Values are defined by NVA0BC_ALLOC_PARAMS_VER_xxx.
 *   hMem
 *     Handle to the system memory allocated by UMD.
 *     RM needs to access the memory to get the raw timestamp data and process it.
 */

#define NVA0BC_ALLOC_PARAMETERS_MESSAGE_ID (0xa0bcU)

typedef struct NVA0BC_ALLOC_PARAMETERS {
    NvU32    codecType;
    NvU32    hResolution;
    NvU32    vResolution;

    NvU32    version;
    NvHandle hMem;
} NVA0BC_ALLOC_PARAMETERS;

#define NVA0BC_ALLOC_PARAMS_VER_0               0x00000000
#define NVA0BC_ALLOC_PARAMS_VER_1               0x00000001

#define NVA0BC_NVENC_SESSION_CODEC_TYPE_H264    0x000000
#define NVA0BC_NVENC_SESSION_CODEC_TYPE_HEVC    0x000001
#define NVA0BC_NVENC_SESSION_CODEC_TYPE_AV1     0x000002
#define NVA0BC_NVENC_SESSION_CODEC_TYPE_UNKNOWN 0x0000FF
