/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"

void *libspdm_hmac_new(uint32_t base_hash_algo)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_new();
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

void libspdm_hmac_free(uint32_t base_hash_algo, void *hmac_ctx)
{
    if (hmac_ctx == NULL) {
        return;
    }
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        libspdm_hmac_sha256_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        libspdm_hmac_sha384_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        libspdm_hmac_sha512_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        libspdm_hmac_sha3_256_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        libspdm_hmac_sha3_384_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        libspdm_hmac_sha3_512_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        libspdm_hmac_sm3_256_free(hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    default:
        LIBSPDM_ASSERT(false);
        break;
    }
}

bool libspdm_hmac_init(uint32_t base_hash_algo,
                       void *hmac_ctx, const uint8_t *key,
                       size_t key_size)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_set_key(hmac_ctx, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_hmac_duplicate(uint32_t base_hash_algo, const void *hmac_ctx, void *new_hmac_ctx)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_duplicate(hmac_ctx, new_hmac_ctx);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_hmac_update(uint32_t base_hash_algo,
                         void *hmac_ctx, const void *data,
                         size_t data_size)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_update(hmac_ctx, data, data_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_hmac_final(uint32_t base_hash_algo, void *hmac_ctx,  uint8_t *hmac_value)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_final(hmac_ctx, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_hmac_all(uint32_t base_hash_algo, const void *data,
                      size_t data_size, const uint8_t *key,
                      size_t key_size, uint8_t *hmac_value)
{
    switch (base_hash_algo) {
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256:
#if LIBSPDM_SHA256_SUPPORT
        return libspdm_hmac_sha256_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384:
#if LIBSPDM_SHA384_SUPPORT
        return libspdm_hmac_sha384_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512:
#if LIBSPDM_SHA512_SUPPORT
        return libspdm_hmac_sha512_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256:
#if LIBSPDM_SHA3_256_SUPPORT
        return libspdm_hmac_sha3_256_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384:
#if LIBSPDM_SHA3_384_SUPPORT
        return libspdm_hmac_sha3_384_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512:
#if LIBSPDM_SHA3_512_SUPPORT
        return libspdm_hmac_sha3_512_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256:
#if LIBSPDM_SM3_256_SUPPORT
        return libspdm_hmac_sm3_256_all(data, data_size, key, key_size, hmac_value);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}
