/*
 * ----------------------------------------------------------------------
 * Copyright (c) 2005-2014 Rich Felker, et al.
 * Copyright (c) 2019-2024, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ----------------------------------------------------------------------
 */

#ifdef NVRM

#    include <core/core.h>
#    include <stddef.h> // size_t

#define LIBOS_LOG_DECODE_PRINTF(level, fmt, ...) portDbgExPrintfLevel(level, fmt, ##__VA_ARGS__)

#    define snprintf         nvDbgSnprintf

#else // NVRM

#if defined(NVSYM_STANDALONE)
#include "time.h"
#endif

#    include <stdio.h>
#    include <stdlib.h>
#    include <ctype.h>
#    include <string.h>
#    include <assert.h>

#    define portStringCopy(d, ld, s, ls) strncpy(d, s, ld)
#    define portStringLength(s)          strlen(s)
#    define portStringLengthSafe(s, l)   strlen(s)
#    define portStringCompare(s1, s2, l) strncmp(s1, s2, l)

#    define portMemCopy(d, ld, s, ls) memcpy(d, s, ld)
#    define portMemSet(d, v, l)       memset(d, v, l)
#    define portMemCmp(d, s, l)       memcmp(d, s, l)
#    define portMemAllocNonPaged(l)   malloc(l)
#    define portMemFree(p)            free(p)
#    define NV_ASSERT                 assert

#    if defined(NVWATCH)
#        pragma warning(push)
#        pragma warning(disable : 4244)
#        pragma warning(disable : 4267)
#    endif

#    if defined(LIBOS_LOG_OFFLINE_DECODER)
#        define LIBOS_LOG_DECODE_PRINTF(level, fmt, ...) NVLD_PRINTF(logDecode->pLogPrinter, fmt, ##__VA_ARGS__)
#    endif

#    define RMCFG_FEATURE_x 0

#endif // NVRM

#ifndef LIBOS_LOG_DECODE_PRINTF
#define LIBOS_LOG_DECODE_PRINTF(level, fmt, ...) printf(fmt, ...)
#endif

#include <stddef.h>

#include "nvtypes.h"
#include "nvstatus.h"
#include "liblogdecode.h"
#include "utils/nvprintf_level.h"
#include "nv-firmware-chip-family-select.h"

#if LIBOS_LOG_DECODE_ENABLE

#    define SYM_DECODED_LINE_MAX_SIZE 1024

// These defines assume RISCV with -mabi=lp64/lp64f/lp64d
#    define LOG_INT_MAX     NV_S32_MAX
#    define LOG_UINT_MAX    NV_U32_MAX
#    define LOG_INTMAX_MAX  NV_S64_MAX
#    define LOG_UINTMAX_MAX NV_U64_MAX
#    define LOG_LLONG_MAX   NV_S64_MAX
#    define LOG_ULLONG_MAX  NV_U64_MAX
#    define LOG_SIZE_MAX    NV_U64_MAX

// Note: this is a WAR and not really used.
#    define NL_ARGMAX  32

/* Some useful macros */
#    define IS_DIGIT(c) (((c) >= '0') && ((c) <= '9'))

/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character. */
#    define ALT_FORM (1U << ('#' - ' '))
#    define ZERO_PAD (1U << ('0' - ' '))
#    define LEFT_ADJ (1U << ('-' - ' '))
#    define PAD_POS  (1U << (' ' - ' '))
#    define MARK_POS (1U << ('+' - ' '))
#    define GROUPED  (1U << ('\'' - ' '))
#    define FLAGMASK (ALT_FORM | ZERO_PAD | LEFT_ADJ | PAD_POS | MARK_POS | GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success. */
enum
{
    LOG_BARE,
    LOG_LPRE,
    LOG_LLPRE,
    LOG_HPRE,
    LOG_HHPRE,
    LOG_BIGLPRE,
    LOG_ZTPRE,
    LOG_JPRE,
    LOG_STOP,

    //
    // Note: for our use-case (no va_args processing),
    // LOG_PTR through LOG_UIPTR could be merged into a
    // single LOG_SPECIFIER state.
    //
    LOG_PTR,
    LOG_INT,
    LOG_UINT,
    LOG_ULLONG,
    LOG_LONG,
    LOG_ULONG,
    LOG_SHORT,
    LOG_USHORT,
    LOG_CHAR,
    LOG_UCHAR,
    LOG_LLONG,
    LOG_SIZET,
    LOG_IMAX,
    LOG_UMAX,
    LOG_PDIFF,
    LOG_UIPTR,

    LOG_NOARG,
    LOG_MAXSTATE
};
#    define S(i,x) states[(NvU32)i][(x) - 'A']
static NvU8 states[]['z' - 'A' + 1] = {
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },};
#    define OOB(x) ((NvU32)(x) - 'A' > 'z' - 'A')
union arg
{
    NvU64 i;
    void *p;
};

/**
 * @brief Initialize the print state struct.
 */
static void states_init(void)
{
    {
        /* 0: bare types */
        S(LOG_BARE,'d') = LOG_INT,   S(LOG_BARE,'i') = LOG_INT,   S(LOG_BARE,'o') = LOG_UINT,  S(LOG_BARE,'u') = LOG_UINT,    S(LOG_BARE,'x') = LOG_UINT,
        S(LOG_BARE,'X') = LOG_UINT,  S(LOG_BARE,'c') = LOG_CHAR,  S(LOG_BARE,'C') = LOG_INT,   S(LOG_BARE,'s') = LOG_PTR,     S(LOG_BARE,'S') = LOG_PTR,
        S(LOG_BARE,'p') = LOG_UIPTR, S(LOG_BARE,'n') = LOG_PTR,   S(LOG_BARE,'a') = LOG_UIPTR, /* NVIDIA decoded address extension */
        S(LOG_BARE,'m') = LOG_NOARG, S(LOG_BARE,'l') = LOG_LPRE,  S(LOG_BARE,'h') = LOG_HPRE,  S(LOG_BARE,'L') = LOG_BIGLPRE, S(LOG_BARE,'z') = LOG_ZTPRE,
        S(LOG_BARE,'j') = LOG_JPRE,  S(LOG_BARE,'t') = LOG_ZTPRE;
    }
    {
        /* 1: l-prefixed */
        S(LOG_LPRE,'d') = LOG_LONG,
        S(LOG_LPRE,'i') = LOG_LONG,
        S(LOG_LPRE,'o') = LOG_ULONG,
        S(LOG_LPRE,'u') = LOG_ULONG,
        S(LOG_LPRE,'x') = LOG_ULONG,
        S(LOG_LPRE,'X') = LOG_ULONG,
        S(LOG_LPRE,'c') = LOG_INT,
        S(LOG_LPRE,'s') = LOG_PTR,
        S(LOG_LPRE,'n') = LOG_PTR,
        S(LOG_LPRE,'l') = LOG_LLPRE;
    }
    {
        /* 2: ll-prefixed */
        S(LOG_LLPRE,'d') = LOG_LLONG,
        S(LOG_LLPRE,'i') = LOG_LLONG,
        S(LOG_LLPRE,'o') = LOG_ULLONG,
        S(LOG_LLPRE,'u') = LOG_ULLONG,
        S(LOG_LLPRE,'x') = LOG_ULLONG,
        S(LOG_LLPRE,'X') = LOG_ULLONG,
        S(LOG_LLPRE,'n') = LOG_PTR;
    }
    {
        /* 3: h-prefixed */
        S(LOG_HPRE,'d') = LOG_SHORT,
        S(LOG_HPRE,'i') = LOG_SHORT,
        S(LOG_HPRE,'o') = LOG_USHORT,
        S(LOG_HPRE,'u') = LOG_USHORT,
        S(LOG_HPRE,'x') = LOG_USHORT,
        S(LOG_HPRE,'X') = LOG_USHORT,
        S(LOG_HPRE,'n') = LOG_PTR,
        S(LOG_HPRE,'h') = LOG_HHPRE;
    }
    {
        /* 4: hh-prefixed */
        S(LOG_HHPRE,'d') = LOG_CHAR,
        S(LOG_HHPRE,'i') = LOG_CHAR,
        S(LOG_HHPRE,'o') = LOG_UCHAR,
        S(LOG_HHPRE,'u') = LOG_UCHAR,
        S(LOG_HHPRE,'x') = LOG_UCHAR,
        S(LOG_HHPRE,'X') = LOG_UCHAR,
        S(LOG_HHPRE,'n') = LOG_PTR;
    }
    {
        /* 5: L-prefixed */
        S(LOG_BIGLPRE,'n') = LOG_PTR;
    }
    {
        /* 6: z- or t-prefixed (assumed to be same size) */
        S(LOG_ZTPRE,'d') = LOG_PDIFF,
        S(LOG_ZTPRE,'i') = LOG_PDIFF,
        S(LOG_ZTPRE,'o') = LOG_SIZET,
        S(LOG_ZTPRE,'u') = LOG_SIZET,
        S(LOG_ZTPRE,'x') = LOG_SIZET,
        S(LOG_ZTPRE,'X') = LOG_SIZET,
        S(LOG_ZTPRE,'n') = LOG_PTR;
    }
    {
        /* 7: j-prefixed */
        S(LOG_JPRE,'d') = LOG_IMAX,
        S(LOG_JPRE,'i') = LOG_IMAX,
        S(LOG_JPRE,'o') = LOG_UMAX,
        S(LOG_JPRE,'u') = LOG_UMAX,
        S(LOG_JPRE,'x') = LOG_UMAX,
        S(LOG_JPRE,'X') = LOG_UMAX,
        S(LOG_JPRE,'n') = LOG_PTR;
    }
}

/**
 * @brief Find the Log buffer corresponding to the elf section name given.
 */
static LIBOS_LOG_DECODE_LOG *_findLogBufferFromSectionName(LIBOS_LOG_DECODE *logDecode, const char *elfSectionName)
{
    NvU32 i;
    for (i = 0; i < logDecode->numLogBuffers; ++i)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];
        if (portStringCompare(pLog->elfSectionName, elfSectionName, sizeof(pLog->elfSectionName)) == 0)
            return pLog;
    }
#if defined(LIBOS_LOG_OFFLINE_DECODER)
    for (i = 0; i < LIBOS_LOG_TASK_MAX_ID; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->mergedLogResolver[i];
        if ((pLog->elfSectionName) &&
            (portStringCompare(pLog->elfSectionName, elfSectionName, sizeof(pLog->elfSectionName)) == 0))
            return pLog;
    }
#endif
    return NULL;
}

/**
 *
 * @brief Print out the line buffer and reset the current line buffer pointer.
 *
 * @param[in/out] logDecode
 *   Structure used to decode log.  Contains both data set at init and working fields.
 */
static void flush_line_buffer(LIBOS_LOG_DECODE *logDecode)
{
    if (logDecode->curLineBufPtr != logDecode->lineBuffer)
    {
        NvU8 logLevel = logDecode->lineLogLevel;

        /* Make sure line is NULL terminated */
        *logDecode->curLineBufPtr = 0;

#if defined(NVRM)
        // Sanitize log level for live decoding from RM.
        if (logLevel > NV_LEVEL_MAX)
            logLevel = NV_LEVEL_MAX;

        //
        // FIXME (bug 3924763): by default, nv_printf prints messages at level >= NV_DBG_WARNINGS=3.
        // We want to show all prints >= LEVEL_NOTICE=2, so bump LEVEL_NOTICE=2 to LEVEL_WARNING=3 for now.
        // This will be fixed by remapping log levels correctly.
        // NB: This is not needed for MODS, because LEVEL_NOTICE is still recorded in MODS logs.
        //
        if (logLevel == LEVEL_NOTICE)
            logLevel = LEVEL_WARNING;
#endif

        LIBOS_LOG_DECODE_PRINTF(logLevel, "%s", logDecode->lineBuffer);
        logDecode->curLineBufPtr = logDecode->lineBuffer;
    }
}

