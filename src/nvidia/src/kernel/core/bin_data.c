/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * @brief Bindata APIs implememtation
 */

#include "core/bin_data.h"
#include "bin_data_pvt.h"
#include "os/os.h"
#include "nvRmReg.h"
#include "gpu_mgr/gpu_mgr.h"

/*
 * Private helper functions
 */
static NV_STATUS   _bindataWriteStorageToBuffer(const BINDATA_STORAGE *pBinStorage, NvU8 *pBuffer, NvU32 bufferSize);

/*!
 * Helper function to get appropriate pointer to bindata, depending on whether it was loaded
 * from disk or compiled with the driver
 */
static const NvU8 *_bindataGetBindataPtr(const BINDATA_STORAGE_PVT *pBinStoragePvt)
{
    return pBinStoragePvt->pData;
}

/*!
 * Retrieve data from Bindata storage and write it to the given memory buffer.  When
 * file overriding feature is enabled and the file exists in the target directory,
 * copy the file to buffer instead.
 *
 * @param[in]   pBinStorage     Bindata storage
 * @param[in]   pBuffer         Pointer of given buffer
 * @param[in]   bufferSize      Size of given buffer
 *
 * @return      'NV_OK'         If the ucode was written to memory buffer successfully
 */
NV_STATUS
bindataWriteToBuffer
(
    const BINDATA_STORAGE *pBinStorage,
    NvU8                  *pBuffer,
    NvU32                  bufferSize
)
{
    // paged memory access check
    osPagedSegmentAccessCheck();

    return _bindataWriteStorageToBuffer(pBinStorage, pBuffer, bufferSize);
}

/*!
 * Retrieve data from Bindata storage and write it to the given memory buffer.
 *
 * @param[in]   pBinStorage     Bindata storage
 * @param[in]   pBuffer         Pointer of given buffer
 * @param[in]   bufferSize      Size of given buffer
 *
 * @return      'NV_OK'         If the ucode was written to memory buffer successfully
 */
NV_STATUS
_bindataWriteStorageToBuffer
(
    const BINDATA_STORAGE *pBinStorage,
    NvU8                  *pBuffer,
    NvU32                  bufferSize
)
{
    const BINDATA_STORAGE_PVT *pBinStoragePvt = (const BINDATA_STORAGE_PVT *) pBinStorage;
    NvU32                      nBytesInflated;
    const NvU8                *pData;

    NV_ASSERT_OR_RETURN(pBinStorage != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_ARGUMENT);
    // resource data should never be NULL
    NV_ASSERT_OR_RETURN(pBinStoragePvt->pData != NULL, NV_ERR_INVALID_ARGUMENT);

    pData = _bindataGetBindataPtr(pBinStoragePvt);

    if (pBinStoragePvt->bCompressed)
    {
        if ((nBytesInflated = utilGzGetData(pData,
                                            bufferSize,
                                            0,
                                            bufferSize,
                                            pBuffer)) != bufferSize)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to get inflated data, got %u bytes, expecting %u\n",
                      nBytesInflated, bufferSize);
            DBG_BREAKPOINT();
            return NV_ERR_INFLATE_COMPRESSED_DATA_FAILED;
        }
    }
    else
    {
        portMemCopy(pBuffer, bufferSize, pData, bufferSize);
    }

    return NV_OK;
}


NvU32
bindataGetBufferSize
(
    const BINDATA_STORAGE *pBinStorage
)
{
    // paged memory access check
    osPagedSegmentAccessCheck();

    if (pBinStorage == NULL)
    {
        return 0;
    }

    return ((const BINDATA_STORAGE_PVT *) pBinStorage)->actualSize;
}


/*!
 * Get Bindata storage from the given Bindata Archive
 *
 * @param[in]   pBinArchive        Pointer to Bindata Archive
 * @param[in]   binName            Name of the bindata
 *
 * @return      Pointer of the target Bindata Storage or
 *              NULL if cannot find the target of the given name
 */
