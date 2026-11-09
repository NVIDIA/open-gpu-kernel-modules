/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_VBIOS_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/vbios/kern_vbios.h"

#include "gpu/gsp/kernel_gsp.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "platform/pci_exp_table.h" // PCI_EXP_ROM_*

#include "published/turing/tu102/dev_bus.h" // for NV_PBUS_VBIOS_SCRATCH, NV_PBUS_IFR_FMT_FIXED*
#include "published/turing/tu102/dev_fb.h"  // for NV_PFB_PRI_MMU_WPR2_ADDR_HI
#include "published/turing/tu102/dev_gc6_island.h"
#include "published/turing/tu102/dev_gc6_island_addendum.h"
#include "published/turing/tu102/dev_ext_devices.h"  // for NV_PROM_DATA

#define NV_ROM_DIRECTORY_IDENTIFIER     0x44524652  // "RFRD"
#define NV_ROM_DIRECTORY_VER_OFFSET              4
#define NV_ROM_DIRECTORY_VER_03             0x0003
#define NV_ROM_DIRECTORY_VER_04             0x0004
#define NV_ROM_DIRECTORY_OPTROM_OFFSET           8

#define NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_BASE   0x00
#define NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_EXT    0xE0

// ---------------------------------------------------------------------------
// Structures and defines for FWSEC commands
// ---------------------------------------------------------------------------

typedef struct
{
    NvU8 version;
    NvU8 headerSize;
    NvU8 entrySize;
    NvU8 entryCount;
} FALCON_APPLICATION_INTERFACE_HEADER_V1;

typedef struct
{
    NvU32 id;
    NvU32 dmemOffset;
} FALCON_APPLICATION_INTERFACE_ENTRY_V1;

#define FALCON_APPLICATION_INTERFACE_ENTRY_ID_DMEMMAPPER     (0x4)

typedef struct
{
    NvU32 signature;
    NvU16 version;
    NvU16 size;
    NvU32 cmd_in_buffer_offset;
    NvU32 cmd_in_buffer_size;
    NvU32 cmd_out_buffer_offset;
    NvU32 cmd_out_buffer_size;
    NvU32 nvf_img_data_buffer_offset;
    NvU32 nvf_img_data_buffer_size;
    NvU32 printfBufferHdr;
    NvU32 ucode_build_time_stamp;
    NvU32 ucode_signature;
    NvU32 init_cmd;
    NvU32 ucode_feature;
    NvU32 ucode_cmd_mask0;
    NvU32 ucode_cmd_mask1;
    NvU32 multiTgtTbl;
} FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3;

#define FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS (0x15)
#define FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB   (0x19)

typedef struct
{
    NvU32 version;
    NvU32 size;
    NvU64 gfwImageOffset;
    NvU32 gfwImageSize;
    NvU32 flags;
} FWSECLIC_READ_VBIOS_DESC;

#define FWSECLIC_READ_VBIOS_STRUCT_FLAGS (2)

typedef struct
{
    NvU32 version;
    NvU32 size;
    NvU32 frtsRegionOffset4K;
    NvU32 frtsRegionSize;
    NvU32 frtsRegionMediaType;
} FWSECLIC_FRTS_REGION_DESC;

#define FWSECLIC_FRTS_REGION_MEDIA_FB (2)
#define FWSECLIC_FRTS_REGION_SIZE_1MB_IN_4K (0x100)

typedef struct
{
    FWSECLIC_READ_VBIOS_DESC readVbiosDesc;
    FWSECLIC_FRTS_REGION_DESC frtsRegionDesc;
} FWSECLIC_FRTS_CMD;

#define NV_VBIOS_FWSECLIC_SCRATCH_INDEX_0E      0x0E
#define NV_VBIOS_FWSECLIC_FRTS_ERR_CODE         31:16
#define NV_VBIOS_FWSECLIC_FRTS_ERR_CODE_NONE    0x00000000

#define NV_VBIOS_FWSECLIC_SCRATCH_INDEX_15      0x15
#define NV_VBIOS_FWSECLIC_SB_ERR_CODE           15:0
#define NV_VBIOS_FWSECLIC_SB_ERR_CODE_NONE      0x00000000

NvU8
kvbiosReadProm08_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    NvU32 offset
)
{
    PORT_UNREFERENCED_VARIABLE(pKernelVbios);

    return GPU_REG_RD08_UNCHECKED(pGpu, NV_PROM_DATA(offset));
}

NvU32
kvbiosReadProm32_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    NvU32 offset
)
{
    PORT_UNREFERENCED_VARIABLE(pKernelVbios);

    return GPU_REG_RD32_UNCHECKED(pGpu, NV_PROM_DATA(offset));
}