/**
 *
 * @brief Copy string to the line buffer.
 *
 * Copy string until 0 encountered up to maximum length l.
 * Flush the line buffer if it gets full.
 *
 * @param[in] s
 *   String to copy.  May be zero-terminated.
 * @param[in] l
 *   Maximum length to copy, if zero is not encountered first.
 * @param[in/out] logDecode
 *   Structure used to decode log.  Contains both data set at init and working fields.
 */
static void emit_string(const char *s, int l, LIBOS_LOG_DECODE *logDecode)
{
    char *line_buffer_end = logDecode->lineBuffer + LIBOS_LOG_LINE_BUFFER_SIZE - 1;
    for (; (l > 0) && (*s != 0); s++)
    {
        if (logDecode->curLineBufPtr >= line_buffer_end)
            flush_line_buffer(logDecode);

        *logDecode->curLineBufPtr++ = *s;
        l--;
    }
}

static NvBool s_getSymbolDataStr(LibosDebugResolver *resolver, char *decodedLine,
    NvLength decodedLineSize, NvUPtr addr, NvBool bPrintDirectory)
{
    const char *directory;
    const char *filename;
    const char *name = NULL;
    NvU64 offset = 0;
    NvU64 outputLine;
    NvU64 outputColumn;
    NvU64 matchedAddress;
    NvBool bResolved = NV_FALSE;

    if (!LibosDebugResolveSymbolToName(resolver, addr, &name, &offset))
    {
        name = NULL;
    }

    decodedLine[decodedLineSize - 1U] = '\0';

    bResolved = LibosDwarfResolveLine(resolver, addr, &directory, &filename, &outputLine,
                                      &outputColumn, &matchedAddress);

    if (bResolved)
    {
        if (name)
        {
            bPrintDirectory &= (directory != NULL);

            snprintf(
                decodedLine, decodedLineSize - 1, "%s+%lld (%s%s%s:%lld)",
                name, offset,
                (bPrintDirectory ? directory : ""),
                (bPrintDirectory ? "/" : ""),
                filename, outputLine);
        }
        else
        {
            snprintf(decodedLine, decodedLineSize - 1, "??? (%s:%lld)", filename, outputLine);
        }
    }
    else if (name)
    {
        snprintf(decodedLine, decodedLineSize - 1, "%s+%lld", name, offset);
    }

    bResolved |= (name != NULL);

    return bResolved;
}

NvBool libosLogSymbolicateAddress(LIBOS_LOG_DECODE *logDecode, char *decodedLine,
                                  NvLength decodedLineSize, NvUPtr addr, NvBool bPrintDirectory)
{
    return s_getSymbolDataStr(&logDecode->log[0].resolver, decodedLine, decodedLineSize, addr, bPrintDirectory);
}

/**
 *
 * @brief Pad a field with ' ' or '0'.
 *
 * This routine is called with different options for left-justified and
 * right-justified fields.
 *
 * @param[in] c
 *   Pad with this character.  Usually '0' or ' '.
 * @param[in] w
 *   Desired width after padding.
 * @param[in] l
 *   Length of field so far.  Pad for w - l.
 * @param[in] fl
 *   Modifier flags.  See FLAGMASK above.
 * @param[in/out] logDecode
 *   Structure used to decode log.  Contains both data set at init and working fields.
 */
static void pad(char c, int w, int l, int fl, LIBOS_LOG_DECODE *logDecode)
{
    char *line_buffer_end = logDecode->lineBuffer + LIBOS_LOG_LINE_BUFFER_SIZE - 1;
    if (fl & (LEFT_ADJ | ZERO_PAD) || l >= w)
        return;
    l = w - l;
    for (; l > 0; l--)
    {
        if (logDecode->curLineBufPtr >= line_buffer_end)
            flush_line_buffer(logDecode);
        *logDecode->curLineBufPtr++ = c;
    }
}
static char *fmt_x(NvU64 x, char *s, int lower)
{
    static const char xdigits[16] = {"0123456789ABCDEF"};
    for (; x; x >>= 4)
        *--s = xdigits[(x & 15)] | lower;
    return s;
}
static char *fmt_o(NvU64 x, char *s)
{
    for (; x; x >>= 3)
        *--s = '0' + (x & 7);
    return s;
}
static char *fmt_u(NvU64 x, char *s)
{
    for (; x > 0; x /= 10)
        *--s = '0' + x % 10;
    return s;
}
static int getint(char **s)
{
    int i;
    for (i = 0; IS_DIGIT(**s); (*s)++)
        i = 10 * i + (**s - '0');
    return i;
}

#if !defined(NVRM)
static const char *printfLevelToString(NvU32 level)
{
    switch (level)
    {
        case LEVEL_SILENT:
            return " SILENT ";
        case LEVEL_INFO:
            return " INFO ";
        case LEVEL_NOTICE:
            return " NOTICE ";
        case LEVEL_WARNING:
            return " WARNING ";
        case LEVEL_ERROR:
            return " ERROR ";
        case LEVEL_HW_ERROR:
            return " HW_ERROR ";
        case LEVEL_FATAL:
            return " FATAL ";
        default:
            return " ";
    }
}
#endif