const BINDATA_STORAGE *
bindataArchiveGetStorage(
    const BINDATA_ARCHIVE *pBinArchive,
    BINDATA_LABEL bindataLabel
)
{
    // paged memory access check
    osPagedSegmentAccessCheck();

    if (pBinArchive == NULL)
    {
        return NULL;
    }

    NvU32 i;
    for (i = 0 ; i < pBinArchive->entryNum; i++)
    {
        if (bindataLabel == pBinArchive->entries[i].bindataLabel)
        {
            bindataMarkReferenced(pBinArchive->entries[i].pBinStorage);
            return pBinArchive->entries[i].pBinStorage;
        }
    }
    return NULL;
}

void bindataInitialize(void)
{
}

void bindataDestroy(void)
{
}

/*!
 * Enable zero copy of bindata. If executed on GSP and if the bindata section is uncompressed,
 * populate ppData with the location in the elf file.
 * Otherwise, allocate buffers in heap and copy the bindata.
 * Assumes pBinstorage and ppData are valid
 *
 * @param[out]  ppData             pointer to elf section/allocated buffer
 * @param[in]   pBinStorage        Pointer to Bindata Storage
 *
 * @return      NV_OK                if operation is successful
 *              NV_ERR_NO_MEMORY     if buffer allocation fails
 */
NV_STATUS bindataStorageAcquireData(
    const BINDATA_STORAGE *pBinStorage,
    const void **ppData
)
{
    NV_STATUS status = NV_OK;

    {
        NvU32 bufferSize = bindataGetBufferSize(pBinStorage);
        *ppData = portMemAllocNonPaged(bufferSize);

        if (*ppData == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "bindata memory alloc failed\n");
            return NV_ERR_NO_MEMORY;
        }

        status = bindataWriteToBuffer(pBinStorage, (NvU8 *)*ppData, bufferSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "bindataWriteToBuffer failed. Freeing alloced memory, return code %u\n", status);
            portMemFree((void*)*ppData);
            *ppData = NULL;
            return status;
        }
    }
    return status;
}

/*!
 * Skip calling free on unallocated bindata zero copy pointers
 * calls free in non gsp cases
 * @param[in]   pData  Pointer to be freed
 *
 */
void bindataStorageReleaseData(
    void *pData
)
{
    {
        portMemFree(pData);
    }
}

// File Overriding Feature is only enabled under MODS

void bindataMarkReferenced(const BINDATA_STORAGE *pBinStorage)
{
    if (BINDATA_IS_MUTABLE)
    {
        // Cast away the constness
        BINDATA_STORAGE_PVT *pMutablePvt = (BINDATA_STORAGE_PVT *)pBinStorage;
        NV_ASSERT(pMutablePvt->pData != NULL || pMutablePvt->actualSize != 0);
        pMutablePvt->bReferenced = NV_TRUE;
    }
}

void* bindataGetNextUnreferencedStorage(NvU32 *pIdx, NvU32 *pDataSize)
{
    extern BINDATA_STORAGE_PVT g_bindata_pvt[];
    extern const NvU32 g_bindata_pvt_count;


    NV_ASSERT_OR_RETURN((pIdx != NULL), NULL);

    // This API makes no sense if the data is const, so just bail out early.
    NV_ASSERT_OR_RETURN(BINDATA_IS_MUTABLE, NULL);

    // Note:    idx passed in from caller will be initialized to 0.
    //          Valid index for g_bindata_pvt starts from 1
    (*pIdx)++;
    while (*pIdx < g_bindata_pvt_count)
    {
        if (!g_bindata_pvt[*pIdx].bReferenced && g_bindata_pvt[*pIdx].pData != NULL)
        {
            *pDataSize = g_bindata_pvt[*pIdx].compressedSize;
            return (void*)g_bindata_pvt[*pIdx].pData;
        }
        (*pIdx)++;
    }

    *pDataSize = 0;
    return NULL;
}

void bindataDestroyStorage(NvU32 idx)
{
    extern BINDATA_STORAGE_PVT g_bindata_pvt[];
    g_bindata_pvt[idx].pData = NULL;
    g_bindata_pvt[idx].actualSize = 0;
    g_bindata_pvt[idx].compressedSize = 0;
}
