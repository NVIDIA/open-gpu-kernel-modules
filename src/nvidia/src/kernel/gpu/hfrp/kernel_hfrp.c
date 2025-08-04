/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvrm_registry.h"

#include "gpu/hfrp/kernel_hfrp.h"


static NvU8
_hfrpReadByte
(
    KernelHFRP *pKernelHfrp,
    NvU32       virtualAddr
)
{
    NvU32 data = HFRP_REG_RD32(pKernelHfrp, ((virtualAddr >> 2U) << 2U));
    return DRF_IDX_VAL(_HFRP, _BYTE, _FIELD, (virtualAddr % 4U), data);
}

static void
_hfrpWriteByte
(
    KernelHFRP *pKernelHfrp,
    NvU32       virtualAddr,
    NvU8        data
)
{
    NvU32 tempData = HFRP_REG_RD32(pKernelHfrp, ((virtualAddr >> 2U) << 2U));
    tempData = FLD_IDX_SET_DRF_NUM(_HFRP, _BYTE, _FIELD, (virtualAddr % 4U), data, tempData);
    HFRP_REG_WR32(pKernelHfrp, ((virtualAddr >> 2U) << 2U), tempData);
}

static NvU32
_hfrpWriteMailboxData
(
    KernelHFRP *pKernelHfrp,
    NvU8       *pData,
    NvU8        size,
    NvU32       virtualAddr
)
{
    NvU32 hfrpBufferStartAddr;
    NvU32 hfrpBufferEndAddr;
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    if (virtualAddr >= pMailboxIoInfo->hfrpCommandBufferStartAddr && virtualAddr <= pMailboxIoInfo->hfrpCommandBufferEndAddr)
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpCommandBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpCommandBufferEndAddr;
    }
    else
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpResponseBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpResponseBufferEndAddr;
    }
    for (NvU32 i = 0U; i < size; )
    {
        NvU32 writeAddr = (virtualAddr >> 2U) << 2U;
        NvU32 dword = 0U;
        if ((virtualAddr % 4U != 0U) || (i + 4U > size) || (virtualAddr + 4U > hfrpBufferEndAddr))
        {
            dword = HFRP_REG_RD32(pKernelHfrp, writeAddr);
        }
        for (NvU32 j = virtualAddr - writeAddr; j < 4U; j++)
        {
            dword = FLD_IDX_SET_DRF_NUM(_HFRP, _BYTE, _FIELD, j, pData[i], dword);
            i++;
            virtualAddr++;
            if (virtualAddr > hfrpBufferEndAddr)
            {
                virtualAddr += hfrpBufferStartAddr - hfrpBufferEndAddr - 1U;
                break;
            }
            if (i >= size)
            {
                break;
            }
        }
        HFRP_REG_WR32(pKernelHfrp, writeAddr, dword);
    }
    return virtualAddr;
}

static NvU32
_hfrpReadMailboxData
(
    KernelHFRP *pKernelHfrp,
    NvU8       *pData,
    NvU8        size,
    NvU32       virtualAddr
)
{
    NvU32 hfrpBufferStartAddr;
    NvU32 hfrpBufferEndAddr;
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    if (virtualAddr >= pMailboxIoInfo->hfrpCommandBufferStartAddr && virtualAddr <= pMailboxIoInfo->hfrpCommandBufferEndAddr)
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpCommandBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpCommandBufferEndAddr;
    }
    else
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpResponseBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpResponseBufferEndAddr;
    }
    for (NvU32 i = 0U; i < size; )
    {
        NvU32 readAddr = (virtualAddr >> 2U) << 2U;
        NvU32 dword = HFRP_REG_RD32(pKernelHfrp, readAddr);
        for (NvU32 j = virtualAddr - readAddr; j < 4U; j++)
        {
            pData[i] = DRF_IDX_VAL(_HFRP, _BYTE, _FIELD, j, dword);
            i++;
            virtualAddr++;
            if (virtualAddr > hfrpBufferEndAddr)
            {
                virtualAddr += hfrpBufferStartAddr - hfrpBufferEndAddr - 1U;
                break;
            }
            if (i >= size)
            {
                break;
            }
        }
    }
    return virtualAddr;
}

