/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * Private helper functions
 */
static NV_STATUS   _bindataWriteStorageToBuffer(const BINDATA_STORAGE *pBinStorage, NvU8 *pBuffer);


/*!
 * Initialize a BINDATA_RUNTIME_INFO structure for use, this function does not allocate any
 * memory for data storage, only the data structure itself.
 *
 * @param[in]   pBinStorage      The BINDATA_STORAGE structure related to this
 *                               binary resource.
 *
 * @param[out]  ppBinInfo        Location where the prepared BINDATA_RUNTIME_INFO data structure
 *                               will be stored to.
 *
 * @return      'NV_OK'          If all initialization operations were successful.
 *
 */
NV_STATUS
bindataAcquire
(
    const BINDATA_STORAGE *pBinStorage,
    PBINDATA_RUNTIME_INFO *ppBinInfo
)
{
    NV_STATUS              status         = NV_OK;
    PBINDATA_RUNTIME_INFO  pBinInfo       = NULL;

    // paged memory access check
    osPagedSegmentAccessCheck();

    NV_ASSERT_OR_RETURN(ppBinInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBinStorage != NULL, NV_ERR_INVALID_ARGUMENT);

    // resource data should never be NULL
    NV_ASSERT_OR_RETURN(((const BINDATA_STORAGE_PVT *) pBinStorage)->pData != NULL, NV_ERR_INVALID_ARGUMENT);


    // allocate memory for the internal structure
    pBinInfo = portMemAllocNonPaged(sizeof(BINDATA_RUNTIME_INFO));
    if (pBinInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "Memory allocation of %u bytes failed, return code %u\n",
                  (NvU32)sizeof(BINDATA_RUNTIME_INFO), status);
        DBG_BREAKPOINT();
        goto FAIL;
    }

    portMemSet(pBinInfo, 0, sizeof(BINDATA_RUNTIME_INFO));

    pBinInfo->pBinStoragePvt = (const BINDATA_STORAGE_PVT *) pBinStorage;

    // if resource is compressed, also initialize the GZ state struct
    if (pBinInfo->pBinStoragePvt->bCompressed)
    {
        if ((status = utilGzAllocate((NvU8*)(pBinInfo->pBinStoragePvt->pData),
                                     pBinInfo->pBinStoragePvt->actualSize,
                                     &(pBinInfo->pGzState))) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "gz state allocation failed, return code %u\n",
                      status);
            DBG_BREAKPOINT();
            goto FAIL;
        }
        NV_ASSERT(pBinInfo->pGzState);
    }

    *ppBinInfo = pBinInfo;

    return status;

FAIL:
    portMemFree(pBinInfo);

    return status;
}

/*!
 * Acquire helper function to implement data decompression. This function
 * inflates the amount of bytes given by nBytes and write to buffer.
 *
 * This helper does NOT allocate any memory, so buffer is assumed to have
 * at least nBytes in size.
 *
 * Being exposed to public interface, this function, however, is only designed
 * for special uses.
 *
 * @param[in]   pBinInfo    Bindata runtime information
 * @param[out]  pBuffer     Buffer area for writing the acquired data
 * @param[in]   nBytes      Number of bytes to acquire (chunk size)
 *
 * @return      'NV_OK'     If the chunk acquire was successful.
 *
 */
NV_STATUS
bindataGetNextChunk
(
    PBINDATA_RUNTIME_INFO pBinInfo,
    NvU8                 *pBuffer,
    NvU32                 nBytes
)
{
    NvU32               nBytesInflated;

    // paged memory access check
    osPagedSegmentAccessCheck();

    NV_ASSERT_OR_RETURN(pBinInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(nBytes + pBinInfo->currDataPos <= pBinInfo->pBinStoragePvt->actualSize,
                      NV_ERR_INVALID_ARGUMENT);

    // if the resource is compressed, the pGzState structure must be initialized
    if (pBinInfo->pBinStoragePvt->bCompressed == NV_TRUE && pBinInfo->pGzState == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "must call bindataAcquire() first!\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_DATA;
    }

    if (pBinInfo->pBinStoragePvt->bCompressed)
    {
        if ((nBytesInflated = utilGzGetData(pBinInfo->pGzState,
                                            pBinInfo->currDataPos,
                                            nBytes,
                                            pBuffer)) != nBytes)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to get inflated data, got %u bytes, expecting %u\n",
                      nBytesInflated, nBytes);
            DBG_BREAKPOINT();
            return NV_ERR_INFLATE_COMPRESSED_DATA_FAILED;
        }
    }
    else
    {
        portMemCopy(pBuffer, nBytes, (NvU8*)(pBinInfo->pBinStoragePvt->pData) + pBinInfo->currDataPos, nBytes);
    }

    pBinInfo->currDataPos += nBytes;

    return NV_OK;
}

