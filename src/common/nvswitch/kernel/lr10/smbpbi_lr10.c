/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/smbpbi_lr10.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_nvlsaw_ip_addendum.h"

#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

#define GET_PFIFO_FROM_DEVICE(dev)  (&(dev)->pSmbpbi->sharedSurface->inforomObjects.DEM.object.v1)

#define DEM_FIFO_SIZE   INFOROM_DEM_OBJECT_V1_00_FIFO_SIZE
#define DEM_FIFO_PTR(x) ((x) % DEM_FIFO_SIZE)
#define DEM_PTR_DIFF(cur, next) (((next) > (cur)) ? ((next) - (cur)) :      \
                    (DEM_FIFO_SIZE - ((cur) - (next))))
#define DEM_BYTES_OCCUPIED(pf) DEM_PTR_DIFF((pf)->readOffset, (pf)->writeOffset)
//
// See how much space is available in the FIFO.
// Must leave 1 word free so the write pointer does not
// catch up with the read pointer. That would be indistinguishable
// from an empty FIFO.
//
#define DEM_BYTES_AVAILABLE(pf) (DEM_PTR_DIFF((pf)->writeOffset, (pf)->readOffset) - \
                                 sizeof(NvU32))
#define DEM_RECORD_SIZE_MAX (sizeof(NV_MSGBOX_DEM_RECORD)   \
                            + NV_MSGBOX_MAX_DRIVER_EVENT_MSG_TXT_SIZE)
#define DEM_RECORD_SIZE_MIN (sizeof(NV_MSGBOX_DEM_RECORD) + 1)

#define FIFO_REC_LOOP_ITERATOR  _curPtr
#define FIFO_REC_LOOP_REC_PTR   _recPtr
#define FIFO_REC_LOOP_REC_SIZE  _recSize
#define FIFO_REC_LOOP_START(pf, cond)                                                           \
{                                                                                               \
    NvU16                           _nextPtr;                                                   \
    for (FIFO_REC_LOOP_ITERATOR = (pf)->readOffset; cond; FIFO_REC_LOOP_ITERATOR = _nextPtr)    \
    {                                                                                           \
        NV_MSGBOX_DEM_RECORD    *FIFO_REC_LOOP_REC_PTR = (NV_MSGBOX_DEM_RECORD *)               \
                                            ((pf)->fifoBuffer + FIFO_REC_LOOP_ITERATOR);        \
        NvU16                   FIFO_REC_LOOP_REC_SIZE =                                        \
                                    FIFO_REC_LOOP_REC_PTR->recordSize * sizeof(NvU32);

#define FIFO_REC_LOOP_END                                                                       \
        _nextPtr = DEM_FIFO_PTR(FIFO_REC_LOOP_ITERATOR + FIFO_REC_LOOP_REC_SIZE);               \
    }                                                                                           \
}

static void _smbpbiDemInit(nvswitch_device *device, struct smbpbi *pSmbpbi, struct INFOROM_DEM_OBJECT_V1_00 *pFifo);

NvlStatus
nvswitch_smbpbi_alloc_lr10
(
    nvswitch_device *device
)
{
    NV_STATUS   status;
    NvU64       dmaHandle;
    void        *cpuAddr;

    // Create DMA mapping for SMBPBI transactions
    status = nvswitch_os_alloc_contig_memory(device->os_handle, &cpuAddr,
                                        sizeof(SOE_SMBPBI_SHARED_SURFACE),
                                        (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to allocate contig memory, rc:%d\n",
                    status);
        return status;
    }

    nvswitch_os_memset(cpuAddr, 0, sizeof(SOE_SMBPBI_SHARED_SURFACE));

    status = nvswitch_os_map_dma_region(device->os_handle, cpuAddr, &dmaHandle,
                        sizeof(SOE_SMBPBI_SHARED_SURFACE),
                        NVSWITCH_DMA_DIR_BIDIRECTIONAL);
    if (status == NVL_SUCCESS)
    {
        device->pSmbpbi->sharedSurface = cpuAddr;
        device->pSmbpbi->dmaHandle = dmaHandle;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to map dma region for SMBPBI shared surface, rc:%d\n",
            status);
        nvswitch_os_free_contig_memory(device->os_handle, cpuAddr, sizeof(SOE_SMBPBI_SHARED_SURFACE));
    }

    return status;
}

