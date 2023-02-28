/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "flcn/flcnqueue_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "soe/soe_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"
#include "common_nvswitch.h"

//
// Yield the CPU temporarily after openWrite() in _flcnQueueCmdWrite_IMPL()
// fails this many times in a row.
//
#define FLCN_QUEUE_CMD_WRITE_SLEEP_TRIES    1024

/*!
 * @file   flcnqueue_nvswitch.c
 * @brief  Provides all the fundamental logic for reading/writing queues.
 *
 * Queues are the primary communication mechanism between the RM and various
 * falcon-based engines such as the PMU and Display Falcon.  The RM requests
 * actions by inserting a data packet (command) into a command queue. This
 * generates an interrupt to the falcon which allows it to wake-up and service
 * the request.  Upon completion of the command, the falcon can optionally
 * write an acknowledgment packet (message) into a separate queue designated
 * for RM-bound messages.
 *
 * There are currently two type of queues supported:
 *      1) DMEM queues.  The original queue type.   For informaiton specific to
 *          DMEM queues see the HDR of flcnqueue_dmem.c
 *      2) FB Queues   For infomation specific to FB Queues, see the HDR of
 *          flcnqueue_fb.c.
 * This file contains routines common to both queue types.
 *
 * </pre>
 *
 * To use the read/write APIs, the caller must first "open" the queue for the
 * appropriate action (read/write).  A queue may only be opened once at any
 * given time.  Subsequent attempts to open an already "opened" queue will
 * result in a failure.  This module will keep track of all "opened" queues.
 * When a queue is opened for "read", a copy of the queue's current tail
 * pointer will be cached.  All reads that occur from the time the queue is
 * opened to the time it is closed will be based on this cached value. The
 * value will be updated for each individual read operation that is requested.
 * When the queue is "closed", this value will be written out to its
 * corresponding PRIV register thus making it visible to the falcon ucode. The
 * same scheme is also applied when opening a queue for "writing" except that
 * instead of caching the tail pointer, the header pointer is cached.
 * Additionally, when a "command queue" is closed and the new head pointer is
 * written, an interrupt will be generated for the falcon to allow the command
 * to be processed.
 */

static NvBool    _flcnQueueCmdValidate      (nvswitch_device *device, PFLCN pFlcn, PRM_FLCN_CMD pCmd, PRM_FLCN_MSG pMsg, void *pPayload, NvU32 queueIdLogical);
static NvU32     _flcnQueuePopulateRewindCmd(nvswitch_device *device, PFLCN pFlcn, PFLCNQUEUE pQueue, RM_FLCN_CMD *pFlcnCmd);
static NV_STATUS _flcnQueueClose            (nvswitch_device *device, PFLCN, PFLCNQUEUE pQueue, NvBool bCommit);
static NvBool    _flcnQueueIsEmpty          (nvswitch_device *device, PFLCN, PFLCNQUEUE pQueue);
static NV_STATUS _flcnQueueOpenRead         (nvswitch_device *device, PFLCN, PFLCNQUEUE pQueue);
static NV_STATUS _flcnQueueHeadGet          (nvswitch_device *device, PFLCN pFlcn, PFLCNQUEUE pQueue, NvU32 *pHead);
static NV_STATUS _flcnQueueHeadSet          (nvswitch_device *device, PFLCN pFlcn, PFLCNQUEUE pQueue, NvU32 head);

/*!
 * @brief Construct a Falcon Queue object
 *
 * This is a constructor/initialization function for Falcon Queue objects.
 * Callers can choose to either provide a pre-allocated Falcon Queue object or
 * allow this function to perform the allocation.  The former case is more
 * ideal cases where a collection of queues must be allocated or when static
 * allocation is desired.
 *
 * @param[in]      device    nvswitch device pointer
 * @param[in]      pFlcn     FLCN object pointer
 * @param[in,out]  ppQueue
 *     Pointer to the queue to construct and optionally allocate.  When pointing
 *     to a non-NULL queue pointer, the queue is simply initialized.  When NULL,
 *     a queue will be allocated and then initialized.
 *
 * @param[in]      queueLogId  Logical-identifier for the queue
 * @param[in]      queuePhyId  Physical-index/identifier for the queue
 * @param[in]      offset      Starting location of queue in memory (DMEM)
 * @param[in]      queueSize   Size (in bytes) of the queue
 * @param[in]      cmdHdrSize  Size (in bytes) of the command header
 *
 * @return 'NV_OK' upon successful construction/initialization.
 * @return 'NV_ERR_NO_MEMORY' when unable to allocate queue.
 */
NV_STATUS
flcnQueueConstruct_common_nvswitch
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE      *ppQueue,
    NvU32            queueLogId,
    NvU32            queuePhyId,
    NvU32            offset,
    NvU32            queueSize,
    NvU32            cmdHdrSize
)
{
    PFLCNQUEUE  pQueue;
    NV_STATUS   status = NV_OK;

    if (ppQueue == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_POINTER;
    }

    if (*ppQueue == NULL)
    {
        *ppQueue = nvswitch_os_malloc(sizeof(FLCNQUEUE));
        if (*ppQueue == NULL)
        {
            NVSWITCH_PRINT(device, ERROR,
                 "%s: Failed to allocate FLCNQUEUE (queueLogId=0x%x).\n",
                 __FUNCTION__, queueLogId);
            return NV_ERR_NO_MEMORY;
        }
        nvswitch_os_memset(*ppQueue, 0, sizeof(FLCNQUEUE));
    }

    pQueue = *ppQueue;

    pQueue->queueLogId        = queueLogId;
    pQueue->queuePhyId        = queuePhyId;
    pQueue->queueOffset       = offset;
    pQueue->position          = offset;
    pQueue->queueSize         = queueSize;
    pQueue->cmdHdrSize        = cmdHdrSize;
    pQueue->oflag             = 0;
    pQueue->bOpened           = NV_FALSE;
    pQueue->bLocked           = NV_FALSE;
    pQueue->close             = _flcnQueueClose;
    pQueue->isEmpty           = _flcnQueueIsEmpty;
    pQueue->openRead          = _flcnQueueOpenRead;
    pQueue->headGet           = _flcnQueueHeadGet;
    pQueue->headSet           = _flcnQueueHeadSet;
    pQueue->populateRewindCmd = _flcnQueuePopulateRewindCmd;

    return status;
}

