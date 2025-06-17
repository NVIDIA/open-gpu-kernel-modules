/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *
 * @file    kern_fsp_gh100.c
 * @brief   Provides the implementation for HOPPER chip specific FSP HAL
 *          interfaces.
 */
#include "gpu/fsp/kern_fsp.h"
#include "gpu/fsp/kern_fsp_retval.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/spdm/spdm.h"
#include "fsp/nvdm_payload_cmd_response.h"
#include "fsp/fsp_clock_boost_rpc.h"
#include "fsp/fsp_caps_query_rpc.h"

#include "published/hopper/gh100/dev_fsp_pri.h"
#include "published/hopper/gh100/dev_fsp_addendum.h"
#include "fsp/fsp_nvdm_format.h"
#include "published/hopper/gh100/dev_bus.h"
#include "published/hopper/gh100/dev_bus_addendum.h"
#include "published/hopper/gh100/dev_gc6_island_addendum.h"
#include "published/hopper/gh100/dev_falcon_v4.h"
#include "published/hopper/gh100/dev_gsp.h"
#include "published/hopper/gh100/dev_therm.h"
#include "published/hopper/gh100/dev_therm_addendum.h"
#include "os/os.h"
#include "nvRmReg.h"
#include "nverror.h"

#include "gpu/conf_compute/conf_compute.h"

// Blocks are 64 DWORDS
#define DWORDS_PER_EMEM_BLOCK 64U

static void _kfspUpdateQueueHeadTail_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 queueHead, NvU32 queueTail);

static void _kfspGetQueueHeadTail_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static void _kfspUpdateMsgQueueHeadTail_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 queueHead, NvU32 queueTail);

static void _kfspGetMsgQueueHeadTail_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static NV_STATUS _kfspConfigEmemc_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 offset, NvBool bAincw, NvBool bAincr);

static NV_STATUS _kfspWriteToEmem_GH100(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU8 *pBuffer, NvU32 size);

static NvBool _kfspWaitBootCond_GH100(OBJGPU *pGpu, void *pArg);

/*!
 * @brief Update command queue head and tail pointers
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] queueHead  Offset to write to command queue head
 * @param[in] queueTail  Offset to write to command queue tail
 */
static void
_kfspUpdateQueueHeadTail_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      queueHead,
    NvU32      queueTail
)
{
    // The write to HEAD needs to happen after TAIL because it will interrupt FSP
    GPU_REG_WR32(pGpu, NV_PFSP_QUEUE_TAIL(FSP_EMEM_CHANNEL_RM), queueTail);
    GPU_REG_WR32(pGpu, NV_PFSP_QUEUE_HEAD(FSP_EMEM_CHANNEL_RM), queueHead);
}

/*!
 * @brief Read command queue head and tail pointers
 *
 * @param[in]  pGpu       OBJGPU pointer
 * @param[in]  pKernelFsp KernelFsp pointer
 * @param[out] pQueueHead Pointer where we write command queue head
 * @param[out] pQueueTail Pointer where we write command queue tail
 */
static void
_kfspGetQueueHeadTail_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32     *pQueueHead,
    NvU32     *pQueueTail
)
{
    *pQueueHead = GPU_REG_RD32(pGpu, NV_PFSP_QUEUE_HEAD(FSP_EMEM_CHANNEL_RM));
    *pQueueTail = GPU_REG_RD32(pGpu, NV_PFSP_QUEUE_TAIL(FSP_EMEM_CHANNEL_RM));
}

/*!
 * @brief Update message queue head and tail pointers
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] msgqHead   Offset to write to message queue head
 * @param[in] msgqTail   Offset to write to message queue tail
 */
static void
_kfspUpdateMsgQueueHeadTail_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      msgqHead,
    NvU32      msgqTail
)
{
    GPU_REG_WR32(pGpu, NV_PFSP_MSGQ_TAIL(FSP_EMEM_CHANNEL_RM), msgqTail);
    GPU_REG_WR32(pGpu, NV_PFSP_MSGQ_HEAD(FSP_EMEM_CHANNEL_RM), msgqHead);
}

/*!
 * @brief Read message queue head and tail pointers
 *
 * @param[in]  pGpu       OBJGPU pointer
 * @param[in]  pKernelFsp KernelFsp pointer
 * @param[out] pMsgqHead  Pointer where we write message queue head
 * @param[out] pMsgqTail  Pointer where we write message queue tail
 */
static void
_kfspGetMsgQueueHeadTail_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32     *pMsgqHead,
    NvU32     *pMsgqTail
)
{
    *pMsgqHead = GPU_REG_RD32(pGpu, NV_PFSP_MSGQ_HEAD(FSP_EMEM_CHANNEL_RM));
    *pMsgqTail = GPU_REG_RD32(pGpu, NV_PFSP_MSGQ_TAIL(FSP_EMEM_CHANNEL_RM));
}

/*!
 * @brief Get maximum size of a packet we can send.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return Packet size in bytes
 */
NvU32
kfspGetMaxSendPacketSize_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    //
    // Channel size is hardcoded to 1K for now. Later we will use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    return FSP_EMEM_CHANNEL_RM_SIZE;
}

/*!
 * @brief Get maximum size of a packet we can receive.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return Packet size in bytes
 */
NvU32
kfspGetMaxRecvPacketSize_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    //
    // Channel size is hardcoded to 1K for now. Later we will use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    return FSP_EMEM_CHANNEL_RM_SIZE;
}

/*!
 * @brief Check if RM has a response from FSP available
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_TRUE if a response is available, NV_FALSE otherwise
 */
NvBool
kfspIsResponseAvailable_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 msgqHead;
    NvU32 msgqTail;

    _kfspGetMsgQueueHeadTail_GH100(pGpu, pKernelFsp, &msgqHead, &msgqTail);
    // FSP updates the head after writing data into the channel
    return (msgqHead != msgqTail);
}


/*!
 * @brief Check if RM can send a packet to FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_TRUE if send is possible, NV_FALSE otherwise
 */
NvBool
kfspCanSendPacket_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 cmdqHead;
    NvU32 cmdqTail;

    _kfspGetQueueHeadTail_GH100(pGpu, pKernelFsp, &cmdqHead, &cmdqTail);

    // FSP will set QUEUE_HEAD = TAIL after each packet is received
    return (cmdqHead == cmdqTail);
}