NvlStatus
nvswitch_smbpbi_post_init_hal_lr10
(
    nvswitch_device *device
)
{
    struct smbpbi             *pSmbpbi = device->pSmbpbi;
    FLCN                      *pFlcn;
    NvU64                      dmaHandle;
    RM_FLCN_CMD_SOE            cmd;
    NVSWITCH_TIMEOUT           timeout;
    NvU32                      cmdSeqDesc;
    RM_SOE_SMBPBI_CMD_INIT    *pInitCmd = &cmd.cmd.smbpbiCmd.init;
    NvlStatus                  status;

    if (!device->pInforom || !device->pSmbpbi->sharedSurface)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Populate shared surface with static InfoROM data
    nvswitch_inforom_read_static_data(device, device->pInforom,
                                      &device->pSmbpbi->sharedSurface->inforomObjects);

    pFlcn = device->pSoe->pFlcn;
    dmaHandle = pSmbpbi->dmaHandle;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SMBPBI;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(SMBPBI, INIT);
    cmd.cmd.smbpbiCmd.cmdType = RM_SOE_SMBPBI_CMD_ID_INIT;
    RM_FLCN_U64_PACK(&pInitCmd->dmaHandle, &dmaHandle);

    //
    // Make the interval twice the heartbeat period to avoid
    // skew between driver and soe threads
    //
    pInitCmd->driverPollingPeriodUs = (NVSWITCH_HEARTBEAT_INTERVAL_NS / 1000) * 2;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: SMBPBI Init command failed. rc:%d\n",
                        __FUNCTION__, status);
    }

    return status;
}

void
nvswitch_smbpbi_destroy_hal_lr10
(
    nvswitch_device *device
)
{
    if (device->pSmbpbi && device->pSmbpbi->sharedSurface)
    {
        nvswitch_os_unmap_dma_region(device->os_handle,
                        device->pSmbpbi->sharedSurface,
                        device->pSmbpbi->dmaHandle,
                        sizeof(SOE_SMBPBI_SHARED_SURFACE),
                        NVSWITCH_DMA_DIR_BIDIRECTIONAL);
        nvswitch_os_free_contig_memory(device->os_handle, device->pSmbpbi->sharedSurface,
                        sizeof(SOE_SMBPBI_SHARED_SURFACE));
    }
}

NvlStatus
nvswitch_smbpbi_get_dem_num_messages_lr10
(
    nvswitch_device *device,
    NvU8            *pMsgCount
)
{
    NvU32 reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_12);

    *pMsgCount = DRF_VAL(_NVLSAW_SW, _SCRATCH_12, _EVENT_MESSAGE_COUNT, reg);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_smbpbi_dem_load_lr10
(
    nvswitch_device *device
)
{
    NvlStatus                           status;
    NvU8                                version = 0;
    NvU8                                subversion = 0;
    struct inforom                      *pInforom = device->pInforom;
    NvU8                                *pPackedObject = NULL;
    struct INFOROM_DEM_OBJECT_V1_00     *pFifo;

    if ((pInforom == NULL) || (device->pSmbpbi == NULL) ||
        (device->pSmbpbi->sharedSurface == NULL))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFifo = GET_PFIFO_FROM_DEVICE(device);

    status = nvswitch_inforom_get_object_version_info(device, "DEM", &version,
                                                    &subversion);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "no DEM object found, rc:%d\n", status);
        goto nvswitch_inforom_dem_load_fail;
    }

    if (!INFOROM_OBJECT_SUBVERSION_SUPPORTS_NVSWITCH(subversion))
    {
        NVSWITCH_PRINT(device, WARN, "DEM v%u.%u not supported\n",
                    version, subversion);
        status = -NVL_ERR_NOT_SUPPORTED;
        goto nvswitch_inforom_dem_load_fail;
    }

    NVSWITCH_PRINT(device, INFO, "DEM v%u.%u found\n", version, subversion);

    if (version != 1)
    {
        NVSWITCH_PRINT(device, WARN, "DEM v%u.%u not supported\n",
                    version, subversion);
        status = -NVL_ERR_NOT_SUPPORTED;
        goto nvswitch_inforom_dem_load_fail;
    }

    pPackedObject = nvswitch_os_malloc(INFOROM_DEM_OBJECT_V1_00_PACKED_SIZE);

    if (pPackedObject == NULL)
    {
        status = -NVL_NO_MEM;
        goto nvswitch_inforom_dem_load_fail;
    }

    status = nvswitch_inforom_load_object(device, pInforom, "DEM",
                                        INFOROM_DEM_OBJECT_V1_00_FMT,
                                        pPackedObject,
                                        pFifo);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load DEM object, rc: %d\n",
                        status);
        goto nvswitch_inforom_dem_load_fail;
    }

