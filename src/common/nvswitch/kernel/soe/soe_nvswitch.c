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

#include "soe/soe_nvswitch.h"
#include "soe/soe_priv_nvswitch.h"

#include "flcn/haldefs_flcnable_nvswitch.h"
#include "flcn/haldefs_flcn_nvswitch.h"
#include "flcn/flcn_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"
#include "common_nvswitch.h"

static NV_STATUS _soeGetInitMessage(nvswitch_device *device, PSOE pSoe, RM_FLCN_MSG_SOE *pMsg);

/*!
 * Use the SOE INIT Message to construct and initialize all SOE Queues.
 *
 * @param[in]      device nvswitch_device pointer
 * @param[in]      pSoe   SOE object pointer
 * @param[in]      pMsg   Pointer to the INIT Message
 *
 * @return 'NV_OK' upon successful creation of all SOE Queues
 */
static NV_STATUS
_soeQMgrCreateQueuesFromInitMsg
(
    nvswitch_device  *device,
    PFLCNABLE         pSoe,
    RM_FLCN_MSG_SOE  *pMsg
)
{
    RM_SOE_INIT_MSG_SOE_INIT *pInit;
    NvU32                     i;
    NvU32                     queueLogId;
    NV_STATUS                 status;
    FLCNQUEUE                *pQueue;
    PFLCN                     pFlcn = ENG_GET_FLCN(pSoe);
    PFALCON_QUEUE_INFO        pQueueInfo;

    NVSWITCH_ASSERT(pFlcn != NULL);

    pQueueInfo = pFlcn->pQueueInfo;
    NVSWITCH_ASSERT(pQueueInfo != NULL);

    pInit = &pMsg->msg.init.soeInit;
    NVSWITCH_ASSERT(pInit->numQueues <= pFlcn->numQueues);

    for (i = 0; i < pFlcn->numQueues; i++)
    {
        queueLogId = pInit->qInfo[i].queueLogId;
        NVSWITCH_ASSERT(queueLogId < pFlcn->numQueues);
        pQueue = &pQueueInfo->pQueues[queueLogId];
        status = flcnQueueConstruct_dmem_nvswitch(
                     device,
                     pFlcn,
                     &pQueue,                                  // ppQueue
                     queueLogId,                               // Logical ID of the queue
                     pInit->qInfo[i].queuePhyId,               // Physical ID of the queue
                     pInit->qInfo[i].queueOffset,              // offset
                     pInit->qInfo[i].queueSize,                // size
                     RM_FLCN_QUEUE_HDR_SIZE);                  // cmdHdrSize
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                        "%s: Error constructing SOE Queue (status="
                        "0x%08x).\n", __FUNCTION__, status);
            NVSWITCH_ASSERT(0);
            return status;
        }
    }
    return NV_OK;
}

/*!
 * Purges all the messages from the SOE's message queue.  Each message will
 * be analyzed, clients will be notified of status, and events will be routed
 * to all registered event listeners.
 *
 * @param[in]  device nvswitch_device pointer
 * @param[in]  pSoe   SOE object pointer
 *
 * @return 'NV_OK' if the message queue was successfully purged.
 */
static NV_STATUS
_soeProcessMessages_IMPL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    RM_FLCN_MSG_SOE  soeMessage;
    NV_STATUS        status;
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);

    // keep processing messages until no more exist in the message queue
    while (NV_OK == (status = flcnQueueReadData(
                                     device,
                                     pFlcn,
                                     SOE_RM_MSGQ_LOG_ID,
                                     (RM_FLCN_MSG *)&soeMessage, NV_TRUE)))
    {
        NVSWITCH_PRINT(device, INFO,
                    "%s: unitId=0x%02x, size=0x%02x, ctrlFlags=0x%02x, " \
                    "seqNumId=0x%02x\n",
                    __FUNCTION__,
                    soeMessage.hdr.unitId,
                    soeMessage.hdr.size,
                    soeMessage.hdr.ctrlFlags,
                    soeMessage.hdr.seqNumId);

        // check to see if the message is a reply or an event.
        if ((soeMessage.hdr.ctrlFlags &= RM_FLCN_QUEUE_HDR_FLAGS_EVENT) != 0)
        {
            flcnQueueEventHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage, NV_OK);
        }
        // the message is a response from a previously queued command
        else
        {
            flcnQueueResponseHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage);
        }
    }

    //
    // Status NV_ERR_NOT_READY implies, Queue is empty.
    // Log the message in other error cases.
    //
    if (status != NV_ERR_NOT_READY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: unexpected error while purging message queue (status=0x%x).\n",
            __FUNCTION__, (status));
    }

    return status;
}

