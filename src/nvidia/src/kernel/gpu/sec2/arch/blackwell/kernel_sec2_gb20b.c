/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file    kernel_sec2_gb20b.c
 * @brief   Provides the implementation for GB20B chip specific SEC2 HAL
 *          interfaces.
 */
#include "gpu/sec2/kernel_sec2.h"
#include "core/core.h"

#include "gpu/fsp/kern_fsp_retval.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/spdm/spdm.h"
#include "sec2/nvdm_payload_cmd_response.h"
#include "fsp/fsp_caps_query_rpc.h"

#include "published/blackwell/gb20b/dev_sec_pri.h"
#include "published/blackwell/gb20b/dev_boot.h"
#include "sec2/sec2_nvdm_format.h"
#include "published/blackwell/gb20b/dev_falcon_v4.h"
#include "os/os.h"
#include "nvRmReg.h"
#include "nverror.h"


#include "gpu/conf_compute/conf_compute.h"
#include "conf_compute/cc_keystore.h"


// Blocks are 64 DWORDS
#define DWORDS_PER_EMEM_BLOCK 64U

static void _ksec2UpdateMsgQueueHeadTail_GB20B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 queueHead, NvU32 queueTail);

static void _ksec2GetMsgQueueHeadTail_GB20B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static NV_STATUS _ksec2ConfigEmemc_GB20B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 offset, NvBool bAincw, NvBool bAincr);

static NvBool _ksec2WaitBootCond_GB20B(OBJGPU *pGpu, void *pArg);

static const BINDATA_ARCHIVE *_ksec2GetGspUcodeArchive(OBJGPU *pGpu, KernelSec2 *pKernelSec2);

/*!
 * @brief Update message queue head and tail pointers
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] msgqHead   Offset to write to message queue head
 * @param[in] msgqTail   Offset to write to message queue tail
 */
static void
_ksec2UpdateMsgQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU32      msgqHead,
    NvU32      msgqTail
)
{
    GPU_REG_WR32(pGpu, NV_PSEC_MSGQ_TAIL(SEC2_EMEM_CHANNEL_RM), msgqTail);
    GPU_REG_WR32(pGpu, NV_PSEC_MSGQ_HEAD(SEC2_EMEM_CHANNEL_RM), msgqHead);
}

/*!
 * @brief Read message queue head and tail pointers
 *
 * @param[in]  pGpu       OBJGPU pointer
 * @param[in]  pKernelSec2 KernelSec2 pointer
 * @param[out] pMsgqHead  Pointer where we write message queue head
 * @param[out] pMsgqTail  Pointer where we write message queue tail
 */
static void
_ksec2GetMsgQueueHeadTail_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU32     *pMsgqHead,
    NvU32     *pMsgqTail
)
{
    *pMsgqHead = GPU_REG_RD32(pGpu, NV_PSEC_MSGQ_HEAD(SEC2_EMEM_CHANNEL_RM));
    *pMsgqTail = GPU_REG_RD32(pGpu, NV_PSEC_MSGQ_TAIL(SEC2_EMEM_CHANNEL_RM));
}

/*!
 * @brief Get maximum size of a packet we can receive.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return Packet size in bytes
 */
NvU32
ksec2GetMaxRecvPacketSize_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    //
    // Channel size is hardcoded to 1K for now. Later we will use EMEMR to
    // properly fetch the lower and higher bounds of the EMEM channel
    //
    return SEC2_EMEM_CHANNEL_RM_SIZE;
}

/*!
 * @brief Check if RM has a response from SEC2 available
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_TRUE if a response is available, NV_FALSE otherwise
 */
NvBool
ksec2IsResponseAvailable_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NvU32 msgqHead;
    NvU32 msgqTail;

    _ksec2GetMsgQueueHeadTail_GB20B(pGpu, pKernelSec2, &msgqHead, &msgqTail);
    // SEC2 updates the head after writing data into the channel
    return (msgqHead != msgqTail);
}

