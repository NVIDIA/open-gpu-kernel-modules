/**
 * \file nist_kw.h
 *
 * \brief This file provides an API for key wrapping (KW) and key wrapping with
 *        padding (KWP) as defined in NIST SP 800-38F.
 *        https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38F.pdf
 *
 *        Key wrapping specifies a deterministic authenticated-encryption mode
 *        of operation, according to <em>NIST SP 800-38F: Recommendation for
 *        Block Cipher Modes of Operation: Methods for Key Wrapping</em>. Its
 *        purpose is to protect cryptographic keys.
 *
 *        Its equivalent is RFC 3394 for KW, and RFC 5649 for KWP.
 *        https://tools.ietf.org/html/rfc3394
 *        https://tools.ietf.org/html/rfc5649
 *
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

#ifndef MBEDTLS_NIST_KW_H
#define MBEDTLS_NIST_KW_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MBEDTLS_KW_MODE_KW = 0,
    MBEDTLS_KW_MODE_KWP = 1
} mbedtls_nist_kw_mode_t;

#if !defined(MBEDTLS_NIST_KW_ALT)
// Regular implementation
//

/**
 * \brief    The key wrapping context-type definition. The key wrapping context is passed
 *           to the APIs called.
 *
 * \note     The definition of this type may change in future library versions.
 *           Don't make any assumptions on this context!
 */
typedef struct {
    mbedtls_cipher_context_t cipher_ctx;    /*!< The cipher context used. */
} mbedtls_nist_kw_context;

#else  /* MBEDTLS_NIST_key wrapping_ALT */
#include "nist_kw_alt.h"
#endif /* MBEDTLS_NIST_KW_ALT */

/**
 * \brief           This function initializes the specified key wrapping context
 *                  to make references valid and prepare the context
 *                  for mbedtls_nist_kw_setkey() or mbedtls_nist_kw_free().
 *
 * \param ctx       The key wrapping context to initialize.
 *
 */
void mbedtls_nist_kw_init( mbedtls_nist_kw_context *ctx );

/**
 * \brief           This function initializes the key wrapping context set in the
 *                  \p ctx parameter and sets the encryption key.
 *
 * \param ctx       The key wrapping context.
 * \param cipher    The 128-bit block cipher to use. Only AES is supported.
 * \param key       The Key Encryption Key (KEK).
 * \param keybits   The KEK size in bits. This must be acceptable by the cipher.
 * \param is_wrap   Specify whether the operation within the context is wrapping or unwrapping
 *
 * \return          \c 0 on success.
 * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for any invalid input.
 * \return          \c MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE for 128-bit block ciphers
 *                  which are not supported.
 * \return          cipher-specific error code on failure of the underlying cipher.
 */
int mbedtls_nist_kw_setkey( mbedtls_nist_kw_context *ctx,
                            mbedtls_cipher_id_t cipher,
                            const unsigned char *key,
                            unsigned int keybits,
                            const int is_wrap );

/**
 * \brief   This function releases and clears the specified key wrapping context
 *          and underlying cipher sub-context.
 *
 * \param ctx       The key wrapping context to clear.
 */
void mbedtls_nist_kw_free( mbedtls_nist_kw_context *ctx );

/**
 * \brief           This function encrypts a buffer using key wrapping.
 *
 * \param ctx       The key wrapping context to use for encryption.
 * \param mode      The key wrapping mode to use (MBEDTLS_KW_MODE_KW or MBEDTLS_KW_MODE_KWP)
 * \param input     The buffer holding the input data.
 * \param in_len    The length of the input data in Bytes.
 *                  The input uses units of 8 Bytes called semiblocks.
 *                  <ul><li>For KW mode: a multiple of 8 bytes between 16 and 2^57-8 inclusive. </li>
 *                  <li>For KWP mode: any length between 1 and 2^32-1 inclusive.</li></ul>
 * \param[out] output    The buffer holding the output data.
 *                  <ul><li>For KW mode: Must be at least 8 bytes larger than \p in_len.</li>
 *                  <li>For KWP mode: Must be at least 8 bytes larger rounded up to a multiple of
 *                  8 bytes for KWP (15 bytes at most).</li></ul>
 * \param[out] out_len The number of bytes written to the output buffer. \c 0 on failure.
 * \param[in] out_size The capacity of the output buffer.
 *
 * \return          \c 0 on success.
 * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for invalid input length.
 * \return          cipher-specific error code on failure of the underlying cipher.
 */
int mbedtls_nist_kw_wrap( mbedtls_nist_kw_context *ctx, mbedtls_nist_kw_mode_t mode,
                          const unsigned char *input, size_t in_len,
                          unsigned char *output, size_t* out_len, size_t out_size );

/**
 * \brief           This function decrypts a buffer using key wrapping.
 *
 * \param ctx       The key wrapping context to use for decryption.
 * \param mode      The key wrapping mode to use (MBEDTLS_KW_MODE_KW or MBEDTLS_KW_MODE_KWP)
 * \param input     The buffer holding the input data.
 * \param in_len    The length of the input data in Bytes.
 *                  The input uses units of 8 Bytes called semiblocks.
 *                  The input must be a multiple of semiblocks.
 *                  <ul><li>For KW mode: a multiple of 8 bytes between 24 and 2^57 inclusive. </li>
 *                  <li>For KWP mode: a multiple of 8 bytes between 16 and 2^32 inclusive.</li></ul>
 * \param[out] output    The buffer holding the output data.
 *                  The output buffer's minimal length is 8 bytes shorter than \p in_len.
 * \param[out] out_len The number of bytes written to the output buffer. \c 0 on failure.
 *                  For KWP mode, the length could be up to 15 bytes shorter than \p in_len,
 *                  depending on how much padding was added to the data.
 * \param[in] out_size The capacity of the output buffer.
 *
 * \return          \c 0 on success.
 * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for invalid input length.
 * \return          \c MBEDTLS_ERR_CIPHER_AUTH_FAILED for verification failure of the ciphertext.
 * \return          cipher-specific error code on failure of the underlying cipher.
 */
int mbedtls_nist_kw_unwrap( mbedtls_nist_kw_context *ctx, mbedtls_nist_kw_mode_t mode,
                            const unsigned char *input, size_t in_len,
                            unsigned char *output, size_t* out_len, size_t out_size);


#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
/**
 * \brief          The key wrapping checkup routine.
 *
 * \return         \c 0 on success.
 * \return         \c 1 on failure.
 */
int mbedtls_nist_kw_self_test( int verbose );
#endif /* MBEDTLS_SELF_TEST && MBEDTLS_AES_C */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_NIST_KW_H */
