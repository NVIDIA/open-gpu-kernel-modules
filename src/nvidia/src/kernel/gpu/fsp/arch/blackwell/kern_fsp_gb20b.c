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
 * @file    kern_fsp_gb20b.c
 * @brief   Provides the implementation for GB20B chip specific FSP HAL
 *          interfaces.
 */
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/fsp/kern_fsp_retval.h"
#include "published/blackwell/gb20b/dev_sec_pri.h"
#include "published/blackwell/gb20b/dev_boot.h"
#include "fsp/fsp_emem_channels.h"
#include "nverror.h"


// Blocks are 64 DWORDS
#define DWORDS_PER_EMEM_BLOCK 64U

static void _kfspUpdateQueueHeadTail_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 queueHead, NvU32 queueTail);

static void _kfspGetQueueHeadTail_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static void _kfspUpdateMsgQueueHeadTail_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 queueHead, NvU32 queueTail);

static void _kfspGetMsgQueueHeadTail_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static NV_STATUS _kfspConfigEmemc_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU32 offset, NvBool bAincw, NvBool bAincr);

static NV_STATUS _kfspWriteToEmem_GB20B(OBJGPU *pGpu, KernelFsp *pKernelFsp,
    NvU8 *pBuffer, NvU32 size);

static NvBool _kfspWaitBootCond_GB20B(OBJGPU *pGpu, void *pArg);

/*!
 * @brief Update command queue head and tail pointers
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 * @param[in] queueHead  Offset to write to command queue head
 * @param[in] queueTail  Offset to write to command queue tail
 */
static void
_kfspUpdateQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      queueHead,
    NvU32      queueTail
)
{
    // The write to HEAD needs to happen after TAIL because it will interrupt FSP
    GPU_REG_WR32(pGpu, NV_PSEC_QUEUE_TAIL(FSP_EMEM_CHANNEL_RM), queueTail);
    GPU_REG_WR32(pGpu, NV_PSEC_QUEUE_HEAD(FSP_EMEM_CHANNEL_RM), queueHead);
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
_kfspGetQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32     *pQueueHead,
    NvU32     *pQueueTail
)
{
    *pQueueHead = GPU_REG_RD32(pGpu, NV_PSEC_QUEUE_HEAD(FSP_EMEM_CHANNEL_RM));
    *pQueueTail = GPU_REG_RD32(pGpu, NV_PSEC_QUEUE_TAIL(FSP_EMEM_CHANNEL_RM));
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
_kfspUpdateMsgQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32      msgqHead,
    NvU32      msgqTail
)
{
    GPU_REG_WR32(pGpu, NV_PSEC_MSGQ_TAIL(FSP_EMEM_CHANNEL_RM), msgqTail);
    GPU_REG_WR32(pGpu, NV_PSEC_MSGQ_HEAD(FSP_EMEM_CHANNEL_RM), msgqHead);
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
_kfspGetMsgQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU32     *pMsgqHead,
    NvU32     *pMsgqTail
)
{
    *pMsgqHead = GPU_REG_RD32(pGpu, NV_PSEC_MSGQ_HEAD(FSP_EMEM_CHANNEL_RM));
    *pMsgqTail = GPU_REG_RD32(pGpu, NV_PSEC_MSGQ_TAIL(FSP_EMEM_CHANNEL_RM));
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
kfspIsResponseAvailable_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 msgqHead;
    NvU32 msgqTail;

    _kfspGetMsgQueueHeadTail_GB20B(pGpu, pKernelFsp, &msgqHead, &msgqTail);
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
kfspCanSendPacket_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NvU32 cmdqHead;
    NvU32 cmdqTail;

    _kfspGetQueueHeadTail_GB20B(pGpu, pKernelFsp, &cmdqHead, &cmdqTail);

    // FSP will set QUEUE_HEAD = TAIL after each packet is received
    return (cmdqHead == cmdqTail);
}

static NvBool
_kfspWaitBootCond_GB20B
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    //
    // 
    // FWSEC writes 0xFF value in NV_PMC_SCRATCH_RESET_PLUS_2 register after completion of boot
    //
    
    NvU32 reg  = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_PLUS_2);
    return reg == 0xFF;
}

