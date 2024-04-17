/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink_export.h"


#include "common_nvswitch.h"
#include "fsprpc_nvswitch.h"
#include "ls10/ls10.h"

#include "nvswitch/ls10/dev_fsp_pri.h"

/*!
 * @brief Update command queue head and tail pointers
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] queueHead  Offset to write to command queue head
 * @param[in] queueTail  Offset to write to command queue tail
 */
void
nvswitch_fsp_update_cmdq_head_tail_ls10
(
    nvswitch_device *device,
    NvU32 queueHead,
    NvU32 queueTail
)
{
    // The write to HEAD needs to happen after TAIL because it will interrupt FSP
    NVSWITCH_REG_WR32(device, _PFSP, _QUEUE_TAIL(FSP_EMEM_CHANNEL_RM), queueTail);
    NVSWITCH_REG_WR32(device, _PFSP, _QUEUE_HEAD(FSP_EMEM_CHANNEL_RM), queueHead);
}

/*!
 * @brief Read command queue head and tail pointers
 *
 * @param[in]  device     nvswitch device pointer
 * @param[out] pQueueHead Pointer where we write command queue head
 * @param[out] pQueueTail Pointer where we write command queue tail
 */
void
nvswitch_fsp_get_cmdq_head_tail_ls10
(
    nvswitch_device *device,
    NvU32 *pQueueHead,
    NvU32 *pQueueTail
)
{
    *pQueueHead = NVSWITCH_REG_RD32(device, _PFSP, _QUEUE_HEAD(FSP_EMEM_CHANNEL_RM));
    *pQueueTail = NVSWITCH_REG_RD32(device, _PFSP, _QUEUE_TAIL(FSP_EMEM_CHANNEL_RM));
}

/*!
 * @brief Update message queue head and tail pointers
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] msgqHead   Offset to write to message queue head
 * @param[in] msgqTail   Offset to write to message queue tail
 */
void
nvswitch_fsp_update_msgq_head_tail_ls10
(
    nvswitch_device *device,
    NvU32 msgqHead,
    NvU32 msgqTail
)
{
    NVSWITCH_REG_WR32(device, _PFSP, _MSGQ_TAIL(FSP_EMEM_CHANNEL_RM), msgqTail);
    NVSWITCH_REG_WR32(device, _PFSP, _MSGQ_HEAD(FSP_EMEM_CHANNEL_RM), msgqHead);
}

/*!
 * @brief Read message queue head and tail pointers
 *
 * @param[in]  device     nvswitch device pointer
 * @param[out] pMsgqHead  Pointer where we write message queue head
 * @param[out] pMsgqTail  Pointer where we write message queue tail
 */
void
nvswitch_fsp_get_msgq_head_tail_ls10
(
    nvswitch_device  *device,
    NvU32 *pMsgqHead,
    NvU32 *pMsgqTail
)
{
    *pMsgqHead = NVSWITCH_REG_RD32(device, _PFSP, _MSGQ_HEAD(FSP_EMEM_CHANNEL_RM));
    *pMsgqTail = NVSWITCH_REG_RD32(device, _PFSP, _MSGQ_TAIL(FSP_EMEM_CHANNEL_RM));
}

/*!
 * @brief Get size of RM's channel in FSP EMEM
 *
 * @param[in] device     nvswitch device pointer
 *
 * @return RM channel size in bytes
 */
NvU32
nvswitch_fsp_get_channel_size_ls10
(
    nvswitch_device *device
)
{
    //
    // Channel size is hardcoded to 1K for now. Later we will use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    return FSP_EMEM_CHANNEL_RM_SIZE;
}

/*!
 * @brief Retreive SEID based on NVDM type
 *
 * For now, SEIDs are only needed for use-cases that send multi-packet RM->FSP
 * messages. The SEID is used in these cases to route packets to the correct
 * task as FSP receives them. Single-packet use-cases are given SEID 0.
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] nvdmType   NVDM message type
 *
 * @return SEID corresponding to passed-in NVDM type
 */
NvU8
nvswitch_fsp_nvdm_to_seid_ls10
(
    nvswitch_device *device,
    NvU8 nvdmType
)
{
    NvU8 seid;

    switch (nvdmType)
    {
        case NVDM_TYPE_INFOROM:
            seid = 1;
            break;
        case NVDM_TYPE_HULK:
        default:
            seid = 0;
            break;
    }

    return seid;
}

/*!
 * @brief Create MCTP header
 *
 * @param[in] device       nvswitch_device pointer
 * @param[in] som        Start of Message flag
 * @param[in] eom        End of Message flag
 * @param[in] tag        Message tag
 * @param[in] seq        Packet sequence number
 *
 * @return Constructed MCTP header
 */
