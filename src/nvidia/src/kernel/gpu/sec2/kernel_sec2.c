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

#include "gpu/sec2/kernel_sec2.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/core.h"
#include "gpu/eng_desc.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/gpu.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/fsp/kern_fsp_cot_payload.h"
#include "rmapi/event.h"

#if RMCFG_MODULE_ENABLED (GSP)
#include "gpu/gsp/gsp.h"
#include "gpu/falcon/objflcnable.h"
#endif

//
// SEC2 communication uses two headers: MCTP per packet and NVDM per message.
// Packet size is based on the underlying transport mechanism which can be
// queried with ksec2GetMax[Send,Recv]PacketSize. If a message is larger than
// can fit in a single packet, only the first packet contains the NVDM header
// while all packets contain the MCTP header. Each of these headers is one
// 32-bit DWORD and these indexes are used when placing the DWORD in the packet.
//
#define HEADER_DWORD_MCTP 0
#define HEADER_DWORD_NVDM 1
#define HEADER_DWORD_MAX  2

/*!
* Local object related functions
*/
static void _ksec2InitRegistryOverrides(OBJGPU *, KernelSec2 *);
static NV_STATUS _ksec2WaitForResponse(OBJGPU *pGpu, KernelSec2 *pKernelSec2);
static NV_INLINE void _ksec2SetResponseTimeout(OBJGPU *pGpu, KernelSec2 *pKernelSec2);
static NV_INLINE NV_STATUS _ksec2CheckResponseTimeout(OBJGPU *pGpu, KernelSec2 *pKernelSec2);
static NV_STATUS _ksec2ReadMessage(OBJGPU *pGpu, KernelSec2 *pKernelSec2, NvU8 *pPayloadBuffer, NvU32 payloadBufferSize);
static void _ksec2ReleaseGspBootImages(OBJGPU *pGpu, KernelSec2 *pKernelSec2);

NV_STATUS
ksec2ConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    ENGDESCRIPTOR engDesc
)
{
    NV_STATUS status;
    status = ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2);

    // Initialize based on registry keys
    _ksec2InitRegistryOverrides(pGpu, pKernelSec2);
    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_IS_MISSING))
    {
        NV_PRINTF(LEVEL_WARNING, "KernelSec2 is disabled\n");
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    return status;
}

/*!
 * Initialize all registry overrides for this object
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelSec2 KernelSec2 object pointer
 */
static void
_ksec2InitRegistryOverrides
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NvU32 data = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DEVINIT_BY_SECURE_BOOT, &data) == NV_OK && data == NV_REG_STR_RM_DEVINIT_BY_SECURE_BOOT_DISABLE)
    {
        NV_PRINTF(LEVEL_WARNING, "RM to boot GSP due to regkey override.\n");
        pKernelSec2->setProperty(pKernelSec2, PDB_PROP_KSEC2_RM_BOOT_GSP, NV_TRUE);
    }

    if (((osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_SEC2, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_DISABLE_SEC2_YES) && IS_EMULATION(pGpu)) ||
        IS_FMODEL(pGpu) || IS_RTLSIM(pGpu))
    {
        //
        // Force disable SEC2 engine, used only on emulation because some
        // emulation netlists stub out SEC2 but leave the engine in PTOP
        //
        NV_PRINTF(LEVEL_WARNING, "SEC2 disabled due to regkey override.\n");
        pKernelSec2->setProperty(pKernelSec2, PDB_PROP_KSEC2_IS_MISSING, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_COT_CMD, &data) == NV_OK)
    {
        // Assume non-zero value only has NV_REG_STR_RM_DISABLE_COT_CMD_YES
        if (data & DRF_SHIFTMASK(NV_REG_STR_RM_DISABLE_COT_CMD_GSPFMC))
        {
            pKernelSec2->setProperty(pKernelSec2, PDB_PROP_KSEC2_DISABLE_GSPFMC, NV_TRUE);
        }
    }

}

void
ksec2ReleaseProxyImage_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    // With Keep WPR feature, keep the GSP-FMC and BootArgsMemdesc.
    if (!(pGpu->getProperty(pGpu, PDB_PROP_GPU_KEEP_WPR_ACROSS_GC6_SUPPORTED) && IS_GPU_GC6_STATE_ENTERING(pGpu)))
    {
        if (pKernelSec2->pGspFmcMemdesc != NULL)
        {
            memdescFree(pKernelSec2->pGspFmcMemdesc);
            memdescDestroy(pKernelSec2->pGspFmcMemdesc);
            pKernelSec2->pGspFmcMemdesc = NULL;
        }

        if (pKernelSec2->pGspBootArgsMemdesc != NULL)
        {
            memdescFree(pKernelSec2->pGspBootArgsMemdesc);
            memdescDestroy(pKernelSec2->pGspBootArgsMemdesc);
            pKernelSec2->pGspBootArgsMemdesc = NULL;
        }
    }
}

