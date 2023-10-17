/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Debug module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_DEBUG_H_
#define _NVPORT_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup NVPORT_DEBUG Debug Support Routines
 * @brief This module provides debug support routines like breakpoints and prints.
 * @{
 */

/** @brief See @ref PORT_UTIL_INLINE */
#ifndef PORT_DEBUG_INLINE
#define PORT_DEBUG_INLINE PORT_INLINE
#endif

/**
 * @name Core Functions
 * @{
 *
 * @note The breakpoint macro comes in several flavors:
 *  - @ref PORT_BREAKPOINT -
 *             Shouldn't be used directly
 *  - @ref PORT_BREAKPOINT_DEBUG -
 *             Causes a breakpoint in debug builds only, use for all debug purposes.
 *  - @ref PORT_BREAKPOINT_CHECKED -
 *             Causes a breakpoint in checked builds only, use when you want the
 *             @c int3 present in develop and release builds, such as QA builds.
 *  - @ref PORT_BREAKPOINT_ALWAYS -
 *             Always breaks, use only if you want to trigger @c int3 even on
 *             public release builds.
 */


/**
 * @brief Prints a string to a platform dependent output stream
 *
 * This function will print the string where you would expect it for a given
 * platform.  In user space it will be standard out.  In kernel space it will
 * be the kernel debug log.
 *
 * Note NvPort does not provide advanced logging capabilities, only the ability
 * to emit a string.  For a more robust logging solution see the NvLog project.
 *
 */
PORT_DEBUG_INLINE void portDbgPrintString(const char *str, NvLength length);

/**
 * @brief Convenience macro when printing a string literal.
 */
#define PORT_DBG_PRINT_STRING_LITERAL(s) portDbgPrintString(s, sizeof(s)-1)

/**
 * @def PORT_BREAKPOINT_DEBUG_ENABLED
 * @brief Controls whether @ref PORT_BREAKPOINT_DEBUG is enabled or not
 */
#if !defined(PORT_BREAKPOINT_DEBUG_ENABLED)
#if defined(DEBUG)
#define PORT_BREAKPOINT_DEBUG_ENABLED 1
#else
#define PORT_BREAKPOINT_DEBUG_ENABLED 0
#endif
#endif


/**
 * @def PORT_BREAKPOINT_DEBUG
 * @brief Cause a breakpoint into the debugger only when
 * @ref PORT_BREAKPOINT_DEBUG_ENABLED is defined.
 *
 * By default PORT_BREAKPOINT_DEBUG_ENABLED is set based on the value of DEBUG.
 * However it is kept as a separate define so you can override separately if so
 * desired.
 */
#if PORT_BREAKPOINT_DEBUG_ENABLED
#define PORT_BREAKPOINT_DEBUG PORT_BREAKPOINT
#else
#define PORT_BREAKPOINT_DEBUG()
#endif

#define PORT_FILE_STR __FILE__

/// @cond NVPORT_INTERNAL
#if !defined(PORT_ASSERT_FAILED_USES_STRINGS)
#define PORT_ASSERT_FAILED_USES_STRINGS PORT_IS_CHECKED_BUILD
#endif

#if PORT_ASSERT_FAILED_USES_STRINGS
#define _PORT_STRINGIFY2(x) #x
#define _PORT_STRINGIFY(x) _PORT_STRINGIFY2(x)
#define _PORT_ASSERT_MESSAGE(cond) "Assertion failed: \"" #cond "\" at "         \
                                   PORT_FILE_STR ":" _PORT_STRINGIFY(__LINE__) "\n"
#else
#define _PORT_ASSERT_MESSAGE(cond) "Assertion failed"
#endif
/// @endcond

/**
 * @brief Causes a breakpoint if the condition evaluates to false.
 */
#define PORT_ASSERT(cond)                                                      \
    do                                                                         \
    {                                                                          \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (!(cond))                                                           \
        {                                                                      \
            PORT_DBG_PRINT_STRING_LITERAL(_PORT_ASSERT_MESSAGE(cond));         \
            PORT_BREAKPOINT();                                                 \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } while (0)

/*
 * Checks osDbgBreakpointEnabled and PDB_PROP_SYS_DEBUGGER_DISABLED
 * to see if breakpoints are allowed
 */
NvBool nvDbgBreakpointEnabled(void);

/**
 * @def PORT_BREAKPOINT_CHECKED()
 * @brief Causes a breakpoint in checked builds only
 */
/**
 * @def PORT_ASSERT_CHECKED(x)
 * @brief Causes an assert in checked builds only
 */
#if PORT_IS_CHECKED_BUILD

/*
 * TODO:  defined(NVRM) && PORT_IS_KERNEL_BUILD && defined(NVWATCH) are all true
 *        when NvWatch is included in the Debug Linux AMD64 Mfg Mods build.
 *        This seems wrong...
 */
#if defined(NVRM) && PORT_IS_KERNEL_BUILD == 1 && !defined(NVWATCH)
#define PORT_BREAKPOINT_CHECKED()                                              \
    do                                                                         \
    {                                                                          \
        if (nvDbgBreakpointEnabled())                                          \
            PORT_BREAKPOINT();                                                 \
    } while (0)
#else
#define PORT_BREAKPOINT_CHECKED() PORT_BREAKPOINT()
#endif
#define PORT_ASSERT_CHECKED(x)    PORT_ASSERT(x)
#else // PORT_IS_CHECKED_BUILD
#define PORT_BREAKPOINT_CHECKED()
#define PORT_ASSERT_CHECKED(x)
#endif // PORT_IS_CHECKED_BUILD

/**
 * @brief Causes a breakpoint into the debugger regardless of build configuration.
 *
 * Note this is equivalent to just calling @ref PORT_BREAKPOINT.  It is only
 * included to provide an alternative to @ref PORT_BREAKPOINT_DEBUG that is
 * consistent in look and usage.
 */
#define PORT_BREAKPOINT_ALWAYS PORT_BREAKPOINT

/**
 * @def PORT_COVERAGE_PUSH_OFF()
 * @brief Saves the current coverage tracking state to a stack and disables it
 *
 * This is useful to do around some error checking code (e.g. "default:") so the
 * bullseye tool doesn't take those branches into account when checking code
 * coverage.
 *
 * - See @ref PORT_ASSERT for usage example.
 * - See https://www.bullseye.com/help/build-exclude.html for more details.
 */
/**
 * @def PORT_COVERAGE_PUSH_ON()
 * @brief Saves the current coverage tracking state to a stack and enables it
 */
/**
 * @def PORT_COVERAGE_POP()
 * @brief Restores the last saved coverage tracking state
 *
 * See @ref PORT_ASSERT for usage example.
 */
#if defined(NV_BULLSEYE)
#define PORT_COVERAGE_PUSH_OFF() "BullseyeCoverage save off"
#define PORT_COVERAGE_PUSH_ON()  "BullseyeCoverage save on"
#define PORT_COVERAGE_POP()      "BullseyeCoverage restore"
#else
#define PORT_COVERAGE_PUSH_OFF()
#define PORT_COVERAGE_PUSH_ON()
#define PORT_COVERAGE_POP()
#endif



/// @} End core functions

/**
 * @def NVPORT_CHECK_PRINTF_ARGUMENTS(a,b)
 * @brief Compile time check that arguments conform to printf rules
 */
#if PORT_COMPILER_HAS_ATTRIBUTE_FORMAT
#define NVPORT_CHECK_PRINTF_ARGUMENTS(a,b) __attribute__((format(printf, a, b)))
#else
#define NVPORT_CHECK_PRINTF_ARGUMENTS(a,b)
#endif

/**
 * @name Extended Functions
 * @{
 */

#if !defined(portDbgPrintf_SUPPORTED)
#define portDbgPrintf_SUPPORTED 0
#endif
#if !defined(portDbgExPrintfLevel_SUPPORTED)
#define portDbgExPrintfLevel_SUPPORTED 0
#endif

#if PORT_IS_FUNC_SUPPORTED(portDbgPrintf)
/**
 * @brief Prints a formatted string to using @ref portDbgPrintString
 *
 * The parameters are like those of printf().
 */
PORT_DEBUG_INLINE void portDbgPrintf(const char *format, ...) NVPORT_CHECK_PRINTF_ARGUMENTS(1, 2);
#endif

#if PORT_IS_FUNC_SUPPORTED(portDbgExPrintfLevel)
/**
 * @brief Similar to @ref portDbgPrintf, except that it passes the level to the
 * underlying implementation.
 *
 * Some platforms (e.g. MODS) have an API where prints are given a level, and
 * some tools may depend on certain prints being at a certain level. This
 * function simply passes the level to that API- NvPort does not understand
 * or filter these levels.
 *
 * @param level - An int representing the level at which to print.
 */
PORT_DEBUG_INLINE void portDbgExPrintfLevel(NvU32 level, const char *format, ...) NVPORT_CHECK_PRINTF_ARGUMENTS(2, 3);
#endif

/// @} End extended functions

// Include platform specific inline definitions

#if   NVOS_IS_QNX
#include "nvport/inline/debug_qnx.h"
#elif NVOS_IS_DCECORE
#include "nvport/inline/debug_dcecore.h"
#else

#if PORT_IS_KERNEL_BUILD

#if NVOS_IS_WINDOWS
#include "nvport/inline/debug_win_kernel.h"
#elif NVOS_IS_UNIX
#include "nvport/inline/debug_unix_kernel_os.h"
#elif NVOS_IS_LIBOS
#include "nvport/inline/debug_libos.h"
#else
#error "Unsupported target OS"
#endif

#else // Usermode build

#if NVOS_IS_WINDOWS
#include "nvport/inline/debug_win_user.h"
#elif NVOS_IS_UNIX
#include "nvport/inline/debug_unix_user.h"
#elif NVOS_IS_LIBOS
#include "nvport/inline/debug_libos.h"
#else
#error "Unsupported target OS"
#endif

#endif // PORT_IS_KERNEL_BUILD
#endif // NV_MODS

#if !defined(PORT_DUMP_STACK)
#define PORT_DUMP_STACK() do {} while (0)
#endif

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // _NVPORT_DEBUG_H_
/// @}
