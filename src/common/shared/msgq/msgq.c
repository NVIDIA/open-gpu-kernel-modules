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

#include <stddef.h>
#include <nvtypes.h>
#include <nvmisc.h>

#if defined(UPROC_RISCV) && !defined(NVRM)
#include "msgq/msgq.h"
#include "msgq/msgq_priv.h"
#include <sections.h>
#include <shared.h>
#else // defined(UPROC_RISCV) && !defined(NVRM)
#include "inc/msgq/msgq.h"
#include "inc/msgq/msgq_priv.h"
/* This is because this code will be shared with CPU */
#define sysSHARED_CODE
// MK TODO: we should have unified memset/memcpy interface at some point
#if PORT_MODULE_memory
#include "nvport/nvport.h"
#define memcpy(d,s,l)  portMemCopy(d,l,s,l)
#define memset         portMemSet
#else  // PORT_MODULE_memory
#include <memory.h>
#endif // PORT_MODULE_memory
#endif // defined(UPROC_RISCV) && !defined(NVRM)

sysSHARED_CODE unsigned
msgqGetMetaSize(void)
{
    return sizeof(msgqMetadata);
}

sysSHARED_CODE void
msgqSetNotification(msgqHandle handle, msgqFcnNotifyRemote fcn, void *pArg)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnNotify = fcn;
    pQueue->fcnNotifyArg = pArg;
}

sysSHARED_CODE void
msgqSetBackendRw(msgqHandle handle, msgqFcnBackendRw fcn, void *pArg)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnBackendRw = fcn;
    pQueue->fcnBackendRwArg = pArg;
}

sysSHARED_CODE void
msgqSetRxInvalidate(msgqHandle handle, msgqFcnCacheOp fcn)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnInvalidate = fcn;
}

sysSHARED_CODE void
msgqSetTxFlush(msgqHandle handle, msgqFcnCacheOp fcn)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnFlush = fcn;
}

sysSHARED_CODE void
msgqSetZero(msgqHandle handle, msgqFcnCacheOp fcn)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnZero = fcn;
}

sysSHARED_CODE void
msgqSetBarrier(msgqHandle handle, msgqFcnBarrier fcn)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnBarrier = fcn;
}

/*
 * Helper functions to access indirect backend.
 */

sysSHARED_CODE static void
_backendRead32(msgqMetadata *pQueue, volatile const void *pAddr, NvU32 *pVal, unsigned flags)
{
    if (pQueue->fcnBackendRw != NULL)
    {
        pQueue->fcnBackendRw(pVal, (const void *)pAddr, sizeof(*pVal),
                             flags | FCN_FLAG_BACKEND_ACCESS_READ,
                             pQueue->fcnBackendRwArg);
    }
    else
    {
        *pVal = *(volatile const NvU32*)pAddr;
    }
}

sysSHARED_CODE static void
_backendWrite32(msgqMetadata *pQueue, volatile void *pAddr, NvU32 *pVal, unsigned flags)
{
    if (pQueue->fcnBackendRw != NULL)
    {
        pQueue->fcnBackendRw((void*)pAddr, pVal, sizeof(*pVal),
                             flags | FCN_FLAG_BACKEND_ACCESS_WRITE,
                             pQueue->fcnBackendRwArg);
    }
    else
    {
        *(volatile NvU32*)pAddr = *pVal;
    }
}

/**
 * @brief Default barrier for (RISC-V) systems.
 */
#ifdef UPROC_RISCV
sysSHARED_CODE static void
msgqRiscvDefaultBarrier(void)
{
    asm volatile("fence iorw,iorw");
}
#endif

/*
 *
 * Init and linking code
 *
 */

sysSHARED_CODE int msgqInit(msgqHandle *pHandle, void *pBuffer)
{
    msgqMetadata *pQueue = pBuffer;

    if (pQueue == NULL)
    {
        return -1;
    }

    memset(pQueue, 0, sizeof *pQueue);

#ifdef UPROC_RISCV
    pQueue->fcnBarrier = msgqRiscvDefaultBarrier;
#endif

    if (pHandle != NULL)
    {
        *pHandle = pQueue;
    }
    return 0;
}

