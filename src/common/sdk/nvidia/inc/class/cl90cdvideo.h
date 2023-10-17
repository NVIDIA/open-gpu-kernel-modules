/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl90cdvideo_h_
#define _cl90cdvideo_h_

/* This file defines parameters for FECS context switch events*/

#define NV_EVENT_BUFFER_VIDEO_VERSION 1

#define NV_EVENT_BUFFER_VIDEO_TAG_ENGINE_START                0x00
#define NV_EVENT_BUFFER_VIDEO_TAG_ENGINE_END                  0x01
#define NV_EVENT_BUFFER_VIDEO_TAG_PSTATE_CHANGE               0x02
#define NV_EVENT_BUFFER_VIDEO_TAG_LOG_DATA                    0x03
#define NV_EVENT_BUFFER_VIDEO_TAG_LAST                        NV_EVENT_BUFFER_VIDEO_TAG_LOG_DATA

#define NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_START        NVBIT(NV_EVENT_BUFFER_VIDEO_TAG_ENGINE_START)
#define NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_END          NVBIT(NV_EVENT_BUFFER_VIDEO_TAG_ENGINE_END)
#define NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_PSTATE_CHANGE       NVBIT(NV_EVENT_BUFFER_VIDEO_TAG_PSTATE_CHANGE)
#define NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_LOG_DATA            NVBIT(NV_EVENT_BUFFER_VIDEO_TAG_LOG_DATA)

/* context_id is set to this value if fecs info doesn't match a known channel/tsg handle*/
#define NV_EVENT_BUFFER_VIDEO_INVALID_CONTEXT     0xFFFFFFFF

/* 
 * PID/context_id are set to these values if the data is from another user's
 * client and the current user is not an administrator
 */
#define NV_EVENT_BUFFER_VIDEO_HIDDEN_PID          0x0
#define NV_EVENT_BUFFER_VIDEO_HIDDEN_CONTEXT      0x0

/* 
 * PID/context_id are set to these values if the data is from a kernel client
 * and the data is being read by a user client
 */
#define NV_EVENT_BUFFER_VIDEO_KERNEL_PID          0xFFFFFFFF
#define NV_EVENT_BUFFER_VIDEO_KERNEL_CONTEXT      0xFFFFFFFF

typedef enum
{
    NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__NVDEC = 0,
    NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__NVENC,
    NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__NVJPEG,
    NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__OFA,
    NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__LAST = NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE__OFA,
} NV_EVENT_BUFFER_VIDEO_ENGINE_TYPE;

typedef enum
{
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__MPEG2 = 0,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__VC1,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__H264,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__MPEG4,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__VP8,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__VP9,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__HEVC,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__AVD,
    NV_EVENT_BUFFER_VIDEO_CODEC_ID__LAST = NV_EVENT_BUFFER_VIDEO_CODEC_ID__AVD,
} NV_EVENT_BUFFER_VIDEO_ENGINE_CODEC_ID;

// V1 ------------------------------------------------------------------------
typedef struct
{
    NvU8  event_id;                       ///< NV_EVENT_BUFFER_VIDEO_TAG_*
    NvU8  vmid;
    NvU16 seqno;
    NvU32 context_id;                     ///< channel/tsg handle 
    NvU64 pid NV_ALIGN_BYTES(8);          ///< process id
    NvU64 timestamp NV_ALIGN_BYTES(8);
    NvU64 api_id NV_ALIGN_BYTES(8);
    union
    {
        NvU64 event_data;

        struct
        {
            NvU8  engine_type;
            NvU8  engine_id;
            NvU16 codec_id;
            NvU32 status;
        } session;

        struct
        {
            NvU16 from;
            NvU16 to;
        } stateChange;

        struct
        {
            NvU8  engine_type;
            NvU8  engine_id;
            NvU16 codec_id;
            NvU32 size;
        } logData;
    };
} NV_EVENT_BUFFER_VIDEO_RECORD_V1;

typedef NV_EVENT_BUFFER_VIDEO_RECORD_V1 NV_EVENT_BUFFER_VIDEO_RECORD;
#endif /* _cl90cdvideo_h_ */