/*!
 * @brief Retreive SEID based on NVDM type
 *
 * For now, SEIDs are only needed for use-cases that send multi-packet RM->SEC2
 * messages. The SEID is used in these cases to route packets to the correct
 * task as SEC2 receives them. Single-packet use-cases are given SEID 0.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] nvdmType   NVDM message type
 *
 * @return SEID corresponding to passed-in NVDM type
 */
NvU8
ksec2NvdmToSeid_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] som        Start of Message flag
 * @param[in] eom        End of Message flag
 * @param[in] tag        Message tag
 * @param[in] seq        Packet sequence number
 *
 * @return Constructed MCTP header
 */
NvU32
ksec2CreateMctpHeader_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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
 * @brief Retrieve and validate info in packet's MCTP headers
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelSec2    KernelSec2 pointer
 * @param[in]  pBuffer       Buffer containing packet
 * @param[in]  size          Size of buffer in bytes
 * @param[out] pPacketState  Pointer where we write packet state
 * @param[out] pTag          Pointer where we write packet's MCTP tag
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
ksec2GetPacketInfo_GB20B
(
    OBJGPU            *pGpu,
    KernelSec2         *pKernelSec2,
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
        status = ksec2ValidateMctpPayloadHeader_HAL(pGpu, pKernelSec2, pBuffer, size);
    }

    *pTag = REF_VAL(MCTP_HEADER_TAG, mctpHeader);

    return status;
}

/*!
 * @brief Validate packet's MCTP payload header
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] pBuffer    Buffer containing packet
 * @param[in] size       Size of buffer in bytes
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
ksec2ValidateMctpPayloadHeader_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NV_OK or NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
ksec2ProcessNvdmMessage_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pBuffer,
    NvU32      size
)
{
    NvU8 nvdmType;
    NV_STATUS status = NV_OK;

    nvdmType = pBuffer[0];

    switch (nvdmType)
    {
        case NVDM_TYPE_SEC2_RESPONSE:
        case NVDM_TYPE_SMBPBI:
            status = ksec2ProcessCommandResponse_HAL(pGpu, pKernelSec2, pBuffer, size);
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
 * @brief Process SEC2 command response
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] pBuffer    Buffer containing packet data
 * @param[in] Size       Buffer size
 *
 * @return NV_OK or NV_ERR_INVALID_DATA
 */
NV_STATUS
ksec2ProcessCommandResponse_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pBuffer,
    NvU32      size
)
{
    NVDM_PAYLOAD_COMMAND_RESPONSE *pCmdResponse;
    NvU32 headerSize = sizeof(NvU8); // NVDM type
    NV_STATUS status = NV_OK;

    if (size < (headerSize + sizeof(NVDM_PAYLOAD_COMMAND_RESPONSE)))
    {
        NV_PRINTF(LEVEL_ERROR, "Expected SEC2 command response, but packet is not big enough for payload. Size: 0x%0x\n", size);
        return NV_ERR_INVALID_DATA;
    }

    pCmdResponse = (NVDM_PAYLOAD_COMMAND_RESPONSE *)&(pBuffer[1]);
    NV_PRINTF(LEVEL_INFO, "Received SEC2 command response. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
              pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);

    status = ksec2ErrorCode2NvStatusMap_HAL(pGpu, pKernelSec2, pCmdResponse->errorCode);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Last command was processed by SEC2 successfully!\n");
    }
    else if (status != NV_ERR_OBJECT_NOT_FOUND)
    {
        NV_PRINTF(LEVEL_ERROR, "SEC2 response reported error. Task ID: 0x%0x Command type: 0x%0x Error code: 0x%0x\n",
                pCmdResponse->taskId, pCmdResponse->commandNvdmType, pCmdResponse->errorCode);
    }

    return status;
}

/*!
 * @brief Configure EMEMC for RM's queue in SEC2 EMEM
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] offset     Offset to write to EMEMC in DWORDS
 * @param[in] bAincw     Flag to set auto-increment on writes
 * @param[in] bAincr     Flag to set auto-increment on reads
 *
 * @return NV_OK
 */
static NV_STATUS
_ksec2ConfigEmemc_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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

    GPU_REG_WR32(pGpu, NV_PSEC_EMEMC(SEC2_EMEM_CHANNEL_RM), reg32);
    return NV_OK;
}

