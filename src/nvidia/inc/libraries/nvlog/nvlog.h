/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NVLOG_H_
#define _NVLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvstatus.h"

/******************* Common Debug & Trace Defines **************************\
*                                                                           *
* Module: NVLOG.H                                                           *
*                                                                           *
\***************************************************************************/

// Include common NvLog definitions
#include "nvlog_defs.h"

// Include printf definitions
#include "nvlog/nvlog_printf.h"

#if defined(NVRM)
#include "rmconfig.h"

#define NVLOG_MODULE_ENABLED RMCFG_MODULE_NVLOG

#else

#define NVLOG_MODULE_ENABLED 1

#endif

/********************************/
/*****  Exported functions  *****/
/********************************/

#if NVLOG_MODULE_ENABLED

extern NVLOG_LOGGER             NvLogLogger;
extern NVLOG_PRINT_LOGGER       NvLogPrintLogger;

/**
 * @brief Global NvLog initialization function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogInit(void *pData);

/**
 * @brief Update the NvLog configuration from the registry
 *
 */
void nvlogUpdate(void);

/**
 * @brief Global NvLog deinitialization function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogDestroy(void);

/**
 * @brief Allocate a new NvLog buffer
 *
 * @param[in]   size          Size of the buffer to allocate
 * @param[in]   flags         Buffer flags, uses NVLOG_BUFFER_FLAGS_* DRF's
 * @param[in]   tag           Tag for the new buffer, to identify it in a dump
 * @param[out]  pBufferHandle Handle of the newly created buffer
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogAllocBuffer(NvU32 size, NvU32 flags, NvU32 tag, NVLOG_BUFFER_HANDLE *pBufferHandle, ...);

/**
 * @brief Deallocate a buffer with the given handle
 *
 * @param[in]   hBuffer     Handle of the buffer to deallocate
 * @param[in]   bDeallocPreserved Deallocate preserved buffers
 */
void nvlogDeallocBuffer(NVLOG_BUFFER_HANDLE hBuffer, NvBool bDeallocPreserved);

/**
 * @brief Write to a buffer with the given handle
 *
 * @param[in]   hBuffer     Handle of the buffer to write to
 * @param[in]   pData       Pointer to the data to be written
 * @param[in]   dataSize    Size of the data to be written
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogWriteToBuffer(NVLOG_BUFFER_HANDLE hBuffer, NvU8 *pData, NvU32 dataSize);

/**
 * @brief Extract a chunk of a buffer
 *
 * @param[in]     hBuffer    Handle of the buffer to extract
 * @param[in]     chunkNum   Index (0-based) of the chunk to extract
 * @param[in,out] pCunkSize  In  - Size of the chunk to extract
 *                           Out - Size that was actually extracted, can be less
 * @param[out]    pDest      Pointer to the memory the chunk will be copied to
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogExtractBufferChunk(NVLOG_BUFFER_HANDLE hBuffer, NvU32 chunkNum, NvU32 *pChunkSize, NvU8 *pDest);

/**
 * @brief Get the size of a specified buffer
 *
 * @param[in]     hBuffer   Handle of the buffer
 * @param[out]    pSize     Buffer size.
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogGetBufferSize(NVLOG_BUFFER_HANDLE hBuffer, NvU32 *pSize);

/**
 * @brief Get the tag of a specified buffer.
 *
 * @param[in]     hBuffer   Handle of the buffer
 * @param[out]    pTag      Buffer tag.
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogGetBufferTag(NVLOG_BUFFER_HANDLE hBuffer, NvU32 *pTag);

/**
 * @brief Get flags for a specified buffer.
 *        Flag fields are defined as NVLOG_BUFFER_FLAGS_* in nvlog_defs.h
 *
 * @param[in]     hBuffer   Handle of the buffer
 * @param[out]    pFlags    Buffer flags.
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogGetBufferFlags(NVLOG_BUFFER_HANDLE hBuffer, NvU32 *pFlags);

/**
 * @brief Pause/resume logging to a specified buffer
 *
 * @param[in]     hBuffer   Handle of the buffer
 * @param[out]    bPause    NV_TRUE ??Pause, NV_FALSE ??resume
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogPauseLoggingToBuffer(NVLOG_BUFFER_HANDLE hBuffer, NvBool bPause);

/**
 * @brief Pause/resume logging to all buffers
 *
 * @param[out]    bPause    NV_TRUE ??Pause, NV_FALSE ??resume
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogPauseAllLogging(NvBool bPause);

/**
 * @brief Get the handle of a buffer with the given tag
 *
 * @param[in]     tag            Tag of the buffer requested
 * @param[out]    pBufferHandle  Handle of the buffer
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogGetBufferHandleFromTag(NvU32 tag, NVLOG_BUFFER_HANDLE *pBufferHandle);

/**
 * @brief Get the handle of a buffer with the given tag
 *
 * @param[in]     tag            Tag of the buffer requested
 * @param[out]    pBufferHandle  Handle of the buffer
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogGetBufferSnapshot(NVLOG_BUFFER_HANDLE hBuffer, NvU8 *pDest, NvU32 destSize);


/**
 * @brief Dumps all logs into the the kernel print log
 *
 * @note this will write to the log even if all other prints are disabled,
 * including external release builds. The output will be base64 encoded and
 * not decodable without the database, and pollute the logs. Use with caution.
 *
 * The format of the dump will be the same as the OS Crash Log dumps.
 */
