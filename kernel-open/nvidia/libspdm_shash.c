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
#ifndef NV_CRYPTO_TFM_CTX_ALIGNED_PRESENT
#include <crypto/internal/hash.h>
#endif
#endif

void *lkca_hash_new(const char* alg_name)
{
#ifndef USE_LKCA
    return NULL;
#else
    //XXX: can we reuse crypto_shash part and just allocate desc
    struct crypto_shash *alg;
    struct shash_desc *desc;

    alg = crypto_alloc_shash(alg_name, 0, 0);
    if (IS_ERR(alg)) {
        printk (KERN_INFO "Failed to alloc %s\n", alg_name);
        return NULL;
    }

    desc = kmalloc(sizeof(*desc) + crypto_shash_descsize(alg), GFP_KERNEL);
    if (desc == NULL){
        printk (KERN_INFO "Kernel out of mem\n");
        crypto_free_shash(alg);
        return NULL;
    }

    desc->tfm = alg;

    return desc;
#endif
}

void lkca_hash_free(struct shash_desc *ctx)
{
#ifndef USE_LKCA
#else
    crypto_free_shash(ctx->tfm);
    kfree(ctx);
#endif
}

bool lkca_hash_duplicate(struct shash_desc *dst, struct shash_desc const *src)
{
#ifndef USE_LKCA
        return false;
#else
        SHASH_DESC_ON_STACK(tmp, src);

        if (crypto_shash_export((struct shash_desc *) src, tmp)) {
            return false;
        }
        if (crypto_shash_import(dst, tmp)) {
            return false;
        }

        return true;
#endif
}

bool lkca_hmac_duplicate(struct shash_desc *dst, struct shash_desc const *src)
{
#ifndef USE_LKCA
        return false;
#else
        // in LKCA hmac export doesn't export ipad/opad, so we need to WAR it

        struct crypto_shash *src_tfm = src->tfm;
        struct crypto_shash *dst_tfm = dst->tfm;
        int ss = crypto_shash_statesize(dst_tfm);

#ifdef NV_CRYPTO_TFM_CTX_ALIGNED_PRESENT
        char *src_ipad = crypto_tfm_ctx_aligned(&src_tfm->base);
        char *dst_ipad = crypto_tfm_ctx_aligned(&dst_tfm->base);
#else
        int ctx_size = crypto_shash_alg(dst_tfm)->base.cra_ctxsize;
        char *src_ipad = crypto_shash_ctx(src_tfm);
        char *dst_ipad = crypto_shash_ctx(dst_tfm);
        /*
         * Actual struct definition is hidden, so I assume data we need is at
         * the end. In 6.0 the struct has a pointer to crpyto_shash followed by: 
         * 'u8 ipad[statesize];', then 'u8 opad[statesize];'
         */
        src_ipad += ctx_size - 2 * ss;
        dst_ipad += ctx_size - 2 * ss;
#endif

        memcpy(dst_ipad, src_ipad, crypto_shash_blocksize(src->tfm));
        memcpy(dst_ipad + ss, src_ipad + ss, crypto_shash_blocksize(src->tfm));
        crypto_shash_clear_flags(dst->tfm, CRYPTO_TFM_NEED_KEY);

        return lkca_hash_duplicate(dst, src);
#endif
}

bool lkca_hash_all(const char* alg_name, const void *data,
                   size_t data_size, uint8_t *hash_value)
{
#ifndef USE_LKCA
    return false;
#else
    int ret;
    struct crypto_shash *alg;
    alg = crypto_alloc_shash(alg_name, 0, 0);
    if (IS_ERR(alg)) {
        return false;
    }

    ret = crypto_shash_tfm_digest(alg, data, data_size, hash_value);

    crypto_free_shash(alg);

    return (ret == 0);
#endif
}

bool lkca_hmac_set_key(struct shash_desc *desc, const uint8_t *key, size_t key_size)
{
#ifndef USE_LKCA
    return false;
#else
    int ret;
    ret = crypto_shash_setkey(desc->tfm, key, key_size);
    if (ret == 0) {
        ret = crypto_shash_init(desc);
    }
    return ret == 0;
#endif
}

bool lkca_hmac_all(const char* alg_name, const uint8_t *key, size_t key_size,
                   const uint8_t *data, size_t data_size, uint8_t *hash_value)
{
#ifndef USE_LKCA
    return false;
#else
    int ret;
    struct crypto_shash *alg;
    alg = crypto_alloc_shash(alg_name, 0, 0);
    if (IS_ERR(alg)) {
        return false;
    }

    ret = crypto_shash_setkey(alg, key, key_size);

    if (ret == 0){
        ret = crypto_shash_tfm_digest(alg, data, data_size, hash_value);
    }

    crypto_free_shash(alg);

    return (ret == 0);
#endif
}

