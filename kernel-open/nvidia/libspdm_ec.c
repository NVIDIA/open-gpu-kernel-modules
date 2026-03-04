/*
* SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

static bool lkca_ecdsa_sign(void *ec_context,
                            const uint8_t *message_hash, size_t hash_size,
                            uint8_t *signature, size_t *sig_size)
{
    return false;
}

bool libspdm_ec_set_pub_key(void *ec_context, const uint8_t *public_key,
                            size_t public_key_size)
{
    if (ec_context == NULL || public_key == NULL) {
        return false;
    }

    return lkca_ec_set_pub_key(ec_context, public_key, public_key_size);
}

bool libspdm_ec_get_pub_key(void *ec_context, uint8_t *public_key,
                            size_t *public_key_size)
{
    if (ec_context == NULL || public_key_size == NULL) {
        return false;
    }

    if (public_key == NULL && *public_key_size != 0) {
        return false;
    }

    return lkca_ec_get_pub_key(ec_context, public_key, public_key_size);
}

bool libspdm_ec_check_key(const void *ec_context)
{
    /* TBD*/
    return true;
}

bool libspdm_ec_generate_key(void *ec_context, uint8_t *public_data,
                             size_t *public_size)
{
    if (ec_context == NULL || public_size == NULL) {
        return false;
    }

    if (public_data == NULL && *public_size != 0) {
        return false;
    }

    return lkca_ec_generate_key(ec_context, public_data, public_size);
}

bool libspdm_ec_compute_key(void *ec_context, const uint8_t *peer_public,
                            size_t peer_public_size, uint8_t *key,
                            size_t *key_size)
{
    if (ec_context == NULL || peer_public == NULL || key_size == NULL ||
        key == NULL) {
        return false;
    }

    if (peer_public_size > INT_MAX) {
        return false;
    }

    return lkca_ec_compute_key(ec_context, peer_public, peer_public_size, key,
                               key_size);
}

bool libspdm_ecdsa_sign(void *ec_context, size_t hash_nid,
                        const uint8_t *message_hash, size_t hash_size,
                        uint8_t *signature, size_t *sig_size)
{
    if (ec_context == NULL || message_hash == NULL) {
        return false;
    }

    if (signature == NULL) {
        return false;
    }

    switch (hash_nid) {
        case LIBSPDM_CRYPTO_NID_SHA256:
            if (hash_size != LIBSPDM_SHA256_DIGEST_SIZE) {
                return false;
            }
            break;

        case LIBSPDM_CRYPTO_NID_SHA384:
            if (hash_size != LIBSPDM_SHA384_DIGEST_SIZE) {
                return false;
            }
            break;

        case LIBSPDM_CRYPTO_NID_SHA512:
            if (hash_size != LIBSPDM_SHA512_DIGEST_SIZE) {
                return false;
            }
            break;

        default:
            return false;
    }

    return lkca_ecdsa_sign(ec_context, message_hash, hash_size, signature, sig_size);
}

bool libspdm_ecdsa_verify(void *ec_context, size_t hash_nid,
                          const uint8_t *message_hash, size_t hash_size,
                          const uint8_t *signature, size_t sig_size)
{
    if (ec_context == NULL || message_hash == NULL || signature == NULL) {
        return false;
    }

    if (sig_size > INT_MAX || sig_size == 0) {
        return false;
    }

    switch (hash_nid) {
    case LIBSPDM_CRYPTO_NID_SHA256:
        if (hash_size != LIBSPDM_SHA256_DIGEST_SIZE) {
            return false;
        }
        break;

    case LIBSPDM_CRYPTO_NID_SHA384:
        if (hash_size != LIBSPDM_SHA384_DIGEST_SIZE) {
            return false;
        }
        break;

    case LIBSPDM_CRYPTO_NID_SHA512:
        if (hash_size != LIBSPDM_SHA512_DIGEST_SIZE) {
            return false;
        }
        break;

    default:
        return false;
    }

    return lkca_ecdsa_verify(ec_context, hash_nid, message_hash, hash_size,
                             signature, sig_size);
}