/*!
 * @brief Retreive SEID based on NVDM type
 *
 * For now, SEIDs are only needed for use-cases that send multi-packet RM->FSP
 * messages. The SEID is used in these cases to route packets to the correct
 * task as FSP receives them. Single-packet use-cases are given SEID 0.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] nvdmType   NVDM message type
 *
 * @return SEID corresponding to passed-in NVDM type
 */
NvU8
kfspNvdmToSeid_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8       nvdmType
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
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] som        Start of Message flag
 * @param[in] eom        End of Message flag
 * @param[in] tag        Message tag
 * @param[in] seq        Packet sequence number
 *
 * @return Constructed MCTP header
 */
NvU32
kfspCreateMctpHeader_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8       som,
    NvU8       eom,
    NvU8       seid,
    NvU8       seq
)
{
    return REF_NUM(MCTP_HEADER_SOM,  (som)) |
           REF_NUM(MCTP_HEADER_EOM,  (eom)) |
           REF_NUM(MCTP_HEADER_SEID, (seid)) |
           REF_NUM(MCTP_HEADER_SEQ,  (seq));
}

/*!
 * @brief Create NVDM payload header
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] nvdmType   NVDM type to include in header
 *
 * @return Constructed NVDM payload header
 */
NvU32
kfspCreateNvdmHeader_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      nvdmType
)
{
    return REF_DEF(MCTP_MSG_HEADER_TYPE, _VENDOR_PCI) |
           REF_DEF(MCTP_MSG_HEADER_VENDOR_ID, _NV)    |
           REF_NUM(MCTP_MSG_HEADER_NVDM_TYPE, (nvdmType));
}

/*!
 * @brief Retrieve and validate info in packet's MCTP headers
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelFsp    KernelFsp pointer
 * @param[in]  pBuffer       Buffer containing packet
 * @param[in]  size          Size of buffer in bytes
 * @param[out] pPacketState  Pointer where we write packet state
 * @param[out] pTag          Pointer where we write packet's MCTP tag
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
kfspGetPacketInfo_GH100
(
    OBJGPU            *pGpu,
    KernelFsp         *pKernelFsp,
    NvU8              *pBuffer,
    NvU32              size,
    MCTP_PACKET_STATE *pPacketState,
    NvU8              *pTag
)
{
    NvU32 mctpHeader;
    NvU8  som, eom;
    NV_STATUS status = NV_OK;

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
        status = kfspValidateMctpPayloadHeader_HAL(pGpu, pKernelFsp, pBuffer, size);
    }

    *pTag = REF_VAL(MCTP_HEADER_TAG, mctpHeader);

    return status;
}

/*!
 * @brief Validate packet's MCTP payload header
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] pBuffer    Buffer containing packet
 * @param[in] size       Size of buffer in bytes
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
kfspValidateMctpPayloadHeader_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pBuffer,
    NvU32     size
)
{
    NvU32 mctpPayloadHeader;
    NvU16 mctpVendorId;
    NvU8  mctpMessageType;

    mctpPayloadHeader = ((NvU32 *)pBuffer)[1];

    mctpMessageType = REF_VAL(MCTP_MSG_HEADER_TYPE, mctpPayloadHeader);
    if (mctpMessageType != MCTP_MSG_HEADER_TYPE_VENDOR_PCI)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid MCTP Message type 0x%0x, expecting 0x7e (Vendor Defined PCI)\n",
                  mctpMessageType);
        return NV_ERR_INVALID_DATA;
    }

    mctpVendorId = REF_VAL(MCTP_MSG_HEADER_VENDOR_ID, mctpPayloadHeader);
    if (mctpVendorId != MCTP_MSG_HEADER_VENDOR_ID_NV)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid PCI Vendor Id 0x%0x, expecting 0x10de (Nvidia)\n",
                  mctpVendorId);
        return NV_ERR_INVALID_DATA;
    }

    if (size < (sizeof(MCTP_HEADER) + sizeof(NvU8)))
    {
        NV_PRINTF(LEVEL_ERROR, "Packet doesn't contain NVDM type in payload header\n");
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

/*!
 * @brief Process NVDM payload
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NV_OK or NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
kfspProcessNvdmMessage_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pBuffer,
    NvU32      size
)
{
    NvU8 nvdmType;
    NV_STATUS status = NV_OK;

    nvdmType = pBuffer[0];

    switch (nvdmType)
    {
        case NVDM_TYPE_FSP_RESPONSE:
        case NVDM_TYPE_SMBPBI:
            status = kfspProcessCommandResponse_HAL(pGpu, pKernelFsp, pBuffer, size);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown or unsupported NVDM type received: 0x%0x\n",
                      nvdmType);
            status = NV_ERR_NOT_SUPPORTED;
            break;
    }

    return status;
}


/*!
 * @brief Process FSP command response
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
kfspProcessCommandResponse_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pBuffer,
    NvU32      size
)
{
    NVDM_PAYLOAD_COMMAND_RESPONSE *pCmdResponse;
    NvU32 headerSize = sizeof(NvU8); // NVDM type
    NV_STATUS status = NV_OK;

    if (size < (headerSize + sizeof(NVDM_PAYLOAD_COMMAND_RESPONSE)))
    {
        NV_PRINTF(LEVEL_ERROR, "Expected FSP command response, but packet is not big enough for payload. Size: 0x%0x\n", size);
        return NV_ERR_INVALID_DATA;
    }

    pCmdResponse = (NVDM_PAYLOAD_COMMAND_RESPONSE *)&(pBuffer[1]);
    NV_PRINTF(LEVEL_INFO, "Received FSP command response. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
              pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);

    status = kfspErrorCode2NvStatusMap_HAL(pGpu, pKernelFsp, pCmdResponse->errorCode);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Last command was processed by FSP successfully!\n");
    }
    else if (status != NV_ERR_OBJECT_NOT_FOUND && status != NV_ERR_INVALID_ARGUMENT)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP response reported error. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
                pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);
    }

    return status;
}

/*!
 * @brief Configure EMEMC for RM's queue in FSP EMEM
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] offset     Offset to write to EMEMC in DWORDS
 * @param[in] bAincw     Flag to set auto-increment on writes
 * @param[in] bAincr     Flag to set auto-increment on reads
 *
 * @return NV_OK
 */
