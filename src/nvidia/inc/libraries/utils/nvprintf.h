/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /**
  * @file
  * @brief Standard printf logging interface
  */

#ifndef _NV_UTILS_PRINTF_H_
#define _NV_UTILS_PRINTF_H_

#ifdef __cplusplus
extern "C" {
#endif


/// @defgroup NV_PRINTF_LEVELS Printf verbosity levels
/// @{
/// @brief Prints at this level are discarded
#define LEVEL_SILENT   0x0
/// @brief Verbose debug logging level        (e.g. signaling function entry)
#define LEVEL_INFO     0x1
/// @brief Standard debug logging level       (e.g. Illegal ctrcall call)
#define LEVEL_NOTICE   0x2
/// @brief Warning logging level              (e.g. feature not supported)
#define LEVEL_WARNING  0x3
/// @brief Error logging level                (e.g. resource allocation failed)
#define LEVEL_ERROR    0x4
/// @brief Recoverable HW error               (e.g. RC events)
#define LEVEL_HW_ERROR 0x5
/// @brief Unrecoverable error                (e.g. Bus timeout)
#define LEVEL_FATAL    0x6
/// @}

// Used only in nvlogFilterApplyRule()
#define NV_LEVEL_MAX   LEVEL_FATAL

/**
 * @def NV_PRINTF_LEVEL_ENABLED(level)
 * @brief This macro evaluates to 1 if prints of a given level will be compiled.
 *
 * By default, it is available on all builds that allow strings
 */
#ifndef NV_PRINTF_LEVEL_ENABLED
#define NV_PRINTF_LEVEL_ENABLED(level)  ((level) >= NV_PRINTF_LEVEL)
#endif

#if defined(GSP_PLUGIN_BUILD) || (defined(NVRM) && NVCPU_IS_RISCV64)

/**
 * GSPRM uses a different system for logging.
 * The format strings, filename, line number, etc. are stored in a separate
 * data segment that is not loaded on the GSP, but is available to the decoder.
 */

#include "libos_log.h"

/**
 * Define NV_PRINTF_LEVEL to the minimum level for debug output.  This is compared
 * to the level for each NV_PRINT to cull them at compile time.
 */
#define NV_PRINTF_LEVEL  LEVEL_NOTICE

#undef  NV_PRINTF_ENABLED
#define NV_PRINTF_ENABLED              1

#undef  NV_PRINTF_STRINGS_ALLOWED
#define NV_PRINTF_STRINGS_ALLOWED      0

#define NV_PRINTF_STRING_SECTION       LIBOS_SECTION_LOGGING

#define MAKE_NV_PRINTF_STR(str)                                     \
({                                                                  \
    static NV_PRINTF_STRING_SECTION const char rm_pvt_str[] = str;  \
    rm_pvt_str;                                                     \
})

// NVLOG is not used on GSP-RM.
#undef  NVLOG_LEVEL
#define NVLOG_LEVEL LEVEL_FATAL

// Direct dmesg printing through NV_PRINTF_STRING is a no-op on GSP-RM
#define NV_PRINTF_STRING(module, level, format, ...)

#if   defined(GSP_PLUGIN_BUILD)

void log_vgpu_log_entry(const NvU64 n_args, const NvU64 * args);

#define NV_PRINTF(level, format, ...) do {                                     \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        LIBOS_LOG_INTERNAL(log_vgpu_log_entry, LOG_LEVEL_ERROR,                \
            format, ##__VA_ARGS__);                                            \
    }                                                                          \
} while (0)

#define NV_PRINTF_EX(module, level, format, ...) do {                          \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        LIBOS_LOG_INTERNAL(log_vgpu_log_entry, LOG_LEVEL_ERROR,                \
            format, ##__VA_ARGS__);                                            \
    }                                                                          \
} while (0)

#define NVLOG_PRINTF(...)

#else

void log_rm_log_entry(const NvU64 n_args, const NvU64 * args);

