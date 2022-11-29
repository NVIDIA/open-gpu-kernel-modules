/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef LIBOS_LOGGER_H_
#define LIBOS_LOGGER_H_

#include "nvtypes.h"
#include <stdarg.h>
#include "libos_printf_arg.h"

/**
 *  @brief The log metadata structures and format strings are stripped
 *         These structures are emitted into the .logging section
 *         which is stripped from the image as the final build step.
 *
 */
typedef struct
{
    NV_DECLARE_ALIGNED(const char *filename, 8);
    NV_DECLARE_ALIGNED(const char *format, 8);
    NV_DECLARE_ALIGNED(NvU32 lineNumber, 8);
    NvU8 argumentCount; //! Count of arguments not including format string.
    NvU8 printLevel;
} libosLogMetadata;

/**
 *  @brief The log metadata structure extended for accomodating remote
 *         task's symbol resolution.
 *         It contains a flag 'version_extended', containing its high bit
 *         set so that this extended struct can be differentiated from
 *         the original struct.
 *
 */
typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 versionExtended, 8);
    NV_DECLARE_ALIGNED(const char *elfSectionName, 8);
    libosLogMetadata meta;
} libosLogMetadata_extended;

/*!
 *  Count arguments
 *
 */
#define LIBOS_MACRO_GET_COUNT(...)                                                                           \
    LIBOS_MACRO_GET_19TH(__VA_ARGS__, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define LIBOS_MACRO_GET_19TH(                                                                                \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, N, ...)                 \
    N

/*!
 *  Utility
 *
 */
#define LIBOS_MACRO_PASTE(fmt, b)             fmt##b
#define LIBOS_MACRO_PASTE_EVAL(fmt, b)        LIBOS_MACRO_PASTE(fmt, b)
#define LIBOS_MACRO_FIRST(format_string, ...) format_string

/*!
 *  Cast remaining log arguments to integers for storage
 *
 */
static inline LibosPrintfArgument LibosPrintfArgumentU64(NvU64 i) {
    LibosPrintfArgument r;
    r.i = i;
    return r;
}

static inline LibosPrintfArgument LibosPrintfArgumentS64(NvS64 i) {
    LibosPrintfArgument r;
    r.i = (NvU64)i;
    return r;
}

#if !defined(NVRM) && !defined(PMU_RTOS) && LIBOS_CONFIG_FLOAT
    static inline LibosPrintfArgument LibosPrintfArgumentFloat(float f) {
        LibosPrintfArgument r;
        r.f = f;
        return r;
    }
    static inline LibosPrintfArgument LibosPrintfArgumentDouble(double f) {
        LibosPrintfArgument r;
        r.f = f;
        return r;
    }

#   define LIBOS_LOG_BUILD_ARG(a)  _Generic((a),                                 \
                                    float: LibosPrintfArgumentFloat(a),          \
                                    double: LibosPrintfArgumentDouble(a),        \
                                    default : LibosPrintfArgumentU64((NvU64)a)),

#else
#   define LIBOS_LOG_BUILD_ARG(a)  (NvU64)(a),
#endif

#define LIBOS_LOG_BUILD_1(fmt)
#define LIBOS_LOG_BUILD_2(fmt, b)       LIBOS_LOG_BUILD_ARG(b)
#define LIBOS_LOG_BUILD_3(fmt, b, c)    LIBOS_LOG_BUILD_ARG(b) LIBOS_LOG_BUILD_ARG(c)
#define LIBOS_LOG_BUILD_4(fmt, b, c, d) LIBOS_LOG_BUILD_ARG(b) LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d)
#define LIBOS_LOG_BUILD_5(fmt, b, c, d, e)                                                                   \
    LIBOS_LOG_BUILD_ARG(b) LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e)
#define LIBOS_LOG_BUILD_6(fmt, b, c, d, e, f)                                                                \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)
#define LIBOS_LOG_BUILD_7(fmt, b, c, d, e, f, g)                                                             \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g)
#define LIBOS_LOG_BUILD_8(fmt, b, c, d, e, f, g, h)                                                          \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h)
#define LIBOS_LOG_BUILD_9(fmt, b, c, d, e, f, g, h, i)                                                       \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i)
#define LIBOS_LOG_BUILD_10(fmt, b, c, d, e, f, g, h, i, j)                                                   \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)
#define LIBOS_LOG_BUILD_11(fmt, b, c, d, e, f, g, h, i, j, k)                                                \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k)
#define LIBOS_LOG_BUILD_12(fmt, b, c, d, e, f, g, h, i, j, k, l)                                             \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l)
#define LIBOS_LOG_BUILD_13(fmt, b, c, d, e, f, g, h, i, j, k, l, m)                                          \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m)
#define LIBOS_LOG_BUILD_14(fmt, b, c, d, e, f, g, h, i, j, k, l, m, n)                                       \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m) LIBOS_LOG_BUILD_ARG(n)
#define LIBOS_LOG_BUILD_15(fmt, b, c, d, e, f, g, h, i, j, k, l, m, n, o)                                    \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m) LIBOS_LOG_BUILD_ARG(n)      \
                LIBOS_LOG_BUILD_ARG(o)