/*!
 * @brief Closes a queue
 *
 * Closes the given command queue.  The 'bCommit' flag is used to commit the
 * changes performed on the queue since it was opened.  When the queue is
 * opened for writing, the commit operation involves writing the queue's head
 * pointer with the queue's current write position value.  When committing a
 * queue that has been opened for reading, the tail pointer is updated instead.
 *
 * @param[in]  device   nvswitch device pointer
 * @param[in]  pFlcn    FLCN object pointer
 * @param[in]  pQueue   The queue to close
 * @param[in]  bCommit  'NV_TRUE' to commit the operations performed on the
 *                      queue since opened.  'NV_FALSE' to leave the queue in
 *                      its current HW state.
 *
 * @return 'NV_OK' if the close operation is successful
 * @pre    The queue must be opened prior to calling this function
 * @see    flcnQueueOpenRead
 * @see    flcnQueueOpenWrite
 */
static NV_STATUS
_flcnQueueClose
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue,
    NvBool           bCommit
)
{
    // ensure that the queue is opened for before continuing.
    if (!pQueue->bOpened)
    {
        NVSWITCH_PRINT(device, ERROR,
             "%s: queue not opened (queueLogId=0x%x).\n",
             __FUNCTION__, pQueue->queueLogId);
        return NV_OK;
    }

    //
    // If committing a queue opened for read, close the queue by updating the
    // queue's tail pointer.  If committing when opened for write, update the
    // head pointer.
    //
    if (bCommit)
    {
        if (QUEUE_OPENED_FOR_READ(pQueue))
        {
            (void)pQueue->tailSet(device, pFlcn, pQueue, pQueue->position);
        }
        else
        {
            (void)pQueue->headSet(device, pFlcn, pQueue, pQueue->position);
        }
    }

    // mark the queue as "not open"
    pQueue->bOpened = NV_FALSE;
    return NV_OK;
}

/*!
 * @brief Checks to see if a queue contains any data that may be read
 *
 * Compares the queue's head and tail pointers to see if any data is available
 * for reading.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  The queue to check
 *
 * @return 'NV_TRUE' if the queue is empty; 'NV_FALSE' otherwise.
 * @see    flcnQueuePop
 */
static NvBool
_flcnQueueIsEmpty
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue
)
{
    NvU32  head;
    NvU32  tail;

    //
    // Reading always occurs from the tail to the head.  If the file is
    // already opened for read, use the queue position as the current tail
    // value (otherwise, get the tail value from hardware).
    //
    (void)pQueue->headGet(device, pFlcn, pQueue, &head);
    if (QUEUE_OPENED_FOR_READ(pQueue))
    {
        tail = pQueue->position;
    }
    else
    {
        (void)pQueue->tailGet(device, pFlcn, pQueue, &tail);
    }
    return head == tail;
}

/*!
 * @brief Opens a queue for reading.
 *
 * Opens the given command queue for read operations.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  The queue to open
 *
 * @return 'NV_OK' if the queue is successfully opened. 'NV_ERR_GENERIC' otherwise.
 * @see    flcnQueuePop
 */
static NV_STATUS
_flcnQueueOpenRead
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue
)
{
    //
    // Verify that the queue is not already opened.  This is not expected to
    // occur.
    //
    if (pQueue->bOpened)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: unable to open queue (already opened, queueLogId=0x%x).\n",
            __FUNCTION__, pQueue->queueLogId);
        NVSWITCH_ASSERT(0);
        return NV_ERR_GENERIC;
    }

    //
    // Update the queue position to specify where the first read will occur
    // from, set the open flag, and mark the queue as "opened".
    //
    (void)pQueue->tailGet(device, pFlcn, pQueue, &pQueue->position);
    pQueue->oflag   = FLCNQUEUE_OFLAG_READ;
    pQueue->bOpened = NV_TRUE;

    return NV_OK;
}

/*!
 * Retrieve the current head pointer for given FLCN queue.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   pQueue  Pointer to the queue
 * @param[out]  pHead   Pointer to write with the queue's head pointer
 *
 * @return 'NV_OK' if head value was successfully retrieved.
 * @return 'NV_ERR_GENERIC' otherwise
 */
static NV_STATUS
_flcnQueueHeadGet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue,
    NvU32           *pHead
)
{
    NVSWITCH_ASSERT(pFlcn->pQueueInfo != NULL);
    if (RM_FLCN_QUEUEID_IS_COMMAND_QUEUE(pFlcn->pQueueInfo, pQueue->queueLogId))
    {
        return flcnCmdQueueHeadGet(device, pFlcn, pQueue, pHead);
    }
    else
    {
        return flcnMsgQueueHeadGet(device, pFlcn, pQueue, pHead);
    }
}

/*!
 * Set the head pointer for the given FLCN queue.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  head    The desired head value for the queue
 *
 * @return 'NV_OK' if the head value was successfully set.
 * @return 'NV_ERR_GENERIC' otherwise
 */
static NV_STATUS
_flcnQueueHeadSet
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue,
    NvU32            head
)
{
    NVSWITCH_ASSERT(pFlcn->pQueueInfo != NULL);
    if (RM_FLCN_QUEUEID_IS_COMMAND_QUEUE(pFlcn->pQueueInfo, pQueue->queueLogId))
    {
        return flcnCmdQueueHeadSet(device, pFlcn, pQueue, head);
    }
    else
    {
        return flcnMsgQueueHeadSet(device, pFlcn, pQueue, head);
    }
}

/*!
 * Populate the FLCN queue rewind command
 *
 * @param[in]  device    nvswitch device pointer
 * @param[in]  pFlcn     FLCN object pointer
 * @param[in]  pQueue    The queue where we will push the command to
 * @param[in]  pFlcnCmd  Pointer storing the content of the rewind command
 *
 * @return The size of the populated data
 */
static NvU32
_flcnQueuePopulateRewindCmd
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PFLCNQUEUE       pQueue,
    RM_FLCN_CMD     *pFlcnCmd
)
{
    pFlcnCmd->cmdGen.hdr.unitId = RM_FLCN_UNIT_ID_REWIND;
    pFlcnCmd->cmdGen.hdr.size   = (NvU8)pQueue->cmdHdrSize;
    return pFlcnCmd->cmdGen.hdr.size;
}