NV_STATUS
khfrpMailboxDequeueMessage_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32      *pMessageHeader,
    NvU8       *pPayloadArray,
    NvU32      *pPayloadSize,
    NvU32       mailboxFlag
)
{
    NvU32 tailAddr;
    NvU32 hfrpBufferStartAddr;
    NvU32 hfrpBufferEndAddr;
    NvU32 hfrpHeadPtrAddr;
    NvU32 hfrpTailPtrAddr;
    NvU32 clientPayloadSize;
    NvU32 sequenceId;
    NvU8  messageSize;
    NvU8  payloadSize;
    NvU8  head;
    NvU8  tail;
    NV_STATUS status = NV_OK;

    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);

    if (mailboxFlag == HFRP_COMMAND_MAILBOX_FLAG)
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpCommandBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpCommandBufferEndAddr;
        hfrpHeadPtrAddr = pMailboxIoInfo->hfrpCommandBufferHeadPtrAddr;
        hfrpTailPtrAddr = pMailboxIoInfo->hfrpCommandBufferTailPtrAddr;
    }
    else
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpResponseBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpResponseBufferEndAddr;
        hfrpHeadPtrAddr = pMailboxIoInfo->hfrpResponseBufferHeadPtrAddr;
        hfrpTailPtrAddr = pMailboxIoInfo->hfrpResponseBufferTailPtrAddr;
    }
    head = _hfrpReadByte(pKernelHfrp, hfrpHeadPtrAddr);
    tail = _hfrpReadByte(pKernelHfrp, hfrpTailPtrAddr);
    if (head == tail)
    {
        return NV_ERR_GENERIC;
    }

    tailAddr = tail + hfrpBufferStartAddr;
    tailAddr = _hfrpReadMailboxData(pKernelHfrp, (NvU8 *)pMessageHeader, HFRP_MESSAGE_HEADER_BYTE_SIZE, tailAddr);
    messageSize = REF_VAL(HFRP_MESSAGE_FIELD_SIZE, *pMessageHeader);
    sequenceId = REF_VAL(HFRP_MESSAGE_FIELD_SEQUENCE_ID, *pMessageHeader);
    payloadSize = messageSize - HFRP_MESSAGE_HEADER_BYTE_SIZE;

    if (sequenceId == HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX)
    {
        clientPayloadSize = HFRP_MAX_PAYLOAD_SIZE;
    }
    else
    {
        // Update response payload size parameter
        NvU32 *pResponsePayloadSize = pSequenceIdInfo->pResponsePayloadSizeArray[sequenceId];
        clientPayloadSize = *pResponsePayloadSize;
        *pResponsePayloadSize = payloadSize;
    }
    if ((payloadSize > HFRP_MAX_PAYLOAD_SIZE) || (payloadSize > clientPayloadSize))
    {
        tailAddr += payloadSize;
        if (tailAddr > hfrpBufferEndAddr)
        {
            tailAddr -= (hfrpBufferEndAddr - hfrpBufferStartAddr + 1U);
        }
        if (payloadSize > HFRP_MAX_PAYLOAD_SIZE)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }
        else
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        tailAddr = _hfrpReadMailboxData(pKernelHfrp, pPayloadArray, payloadSize, tailAddr);
    }
    *pPayloadSize = payloadSize;
    tail = tailAddr - hfrpBufferStartAddr;
    _hfrpWriteByte(pKernelHfrp, hfrpTailPtrAddr, tail);
    return status;
}

