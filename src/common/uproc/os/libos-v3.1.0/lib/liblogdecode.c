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

#    define portStringCopy(d, ld, s, ls) strncpy(d, s, ld)
#    define portStringLength(s)          strlen(s)
#    define portStringLengthSafe(s, l)   strlen(s)
#    define portStringCompare(s1, s2, l) strncmp(s1, s2, l)

#    define portMemCopy(d, ld, s, ls) memcpy(d, s, ld)
#    define portMemSet(d, v, l)       memset(d, v, l)
#    define portMemCmp(d, s, l)       memcmp(d, s, l)
#    define portMemAllocNonPaged(l)   malloc(l)
#    define portMemFree(p)            free(p)

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

#if LIBOS_LOG_DECODE_ENABLE

#    define SYM_DECODED_LINE_MAX_SIZE 1024

// These defines assume RISCV with -mabi=lp64/lp64f/lp64d
#    define LOG_INT_MAX     NV_S32_MAX
#    define LOG_UINT_MAX    NV_U32_MAX
#    define LOG_LONG_MAX    NV_S64_MAX
#    define LOG_ULONG_MAX   NV_U64_MAX
#    define LOG_INTMAX_MAX  NV_S64_MAX
#    define LOG_UINTMAX_MAX NV_U64_MAX
#    define LOG_LLONG_MAX   NV_S64_MAX
#    define LOG_ULLONG_MAX  NV_U64_MAX
#    define LOG_SIZE_MAX    NV_U64_MAX
#    define NL_ARGMAX  32

/* Some useful macros */
#    define MAX(a, b) ((int)(a) > (int)(b) ? (int)(a) : (int)(b))
#    define MIN(a, b) ((int)(a) < (int)(b) ? (int)(a) : (int)(b))

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
#    if LOG_UINT_MAX == LOG_ULONG_MAX
#        define LOG_LONG_IS_INT
#    endif
#    if LOG_SIZE_MAX != LOG_ULONG_MAX || LOG_UINTMAX_MAX != LOG_ULLONG_MAX
#        define LOG_ODD_TYPES
#    endif

#if defined(LOG_LONG_IS_INT) || defined(LOG_ODD_TYPES)
#error "Type sizes don't match RISCV lp64 ABI!"
#endif // defined(LOG_LONG_IS_INT) || defined(LOG_ODD_TYPES)

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
    LOG_PTR,
    LOG_INT,
    LOG_UINT,
    LOG_ULLONG,
#    ifndef LOG_LONG_IS_INT
    LOG_LONG,
    LOG_ULONG,
#    else
#        define LOG_LONG  LOG_INT
#        define LOG_ULONG LOG_UINT
#    endif
    LOG_SHORT,
    LOG_USHORT,
    LOG_CHAR,
    LOG_UCHAR,
#    ifdef LOG_ODD_TYPES
    LOG_LLONG,
    LOG_SIZET,
    LOG_IMAX,
    LOG_UMAX,
    LOG_PDIFF,
    LOG_UIPTR,