sysSHARED_CODE int
msgqTxCreate
(
    msgqHandle  handle,
    void       *pBackingStore,
    unsigned    size,
    unsigned    msgSize,
    unsigned    hdrAlign,
    unsigned    entryAlign,
    unsigned    flags
)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;
    msgqTxHeader *pTx;

    if ((pQueue == NULL) || pQueue->txLinked)
    {
        return -1;
    }

    if ((msgSize < MSGQ_MSG_SIZE_MIN) || (msgSize > size))
    {
        return -1;
    }

    // Check that alignments are in range.
    if ((hdrAlign < MSGQ_META_MIN_ALIGN)     ||
        (hdrAlign > MSGQ_META_MAX_ALIGN))
    {
        return -1;
    }

    if ((entryAlign < MSGQ_META_MIN_ALIGN)   ||
        (entryAlign > MSGQ_META_MAX_ALIGN))
    {
        return -1;
    }

    if (pBackingStore == NULL)
    {
        return -1;
    }

    // Make sure backing store is aligned for hdrAlign.
    if ((NvUPtr)pBackingStore & ((hdrAlign - 1)))
    {
        return -1;
    }

    pQueue->tx.rxHdrOff = NV_ALIGN_UP(sizeof(msgqTxHeader), 1 << hdrAlign);
    pQueue->tx.entryOff = NV_ALIGN_UP(pQueue->tx.rxHdrOff + sizeof(msgqRxHeader),
                              1 << entryAlign);

    if (size < (pQueue->tx.entryOff + msgSize))
    {
        return -1;
    }

    // Fill in local copy of msgqTxHeader.
    pQueue->tx.version  = MSGQ_VERSION;
    pQueue->tx.size     = size;
    pQueue->tx.msgSize  = msgSize;
    pQueue->tx.writePtr = 0;
    pQueue->tx.flags    = flags;
    pQueue->tx.msgCount = (NvU32)((size - pQueue->tx.entryOff) / msgSize);

    // Write our tracking metadata
    pQueue->pOurTxHdr   = (msgqTxHeader*)pBackingStore;
    pQueue->pOurRxHdr   = (msgqRxHeader*)((NvU8*)pBackingStore + pQueue->tx.rxHdrOff);
    pQueue->pOurEntries = (NvU8*)pBackingStore + pQueue->tx.entryOff;
    pQueue->txLinked    = NV_TRUE;
    pQueue->rxAvail     = 0;

    // Allow adding queue messages before rx is linked.
    pQueue->txFree      = pQueue->tx.msgCount - 1;

    // Swap only if both sides agree on it
    pQueue->rxSwapped = (flags & MSGQ_FLAGS_SWAP_RX) &&
                        (pQueue->rx.flags & MSGQ_FLAGS_SWAP_RX);

    pQueue->pWriteOutgoing = &pQueue->pOurTxHdr->writePtr;

    // if set, other side is already linked
    if (pQueue->rxSwapped)
    {
        pQueue->pReadOutgoing = &pQueue->pOurRxHdr->readPtr;
        pQueue->pReadIncoming = &pQueue->pTheirRxHdr->readPtr;
    }
    else
    {
        pQueue->pReadIncoming = &pQueue->pOurRxHdr->readPtr;
        if (pQueue->rxLinked)
        {
            pQueue->pReadOutgoing = &pQueue->pTheirRxHdr->readPtr;
        }
    }

    // write shared buffer (backend)
    pTx = pQueue->pOurTxHdr;

    if (pQueue->fcnZero != NULL)
    {
        pQueue->fcnZero(pTx, sizeof *pTx);
    }

    // Indirect access to backend
    if (pQueue->fcnBackendRw != NULL)
    {
        pQueue->fcnBackendRw(pTx, &pQueue->tx, sizeof *pTx,
            FCN_FLAG_BACKEND_ACCESS_WRITE | FCN_FLAG_BACKEND_QUEUE_TX,
            pQueue->fcnBackendRwArg);
    } else
    {
        memcpy(pTx, &pQueue->tx, sizeof *pTx);
    }

    // Flush
    if (pQueue->fcnFlush != NULL)
    {
        pQueue->fcnFlush(pTx, sizeof *pTx);
    }

    // Barrier
    if (pQueue->fcnBarrier != NULL)
    {
        pQueue->fcnBarrier();
    }

    // Notify that pQueue was created
    if (pQueue->fcnNotify != NULL)
    {
        pQueue->fcnNotify(FCN_FLAG_NOTIFY_MSG_WRITE, pQueue->fcnNotifyArg);
    }

    return 0;
}