nvswitch_inforom_dem_load_fail:

    if (pPackedObject)
    {
        nvswitch_os_free(pPackedObject);
    }

    //
    // Mark the cached DEM as usable for Xid logging, even if we were
    // unable to find it in the InfoROM image.
    //

    device->pSmbpbi->sharedSurface->inforomObjects.DEM.bValid = NV_TRUE;

    _smbpbiDemInit(device, device->pSmbpbi, pFifo);

    return status;
}

/*!
 * Validate/Initialize the Driver Event Message (SXid) FIFO buffer
 *
 * @param[in]       device     device object pointer
 * @param[in]       pSmbpbi    SMBPBI object pointer
 * @param[in,out]   pFifo      DEM object pointer
 *
 * @return  void
 */
static void
_smbpbiDemInit
(
    nvswitch_device                     *device,
    struct smbpbi                       *pSmbpbi,
    struct INFOROM_DEM_OBJECT_V1_00     *pFifo
)
{
    NvU8                            msgLeft;
    unsigned                        recordsHeld = 0;
    NvU16                           FIFO_REC_LOOP_ITERATOR;
    NvU16                           bytesOccupied;
    NvU16                           bytesSeen;
    NvBool                          status = NV_FALSE;

    // validate the FIFO buffer

    if ((DEM_FIFO_PTR(pFifo->writeOffset) != pFifo->writeOffset) ||
        (DEM_FIFO_PTR(pFifo->readOffset) != pFifo->readOffset)   ||
        ((pFifo->writeOffset % sizeof(NvU32)) != 0)              ||
        ((pFifo->readOffset % sizeof(NvU32)) != 0))
    {
        goto smbpbiDemInit_exit;
    }

    if (pFifo->writeOffset == pFifo->readOffset)
    {
        // The FIFO is empty
        status = NV_TRUE;
        goto smbpbiDemInit_exit;
    }

    //
    // This HAL extracts from a scratch register the count of DEM messages
    // in the FIFO that has not yet been requested by the SMBPBI client.
    // If the FIFO holds more messages than that, it means those in excess
    // of this count have been delivered to the client by PreOS app.
    //
    if (device->hal.nvswitch_smbpbi_get_dem_num_messages(device, &msgLeft) != NVL_SUCCESS)
    {
        // assume the maximum
        msgLeft = ~0;
    }

    if (msgLeft == 0)
    {
        // Nothing of value in the FIFO. Lets reset it explicitly.
        status = NV_TRUE;
        pFifo->writeOffset = 0;
        pFifo->readOffset = 0;
        goto smbpbiDemInit_exit;
    }

    //
    // Count the messages in the FIFO, while also checking the structure
    // for integrity. Reset the FIFO in case any corruption is found.
    //
    bytesOccupied = DEM_BYTES_OCCUPIED(pFifo);

    bytesSeen = 0;
    FIFO_REC_LOOP_START(pFifo, bytesSeen < bytesOccupied)
        if ((_recSize > DEM_RECORD_SIZE_MAX) ||
            (FIFO_REC_LOOP_REC_SIZE < DEM_RECORD_SIZE_MIN))
        {
            goto smbpbiDemInit_exit;
        }

        bytesSeen += FIFO_REC_LOOP_REC_SIZE;
        ++recordsHeld;
    FIFO_REC_LOOP_END

    if ((bytesSeen != bytesOccupied) || (msgLeft > recordsHeld))
    {
        goto smbpbiDemInit_exit;
    }

    //
    // Advance the FIFO read ptr in order to remove those messages that
    // have already been delivered to the client.
    //
    FIFO_REC_LOOP_START(pFifo, recordsHeld > msgLeft)
        --recordsHeld;
    FIFO_REC_LOOP_END

    pFifo->readOffset =  FIFO_REC_LOOP_ITERATOR;
    status = NV_TRUE;

smbpbiDemInit_exit:

    if (!status)
    {
        // Reset the FIFO
        pFifo->writeOffset = 0;
        pFifo->readOffset = 0;
        pFifo->seqNumber = 0;
    }
}

