/**
 * \file platform_util.h
 *
 * \brief Common and shared functions used by multiple modules in the Mbed TLS
 *        library.
 */
//
//  Copyright The Mbed TLS Contributors
//  NVIDIA_EDIT: This version of MbedTLS is dual-licensed - NVIDIA
//  receives it under the GPL-2.0 license - these headers are updated
//  accordingly.
//
//  SPDX-License-Identifier: GPL-2.0
//
//  This file is provided under the GNU General Public License v2.0.
//
//  **********
//  GNU General Public License v2.0:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation version 2 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  **********
//
#ifndef MBEDTLS_PLATFORM_UTIL_H
#define MBEDTLS_PLATFORM_UTIL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>
#if defined(MBEDTLS_HAVE_TIME_DATE)
#include "platform_time.h"
#include <time.h>
#endif /* MBEDTLS_HAVE_TIME_DATE */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_CHECK_PARAMS)

#if defined(MBEDTLS_CHECK_PARAMS_ASSERT)
/* Allow the user to define MBEDTLS_PARAM_FAILED to something like assert
 * (which is what our config.h suggests). */
#include <assert.h>
#endif /* MBEDTLS_CHECK_PARAMS_ASSERT */

#if defined(MBEDTLS_PARAM_FAILED)
/** An alternative definition of MBEDTLS_PARAM_FAILED has been set in config.h.
 *
 * This flag can be used to check whether it is safe to assume that
 * MBEDTLS_PARAM_FAILED() will expand to a call to mbedtls_param_failed().
 */
#define MBEDTLS_PARAM_FAILED_ALT

#elif defined(MBEDTLS_CHECK_PARAMS_ASSERT)
#define MBEDTLS_PARAM_FAILED( cond ) assert( cond )
#define MBEDTLS_PARAM_FAILED_ALT

#else /* MBEDTLS_PARAM_FAILED */
#define MBEDTLS_PARAM_FAILED( cond ) \
    mbedtls_param_failed( #cond, __FILE__, __LINE__ )

/**
 * \brief       User supplied callback function for parameter validation failure.
 *              See #MBEDTLS_CHECK_PARAMS for context.
 *
 *              This function will be called unless an alternative treatement
 *              is defined through the #MBEDTLS_PARAM_FAILED macro.
 *
 *              This function can return, and the operation will be aborted, or
 *              alternatively, through use of setjmp()/longjmp() can resume
 *              execution in the application code.
 *
 * \param failure_condition The assertion that didn't hold.
 * \param file  The file where the assertion failed.
 * \param line  The line in the file where the assertion failed.
 */
void mbedtls_param_failed( const char *failure_condition,
                           const char *file,
                           int line );
#endif /* MBEDTLS_PARAM_FAILED */

/* Internal macro meant to be called only from within the library. */
#define MBEDTLS_INTERNAL_VALIDATE_RET( cond, ret )  \
    do {                                            \
        if( !(cond) )                               \
        {                                           \
            MBEDTLS_PARAM_FAILED( cond );           \
            return( ret );                          \
        }                                           \
    } while( 0 )

/* Internal macro meant to be called only from within the library. */
#define MBEDTLS_INTERNAL_VALIDATE( cond )           \
    do {                                            \
        if( !(cond) )                               \
        {                                           \
            MBEDTLS_PARAM_FAILED( cond );           \
            return;                                 \
        }                                           \
    } while( 0 )

#else /* MBEDTLS_CHECK_PARAMS */

/* Internal macros meant to be called only from within the library. */
#define MBEDTLS_INTERNAL_VALIDATE_RET( cond, ret )  do { } while( 0 )
#define MBEDTLS_INTERNAL_VALIDATE( cond )           do { } while( 0 )

#endif /* MBEDTLS_CHECK_PARAMS */

/* Internal helper macros for deprecating API constants. */
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_DEPRECATED_WARNING)
/* Deliberately don't (yet) export MBEDTLS_DEPRECATED here
 * to avoid conflict with other headers which define and use
 * it, too. We might want to move all these definitions here at
 * some point for uniformity. */
#define MBEDTLS_DEPRECATED __attribute__((deprecated))
MBEDTLS_DEPRECATED typedef char const * mbedtls_deprecated_string_constant_t;
#define MBEDTLS_DEPRECATED_STRING_CONSTANT( VAL )       \
    ( (mbedtls_deprecated_string_constant_t) ( VAL ) )
MBEDTLS_DEPRECATED typedef int mbedtls_deprecated_numeric_constant_t;
#define MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( VAL )       \
    ( (mbedtls_deprecated_numeric_constant_t) ( VAL ) )
#undef MBEDTLS_DEPRECATED
#else /* MBEDTLS_DEPRECATED_WARNING */
#define MBEDTLS_DEPRECATED_STRING_CONSTANT( VAL ) VAL
#define MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( VAL ) VAL
#endif /* MBEDTLS_DEPRECATED_WARNING */
#endif /* MBEDTLS_DEPRECATED_REMOVED */

/**
 * \brief       Securely zeroize a buffer
 *
 *              The function is meant to wipe the data contained in a buffer so
 *              that it can no longer be recovered even if the program memory
 *              is later compromised. Call this function on sensitive data
 *              stored on the stack before returning from a function, and on
 *              sensitive data stored on the heap before freeing the heap
 *              object.
 *
 *              It is extremely difficult to guarantee that calls to
 *              mbedtls_platform_zeroize() are not removed by aggressive
 *              compiler optimizations in a portable way. For this reason, Mbed
 *              TLS provides the configuration option
 *              MBEDTLS_PLATFORM_ZEROIZE_ALT, which allows users to configure
 *              mbedtls_platform_zeroize() to use a suitable implementation for
 *              their platform and needs
 *
 * \param buf   Buffer to be zeroized
 * \param len   Length of the buffer in bytes
 *
 */
void mbedtls_platform_zeroize( void *buf, size_t len );

#if defined(MBEDTLS_HAVE_TIME_DATE)
/**
 * \brief      Platform-specific implementation of gmtime_r()
 *
 *             The function is a thread-safe abstraction that behaves
 *             similarly to the gmtime_r() function from Unix/POSIX.
 *
 *             Mbed TLS will try to identify the underlying platform and
 *             make use of an appropriate underlying implementation (e.g.
 *             gmtime_r() for POSIX and gmtime_s() for Windows). If this is
 *             not possible, then gmtime() will be used. In this case, calls
 *             from the library to gmtime() will be guarded by the mutex
 *             mbedtls_threading_gmtime_mutex if MBEDTLS_THREADING_C is
 *             enabled. It is recommended that calls from outside the library
 *             are also guarded by this mutex.
 *
 *             If MBEDTLS_PLATFORM_GMTIME_R_ALT is defined, then Mbed TLS will
 *             unconditionally use the alternative implementation for
 *             mbedtls_platform_gmtime_r() supplied by the user at compile time.
 *
 * \param tt     Pointer to an object containing time (in seconds) since the
 *               epoch to be converted
 * \param tm_buf Pointer to an object where the results will be stored
 *
 * \return      Pointer to an object of type struct tm on success, otherwise
 *              NULL
 */
struct tm *mbedtls_platform_gmtime_r( const mbedtls_time_t *tt,
                                      struct tm *tm_buf );
#endif /* MBEDTLS_HAVE_TIME_DATE */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PLATFORM_UTIL_H */