static int libos_printf_a(
    LIBOS_LOG_DECODE *logDecode, LIBOS_LOG_DECODE_RECORD *pRec, const char *fmt, const char *filename)
{
    NvU64 *args                     = pRec->args;
    NvU64 arg_count                 = pRec->meta->argumentCount;
    union arg nl_arg[NL_ARGMAX + 1] = {{0}};
    char *a, *z, *s = (char *)fmt;
    unsigned l10n = 0, fl;
    int w, p;
    union arg arg = {0};
    int argpos;
    unsigned st, ps;
    int cnt = 0, l = 0;
    char buf[sizeof(NvU64) * 3 + 3];
    const char *prefix;
    int t, pl;
    NvU64 arg_index = 0;
    char wc[2];
    char *line_buffer_end = logDecode->lineBuffer + LIBOS_LOG_LINE_BUFFER_SIZE - 1;
    NvBool bResolvePtrVal = NV_FALSE;
    NvBool bFixedString = NV_FALSE;

    for (;;)
    {
        /* Update output count, end loop when fmt is exhausted */
        if (cnt >= 0)
        {
            if (l > LOG_INT_MAX - cnt)
            {
                cnt = -1;
            }
            else
                cnt += l;
        }

        if (logDecode->curLineBufPtr == logDecode->lineBuffer)
        {
            size_t remain = LIBOS_LOG_LINE_BUFFER_SIZE - 1;
            int len;

#    if defined(NVRM)
            // Prefix every line with NVRM: GPUn Ucode-task: filename(lineNumber):
            char *taskPrefix = pRec->log->taskPrefix;
#if defined(LIBOS_LOG_OFFLINE_DECODER)
            char taskPrefixString[8];

            if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
                && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
            {
                sprintf(&taskPrefixString[0], "%s%c", logDecode->mergedLogResolver[pRec->taskId].taskPrefix, pRec->log->taskPrefix[4]);
                taskPrefix = &taskPrefixString[0];
            }
#endif
            len = snprintf(
                logDecode->curLineBufPtr, remain,
                NV_PRINTF_ADD_PREFIX("GPU%u %s-%s: %s(%u): "), pRec->log->gpuInstance,
                logDecode->sourceName, taskPrefix, filename, pRec->meta->lineNumber);

            if (len < 0)
            {
                return -1;
            }
            if ((size_t)len >= remain)
            {
                // Truncated.
                remain = 0;
                logDecode->curLineBufPtr = line_buffer_end;
            }
            else
            {
                remain -= len;
                logDecode->curLineBufPtr += len;
            }
#    else
            // Prefix every line with T:nnnn GPUn Ucode-task: filename(lineNumber):
            len = snprintf(
                logDecode->curLineBufPtr, remain,
                "T:%llu ", pRec->timeStamp + pRec->log->localToGlobalTimerDelta);
            if (len < 0)
            {
                return -1;
            }
            if ((size_t)len >= remain)
            {
                // Truncated.
                remain = 0;
                logDecode->curLineBufPtr = line_buffer_end;
            }
            else
            {
                remain -= len;
                logDecode->curLineBufPtr += len;
            }

#if defined(NVSYM_STANDALONE)
            {
                struct tm   tmStruct;
                // Libos timestamp is a PTIMER value, which is UNIX time in ns
                time_t      timeSec   = (pRec->timeStamp + pRec->log->localToGlobalTimerDelta) / 1000000000;
#if NVOS_IS_WINDOWS
                // "The implementation of localtime_s in Microsoft CRT is incompatible with the
                // C standard since it has reversed parameter order and returns errno_t."
                errno_t     err       = localtime_s(&tmStruct, &timeSec);
#else
                int err = (localtime_r(&timeSec, &tmStruct) == NULL);
#endif
                if (err == 0)
                {
                    len = snprintf(logDecode->curLineBufPtr, remain,
                                   "%04d-%02d-%02d %02d:%02d:%02d ",
                                   tmStruct.tm_year + 1900,    // years since 1900
                                   tmStruct.tm_mon + 1,        // months since January - [0,11]
                                   tmStruct.tm_mday,           // day of the month - [1,31]
                                   tmStruct.tm_hour,           // hours since midnight - [0,23]
                                   tmStruct.tm_min,            // minutes after the hour - [0,59]
                                   tmStruct.tm_sec);           // seconds after the minute - [0,59]
                    if (len < 0)
                    {
                        return -1;
                    }
                    if ((size_t)len >= remain)
                    {
                        // Truncated.
                        remain = 0;
                        logDecode->curLineBufPtr = line_buffer_end;
                    }
                    else
                    {
                        remain -= len;
                        logDecode->curLineBufPtr += len;
                    }
                }
        }
#endif
            char *taskPrefix = pRec->log->taskPrefix;
#if defined(LIBOS_LOG_OFFLINE_DECODER)
            char taskPrefixString[8];

            if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
                && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
            {
                sprintf(&taskPrefixString[0], "%s%c", logDecode->mergedLogResolver[pRec->taskId].taskPrefix, pRec->log->taskPrefix[4]);
                taskPrefix = &taskPrefixString[0];
            }
#endif
            len = snprintf(
                logDecode->curLineBufPtr, remain,
                "GPU%u%s%s-%s: %s(%u): ",
                pRec->log->gpuInstance, printfLevelToString(pRec->meta->printLevel),
                 logDecode->sourceName, taskPrefix,
                filename, pRec->meta->lineNumber);
            if (len < 0)
            {
                return -1;
            }
            if ((size_t)len >= remain)
            {
                // Truncated.
                remain = 0;
                logDecode->curLineBufPtr = line_buffer_end;
            }
            else
            {
                remain -= len;
                logDecode->curLineBufPtr += len;
            }
#    endif

            len = 0;
            switch (pRec->meta->specialType)
            {
                case RM_GSP_LOG_SPECIAL_NONE:
                    break;
                case RM_GSP_LOG_SPECIAL_ASSERT_FAILED:
                    // Add "Assertion failed: " prefix, and interpret the "format"
                    // as a fixed string.
                    len = snprintf(
                        logDecode->curLineBufPtr, remain,
                        "Assertion failed: ");
                    bFixedString = NV_TRUE;
                    break;
                case RM_GSP_LOG_SPECIAL_ASSERT_OK_FAILED:
                {
                    // Add "Assertion failed: <status> (0xNN) returned from "
                    // prefix, and interpret the "format" as a fixed string.
                    NV_STATUS status;
                    if (arg_index >= arg_count)
                    {
                        return 0;
                    }
                    status = args[arg_index++];
                    len = snprintf(
                        logDecode->curLineBufPtr, remain,
                        "Assertion failed: %s (0x%08X) returned from ",
                        nvstatusToString(status), status);
                    bFixedString = NV_TRUE;
                    break;
                }
                case RM_GSP_LOG_SPECIAL_CHECK_FAILED:
                    // Add "Check failed: " prefix, and interpret the "format"
                    // as a fixed string.
                    len = snprintf(
                        logDecode->curLineBufPtr, remain,
                        "Check failed: ");
                    bFixedString = NV_TRUE;
                    break;
                case RM_GSP_LOG_SPECIAL_CHECK_OK_FAILED:
                {
                    // Add "Check failed: <status> (0xNN) returned from "
                    // prefix, and interpret the "format" as a fixed string.
                    NV_STATUS status;
                    if (arg_index >= arg_count)
                    {
                        return 0;
                    }
                    status = args[arg_index++];
                    len = snprintf(
                        logDecode->curLineBufPtr, remain,
                        "Check failed: %s (0x%08X) returned from ",
                        nvstatusToString(status), status);
                    bFixedString = NV_TRUE;
                    break;
                }
            }
            if (len < 0)
            {
                return -1;
            }
            if ((size_t)len >= remain)
            {
                // Truncated.
                remain = 0;
                logDecode->curLineBufPtr = line_buffer_end;
            }
            else
            {
                remain -= len;
                logDecode->curLineBufPtr += len;
            }
        }

        /* Handle literal text and %% format specifiers */
        for (; *s; s++)
        {
            if (logDecode->curLineBufPtr >= line_buffer_end)
                flush_line_buffer(logDecode);

            if (!bFixedString && (*s == '%'))
            {
                if (s[1] == '%')
                    s++;
                else
                    break;
            }

            *logDecode->curLineBufPtr++ = *s;

            if (*s == '\n')
                break;
        }

        if (*s == '\n')
        {
            flush_line_buffer(logDecode);
            s++;

            if (!*s)
                break;
            else
                continue;
        }
        if (!*s)
            break;

        a = s;
        z = s;
        l = z - a;
        if (IS_DIGIT(s[1]) && s[2] == '$')
        {
            l10n   = 1;
            argpos = s[1] - '0';
            s += 3;
        }
        else
        {
            argpos = -1;
            s++;
        }
        /* Read modifier flags */
        for (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & (1U << (*s - ' '))); s++)
            fl |= 1U << (*s - ' ');
        /* Read field width */
        if (*s == '*')
        {
            if (IS_DIGIT(s[1]) && s[2] == '$')
            {
                l10n = 1;
                w    = nl_arg[s[1] - '0'].i;
                s += 3;
            }
            else if (!l10n)
            {
                if (arg_index >= arg_count)
                    return 0;
                w = args[arg_index++];
                s++;
            }
            else
                return -1;
            if (w < 0)
                fl |= LEFT_ADJ, w = -w;
        }
        else if ((w = getint(&s)) < 0)
            return -1;
        /* Read precision */
        if (*s == '.' && s[1] == '*')
        {
            if (IS_DIGIT(s[2]) && s[3] == '$')
            {
                p = nl_arg[s[2] - '0'].i;
                s += 4;
            }
            else if (!l10n)
            {
                if (arg_index >= arg_count)
                    return 0;
                p = args[arg_index++];
                s += 2;
            }
            else
                return -1;
        }
        else if (*s == '.')
        {
            s++;
            p = getint(&s);
        }
        else
            p = -1;
        /* Format specifier state machine */
        st = 0;
        do
        {
            if (OOB(*s))
                return -1;
            ps = st;
            st = S(st, *s++);
        } while (st - 1 < LOG_STOP);
        if (!st)
            return -1;
        /* Check validity of argument type (nl/normal) */
        if (st == LOG_NOARG)
        {
            if (argpos >= 0)
                return -1;
        }
        else
        {
            if (argpos < 0)
            {
                if (arg_index >= arg_count)
                    return 0;
                arg.i = args[arg_index++];
            }
        }
        z      = buf + sizeof(buf);
        prefix = "-+   0X0x";
        pl     = 0;
        t      = s[-1];
        /* Transform ls,lc -> S,C */
        if (ps && (t & 15) == 3)
            t &= ~32;
        /* - and 0 flags are mutually exclusive */
        if (fl & LEFT_ADJ)
            fl &= ~ZERO_PAD;

        bResolvePtrVal = NV_FALSE;
        switch (t)
        {
        case 'n':
            // We can't support %n when decoding, these pointers do not exist here!
            continue;
        case 'p':
            t = 'x';
            fl |= ALT_FORM;

            if (logDecode->bPtrSymbolResolve)
            {
                bResolvePtrVal = NV_TRUE;
            }
        case 'x':
        case 'X':
            a = fmt_x(arg.i, z, t & 32);
            if (arg.i && (fl & ALT_FORM))
                prefix += (t >> 4), pl = 2;
            if (0)
            {
            case 'o':
                a = fmt_o(arg.i, z);
                if ((fl & ALT_FORM) && arg.i)
                    prefix += 5, pl = 1;
            }
            if (0)
            {
            case 'd':
            case 'i':
                pl = 1;
                if (arg.i > LOG_INTMAX_MAX)
                {
                    arg.i = -(NvS64)arg.i;
                }
                else if (fl & MARK_POS)
                {
                    prefix++;
                }
                else if (fl & PAD_POS)
                {
                    prefix += 2;
                }
                else
                    pl = 0;
            case 'u':
                a = fmt_u(arg.i, z);
            }
            if (p >= 0)
                fl &= ~ZERO_PAD;
            if (!arg.i && !p)
            {
                a = z;
                break;
            }
            p = NV_MAX(p, (int)(NvSPtr)(z - a + !arg.i));
            break;
        case 'c':
            *(a = z - (p = 1)) = arg.i;
            fl &= ~ZERO_PAD;
            break;
        case 'a':
            {
                static char symDecodedLine[SYM_DECODED_LINE_MAX_SIZE];

                s_getSymbolDataStr(&pRec->logSymbolResolver->resolver, symDecodedLine, sizeof(symDecodedLine), (NvUPtr)arg.i, NV_FALSE);

                // Set common vars
                a = &symDecodedLine[0];
                z = &symDecodedLine[sizeof(symDecodedLine) - 1];
            }
            goto print_string;
        case 's':
#if defined(LIBOS_LOG_OFFLINE_DECODER)
            if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
                && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
            {
                a = (char *)LibosElfMapVirtualString(&logDecode->mergedLogResolver[pRec->taskId].elfImage,
                                                        (NvUPtr)arg.p, logDecode->bDecodeStrShdr);
            }
            else
#endif
            {
                a = (char *)LibosElfMapVirtualString(&pRec->log->elfImage, (NvUPtr)arg.p, logDecode->bDecodeStrShdr);
            }

            if (!a)
                a = (char *)"(bad-pointer)";
        print_string:
            p = portStringLengthSafe(a, p);
            z = a + p;
            fl &= ~ZERO_PAD;
            break;
        case 'C':
            wc[0] = arg.i;
            wc[1] = 0;
            arg.p = wc;
            p     = -1;
        }
        if (p < z - a)
            p = z - a;
        if (w < pl + p)
            w = pl + p;
        pad(' ', w, pl + p, fl, logDecode);
        emit_string(prefix, pl, logDecode);
        pad('0', w, pl + p, fl ^ ZERO_PAD, logDecode);
        pad('0', p, z - a, 0, logDecode);
        emit_string(a, z - a, logDecode);

        if (bResolvePtrVal)
        {
            // Append symbol info to ptr addr value in the following format: 0x123 <sym+data>

            static char symDecodedLine[SYM_DECODED_LINE_MAX_SIZE];
            NvLength symDecodedLineLen;
            NvU32 prefixLen = 0;
            NvU32 suffixLen = 0;

            portMemSet(symDecodedLine, 0, SYM_DECODED_LINE_MAX_SIZE);

            symDecodedLine[prefixLen++] = ' ';

#ifdef NVWATCH
            // Windbg nvwatch uses DML, so < becomes &lt;
            prefixLen += sprintf(symDecodedLine + prefixLen, "&lt;");
#else // NVWATCH
            symDecodedLine[prefixLen++] = '<';
#endif // NVWATCH

#if defined(LIBOS_LOG_OFFLINE_DECODER)
            if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
                && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
            {
                s_getSymbolDataStr(&logDecode->mergedLogResolver[pRec->taskId].resolver, symDecodedLine + prefixLen, sizeof(symDecodedLine) - prefixLen, (NvUPtr)arg.i, NV_FALSE);
            }
            else
#endif
            {
                s_getSymbolDataStr(&pRec->log->resolver, symDecodedLine + prefixLen, sizeof(symDecodedLine) - prefixLen, (NvUPtr)arg.i, NV_FALSE);
            }


            symDecodedLineLen = portStringLength(symDecodedLine);
            symDecodedLineLen = NV_MIN(symDecodedLineLen, sizeof(symDecodedLine) - 1); // just in case

#ifdef NVWATCH
            // Windbg nvwatch uses DML, so > becomes &gt;
            suffixLen += sprintf(symDecodedLine + symDecodedLineLen + suffixLen, "&gt;");
#else // NVWATCH
            symDecodedLine[symDecodedLineLen + (suffixLen++)] = '>';
#endif // NVWATCH

            symDecodedLine[symDecodedLineLen + suffixLen] = '\0';

            // Set common vars
            a = &symDecodedLine[0];
            z = &symDecodedLine[sizeof(symDecodedLine) - 1];

            emit_string(a, z - a, logDecode);
        }

        pad(' ', w, pl + p, fl ^ LEFT_ADJ, logDecode);
        l = w;
    }
    return cnt;
}

/**
 *
 * @brief Print log records from scratch buffer.
 *
 * Prints log records from the scratch buffer scanning forwards.
 *
 * @param[in/out] logDecode
 *   Pointer to LIBOS_LOG_DECODE structure.
 * @param[in] scratchBuffer
 *   Pointer to first byte in scratch buffer that contains valid data.
 * @param[int] valid_elements
 *   Number of valid elements in the scratch buffer.
 */
static void libosPrintLogRecords(LIBOS_LOG_DECODE *logDecode, NvU64 *scratchBuffer, NvU64 valid_elements)
{
    NvU64 index;
    NvU64 i;

    if (valid_elements == 0)
        return;

    for (index = 0; index < valid_elements;)
    {
        LIBOS_LOG_DECODE_RECORD *pRec = (LIBOS_LOG_DECODE_RECORD *)&scratchBuffer[index];
        const char *format;
        const char *filename;

        if (pRec->meta == NULL)
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING,
                "**** Bad metadata.  Lost %lld entries from %s ****\n", valid_elements - index,
                logDecode->sourceName);
            return;
        }

        logDecode->lineLogLevel = pRec->meta->printLevel;


#if defined(LIBOS_LOG_OFFLINE_DECODER)
        if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
            && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
        {
            format = LibosElfMapVirtualString(&logDecode->mergedLogResolver[pRec->taskId].elfImage,
                                                (NvU64)(NvUPtr)pRec->meta->format, NV_TRUE);
        }
        else
