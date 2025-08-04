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

#include "gpu/sec2/kernel_sec2.h"
#include "core/core.h"

#include "fsp/fsp_nvdm_format.h"
#include "fsp/fsp_mctp_format.h"
#include "gpu/fsp/kern_fsp_cot_payload.h"
#include "gpu/gsp/kernel_gsp.h"
#include "published/blackwell/gb10b/dev_gsp.h"
#include "published/blackwell/gb10b/dev_sec_pri.h"
#include "published/blackwell/gb10b/dev_falcon_v4.h"

// RM uses channel 0 for SEC EMEM.
#define SEC2_EMEM_CHANNEL_RM             0x0

// EMEM channel 0 (RM) is allocated 1K bytes.
#define SEC2_EMEM_CHANNEL_RM_SIZE        1024

// Blocks are 64 DWORDS
#define DWORDS_PER_EMEM_BLOCK 64U

static void _ksec2UpdateQueueHeadTail_GB10B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 queueHead, NvU32 queueTail);

static void _ksec2GetQueueHeadTail_GB10B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 *pQueueHead, NvU32 *pQueueTail);

static NV_STATUS _ksec2ConfigEmemc_GB10B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU32 offset, NvBool bAincw, NvBool bAincr);

static NV_STATUS _ksec2WriteToEmem_GB10B(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
    NvU8 *pBuffer, NvU32 size);

static NvBool _ksec2IsGspTargetMaskReleased(OBJGPU *pGpu, void *pVoid);

static NvBool _ksec2IsCmdhandlingCompleted_GB10B(OBJGPU *pGpu, void *pVoid);

static NV_STATUS _ksec2WaitForCmdHandling(OBJGPU *pGpu, KernelSec2 *pKernelSec2);
static const BINDATA_ARCHIVE *_ksec2GetGspUcodeArchive(OBJGPU *pGpu, KernelSec2 *pKernelSec2);
static NV_STATUS _ksec2GetGspBootArgs(OBJGPU *pGpu, KernelSec2 *pKernelSec2,
                                      RmPhysAddr *pBootArgsGspSysmemOffset);
/*!
 * @brief Update command queue head and tail pointers
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 * @param[in] queueHead     Offset to write to command queue head
 * @param[in] queueTail     Offset to write to command queue tail
 */
static void
_ksec2UpdateQueueHeadTail_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU32      queueHead,
    NvU32      queueTail
)
{
    // The write to HEAD needs to happen after TAIL because it will interrupt SEC2
    GPU_REG_WR32(pGpu, NV_PSEC_QUEUE_TAIL(SEC2_EMEM_CHANNEL_RM), queueTail);
    GPU_REG_WR32(pGpu, NV_PSEC_QUEUE_HEAD(SEC2_EMEM_CHANNEL_RM), queueHead);
}

/*!
 * @brief Read command queue head and tail pointers
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelSec2  KernelSec2 pointer
 * @param[out] pQueueHead   Pointer where we write command queue head
 * @param[out] pQueueTail   Pointer where we write command queue tail
 */
static void
_ksec2GetQueueHeadTail_GB10B
(
    OBJGPU      *pGpu,
    KernelSec2  *pKernelSec2,
    NvU32       *pQueueHead,
    NvU32       *pQueueTail
)
{
    *pQueueHead = GPU_REG_RD32(pGpu, NV_PSEC_QUEUE_HEAD(SEC2_EMEM_CHANNEL_RM));
    *pQueueTail = GPU_REG_RD32(pGpu, NV_PSEC_QUEUE_TAIL(SEC2_EMEM_CHANNEL_RM));
}

/*!
 * @brief Configure EMEMC for RM's queue in SEC2 EMEM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 * @param[in] offset        Offset to write to EMEMC in DWORDS
 * @param[in] bAincw        Flag to set auto-increment on writes
 * @param[in] bAincr        Flag to set auto-increment on reads
 *
 * @return NV_OK
 */
static NV_STATUS
_ksec2ConfigEmemc_GB10B
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

    GPU_REG_WR32(pGpu, NV_PSEC_EMEMC(0), reg32);

    return NV_OK;
}

/*!
 * @brief Write data in buffer to RM channel in SEC's EMEM
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 * @param[in] pBuffer     Buffer with data to write to EMEM
 * @param[in] Size        Size of buffer in bytes, assumed DWORD aligned
 *
 * @return NV_OK
 */
