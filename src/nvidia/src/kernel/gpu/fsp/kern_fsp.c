/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
  * @file   kern_fsp.c
  * @brief  Provides all kernel side interfaces for tracking the FSP state,
  *         submitting commands and parsing replies needed from the CPU.
  */
#include "gpu/gpu.h"
#include "gpu/fsp/kern_fsp.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "gpu_mgr/gpu_mgr.h"
#include "fsp/fsp_clock_boost_rpc.h"

#if RMCFG_MODULE_ENABLED (GSP)
#include "gpu/gsp/gsp.h"
#include "gpu/falcon/objflcnable.h"
#endif

#define ASYNC_FSP_POLL_PERIOD_MS 50

/*!
* Local object related functions
*/
static void kfspInitRegistryOverrides(OBJGPU *, KernelFsp *);
static NV_STATUS kfspWaitForResponse(OBJGPU *pGpu, KernelFsp *pKernelFsp);
static NV_INLINE void kfspSetResponseTimeout(OBJGPU *pGpu, KernelFsp *pKernelFsp);
static NV_INLINE NV_STATUS kfspCheckResponseTimeout(OBJGPU *pGpu, KernelFsp *pKernelFsp);
static NV_STATUS kfspSendMessage(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType);
static NV_STATUS kfspReadMessage(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *pPayloadBuffer, NvU32 payloadBufferSize);
static NV_STATUS kfspPollForAsyncResponse(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pEvent);
static void kfspProcessAsyncResponseCallback(NvU32 gpuInstance, void *pCallbackArgs);
static void kfspProcessAsyncResponse(OBJGPU *pGpu,KernelFsp *pKernelFsp);
static void kfspExecuteAsyncRpcCallback(KernelFsp *pKernelFsp, NV_STATUS status);
static NV_STATUS kfspScheduleAsyncResponseCheck(OBJGPU *pGpu, KernelFsp *pKernelFsp,
                                                AsyncRpcCallback callback, void  *pCallbackArgs,
                                                NvU8 *pBuffer, NvU32  bufferSize);
static void kfspClearAsyncResponseState(KernelFsp *pKernelFsp);

NV_STATUS
kfspConstructEngine_IMPL(OBJGPU *pGpu, KernelFsp *pKernelFsp, ENGDESCRIPTOR engDesc)
{
    NV_STATUS status;

    // Initialize based on registry keys
    kfspInitRegistryOverrides(pGpu, pKernelFsp);
    if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_IS_MISSING))
    {
        NV_PRINTF(LEVEL_WARNING, "KernelFsp is disabled\n");
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    kfspClearAsyncResponseState(pKernelFsp);

    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    status = tmrEventCreate(pTmr, &(pKernelFsp->pPollEvent),
                            kfspPollForAsyncResponse, NULL,
                            TMR_FLAGS_NONE);

    return status;
}

/*!
 * Initialize all registry overrides for this object
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelFsp KernelFsp object pointer
 */
static void
kfspInitRegistryOverrides
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 data = 0;

    if (((osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_FSP, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_DISABLE_FSP_YES) && IS_EMULATION(pGpu)) ||
        IS_FMODEL(pGpu) || IS_RTLSIM(pGpu))
    {
        //
        // Force disable FSP engine, used only on emulation because some
        // emulation netlists stub out FSP but leave the engine in PTOP
        //
        NV_PRINTF(LEVEL_WARNING, "FSP disabled due to regkey override.\n");
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_IS_MISSING, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_COT_CMD, &data) == NV_OK)
    {
        // Assume non-zero value only has NV_REG_STR_RM_DISABLE_COT_CMD_YES
        if (data & DRF_SHIFTMASK(NV_REG_STR_RM_DISABLE_COT_CMD_FRTS_SYSMEM))
        {
            pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM, NV_TRUE);
        }

        if (data & DRF_SHIFTMASK(NV_REG_STR_RM_DISABLE_COT_CMD_FRTS_VIDMEM))
        {
            pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM, NV_TRUE);
        }

        if (data & DRF_SHIFTMASK(NV_REG_STR_RM_DISABLE_COT_CMD_GSPFMC))
        {
            pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC, NV_TRUE);
        }
    }

    // Inst-in-sys must only set up FRTS in SYSMEM. This includes FB broken and cache only.
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
        gpuIsCacheOnlyModeEnabled(pGpu))
    {
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM, NV_TRUE);
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK_YES))
    {
        NV_PRINTF(LEVEL_ERROR, "FSP's fuse error detection status check "
                               "during boot is disabled using the regkey.\n");
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED, NV_FALSE);
    }
}