typedef struct RomImgSrc
{

    NvU32 baseOffset;
    NvU32 maxOffset;

    OBJGPU *pGpu;
    KernelVbios *pKernelVbios;
} RomImgSrc;

/*!
 * Read unaligned data from PROM (e.g. VBIOS ROM image) using 32-bit accesses.
 *
 * @param[in]   pSrc        RomImgSrc pointer
 * @param[in]   offset      NV_PROM offset to read (note: added to baseOffset)
 * @param[in]   sizeBytes   byte count to read (1, 2, or 4)
 * @param[out]  pStatus     status of attempted read (NV_OK on success)
 *
 * @return   value read
 */
static NvU32
s_romImgReadGeneric
(
    const RomImgSrc * const pSrc,
    NvU32 offset,
    NvU32 sizeBytes,
    NV_STATUS *pStatus
)
{

    union
    {
        NvU32 word[2];
        NvU8  byte[2 * sizeof(NvU32)];
    } buf;

    NvU32  retValue = 0;
    NvU32  byteIndex;
    NvBool bSafe;
    NvBool bReadWord1;

    NV_ASSERT(pSrc != NULL);
    NV_ASSERT(pStatus != NULL);
    NV_ASSERT(sizeBytes <= 4);

    if (NV_UNLIKELY(*pStatus != NV_OK))
    {
        // Do not attempt read if previous status was not NV_OK
        return 0;
    }

    bSafe = portSafeAddU32(offset, pSrc->baseOffset, &offset);
    if (NV_UNLIKELY(!bSafe))
    {
        *pStatus = NV_ERR_INVALID_OFFSET;
        return 0;
    }

    if (pSrc->maxOffset > 0)
    {
        NvU32 tmp;
        bSafe = portSafeAddU32(offset, sizeBytes, &tmp);
        if (NV_UNLIKELY(!bSafe || tmp > pSrc->maxOffset))
        {
            *pStatus = NV_ERR_INVALID_OFFSET;
            return 0;
        }
    }

    byteIndex  = offset & (sizeof(NvU32) - 1);  // buf.byte[] index for first byte to read.
    offset    -= byteIndex;                     // Align offset.
    byteIndex += sizeBytes;                     // Index of last byte to read + 1.
    bReadWord1 = (byteIndex > sizeof(NvU32));   // Last byte past the first 32-bit word?

    NV_ASSERT(pSrc->pGpu != NULL);

    // Read bios image as aligned 32-bit word(s).
    buf.word[0] = kvbiosReadProm32_HAL(pSrc->pGpu, pSrc->pKernelVbios, offset);
    if (bReadWord1)
    {
        buf.word[1] = kvbiosReadProm32_HAL(pSrc->pGpu, pSrc->pKernelVbios,
                                         offset + sizeof(NvU32));
    }

    // Combine bytes into number.
    for (; sizeBytes > 0; sizeBytes--)
    {
        retValue = (retValue << 8) + buf.byte[--byteIndex];
    }

    *pStatus = NV_OK;
    return retValue;
}

/*!
 * Read a byte from PROM
 */
static NvU8 s_romImgRead8(const RomImgSrc *pSrc, NvU32 offset, NV_STATUS *pStatus)
{
    return (NvU8) s_romImgReadGeneric(pSrc, offset, sizeof(NvU8), pStatus);
}

/*!
 * Read a word in lsb,msb format from PROM
 */
static NvU16 s_romImgRead16(const RomImgSrc *pSrc, NvU32 offset, NV_STATUS *pStatus)
{
    return (NvU16) s_romImgReadGeneric(pSrc, offset, sizeof(NvU16), pStatus);
}

/*!
 * Read a dword in lsb,msb format from PROM
 */
static NvU32 s_romImgRead32(const RomImgSrc *pSrc, NvU32 offset, NV_STATUS *pStatus)
{
    return (NvU32) s_romImgReadGeneric(pSrc, offset, sizeof(NvU32), pStatus);
}

/*!
 * Determine the size of the IFR section from the beginning of a VBIOS image.
 *
 * @param[in]     pGpu      OBJGPU pointer
 * @param[out]    pIfrSize  size of the IFR section
 */