sysSHARED_CODE int
msgqRxLink(msgqHandle handle, const void *pBackingStore, unsigned size, unsigned msgSize)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || pQueue->rxLinked)
    {
        return -1;
    }

    if (msgSize < MSGQ_MSG_SIZE_MIN)
    {
        return -2;
    }

    if (msgSize > size)
    {
        return -3;
    }

    if (pBackingStore == NULL)
    {
        return -5;
    }

    pQueue->pTheirTxHdr = (msgqTxHeader*)pBackingStore;

    // Invalidate
    if (pQueue->fcnInvalidate != NULL)
    {
        pQueue->fcnInvalidate(pQueue->pTheirTxHdr, sizeof(msgqTxHeader));
    }

    // copy their metadata
    if (pQueue->fcnBackendRw != NULL)
    {
        pQueue->fcnBackendRw(&pQueue->rx, (const void *)pQueue->pTheirTxHdr,
            sizeof pQueue->rx,
            FCN_FLAG_BACKEND_ACCESS_READ | FCN_FLAG_BACKEND_QUEUE_RX,
            pQueue->fcnBackendRwArg);
    }
    else
    {
        memcpy(&pQueue->rx, (const void *)pQueue->pTheirTxHdr, sizeof pQueue->rx);
    }

    if (size < (pQueue->rx.entryOff + msgSize))
    {
        return -6;
    }

    // Sanity check
    if (pQueue->rx.size != size)
    {
        return -7;
    }
    if (pQueue->rx.msgSize != msgSize)
    {
        return -8;
    }
    if (pQueue->rx.version != MSGQ_VERSION)
    {
        return -9;
    }

    // Also check the calculated fields, to make sure the header arrived intact.
    if ((pQueue->rx.rxHdrOff < sizeof(msgqTxHeader))                          ||
        (pQueue->rx.entryOff < pQueue->tx.rxHdrOff + sizeof(msgqRxHeader))    ||
        (pQueue->rx.msgCount != (NvU32)((size - pQueue->rx.entryOff) / msgSize)))
    {
        return -10;
    }

    pQueue->pTheirRxHdr    = (msgqRxHeader*)((NvU8*)pBackingStore + pQueue->rx.rxHdrOff);
    pQueue->pTheirEntries  = (NvU8*)pBackingStore + pQueue->rx.entryOff;

    pQueue->rxLinked       = NV_TRUE;
    pQueue->rxSwapped      = (pQueue->tx.flags & MSGQ_FLAGS_SWAP_RX) &&
                             (pQueue->rx.flags & MSGQ_FLAGS_SWAP_RX);
    pQueue->pWriteIncoming = &pQueue->pTheirTxHdr->writePtr;

    // if set, other side is always linked
    if (pQueue->rxSwapped)
    {
        pQueue->pReadOutgoing = &pQueue->pOurRxHdr->readPtr;
        pQueue->pReadIncoming = &pQueue->pTheirRxHdr->readPtr;
    }
    else // may be unidir
    {
        pQueue->pReadOutgoing = &pQueue->pTheirRxHdr->readPtr;
        if (pQueue->txLinked)
        {
            pQueue->pReadIncoming = &pQueue->pOurRxHdr->readPtr;
        }
    }

    if (pQueue->fcnZero != NULL)
    {
        pQueue->fcnZero(pQueue->pReadOutgoing, sizeof(NvU32));
    }

    pQueue->rxReadPtr = 0;
    _backendWrite32(pQueue, pQueue->pReadOutgoing, &pQueue->rxReadPtr,
        pQueue->rxSwapped ? FCN_FLAG_BACKEND_QUEUE_TX : FCN_FLAG_BACKEND_QUEUE_RX);
    if (pQueue->fcnFlush != NULL)
    {
        pQueue->fcnFlush(pQueue->pReadOutgoing, sizeof(NvU32));
    }

    // Barrier, notify
    if (pQueue->fcnBarrier != NULL)
    {
        pQueue->fcnBarrier();
    }

    // Notify that pQueue was created
    if (pQueue->fcnNotify != NULL)
    {
        pQueue->fcnNotify(FCN_FLAG_NOTIFY_MSG_READ, pQueue->fcnNotifyArg);
    }

    return 0;
}

