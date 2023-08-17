/*
* SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "internal_crypt_lib.h"

#ifdef USE_LKCA
#include <linux/module.h>
MODULE_SOFTDEP("pre: ecdh_generic,ecdsa_generic");

#include <crypto/akcipher.h>
#include <crypto/ecdh.h>
#include <crypto/internal/ecc.h>

struct ecc_ctx {
    unsigned int curve_id;
    u64 priv_key[ECC_MAX_DIGITS]; // In big endian

    struct {
        // ecdsa wants byte preceding pub_key to be set to '4'
        u64 pub_key_prefix;
        u64 pub_key[2 * ECC_MAX_DIGITS];
    };

    bool pub_key_set;
    bool priv_key_set;
    char const *name;
    int size;
};
#endif

void *libspdm_ec_new_by_nid(size_t nid)
{
#ifndef USE_LKCA
    return NULL;
#else
    struct ecc_ctx *ctx;

    if ((nid != LIBSPDM_CRYPTO_NID_SECP256R1) && (nid != LIBSPDM_CRYPTO_NID_SECP384R1)){
        return NULL;
    }

    ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
    if (!ctx) {
        return NULL;
    }

    if (nid == LIBSPDM_CRYPTO_NID_SECP256R1) {
        ctx->curve_id = ECC_CURVE_NIST_P256;
        ctx->size = 64;
        ctx->name = "ecdsa-nist-p256";
    } else {
        ctx->curve_id = ECC_CURVE_NIST_P384;
        ctx->size = 96;
        ctx->name = "ecdsa-nist-p384";
    }
    ctx->pub_key_set = false;
    ctx->priv_key_set = false;

    return ctx;
#endif
}

void libspdm_ec_free(void *ec_context)
{
#ifdef USE_LKCA
    kfree(ec_context);
#endif
}

bool lkca_ecdsa_set_priv_key(void *context, uint8_t *key, size_t key_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = context;
    unsigned int ndigits = ctx->size / 16;

    if (key_size != (ctx->size / 2)) {
        return false;
    }

    memcpy(ctx->priv_key, key, key_size);

    // XXX: if this fails, do we want to retry generating new key?
    if(ecc_make_pub_key(ctx->curve_id, ndigits, ctx->priv_key, ctx->pub_key)) {
        return false;
    }

    ctx->pub_key_set = true;
    ctx->priv_key_set = true;
    return true;
#endif
}

bool lkca_ec_set_pub_key(void *ec_context, const uint8_t *public_key,
                         size_t public_key_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = ec_context;
    struct ecc_point pub_key;
    unsigned int ndigits;

    if (public_key_size != ctx->size) {
        return false;
    }

    // We can reuse pub_key for now
    ndigits = ctx->size / 16;
    pub_key = ECC_POINT_INIT(ctx->pub_key, ctx->pub_key + ndigits, ndigits);

    ecc_swap_digits(public_key, ctx->pub_key, ndigits);
    ecc_swap_digits(((u64 *)public_key) + ndigits, ctx->pub_key + ndigits, ndigits);
    if(ecc_is_pubkey_valid_full(ecc_get_curve(ctx->curve_id), &pub_key)) {
        return false;
    }

    memcpy(ctx->pub_key, public_key, public_key_size);
    ctx->pub_key_set = true;
    return true;
#endif
}

bool lkca_ec_get_pub_key(void *ec_context, uint8_t *public_key,
                         size_t *public_key_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = ec_context;

    if (*public_key_size < ctx->size) {
        *public_key_size = ctx->size;
        return false;
    }
    *public_key_size = ctx->size;

    memcpy(public_key, ctx->pub_key, ctx->size);
    return true;
#endif
}

bool lkca_ec_generate_key(void *ec_context, uint8_t *public_data,
                          size_t *public_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = ec_context;

    unsigned int ndigits = ctx->size / 16;

    if(ecc_gen_privkey(ctx->curve_id, ndigits, ctx->priv_key)) {
        return false;
    }
    // XXX: if this fails, do we want to retry generating new key?
    if(ecc_make_pub_key(ctx->curve_id, ndigits, ctx->priv_key, ctx->pub_key)) {
        return false;
    }

    memcpy(public_data, ctx->pub_key, ctx->size);
    *public_size = ctx->size;
    ctx->priv_key_set = true;
    ctx->pub_key_set = true;

    return true;
#endif
}

bool lkca_ec_compute_key(void *ec_context, const uint8_t *peer_public,
                         size_t peer_public_size, uint8_t *key,
                         size_t *key_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = ec_context;

    if (peer_public_size != ctx->size) {
        return false;
    }

    if (!ctx->priv_key_set) {
        return false;
    }

    if ((ctx->size / 2) > *key_size) {
        return false;
    }

    if (crypto_ecdh_shared_secret(ctx->curve_id, ctx->size / 16,
                                  (const u64 *) ctx->priv_key,
                                  (const u64 *) peer_public,
                                  (u64 *) key)) {
        return false;
    }

    *key_size = ctx->size / 2;
    return true;
#endif
}

bool lkca_ecdsa_verify(void *ec_context, size_t hash_nid,
                       const uint8_t *message_hash, size_t hash_size,
                       const uint8_t *signature, size_t sig_size)
{
#ifndef USE_LKCA
    return false;
#else
    struct ecc_ctx *ctx = ec_context;

    // Roundabout way
    u64 ber_max_len = 3 + 2 * (4 + (ECC_MAX_BYTES));
    u64 ber_len = 0;
    u8 *ber = NULL;
    u8 *pub_key;
    struct akcipher_request *req = NULL;
    struct crypto_akcipher *tfm = NULL;
    struct scatterlist sg;
    DECLARE_CRYPTO_WAIT(wait);
    int err;

    if (sig_size != ctx->size) {
        return false;
    }

    if(ctx->pub_key_set == false){
        return false;
    }

    tfm = crypto_alloc_akcipher(ctx->name, CRYPTO_ALG_TYPE_AKCIPHER, 0);
    if (IS_ERR(tfm)) {
        pr_info("ALLOC FAILED\n");
        return false;
    }

    pub_key = (u8 *) ctx->pub_key;
    pub_key--; // Go back into byte of pub_key_prefix
    *pub_key = 4; // And set it to 4 to placate kernel
    if ((err = crypto_akcipher_set_pub_key(tfm, pub_key, ctx->size + 1)) != 0) {
        pr_info("SET PUB KEY FAILED: %d\n", -err);
        goto failTfm;
    }

    req = akcipher_request_alloc(tfm, GFP_KERNEL);
    if (IS_ERR(req)) {
        pr_info("REQUEST ALLOC FAILED\n");
        goto failTfm;
    }

    // We concatenate signature and hash and ship it to kernel
    ber = kmalloc(ber_max_len + hash_size, GFP_KERNEL);
    if (ber == NULL) {
        goto failReq;
    }

    // XXX: NOTE THIS WILL WORK ONLY FOR 256 AND 384 bits. For larger keys
    // length field will be longer than 1 byte and I haven't taken care of that!

    // Signature
    ber[ber_len++] = 0x30;
    ber[ber_len++] = 2 * (2 + ctx->size / 2);
    ber[ber_len++] = 0x02;
    if (signature[0] > 127) {
        ber[ber_len++] = ctx->size / 2 + 1;
        ber[1]++;
        ber[ber_len++] = 0;
    } else {
        ber[ber_len++] = ctx->size / 2;
    }
    memcpy(ber + ber_len, signature, sig_size / 2);
    ber_len += sig_size / 2;
    ber[ber_len++] = 0x02;
    if (signature[sig_size / 2] > 127) {
        ber[ber_len++] = ctx->size / 2 + 1;
        ber[1]++;
        ber[ber_len++] = 0;
    } else {
        ber[ber_len++] = ctx->size / 2;
    }
    memcpy(ber + ber_len, signature + sig_size / 2, sig_size / 2);
    ber_len += sig_size / 2;

    // Just append hash, for scatterlists it can't be on stack anyway
    memcpy(ber + ber_len, message_hash, hash_size);

    sg_init_one(&sg, ber, ber_len + hash_size);
    akcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
                                  CRYPTO_TFM_REQ_MAY_SLEEP, crypto_req_done, &wait);
    akcipher_request_set_crypt(req, &sg, NULL, ber_len, hash_size);
    err = crypto_wait_req(crypto_akcipher_verify(req), &wait);

    if (err != 0){
        pr_info("Verify FAILED %d\n", -err);
    }

    kfree(ber);
failReq:
    akcipher_request_free(req);
failTfm:
    crypto_free_akcipher(tfm);

    return err == 0;
#endif
}