void
khfrpProcessResponse_IMPL
(
    KernelHFRP *pKernelHfrp
)
{
    while (NV_TRUE)
    {
        NV_STATUS status;
        NvU32 responseHeader;
        NvU32 responseSize;
        NvU32 responseSequenceId;
        NvU16 responseStatus;
        NvU8 response[HFRP_MAX_PAYLOAD_SIZE];

        status = khfrpMailboxDequeueMessage_IMPL(pKernelHfrp, &responseHeader, response,
            &responseSize, HFRP_RESPONSE_MAILBOX_FLAG);
        if (status == NV_ERR_GENERIC)
        {
            return;
        }

        if (status == NV_ERR_BUFFER_EMPTY)
        {
            return;
        }

        responseSequenceId = REF_VAL(HFRP_MESSAGE_FIELD_SEQUENCE_ID, responseHeader);
        responseStatus = REF_VAL(HFRP_MESSAGE_FIELD_INDEX_OR_STATUS, responseHeader);
        if (responseSequenceId == HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX)
        {
            // pKernelHfrp->asyncRegisteredFunction(&response, responseStatus);
        }
        else
        {
            // The sequence id of the response received invalid (not in accepted range)
            if (responseSequenceId >= HFRP_NUMBER_OF_SEQUENCEID_INDEX)
            {
                continue;
            }
            // The sequence id of the response is not allocated for any command
            if (khfrpIsSequenceIdFree_IMPL(pKernelHfrp, responseSequenceId))
            {
                continue;
            }
            HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);
            NvU16 *pResponseStatus = pSequenceIdInfo->pResponseStatusArray[responseSequenceId];
            NV_STATUS *pStatus = pSequenceIdInfo->pStatusArray[responseSequenceId];
           *pStatus = status;
            NvU8 *pResponse;
            *pResponseStatus = responseStatus;
            pResponse = pSequenceIdInfo->pResponsePayloadArray[responseSequenceId];

            if (status == NV_OK)
            {
                for (NvU32 i = 0U; i < responseSize; i++)
                {
                    pResponse[i] = response[i];
                }
            }
            khfrpFreeSequenceId_IMPL(pKernelHfrp, responseSequenceId);
        }
    }
}

void
khfrpServiceEvent_IMPL
(
    KernelHFRP *pKernelHfrp
)
{
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    khfrpProcessResponse_IMPL(pKernelHfrp);
    khfrpWriteBit_IMPL(pKernelHfrp, pMailboxIoInfo->hfrpIrqOutClrAddr, HFRP_IRQ_DOORBELL_BIT_INDEX, 1U);
    // processing the response again since the response maybe queued after writing IRQ_OUT_CLR
    khfrpProcessResponse_IMPL(pKernelHfrp);
}

NV_STATUS
khfrpConstructEngine_IMPL
(
    OBJGPU       *pGpu,
    KernelHFRP   *pKernelHfrp,
    ENGDESCRIPTOR engDesc
)
{
    NvU32 data32;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_DSTATE_HFRP, &data32) == NV_OK &&
        data32 == NV_REG_STR_RM_ENABLE_DSTATE_HFRP_TRUE)
    {
        pKernelHfrp->setProperty(pKernelHfrp, PDB_PROP_KHFRP_IS_ENABLED, NV_TRUE);
    }
    else
    {
        pKernelHfrp->setProperty(pKernelHfrp, PDB_PROP_KHFRP_IS_ENABLED, NV_FALSE);
        return NV_OK;
    }

    khfrpCommonConstruct(pKernelHfrp);

//    khfrpIoApertureConstruct(pGpu, pKernelHfrp);
    return NV_OK;
}