/*!
 * @brief Unload SEC2 state
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelSec2  SEC2 object pointer
 * @param[in]  flags
 */

NV_STATUS
ksec2StateUnload_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU32      flags
)
{
    ksec2ReleaseProxyImage(pGpu, pKernelSec2);
    return NV_OK;
}

/*!
 * @brief Poll for response from SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
NV_STATUS
ksec2PollForResponse_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    _ksec2SetResponseTimeout(pGpu, pKernelSec2);
    return _ksec2WaitForResponse(pGpu, pKernelSec2);
}

/*!
 * @brief Wait for response from SEC2 via RM message queue
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
static NV_STATUS
_ksec2WaitForResponse
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    // Poll for message queue to wait for SEC2's reply
    while (!ksec2IsResponseAvailable_HAL(pGpu, pKernelSec2))
    {
        osSpinLoop();

        status = _ksec2CheckResponseTimeout(pGpu, pKernelSec2);
        if (status != NV_OK)
        {
            if ((status == NV_ERR_TIMEOUT) &&
                ksec2IsResponseAvailable_HAL(pGpu, pKernelSec2))
            {
                status = NV_OK;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "SEC2 command timed out\n");
            }
            break;
        }
    }

    return status;
}

/*!
 * @brief Set a timeout for response from SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 */
static NV_INLINE void
_ksec2SetResponseTimeout
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2
)
{
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &(pKernelSec2->rpcTimeout),
                  GPU_TIMEOUT_FLAGS_OSTIMER);
}

/*!
 * @brief Check for timeout for response from SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
static NV_INLINE NV_STATUS
_ksec2CheckResponseTimeout
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2
)
{
    return gpuCheckTimeout(pGpu, &(pKernelSec2->rpcTimeout));
}

/*!
 * @brief Send a MCTP message to SEC2 via EMEM
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelSec2         KernelSec2 pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 *
 * @return NV_OK, or NV_ERR_*
 */
NV_STATUS
ksec2SendMessage_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pPayload,
    NvU32      size,
    NvU32      nvdmType
)
{
    NvU32 dataSent;
    NvU32 dataRemaining;
    NvU32 packetPayloadCapacity;
    NvU32 curPayloadSize;
    NvU32 headerSize;
    NvU32 *pHeader;
    NvU32 maxPacketSize;
    NvBool bSinglePacket;
    NV_STATUS status;
    NvU8 *pBuffer = NULL;
    NvU8 seq = 0;
    NvU8 seid = 0;

    //
    // Check if message will fit in single packet
    // We lose 2 DWORDS to MCTP and NVDM headers
    //
    headerSize = sizeof(NvU32) * HEADER_DWORD_MAX;
    maxPacketSize = ksec2GetMaxSendPacketSize_HAL(pGpu, pKernelSec2);
    packetPayloadCapacity = maxPacketSize - headerSize;
    bSinglePacket = (size <= packetPayloadCapacity);
    // Allocate buffer to hold a full packet
    pBuffer = portMemAllocNonPaged(maxPacketSize);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pBuffer != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pBuffer, 0, maxPacketSize);
    pHeader = (NvU32*)pBuffer;
    // First packet
    seid = ksec2NvdmToSeid_HAL(pGpu, pKernelSec2, nvdmType);
    // SOM=1,EOM=?,SEID,SEQ=0
    pHeader[HEADER_DWORD_MCTP] = ksec2CreateMctpHeader_HAL(pGpu, pKernelSec2, 1,
                                                     (NvU8)bSinglePacket, seid, seq);
    pHeader[HEADER_DWORD_NVDM] = ksec2CreateNvdmHeader_HAL(pGpu, pKernelSec2, nvdmType);

    curPayloadSize = NV_MIN(size, packetPayloadCapacity);
    portMemCopy(pBuffer + headerSize, packetPayloadCapacity, pPayload, curPayloadSize);
    status = ksec2SendPacket_HAL(pGpu, pKernelSec2, pBuffer, curPayloadSize + headerSize);
    if (status != NV_OK)
    {
        goto failed;
    }

    if (!bSinglePacket)
    {
        // Multi packet case
        dataSent = curPayloadSize;
        dataRemaining = size - dataSent;
        headerSize = sizeof(NvU32); // No longer need NVDM header
        packetPayloadCapacity = maxPacketSize - headerSize;

        while (dataRemaining > 0)
        {
            NvBool bLastPacket = (dataRemaining <= packetPayloadCapacity);
            curPayloadSize = (bLastPacket) ? dataRemaining : packetPayloadCapacity;

            pHeader[HEADER_DWORD_MCTP] = ksec2CreateMctpHeader_HAL(pGpu, pKernelSec2, 0,
                                                             (NvU8)bLastPacket, seid,
                                                             (++seq) % 4);
            portMemCopy(pBuffer + headerSize, packetPayloadCapacity,
                        pPayload + dataSent, curPayloadSize);

            status = ksec2SendPacket_HAL(pGpu, pKernelSec2, pBuffer, curPayloadSize + headerSize);
            if (status != NV_OK)
            {
                goto failed;
            }

            dataSent += curPayloadSize;
            dataRemaining -= curPayloadSize;
        }
    }