#define LIBOS_LOG_BUILD_16(fmt, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)                                 \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m) LIBOS_LOG_BUILD_ARG(n)      \
                LIBOS_LOG_BUILD_ARG(o) LIBOS_LOG_BUILD_ARG(p)
#define LIBOS_LOG_BUILD_17(fmt, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)                              \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m) LIBOS_LOG_BUILD_ARG(n)      \
                LIBOS_LOG_BUILD_ARG(o) LIBOS_LOG_BUILD_ARG(p) LIBOS_LOG_BUILD_ARG(q)
#define LIBOS_LOG_BUILD_18(fmt, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)                           \
    LIBOS_LOG_BUILD_ARG(b)                                                                                   \
    LIBOS_LOG_BUILD_ARG(c) LIBOS_LOG_BUILD_ARG(d) LIBOS_LOG_BUILD_ARG(e) LIBOS_LOG_BUILD_ARG(f)              \
        LIBOS_LOG_BUILD_ARG(g) LIBOS_LOG_BUILD_ARG(h) LIBOS_LOG_BUILD_ARG(i) LIBOS_LOG_BUILD_ARG(j)          \
            LIBOS_LOG_BUILD_ARG(k) LIBOS_LOG_BUILD_ARG(l) LIBOS_LOG_BUILD_ARG(m) LIBOS_LOG_BUILD_ARG(n)      \
                LIBOS_LOG_BUILD_ARG(o) LIBOS_LOG_BUILD_ARG(p) LIBOS_LOG_BUILD_ARG(q) LIBOS_LOG_BUILD_ARG(r)

#define LIBOS_LOG_BUILD_APPLY(F, ...) F(__VA_ARGS__)
#define APPLY_REMAINDER(...)                                                                                 \
    LIBOS_LOG_BUILD_APPLY(                                                                                   \
        LIBOS_MACRO_PASTE_EVAL(LIBOS_LOG_BUILD_, LIBOS_MACRO_GET_COUNT(__VA_ARGS__)), __VA_ARGS__)

#define LOG_LEVEL_INFO    0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR   2

#define LIBOS_SECTION_LOGGING __attribute__((section(".logging")))

// The compiler used for Windows doesn't have access to the format attribute
__attribute__((format(printf, 1, 2)))
static inline void gccFakePrintf(const char *pFmt, ...)
{
    (void)pFmt;
}

void LibosLogTokens(const libosLogMetadata * metadata, const LibosPrintfArgument * tokens, NvU64 count);

/*!
 *  Cast remaining log arguments to integers for storage
 *
 */
#define LibosLog(level, ...)                                                                                   \
    do                                                                                                         \
    {                                                                                                          \
        gccFakePrintf(__VA_ARGS__);                                                                            \
        static const LIBOS_SECTION_LOGGING char libos_pvt_format[]         = {LIBOS_MACRO_FIRST(__VA_ARGS__)}; \
        static const LIBOS_SECTION_LOGGING char libos_pvt_file[]           = {__FILE__};                       \
        static const LIBOS_SECTION_LOGGING libosLogMetadata libos_pvt_meta = {                                 \
            .filename      = &libos_pvt_file[0],                                                               \
            .format        = &libos_pvt_format[0],                                                             \
            .lineNumber    = __LINE__,                                                                         \
            .argumentCount = LIBOS_MACRO_GET_COUNT(__VA_ARGS__) - 1,                                           \
            .printLevel    = level};                                                                           \
        const LibosPrintfArgument tokens[] = { APPLY_REMAINDER(__VA_ARGS__) };                                 \
        LibosLogTokens(&libos_pvt_meta, &tokens[0], sizeof(tokens) / sizeof(*tokens));                         \
    } while (0)

