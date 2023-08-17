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

// RFC 5869 has some very non-intuitive points, reading it is advised
static bool lkca_hkdf_expand_only(struct crypto_shash *alg,
                                  const uint8_t *prk, size_t prk_size,
                                  const uint8_t *info, size_t info_size,
                                  uint8_t *out, size_t out_size)
{
#ifndef USE_LKCA
    return false;
#else
    int ret;
    int i;
    uint8_t ctr = 1;
    uint8_t tmp[HASH_MAX_DIGESTSIZE];
    SHASH_DESC_ON_STACK(desc, alg);
    desc->tfm = alg;

    ret = crypto_shash_setkey(desc->tfm, prk, prk_size);
    if (ret != 0) {
        pr_info("key size mismatch %ld\n", prk_size);
        return false;
    }

    for (i = 0, ctr = 1; i < out_size; i += prk_size, ctr++) {
        ret = crypto_shash_init(desc);
        if (ret) {
            return false;
        }

        if (i != 0) {
            ret = crypto_shash_update(desc, out + i - prk_size, prk_size);
            if (ret) {
                return false;
            }
        }

        if (info_size > 0) {
            ret = crypto_shash_update(desc, info, info_size);
            if (ret) {
                return false;
            }
        }

        ret = crypto_shash_update(desc, &ctr, 1);
        if (ret)
            return false;

        if ((out_size - i) < prk_size) {
            ret = crypto_shash_final(desc, tmp);
            if (ret) {
                return false;
            }
            memcpy(out + i, tmp, out_size - i);
            memzero_explicit(tmp, sizeof(tmp));
        } else {
            ret = crypto_shash_final(desc, out + i);
            if (ret) {
                return false;
            }
        }
    }

    return true;
#endif
}

bool lkca_hkdf_extract_and_expand(const char *alg_name,
                                  const uint8_t *key, size_t key_size,
                                  const uint8_t *salt, size_t salt_size,
                                  const uint8_t *info, size_t info_size,
                                  uint8_t *out, size_t out_size)
{
#ifndef USE_LKCA
    return false;
#else
    int ret = 0;
    struct crypto_shash *alg;
    uint8_t prk[HASH_MAX_DIGESTSIZE];

    if (key == NULL || salt == NULL || info == NULL || out == NULL ||
        key_size > sizeof(prk) || salt_size > INT_MAX || info_size > INT_MAX ||
        out_size > (sizeof(prk) * 255)) {
        return false;
    }

    alg = crypto_alloc_shash(alg_name, 0, 0);
    if (IS_ERR(alg)) {
        return false;
    }

    ret = crypto_shash_setkey(alg, salt, salt_size);
    if (ret != 0) {
        goto out;
    }
    ret = crypto_shash_tfm_digest(alg, key, key_size, prk);
    if (ret != 0) {
        goto out;
    }

    ret = !lkca_hkdf_expand_only(alg, prk, crypto_shash_digestsize(alg), info, info_size, out, out_size);

out:
    crypto_free_shash(alg);
    return ret == 0;
#endif
}

bool lkca_hkdf_expand(const char *alg_name,
                      const uint8_t *prk, size_t prk_size,
                      const uint8_t *info, size_t info_size,
                      uint8_t *out, size_t out_size)
{
#ifndef USE_LKCA
    return false;
#else
    bool ret = false;
    struct crypto_shash *alg;

    if (prk == NULL || info == NULL || out == NULL || prk_size > (512 / 8) ||
        info_size > INT_MAX || (out_size > (prk_size * 255))) {
        return false;
    }

    alg = crypto_alloc_shash(alg_name, 0, 0);
    if (IS_ERR(alg)) {
        return false;
    }

    ret = lkca_hkdf_expand_only(alg, prk, prk_size, info, info_size, out, out_size);

    crypto_free_shash(alg);
    return ret;
#endif
}
