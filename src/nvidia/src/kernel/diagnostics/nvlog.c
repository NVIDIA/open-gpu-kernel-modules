/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlog/nvlog.h"
#include "nvrm_registry.h"
#include "os/os.h"
#include "diagnostics/tracer.h"
#include "tls/tls.h"
#include "core/locks.h"

//
// Buffer push method declarations
//
NvBool nvlogRingBufferPush  (NVLOG_BUFFER *pBuffer, NvU8 *pData, NvU32 dataSize);
NvBool nvlogNowrapBufferPush(NVLOG_BUFFER *pBuffer, NvU8 *pData, NvU32 dataSize);
NvBool nvlogStringBufferPush(NVLOG_BUFFER *unused,  NvU8 *pData, NvU32 dataSize);
NvBool nvlogKernelLogPush(NVLOG_BUFFER *unused, NvU8 *pData, NvU32 dataSize);

static void _printBase64(NvU8 *pData, NvU32 dataSize);
static NV_STATUS _allocateNvlogBuffer(NvU32 size, NvU32 flags, NvU32 tag,
                                      NVLOG_BUFFER **ppBuffer);
static void _deallocateNvlogBuffer(NVLOG_BUFFER *pBuffer);

volatile NvU32 nvlogInitCount;
static void *nvlogRegRoot;

// Zero (null) buffer definition.
static NVLOG_BUFFER _nvlogZeroBuffer =
{
    {nvlogStringBufferPush},
    0,
    NvU32_BUILD('l','l','u','n'),
    0,
    0,
    0
};

NVLOG_LOGGER NvLogLogger =
{
    NVLOG_LOGGER_VERSION,

    // Default buffers
    {
        // The 0th buffer just prints to the screen in debug builds.
        &_nvlogZeroBuffer
    },

    // Next available slot
    1,

    // Free slots
    NVLOG_MAX_BUFFERS-1,

    // Main lock, must be allocated at runtime.
    NULL
};

#define NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer)                                  \
  ((hBuffer < NVLOG_MAX_BUFFERS) && (NvLogLogger.pBuffers[hBuffer] != NULL))

NV_STATUS
nvlogInit(void *pData)
{
    nvlogRegRoot = pData;
    portInitialize();
    NvLogLogger.mainLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (NvLogLogger.mainLock == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    tlsInitialize();
    return NV_OK;
}

void nvlogUpdate() {
}

NV_STATUS
nvlogDestroy()
{
    NvU32 i;

    tlsShutdown();
    for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
    {
        nvlogDeallocBuffer(i);
    }
    if (NvLogLogger.mainLock != NULL)
    {
        portSyncSpinlockDestroy(NvLogLogger.mainLock);
        NvLogLogger.mainLock = NULL;
    }

    /// @todo Destructor should return void.
    portShutdown();
    return NV_OK;
}

static NV_STATUS
_allocateNvlogBuffer
(
    NvU32          size,
    NvU32          flags,
    NvU32          tag,
    NVLOG_BUFFER **ppBuffer
)
{
    NVLOG_BUFFER          *pBuffer;
    NVLOG_BUFFER_PUSHFUNC  pushfunc;

    // Sanity check on some invalid combos:
    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _EXPANDABLE, _YES, flags))
    {
        // Only nonwrapping buffers can be expanded
        if (!FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _NOWRAP, flags))
            return NV_ERR_INVALID_ARGUMENT;
        // Full locking required to expand the buffer.
        if (!FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _LOCKING, _FULL, flags))
            return NV_ERR_INVALID_ARGUMENT;
    }

    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _SYSTEMLOG, flags))
    {
        // System log does not need to allocate memory for buffer.
        pushfunc = (NVLOG_BUFFER_PUSHFUNC) nvlogKernelLogPush;
        size = 0;
    }
    else
    {
        NV_ASSERT_OR_RETURN(size > 0, NV_ERR_INVALID_ARGUMENT);

        if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _RING, flags))
        {
            pushfunc = (NVLOG_BUFFER_PUSHFUNC) nvlogRingBufferPush;
        }
        else if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _NOWRAP, flags))
        {
            pushfunc = (NVLOG_BUFFER_PUSHFUNC) nvlogNowrapBufferPush;
        }
        else
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _NONPAGED, _YES, flags))
        pBuffer = portMemAllocNonPaged(sizeof(*pBuffer) + size);
    else
        pBuffer = portMemAllocPaged(sizeof(*pBuffer) + size);

    if (!pBuffer)
        return NV_ERR_NO_MEMORY;

    portMemSet(pBuffer, 0, sizeof(*pBuffer) + size);
    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _OCA, _YES, flags))
    {
        osAddRecordForCrashLog(pBuffer, NV_OFFSETOF(NVLOG_BUFFER, data) + size);
    }

    pBuffer->push.fn  = pushfunc;
    pBuffer->size     = size;
    pBuffer->flags    = flags;
    pBuffer->tag      = tag;

    *ppBuffer = pBuffer;

    return NV_OK;
}

