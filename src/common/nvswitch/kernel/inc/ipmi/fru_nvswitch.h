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

#ifndef _FRU_NVSWITCH_H_
#define _FRU_NVSWITCH_H_

#include "common_nvswitch.h"

//
// FRU EEPROM board data  
// Defined according to
// ipmi-platform-mgt-fru-infostorage-def-v1-0-rev-1-3-spec
//
#define NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_TYPE                7:6
#define NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_LENGTH              5:0
#define NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_TYPE_ASCII_6BIT     (0x2)
#define NVSWITCH_IPMI_FRU_TYPE_LENGTH_BYTE_TYPE_ASCII_8BIT     (0x3)
#define NVSWITCH_IPMI_FRU_SENTINEL                             (0xC1)

// this includes null term
#define NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN             64

// mfgDateTime is in minutes from 0:00 hrs 1/1/1996
typedef struct
{
    NvU32 mfgDateTime; 
    char mfg[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
    char productName[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
    char serialNum[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
    char partNum[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
    char fileId[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
    char customMfgInfo[NVSWITCH_IPMI_FRU_BOARD_INFO_FIELD_MAX_LEN];
} NVSWITCH_IPMI_FRU_BOARD_INFO;

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_IPMI_FRU_EEPROM_COMMON_HEADER, 1)
{
    NvU8 version;
    NvU8 internalUseOffset;
    NvU8 chassisInfoOffset;
    NvU8 boardInfoOffset;
    NvU8 productInfoOffset;
    NvU8 multirecordOffset;
    NvU8 padding;
    NvU8 checksum;
} NVSWITCH_IPMI_FRU_EEPROM_COMMON_HEADER;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

//
// Board Info area will be (size * 8) bytes. The last byte is a checksum byte
//
NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_IPMI_FRU_EEPROM_BOARD_INFO, 1)
{
    NvU8 version;
    NvU8 size;
    NvU8 languageCode;
    NVSWITCH_IPMI_FRU_BOARD_INFO boardInfo; // True size in rom could be smaller, layout will be different
} NVSWITCH_IPMI_FRU_EEPROM_BOARD_INFO;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NvlStatus nvswitch_read_partition_fru_board_info(nvswitch_device *device,
                                                 NVSWITCH_IPMI_FRU_BOARD_INFO *pBoardInfo,
                                                 NvU8 *pRomImage);

#endif //_FRU_NVSWITCH_H_