/*!
 * Write a command to the specified command queue.
 *
 * @param[in]  device      nvswitch device pointer
 * @param[in]  pFlcn       FLCN object Pointer
 * @param[in]  queueLogId  Logical ID of the queue
 * @param[in]  pCmd        The command buffer to submit
 *
 * @param[in]  pTimeout
 *     An optional pointer (may be NULL) to a pre-configured timeout structure
 *     that when non-NULL is used to indicate that blocking behavior is
 *     allowed (within the bounds of the timeout) for operations that have the
 *     potential to fail on transient conditions and can be retried (mutex
 *     acquirement, queue insertion, etc ...).  When NULL, this function does
 *     NOT retry such operations when they fail (the function becomes non-
 *     blocking) and returns back control to the caller.  Example scenarios
 *     include when the command queue mutex can not be obtained and if the
 *     queue does not have enough free space to fit the command.
 *
 * @return 'NV_OK'
 *     If the command is successfully written to the command queue.
 *
 * @return 'NV_ERR_INSUFFICIENT_RESOURCES'
 *     If the command could not be queued as a result of the target command
 *     queue having insufficient space to fit the command.  Could be after
 *     the initial queue attempt if the non-blocking behavior has been
 *     requested or after successive retries if the timeout expired before
 *     enough space was free'd in the queue.
 *
 * @return 'NV_ERR_FLCN_ERROR'
 *     If the command could not be queued due to a failure such as a HALTed
 *     SOE. This is considered a fatal error.
 *
 * @return  NV_ERR_TIMEOUT
 *      A timeout occurred before the command write completed.
 */
static NV_STATUS
_flcnQueueCmdWrite_IMPL
(
    nvswitch_device    *device,
    PFLCN               pFlcn,
    NvU32               queueLogId,
    RM_FLCN_CMD        *pCmd,
    NVSWITCH_TIMEOUT   *pTimeout
)
{
    NV_STATUS           status;
    PFLCNQUEUE          pQueue;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;
    NvBool              bKeepPolling;
    NvU32               nTries = 0;

    NVSWITCH_ASSERT(pTimeout != NULL);
    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueueInfo->pQueues != NULL);
    pQueue = &pQueueInfo->pQueues[queueLogId];

    //
    // Open the command queue for writing. It is guaranteed that the queue will
    // have sufficient space for the command if successfully opened. Upon
    // failure, retries will be conducted until either space free's up in the
    // queue for the command or until a timeout occurs, assuming the SOE is 
    // operating normally (not HALTed).
    //
    do
    {
        bKeepPolling = (nvswitch_timeout_check(pTimeout)) ? NV_FALSE : NV_TRUE;

        status = pQueue->openWrite(device, pFlcn, pQueue, pCmd->cmdGen.hdr.size);
        if (status == NV_ERR_INSUFFICIENT_RESOURCES)
        {
            if (soeIsCpuHalted_HAL(device, ((PSOE)pFlcn->pFlcnable)))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: SOE Halt detected (queueLogId=0x%x).\n",
                    __FUNCTION__, pQueue->queueLogId);
               return NV_ERR_FLCN_ERROR;
            }

            if (++nTries < 4)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s: queue is too full to write data (write-size=0x%x).\n",
                    __FUNCTION__, pCmd->cmdGen.hdr.size);
            }
            else if ((nTries % FLCN_QUEUE_CMD_WRITE_SLEEP_TRIES) == 0)
            {
                nvswitch_os_sleep(1);
            }
        }
        else
        {
            // Anything else is likely not transient 
            break;
        }
    } while (bKeepPolling);

    if (status == NV_ERR_INSUFFICIENT_RESOURCES)
    {
#if defined(DEBUG)
        RM_FLCN_CMD         FlcnCmd;
        NV_STATUS           dumpStatus;

        dumpStatus = flcnRtosDumpCmdQueue_nvswitch(device, pFlcn, queueLogId, &FlcnCmd);
        if (dumpStatus != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Dumping Falcon Command queue completed with status 0x%x \n",
                __FUNCTION__ , dumpStatus);
        }
#endif  // DEBUG

        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout after %d tries while waiting for space (queueLogId=0x%x).\n"
            "Command queue:\n",
            __FUNCTION__, nTries, pQueue->queueLogId);

        return NV_ERR_TIMEOUT;
    }

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: error while opening queue (queueLogId=0x%x, status=0x%x).\n",
            __FUNCTION__, pQueue->queueLogId, status);
        return status;
    }

    // write the command to the queue.
    pQueue->push(device, pFlcn, pQueue, pCmd, pCmd->cmdGen.hdr.size);

    //
    // Close the command queue to flush out the new head pointer.  A failure
    // to properly close the queue indicates that the head pointer is unchanged.
    // In that case, nothing new has been enqueued from the FLCN's perspective.
    //
    status = pQueue->close(device, pFlcn, pQueue, NV_TRUE);
    if (status == NV_OK)
    {
        NVSWITCH_PRINT(device, MMIO,
            "%s: command queued (unit-id=0x%x).\n",
            __FUNCTION__, pCmd->cmdGen.hdr.unitId);
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: error while closing queue (queueLogId=0x%x, status=0x%x).\n",
            __FUNCTION__, pQueue->queueLogId, status);
    }

    return status;
}

/*!
 * Lookup/find the info structure for a sequence given a sequence descriptor.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   seqDesc Descriptor for the sequencer to find
 *
 * @return  SEQ_INFO structure pointer or NULL if no free entry was found.
 */
static PFLCN_QMGR_SEQ_INFO
_flcnQueueSeqInfoFind_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo;
    NvU32               seqIndex;

    for (seqIndex = 0; seqIndex < pFlcn->numSequences; seqIndex++)
    {
        // (seqIndex < numSequences) so no need to check pointer for NULL.
        pSeqInfo = flcnableQueueSeqInfoGet(device, pFlcn->pFlcnable, seqIndex);

        if (pSeqInfo->seqDesc == seqDesc)
        {
            return pSeqInfo;
        }
    }
    return NULL;
}

/*!
 * Find a free sequence info structure and reserve it so that it may not be
 * taken by another client.  We always search the free seq starting from the
 * next to the latest used seq since it is the most possible free sequence if
 * we consume the sequence in serial.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 *
 * @return  SEQ_INFO structure pointer or NULL if no free entry was fount
 */
static PFLCN_QMGR_SEQ_INFO
_flcnQueueSeqInfoAcq_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;
    NvU32               seqIndex;

    seqIndex = pQueueInfo->latestUsedSeqNum;

    for (++seqIndex, seqIndex %= pFlcn->numSequences; seqIndex != pQueueInfo->latestUsedSeqNum;
         ++seqIndex, seqIndex %= pFlcn->numSequences)
    {
        // (seqIndex < numSequences) so no need to check pointer for NULL.
        pSeqInfo = flcnableQueueSeqInfoGet(device, pFlcn->pFlcnable, seqIndex);

        if (pSeqInfo->seqState == FLCN_QMGR_SEQ_STATE_FREE)
        {
            pSeqInfo->seqState = FLCN_QMGR_SEQ_STATE_PENDING;
            pQueueInfo->latestUsedSeqNum = seqIndex;
            return pSeqInfo;
        }

    }

    NVSWITCH_PRINT(device, ERROR, "%s: No free sequence numbers.\n", __FUNCTION__);
    return NULL;
}

