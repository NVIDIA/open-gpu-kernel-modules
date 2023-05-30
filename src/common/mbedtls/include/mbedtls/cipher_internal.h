/**
 * \file cipher_internal.h
 *
 * \brief Cipher wrappers.
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
#ifndef MBEDTLS_CIPHER_WRAP_H
#define MBEDTLS_CIPHER_WRAP_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Base cipher information. The non-mode specific functions and values.
 */
struct mbedtls_cipher_base_t
{
    /** Base Cipher type (e.g. MBEDTLS_CIPHER_ID_AES) */
    mbedtls_cipher_id_t cipher;

    /** Encrypt using ECB */
    int (*ecb_func)( void *ctx, mbedtls_operation_t mode,
                     const unsigned char *input, unsigned char *output );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    /** Encrypt using CBC */
    int (*cbc_func)( void *ctx, mbedtls_operation_t mode, size_t length,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );
#endif

#if defined(MBEDTLS_CIPHER_MODE_CFB)
    /** Encrypt using CFB (Full length) */
    int (*cfb_func)( void *ctx, mbedtls_operation_t mode, size_t length, size_t *iv_off,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );
#endif

#if defined(MBEDTLS_CIPHER_MODE_OFB)
    /** Encrypt using OFB (Full length) */
    int (*ofb_func)( void *ctx, size_t length, size_t *iv_off,
                     unsigned char *iv,
                     const unsigned char *input,
                     unsigned char *output );
#endif

#if defined(MBEDTLS_CIPHER_MODE_CTR)
    /** Encrypt using CTR */
    int (*ctr_func)( void *ctx, size_t length, size_t *nc_off,
                     unsigned char *nonce_counter, unsigned char *stream_block,
                     const unsigned char *input, unsigned char *output );
#endif

#if defined(MBEDTLS_CIPHER_MODE_XTS)
    /** Encrypt or decrypt using XTS. */
    int (*xts_func)( void *ctx, mbedtls_operation_t mode, size_t length,
                     const unsigned char data_unit[16],
                     const unsigned char *input, unsigned char *output );
#endif

#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    /** Encrypt using STREAM */
    int (*stream_func)( void *ctx, size_t length,
                        const unsigned char *input, unsigned char *output );
#endif

    /** Set key for encryption purposes */
    int (*setkey_enc_func)( void *ctx, const unsigned char *key,
                            unsigned int key_bitlen );

    /** Set key for decryption purposes */
    int (*setkey_dec_func)( void *ctx, const unsigned char *key,
                            unsigned int key_bitlen);

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

};

typedef struct
{
    mbedtls_cipher_type_t type;
    const mbedtls_cipher_info_t *info;
} mbedtls_cipher_definition_t;

extern const mbedtls_cipher_definition_t mbedtls_cipher_definitions[];

extern int mbedtls_cipher_supported[];

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CIPHER_WRAP_H */