NV_STATUS
khfrpStatePreInitLocked_IMPL
(
    OBJGPU       *pGpu,
    KernelHFRP   *pKernelHfrp
)
{
    if (!pKernelHfrp->getProperty(pKernelHfrp, PDB_PROP_KHFRP_IS_ENABLED))
    {
        return NV_OK;
    }

    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_GPU_GET_HFRP_INFO_PARAMS hfrpParams = {0};

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_GPU_GET_HFRP_INFO,
                        &hfrpParams, sizeof(hfrpParams));

    if (status != NV_OK)
    {
        // Keep processing so RM can keep loading instead of failing in early stage
        NV_PRINTF(LEVEL_ERROR, "Failed to get HFRP info, bail out\n");
    }

    pKernelHfrp->khfrpPrivBase[0] = hfrpParams.hfrpPrivBase[0];
    pKernelHfrp->khfrpPrivBase[1] = hfrpParams.hfrpPrivBase[1];
    pKernelHfrp->khfrpPrivBase[2] = hfrpParams.hfrpPrivBase[2];
    pKernelHfrp->khfrpPrivBase[3] = hfrpParams.hfrpPrivBase[3];
    pKernelHfrp->khfrpPrivBase[4] = hfrpParams.hfrpPrivBase[4];

    pKernelHfrp->khfrpIntrCtrlReg[0] = hfrpParams.hfrpIntrCtrlReg[0];
    pKernelHfrp->khfrpIntrCtrlReg[1] = hfrpParams.hfrpIntrCtrlReg[1];
    pKernelHfrp->khfrpIntrCtrlReg[2] = hfrpParams.hfrpIntrCtrlReg[2];
    pKernelHfrp->khfrpIntrCtrlReg[3] = hfrpParams.hfrpIntrCtrlReg[3];
    pKernelHfrp->khfrpIntrCtrlReg[4] = hfrpParams.hfrpIntrCtrlReg[4];

    khfrpIoApertureConstruct(pGpu, pKernelHfrp);
    return NV_OK;
}

void
khfrpDestruct_IMPL
(
    KernelHFRP *pKernelHfrp
)
{
    for (NvU32 i = 0U; i < HFRP_NUMBER_OF_MAILBOXES; i++)
    {
        khfrpIoApertureDestruct(pKernelHfrp, i);
    }
}

void
khfrpCommonConstruct_IMPL
(
    KernelHFRP *pKernelHfrp
)
{
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);

    if (HFRP_COMMAND_MAILBOX_INDEX == HFRP_RESPONSE_MAILBOX_INDEX)
    {
        pMailboxIoInfo->hfrpCommandBufferHeadPtrAddr = HFRP_COMMAND_BUFFER_HEAD_PTR_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferTailPtrAddr = HFRP_COMMAND_BUFFER_TAIL_PTR_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferStartAddr = HFRP_COMMAND_BUFFER_START_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferEndAddr = HFRP_COMMAND_BUFFER_END_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferHeadPtrAddr = HFRP_RESPONSE_BUFFER_HEAD_PTR_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferTailPtrAddr = HFRP_RESPONSE_BUFFER_TAIL_PTR_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferStartAddr = HFRP_RESPONSE_BUFFER_START_ADDR_ONE_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferEndAddr = HFRP_RESPONSE_BUFFER_END_ADDR_ONE_MAILBOX_INTERFACE;
    }
    else
    {
        pMailboxIoInfo->hfrpCommandBufferHeadPtrAddr = HFRP_COMMAND_BUFFER_HEAD_PTR_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferTailPtrAddr = HFRP_COMMAND_BUFFER_TAIL_PTR_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferStartAddr = HFRP_COMMAND_BUFFER_START_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpCommandBufferEndAddr = HFRP_COMMAND_BUFFER_END_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferHeadPtrAddr = HFRP_RESPONSE_BUFFER_HEAD_PTR_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferTailPtrAddr = HFRP_RESPONSE_BUFFER_TAIL_PTR_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferStartAddr = HFRP_RESPONSE_BUFFER_START_ADDR_TWO_MAILBOX_INTERFACE;
        pMailboxIoInfo->hfrpResponseBufferEndAddr = HFRP_RESPONSE_BUFFER_END_ADDR_TWO_MAILBOX_INTERFACE;
    }
    pMailboxIoInfo->hfrpIrqInSetAddr = HFRP_IRQ_IN_SET_ADDR;
    pMailboxIoInfo->hfrpIrqOutSetAddr = HFRP_IRQ_OUT_SET_ADDR;
    pMailboxIoInfo->hfrpIrqInClrAddr = HFRP_IRQ_IN_CLR_ADDR;
    pMailboxIoInfo->hfrpIrqOutClrAddr = HFRP_IRQ_OUT_CLR_ADDR;

    for (NvU32 i = 0U; i < HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX; i++)
    {
        pSequenceIdInfo->sequenceIdState[i] = 0U;
    }
    pSequenceIdInfo->sequenceIdArrayIndex = 0U;
}