failed:
    portMemFree(pBuffer);

    return status;
}

/*!
 * @brief Read and process message from SEC2 via RM message queue.
 *
 * Supports both single and multi-packet message. For multi-packet messages, this
 * loops until all packets are received, polling at each iteration for the next
 * packet to come in. If a buffer is provided, the message payload will be
 * returned there.
 *
 * @note: For multi-packet messages, a buffer in which the message payload will
 * be reconstructed must be provided.
 *
 * @param[in]     pGpu              OBJGPU pointer
 * @param[in]     pKernelSec2        KernelSec2 pointer
 * @param[in/out] pPayloadBuffer    Buffer in which to return message payload
 * @param[in]     payloadBufferSize Payload buffer size
 *
 * @return NV_OK, NV_ERR_INVALID_DATA, NV_ERR_INSUFFICIENT_RESOURCES, or errors
 *         from functions called within
 */
static NV_STATUS
_ksec2ReadMessage
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pPayloadBuffer,
    NvU32      payloadBufferSize
)
{
    NvU8             *pPacketBuffer;
    NV_STATUS         status;
    NvU32             totalPayloadSize = 0;
    NvU32             recvBufferSize;
    MCTP_PACKET_STATE packetState = MCTP_PACKET_STATE_START;

    if (!ksec2IsResponseAvailable_HAL(pGpu, pKernelSec2))
    {
        NV_PRINTF(LEVEL_WARNING, "Tried to read SEC2 response but none is available\n");
        return NV_OK;
    }

    recvBufferSize = ksec2GetMaxRecvPacketSize(pGpu, pKernelSec2);
    pPacketBuffer = portMemAllocNonPaged(recvBufferSize);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pPacketBuffer != NULL, NV_ERR_NO_MEMORY);

    while ((packetState != MCTP_PACKET_STATE_END) && (packetState != MCTP_PACKET_STATE_SINGLE_PACKET))
    {
        NvU32 packetSize;
        NvU32 curPayloadSize;
        NvU8  curHeaderSize;
        NvU8  tag;

        // Wait for next packet
        status = ksec2PollForResponse(pGpu, pKernelSec2);
        if (status != NV_OK)
        {
            goto done;
        }

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                ksec2ReadPacket_HAL(pGpu, pKernelSec2, pPacketBuffer, recvBufferSize, &packetSize),
                done);

        status = ksec2GetPacketInfo_HAL(pGpu, pKernelSec2, pPacketBuffer, packetSize, &packetState, &tag);
        if (status != NV_OK)
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
            NV_PRINTF(LEVEL_ERROR, "No buffer provided when receiving multi-packet message. Buffer needed to reconstruct message\n");
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto done;
        }

        if (pPayloadBuffer != NULL)
        {
            if (payloadBufferSize < (totalPayloadSize + curPayloadSize))
            {
                NV_PRINTF(LEVEL_ERROR, "Buffer provided for message payload too small. Payload size: 0x%x Buffer size: 0x%x\n",
                          totalPayloadSize + curPayloadSize, payloadBufferSize);
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto done;
            }
            portMemCopy(pPayloadBuffer + totalPayloadSize, payloadBufferSize - totalPayloadSize,
                        pPacketBuffer + curHeaderSize, curPayloadSize);
        }
        totalPayloadSize += curPayloadSize;
    }

    NvU8 *pMessagePayload = (pPayloadBuffer == NULL) ? (pPacketBuffer + sizeof(MCTP_HEADER)) : pPayloadBuffer;

    status = ksec2ProcessNvdmMessage_HAL(pGpu, pKernelSec2, pMessagePayload, totalPayloadSize);

