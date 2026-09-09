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

#include <nv_stddef.h>
#include <nvtypes.h>
#include <nvmisc.h>


#include "msgq/msgq.h"
#include "msgq/msgq_priv.h"


#include "nvport/nvport.h"
#include "core/core.h"

void osGpuWriteReg032(OBJGPU *pGpu, NvU32 thisAddress, NvV32 thisValue);
NvU32 osGpuReadReg032(OBJGPU *pGpu, NvU32 thisAddress);
#define WRITE_REG(gpu, addr, value) osGpuWriteReg032(gpu, addr, value)
#define READ_REG(gpu, addr) osGpuReadReg032(gpu, addr)

unsigned
msgqGetMetaSize(void)
{
    return sizeof(msgqMetadata);
}

void
msgqSetBackendRw(msgqHandle handle, msgqFcnBackendRw fcn, void *pArg)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    pQueue->fcnBackendRw = fcn;
    pQueue->fcnBackendRwArg = pArg;
}

/*
 *
 * Init and linking code
 *
 */

int msgqInit(msgqHandle *pHandle, void *pBuffer)
{
    msgqMetadata *pQueue = pBuffer;

    if (pQueue == NULL)
    {
        return -1;
    }

    portMemSet(pQueue, 0, sizeof *pQueue);

    if (pHandle != NULL)
    {
        *pHandle = pQueue;
    }
    return 0;
}

int
msgqTxCreate
(
    msgqHandle  handle,
    void       *pBackingStore,
    unsigned    size,
    unsigned    msgSize,
    unsigned    hdrAlign,
    unsigned    entryAlign,
    OBJGPU     *pGpu,
    NvU32       regHead,
    NvU32       regTail
)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;
    msgqTxHeader *pTx;
    int status;

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

    portMemSet(&pQueue->tx, 0, sizeof(pQueue->tx));

    pQueue->tx.entryOff = NV_ALIGN_UP(sizeof(msgqTxHeader), 1 << entryAlign);

    if (size < (pQueue->tx.entryOff + msgSize))
    {
        return -1;
    }

    // Fill in local copy of msgqTxHeader.
    pQueue->tx.versionMajor = MSGQ_VERSION_MAJOR;
    pQueue->tx.versionMinor = MSGQ_VERSION_MINOR;
    pQueue->tx.size     = size;
    pQueue->tx.msgSize  = msgSize;
    pQueue->tx.msgCount = (NvU32)((size - pQueue->tx.entryOff) / msgSize);

    // Write our tracking metadata
    pQueue->pOurTxHdr   = (msgqTxHeader*)pBackingStore;
    pQueue->pOurEntries = (NvU8*)pBackingStore + pQueue->tx.entryOff;
    pQueue->txLinked    = NV_TRUE;
    pQueue->txWritePtr  = 0;
    pQueue->rxAvail     = 0;

    // Allow adding queue messages before rx is linked.
    pQueue->txFree      = pQueue->tx.msgCount;

    pQueue->pGpu        = pGpu;
    pQueue->regTxHead   = regHead;
    pQueue->regTxTail   = regTail;

    // write shared buffer (backend)
    pTx = pQueue->pOurTxHdr;

    // Indirect access to backend
    if (pQueue->fcnBackendRw != NULL)
    {
        status = pQueue->fcnBackendRw(pTx, &pQueue->tx, sizeof *pTx,
                                      FCN_FLAG_BACKEND_ACCESS_WRITE | FCN_FLAG_BACKEND_QUEUE_TX,
                                      pQueue->fcnBackendRwArg);
        if (status != 0)
        {
            return -1;
        }
    }
    else
    {
        portMemCopy(pTx, sizeof(*pTx), &pQueue->tx, sizeof(pQueue->tx));
    }

    portAtomicMemoryFenceStore();
    WRITE_REG(pQueue->pGpu, pQueue->regTxHead, 0);
    portAtomicMemoryFenceFull();

    return 0;
}