#    else
#        define LOG_LLONG LOG_ULLONG
#        define LOG_SIZET LOG_ULONG
#        define LOG_IMAX  LOG_LLONG
#        define LOG_UMAX  LOG_ULLONG
#        define LOG_PDIFF LOG_LONG
#        define LOG_UIPTR LOG_ULONG
#    endif
    LOG_NOARG,
    LOG_MAXSTATE
};
#    define S(i,x) states[i][(x) - 'A']
static unsigned char states[]['z' - 'A' + 1] = {
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
#    define OOB(x) ((unsigned)(x) - 'A' > 'z' - 'A')
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
        S(0,'d') = LOG_INT,   S(0,'i') = LOG_INT,   S(0,'o') = LOG_UINT,  S(0,'u') = LOG_UINT,    S(0,'x') = LOG_UINT,
        S(0,'X') = LOG_UINT,  S(0,'c') = LOG_CHAR,  S(0,'C') = LOG_INT,   S(0,'s') = LOG_PTR,     S(0,'S') = LOG_PTR,
        S(0,'p') = LOG_UIPTR, S(0,'n') = LOG_PTR,   S(0,'a') = LOG_UIPTR, /* NVIDIA decoded address extension */
        S(0,'m') = LOG_NOARG, S(0,'l') = LOG_LPRE,  S(0,'h') = LOG_HPRE,  S(0,'L') = LOG_BIGLPRE, S(0,'z') = LOG_ZTPRE,
        S(0,'j') = LOG_JPRE,  S(0,'t') = LOG_ZTPRE;
    }
    {
        /* 1: l-prefixed */
        S(1,'d') = LOG_LONG,
        S(1,'i') = LOG_LONG,
        S(1,'o') = LOG_ULONG,
        S(1,'u') = LOG_ULONG,
        S(1,'x') = LOG_ULONG,
        S(1,'X') = LOG_ULONG,
        S(1,'c') = LOG_INT,
        S(1,'s') = LOG_PTR,
        S(1,'n') = LOG_PTR,
        S(1,'l') = LOG_LLPRE;
    }
    {
        /* 2: ll-prefixed */
        S(2,'d') = LOG_LLONG,
        S(2,'i') = LOG_LLONG,
        S(2,'o') = LOG_ULLONG,
        S(2,'u') = LOG_ULLONG,
        S(2,'x') = LOG_ULLONG,
        S(2,'X') = LOG_ULLONG,
        S(2,'n') = LOG_PTR;
    }
    {
        /* 3: h-prefixed */
        S(3,'d') = LOG_SHORT,
        S(3,'i') = LOG_SHORT,
        S(3,'o') = LOG_USHORT,
        S(3,'u') = LOG_USHORT,
        S(3,'x') = LOG_USHORT,
        S(3,'X') = LOG_USHORT,
        S(3,'n') = LOG_PTR,
        S(3,'h') = LOG_HHPRE;
    }
    {
        /* 4: hh-prefixed */
        S(4,'d') = LOG_CHAR,
        S(4,'i') = LOG_CHAR,
        S(4,'o') = LOG_UCHAR,
        S(4,'u') = LOG_UCHAR,
        S(4,'x') = LOG_UCHAR,
        S(4,'X') = LOG_UCHAR,
        S(4,'n') = LOG_PTR;
    }
    {
        /* 5: L-prefixed */
        S(5,'n') = LOG_PTR;
    }
    {
        /* 6: z- or t-prefixed (assumed to be same size) */
        S(6,'d') = LOG_PDIFF,
        S(6,'i') = LOG_PDIFF,
        S(6,'o') = LOG_SIZET,
        S(6,'u') = LOG_SIZET,
        S(6,'x') = LOG_SIZET,
        S(6,'X') = LOG_SIZET,
        S(6,'n') = LOG_PTR;
    }
    {
        /* 7: j-prefixed */
        S(7,'d') = LOG_IMAX,
        S(7,'i') = LOG_IMAX,
        S(7,'o') = LOG_UMAX,
        S(7,'u') = LOG_UMAX,
        S(7,'x') = LOG_UMAX,
        S(7,'X') = LOG_UMAX,
        S(7,'n') = LOG_PTR;
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
    unsigned long y;
    for (; x > LOG_ULONG_MAX; x /= 10)
        *--s = '0' + x % 10;
    for (y = x; y; y /= 10)
        *--s = '0' + y % 10;
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
static const char* printfLevelToString(NvU32 level)
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
            len = snprintf(
                logDecode->curLineBufPtr, remain,
                NV_PRINTF_ADD_PREFIX("GPU%u %s-%s: %s(%u): "), pRec->log->gpuInstance,
                logDecode->sourceName, pRec->log->taskPrefix, filename, pRec->meta->lineNumber);
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
                                   "%04d-%02d-%02d %d:%02d:%02d ",
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

            len = snprintf(
                logDecode->curLineBufPtr, remain,
                "GPU%u%s%s-%s: %s(%u): ",
                pRec->log->gpuInstance, printfLevelToString(pRec->meta->printLevel),
                 logDecode->sourceName, pRec->log->taskPrefix,
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
#if !LIBOS_LOG_DECODE_ENABLE
            // We can't support %n when decoding, these pointers do not exist here!
            switch (ps)
            {
            case LOG_BARE:
                *(int *)arg.p = cnt;
                break;
            case LOG_LPRE:
                *(long *)arg.p = cnt;
                break;
            case LOG_LLPRE:
                *(long long *)arg.p = cnt;
                break;
            case LOG_HPRE:
                *(unsigned short *)arg.p = cnt;
                break;
            case LOG_HHPRE:
                *(unsigned char *)arg.p = cnt;
                break;
            case LOG_ZTPRE:
                *(size_t *)arg.p = cnt;
                break;
            case LOG_JPRE:
                *(NvU64 *)arg.p = cnt;
                break;
            }
#endif // !LIBOS_LOG_DECODE_ENABLE
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
            p = MAX(p, z - a + !arg.i);
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
            a = (char *)LibosElfMapVirtualString(&pRec->log->elfImage, (NvUPtr)arg.p, logDecode->bDecodeStrShdr);
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

            s_getSymbolDataStr(&pRec->log->resolver, symDecodedLine + prefixLen, sizeof(symDecodedLine) - prefixLen, (NvUPtr)arg.i, NV_FALSE);

            symDecodedLineLen = portStringLength(symDecodedLine);
            symDecodedLineLen = MIN(symDecodedLineLen, sizeof(symDecodedLine) - 1); // just in case

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

        // Locate format string
        format = LibosElfMapVirtualString(&pRec->log->elfImage, (NvU64)(NvUPtr)pRec->meta->format, NV_TRUE);
        if (!format)
            break;

        // Locate filename
        filename = LibosElfMapVirtualString(&pRec->log->elfImage, (NvU64)(NvUPtr)pRec->meta->filename, NV_TRUE);
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
static libosLogMetadata *_getLoggingMetadata(LIBOS_LOG_DECODE_LOG *pLog, NvU64 idx, const char **elfSectionName)
{
    libosLogMetadata *meta = NULL;
    libosLogMetadata_extended *metaEx = NULL;
    const char *filename = NULL;
    *elfSectionName = NULL;

    meta = (libosLogMetadata *) LibosElfMapVirtual(&pLog->elfImage, pLog->physicLogBuffer[idx], sizeof(libosLogMetadata));
    if (meta == NULL)
        return NULL;

    // Try to fetch filename to check if we received an extended metadata
    filename = LibosElfMapVirtualString(&pLog->elfImage, (NvU64)(NvUPtr)meta->filename, NV_TRUE);
    if ((filename == NULL) && ((NvU64) meta->filename == 0x8000000000000000ULL))
    {
        // Filename wasn't mapped correctly. That means this is an extended metadata.
        metaEx = (libosLogMetadata_extended *) LibosElfMapVirtual(&pLog->elfImage, pLog->physicLogBuffer[idx], sizeof(libosLogMetadata_extended));
        if (metaEx == NULL)
            return NULL;
        meta = &metaEx->meta;

        // Fetch the remote task's elf section name string.
        if (metaEx->elfSectionName != NULL)
        {
            *elfSectionName = LibosElfMapVirtualString(&pLog->elfImage, (NvU64)(NvUPtr)metaEx->elfSectionName, NV_TRUE);
        }
    }

    return meta;
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
    NvU64 i           = pLog->putIter;
    NvU64 argCount;
    NvU64 j;
    const char *elfSectionName;

    if (pLog->putIter == pLog->previousPut)
    {
        pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_END;
        return;
    }
    if (pLog->elf == NULL) // Can't decode if task ELF isn't initialized
    {
        pLog->record.timeStamp = LIBOS_LOG_TIMESTAMP_END;
        return;
    }

    // If we wrapped, adjust local copy of previousPut.
    if (previousPut + log_entries < pLog->putCopy)
        previousPut = pLog->putCopy - log_entries;

    pLog->record.log = pLog;

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

    pLog->record.meta = _getLoggingMetadata(pLog, 1 + (--i % log_entries), &elfSectionName);

    // Sanity check meta data.
    if (pLog->record.meta == NULL || pLog->record.meta->argumentCount > LIBOS_LOG_MAX_ARGS)
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_WARNING,
            "**** Bad metadata.  Lost %lld entries from %s-%s ****\n", pLog->putIter - previousPut,
            logDecode->sourceName, pLog->taskPrefix);
        goto error_ret;
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

        pLog->putCopy = pLog->physicLogBuffer[0];
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

    // Update the previous put pointers.
    for (i = 0; i < logDecode->numLogBuffers; i++)
        logDecode->log[i].previousPut = logDecode->log[i].putCopy;

    // Print out the copied records.
    if (dst != scratchSize)
        libosPrintLogRecords(logDecode, &logDecode->scratchBuffer[dst], scratchSize - dst);
}

#endif // LIBOS_LOG_DECODE_ENABLE

#if LIBOS_LOG_TO_NVLOG

#    define LIBOS_LOG_NVLOG_BUFFER_TAG(_name, _i) NvU32_BUILD((_name)[2], (_name)[1], (_name)[0], (NvU8)('1' + _i))

static NvBool libosCopyLogToNvlog_nowrap(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogNoWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogNoWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pNoWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 putCopy                      = pLog->physicLogBuffer[0];
    NvU64 putOffset                    = putCopy * sizeof(NvU64) + sizeof(NvU64);

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
    *(NvU64 *)(pNoWrapBuf->data) = putCopy + (pLog->preservedNoWrapPos-NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data)-sizeof(NvU64)) / sizeof(NvU64);

    return NV_TRUE;
}