#endif
        {
            // Locate format string
            format = LibosElfMapVirtualString(&pRec->log->elfImage, (NvU64)(NvUPtr)pRec->meta->format, NV_TRUE);
        }

        if (!format)
            break;

#if defined(LIBOS_LOG_OFFLINE_DECODER)
        if ((pRec->log->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
            && (pRec->taskId != LIBOS_LOG_TASK_UNKNOWN))
        {
            filename = LibosElfMapVirtualString(&logDecode->mergedLogResolver[pRec->taskId].elfImage,
                                                (NvU64)(NvUPtr)pRec->meta->filename, NV_TRUE);
        }
        else
#endif
        {
            // Locate filename
            filename = LibosElfMapVirtualString(&pRec->log->elfImage, (NvU64)(NvUPtr)pRec->meta->filename, NV_TRUE);
        }

        if (!filename || !filename[0])
            filename = "unknown";

        // Strip off path
        for (i = portStringLength(filename) - 1; i > 0; i--)
        {
            if (filename[i] == '/')
            {
                i++;
                break;
            }
        }
        filename = &filename[i];

        // Format
        libos_printf_a(logDecode, pRec, format, filename);

        // Advance
        index += pRec->meta->argumentCount + LIBOS_LOG_DECODE_RECORD_BASE;
    }
}

/**
 * @brief Fetch the correct metadata from the given index in the physical buffer.
 *        Initialize elfSectionName appropriately in case extended version of metadata is found.
 */
static libosLogMetadata *_getLoggingMetadata(LIBOS_LOG_DECODE *logDecode, LIBOS_LOG_DECODE_LOG *pLog, NvU64 idx, const char **elfSectionName, NvU32 *logEntrySize, NvU32 *taskId)
{
    libosLogMetadata *pMetadata = NULL;
    libosLogMetadata_extended *pMetadataEx = NULL;
    const char *filename = NULL;
    *elfSectionName = NULL;
    NvU64 metadataVA; 
    NvU64 meta = pLog->physicLogBuffer[idx];
    LIBOS_LOG_DECODE_LOG *pLogLocal = pLog;

    if ((pLog->libosLogNvlogBufferVersion >= LIBOS_LOG_NVLOG_BUFFER_VERSION_2) &&
        ((pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_PACKED_METADATA) != 0) &&
        (REF_VAL64(LIBOS_LOG_METADATA_TOTAL_ARGS, meta) >= LIBOS_LOG_ENTRY_MIN_SIZE) &&
        (REF_VAL64(LIBOS_LOG_METADATA_TOTAL_ARGS, meta) <= LIBOS_LOG_ENTRY_MAX_SIZE))
    {
        *logEntrySize = REF_VAL64(LIBOS_LOG_METADATA_TOTAL_ARGS, meta);
        *taskId = REF_VAL64(LIBOS_LOG_METADATA_TASK_ID, meta);

#if defined(LIBOS_LOG_OFFLINE_DECODER)
        if (pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
            pLogLocal = &logDecode->mergedLogResolver[*taskId];
#endif

        metadataVA = REF_VAL64(LIBOS_LOG_METADATA_VA, meta) + pLogLocal->loggingBaseAddress;
    }
    else
    {
        metadataVA = pLogLocal->physicLogBuffer[idx];
        *taskId = LIBOS_LOG_TASK_UNKNOWN;
    }

    if (metadataVA < pLogLocal->loggingBaseAddress ||
        metadataVA >= pLogLocal->loggingBaseAddress + pLogLocal->loggingSize)
    {
        return NULL;
    }

    pMetadata = (libosLogMetadata *) LibosElfMapVirtual(&pLogLocal->elfImage, metadataVA, sizeof(libosLogMetadata));
    if (pMetadata == NULL)
        return NULL;

    // Try to fetch filename to check if we received an extended metadata
    filename = LibosElfMapVirtualString(&pLogLocal->elfImage, (NvU64)(NvUPtr)pMetadata->filename, NV_TRUE);
    if ((filename == NULL) && ((NvU64) pMetadata->filename == 0x8000000000000000ULL))
    {
        // Filename wasn't mapped correctly. That means this is an extended metadata.
        pMetadataEx = (libosLogMetadata_extended *) LibosElfMapVirtual(&pLogLocal->elfImage, metadataVA, sizeof(libosLogMetadata_extended));
        if (pMetadataEx == NULL)
            return NULL;
        pMetadata = &pMetadataEx->meta;

        // Fetch the remote task's elf section name string.
        if (pMetadataEx->elfSectionName != NULL)
        {
            *elfSectionName = LibosElfMapVirtualString(&pLogLocal->elfImage, (NvU64)(NvUPtr)pMetadataEx->elfSectionName, NV_TRUE);
        }
    }

    return pMetadata;
}

#    define LIBOS_LOG_TIMESTAMP_END  0
#    define LIBOS_LOG_TIMESTAMP_MAX  NV_U64_MAX

/**
 *
 * @brief Extract a single log record from one log buffer.
 *
 * This routine is designed to scan backwards from the put pointer.  It changes
 * the order of the parameters from backward scanning order {args, meta data,
 * timestamp} to forward scanning order {pLog, meta data, timestamp args}.
 * It also decodes meta data into a pointer.
 *
 * pLog->putIter points to the start of the entry last successfully extracted.
 *
 * @param[in/out] logDecode
 *   Pointer to LIBOS_LOG_DECODE structure.
 * @param[in/out] pLog
 *   Pointer to LIBOS_LOG_DECODE_LOG structure for the log to extract from.
 *
 * timeStamp is set to LIBOS_LOG_TIMESTAMP_END (0) when there is an error or we
 *   run out of records.
 */
static void libosExtractLog_ReadRecord(LIBOS_LOG_DECODE *logDecode, LIBOS_LOG_DECODE_LOG *pLog)
{
    NvU64 log_entries = pLog->logBufferSize / sizeof(NvU64) - 1 /* -1 for PUT pointer */;
    NvU64 previousPut = pLog->previousPut;
    NvU64 i;
    NvU64 argCount;
    NvU64 j;
    const char *elfSectionName;

    while (1)
    {
        // Number of NvU64 entries for this record
        NvU32 logEntrySize = 0;
        NvU32 taskId = 0;

        i = pLog->putIter;

        if (pLog->putIter == pLog->previousPut)
        {
            pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_END;
            return;
        }
        if (pLog->elf == NULL) // Can't decode if task ELF isn't initialized
        {
#if defined(LIBOS_LOG_OFFLINE_DECODER)
            if ((pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER) == 0)
#endif
            {
                pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_END;
                return;
            }
        }

        // If we wrapped, adjust local copy of previousPut.
        if (previousPut + log_entries < pLog->putCopy)
            previousPut = pLog->putCopy - log_entries;

        if (logDecode->bSynchronousBuffer)
        {
            // Fake timestamp for sync buffer, marked as different from the "end" timestamp
            pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_MAX;
        }
        else
        {
            // Check whether record goes past previous put (buffer wrapped).
            if (i < previousPut + 1)
                goto buffer_wrapped;

            pLog->record.timeStamp = pLog->physicLogBuffer[1 + (--i % log_entries)];
        }

        // Check whether record goes past previous put (buffer wrapped).
        if (i < previousPut + 1)
            goto buffer_wrapped;

        pLog->record.meta = _getLoggingMetadata(logDecode, pLog, 1 + (--i % log_entries), &elfSectionName, &logEntrySize, &taskId);

        pLog->record.log = pLog;
        pLog->record.taskId = taskId;

        // Sanity check meta data.
        if (pLog->record.meta != NULL && 
            pLog->record.meta->argumentCount <= LIBOS_LOG_MAX_ARGS &&
            ((taskId == LIBOS_LOG_TASK_UNKNOWN) || (pLog->record.meta->argumentCount == logEntrySize - 2)))
        {
            // Found valid record
            break;
        }

        if (logDecode->bSynchronousBuffer || (taskId == LIBOS_LOG_TASK_UNKNOWN))
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING,
                "**** Bad metadata.  Lost %lld entries from %s-%s ****\n", pLog->putIter - previousPut,
                logDecode->sourceName, pLog->taskPrefix);
            goto error_ret;
        }

        LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING,
            "**** Bad metadata.  Lost %lld entries from %s-%s ****\n", logEntrySize,
            logDecode->sourceName, pLog->taskPrefix);
        pLog->putIter -= logEntrySize;
    }

    pLog->record.logSymbolResolver = pLog;
    if (elfSectionName != NULL)
    {
        // This log is in the context of another task.
        LIBOS_LOG_DECODE_LOG *pLogSymbolResolver;

        // Initialize the correct Log Buffer that will be used to resolve symbols
        pLogSymbolResolver = _findLogBufferFromSectionName(logDecode, elfSectionName);
        if ((pLogSymbolResolver != NULL) && (pLogSymbolResolver->elf != NULL))
        {
            pLog->record.logSymbolResolver = pLogSymbolResolver; // Valid log buffer found
        }
    }

    argCount = pLog->record.meta->argumentCount;

    // Check whether record goes past previous put (buffer wrapped).
    if (i < previousPut + argCount)
        goto buffer_wrapped;

    for (j = argCount; j > 0; j--)
    {
        pLog->record.args[j - 1] = pLog->physicLogBuffer[1 + (--i % log_entries)];
    }

    pLog->putIter = i;
    return;

buffer_wrapped:
    // Put pointer wrapped and caught up to us.  This means we lost entries.
    LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING,
        "**** Buffer wrapped. Lost %lld entries from %s-%s ****\n", pLog->putIter - pLog->previousPut,
        logDecode->sourceName, pLog->taskPrefix);

error_ret:
    pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_END;
    return;
}

/**
 *
 * @brief Extract all log records from all log buffers.
 *
 * Copy log records from all buffers to the scratch buffer in order of time stamp.
 *
 * @param[in/out] logDecode
 *   Pointer to LIBOS_LOG_DECODE structure.
 */
static void libosExtractLogs_decode(LIBOS_LOG_DECODE *logDecode)
{
    LIBOS_LOG_DECODE_LOG *pLog;
    LIBOS_LOG_DECODE_RECORD *pPrevRec = NULL;
    NvU64 timeStamp;
    NvU64 scratchSize = logDecode->scratchBufferSize / sizeof(NvU64);
    NvU64 dst         = scratchSize;
    NvU64 recSize;
    NvU64 i;

    // Initialize iterators and prime the pump.
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        pLog = &logDecode->log[i];

        if (!pLog->physicLogBuffer)
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "logDecode->physicLogBuffer is NULL\n");
            return;
        }

        pLog->putIter = pLog->putCopy;
        libosExtractLog_ReadRecord(logDecode, pLog);
    }

    // Copy records in order of highest time stamp.
    for (;;)
    {
        timeStamp = LIBOS_LOG_TIMESTAMP_END;
        pLog      = NULL; // for debugging.

        // Find log with the highest timestamp.
        for (i = 0; i < logDecode->numLogBuffers; i++)
        {
            if (timeStamp < logDecode->log[i].record.timeStamp)
            {
                pLog      = &logDecode->log[i];
                timeStamp = pLog->record.timeStamp;
            }
        }

        if (timeStamp == LIBOS_LOG_TIMESTAMP_END)
            break;

        // Copy records with highest timestamp.
        recSize = pLog->record.meta->argumentCount + LIBOS_LOG_DECODE_RECORD_BASE;

        //
        // Skip duplicate records.  The same record can be in both wrap and nowrap buffers.
        // memcmp LIBOS_LOG_DECODE_RECORD + args starting at LIBOS_LOG_DECODE_RECORD::meta.
        //
        NvU64 compareSz = recSize - offsetof(LIBOS_LOG_DECODE_RECORD, meta) / sizeof(NvU64);
        if ((pPrevRec == NULL) ||
            (pPrevRec->log->gpuInstance != pLog->gpuInstance) ||
            (portMemCmp(&pPrevRec->meta, &pLog->record.meta, sizeof(NvU64) * compareSz) != 0))
        {
            // Record is not identical to previous record.
            if (dst < recSize)
            {
                LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING, "**** scratch buffer overflow.  lost entries ****\n");
                break;
            }

            dst -= recSize;
            portMemCopy(
                &logDecode->scratchBuffer[dst], recSize * sizeof(NvU64), &pLog->record,
                recSize * sizeof(NvU64));

            pPrevRec = (LIBOS_LOG_DECODE_RECORD *)&logDecode->scratchBuffer[dst];
        }

        // Read in the next record from the log we just copied.
        libosExtractLog_ReadRecord(logDecode, pLog);
    }

    // Print out the copied records.
    if (dst != scratchSize)
        libosPrintLogRecords(logDecode, &logDecode->scratchBuffer[dst], scratchSize - dst);
}