/*!
 * @brief   Mark the sequence info structure as available (and clear it out).
 *
 * @param[in]       device      nvswitch device pointer
 * @param[in]       pFlcn       FLCN object pointer
 * @param[in,out]   pSeqInfo    Pointer to the sequence info struct to release
 */
static void
_flcnQueueSeqInfoRel_IMPL
(
    nvswitch_device    *device,
    PFLCN               pFlcn,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    // Leave @ref seqNum untouched.
    pSeqInfo->seqDesc         = FLCN_INVALID_SEQ_DESC;
    pSeqInfo->seqState        = FLCN_QMGR_SEQ_STATE_FREE;
    pSeqInfo->pCallback       = NULL;
    pSeqInfo->pCallbackParams = NULL;

    // Clear any engine specific SEQ_INFO structure extension.
    flcnableQueueSeqInfoClear(device, pFlcn->pFlcnable, pSeqInfo);
}

/*!
 * Initializes all global sequence tracking data -- releases all sequence info
 * elements, initializes sequence numbers, etc.
 *
 * @param[in]       device  nvswitch device pointer
 * @param[in,out]   pFlcn   FLCN object pointer
 */
static void
_flcnQueueSeqInfoStateInit_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo;
    NvU32               seqIndex;

    // initialize all elements of the sequence info table
    for (seqIndex = 0; seqIndex < pFlcn->numSequences; seqIndex++)
    {
        // (seqIndex < numSequences) so no need to check pointer for NULL.
        pSeqInfo = flcnableQueueSeqInfoGet(device, pFlcn->pFlcnable, seqIndex);

        pSeqInfo->seqNum = (NvU8)seqIndex;
        flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);
    }
}

/*!
 * @brief Cancel all sequences that are not currently "free".
 *
 * Invokes the callback function for any commands that are currently running
 * to inform the client that the command has been cancelled/failed.  This could
 * happen as a result of restarting the FLCN or tearing down the driver.
 *
 * All sequences that are currently running will be marked as "cancelled". The
 * sequence itself will not be released for reuse.  This is to allow the status
 * to persist so that the data is not stale when/if the client queries for
 * status.  It also prevents multiple commands containing the same sequence
 * number from being sent to the FLCN.  Only call this function prior to event
 * which will reset the FLCN (or shut it down completely) to avoid leaking
 * sequences.
 *
 * To ease the teardown process, this function MAY be called when:
 *     - No outstanding sequences exist that need to be cancelled
 *     - When the sequence-info table is not allocated
 *
 * @param[in]      device nvswitch device pointer
 * @param[in,out]  pFlcn  FLCN object pointer
 */
static void
_flcnQueueSeqInfoCancelAll_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo;
    NvU32               seqIndex;

    // find the sequence info date for the given sequence
    for (seqIndex = 0; seqIndex < pFlcn->numSequences; seqIndex++)
    {
        // (seqIndex < numSequences) so no need to check pointer for NULL.
        pSeqInfo = flcnableQueueSeqInfoGet(device, pFlcn->pFlcnable, seqIndex);

        if (pSeqInfo->seqState != FLCN_QMGR_SEQ_STATE_FREE)
        {
            if (pSeqInfo->seqState != FLCN_QMGR_SEQ_STATE_CANCELED)
            {
                //
                // Cancel the sequence and report an error to the client that
                // issued to the command to allow them to perform any necessary
                // cleanup (if applicable).
                //
                pSeqInfo->seqState = FLCN_QMGR_SEQ_STATE_CANCELED;
                if (pSeqInfo->pCallback != NULL)
                {
                    pSeqInfo->pCallback(device, NULL,
                                        pSeqInfo->pCallbackParams,
                                        pSeqInfo->seqDesc,
                                        NV_ERR_INVALID_STATE);
                }
            }

            // Free resources associated with all outstanding sequences.
            (void)flcnQueueSeqInfoFree(device, pFlcn, pSeqInfo);
        }

        // Re-initialize SEQ_INFO structures.
        flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);
    }
}

/*!
 * @brief   Free up all the engine specific sequence allocations.
 *
 * @param[in]   device      nvswitch device pointer
 * @param[in]   pPmu        PMU object pointer
 * @param[in]   pSeqInfo    SEQ_INFO structure pointer
 *
 * @return 'NV_OK' if FB Queue Element in use bit cleared.  Otherwise error code
 *     from elementUseStateClr().
 */
static NV_STATUS
_flcnQueueSeqInfoFree_IMPL
(
    nvswitch_device    *device,
    PFLCN               pFlcn,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{

    NV_STATUS status = NV_OK;

    flcnableQueueSeqInfoFree(device, pFlcn->pFlcnable, pSeqInfo);

    return status;
}

/*!
 * @brief Finds a free event descriptor
 *
 * Searches through the FLCN Event Info list and finds an available event
 * descriptor. Note that this function does not guarantee that the returned
 * will remain reserved.  It simply returns an event descriptor that is
 * currently unassigned.
 *
 * @param device    nvswitch device pointer
 * @param pFlcn     FLCN object pointer
 * @param pEvtDesc  Pointer to write with the assigned event descriptor
 *
 * @return 'NV_OK' if a free event descriptor was found and assigned.
 */
static NV_STATUS
_flcnQueueAssignEventDesc
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32           *pEvtDesc
)
{
    PFLCN_EVENT_INFO pEventInfo;
    NvU32               nextDesc;
    NV_STATUS           status     = NV_OK;
    NvBool              bAvailable = NV_FALSE;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);

    //
    // Search through the event info list to see if the current event
    // descriptor is available.  When not available, move on to the next
    // descriptor (allow wrapping) and check the list again.
    //
    nextDesc = pQueueInfo->nextEvtDesc;
    while (!bAvailable)
    {
        pEventInfo = pQueueInfo->pEventInfo;
        bAvailable = NV_TRUE;
        while (pEventInfo != NULL)
        {
            // check if already assigned
            if (pEventInfo->evtDesc == nextDesc)
            {
                bAvailable = NV_FALSE;
                break;
            }
            pEventInfo = pEventInfo->pNext;
        }

        //
        // Move on to the next descriptor if the current descriptor is not
        // available, verify that we did not wrap around back to where we
        // started.
        //
        ++nextDesc;
        if ((!bAvailable) && (nextDesc == pQueueInfo->nextEvtDesc))
        {
            //
            // Hitting this point is bad.  It indicates that all 2^32 event
            // descriptors are currently assigned!
            //
            NVSWITCH_ASSERT(0);
            break;
        }
    }

    //
    // If an available descriptor was found set the returned event descriptor
    // value and update the next event descriptor value.
    //
    if (bAvailable)
    {
        *pEvtDesc = nextDesc - 1;
        pQueueInfo->nextEvtDesc = nextDesc;
    }
    else
    {
        status = NV_ERR_GENERIC;
    }
    return status;
}