NvU32
nvswitch_fsp_create_mctp_header_ls10
(
    nvswitch_device *device,
    NvU8 som,
    NvU8 eom,
    NvU8 seid,
    NvU8 seq
)
{
    return (REF_NUM(MCTP_HEADER_SOM,  (som)) |
            REF_NUM(MCTP_HEADER_EOM,  (eom)) |
            REF_NUM(MCTP_HEADER_SEID, (seid)) |
            REF_NUM(MCTP_HEADER_SEQ,  (seq)));
}

/*!
 * @brief Create NVDM payload header
 *
 * @param[in] device       nvswitch_device pointer
 * @param[in] nvdmType   NVDM type to include in header
 *
 * @return Constructed NVDM payload header
 */
NvU32
nvswitch_fsp_create_nvdm_header_ls10
(
    nvswitch_device *device,
    NvU32 nvdmType
)
{
    return (REF_DEF(MCTP_MSG_HEADER_TYPE, _VENDOR_PCI) |
            REF_DEF(MCTP_MSG_HEADER_VENDOR_ID, _NV)    |
            REF_NUM(MCTP_MSG_HEADER_NVDM_TYPE, (nvdmType)));
}

/*!
 * @brief Retrieve and validate info in packet's MCTP headers
 *
 * @param[in]  device        nvswitch device pointer
 * @param[in]  pBuffer       Buffer containing packet
 * @param[in]  size          Size of buffer in bytes
 * @param[out] pPacketState  Pointer where we write packet state
 * @param[out] pTag          Pointer where we write packet's MCTP tag
 *
 * @return NVL_SUCCESS or NV_ERR_INVALID_DATA
 */
NvlStatus
nvswitch_fsp_get_packet_info_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size,
    NvU8 *pPacketState,
    NvU8 *pTag
)
{
    NvU32 mctpHeader;
    NvU8  som, eom;
    NvlStatus status = NVL_SUCCESS;

    mctpHeader = ((NvU32 *)pBuffer)[0];

    som = REF_VAL(MCTP_HEADER_SOM, mctpHeader);
    eom = REF_VAL(MCTP_HEADER_EOM, mctpHeader);

    if ((som == 1) && (eom == 0))
    {
        *pPacketState = MCTP_PACKET_STATE_START;
    }
    else if ((som == 0) && (eom == 1))
    {
        *pPacketState = MCTP_PACKET_STATE_END;
    }
    else if ((som == 1) && (eom == 1))
    {
        *pPacketState = MCTP_PACKET_STATE_SINGLE_PACKET;
    }
    else
    {
        *pPacketState = MCTP_PACKET_STATE_INTERMEDIATE;
    }

    if ((*pPacketState == MCTP_PACKET_STATE_START) ||
        (*pPacketState == MCTP_PACKET_STATE_SINGLE_PACKET))
    {
        // Packet contains payload header, check it
        status = nvswitch_fsp_validate_mctp_payload_header(device, pBuffer, size);
    }

    *pTag = REF_VAL(MCTP_HEADER_TAG, mctpHeader);

    return status;
}

/*!
 * @brief Validate packet's MCTP payload header
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] pBuffer    Buffer containing packet
 * @param[in] size       Size of buffer in bytes
 *
 * @return NVL_SUCCESS or NV_ERR_INVALID_DATA
 */
NvlStatus
nvswitch_fsp_validate_mctp_payload_header_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    NvU32 mctpPayloadHeader;
    NvU16 mctpVendorId;
    NvU8  mctpMessageType;

    mctpPayloadHeader = ((NvU32 *)pBuffer)[1];

    mctpMessageType = REF_VAL(MCTP_MSG_HEADER_TYPE, mctpPayloadHeader);
    if (mctpMessageType != MCTP_MSG_HEADER_TYPE_VENDOR_PCI)
    {
        NVSWITCH_PRINT(device, ERROR, "Invalid MCTP Message type 0x%0x, expecting 0x7e (Vendor Defined PCI)\n",
                  mctpMessageType);
        return -NVL_ERR_INVALID_STATE ;
    }

    mctpVendorId = REF_VAL(MCTP_MSG_HEADER_VENDOR_ID, mctpPayloadHeader);
    if (mctpVendorId != MCTP_MSG_HEADER_VENDOR_ID_NV)
    {
        NVSWITCH_PRINT(device, ERROR, "Invalid PCI Vendor Id 0x%0x, expecting 0x10de (Nvidia)\n",
                  mctpVendorId);
        return -NVL_ERR_INVALID_STATE ;
    }

    if (size < (sizeof(MCTP_HEADER) + sizeof(NvU8)))
    {
        NVSWITCH_PRINT(device, ERROR, "Packet doesn't contain NVDM type in payload header\n");
        return -NVL_ERR_INVALID_STATE ;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Process NVDM payload
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NVL_SUCCESS or NV_ERR_NOT_SUPPORTED
 */
NvlStatus
nvswitch_fsp_process_nvdm_msg_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    NvU8 nvdmType;
    NvlStatus status = NVL_SUCCESS;

    nvdmType = pBuffer[0];

    switch (nvdmType)
    {
        case NVDM_TYPE_TNVL:
        case NVDM_TYPE_FSP_RESPONSE:
            status = nvswitch_fsp_process_cmd_response(device, pBuffer, size);
            break;
        default:
            NVSWITCH_PRINT(device, ERROR, "Unknown or unsupported NVDM type received: 0x%0x\n",
                      nvdmType);
            status = -NVL_ERR_NOT_SUPPORTED;
            break;
    }

    return status;
}