NV_STATUS
kvbiosRomImgFindPciHeader_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    void *pRomImgSrc,
    NvU32 *pIfrSize
)
{

    NV_STATUS status = NV_OK;
    const RomImgSrc * const pSrc = (const RomImgSrc *)pRomImgSrc;

    NvU32 fixed0;
    NvU32 fixed1;
    NvU32 fixed2;
    NvU32 extendedOffset;
    NvU32 imageOffset = 0;
    NvU32 ifrVersion = 0;
    NvU32 ifrTotalDataSize;
    NvU32 flashStatusOffset;
    NvU32 romDirectoryOffset;
    NvU32 romDirectorySig;

    PORT_UNREFERENCED_VARIABLE(pGpu);
    PORT_UNREFERENCED_VARIABLE(pKernelVbios);

    NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pIfrSize != NULL, NV_ERR_INVALID_ARGUMENT);

    fixed0 = s_romImgRead32(pSrc, NV_PBUS_IFR_FMT_FIXED0, &status);
    fixed1 = s_romImgRead32(pSrc, NV_PBUS_IFR_FMT_FIXED1, &status);
    fixed2 = s_romImgRead32(pSrc, NV_PBUS_IFR_FMT_FIXED2, &status);
    NV_ASSERT_OK_OR_RETURN(status);

    // Check for IFR signature.
    if (REF_VAL(NV_PBUS_IFR_FMT_FIXED0_SIGNATURE, fixed0) ==
        NV_PBUS_IFR_FMT_FIXED0_SIGNATURE_VALUE)
    {
        ifrVersion = REF_VAL(NV_PBUS_IFR_FMT_FIXED1_VERSIONSW, fixed1);
        switch (ifrVersion)
        {
            case 0x01:
            case 0x02:
                extendedOffset = REF_VAL(NV_PBUS_IFR_FMT_FIXED1_FIXED_DATA_SIZE, fixed1);
                imageOffset = s_romImgRead32(pSrc, extendedOffset + 4, &status);
                break;

            case 0x03:
                ifrTotalDataSize = REF_VAL(NV_PBUS_IFR_FMT_FIXED2_TOTAL_DATA_SIZE, fixed2);
                flashStatusOffset = s_romImgRead32(pSrc, ifrTotalDataSize, &status);
                romDirectoryOffset = flashStatusOffset + 4096;
                romDirectorySig = s_romImgRead32(pSrc, romDirectoryOffset, &status);
                NV_ASSERT_OK_OR_RETURN(status);

                if (romDirectorySig == NV_ROM_DIRECTORY_IDENTIFIER)
                {
                    imageOffset = s_romImgRead32(pSrc, romDirectoryOffset + 8, &status);
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR, "Error: ROM Directory not found = 0x%08x.\n",
                              romDirectorySig);
                    return NV_ERR_INVALID_DATA;
                }
                break;

            default:
                NV_PRINTF(LEVEL_ERROR, "Error: IFR version not supported = 0x%08x.\n",
                          ifrVersion);
                return NV_ERR_INVALID_DATA;
        }
    }

    NV_ASSERT_OK_OR_RETURN(status);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(imageOffset, 4), NV_ERR_INVALID_ADDRESS);
    *pIfrSize = imageOffset;

    return NV_OK;
}