static void
_deallocateNvlogBuffer
(
    NVLOG_BUFFER *pBuffer
)
{
    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _OCA, _YES, pBuffer->flags))
        osDeleteRecordForCrashLog(pBuffer);

    portMemFree(pBuffer);
}

NV_STATUS
nvlogAllocBuffer
(
    NvU32                size,
    NvU32                flags,
    NvU32                tag,
    NVLOG_BUFFER_HANDLE *pBufferHandle,
    ...
)
{
    NVLOG_BUFFER *pBuffer;
    NV_STATUS     status;

    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _SYSTEMLOG, flags))
    {
    }
    else
    {
        NV_ASSERT_OR_RETURN(NvLogLogger.totalFree > 0,
                          NV_ERR_INSUFFICIENT_RESOURCES);
    }

    status = _allocateNvlogBuffer(size, flags, tag, &pBuffer);

    if (status != NV_OK)
    {
        return status;
    }

    portSyncSpinlockAcquire(NvLogLogger.mainLock);

    if (NvLogLogger.nextFree < NVLOG_MAX_BUFFERS)
    {
        NvLogLogger.pBuffers[NvLogLogger.nextFree] = pBuffer;
        *pBufferHandle = NvLogLogger.nextFree++;
        NvLogLogger.totalFree--;
    }
    else
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Find the next slot in the buffers array
    while (NvLogLogger.nextFree < NVLOG_MAX_BUFFERS)
    {
        if (NvLogLogger.pBuffers[NvLogLogger.nextFree] != NULL)
            NvLogLogger.nextFree++;
        else break;
    }
    portSyncSpinlockRelease(NvLogLogger.mainLock);

    if (status != NV_OK)
    {
        portMemFree(pBuffer);
    }

    return status;
}

void
nvlogDeallocBuffer
(
    NVLOG_BUFFER_HANDLE hBuffer
)
{
    NVLOG_BUFFER *pBuffer;

    if ((hBuffer == 0) || !NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer))
        return;

    pBuffer = NvLogLogger.pBuffers[hBuffer];

    pBuffer->flags = FLD_SET_DRF(LOG_BUFFER, _FLAGS, _DISABLED,
                                 _YES, pBuffer->flags);

    while (pBuffer->threadCount > 0) { /*spin*/ }
    portSyncSpinlockAcquire(NvLogLogger.mainLock);
      NvLogLogger.pBuffers[hBuffer] = NULL;
      NvLogLogger.nextFree = NV_MIN(hBuffer, NvLogLogger.nextFree);
      NvLogLogger.totalFree++;
    portSyncSpinlockRelease(NvLogLogger.mainLock);

    _deallocateNvlogBuffer(pBuffer);
}

NV_STATUS
nvlogWriteToBuffer
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU8 *pData,
    NvU32 size
)
{
    NvBool status;
    NVLOG_BUFFER *pBuffer;

    NV_ASSERT_OR_RETURN(size > 0,                    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pData != NULL,               NV_ERR_INVALID_POINTER);

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    pBuffer = NvLogLogger.pBuffers[hBuffer];

    // Normal condition when fetching nvLog from NV0000_CTRL_CMD_NVD_GET_NVLOG.
    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _DISABLED, _YES, pBuffer->flags))
        return NV_ERR_NOT_READY;

    portAtomicIncrementS32(&pBuffer->threadCount);
    status = pBuffer->push.fn(pBuffer, pData, size);
    // Get pBuffer from the handle again, as it might have realloc'd
    portAtomicDecrementS32(&NvLogLogger.pBuffers[hBuffer]->threadCount);

    return (status == NV_TRUE) ? NV_OK : NV_ERR_BUFFER_TOO_SMALL;
}



