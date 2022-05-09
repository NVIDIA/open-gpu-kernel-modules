/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * GSP MESSAGE QUEUE
 */

#ifndef _MESSAGE_QUEUE_PRIV_H_
#define _MESSAGE_QUEUE_PRIV_H_

#include "msgq/msgq.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

// Shared memory layout.
//
// Each of the following are page aligned:
//   Page table for entire shared memory layout.
//   Command queue header
//   Command queue entries
//   Status queue header
//   Status queue entries

typedef struct GSP_MSG_QUEUE_ELEMENT
{
    NvU32 checkSum;        // Set to value needed to make checksum always zero.
    NvU32 seqNum;          // Sequence number maintained by the message queue.
    rpc_message_header_v  rpc;
} GSP_MSG_QUEUE_ELEMENT;

typedef struct _message_queue_info
{
    // Parameters
    NvLength               pageTableEntryCount;
    NvLength               pageTableSize;
    NvLength               commandQueueSize;
    NvLength               statusQueueSize;

    // Shared memory area.
    MEMORY_DESCRIPTOR     *pSharedMemDesc;
    RmPhysAddr             sharedMemPA;   // Page table for all of shared mem.
    void                  *pCommandQueue;
    void                  *pStatusQueue;
    rpc_message_header_v  *pRpcMsgBuf;    // RPC message buffer VA.

    void                  *pInitMsgBuf;   // RPC message buffer VA.
    RmPhysAddr             initMsgBufPA;  // RPC message buffer PA.

    // Other CPU-side fields
    void                  *pWorkArea;
    GSP_MSG_QUEUE_ELEMENT *pCmdQueueElement;    // Working copy of command queue element.
    void                  *pMetaData;
    msgqHandle             hQueue;              // Do not allow requests when hQueue is null.
    NvU32                  txSeqNum;            // Next sequence number for tx.
    NvU32                  rxSeqNum;            // Next sequence number for rx.
} MESSAGE_QUEUE_INFO;

//
// Most of the following defines resolve to compile-time constants.
//
#define GSP_MSG_QUEUE_ELEMENT_SIZE_MIN                              RM_PAGE_SIZE
#define GSP_MSG_QUEUE_ELEMENT_SIZE_MAX     (GSP_MSG_QUEUE_ELEMENT_SIZE_MIN * 16)
#define GSP_MSG_QUEUE_ELEMENT_HDR_SIZE   NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, rpc)

#define GSP_MSG_QUEUE_RPC_SIZE_MAX                                             \
    (GSP_MSG_QUEUE_ELEMENT_SIZE_MAX - GSP_MSG_QUEUE_ELEMENT_HDR_SIZE)

#define GSP_MSG_QUEUE_BYTES_TO_ELEMENTS(b)                                     \
        NV_DIV_AND_CEIL(b, GSP_MSG_QUEUE_ELEMENT_SIZE_MIN)

#define GSP_MSG_QUEUE_ALIGN                                        RM_PAGE_SHIFT   // 2 ^ 12 = 4096
#define GSP_MSG_QUEUE_ELEMENT_ALIGN                                RM_PAGE_SHIFT   // 2 ^ 12 = 4096
#define GSP_MSG_QUEUE_HEADER_SIZE                                   RM_PAGE_SIZE
#define GSP_MSG_QUEUE_HEADER_ALIGN                                             4   // 2 ^ 4 = 16

#endif // _MESSAGE_QUEUE_PRIV_H_
