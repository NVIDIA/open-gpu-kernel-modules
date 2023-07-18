/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_HKDF_H
#define CRYPTLIB_HKDF_H

/*=====================================================================================
 *    Key Derivation Function Primitives
 *=====================================================================================*/

#if LIBSPDM_SHA256_SUPPORT
/**
 * Derive SHA-256 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive prk value.
 * @param[in]   prk_out_size  Size of prk bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha256_extract(const uint8_t *key, size_t key_size,
                                        const uint8_t *salt, size_t salt_size,
                                        uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA256 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha256_expand(const uint8_t *prk, size_t prk_size,
                                       const uint8_t *info, size_t info_size,
                                       uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA256_SUPPORT */

#if LIBSPDM_SHA384_SUPPORT
/**
 * Derive SHA384 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha384_extract(const uint8_t *key, size_t key_size,
                                        const uint8_t *salt, size_t salt_size,
                                        uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA384 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha384_expand(const uint8_t *prk, size_t prk_size,
                                       const uint8_t *info, size_t info_size,
                                       uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA384_SUPPORT */

#if LIBSPDM_SHA512_SUPPORT
/**
 * Derive SHA512 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha512_extract(const uint8_t *key, size_t key_size,
                                        const uint8_t *salt, size_t salt_size,
                                        uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA512 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha512_expand(const uint8_t *prk, size_t prk_size,
                                       const uint8_t *info, size_t info_size,
                                       uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA512_SUPPORT */

#if LIBSPDM_SHA3_256_SUPPORT
/**
 * Derive SHA3_256 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_256_extract(const uint8_t *key, size_t key_size,
                                          const uint8_t *salt, size_t salt_size,
                                          uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA3_256 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_256_expand(const uint8_t *prk, size_t prk_size,
                                         const uint8_t *info, size_t info_size,
                                         uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA3_256_SUPPORT */

#if LIBSPDM_SHA3_384_SUPPORT
/**
 * Derive SHA3_384 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_384_extract(const uint8_t *key, size_t key_size,
                                          const uint8_t *salt, size_t salt_size,
                                          uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA3_384 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_384_expand(const uint8_t *prk, size_t prk_size,
                                         const uint8_t *info, size_t info_size,
                                         uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA3_384_SUPPORT */

#if LIBSPDM_SHA3_512_SUPPORT
/**
 * Derive SHA3_512 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_512_extract(const uint8_t *key, size_t key_size,
                                          const uint8_t *salt, size_t salt_size,
                                          uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SHA3_512 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_512_expand(const uint8_t *prk, size_t prk_size,
                                         const uint8_t *info, size_t info_size,
                                         uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SHA3_512_SUPPORT */

#if LIBSPDM_SM3_256_SUPPORT
/**
 * Derive SM3_256 HMAC-based Extract key Derivation Function (HKDF).
 *
 * @param[in]   key           Pointer to the user-supplied key.
 * @param[in]   key_size      Key size in bytes.
 * @param[in]   salt          Pointer to the salt value.
 * @param[in]   salt_size     Salt size in bytes.
 * @param[out]  prk_out       Pointer to buffer to receive hkdf value.
 * @param[in]   prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sm3_256_extract(const uint8_t *key, size_t key_size,
                                         const uint8_t *salt, size_t salt_size,
                                         uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive SM3_256 HMAC-based Expand key Derivation Function (HKDF).
 *
 * @param[in]   prk        Pointer to the user-supplied key.
 * @param[in]   prk_size   Key size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sm3_256_expand(const uint8_t *prk, size_t prk_size,
                                        const uint8_t *info, size_t info_size,
                                        uint8_t *out, size_t out_size);
#endif /* LIBSPDM_SM3_256_SUPPORT */

#endif /* CRYPTLIB_HKDF_H */
