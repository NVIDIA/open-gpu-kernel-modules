/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef MSGQ_H
#define MSGQ_H

// Handle used to refer to queues.
typedef void *msgqHandle;

// Minimal size of message
#define MSGQ_MSG_SIZE_MIN           16

// Minimal alignment of metadata, must be power of 2
#define MSGQ_META_MIN_ALIGN        3U // 2^3 = 8
#define MSGQ_META_MAX_ALIGN       12U // 4096.  Used to sanity-check alignment
                                      // parameters.  Increase if needed.
// If set we swap read pointers (for bidirectional communication).
// That way each peers can have their "receive" channel mapped RO
#define MSGQ_FLAGS_SWAP_RX          1

#define FCN_FLAG_NOTIFY_MSG_WRITE   0
#define FCN_FLAG_NOTIFY_MSG_READ    1

// msgqFcnBackendRw flags
#define FCN_FLAG_BACKEND_ACCESS_MASK       0x0001
#define FCN_FLAG_BACKEND_ACCESS_READ       0x0000
#define FCN_FLAG_BACKEND_ACCESS_WRITE      0x0001

#define FCN_FLAG_BACKEND_QUEUE_MASK        0x0100
#define FCN_FLAG_BACKEND_QUEUE_RX          0x0000  // Access rx queue backing store
#define FCN_FLAG_BACKEND_QUEUE_TX          0x0100  // Access tx queue backing store

/*
 * Hook functions. In future it should be possible to replace them (as an
 * option) with compile time macros.
 */

// Notify other peer that queue state change.
// Should return 0 on success.
typedef int (*msgqFcnNotifyRemote)(int isRead, void *pArg);

// Generic cache operation function (may be flush, zero, invalidate)
typedef void (*msgqFcnCacheOp)(const volatile void *pAddr, unsigned size);

// Generic barrier
typedef void (*msgqFcnBarrier)(void);

// Function to access backend memory (if it's not memory mapped).
// Keep in mind than when using it, pointers given by peek can't be trusted
// Should return 0 on success.
typedef int (*msgqFcnBackendRw)(void *pDest, const void *pSrc, unsigned size,
                                unsigned flags, void *pArg);

/**
 * @brief Return size of metadata (that must be allocated)
 */
unsigned msgqGetMetaSize(void);

/**
 * @brief Create queue object.
 * @param handle Pointer to handle.
 * @param metaBuf Pre-allocated buffer. It's size must be at least
 * msgqGetMetaSize() bytes, and must be MSGQ_META_MIN_ALIGN aligned.
 * @return 0 on success.
 */
int msgqInit(msgqHandle *pHandle, void *pBuffer);

/*
 * Queue configuration. Should be done after Init, before linking (unless you
 * know what you're doing).
 */

void msgqSetNotification(msgqHandle handle, msgqFcnNotifyRemote fcn, void *pArg);
void msgqSetBackendRw(msgqHandle handle, msgqFcnBackendRw fcn, void *pArg);
void msgqSetRxInvalidate(msgqHandle handle, msgqFcnCacheOp fcn);
void msgqSetTxFlush(msgqHandle handle, msgqFcnCacheOp fcn);
void msgqSetZero(msgqHandle handle, msgqFcnCacheOp fcn);
void msgqSetBarrier(msgqHandle handle, msgqFcnBarrier fcn);

/**
 * @brief Creates outgoing queue. That includes initializing of backend.
 * @param handle queue (must be already initialized with msgqInit())
 * @param pBackingStore memory buffer (or cookie if backendRW is used). As a
 * general rule it must be mapped RW to us.
 * @param size Size of buffer (in bytes).
 * @param msgSize Size of message (in bytes).
 * @param hdrAlign Alignment of header (2^n).
 * @param entryAlign Alignment of entry (2^n).
 * @param flags For now only SWAP_RX is to be used.
 * @return 0 on success.
 *
 * After TX was created, optional notification callback is executed.
 */