/*!
 * Release the previously acquired binary resource.
 *
 * @param[in]   pBinInfo     Bindata runtime information
 *
 * @return      void
 */
void
bindataRelease
(
    PBINDATA_RUNTIME_INFO pBinInfo
)
{
    if (pBinInfo == NULL)
    {
        return;
    }

    if (pBinInfo->pGzState != NULL)
    {
        utilGzDestroy(pBinInfo->pGzState);
    }

    portMemFree(pBinInfo);
}

/*!
 * Retrieve data from Bindata storage and write it to the given memory buffer.  When
 * file overriding feature is enabled and the file exists in the target directory,
 * copy the file to buffer instead.
 *
 * @param[in]   pBinStorage     Bindata storage
 * @param[in]   pBuffer         Pointer of given buffer
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

    NV_ASSERT_OR_RETURN(bufferSize >= bindataGetBufferSize(pBinStorage), NV_ERR_BUFFER_TOO_SMALL);

    return _bindataWriteStorageToBuffer(pBinStorage, pBuffer);
}


/*!
 * Retrieve data from Bindata storage and write it to the given memory buffer.
 *
 * @param[in]   pBinStorage     Bindata storage
 * @param[in]   pBuffer         Pointer of given buffer
 *
 * @return      'NV_OK'         If the ucode was written to memory buffer successfully
 */
NV_STATUS
_bindataWriteStorageToBuffer
(
    const BINDATA_STORAGE *pBinStorage,
    NvU8                  *pBuffer
)
{
    NV_STATUS             status  = NV_OK;
    PBINDATA_RUNTIME_INFO pBinInfo = NULL;

    NV_ASSERT_OR_RETURN(pBinStorage != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_ARGUMENT);

    if ((status = bindataAcquire(pBinStorage, &pBinInfo)) != NV_OK)
    {
        DBG_BREAKPOINT();
        goto EXIT;
    }

    if ((status = bindataGetNextChunk(pBinInfo,
                                      pBuffer,
                                      pBinInfo->pBinStoragePvt->actualSize)) != NV_OK)
    {
        DBG_BREAKPOINT();
        goto EXIT;
    }

EXIT:

    bindataRelease(pBinInfo);
    return status;
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
    const char *binName
)
{
    // paged memory access check
    osPagedSegmentAccessCheck();

    if ((pBinArchive == NULL) || (binName == NULL))
    {
        return NULL;
    }

    NvU32 i;
    NvLength len = portStringLength(binName) + 1;
    for (i = 0 ; i < pBinArchive->entryNum; i++)
    {
        if (portStringCompare(binName, pBinArchive->entries[i].name, len) == 0)
        {
            bindataMarkReferenced(pBinArchive->entries[i].pBinStorage);
            return pBinArchive->entries[i].pBinStorage;
        }
    }
    return NULL;
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

void* bindataGetNextUnreferencedStorage(const BINDATA_STORAGE **iter, NvU32 *pDataSize)
{
    extern BINDATA_STORAGE_PVT g_bindata_pvt;
    extern const NvU32 g_bindata_pvt_count;

    const BINDATA_STORAGE_PVT *iterPvt  = *(const BINDATA_STORAGE_PVT **)iter;
    const BINDATA_STORAGE_PVT *firstPvt = &g_bindata_pvt;
    const BINDATA_STORAGE_PVT *lastPvt  = firstPvt + g_bindata_pvt_count - 1;

    // This API makes no sense if the data is const, so just bail out early.
    NV_ASSERT_OR_RETURN(BINDATA_IS_MUTABLE, NULL);

    if (iterPvt == NULL || (iterPvt >= firstPvt && iterPvt < lastPvt))
    {
        // Passing in NULL means start iterating.
        iterPvt = (iterPvt == NULL) ? firstPvt : (iterPvt + 1);
        while (iterPvt <= lastPvt)
        {
            if (!iterPvt->bReferenced && iterPvt->pData != NULL)
            {
                *iter = (const BINDATA_STORAGE *)iterPvt;
                *pDataSize = iterPvt->compressedSize;
                return (void*)iterPvt->pData;
            }
            iterPvt++;
        }
    }

    *iter = NULL;
    *pDataSize = 0;
    return NULL;
}

void bindataDestroyStorage(BINDATA_STORAGE *storage)
{
    BINDATA_STORAGE_PVT *pBindataPvt = (BINDATA_STORAGE_PVT *)storage;
    pBindataPvt->pData = NULL;
    pBindataPvt->actualSize = 0;
    pBindataPvt->compressedSize = 0;
}