NV_STATUS
nvlogExtractBufferChunk
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU32               chunkNum,
    NvU32              *pChunkSize,
    NvU8               *pDest
)
{
    NVLOG_BUFFER *pBuffer;
    NvU32 index;

    NV_ASSERT_OR_RETURN(*pChunkSize > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDest != NULL,   NV_ERR_INVALID_POINTER);

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    pBuffer = NvLogLogger.pBuffers[hBuffer];

    index = chunkNum * (*pChunkSize);
    NV_ASSERT_OR_RETURN(index <= pBuffer->size,   NV_ERR_OUT_OF_RANGE);
    *pChunkSize = NV_MIN(*pChunkSize, (pBuffer->size - index));

    portSyncSpinlockAcquire(NvLogLogger.mainLock);
    portMemCopy(pDest, *pChunkSize, &pBuffer->data[index], *pChunkSize);
    portSyncSpinlockRelease(NvLogLogger.mainLock);

    return NV_OK;
}


NV_STATUS
nvlogGetBufferSize
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU32 *pSize
)
{
    NV_ASSERT_OR_RETURN(pSize != NULL, NV_ERR_INVALID_POINTER);

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    *pSize = NvLogLogger.pBuffers[hBuffer]->size;
    return NV_OK;
}

NV_STATUS
nvlogGetBufferTag
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU32 *pTag
)
{
    NV_ASSERT_OR_RETURN(pTag != NULL, NV_ERR_INVALID_POINTER);

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    *pTag = NvLogLogger.pBuffers[hBuffer]->tag;
    return NV_OK;
}

NV_STATUS
nvlogGetBufferFlags
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU32 *pFlags
)
{
    NV_ASSERT_OR_RETURN(pFlags != NULL, NV_ERR_INVALID_POINTER);

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    *pFlags = NvLogLogger.pBuffers[hBuffer]->flags;
    return NV_OK;
}


NV_STATUS
nvlogPauseLoggingToBuffer
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvBool bPause
)
{
    NVLOG_BUFFER *pBuffer;

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    pBuffer = NvLogLogger.pBuffers[hBuffer];

    pBuffer->flags = (bPause)
        ? FLD_SET_DRF(LOG, _BUFFER_FLAGS, _DISABLED, _YES, pBuffer->flags)
        : FLD_SET_DRF(LOG, _BUFFER_FLAGS, _DISABLED, _NO,  pBuffer->flags);

    return NV_OK;
}


NV_STATUS
nvlogPauseAllLogging
(
    NvBool bPause
)
{
    return NV_OK;
}