/*!
 * @brief Clean up objects used when sending GSP-FMC and FRTS info to FSP
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelFsp  FSP object pointer
 */
void
kfspCleanupBootState_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    tmrEventDestroy(pTmr, pKernelFsp->pPollEvent);

    portMemFree(pKernelFsp->pCotPayload);
    pKernelFsp->pCotPayload = NULL;

    if (pKernelFsp->pSysmemFrtsMemdesc != NULL)
    {
        kfspFrtsSysmemLocationClear_HAL(pGpu, pKernelFsp);
        memdescUnmap(pKernelFsp->pSysmemFrtsMemdesc, NV_TRUE, 0,
            memdescGetKernelMapping(pKernelFsp->pSysmemFrtsMemdesc),
            memdescGetKernelMappingPriv(pKernelFsp->pSysmemFrtsMemdesc));
        memdescFree(pKernelFsp->pSysmemFrtsMemdesc);
        memdescDestroy(pKernelFsp->pSysmemFrtsMemdesc);
        pKernelFsp->pSysmemFrtsMemdesc = NULL;
    }

    if (pKernelFsp->pGspFmcMemdesc != NULL)
    {
        memdescFree(pKernelFsp->pGspFmcMemdesc);
        memdescDestroy(pKernelFsp->pGspFmcMemdesc);
        pKernelFsp->pGspFmcMemdesc = NULL;
    }

    if (pKernelFsp->pGspBootArgsMemdesc != NULL)
    {
        memdescFree(pKernelFsp->pGspBootArgsMemdesc);
        memdescDestroy(pKernelFsp->pGspBootArgsMemdesc);
        pKernelFsp->pGspBootArgsMemdesc = NULL;
    }

    if (pKernelFsp->bClockBoostSupported)
    {
        NV_STATUS status = kfspSendClockBoostRpc_HAL(pGpu, pKernelFsp,
                                                     FSP_CLOCK_BOOST_FEATURE_DISABLE_SUBMESSAGE_ID);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,"Clock boost disbalement via FSP failed with error 0x%x\n", status);
        }
    }
}

/*!
 * @brief Destroy FSP state
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelFsp  FSP object pointer
 */
void
kfspStateDestroy_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    kfspCleanupBootState(pGpu, pKernelFsp);

    if (pKernelFsp->pVidmemFrtsMemdesc != NULL)
    {
        memdescFree(pKernelFsp->pVidmemFrtsMemdesc);
        memdescDestroy(pKernelFsp->pVidmemFrtsMemdesc);
        pKernelFsp->pVidmemFrtsMemdesc = NULL;
    }
}

/*!
 * @brief Check if FSP RM command queue is empty
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_TRUE if queue is empty, NV_FALSE otherwise
 */
NvBool
kfspIsQueueEmpty_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 cmdqHead, cmdqTail;

    kfspGetQueueHeadTail_HAL(pGpu, pKernelFsp, &cmdqHead, &cmdqTail);

    // FSP will set QUEUE_HEAD = TAIL after each packet is received
    return (cmdqHead == cmdqTail);
}

/*!
 * @brief Wait for FSP RM command queue to be empty
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
NV_STATUS
kfspPollForQueueEmpty_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout,
        GPU_TIMEOUT_FLAGS_OSTIMER |
        GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    while (!kfspIsQueueEmpty(pGpu, pKernelFsp))
    {
        //
        // For now we assume that any response from FSP before RM message
        // send is complete indicates an error and we should abort.
        //
        // Ongoing dicussion on usefullness of this check. Bug to be filed.
        //
        if (!kfspIsMsgQueueEmpty(pGpu, pKernelFsp))
        {
            kfspReadMessage(pGpu, pKernelFsp, NULL, 0);
            NV_PRINTF(LEVEL_ERROR,
                "Received error message from FSP while waiting for CMDQ to be empty.\n");
            status = NV_ERR_GENERIC;
            break;
        }

        osSpinLoop();

        status = gpuCheckTimeout(pGpu, &timeout);
        if (status != NV_OK)
        {
            if ((status == NV_ERR_TIMEOUT) &&
                kfspIsQueueEmpty(pGpu, pKernelFsp))
            {
                status = NV_OK;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Timed out waiting for FSP command queue to be empty.\n");
            }
            break;
        }
    }

    return status;
}

/*!
 * @brief Check if FSP RM message queue is empty
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_TRUE if queue is empty, NV_FALSE otherwise
 */
