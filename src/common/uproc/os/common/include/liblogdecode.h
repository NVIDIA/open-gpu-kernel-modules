/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef LOGDECODE_H_
#define LOGDECODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NVRM

#        define LIBOS_LOG_DECODE_ENABLE 1

#        define LIBOS_LOG_TO_NVLOG 1

#    define LIBOS_LOG_MAX_LOGS      7   // Max logs per GPU

#elif defined(LIBOS_LOG_OFFLINE_DECODER)
#    include "nvlog_decoder.h"

#    define LIBOS_LOG_DECODE_ENABLE 1
#    define LIBOS_LOG_TO_NVLOG      0

#    define LIBOS_LOG_MAX_LOGS    160   // Max logs for all GPUs for offline decoder

#else
#    error "Need to define either NVRM or LIBOS_LOG_OFFLINE_DECODER."
#endif // NVRM

#define LIBOS_LOG_ENABLE          (LIBOS_LOG_TO_NVLOG || LIBOS_LOG_DECODE_ENABLE)

#if LIBOS_LOG_DECODE_ENABLE
#    include "../include/libos_log.h"
#    include "libdwarf.h"
#endif

// Forward declarations.
struct LIBOS_LOG_DECODE_LOG;
typedef struct LIBOS_LOG_DECODE_LOG LIBOS_LOG_DECODE_LOG;

#define LIBOS_LOG_LINE_BUFFER_SIZE 128
#define LIBOS_LOG_MAX_ARGS         20

#if LIBOS_LOG_DECODE_ENABLE

#    include "nvctassert.h"

typedef struct
{
    NV_DECLARE_ALIGNED(LIBOS_LOG_DECODE_LOG *log, 8);
    NV_DECLARE_ALIGNED(LIBOS_LOG_DECODE_LOG *logSymbolResolver, 8);
    NV_DECLARE_ALIGNED(libosLogMetadata *meta, 8);
    NV_DECLARE_ALIGNED(NvU64 timeStamp, 8);
    NvU64 args[LIBOS_LOG_MAX_ARGS];
} LIBOS_LOG_DECODE_RECORD;

// Size of LIBOS_LOG_DECODE_RECORD without args, in number of NvU64 entries.
#    define LIBOS_LOG_DECODE_RECORD_BASE 4

// Ensure that the size matches up (no padding in the struct)
ct_assert((LIBOS_LOG_DECODE_RECORD_BASE * sizeof(NvU64)) == (sizeof(LIBOS_LOG_DECODE_RECORD) - sizeof(((LIBOS_LOG_DECODE_RECORD*)NULL)->args)));

#endif // LIBOS_LOG_DECODE_ENABLE

#define TASK_NAME_MAX_LENGTH   (8)
#define SOURCE_NAME_MAX_LENGTH (4)

#define ELF_SECTION_NAME_MAX   (32) // In bytes

// NvLog buffer
typedef struct
{
    NvU32 gpuArch;
    NvU32 gpuImpl;
    NvU32 rsvd1;
    NvU32 rsvd2;
    char taskPrefix[TASK_NAME_MAX_LENGTH]; // Prefix string printed before each line.
    NvU8 data[0];
} LIBOS_LOG_NVLOG_BUFFER;

#define LIBOS_LOG_NVLOG_BUFFER_SIZE(dataSize) (NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + (dataSize))

struct LIBOS_LOG_DECODE_LOG
{
    volatile NvU64 *physicLogBuffer;
    NvU64 logBufferSize;             // Includes put pointer located in first 8 bytes.
    NvU64 previousPut;               // Keeps track of records already printed.
    NvU64 putCopy;                   // End pointer for this batch.
    NvU64 putIter;                   // Iterator for this batch.
    NvU32 gpuInstance;               // GPU that this log is associated with.
    char taskPrefix[TASK_NAME_MAX_LENGTH];     // Prefix string printed before each line.
    char elfSectionName[ELF_SECTION_NAME_MAX]; // Task section name in container logging ELF serving as ID.

#if LIBOS_LOG_TO_NVLOG
    NvU32 hNvLogNoWrap;  // No wrap buffer captures first records.
    NvU32 hNvLogWrap;    // Wrap buffer captures last records.
    NvBool bNvLogNoWrap; // NV_TRUE if no wrap buffer not full.