done:
    portMemFree(pPacketBuffer);
    return status;
}

/*!
 * @brief Send a MCTP message to SEC2 and read response
 *
 *
 * Response payload buffer is optional if response fits in a single packet.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelSec2         KernelSec2 pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 * @param[in] pResponsePayload   Buffer in which to return response payload
 * @param[in] responseBufferSize Response payload buffer size
 *
 * @return NV_OK, or NV_ERR_*
 */
NV_STATUS
ksec2SendAndReadMessage_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pPayload,
    NvU32      size,
    NvU32      nvdmType,
    NvU8      *pResponsePayload,
    NvU32      responseBufferSize
)
{
    NV_STATUS status;

    status = ksec2SendMessage(pGpu, pKernelSec2, pPayload, size, nvdmType);
    if (status != NV_OK)
    {
        return status;
    }

    status = ksec2PollForResponse(pGpu, pKernelSec2);
    if (status != NV_OK)
    {
        return status;
    }

    return _ksec2ReadMessage(pGpu, pKernelSec2, pResponsePayload, responseBufferSize);;
}

void
ksec2Destruct_IMPL
(
    KernelSec2 *pKernelSec2
)
{
    portMemFree((void * /* const_cast */) pKernelSec2->pGenericBlUcodeDesc);
    pKernelSec2->pGenericBlUcodeDesc = NULL;

    portMemFree((void * /* const_cast */) pKernelSec2->pGenericBlUcodeImg);
    pKernelSec2->pGenericBlUcodeImg = NULL;
}

void
ksec2RegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelSec2, KernelFalcon);
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon);

    // Register to handle nonstalling interrupts
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon->physEngDesc != ENG_INVALID);

    NvU32 mcIdx = MC_ENGINE_IDX_SEC2;

    NV_PRINTF(LEVEL_INFO, "Registering 0x%x/0x%x to handle SEC2 nonstall intr\n", pKernelFalcon->physEngDesc, mcIdx);

    NV_ASSERT(pRecords[mcIdx].pNotificationService == NULL);
    pRecords[mcIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[mcIdx].pNotificationService = staticCast(pKernelSec2, IntrService);

}

NV_STATUS
ksec2ServiceNotificationInterrupt_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    IntrServiceServiceNotificationInterruptArguments *pParams
)
{
    NV_PRINTF(LEVEL_INFO, "servicing nonstall intr for SEC2 engine\n");

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu, RM_ENGINE_TYPE_SEC2);
    return NV_OK;
}



/*!
 * @brief Destroy SEC2 state
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelSec2 SEC2 object pointer
 */
void
ksec2StateDestroy_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{

    ksec2CleanupBootState(pGpu, pKernelSec2);

}

/*!
 * @brief Wait until RM can send to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
NV_STATUS
ksec2PollForCanSend_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout,
        GPU_TIMEOUT_FLAGS_OSTIMER);

    while (!ksec2CanSendPacket_HAL(pGpu, pKernelSec2))
    {
        osSpinLoop();

        status = gpuCheckTimeout(pGpu, &timeout);
        if (status != NV_OK)
        {
            if ((status == NV_ERR_TIMEOUT) &&
                ksec2CanSendPacket_HAL(pGpu, pKernelSec2))
            {
                status = NV_OK;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Timed out waiting for SEC2 command queue to be empty.\n");
            }
            break;
        }
    }

    return status;
}


static void
_ksec2ReleaseGspBootImages
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    if (pKernelSec2->pGspFmcMemdesc != NULL)
    {
        memdescFree(pKernelSec2->pGspFmcMemdesc);
        memdescDestroy(pKernelSec2->pGspFmcMemdesc);
        pKernelSec2->pGspFmcMemdesc = NULL;
    }
}

/*!
 * @brief Clean up objects used when sending GSP-FMC info to SEC2
 *
 * @param[in]  pGpu         GPU object pointer
 * @param[in]  pKernelSec2  SEC2 object pointer
 */
void
ksec2CleanupBootState_IMPL
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    if (pKernelSec2->pCotPayload)
    {
        portMemFree(pKernelSec2->pCotPayload);
        pKernelSec2->pCotPayload = NULL;
    }

    _ksec2ReleaseGspBootImages(pGpu, pKernelSec2);

}