NV_STATUS
khfrpIoApertureConstruct_IMPL
(
    OBJGPU     *pGpu,
    KernelHFRP *pKernelHfrp
)
{
    NV_STATUS status;
    status = objCreate(&pKernelHfrp->pAperture[0U],
                       pKernelHfrp,
                       IoAperture,
                       pGpu->pIOApertures[DEVICE_INDEX_GPU],
                       pGpu,
                       DEVICE_INDEX_GPU,
                       0,
                       NULL,
                       0,
                       pKernelHfrp->khfrpPrivBase[HFRP_COMMAND_MAILBOX_INDEX],
                       HFRP_MAILBOX_ACCESS_RANGE);
    if (status != NV_OK)
    {
        return status;
    }
    if (HFRP_COMMAND_MAILBOX_INDEX != HFRP_RESPONSE_MAILBOX_INDEX)
    {
        status = objCreate(&pKernelHfrp->pAperture[1U],
                        pKernelHfrp,
                        IoAperture,
                        pGpu->pIOApertures[DEVICE_INDEX_GPU],
                        pGpu,
                        DEVICE_INDEX_GPU,
                        0,
                        NULL,
                        0,
                        pKernelHfrp->khfrpPrivBase[HFRP_RESPONSE_MAILBOX_INDEX],
                        HFRP_MAILBOX_ACCESS_RANGE);
    }
    return status;
}

void
khfrpIoApertureDestruct_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       index
)
{
    if (pKernelHfrp->pAperture[index] != NULL)
    {
        objDelete(pKernelHfrp->pAperture[index]);
        pKernelHfrp->pAperture[index] = NULL;
    }
}

NvU32
khfrpReadBit_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32 virtualAddr,
    NvU32 bitIndex
)
{
    NvU32 dword = HFRP_REG_RD32(pKernelHfrp, virtualAddr);
    return (dword >> bitIndex) & 1U;
}

void
khfrpWriteBit_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       virtualAddr,
    NvU32       bitIndex,
    NvU32       data
)
{
    NvU32 dword = HFRP_REG_RD32(pKernelHfrp, virtualAddr);
    NvU32 mask = ~(1U << bitIndex);
    dword &= mask;
    dword |= ((data ? 1U : 0U) << bitIndex);
    HFRP_REG_WR32(pKernelHfrp, virtualAddr, dword);
}

NV_STATUS
khfrpMailboxQueueMessage_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       messageHeader,
    NvU8       *pPayloadArray,
    NvU32       payloadSize,
    NvU32       mailboxFlag
)
{
    NvU32 headAddr;
    NvU32 hfrpBufferStartAddr = 0;
    NvU32 hfrpBufferEndAddr = 0;
    NvU32 hfrpHeadPtrAddr = 0 ;
    NvU32 hfrpTailPtrAddr = 0;
    NvU32 mailboxBufferSize = 0;
    NvU8 head;
    NvU8 tail;
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);

    if (mailboxFlag == HFRP_COMMAND_MAILBOX_FLAG)
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpCommandBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpCommandBufferEndAddr;
        hfrpHeadPtrAddr = pMailboxIoInfo->hfrpCommandBufferHeadPtrAddr;
        hfrpTailPtrAddr = pMailboxIoInfo->hfrpCommandBufferTailPtrAddr;
    }
    else
    {
        hfrpBufferStartAddr = pMailboxIoInfo->hfrpResponseBufferStartAddr;
        hfrpBufferEndAddr = pMailboxIoInfo->hfrpResponseBufferEndAddr;
        hfrpHeadPtrAddr = pMailboxIoInfo->hfrpResponseBufferHeadPtrAddr;
        hfrpTailPtrAddr = pMailboxIoInfo->hfrpResponseBufferTailPtrAddr;
    }

    mailboxBufferSize = hfrpBufferEndAddr - hfrpBufferStartAddr + 1U;
    head = _hfrpReadByte(pKernelHfrp, hfrpHeadPtrAddr);
    tail = _hfrpReadByte(pKernelHfrp, hfrpTailPtrAddr);
    NvU32 bufferSizeUsed = (mailboxBufferSize + head - tail) % mailboxBufferSize;
    if ((bufferSizeUsed + payloadSize + HFRP_MESSAGE_HEADER_BYTE_SIZE) >= mailboxBufferSize)
    {
        return NV_ERR_GENERIC;
    }

    headAddr = head + hfrpBufferStartAddr;
    headAddr = _hfrpWriteMailboxData(pKernelHfrp, (NvU8 *)&messageHeader, HFRP_MESSAGE_HEADER_BYTE_SIZE, headAddr);
    headAddr = _hfrpWriteMailboxData(pKernelHfrp, pPayloadArray, payloadSize, headAddr);
    head = headAddr - hfrpBufferStartAddr;
    _hfrpWriteByte(pKernelHfrp, hfrpHeadPtrAddr, head);
    return NV_OK;
}