/*!
 * This function exists to solve a natural chicken-and-egg problem that arises
 * due to the fact that queue information (location, size, id, etc...) is
 * relayed to the RM as a message in a queue.  Queue construction is done when
 * the message arives and the normal queue read/write functions are not
 * available until construction is complete.  Construction cannot be done until
 * the message is read from the queue.  Therefore, the very first message read
 * from the Message Queue must be considered as a special-case and must NOT use
 * any functionality provided by the SOE's queue manager.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 *
 * @return 'NV_OK'
 *     Upon successful extraction and processing of the first SOE message.
 */
static NV_STATUS
_soeProcessMessagesPreInit_IMPL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    RM_FLCN_MSG_SOE   msg;
    NV_STATUS        status;
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);

    // extract the "INIT" message (this is never expected to fail)
    status = _soeGetInitMessage(device, pSoe, &msg);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to extract the INIT message "
                    "from the SOE Message Queue (status=0x%08x).",
                    __FUNCTION__, status);
        NVSWITCH_ASSERT(0);
        return status;
    }

    //
    // Now hookup the "real" message-processing function and handle the "INIT"
    // message.
    //
    pSoe->base.pHal->processMessages = _soeProcessMessages_IMPL;
    return flcnQueueEventHandle(device, pFlcn, (RM_FLCN_MSG *)&msg, NV_OK);
}

/*!
 * @brief Process the "INIT" message sent from the SOE ucode application.
 *
 * When the SOE ucode is done initializing, it will post an INIT message in
 * the Message Queue that contains all the necessary attributes that are
 * needed to enqueuing commands and extracting messages from the queues.
 * The packet will also contain the offset and size of portion of DMEM that
 * the RM must manage.  Upon receiving this message it will be assume that
 * the SOE is ready to start accepting commands.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 * @param[in]  pMsg    Pointer to the event's message data
 *
 * @return 'NV_OK' if the event was successfully handled.
 */
