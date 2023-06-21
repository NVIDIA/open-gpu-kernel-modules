/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef BASE_H
#define BASE_H

#ifndef LIBSPDM_STDINT_ALT

#include <stdint.h>

/* LIBSPDM_OPENSSL_STDINT_WORKAROUND */

/* This is a workaround for OpenSSL compilation problems when used with <stdint.h>
 * on Windows platforms built with Visual Studio. Including <stdint.h> pulls in
 * <vcruntime.h>, which causes the type size_t to be defined. The size_t type
 * depends on if _WIN32 or _WIN64 is defined. The default if neither is defined
 * is the 32-bit version of size_t. */

/* Our OpenSSL compilation requires _WIN32 and _WIN64 to NOT be defined.
 * This will force the <vcruntime.h> to use the wrong 32-bit definition of size_t
 * if we are compiling as 64-bit. This 32-bit definition then does not agree with
 * the 64-bit definition defined in libspdm and generates compile errors. */

/* To workaround this issue, LIBSPDM_OPENSSL_STDINT_WORKAROUND was created
 * that is only defined for compilation via tha makefile of the OpenSSL library
 * portion of libspdm. */

/* This will lead to _WIN32 and _WIN64 to be NOT defined when reaching the OpenSSL
 * portions of a compilation unit (header files + c file), thus meeting the
 * no Win32/Win64 requirement for OpenSSL, but will still be defined when compiling
 * the <vcruntime.h> file in the compilation unit (and getting the right size_t). */

/* In the future libspdm intends to use the Windows native compilation flags and defines,
 * in place of the UEFI profile / personality. */

#ifdef LIBSPDM_OPENSSL_STDINT_WORKAROUND
#undef _WIN32
#undef _WIN64
#endif

#else /* LIBSPDM_STDINT_ALT */
#include LIBSPDM_STDINT_ALT
#endif /* LIBSPDM_STDINT_ALT */

#ifndef LIBSPDM_STDBOOL_ALT
#include <stdbool.h>
#else
#include LIBSPDM_STDBOOL_ALT
#endif

#ifndef LIBSPDM_STDDEF_ALT
#include <stddef.h>
#else
#include LIBSPDM_STDDEF_ALT
#endif

/**
 * Return the minimum of two operands.
 *
 * This macro returns the minimal of two operand specified by a and b.
 * Both a and b must be the same numerical types, signed or unsigned.
 *
 * @param   a        The first operand with any numerical type.
 * @param   b        The second operand. It should be the same any numerical type with a.
 *
 * @return  Minimum of two operands.
 *
 **/
#define LIBSPDM_MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * Return the number of elements in an array.
 *
 * @param  array  An object of array type. Array is only used as an argument to
 *               the sizeof operator, therefore Array is never evaluated. The
 *               caller is responsible for ensuring that Array's type is not
 *               incomplete; that is, Array must have known constant size.
 *
 * @return The number of elements in Array. The result has type size_t.
 *
 **/
#define LIBSPDM_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#endif /* BASE_H */