#ifdef LIBOS_LOGGING_METADATA_SPLIT
/*!
 * When the METADATA_SPLIT feature is enabled, libos print data is split between 4 input sections (all of which
 * must be dropped from the final image). The default .logging is used for str literals and custom strings which
 * are directly referenced by pointer (for %s substitution); .logging_const is used for format strings and the
 * aux metadata; and .logging_metadata is used for metadata vars.
 *
 * The idea is to have a split where  metadata-only changes (i.e. changing what data gets printed, line number changes
 * in file with prints, file renames) will usually not affect the main image, and can be bundled together and
 * relied upon to trigger custom ucode build release behavior.
 * The only exception is renaming headers with prints inside static inline functions; since these don't overlap
 * with basenames, and we can't reliably get a nonvolatile version of a header file name here, such name changes
 * alone won't be able to trigger ucode releases.
 */
#    define LIBOS_SECTION_LOGGING_CONST    __attribute__((section(".logging_const")))
#    define LIBOS_SECTION_LOGGING_METADATA __attribute__((section(".logging_metadata")))
#    define LIBOS_LOGGING_AUX_METADATA_DUMP \
    static const LIBOS_SECTION_LOGGING_CONST int  libos_dummy_line[] LIBOS_ATTR_USED = {__LINE__};
#else // LIBOS_LOGGING_VOLATILE_METADATA_SPLIT
#    define LIBOS_SECTION_LOGGING_CONST  LIBOS_SECTION_LOGGING
#    define LIBOS_SECTION_LOGGING_METADATA  LIBOS_SECTION_LOGGING
#    define LIBOS_LOGGING_AUX_METADATA_DUMP
#endif // LIBOS_LOGGING_VOLATILE_METADATA_SPLIT

/*!
 *  Used for log variables which we want to dump; clients may want to pick these up to check for metadata changes
 */
#define LIBOS_ATTR_USED                __attribute__((used))

/*!
 *  Cast remaining log arguments to integers for storage
 */
#define LIBOS_LOG_INTERNAL(dispatcher, level, ...)                                                             \
    do                                                                                                         \
    {                                                                                                          \
        static const LIBOS_SECTION_LOGGING_CONST char libos_pvt_format[]   = {LIBOS_MACRO_FIRST(__VA_ARGS__)}; \
        static const LIBOS_SECTION_LOGGING_CONST char libos_pvt_file[]     = {__FILE__};                       \
        LIBOS_LOGGING_AUX_METADATA_DUMP;                                                                       \
        static const LIBOS_SECTION_LOGGING_METADATA libosLogMetadata libos_pvt_meta = {                        \
            .filename      = &libos_pvt_file[0],                                                               \
            .format        = &libos_pvt_format[0],                                                             \
            .lineNumber    = __LINE__,                                                                         \
            .argumentCount = LIBOS_MACRO_GET_COUNT(__VA_ARGS__) - 1,                                           \
            .printLevel    = level};                                                                           \
        const NvU64 tokens[] = {APPLY_REMAINDER(__VA_ARGS__)(NvU64) & libos_pvt_meta};                         \
        dispatcher(sizeof(tokens) / sizeof(*tokens), &tokens[0]);                                              \
    } while (0)

/*!
 *  Cast remaining log arguments to integers for storage
 *  This macro is used when logging for other task's address
 */
#define LIBOS_LOG_ADDRESS(dispatcher, taskElfSectionName, level, ...)                                          \
    do                                                                                                         \
    {                                                                                                          \
        static const LIBOS_SECTION_LOGGING_CONST char libos_pvt_format[]   = {LIBOS_MACRO_FIRST(__VA_ARGS__)}; \
        static const LIBOS_SECTION_LOGGING_CONST char libos_pvt_file[]     = {__FILE__};                       \
        static const LIBOS_SECTION_LOGGING_CONST char libos_pvt_elf_name[] = {taskElfSectionName};             \
        LIBOS_LOGGING_AUX_METADATA_DUMP;                                                                       \
        const libosLogMetadata libos_pvt_meta = {                                                              \
            .filename      = &libos_pvt_file[0],                                                               \
            .format        = &libos_pvt_format[0],                                                             \
            .lineNumber    = __LINE__,                                                                         \
            .argumentCount = LIBOS_MACRO_GET_COUNT(__VA_ARGS__) - 1,                                           \
            .printLevel    = level};                                                                           \
        static const LIBOS_SECTION_LOGGING_METADATA libosLogMetadata_extended libos_pvt_meta_ex = {            \
            .versionExtended = 0x8000000000000000ULL,                                                          \
            .elfSectionName  = &libos_pvt_elf_name[0],                                                         \
            .meta            = libos_pvt_meta};                                                                \
        const NvU64 tokens[] = {APPLY_REMAINDER(__VA_ARGS__)(NvU64) & libos_pvt_meta_ex};                      \
        dispatcher(sizeof(tokens) / sizeof(*tokens), &tokens[0]);                                              \
    } while (0)

#endif