NV_STATUS
kfspWaitForSecureBoot_GB20B
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

    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GB20B, NULL, &timeout);

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
_kfspConfigEmemc_GB20B
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

    reg32 = FLD_SET_DRF_NUM(_PSEC, _EMEMC, _OFFS, offsetDwords, reg32);
    reg32 = FLD_SET_DRF_NUM(_PSEC, _EMEMC, _BLK, offsetBlks, reg32);

    if (bAincw)
    {
        reg32 = FLD_SET_DRF(_PSEC, _EMEMC, _AINCW, _TRUE, reg32);
    }
    if (bAincr)
    {
        reg32 = FLD_SET_DRF(_PSEC, _EMEMC, _AINCR, _TRUE, reg32);
    }

    GPU_REG_WR32(pGpu, NV_PSEC_EMEMC(FSP_EMEM_CHANNEL_RM), reg32);
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
_kfspWriteToEmem_GB20B
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

    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetStart = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetStart += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "About to send data to FWSEC, ememcOff=0x%x, size=0x%x\n", ememOffsetStart, size);

    // Now write to EMEMD, we always have to write a DWORD at a time so write
    // all the full DWORDs in the buffer and then pad any leftover bytes with 0
    for (i = 0; i < (size / sizeof(NvU32)); i++)
    {
        GPU_REG_WR32(pGpu, NV_PSEC_EMEMD(FSP_EMEM_CHANNEL_RM), ((NvU32 *)(void *)pBuffer)[i]);
    }
    wordsWritten = size / sizeof(NvU32);

    leftoverBytes = size % sizeof(NvU32);
    if (leftoverBytes != 0)
    {
        reg32 = 0;
        portMemCopy(&reg32, sizeof(NvU32), &(pBuffer[size - leftoverBytes]), leftoverBytes);
        GPU_REG_WR32(pGpu, NV_PSEC_EMEMD(FSP_EMEM_CHANNEL_RM), reg32);
        wordsWritten++;
    }

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
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
kfspSendPacket_GB20B
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
    _kfspConfigEmemc_GB20B(pGpu, pKernelFsp, 0, NV_TRUE, NV_FALSE);
    _kfspWriteToEmem_GB20B(pGpu, pKernelFsp, pPacket, packetSize);

    paddedSize = NV_ALIGN_UP(packetSize, sizeof(NvU32));
    // Update HEAD and TAIL with new EMEM offset; RM always starts at offset 0.
    // TAIL points to the last DWORD written, so subtract 1 DWORD
    _kfspUpdateQueueHeadTail_GB20B(pGpu, pKernelFsp, 0, paddedSize - sizeof(NvU32));

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
kfspReadPacket_GB20B
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
    _kfspGetMsgQueueHeadTail_GB20B(pGpu, pKernelFsp, &msgqHead, &msgqTail);

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
    _kfspConfigEmemc_GB20B(pGpu, pKernelFsp, 0, NV_FALSE, NV_TRUE);

    NV_PRINTF(LEVEL_INFO, "About to read data from FWSEC, ememcOff=0, size=0x%x\n", packetSize);
    if (!NV_IS_ALIGNED(packetSize, sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_WARNING, "Size=0x%x is not DWORD-aligned, data will be truncated!\n", packetSize);
    }

    // Now read from EMEMD
    for (i = 0; i < (packetSize / sizeof(NvU32)); i++)
    {
        ((NvU32 *)(void *)pPacket)[i] = GPU_REG_RD32(pGpu, NV_PSEC_EMEMD(FSP_EMEM_CHANNEL_RM));
    }
    *bytesRead = packetSize;

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(FSP_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "After reading data, ememcOff = 0x%x\n", ememOffsetEnd);

    // Set TAIL = HEAD to indicate CPU received packet
    _kfspUpdateMsgQueueHeadTail_GB20B(pGpu, pKernelFsp, msgqHead, msgqHead);

    NV_ASSERT_OR_RETURN((ememOffsetEnd) == (packetSize / sizeof(NvU32)), NV_ERR_INVALID_STATE);
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
kfspDumpDebugState_GB20B
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
   
    NV_PRINTF(LEVEL_ERROR, "GPU %04x:%02x:%02x\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu));

}