/*!
 * A helper to create a new DEM FIFO record
 *
 * @param[in,out]   pFifo           DEM object pointer
 * @param[in]       num             Xid number
 * @param[in]       osErrorString   text message to store
 * @param[in]       msglen          message size
 * @param[out]      pRecSize        new record size in bytes
 *
 * @return          ptr to the new record
 * @return          NULL if there's no room in the FIFO
 *                       or dynamic allocation error
 */
static NV_MSGBOX_DEM_RECORD *
_makeNewRecord
(
    INFOROM_DEM_OBJECT_V1_00    *pFifo,
    NvU32                       num,
    NvU8                        *osErrorString,
    NvU32                       msglen,
    NvU32                       *pRecSize
)
{
    NV_MSGBOX_DEM_RECORD                *pNewRec;

    *pRecSize = NV_MIN(sizeof(NV_MSGBOX_DEM_RECORD) + msglen,
                     DEM_RECORD_SIZE_MAX);

    if ((*pRecSize > DEM_BYTES_AVAILABLE(pFifo)) ||
        ((pNewRec = nvswitch_os_malloc(*pRecSize)) == NULL))
    {
        return NULL;
    }

    // Fill the new record.
    nvswitch_os_memset(pNewRec, 0, *pRecSize);
    pNewRec->recordSize = NV_UNSIGNED_DIV_CEIL(*pRecSize, sizeof(NvU32));
    pNewRec->xidId = num;
    pNewRec->seqNumber = pFifo->seqNumber++;
    pNewRec->timeStamp = nvswitch_os_get_platform_time_epoch() / NVSWITCH_NSEC_PER_SEC;

    if (osErrorString[msglen - 1] != 0)
    {
        // The text string is too long. Truncate and notify the client.
        osErrorString[msglen - 1] = 0;
        pNewRec->flags = FLD_SET_DRF(_MSGBOX, _DEM_RECORD_FLAGS,
                                       _TRUNC, _SET, pNewRec->flags);
        msglen = NV_MSGBOX_MAX_DRIVER_EVENT_MSG_TXT_SIZE;
    }

    nvswitch_os_memcpy(pNewRec->textMessage, osErrorString, msglen);

    return pNewRec;
}

/*!
 * A helper to add the new record to the DEM FIFO
 *
 * @param[in,out]   pFifo           DEM object pointer
 * @param[in]       pNewRec         the new record
 * @param[in]       recSize         new record size in bytes
 *
 * @return          void
 */
static void
_addNewRecord
(
    INFOROM_DEM_OBJECT_V1_00    *pFifo,
    NV_MSGBOX_DEM_RECORD        *pNewRec,
    NvU32                       recSize
)
{
    NvU16   rem;
    NvU16   curPtr;
    NvU16   copySz;
    NvU8    *srcPtr;

    // Copy the new record into the FIFO, handling a possible wrap-around.
    rem = recSize;
    curPtr = pFifo->writeOffset;
    srcPtr = (NvU8 *)pNewRec;
    while (rem > 0)
    {
        copySz = NV_MIN(rem, DEM_FIFO_SIZE - curPtr);
        nvswitch_os_memcpy(pFifo->fifoBuffer + curPtr, srcPtr, copySz);
        rem -= copySz;
        srcPtr += copySz;
        curPtr = DEM_FIFO_PTR(curPtr + copySz);
    }

    // Advance the FIFO write ptr.
    pFifo->writeOffset = DEM_FIFO_PTR(pFifo->writeOffset +
                                      (pNewRec->recordSize * sizeof(NvU32)));
}