NvBool
kfspIsMsgQueueEmpty_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 msgqHead, msgqTail;

    kfspGetMsgQueueHeadTail_HAL(pGpu, pKernelFsp, &msgqHead, &msgqTail);
    return (msgqHead == msgqTail);
}

/*!
 * @brief Poll for response from FSP via RM message queue
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
NV_STATUS
kfspPollForResponse_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    kfspSetResponseTimeout(pGpu, pKernelFsp);
    return kfspWaitForResponse(pGpu, pKernelFsp);
}

/*!
 * @brief Wait for response from FSP via RM message queue
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
static NV_STATUS
kfspWaitForResponse
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;

    // Poll for message queue to wait for FSP's reply
    while (kfspIsMsgQueueEmpty(pGpu, pKernelFsp))
    {
        osSpinLoop();

        status = kfspCheckResponseTimeout(pGpu, pKernelFsp);
        if (status != NV_OK)
        {
            if ((status == NV_ERR_TIMEOUT) &&
                !kfspIsMsgQueueEmpty(pGpu, pKernelFsp))
            {
                status = NV_OK;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "FSP command timed out\n");
            }
            break;
        }
    }

    return status;
}

/*!
 * @brief Set a timeout for response from FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 */
static NV_INLINE void
kfspSetResponseTimeout
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &(pKernelFsp->rpcTimeout),
                  GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
}

/*!
 * @brief Check for timeout for response from FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or NV_ERR_TIMEOUT
 */
static NV_INLINE NV_STATUS
kfspCheckResponseTimeout
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    return gpuCheckTimeout(pGpu, &(pKernelFsp->rpcTimeout));
}

/*!
 * @brief Send a MCTP message to FSP via EMEM
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelFsp         KernelFsp pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 *
 * @return NV_OK, or NV_ERR_*
 */
