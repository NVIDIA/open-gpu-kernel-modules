/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "haldef_nvswitch.h"
#include "fsprpc_nvswitch.h"

/*!
 * @brief Check if FSP RM command queue is empty
 *
 * @param[in] device     nvswitch device pointer
 *
 * @return NV_TRUE if queue is empty, NV_FALSE otherwise
 */
static NvBool
_nvswitch_fsp_is_queue_empty
(
    nvswitch_device *device
)
{
    NvU32 cmdqHead, cmdqTail;

    nvswitch_fsp_get_cmdq_head_tail(device, &cmdqHead, &cmdqTail);

    // FSP will set QUEUE_HEAD = TAIL after each packet is received
    return (cmdqHead == cmdqTail);
}

/*!
 * @brief Check if FSP RM message queue is empty
 *
 * @param[in] device       nvswitch_device pointer
 *
 * @return NV_TRUE if queue is empty, NV_FALSE otherwise
 */
static NvBool
_nvswitch_fsp_is_msgq_empty
(
    nvswitch_device *device
)
{
    NvU32 msgqHead, msgqTail;

    nvswitch_fsp_get_msgq_head_tail(device, &msgqHead, &msgqTail);
    return (msgqHead == msgqTail);
}

/*!
 * @brief Wait for FSP RM command queue to be empty
 *
 * @param[in] device       nvswitch_device pointer
 *
 * @return NVL_SUCCESS, or NV_ERR_TIMEOUT
 */
static NvlStatus
_nvswitch_fsp_poll_for_queue_empty
(
    nvswitch_device *device
)
{
    NvBool bKeepPolling;
    NvBool bMsgqEmpty;
    NvBool bCmdqEmpty;
    NVSWITCH_TIMEOUT timeout;

    nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    do
    {
        bKeepPolling = nvswitch_timeout_check(&timeout) ? NV_FALSE : NV_TRUE;

        bMsgqEmpty = _nvswitch_fsp_is_msgq_empty(device);
        bCmdqEmpty = _nvswitch_fsp_is_queue_empty(device);

        //
        // For now we assume that any response from FSP before sending a command
        // indicates an error and we should abort.
        //
        if (!bCmdqEmpty && !bMsgqEmpty)
        {
            nvswitch_fsp_read_message(device,  NULL, 0, &timeout);
            NVSWITCH_PRINT(device, ERROR, "Received error message from FSP while waiting for CMDQ to be empty.\n");
            return -NVL_ERR_GENERIC;
        }

        if (bCmdqEmpty)
        {
            break;
        }

        if (!bKeepPolling)
        {
            NVSWITCH_PRINT(device, ERROR, "Timed out waiting for FSP command queue to be empty.\n");
            return -NVL_ERR_GENERIC;
        }

        nvswitch_os_sleep(1);
    }
    while(bKeepPolling);

    return NVL_SUCCESS;
}

/*!
 * @brief Poll for response from FSP via RM message queue
 *
 * @param[in] device       nvswitch_device pointer
 * @param[in] pTimeout     RPC timeout
 *
 * @return NVL_SUCCESS, or NV_ERR_TIMEOUT
 */
static NvlStatus
_nvswitch_fsp_poll_for_response
(
    nvswitch_device *device,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NvBool bKeepPolling;

    do
    {
        bKeepPolling = nvswitch_timeout_check(pTimeout) ? NV_FALSE : NV_TRUE;

        //
        // Poll for message queue to wait for FSP's reply
        //
        if (!_nvswitch_fsp_is_msgq_empty(device))
        {
            break;
        }

        if (!bKeepPolling)
        {
            NVSWITCH_PRINT(device, ERROR, "FSP command timed out.\n");
            return -NVL_ERR_GENERIC;
        }

        nvswitch_os_sleep(1);
    }
    while(bKeepPolling);

    return NVL_SUCCESS;
}

/*!
 * @brief Read and process message from FSP via RM message queue.
 *
 * Supports both single and multi-packet message. For multi-packet messages, this
 * loops until all packets are received, polling at each iteration for the next
 * packet to come in. If a buffer is provided, the message payload will be
 * returned there.
 *
 * @note: For multi-packet messages, a buffer in which the message payload will
 * be reconstructed must be provided.
 *
 * @param[in]     device              nvswitch_device pointer
 * @param[in/out] pPayloadBuffer    Buffer in which to return message payload
 * @param[in]     payloadBufferSize Payload buffer size
 * @param[in]     pTimeout          RPC timeout
 *
 *
 * @return NVL_SUCCESS, NV_ERR_INVALID_DATA, NV_ERR_INSUFFICIENT_RESOURCES, or errors
 *         from functions called within
 */