static NV_STATUS
_ksec2WriteToEmem_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
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

    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(0x0));
    ememOffsetStart = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetStart += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "About to send data to SEC, ememcOff=0x%x, size=0x%x\n", ememOffsetStart, size);

    // Now write to EMEMD, we always have to write a DWORD at a time so write
    // all the full DWORDs in the buffer and then pad any leftover bytes with 0
    for (i = 0; i < (size / sizeof(NvU32)); i++)
    {
        GPU_REG_WR32(pGpu, NV_PSEC_EMEMD(0x0), ((NvU32 *)(void *)pBuffer)[i]);
    }
    wordsWritten = size / sizeof(NvU32);

    leftoverBytes = size % sizeof(NvU32);
    if (leftoverBytes != 0)
    {
        reg32 = 0;
        portMemCopy(&reg32, sizeof(NvU32), &(pBuffer[size - leftoverBytes]), leftoverBytes);
        GPU_REG_WR32(pGpu, NV_PSEC_EMEMD(0x0), reg32);
        wordsWritten++;
    }

    // Sanity check offset. If this fails, the autoincrement did not work
    reg32 = GPU_REG_RD32(pGpu, NV_PSEC_EMEMC(0x0));
    ememOffsetEnd = DRF_VAL(_PSEC, _EMEMC, _OFFS, reg32);
    ememOffsetEnd += DRF_VAL(_PSEC, _EMEMC, _BLK, reg32) * DWORDS_PER_EMEM_BLOCK;
    NV_PRINTF(LEVEL_INFO, "After sending data, ememcOff = 0x%x\n", ememOffsetEnd);

    NV_ASSERT_OR_RETURN((ememOffsetEnd - ememOffsetStart) == wordsWritten, NV_ERR_INVALID_STATE);

    return NV_OK;
}

static const BINDATA_ARCHIVE *
_ksec2GetGspUcodeArchive
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
    {
        if (kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp))
        {
            NV_PRINTF(LEVEL_INFO, "Loading Debug GSP FMC image for GSP RM using SEC2.\n");
            return kgspGetBinArchiveGspRmFmcGfwDebugSigned_HAL(pKernelGsp);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Loading Prod GSP FMC image for GSP RM using SEC2.\n");
            return kgspGetBinArchiveGspRmFmcGfwProdSigned_HAL(pKernelGsp);
        }
    }
    return NULL;
}

static NV_STATUS
_ksec2GetGspBootArgs
(
    OBJGPU     *pGpu,
    KernelSec2  *pKernelSec2,
    RmPhysAddr *pBootArgsGspSysmemOffset
)
{
    NV_STATUS status = NV_OK;

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        // Ensure bootArgs have been set up before
        NV_ASSERT_OR_RETURN(pKernelGsp->pGspFmcArgumentsCached != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pKernelGsp->pGspFmcArgumentsDescriptor) ==
                            ADDR_SYSMEM, NV_ERR_INVALID_STATE);
        *pBootArgsGspSysmemOffset = memdescGetPhysAddr(pKernelGsp->pGspFmcArgumentsDescriptor, AT_GPU, 0);
    }

    return status;
}

static NvBool
_ksec2IsGspTargetMaskReleased
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

static NvBool
_ksec2IsCmdhandlingCompleted_GB10B
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    const NvU32 cmdHandlingState = 0xc001cafe;
    NvU32 reg;

    reg = GPU_REG_RD32(pGpu, NV_PSEC_FALCON_MAILBOX0);
    return (reg  != cmdHandlingState);
}

static NV_STATUS
_ksec2WaitForCmdHandling
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    status =  gpuTimeoutCondWait(pGpu, _ksec2IsCmdhandlingCompleted_GB10B, NULL, NULL);

    return status;
}

/*!
 * @brief Get maximum size of a packet we can send.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return Packet size in bytes
 */
NvU32
ksec2GetMaxSendPacketSize_GB10B
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
 * @brief Check if RM can send a packet to SEC2
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 *
 * @return NV_TRUE if send is possible, NV_FALSE otherwise
 */
NvBool
ksec2CanSendPacket_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NvU32 cmdqHead;
    NvU32 cmdqTail;

    _ksec2GetQueueHeadTail_GB10B(pGpu, pKernelSec2, &cmdqHead, &cmdqTail);

    // SEC2 will set QUEUE_HEAD = TAIL after each packet is received
    return (cmdqHead == cmdqTail);
}

/*!
 * Determine if GSP's target mask is released.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK if GSP target mask released
 */
NV_STATUS
ksec2WaitForGspTargetMaskReleased_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    status =  gpuTimeoutCondWait(pGpu, _ksec2IsGspTargetMaskReleased, NULL, NULL);

    return status;
}

