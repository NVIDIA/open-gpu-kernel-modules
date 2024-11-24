/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _IFRSTRUCT_H_
#define _IFRSTRUCT_H_

#include "inforom/types.h"

#include "inforom/ifrnvl.h"
#include "inforom/ifrecc.h"
#include "inforom/ifrdem.h"
#include "inforom/omsdef.h"

#define INFOROM_OBD_OBJECT_V1_XX_PACKED_SIZE 128

struct INFOROM_OBD_OBJECT_V1_XX
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U032 buildDate;
    inforom_U008 marketingName[24];
    inforom_U008 serialNumber[16];
    inforom_U008 memoryManufacturer;
    inforom_U008 memoryPartID[20];
    inforom_U008 memoryDateCode[6];
    inforom_U008 productPartNumber[20];
    inforom_U008 boardRevision[3];
    inforom_U008 boardType;
    inforom_U008 board699PartNumber[20];
    inforom_U008 reserved[5];
};
#define INFOROM_OBD_OBJECT_V1_XX_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "d116b"
typedef struct INFOROM_OBD_OBJECT_V1_XX INFOROM_OBD_OBJECT_V1_XX;

#define INFOROM_OBD_OBJECT_V2_XX_PACKED_SIZE 224

struct INFOROM_OBD_OBJECT_V2_XX
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U032 buildDate;
    inforom_U008 marketingName[24];
    inforom_U008 serialNumber[16];
    inforom_U008 memoryManufacturer;
    inforom_U008 memoryPartID[20];
    inforom_U008 memoryDateCode[6];
    inforom_U008 productPartNumber[20];
    inforom_U008 boardRevision[3];
    inforom_U008 boardType;
    inforom_U008 board699PartNumber[20];
    inforom_U032 productLength;
    inforom_U032 productWidth;
    inforom_U032 productHeight;
    inforom_U008 vbiosFactoryVersion[5];
    inforom_U008 board965PartNumber[20];
    inforom_U008 reserved[64];
};
#define INFOROM_OBD_OBJECT_V2_XX_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "d111b3d89b"
typedef struct INFOROM_OBD_OBJECT_V2_XX INFOROM_OBD_OBJECT_V2_XX;

//
// OEM 1.0
//

#define INFOROM_OEM_OBJECT_V1_00_PACKED_SIZE 512
#define INFOROM_OEM_OBJECT_HEADER_VERSION   1

struct INFOROM_OEM_OBJECT_V1_00
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U008 oemInfo[504];
};
#define INFOROM_OEM_OBJECT_V1_00_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "504b"
typedef struct INFOROM_OEM_OBJECT_V1_00 INFOROM_OEM_OBJECT_V1_00;

#define INFOROM_IMG_OBJECT_V1_00_PACKED_SIZE                                 64
#define INFOROM_IMG_OBJECT_V1_00_VERSION_LENGTH                              16

struct INFOROM_IMG_OBJECT_V1_00
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U008 version[INFOROM_IMG_OBJECT_V1_00_VERSION_LENGTH];
    inforom_U016 pciDeviceId;
    inforom_U016 pciVendorId;
    inforom_U016 pciSubsystemId;
    inforom_U016 pciSubsystemVendorId;
    inforom_U008 reserved[32];
};
#define INFOROM_IMG_OBJECT_V1_00_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "16b4w32b"
typedef struct INFOROM_IMG_OBJECT_V1_00 INFOROM_IMG_OBJECT_V1_00;

#endif // _IFRSTRUCT_H_