#endif // LIBOS_LOG_DECODE_ENABLE

#if LIBOS_LOG_TO_NVLOG

#    define LIBOS_LOG_NVLOG_BUFFER_TAG(_name, _i) NvU32_BUILD((_name)[2], (_name)[1], (_name)[0], (_i < 9) ? (NvU8)('1' + _i) : (NvU8)('a' + _i - 9))

static NvBool libosCopyLogToNvlog_nowrap(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogNoWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogNoWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pNoWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 putOffset                    = pLog->putCopy * sizeof(NvU64) + sizeof(NvU64);

    //
    // If RM was not unloaded, we will reuse a preserved nowrap nvlog buffer with the fresh
    // physical log buffer. In this case, we fix up all the offsets into the nvlog buffer to be
    // relative to its preserved position rather than the start.
    // nvlogPos here is the old putOffset, where putOffset should include size of putPointer
    // as well, so need to add sizeof(NvU64) after taking differents between pos and preserved
    //
    NvU64 nvlogPos                     = pNvLogBuffer->pos - pLog->preservedNoWrapPos + sizeof(NvU64);

    if (putOffset < nvlogPos)
    {
        // Buffer put counter unexpectedly reset. Terminate nowrap log collection.
        return NV_FALSE;
    }

    if (putOffset == nvlogPos)
    {
        // No new data
        return NV_TRUE;
    }

    NvU64 len  = putOffset - nvlogPos;

    if (pNvLogBuffer->pos + len >= pNvLogBuffer->size )
    {
        // we are done filling nowrap
        return NV_FALSE;
    }

    NvU8 *pSrc = ((NvU8 *)pLog->physicLogBuffer) + nvlogPos;
    NvU8 *pDst = pNoWrapBuf->data + pNvLogBuffer->pos - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);

    pLog->bDidPush = NV_TRUE;

    portMemCopy(pDst, len, pSrc, len);

    // preservedNoWrapPos already accounted for both LIBOS buffer header and put pointer
    pNvLogBuffer->pos            = putOffset + pLog->preservedNoWrapPos - sizeof(NvU64);
    *(NvU64 *)(pNoWrapBuf->data) = pLog->putCopy + (pLog->preservedNoWrapPos-NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data)-sizeof(NvU64)) / sizeof(NvU64);

    return NV_TRUE;
}

static NvBool libosCopyLogToNvlog_wrap(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 logEntries                 = pLog->logBufferSize / sizeof(NvU64) - 1 /* -1 for PUT pointer */;
    NvU64 putOffset                  = pLog->putCopy * sizeof(NvU64) + sizeof(NvU64);

    NvU64 prevPutOffset              = pNvLogBuffer->pos - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);
    NvU64 nvlogPos                   = (prevPutOffset - sizeof(NvU64)) % (logEntries * sizeof(NvU64)) + sizeof(NvU64);
    NvU64 putOffsetWrapped           = (pLog->putCopy % logEntries) * sizeof(NvU64) + sizeof(NvU64);

    if (putOffsetWrapped == nvlogPos)
    {
        // No new data
        return NV_TRUE;
    }

    if (putOffsetWrapped < nvlogPos)
    {
        // ring buffer wrapped around since last dump
        // copy from pNvLogBuffer->pos to end of buffer, update nvlogPos to 0
        NvU64 len  = pNvLogBuffer->size - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) - nvlogPos;
        NvU8 *pSrc = ((NvU8 *)pLog->physicLogBuffer) + nvlogPos;
        NvU8 *pDst = pWrapBuf->data + nvlogPos;
        portMemCopy(pDst, len, pSrc, len);
        nvlogPos = sizeof(NvU64);
    }

    if (putOffsetWrapped > nvlogPos)
    {
        NvU64 len  = putOffsetWrapped - nvlogPos;
        NvU8 *pSrc = ((NvU8 *)pLog->physicLogBuffer) + nvlogPos;
        NvU8 *pDst = pWrapBuf->data + nvlogPos;
        portMemCopy(pDst, len, pSrc, len);
    }

    pNvLogBuffer->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + putOffset;
    *(NvU64 *)(pWrapBuf->data) = pLog->putCopy;

    return NV_TRUE;
}

static NvBool libosSyncNvlog(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 putOffset                  = pLog->putCopy * sizeof(NvU64) + sizeof(NvU64);

    portMemCopy(pWrapBuf->data, pLog->logBufferSize, (void *)pLog->physicLogBuffer, pLog->logBufferSize);
    pNvLogBuffer->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + putOffset;
    return NV_TRUE;
}

/**
 *
 * @brief Functions for merge log entries from multiple libos log buffers
 *
 */
static void libosMergeNvlog(LIBOS_LOG_DECODE *logDecode, NvU64 *totalNumNewEntries)
{
    NvU64 mergedBufferSize = logDecode->mergedBufferSize / sizeof(NvU64);
    NvU64 dst         = mergedBufferSize;
    NvU64 numNewEntries = 0;

    if (logDecode->mergedBuffer == NULL)
        return;

    // Copy records in order of highest time stamp.
    for (;;)
    {
        LIBOS_LOG_DECODE_LOG *pLog      = NULL; // for debugging.
        NvU64 timeStamp = LIBOS_LOG_TIMESTAMP_END;
        NvU64 previousPut;
        NvU64 numLogEntries;

        // Find log with the highest timestamp.
        for (NvU64 i = 0; i < logDecode->numLogBuffers; i++)
        {
            LIBOS_LOG_DECODE_LOG *pLogLocal = &logDecode->log[i];
            NvU64 previousPutLocal = pLogLocal->previousPut;
            NvU64 numLogEntriesLocal = pLogLocal->logBufferSize / sizeof(NvU64) - 1;

            if (pLogLocal->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
                continue;
            if ((pLogLocal->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER) == 0)
                continue;

            // If we wrapped, adjust local copy of previousPut.
            if (previousPutLocal + numLogEntriesLocal < pLogLocal->putCopy)
                previousPutLocal = pLogLocal->putCopy - numLogEntriesLocal;

            if (pLogLocal->putIter <= previousPutLocal)
                continue;

            NvU64 lastEntryTimestampIndex = (pLogLocal->putIter - 1) % numLogEntriesLocal + 1;

            NvU64 localTimeStamp = logDecode->log[i].physicLogBuffer[lastEntryTimestampIndex];
            if (timeStamp < localTimeStamp)
            {
                timeStamp = localTimeStamp;
                pLog      = &logDecode->log[i];
                previousPut = previousPutLocal;
                numLogEntries = numLogEntriesLocal;
            }
        }

        if (timeStamp == LIBOS_LOG_TIMESTAMP_END)
            break;

        NvU64 lastEntryMetaIndex = (pLog->putIter - 2) % numLogEntries + 1;
        NvU32 logEntrySize = REF_VAL64(LIBOS_LOG_METADATA_TOTAL_ARGS, pLog->physicLogBuffer[lastEntryMetaIndex]);

        if ((logEntrySize < LIBOS_LOG_ENTRY_MIN_SIZE) ||
            (logEntrySize > LIBOS_LOG_ENTRY_MAX_SIZE))
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "libosMergeNvlog: Invalid logEntrySize: %d, merged buffer configured incorrectly!\n", logEntrySize);
            break;
        }

        if (dst <= logEntrySize)
        {
            break;
        }

        pLog->putIter -= logEntrySize;

        if (pLog->putIter <= previousPut)
            continue;

        dst -= logEntrySize;
        pLog->bDidPush = NV_TRUE;
        NvU64 lastEntryStartOffset = (pLog->putIter % numLogEntries + 1) * sizeof(NvU64);

        NvU8 *pSrc = ((NvU8 *)pLog->physicLogBuffer) + lastEntryStartOffset;
        NvU8 *pDst = (NvU8 *)&logDecode->mergedBuffer[dst];
        NvU64 len = logEntrySize * sizeof(NvU64);

        if (lastEntryStartOffset + logEntrySize * sizeof(NvU64) >= pLog->logBufferSize)
        {
            portMemCopy(pDst, pLog->logBufferSize - lastEntryStartOffset, pSrc, pLog->logBufferSize - lastEntryStartOffset);
            len -= pLog->logBufferSize - lastEntryStartOffset;
            pSrc = ((NvU8 *)pLog->physicLogBuffer) + sizeof(NvU64);
            pDst = ((NvU8 *)&logDecode->mergedBuffer[dst]) + pLog->logBufferSize - lastEntryStartOffset;
        }

        portMemCopy(pDst, len, pSrc, len);

        numNewEntries += logEntrySize;
    }

    *totalNumNewEntries = numNewEntries;
}

/**
 *
 * @brief Functions for copying log entries to merged nvlog buffers
 *
 * Similar to libosCopyLogToNvlog_nowrap and libosCopyLogToNvlog_wrap
 * But this function handles merged log instead of coping from single libos log buffer
 */

static NvBool libosCopyLogToMergeNvlog_Nowrap(LIBOS_LOG_DECODE *logDecode, NVLOG_BUFFER *pNvLogBuffer, NvU64 totalNumNewEntries)
{
    LIBOS_LOG_NVLOG_BUFFER *pNoWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 newEntriesSize  = totalNumNewEntries * sizeof(NvU64);

    if (totalNumNewEntries == 0)
        return NV_TRUE;

    if (pNvLogBuffer->pos + newEntriesSize > pNvLogBuffer->size)
    {
        // we are done filling nowrap
        return NV_FALSE;
    }

    NvU64 mergedBufferTotalEntries = logDecode->mergedBufferSize / sizeof(NvU64);

    NvU8 *pSrc = (NvU8 *)&logDecode->mergedBuffer[mergedBufferTotalEntries - totalNumNewEntries];
    NvU8 *pDst = pNvLogBuffer->data + pNvLogBuffer->pos;


    portMemCopy(pDst, newEntriesSize, pSrc, newEntriesSize);

    // preservedNoWrapPos already accounted for both LIBOS buffer header and put pointer
    pNvLogBuffer->pos += newEntriesSize;
    *(NvU64 *)(pNoWrapBuf->data) += totalNumNewEntries;

    return NV_TRUE;
}
static void libosCopyLogToMergeNvlog_Wrap(LIBOS_LOG_DECODE *logDecode, NVLOG_BUFFER *pNvLogBuffer, NvU64 totalNumNewEntries)
{
    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 numNewEntries = totalNumNewEntries;
    NvU64 newEntriesSize  = totalNumNewEntries * sizeof(NvU64);
    NvU64 mergedBufferTotalEntries = logDecode->mergedBufferSize / sizeof(NvU64);

    if (numNewEntries == 0)
        return;

    while (pNvLogBuffer->pos + newEntriesSize >= pNvLogBuffer->size)
    {
        NvU8 *pSrc = (NvU8 *)&logDecode->mergedBuffer[mergedBufferTotalEntries - numNewEntries];
        NvU8 *pDst = pNvLogBuffer->data + pNvLogBuffer->pos;
        NvU64 len  = pNvLogBuffer->size - pNvLogBuffer->pos;

        portMemCopy(pDst, len, pSrc, len);

        newEntriesSize -= len;
        numNewEntries -= len / sizeof(NvU64);
        pNvLogBuffer->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64);

    }

    if (numNewEntries > 0)
    {
        NvU8 *pSrc = (NvU8 *)&logDecode->mergedBuffer[mergedBufferTotalEntries - numNewEntries];
        NvU8 *pDst = pNvLogBuffer->data + pNvLogBuffer->pos;
        NvU64 len  = newEntriesSize;

        portMemCopy(pDst, len, pSrc, len);
        pNvLogBuffer->pos += newEntriesSize;

    }

    *(NvU64 *)(pWrapBuf->data) += totalNumNewEntries;
}