void nvlogDumpToKernelLog(NvBool bDumpUnchangedBuffersOnlyOnce);

//
// The values returned by CheckFilter functions contain up to four buffers.
// These indexes are in the local buffer array (i.e. in NVLOG_PRINT_LOGGER)
// There can be more than 256 total NvLog buffers, but only 256 per subsystem.
//
#define NVLOG_FILTER_BUFFER_NONE    0xFF

//
// NvLog Print functions
//

/**
 * @brief Check the filtering rules for a given DBG_PRINTF
 *
 * @param[in]     fileId         ID (name hash) of the file
 * @param[in]     line           Line number of the print
 * @param[in]     level          Debug level (DBG_LEVEL_*) of the print
 * @param[in]     module         Debug module (DBG_MODULE_*) of the print
 *
 * @return 32 bits to indicate which of the print buffers to log to.
 */
NvU32 nvlogPrintCheckFilter(NvU32 fileId, NvU16 line, NvU32 level, NvU32 module);

/**
 * @brief Global NvLog Print initialization function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogPrintInit(void);

/**
 * @brief NvLog Print update function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogPrintUpdate(void);

/**
 * @brief Global NvLog Print deinitialization function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogPrintDestroy(void);

//
// NvLog ETW functions
//

/**
 * @brief Global NvLog ETW capture state function
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogETWCaptureState(void);

/**
 * @brief Pushes all buffer tags to ETW
 */
void nvlogETWPushTags(void);

/**
 * @brief Pushes an nvlog buffer header to ETW
 */
void nvlogETWPushBufferHeader(NVLOG_BUFFER *pBuffer);

/**
 * @brief Pushes an nvlog entry to ETW
 *
 * @return NV_TRUE on success
 */
NvBool nvlogETWPush(NVLOG_BUFFER *pBuffer, NvU8 *pData, NvU32 dataSize);

//
// Global initialization macros
//
extern volatile NvU32 nvlogInitCount;
#define NVLOG_INIT(pData)                                 \
    do                                                    \
    {                                                     \
        if (portAtomicIncrementU32(&nvlogInitCount) == 1) \
        {                                                 \
            nvlogInit(pData);                             \
        }                                                 \
    } while (0)

#define NVLOG_UPDATE()                                    \
    do                                                    \
    {                                                     \
        if (nvlogInitCount == 1)                          \
        {                                                 \
            nvlogUpdate();                                \
        }                                                 \
    } while (0)

#define NVLOG_DESTROY()                                   \
    do                                                    \
    {                                                     \
        if (portAtomicDecrementU32(&nvlogInitCount) == 0) \
        {                                                 \
            nvlogDestroy();                               \
        }                                                 \
    } while (0)

/********************************/
/******  NvLog Filtering  *******/
/********************************/

//
// Used both by print and regtrace functions.
//

/**
 * @brief Binary search the range array for a given number
 *
 * @param[in]   ranges    Range array to search
 * @param[in]   numRanges Size of the given array
 * @param[in]   num       Number to search for.
 *
 * @return  Number that is found in the given range.
 *          If no number is found, returns ~0 (0xFFFFFFFF)
 */
NvU32 nvlogFindInRange16Array(NVLOG_RANGE_16 *ranges, NvU32 numRanges, NvU16 num);
/**
 * @brief Binary search the range array for a given number
 *
 * @param[in]   ranges    Range array to search
 * @param[in]   numRanges Size of the given array
 * @param[in]   num       Number to search for.
 *
 * @return  Number that is found in the given range.
 *          If no number is found, returns ~0 (0xFFFFFFFF)
 */
NvU32 nvlogFindInRange32Array(NVLOG_RANGE_32 *ranges, NvU32 numRanges, NvU32 num);

// Returns the rules for the given fileId-lineNum pair
/**
 * @brief Binary search the range array for a given number
 *
 * @param[in]   pFileLineFilter    File:line filter to check
 * @param[in]   fileId             ID of the file to search
 * @param[in]   lineNum            Line number to search in the file entry
 *
 * @return  Number that is found for the given file:line.
 *          If no number is found, returns ~0 (0xFFFFFFFF)
 */
NvU32 nvlogGetFileLineFilterRules(NVLOG_FILELINE_FILTER *pFileLineFilter, NvU32 fileId, NvU16 lineNum);


/**
 * @brief Dump nvlog to kernel log only if enabled (performs regkey and platform checks)
 */
void nvlogDumpToKernelLogIfEnabled(void);

/**
 * @param[in]   pCb   callback function to be called when nvlog buffers need to be flushed
 * @param[in]   pData argument to pass to pCb
 * @param[out]  ppCb  output callback data pointer
 *
 * @return NV_OK on success
 */
NV_STATUS nvlogRegisterFlushCb(void (*pCb)(void*), void *pData);

/**
 * @param[in] pCb   callback pCb to be deregistered
 * @param[in] pData argument that pCb was registered with
 */
void nvlogDeregisterFlushCb(void (*pCb)(void*), void *pData);

//
// Run registered callbacks.
// All callback list accesses are synchronised.
//
void nvlogRunFlushCbs(void);

#else

#define NVLOG_INIT(pData)
#define NVLOG_UPDATE()
#define NVLOG_DESTROY()

#define nvlogDumpToKernelLogIfEnabled()
#define nvlogDumpToKernelLog(bDumpUnchangedBuffersOnlyOnce)

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _NVLOG_H_
