/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "ipmi/fru_nvswitch.h"

#define ASCII_6BIT_TO_8BIT(b)                                  ((b) + 0x20) 

#define OFFSET_SCALE                                           (8)

static NvU8
_nvswitch_calculate_checksum
(
    NvU8 *data,
    NvU32 size
)
{
    NvU32 i;
    NvU8 checksum = 0;

    for (i = 0; i < size; ++i)
    {
        checksum += data[i];
    }
    return checksum;
}

/*
 * @brief Retieves from bytes from src and stores into dest.
 *
 * @return The size of the field including the type/length byte. 
 */
static NvU8
_nvswitch_get_field_bytes
(
    NvU8 *pFieldSrc,
    NvU8 *pFieldDest
)
{
    NvU32 i;
    NvU8 type;
    NvU8 length;
    NvU8 byte;

    if (*pFieldSrc == NVSWITCH_IPMI_FRU_SENTINEL)
    {
        return 0;
    }

    type = DRF_VAL(SWITCH_IPMI, _FRU_TYPE_LENGTH_BYTE, _TYPE, *pFieldSrc);
    length = DRF_VAL(SWITCH_IPMI, _FRU_TYPE_LENGTH_BYTE, _LENGTH, *pFieldSrc);

    pFieldSrc++;

    for (i = 0; i < length; ++i)
    {
        switch (type)
        {
            case NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_TYPE_ASCII_6BIT:
                byte = ASCII_6BIT_TO_8BIT(pFieldSrc[i]);
                break;
            case NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_TYPE_ASCII_8BIT:
                byte = pFieldSrc[i];
                break;
            default:
                byte = 0;
                break;
        }
        pFieldDest[i] = byte;
    }

    return (length + 1);
}

/*
 * @brief Parse FRU board info from the given rom image.
 *
 * @return NVL_SUCCESS if board field is valid
 */
NvlStatus
nvswitch_read_partition_fru_board_info
(
    nvswitch_device *device,
    NVSWITCH_IPMI_FRU_BOARD_INFO *pBoardInfo,
    NvU8 *pRomImage
)
{
    NVSWITCH_IPMI_FRU_EEPROM_COMMON_HEADER *pEepromHeader;
    NVSWITCH_IPMI_FRU_EEPROM_BOARD_INFO *pEepromBoardInfo;
    NvU8 *pInfoSrc;

    if (pBoardInfo == NULL || pRomImage == NULL)
    {
        return -NVL_ERR_GENERIC; 
    }
    pEepromHeader = (NVSWITCH_IPMI_FRU_EEPROM_COMMON_HEADER *)pRomImage;
    
    // zero checksum
    if (_nvswitch_calculate_checksum((NvU8 *)pEepromHeader, 
        sizeof(NVSWITCH_IPMI_FRU_EEPROM_COMMON_HEADER)) != 0)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Common header checksum error.\n", __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    pEepromBoardInfo = (NVSWITCH_IPMI_FRU_EEPROM_BOARD_INFO *)(pRomImage + 
                           (pEepromHeader->boardInfoOffset * OFFSET_SCALE));

    if (_nvswitch_calculate_checksum((NvU8 *)pEepromBoardInfo, 
        pEepromBoardInfo->size * OFFSET_SCALE) != 0)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Board info checksum error.\n", __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    if (pEepromBoardInfo->version != 0x1 || pEepromBoardInfo->languageCode != 0x0)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvswitch_os_memset(pBoardInfo, 0, sizeof(NVSWITCH_IPMI_FRU_BOARD_INFO));

    pInfoSrc = (NvU8 *)&pEepromBoardInfo->boardInfo;

    // LS byte first
    pBoardInfo->mfgDateTime = pInfoSrc[0] | (pInfoSrc[1] << 8) | (pInfoSrc[2] << 16);
    pInfoSrc += 3;
    
    pInfoSrc += _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->mfg);
    pInfoSrc += _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->productName);
    pInfoSrc += _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->serialNum);
    pInfoSrc += _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->partNum);
    pInfoSrc += _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->fileId);
    _nvswitch_get_field_bytes(pInfoSrc, (NvU8 *)pBoardInfo->customMfgInfo);

    return NVL_SUCCESS;
}
