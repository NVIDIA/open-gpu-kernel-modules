/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * AEAD (AES-GCM) Wrapper Implementation.
 *
 * RFC 5116 - An Interface and Algorithms for Authenticated Encryption
 * NIST SP800-38d - Cipher Modes of Operation: Galois / Counter Mode(GCM) and GMAC
 **/

#include "internal_crypt_lib.h"
#include <mbedtls/gcm.h>

/**
 * Performs AEAD AES-GCM authenticated encryption on a data buffer and additional authenticated data (AAD).
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16, 24 or 32, otherwise false is returned.
 * tag_size must be 12, 13, 14, 15, 16, otherwise false is returned.
 *
 * @param[in]   key         Pointer to the encryption key.
 * @param[in]   key_size     size of the encryption key in bytes.
 * @param[in]   iv          Pointer to the IV value.
 * @param[in]   iv_size      size of the IV value in bytes.
 * @param[in]   a_data       Pointer to the additional authenticated data (AAD).
 * @param[in]   a_data_size   size of the additional authenticated data (AAD) in bytes.
 * @param[in]   data_in      Pointer to the input data buffer to be encrypted.
 * @param[in]   data_in_size  size of the input data buffer in bytes.
 * @param[out]  tag_out      Pointer to a buffer that receives the authentication tag output.
 * @param[in]   tag_size     size of the authentication tag in bytes.
 * @param[out]  data_out     Pointer to a buffer that receives the encryption output.
 * @param[out]  data_out_size size of the output data buffer in bytes.
 *
 * @retval true   AEAD AES-GCM authenticated encryption succeeded.
 * @retval false  AEAD AES-GCM authenticated encryption failed.
 *
 **/
bool libspdm_aead_aes_gcm_encrypt(const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  uint8_t *tag_out, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    mbedtls_gcm_context ctx;
    int32_t ret;

    if (data_in_size > INT_MAX) {
        return false;
    }
    if (a_data_size > INT_MAX) {
        return false;
    }
    if (iv_size != 12) {
        return false;
    }
    switch (key_size) {
    case 16:
    case 24:
    case 32:
        break;
    default:
        return false;
    }
    if ((tag_size != 12) && (tag_size != 13) && (tag_size != 14) &&
        (tag_size != 15) && (tag_size != 16)) {
        return false;
    }
    if (data_out_size != NULL) {
        if ((*data_out_size > INT_MAX) ||
            (*data_out_size < data_in_size)) {
            return false;
        }
    }

    mbedtls_gcm_init(&ctx);

    ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key,
                             (uint32_t)(key_size * 8));
    if (ret != 0) {
        return false;
    }

    ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                    (uint32_t)data_in_size, iv,
                                    (uint32_t)iv_size, a_data,
                                    (uint32_t)a_data_size, data_in, data_out,
                                    tag_size, tag_out);
    mbedtls_gcm_free(&ctx);
    if (ret != 0) {
        return false;
    }
    if (data_out_size != NULL) {
        *data_out_size = data_in_size;
    }

    return true;
}

/**
 * Performs AEAD AES-GCM authenticated decryption on a data buffer and additional authenticated data (AAD).
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16, 24 or 32, otherwise false is returned.
 * tag_size must be 12, 13, 14, 15, 16, otherwise false is returned.
 * If additional authenticated data verification fails, false is returned.
 *
 * @param[in]   key         Pointer to the encryption key.
 * @param[in]   key_size     size of the encryption key in bytes.
 * @param[in]   iv          Pointer to the IV value.
 * @param[in]   iv_size      size of the IV value in bytes.
 * @param[in]   a_data       Pointer to the additional authenticated data (AAD).
 * @param[in]   a_data_size   size of the additional authenticated data (AAD) in bytes.
 * @param[in]   data_in      Pointer to the input data buffer to be decrypted.
 * @param[in]   data_in_size  size of the input data buffer in bytes.
 * @param[in]   tag         Pointer to a buffer that contains the authentication tag.
 * @param[in]   tag_size     size of the authentication tag in bytes.
 * @param[out]  data_out     Pointer to a buffer that receives the decryption output.
 * @param[out]  data_out_size size of the output data buffer in bytes.
 *
 * @retval true   AEAD AES-GCM authenticated decryption succeeded.
 * @retval false  AEAD AES-GCM authenticated decryption failed.
 *
 **/
bool libspdm_aead_aes_gcm_decrypt(const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  const uint8_t *tag, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    mbedtls_gcm_context ctx;
    int32_t ret;

    if (data_in_size > INT_MAX) {
        return false;
    }
    if (a_data_size > INT_MAX) {
        return false;
    }
    if (iv_size != 12) {
        return false;
    }
    switch (key_size) {
    case 16:
    case 24:
    case 32:
        break;
    default:
        return false;
    }
    if ((tag_size != 12) && (tag_size != 13) && (tag_size != 14) &&
        (tag_size != 15) && (tag_size != 16)) {
        return false;
    }
    if (data_out_size != NULL) {
        if ((*data_out_size > INT_MAX) ||
            (*data_out_size < data_in_size)) {
            return false;
        }
    }

    mbedtls_gcm_init(&ctx);

    ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key,
                             (uint32_t)(key_size * 8));
    if (ret != 0) {
        return false;
    }

    ret = mbedtls_gcm_auth_decrypt(&ctx, (uint32_t)data_in_size, iv,
                                   (uint32_t)iv_size, a_data,
                                   (uint32_t)a_data_size, tag,
                                   (uint32_t)tag_size, data_in, data_out);
    mbedtls_gcm_free(&ctx);
    if (ret != 0) {
        return false;
    }
    if (data_out_size != NULL) {
        *data_out_size = data_in_size;
    }

    return true;
}