#define NV_PRINTF(level, format, ...) do {                                     \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        LIBOS_LOG_INTERNAL(log_rm_log_entry, LOG_LEVEL_ERROR,                  \
            format, ##__VA_ARGS__);                                            \
    }                                                                          \
} while (0)

#define NV_PRINTF_EX(module, level, format, ...) do {                          \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        LIBOS_LOG_INTERNAL(log_rm_log_entry, LOG_LEVEL_ERROR,                  \
            format, ##__VA_ARGS__);                                            \
    }                                                                          \
} while (0)

#endif // NVOC

#else // defined(NVRM) && NVCPU_IS_RISCV64

/**
 * @defgroup NV_UTILS_PRINTF Utility Printing Macros
 *
 * @brief Provides a light abstraction layer for printf logging.
 *
 * NvPort and NvLog are used for portability and logging primitives.
 * If an environment cannot use these directly then it can override
 * the NV_PORT_HEADER and NV_LOG_HEADER defines in its makefile
 * to point to appropriate replacements.
 * @{
 */

#ifndef NV_PRINTF_PREFIX
/**
 * @brief Prefix to prepend to all messages printed by @ref NV_PRINTF.
 */
#define NV_PRINTF_PREFIX ""
#endif

#ifndef NV_PRINTF_PREFIX_SEPARATOR
/**
 * @brief Separator between prefix  messages printed by @ref NV_PRINTF.
 *
 * If defined, it must be a single character followed by an optional space.
 */
#define NV_PRINTF_PREFIX_SEPARATOR ""
#endif

#ifndef NV_PRINTF_ADD_PREFIX
/**
 * @brief Apply the full prefix string to a format string.
 *
 * This is a function-like macro so it can support inserting arguments after the
 * format string. Example:
 *    #define NV_PRINTF_ADD_PREFIX(fmt) "%s():"fmt, __FUNCTION__
 */
#define NV_PRINTF_ADD_PREFIX(fmt) NV_PRINTF_PREFIX NV_PRINTF_PREFIX_SEPARATOR fmt
#endif

// Include portability header, falling back to NvPort if not provided.
#ifndef NV_PORT_HEADER
#define NV_PORT_HEADER "nvport/nvport.h"
#endif
#include NV_PORT_HEADER


// Include logging header, falling back to NvLog if not provided.
#ifndef NV_LOG_HEADER
#define NV_LOG_HEADER "nvlog/nvlog_printf.h"
#endif
#include NV_LOG_HEADER

#define NV_PRINTF_STRING_SECTION

#define MAKE_NV_PRINTF_STR(str) str

/**
 * @def NV_PRINTF(level, format, args...)
 * @brief Standard formatted printing/logging interface.
 *
 * @param level   - Debug level to print at. One of @ref NV_PRINTF_LEVELS
 * @param format  - A standard printf format string. Must be a string literal.
 * @param args... - Arguments for the format string literal, like regular printf
 *
 * The logging header can redefine the behavior, but the basic implementation
 * will just print to standard output, like the printf function.
 *
 * This will print to the @ref NV_PRINTF_MODULE module. If the module is not
 * defined, it will default to GLOBAL. Use @ref NV_PRINTF_EX to specify another
 * module.
 *
 * This will prefix the prints with @ref NV_PRINTF_PREFIX string and function
 * name. To specify a different (or no) prefix, use @ref NV_PRINTF_EX
 *
 * @note The format string must be a string literal. The level can be a variable,
 * but it may have positive speed and size effects to use the above levels
 * directly.
 */