NvlStatus
nvswitch_fsp_read_message
(
    nvswitch_device *device,
    NvU8 *pPayloadBuffer,
    NvU32 payloadBufferSize,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NvU8             *pPacketBuffer;
    NvlStatus         status;
    NvU32             totalPayloadSize = 0;
    NvU8             *pMessagePayload;
    NvU8 packetState = MCTP_PACKET_STATE_START;

    if (_nvswitch_fsp_is_msgq_empty(device))
    {
        NVSWITCH_PRINT(device, WARN, "Tried to read FSP response but MSG queue is empty\n");
        return NVL_SUCCESS;
    }

    pPacketBuffer = nvswitch_os_malloc(nvswitch_fsp_get_channel_size(device));
    if (pPacketBuffer == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory!!\n", __FUNCTION__);
        return -NVL_NO_MEM;
    }

    while ((packetState != MCTP_PACKET_STATE_END) && (packetState != MCTP_PACKET_STATE_SINGLE_PACKET))
    {
        NvU32 msgqHead, msgqTail;
        NvU32 packetSize;
        NvU32 curPayloadSize;
        NvU8  curHeaderSize;
        NvU8  tag;

        // Wait for next packet
        status = _nvswitch_fsp_poll_for_response(device, pTimeout);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Timed out waiting for response from FSP!\n", __FUNCTION__);
            goto done;
        }

        nvswitch_fsp_get_msgq_head_tail(device, &msgqHead, &msgqTail);

        // Tail points to last DWORD in packet, not DWORD immediately following it
        packetSize = (msgqTail - msgqHead) + sizeof(NvU32);

        if ((packetSize < sizeof(NvU32)) ||
            (packetSize > nvswitch_fsp_get_channel_size(device)))
        {
            NVSWITCH_PRINT(device, ERROR, "FSP response packet is invalid size: size=0x%x bytes\n", packetSize);
            status = -NVL_ERR_INVALID_STATE;
            goto done;
        }

        nvswitch_fsp_read_from_emem(device, pPacketBuffer, packetSize);

        status = nvswitch_fsp_get_packet_info(device, pPacketBuffer, packetSize, &packetState, &tag);
        if (status != NVL_SUCCESS)
        {
            goto done;
        }

        if ((packetState == MCTP_PACKET_STATE_START) || (packetState == MCTP_PACKET_STATE_SINGLE_PACKET))
        {
            // Packet contains payload header
            curHeaderSize = sizeof(MCTP_HEADER);
        }
        else
        {
            curHeaderSize = sizeof(NvU32);
        }

        curPayloadSize = packetSize - curHeaderSize;

        if ((pPayloadBuffer == NULL) && (packetState != MCTP_PACKET_STATE_SINGLE_PACKET))
        {
            NVSWITCH_PRINT(device, ERROR, "No buffer provided when receiving multi-packet message. Buffer needed to reconstruct message\n");
            status = -NVL_ERR_GENERIC;
            goto done;
        }

        if (pPayloadBuffer != NULL)
        {
            if (payloadBufferSize < (totalPayloadSize + curPayloadSize))
            {
                NVSWITCH_PRINT(device, ERROR, "Buffer provided for message payload too small. Payload size: 0x%x Buffer size: 0x%x\n",
                          totalPayloadSize + curPayloadSize, payloadBufferSize);
                status = -NVL_ERR_GENERIC;
                goto done;
            }
            nvswitch_os_memcpy(pPayloadBuffer + totalPayloadSize,
                        pPacketBuffer + curHeaderSize, curPayloadSize);
        }
        totalPayloadSize += curPayloadSize;

        // Set TAIL = HEAD to indicate CPU received message
        nvswitch_fsp_update_msgq_head_tail(device, msgqHead, msgqHead);
    }

    pMessagePayload = (pPayloadBuffer == NULL) ? (pPacketBuffer + sizeof(MCTP_HEADER)) : pPayloadBuffer;

    status = nvswitch_fsp_process_nvdm_msg(device, pMessagePayload, totalPayloadSize);

done:
    nvswitch_os_free(pPacketBuffer);
    return status;
}

/*!
 * @brief Send one MCTP packet to FSP via EMEM
 *
 * @param[in] device        nvswitch_device pointer
 * @param[in] pPacket       MCTP packet
 * @param[in] packetSize    MCTP packet size in bytes
 *
 * @return NVL_SUCCESS, or NV_ERR_INSUFFICIENT_RESOURCES
 */
