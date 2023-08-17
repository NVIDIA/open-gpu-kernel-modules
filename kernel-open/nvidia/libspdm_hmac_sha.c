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
*
* Prototypes and checks taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
* License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
*/

#include "internal_crypt_lib.h"

void *libspdm_hmac_sha256_new(void)
{
    return lkca_hash_new("hmac(sha256)");
}

void libspdm_hmac_sha256_free(void *hmac_sha256_ctx)
{
    lkca_hash_free(hmac_sha256_ctx);
}

bool libspdm_hmac_sha256_set_key(void *hmac_sha256_ctx, const uint8_t *key,
                                 size_t key_size)
{
    if (hmac_sha256_ctx == NULL)
        return false;

    return lkca_hmac_set_key(hmac_sha256_ctx, key, key_size);
}

bool libspdm_hmac_sha256_duplicate(const void *hmac_sha256_ctx,
                                   void *new_hmac_sha256_ctx)
{
    if (hmac_sha256_ctx == NULL || new_hmac_sha256_ctx == NULL) {
        return false;
    }

    return lkca_hmac_duplicate(new_hmac_sha256_ctx, hmac_sha256_ctx);
}

bool libspdm_hmac_sha256_update(void *hmac_sha256_ctx, const void *data,
                                size_t data_size)
{
    int32_t ret;

    if (hmac_sha256_ctx == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(hmac_sha256_ctx, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha256_final(void *hmac_sha256_ctx, uint8_t *hmac_value)
{
    int32_t ret;

    if (hmac_sha256_ctx == NULL || hmac_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(hmac_sha256_ctx, hmac_value);

    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha256_all(const void *data, size_t data_size,
                             const uint8_t *key, size_t key_size,
                             uint8_t *hmac_value)
{
    if (hmac_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hmac_all("hmac(sha256)", key, key_size, data, data_size, hmac_value);
}

void *libspdm_hmac_sha384_new(void)
{
    return lkca_hash_new("hmac(sha384)");
}

void libspdm_hmac_sha384_free(void *hmac_sha384_ctx)
{
    lkca_hash_free(hmac_sha384_ctx);
}

bool libspdm_hmac_sha384_set_key(void *hmac_sha384_ctx, const uint8_t *key,
                                 size_t key_size)
{
    if (hmac_sha384_ctx == NULL)
        return false;

    return lkca_hmac_set_key(hmac_sha384_ctx, key, key_size);
}

bool libspdm_hmac_sha384_duplicate(const void *hmac_sha384_ctx,
                                   void *new_hmac_sha384_ctx)
{
    if (hmac_sha384_ctx == NULL || new_hmac_sha384_ctx == NULL) {
        return false;
    }

    return lkca_hmac_duplicate(new_hmac_sha384_ctx, hmac_sha384_ctx);
}

bool libspdm_hmac_sha384_update(void *hmac_sha384_ctx, const void *data,
                                size_t data_size)
{
    int32_t ret;

    if (hmac_sha384_ctx == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(hmac_sha384_ctx, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha384_final(void *hmac_sha384_ctx, uint8_t *hmac_value)
{
    int32_t ret;

    if (hmac_sha384_ctx == NULL || hmac_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(hmac_sha384_ctx, hmac_value);

    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha384_all(const void *data, size_t data_size,
                             const uint8_t *key, size_t key_size,
                             uint8_t *hmac_value)
{
    if (hmac_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hmac_all("hmac(sha384)", key, key_size, data, data_size, hmac_value);
}

void *libspdm_hmac_sha512_new(void)
{
    return lkca_hash_new("hmac(sha512)");
}

void libspdm_hmac_sha512_free(void *hmac_sha512_ctx)
{
    lkca_hash_free(hmac_sha512_ctx);
}

bool libspdm_hmac_sha512_set_key(void *hmac_sha512_ctx, const uint8_t *key,
                                 size_t key_size)
{
    if (hmac_sha512_ctx == NULL)
        return false;

    return lkca_hmac_set_key(hmac_sha512_ctx, key, key_size);
}

bool libspdm_hmac_sha512_duplicate(const void *hmac_sha512_ctx,
                                   void *new_hmac_sha512_ctx)
{
    if (new_hmac_sha512_ctx == NULL || new_hmac_sha512_ctx == NULL) {
        return false;
    }

    return lkca_hmac_duplicate(new_hmac_sha512_ctx, hmac_sha512_ctx);
}

bool libspdm_hmac_sha512_update(void *hmac_sha512_ctx, const void *data,
                                size_t data_size)
{
    int32_t ret;

    if (hmac_sha512_ctx == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(hmac_sha512_ctx, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha512_final(void *hmac_sha512_ctx, uint8_t *hmac_value)
{
    int32_t ret;

    if (hmac_sha512_ctx == NULL || hmac_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(hmac_sha512_ctx, hmac_value);

    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_hmac_sha512_all(const void *data, size_t data_size,
                             const uint8_t *key, size_t key_size,
                             uint8_t *hmac_value)
{
    if (hmac_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hmac_all("hmac(sha512)", key, key_size, data, data_size, hmac_value);
}