/*!
 * @brief Read data to buffer from RM channel in SEC2's EMEM
 *
 * @param[in]     pGpu          OBJGPU pointer
 * @param[in]     pKernelSec2    KernelSec2 pointer
 * @param[in/out] pPacket       Buffer where we copy data from EMEM
 * @param[in]     maxPacketSize Size in bytes of pPacket buffer
 * @param[out]    bytesRead     Size in bytes that was read into the packet buffer
 *
 * @return NV_OK
 */
NV_STATUS
ksec2ReadPacket_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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
    _ksec2GetMsgQueueHeadTail_GB20B(pGpu, pKernelSec2, &msgqHead, &msgqTail);

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
    _ksec2ConfigEmemc_GB20B(pGpu, pKernelSec2, 0, NV_FALSE, NV_TRUE);

    NV_PRINTF(LEVEL_INFO, "About to read data from SEC2, ememcOff=0, size=0x%x\n", packetSize);
    if (!NV_IS_ALIGNED(packetSize, sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_WARNING, "Size=0x%x is not DWORD-aligned, data will be truncated!\n", packetSize);
    }

    // Now read from EMEMD
    for (i = 0; i < (packetSize / sizeof(NvU32)); i++)
    {
        ((NvU32 *)(void *)pPacket)[i] = GPU_REG_RD32(pGpu, NV_PSEC_EMEMD(SEC2_EMEM_CHANNEL_RM));
    }
    *bytesRead = packetSize;

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(SEC2_EMEM_CHANNEL_RM));
    ememOffsetEnd = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "After reading data, ememcOff = 0x%x\n", ememOffsetEnd);

    // Set TAIL = HEAD to indicate CPU received packet
    _ksec2UpdateMsgQueueHeadTail_GB20B(pGpu, pKernelSec2, msgqHead, msgqHead);

    NV_ASSERT_OR_RETURN((ememOffsetEnd) == (packetSize / sizeof(NvU32)), NV_ERR_INVALID_STATE);
    return NV_OK;
}

static NvBool
_ksec2WaitBootCond_GB20B
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    // FWSEC writes 0xFF value in NV_PMC_SCRATCH_RESET_PLUS_2 register after completion of boot
    NvU32 reg  = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_PLUS_2);
    return reg == 0xFF;
}

NV_STATUS
ksec2WaitForSecureBoot_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;

    //
    // Polling for SEC2 boot complete
    // FBFalcon training during devinit alone takes 2 seconds, up to 3 on HBM3,
    // but the default threadstate timeout on windows is 1800 ms. Increase to 4 seconds
    // for this wait to match MODS GetGFWBootTimeoutMs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    //
    gpuSetTimeout(pGpu, NV_MAX(gpuScaleTimeout(pGpu, 4000000), pGpu->timeoutData.defaultus),
                  &timeout, GPU_TIMEOUT_FLAGS_OSTIMER);

    status = gpuTimeoutCondWait(pGpu, _ksec2WaitBootCond_GB20B, NULL, &timeout);

    return status;
}