/*!
 * @brief Process FSP command response
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NVL_SUCCESS or -NVL_ERR_INVALID_STATE
 */
NvlStatus
nvswitch_fsp_process_cmd_response_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    NVDM_PAYLOAD_COMMAND_RESPONSE *pCmdResponse;
    NvU32 headerSize = sizeof(NvU8); // NVDM type
    NvlStatus status = NVL_SUCCESS;

    if (size < (headerSize + sizeof(NVDM_PAYLOAD_COMMAND_RESPONSE)))
    {
        NVSWITCH_PRINT(device, ERROR, "Expected FSP command response, but packet is not big enough for payload. Size: 0x%0x\n", size);
        return -NVL_ERR_INVALID_STATE;
    }

    pCmdResponse = (NVDM_PAYLOAD_COMMAND_RESPONSE *)&(pBuffer[1]);

    NVSWITCH_PRINT(device, INFO, "Received FSP command response. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
              pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);

    status = nvswitch_fsp_error_code_to_nvlstatus_map(device, pCmdResponse->errorCode);
    if (status == NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Last command was processed by FSP successfully!\n");
    }
    else if (status != -NVL_NOT_FOUND)
    {

        NVSWITCH_PRINT(device, ERROR, "FSP response reported error. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
                pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);
    }

    return status;
}

/*!
 * @brief Configure EMEMC for RM's queue in FSP EMEM
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] offset     Offset to write to EMEMC in DWORDS
 * @param[in] bAincw     Flag to set auto-increment on writes
 * @param[in] bAincr     Flag to set auto-increment on reads
 *
 * @return NVL_SUCCESS
 */
NvlStatus
nvswitch_fsp_config_ememc_ls10
(
    nvswitch_device *device,
    NvU32 offset,
    NvBool bAincw,
    NvBool bAincr
)
{
    NvU32 offsetBlks, offsetDwords;
    NvU32 reg32 = 0;

    //
    // EMEMC offset is encoded in terms of blocks (64 DWORDS) and DWORD offset
    // within a block, so calculate each.
    //
    offsetBlks = offset / 64;
    offsetDwords = offset % 64;

    reg32 = FLD_SET_DRF_NUM(_PFSP, _EMEMC, _OFFS, offsetDwords, reg32);
    reg32 = FLD_SET_DRF_NUM(_PFSP, _EMEMC, _BLK, offsetBlks, reg32);

    if (bAincw)
    {
        reg32 = FLD_SET_DRF(_PFSP, _EMEMC, _AINCW, _TRUE, reg32);
    }
    if (bAincr)
    {
        reg32 = FLD_SET_DRF(_PFSP, _EMEMC, _AINCR, _TRUE, reg32);
    }

    NVSWITCH_REG_WR32(device, _PFSP, _EMEMC(FSP_EMEM_CHANNEL_RM), reg32);
    
    return NVL_SUCCESS;
}

/*!
 * @brief Write data in buffer to RM channel in FSP's EMEM
 *
 * @param[in] device     nvswitch device pointer
 * @param[in] pBuffer    Buffer with data to write to EMEM
 * @param[in] Size       Size of buffer in bytes, assumed DWORD aligned
 *
 * @return NVL_SUCCESS
 */