static NvBool libosCopyLogToNvlog_wrap(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 logEntries                 = pLog->logBufferSize / sizeof(NvU64) - 1 /* -1 for PUT pointer */;
    NvU64 putCopy                    = pLog->physicLogBuffer[0];
    NvU64 putOffset                  = putCopy * sizeof(NvU64) + sizeof(NvU64);

    NvU64 prevPutOffset              = pNvLogBuffer->pos - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);
    NvU64 nvlogPos                   = (prevPutOffset - sizeof(NvU64)) % (logEntries * sizeof(NvU64)) + sizeof(NvU64);
    NvU64 putOffsetWrapped           = (putCopy % logEntries) * sizeof(NvU64) + sizeof(NvU64);

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
    *(NvU64 *)(pWrapBuf->data) = putCopy;

    return NV_TRUE;
}

static NvBool libosSyncNvlog(LIBOS_LOG_DECODE_LOG *pLog)
{
    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[pLog->hNvLogWrap];
    NV_ASSERT_OR_RETURN((pLog->hNvLogWrap != 0) && (pNvLogBuffer != NULL), NV_FALSE);

    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)pNvLogBuffer->data;
    NvU64 putCopy                    = pLog->physicLogBuffer[0];
    NvU64 putOffset                  = putCopy * sizeof(NvU64) + sizeof(NvU64);

    portMemCopy(pWrapBuf->data, pLog->logBufferSize, (void *)pLog->physicLogBuffer, pLog->logBufferSize);
    pNvLogBuffer->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + putOffset;
    return NV_TRUE;
}

