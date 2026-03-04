/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INFOROM_OMSDEF_H_
#define _INFOROM_OMSDEF_H_

#include "inforom/types.h"

#define INFOROM_OMS_OBJECT_V1_PACKED_SIZE  112

#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_ENTRY_AVAILABLE          0:0
#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_ENTRY_AVAILABLE_NO         0
#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_ENTRY_AVAILABLE_YES        1
#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE     1:1
#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE_NO    0
#define INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE_YES   1

#define INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY_DATA_RESERVED               23:2
#define INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY_DATA_ENTRY_CHECKSUM        31:24

typedef struct INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY
{
    inforom_U032 data;
    inforom_U032 clockLimitMin;
    inforom_U032 clockLimitMax;
} INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY;

#define INFOROM_OMS_OBJECT_V1G_NUM_SETTINGS_ENTRIES 8

typedef struct INFOROM_OMS_OBJECT_V1G
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U032                lifetimeRefreshCount;
    INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY settings[
        INFOROM_OMS_OBJECT_V1G_NUM_SETTINGS_ENTRIES];
    inforom_U032                reserved;
} INFOROM_OMS_OBJECT_V1G;

#define INFOROM_OMS_OBJECT_V1G_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "26d"

#define INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY_DATA_RESERVED                7:2
#define INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY_DATA_ENTRY_CHECKSUM         15:8

typedef struct INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY
{
    inforom_U016 data;
} INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY;

#define INFOROM_OMS_OBJECT_V1S_NUM_SETTINGS_ENTRIES 50

typedef struct INFOROM_OMS_OBJECT_V1S
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U032                lifetimeRefreshCount;
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY settings[
        INFOROM_OMS_OBJECT_V1S_NUM_SETTINGS_ENTRIES];
} INFOROM_OMS_OBJECT_V1S;

#define INFOROM_OMS_OBJECT_V1S_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "d50w"

typedef struct INFOROM_OMS_V1S_DATA
{
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY *pIter;
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY  prev;
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY *pNext;
} INFOROM_OMS_V1S_DATA;

typedef struct INFOROM_OMS_V1G_DATA
{
    INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY *pIter;
    INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY  prev;
    INFOROM_OMS_OBJECT_V1G_SETTINGS_ENTRY *pNext;
} INFOROM_OMS_V1G_DATA;

typedef union
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    INFOROM_OMS_OBJECT_V1S v1s;
    INFOROM_OMS_OBJECT_V1G v1g;
} INFOROM_OMS_OBJECT;

typedef union
{
    INFOROM_OMS_V1S_DATA v1s;
    INFOROM_OMS_V1G_DATA v1g;
} INFOROM_OMS_DATA;

typedef struct
{
    const char         *pFmt;
    NvU8               *pPackedObject;
    INFOROM_OMS_OBJECT *pOms;
    INFOROM_OMS_DATA    omsData;
} INFOROM_OMS_STATE;

#endif /* _INFOROM_OMSDEF_H_ */
