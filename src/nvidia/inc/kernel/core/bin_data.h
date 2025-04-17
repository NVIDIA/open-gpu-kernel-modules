/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _BINDATA_H
#define _BINDATA_H

#include "core/core.h"

/**************************************************************************************************************
*
*    File:  bindata.h
*
*    Description:
*        Bindata management APIs
*
**************************************************************************************************************/

//
// Public interface for accessing the acquired binary data
//

typedef enum {
    BINDATA_LABEL_HEADER_DBG,
    BINDATA_LABEL_HEADER_PROD,
    BINDATA_LABEL_HS_SIG_PATCH_METADATA,
    BINDATA_LABEL_IMAGE_DBG,
    BINDATA_LABEL_IMAGE_PROD,
    BINDATA_LABEL_LC128_DBG,
    BINDATA_LABEL_LC128_PROD,
    BINDATA_LABEL_NUM_SIGS,
    BINDATA_LABEL_NUM_SIGS_PER_UCODE_DBG,
    BINDATA_LABEL_NUM_SIGS_PER_UCODE_PROD,
    BINDATA_LABEL_PATCH_LOC,
    BINDATA_LABEL_PATCH_META,
    BINDATA_LABEL_PATCH_SIG,
    BINDATA_LABEL_SIG_DBG,
    BINDATA_LABEL_SIG_PROD,
    BINDATA_LABEL_UCODE_DATA,
    BINDATA_LABEL_UCODE_DATA_DBG,
    BINDATA_LABEL_UCODE_DATA_PROD,
    BINDATA_LABEL_UCODE_DESC,
    BINDATA_LABEL_UCODE_DESC_DBG,
    BINDATA_LABEL_UCODE_DESC_PROD,
    BINDATA_LABEL_UCODE_FALCON_HEADER_DBG,
    BINDATA_LABEL_UCODE_FALCON_IMAGE_DBG,
    BINDATA_LABEL_UCODE_HASH,
    BINDATA_LABEL_UCODE_HEADER,
    BINDATA_LABEL_UCODE_HEADER_DBG,
    BINDATA_LABEL_UCODE_HEADER_PROD,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_DBG,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_INDEX_DBG,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_INDEX_PROD,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_PATCH_INDEX_DBG,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_PATCH_INDEX_PROD,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_PATCH_METADATA_DBG,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_PATCH_METADATA_PROD,
    BINDATA_LABEL_UCODE_HS_OVL_SIG_PROD,
    BINDATA_LABEL_UCODE_IMAGE,
    BINDATA_LABEL_UCODE_IMAGE_DBG,
    BINDATA_LABEL_UCODE_IMAGE_DESC,
    BINDATA_LABEL_UCODE_IMAGE_PROD,
    BINDATA_LABEL_UCODE_LSF_DESC,
    BINDATA_LABEL_UCODE_LSF_DESC_DBG,
    BINDATA_LABEL_UCODE_LSF_DESC_PROD,
    BINDATA_LABEL_UCODE_LSF_LSB_HEADER,
    BINDATA_LABEL_UCODE_MANIFEST,
    BINDATA_LABEL_UCODE_MANIFEST_DBG,
    BINDATA_LABEL_UCODE_MANIFEST_PROD,
    BINDATA_LABEL_UCODE_NUM_SIGS_PER_UCODE,
    BINDATA_LABEL_UCODE_NUM_SIGS_PER_UCODE_DBG,
    BINDATA_LABEL_UCODE_NUM_SIGS_PER_UCODE_PROD,
    BINDATA_LABEL_UCODE_PATCH_LOC,
    BINDATA_LABEL_UCODE_PATCH_SIG,
    BINDATA_LABEL_UCODE_PKEY,
    BINDATA_LABEL_UCODE_RISCV_DESC_DBG,
    BINDATA_LABEL_UCODE_RISCV_IMAGE_DBG,
    BINDATA_LABEL_UCODE_RISCV_NO_MANIFEST_DESC_DBG,
    BINDATA_LABEL_UCODE_RISCV_NO_MANIFEST_IMAGE_DBG,
    BINDATA_LABEL_UCODE_SIG,
    BINDATA_LABEL_UCODE_SIG_DBG,
    BINDATA_LABEL_UCODE_SIG_PATCH_IND,
    BINDATA_LABEL_UCODE_SIG_PATCH_LOC,
    BINDATA_LABEL_UCODE_SIG_PATCH_METADATA,
    BINDATA_LABEL_UCODE_SIG_PROD,
    BINDATA_LABEL_UCODE_SIG_PATCH_SIG,
    BINDATA_LABEL_LC128_PATCH_LOC_PROD,
    BINDATA_LABEL_LC128_PATCH_LOC_DBG,
    BINDATA_LABEL_CERTIFICATE_PEM,
    BINDATA_LABEL_CERTIFICATE_DER,
} BINDATA_LABEL;

//
// Binary data access handler
//
typedef struct BINDATA_RUNTIME_INFO BINDATA_RUNTIME_INFO, *PBINDATA_RUNTIME_INFO;

//
// Public binary storage information
//
struct BINDATA_STORAGE;         // currently no public data fields
typedef struct BINDATA_STORAGE BINDATA_STORAGE, *PBINDATA_STORAGE;


//
// Primitives
//
NV_STATUS bindataAcquire(const BINDATA_STORAGE *pBinStorage, PBINDATA_RUNTIME_INFO *ppBinInfo);
NV_STATUS bindataGetNextChunk(PBINDATA_RUNTIME_INFO pBinInfo, NvU8 *pBuffer, NvU32 nBytes);
void      bindataRelease(PBINDATA_RUNTIME_INFO pBinInfo);


//
// Utilities
//
NV_STATUS bindataWriteToBuffer(const BINDATA_STORAGE *pBinStorage, NvU8 *pBuffer, NvU32 bufferSize);
NvU32     bindataGetBufferSize(const BINDATA_STORAGE *pBinStorage);
NV_STATUS bindataStorageAcquireData(const BINDATA_STORAGE *pBinStorage, const void **ppData);
void bindataStorageReleaseData(void *pData);

void bindataInitialize(void);
void bindataDestroy(void);

//
// Bindata Archive support
//
typedef struct
{
    BINDATA_LABEL            bindataLabel;
    const PBINDATA_STORAGE   pBinStorage;         // pointer to the binary storage
} BINDATA_ARCHIVE_ENTRY;

typedef struct
{
    NvU32 entryNum;
    BINDATA_ARCHIVE_ENTRY entries[];
} BINDATA_ARCHIVE;


// Bindata Archive API - get Bindata storage from a Bindata Archive
const BINDATA_STORAGE * bindataArchiveGetStorage(const BINDATA_ARCHIVE *pBinArchive, BINDATA_LABEL bindataLabel);

//
// Iterate over all BINDATA_STORAGE entries that have not been referenced so far
// Returns the pointer to unreferenced data or NULL if no more are available.
// Example usage:
//    const BINDATA_STORAGE *iter = NULL;
//    void *datablock;
//    NvU32 size;
//    while ((datablock = bindataGetNextUnreferencedStorage(&iter, &size))) {
//        do_stuff(datablock, size);
//    }
//
void* bindataGetNextUnreferencedStorage(NvU32 *pIdx, NvU32 *pDataSize);
//
// Marks a given BINDATA_STORAGE as destroyed, making all subsequent attempts
// to access it fail and return NULL/0
//
void bindataDestroyStorage(NvU32 idx);

#endif // _BINDATA_H