/*
 *
 * Send code (outgoing messages)
 *
 */

sysSHARED_CODE unsigned
msgqTxGetFreeSpace(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return 0;
    }

    _backendRead32(pQueue, pQueue->pReadIncoming, &pQueue->txReadPtr,
        pQueue->rxSwapped ? FCN_FLAG_BACKEND_QUEUE_RX : FCN_FLAG_BACKEND_QUEUE_TX);
    if (pQueue->txReadPtr >= pQueue->tx.msgCount)
    {
        return 0;
    }

    pQueue->txFree = pQueue->txReadPtr + pQueue->tx.msgCount - pQueue->tx.writePtr - 1;

    // Avoid % operator due to performance issues on RISC-V.
    if (pQueue->txFree >= pQueue->tx.msgCount)
    {
        pQueue->txFree -= pQueue->tx.msgCount;
    }

    return pQueue->txFree;
}

sysSHARED_CODE void *
msgqTxGetWriteBuffer(msgqHandle handle, unsigned n)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;
    NvU32 wp;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return NULL;
    }

    //
    // Look at the cached free space first.  If the cached value shows enough
    // remaining free space from last time, there is no reason to read and
    // calculate the free space again.  Depending on the location of txReadPtr,
    // msgqTxGetFreeSpace can be a very costly operation.
    //
    if ((n >= pQueue->txFree) &&
        (n >= msgqTxGetFreeSpace(handle)))
    {
        return NULL;
    }

    wp = pQueue->tx.writePtr + n;
    if (wp >= pQueue->tx.msgCount)
    {
        wp -= pQueue->tx.msgCount;
    }

    return pQueue->pOurEntries + (wp * pQueue->tx.msgSize);
}

sysSHARED_CODE int
msgqTxSubmitBuffers(msgqHandle handle, unsigned n)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return -1;
    }

    if ((n > pQueue->txFree) &&
        (n > msgqTxGetFreeSpace(handle)))
    {
        return -1;
    }

    // flush queues - TODO: make it more precise possibly
    if (pQueue->fcnFlush != NULL)
    {
        pQueue->fcnFlush(pQueue->pOurEntries,
                         pQueue->tx.msgCount * pQueue->tx.msgSize);
    }

    // write pointer
    pQueue->tx.writePtr += n;
    if (pQueue->tx.writePtr >= pQueue->tx.msgCount)
    {
        pQueue->tx.writePtr -= pQueue->tx.msgCount;
    }

    _backendWrite32(pQueue, pQueue->pWriteOutgoing,
        &pQueue->tx.writePtr, FCN_FLAG_BACKEND_QUEUE_TX);

    // Adjust cached value for number of free elements.
    pQueue->txFree -= n;

    // flush tx header
    if (pQueue->fcnFlush != NULL)
    {
        pQueue->fcnFlush(pQueue->pWriteOutgoing, sizeof(NvU32));
    }

    // barrier
    if (pQueue->fcnBarrier != NULL)
    {
        pQueue->fcnBarrier();
    }

    // Send notification
    if (pQueue->fcnNotify != NULL)
    {
        pQueue->fcnNotify(FCN_FLAG_NOTIFY_MSG_WRITE, pQueue->fcnNotifyArg);
    }

    return 0;
}

sysSHARED_CODE int
msgqTxSync(msgqHandle handle) // "transmit"
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return -1;
    }

    if (pQueue->fcnInvalidate != NULL)
    {
        // Invalidate caches for read / write pointers
        pQueue->fcnInvalidate((void*)pQueue->pReadIncoming, sizeof(NvU32));
    }

    return msgqTxGetFreeSpace(handle);
}