static NV_STATUS
kfspSendMessage
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPayload,
    NvU32      size,
    NvU32      nvdmType
)
{
    NvU32 dataSent, dataRemaining;
    NvU32 packetPayloadCapacity;
    NvU32 curPayloadSize;
    NvU32 headerSize;
    NvU32 fspEmemRmChannelSize;
    NvBool bSinglePacket;
    NV_STATUS status;
    NvU8 *pBuffer = NULL;
    NvU8  seq = 0;
    NvU8  seid = 0;

    // Allocate buffer of same size as channel
    fspEmemRmChannelSize = kfspGetRmChannelSize_HAL(pGpu, pKernelFsp);
    pBuffer = portMemAllocNonPaged(fspEmemRmChannelSize);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pBuffer != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pBuffer, 0, fspEmemRmChannelSize);

    //
    // Check if message will fit in single packet
    // We lose 2 DWORDS to MCTP and NVDM headers
    //
    headerSize = 2 * sizeof(NvU32);
    packetPayloadCapacity = fspEmemRmChannelSize - headerSize;
    bSinglePacket = (size <= packetPayloadCapacity);

    // First packet
    seid = kfspNvdmToSeid_HAL(pGpu, pKernelFsp, nvdmType);
    // SOM=1,EOM=?,SEID,SEQ=0
    ((NvU32 *)pBuffer)[0] = kfspCreateMctpHeader_HAL(pGpu, pKernelFsp, 1,
                                                     (NvU8)bSinglePacket, seid, seq);
    ((NvU32 *)pBuffer)[1] = kfspCreateNvdmHeader_HAL(pGpu, pKernelFsp, nvdmType);

    curPayloadSize = NV_MIN(size, packetPayloadCapacity);
    portMemCopy(pBuffer + headerSize, packetPayloadCapacity, pPayload, curPayloadSize);

    status = kfspSendPacket(pGpu, pKernelFsp, pBuffer, curPayloadSize + headerSize);
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
        packetPayloadCapacity = fspEmemRmChannelSize - headerSize;

        while (dataRemaining > 0)
        {
            NvBool bLastPacket = (dataRemaining <= packetPayloadCapacity);
            curPayloadSize = (bLastPacket) ? dataRemaining : packetPayloadCapacity;

            portMemSet(pBuffer, 0, fspEmemRmChannelSize);
            ((NvU32 *)pBuffer)[0] = kfspCreateMctpHeader_HAL(pGpu, pKernelFsp, 0,
                                                             (NvU8)bLastPacket, seid,
                                                             (++seq) % 4);

            portMemCopy(pBuffer + headerSize, packetPayloadCapacity,
                        pPayload + dataSent, curPayloadSize);

            status = kfspSendPacket(pGpu, pKernelFsp, pBuffer, curPayloadSize + headerSize);
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
 * @param[in]     pGpu              OBJGPU pointer
 * @param[in]     pKernelFsp        KernelFsp pointer
 * @param[in/out] pPayloadBuffer    Buffer in which to return message payload
 * @param[in]     payloadBufferSize Payload buffer size
 *
 * @return NV_OK, NV_ERR_INVALID_DATA, NV_ERR_INSUFFICIENT_RESOURCES, or errors
 *         from functions called within
 */
static NV_STATUS
kfspReadMessage
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPayloadBuffer,
    NvU32      payloadBufferSize
)
{
    NvU8             *pPacketBuffer;
    NV_STATUS         status;
    NvU32             totalPayloadSize = 0;
    MCTP_PACKET_STATE packetState = MCTP_PACKET_STATE_START;

    if (kfspIsMsgQueueEmpty(pGpu, pKernelFsp))
    {
        NV_PRINTF(LEVEL_WARNING, "Tried to read FSP response but MSG queue is empty\n");
        return NV_OK;
    }

    pPacketBuffer = portMemAllocNonPaged(kfspGetRmChannelSize_HAL(pGpu, pKernelFsp));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pPacketBuffer != NULL, NV_ERR_NO_MEMORY);

    while ((packetState != MCTP_PACKET_STATE_END) && (packetState != MCTP_PACKET_STATE_SINGLE_PACKET))
    {
        NvU32 msgqHead, msgqTail;
        NvU32 packetSize;
        NvU32 curPayloadSize;
        NvU8  curHeaderSize;
        NvU8  tag;

        // Wait for next packet
        status = kfspPollForResponse(pGpu, pKernelFsp);
        if (status != NV_OK)
        {
            goto done;
        }

        kfspGetMsgQueueHeadTail_HAL(pGpu, pKernelFsp, &msgqHead, &msgqTail);

        // Tail points to last DWORD in packet, not DWORD immediately following it
        packetSize = (msgqTail - msgqHead) + sizeof(NvU32);

        if ((packetSize < sizeof(NvU32)) ||
            (packetSize > kfspGetRmChannelSize_HAL(pGpu, pKernelFsp)))
        {
            NV_PRINTF(LEVEL_ERROR, "FSP response packet is invalid size: size=0x%x bytes\n", packetSize);
            status = NV_ERR_INVALID_DATA;
            goto done;
        }

        kfspReadFromEmem_HAL(pGpu, pKernelFsp, pPacketBuffer, packetSize);

        status = kfspGetPacketInfo_HAL(pGpu, pKernelFsp, pPacketBuffer, packetSize, &packetState, &tag);
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
            portMemCopy(pPayloadBuffer + totalPayloadSize, payloadBufferSize - totalPayloadSize ,
                        pPacketBuffer + curHeaderSize, curPayloadSize);
        }
        totalPayloadSize += curPayloadSize;

        // Set TAIL = HEAD to indicate CPU received message
        kfspUpdateMsgQueueHeadTail_HAL(pGpu, pKernelFsp, msgqHead, msgqHead);
    }

    NvU8 *pMessagePayload = (pPayloadBuffer == NULL) ? (pPacketBuffer + sizeof(MCTP_HEADER)) : pPayloadBuffer;

    status = kfspProcessNvdmMessage_HAL(pGpu, pKernelFsp, pMessagePayload, totalPayloadSize);

done:
    portMemFree(pPacketBuffer);
    return status;
}

/*!
 * @brief Send one MCTP packet to FSP via EMEM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelFsp    KernelFsp pointer
 * @param[in] pPacket       MCTP packet
 * @param[in] packetSize    MCTP packet size in bytes
 *
 * @return NV_OK, or NV_ERR_INSUFFICIENT_RESOURCES
 */