static void libosExtractLogs_mergeNvlog(LIBOS_LOG_DECODE *logDecode)
{
    NvU64 totalNumNewEntries = 0;
    NvBool bNvLogNoWrap = NV_FALSE;
    NvU64 hNvLogNoWrap = 0;
    NvU64 hNvLogWrap = 0;

    for (NvU64 i = 0; i < logDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];
        if (pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;
        if ((pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER) == 0)
            continue;

        pLog->putIter = pLog->putCopy;

        if (hNvLogNoWrap == 0)
        {
            hNvLogNoWrap = pLog->hNvLogNoWrap;
            hNvLogWrap = pLog->hNvLogWrap;
            bNvLogNoWrap = pLog->bNvLogNoWrap;
        }
    }

    libosMergeNvlog(logDecode, &totalNumNewEntries);

    if (bNvLogNoWrap)
    {
        bNvLogNoWrap = libosCopyLogToMergeNvlog_Nowrap(logDecode, NvLogLogger.pBuffers[hNvLogNoWrap], totalNumNewEntries);
        if (!bNvLogNoWrap)
        {
            for (NvU64 i = 0; i < logDecode->numLogBuffers; i++)
            {
                LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];

                if ((pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED) ||
                    (pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER) == 0)
                    continue;

                pLog->bNvLogNoWrap = NV_FALSE;
            }
        }
    }

    if (!bNvLogNoWrap)
    {
        libosCopyLogToMergeNvlog_Wrap(logDecode, NvLogLogger.pBuffers[hNvLogWrap], totalNumNewEntries);
    }
}

static void libosExtractLogs_nvlog(LIBOS_LOG_DECODE *logDecode, NvBool bSyncNvLog)
{
    NvU64 i;
    NvBool hasMergedNvlogBuffers = NV_FALSE;
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];

        if (pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;

        if (pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
        {
            hasMergedNvlogBuffers = NV_TRUE;
            continue;
        }

        if (pLog->bNvLogNoWrap)
        {
            pLog->bNvLogNoWrap = libosCopyLogToNvlog_nowrap(pLog);
        }

        if (!pLog->bNvLogNoWrap)
        {
            libosCopyLogToNvlog_wrap(pLog);
        }

        if (bSyncNvLog)
        {
            libosSyncNvlog(pLog);
        }
    }

    if (hasMergedNvlogBuffers)
        libosExtractLogs_mergeNvlog(logDecode);
}

