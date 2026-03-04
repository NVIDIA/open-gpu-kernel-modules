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
* Comments, prototypes and checks taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
* License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
*/

#include "internal_crypt_lib.h"

void *libspdm_sha256_new(void)
{
    return lkca_hash_new("sha256");
}

void libspdm_sha256_free(void *sha256_ctx)
{
    lkca_hash_free(sha256_ctx);
}

bool libspdm_sha256_init(void *sha256_context)
{
    return crypto_shash_init(sha256_context) == 0;
}

bool libspdm_sha256_duplicate(const void *sha256_context,
                              void *new_sha256_context)
{
    if (sha256_context == NULL || new_sha256_context == NULL) {
        return false;
    }

    return lkca_hash_duplicate(new_sha256_context, sha256_context);
}

bool libspdm_sha256_update(void *sha256_context, const void *data,
                           size_t data_size)
{
    int32_t ret;

    if (sha256_context == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(sha256_context, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha256_final(void *sha256_context, uint8_t *hash_value)
{
    int32_t ret;

    if (sha256_context == NULL || hash_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(sha256_context, hash_value);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha256_hash_all(const void *data, size_t data_size,
                             uint8_t *hash_value)
{
    if (hash_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hash_all("sha256", data, data_size, hash_value);
}

void *libspdm_sha384_new(void)
{
    return lkca_hash_new("sha384");
}

void libspdm_sha384_free(void *sha384_ctx)
{
    lkca_hash_free(sha384_ctx);
}

bool libspdm_sha384_init(void *sha384_context)
{
    return crypto_shash_init(sha384_context) == 0;
}

bool libspdm_sha384_duplicate(const void *sha384_context,
                              void *new_sha384_context)
{
    if (sha384_context == NULL || new_sha384_context == NULL) {
        return false;
    }

    return lkca_hash_duplicate(new_sha384_context, sha384_context);
}

bool libspdm_sha384_update(void *sha384_context, const void *data,
                           size_t data_size)
{
    int32_t ret;

    if (sha384_context == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(sha384_context, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha384_final(void *sha384_context, uint8_t *hash_value)
{
    int32_t ret;

    if (sha384_context == NULL || hash_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(sha384_context, hash_value);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha384_hash_all(const void *data, size_t data_size,
                             uint8_t *hash_value)
{
    if (hash_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hash_all("sha384", data, data_size, hash_value);
}

void *libspdm_sha512_new(void)
{
    return lkca_hash_new("sha512");
}

void libspdm_sha512_free(void *sha512_ctx)
{
    lkca_hash_free(sha512_ctx);
}

bool libspdm_sha512_init(void *sha512_context)
{
    return crypto_shash_init(sha512_context) == 0;
}

bool libspdm_sha512_duplicate(const void *sha512_context,
                              void *new_sha512_context)
{
    if (sha512_context == NULL || new_sha512_context == NULL) {
        return false;
    }

    return lkca_hash_duplicate(new_sha512_context, sha512_context);
}

bool libspdm_sha512_update(void *sha512_context, const void *data,
                           size_t data_size)
{
    int32_t ret;

    if (sha512_context == NULL) {
        return false;
    }

    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    ret = crypto_shash_update(sha512_context, data, data_size);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha512_final(void *sha512_context, uint8_t *hash_value)
{
    int32_t ret;

    if (sha512_context == NULL || hash_value == NULL) {
        return false;
    }

    ret = crypto_shash_final(sha512_context, hash_value);
    if (ret != 0) {
        return false;
    }
    return true;
}

bool libspdm_sha512_hash_all(const void *data, size_t data_size,
                             uint8_t *hash_value)
{
    if (hash_value == NULL) {
        return false;
    }
    if (data == NULL && data_size != 0) {
        return false;
    }
    if (data_size > INT_MAX) {
        return false;
    }

    return lkca_hash_all("sha512", data, data_size, hash_value);
}