NV_STATUS
khfrpAllocateSequenceId_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU16      *pResponseStatus,
    void       *pResponsePayload,
    NvU32      *pResponsePayloadSize,
    NV_STATUS  *pStatus,
    NvU32      *pSequenceIdIndex
)
{
    HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);
    NvU32 arrayIndex = pSequenceIdInfo->sequenceIdArrayIndex;
    NvU32 sequenceIdIndex = HFRP_NUMBER_OF_SEQUENCEID_INDEX;
    for (NvU32 j = 0U; j < HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX; j++)
    {
        NvU32 state = pSequenceIdInfo->sequenceIdState[arrayIndex];
        NvU32 indexValue;
        NvU32 index;
        if (state == NV_U32_MAX)
        {
            arrayIndex = (arrayIndex + 1) % HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX;
            continue;
        }
        indexValue = LOWESTBIT(~state);
        index = BIT_IDX_32(indexValue);
        sequenceIdIndex = arrayIndex * 32U + index;
        if (sequenceIdIndex == HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX)
        {
            arrayIndex = (arrayIndex + 1) % HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX;
            continue;
        }
        state |= indexValue;
        pSequenceIdInfo->sequenceIdState[arrayIndex] = state;
        pSequenceIdInfo->sequenceIdArrayIndex = arrayIndex;
        pSequenceIdInfo->pResponsePayloadArray[sequenceIdIndex] = pResponsePayload;
        pSequenceIdInfo->pResponseStatusArray[sequenceIdIndex] = pResponseStatus;
        pSequenceIdInfo->pResponsePayloadSizeArray[sequenceIdIndex] = pResponsePayloadSize;
        pSequenceIdInfo->pStatusArray[sequenceIdIndex] = pStatus;
        break;
    }

    *pSequenceIdIndex = sequenceIdIndex;

    if (sequenceIdIndex == HFRP_NUMBER_OF_SEQUENCEID_INDEX)
    {
        return NV_ERR_IN_USE;
    }

    return NV_OK;
}

void
khfrpFreeSequenceId_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       index
)
{
    NvU32 arrayIndex = index / 32U;
    NvU32 bitIndex = index % 32U;
    HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);
    pSequenceIdInfo->pResponsePayloadArray[index] = NULL;
    pSequenceIdInfo->pResponseStatusArray[index] = NULL;
    pSequenceIdInfo->pResponsePayloadSizeArray[index] = NULL;
    pSequenceIdInfo->pStatusArray[index] = NULL;
    pSequenceIdInfo->sequenceIdState[arrayIndex] &= ~(1U << bitIndex);
}

NvBool
khfrpIsSequenceIdFree_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       index
)
{
    HFRP_SEQUENCEID_INFO *pSequenceIdInfo = &(pKernelHfrp->khfrpInfo.sequenceIdInfo);
    return pSequenceIdInfo->pResponseStatusArray[index] == NULL;
}