static NV_STATUS
_kfspConfigEmemc_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      offset,
    NvBool     bAincw,
    NvBool     bAincr
)
{
    NvU32 offsetBlks;
    NvU32 offsetDwords;
    NvU32 reg32 = 0;

    //
    // EMEMC offset is encoded in terms of blocks and DWORD offset
    // within a block, so calculate each.
    //
    offsetBlks = offset / DWORDS_PER_EMEM_BLOCK;
    offsetDwords = offset % DWORDS_PER_EMEM_BLOCK;

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

    GPU_REG_WR32(pGpu, NV_PFSP_EMEMC(FSP_EMEM_CHANNEL_RM), reg32);
    return NV_OK;
}

/*!
 * @brief Write data in buffer to RM channel in FSP's EMEM
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] pBuffer    Buffer with data to write to EMEM
 * @param[in] Size       Size of buffer in bytes, assumed DWORD aligned
 *
 * @return NV_OK
 */
static NV_STATUS
_kfspWriteToEmem_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pBuffer,
    NvU32      size
)
{
    NvU32 i;
    NvU32 reg32;
    NvU32 leftoverBytes;
    NvU32 wordsWritten;
    NvU32 ememOffsetStart;
    NvU32 ememOffsetEnd;

    reg32 = GPU_REG_RD32(pGpu, NV_PFSP_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetStart = DRF_VAL(_PFSP, _EMEMC, _OFFS, reg32);
    ememOffsetStart += DRF_VAL(_PFSP, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "About to send data to FSP, ememcOff=0x%x, size=0x%x\n", ememOffsetStart, size);

    // Now write to EMEMD, we always have to write a DWORD at a time so write
    // all the full DWORDs in the buffer and then pad any leftover bytes with 0
    for (i = 0; i < (size / sizeof(NvU32)); i++)
    {
        GPU_REG_WR32(pGpu, NV_PFSP_EMEMD(FSP_EMEM_CHANNEL_RM), ((NvU32 *)(void *)pBuffer)[i]);
    }
    wordsWritten = size / sizeof(NvU32);

    leftoverBytes = size % sizeof(NvU32);
    if (leftoverBytes != 0)
    {
        reg32 = 0;
        portMemCopy(&reg32, sizeof(NvU32), &(pBuffer[size - leftoverBytes]), leftoverBytes);
        GPU_REG_WR32(pGpu, NV_PFSP_EMEMD(FSP_EMEM_CHANNEL_RM), reg32);
        wordsWritten++;
    }

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PFSP_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PFSP, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PFSP, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "After sending data, ememcOff = 0x%x\n", ememOffsetEnd);

    NV_ASSERT_OR_RETURN((ememOffsetEnd - ememOffsetStart) == wordsWritten, NV_ERR_INVALID_STATE);

    return NV_OK;
}

/*!
 * @brief Send one packet to FSP via EMEM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelFsp    KernelFsp pointer
 * @param[in] pPacket       Packet buffer
 * @param[in] packetSize    Packet size in bytes
 *
 * @return NV_OK, or NV_ERR_INSUFFICIENT_RESOURCES
 */
NV_STATUS
kfspSendPacket_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPacket,
    NvU32      packetSize
)
{
    NvU32 paddedSize;
    NV_STATUS status = NV_OK;

    // Check that queue is ready to receive data
    status = kfspPollForCanSend(pGpu, pKernelFsp);
    if (status != NV_OK)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    //
    // First configure EMEMC, RM always writes 0 to the offset, which is OK
    // because RM's channel starts at 0 on GH100 and we always start from the
    // beginning for each packet. It should be improved later to use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    _kfspConfigEmemc_GH100(pGpu, pKernelFsp, 0, NV_TRUE, NV_FALSE);
    _kfspWriteToEmem_GH100(pGpu, pKernelFsp, pPacket, packetSize);

    paddedSize = NV_ALIGN_UP(packetSize, sizeof(NvU32));
    // Update HEAD and TAIL with new EMEM offset; RM always starts at offset 0.
    // TAIL points to the last DWORD written, so subtract 1 DWORD
    _kfspUpdateQueueHeadTail_GH100(pGpu, pKernelFsp, 0, paddedSize - sizeof(NvU32));

    return status;
}

/*!
 * @brief Read data to buffer from RM channel in FSP's EMEM
 *
 * @param[in]     pGpu          OBJGPU pointer
 * @param[in]     pKernelFsp    KernelFsp pointer
 * @param[in/out] pPacket       Buffer where we copy data from EMEM
 * @param[in]     maxPacketSize Size in bytes of pPacket buffer
 * @param[out]    bytesRead     Size in bytes that was read into the packet buffer
 *
 * @return NV_OK
 */
NV_STATUS
kfspReadPacket_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPacket,
    NvU32      maxPacketSize,
    NvU32     *bytesRead
)
{
    NvU32 i, reg32;
    NvU32 ememOffsetEnd;
    NvU32 msgqHead;
    NvU32 msgqTail;
    NvU32 packetSize;

    // First check that the packet buffer has enough space for the
    // packet and that the size is valid.
    _kfspGetMsgQueueHeadTail_GH100(pGpu, pKernelFsp, &msgqHead, &msgqTail);

    // Tail points to last DWORD in packet, not DWORD immediately following it
    packetSize = (msgqTail - msgqHead) + sizeof(NvU32);

    NV_ASSERT_OR_RETURN((packetSize >= sizeof(NvU32)) && (packetSize <= maxPacketSize),
        NV_ERR_INVALID_DATA);

    //
    // Next configure EMEMC, RM always writes 0 to the offset, which is OK
    // because RM's channel starts at 0 on GH100 and we always start from the
    // beginning for each packet. It should be improved later to use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    _kfspConfigEmemc_GH100(pGpu, pKernelFsp, 0, NV_FALSE, NV_TRUE);

    NV_PRINTF(LEVEL_INFO, "About to read data from FSP, ememcOff=0, size=0x%x\n", packetSize);
    if (!NV_IS_ALIGNED(packetSize, sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_WARNING, "Size=0x%x is not DWORD-aligned, data will be truncated!\n", packetSize);
    }

    // Now read from EMEMD
    for (i = 0; i < (packetSize / sizeof(NvU32)); i++)
    {
        ((NvU32 *)(void *)pPacket)[i] = GPU_REG_RD32(pGpu, NV_PFSP_EMEMD(FSP_EMEM_CHANNEL_RM));
    }
    *bytesRead = packetSize;

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PFSP_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PFSP, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PFSP, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "After reading data, ememcOff = 0x%x\n", ememOffsetEnd);

    // Set TAIL = HEAD to indicate CPU received packet
    _kfspUpdateMsgQueueHeadTail_GH100(pGpu, pKernelFsp, msgqHead, msgqHead);

    NV_ASSERT_OR_RETURN((ememOffsetEnd) == (packetSize / sizeof(NvU32)), NV_ERR_INVALID_STATE);
    return NV_OK;
}

