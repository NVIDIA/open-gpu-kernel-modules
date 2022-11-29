/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NBSI_READ_H
#define NBSI_READ_H

#include "platform/nbsi/nbsi_table.h"

typedef enum _NBSI_VALIDATE
{
    NBSI_VALIDATE_ALL        = 0,          // do CRC and all normal tests
    NBSI_VALIDATE_IGNORE_CRC               // normal tests, skip CRC
} NBSI_VALIDATE;

typedef struct OBJGPU OBJGPU;

NvU16 fnv1Hash16(const NvU8 * data, NvU32 dataLen);
NvU16 fnv1Hash16Unicode(const NvU16 * data, NvU32 dataLen);
NvU32 fnv32buf(const void *buf, NvU32 len, NvU32 hval, NvU32 prevPartHashLen);
NvU32 fnv32bufUnicode(const void *buf, NvU32 len, NvU32 hval, NvU32 prevPartHashLen);
void fnv1Hash20Array(const NvU8 *, NvU32, NvU32 * , NvU8);
void fnv1Hash20ArrayUnicode(const NvU16 *, NvU32, NvU32 * , NvU8);
NvU64 fnv1Hash64(const NvU8 * data, NvU32 dataLen);
NvU32 getNbsiValue (OBJGPU *, NvU32, NvU16, NvU8, NvU32*, NvU8*, NvU32*, NvU32*);
NV_STATUS initNbsiTable(OBJGPU *);
void freeNbsiTable(OBJGPU *);
NV_STATUS nbsiReadRegistryDword(OBJGPU *, const char *, NvU32 *);
NV_STATUS nbsiReadRegistryString(OBJGPU *, const char *, NvU8 *, NvU32 *);
NV_STATUS getNbsiObjByType(OBJGPU *, NvU16, NBSI_SOURCE_LOC *, NvU8 *, NvU32, NvU8 *, NvU32*, NvU32*, NvU32*, ACPI_DSM_FUNCTION, NBSI_VALIDATE);
void initNbsiObject(NBSI_OBJ *pNbsiObj);
NBSI_OBJ *getNbsiObject(void);


/*
 * 32 bit magic FNV-0 and FNV-1 prime
 */
#define FNV_32_PRIME 0x01000193 // is 16777619.
#define MASK_20 0xfffff
#define MASK_16 0xffff
#define FNV1_32_INIT 0x811C9DC5 // is 2166136261.

// State (undefined, open, bad or not present) of the nbsiTable
#define NBSI_TABLE_UNDEFINED 0  // nbsi table initialization not called so
                                // nbsiTableptr unallocated
#define NBSI_TABLE_INIT 1       // nbsi table being opened not yet ready
#define NBSI_TABLE_OPEN 2       // nbsi table ready for business
#define NBSI_TABLE_BAD 3        // nbsi table structure bad
#define NBSI_TABLE_NOTPRESENT 4 // nbsi table is not present

#define NBSI_TABLE_FMT_UNKNOWN 0xff  // Unknown format for NBSI table

#define NBSI_REVISION_ID   0x00000101
#define NVHG_NBSI_REVISION_ID   0x00000101

#define NBSI_FUNC_PLATCAPS      0x00000001    // Platform NBSI capabilities
#define NBSI_FUNC_PLATPOLICY    0x00000002    // Query/Set Platform Policy
#define NBSI_FUNC_DISPLAYSTATUS 0x00000003    // Query the Display Hot-Key
#define NBSI_FUNC_MDTL          0x00000004    //
#define NBSI_FUNC_CALLBACKS     0x00000005    // Get Callbacks
#define NBSI_FUNC_GETOBJBYTYPE  0x00000006    // Get an Object by Type
#define NBSI_FUNC_GETALLOBJS    0x00000007    // Get Driver Object

// Return values for NBSI_FUNC_SUPPORT call.
#define NBSI_FUNC_PLATCAPS_SUPPORTED      NVBIT(1) // Platform NBSI capabilities
#define NBSI_FUNC_PLATPOLICY_SUPPORTED    NVBIT(2) // Query/Set Platform Policy
#define NBSI_FUNC_DISPLAYSTATUS_SUPPORTED NVBIT(3) // Query the Display Hot-Key
#define NBSI_FUNC_MDTL_SUPPORTED          NVBIT(4) //
#define NBSI_FUNC_CALLBACKS_SUPPORTED     NVBIT(5) // Get Callbacks
#define NBSI_FUNC_GETOBJBYTYPE_SUPPORTED  NVBIT(6) // Get an Object by Type
#define NBSI_FUNC_GETALLOBJS_SUPPORTED    NVBIT(7) // Get Driver Object

#define NBSI_READ_SIZE          (4*1024)      // 4K as per spec
#define NBSI_INIT_TABLE_ALLOC_SIZE  NBSI_READ_SIZE // must be larger than
                                              // NBSI_READ_SIZE

#define NBPB_FUNC_SUPPORT           0x00000000  // Function is supported?
#define NBPB_FUNC_GETOBJBYTYPE      0x00000010  // Fetch any specific Object by Type
#define NBPB_FUNC_GETALLOBJS        0x00000011  // Fetch all Objects
#define NBPB_FUNC_GETTHERMALBUDGET  0x00000020  // Get the current thermal budget
#define NBPB_FUNC_GETPSS            0x00000021  // Get the PSS table
#define NBPB_FUNC_SETPPC            0x00000022  // Set p-State Cap
#define NBPB_FUNC_GETPPC            0x00000023  // Get the last p-State cap set with _FUNC_SETPPC

#endif // NBSI_READ_H
