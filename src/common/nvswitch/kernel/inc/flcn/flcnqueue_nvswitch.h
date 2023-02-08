/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _FLCNQUEUE_NVSWITCH_H_
#define _FLCNQUEUE_NVSWITCH_H_

/*!
 * @file      flcnqueue_nvswitch.h
 * @copydoc   flcnqueue_nvswitch.c
 */

#include "nvstatus.h"
#include "flcnifcmn.h"

struct nvswitch_device;
struct NVSWITCH_TIMEOUT;
struct FLCN;
struct FLCNQUEUE;
union RM_FLCN_MSG;
union RM_FLCN_CMD;

/*!
 * Define the signature of the callback function that FLCN clients must
 * register when sending a FLCN command or registering for FLCN event
 * notification.  Upon completion of the command or upon intercepting an event
 * of a specific type, the callback will be invoked passing the completed
 * sequence or event descriptor to the client along with status to indicate if
 * the message buffer was properly populated.
 *
 * @param[in] device    nvswitch_device pointer
 * @param[in] pMsg      Pointer to the received message
 * @param[in] pParams   Pointer to the parameters
 * @param[in] seqDesc   Sequencer descriptor number
 * @param[in] status    Status for command execution result
 */
typedef void (*FlcnQMgrClientCallback)(struct nvswitch_device *, union RM_FLCN_MSG *pMsg, void *pParams, NvU32 seqDesc, NV_STATUS status);

typedef NV_STATUS (*FlcnQueueClose    )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvBool);
typedef NvBool    (*FlcnQueueIsEmpty  )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue);
typedef NV_STATUS (*FlcnQueueOpenRead )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue);
typedef NV_STATUS (*FlcnQueueOpenWrite)(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32);
typedef NV_STATUS (*FlcnQueuePop      )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, void*, NvU32, NvU32 *);
typedef void      (*FlcnQueuePush     )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, void*, NvU32);
typedef void      (*FlcnQueueRewind   )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue);

typedef NV_STATUS (*FlcnQueueHeadGet           )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32 *pHead);
typedef NV_STATUS (*FlcnQueueHeadSet           )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32  head );
typedef NV_STATUS (*FlcnQueueTailGet           )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32 *pTail);
typedef NV_STATUS (*FlcnQueueTailSet           )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32  tail );
typedef void      (*FlcnQueueRead              )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32  offset, NvU8 *pDst, NvU32 sizeBytes);
typedef void      (*FlcnQueueWrite             )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32  offset, NvU8 *pSrc, NvU32 sizeBytes);
typedef NV_STATUS (*FlcnQueueHasRoom           )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32 writeSize, NvBool *pBRewind);
typedef NV_STATUS (*FlcnQueueLock              )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, struct NVSWITCH_TIMEOUT *pTimeout);
typedef NV_STATUS (*FlcnQueueUnlock            )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue);
typedef NvU32     (*FlcnQueuePopulateRewindCmd )(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, union RM_FLCN_CMD *pFlcnCmd);
typedef NV_STATUS (*FlcnQueueElementUseStateClr)(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE *pQueue, NvU32 queuePos);

/*!
 * This structure defines the various flags that may be passed to the queue
 * "open" API.  Read-operations are allowed on queues opened for 'READ';
 * write-operations are allowed when opened for "WRITE".  The specific flag
 * used when a queue is opened defines behavior of the "close" operation.
 */
typedef enum
{
    FLCNQUEUE_OFLAG_READ = 0,
    FLCNQUEUE_OFLAG_WRITE
} FLCNQUEUE_OFLAG;

/*!
 * Contains all fields, attributes, and functions pertaining to Falcon Queues.
 */
typedef struct FLCNQUEUE
{
    FlcnQueueClose              close;
    FlcnQueueIsEmpty            isEmpty;
    FlcnQueueOpenRead           openRead;
    FlcnQueueOpenWrite          openWrite;
    FlcnQueuePop                pop;
    FlcnQueuePush               push;
    FlcnQueueRewind             rewind;

    FlcnQueueHeadGet            headGet;
    FlcnQueueHeadSet            headSet;
    FlcnQueueTailGet            tailGet;
    FlcnQueueTailSet            tailSet;
    FlcnQueueRead               read;
    FlcnQueueWrite              write;
    FlcnQueueHasRoom            hasRoom;
    FlcnQueuePopulateRewindCmd  populateRewindCmd;
    FlcnQueueElementUseStateClr elementUseStateClr;

    /*!
     * When the queue is currently opened for writing, this value stores the
     * current write position.  This allows multiple writes to be streamed into
     * the queue without updating the head pointer for each individual write.
     */
    NvU32                       position;

    /*! The physical DMEM offset where this queue resides/begins. */
    NvU32                       queueOffset;

    /*!
     * The logical queue identifier for the queue which we use to index into
     * the queue structures inside RM.
     */
    NvU32                       queueLogId;

    /*!
     * The physical queue index indicates the index of the queue pertaining to
     * its type. We can use it to index into the head and tail registers of
     * a particular type(CMD or MSG) of queue.
     * For e.g., consider we have 3 command queues and 2 message queues allocated
     * for a particular falcon, their queueLogId and queuePhyId values will be as:
     * <Assuming the command queues are allocated first>
     * CMDQ0 queuePhyId = 0, queueLogId = 0
     * CMDQ1 queuePhyId = 1, queueLogId = 1
     * CMDQ2 queuePhyId = 2, queueLogId = 2
     *
     * MSGQ0 queuePhyId = 0, queueLogId = 3
     * MSGQ1 queuePhyId = 1, queueLogId = 4
     */
    NvU32                       queuePhyId;

    /*! The size of the queue in bytes for DMEM queue, number of entries for FB queue */
    NvU32                       queueSize;

    /*! The size of the command header in bytes. */
    NvU32                       cmdHdrSize;

    /*!
     * Maximum size for each command.
     */
    NvU32                       maxCmdSize;

    /*! The open-flag that was specified when the queue was opened. */
    FLCNQUEUE_OFLAG             oflag;

    /*!
     * 'NV_TRUE' when data is currently being written info the queue (only
     * pertains to command queues).
     */
    NvBool                      bOpened;

    /*!
     * 'NV_TRUE' when locked granting exclusive access the the lock owner.
     */
    NvBool                      bLocked;

} FLCNQUEUE, *PFLCNQUEUE;