static NvBool
_kfspWaitBootCond_GH100
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    //
    // In Hopper, Bootfsm triggers FSP execution out of chip reset.
    // FSP writes 0xFF value in NV_THERM_I2CS_SCRATCH register after completion of boot
    //
    return GPU_FLD_TEST_DRF_DEF(pGpu, _THERM_I2CS_SCRATCH, _FSP_BOOT_COMPLETE, _STATUS, _SUCCESS);
}

NV_STATUS
kfspWaitForSecureBoot_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;

    //
    // Polling for FSP boot complete
    // FBFalcon training during devinit alone takes 2 seconds, up to 3 on HBM3,
    // but the default threadstate timeout on windows is 1800 ms. Increase to 4 seconds
    // for this wait to match MODS GetGFWBootTimeoutMs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    //
    gpuSetTimeout(pGpu, NV_MAX(gpuScaleTimeout(pGpu, 4000000), pGpu->timeoutData.defaultus),
                  &timeout, GPU_TIMEOUT_FLAGS_OSTIMER);

    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GH100, NULL, &timeout);

    if (status != NV_OK)
    {
        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
        NV_ERROR_LOG((void*) pGpu, GPU_INIT_ERROR, "Error status 0x%x while polling for FSP boot complete, "
                     "0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
                     status,
                     GPU_REG_RD32(pGpu, NV_THERM_I2CS_SCRATCH_FSP_BOOT_COMPLETE),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));
    }
    return status;
}

static const BINDATA_ARCHIVE *
kfspGetGspUcodeArchive
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    KernelGsp *pKernelGsp                 = GPU_GET_KERNEL_GSP(pGpu);
    ConfidentialCompute *pCC              = GPU_GET_CONF_COMPUTE(pGpu);
    NV_ASSERT(pCC != NULL);

    if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
    {
        NV_PRINTF(LEVEL_NOTICE, "Loading GSP-RM image using FSP.\n");

        if (kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp))
        {
            if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
            {
                return NULL;
            }
            else
            {

                return kgspGetBinArchiveGspRmFmcGfwDebugSigned_HAL(pKernelGsp);
            }
        }
        else
        {
            if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
            {
                return kgspGetBinArchiveGspRmCcFmcGfwProdSigned_HAL(pKernelGsp);
            }
            else
            {
                return kgspGetBinArchiveGspRmFmcGfwProdSigned_HAL(pKernelGsp);
            }
        }
    }
#if RMCFG_MODULE_ENABLED (GSP)
    else
    {
        Gsp *pGsp = GPU_GET_GSP(pGpu);
        Spdm *pSpdm = GPU_GET_SPDM(pGpu);

        // Intentional error print so that we know which mode RM is loaded with
        NV_PRINTF(LEVEL_ERROR, "Loading GSP image for monolithic RM using FSP.\n");
        if (gspIsDebugModeEnabled_HAL(pGpu, pGsp))
        {
            if (gpuIsGspToBootInInstInSysMode_HAL(pGpu))
            {
                NV_PRINTF(LEVEL_ERROR, "Loading GSP debug inst-in-sys image for monolithic RM using FSP.\n");

                //
                // GB20X inst-in-sys images will reset PMU, FBFALCON, FECS and GPCCS
                // and then lower CPUCTL/reset PLM.
                // Reason for that is that there is fuse configration
                // changing source isolation of those registers (to GSP/FSP/SEC2).
                //
                if (IsGB20X(pGpu))
                {
                    pGsp->setProperty(pGsp, PDB_PROP_GSP_INST_IN_SYS_FMC_WILL_RESET_ENGINES, NV_TRUE);
                }

                return gspGetBinArchiveGspFmcInstInSysGfwDebugSigned_HAL(pGsp);
            }
            else
            {
                //
                // Non Resident (GspCcGfw)Image will have just the FMC in it.
                // When GSP-RM is not enabled, we will need to load GSP RM Proxy.
                // We will prepare the GSP-Proxy Image in SYSMEM And pass that
                // into to the FMC. FMC will then boot the RM Proxy.
                //
                NV_ASSERT_OR_RETURN(gspSetupRMProxyImage(pGpu, pGsp) == NV_OK, NULL);

                // For debug board, when CC enabled, only pick SPDM profile if SPDM is enabled.
                if  (pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED))
                {
                    if (confComputeIsSpdmEnabled(pGpu, pCC) &&
                        pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
                    {
                        return gspGetBinArchiveGspFmcSpdmGfwDebugSigned_HAL(pGsp);
                    }
                    else
                    {
                        // When CC is enabled but SPDM is not enabled. Only for MODS.
                        return gspGetBinArchiveGspFmcGfwDebugSigned_HAL(pGsp);
                    }
                }
                else
                {
                    return gspGetBinArchiveGspFmcGfwDebugSigned_HAL(pGsp);
                }
            }
        }
        else
        {
            if (gpuIsGspToBootInInstInSysMode_HAL(pGpu))
            {
                NV_PRINTF(LEVEL_ERROR, "Loading GSP prod inst-in-sys image for monolithic RM using FSP.\n");

                //
                // GB20X inst-in-sys images will reset PMU, FBFALCON, FECS and GPCCS
                // and then lower CPUCTL/reset PLM.
                // Reason for that is that there is fuse configration
                // changing source isolation of those registers (to GSP/FSP/SEC2).
                //
                if (IsGB20X(pGpu))
                {
                    pGsp->setProperty(pGsp, PDB_PROP_GSP_INST_IN_SYS_FMC_WILL_RESET_ENGINES, NV_TRUE);
                }

                return gspGetBinArchiveGspFmcInstInSysGfwProdSigned_HAL(pGsp);
            }
            else
            {
                NV_ASSERT_OR_RETURN(gspSetupRMProxyImage(pGpu, pGsp) == NV_OK, NULL);
                Spdm  *pSpdm = GPU_GET_SPDM(pGpu);

                if (pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED))
                {
                    if (confComputeIsSpdmEnabled(pGpu, pCC) &&
                        pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
                    {
                        return gspGetBinArchiveGspCcFmcGfwProdSigned_HAL(pGsp);
                    }
                    else
                    {
                        // When CC is enabled but SPDM is not enabled. Only for MODS.
                        return gspGetBinArchiveGspFmcGfwDebugSigned_HAL(pGsp);
                    }
                }
                else
                {
                    return gspGetBinArchiveGspFmcGfwProdSigned_HAL(pGsp);
                }
            }
        }
    }