void libosPreserveLogs(LIBOS_LOG_DECODE *pLogDecode)
{
    NvU64 i;
    for (i = 0; i < pLogDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &pLogDecode->log[i];

        if (pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;

        if (pLog->bDidPush)
        {
            NvHandle hNvlog = pLog->hNvLogNoWrap;
            NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[hNvlog];

            if (hNvlog == 0 || pNvLogBuffer == NULL)
                continue;

            pNvLogBuffer->flags |= DRF_DEF(LOG, _BUFFER_FLAGS, _PRESERVE, _YES);
        }
    }
}

NvBool isLibosPreserveLogBufferFull(LIBOS_LOG_DECODE *pLogDecode, NvU32 gpuInstance)
{
    NvU64 i = (NvU32)(pLogDecode->numLogBuffers);
    NvU32 tag = LIBOS_LOG_NVLOG_BUFFER_TAG(pLogDecode->sourceName, i * 2);

    //
    // Cannot use nvlogGetBufferHandleFromTag here since in multi GPU case,
    // we can have multiple buffers with exact same tag, only differentiable
    // from gpuInstance
    //
    for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
    {
        if (NvLogLogger.pBuffers[i] != NULL)
        {
            NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[i];
            if ((pNvLogBuffer->tag == tag) &&
                (DRF_VAL(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, pNvLogBuffer->flags) == gpuInstance) &&
                FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _PRESERVE, _YES, pNvLogBuffer->flags) &&
                (pNvLogBuffer->pos >= pNvLogBuffer->size - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) - sizeof(NvU64)))
            {
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

static NvBool findPreservedNvlogBuffer(NvU32 tag, NvU32 gpuInstance, NVLOG_BUFFER_HANDLE *pHandle)
{
    NvU64 i;

    //
    // Cannot use nvlogGetBufferHandleFromTag here since in multi GPU case,
    // we can have multiple buffers with exact same tag, only differentiable
    // from gpuInstance
    //
    for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
    {
        if (NvLogLogger.pBuffers[i] != NULL)
        {
            NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[i];
            if ((pNvLogBuffer->tag == tag) &&
                (DRF_VAL(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, pNvLogBuffer->flags) == gpuInstance) &&
                FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _PRESERVE, _YES, pNvLogBuffer->flags) &&
                (pNvLogBuffer->pos < pNvLogBuffer->size - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) - sizeof(NvU64)))
            {
                *pHandle = i;
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

#endif // LIBOS_LOG_TO_NVLOG

/**
 *
 * @brief Helper functions for creating and destroying log buffers.
 *
 * Call the functions in this order:
 *  libosLogCreate - Just clears the LIBOS_LOG_DECODE structure.
 *  libosLogAddLog - Call once for each log buffer.
 *  libosLogInit   - Sizes/allocates scratch buffer, and inits resolver.
 *  ...
 *  libosLogDestroy - Destroys resolver and frees scratch buffer. *
 */

static void libosLogCreateGeneric(LIBOS_LOG_DECODE *logDecode)
{
#if LIBOS_LOG_DECODE_ENABLE
    states_init();
#endif // LIBOS_LOG_DECODE_ENABLE

    portMemSet(logDecode, 0, sizeof *logDecode);

    // Default name value: GSP
    portStringCopy(logDecode->sourceName, sizeof(logDecode->sourceName), "GSP", sizeof(logDecode->sourceName));
}
static void libosLogCreateExGeneric(LIBOS_LOG_DECODE *logDecode, const char *pSourceName)
{
#if LIBOS_LOG_DECODE_ENABLE
    states_init();
#endif // LIBOS_LOG_DECODE_ENABLE

    portMemSet(logDecode, 0, sizeof *logDecode);

    // Extended args - set name value
    portStringCopy(logDecode->sourceName, sizeof(logDecode->sourceName), pSourceName, sizeof(logDecode->sourceName));
}

#if defined(LIBOS_LOG_OFFLINE_DECODER)
void libosLogCreate(LIBOS_LOG_DECODE *logDecode, LogPrinter *pLogPrinter)
{
    libosLogCreateGeneric(logDecode);
    logDecode->pLogPrinter = pLogPrinter;
}
void libosLogCreateEx(LIBOS_LOG_DECODE *logDecode, const char *pSourceName, LogPrinter *pLogPrinter)
{
    libosLogCreateExGeneric(logDecode, pSourceName);
    logDecode->pLogPrinter = pLogPrinter;
}
#else
void libosLogCreate(LIBOS_LOG_DECODE *logDecode)
{
    libosLogCreateGeneric(logDecode);
}
void libosLogCreateEx(LIBOS_LOG_DECODE *logDecode, const char *pSourceName)
{
    libosLogCreateExGeneric(logDecode, pSourceName);
}
#endif

static void libosLogInitHeader(
    LIBOS_LOG_NVLOG_BUFFER *pBuf,
    const char *taskPrefix,
    NvU32 gpuArch,
    NvU32 gpuImpl,
    NvU32 libosLogFlags,
    LibosElfNoteHeader *buildIdSection
)
{
    if (taskPrefix)
    {
        portStringCopy(pBuf->taskPrefix, sizeof(pBuf->taskPrefix), taskPrefix, sizeof(pBuf->taskPrefix));
    }

    pBuf->gpuArch = gpuArch;
    pBuf->gpuImpl = gpuImpl;
    pBuf->version = LIBOS_LOG_NVLOG_BUFFER_VERSION;
    pBuf->flags   = libosLogFlags;

    if (buildIdSection != NULL)
    {
        pBuf->buildIdLength = buildIdSection->descsz;
        portMemCopy(&pBuf->buildId[0], sizeof(pBuf->buildId), 
                    buildIdSection->data + buildIdSection->namesz, buildIdSection->descsz);
    }
}

/**
 *
 * @brief Functions for allocating merged nvlog buffers
 *
 * When nvlog buffer are not merged for different GSP task, libosLogAddLog would initialize the
 * libos log buffer and allocate 2 nvlog buffers for wrap and no wrap
 *
 * When merge nvlog flag is set, libosLogAddLog will skip nvlog allocation. Instead this function
 * will allocate the merged nvlog buffer after every libos log has been initilized
 */
void libosLogSetupMergedNvlog(
    LIBOS_LOG_DECODE *logDecode,
    NvU32 gpuInstance,
    NvU64 mergedBufferSize,
    const char *name,
    NvU32 gpuArch,
    NvU32 gpuImpl,
    void *buildId)
{
#if LIBOS_LOG_TO_NVLOG
    NV_STATUS status;

    const NvU32 libosNoWrapBufferFlags =
        DRF_DEF(LOG, _BUFFER_FLAGS, _DISABLED, _NO)      | DRF_DEF(LOG, _BUFFER_FLAGS, _TYPE, _NOWRAP)  |
        DRF_DEF(LOG, _BUFFER_FLAGS, _EXPANDABLE, _NO)    | DRF_DEF(LOG, _BUFFER_FLAGS, _NONPAGED, _YES) |
        DRF_DEF(LOG, _BUFFER_FLAGS, _LOCKING, _NONE)     | DRF_DEF(LOG, _BUFFER_FLAGS, _OCA, _YES)      |
        DRF_DEF(LOG, _BUFFER_FLAGS, _FORMAT, _LIBOS_LOG) |
        DRF_NUM(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, gpuInstance);

    const NvU32 libosWrapBufferFlags =
        DRF_DEF(LOG, _BUFFER_FLAGS, _DISABLED, _NO)      | DRF_DEF(LOG, _BUFFER_FLAGS, _TYPE, _RING)    |
        DRF_DEF(LOG, _BUFFER_FLAGS, _EXPANDABLE, _NO)    | DRF_DEF(LOG, _BUFFER_FLAGS, _NONPAGED, _YES) |
        DRF_DEF(LOG, _BUFFER_FLAGS, _LOCKING, _NONE)     | DRF_DEF(LOG, _BUFFER_FLAGS, _OCA, _YES)      |
        DRF_DEF(LOG, _BUFFER_FLAGS, _FORMAT, _LIBOS_LOG) |
        DRF_NUM(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, gpuInstance);

    if (mergedBufferSize == 0)
        return;

    if (logDecode->mergedBuffer == NULL)
    {
        logDecode->mergedBuffer     = portMemAllocNonPaged(mergedBufferSize);
        logDecode->mergedBufferSize = mergedBufferSize;
    }

    NvU32 hNvLogNoWrap = 0;
    NvU32 hNvLogWrap   = 0;
    NvU64 preservedNoWrapPos = 0;

    NvU32 tag = LIBOS_LOG_NVLOG_BUFFER_TAG(logDecode->sourceName, logDecode->numLogBuffers*2);
    NvBool bFoundPreserved = findPreservedNvlogBuffer(tag, gpuInstance, &hNvLogNoWrap);
    NvU32 libosLogFlags = 0;

    libosLogFlags |= LIBOS_LOG_NVLOG_BUFFER_FLAG_PACKED_METADATA;
    libosLogFlags |= LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER;

    if (!bFoundPreserved)
    {
        status = nvlogAllocBuffer(
            LIBOS_LOG_NVLOG_BUFFER_SIZE(mergedBufferSize), libosNoWrapBufferFlags,
            tag,
            &hNvLogNoWrap);

        if (status == NV_OK)
        {
            libosLogInitHeader(
                (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[hNvLogNoWrap]->data,
                name,
                gpuArch,
                gpuImpl,
                libosLogFlags,
                buildId
            );

            NvLogLogger.pBuffers[hNvLogNoWrap]->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer

        }
        else
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "nvlogAllocBuffer nowrap failed\n");
        }
    }
    else
    {
        preservedNoWrapPos = NvLogLogger.pBuffers[hNvLogNoWrap]->pos;
    }

    tag = LIBOS_LOG_NVLOG_BUFFER_TAG(logDecode->sourceName, logDecode->numLogBuffers*2 + 1);

    status = nvlogAllocBuffer(
        LIBOS_LOG_NVLOG_BUFFER_SIZE(mergedBufferSize), libosWrapBufferFlags,
        tag, &hNvLogWrap);

    if (status == NV_OK)
    {
        libosLogInitHeader(
            (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[hNvLogWrap]->data,
            name,
            gpuArch,
            gpuImpl,
            libosLogFlags,
            buildId
        );

        NvLogLogger.pBuffers[hNvLogWrap]->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer

    }
    else
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "nvlogAllocBuffer wrap failed\n");
    }

    for (NvU32 i = 0; i < logDecode->numLogBuffers; i++)
    {
        if (logDecode->log[i].libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
        {
            logDecode->log[i].hNvLogNoWrap = hNvLogNoWrap;
            logDecode->log[i].hNvLogWrap   = hNvLogWrap;
            logDecode->log[i].bNvLogNoWrap = NV_TRUE;
            if (preservedNoWrapPos != 0)
                logDecode->log[i].preservedNoWrapPos = preservedNoWrapPos;
            else
                logDecode->log[i].preservedNoWrapPos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64);;
        }
    }
        
#endif // LIBOS_LOG_TO_NVLOG
}

void libosLogAddLogEx(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, NvU32 gpuArch, NvU32 gpuImpl, const char *name, const char *elfSectionName, NvU32 libosLogDecodeFlags, void *buildId, NvU32 libosLogNvlogBufferVersion, NvU32 libosLogFlags)
{
    NvU32 i;
    LIBOS_LOG_DECODE_LOG *pLog;

    if (logDecode->numLogBuffers >= LIBOS_LOG_MAX_LOGS)
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "LIBOS_LOG_DECODE::log array is too small. Increase LIBOS_LOG_MAX_LOGS.\n");
        return;
    }

    NV_ASSERT(libosLogNvlogBufferVersion >= LIBOS_LOG_NVLOG_BUFFER_VERSION_2 || libosLogFlags == 0);

    i                          = (NvU32)(logDecode->numLogBuffers++);
    pLog                       = &logDecode->log[i];
    pLog->physicLogBuffer      = (volatile NvU64 *)buffer;
    pLog->logBufferSize        = bufferSize;
    pLog->previousPut          = 0;
    pLog->putCopy              = 0;
    pLog->putIter              = 0;
    pLog->libosLogDecodeFlags  = libosLogDecodeFlags;
    pLog->libosLogFlags        = libosLogFlags;
    pLog->libosLogNvlogBufferVersion = libosLogNvlogBufferVersion;
    pLog->gpuInstance  = gpuInstance;

    if (name)
        portStringCopy(pLog->taskPrefix, sizeof(pLog->taskPrefix), name, sizeof(pLog->taskPrefix));

    if (elfSectionName)
        portStringCopy(pLog->elfSectionName, sizeof(pLog->elfSectionName), elfSectionName, sizeof(pLog->elfSectionName));
    else
        portStringCopy(pLog->elfSectionName, sizeof(pLog->elfSectionName), "default", sizeof(pLog->elfSectionName));

#if LIBOS_LOG_TO_NVLOG
    if (pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
        return;

    if (pLog->libosLogFlags & LIBOS_LOG_NVLOG_BUFFER_FLAG_MERGED_NVLOG_BUFFER)
        return;

    NV_STATUS status;

    const NvU32 libosNoWrapBufferFlags =
        DRF_DEF(LOG, _BUFFER_FLAGS, _DISABLED, _NO)      | DRF_DEF(LOG, _BUFFER_FLAGS, _TYPE, _NOWRAP)  |
        DRF_DEF(LOG, _BUFFER_FLAGS, _EXPANDABLE, _NO)    | DRF_DEF(LOG, _BUFFER_FLAGS, _NONPAGED, _YES) |
        DRF_DEF(LOG, _BUFFER_FLAGS, _LOCKING, _NONE)     | DRF_DEF(LOG, _BUFFER_FLAGS, _OCA, _YES)      |
        DRF_DEF(LOG, _BUFFER_FLAGS, _FORMAT, _LIBOS_LOG) |
        DRF_NUM(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, gpuInstance);

    const NvU32 libosWrapBufferFlags =
        DRF_DEF(LOG, _BUFFER_FLAGS, _DISABLED, _NO)      | DRF_DEF(LOG, _BUFFER_FLAGS, _TYPE, _RING)    |
        DRF_DEF(LOG, _BUFFER_FLAGS, _EXPANDABLE, _NO)    | DRF_DEF(LOG, _BUFFER_FLAGS, _NONPAGED, _YES) |
        DRF_DEF(LOG, _BUFFER_FLAGS, _LOCKING, _NONE)     | DRF_DEF(LOG, _BUFFER_FLAGS, _OCA, _YES)      |
        DRF_DEF(LOG, _BUFFER_FLAGS, _FORMAT, _LIBOS_LOG) |
        DRF_NUM(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, gpuInstance);

    pLog->hNvLogNoWrap = 0;
    pLog->hNvLogWrap   = 0;
    pLog->bNvLogNoWrap = NV_FALSE;

    pLog->bDidPush             = NV_FALSE;
    pLog->preservedNoWrapPos   = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer

    NvU32 tag = LIBOS_LOG_NVLOG_BUFFER_TAG(logDecode->sourceName, i * 2);
    NvBool bFoundPreserved = findPreservedNvlogBuffer(tag, gpuInstance, &pLog->hNvLogNoWrap);

    if (!bFoundPreserved)
    {
        status = nvlogAllocBuffer(
            LIBOS_LOG_NVLOG_BUFFER_SIZE(bufferSize), libosNoWrapBufferFlags,
            tag,
            &pLog->hNvLogNoWrap);

        if (status == NV_OK)
        {
            libosLogInitHeader(
                (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[pLog->hNvLogNoWrap]->data,
                name,
                gpuArch,
                gpuImpl,
                libosLogFlags,
                buildId
            );

            NvLogLogger.pBuffers[pLog->hNvLogNoWrap]->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer
            pLog->bNvLogNoWrap                            = NV_TRUE;

        }
        else
        {
            LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "nvlogAllocBuffer nowrap failed\n");
        }
    }
    else
    {
        pLog->bNvLogNoWrap = NV_TRUE;
        pLog->preservedNoWrapPos = NvLogLogger.pBuffers[pLog->hNvLogNoWrap]->pos;
    }

    tag = LIBOS_LOG_NVLOG_BUFFER_TAG(logDecode->sourceName, i * 2 + 1);

    status = nvlogAllocBuffer(
        bufferSize + NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data), libosWrapBufferFlags,
        tag, &pLog->hNvLogWrap);

    if (status == NV_OK)
    {
        libosLogInitHeader(
            (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[pLog->hNvLogWrap]->data,
            name,
            gpuArch,
            gpuImpl,
            libosLogFlags,
            buildId
        );

        NvLogLogger.pBuffers[pLog->hNvLogWrap]->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer

    }
    else
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "nvlogAllocBuffer wrap failed\n");
    }
#endif // LIBOS_LOG_TO_NVLOG
}

void libosLogAddLog(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, const char *name, const char *elfSectionName, NvU32 libosLogDecodeFlags, NvU32 libosLogNvlogBufferVersion, NvU32 libosLogFlags)
{
    // Use defaults for gpuArch and gpuImpl
    libosLogAddLogEx(logDecode, buffer, bufferSize, gpuInstance, 0, 0, name, elfSectionName, libosLogDecodeFlags, NULL, libosLogNvlogBufferVersion, libosLogFlags);
}

#if LIBOS_LOG_DECODE_ENABLE

static NvBool _checkIsElfContainer(LibosElfImage *image)
{
    // Container ELF will contain a section denoting that it is a container!
    LibosElfSectionHeaderPtr shdr = LibosElfFindSectionByName(image, ".fwIsContainerElf");
    return shdr.raw != NULL;
}

static void _libosLogInitLogBuffer(LIBOS_LOG_DECODE_LOG *pLog, void *elf, NvU64 elfSize)
{
    pLog->elf = elf;

    if (elf != NULL)
    {
        LibosElfImageConstruct(&pLog->elfImage, elf, elfSize);
        LibosDebugResolverConstruct(&pLog->resolver, &pLog->elfImage);

        // Prioritize .logging_metadata first for clients that don't merge their sections
        LibosElfSectionHeaderPtr shdr = LibosElfFindSectionByName(&pLog->elfImage, ".logging_metadata");

        if (shdr.raw == NULL)
        {
            shdr = LibosElfFindSectionByName(&pLog->elfImage, ".logging");
        }

        if (shdr.raw != NULL)
        {
            if (LibosElfGetClass(&pLog->elfImage) == ELFCLASS32)
            {
                pLog->loggingBaseAddress = shdr.shdr32->addr;
                pLog->loggingSize = shdr.shdr32->size;
            }
            else
            {
                pLog->loggingBaseAddress = shdr.shdr64->addr;
                pLog->loggingSize = shdr.shdr64->size;
            }
        }
    }
}

static void libosLogInitLogBuffer(LIBOS_LOG_DECODE_LOG *pLog, LibosElfImage *pImage, const char *taskPrefix, const char *pElfSectionName)
{
    if (taskPrefix)
        portStringCopy(pLog->taskPrefix, sizeof(pLog->taskPrefix), taskPrefix, sizeof(pLog->taskPrefix));

    if (pElfSectionName)
        portStringCopy(pLog->elfSectionName, sizeof(pLog->elfSectionName), pElfSectionName, sizeof(pLog->elfSectionName));

    LibosElfSectionHeaderPtr sectionHdr;
    NvU8 *sectionData = NULL, *sectionDataEnd = NULL;
    sectionHdr.raw = NULL;

    // Initialize each log buffer with its own task's logging ELF
    sectionHdr = LibosElfFindSectionByName(pImage, (const char *) pLog->elfSectionName);
    if (sectionHdr.raw != NULL)
    {
        LibosElfMapSection(pImage, sectionHdr, &sectionData, &sectionDataEnd);
        _libosLogInitLogBuffer(pLog, sectionData, sectionDataEnd - sectionData);
    }
}