    NvBool bDidPush;     // NV_TRUE if this buffer was ever pushed to
    NvU64 preservedNoWrapPos; // Position in preserved nvlog buffer
#endif

#if LIBOS_LOG_DECODE_ENABLE
    LibosElf64Header *elf;
    LibosElfImage elfImage;
    LibosDebugResolver resolver;
    LIBOS_LOG_DECODE_RECORD record;
#endif
};

typedef struct
{
    char sourceName[SOURCE_NAME_MAX_LENGTH]; // GSP, PMU etc

    NvU64 numLogBuffers;
    LIBOS_LOG_DECODE_LOG log[LIBOS_LOG_MAX_LOGS];

#if LIBOS_LOG_DECODE_ENABLE
    NvBool bIsDecodable;     // True if a logging ELF is provided, False on NULL
    NvU64 *scratchBuffer;    // Sorted by timestamp.
    NvU64 scratchBufferSize; // Sum of logBufferSize.
    char *curLineBufPtr;     // Current position in lineBuffer.
    // Decodes into lineBuffer, then prints as a string.
    char lineBuffer[LIBOS_LOG_LINE_BUFFER_SIZE];
    NvBool bSynchronousBuffer;
    NvBool bPtrSymbolResolve;
    NvU8 lineLogLevel;

    // Fall back to SHDR when a PDHR with %s argument is not found.
    NvBool bDecodeStrShdr;
#endif // LIBOS_LOG_DECODE_ENABLE

#if defined(LIBOS_LOG_OFFLINE_DECODER)
    LogPrinter *pLogPrinter;
#endif

} LIBOS_LOG_DECODE;

#if defined(LIBOS_LOG_OFFLINE_DECODER)
void libosLogCreate(LIBOS_LOG_DECODE *logDecode, LogPrinter *pLogPrinter);
void libosLogCreateEx(LIBOS_LOG_DECODE *logDecode, const char *pSourceName, LogPrinter *pLogPrinter);
#else
void libosLogCreate(LIBOS_LOG_DECODE *logDecode);
void libosLogCreateEx(LIBOS_LOG_DECODE *logDecode, const char *pSourceName);
#endif

void libosLogAddLogEx(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, NvU32 gpuArch, NvU32 gpuImpl, const char *name, const char *elfSectionName);
void libosLogAddLog(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, const char *name, const char *elfSectionName);

#if LIBOS_LOG_DECODE_ENABLE
void libosLogInit(LIBOS_LOG_DECODE *logDecode, LibosElf64Header *elf, NvU64 elfSize);
void libosLogInitEx(
    LIBOS_LOG_DECODE *logDecode, LibosElf64Header *elf, NvBool bSynchronousBuffer,
    NvBool bPtrSymbolResolve, NvBool bDecodeStrFmt, NvU64 elfSize);
#else
void libosLogInit(LIBOS_LOG_DECODE *logDecode, void *elf, NvU64 elfSize);
void libosLogInitEx(
    LIBOS_LOG_DECODE *logDecode, void *elf, NvBool bSynchronousBuffer, NvBool bPtrSymbolResolve,
    NvBool bDecodeStrFmt, NvU64 elfSize);
#endif // LIBOS_LOG_DECODE_ENABLE

NvBool libosLogSymbolicateAddress(LIBOS_LOG_DECODE *logDecode, char *decodedLine, NvLength decodedLineSize, NvUPtr addr, NvBool bPrintDirectory);

void libosLogDestroy(LIBOS_LOG_DECODE *logDecode);

void libosExtractLogs(LIBOS_LOG_DECODE *logDecode, NvBool bSyncNvLog);

void libosPreserveLogs(LIBOS_LOG_DECODE *pLogDecode);

#ifdef __cplusplus
}
#endif

#endif // LOGDECODE_H_