#endif

    //
    // It does not make sense to boot monolithic RM when physical FSP module
    // does not exist
    //
    return NULL;
}

static NV_STATUS
kfspGetGspBootArgs
(
    OBJGPU     *pGpu,
    KernelFsp  *pKernelFsp,
    RmPhysAddr *pBootArgsGspSysmemOffset
)
{
    NV_STATUS status         = NV_OK;
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    NV_ASSERT(pCC != NULL);

    if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        // Ensure bootArgs have been set up before
        NV_ASSERT_OR_RETURN(pKernelGsp->pGspFmcArgumentsCached != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pKernelGsp->pGspFmcArgumentsDescriptor) == ADDR_SYSMEM, NV_ERR_INVALID_STATE);
        *pBootArgsGspSysmemOffset = memdescGetPhysAddr(pKernelGsp->pGspFmcArgumentsDescriptor, AT_GPU, 0);
    }

    return status;
}


/*!
 * @brief Set up GSP-FMC and boot args for FSP command
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or error if failed
 */
static NV_STATUS
kfspSetupGspImages
(
    OBJGPU           *pGpu,
    KernelFsp        *pKernelFsp,
    NVDM_PAYLOAD_COT *pCotPayload
)
{
    NV_STATUS status = NV_OK;

    const BINDATA_ARCHIVE *pBinArchive;
    PBINDATA_STORAGE pGspImage;
    PBINDATA_STORAGE pGspImageHash;
    PBINDATA_STORAGE pGspImageSignature;
    PBINDATA_STORAGE pGspImagePublicKey;
    NvBool bReUseInitMem  = pGpu->getProperty(pGpu, PDB_PROP_GPU_REUSE_INIT_CONTING_MEM);
    NvU32 pGspImageSize;
    NvU32 pGspImageMapSize;
    NvP64 pVaKernel = NULL;
    NvP64 pPrivKernel = NULL;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    //
    // On systems with SEV enabled, the GSP-FMC image has to be accessible
    // to FSP (an unit inside GPU) and hence placed in unprotected sysmem
    //
    flags = MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    // Detect the mode of operation for GSP and fetch the right image to boot
    pBinArchive = kfspGetGspUcodeArchive(pGpu, pKernelFsp);
    if (pBinArchive == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot find correct ucode archive for booting!\n");
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto failed;
    }

    // Set up the structures to send GSP-FMC
    pGspImage = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_IMAGE);
    pGspImageHash = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_HASH);
    pGspImageSignature = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_SIG);
    pGspImagePublicKey = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_PKEY);

    if ((pGspImage == NULL) || (pGspImageHash == NULL) ||
        (pGspImageSignature == NULL) || (pGspImagePublicKey == NULL))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto failed;
    }

    pGspImageSize = bindataGetBufferSize(pGspImage);
    pGspImageMapSize = NV_ALIGN_UP(pGspImageSize, 0x1000);
    if ((pKernelFsp->pGspFmcMemdesc == NULL) || !bReUseInitMem)
    {
        NV_ASSERT(pKernelFsp->pGspFmcMemdesc == NULL); // If we assert the pointer becomes a zombie.
        status = memdescCreate(&pKernelFsp->pGspFmcMemdesc, pGpu, pGspImageMapSize,
                               0, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED, flags);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_7,
                        pKernelFsp->pGspFmcMemdesc);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);
    }
    status = memdescMap(pKernelFsp->pGspFmcMemdesc, 0, pGspImageMapSize, NV_TRUE,
                        NV_PROTECT_READ_WRITE, &pVaKernel, &pPrivKernel);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    portMemSet(pVaKernel, 0, pGspImageMapSize);

    status = bindataWriteToBuffer(pGspImage, pVaKernel, pGspImageSize);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    // Clean up CPU side resources since they are not needed anymore
    memdescUnmap(pKernelFsp->pGspFmcMemdesc, NV_TRUE, 0, pVaKernel, pPrivKernel);

    pCotPayload->gspFmcSysmemOffset = memdescGetPhysAddr(pKernelFsp->pGspFmcMemdesc, AT_GPU, 0);

    status = bindataWriteToBuffer(pGspImageHash, (NvU8*)pCotPayload->hash384, sizeof(pCotPayload->hash384));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) == pKernelFsp->cotPayloadSignatureSize, failed);
    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) <= sizeof(pCotPayload->signature), failed);
    status = bindataWriteToBuffer(pGspImageSignature, (NvU8*)pCotPayload->signature, bindataGetBufferSize(pGspImageSignature));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) == pKernelFsp->cotPayloadPublicKeySize, failed);
    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) <= sizeof(pCotPayload->publicKey), failed);
    status = bindataWriteToBuffer(pGspImagePublicKey, (NvU8*)pCotPayload->publicKey, bindataGetBufferSize(pGspImagePublicKey));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    // Set up boot args based on the mode of operation
    status = kfspGetGspBootArgs(pGpu, pKernelFsp, &pCotPayload->gspBootArgsSysmemOffset);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    return NV_OK;

failed:
    memdescDestroy(pKernelFsp->pGspFmcMemdesc);
    pKernelFsp->pGspFmcMemdesc = NULL;

    return status;
}

