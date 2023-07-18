/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_RSA_H
#define CRYPTLIB_RSA_H

/*=====================================================================================
 *    RSA Cryptography Primitives
 *=====================================================================================
 */

#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
/* RSA key Tags Definition used in libspdm_rsa_set_key() function for key component
 * identification.
 */
typedef enum {
    LIBSPDM_RSA_KEY_N, /*< RSA public Modulus (N)*/
    LIBSPDM_RSA_KEY_E, /*< RSA public exponent (e)*/
    LIBSPDM_RSA_KEY_D, /*< RSA Private exponent (d)*/
    LIBSPDM_RSA_KEY_P, /*< RSA secret prime factor of Modulus (p)*/
    LIBSPDM_RSA_KEY_Q, /*< RSA secret prime factor of Modules (q)*/
    LIBSPDM_RSA_KEY_DP, /*< p's CRT exponent (== d mod (p - 1))*/
    LIBSPDM_RSA_KEY_DQ, /*< q's CRT exponent (== d mod (q - 1))*/
    LIBSPDM_RSA_KEY_Q_INV /*< The CRT coefficient (== 1/q mod p)*/
} libspdm_rsa_key_tag_t;

/**
 * Allocates and initializes one RSA context for subsequent use.
 *
 * @return  Pointer to the RSA context that has been initialized.
 *          If the allocations fails, libspdm_rsa_new() returns NULL.
 **/
extern void *libspdm_rsa_new(void);

/**
 * Release the specified RSA context.
 *
 * If rsa_context is NULL, then return false.
 *
 * @param[in]  rsa_context  Pointer to the RSA context to be released.
 **/
extern void libspdm_rsa_free(void *rsa_context);

/**
 * Sets the tag-designated key component into the established RSA context.
 *
 * This function sets the tag-designated RSA key component into the established
 * RSA context from the user-specified non-negative integer (octet string format
 * represented in RSA PKCS#1).
 * If big_number is NULL, then the specified key component in RSA context is cleared.
 * If rsa_context is NULL, then return false.
 *
 * @param[in, out]  rsa_context  Pointer to RSA context being set.
 * @param[in]       key_tag      tag of RSA key component being set.
 * @param[in]       big_number   Pointer to octet integer buffer.
 *                               If NULL, then the specified key component in RSA
 *                               context is cleared.
 * @param[in]       bn_size      Size of big number buffer in bytes.
 *                               If big_number is NULL, then it is ignored.
 *
 * @retval  true   RSA key component was set successfully.
 * @retval  false  Invalid RSA key component tag.
 **/
extern bool libspdm_rsa_set_key(void *rsa_context, const libspdm_rsa_key_tag_t key_tag,
                                const uint8_t *big_number, size_t bn_size);

/**
 * Gets the tag-designated RSA key component from the established RSA context.
 *
 * This function retrieves the tag-designated RSA key component from the
 * established RSA context as a non-negative integer (octet string format
 * represented in RSA PKCS#1).
 * If specified key component has not been set or has been cleared, then returned
 * bn_size is set to 0.
 * If the big_number buffer is too small to hold the contents of the key, false
 * is returned and bn_size is set to the required buffer size to obtain the key.
 *
 * If rsa_context is NULL, then return false.
 * If bn_size is NULL, then return false.
 * If bn_size is large enough but big_number is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  rsa_context  Pointer to RSA context being set.
 * @param[in]       key_tag      Tag of RSA key component being set.
 * @param[out]      big_number   Pointer to octet integer buffer.
 * @param[in, out]  bn_size      On input, the size of big number buffer in bytes.
 *                               On output, the size of data returned in big number buffer in bytes.
 *
 * @retval  true   RSA key component was retrieved successfully.
 * @retval  false  Invalid RSA key component tag.
 * @retval  false  bn_size is too small.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_get_key(void *rsa_context, const libspdm_rsa_key_tag_t key_tag,
                                uint8_t *big_number, size_t *bn_size);

/**
 * Generates RSA key components.
 *
 * This function generates RSA key components. It takes RSA public exponent E and
 * length in bits of RSA modulus N as input, and generates all key components.
 * If public_exponent is NULL, the default RSA public exponent (0x10001) will be used.
 *
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  rsa_context           Pointer to RSA context being set.
 * @param[in]       modulus_length        Length of RSA modulus N in bits.
 * @param[in]       public_exponent       Pointer to RSA public exponent.
 * @param[in]       public_exponent_size  Size of RSA public exponent buffer in bytes.
 *
 * @retval  true   RSA key component was generated successfully.
 * @retval  false  Invalid RSA key component tag.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_generate_key(void *rsa_context, size_t modulus_length,
                                     const uint8_t *public_exponent,
                                     size_t public_exponent_size);

/**
 * Validates key components of RSA context.
 * NOTE: This function performs integrity checks on all the RSA key material, so
 *      the RSA key structure must contain all the private key data.
 *
 * This function validates key components of RSA context in following aspects:
 * - Whether p is a prime
 * - Whether q is a prime
 * - Whether n = p * q
 * - Whether d*e = 1  mod lcm(p-1,q-1)
 *
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  rsa_context  Pointer to RSA context to check.
 *
 * @retval  true   RSA key components are valid.
 * @retval  false  RSA key components are not valid.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_check_key(void *rsa_context);
#endif /* (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) */

