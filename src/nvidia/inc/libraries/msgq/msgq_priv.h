/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef MSGQ_PRIV_H
#define MSGQ_PRIV_H

#include <nvtypes.h>

#include "msgq.h"

// Major versions get increased with incompatible changes
#define MSGQ_VERSION_MAJOR 2
// Minor versions are compatible
#define MSGQ_VERSION_MINOR 0


/*
 * NVIDIA MSGQ Protocol, version 2.0
 *
 * A single-producer, single-consumer ring buffer message queue for
 * communication between CPU and GSP.
 *
 * Each direction of communication uses a separate unidirectional queue.
 * A bidirectional channel consists of two queues with roles swapped:
 * one side's TX queue is the other side's RX queue.
 *
 * Backing store layout (owned and initialized by the TX side):
 *   [msgqTxHeader]  Header with version, sizes, and entry offset
 *   [pad]           Padding to entry alignment
 *   [entry 0]
 *   [entry 1]
 *   ...             msgCount entries, each msgSize bytes
 *   [entry N]
 *
 * Synchronization:
 *   Head/tail pointers are exchanged via hardware registers. Typically:
 *      NV_PGSP_QUEUE_HEAD(i) -> TX head (CPU), RX head (GSP)
 *      NV_PGSP_QUEUE_TAIL(i) -> TX tail (CPU), RX tail (GSP)
 *      NV_PGSP_MSGQ_HEAD(i)  -> RX head (CPU), TX head (GSP)
 *      NV_PGSP_MSGQ_TAIL(i)  -> RX tail (CPU), TX tail (GSP)
 *   Each register holds a monotonic counter, that is used modulo msgCount.
 *     Buffer is full when (writePtr - readPtr == msgCount)
 *     Buffer is empty when (writePtr == readPtr)
 *
 * TX (send) flow:
 *   1. msgqTxGetWriteBuffer(n) - get pointer to slot (writePtr + n) % msgCount
 *   2. Write message payload into returned buffer
 *   3. msgqTxSubmitBuffers(n)  - advance writePtr by n, write to TX_HEAD
 *
 * RX (receive) flow:
 *   1. msgqRxGetReadBuffer(n)  - get pointer to slot (readPtr + n) % msgCount
 *   2. Read message payload from returned buffer
 *   3. msgqRxMarkConsumed(n)   - advance readPtr by n, write to RX_TAIL
 *
 * The protocol itself does not define how to notify the receiver that a new
 * message is available; however, writing to NV_PGSP_QUEUE_HEAD will trigger
 * a hardware interrupt on GSP which does this.
 */


// Queue header, written to the start of the backing store by the TX side.
typedef struct NV_ABI_STABLE msgqTxHeader
{
    NvU16 versionMajor; // MSGQ_VERSION_MAJOR
    NvU16 versionMinor; // MSGQ_VERSION_MINOR
    NvU32 size;         // Total backing store size in bytes
    NvU32 msgSize;      // Entry size in bytes (minimum MSGQ_MSG_SIZE_MIN)
    NvU32 msgCount;     // Number of entries that fit in the ring buffer
    NvU32 entryOff;     // Byte offset of first entry from start of backing store
    NvU32 reserved[3];  // Reserved for future use. Init to zero
} msgqTxHeader;

// Internal tracking structure (handle)
typedef struct
{
    msgqTxHeader                *pOurTxHdr;
    volatile const msgqTxHeader *pTheirTxHdr;

    NvU8        *pOurEntries;   // first tx entry
    const NvU8  *pTheirEntries; // first rx entry

    // Hardware register addresses for head/tail pointers
    OBJGPU *pGpu;
    NvU32 regTxHead;    // TX write pointer (written by us)
    NvU32 regTxTail;    // TX read pointer  (written by remote)
    NvU32 regRxHead;    // RX write pointer (written by remote)
    NvU32 regRxTail;    // RX read pointer  (written by us)

    // tx == our
    msgqTxHeader tx;
    NvU32        txWritePtr;   // Monotonic write counter (slot = txWritePtr % tx.msgCount)
    NvU32        txReadPtr;    // Cached value read from regTxTail
    NvU32        txFree;       // Cached free slot count (may undercount)
    NvBool       txLinked;

    // rx == theirs
    msgqTxHeader rx;
    NvU32        rxWritePtr;   // Cached value read from regRxHead
    NvU32        rxReadPtr;    // Our read position, written to regRxTail
    NvU32        rxAvail;      // Cached available message count (may undercount)
    NvBool       rxLinked;

    msgqFcnBackendRw      fcnBackendRw;
    void                 *fcnBackendRwArg;
} msgqMetadata;

#endif // MSGQ_PRIV_H