NV_STATUS
kfspSendPacket_IMPL
(
    OBJGPU      *pGpu,
    KernelFsp   *pKernelFsp,
    NvU8        *pPacket,
    NvU32        packetSize
)
{
    NvU32 paddedSize;
    NvU8 *pBuffer = NULL;
    NV_STATUS status = NV_OK;

    // Check that queue is ready to receive data
    status = kfspPollForQueueEmpty(pGpu, pKernelFsp);
    if (status != NV_OK)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Pad to align size to 4-bytes boundary since EMEMC increments by DWORDS
    paddedSize = NV_ALIGN_UP(packetSize, sizeof(NvU32));
    pBuffer = portMemAllocNonPaged(paddedSize);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pBuffer != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pBuffer, 0, paddedSize);
    portMemCopy(pBuffer, paddedSize, pPacket, paddedSize);

    kfspWriteToEmem_HAL(pGpu, pKernelFsp, pBuffer, paddedSize);

    // Update HEAD and TAIL with new EMEM offset; RM always starts at offset 0.
    kfspUpdateQueueHeadTail_HAL(pGpu, pKernelFsp, 0, paddedSize - sizeof(NvU32));

    portMemFree(pBuffer);
    return status;
}

/*!
 * @brief Send a MCTP message to FSP via EMEM, and read response
 *
 *
 * Response payload buffer is optional if response fits in a single packet.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelFsp         KernelFsp pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 * @param[in] pResponsePayload   Buffer in which to return response payload
 * @param[in] responseBufferSize Response payload buffer size
 *
 * @return NV_OK, or NV_ERR_*
 */
NV_STATUS
kfspSendAndReadMessage_IMPL
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPayload,
    NvU32      size,
    NvU32      nvdmType,
    NvU8      *pResponsePayload,
    NvU32      responseBufferSize
)
{
    NV_STATUS status;

    // If we are waiting for an async response, handle that first.
    if (pKernelFsp->bBusy)
    {
        status = kfspWaitForResponse(pGpu, pKernelFsp);
        if (status != NV_OK)
        {
            kfspExecuteAsyncRpcCallback(pKernelFsp, status);
        }
        else
        {
            kfspProcessAsyncResponse(pGpu, pKernelFsp);
        }
    }

    status = kfspSendMessage(pGpu, pKernelFsp, pPayload, size, nvdmType);
    if (status != NV_OK)
    {
        return status;
    }

    status = kfspPollForResponse(pGpu, pKernelFsp);
    if (status != NV_OK)
    {
        return status;
    }

    return kfspReadMessage(pGpu, pKernelFsp, pResponsePayload, responseBufferSize);;
}

/*!
 * @brief Send a MCTP message to FSP via EMEM, and read response asynchronously
 *        afterwards
 *
 *
 * Response payload buffer is optional if response fits in a single packet.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pKernelFsp         KernelFsp pointer
 * @param[in] pPayload           Pointer to message payload
 * @param[in] size               Message payload size
 * @param[in] nvdmType           NVDM type of message being sent
 * @param[in] pResponsePayload   Buffer in which to return response payload
 * @param[in] responseBufferSize Response payload buffer size
 * @param[in] callback           Callback to call for this response
 * @param[in] pCallbackArgs      Args to be passed to the callback
 *
 * @return NV_OK, or NV_ERR_*
 */
NV_STATUS
kfspSendAndReadMessageAsync_IMPL
(
    OBJGPU           *pGpu,
    KernelFsp        *pKernelFsp,
    NvU8             *pPayload,
    NvU32             size,
    NvU32             nvdmType,
    NvU8             *pResponsePayload,
    NvU32             responseBufferSize,
    AsyncRpcCallback  callback,
    void             *pCallbackArgs
)
{
    NV_STATUS status;

    if (pKernelFsp->bBusy)
    {
        return NV_ERR_BUSY_RETRY;
    }

    status = kfspSendMessage(pGpu, pKernelFsp, pPayload, size, nvdmType);
    if (status != NV_OK)
    {
        return status;
    }

    status = kfspScheduleAsyncResponseCheck(pGpu, pKernelFsp, callback, pCallbackArgs,
                                            pResponsePayload, responseBufferSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP queuing failed, status=%x\n", status);
    }

    return status;
}

/*!
 * @brief Polls for a response from the FSP and takes the appropriate
 *        action. This is called repeatedly by a timer when we are
 *        expecting an async response from the FSP.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pTmr               Timer object pointer
 * @param[in] pEvent             Timer event pointer
 *
 * @return NV_OK, or NV_ERR_*
 */