#if LIBSPDM_RSA_SSA_SUPPORT
/**
 * Carries out the RSA-SSA signature generation with EMSA-PKCS1-v1_5 encoding scheme.
 *
 * This function carries out the RSA-SSA signature generation with EMSA-PKCS1-v1_5 encoding scheme
 * defined in RSA PKCS#1. If the signature buffer is too small to hold the contents of signature,
 * false is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If hash_size need match the hash_nid. hash_nid could be SHA256, SHA384, SHA512, SHA3_256,
 * SHA3_384, SHA3_512.
 * If sig_size is large enough but signature is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      rsa_context   Pointer to RSA context for signature generation.
 * @param[in]      hash_nid      hash NID
 * @param[in]      message_hash  Pointer to octet message hash to be signed.
 * @param[in]      hash_size     Size of the message hash in bytes.
 * @param[out]     signature     Pointer to buffer to receive RSA PKCS1-v1_5 signature.
 * @param[in, out] sig_size      On input, the size of signature buffer in bytes.
 *                               On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   signature successfully generated in PKCS1-v1_5.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_pkcs1_sign_with_nid(void *rsa_context, size_t hash_nid,
                                            const uint8_t *message_hash,
                                            size_t hash_size, uint8_t *signature,
                                            size_t *sig_size);

/**
 * Verifies the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If signature is NULL, then return false.
 * If hash_size need match the hash_nid. hash_nid could be SHA256, SHA384, SHA512, SHA3_256,
 * SHA3_384, SHA3_512.
 *
 * @param[in]  rsa_context   Pointer to RSA context for signature verification.
 * @param[in]  hash_nid      hash NID
 * @param[in]  message_hash  Pointer to octet message hash to be checked.
 * @param[in]  hash_size     Size of the message hash in bytes.
 * @param[in]  signature     Pointer to RSA PKCS1-v1_5 signature to be verified.
 * @param[in]  sig_size      Size of signature in bytes.
 *
 * @retval  true   Valid signature encoded in PKCS1-v1_5.
 * @retval  false  Invalid signature or invalid RSA context.
 **/
extern bool libspdm_rsa_pkcs1_verify_with_nid(void *rsa_context, size_t hash_nid,
                                              const uint8_t *message_hash,
                                              size_t hash_size, const uint8_t *signature,
                                              size_t sig_size);
#endif /* LIBSPDM_RSA_SSA_SUPPORT */

#if LIBSPDM_RSA_PSS_SUPPORT
/**
 * Carries out the RSA-SSA signature generation with EMSA-PSS encoding scheme.
 *
 * This function carries out the RSA-SSA signature generation with EMSA-PSS encoding scheme defined
 * in RSA PKCS#1 v2.2.
 *
 * The salt length is same as digest length.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If hash_size need match the hash_nid. nid could be SHA256, SHA384, SHA512, SHA3_256, SHA3_384,
 * SHA3_512.
 * If sig_size is large enough but signature is NULL, then return false.
 *
 * @param[in]       rsa_context   Pointer to RSA context for signature generation.
 * @param[in]       hash_nid      hash NID
 * @param[in]       message_hash  Pointer to octet message hash to be signed.
 * @param[in]       hash_size     Size of the message hash in bytes.
 * @param[out]      signature     Pointer to buffer to receive RSA-SSA PSS signature.
 * @param[in, out]  sig_size      On input, the size of signature buffer in bytes.
 *                                On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   signature successfully generated in RSA-SSA PSS.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 **/
extern bool libspdm_rsa_pss_sign(void *rsa_context, size_t hash_nid,
                                 const uint8_t *message_hash, size_t hash_size,
                                 uint8_t *signature, size_t *sig_size);

/**
 * Verifies the RSA-SSA signature with EMSA-PSS encoding scheme defined in
 * RSA PKCS#1 v2.2.
 *
 * The salt length is same as digest length.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If signature is NULL, then return false.
 * If hash_size need match the hash_nid. nid could be SHA256, SHA384, SHA512, SHA3_256, SHA3_384,
 * SHA3_512.
 *
 * @param[in]  rsa_context   Pointer to RSA context for signature verification.
 * @param[in]  hash_nid      hash NID
 * @param[in]  message_hash  Pointer to octet message hash to be checked.
 * @param[in]  hash_size     Size of the message hash in bytes.
 * @param[in]  signature     Pointer to RSA-SSA PSS signature to be verified.
 * @param[in]  sig_size      Size of signature in bytes.
 *
 * @retval  true   Valid signature encoded in RSA-SSA PSS.
 * @retval  false  Invalid signature or invalid RSA context.
 **/
extern bool libspdm_rsa_pss_verify(void *rsa_context, size_t hash_nid,
                                   const uint8_t *message_hash, size_t hash_size,
                                   const uint8_t *signature, size_t sig_size);
#endif /* LIBSPDM_RSA_PSS_SUPPORT */
#endif /* CRYPTLIB_RSA_H */