/*!
 * Determine if PRIV target mask is unlocked for GSP and BAR0 Decoupler allows GSP access.
 *
 * This is temporary WAR for the PRIV target mask bug 3640831 until we have notification
 * protocol in place (there is no HW mechanism for CPU to check if GSP is open other than
 * reading 0xBADF41YY code).
 *
 * Until the programmed BAR0 decoupler settings are cleared, GSP access is blocked from
 * the CPU so all reads will return 0.
 */
static NvBool
_kfspIsGspTargetMaskReleased
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    const NvU32   privErrTargetLocked      = 0xBADF4100U;
    const NvU32   privErrTargetLockedMask  = 0xFFFFFF00U; // Ignore LSB - it has extra error information
    NvU32 reg;

    //
    // This register is read with the raw OS read to avoid the 0xbadf sanity checking
    // done by the usual register read utilities.
    //
    reg = osDevReadReg032(pGpu, gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0),
                          DRF_BASE(NV_PGSP) + NV_PFALCON_FALCON_HWCFG2);

    return ((reg != 0) && ((reg & privErrTargetLockedMask) != privErrTargetLocked));
}

/*!
 * Determine if GSP's target mask is released.
 */
NV_STATUS
kfspWaitForGspTargetMaskReleased_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;

    status =  gpuTimeoutCondWait(pGpu, _kfspIsGspTargetMaskReleased, NULL, NULL);

    return status;
}

static NV_STATUS
_kfspCheckGspBootStatus
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    // On GSP-RM, the kgsp code path will check for GSP boot status
    return NV_OK;
}

/*!
 * @brief Dump debug registers for FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or error if failed
 */
void
kfspDumpDebugState_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    //
    // Older microcodes did not have the version populated in scratch.
    // They will report a version of 0.
    //
    const NvU32 fspUcodeVersion = GPU_REG_RD_DRF(pGpu, _GFW, _FSP_UCODE_VERSION, _FULL);
    if (fspUcodeVersion > 0)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP microcode v%u.%u\n",
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MAJOR, fspUcodeVersion),
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MINOR, fspUcodeVersion));
    }

    NV_PRINTF(LEVEL_ERROR, "GPU %04x:%02x:%02x\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu));


    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));

    NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX0 = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PGSP_FALCON_MAILBOX0));
    NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX1 = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PGSP_FALCON_MAILBOX1));
}

/*!
 * @brief Checks whether GSP_FMC is enforced on this
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_TRUE, if GspFmc is enforced.
 */
NvBool
kfspGspFmcIsEnforced_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    return NV_TRUE;
}

/*!
 * @brief Check if okay to send GSP-FMC and FRTS info to FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK
 *     Okay to send boot commands
 * @return NV_WARN_NOTHING_TO_DO
 *     No need to send boot commands
 * @return NV_ERR_NOT_SUPPORTED
 *     Should not send boot commands
 */
static NV_STATUS
kfspSafeToSendBootCommands
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    if (!IS_EMULATION(pGpu) && !IS_SILICON(pGpu))
    {
        //
        // FSP managment partition is only enabled when secure boot is enabled
        // on silicon and certain emulation configs
        //
        return NV_WARN_NOTHING_TO_DO;
    }

    if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_IS_MISSING))
    {
        if (IS_SILICON(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR, "RM cannot boot with FSP missing on silicon.\n");
            return NV_ERR_NOT_SUPPORTED;
        }

        NV_PRINTF(LEVEL_WARNING, "Secure boot is disabled due to missing FSP.\n");
        return NV_WARN_NOTHING_TO_DO;
    }

    // Enforce GSP-FMC can only be booted by FSP on silicon.
    if (IS_SILICON(pGpu) &&
        kfspGspFmcIsEnforced_HAL(pGpu, pKernelFsp) &&
        pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_ERROR, "Chain-of-trust (GSP-FMC) cannot be disabled on silicon.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM) &&
        pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM) &&
        pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_WARNING, "Chain-of-trust is disabled via regkey\n");
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_BOOT_COMMAND_OK, NV_TRUE);
        return NV_WARN_NOTHING_TO_DO;
    }

    return NV_OK;
}

/*!
 * @brief Prepare GSP-FMC and FRTS info to send to FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info ready to send
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped preparing boot commands
 * @return Other error
 *     Error preparing GSP-FMC and FRTS info
 */