static NV_STATUS
kfspPollForAsyncResponse
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    NV_STATUS status = NV_OK;
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    if (!pKernelFsp->bBusy)
    {
        return status;
    }

    status = kfspCheckResponseTimeout(pGpu, pKernelFsp);

    if (!kfspIsMsgQueueEmpty(pGpu, pKernelFsp))
    {
        status = osQueueWorkItemWithFlags(pGpu, kfspProcessAsyncResponseCallback, NULL,
                                          OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA |
                                          OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to schedule work item, status=%x\n", status);
            kfspExecuteAsyncRpcCallback(pKernelFsp, status);
            return status;
        }

        return NV_OK;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP async command timed out\n");
        kfspExecuteAsyncRpcCallback(pKernelFsp, status);
        return status;
    }

    status = tmrEventScheduleRel(pTmr, pEvent, ASYNC_FSP_POLL_PERIOD_MS * 1000 * 1000);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to reschedule callback, status=%x\n", status);
        kfspExecuteAsyncRpcCallback(pKernelFsp, status);
    }

    return status;
}

/*!
 * @brief Deferred work item callback that processes an asyc
 *        response from the FSP
 *
 * @param[in] gpuInstance   GPU instance number
 * @param[in] pCallbackArgs Unused
 */
static void
kfspProcessAsyncResponseCallback
(
    NvU32 gpuInstance,
    void *pCallbackArgs
)
{
    OBJGPU    *pGpu = gpumgrGetGpu(gpuInstance);
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    kfspProcessAsyncResponse(pGpu, pKernelFsp);
}

/*!
 * @brief Handle the async response from the FSP
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelFsp    KernelFsp pointer
 */
static void
kfspProcessAsyncResponse
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = kfspReadMessage(pGpu, pKernelFsp, pKernelFsp->rpcState.pResponseBuffer,
                                       pKernelFsp->rpcState.responseBufferSize);
    kfspExecuteAsyncRpcCallback(pKernelFsp, status);
}

/*!
 * @brief Execute the callback for the async FSP response.
 *
 * @param[in] pKernelFsp    KernelFsp pointer
 * @param[in] status        Status of the FSP RPC
 */
static void
kfspExecuteAsyncRpcCallback
(
    KernelFsp *pKernelFsp,
    NV_STATUS status
)
{
    if (!pKernelFsp->bBusy)
    {
        return;
    }

    if (pKernelFsp->rpcState.callback)
    {
        pKernelFsp->rpcState.callback(status, pKernelFsp->rpcState.pCallbackArgs);
    }

    kfspClearAsyncResponseState(pKernelFsp);
}

/*!
 * @brief Set the state to service an FSP response and schedule
 *        a timer event to periodically poll the FSP.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelFsp    KernelFsp pointer
 * @param[in] callback      Callback to call for this response
 * @param[in] pCallbackArgs Args to be passed to the callback
 * @param[in] pBuffer       Buffer in which to return response payload
 * @param[in] bufferSize    Response payload buffer size
 *
 * @return NV_OK, or NV_ERR_*
 */
static NV_STATUS
kfspScheduleAsyncResponseCheck
(
    OBJGPU           *pGpu,
    KernelFsp        *pKernelFsp,
    AsyncRpcCallback  callback,
    void             *pCallbackArgs,
    NvU8             *pBuffer,
    NvU32             bufferSize
)
{
    if (pKernelFsp->bBusy)
    {
        return NV_ERR_IN_USE;
    }

    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS status = tmrEventScheduleRel(pTmr, pKernelFsp->pPollEvent,
                                           ASYNC_FSP_POLL_PERIOD_MS * 1000 * 1000);
    if (status != NV_OK)
    {
        return status;
    }

    pKernelFsp->rpcState.callback = callback;
    pKernelFsp->rpcState.pCallbackArgs = pCallbackArgs;
    pKernelFsp->rpcState.pResponseBuffer = pBuffer;
    pKernelFsp->rpcState.responseBufferSize = bufferSize;

    kfspSetResponseTimeout(pGpu, pKernelFsp);
    pKernelFsp->bBusy = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Clear the state associated with async FSP response
 *
 * @param[in] pKernelFsp    KernelFsp pointer
 */
static void
kfspClearAsyncResponseState
(
    KernelFsp *pKernelFsp
)
{
    pKernelFsp->bBusy = NV_FALSE;

    pKernelFsp->rpcState.callback = NULL;
    pKernelFsp->rpcState.pCallbackArgs = NULL;
    pKernelFsp->rpcState.pResponseBuffer = NULL;
    pKernelFsp->rpcState.responseBufferSize = 0;
}