static const BINDATA_ARCHIVE *
_ksec2GetGspUcodeArchive
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    KernelGsp *pKernelGsp                 = GPU_GET_KERNEL_GSP(pGpu);
    ConfidentialCompute *pCC              = GPU_GET_CONF_COMPUTE(pGpu);
    NV_ASSERT(pCC != NULL);

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
    {
        NV_PRINTF(LEVEL_NOTICE, "Loading GSP-RM image using SEC2.\n");

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
        NV_PRINTF(LEVEL_ERROR, "Loading GSP image for monolithic RM using SEC2.\n");
        if (gspIsDebugModeEnabled_HAL(pGpu, pGsp))
        {
            if (gpuIsGspToBootInInstInSysMode_HAL(pGpu))
            {
                NV_PRINTF(LEVEL_ERROR, "Loading GSP debug inst-in-sys image for monolithic RM using SEC2.\n");

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
                if  (pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED) == NV_TRUE)
                {
                    if ((confComputeIsSpdmEnabled(pGpu, pCC)              == NV_TRUE) &&
                        (pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED) == NV_TRUE))
                    {
                        return gspGetBinArchiveGspFmcSpdmGfwDebugSigned_HAL(pGsp);
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
                NV_PRINTF(LEVEL_ERROR, "Loading GSP prod inst-in-sys image for monolithic RM using SEC2.\n");

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
    // It does not make sense to boot monolithic RM when physical SEC2 module
    // does not exist
    //
    return NULL;
}

static NV_STATUS
ksec2GetGspBootArgs
(
    OBJGPU     *pGpu,
    KernelSec2  *pKernelSec2,
    RmPhysAddr *pBootArgsGspSysmemOffset
)
{
    NV_STATUS status         = NV_OK;

    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    NV_ASSERT(pCC != NULL);

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
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
 * @brief Set up GSP-FMC and boot args for SEC2 command
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or error if failed
 */
NV_STATUS
ksec2SetupGspImages_GB20B
(
    OBJGPU           *pGpu,
    KernelSec2        *pKernelSec2,
    NVDM_PAYLOAD_COT *pCotPayload
)
{
    NV_STATUS status = NV_OK;

    const BINDATA_ARCHIVE *pBinArchive;
    PBINDATA_STORAGE pGspImage;
    PBINDATA_STORAGE pGspImageHash;
    PBINDATA_STORAGE pGspImageSignature;
    PBINDATA_STORAGE pGspImagePublicKey;
    NvU32 pGspImageSize;
    NvU32 pGspImageMapSize;
    NvP64 pVaKernel = NULL;
    NvP64 pPrivKernel = NULL;
    NvU64 flags = MEMDESC_FLAGS_NONE;

    //
    // On systems with SEV enabled, the GSP-FMC image has to be accessible
    // to SEC2 (an unit inside GPU) and hence placed in unprotected sysmem
    //
    flags = MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    // Detect the mode of operation for GSP and fetch the right image to boot
    pBinArchive = _ksec2GetGspUcodeArchive(pGpu, pKernelSec2);
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

    status = memdescCreate(&pKernelSec2->pGspFmcMemdesc, pGpu, pGspImageMapSize,
                           0, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED, flags);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_7,
                    pKernelSec2->pGspFmcMemdesc);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    status = memdescMap(pKernelSec2->pGspFmcMemdesc, 0, pGspImageMapSize, NV_TRUE,
                        NV_PROTECT_READ_WRITE, &pVaKernel, &pPrivKernel);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    portMemSet(pVaKernel, 0, pGspImageMapSize);

    status = bindataWriteToBuffer(pGspImage, pVaKernel, pGspImageSize);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    // Clean up CPU side resources since they are not needed anymore
    memdescUnmap(pKernelSec2->pGspFmcMemdesc, NV_TRUE, pVaKernel, pPrivKernel);

    pCotPayload->gspFmcSysmemOffset = memdescGetPhysAddr(pKernelSec2->pGspFmcMemdesc, AT_GPU, 0);

    status = bindataWriteToBuffer(pGspImageHash, (NvU8*)pCotPayload->hash384, sizeof(pCotPayload->hash384));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) == pKernelSec2->cotPayloadSignatureSize, failed);
    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) <= sizeof(pCotPayload->signature), failed);
    status = bindataWriteToBuffer(pGspImageSignature, (NvU8*)pCotPayload->signature, bindataGetBufferSize(pGspImageSignature));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) == pKernelSec2->cotPayloadPublicKeySize, failed);
    NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) <= sizeof(pCotPayload->publicKey), failed);
    status = bindataWriteToBuffer(pGspImagePublicKey, (NvU8*)pCotPayload->publicKey, bindataGetBufferSize(pGspImagePublicKey));
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    // Set up boot args based on the mode of operation
    status = ksec2GetGspBootArgs(pGpu, pKernelSec2, &pCotPayload->gspBootArgsSysmemOffset);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    return NV_OK;

failed:
    memdescDestroy(pKernelSec2->pGspFmcMemdesc);
    pKernelSec2->pGspFmcMemdesc = NULL;

    return status;
}

