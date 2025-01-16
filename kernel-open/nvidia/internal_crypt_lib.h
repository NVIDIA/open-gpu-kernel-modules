/*
* SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __INTERNAL_CRYPT_LIB_H__
#define __INTERNAL_CRYPT_LIB_H__

/*
 * This code uses Linux Kernel Crypto API extensively. Web page written by
 * Stephan Mueller and Marek Vasut is a good starting reference on how linux
 * kernel provides crypto api.
 */
#include "conftest.h"

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/limits.h>
#include <linux/random.h>
#include <linux/string.h>

// Check if ECDH/ECDSA are there, on some platforms they might not be...
#ifndef AUTOCONF_INCLUDED
#if defined(NV_GENERATED_AUTOCONF_H_PRESENT)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#endif
#if \
    (defined(CONFIG_CRYPTO_AEAD) || defined(CONFIG_CRYPTO_AEAD_MODULE)) && \
    (defined(CONFIG_CRYPTO_AKCIPHER) || defined(CONFIG_CRYPTO_AKCIPHER_MODULE)) && \
    (defined(CONFIG_CRYPTO_SKCIPHER) || defined(CONFIG_CRYPTO_SKCIPHER_MODULE)) && \
    (defined(CONFIG_CRYPTO_HASH) || defined(CONFIG_CRYPTO_HASH_MODULE)) && \
    (defined(CONFIG_CRYPTO_HMAC) || defined(CONFIG_CRYPTO_HMAC_MODULE)) && \
    (defined(CONFIG_CRYPTO_ECDH) || defined(CONFIG_CRYPTO_ECDH_MODULE)) && \
    (defined(CONFIG_CRYPTO_ECDSA) || defined(CONFIG_CRYPTO_ECDSA_MODULE)) && \
    (defined(CONFIG_CRYPTO_RSA) || defined(CONFIG_CRYPTO_RSA_MODULE)) && \
    (defined(CONFIG_X509_CERTIFICATE_PARSER) || defined(CONFIG_X509_CERTIFICATE_PARSER_MODULE))
#define NV_CONFIG_CRYPTO_PRESENT 1
#endif

/*
 * It is possible that we don't have access to all the functions we have. This
 * could be either because we are running non-gpl kernel, because kernel is too
 * old or even just user disabled. If we should use LKCA, include headers, else
 * define stubs to return errors.
 */
#if defined(NV_CRYPTO_PRESENT) && defined (NV_CONFIG_CRYPTO_PRESENT) && \
    (defined(NV_CRYPTO_AKCIPHER_VERIFY_PRESENT) || \
     (defined(NV_CRYPTO_SIG_H_PRESENT) && defined(NV_ECC_DIGITS_FROM_BYTES_PRESENT)))
#define USE_LKCA 1
#endif

#ifdef USE_LKCA
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <crypto/aead.h>
#include <crypto/algapi.h>
#include <crypto/hash.h>
#include <crypto/sm3.h>

// HASH_MAX_DIGESTSIZE is available since 4.20.
// This value is accurate as of 6.1
#ifndef HASH_MAX_DIGESTSIZE
#define HASH_MAX_DIGESTSIZE 64
#endif

#else
// Just stub everything out
struct shash_desc;
struct crypto_shash;
#define crypto_shash_setkey(...) -ENOMEM
#define crypto_shash_init(...) -ENOMEM
#define crypto_shash_update(...) -ENOMEM
#define crypto_shash_update(...) -ENOMEM
#define crypto_shash_final(...) -ENOMEM
#endif

#define CHAR_BIT 8U
#undef SIZE_MAX
#define SIZE_MAX 8

#include "library/cryptlib.h"

#define LIBSPDM_ASSERT(...)
struct lkca_aead_ctx;
int lkca_aead_alloc(struct lkca_aead_ctx **ctx, char const *alg);
void lkca_aead_free(struct lkca_aead_ctx *ctx);
int lkca_aead_ex(struct lkca_aead_ctx *ctx,
                 const uint8_t *key, size_t key_size,
                 uint8_t *iv, size_t iv_size,
                 const uint8_t *data_in, size_t data_in_size,
                 uint8_t *tag, size_t tag_size,
                 uint8_t *data_out, size_t *data_out_size,
                 bool enc);

int libspdm_aead(const uint8_t *key, size_t key_size,
                 const uint8_t *iv, size_t iv_size,
                 const uint8_t *a_data, size_t a_data_size,
                 const uint8_t *data_in, size_t data_in_size,
                 const uint8_t *tag, size_t tag_size,
                 uint8_t *data_out, size_t *data_out_size,
                 bool enc, char const *alg);

void *lkca_hash_new(const char* alg_name);
void lkca_hash_free(struct shash_desc *ctx);
bool lkca_hash_duplicate(struct shash_desc *dst, struct shash_desc const *src);
bool lkca_hash_all(const char* alg_name, const void *data,
                   size_t data_size, uint8_t *hash_value);
bool lkca_hmac_duplicate(struct shash_desc *dst, struct shash_desc const *src);
bool lkca_hmac_set_key(struct shash_desc *ctx, const uint8_t *key, size_t key_size);
bool lkca_hmac_all(const char* alg_name, const uint8_t *key, size_t key_size,
                   const uint8_t *data, size_t data_size, uint8_t *hash_value);
bool lkca_hkdf_extract_and_expand(const char *alg_name,
                                  const uint8_t *key, size_t key_size,
                                  const uint8_t *salt, size_t salt_size,
                                  const uint8_t *info, size_t info_size,
                                  uint8_t *out, size_t out_size);
bool lkca_hkdf_expand(const char *alg_name,
                      const uint8_t *prk, size_t prk_size,
                      const uint8_t *info, size_t info_size,
                      uint8_t *out, size_t out_size);


bool lkca_ecdsa_set_priv_key(void *context, uint8_t *key, size_t key_size);
bool lkca_ec_set_pub_key(void *ec_context, const uint8_t *public_key,
                         size_t public_key_size);
bool lkca_ec_get_pub_key(void *ec_context, uint8_t *public_key,
                         size_t *public_key_size);
bool lkca_ec_generate_key(void *ec_context, uint8_t *public_data,
                          size_t *public_size);
bool lkca_ec_compute_key(void *ec_context, const uint8_t *peer_public,
                         size_t peer_public_size, uint8_t *key,
                         size_t *key_size);
bool lkca_ecdsa_verify(void *ec_context, size_t hash_nid,
                       const uint8_t *message_hash, size_t hash_size,
                       const uint8_t *signature, size_t sig_size);

bool lkca_rsa_verify(void *rsa_context, size_t hash_nid,
                     const uint8_t *message_hash, size_t hash_size,
                     const uint8_t *signature, size_t sig_size);

bool lkca_rsa_pkcs1_sign(void *rsa_context, size_t hash_nid,
                         const uint8_t *message_hash, size_t hash_size,
                         uint8_t *signature, size_t *sig_size);

bool lkca_rsa_pss_sign(void *rsa_context, size_t hash_nid,
                       const uint8_t *message_hash, size_t hash_size,
                       uint8_t *signature, size_t *sig_size);

#endif
