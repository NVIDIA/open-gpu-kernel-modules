/**
 * \file md_internal.h
 *
 * \brief Message digest wrappers.
 *
 * \warning This in an internal header. Do not include directly.
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
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
#ifndef MBEDTLS_MD_WRAP_H
#define MBEDTLS_MD_WRAP_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "md.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Message digest information.
 * Allows message digest functions to be called in a generic way.
 */
struct mbedtls_md_info_t
{
    /** Digest identifier */
    mbedtls_md_type_t type;

    /** Name of the message digest */
    const char * name;

    /** Output length of the digest function in bytes */
    int size;

    /** Block length of the digest function in bytes */
    int block_size;

    /** Digest initialisation function */
    int (*starts_func)( void *ctx );

    /** Digest update function */
    int (*update_func)( void *ctx, const unsigned char *input, size_t ilen );

    /** Digest finalisation function */
    int (*finish_func)( void *ctx, unsigned char *output );

    /** Generic digest function */
    int (*digest_func)( const unsigned char *input, size_t ilen,
                        unsigned char *output );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

    /** Clone state from a context */
    void (*clone_func)( void *dst, const void *src );

    /** Internal use only */
    int (*process_func)( void *ctx, const unsigned char *input );
};

#if defined(MBEDTLS_MD2_C)
extern const mbedtls_md_info_t mbedtls_md2_info;
#endif
#if defined(MBEDTLS_MD4_C)
extern const mbedtls_md_info_t mbedtls_md4_info;
#endif
#if defined(MBEDTLS_MD5_C)
extern const mbedtls_md_info_t mbedtls_md5_info;
#endif
#if defined(MBEDTLS_RIPEMD160_C)
extern const mbedtls_md_info_t mbedtls_ripemd160_info;
#endif
#if defined(MBEDTLS_SHA1_C)
extern const mbedtls_md_info_t mbedtls_sha1_info;
#endif
#if defined(MBEDTLS_SHA256_C)
extern const mbedtls_md_info_t mbedtls_sha224_info;
extern const mbedtls_md_info_t mbedtls_sha256_info;
#endif
#if defined(MBEDTLS_SHA512_C)
extern const mbedtls_md_info_t mbedtls_sha384_info;
extern const mbedtls_md_info_t mbedtls_sha512_info;
#endif

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_MD_WRAP_H */