int
msgqRxLink
(
    msgqHandle   handle,
    const void  *pBackingStore,
    unsigned     size,
    unsigned     msgSize,
    NvU32        regHead,
    NvU32        regTail
)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;
    int status;

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

    if (!pQueue->txLinked)
    {
        return -4;
    }

    if (pBackingStore == NULL)
    {
        return -5;
    }

    pQueue->pTheirTxHdr = (msgqTxHeader*)pBackingStore;

    // copy their metadata
    if (pQueue->fcnBackendRw != NULL)
    {
        status = pQueue->fcnBackendRw(&pQueue->rx, (const void *)pQueue->pTheirTxHdr,
                                      sizeof pQueue->rx,
                                      FCN_FLAG_BACKEND_ACCESS_READ | FCN_FLAG_BACKEND_QUEUE_RX,
                                      pQueue->fcnBackendRwArg);
        if (status != 0)
        {
            return -11;
        }
    }
    else
    {
        portMemCopy(&pQueue->rx, sizeof(pQueue->rx), (const void *)pQueue->pTheirTxHdr, sizeof(*pQueue->pTheirTxHdr));
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
    if (pQueue->rx.versionMajor != MSGQ_VERSION_MAJOR)
    {
        return -9;
    }

    if (pQueue->rx.msgCount != (NvU32)((size - pQueue->rx.entryOff) / msgSize))
    {
        return -10;
    }

    pQueue->pTheirEntries  = (NvU8*)pBackingStore + pQueue->rx.entryOff;
    pQueue->rxLinked       = NV_TRUE;

    pQueue->regRxHead      = regHead;
    pQueue->regRxTail      = regTail;

    pQueue->rxReadPtr = 0;
    portAtomicMemoryFenceStore();
    WRITE_REG(pQueue->pGpu, pQueue->regRxTail, 0);

    return 0;
}

/*
 *
 * Send code (outgoing messages)
 *
 */

unsigned
msgqTxGetFreeSpace(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return 0;
    }

    NvU32 newReadPtr = READ_REG(pQueue->pGpu, pQueue->regTxTail);
    portAtomicMemoryFenceLoad();

    //
    // It can happen that the register value gets reset to 0, notably during
    // suspend/resume cycles. Normally, this is not an issue as we can just
    // use the cached value, which might undercount the free space, but the
    // real value will be restored as soon as the peer consumes one message.
    // However, if the queue was empty at exactly the wrong time, restoring
    // the cached value will just make it look full, so in this case force
    // it back to empty. This was only observed on Turing.
    //
    if ((pQueue->txWritePtr - newReadPtr) > pQueue->tx.msgCount)
    {
        if (newReadPtr == 0 &&
            (pQueue->txWritePtr - pQueue->txReadPtr) == pQueue->tx.msgCount)
        {
            newReadPtr = pQueue->txWritePtr;
        }
        else
        {
            newReadPtr = pQueue->txReadPtr;
        }
    }

    pQueue->txReadPtr = newReadPtr;

    pQueue->txFree = pQueue->tx.msgCount - (pQueue->txWritePtr - pQueue->txReadPtr);

    return pQueue->txFree;
}

void *
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

    wp = (pQueue->txWritePtr + n) % pQueue->tx.msgCount;
    return pQueue->pOurEntries + (wp * pQueue->tx.msgSize);
}

int
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

    pQueue->txWritePtr += n;

    portAtomicMemoryFenceStore();
    WRITE_REG(pQueue->pGpu, pQueue->regTxHead, pQueue->txWritePtr);

    // Adjust cached value for number of free elements.
    pQueue->txFree -= n;

    return 0;
}

unsigned
msgqTxGetPending(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->txLinked)
    {
        return 0;
    }

    return pQueue->tx.msgCount - msgqTxGetFreeSpace(handle);
}

/*
 *
 * Receive code (incoming messages)
 *
 */

unsigned
msgqRxGetReadAvailable(msgqHandle handle)
{
    msgqMetadata *pQueue = (msgqMetadata*)handle;

    if ((pQueue == NULL) || !pQueue->rxLinked)
    {
        return 0;
    }

    NvU32 newWritePtr = READ_REG(pQueue->pGpu, pQueue->regRxHead);
    portAtomicMemoryFenceLoad();

    // See comment in msgqTxGetFreeSpace().
    if ((newWritePtr - pQueue->rxReadPtr) > pQueue->rx.msgCount)
    {
        if (newWritePtr == 0 &&
            (pQueue->rxWritePtr - pQueue->rxReadPtr) == pQueue->rx.msgCount)
        {
            newWritePtr = pQueue->rxReadPtr;
        }
        else
        {
            newWritePtr = pQueue->rxWritePtr;
        }
    }

    pQueue->rxWritePtr = newWritePtr;

    pQueue->rxAvail = pQueue->rxWritePtr - pQueue->rxReadPtr;

    return pQueue->rxAvail;
}

const void *
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
    // rxWritePtr, msgqRxGetReadAvailable can be a very costly operation.
    //
    if ((n >= pQueue->rxAvail) &&
        (n >= msgqRxGetReadAvailable(handle)))
    {
        return NULL;
    }

    rp = (pQueue->rxReadPtr + n) % pQueue->rx.msgCount;
    return pQueue->pTheirEntries + (rp * pQueue->rx.msgSize);
}

int
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

    portAtomicMemoryFenceStore();
    WRITE_REG(pQueue->pGpu, pQueue->regRxTail, pQueue->rxReadPtr);

    // Adjust cached value for number of available elements.
    pQueue->rxAvail -= n;

    return 0;
}