NV_STATUS
kfspPrepareBootCommands_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;
    NV_STATUS statusBoot;
    NvU32 data = 0;
    NvU32 frtsSize = 0;
    NvP64 pVaKernel = NULL;
    NvP64 pPrivKernel = NULL;
    NvBool bIsKeepWPRGc6 = IS_GPU_GC6_STATE_EXITING(pGpu);
    NvBool bReUseInitMem  = pGpu->getProperty(pGpu, PDB_PROP_GPU_REUSE_INIT_CONTING_MEM);

    statusBoot = kfspWaitForSecureBoot_HAL(pGpu, pKernelFsp);

    // Confirm FSP secure boot partition is done
    if (statusBoot != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP secure boot partition timed out.\n");
        status = statusBoot;
        goto failed;
    }

    statusBoot = kfspSafeToSendBootCommands(pGpu, pKernelFsp);
    if (statusBoot != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP secure boot GSP prechecks failed.\n");
        status = statusBoot;
        goto failed;
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        if (!IS_GSP_CLIENT(pGpu))
        {
            kfspCheckForClockBoostCapability_HAL(pGpu, pKernelFsp);
        }

        pKernelFsp->bClockBoostDisabledViaRegkey = NV_FALSE;

        // Read and parse clock boost regkey, for the feature override
        if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS, &data) == NV_OK) &&
            (data == NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS_DISABLED))
        {
            pKernelFsp->bClockBoostDisabledViaRegkey = NV_TRUE;
        }
    }

    if (pKernelFsp->bClockBoostSupported)
    {
        NvU8 cmd;

        if (pKernelFsp->bClockBoostDisabledViaRegkey)
        {
            cmd = FSP_CLOCK_BOOST_FEATURE_DISABLE_SUBMESSAGE_ID;
        }
        else
        {
            cmd = FSP_CLOCK_BOOST_FEATURE_ENABLE_SUBMESSAGE_ID;
        }

        status = kfspSendClockBoostRpc_HAL(pGpu, pKernelFsp, cmd);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Clock boost feature cmd %d via FSP failed with error 0x%x\n", cmd, status);
            goto failed;
        }
    }

    // COT payload is freed in FSP state cleanup during state destroy.
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        pKernelFsp->pCotPayload = portMemAllocNonPaged(sizeof(NVDM_PAYLOAD_COT));
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, (pKernelFsp->pCotPayload == NULL) ? NV_ERR_NO_MEMORY : NV_OK, failed);
        portMemSet(pKernelFsp->pCotPayload, 0, sizeof(NVDM_PAYLOAD_COT));
    }

    frtsSize = NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K << 12;
    NV_ASSERT(frtsSize != 0);

    pKernelFsp->pCotPayload->version = pKernelFsp->cotPayloadVersion;
    pKernelFsp->pCotPayload->size = sizeof(NVDM_PAYLOAD_COT);

    // Set up sysmem for FRTS copy
    if (!pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM))
    {
        NvU64 flags = MEMDESC_FLAGS_NONE;

        //
        // On systems with SEV enabled, the FRTS has to be accessible to
        // FSP (an unit inside GPU) and hence placed in unprotected sysmem
        //
        flags = MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
        if ((pKernelFsp->pSysmemFrtsMemdesc == NULL) || !bReUseInitMem)
        {
            NV_ASSERT(pKernelFsp->pSysmemFrtsMemdesc == NULL); // If we assert the pointer becomes a zombie.
            status = memdescCreate(&pKernelFsp->pSysmemFrtsMemdesc, pGpu, frtsSize,
                                   0, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED, flags);
            NV_ASSERT_OR_GOTO(status == NV_OK, failed);

            memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_8,
                            pKernelFsp->pSysmemFrtsMemdesc);
            NV_ASSERT_OR_GOTO(status == NV_OK, failed);
        }

        // Set up a kernel mapping for future use in RM
        status = memdescMap(pKernelFsp->pSysmemFrtsMemdesc, 0, frtsSize, NV_TRUE,
                            NV_PROTECT_READ_WRITE, &pVaKernel, &pPrivKernel);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);

        portMemSet(pVaKernel, 0, frtsSize);

        memdescSetKernelMapping(pKernelFsp->pSysmemFrtsMemdesc, pVaKernel);
        memdescSetKernelMappingPriv(pKernelFsp->pSysmemFrtsMemdesc, pPrivKernel);

        pKernelFsp->pCotPayload->frtsSysmemOffset = memdescGetPhysAddr(pKernelFsp->pSysmemFrtsMemdesc, AT_GPU, 0);
        pKernelFsp->pCotPayload->frtsSysmemSize = frtsSize;

        NV_ASSERT_OK_OR_GOTO(status,
            kfspFrtsSysmemLocationProgram_HAL(pGpu, pKernelFsp),
            failed);
    }

    //
    // Set up vidmem for FRTS copy
    // With Keep WPR feature, set Frts_In_Fb_Requested to False for VBIOS.
    //
    if (!pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM) && !bIsKeepWPRGc6)
    {
        //
        // Since we are very early in the boot path, we cannot know how much
        // vidmem reservation RM will need at the end of FB. For now use an
        // estimated value to leave enough space for buffers such as vga
        // workspace, BAR instance blocks and BAR page directories which will
        // be allocated at the end of FB. If more reservations are added in the
        // future, this code will need to be updated.
        // Bug 200711957 has more info and tracks longer term improvements.
        //

        // Offset from end of FB to be used by FSP
        NvU64 frtsOffsetFromEnd =
            memmgrGetFBEndReserveSizeEstimate_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu));

        //
        // Layout: 0|| ....... | FRTS | PMU | rsvd est ||END
        // frtsOffsetFromEnd =        ^ .............. ^
        //
        if (kpmuReservedMemorySizeGet(GPU_GET_KERNEL_PMU(pGpu)) != 0U)
        {
            frtsOffsetFromEnd += kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp) +
                kpmuReservedMemorySizeGet(GPU_GET_KERNEL_PMU(pGpu));

            //
            // 2M alignment seems to be required? Should be NOP unless kpmuReserve
            // is non 0 and GH100 WAR is needed
            //
            frtsOffsetFromEnd = NV_ALIGN_UP(frtsOffsetFromEnd, 0x200000U);
        }
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        //
        // In the boot retry path, we may need to apply an extra margin the end of memory.
        // This is done to avoid memory with an ECC error that caused the first boot
        // attempt failure. This value will be 0 during normal boot.
        //
        // Align the margin size to 2MB, as there's potentially an undocumented alignment
        // requirement (the previous value should already be 2MB-aligned) and the extra
        // padding won't hurt.
        //
        if (pKernelGsp != NULL)
            frtsOffsetFromEnd += NV_ALIGN_UP64(kgspGetWprEndMargin(pGpu, pKernelGsp), 0x200000U);

        pKernelFsp->pCotPayload->frtsVidmemOffset = frtsOffsetFromEnd;
        pKernelFsp->pCotPayload->frtsVidmemSize = frtsSize;
    }
    else
    {
        pKernelFsp->pCotPayload->frtsVidmemOffset = 0;
        pKernelFsp->pCotPayload->frtsVidmemSize = 0;
    }

    if (!bIsKeepWPRGc6)
    {
        pKernelFsp->pCotPayload->gspFmcSysmemOffset = (NvU64)-1;
        pKernelFsp->pCotPayload->gspBootArgsSysmemOffset = (NvU64)-1;
    }

    // Set up GSP-FMC for FSP to boot GSP
    if (!pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        // With Keep WPR feature, we can reuse the GSP-FMC image in FB.
        if (!bIsKeepWPRGc6)
        {
            status = kfspSetupGspImages(pGpu, pKernelFsp, pKernelFsp->pCotPayload);
            if (status!= NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Ucode image preparation failed!\n");
                goto failed;
            }
        }

    }
    return NV_OK;

failed:
    NV_PRINTF(LEVEL_ERROR, "Preparing FSP boot cmds failed. RM cannot boot.\n");

    kfspCleanupBootState(pGpu, pKernelFsp);

    return status;
}
/*!
 * @brief Send GSP-FMC and FRTS info to FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info sent to FSP
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped sending boot commands
 * @return Other error
 *     Error sending GSP-FMC and FRTS info to FSP
 */