static NV_STATUS
_soeHandleInitEvent_IMPL
(
    nvswitch_device  *device,
    PFLCNABLE         pSoe,
    RM_FLCN_MSG      *pGenMsg
)
{
    NV_STATUS         status;
    PFLCN             pFlcn = ENG_GET_FLCN(pSoe);
    RM_FLCN_MSG_SOE *pMsg  = (RM_FLCN_MSG_SOE *)pGenMsg;

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    NVSWITCH_PRINT(device, INFO,
                "%s: Received INIT message from SOE\n",
                __FUNCTION__);

    //
    // Pass the INIT message to the queue manager to allow it to create the
    // queues.
    //
    status = _soeQMgrCreateQueuesFromInitMsg(device, pSoe, pMsg);
    if (status != NV_OK)
    {
        NVSWITCH_ASSERT(0);
        return status;
    }

    flcnDbgInfoDmemOffsetSet(device, pFlcn,
        pMsg->msg.init.soeInit.osDebugEntryPoint);

    // the SOE ucode is now initialized and ready to accept commands
    pFlcn->bOSReady = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Read the INIT message directly out of the Message Queue.
 *
 * This function accesses the Message Queue directly using the HAL.  It does
 * NOT and may NOT use the queue manager as it has not yet been constructed and
 * initialized.  The Message Queue may not be empty when this function is called
 * and the first message in the queue MUST be the INIT message.
 *
 * @param[in]   device  nvswitch_device pointer
 * @param[in]   pSoe    SOE object pointer
 * @param[out]  pMsg    Message structure to fill with the INIT message data
 *
 * @return 'NV_OK' upon successful extraction of the INIT message.
 * @return
 *     'NV_ERR_INVALID_STATE' if the first message found was not an INIT
 *     message or if the message was improperly formatted.
 */
static NV_STATUS
_soeGetInitMessage
(
    nvswitch_device  *device,
    PSOE              pSoe,
    RM_FLCN_MSG_SOE  *pMsg
)
{
    PFLCN               pFlcn   = ENG_GET_FLCN(pSoe);
    NV_STATUS           status  = NV_OK;
    NvU32               tail    = 0;
    PFALCON_QUEUE_INFO  pQueueInfo;
    // on the GPU, rmEmemPortId = sec2RmEmemPortIdGet_HAL(...);
    NvU8                rmEmemPortId = 0;

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    pQueueInfo = pFlcn->pQueueInfo;
    if (pQueueInfo == NULL)
    {
        NVSWITCH_ASSERT(pQueueInfo != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    //
    // Message queue 0 is used by SOE to communicate with RM
    // Check SOE_CMDMGMT_MSG_QUEUE_RM in //uproc/soe/inc/soe_cmdmgmt.h
    //
    pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID].queuePhyId = 0;

    // read the header starting at the current tail position
    (void)flcnMsgQueueTailGet(device, pFlcn,
        &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID], &tail);
    if (pFlcn->bEmemEnabled)
    {
        //
        // We use the offset in DMEM for the src address, since
        // EmemCopyFrom automatically converts it to the offset in EMEM
        //
        flcnableEmemCopyFrom(
            device, pFlcn->pFlcnable,
            tail,                   // src
            (NvU8 *)&pMsg->hdr,     // pDst
            RM_FLCN_QUEUE_HDR_SIZE, // numBytes
            rmEmemPortId);          // port
    }
    else
    {
        status = flcnDmemCopyFrom(device,
                                  pFlcn,
                                  tail,                     // src
                                  (NvU8 *)&pMsg->hdr,       // pDst
                                  RM_FLCN_QUEUE_HDR_SIZE,   // numBytes
                                  0);                       // port
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to copy from SOE DMEM\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
            goto _soeGetInitMessage_exit;
        }
    }

    if (pMsg->hdr.unitId != RM_SOE_UNIT_INIT)
    {
        status = NV_ERR_INVALID_STATE;
        NVSWITCH_ASSERT(0);
        goto _soeGetInitMessage_exit;
    }

    // read the message body and update the tail position
    if (pFlcn->bEmemEnabled)
    {
        //
        // We use the offset in DMEM for the src address, since
        // EmemCopyFrom automatically converts it to the offset in EMEM
        //
        flcnableEmemCopyFrom(
            device, pFlcn->pFlcnable,
            tail + RM_FLCN_QUEUE_HDR_SIZE,              // src
            (NvU8 *)&pMsg->msg,                         // pDst
            pMsg->hdr.size - RM_FLCN_QUEUE_HDR_SIZE,    // numBytes
            rmEmemPortId);                              // port
    }
    else
    {
        status = flcnDmemCopyFrom(device,
            pFlcn,
            tail + RM_FLCN_QUEUE_HDR_SIZE,              // src
            (NvU8 *)&pMsg->msg,                         // pDst
            pMsg->hdr.size - RM_FLCN_QUEUE_HDR_SIZE,    // numBytes
            0);                                         // port
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to copy from SOE DMEM\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
            goto _soeGetInitMessage_exit;
        }
    }

    tail += NV_ALIGN_UP(pMsg->hdr.size, SOE_DMEM_ALIGNMENT);
    flcnMsgQueueTailSet(device, pFlcn,
        &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID], tail);