static void libosExtractLogs_nvlog(LIBOS_LOG_DECODE *logDecode, NvBool bSyncNvLog)
{
    NvU64 i;
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];

        if (pLog->flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
            continue;

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
}

void libosPreserveLogs(LIBOS_LOG_DECODE *pLogDecode)
{
    NvU64 i;
    for (i = 0; i < pLogDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &pLogDecode->log[i];

        if (pLog->flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
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
    NVLOG_BUFFER_HANDLE handle = 0;
    NV_STATUS status = nvlogGetBufferHandleFromTag(tag, &handle);

    if (status != NV_OK)
    {
        return NV_FALSE;
    }

    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[handle];
    if (pNvLogBuffer == NULL)
    {
        return NV_FALSE;
    }

    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _PRESERVE, _YES, pNvLogBuffer->flags) &&
        DRF_VAL(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, pNvLogBuffer->flags) == gpuInstance &&
        (pNvLogBuffer->pos >= pNvLogBuffer->size - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) - sizeof(NvU64)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NvBool findPreservedNvlogBuffer(NvU32 tag, NvU32 gpuInstance, NVLOG_BUFFER_HANDLE *pHandle)
{
    NVLOG_BUFFER_HANDLE handle = 0;
    NV_STATUS status = nvlogGetBufferHandleFromTag(tag, &handle);

    if (status != NV_OK)
        return NV_FALSE;

    NVLOG_BUFFER *pNvLogBuffer = NvLogLogger.pBuffers[handle];
    if (FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _PRESERVE, _YES, pNvLogBuffer->flags) &&
        DRF_VAL(LOG, _BUFFER_FLAGS, _GPU_INSTANCE, pNvLogBuffer->flags) == gpuInstance &&
        (pNvLogBuffer->pos < pNvLogBuffer->size - NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) - sizeof(NvU64)))
    {
        *pHandle = handle;
        return NV_TRUE;
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

void libosLogAddLogEx(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, NvU32 gpuArch, NvU32 gpuImpl, const char *name, const char *elfSectionName, NvU32 libosLogFlags, void *buildId)
{
    NvU32 i;
    LIBOS_LOG_DECODE_LOG *pLog;

    if (logDecode->numLogBuffers >= LIBOS_LOG_MAX_LOGS)
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "LIBOS_LOG_DECODE::log array is too small. Increase LIBOS_LOG_MAX_LOGS.\n");
        return;
    }

    i                          = (NvU32)(logDecode->numLogBuffers++);
    pLog                       = &logDecode->log[i];
    pLog->physicLogBuffer      = (volatile NvU64 *)buffer;
    pLog->logBufferSize        = bufferSize;
    pLog->previousPut          = 0;
    pLog->putCopy              = 0;
    pLog->putIter              = 0;
    pLog->flags                = libosLogFlags;

    pLog->gpuInstance  = gpuInstance;

    if (name)
        portStringCopy(pLog->taskPrefix, sizeof(pLog->taskPrefix), name, sizeof(pLog->taskPrefix));

    if (elfSectionName)
        portStringCopy(pLog->elfSectionName, sizeof(pLog->elfSectionName), elfSectionName, sizeof(pLog->elfSectionName));
    else
        portStringCopy(pLog->elfSectionName, sizeof(pLog->elfSectionName), "default", sizeof(pLog->elfSectionName));

#if LIBOS_LOG_TO_NVLOG
    if (pLog->flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
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

    LIBOS_LOG_NVLOG_BUFFER *pNoWrapBuf;
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
            pNoWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)NvLogLogger.pBuffers[pLog->hNvLogNoWrap]->data;
            if (name)
            {
                portStringCopy(
                    pNoWrapBuf->taskPrefix, sizeof pNoWrapBuf->taskPrefix, name, sizeof pNoWrapBuf->taskPrefix);
            }

            pNoWrapBuf->gpuArch = gpuArch;
            pNoWrapBuf->gpuImpl = gpuImpl;
            pNoWrapBuf->version = LIBOS_LOG_NVLOG_BUFFER_VERSION;

            LibosElfNoteHeader *buildIdSection = buildId;

            if (buildIdSection != NULL)
            {
                pNoWrapBuf->buildIdLength = buildIdSection->descsz;
                portMemCopy(&pNoWrapBuf->buildId[0], sizeof(pNoWrapBuf->buildId), 
                            buildIdSection->data + buildIdSection->namesz, buildIdSection->descsz);
            }

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

    LIBOS_LOG_NVLOG_BUFFER *pWrapBuf;
    tag = LIBOS_LOG_NVLOG_BUFFER_TAG(logDecode->sourceName, i * 2 + 1);

    status = nvlogAllocBuffer(
        bufferSize + NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data), libosWrapBufferFlags,
        tag, &pLog->hNvLogWrap);

    if (status == NV_OK)
    {
        pWrapBuf = (LIBOS_LOG_NVLOG_BUFFER *)NvLogLogger.pBuffers[pLog->hNvLogWrap]->data;
        if (name)
        {
            portStringCopy(
                pWrapBuf->taskPrefix, sizeof pWrapBuf->taskPrefix, name, sizeof pWrapBuf->taskPrefix);
        }

        pWrapBuf->gpuArch = gpuArch;
        pWrapBuf->gpuImpl = gpuImpl;
        pWrapBuf->version = LIBOS_LOG_NVLOG_BUFFER_VERSION;

        LibosElfNoteHeader *buildIdSection = buildId;

        if (buildIdSection != NULL)
        {
            pWrapBuf->buildIdLength = buildIdSection->descsz;
            portMemCopy(&pWrapBuf->buildId[0], sizeof(pWrapBuf->buildId), 
                        buildIdSection->data + buildIdSection->namesz, buildIdSection->descsz);
        }

        NvLogLogger.pBuffers[pLog->hNvLogWrap]->pos = NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data) + sizeof(NvU64); // offset to account for LIBOS buffer header and put pointer
    }
    else
    {
        LIBOS_LOG_DECODE_PRINTF(LEVEL_ERROR, "nvlogAllocBuffer wrap failed\n");
    }