static NV_STATUS
_ksec2CheckGspBootStatus
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    // On GSP-RM, the kgsp code path will check for GSP boot status
    return NV_OK;
}

/*!
 * @brief Dump debug registers for SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK, or error if failed
 */
void
ksec2DumpDebugState_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{

    NV_PRINTF(LEVEL_ERROR, "GPU %04x:%02x:%02x\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu));

}

/*!
 * @brief Checks whether GSP_FMC is enforced on this
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_TRUE, if GspFmc is enforced.
 */
NvBool
ksec2GspFmcIsEnforced_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    return NV_TRUE;
}

/*!
 * @brief Check if okay to send GSP-FMC and FRTS info to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK
 *     Okay to send boot commands
 * @return NV_WARN_NOTHING_TO_DO
 *     No need to send boot commands
 * @return NV_ERR_NOT_SUPPORTED
 *     Should not send boot commands
 */
NV_STATUS
ksec2SafeToSendBootCommands_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    if (!IS_EMULATION(pGpu) && !IS_SILICON(pGpu))
    {
        //
        // SEC2 managment partition is only enabled when secure boot is enabled
        // on silicon and certain emulation configs
        //
        return NV_WARN_NOTHING_TO_DO;
    }

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_IS_MISSING))
    {
        if (IS_SILICON(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR, "RM cannot boot with SEC2 missing on silicon.\n");
            return NV_ERR_NOT_SUPPORTED;
        }

        NV_PRINTF(LEVEL_WARNING, "Secure boot is disabled due to missing SEC2.\n");
        return NV_WARN_NOTHING_TO_DO;
    }

    // Enforce GSP-FMC can only be booted by SEC2 on silicon.
    if (IS_SILICON(pGpu) &&
        ksec2GspFmcIsEnforced_HAL(pGpu, pKernelSec2) &&
        pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_ERROR, "Chain-of-trust (GSP-FMC) cannot be disabled on silicon.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_WARNING, "Chain-of-trust is disabled via regkey\n");
        pKernelSec2->setProperty(pKernelSec2, PDB_PROP_KSEC2_BOOT_COMMAND_OK, NV_TRUE);
        return NV_WARN_NOTHING_TO_DO;
    }

    return NV_OK;
}

/*!
 * @brief Prepare GSP-FMC and FRTS info to send to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info ready to send
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped preparing boot commands
 * @return Other error
 *     Error preparing GSP-FMC and FRTS info
 */
NV_STATUS
ksec2PrepareBootCommands_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;
    NV_STATUS statusBoot;

    statusBoot = ksec2WaitForSecureBoot_HAL(pGpu, pKernelSec2);

    // Confirm SEC2 secure boot partition is done
    if (statusBoot != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SEC2 secure boot partition timed out.\n");
        status = statusBoot;
        goto failed;
    }

    statusBoot = ksec2SafeToSendBootCommands_HAL(pGpu, pKernelSec2);
    if (statusBoot != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SEC2 secure boot GSP prechecks failed.\n");
        status = statusBoot;
        goto failed;
    }

    // COT payload is freed in SEC2 state cleanup during state destroy.
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        pKernelSec2->pCotPayload = portMemAllocNonPaged(sizeof(NVDM_PAYLOAD_COT));
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, (pKernelSec2->pCotPayload == NULL) ? NV_ERR_NO_MEMORY : NV_OK, failed);
        portMemSet(pKernelSec2->pCotPayload, 0, sizeof(NVDM_PAYLOAD_COT));
    }

    pKernelSec2->pCotPayload->version = pKernelSec2->cotPayloadVersion;
    pKernelSec2->pCotPayload->size = sizeof(NVDM_PAYLOAD_COT);

    //
    // Set up vidmem for FRTS copy

    pKernelSec2->pCotPayload->frtsVidmemOffset = 0;
    pKernelSec2->pCotPayload->frtsVidmemSize = 0;

    pKernelSec2->pCotPayload->gspFmcSysmemOffset = (NvU64)-1;
    pKernelSec2->pCotPayload->gspBootArgsSysmemOffset = (NvU64)-1;

    // Set up GSP-FMC for SEC2 to boot GSP
    if (!pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_DISABLE_GSPFMC))
    {
            status = ksec2SetupGspImages_HAL(pGpu, pKernelSec2, pKernelSec2->pCotPayload);
            if (status!= NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Ucode image preparation failed!\n");
                goto failed;
            }

    }
    return NV_OK;

