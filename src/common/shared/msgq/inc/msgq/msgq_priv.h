/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * Private header for message queues
 * Should not be used except for (direct) higher layer of messaging (rm, rpc)
 * Keep structures padded to 64bytes (to avoid cache issues)
 */

#ifndef MSGQ_PRIV_H
#define MSGQ_PRIV_H

#include <nvtypes.h>

#include "msgq.h"

// Version, gets increased with incompatible changes.
#define MSGQ_VERSION 0

/*
 * (Shared) queue Layout:
 * TX header (padded to cache line)
 * RX header (padded to cache line)
 * Ring buffer of messages
 * <possibly some leftover memory>
 */

// buffer metadata, written by source, at start of block
typedef struct
{
    NvU32 version;   // queue version
    NvU32 size;      // bytes, page aligned
    NvU32 msgSize;   // entry size, bytes, must be power-of-2, 16 is minimum
    NvU32 msgCount;  // number of entries in queue
    NvU32 writePtr;  // message id of next slot
    NvU32 flags;     // if set it means "i want to swap RX"
    NvU32 rxHdrOff;  // Offset of msgqRxHeader from start of backing store.
    NvU32 entryOff;  // Offset of entries from start of backing store.
} msgqTxHeader;

// buffer metadata, written by sink
typedef struct
{
    NvU32 readPtr; // message id of last message read
} msgqRxHeader;

// Internal tracking structure (handle)
typedef struct
{
    // Those are also bases of buffers; our / their means belonging to our/their buffer
    msgqTxHeader                *pOurTxHdr;
    volatile const msgqTxHeader *pTheirTxHdr;
    msgqRxHeader                *pOurRxHdr;    // Can't set either RxHdr to volatile
    msgqRxHeader                *pTheirRxHdr;  //   const due to MSGQ_FLAGS_SWAP_RX.

    NvU8        *pOurEntries;   // first tx entry
    const NvU8  *pTheirEntries; // first rx entry

    // To simplify things - those elements are at *destination* surfaces
    volatile const NvU32 *pReadIncoming;   // rx we read, they write
    volatile const NvU32 *pWriteIncoming;  // tx we read, they write
    NvU32                *pReadOutgoing;   // rx we write they read
    NvU32                *pWriteOutgoing;  // tx we write they read

    // tx == our
    msgqTxHeader tx;
    NvU32        txReadPtr;    // Local cache for pQueue->pReadIncoming.
    NvU32        txFree;       // Cached copy of msgqTxGetFreeSpace.
    NvBool       txLinked;

    // rx == theirs
    msgqTxHeader rx;
    NvU32        rxReadPtr;    // Local cache for pQueue->pReadOutgoing.
    NvU32        rxAvail;      // Cached copy of msgqRxGetReadAvailable.
    NvBool       rxLinked;

    // swap rx backing store
    NvBool       rxSwapped;

    // notifications
    msgqFcnNotifyRemote   fcnNotify;
    void                 *fcnNotifyArg;
    msgqFcnBackendRw      fcnBackendRw;
    void                 *fcnBackendRwArg;
    msgqFcnCacheOp        fcnInvalidate;
    msgqFcnCacheOp        fcnFlush;
    msgqFcnCacheOp        fcnZero;
    msgqFcnBarrier        fcnBarrier;
} msgqMetadata;

#endif // MSGQ_PRIV_H