NV_STATUS
kfspSendBootCommands_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelFsp->pCotPayload != NULL, NV_ERR_INVALID_STATE);

    status = kfspSendAndReadMessage(pGpu, pKernelFsp, (NvU8 *)pKernelFsp->pCotPayload,
                                    sizeof(NVDM_PAYLOAD_COT), NVDM_TYPE_COT, NULL, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Sent following content to FSP: \n");
        NV_PRINTF(LEVEL_ERROR, "version=0x%x, size=0x%x, gspFmcSysmemOffset=0x%llx\n",
            pKernelFsp->pCotPayload->version, pKernelFsp->pCotPayload->size,
            pKernelFsp->pCotPayload->gspFmcSysmemOffset);
        NV_PRINTF(LEVEL_ERROR, "frtsSysmemOffset=0x%llx, frtsSysmemSize=0x%x\n",
            pKernelFsp->pCotPayload->frtsSysmemOffset, pKernelFsp->pCotPayload->frtsSysmemSize);
        NV_PRINTF(LEVEL_ERROR, "frtsVidmemOffset=0x%llx, frtsVidmemSize=0x%x\n",
            pKernelFsp->pCotPayload->frtsVidmemOffset, pKernelFsp->pCotPayload->frtsVidmemSize);
        NV_PRINTF(LEVEL_ERROR, "gspBootArgsSysmemOffset=0x%llx\n",
            pKernelFsp->pCotPayload->gspBootArgsSysmemOffset);
        goto failed;
    }

    //
    // Need to check if GSP has been booted here so that we can skip booting
    // GSP again later in ACR code path. On GSP-RM, the calling code path (kgsp)
    // will check for GSP boot status.
    //
    if (!pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC) &&
        !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
    {
        status = _kfspCheckGspBootStatus(pGpu, pKernelFsp);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);
    }

    // Set property to indicate we only support secure boot at this point
    pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_BOOT_COMMAND_OK, NV_TRUE);
    return NV_OK;

failed:
    NV_PRINTF(LEVEL_ERROR, "FSP boot cmds failed. RM cannot boot.\n");
    kfspDumpDebugState_HAL(pGpu, pKernelFsp);

    kfspCleanupBootState(pGpu, pKernelFsp);

    return status;
}

/*!
 * @brief Prepare and send GSP-FMC and FRTS info to FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info sent to FSP or determined okay to skip sending info
 * @return Other error
 *     Error preparing or sending GSP-FMC and FRTS info to FSP
 */
NV_STATUS
kfspPrepareAndSendBootCommands_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status;
    status = kfspPrepareBootCommands_GH100(pGpu, pKernelFsp);
    if (status != NV_OK)
    {
        return (status == NV_WARN_NOTHING_TO_DO) ? NV_OK : status;
    }
    return kfspSendBootCommands_GH100(pGpu, pKernelFsp);
}

NV_STATUS
kfspErrorCode2NvStatusMap_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      errorCode
)
{
    switch (errorCode)
    {
        case FSP_OK:
        return NV_OK;

        case FSP_ERR_IFR_FILE_NOT_FOUND:
        return NV_ERR_OBJECT_NOT_FOUND;

        case FSP_ERR_IFS_ERR_INVALID_STATE:
        case FSP_ERR_IFS_ERR_INVALID_DATA:
        return NV_ERR_INVALID_DATA;

        case FSP_ERR_PRC_ERROR_INVALID_KNOB_ID:
        return NV_ERR_INVALID_ARGUMENT;

        default:
        return NV_ERR_GENERIC;
    }
}

/*!
 * Size of extra memory required to be reserved after FRTS region
 */
NvU64
kfspGetExtraReservedMemorySize_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    // Bug: 3763996
    return 4 * 1024;
}

NvBool
kfspRequiresBug3957833WAR_GH100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    const NvU32 FSP_BUG_3957833_FIX_VERSION = 0x44C;
    const NvU32 fspUcodeVersion = GPU_REG_RD_DRF(pGpu, _GFW, _FSP_UCODE_VERSION, _FULL);
    return fspUcodeVersion < FSP_BUG_3957833_FIX_VERSION;
}

NV_STATUS
kfspFrtsSysmemLocationProgram_GH100
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status;
    RmPhysAddr frtsSysmemAddr;

    NV_ASSERT_TRUE_OR_GOTO(status,
        (pKernelFsp->pSysmemFrtsMemdesc != NULL),
        NV_ERR_INVALID_STATE,
        kfspFrtsSysmemLocationProgram_GH100_exit);

    frtsSysmemAddr = memdescGetPhysAddr(
        pKernelFsp->pSysmemFrtsMemdesc, AT_GPU, 0U);

    GPU_REG_WR32(
        pGpu, NV_PBUS_SW_FRTS_INSECURE_ADDR_LO32, NvU64_LO32(frtsSysmemAddr));
    GPU_REG_WR32(
        pGpu, NV_PBUS_SW_FRTS_INSECURE_ADDR_HI32, NvU64_HI32(frtsSysmemAddr));
    GPU_REG_WR32(
        pGpu,
        NV_PBUS_SW_FRTS_INSECURE_CONFIG,
        FLD_SET_DRF(
            _PBUS, _SW_FRTS_INSECURE_CONFIG, _MEDIA_TYPE, _SYSMEM,
        REF_NUM(
            NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K,
            (memdescGetSize(pKernelFsp->pSysmemFrtsMemdesc) >>
                NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K_SHIFT))));

kfspFrtsSysmemLocationProgram_GH100_exit:
    return status;
}

void
kfspFrtsSysmemLocationClear_GH100
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    GPU_REG_WR32(
        pGpu,
        NV_PBUS_SW_FRTS_INSECURE_CONFIG,
        REF_DEF(NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K, _INVALID));
    GPU_REG_WR32(
        pGpu, NV_PBUS_SW_FRTS_INSECURE_ADDR_HI32, 0U);
    GPU_REG_WR32(
        pGpu, NV_PBUS_SW_FRTS_INSECURE_ADDR_LO32, 0U);
}
