/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * KernelGsp functions and helpers for extracting a VBIOS image from
 * ROM.
 *
 * TODO: JIRA CORERM-4685: Consider moving stuff in here to, e.g. KernelVbios
 *
 * Note: Most functions here (other than those suffixed by a chip name)
 *       do not actually need to be HAL'd; we are simply keeping them all in
 *       one file to try to keep it self-contained.
 */

#include "gpu/gsp/kernel_gsp.h"
#include "gpu/bif/kernel_bif.h"

#include "platform/pci_exp_table.h" // PCI_EXP_ROM_*
#include "gpu/gpu.h"

#include "published/turing/tu102/dev_bus.h"  // for NV_PBUS_IFR_FMT_FIXED*
#include "published/turing/tu102/dev_ext_devices.h"  // for NV_PROM_DATA

#define NV_ROM_DIRECTORY_IDENTIFIER     0x44524652  // "RFRD"

#define NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_BASE   0x00
#define NV_BCRT_HASH_INFO_BASE_CODE_TYPE_VBIOS_EXT    0xE0

typedef struct RomImgSrc
{

    NvU32 baseOffset;
    NvU32 maxOffset;

    OBJGPU *pGpu;
} RomImgSrc;

/*!
 * Equivalent to GPU_REG_RD08(pGpu, NV_PROM_DATA(offset)), but use raw OS read
 * to avoid the 0xbadf sanity checking done by the usual register read
 * utilities.
 */
static inline NvU8
s_promRead08
(
    OBJGPU *pGpu,
    NvU32 offset
)
{
    return osDevReadReg008(pGpu, gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0),
                           NV_PROM_DATA(offset));
}

/*!
 * Equivalent to GPU_REG_RD32(pGpu, NV_PROM_DATA(offset)), but use raw OS read
 * to avoid the 0xbadf sanity checking done by the usual register read
 * utilities.
 */
static inline NvU32
s_promRead32
(
    OBJGPU *pGpu,
    NvU32 offset
)
{
    return osDevReadReg032(pGpu, gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0),
                           NV_PROM_DATA(offset));
}

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
    buf.word[0] = s_promRead32(pSrc->pGpu, offset);
    if (bReadWord1)
    {
        buf.word[1] = s_promRead32(pSrc->pGpu, offset + sizeof(NvU32));
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
static NV_STATUS
s_romImgFindPciHeader_TU102
(
    const RomImgSrc * const pSrc,
    NvU32 *pIfrSize
)
{

    NV_STATUS status = NV_OK;

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
 * Returns max size of VBIOS image in ROM
 * (including expansion ROMs).
 */
static NvU32
s_getBaseBiosMaxSize_TU102
(
    OBJGPU *pGpu
)
{
    return 0x100000;  // 1 MB
}

/*!
 * Extract VBIOS image from ROM.
 *
 * The resulting KernelGspVbiosImg should be freed with kgspFreeVbiosImg
 * after use.
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   pGpu        KernelGsp pointer
 * @param[out]  ppVbiosImg  Pointer to resulting KernelGspVbiosImg
 */
NV_STATUS
kgspExtractVbiosFromRom_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspVbiosImg **ppVbiosImg
)
{

    NV_STATUS status = NV_OK;

    KernelGspVbiosImg *pVbiosImg = NULL;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    RomImgSrc src;
    NvU32 romSig;
    NvU32 pciOffset = 0;

    NvU32 biosSize = s_getBaseBiosMaxSize_TU102(pGpu);
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
        NV_ASSERT_OK_OR_GOTO(status, s_romImgFindPciHeader_TU102(&src, &pciOffset), out);

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
            pImageDwords[i >> 2] = s_promRead32(pGpu, pciOffset + i);
        }

        for (; i < biosSize; i++)
        {
            // Finish for non-32-bit-aligned biosSize
            ((NvU8 *) pImageDwords)[i] = s_promRead08(pGpu, pciOffset + i);
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
        kgspFreeVbiosImg(pVbiosImg);
        pVbiosImg = NULL;
    }

    return status;
}