static NV_STATUS
s_locateExpansionRoms
(
    const RomImgSrc * const pSrc,
    const NvU32 pciOffset,
    NvU32 *pBiosSize,
    NvU32 *pExpansionRomOffset
)
{
    NV_STATUS status = NV_OK;
    NvU32 currBlock = pciOffset;

    // Note: used to compute output for pExpanionRomOffset
    NvU32 extRomOffset = 0;
    NvU32 baseRomSize = 0;

    NvU8 type;
    NvU32 blockOffset = 0;
    NvU32 blockSize = 0;

    // Find all ROMs
    for (;;) {
        RomImgSrc currSrc;
        NvU32 pciBlck;
        NvU32 pciDataSig;

        NvBool bIsLastImage;
        NvU32 imgLen;
        NvU32 subImgLen;

        currSrc = *pSrc;
        currSrc.baseOffset = currBlock;

        pciBlck = s_romImgRead16(&currSrc, OFFSETOF_PCI_EXP_ROM_PCI_DATA_STRUCT_PTR, &status);
        NV_ASSERT_OK_OR_RETURN(status);

        currSrc.baseOffset = currBlock + pciBlck;

        pciDataSig = s_romImgRead32(&currSrc, OFFSETOF_PCI_EXP_ROM_SIG, &status);
        NV_ASSERT_OK_OR_RETURN(status);

        if (!IS_VALID_PCI_DATA_SIG(pciDataSig))
        {
            return NV_ERR_INVALID_DATA;
        }

        bIsLastImage = \
            ((s_romImgRead8(&currSrc, OFFSETOF_PCI_DATA_STRUCT_LAST_IMAGE, &status) & PCI_LAST_IMAGE) != 0);
        imgLen = s_romImgRead16(&currSrc, OFFSETOF_PCI_DATA_STRUCT_IMAGE_LEN, &status);
        subImgLen = imgLen;
        NV_ASSERT_OK_OR_RETURN(status);

        // Look for PCI Data Extension
        {
            RomImgSrc extSrc;
            NvU16 pciDataStructLen = s_romImgRead16(&currSrc, OFFSETOF_PCI_DATA_STRUCT_LEN, &status);
            NvU32 nvPciDataExtAt = (currSrc.baseOffset + pciDataStructLen + 0xF) & ~0xF;
            NvU32 nvPciDataExtSig;
            NV_ASSERT_OK_OR_RETURN(status);

            extSrc = currSrc;
            extSrc.baseOffset = nvPciDataExtAt;

            nvPciDataExtSig = s_romImgRead32(&extSrc, OFFSETOF_PCI_DATA_EXT_STRUCT_SIG, &status);
            NV_ASSERT_OK_OR_RETURN(status);

            if (nvPciDataExtSig == NV_PCI_DATA_EXT_SIG)
            {
                NvU16 nvPciDataExtRev = s_romImgRead16(&extSrc, OFFSETOF_PCI_DATA_EXT_STRUCT_REV, &status);
                NV_ASSERT_OK_OR_RETURN(status);

                if ((nvPciDataExtRev == NV_PCI_DATA_EXT_REV_10) || (nvPciDataExtRev == NV_PCI_DATA_EXT_REV_11))
                {
                    NvU16 nvPciDataExtLen = s_romImgRead16(&extSrc, OFFSETOF_PCI_DATA_EXT_STRUCT_LEN, &status);

                    // use the image length from PCI Data Extension
                    subImgLen = s_romImgRead16(&extSrc, OFFSETOF_PCI_DATA_EXT_STRUCT_SUBIMAGE_LEN, &status);
                    NV_ASSERT_OK_OR_RETURN(status);

                    // use the last image from PCI Data Extension if it is present
                    if (OFFSETOF_PCI_DATA_EXT_STRUCT_LAST_IMAGE + sizeof(NvU8) <= nvPciDataExtLen)
                    {
                        bIsLastImage = \
                            ((s_romImgRead8(&extSrc, OFFSETOF_PCI_DATA_EXT_STRUCT_LAST_IMAGE, &status) & PCI_LAST_IMAGE) != 0);
                    }
                    else if (subImgLen < imgLen)
                    {
                        bIsLastImage = NV_FALSE;
                    }

                    NV_ASSERT_OK_OR_RETURN(status);
                }
            }
        }

        // Determine size and offset for this expansion ROM
        type = s_romImgRead8(&currSrc, OFFSETOF_PCI_DATA_STRUCT_CODE_TYPE, &status);
        NV_ASSERT_OK_OR_RETURN(status);

        blockOffset = currBlock - pciOffset;
        blockSize = subImgLen * PCI_ROM_IMAGE_BLOCK_SIZE;

        if (extRomOffset == 0 && type == NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_EXT)
        {
            extRomOffset = blockOffset;
        }
        else if (baseRomSize == 0 && type == NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_BASE)
        {
            baseRomSize = blockSize;
        }

        // Advance to next ROM
        if (bIsLastImage)
        {
            break;
        }
        else
        {
            currBlock = currBlock + subImgLen * PCI_ROM_IMAGE_BLOCK_SIZE;
        }
    }

    if (pBiosSize != NULL)
    {
        // Pick up last ROM found for total size
        *pBiosSize = blockOffset + blockSize;
    }

    if (pExpansionRomOffset != NULL)
    {
        if (extRomOffset > 0 && baseRomSize > 0)
        {
            *pExpansionRomOffset = extRomOffset - baseRomSize;
        }
        else
        {
            *pExpansionRomOffset = 0;
        }
    }

    return status;
}

/*!
 * Extract VBIOS image from ROM.
 *
 * The resulting KernelVbiosImg should be freed with kvbiosFreeVbiosImg
 * after use.
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   pGpu        KernelGsp pointer
 * @param[out]  ppVbiosImg  Pointer to resulting KernelVbiosImg
 */