failed:
    NV_PRINTF(LEVEL_ERROR, "Preparing SEC2 boot cmds failed. RM cannot boot.\n");

    ksec2CleanupBootState(pGpu, pKernelSec2);

    return status;
}
/*!
 * @brief Send GSP-FMC and FRTS info to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info sent to SEC2
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped sending boot commands
 * @return Other error
 *     Error sending GSP-FMC and FRTS info to SEC2
 */
NV_STATUS
ksec2SendBootCommands_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelSec2->pCotPayload != NULL, NV_ERR_INVALID_STATE);

    status = ksec2SendAndReadMessage(pGpu, pKernelSec2, (NvU8 *)pKernelSec2->pCotPayload,
                                    sizeof(NVDM_PAYLOAD_COT), NVDM_TYPE_COT, NULL, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Sent following content to SEC2: \n");
        NV_PRINTF(LEVEL_ERROR, "version=0x%x, size=0x%x, gspFmcSysmemOffset=0x%llx\n",
            pKernelSec2->pCotPayload->version, pKernelSec2->pCotPayload->size,
            pKernelSec2->pCotPayload->gspFmcSysmemOffset);
        NV_PRINTF(LEVEL_ERROR, "frtsSysmemOffset=0x%llx, frtsSysmemSize=0x%x\n",
            pKernelSec2->pCotPayload->frtsSysmemOffset, pKernelSec2->pCotPayload->frtsSysmemSize);
        NV_PRINTF(LEVEL_ERROR, "frtsVidmemOffset=0x%llx, frtsVidmemSize=0x%x\n",
            pKernelSec2->pCotPayload->frtsVidmemOffset, pKernelSec2->pCotPayload->frtsVidmemSize);
        NV_PRINTF(LEVEL_ERROR, "gspBootArgsSysmemOffset=0x%llx\n",
            pKernelSec2->pCotPayload->gspBootArgsSysmemOffset);
        goto failed;
    }

    //
    // Need to check if GSP has been booted here so that we can skip booting
    // GSP again later in ACR code path. On GSP-RM, the calling code path (kgsp)
    // will check for GSP boot status.
    //
    if (!pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_DISABLE_GSPFMC) &&
        !pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
    {
        status = _ksec2CheckGspBootStatus(pGpu, pKernelSec2);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);
    }

    // Set property to indicate we only support secure boot at this point
    pKernelSec2->setProperty(pKernelSec2, PDB_PROP_KSEC2_BOOT_COMMAND_OK, NV_TRUE);
    return NV_OK;

failed:
    NV_PRINTF(LEVEL_ERROR, "SEC2 boot cmds failed. RM cannot boot.\n");
    ksec2DumpDebugState_HAL(pGpu, pKernelSec2);

    ksec2CleanupBootState(pGpu, pKernelSec2);

    return status;
}

/*!
 * @brief Prepare and send GSP-FMC and FRTS info to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK
 *     GSP-FMC and FRTS info sent to SEC2 or determined okay to skip sending info
 * @return Other error
 *     Error preparing or sending GSP-FMC and FRTS info to SEC2
 */
NV_STATUS
ksec2PrepareAndSendBootCommands_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status;
    status = ksec2PrepareBootCommands_HAL(pGpu, pKernelSec2);
    if (status != NV_OK)
    {
        return (status == NV_WARN_NOTHING_TO_DO) ? NV_OK : status;
    }
    return ksec2SendBootCommands_HAL(pGpu, pKernelSec2);
}

NV_STATUS
ksec2ErrorCode2NvStatusMap_GB20B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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

        default:
        return NV_ERR_GENERIC;
    }
}