/*!
 * @brief Register for event notification.
 *
 * Registers the given client callback for notification of 'unitId' events.
 * Returns a unique handle from which clients must use to identify themselves
 * later when un-registering for events.
 *
 * @param[in]      device  nvswitch device pointer
 * @param[in]      pFlcn   FLCN object pointer
 * @param[in]      unitId
 *          The identifier which describes the type of event to register for.
 *          See 'nvswitch/common/inc/rmflcncmdif_nvswitch.h' for a list of all
 *          available unit identifiers.
 *
 * @param[in,out]  pMsg
 *          A buffer to contain the data associated with the event.  It is the
 *          caller's responsibility to initialize this buffer.  This parameter
 *          is optional (may be NULL) for callers interested in the occurrence
 *          of an event, but not interested in the data passed in the event.
 *
 * @param[in]      pCallback
 *          The callback function pointer to be called when the event fires.
 *
 * @param[in]      pCallbackParams
 *          Additional optional (may be NULL) parameters that will be blindly
 *          passed to the callback function.  This API will not use these
 *          parameters whatsoever.
 *
 * @param[out]     pEvtDesc
 *          Represents and identifier that will be assigned for this particular
 *          registration.  This will be required for properly un-registering
 *          for the event.
 *
 * @return 'NV_OK' if a successful registration attempt was made.
 */
static NV_STATUS
_flcnQueueEventRegister_IMPL
(
    nvswitch_device        *device,
    PFLCN                   pFlcn,
    NvU32                   unitId,
    NvU8                   *pMsg,
    FlcnQMgrClientCallback  pCallback,
    void                   *pCallbackParams,
    NvU32                  *pEvtDesc
)
{
    PFLCN_EVENT_INFO pEventInfo;
    NvU32               evtDesc;
    NV_STATUS           status;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);

    if ((pCallback == NULL) || (pEvtDesc == NULL))
    {
        NVSWITCH_PRINT(device, ERROR,
                   "%s: Callback and event descriptor may not be NULL",
                   __FUNCTION__);
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Validate the UNIT ID (all other pointers are either optional or are
    // expected to be checked in the higher-level event registration API).
    //
    if (!RM_FLCN_UNIT_ID_IS_VALID(pQueueInfo, unitId))
    {
        NVSWITCH_PRINT(device, ERROR,
                   "%s: Invalid unit-id (0x%x)",
                   __FUNCTION__, unitId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // find an available event descriptor to assign to this event registration
    status = _flcnQueueAssignEventDesc(device, pFlcn, &evtDesc);
    if (status != NV_OK)
    {
        //
        // Hitting this point means there is a serious descriptor leak
        // somewhere (2^32 descriptors assigned).  This is not expected to
        // happen.
        //
        NVSWITCH_PRINT(device, ERROR,
                   "%s: Error assigning a FLCN event descriptor. No " \
                   "more descriptors?\n",
                   __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return status;
    }

    // allocate an event-info structure
    pEventInfo = nvswitch_os_malloc(sizeof(FLCN_EVENT_INFO));
    if (pEventInfo == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "%s: could not allocate memory for event info " \
                    "structure.\n",
                    __FUNCTION__);
        return NV_ERR_NO_MEMORY;
    }

    // fill the newly created event info structure
    pEventInfo->unitId          = unitId;
    pEventInfo->pMsg            = (RM_FLCN_MSG *)pMsg;
    pEventInfo->pCallback       = pCallback;
    pEventInfo->pCallbackParams = pCallbackParams;
    pEventInfo->pNext           = pQueueInfo->pEventInfo;
    pEventInfo->evtDesc         = evtDesc;

    // prepend the new event-info to the front of the event-list
    pQueueInfo->pEventInfo = pEventInfo;

    // return a descriptor for the event-info data to the client
    *pEvtDesc = evtDesc;
    return NV_OK;
}

/*!
 * @brief Unregister for event notification.
 *
 * Un-registers the given client for event notification.  Clients must
 * identify themselves using the handle provided by the Registration
 * function.
 *
 * @param[in]  device   nvswitch device pointer
 * @param[in]  pFlcn    FLCN object pointer
 * @param[in]  evtDesc  The descriptor that was assigned when the event was
 *                      first registered for.
 *
 * @return 'NV_OK' if the un-registration was successful.
 */
static NV_STATUS
_flcnQueueEventUnregister_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            evtDesc
)
{
    PFLCN_EVENT_INFO    pEventInfo;
    PFLCN_EVENT_INFO    pEventInfoPrev = NULL;
    NV_STATUS           status         = NV_OK;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    if (pQueueInfo == NULL)
    {
        NVSWITCH_ASSERT(pQueueInfo != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    // get the event-info list
    pEventInfo = pQueueInfo->pEventInfo;

    //
    // Search through the event-info list to find the event-info for the
    // specified client.
    //
    while (pEventInfo != NULL && evtDesc != pEventInfo->evtDesc)
    {
        // move on to the next event-info structure
        pEventInfoPrev = pEventInfo;
        pEventInfo     = pEventInfo->pNext;
    }

    //
    // If the event-info data was found, unlink it from the list and free the
    // memory.  Return an error otherwise.
    //
    if (pEventInfo != NULL)
    {
       if (pEventInfoPrev != NULL)
       {
           pEventInfoPrev->pNext  = pEventInfo->pNext;
       }
       else
       {
           pQueueInfo->pEventInfo = pEventInfo->pNext;
       }
       pEventInfo->pNext = NULL;
       nvswitch_os_free(pEventInfo);
       pEventInfo = NULL;
    }
    else
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
    }
    return status;
}

/*!
 * High-level event dispatcher for all event messages posted to the Message
 * Queue. This function will perform the following operations on the received
 * message:
 *
 *     # Inspect the message to determine if the message represents an event
 *       that must be generically handled by the FLCN object.
 *
 *     # Relay the message to the FLCN HAL layer in case any chip-specific
 *       processing of the message is required.
 *
 *     # Notify all clients that have register for notification on this type
 *       of FLCN message.
 *
 * All three of the actions are ALWAYS performed. The only exceptions are if/
 * when error occur at the object- or HAL-layer processing.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pMsg    Pointer to event's message data
 * @param[in]  evtStatus
 *     status to be passed to the event listener. If this is not NV_OK, the
 *     event handler can take appropriate actions upon errors. e.g. event
 *     cancelled, etc.
 *
 * @return 'NV_OK' if the event was successfully handled.  Otherwise, an
 *          error occurred at the object- or HAL-layer processing. Such errors
 *          will be reflected in the returned status value.
 */
static NV_STATUS
_flcnQueueEventHandle_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    RM_FLCN_MSG     *pMsg,
    NV_STATUS        evtStatus
)
{
    PFLCN_EVENT_INFO    pEventInfo;
    PFLCN_EVENT_INFO    pEventInfoNext;
    NV_STATUS           status    = NV_OK;
    RM_FLCN_MSG_GEN    *pMsgGen   = (RM_FLCN_MSG_GEN *)pMsg;
    PFLCNABLE           pFlcnable = pFlcn->pFlcnable;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);

    // get the event-info list
    pEventInfo = pQueueInfo->pEventInfo;

    //
    // Inspect the message and determine if the message needs handled at the
    // object-layer.
    //
    if (pMsgGen->hdr.unitId == pQueueInfo->initEventUnitId)
    {
        status = flcnableHandleInitEvent(device, pFlcnable, pMsg);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                        "%s: Error processing FLCN message at object-layer " \
                        "(unitId=0x%02x, seqNum=0x%02x).\n",
                        __FUNCTION__, pMsgGen->hdr.unitId, pMsgGen->hdr.seqNumId);
            NVSWITCH_ASSERT(0);
            return status;
        }
    }

    //
    // Search through the event listener list and signal all clients listening
    // for this type of event.
    //
    for (pEventInfo = pQueueInfo->pEventInfo; pEventInfo != NULL; pEventInfo = pEventInfoNext)
    {
        //
        // Get a pointer to the next event info structure now just in case
        // the client decides to unregister from within the callback.
        //
        pEventInfoNext = pEventInfo->pNext;

        //
        // When a client is found listening for this type of event, copy the
        // message into the buffer given by the client when they registered
        // for the event notification.  Call the client's callback function
        // when after the copy completes.
        //
        if (pEventInfo->unitId == pMsgGen->hdr.unitId)
        {
            if (pEventInfo->pMsg != NULL)
            {
                nvswitch_os_memcpy(pEventInfo->pMsg, pMsg, pMsgGen->hdr.size);
            }

            // callback function cannot be NULL
            NVSWITCH_ASSERT(pEventInfo->pCallback != NULL);
            pEventInfo->pCallback(device,
                                  pMsg,
                                  pEventInfo->pCallbackParams,
                                  pEventInfo->evtDesc,
                                  evtStatus);
        }
    }

    return status;
}