NV_STATUS
kvbiosExtractVbiosFromRom_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    KernelVbiosImg **ppVbiosImg
)
{

    NV_STATUS status = NV_OK;

    KernelVbiosImg *pVbiosImg = NULL;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    RomImgSrc src;
    NvU32 romSig;
    NvU32 pciOffset = 0;

    NvU32 biosSize = kvbiosGetBaseBiosMaxSize_HAL(pGpu, pKernelVbios);
    NvU32 biosSizeFromRom;
    NvU32 expansionRomOffset;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(ppVbiosImg != NULL, NV_ERR_INVALID_ARGUMENT);

    pVbiosImg = portMemAllocNonPaged(sizeof(*pVbiosImg));
    if (pVbiosImg == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pVbiosImg, 0, sizeof(*pVbiosImg));

    portMemSet(&src, 0, sizeof(src));
    src.baseOffset = 0;
    src.maxOffset = biosSize;
    src.pGpu = pGpu;
    src.pKernelVbios = pKernelVbios;

    if (pKernelBif != NULL)
    {
        status = kbifPreOsGlobalErotGrantRequest_HAL(pGpu, pKernelBif);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ERoT Req/Grant for EEPROM access failed, status=%u\n",
                    status);
            goto out;
        }
    }

    // Find ROM start
    romSig = s_romImgRead16(&src, OFFSETOF_PCI_EXP_ROM_SIG, &status);
    NV_ASSERT_OK_OR_GOTO(status, status, out);
    if (!IS_VALID_PCI_ROM_SIG(romSig))
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             kvbiosRomImgFindPciHeader_HAL(pGpu, pKernelVbios, &src, &pciOffset),
                             out);

        // Adjust base offset for PCI header
        src.baseOffset = pciOffset;

        romSig = s_romImgRead16(&src, OFFSETOF_PCI_EXP_ROM_SIG, &status);
        NV_ASSERT_OK_OR_GOTO(status, status, out);
    }

    if (!IS_VALID_PCI_ROM_SIG(romSig))
    {
        NV_PRINTF(LEVEL_ERROR, "did not find valid ROM signature\n");
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    status = s_locateExpansionRoms(&src, pciOffset, &biosSizeFromRom, &expansionRomOffset);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to locate expansion ROMs: 0x%x\n", status);
        goto out;
    }

    if (biosSizeFromRom > biosSize)
    {
        NV_PRINTF(LEVEL_ERROR, "expansion ROM has exceedingly large size: 0x%x\n", biosSizeFromRom);
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    biosSize = biosSizeFromRom;

    // Copy to system memory and populate pVbiosImg
    {
        NvU32 i;
        NvU32 biosSizeAligned;

        NvU32 *pImageDwords = portMemAllocNonPaged(biosSize);
        if (pImageDwords == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto out;
        }

        biosSizeAligned = biosSize & (~0x3);
        for (i = 0; i < biosSizeAligned; i += 4)
        {
            pImageDwords[i >> 2] = kvbiosReadProm32_HAL(pGpu, pKernelVbios, pciOffset + i);
        }

        for (; i < biosSize; i++)
        {
            // Finish for non-32-bit-aligned biosSize
            ((NvU8 *) pImageDwords)[i] = kvbiosReadProm08_HAL(pGpu, pKernelVbios,
                                                            pciOffset + i);
        }

        pVbiosImg->pImage = (NvU8 *) pImageDwords;
        pVbiosImg->biosSize = biosSize;
        pVbiosImg->expansionRomOffset = expansionRomOffset;
    }

out:
    if (status == NV_OK)
    {
        *ppVbiosImg = pVbiosImg;
    }
    else
    {
        kvbiosFreeVbiosImg(pVbiosImg);
        pVbiosImg = NULL;
    }

    return status;
}

// ---------------------------------------------------------------------------
// Functions for preparing and executing FWSEC commands
// ---------------------------------------------------------------------------

/*!
 * Patch DMEM of FWSEC for a given command
 *
 * @param[inout]  pMappedData      Pointer to mapped DMEM of FWSEC
 * @param[in]     mappedDataSize   Number of bytes valid under pMappedData
 * @param[in]     cmd              FWSEC command to invoke
 * @param[in]     pCmdBuffer       Buffer containing command arguments to patch in
 * @param[in]     cmdBufferSize    Size of buffer pointed by pCmdBuffer
 * @param[in]     interfaceOffset  Interface offset given by VBIOS for FWSEC
 */