_soeGetInitMessage_exit:
    return status;
}

/*!
 * Copies 'sizeBytes' from DMEM address 'src' to 'pDst' using EMEM access port.
 *
 * The address must be located in the EMEM region located directly above the
 * maximum virtual address of DMEM.
 *
 * @param[in]   device      nvswitch_device pointer
 * @param[in]   pSoe        SOE pointer
 * @param[in]   src         The DMEM address for the source of the copy
 * @param[out]  pDst        Pointer to write with copied data from EMEM
 * @param[in]   sizeBytes   The number of bytes to copy from EMEM
 * @param[in]   port        EMEM port
 */
static void
_soeEmemCopyFrom_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               src,
    NvU8               *pDst,
    NvU32               sizeBytes,
    NvU8                port
)
{
    soeEmemTransfer_HAL(device, (PSOE)pSoe, src, pDst, sizeBytes, port, NV_TRUE);
}

/*!
 * Copies 'sizeBytes' from 'pDst' to DMEM address 'dst' using EMEM access port.
 *
 * The address must be located in the EMEM region located directly above the
 * maximum virtual address of DMEM.
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 * @param[in]  dst         The DMEM address for the copy destination.
 * @param[in]  pSrc        The pointer to the buffer containing the data to copy
 * @param[in]  sizeBytes   The number of bytes to copy into EMEM
 * @param[in]  port        EMEM port
 */
static void
_soeEmemCopyTo_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               dst,
    NvU8               *pSrc,
    NvU32               sizeBytes,
    NvU8                port
)
{
    soeEmemTransfer_HAL(device, (PSOE)pSoe, dst, pSrc, sizeBytes, port, NV_FALSE);
}

/*!
 * Loop until SOE RTOS is loaded and gives us an INIT message
 *
 * @param[in]  device  nvswitch_device object pointer
 * @param[in]  pSoe    SOE object pointer
 */
static NV_STATUS
_soeWaitForInitAck_IMPL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);
    // POBJMC           pMc    = GPU_GET_MC(device);
    NVSWITCH_TIMEOUT timeout;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    while (!pFlcn->bOSReady && !nvswitch_timeout_check(&timeout))
    {
        // Once interrupt handling is ready, might need to replace this with
        //mcServiceSingle_HAL(device, pMc, MC_ENGINE_IDX_SOE, NV_FALSE);
        soeService_HAL(device, pSoe);
        nvswitch_os_sleep(1);
    }

    if (!pFlcn->bOSReady)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s Timeout while waiting for SOE bootup\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return NV_ERR_TIMEOUT;
    }
    return NV_OK;
}

/*!
 * @brief   Retrieves a pointer to the engine specific SEQ_INFO structure.
 *
 * @param[in]   device      nvswitch_device pointer
 * @param[in]   pSoe        SOE pointer
 * @param[in]   seqIndex    Index of the structure to retrieve
 *
 * @return  Pointer to the SEQ_INFO structure or NULL on invalid index.
 */
static PFLCN_QMGR_SEQ_INFO
_soeQueueSeqInfoGet_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               seqIndex
)
{
    FLCN *pFlcn = ENG_GET_FLCN(pSoe);

    if (seqIndex < pFlcn->numSequences)
    {
        return &(((PSOE)pSoe)->seqInfo[seqIndex]);
    }
    return NULL;
}

/*!
 * @copydoc flcnableQueueCmdValidate_IMPL
 */
