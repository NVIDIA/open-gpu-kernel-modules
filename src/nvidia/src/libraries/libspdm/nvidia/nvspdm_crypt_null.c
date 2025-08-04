/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


//
// Included libspdm copyright alongside NVIDIA copyright, as these functions
// are copied from libspdm cryptlib_null files. Meant to more easily stub out
// cryptography functions when not needed.
//

/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/


// Use headers directly so we don't rely on internal_crypt_lib.h
#include <base.h>
#include "library/debuglib.h"
#include "library/memlib.h"
#include "library/cryptlib.h"
#include <rmconfig.h>
#include "core/prelude.h"
// Stub out all crypto functions that we do not support.

bool libspdm_ec_get_private_key_from_pem(const uint8_t *pem_data, size_t pem_size,
                                         const char *password,
                                         void **ec_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ec_get_public_key_from_der(const uint8_t *der_data,
                                        size_t der_size,
                                        void **ec_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_rsa_get_public_key_from_der(const uint8_t *der_data,
                                         size_t der_size,
                                         void **rsa_context)
{
    return true;
}

#if defined(NV_USE_MBEDTLS) && NV_USE_MBEDTLS
bool libspdm_check_crypto_backend(void)
{
    NV_PRINTF(LEVEL_INFO, "libspdm_check_crypto_backend: MbedTLS found\n");
    return true;
}
#endif

#define SPDM_NULL_PRESENT 0 /* Use LKCA */

#if !defined(NV_USE_MBEDTLS)
#define NV_USE_MBEDTLS 0
#endif

#if !NV_USE_MBEDTLS && SPDM_NULL_PRESENT && !defined(NV_FORCE_SKIP_LIBSPDM_CRYPTLIB_NULL)

bool libspdm_check_crypto_backend(void)
{
    NV_PRINTF(LEVEL_ERROR, "libspdm_check_crypto_backend: Error - libspdm expects MbedTLS but found stubs!\n");
    return false;
}

bool libspdm_aead_aes_gcm_encrypt(const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  uint8_t *tag_out, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    libspdm_copy_mem(data_out, *data_out_size, data_in, data_in_size);
    *data_out_size = data_in_size;
    libspdm_zero_mem(tag_out, tag_size);
    return true;
}

bool libspdm_aead_aes_gcm_decrypt(const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  const uint8_t *tag, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    libspdm_copy_mem(data_out, *data_out_size, data_in, data_in_size);
    *data_out_size = data_in_size;
    return true;
}

void *libspdm_sha256_new(void)
{
    LIBSPDM_ASSERT(false);
    return NULL;
}

void libspdm_sha256_free(void *sha256_ctx)
{
    LIBSPDM_ASSERT(false);
}

bool libspdm_sha256_init(void *sha256_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha256_duplicate(const void *sha256_context,
                              void *new_sha256_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha256_update(void *sha256_context, const void *data,
                           size_t data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha256_final(void *sha256_context, uint8_t *hash_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha256_hash_all(const void *data, size_t data_size,
                             uint8_t *hash_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

void *libspdm_sha384_new(void)
{
    LIBSPDM_ASSERT(false);
    return NULL;
}

void libspdm_sha384_free(void *sha384_ctx)
{
    LIBSPDM_ASSERT(false);
}

bool libspdm_sha384_init(void *sha384_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha384_duplicate(const void *sha384_context,
                              void *new_sha384_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha384_update(void *sha384_context, const void *data,
                           size_t data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha384_final(void *sha384_context, uint8_t *hash_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sha384_hash_all(const void *data, size_t data_size,
                             uint8_t *hash_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

void *libspdm_hmac_sha256_new(void)
{
    LIBSPDM_ASSERT(false);
    return NULL;
}

void libspdm_hmac_sha256_free(void *hmac_sha256_ctx)
{
    LIBSPDM_ASSERT(false);
}

bool libspdm_hmac_sha256_set_key(void *hmac_sha256_ctx, const uint8_t *key,
                                 size_t key_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha256_duplicate(const void *hmac_sha256_ctx,
                                   void *new_hmac_sha256_ctx)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha256_update(void *hmac_sha256_ctx, const void *data,
                                size_t data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha256_final(void *hmac_sha256_ctx, uint8_t *hmac_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha256_all(const void *data, size_t data_size,
                             const uint8_t *key, size_t key_size,
                             uint8_t *hmac_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

void *libspdm_hmac_sha384_new(void)
{
    LIBSPDM_ASSERT(false);
    return NULL;
}

void libspdm_hmac_sha384_free(void *hmac_sha384_ctx)
{
    LIBSPDM_ASSERT(false);
}

bool libspdm_hmac_sha384_set_key(void *hmac_sha384_ctx, const uint8_t *key,
                                 size_t key_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha384_duplicate(const void *hmac_sha384_ctx,
                                   void *new_hmac_sha384_ctx)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha384_update(void *hmac_sha384_ctx, const void *data,
                                size_t data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha384_final(void *hmac_sha384_ctx, uint8_t *hmac_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hmac_sha384_all(const void *data, size_t data_size,
                             const uint8_t *key, size_t key_size,
                             uint8_t *hmac_value)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha256_extract_and_expand(const uint8_t *key, size_t key_size,
                                            const uint8_t *salt, size_t salt_size,
                                            const uint8_t *info, size_t info_size,
                                            uint8_t *out, size_t out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha256_extract(const uint8_t *key, size_t key_size,
                                 const uint8_t *salt, size_t salt_size,
                                 uint8_t *prk_out, size_t prk_out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha256_expand(const uint8_t *prk, size_t prk_size,
                                const uint8_t *info, size_t info_size,
                                uint8_t *out, size_t out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha384_extract_and_expand(const uint8_t *key, size_t key_size,
                                            const uint8_t *salt, size_t salt_size,
                                            const uint8_t *info, size_t info_size,
                                            uint8_t *out, size_t out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha384_extract(const uint8_t *key, size_t key_size,
                                 const uint8_t *salt, size_t salt_size,
                                 uint8_t *prk_out, size_t prk_out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_hkdf_sha384_expand(const uint8_t *prk, size_t prk_size,
                                const uint8_t *info, size_t info_size,
                                uint8_t *out, size_t out_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

void *libspdm_ec_new_by_nid(size_t nid)
{
    LIBSPDM_ASSERT(false);
    return NULL;
}

void libspdm_ec_free(void *ec_context)
{
    LIBSPDM_ASSERT(false);
}

bool libspdm_ec_generate_key(void *ec_context, uint8_t *public,
                             size_t *public_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ec_compute_key(void *ec_context, const uint8_t *peer_public,
                            size_t peer_public_size, uint8_t *key,
                            size_t *key_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ecdsa_sign(void *ec_context, size_t hash_nid,
                        const uint8_t *message_hash, size_t hash_size,
                        uint8_t *signature, size_t *sig_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ecdsa_verify(void *ec_context, size_t hash_nid,
                          const uint8_t *message_hash, size_t hash_size,
                          const uint8_t *signature, size_t sig_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_validity(const uint8_t *cert, size_t cert_size,
                               uint8_t *from, size_t *from_size, uint8_t *to,
                               size_t *to_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

int32_t libspdm_x509_compare_date_time(const void *date_time1, const void *date_time2)
{
    LIBSPDM_ASSERT(false);
    return -3;
}

bool libspdm_x509_get_issuer_name(const uint8_t *cert, size_t cert_size,
                                  uint8_t *cert_issuer,
                                  size_t *issuer_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extended_basic_constraints(const uint8_t *cert,
                                                 size_t cert_size,
                                                 uint8_t *basic_constraints,
                                                 size_t *basic_constraints_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extended_key_usage(const uint8_t *cert,
                                         size_t cert_size, uint8_t *usage,
                                         size_t *usage_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extension_data(const uint8_t *cert, size_t cert_size,
                                     const uint8_t *oid, size_t oid_size,
                                     uint8_t *extension_data,
                                     size_t *extension_data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_version(const uint8_t *cert, size_t cert_size,
                              size_t *version)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_asn1_get_tag(uint8_t **ptr, const uint8_t *end, size_t *length,
                          uint32_t tag)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_set_date_time(const char *date_time_str, void *date_time,
                                size_t *date_time_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_serial_number(const uint8_t *cert, size_t cert_size,
                                    uint8_t *serial_number,
                                    size_t *serial_number_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_key_usage(const uint8_t *cert, size_t cert_size,
                                size_t *usage)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_subject_name(const uint8_t *cert, size_t cert_size,
                                   uint8_t *cert_subject,
                                   size_t *subject_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ec_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                         void **ec_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_verify_cert(const uint8_t *cert, size_t cert_size,
                              const uint8_t *ca_cert, size_t ca_cert_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_verify_cert_chain(const uint8_t *root_cert, size_t root_cert_length,
                                    const uint8_t *cert_chain, size_t cert_chain_length)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_cert_from_cert_chain(const uint8_t *cert_chain,
                                           size_t cert_chain_length,
                                           const int32_t cert_index, const uint8_t **cert,
                                           size_t *cert_length)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_random_seed(const uint8_t *seed, size_t seed_size)
{
    /* TBD*/
    return true;
}

bool libspdm_random_bytes(uint8_t *output, size_t size)
{
    return true;
}

void *libspdm_rsa_new
(
    void
)
{
    return NULL;
}

bool libspdm_rsa_get_key(void *rsa_context, const libspdm_rsa_key_tag_t key_tag,
                         uint8_t *big_number, size_t *bn_size)
{
    return true;
}

bool libspdm_rsa_generate_key(void *rsa_context, size_t modulus_length,
                              const uint8_t *public_exponent,
                              size_t public_exponent_size)
{
    return true;
}

bool libspdm_rsa_check_key(void *rsa_context)
{
    return true;
}

bool libspdm_rsa_set_key(void *rsa_context, const libspdm_rsa_key_tag_t key_tag,
                         const uint8_t *big_number, size_t bn_size)
{
     return true;
}

bool libspdm_rsa_pss_sign(void *rsa_context, size_t hash_nid,
                          const uint8_t *message_hash, size_t hash_size,
                          uint8_t *signature, size_t *sig_size)
{
    return false;
}


bool libspdm_encode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return false;
}

bool libspdm_decode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return false;
}

void libspdm_rsa_free(void *rsa_context)
{
    return;
}

bool libspdm_x509_get_signature_algorithm(const uint8_t *cert,
                                          size_t cert_size, uint8_t *oid,
                                          size_t *oid_size)
{
    return false;
}

bool libspdm_rsa_pss_verify(void *rsa_context, size_t hash_nid,
                            const uint8_t *message_hash, size_t hash_size,
                            const uint8_t *signature, size_t sig_size)
{
    return false;
}

bool libspdm_rsa_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                          void **rsa_context)
{
    return false;
}

#endif // !NV_USE_MBEDTLS && SPDM_NULL_PRESENT && !defined(NV_FORCE_SKIP_LIBSPDM_CRYPTLIB_NULL)