int msgqTxCreate(msgqHandle handle, void *pBackingStore, unsigned size, unsigned msgSize,
                 unsigned hdrAlign, unsigned entryAlign, unsigned flags);

/**
 * @brief Links into RX buffer (initialized by third party).
 * @param handle queue (must be already initialized with msgqInit())
 * @param pBackingStore memory buffer (or cookie if backendRW is used). As a
 * general rule it must be mapped RW to us. With SWAP_RX it can be RO.
 * @param size Size of buffer (in bytes)
 * @param msgSize Size of message (in bytes)
 * @return 0 on success
 * Note that msgSize and size are there only to do sanity check. Backing store
 * must be already initialized.
 *
 * After link is estabilished, optional notification callback is executed.
 */
int msgqRxLink(msgqHandle handle, const void *pBackingStore, unsigned size,
               unsigned msgSize);

/**
 * @brief Get number of free out messages.
 * @param handle
 * @return 0 if outgoing queue is full or not initialized
 *
 * WARING: This function doesn NOT flush caches. It's fast, but in reality
 * there may be more space than is reported.
 */
unsigned msgqTxGetFreeSpace(msgqHandle handle);

/**
 * @brief Get pointer to "out" message
 * @param handle
 * @param n number of message (0..msgqTxGetFreeSpace()-1)
 * @return Writable memory
 *
 * WARNING: If memory cookie is used, returned pointer can't be accessed
 * directly, wrappers must be used.
 * Note that it should be not assumed that pointers returned for n=0 and n=1
 * are continuous.
 * This function is cheap and should be used for batch submissions.
 */
void *msgqTxGetWriteBuffer(msgqHandle handle, unsigned n);

/**
 * @brief Submits message(s)
 * @param handle
 * @param n Number of messages to submit (0..msgqTxGetFreeSpace()-1)
 * @return 0 on success
 *
 * This is basically "send" function. It submits buffers that were previously
 * filled with msgqTxGetWriteBuffer(). Obviously it may send "trash", if
 * we send more than we filled.
 */
int msgqTxSubmitBuffers(msgqHandle handle, unsigned n);

/**
 * @brief Synchronize TX channel.
 * @param handle
 * @return Number of free buffers.
 *
 * This function is similar to msgqTxGetFreeSpace(), except it invalidates cache
 * to get latest read pointer.
 */
int msgqTxSync(msgqHandle handle);

/**
 * @brief Get number of unread messages in TX channel
 * @param handle
 * @return Number of messages. 0 if queue is empty or not linked. 
 */
unsigned msgqTxGetPending(msgqHandle handle);

/**
 * @brief Get number of unread messages in RX channel
 * @param handle
 * @return Number of messages. 0 if queue is empty or not linked.
 */
unsigned msgqRxGetReadAvailable(msgqHandle handle);

/**
 * @brief Read messages
 * @param handle
 * @param n Number of message, (0..msgqRxGetReadAvailable()-1)
 * @return Pointer to message payload or NULL.
 *
 * WARNING: If memory cookie is used, returned pointer can't be accessed
 * directly, wrappers must be used.
 * Note that it should be not assumed that pointers returned for n=0 and n=1
 * are continuous.
 * This function is cheap and should be used for batch receive.
 */
const void *msgqRxGetReadBuffer(msgqHandle handle, unsigned n);

/**
 * @brief Mark messages as read
 * @param handle
 * @param n Number of messages to be read, (0..msgqRxGetReadAvailable()-1)
 * @return 0 on success
 *
 * This function (may) notify other side that messages were consumed.
 */
int msgqRxMarkConsumed(msgqHandle handle, unsigned n);

/**
 * @brief Synchronize RX channel.
 * @param handle
 * @return Number of pending messages
 *
 * This function is similar to msgqRxGetReadAvailable(), except it invalidates
 * cache to get latest write pointer.
 */
int msgqRxSync(msgqHandle handle);

#endif // MSGQ_H