/*!
 * @brief Create NVDM payload header
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 * @param[in] nvdmType      NVDM type to include in header
 *
 * @return Constructed NVDM payload header
 */
NvU32
ksec2CreateNvdmHeader_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU32      nvdmType
)
{
    return REF_DEF(MCTP_MSG_HEADER_TYPE, _VENDOR_PCI) |
           REF_DEF(MCTP_MSG_HEADER_VENDOR_ID, _NV)    |
           REF_NUM(MCTP_MSG_HEADER_NVDM_TYPE, (nvdmType));
}

/*!
 * @brief Check if okay to send GSP-FMC and FRTS info to SEC2
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelSec2 KernelSec2 pointer
 *
 * @return NV_OK
 *     Okay to send boot commands
 * @return NV_ERR_NOT_SUPPORTED
 *     Should not send boot commands
 */
NV_STATUS
ksec2SafeToSendBootCommands_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_BOOT_GSPFMC))
    {
       return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
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
ksec2SetupGspImages_GB10B
(
    OBJGPU           *pGpu,
    KernelSec2       *pKernelSec2,
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

    NV_PRINTF(LEVEL_INFO, "Setup GSP FMC Images!\n");

    // Detect the mode of operation for GSP and fetch the right image to boot
    pBinArchive = _ksec2GetGspUcodeArchive(pGpu, pKernelSec2);
    if (pBinArchive == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot find correct ucode archive for booting!\n");
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto gsp_image_err;
    }

    if (pKernelSec2->getProperty(pKernelSec2, PDB_PROP_KSEC2_GSP_MODE_GSPRM))
    {
        // Set up the structures to send GSP-FMC
        pGspImage = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_IMAGE);
        pGspImageHash = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_HASH);
        pGspImageSignature = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_SIG);
        pGspImagePublicKey = (PBINDATA_STORAGE)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_PKEY);

        if ((pGspImage == NULL) || (pGspImageHash == NULL) ||
            (pGspImageSignature == NULL) || (pGspImagePublicKey == NULL))
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto gsp_image_err;
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
        pCotPayload->frtsSysmemSize = pGspImageSize;
        NV_PRINTF(LEVEL_INFO, "Setup GSP FMC SysMem Offset GPA 0x%llx, SPA 0x%llx ,Size = 0x%x!\n",
                  pCotPayload->gspFmcSysmemOffset,
                  memdescGetPhysAddr(pKernelSec2->pGspFmcMemdesc, AT_CPU, 0),
                  pCotPayload->frtsSysmemSize);

        status = bindataWriteToBuffer(pGspImageHash, (NvU8*)pCotPayload->hash384, sizeof(pCotPayload->hash384));
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);

        NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) == pKernelSec2->cotPayloadSignatureSize, failed);
        NV_PRINTF(LEVEL_INFO, "GSP FMC Signature Size 0x%08x\n", bindataGetBufferSize(pGspImageSignature));
        NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImageSignature) <= sizeof(pCotPayload->signature), failed);
        status = bindataWriteToBuffer(pGspImageSignature, (NvU8*)pCotPayload->signature,
                                      bindataGetBufferSize(pGspImageSignature));
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);

        NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) == pKernelSec2->cotPayloadPublicKeySize, failed);
        NV_PRINTF(LEVEL_INFO, "GSP FMC PK Size 0x%08x\n", bindataGetBufferSize(pGspImagePublicKey));
        NV_ASSERT_OR_GOTO(bindataGetBufferSize(pGspImagePublicKey) <= sizeof(pCotPayload->publicKey), failed);
        status = bindataWriteToBuffer(pGspImagePublicKey, (NvU8*)pCotPayload->publicKey,
                                      bindataGetBufferSize(pGspImagePublicKey));
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);

        // Set up boot args based on the mode of operation
        status = _ksec2GetGspBootArgs(pGpu, pKernelSec2, &pCotPayload->gspBootArgsSysmemOffset);
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);
    }

    return NV_OK;

failed:
    memdescDestroy(pKernelSec2->pGspFmcMemdesc);
    pKernelSec2->pGspFmcMemdesc = NULL;

gsp_image_err:
    return status;
}


/*!
 * @brief Prepare GSP-FMC info to send to SEC2
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 *
 * @return NV_OK
 *     GSP-FMC info ready to send
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped preparing boot commands
 * @return Other error
 *     Error preparing GSP-FMC info
 */
