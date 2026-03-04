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
 * Generates RSA context from DER-encoded public key data.
 *
 * The public key is ASN.1 DER-encoded as RFC7250 describes,
 * namely, the SubjectPublicKeyInfo structure of a X.509 certificate.
 *
 * @param[in]  der_data    Pointer to the DER-encoded public key data.
 * @param[in]  der_size    Size of the DER-encoded public key data in bytes.
 * @param[out] rsa_context Pointer to newly generated RSA context which contains the
 *                         RSA public key component.
 *                         Use libspdm_rsa_free() function to free the resource.
 *
 * If der_data is NULL, then return false.
 * If rsa_context is NULL, then return false.
 *
 * @retval  true   RSA context was generated successfully.
 * @retval  false  Invalid DER public key data.
 **/
extern bool libspdm_rsa_get_public_key_from_der(const uint8_t *der_data,
                                                size_t der_size,
                                                void **rsa_context);

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

#if LIBSPDM_FIPS_MODE
/**
 * Carries out the RSA-SSA signature generation with EMSA-PSS encoding scheme for FIPS test.
 *
 * This function carries out the RSA-SSA signature generation with EMSA-PSS encoding scheme defined in
 * RSA PKCS#1 v2.2 for FIPS test.
 *
 * The salt length is zero.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If hash_size need match the hash_nid. nid could be SHA256, SHA384, SHA512, SHA3_256, SHA3_384, SHA3_512.
 * If sig_size is large enough but signature is NULL, then return false.
 *
 * @param[in]       rsa_context   Pointer to RSA context for signature generation.
 * @param[in]       hash_nid      hash NID
 * @param[in]       message_hash  Pointer to octet message hash to be signed.
 * @param[in]       hash_size     size of the message hash in bytes.
 * @param[out]      signature    Pointer to buffer to receive RSA-SSA PSS signature.
 * @param[in, out]  sig_size      On input, the size of signature buffer in bytes.
 *                              On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   signature successfully generated in RSA-SSA PSS.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 *
 **/
extern bool libspdm_rsa_pss_sign_fips(void *rsa_context, size_t hash_nid,
                                      const uint8_t *message_hash, size_t hash_size,
                                      uint8_t *signature, size_t *sig_size);

/**
 * Verifies the RSA-SSA signature with EMSA-PSS encoding scheme defined in
 * RSA PKCS#1 v2.2 for FIPS test.
 *
 * The salt length is zero.
 *
 * If rsa_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If signature is NULL, then return false.
 * If hash_size need match the hash_nid. nid could be SHA256, SHA384, SHA512, SHA3_256, SHA3_384, SHA3_512.
 *
 * @param[in]  rsa_context   Pointer to RSA context for signature verification.
 * @param[in]  hash_nid      hash NID
 * @param[in]  message_hash  Pointer to octet message hash to be checked.
 * @param[in]  hash_size     size of the message hash in bytes.
 * @param[in]  signature    Pointer to RSA-SSA PSS signature to be verified.
 * @param[in]  sig_size      size of signature in bytes.
 *
 * @retval  true   Valid signature encoded in RSA-SSA PSS.
 * @retval  false  Invalid signature or invalid RSA context.
 *
 **/
extern bool libspdm_rsa_pss_verify_fips(void *rsa_context, size_t hash_nid,
                                        const uint8_t *message_hash, size_t hash_size,
                                        const uint8_t *signature, size_t sig_size);
#endif /*LIBSPDM_FIPS_MODE*/

#endif /* LIBSPDM_RSA_PSS_SUPPORT */
#endif /* CRYPTLIB_RSA_H */