NvlStatus
nvswitch_fsp_write_to_emem_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    NvU32 i, reg32;
    NvU32 ememOffsetEnd;

    //
    // First configure EMEMC, RM always writes 0 to the offset, which is OK
    // because RM's channel starts at 0 on GH100 and we always start from the
    // beginning for each packet. It should be improved later to use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    nvswitch_fsp_config_ememc(device, 0, NV_TRUE, NV_FALSE);

    NVSWITCH_PRINT(device, INFO, "About to send data to FSP, ememcOff=0, size=0x%x\n", size);
    if (!NV_IS_ALIGNED(size, sizeof(NvU32)))
    {
        NVSWITCH_PRINT(device, WARN, "Size=0x%x is not DWORD-aligned, data will be truncated!\n", size);
    }

    // Now write to EMEMD
    for (i = 0; i < (size / 4); i++)
    {
        NVSWITCH_REG_WR32(device, _PFSP, _EMEMD(FSP_EMEM_CHANNEL_RM), ((NvU32*)(void*)pBuffer)[i]);
    }

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = NVSWITCH_REG_RD32(device, _PFSP, _EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PFSP, _EMEMC, _OFFS, reg32);

    // Blocks are 64 DWORDS
    ememOffsetEnd += DRF_VAL(_PFSP, _EMEMC, _BLK, reg32) * 64;
    NVSWITCH_PRINT(device, INFO, "After sending data, ememcOff = 0x%x\n", ememOffsetEnd);

    NVSWITCH_ASSERT((ememOffsetEnd) == (size / sizeof(NvU32)));
    return NVL_SUCCESS;
}

/*!
 * @brief Read data to buffer from RM channel in FSP's EMEM
 *
 * @param[in]     device       nvswitch_device pointer
 * @param[in/out] pBuffer    Buffer where we copy data from EMEM
 * @param[in]     size       Size to read in bytes, assumed DWORD aligned
 *
 * @return NVL_SUCCESS
 */
NvlStatus
nvswitch_fsp_read_from_emem_ls10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    NvU32 i, reg32;
    NvU32 ememOffsetEnd;

    //
    // First configure EMEMC, RM always writes 0 to the offset, which is OK
    // because RM's channel starts at 0 on GH100 and we always start from the
    // beginning for each packet. It should be improved later to use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    nvswitch_fsp_config_ememc(device, 0, NV_FALSE, NV_TRUE);

    NVSWITCH_PRINT(device, INFO, "About to read data from FSP, ememcOff=0, size=0x%x\n", size);
    if (!NV_IS_ALIGNED(size, sizeof(NvU32)))
    {
        NVSWITCH_PRINT(device, WARN, "Size=0x%x is not DWORD-aligned, data will be truncated!\n", size);
    }

    // Now read from EMEMD
    for (i = 0; i < (size / 4); i++)
    {
        ((NvU32*)(void*)pBuffer)[i] = NVSWITCH_REG_RD32(device, _PFSP, _EMEMD(FSP_EMEM_CHANNEL_RM));
    }

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = NVSWITCH_REG_RD32(device, _PFSP, _EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PFSP, _EMEMC, _OFFS, reg32);

    // Blocks are 64 DWORDS
    ememOffsetEnd += DRF_VAL(_PFSP, _EMEMC, _BLK, reg32) * 64;
    NVSWITCH_PRINT(device, INFO, "After reading data, ememcOff = 0x%x\n", ememOffsetEnd);

    NVSWITCH_ASSERT((ememOffsetEnd) == (size / sizeof(NvU32)));
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_fsp_error_code_to_nvlstatus_map_ls10
(
    nvswitch_device *device,
    NvU32 errorCode
)
{
    switch (errorCode)
    {
        case FSP_OK:
        return NVL_SUCCESS;

        case FSP_ERR_IFR_FILE_NOT_FOUND:
        return -NVL_NOT_FOUND;

        case FSP_ERR_IFS_ERR_INVALID_STATE:
        case FSP_ERR_IFS_ERR_INVALID_DATA:
        return -NVL_ERR_INVALID_STATE;

        default:
        return -NVL_ERR_GENERIC;
    }
}

NvlStatus
nvswitch_fsprpc_get_caps_ls10
(
    nvswitch_device *device,
    NVSWITCH_FSPRPC_GET_CAPS_PARAMS *params
)
{
    TNVL_RPC_CAPS_PAYLOAD payload;
    TNVL_RPC_CAPS_RSP_PAYLOAD responsePayload;
    NvlStatus status;

    payload.subMessageId = TNVL_CAPS_SUBMESSAGE_ID;
    nvswitch_os_memset(&responsePayload, 0, sizeof(TNVL_RPC_CAPS_RSP_PAYLOAD));

    status = nvswitch_fsp_send_and_read_message(device,
        (NvU8*) &payload, sizeof(TNVL_RPC_CAPS_PAYLOAD), NVDM_TYPE_CAPS_QUERY,
        (NvU8*) &responsePayload, sizeof(TNVL_RPC_CAPS_RSP_PAYLOAD));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "RPC failed for FSP caps query\n");
        return status;
    }

    params->responseNvdmType = responsePayload.nvdmType;
    params->commandNvdmType  = responsePayload.cmdResponse.commandNvdmType;
    params->errorCode        = responsePayload.cmdResponse.errorCode;

    return NVL_SUCCESS;
}