static NV_STATUS
s_vbiosPatchInterfaceData
(
    NvU8 *pMappedData,  // inout
    const NvU32 mappedDataSize,
    const NvU32 cmd,
    const void *pCmdBuffer,
    const NvU32 cmdBufferSize,
    const NvU32 interfaceOffset
)
{
    FALCON_APPLICATION_INTERFACE_HEADER_V1 *pIntFaceHdr = NULL;
    FALCON_APPLICATION_INTERFACE_ENTRY_V1 *pIntFaceEntry = NULL;
    FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3 *pDmemMapper = NULL;

    NvBool bSafe;
    NvU32 index;

    NvU32 curOffset;
    NvU32 nextOffset;

    if (interfaceOffset >= mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(interfaceOffset, sizeof(*pIntFaceHdr), &nextOffset);
    if (!bSafe || nextOffset > mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    pIntFaceHdr = (FALCON_APPLICATION_INTERFACE_HEADER_V1 *) (pMappedData + interfaceOffset);
    if (pIntFaceHdr->entryCount < 2)
    {
        NV_PRINTF(LEVEL_ERROR, "too few interface entires found for FWSEC cmd 0x%x\n", cmd);
        return NV_ERR_INVALID_DATA;
    }

    curOffset = nextOffset;
    for (index = 0; index < pIntFaceHdr->entryCount; index++)
    {
        if (curOffset >= mappedDataSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        bSafe = portSafeAddU32(curOffset, sizeof(*pIntFaceEntry), &nextOffset);
        if (!bSafe || nextOffset > mappedDataSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        pIntFaceEntry = (FALCON_APPLICATION_INTERFACE_ENTRY_V1 *) (pMappedData + curOffset);
        curOffset = nextOffset;

        if (pIntFaceEntry->id == FALCON_APPLICATION_INTERFACE_ENTRY_ID_DMEMMAPPER)
        {
            NvU32 dmemMapperMaxOffset;

            if (pIntFaceEntry->dmemOffset >= mappedDataSize)
            {
                return NV_ERR_INVALID_OFFSET;
            }

            bSafe = portSafeAddU32(pIntFaceEntry->dmemOffset, sizeof(*pDmemMapper),
                                &dmemMapperMaxOffset);
            if (!bSafe || dmemMapperMaxOffset > mappedDataSize)
            {
                return NV_ERR_INVALID_OFFSET;
            }

            pDmemMapper = (FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3 *)
                                (pMappedData + pIntFaceEntry->dmemOffset);
        }
    }

    if (!pDmemMapper)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to find required interface entry for FWSEC cmd 0x%x\n", cmd);
        return NV_ERR_INVALID_DATA;
    }

    pDmemMapper->init_cmd = cmd;

    if (pDmemMapper->cmd_in_buffer_size < cmdBufferSize)
    {
        NV_PRINTF(LEVEL_ERROR, "insufficient cmd buffer for FWSEC interface cmd 0x%x\n", cmd);
    }

    if (pDmemMapper->cmd_in_buffer_offset >= mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pIntFaceEntry->dmemOffset, cmdBufferSize, &nextOffset);
    if (!bSafe || nextOffset > mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    portMemCopy(pMappedData + pDmemMapper->cmd_in_buffer_offset, cmdBufferSize,
                pCmdBuffer, cmdBufferSize);

    return NV_OK;
}
 
/*!
 * Prepare to execute a given FWSEC cmd.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelVbios     KernelGsp pointer
 * @param[in]   pFwsecUcode    KernelGspFlcnUcode structure of FWSEC ucode
 * @param[in]   cmd            FWSEC cmd (FRTS or SB)
 * @param[in]   frtsOffset     (if cmd is FRTS) desired FB offset of FRTS data
 * @param[out]  pPreparedCmd   Prepared command state to pass to kvbiosExecuteFwsec_TU102
 */
NV_STATUS
kvbiosPrepareFwsecCmd_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    const NvU32 cmd,
    const NvU64 frtsOffset,
    KernelVbiosPreparedFwsecCmd *pPreparedCmd
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NV_STATUS status;

    FWSECLIC_READ_VBIOS_DESC readVbiosDesc;
    FWSECLIC_FRTS_CMD frtsCmd;

    void *pCmdBuffer;
    NvU32 cmdBufferSize;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pKernelVbios->pFwsecUcode != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pPreparedCmd != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS) ||
                        (frtsOffset > 0), NV_ERR_INVALID_ARGUMENT);

    if ((cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS) &&
        (cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pPreparedCmd->pFwsecUcode = pKernelVbios->pFwsecUcode;
    pPreparedCmd->cmd = cmd;
    pPreparedCmd->frtsOffset = frtsOffset;

    readVbiosDesc.version = 1;
    readVbiosDesc.size = sizeof(readVbiosDesc);
    readVbiosDesc.gfwImageOffset = 0;
    readVbiosDesc.gfwImageSize = 0;
    readVbiosDesc.flags = FWSECLIC_READ_VBIOS_STRUCT_FLAGS;

    if (cmd == FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS)
    {
        // FRTS takes an FRTS_CMD, here we build that up
        NvU32 blockSizeIn4K = NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K;

        frtsCmd.frtsRegionDesc.version = 1;
        frtsCmd.frtsRegionDesc.size = sizeof(frtsCmd.frtsRegionDesc);
        frtsCmd.frtsRegionDesc.frtsRegionOffset4K = (NvU32) (frtsOffset >> 12);
        frtsCmd.frtsRegionDesc.frtsRegionSize = blockSizeIn4K;
        frtsCmd.frtsRegionDesc.frtsRegionMediaType = FWSECLIC_FRTS_REGION_MEDIA_FB;

        frtsCmd.readVbiosDesc = readVbiosDesc;

        pCmdBuffer = &frtsCmd;
        cmdBufferSize = sizeof(frtsCmd);

    }
    else  // i.e. FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB
    {
        // SB takes READ_VBIOS_DESC directly
        pCmdBuffer = &readVbiosDesc;
        cmdBufferSize = sizeof(readVbiosDesc);
    }

    if (pKernelVbios->pFwsecUcode->bootType == KGSP_FLCN_UCODE_BOOT_FROM_HS)
    {
        KernelGspFlcnUcodeBootFromHs *pUcode = &pKernelVbios->pFwsecUcode->ucodeBootFromHs;
        NvU8 *pMappedImage;
        NvU8 *pMappedData;

        NvBool bSafe;

        NvU32 ucodeVersionVal;
        NvU32 hsSigVersions;
        NvU32 sigOffset;
        NvU32 sigMaxOffset;

        NV_ASSERT_OR_RETURN(pUcode->pUcodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pUcode->pSignatures != NULL, NV_ERR_INVALID_ARGUMENT);

        ucodeVersionVal = kgspReadUcodeFuseVersion_HAL(pGpu, pKernelGsp, pUcode->ucodeId);

        ucodeVersionVal = 1 << ucodeVersionVal;
        hsSigVersions = pUcode->vbiosSigVersions;

        if ((ucodeVersionVal & hsSigVersions) == 0)
        {
            return NV_ERR_NOT_SUPPORTED;;
        }

        sigOffset = 0;
        while ((ucodeVersionVal & hsSigVersions & 1) == 0)
        {
            sigOffset += (hsSigVersions & 1) * pUcode->sigSize;
            hsSigVersions >>= 1;
            ucodeVersionVal >>= 1;
        }

        if (sigOffset >= pUcode->signaturesTotalSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        bSafe = portSafeAddU32(sigOffset, pUcode->sigSize, &sigMaxOffset);
        if (!bSafe || sigMaxOffset > pUcode->signaturesTotalSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        pMappedImage = memdescMapInternal(pGpu, pUcode->pUcodeMemDesc, TRANSFER_FLAGS_NONE);
        if (pMappedImage == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
        pMappedData = pMappedImage + pUcode->dataOffset;

        status = s_vbiosPatchInterfaceData(pMappedData, pUcode->dmemSize, cmd,
                                        pCmdBuffer, cmdBufferSize, pUcode->interfaceOffset);

        portMemCopy(pMappedData + pUcode->hsSigDmemAddr, pUcode->sigSize,
                    ((NvU8 *) pUcode->pSignatures) + sigOffset, pUcode->sigSize);

        memdescUnmapInternal(pGpu, pUcode->pUcodeMemDesc,
                            TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedImage = NULL;
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FWSEC cmd 0x%x: 0x%x\n",
                    cmd, status);
            goto out;
        }
    }
    else if (pKernelVbios->pFwsecUcode->bootType == KGSP_FLCN_UCODE_BOOT_WITH_LOADER)
    {
        KernelGspFlcnUcodeBootWithLoader *pUcode = &pKernelVbios->pFwsecUcode->ucodeBootWithLoader;
        NvU8 *pMappedData;

        NV_ASSERT_OR_RETURN(pUcode->pCodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pUcode->pDataMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

        pMappedData = memdescMapInternal(pGpu, pUcode->pDataMemDesc, TRANSFER_FLAGS_NONE);
        if (pMappedData == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = s_vbiosPatchInterfaceData(pMappedData, pUcode->dmemSize, cmd,
                                        pCmdBuffer, cmdBufferSize, pUcode->interfaceOffset);

        memdescUnmapInternal(pGpu, pUcode->pDataMemDesc,
                            TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FWSEC cmd 0x%x: 0x%x\n",
                    cmd, status);
            goto out;
        }
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

out:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "(note: VBIOS version %s)\n", pKernelVbios->vbiosVersionStr);
    }

    return status;
}
 
/*!
 * Execute a given FWSEC cmd and wait for completion.
 * KernelVbiosPreparedFwsecCmd should be set by kvbiosPrepareFwsecCmd and
 * not filled in manually
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelVbios   KernelGsp pointer
 * @param[in]   pPreparedCmd   Prepared command state from kvbiosPrepareFwsecCmd
 */
NV_STATUS
kvbiosExecuteFwsecCmd_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    KernelVbiosPreparedFwsecCmd *pPreparedCmd
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pPreparedCmd != NULL, NV_ERR_INVALID_ARGUMENT);

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_ERR_GPU_IN_FULLCHIP_RESET;

    status = kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp, pPreparedCmd->pFwsecUcode,
                                     staticCast(pKernelGsp, KernelFalcon), NULL, NULL);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC cmd 0x%x: status 0x%x\n", pPreparedCmd->cmd, status);
        goto out;
    }

    if (pPreparedCmd->cmd == FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS)
    {
        NvU32 data;
        NvU32 frtsErrCode;
        NvU32 wpr2HiVal;
        NvU32 wpr2LoVal;
        NvU32 expectedLoVal;

        data = GPU_REG_RD32(pGpu, NV_PBUS_VBIOS_SCRATCH(NV_VBIOS_FWSECLIC_SCRATCH_INDEX_0E));
        frtsErrCode = DRF_VAL(_VBIOS, _FWSECLIC, _FRTS_ERR_CODE, data);
        if (frtsErrCode != NV_VBIOS_FWSECLIC_FRTS_ERR_CODE_NONE)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: FRTS error code 0x%x\n", frtsErrCode);
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
        wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
        if (wpr2HiVal == 0)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: no initialized WPR2 found\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_LO);
        wpr2LoVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_LO, _VAL, data);
        expectedLoVal = (NvU32) (pPreparedCmd->frtsOffset >> NV_PFB_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT);
        if (wpr2LoVal != expectedLoVal)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed to execute FWSEC for FRTS: WPR2 initialized at an unexpected location: 0x%08x (expected 0x%08x)\n",
                    wpr2LoVal, expectedLoVal);
            status = NV_ERR_GENERIC;
            goto out;
        }
    }
    else if (pPreparedCmd->cmd == FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB)
    {
        NvU32 data;
        NvU32 sbErrCode;

        if (!GPU_FLD_TEST_DRF_DEF(pGpu, _PGC6, _AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK,
                                _READ_PROTECTION_LEVEL0, _ENABLE))
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: GFW PLM not lowered\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        if (!GPU_FLD_TEST_DRF_DEF(pGpu, _PGC6, _AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT,
                                _PROGRESS, _COMPLETED))
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: GFW progress not completed\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PBUS_VBIOS_SCRATCH(NV_VBIOS_FWSECLIC_SCRATCH_INDEX_15));
        sbErrCode = DRF_VAL(_VBIOS, _FWSECLIC, _SB_ERR_CODE, data);
        if (sbErrCode != NV_VBIOS_FWSECLIC_SB_ERR_CODE_NONE)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: SB error code 0x%x\n", sbErrCode);
            status = NV_ERR_GENERIC;
            goto out;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC cmd 0x%x: not supported\n", pPreparedCmd->cmd);
        status = NV_ERR_NOT_SUPPORTED;
        goto out;
    }

out:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "(note: VBIOS version %s)\n", pKernelVbios->vbiosVersionStr);
    }

    return status;
}

/*!
 * Execute FWSEC FRTS ucode to setup FRTS if needed
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelVbios   KernelVbios pointer
 * @param[in]   frtsOffset     Desired offset in FB of FRTS data and WPR2
 */
NV_STATUS
kvbiosExecuteFwsecFrtsCmd_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios,
    NvU64 frtsOffset
)
{
    KernelVbiosPreparedFwsecCmd preparedCmd = {0};

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvbiosPrepareFwsecCmd_HAL(pGpu, pKernelVbios,
        FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS, frtsOffset, &preparedCmd));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvbiosExecuteFwsecCmd_HAL(pGpu, pKernelVbios, &preparedCmd));

    return NV_OK;
}

/*!
 * Execute FWSEC SB ucode to setup FRTS
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelVbios   KernelVbios pointer
 */
NV_STATUS
kvbiosExecuteFwsecSbCmd_TU102
(
    OBJGPU *pGpu,
    KernelVbios *pKernelVbios
)
{
    KernelVbiosPreparedFwsecCmd preparedCmd = {0};

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvbiosPrepareFwsecCmd_HAL(pGpu, pKernelVbios,
        FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB, 0, &preparedCmd));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvbiosExecuteFwsecCmd_HAL(pGpu, pKernelVbios, &preparedCmd));

    return NV_OK;
}
