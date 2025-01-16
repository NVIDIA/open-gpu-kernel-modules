/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _IFRECC_H_
#define _IFRECC_H_

#include "nvtypes.h"
#include "inforom/types.h"

// NVSwitch ECC v6 object definition
#define INFOROM_ECC_OBJECT_V6_S0_PACKED_SIZE 3808

//Used to determine if the entry is empty or not
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_VALID                  0:0
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_VALID_FALSE              0
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_VALID_TRUE               1
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_CORRUPT_TIMEDATA       1:1
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_CORRUPT_TIMEDATA_FALSE   0
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_CORRUPT_TIMEDATA_TRUE    1
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_ADDR_VALID             2:2
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_ADDR_VALID_FALSE         0
#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_HEADER_ADDR_VALID_TRUE          1

#define NV_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_LOCATION_LINK_ID           7:0

typedef struct INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER
{
    inforom_U008 header;
    inforom_U032 errId;
    inforom_U032 lastErrorTimestamp;
    inforom_U032 averageEventDelta;
    inforom_U016 location;
    inforom_U016 sublocation;
    inforom_U032 correctedCount;
    inforom_U032 uncorrectedCount;
    inforom_U032 address;

} INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER;

#define INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT    128

typedef struct INFOROM_ECC_OBJECT_V6_S0
{
    INFOROM_OBJECT_HEADER_V1_00 header;

    NvU64_ALIGN32 uncorrectedTotal;
    NvU64_ALIGN32 correctedTotal;
    inforom_U032 lastClearedTimestamp;

    INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER errorEntries[INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT];

    inforom_U008 padding[68];
} INFOROM_ECC_OBJECT_V6_S0;

#define INFOROM_ECC_OBJECT_V6_S0_HEADER_FMT INFOROM_OBJECT_HEADER_V1_00_FMT

#define INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_FMT "b3d2w3d"

#define INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_ARRAY_FMT                                \
                        INFOROM_FMT_REP128(INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_FMT)

#define INFOROM_ECC_OBJECT_V6_S0_PADDING_FMT "68b"

#define INFOROM_ECC_OBJECT_V6_S0_FMT INFOROM_ECC_OBJECT_V6_S0_HEADER_FMT "qqd" \
                        INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_ARRAY_FMT       \
                        INFOROM_ECC_OBJECT_V6_S0_PADDING_FMT

// Error event structure for NVSwitch ECC errors
typedef struct
{
    NvU32 sxid;
    NvU32 linkId;
    NvBool bAddressValid;
    NvU32 address;

    // The timestamp is filled in by the inforom ECC error logging API
    NvU32 timestamp;

    NvBool bUncErr;
    NvU32  errorCount;
} INFOROM_NVS_ECC_ERROR_EVENT;

typedef union
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    INFOROM_ECC_OBJECT_V6_S0  v6s;
} INFOROM_ECC_OBJECT;

typedef struct
{
    const char          *pFmt;
    NvU8                *pPackedObject;
    INFOROM_ECC_OBJECT  *pEcc;

    // Signals if there are pending updates to be flushed to InfoROM
    NvBool bDirty;
} INFOROM_ECC_STATE, *PINFOROM_ECC_STATE;

#endif // _IFRECC_H_
