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
    NvU8  authTagBuffer[16];         // Authentication tag buffer.
    NvU8  aadBuffer[16];             // AAD buffer.
    NvU32 checkSum;                  // Set to value needed to make checksum always zero.
    NvU32 seqNum;                    // Sequence number maintained by the message queue.
    NvU32 elemCount;                 // Number of message queue elements this message has.
    NV_DECLARE_ALIGNED(rpc_message_header_v rpc, 8);
} GSP_MSG_QUEUE_ELEMENT;

typedef struct _message_queue_info
{
    // Parameters
    NvLength               commandQueueSize;
    NvLength               statusQueueSize;

    // Shared memory area.
    void                  *pCommandQueue;
    void                  *pStatusQueue;
    rpc_message_header_v  *pRpcMsgBuf;    // RPC message buffer VA.

    // Other CPU-side fields
    void                  *pWorkArea;
    GSP_MSG_QUEUE_ELEMENT *pCmdQueueElement;    // Working copy of command queue element.
    void                  *pMetaData;
    msgqHandle             hQueue;              // Do not allow requests when hQueue is null.
    NvU32                  txSeqNum;            // Next sequence number for tx.
    NvU32                  rxSeqNum;            // Next sequence number for rx.
    NvU32                  txBufferFull;
    NvU32                  queueIdx;            // QueueIndex used to identify which task the message is supposed to be sent to.
} MESSAGE_QUEUE_INFO;

typedef struct MESSAGE_QUEUE_COLLECTION
{
    // Parameters
    NvLength               pageTableEntryCount;
    NvLength               pageTableSize;

    // Shared memory area.
    MEMORY_DESCRIPTOR     *pSharedMemDesc;
    RmPhysAddr             sharedMemPA;   // Page table for all of shared mem.

    MESSAGE_QUEUE_INFO rpcQueues[RPC_QUEUE_COUNT];
} MESSAGE_QUEUE_COLLECTION;

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

/*!
 * Calculate 32-bit checksum
 *
 * This routine assumes that the data is padded out with zeros to the next
 * 8-byte alignment, and it is OK to read past the end to the 8-byte alignment.
 */
static NV_INLINE NvU32 _checkSum32(void *pData, NvU32 uLen)
{
    NvU64 *p        = (NvU64 *)pData;
    NvU64 *pEnd     = (NvU64 *)((NvUPtr)pData + uLen);
    NvU64  checkSum = 0;

    NV_ASSERT_CHECKED(uLen > 0);

    while (p < pEnd)
        checkSum ^= *p++;

    return NvU64_HI32(checkSum) ^ NvU64_LO32(checkSum);
}

#endif // _MESSAGE_QUEUE_PRIV_H_
