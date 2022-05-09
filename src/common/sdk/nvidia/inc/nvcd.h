/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2002 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVCD_H
#define NVCD_H

//******************************************************************************
//
// Module Name: NVCD.H
//
// This file contains structures and constants that define the NV specific
// data to be returned by the miniport's new VideoBugCheckCallback. The callback
// can return up to 4k bytes of data that will be appended to the dump file.
// The bugcheck callback is currently only invoked for bugcheck 0xEA failures.
// The buffer returned contains a top level header, followed by a variable
// number of data records. The top level header contains an ASCII signature
// that can be located with a search as well as a GUID for unique identification
// of the crash dump layout, i.e. future bugcheck callbacks can define a new
// GUID to redefine the entire crash dump layout. A checksum and crash dump
// size values are also included to insure crash dump data integrity. The
// data records each contain a header indicating what group the data belongs to
// as well as the actual record type and size. This flexibility allows groups
// to define and extend the information in their records without adversely
// affecting the code in the debugger extension that has to parse and display
// this information. The structures for these individual data records are
// contained in separate header files for each group.
//
//******************************************************************************
#include "nvtypes.h"

// Define the GUID type for non-Windows OSes

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    NvU32   Data1;
    NvU16   Data2;
    NvU16   Data3;
    NvU8    Data4[8];
} GUID, *LPGUID;
#endif

// Define the crash dump ASCII tag value and the dump format GUIDs
#define NVCD_SIGNATURE      0x4443564E  /* ASCII crash dump signature "NVCD" */

#define GUID_NVCD_DUMP_V1   { /* e3d5dc6e-db7d-4e28-b09e-f59a942f4a24 */    \
                            0xe3d5dc6e, 0xdb7d, 0x4e28,                     \
                            {0xb0, 0x9e, 0xf5, 0x9a, 0x94, 0x2f, 0x4a, 0x24}\
};
#define GUID_NVCD_DUMP_V2   { /* cd978ac1-3aa1-494b-bb5b-e93daf2b0536 */    \
                            0xcd978ac1, 0x3aa1, 0x494b,                     \
                            {0xbb, 0x5b, 0xe9, 0x3d, 0xaf, 0x2b, 0x05, 0x36}\
};
#define GUID_NVCDMP_RSVD1   { /* 391fc656-a37c-4574-8d57-b29a562f909b */    \
                            0x391fc656, 0xa37c, 0x4574,                     \
                            {0x8d, 0x57, 0xb2, 0x9a, 0x56, 0x2f, 0x90, 0x9b}\
};
#define GUID_NVCDMP_RSVD2   { /* c6d9982d-1ba9-4f80-badd-3dc992d41b46 */    \
                            0xc6d9982d, 0x1ba9, 0x4f80,                     \
                            {0xba, 0xdd, 0x3d, 0xc9, 0x92, 0xd4, 0x1b, 0x46}\
};

// RC 2.0 NVCD (NV crash dump) GUID
#define GUID_NVCD_RC2_V1    {  /* d3793533-a4a6-46d3-97f2-1446cfdc1ee7 */   \
                            0xd3793533, 0xa4a6, 0x46d3,                     \
                            {0x97, 0xf2, 0x14, 0x46, 0xcf, 0xdc, 0x1e, 0xe7}\
};


// Define NVIDIA crash dump header structure (First data block in crash dump)
typedef struct
{
    NvU32   dwSignature;            // ASCII crash dump signature "NVCD"
    GUID    gVersion;               // GUID for crashdump file (Version)
    NvU32   dwSize;                 // Size of the crash dump data
    NvU8    cCheckSum;              // Crash dump checksum (Zero = ignore)
    NvU8    cFiller[3];             // Filler (Possible CRC value)
} NVCD_HEADER;
typedef NVCD_HEADER *PNVCD_HEADER;

// Define the crash dump record groups
typedef enum
{
    NvcdGroup               = 0,    // NVIDIA crash dump group (System NVCD records)
    RmGroup                 = 1,    // Resource manager group (RM records)
    DriverGroup             = 2,    // Driver group (Driver/miniport records)
    HardwareGroup           = 3,    // Hardware group (Hardware records)
    InstrumentationGroup    = 4,    // Instrumentation group (Special records)
} NVCD_GROUP_TYPE;

// Define the crash dump group record types (Single end of data record type)
typedef enum
{
    EndOfData               = 0,    // End of crash dump data record
    CompressedDataHuffman   = 1,    // Compressed huffman data 
} NVCD_RECORD_TYPE;

// Define the crash dump data record header
typedef struct
{
    NvU8    cRecordGroup;           // Data record group (NVCD_GROUP_TYPE)
    NvU8    cRecordType;            // Data record type (See group header)
    NvU16   wRecordSize;            // Size of the data record in bytes
} NVCD_RECORD;
typedef NVCD_RECORD *PNVCD_RECORD;

// Define the EndOfData record structure
typedef struct
{
    NVCD_RECORD Header;             // End of data record header
} EndOfData_RECORD;
typedef EndOfData_RECORD *PEndOfData_RECORD;

//
// Generic mini-record type (keep the size at 64bits)
//
typedef struct
{
    NVCD_RECORD     Header;         // header for mini record
    NvU32           Payload;        // 32 bit payload value
} NVCDMiniRecord;
typedef NVCDMiniRecord *PNVCDMiniRecord;

//
// Generic record collection type
// 
typedef struct
{
    NVCD_RECORD     Header;         // generic header to binary type this in OCA buffer
                                    // size is actual size of this struct + all items in collection
    NvU32           NumRecords;     // number of records this collection contain
    NVCD_RECORD     FirstRecord;    // first record, its data follow
} NVCDRecordCollection;
typedef NVCDRecordCollection *PNVCDRecordCollection;

#define COLL_HEADER_SIZEOF (sizeof(NVCDRecordCollection) - sizeof(NVCD_RECORD))


#endif  // NVCD_H