NV_STATUS
ksec2PrepareBootCommands_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    status = ksec2SafeToSendBootCommands_HAL(pGpu, pKernelSec2);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SEC2 not yet ready to accept GSP Boot command!\n");
        return status;
    }

    pKernelSec2->pCotPayload = portMemAllocNonPaged(sizeof(NVDM_PAYLOAD_COT));
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, (pKernelSec2->pCotPayload == NULL) ? NV_ERR_NO_MEMORY : NV_OK, failed);
    portMemSet(pKernelSec2->pCotPayload, 0, sizeof(NVDM_PAYLOAD_COT));

    pKernelSec2->pCotPayload->version = pKernelSec2->cotPayloadVersion;
    pKernelSec2->pCotPayload->size = sizeof(NVDM_PAYLOAD_COT);

    status = ksec2SetupGspImages_HAL(pGpu, pKernelSec2, pKernelSec2->pCotPayload);
    if (status!= NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP Ucode image preparation failed!\n");
        goto failed;
    }

    return NV_OK;

failed:
    ksec2CleanupBootState(pGpu, pKernelSec2);

    return status;
}

/*!
 * @brief Send BOOT_GSP cmd with GSP-FMC info to SEC2
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 *
 * @return NV_OK
 *     Boot cmd with GSP-FMC info sent to SEC2
 * @return NV_WARN_NOTHING_TO_DO
 *     Skipped sending boot commands
 * @return Other error
 *     Error sending GSP-FMC info to SEC2
 */
NV_STATUS
ksec2SendBootCommands_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelSec2->pCotPayload != NULL, NV_ERR_INVALID_STATE);

    status = ksec2SendMessage(pGpu, pKernelSec2, (NvU8 *)pKernelSec2->pCotPayload,
                                    sizeof(NVDM_PAYLOAD_COT), NVDM_TYPE_COT);
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

    NV_PRINTF(LEVEL_INFO, "Wait for SEC2 CMD handling: \n");
    status = _ksec2WaitForCmdHandling(pGpu, pKernelSec2);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timed out waiting for SEC2 Cmd Handling.\n");
        NV_PRINTF(LEVEL_ERROR, "After CMD timeout NV_PSEC_FALCON_MAILBOX0 = 0x%x\n",
                  GPU_REG_RD32(pGpu, NV_PSEC_FALCON_MAILBOX0));
        NV_PRINTF(LEVEL_ERROR, "After CMD timeout NV_PSEC_FALCON_MAILBOX1 = 0x%x\n",
                  GPU_REG_RD32(pGpu, NV_PSEC_FALCON_MAILBOX1));
        DBG_BREAKPOINT();
        goto failed;
    }

    return NV_OK;

failed:
    ksec2CleanupBootState(pGpu, pKernelSec2);

    return status;
}

/*!
 * @brief Prepare and send GSP-FMC info to SEC2
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 *
 * @return NV_OK
 *     GSP-FMC info sent to SEC2 or determined okay to skip sending info
 * @return Other error
 *     Error preparing or sending GSP-FMC info to SEC2
 */
NV_STATUS
ksec2PrepareAndSendBootCommands_GB10B
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

/*!
 * @brief Send one packet to SEC via EMEM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelSec2   KernelSec2 pointer
 * @param[in] pPacket       Packet buffer
 * @param[in] packetSize    Packet size in bytes
 *
 * @return NV_OK, or NV_ERR_INSUFFICIENT_RESOURCES
 */
NV_STATUS
ksec2SendPacket_GB10B
(
    OBJGPU    *pGpu,
    KernelSec2 *pKernelSec2,
    NvU8      *pPacket,
    NvU32      packetSize
)
{
    NvU32 paddedSize;
    NV_STATUS status = NV_OK;

    // Check that queue is ready to receive data
    status = ksec2PollForCanSend(pGpu, pKernelSec2);
    if (status != NV_OK)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    //
    // First configure EMEMC, RM always writes 0 to the offset, which is OK
    // because RM's channel starts at 0. RM sends only 1 command for now,
    // which is the BOOT GSP command. This can be later improved if requirement
    // for multiple commands come up.
    _ksec2ConfigEmemc_GB10B(pGpu, pKernelSec2, 0, NV_TRUE, NV_FALSE);
    _ksec2WriteToEmem_GB10B(pGpu, pKernelSec2, pPacket, packetSize);

    paddedSize = NV_ALIGN_UP(packetSize, sizeof(NvU32));

    // Update HEAD and TAIL with new EMEM offset; RM always starts at offset 0.
    // TAIL points to the last DWORD written, so subtract 1 DWORD
    _ksec2UpdateQueueHeadTail_GB10B(pGpu, pKernelSec2, 0, paddedSize - sizeof(NvU32));

    return status;
}
