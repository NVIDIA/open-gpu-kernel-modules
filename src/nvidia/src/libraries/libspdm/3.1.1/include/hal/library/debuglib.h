/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef DEBUG_LIB_H
#define DEBUG_LIB_H

/** @file
 * Provides services to print debug and assert messages to a debug output device.
 *
 * The debug library supports debug print and asserts based on a combination of macros and code.
 * The debug library can be turned on and off so that the debug code does not increase the size of
 * an image.
 **/

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"

#if LIBSPDM_DEBUG_PRINT_ENABLE

/* Declare bits for the error_level parameter of libspdm_debug_print(). */
#define LIBSPDM_DEBUG_INFO 0x00000040
#define LIBSPDM_DEBUG_ERROR 0x80000000

/**
 * Prints a debug message to the debug output device if the specified error level is enabled.
 *
 * @param  error_level  The error level of the debug message, either LIBSPDM_DEBUG_INFO or
 *                      LIBSPDM_DEBUG_ERROR.
 * @param  format       The format string for the debug message to print.
 * @param  ...          The variable argument list whose contents are accessed
 *                      based on the format string specified by format.
 **/
extern void libspdm_debug_print(size_t error_level, const char *format, ...);
#endif /* LIBSPDM_DEBUG_PRINT_ENABLE */

#if LIBSPDM_DEBUG_ASSERT_ENABLE
/**
 * Prints an assert message containing a filename, line number, and description.
 * This may be followed by a breakpoint or a dead loop.
 *
 * @param  file_name     The pointer to the name of the source file that generated the assert
 *                       condition.
 * @param  line_number   The line number in the source file that generated the assert condition
 * @param  description   The pointer to the description of the assert condition.
 *
 **/
extern void libspdm_debug_assert(const char *file_name, size_t line_number,
                                 const char *description);
#endif /* LIBSPDM_DEBUG_ASSERT_ENABLE */

/**
 * LIBSPDM_ASSERT(expression) - Macro that calls libspdm_debug_assert() if an expression evaluates
 *                              to false. It is enabled through the LIBSPDM_DEBUG_ASSERT_ENABLE
 *                              macro.
 *
 * @param  expression  Boolean expression.
 **/
#if LIBSPDM_DEBUG_ASSERT_ENABLE
#define LIBSPDM_ASSERT(expression) \
    do { \
        if (!(expression)) { \
            libspdm_debug_assert(__FILE__, __LINE__, #expression); \
        } \
    } while (false)
#else
#define LIBSPDM_ASSERT(expression)
#endif

/**
 * LIBSPDM_DEBUG(level, format_string, argument_list...) - Macro that calls libspdm_debug_print().
 *                                                         It is enabled through the
 *                                                         LIBSPDM_PRINT_ENABLE macro.
 * @param print_level    Either LIBSPDM_DEBUG_INFO or LIBSPDM_DEBUG_ERROR.
 * @param format_string  Formatted string.
 * @param argument_list  List of arguments.
 *
 * Note that format_string and argument_list are the same as those defined by the C printf function.
 **/
#if LIBSPDM_DEBUG_PRINT_ENABLE
#define LIBSPDM_DEBUG(expression) \
    do { \
        LIBSPDM_DEBUG_INTERNAL(expression); \
    } while (false)

#define LIBSPDM_DEBUG_PRINT_INTERNAL(print_level, ...) \
    do { \
        libspdm_debug_print(print_level, ## __VA_ARGS__); \
    } while (false)

#define LIBSPDM_DEBUG_INTERNAL(expression) LIBSPDM_DEBUG_PRINT_INTERNAL expression
#else
#define LIBSPDM_DEBUG(expression)
#endif

/**
 * LIBSPDM_DEBUG_CODE(expression) - Macro that defines a section of debug source code.
 *                                  It is enabled through the LIBSPDM_BLOCK_ENABLE macro.
 *
 * @param expression  One or more lines of expressions that are used for debugging and should not
 *                    affect the behavior of functional code.
 **/
#if LIBSPDM_DEBUG_BLOCK_ENABLE
#define LIBSPDM_DEBUG_CODE(expression) \
    do { \
        uint8_t __debug_code_local; \
        expression \
            __debug_code_local = 0; \
        __debug_code_local++; \
    } while (false)
#else
#define LIBSPDM_DEBUG_CODE(expression)
#endif

#endif /* DEBUG_LIB_H */