NV_STATUS
khfrpPollOnIrqWrapper_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       irqRegAddr,
    NvU32       bitIndex,
    NvBool      bData
)
{
    return HFRP_POLL_ON_IRQ(pKernelHfrp, irqRegAddr, bitIndex, bData);
}

NV_STATUS
khfrpPollOnIrqRm_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU32       irqRegAddr,
    NvU32       bitIndex,
    NvBool      bData
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelHfrp);
    RMTIMEOUT timeout;
    NvU32 data = bData ? 1U : 0U;
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    while (khfrpReadBit(pKernelHfrp, irqRegAddr, bitIndex) != data)
    {
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            return NV_ERR_TIMEOUT;
        }
        osSpinLoop();
    }
    return NV_OK;
}

NV_STATUS
khfrpPostCommandBlocking_IMPL
(
    KernelHFRP *pKernelHfrp,
    NvU16       commandIndex,
    void       *pCommandPayload,
    NvU32       commandPayloadSize,
    NvU16      *pResponseStatus,
    void       *pResponsePayload,
    NvU32      *pResponsePayloadSize,
    NV_STATUS  *pStatus
)
{
    NvU32 sequenceId;
    NvU32 commandHeader;
    HFRP_MAILBOX_IO_INFO *pMailboxIoInfo;
    NV_STATUS status;

    if (pKernelHfrp->getProperty(pKernelHfrp, PDB_PROP_KHFRP_IS_ENABLED) == NV_FALSE)
    {
        NV_PRINTF(LEVEL_ERROR, "hfrp is not enabled \n");
        return NV_ERR_FEATURE_NOT_ENABLED;
    }

    commandHeader = FLD_SET_REF_NUM(HFRP_MESSAGE_FIELD_SIZE, commandPayloadSize + HFRP_MESSAGE_HEADER_BYTE_SIZE, 0U);
    pMailboxIoInfo = &(pKernelHfrp->khfrpInfo.mailboxIoInfo);
    status = khfrpAllocateSequenceId_IMPL(pKernelHfrp, pResponseStatus, pResponsePayload,pResponsePayloadSize, pStatus, &sequenceId);
    if (status == NV_ERR_NO_MEMORY)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot allocate hfrp sequence ID due to out of memory \n");
        *pStatus = status;
        return status;
    }

    if (status == NV_ERR_IN_USE)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot allocate hfrp sequence ID due to out of sequence in use \n");
        *pStatus = status;
        return status;
    }
    commandHeader = FLD_SET_REF_NUM(HFRP_MESSAGE_FIELD_SEQUENCE_ID, sequenceId, commandHeader);
    commandHeader = FLD_SET_REF_NUM(HFRP_MESSAGE_FIELD_INDEX_OR_STATUS, (NvU32)commandIndex, commandHeader);

    status = khfrpMailboxQueueMessage_IMPL(pKernelHfrp, commandHeader, pCommandPayload,
                                     commandPayloadSize, HFRP_COMMAND_MAILBOX_FLAG);
    if (status != NV_OK)
    {
        khfrpFreeSequenceId_IMPL(pKernelHfrp, sequenceId);
        NV_PRINTF(LEVEL_ERROR, "Failed to queue HFRP command\n");
                *pStatus = status;
        return NV_ERR_GENERIC;
    }

    khfrpWriteBit_IMPL(pKernelHfrp, pMailboxIoInfo->hfrpIrqInSetAddr, HFRP_IRQ_DOORBELL_BIT_INDEX, 1U);

    while (NV_TRUE)
    {
        status = khfrpPollOnIrqWrapper_IMPL(pKernelHfrp, pMailboxIoInfo->hfrpIrqOutSetAddr, HFRP_IRQ_DOORBELL_BIT_INDEX, NV_TRUE);
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "timed out while waiting for IRQ_OUT_SET to receive notify reset server complete\n");
            *pStatus = status;
            return status;
        }

        khfrpServiceEvent_IMPL(pKernelHfrp);
        if (khfrpIsSequenceIdFree_IMPL(pKernelHfrp, sequenceId))
        {
            break;
        }
    }
    return NV_OK;
}

