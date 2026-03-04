/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_AEAD_H
#define CRYPTLIB_AEAD_H

/*=====================================================================================
 *    Authenticated Encryption with Associated data (AEAD) Cryptography Primitives
 *=====================================================================================
 */

#if LIBSPDM_AEAD_GCM_SUPPORT
/**
 * Performs AEAD AES-GCM authenticated encryption on a data buffer and additional authenticated
 * data.
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16 or 32, otherwise false is returned.
 * tag_size must be 12, 13, 14, 15, 16, otherwise false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be encrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[out]  tag_out        Pointer to a buffer that receives the authentication tag output.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the encryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD AES-GCM authenticated encryption succeeded.
 * @retval false  AEAD AES-GCM authenticated encryption failed.
 **/
extern bool libspdm_aead_aes_gcm_encrypt(const uint8_t *key, size_t key_size,
                                         const uint8_t *iv, size_t iv_size,
                                         const uint8_t *a_data, size_t a_data_size,
                                         const uint8_t *data_in, size_t data_in_size,
                                         uint8_t *tag_out, size_t tag_size,
                                         uint8_t *data_out, size_t *data_out_size);

/**
 * Performs AEAD AES-GCM authenticated decryption on a data buffer and additional authenticated
 * data.
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16 or 32, otherwise false is returned.
 * tag_size must be 12, 13, 14, 15, 16, otherwise false is returned.
 *
 * If data verification fails, false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be decrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[in]   tag            Pointer to a buffer that contains the authentication tag.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the decryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD AES-GCM authenticated decryption succeeded.
 * @retval false  AEAD AES-GCM authenticated decryption failed.
 **/
extern bool libspdm_aead_aes_gcm_decrypt(const uint8_t *key, size_t key_size,
                                         const uint8_t *iv, size_t iv_size,
                                         const uint8_t *a_data, size_t a_data_size,
                                         const uint8_t *data_in, size_t data_in_size,
                                         const uint8_t *tag, size_t tag_size,
                                         uint8_t *data_out, size_t *data_out_size);
#endif /* LIBSPDM_AEAD_GCM_SUPPORT */

#if LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT
/**
 * Performs AEAD ChaCha20Poly1305 authenticated encryption on a data buffer and additional
 * authenticated data.
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 32, otherwise false is returned.
 * tag_size must be 16, otherwise false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be encrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[out]  tag_out        Pointer to a buffer that receives the authentication tag output.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the encryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD ChaCha20Poly1305 authenticated encryption succeeded.
 * @retval false  AEAD ChaCha20Poly1305 authenticated encryption failed.
 **/
extern bool libspdm_aead_chacha20_poly1305_encrypt(
    const uint8_t *key, size_t key_size, const uint8_t *iv,
    size_t iv_size, const uint8_t *a_data, size_t a_data_size,
    const uint8_t *data_in, size_t data_in_size, uint8_t *tag_out,
    size_t tag_size, uint8_t *data_out, size_t *data_out_size);

/**
 * Performs AEAD ChaCha20Poly1305 authenticated decryption on a data buffer and additional authenticated data (AAD).
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 32, otherwise false is returned.
 * tag_size must be 16, otherwise false is returned.
 *
 * If data verification fails, false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be decrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[in]   tag            Pointer to a buffer that contains the authentication tag.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the decryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD ChaCha20Poly1305 authenticated decryption succeeded.
 * @retval false  AEAD ChaCha20Poly1305 authenticated decryption failed.
 *
 **/
extern bool libspdm_aead_chacha20_poly1305_decrypt(
    const uint8_t *key, size_t key_size, const uint8_t *iv,
    size_t iv_size, const uint8_t *a_data, size_t a_data_size,
    const uint8_t *data_in, size_t data_in_size, const uint8_t *tag,
    size_t tag_size, uint8_t *data_out, size_t *data_out_size);
#endif /* LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT */

#if LIBSPDM_AEAD_SM4_SUPPORT
/**
 * Performs AEAD SM4-GCM authenticated encryption on a data buffer and additional authenticated
 * data.
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16, otherwise false is returned.
 * tag_size must be 16, otherwise false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be encrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[out]  tag_out        Pointer to a buffer that receives the authentication tag output.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the encryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD SM4-GCM authenticated encryption succeeded.
 * @retval false  AEAD SM4-GCM authenticated encryption failed.
 **/
extern bool libspdm_aead_sm4_gcm_encrypt(const uint8_t *key, size_t key_size,
                                         const uint8_t *iv, size_t iv_size,
                                         const uint8_t *a_data, size_t a_data_size,
                                         const uint8_t *data_in, size_t data_in_size,
                                         uint8_t *tag_out, size_t tag_size,
                                         uint8_t *data_out, size_t *data_out_size);

/**
 * Performs AEAD SM4-GCM authenticated decryption on a data buffer and additional authenticated
 * data.
 *
 * iv_size must be 12, otherwise false is returned.
 * key_size must be 16, otherwise false is returned.
 * tag_size must be 16, otherwise false is returned.
 *
 * If data verification fails, false is returned.
 *
 * @param[in]   key            Pointer to the encryption key.
 * @param[in]   key_size       Size of the encryption key in bytes.
 * @param[in]   iv             Pointer to the IV value.
 * @param[in]   iv_size        Size of the IV value in bytes.
 * @param[in]   a_data         Pointer to the additional authenticated data.
 * @param[in]   a_data_size    Size of the additional authenticated data in bytes.
 * @param[in]   data_in        Pointer to the input data buffer to be decrypted.
 * @param[in]   data_in_size   Size of the input data buffer in bytes.
 * @param[in]   tag            Pointer to a buffer that contains the authentication tag.
 * @param[in]   tag_size       Size of the authentication tag in bytes.
 * @param[out]  data_out       Pointer to a buffer that receives the decryption output.
 * @param[out]  data_out_size  Size of the output data buffer in bytes.
 *
 * @retval true   AEAD SM4-GCM authenticated decryption succeeded.
 * @retval false  AEAD SM4-GCM authenticated decryption failed.
 **/
extern bool libspdm_aead_sm4_gcm_decrypt(const uint8_t *key, size_t key_size,
                                         const uint8_t *iv, size_t iv_size,
                                         const uint8_t *a_data, size_t a_data_size,
                                         const uint8_t *data_in, size_t data_in_size,
                                         const uint8_t *tag, size_t tag_size,
                                         uint8_t *data_out, size_t *data_out_size);
#endif /* LIBSPDM_AEAD_SM4_SUPPORT */

#endif /* CRYPTLIB_AEAD_H */
