/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"

uint32_t libspdm_get_aead_key_size(uint16_t aead_cipher_suite)
{
    switch (aead_cipher_suite) {
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM:
#if LIBSPDM_AEAD_AES_128_GCM_SUPPORT
        return 16;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM:
#if LIBSPDM_AEAD_AES_256_GCM_SUPPORT
        return 32;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305:
#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
        return 32;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM:
#if LIBSPDM_AEAD_SM4_128_GCM_SUPPORT
        return 16;
#else
        return 0;
#endif
    default:
        return 0;
    }
}

uint32_t libspdm_get_aead_iv_size(uint16_t aead_cipher_suite)
{
    switch (aead_cipher_suite) {
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM:
#if LIBSPDM_AEAD_AES_128_GCM_SUPPORT
        return 12;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM:
#if LIBSPDM_AEAD_AES_256_GCM_SUPPORT
        return 12;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305:
#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
        return 12;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM:
#if LIBSPDM_AEAD_SM4_128_GCM_SUPPORT
        return 12;
#else
        return 0;
#endif
    default:
        return 0;
    }
}

uint32_t libspdm_get_aead_tag_size(uint16_t aead_cipher_suite)
{
    switch (aead_cipher_suite) {
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM:
#if LIBSPDM_AEAD_AES_128_GCM_SUPPORT
        return 16;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM:
#if LIBSPDM_AEAD_AES_256_GCM_SUPPORT
        return 16;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305:
#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
        return 16;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM:
#if LIBSPDM_AEAD_SM4_128_GCM_SUPPORT
        return 16;
#else
        return 0;
#endif
    default:
        return 0;
    }
}

bool libspdm_aead_encryption(const spdm_version_number_t secured_message_version,
                             uint16_t aead_cipher_suite, const uint8_t *key,
                             size_t key_size, const uint8_t *iv,
                             size_t iv_size, const uint8_t *a_data,
                             size_t a_data_size, const uint8_t *data_in,
                             size_t data_in_size, uint8_t *tag_out,
                             size_t tag_size, uint8_t *data_out,
                             size_t *data_out_size)
{
    switch (aead_cipher_suite) {
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM:
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM:
#if LIBSPDM_AEAD_GCM_SUPPORT
#if !LIBSPDM_AEAD_AES_128_GCM_SUPPORT
        LIBSPDM_ASSERT(aead_cipher_suite != SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM);
#endif
#if !LIBSPDM_AEAD_AES_256_GCM_SUPPORT
        LIBSPDM_ASSERT(aead_cipher_suite != SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM);
#endif
        return libspdm_aead_aes_gcm_encrypt(key, key_size, iv, iv_size, a_data,
                                            a_data_size, data_in, data_in_size, tag_out,
                                            tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305:
#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
        return libspdm_aead_chacha20_poly1305_encrypt(key, key_size, iv, iv_size, a_data,
                                                      a_data_size, data_in, data_in_size, tag_out,
                                                      tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM:
#if LIBSPDM_AEAD_SM4_SUPPORT
        return libspdm_aead_sm4_gcm_encrypt(key, key_size, iv, iv_size, a_data,
                                            a_data_size, data_in, data_in_size, tag_out,
                                            tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_aead_decryption(const spdm_version_number_t secured_message_version,
                             uint16_t aead_cipher_suite, const uint8_t *key,
                             size_t key_size, const uint8_t *iv,
                             size_t iv_size, const uint8_t *a_data,
                             size_t a_data_size, const uint8_t *data_in,
                             size_t data_in_size, const uint8_t *tag,
                             size_t tag_size, uint8_t *data_out,
                             size_t *data_out_size)
{
    switch (aead_cipher_suite) {
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM:
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM:
#if LIBSPDM_AEAD_GCM_SUPPORT
#if !LIBSPDM_AEAD_AES_128_GCM_SUPPORT
        LIBSPDM_ASSERT(aead_cipher_suite != SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM);
#endif
#if !LIBSPDM_AEAD_AES_256_GCM_SUPPORT
        LIBSPDM_ASSERT(aead_cipher_suite != SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM);
#endif
        return libspdm_aead_aes_gcm_decrypt(key, key_size, iv, iv_size, a_data,
                                            a_data_size, data_in, data_in_size, tag,
                                            tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305:
#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
        return libspdm_aead_chacha20_poly1305_decrypt(key, key_size, iv, iv_size, a_data,
                                                      a_data_size, data_in, data_in_size, tag,
                                                      tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM:
#if LIBSPDM_AEAD_SM4_SUPPORT
        return libspdm_aead_sm4_gcm_decrypt(key, key_size, iv, iv_size, a_data,
                                            a_data_size, data_in, data_in_size, tag,
                                            tag_size, data_out, data_out_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}