#endif // LIBOS_LOG_TO_NVLOG
}

void libosLogAddLog(LIBOS_LOG_DECODE *logDecode, void *buffer, NvU64 bufferSize, NvU32 gpuInstance, const char *name, const char *elfSectionName, NvU32 libosLogFlags)
{
    // Use defaults for gpuArch and gpuImpl
    libosLogAddLogEx(logDecode, buffer, bufferSize, gpuInstance, 0, 0, name, elfSectionName, libosLogFlags, NULL);
}

#if LIBOS_LOG_DECODE_ENABLE

static NvBool _checkIsElfContainer(LibosElfImage *image)
{
    // Container ELF will contain a section denoting that it is a container!
    if (LibosElfFindSectionByName(image, ".fwIsContainerElf") == NULL)
        return NV_FALSE;

    return NV_TRUE;
}

void _libosLogInitLogBuffer(LIBOS_LOG_DECODE_LOG *pLog, LibosElf64Header *elf, NvU64 elfSize)
{
    pLog->elf = elf;

    if (elf != NULL)
    {
        LibosElfImageConstruct(&pLog->elfImage, elf, elfSize);
        LibosDebugResolverConstruct(&pLog->resolver, &pLog->elfImage);
    }
}

void libosLogInit(LIBOS_LOG_DECODE *logDecode, LibosElf64Header *elf, NvU64 elfSize)
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
            LibosElf64SectionHeader *sectionHdr;
            NvU8 *sectionData = NULL, *sectionDataEnd;
            NvU64 sectionSize = 0;

            // Initialize each log buffer with its own task's logging ELF
            sectionHdr = LibosElfFindSectionByName(&image, (const char *) logDecode->log[i].elfSectionName);
            if (sectionHdr != NULL)
            {
                LibosElfMapSection(&image, sectionHdr, &sectionData, &sectionDataEnd);
                sectionSize = sectionHdr->size;
            }
            _libosLogInitLogBuffer(&logDecode->log[i], (LibosElf64Header *) sectionData, sectionSize);