/*!
 * @brief Enumeration to represent each discrete sequence state
 *
 * Each sequence stored in the Sequence Table must have a state associated
 * with it to keep track of used vs. available sequences.
 */
typedef enum
{
    /*! Indicates the sequence is not be used and is available */
    FLCN_QMGR_SEQ_STATE_FREE = 0,

    /*!
     * Indicates the sequence has been reserved for a command, but command has
     * not yet been queued in a command queue.
     */
    FLCN_QMGR_SEQ_STATE_PENDING,

    /*!
     * Indicates the sequence has been reserved for a command and has been
     * queued.
     */
    FLCN_QMGR_SEQ_STATE_USED,

    /*!
     * Indicates that an event has occurred (shutdown/reset/...) that caused
     * the sequence to be canceled.
     */
    FLCN_QMGR_SEQ_STATE_CANCELED
} FLCN_QMGR_SEQ_STATE;

/*!
 * @brief   Common SEQ_INFO used by all falcons.
 */
typedef struct FLCN_QMGR_SEQ_INFO
{
    /*!
     * The unique identifier used by the FLCN ucode to distinguish sequences.
     * The ID is unique to all sequences currently in-flight but may be reused
     * as sequences are completed by the FLCN.
     */
    NvU8                    seqNum;
    /*!
     * Similar to 'seqNum' but unique for all sequences ever submitted (i.e.
     * never reused).
     */
    NvU32                   seqDesc;
    /*!
     * The state of the sequence (@ref FLCN_QMGR_SEQ_STATE).
     */
    FLCN_QMGR_SEQ_STATE     seqState;
    /*!
     * The client function to be called when the sequence completes.
     */
    FlcnQMgrClientCallback  pCallback;
    /*!
     * Client-specified params that must be provided to the callback function.
     */
    void                   *pCallbackParams;
    /*!
     * The client message buffer that will be filled when the sequence completes.
     */
    RM_FLCN_MSG_GEN        *pMsgResp; 

    /*!
     * CMD Queue associated with this Seq.
     */
    struct FLCNQUEUE       *pCmdQueue;

} FLCN_QMGR_SEQ_INFO, *PFLCN_QMGR_SEQ_INFO;

NV_STATUS flcnQueueConstruct_common_nvswitch(struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE **ppQueue, NvU32 queueId, NvU32 queuePhyId, NvU32 offset, NvU32 queueSize, NvU32 cmdHdrSize);
NV_STATUS flcnQueueConstruct_dmem_nvswitch  (struct nvswitch_device *device, struct FLCN *pFlcn, struct FLCNQUEUE **ppQueue, NvU32 queueId, NvU32 queuePhyId, NvU32 offset, NvU32 queueSize, NvU32 cmdHdrSize);


// Dumping queues for debugging purpose
NV_STATUS flcnRtosDumpCmdQueue_nvswitch(struct nvswitch_device *device, struct FLCN *pFlcn, NvU32 queueLogId, union RM_FLCN_CMD *FlcnCmd);

/*!
 * Alignment to use for all head/tail pointer updates.  Pointers are always
 * rouned up to the nearest multiple of this value.
 */
#define QUEUE_ALIGNMENT  (4)

/*!
 * Checks if the given queue is currently opened for read.
 */
#define QUEUE_OPENED_FOR_READ(pQueue)                                          \
    (((pQueue)->bOpened) && ((pQueue)->oflag == FLCNQUEUE_OFLAG_READ))

/*!
 * Checks if the given queue is currently opened for write.
 */
#define QUEUE_OPENED_FOR_WRITE(pQueue)                                         \
    (((pQueue)->bOpened) && ((pQueue)->oflag == FLCNQUEUE_OFLAG_WRITE))

#endif // _FLCNQUEUE_NVSWITCH_H_