/*!
 * Handle non-event messages. This functions checks whether a message is a
 * valid reply to the commands sent previously, and if it's valid, it calls
 * registered callback functions.
 *
 * @param[in] device nvswitch device pointer
 * @param[in  pFlcn  FLCN object pointer
 * @param[in] pMsg   Pointer to event's message data
 */
static NV_STATUS
_flcnQueueResponseHandle_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    RM_FLCN_MSG     *pMsg
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo;
    NV_STATUS           status     = NV_OK;
    RM_FLCN_MSG_GEN    *pMsgGen    = (RM_FLCN_MSG_GEN *)pMsg;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;
    NvU32               msgSize;

    NVSWITCH_ASSERT(pQueueInfo != NULL);

    if (pMsgGen == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NULL message\n",
            __FUNCTION__);
        return NV_ERR_GENERIC;
    }

    // get the sequence info data associated with this message
    pSeqInfo = flcnableQueueSeqInfoGet(device, pFlcn->pFlcnable, pMsgGen->hdr.seqNumId);
    if ((pSeqInfo == NULL) ||
        (pSeqInfo->seqState != FLCN_QMGR_SEQ_STATE_USED))
    {
        //
        // Hitting this case indicates that the FLCN has reported a non-event
        // message for a command that is not known to be queued.
        //
        NVSWITCH_PRINT(device, ERROR,
            "%s: message received for an unknown sequence number = %d\n",
            __FUNCTION__, pMsgGen->hdr.seqNumId);
        return NV_ERR_GENERIC;
    }

    // If response was requested
    if (pSeqInfo->pMsgResp != NULL)
    {
        NVSWITCH_ASSERT(pSeqInfo->pMsgResp->hdr.size == pMsgGen->hdr.size);
        msgSize = pMsgGen->hdr.size - RM_FLCN_QUEUE_HDR_SIZE;
        nvswitch_os_memcpy(&pSeqInfo->pMsgResp->msg, &pMsgGen->msg, msgSize);
    }

    //
    // Make a client callback to notify the client that the command has
    // completed.  Also provide the private client data, the sequence
    // descriptor of the completed command, and the error status.
    //
    if (pSeqInfo->pCallback != NULL)
    {
        pSeqInfo->pCallback(device, pMsg,
                            pSeqInfo->pCallbackParams,
                            pSeqInfo->seqDesc,
                            status);
    }

    status = flcnQueueSeqInfoFree(device, pFlcn, pSeqInfo);
    // We do not check status since we want to continue with clean-up items.

    // release the sequence so that it may be used for other commands
    flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);

    return status;
}

/*!
 * FLCN interface for retrieving status on a currently queued command
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   seqDesc The identifier that was assigned to the command when it
 *                      was submitted.
 *
 * @return  Command's current status, see FLCN_CMD_STATE_<XYZ>
 */
static NvU32
_flcnQueueCmdStatus_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo = NULL;
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;
    NvU32               seqStatus  = FLCN_CMD_STATE_NONE;

    NVSWITCH_ASSERT(pQueueInfo != NULL);

    // attempt to find the sequence-info data for the given sequence
    pSeqInfo = flcnQueueSeqInfoFind(device, pFlcn, seqDesc);
    if (pSeqInfo == NULL)
    {
        // sequence completed/done
        if (seqDesc < pQueueInfo->nextSeqDesc)
        {
            seqStatus = FLCN_CMD_STATE_DONE;
        }
        // sequence never submitted
        else
        {
            seqStatus = FLCN_CMD_STATE_NONE;
        }
    }
    else
    {
        switch (pSeqInfo->seqState)
        {
            case FLCN_QMGR_SEQ_STATE_FREE:
            case FLCN_QMGR_SEQ_STATE_CANCELED:
            {
                 seqStatus = FLCN_CMD_STATE_DONE;
                 break;
            }
            case FLCN_QMGR_SEQ_STATE_PENDING:
            case FLCN_QMGR_SEQ_STATE_USED:
            {
                 seqStatus= FLCN_CMD_STATE_RUNNING;
                 break;
            }
        }
    }

    return seqStatus;
}