static NvBool
_soeQueueCmdValidate_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    PRM_FLCN_CMD        pCmd,
    PRM_FLCN_MSG        pMsg,
    void               *pPayload,
    NvU32               queueIdLogical
)
{
    PFLCN       pFlcn   = ENG_GET_FLCN(pSoe);
    FLCNQUEUE  *pQueue  = &pFlcn->pQueueInfo->pQueues[queueIdLogical];
    NvU32       cmdSize = pCmd->cmdGen.hdr.size;

    // Verify that the target queue ID represents a valid RM queue.
    if (queueIdLogical != SOE_RM_CMDQ_LOG_ID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid SOE command queue ID = 0x%x\n",
            __FUNCTION__, queueIdLogical);
        return NV_FALSE;
    }

    //
    // Command size cannot be larger than queue size / 2. Otherwise, it is
    // impossible to send two commands back to back if we start from the
    // beginning of the queue.
    //
    if (cmdSize > (pQueue->queueSize / 2))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid command (illegal size = 0x%x)\n",
            __FUNCTION__, cmdSize);
        return NV_FALSE;
    }

    // Validate the command's unit identifier.
    if (!RM_SOE_UNITID_IS_VALID(pCmd->cmdGen.hdr.unitId))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid unitID = %d\n",
            __FUNCTION__, pCmd->cmdGen.hdr.unitId);
        return NV_FALSE;
    }

    return NV_TRUE;
}

/* -------------------- Object construction/initialization ------------------- */

static void
soeSetupHal
(
    SOE      *pSoe,
    NvU32     pci_device_id
)
{
    soe_hal *pHal = NULL;
    flcnable_hal *pParentHal = NULL;

    if (nvswitch_is_lr10_device_id(pci_device_id))
    {
        soeSetupHal_LR10(pSoe);
    }
    else
    {
        // we're on a device which doesn't support SOE
        NVSWITCH_PRINT(NULL, ERROR, "Tried to initialize SOE on device with no SOE\n");
        NVSWITCH_ASSERT(0);
    }

    pHal = pSoe->base.pHal;
    pParentHal = (flcnable_hal *)pHal;
    //set any functions we want to override
    pParentHal->handleInitEvent     = _soeHandleInitEvent_IMPL;
    pParentHal->ememCopyTo          = _soeEmemCopyTo_IMPL;
    pParentHal->ememCopyFrom        = _soeEmemCopyFrom_IMPL;
    pParentHal->queueSeqInfoGet     = _soeQueueSeqInfoGet_IMPL;
    pParentHal->queueCmdValidate    = _soeQueueCmdValidate_IMPL;

    //set any functions specific to SOE
    pHal->processMessages           = _soeProcessMessagesPreInit_IMPL;
    pHal->waitForInitAck            = _soeWaitForInitAck_IMPL;
}

SOE *
soeAllocNew(void)
{
    SOE *pSoe = nvswitch_os_malloc(sizeof(*pSoe));
    if (pSoe != NULL)
    {
        nvswitch_os_memset(pSoe, 0, sizeof(*pSoe));
    }

    return pSoe;
}

NvlStatus
soeInit
(
    nvswitch_device    *device,
    SOE                *pSoe,
    NvU32               pci_device_id
)
{
    NvlStatus retval;

    // allocate hal if a child class hasn't already
    if (pSoe->base.pHal == NULL)
    {
        soe_hal *pHal = pSoe->base.pHal = nvswitch_os_malloc(sizeof(*pHal));
        if (pHal == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Flcn allocation failed!\n");
            retval = -NVL_NO_MEM;
            goto soe_init_fail;
        }
        nvswitch_os_memset(pHal, 0, sizeof(*pHal));
    }

    // init parent class
    retval = flcnableInit(device, (PFLCNABLE)pSoe, pci_device_id);
    if (retval != NVL_SUCCESS)
    {
        goto soe_init_fail;
    }

    soeSetupHal(pSoe, pci_device_id);

    return retval;
soe_init_fail:
    soeDestroy(device, pSoe);
    return retval;
}

// reverse of soeInit()
void
soeDestroy
(
    nvswitch_device    *device,
    SOE                *pSoe
)
{
    // destroy parent class
    flcnableDestroy(device, (PFLCNABLE)pSoe);

    if (pSoe->base.pHal != NULL)
    {
        nvswitch_os_free(pSoe->base.pHal);
        pSoe->base.pHal = NULL;
    }
}