NV_STATUS
nvlogGetBufferHandleFromTag
(
    NvU32 tag,
    NVLOG_BUFFER_HANDLE *pBufferHandle
)
{
    NvU32 i;

    NV_ASSERT_OR_RETURN(pBufferHandle != NULL, NV_ERR_INVALID_POINTER);

    for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
    {
        if (NvLogLogger.pBuffers[i] != NULL)
        {
            if (NvLogLogger.pBuffers[i]->tag == tag)
            {
                *pBufferHandle = i;
                return NV_OK;
            }
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
nvlogGetBufferSnapshot
(
    NVLOG_BUFFER_HANDLE hBuffer,
    NvU8               *pDest,
    NvU32               destSize
)
{
    NVLOG_BUFFER *pBuffer;

    NV_ASSERT_OR_RETURN(NVLOG_IS_VALID_BUFFER_HANDLE(hBuffer),
                      NV_ERR_INVALID_OBJECT_HANDLE);

    NV_ASSERT_OR_RETURN(pDest != NULL, NV_ERR_INVALID_POINTER);

    pBuffer = NvLogLogger.pBuffers[hBuffer];

    NV_ASSERT_OR_RETURN(destSize >= NVLOG_BUFFER_SIZE(pBuffer),
                        NV_ERR_BUFFER_TOO_SMALL);

    portSyncSpinlockAcquire(NvLogLogger.mainLock);
    portMemCopy(pDest, NVLOG_BUFFER_SIZE(pBuffer), pBuffer, NVLOG_BUFFER_SIZE(pBuffer));
    portSyncSpinlockRelease(NvLogLogger.mainLock);

    return NV_OK;
}



NvBool
nvlogRingBufferPush
(
    NVLOG_BUFFER *pBuffer,
    NvU8         *pData,
    NvU32        dataSize
)
{
    NvU32 writeSize;
    NvU32 oldPos;
    NvU32 lock = DRF_VAL(LOG, _BUFFER_FLAGS, _LOCKING, pBuffer->flags);

    if (lock != NVLOG_BUFFER_FLAGS_LOCKING_NONE)
        portSyncSpinlockAcquire(NvLogLogger.mainLock);

    oldPos = pBuffer->pos;
    pBuffer->extra.ring.overflow += (pBuffer->pos + dataSize) / pBuffer->size;
    pBuffer->pos                  = (pBuffer->pos + dataSize) % pBuffer->size;

    // State locking does portMemCopy unlocked.
    if (lock == NVLOG_BUFFER_FLAGS_LOCKING_STATE)
        portSyncSpinlockRelease(NvLogLogger.mainLock);

    while (dataSize > 0)
    {
        writeSize = NV_MIN(pBuffer->size - oldPos, dataSize);
        portMemCopy(&pBuffer->data[oldPos], writeSize, pData, writeSize);
        oldPos = 0;
        dataSize -= writeSize;
        pData    += writeSize;
    }

    if (lock == NVLOG_BUFFER_FLAGS_LOCKING_FULL)
        portSyncSpinlockRelease(NvLogLogger.mainLock);

    return NV_TRUE;
}

NvBool
nvlogNowrapBufferPush
(
    NVLOG_BUFFER *pBuffer,
    NvU8 *pData,
    NvU32 dataSize
)
{
    NvU32 oldPos;
    NvU32 lock = DRF_VAL(LOG, _BUFFER_FLAGS, _LOCKING, pBuffer->flags);

    if (pBuffer->pos + dataSize >= pBuffer->size)
    {
        NvBool bExpandable = FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _EXPANDABLE, _YES, pBuffer->flags);
        NvBool bNonPaged   = FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _NONPAGED,   _YES, pBuffer->flags);

        // Expandable buffer, and we are at IRQL where we can do realloc
        if (bExpandable &&
            ((bNonPaged && portMemExSafeForNonPagedAlloc()) || (!bNonPaged && portMemExSafeForPagedAlloc())))
        {
            NVLOG_BUFFER *pNewBuffer;
            NvU32 i;
            NvU32 newSize = pBuffer->size * 2;
            NvU32 allocSize = sizeof(*pBuffer) + newSize;

            pNewBuffer = bNonPaged ? portMemAllocNonPaged(allocSize) : portMemAllocPaged(allocSize);
            if (pNewBuffer == NULL)
                return NV_FALSE;

            //
            // Two threads couid have entered this block at the same time, and
            // both will have allocated their own bigger buffer. Only the one
            // that takes the spinlock first should do the copy and the swap.
            //
            portSyncSpinlockAcquire(NvLogLogger.mainLock);
              // Check if this buffer is still there and was not swapped for a bigger one
              for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
              {
                  if (NvLogLogger.pBuffers[i] == pBuffer)
                    break;
              }
              if (i == NVLOG_MAX_BUFFERS)
              {
                  // Another thread has already expanded the buffer, bail out.
                  // TODO: Maybe we could store the handle and then try again?
                  portSyncSpinlockRelease(NvLogLogger.mainLock);
                  return NV_FALSE;
              }

              portMemCopy(pNewBuffer, allocSize, pBuffer, sizeof(*pBuffer)+pBuffer->size);
              pNewBuffer->size = newSize;
              for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
              {
                  if (NvLogLogger.pBuffers[i] == pBuffer)
                      NvLogLogger.pBuffers[i] = pNewBuffer;
              }
            portSyncSpinlockRelease(NvLogLogger.mainLock);

            //
            // Before we can free this buffer, we need to make sure any threads
            // that were still accessing it are done. Spin on volatile threadCount
            // NOTE: threadCount includes the current thread too.
            //
            while (pBuffer->threadCount > 1) { /*spin*/ }
            portMemFree(pBuffer);
            pBuffer = pNewBuffer;
        }
        else
        {
            return NV_FALSE;
        }
    }

    if (lock != NVLOG_BUFFER_FLAGS_LOCKING_NONE)
        portSyncSpinlockAcquire(NvLogLogger.mainLock);

      oldPos = pBuffer->pos;
      pBuffer->pos = oldPos + dataSize;

    // State locking does portMemCopy unlocked.
    if (lock == NVLOG_BUFFER_FLAGS_LOCKING_STATE)
        portSyncSpinlockRelease(NvLogLogger.mainLock);

    portMemCopy(&pBuffer->data[oldPos], dataSize, pData, dataSize);

    if (lock == NVLOG_BUFFER_FLAGS_LOCKING_FULL)
        portSyncSpinlockRelease(NvLogLogger.mainLock);

    return NV_TRUE;
}

NvBool
nvlogStringBufferPush
(
    NVLOG_BUFFER *unused,
    NvU8         *pData,
    NvU32         dataSize
)
{
    return NV_TRUE;
}

//
// Prints the buffer encoded as base64, with a prefix for easy grepping.
// Base64 allows the padding characters ('=') to appear anywhere, not just at
// the end, so it is fine to print buffers one at a time without merging.
//
static void _printBase64(NvU8 *pData, NvU32 dataSize)
{
    const NvU8 base64_key[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    NvU8 output[64+1]; // 64 bas64 characters per line of output
    NvU32 i;

    do
    {
        i = 0;
        while (i < (sizeof(output)-1) && (dataSize > 0))
        {
            output[i++] = base64_key[pData[0] >> 2];
            if (dataSize == 1)
            {
                output[i++] = base64_key[(pData[0] << 4) & 0x3F];
                output[i++] = '=';
                output[i++] = '=';
                dataSize = 0;
                break;
            }

            output[i++] = base64_key[((pData[0] << 4) & 0x3F) | (pData[1] >> 4)];
            if (dataSize == 2)
            {
                output[i++] = base64_key[(pData[1] << 2) & 0x3F];
                output[i++] = '=';
                dataSize = 0;
                break;
            }

            output[i++] = base64_key[((pData[1] << 2) & 0x3F) | (pData[2] >> 6)];
            output[i++] = base64_key[pData[2] & 0x3F];

            pData += 3;
            dataSize -= 3;
        }
        output[i] = 0;
        portDbgPrintf("nvrm-nvlog: %s\n", output);
    } while (dataSize > 0);
}

NvBool nvlogKernelLogPush(NVLOG_BUFFER *unused, NvU8 *pData, NvU32 dataSize)
{
    PORT_UNREFERENCED_VARIABLE(unused);
    _printBase64(pData, dataSize);
    return NV_TRUE;
}

void nvlogDumpToKernelLog(NvBool bDumpUnchangedBuffersOnlyOnce)
{
    NvU32 i;
    static NvU32 lastDumpPos[NVLOG_MAX_BUFFERS];

    for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
    {
        NVLOG_BUFFER *pBuf = NvLogLogger.pBuffers[i];

        if (pBuf && pBuf->size)
        {
            if (bDumpUnchangedBuffersOnlyOnce)
            {
                NvU32 pos = pBuf->pos + (pBuf->size * pBuf->extra.ring.overflow);

                //Dump the buffer only if it's contents have changed
                if (lastDumpPos[i] != pos)
                {
                    lastDumpPos[i] = pos;
                    _printBase64((NvU8*)pBuf, NVLOG_BUFFER_SIZE(pBuf));
                }
            }
            else
            {
                _printBase64((NvU8*)pBuf, NVLOG_BUFFER_SIZE(pBuf));
            }
        }
    }
}

void nvlogDumpToKernelLogIfEnabled(void)
{
    NvU32 dumpNvlogValue;

    // Debug and develop builds already dump everything as it happens.
#if defined(DEBUG) || defined(DEVELOP)
    return;
#endif

    // Enable only if the regkey has been set
    if (osReadRegistryDword(NULL, NV_REG_STR_RM_DUMP_NVLOG, &dumpNvlogValue) != NV_OK)
        return;

    if (dumpNvlogValue != NV_REG_STR_RM_DUMP_NVLOG_ENABLE)
        return;

    nvlogDumpToKernelLog(NV_FALSE);
}