sysSHARED_CODE unsigned
msgqTxGetPending(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return 0;
    }

    return pQueue->tx.msgCount - msgqTxSync(handle) - 1;
}

/*
 *
 * Receive code (incoming messages)
 *
 */

sysSHARED_CODE unsigned
msgqRxGetReadAvailable(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->rxLinked)
    {
        return 0;
    }

    _backendRead32(pQueue, pQueue->pWriteIncoming, &pQueue->rx.writePtr, FCN_FLAG_BACKEND_QUEUE_RX);
    if (pQueue->rx.writePtr >= pQueue->rx.msgCount)
    {
        return 0;
    }

    pQueue->rxAvail = pQueue->rx.writePtr + pQueue->rx.msgCount - pQueue->rxReadPtr;

    // Avoid % operator due to performance issues on RISC-V.
    if (pQueue->rxAvail >= pQueue->rx.msgCount)
    {
        pQueue->rxAvail -= pQueue->rx.msgCount;
    }

    return pQueue->rxAvail;
}

sysSHARED_CODE const void *
msgqRxGetReadBuffer(msgqHandle handle, unsigned n)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;
    NvU32 rp;

    if ((pQueue == NULL) || !pQueue->rxLinked)
    {
        return NULL;
    }

    //
    // Look at the cached elements available first.  If the cached value shows
    // enough elements available from last time, there is no reason to read and
    // calculate the elements available again.  Depending on the location of
    // rx.writePtr, msgqRxGetReadAvailable can be a very costly operation.
    //
    if ((n >= pQueue->rxAvail) &&
        (n >= msgqRxGetReadAvailable(handle)))
    {
        return NULL;
    }

    rp = pQueue->rxReadPtr + n;
    if (rp >= pQueue->rx.msgCount)
    {
        rp -= pQueue->rx.msgCount;
    }

    return pQueue->pTheirEntries + (rp * pQueue->rx.msgSize);
}

sysSHARED_CODE int
msgqRxMarkConsumed(msgqHandle handle, unsigned n)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->rxLinked)
    {
        return -1;
    }

    if ((n > pQueue->rxAvail) &&
        (n > msgqRxGetReadAvailable(handle)))
    {
        return -1;
    }

    // read pointer
    pQueue->rxReadPtr += n;
    if (pQueue->rxReadPtr >= pQueue->rx.msgCount)
    {
        pQueue->rxReadPtr -= pQueue->rx.msgCount;
    }

    // Copy to backend
    _backendWrite32(pQueue, pQueue->pReadOutgoing, &pQueue->rxReadPtr,
        pQueue->rxSwapped ? FCN_FLAG_BACKEND_QUEUE_TX : FCN_FLAG_BACKEND_QUEUE_RX);

    // Adjust cached value for number of available elements.
    pQueue->rxAvail -= n;

    // flush rx header
    if (pQueue->fcnFlush != NULL)
    {
        pQueue->fcnFlush(pQueue->pReadOutgoing, sizeof(NvU32));
    }

    // barrier
    if (pQueue->fcnBarrier != NULL)
    {
        pQueue->fcnBarrier();
    }

    // Send notification
    if (pQueue->fcnNotify != NULL)
    {
        pQueue->fcnNotify(FCN_FLAG_NOTIFY_MSG_READ, pQueue->fcnNotifyArg);
    }

    return 0;
}

sysSHARED_CODE int
msgqRxSync(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->rxLinked)
    {
        return -1;
    }

    // flush queues - TODO: make it more precise :)
    if (pQueue->fcnInvalidate != NULL)
    {
        pQueue->fcnInvalidate(pQueue->pTheirEntries,
                              pQueue->rx.msgCount * pQueue->rx.msgSize);
        // Invalidate caches for read / write pointers
        pQueue->fcnInvalidate((void*)pQueue->pWriteIncoming, sizeof(NvU32));
    }

    return msgqRxGetReadAvailable(handle);
}