/**
 *
 * @brief Initialize log resolver to handle merged log decoding
 *
 * During offline decoding, if log buffers are not merged, we use pLog themselves as the resolver since
 * the buffer tag/task prefix tells us the corresponding elf sefction.
 *
 * In merged log case, the libos log header extracted from the nvlog dump file would be from a merged log
 * buffer without proper task prefix. So we need resolvers initialized by this function
 *
 */
void libosLogInitGspMergedLogResolver(LIBOS_LOG_DECODE *logDecode, void *elf, NvU64 elfSize, NvU32 gpuArch, NvU32 gpuImpl)
{
    NvU32 numTaskLogInfos = 0;
    const nv_firmware_task_log_info_t *pTaskLogInfos = nv_firmware_get_task_log_infos(&numTaskLogInfos);
    const nv_firmware_kernel_log_info_t *pkernelLogInfo = nv_firmware_kernel_log_info_for_gpu(gpuArch << NV_FIRMWARE_GPU_ARCH_SHIFT, gpuImpl);
    LibosElfImage image;

    if (!elf)
        return;

    // Set up log decoder
    if (LibosElfImageConstruct(&image, elf, elfSize) != LibosOk)
        return;

    if (pkernelLogInfo)
    {
        libosLogInitLogBuffer(&logDecode->mergedLogResolver[LIBOS_LOG_TASK_GSP_KERNEL],
                                &image, NV_FIRMWARE_KERNEL_LOG_PREFIX,
                                pkernelLogInfo->elf_section_name);
    }

    for (NvU32 i = 0; i < numTaskLogInfos; i++)
    {
        libosLogInitLogBuffer(&logDecode->mergedLogResolver[i+2], &image, pTaskLogInfos[i].prefix, pTaskLogInfos[i].elf_section_name);
    }
}

void libosLogInit(LIBOS_LOG_DECODE *logDecode, void *elf, NvU64 elfSize)
{
    NvU64 scratchBufferSize = 0;
    NvU64 i;
    NvBool bIsContainer;
    LibosElfImage image;

    //
    // The scratch buffer holds the sorted records in flight from all logs.
    // If we overflow it, we lose records.
    //

    //
    // First, calculate the smallest possible length (in 64-bit words)
    // of a log buffer entry for the current log (0 args).
    // This will allow us to calculate for max possible number of log entries,
    // i.e. if none of them have args and are thus the smallest size possible.
    //
    NvU64 minLogBufferEntryLength = 1;// account for metadata pointer

    if (!logDecode->bSynchronousBuffer)
    {
        minLogBufferEntryLength++; // account for timestamp
    }

    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        scratchBufferSize += logDecode->log[i].logBufferSize;
    }

    // The scratch buffer is sized to handle worst-case overhead
    scratchBufferSize = (scratchBufferSize * LIBOS_LOG_DECODE_RECORD_BASE) / minLogBufferEntryLength;

    logDecode->scratchBuffer     = portMemAllocNonPaged(scratchBufferSize);
    logDecode->scratchBufferSize = scratchBufferSize;
    logDecode->curLineBufPtr     = logDecode->lineBuffer;

    if (elf == NULL)
    {
        logDecode->bIsDecodable = NV_FALSE;
        return;
    }

    // Set up log decoder
    if (LibosElfImageConstruct(&image, elf, elfSize) == LibosOk)
    {
        logDecode->bIsDecodable = NV_TRUE;
    }
    else
    {
        logDecode->bIsDecodable = NV_FALSE;
        return;
    }
    bIsContainer = _checkIsElfContainer(&image);

    // Set up individual log buffers
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        if (bIsContainer)
        {
            libosLogInitLogBuffer(&logDecode->log[i], &image, NULL, NULL);
#if LIBOS_LOG_TO_NVLOG
            if (logDecode->log[i].libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
                continue;
#endif
        }
        else
        {
            // If it isn't a container, all buffers are initialized with the common logging elf
            _libosLogInitLogBuffer(&logDecode->log[i], elf, elfSize);
        }
    }
}

void libosLogInitEx(
    LIBOS_LOG_DECODE *logDecode, void *elf, NvBool bSynchronousBuffer,
    NvBool bPtrSymbolResolve, NvBool bDecodeStrShdr, NvU64 elfSize)
{
    // Set extended config
    logDecode->bSynchronousBuffer = bSynchronousBuffer;
    logDecode->bPtrSymbolResolve = bPtrSymbolResolve;
    logDecode->bDecodeStrShdr = bDecodeStrShdr;

    // Complete init
    libosLogInit(logDecode, elf, elfSize);
}

void libosLogUpdateTimerDelta(LIBOS_LOG_DECODE *logDecode, NvU64 localToGlobalTimerDelta)
{

    for (NvU64 i = 0; i < logDecode->numLogBuffers; i++)
    {
        if (logDecode->log[i].libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;

#if LIBOS_LOG_TO_NVLOG
        LIBOS_LOG_NVLOG_BUFFER *pNoWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[logDecode->log[i].hNvLogNoWrap]->data;
        LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *) NvLogLogger.pBuffers[logDecode->log[i].hNvLogWrap]->data;

        pNoWrapBuf->localToGlobalTimerDelta = localToGlobalTimerDelta;
        pWrapBuf->localToGlobalTimerDelta = localToGlobalTimerDelta;

#endif
        logDecode->log[i].localToGlobalTimerDelta = localToGlobalTimerDelta;
    }

}

#else // LIBOS_LOG_DECODE_ENABLE

void libosLogInit(LIBOS_LOG_DECODE *logDecode, void *elf, NvU64 elfSize) {}

void libosLogInitEx(
    LIBOS_LOG_DECODE *logDecode, void *elf,
    NvBool bSynchronousBuffer, NvBool bPtrSymbolResolve, NvBool bDecodeStrShdr, NvU64 elfSize)
{
    // No extended config to set when decode is disabled
}

void libosLogInitGspMergedLogResolver(LIBOS_LOG_DECODE *logDecode, void *elf, NvU64 elfSize, NvU32 gpuArch, NvU32 gpuImpl) {}

#endif // LIBOS_LOG_DECODE_ENABLE

void libosLogDestroy(LIBOS_LOG_DECODE *logDecode)
{
#if LIBOS_LOG_TO_NVLOG
    NvU64 i;
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];

        if (pLog->libosLogDecodeFlags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;

        if (pLog->hNvLogNoWrap != 0)
        {
            nvlogDeallocBuffer(pLog->hNvLogNoWrap, NV_FALSE);
            pLog->hNvLogNoWrap = 0;
        }

        if (pLog->hNvLogWrap != 0)
        {
            nvlogDeallocBuffer(pLog->hNvLogWrap, NV_FALSE);
            pLog->hNvLogWrap = 0;
        }
    }

    if (logDecode->mergedBuffer)
    {
        portMemFree(logDecode->mergedBuffer);
        logDecode->mergedBuffer = NULL;
        logDecode->mergedBufferSize = 0;
    }
#endif // LIBOS_LOG_TO_NVLOG

#if LIBOS_LOG_DECODE_ENABLE
    NvU64 j;
    for (j = 0; j < logDecode->numLogBuffers; j++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[j];
        LibosDebugResolverDestroy(&pLog->resolver);
    }

    if (logDecode->scratchBuffer)
    {
        portMemFree(logDecode->scratchBuffer);
        logDecode->scratchBuffer = NULL;
    }
#endif // LIBOS_LOG_DECODE_ENABLE

    logDecode->numLogBuffers = 0;
    logDecode->bIsDecodable = NV_FALSE;
}

void libosExtractLogs(LIBOS_LOG_DECODE *logDecode, NvBool bSyncNvLog)
{
    NvU64 i = 0;
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        if (logDecode->log[i].physicLogBuffer)
        {
            logDecode->log[i].putCopy = logDecode->log[i].physicLogBuffer[0];
        }
    }
#if LIBOS_LOG_DECODE_ENABLE
    if (logDecode->bIsDecodable)
        libosExtractLogs_decode(logDecode);
#endif
#if LIBOS_LOG_TO_NVLOG
    libosExtractLogs_nvlog(logDecode, bSyncNvLog);
#endif
    // Update the previous put pointers.
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        logDecode->log[i].previousPut = logDecode->log[i].putCopy;
    }
}

/**
 *
 * @brief Return pointer to data buffer in LibosBuf
 *
 * Tools like protodmp/nvwatch/nvlog_decoder needs to handle older drivers 
 * that had older libos log format, which has different offset to data buffer.
 *
 * These function abstract out libos log version handling for tools.
 *
 * LIBOS_LOG_NVLOG_BUFFER memebr access like pLibosBuf->data 
 * shouldn't be used outside of liblogdecode.c
 *
 */
void* libosLogGetDataPointer(LIBOS_LOG_NVLOG_BUFFER *pLibosBuf)
{
    void *dataPointer = NULL;
    switch (pLibosBuf->version)
    {
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_0:
        {
            LIBOS_LOG_NVLOG_BUFFER_V0 *pLibosBufV0 = (LIBOS_LOG_NVLOG_BUFFER_V0 *) pLibosBuf;
            dataPointer = pLibosBufV0->data;
            break;
        }

        case LIBOS_LOG_NVLOG_BUFFER_VERSION_1:
        {
            LIBOS_LOG_NVLOG_BUFFER_V1 *pLibosBufV1 = (LIBOS_LOG_NVLOG_BUFFER_V1 *) pLibosBuf;
            dataPointer = pLibosBufV1->data;
            break;
        }
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_2:
        {
            LIBOS_LOG_NVLOG_BUFFER_V2 *pLibosBufV2 = (LIBOS_LOG_NVLOG_BUFFER_V2 *) pLibosBuf;
            dataPointer = pLibosBufV2->data;
            break;
        }
        default:
            dataPointer = pLibosBuf->data;
            break;
    }
    return dataPointer;
}

/**
 * @brief Return size of data buffer in LibosBuf
 */
NvU32 libosLogGetDataSize(LIBOS_LOG_NVLOG_BUFFER *pLibosBuf, NVLOG_BUFFER *pBuffer)
{
    NvU32 libosLogDataOffset = 0;
    switch (pLibosBuf->version)
    {
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_0:
            libosLogDataOffset = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V0, data);
            break;
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_1:
            libosLogDataOffset = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V1, data);
            break;
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_2:
            libosLogDataOffset = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V2, data);
            break;
        default:
            libosLogDataOffset = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);
            break;
    }

    return pBuffer->size - libosLogDataOffset;
}

/**
 * @brief Return offset to data buffer in LibosBuf
 */
NvU32 libosLogGetDataOffset(LIBOS_LOG_NVLOG_BUFFER *pLibosBuf)
{
    switch (pLibosBuf->version)
    {
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_0:
            return NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V0, data);
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_1:
            return NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V1, data);
        case LIBOS_LOG_NVLOG_BUFFER_VERSION_2:
            return NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER_V2, data);
        default:
            return NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);
    }
}


#if defined(NVWATCH)
# pragma warning(pop)
#endif // defined(NVWATCH)