/*!
 * FLCN interface for cancelling previously submitted/posted command
 *
 * Cancelling a command is the way for a caller to indicate that they no longer
 * desire to be notified when a command they previously submitted completes
 * and to request that no processing occur on the data returned by the falcon for
 * that command.  This function does not affect processing of the command on
 * the falcon. The falcon will continue to operate on the command in the same manner
 * as it would if the command had not been cancelled.
 *
 * @param[in]      device   nvswitch device pointer
 * @param[in]      pFlcn    FLCN object pointer
 * @param[in]      seqDesc  The identifier that was assigned to the command
 *                          when it was submitted.
 *
 * @return  NV_OK
 *     falcon command cancelled
 *
 * @return  NV_ERR_OBJECT_NOT_FOUND
 *     The provided command-descriptor does not correspond to any in-flight
 *     commands.
 */
static NV_STATUS
_flcnQueueCmdCancel_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc
)
{
    PFLCN_QMGR_SEQ_INFO pSeqInfo = NULL;

    // attempt to find the sequence-info data for the given sequence
    pSeqInfo = flcnQueueSeqInfoFind(device, pFlcn, seqDesc);
    if (pSeqInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    //
    // Mark the sequence as 'canceled' so that we do not attempt to process
    // command response when its received.
    //
    pSeqInfo->seqState = FLCN_QMGR_SEQ_STATE_CANCELED;
    return NV_OK;
}

/*!
 * @brief   Post a non-blocking command to the FLCN CMD queue(s) for processing.
 *
 * The FLCN interface for submitting a (non-blocking) command to the FLCN HW.
 * This interface is also used for setting the callback that will occur when
 * command has completed.  The client must specify a callback function, any
 * private arguments that should be passed in the callback as well as all
 * necessary pre-allocated buffers for storing the command message and message
 * payload (if applicable).  For commands requiring input payloads or output
 * payloads (or both), the client must provide the command offsets to the
 * allocation structures that contain all needed information describing the
 * allocation (size and location).
 *
 * @param[in]       device  nvswitch device pointer
 * @param[in,out]   pFlcn   FLCN object pointer
 * @param[in]       pCmd    Buffer containing raw command data
 * @param[in,out]   pMsg
 *      Buffer (may be NULL) that will be filled in response to execution of
 *      the command (ie. the command's output, initialized by the caller).
 * @param[in]       pPayload
 *      Optional pointer (may be NULL) to an engine specific structure that
 *      describes any input and output payloads that may be associated with
 *      the command being submitted.
 * @param[in]       queueIdLogical
 *      The logical identifier for the command queue this command is destined
 *      for. Note that only RM command queues are allowed.
 * @param[in]       pCallback
 *      Specifies the optional (may be NULL) callback function that will be
 *      called when the command completes.
 * @param[in]       pCallbackParams
 *      Additional optional (may be NULL) parameters that will be blindly passed
 *      to the callback function. This API will not use these parameters at all.
 * @param[out]      pSeqDesc
 *      All commands submitted to the FLCN will be assigned a unique identifier
 *      before being queued. This identifier will be stored by this pointer and
 *      may be later used to query for command's status.
 * @param[in]       pTimeout
 *      An optional pointer (may be NULL) to a pre-configured timeout structure
 *      that when non-NULL is be used to indicate that blocking behavior is
 *      allowed (within the bounds of the timeout) for operations that have the
 *      potential to fail on transient conditions and can be retried (mutex
 *      acquirement, queue insertion, etc ...).  When NULL, this function does
 *      NOT retry such operations when they fail (the function becomes non-
 *      blocking) and returns back control to the caller.  Example scenarios
 *      include when the command queue mutex can not be obtained and if the
 *      queue does not have enough free space to fit the command.
 *
 * @return  'NV_OK'
 *      If the command is successfully written to the command queue.
 *
 * @return  'NV_ERR_INSUFFICIENT_RESOURCES'
 *      If the command could not be queued as a result of the target command
 *      queue having insufficient space to fit the command.  Could be after
 *      the initial queue attempt if the non-blocking behavior has been
 *      requested or after successive retries if the timeout expired before
 *      enough space was free'd in the queue.
 *
 * @return  'NV_ERR_STATE_IN_USE'
 *      If the command couldn't be queued due to a failure to successfully
 *      acquire the mutex which protects access to the target command queue.
 *      Same comments as above on blocking and non-blocking behavior apply.
 */
static NV_STATUS
_flcnQueueCmdPostNonBlocking_IMPL
(
    nvswitch_device        *device,
    PFLCN                   pFlcn,
    PRM_FLCN_CMD            pCmd,
    PRM_FLCN_MSG            pMsg,
    void                   *pPayload,
    NvU32                   queueIdLogical,
    FlcnQMgrClientCallback  pCallback,
    void                   *pCallbackParams,
    NvU32                  *pSeqDesc,
    NVSWITCH_TIMEOUT       *pTimeout
)
{
    PFALCON_QUEUE_INFO  pQueueInfo;
    PFLCN_QMGR_SEQ_INFO pSeqInfo = NULL;
    PFLCNQUEUE          pQueue;
    NV_STATUS           status;

    // Sanity check the object pointers.
    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return NV_ERR_INVALID_STATE;
    }

    pQueueInfo = pFlcn->pQueueInfo;
    if (pQueueInfo == NULL)
    {
        NVSWITCH_ASSERT(pQueueInfo != NULL);
        return NV_ERR_INVALID_STATE;
    }

    pQueue     = &pQueueInfo->pQueues[queueIdLogical];

    // Sequence descriptor pointer may never be NULL.
    if (pSeqDesc == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Falcon must be in a ready state before commands may be submitted.
    status = soeWaitForInitAck_HAL(device,  (PSOE)pFlcn->pFlcnable);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE not ready for command processing\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return status;
    }

    // Sanity check the command input.
    if (!_flcnQueueCmdValidate(device, pFlcn, pCmd, pMsg, pPayload, queueIdLogical))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: illformed command request. Skipping.\n",
            __FUNCTION__);
        status = NV_ERR_INVALID_ARGUMENT;
        goto flcnQueueCmdPostNonBlocking_exit;
    }

    // Attempt to reserve a sequence for this command.
    pSeqInfo = flcnQueueSeqInfoAcq(device, pFlcn);
    if (pSeqInfo == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: could not generate a sequence ID for the command\n",
            __FUNCTION__);
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto flcnQueueCmdPostNonBlocking_exit;
    }

    // Set the sequence number in the command header.
    pCmd->cmdGen.hdr.seqNumId = pSeqInfo->seqNum;

    //
    // Set the control flags in the command header so that we get a status
    // message in the Message Queue and an interrupt when the command completes.
    //
    pCmd->cmdGen.hdr.ctrlFlags = RM_FLCN_QUEUE_HDR_FLAGS_STATUS;

    // Save the Queue in the Seq structure.
    pSeqInfo->pCmdQueue = pQueue;

    //
    // Perform all necessary bookkeeping work before enqueuing the command.
    // This must be done first to protect against the case where the FLCN
    // processes the command before we have the chance to save-off the client
    // callback information.
    //
    pSeqInfo->pCallback       = pCallback;
    pSeqInfo->pCallbackParams = pCallbackParams;
    pSeqInfo->pMsgResp        = (RM_FLCN_MSG_GEN *)pMsg;
    pSeqInfo->seqDesc         = pQueueInfo->nextSeqDesc++;

    // Set the sequence descriptor return value.
    *pSeqDesc = pSeqInfo->seqDesc;

    // Engine specific command post handling.
    status = flcnableQueueCmdPostExtension(device, pFlcn->pFlcnable, pCmd,
                                           pMsg, pPayload, pTimeout, pSeqInfo);
    if (NV_OK != status)
    {
        flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);
        goto flcnQueueCmdPostNonBlocking_exit;
    }

    // Enqueue the command in the target FLCN command queue.
    status = flcnQueueCmdWrite(device, pFlcn, queueIdLogical, pCmd, pTimeout);
    if (status == NV_OK)
    {
        pSeqInfo->seqState = FLCN_QMGR_SEQ_STATE_USED;
    }
    else
    {
        // On failure cleanup any allocations and release the sequence number.
        (void)flcnQueueSeqInfoFree(device, pFlcn, pSeqInfo);
        // Already have not OK status from flcnQueueCmdWrite

        flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);
    }