#if LIBOS_LOG_TO_NVLOG
            if (logDecode->log[i].flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
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
    LIBOS_LOG_DECODE *logDecode, LibosElf64Header *elf, NvBool bSynchronousBuffer,
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
        if (logDecode->log[i].flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
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

#endif // LIBOS_LOG_DECODE_ENABLE

void libosLogDestroy(LIBOS_LOG_DECODE *logDecode)
{
#if LIBOS_LOG_TO_NVLOG
    NvU64 i;
    for (i = 0; i < logDecode->numLogBuffers; i++)
    {
        LIBOS_LOG_DECODE_LOG *pLog = &logDecode->log[i];

        if (pLog->flags & LIBOS_LOG_DECODE_LOG_FLAG_NVLOG_DISABLED)
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
#if LIBOS_LOG_DECODE_ENABLE
    if (logDecode->bIsDecodable)
        libosExtractLogs_decode(logDecode);
#endif

#if LIBOS_LOG_TO_NVLOG
    libosExtractLogs_nvlog(logDecode, bSyncNvLog);
#endif
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
        default:
            return NV_OFFSETOF(LIBOS_LOG_NVLOG_BUFFER, data);
    }
}


#if defined(NVWATCH)
# pragma warning(pop)
#endif // defined(NVWATCH)