#ifndef NV_PRINTF
#define NV_PRINTF(level, format, ...) \
    NV_PRINTF_EX(NV_PRINTF_MODULE, level, NV_PRINTF_ADD_PREFIX(format), ##__VA_ARGS__)
#endif



/**
 * @def NV_PRINTF_EX(module, level, format, args...)
 * @brief Extended version of the standard @ref NV_PRINTF
 *
 * This interface allows you to explicitly specify the module to print to and
 * doesn't perform any automatic prefixing.
 *
 * The logging header can redefine the behavior, but the basic implementation
 * will just print to standard output, like the printf function.
 *
 * @note The format string must be a string literal. The level can be a variable,
 * but it may have positive speed and size effects to use the above levels
 * directly.
 */
#ifndef NV_PRINTF_EX
#define NV_PRINTF_EX(module, level, format, ...)                            \
    do                                                                      \
    {                                                                       \
        NVLOG_PRINTF(module, NVLOG_ROUTE_RM, level, format, ##__VA_ARGS__); \
        NV_PRINTF_STRING(module, level, format, ##__VA_ARGS__);             \
    } while (0)
#endif


/**
 * @def NV_PRINTF_STRINGS_ALLOWED
 * @brief This switch controls whether strings are allowed to appear in the
 * final binary.
 *
 * By default, strings are allowed on DEBUG and QA builds, and all MODS builds
 */
#ifndef NV_PRINTF_STRINGS_ALLOWED
#if defined(DEBUG) || defined(NV_MODS) || defined(QA_BUILD)
#define NV_PRINTF_STRINGS_ALLOWED 1
#else
#define NV_PRINTF_STRINGS_ALLOWED 0
#endif
#endif // NV_PRINTF_STRINGS_ALLOWED



//
// Default values for the compile time switches:
// - Strings are allowed on DEBUG and QA builds, and all MODS builds
// - NV_PRINTF is only available if strings are allowed
// - All levels are available if NV_PRINTF is available.



//
// Special handling for RM internal prints so we have equivalent functionality
// between NV_PRINTF and DBG_PRINTF. This is needed to seamlessly migrate RM to
// the new interface. The implementations will eventually be fully extracted and
// only depend on other common code, such as NvPort.
//
#if defined(NVRM) && !defined(NVWATCH)

#undef NV_PRINTF_PREFIX
#define NV_PRINTF_PREFIX "NVRM"
#undef NV_PRINTF_PREFIX_SEPARATOR
#define NV_PRINTF_PREFIX_SEPARATOR ": "

#if NV_PRINTF_STRINGS_ALLOWED

// Declare internal RM print function:
// This is utDbg_Printf in unit tests and nvDbg_Printf in regular RM builds
#if defined(RM_UNITTEST)
#define NVRM_PRINTF_FUNCTION utDbg_Printf
#else
#define NVRM_PRINTF_FUNCTION nvDbg_Printf
#endif // defined(RM_UNITTEST)

void NVRM_PRINTF_FUNCTION(const char *file,
                          int line,
                          const char *function,
                          int debuglevel,
                          const char *s,
                          ...) NVPORT_CHECK_PRINTF_ARGUMENTS(5, 6);

#define NV_PRINTF_STRING(module, level, format, ...) \
     NVRM_PRINTF_FUNCTION(NV_FILE_STR, __LINE__, NV_FUNCTION_STR, level, format, ##__VA_ARGS__)

#endif // NV_PRINTF_STRINGS_ALLOWED

// RM always has printf enabled
#define NV_PRINTF_ENABLED 1
#endif // defined(NVRM) && !defined(NVWATCH)


//
// Default definitions if none are specified
//

/**
 * @def NV_PRINTF_ENABLED
 * @brief This macro evaluates to 1 if NV_PRINTF is available (either as regular
 * debug prints or binary logging)
 *
 * By default, it is available on all builds that allow strings
 */
#ifndef NV_PRINTF_ENABLED
#define NV_PRINTF_ENABLED NV_PRINTF_STRINGS_ALLOWED
#endif

#if NV_PRINTF_STRINGS_ALLOWED
#define NV_PRINTF_LEVEL  LEVEL_INFO
#else
#define NV_PRINTF_LEVEL  LEVEL_NOTICE
#endif

/**
 * @def NV_PRINTF_STRING(module, level, format, ...)
 * @brief Prints the string to the given output, if strings are allowed.
 */
#ifndef NV_PRINTF_STRING
#if NV_PRINTF_STRINGS_ALLOWED
#define NV_PRINTF_STRING(module, level, format, ...) \
    portDbgPrintf(format, ##__VA_ARGS__)

#if !defined(portDbgPrintf) && !PORT_IS_FUNC_SUPPORTED(portDbgPrintf)
#error "NV_PORT_HEADER must implement portDbgPrintf()"
#endif

#else
#define NV_PRINTF_STRING(module, level, format, ...)
#endif
#endif // NV_PRINTF_STRING

#ifndef NVLOG_PRINTF
#define NVLOG_PRINTF(...)
#endif

#endif // defined(NVRM) && NVCPU_IS_RISCV64

/**
 * @def NV_PRINTF_COND(condition, leveltrue, levelfalse, format, args...)
 * @brief NV_PRINTF with conditional print level
 *
 * Splits NV_PRINTF calls with a print level based on a variable or ternary
 * operation, to be handled by preprocessors to remove INFO-level prints
 *
 * If condition is true, uses leveltrue, else uses levelfalse
 */
#ifndef NV_PRINTF_COND
#define NV_PRINTF_COND(condition, leveltrue, levelfalse, format, ...)       \
    do {                                                                    \
        if (condition)                                                      \
        {                                                                   \
            NV_PRINTF(leveltrue, format, ##__VA_ARGS__);                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            NV_PRINTF(levelfalse, format, ##__VA_ARGS__);                   \
        }                                                                   \
    } while (0)
#endif


//
// NV_FILE and NV_FUNCTION macros are used to wrap the __FILE__ and __FUNCTION__
// macros, respectively, to enable passing them as parameters on release builds
// without linking the strings into the object files.  Instead, this will use
// NV_LOG and other utilities to pass values in a way that the same information
// can be decoded on retail builds.
//
// On non-release builds, the strings are directly referenced and included in
// the builds (just like their normal references in DBG_PRINTF() and
// DBG_BREAKPOINT()).
//
// In MODS builds, we allow all printfs, but don't automatically include the
// __FILE__ or __FUNCTION__ references.
//
#if NV_PRINTF_STRINGS_ALLOWED && (!defined(NV_MODS) || defined(SIM_BUILD) || defined(DEBUG) || defined(NV_MODS_INTERNAL))
#define NV_FILE_STR      __FILE__
#define NV_FILE          __FILE__
#define NV_FILE_FMT      "%s"
#define NV_FILE_TYPE     const char *
#define NV_FUNCTION_STR  __FUNCTION__
#define NV_FUNCTION      __FUNCTION__
#define NV_FUNCTION_FMT  "%s"
#define NV_FUNCTION_TYPE const char *
#else
#ifndef NV_FILE_STR
#define NV_FILE_STR      "<file>"
#endif
#ifdef NVLOG_FILEID
#   define NV_FILE          NVLOG_FILEID
#else
#   define NV_FILE          0
#endif
#define NV_FILE_FMT      "<fileid:0x%06x>"
#define NV_FILE_TYPE     NvU32
//
// A couple caveats on portUtilExGetStackTrace():
//
// 1.  portUtilExGetStackTrace is not supported on all builds.  For example, see
// GCC support in util-gcc-clang.h.
//
// 2.  portUtilExGetStackTrace(0) will give us the current IP, which is
// current_function()+offset. Commands such as `ln` in windbg can translate the
// IP into func+offset. But sometimes, due to inlining/optimizations, the
// current function at runtime is not the same as at compile time.  In the
// inlining example, if a function using NV_FUNCTION is inlined, the pointer
// printed will be calling_function()+offset.
//
//#define NV_FUNCTION      portUtilExGetStackTrace(0)
#define NV_FUNCTION_STR  "<func>"
#define NV_FUNCTION      0
#define NV_FUNCTION_FMT  "<func:%p>"
#define NV_FUNCTION_TYPE NvUPtr
#endif

#ifdef __cplusplus
}
#endif //__cplusplus

/// @}
#endif // _NV_UTILS_PRINTF_H_