flcnQueueCmdPostNonBlocking_exit:
    if (NV_OK != status)
    {
        if (NULL != pSeqInfo)
        {
            flcnQueueSeqInfoRel(device, pFlcn, pSeqInfo);
        }
    }
    return status;
}

/*!
 * @brief   Validate that the basic CMD params are properly formed.
 *
 * @return  Boolean if command was properly formed.
 */
static NvBool
_flcnQueueCmdValidate
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    PRM_FLCN_CMD     pCmd,
    PRM_FLCN_MSG     pMsg,
    void            *pPayload,
    NvU32            queueIdLogical
)
{
    // Command pointer may never be NULL.
    if (pCmd == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: command pointer is NULL\n",
            __FUNCTION__);
        return NV_FALSE;
    }

    // Each command must contain a header (at minimum).
    if (pCmd->cmdGen.hdr.size < RM_FLCN_QUEUE_HDR_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid command (illegal size = 0x%x)\n",
            __FUNCTION__, pCmd->cmdGen.hdr.size);
        return NV_FALSE;
    }

    // Call engine specific command input validation.
    return flcnableQueueCmdValidate(device, pFlcn->pFlcnable, pCmd, pMsg,
                                    pPayload, queueIdLogical);
}

/*!
 * @brief   Wait for a command to complete on the falcon.
 *
 * Continuously processes messages posted to the message-queue waiting for the
 * falcon to post the response to the command associated with 'seqDesc' or until
 * a timeout occurs. This function does not return the data the falcon responded
 * with.
 *
 * @note:   Use this function sparingly. It should generally only be used when a
 * synchronous response from the falcon is absolutely required. Common use-cases
 * are processing of RmCtrl-s where call cannot return until a command completes
 * or when executing some linear process where the next step the sequence cannot
 * commence until a previously issued command completes.
 *
 * @param[in]       device      nvswitch device pointer
 * @param[in]       pFlcn       FLCN object pointer
 * @param[in]       seqDesc     Command identifier issued when it was submitted
 * @param[in,out]   pTimeout    Timeout struct. used while waiting for completion
 *
 * @return  NV_OK
 *      Falcon command completion received.
 *
 * @return  NV_ERR_INVALID_REQUEST
 *      The command-descriptor doesn't correspond to any in-flight commands.
 *
 * @return  NV_ERR_TIMEOUT
 *      A timeout occurred before the command completed.
 */
static NV_STATUS
_flcnQueueCmdWait_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            seqDesc,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NvBool bKeepPolling;

    if (_flcnQueueCmdStatus_IMPL(device, pFlcn, seqDesc) == FLCN_CMD_STATE_NONE)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    do
    {
        bKeepPolling = (nvswitch_timeout_check(pTimeout)) ? NV_FALSE : NV_TRUE;

        //
        // Directly invoke interrupt handler to process FLCN response.
        // This is needed for the following scenarios:
        // 1. When interrupts are disabled (init path)
        // 2. When interrupts are enabled (ioctl, background task), we hold
        //    the device_mutex which nvswitch_isr_thread also tries to acquire.
        //
        soeService_HAL(device, (PSOE)pFlcn->pFlcnable);

        if (_flcnQueueCmdStatus_IMPL(device, pFlcn, seqDesc) == FLCN_CMD_STATE_DONE)
        {
            return NV_OK;
        }
    } while (bKeepPolling);

    return NV_ERR_TIMEOUT;
}

void
flcnQueueSetupHal
(
    FLCN   *pFlcn
)
{
    flcn_hal *pHal = pFlcn->pHal;

    pHal->queueCmdWrite           = _flcnQueueCmdWrite_IMPL;

    pHal->queueSeqInfoFind        = _flcnQueueSeqInfoFind_IMPL;
    pHal->queueSeqInfoAcq         = _flcnQueueSeqInfoAcq_IMPL;
    pHal->queueSeqInfoRel         = _flcnQueueSeqInfoRel_IMPL;
    pHal->queueSeqInfoStateInit   = _flcnQueueSeqInfoStateInit_IMPL;
    pHal->queueSeqInfoCancelAll   = _flcnQueueSeqInfoCancelAll_IMPL;
    pHal->queueSeqInfoFree        = _flcnQueueSeqInfoFree_IMPL;

    pHal->queueEventRegister      = _flcnQueueEventRegister_IMPL;
    pHal->queueEventUnregister    = _flcnQueueEventUnregister_IMPL;
    pHal->queueEventHandle        = _flcnQueueEventHandle_IMPL;
    pHal->queueResponseHandle     = _flcnQueueResponseHandle_IMPL;

    pHal->queueCmdStatus          = _flcnQueueCmdStatus_IMPL;
    pHal->queueCmdCancel          = _flcnQueueCmdCancel_IMPL;
    pHal->queueCmdPostNonBlocking = _flcnQueueCmdPostNonBlocking_IMPL;
    pHal->queueCmdWait            = _flcnQueueCmdWait_IMPL;
}