/*!
 * Add a Driver Event Message (SXid) to the InfoROM DEM FIFO buffer
 *
 * @param[in]   device          device object pointer
 * @param[in]   num             Xid number
 * @param[in]   msglen          message size
 * @param[in]   osErrorString   text message to store
 *
 * @return  void
 */
void
nvswitch_smbpbi_log_message_lr10
(
    nvswitch_device *device,
    NvU32           num,
    NvU32           msglen,
    NvU8            *osErrorString
)
{
    INFOROM_DEM_OBJECT_V1_00            *pFifo;
    NvU32                               recSize;
    NvU16                               FIFO_REC_LOOP_ITERATOR;
    NV_MSGBOX_DEM_RECORD                *pNewRec;

    if ((device->pSmbpbi == NULL) ||
        (device->pSmbpbi->sharedSurface == NULL))
    {
        return;
    }

    pFifo = GET_PFIFO_FROM_DEVICE(device);

    pNewRec = _makeNewRecord(pFifo, num, osErrorString, msglen, &recSize);

    if (pNewRec != NULL)
    {
        _addNewRecord(pFifo, pNewRec, recSize);
        nvswitch_os_free(pNewRec);
    }
    else
    {
        //
        // We are unable to log this message. Mark the latest record
        // with a flag telling the client that message(s) were dropped.
        //

        NvU16                   bytesOccupied = DEM_BYTES_OCCUPIED(pFifo);
        NvU16                   bytesSeen;
        NV_MSGBOX_DEM_RECORD    *pLastRec = NULL;

        // Find the newest record
        bytesSeen = 0;
        FIFO_REC_LOOP_START(pFifo, bytesSeen < bytesOccupied)
            pLastRec = FIFO_REC_LOOP_REC_PTR;
            bytesSeen += FIFO_REC_LOOP_REC_SIZE;
        FIFO_REC_LOOP_END

        if (pLastRec != NULL)
        {
            pLastRec->flags = FLD_SET_DRF(_MSGBOX, _DEM_RECORD_FLAGS,
                                           _OVFL, _SET, pLastRec->flags);
        }
    }

    return;
}

void
nvswitch_smbpbi_dem_flush_lr10
(
    nvswitch_device *device
)
{
    NvU8                                *pPackedObject = NULL;
    struct INFOROM_DEM_OBJECT_V1_00     *pFifo;
    NvlStatus                           status = NVL_SUCCESS;

    pPackedObject = nvswitch_os_malloc(INFOROM_DEM_OBJECT_V1_00_PACKED_SIZE);

    if (pPackedObject == NULL)
    {
        status = -NVL_NO_MEM;
        goto nvswitch_smbpbi_dem_flush_exit;
    }

    pFifo = GET_PFIFO_FROM_DEVICE(device);

    status = nvswitch_inforom_write_object(device, "DEM",
                                        INFOROM_DEM_OBJECT_V1_00_FMT,
                                        pFifo,
                                        pPackedObject);

nvswitch_smbpbi_dem_flush_exit:
    nvswitch_os_free(pPackedObject);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "DEM object write failed, status=%d\n",
                       status);
    }
}

void
nvswitch_smbpbi_send_unload_lr10
(
    nvswitch_device *device
)
{
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            cmd;
    NVSWITCH_TIMEOUT           timeout;
    NvU32                      cmdSeqDesc;
    NvlStatus                  status;

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SMBPBI;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(SMBPBI, UNLOAD);
    cmd.cmd.smbpbiCmd.cmdType = RM_SOE_SMBPBI_CMD_ID_UNLOAD;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: SMBPBI unload command failed. rc:%d\n",
                        __FUNCTION__, status);
    }
}

NvlStatus
nvswitch_smbpbi_send_init_data_lr10
(
    nvswitch_device *device
)
{
    return NVL_SUCCESS;
}

