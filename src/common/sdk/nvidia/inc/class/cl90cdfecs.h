/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl90cdFecs_h_
#define _cl90cdFecs_h_

/* This file defines parameters for FECS context switch events*/

#define NV_EVENT_BUFFER_FECS_VERSION 2

/*
 * These are the types of context switch events
 * This field gets added to NV_EVENT_BUFFER_FECS_RECORD to specify the sub type of fecs event
 * Do *not* edit these as they are defined to maintain consistency with Tegra tools
 */
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_SO                  0x00
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_CTXSW_REQ_BY_HOST   0x01
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK              0x02
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_WFI          0x0a
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_GFXP         0x0b
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CTAP         0x0c
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CILP         0x0d
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_SAVE_END            0x03
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_RESTORE_START       0x04
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_CONTEXT_START       0x05
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_START        0x06
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_END          0x07
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_ENGINE_RESET        0xfe
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_INVALID_TIMESTAMP   0xff
#define NV_EVENT_BUFFER_FECS_CTXSWTAG_LAST                NV_EVENT_BUFFER_FECS_EVENTS_CTXSWTAG_INVALID_TIMESTAMP

/*
 * Bit fields used to enable a particular sub type of event 
 */
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SO                   NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_SO)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_CTXSW_REQ_BY_HOST    NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_CTXSW_REQ_BY_HOST)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_FE_ACK               NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SAVE_END             NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_SAVE_END)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_RESTORE_START        NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_RESTORE_START)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_CONTEXT_START        NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_CONTEXT_START)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_START         NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_START)
#define NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_END           NVBIT(NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_END)

/* context_id is set to this value if fecs info doesn't match a known channel/tsg handle*/
#define NV_EVENT_BUFFER_INVALID_CONTEXT     0xFFFFFFFF

/* 
 * PID/context_id are set to these values if the data is from another user's
 * client and the current user is not an administrator
 */
#define NV_EVENT_BUFFER_HIDDEN_PID          0x0
#define NV_EVENT_BUFFER_HIDDEN_CONTEXT      0x0

/* 
 * PID/context_id are set to these values if the data is from a kernel client
 * and the data is being read by a user client
 */
#define NV_EVENT_BUFFER_KERNEL_PID          0xFFFFFFFF
#define NV_EVENT_BUFFER_KERNEL_CONTEXT      0xFFFFFFFF

// V1 ------------------------------------------------------------------------
typedef struct
{
    NvU8    tag;                            ///< NV_EVENT_BUFFER_FECS_CTXSWTAG_*
    NvU8    vmid;
    NvU16   seqno;                          ///< used to detect drop 
    NvU32   context_id;                     ///< channel/tsg handle 
    NvU64   pid NV_ALIGN_BYTES(8);          ///< process id
    NvU64   timestamp NV_ALIGN_BYTES(8);
    /* Do *not* edit items above this to maintain consistency with tegra tools
    Always add to the end of this structure to retain backward compatibility */
} NV_EVENT_BUFFER_FECS_RECORD_V1;

// V2 ------------------------------------------------------------------------
typedef struct
{
    NvU8    tag;                            ///< NV_EVENT_BUFFER_FECS_CTXSWTAG_*
    NvU8    vmid;
    NvU16   seqno;                          ///< used to detect drop 
    NvU32   context_id;                     ///< channel/tsg handle 
    NvU32   pid;                            ///< process id
    NvU16   reserved0;
    NvU8    migGpuInstanceId;
    NvU8    migComputeInstanceId;
    NvU64   timestamp NV_ALIGN_BYTES(8);
    /* Do *not* edit items above this to maintain consistency with tegra tools
    Always add to the end of this structure to retain backward compatibility */
} NV_EVENT_BUFFER_FECS_RECORD_V2;

typedef NV_EVENT_BUFFER_FECS_RECORD_V1 NV_EVENT_BUFFER_FECS_RECORD_V0;
typedef NV_EVENT_BUFFER_FECS_RECORD_V1 NV_EVENT_BUFFER_FECS_RECORD;
#endif /* _cl90cdFecs_h_ */