NvlStatus
nvswitch_fsp_send_packet
(
    nvswitch_device *device,
    NvU8 *pPacket,
    NvU32 packetSize
)
{
    NvU32 paddedSize;
    NvU8 *pBuffer = NULL;
    NV_STATUS status = NVL_SUCCESS;

    // Check that queue is ready to receive data
    status = _nvswitch_fsp_poll_for_queue_empty(device);
    if (status != NVL_SUCCESS)
    {
        return -NVL_ERR_GENERIC;
    }

    // Pad to align size to 4-bytes boundary since EMEMC increments by DWORDS
    paddedSize = NV_ALIGN_UP(packetSize, sizeof(NvU32));
    pBuffer = nvswitch_os_malloc(paddedSize);
    if (pBuffer == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to allocate memory!!\n");
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(pBuffer, 0, paddedSize);
    nvswitch_os_memcpy(pBuffer, pPacket, paddedSize);

    nvswitch_fsp_write_to_emem(device, pBuffer, paddedSize);

    // Update HEAD and TAIL with new EMEM offset; RM always starts at offset 0.
    nvswitch_fsp_update_cmdq_head_tail(device, 0, paddedSize - sizeof(NvU32));

    nvswitch_os_free(pBuffer);
    return status;
}

/*!
 * @brief Send a MCTP message to FSP via EMEM, and read response
 *
 *
 * Response payload buffer is optional if response fits in a single packet.
 *
 * @param[in] device             nvswitch_device pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 * @param[in] pResponsePayload   Buffer in which to return response payload
 * @param[in] responseBufferSize Response payload buffer size
 * @param[in] pTimeout           RPC timeout
 *
 * @return NVL_SUCCESS, or NV_ERR_*
 */
NvlStatus
nvswitch_fsp_send_and_read_message
(
    nvswitch_device *device,
    NvU8 *pPayload,
    NvU32 size,
    NvU32 nvdmType,
    NvU8 *pResponsePayload,
    NvU32 responseBufferSize,
    NVSWITCH_TIMEOUT *pTimeout
)
{
    NvU32 dataSent, dataRemaining;
    NvU32 packetPayloadCapacity;
    NvU32 curPayloadSize;
    NvU32 headerSize;
    NvU32 fspEmemChannelSize;
    NvBool bSinglePacket;
    NV_STATUS status;
    NvU8 *pBuffer = NULL;
    NvU8  seq = 0;
    NvU8  seid = 0;

    // Allocate buffer of same size as channel
    fspEmemChannelSize = nvswitch_fsp_get_channel_size(device);
    pBuffer = nvswitch_os_malloc(fspEmemChannelSize);
    if (pBuffer == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory!!\n",
            __FUNCTION__);
        return -NVL_NO_MEM;

    }

    nvswitch_os_memset(pBuffer, 0, fspEmemChannelSize);

    //
    // Check if message will fit in single packet
    // We lose 2 DWORDS to MCTP and NVDM headers
    //
    headerSize = 2 * sizeof(NvU32);
    packetPayloadCapacity = fspEmemChannelSize - headerSize;
    bSinglePacket = (size <= packetPayloadCapacity);

    // First packet
    seid = nvswitch_fsp_nvdm_to_seid(device, nvdmType);
    ((NvU32 *)pBuffer)[0] = nvswitch_fsp_create_mctp_header(device, 1, (NvU8)bSinglePacket, seid, seq); // SOM=1,EOM=?,SEID,SEQ=0
    ((NvU32 *)pBuffer)[1] = nvswitch_fsp_create_nvdm_header(device, nvdmType);

    curPayloadSize = NV_MIN(size, packetPayloadCapacity);
    nvswitch_os_memcpy(pBuffer + headerSize, pPayload, curPayloadSize);

    status = nvswitch_fsp_send_packet(device, pBuffer, curPayloadSize + headerSize);
    if (status != NVL_SUCCESS)
    {
        goto failed;
    }

    if (!bSinglePacket)
    {
        // Multi packet case
        dataSent = curPayloadSize;
        dataRemaining = size - dataSent;
        headerSize = sizeof(NvU32); // No longer need NVDM header
        packetPayloadCapacity = fspEmemChannelSize - headerSize;

        while (dataRemaining > 0)
        {
            NvBool bLastPacket = (dataRemaining <= packetPayloadCapacity);
            curPayloadSize = (bLastPacket) ? dataRemaining : packetPayloadCapacity;

            nvswitch_os_memset(pBuffer, 0, fspEmemChannelSize);
            ((NvU32 *)pBuffer)[0] = nvswitch_fsp_create_mctp_header(device, 0, (NvU8)bLastPacket, seid, (++seq) % 4);

            nvswitch_os_memcpy(pBuffer + headerSize, pPayload + dataSent, curPayloadSize);

            status = nvswitch_fsp_send_packet(device, pBuffer, curPayloadSize + headerSize);
            if (status != NVL_SUCCESS)
            {
                goto failed;
            }

            dataSent += curPayloadSize;
            dataRemaining -= curPayloadSize;
        }
    }

    status = _nvswitch_fsp_poll_for_response(device, pTimeout);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Timed out waiting for response from FSP!\n", __FUNCTION__);
        goto failed;
    }
    status = nvswitch_fsp_read_message(device, pResponsePayload, responseBufferSize, pTimeout);

failed:
    nvswitch_os_free(pBuffer);

    return status;
}
