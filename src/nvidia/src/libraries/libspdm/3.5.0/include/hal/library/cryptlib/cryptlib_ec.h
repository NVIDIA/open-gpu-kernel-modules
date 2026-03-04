/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_EC_H
#define CRYPTLIB_EC_H

/*=====================================================================================
 *    Elliptic Curve Primitives
 *=====================================================================================*/

#if (LIBSPDM_ECDHE_SUPPORT) || (LIBSPDM_ECDSA_SUPPORT)
/**
 * Allocates and Initializes one Elliptic Curve context for subsequent use with the NID.
 *
 * @param nid cipher NID
 *
 * @return  Pointer to the Elliptic Curve context that has been initialized.
 *          If the allocations fails, libspdm_ec_new_by_nid() returns NULL.
 **/
extern void *libspdm_ec_new_by_nid(size_t nid);

/**
 * Release the specified EC context.
 *
 * @param[in]  ec_context  Pointer to the EC context to be released.
 **/
extern void libspdm_ec_free(void *ec_context);

#if LIBSPDM_FIPS_MODE
/**
 * Sets the private key component into the established EC context.
 *
 * For P-256, the private_key_size is 32 byte.
 * For P-384, the private_key_size is 48 byte.
 * For P-521, the private_key_size is 66 byte.
 *
 * @param[in, out]  ec_context       Pointer to EC context being set.
 * @param[in]       private_key      Pointer to the private key buffer.
 * @param[in]       private_key_size The size of private key buffer in bytes.
 *
 * @retval  true   EC private key component was set successfully.
 * @retval  false  Invalid EC private key component.
 *
 **/
extern bool libspdm_ec_set_priv_key(void *ec_context, const uint8_t *private_key,
                                    size_t private_key_size);

/**
 * Sets the public key component into the established EC context.
 *
 * For P-256, the public_size is 64. first 32-byte is X, second 32-byte is Y.
 * For P-384, the public_size is 96. first 48-byte is X, second 48-byte is Y.
 * For P-521, the public_size is 132. first 66-byte is X, second 66-byte is Y.
 *
 * @param[in, out]  ec_context   Pointer to EC context being set.
 * @param[in]       public       Pointer to the buffer to receive generated public X,Y.
 * @param[in]       public_size  The size of public buffer in bytes.
 *
 * @retval  true   EC public key component was set successfully.
 * @retval  false  Invalid EC public key component.
 **/
extern bool libspdm_ec_set_pub_key(void *ec_context, const uint8_t *public_key,
                                   size_t public_key_size);
#endif /* LIBSPDM_FIPS_MODE */

#endif /* (LIBSPDM_ECDHE_SUPPORT) || (LIBSPDM_ECDSA_SUPPORT) */

#if LIBSPDM_ECDHE_SUPPORT
/**
 * Generates EC key and returns EC public key (X, Y).
 *
 * This function generates random secret, and computes the public key (X, Y), which is
 * returned via parameter public, public_size.
 * X is the first half of public with size being public_size / 2,
 * Y is the second half of public with size being public_size / 2.
 * EC context is updated accordingly.
 * If the public buffer is too small to hold the public X, Y, false is returned and
 * public_size is set to the required buffer size to obtain the public X, Y.
 *
 * For P-256, the public_size is 64. first 32-byte is X, second 32-byte is Y.
 * For P-384, the public_size is 96. first 48-byte is X, second 48-byte is Y.
 * For P-521, the public_size is 132. first 66-byte is X, second 66-byte is Y.
 *
 * If ec_context is NULL, then return false.
 * If public_size is NULL, then return false.
 * If public_size is large enough but public is NULL, then return false.
 *
 * @param[in, out]  ec_context   Pointer to the EC context.
 * @param[out]      public       Pointer to the buffer to receive generated public X,Y.
 * @param[in, out]  public_size  On input, the size of public buffer in bytes.
 *                               On output, the size of data returned in public buffer in bytes.
 *
 * @retval true   EC public X,Y generation succeeded.
 * @retval false  EC public X,Y generation failed.
 * @retval false  public_size is not large enough.
 **/
extern bool libspdm_ec_generate_key(void *ec_context, uint8_t *public_key, size_t *public_key_size);

/**
 * Computes exchanged common key.
 *
 * Given peer's public key (X, Y), this function computes the exchanged common key,
 * based on its own context including value of curve parameter and random secret.
 * X is the first half of peer_public with size being peer_public_size / 2,
 * Y is the second half of peer_public with size being peer_public_size / 2.
 *
 * If ec_context is NULL, then return false.
 * If peer_public is NULL, then return false.
 * If peer_public_size is 0, then return false.
 * If key is NULL, then return false.
 * If key_size is not large enough, then return false.
 *
 * For P-256, the peer_public_size is 64. first 32-byte is X, second 32-byte is Y.
 * The key_size is 32.
 * For P-384, the peer_public_size is 96. first 48-byte is X, second 48-byte is Y.
 * The key_size is 48.
 * For P-521, the peer_public_size is 132. first 66-byte is X, second 66-byte is Y.
 * The key_size is 66.
 *
 * @param[in, out]  ec_context        Pointer to the EC context.
 * @param[in]       peer_public       Pointer to the peer's public X,Y.
 * @param[in]       peer_public_size  Size of peer's public X,Y in bytes.
 * @param[out]      key               Pointer to the buffer to receive generated key.
 * @param[in, out]  key_size          On input, the size of key buffer in bytes.
 *                                    On output, the size of data returned in key buffer in bytes.
 *
 * @retval true   EC exchanged key generation succeeded.
 * @retval false  EC exchanged key generation failed.
 * @retval false  key_size is not large enough.
 **/
extern bool libspdm_ec_compute_key(void *ec_context, const uint8_t *peer_public,
                                   size_t peer_public_size, uint8_t *key,
                                   size_t *key_size);
#endif /* LIBSPDM_ECDHE_SUPPORT */

#if LIBSPDM_ECDSA_SUPPORT
/**
 * Generates Elliptic Curve context from DER-encoded public key data.
 *
 * The public key is ASN.1 DER-encoded as RFC7250 describes,
 * namely, the SubjectPublicKeyInfo structure of a X.509 certificate.
 *
 * @param[in]  der_data    Pointer to the DER-encoded public key data.
 * @param[in]  der_size    Size of the DER-encoded public key data in bytes.
 * @param[out] ec_context  Pointer to newly generated EC context which contains the
 *                         EC public key component.
 *                         Use libspdm_ec_free() function to free the resource.
 *
 * If der_data is NULL, then return false.
 * If ec_context is NULL, then return false.
 *
 * @retval  true   EC context was generated successfully.
 * @retval  false  Invalid DER public key data.
 *
 **/
extern bool libspdm_ec_get_public_key_from_der(const uint8_t *der_data,
                                               size_t der_size,
                                               void **ec_context);

/**
 * Carries out the EC-DSA signature.
 *
 * This function carries out the EC-DSA signature.
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * If ec_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If hash_size need match the hash_nid. hash_nid could be SHA256, SHA384, SHA512, SHA3_256,
 * SHA3_384, SHA3_512.
 * If sig_size is large enough but signature is NULL, then return false.
 *
 * For P-256, the sig_size is 64. first 32-byte is R, second 32-byte is S.
 * For P-384, the sig_size is 96. first 48-byte is R, second 48-byte is S.
 * For P-521, the sig_size is 132. first 66-byte is R, second 66-byte is S.
 *
 * @param[in]       ec_context    Pointer to EC context for signature generation.
 * @param[in]       hash_nid      hash NID
 * @param[in]       message_hash  Pointer to octet message hash to be signed.
 * @param[in]       hash_size     Size of the message hash in bytes.
 * @param[out]      signature     Pointer to buffer to receive EC-DSA signature.
 * @param[in, out]  sig_size      On input, the size of signature buffer in bytes.
 *                                On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   signature successfully generated in EC-DSA.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 **/
extern bool libspdm_ecdsa_sign(void *ec_context, size_t hash_nid,
                               const uint8_t *message_hash, size_t hash_size,
                               uint8_t *signature, size_t *sig_size);

#if LIBSPDM_FIPS_MODE
/**
 * Carries out the EC-DSA signature with caller input random function. This API can be used for FIPS test.
 *
 * @param[in]       ec_context    Pointer to EC context for signature generation.
 * @param[in]       hash_nid      hash NID
 * @param[in]       message_hash  Pointer to octet message hash to be signed.
 * @param[in]       hash_size     Size of the message hash in bytes.
 * @param[out]      signature     Pointer to buffer to receive EC-DSA signature.
 * @param[in, out]  sig_size      On input, the size of signature buffer in bytes.
 *                                On output, the size of data returned in signature buffer in bytes.
 * @param[in]       random_func   random number function
 *
 * @retval  true   signature successfully generated in EC-DSA.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 **/
extern bool libspdm_ecdsa_sign_ex(void *ec_context, size_t hash_nid,
                                  const uint8_t *message_hash, size_t hash_size,
                                  uint8_t *signature, size_t *sig_size,
                                  int (*random_func)(void *, unsigned char *, size_t));
#endif/*LIBSPDM_FIPS_MODE*/

/**
 * Verifies the EC-DSA signature.
 *
 * If ec_context is NULL, then return false.
 * If message_hash is NULL, then return false.
 * If signature is NULL, then return false.
 * If hash_size need match the hash_nid. hash_nid could be SHA256, SHA384, SHA512, SHA3_256,
 * SHA3_384, SHA3_512.
 *
 * For P-256, the sig_size is 64. first 32-byte is R, second 32-byte is S.
 * For P-384, the sig_size is 96. first 48-byte is R, second 48-byte is S.
 * For P-521, the sig_size is 132. first 66-byte is R, second 66-byte is S.
 *
 * @param[in]  ec_context    Pointer to EC context for signature verification.
 * @param[in]  hash_nid      hash NID
 * @param[in]  message_hash  Pointer to octet message hash to be checked.
 * @param[in]  hash_size     Size of the message hash in bytes.
 * @param[in]  signature     Pointer to EC-DSA signature to be verified.
 * @param[in]  sig_size      Size of signature in bytes.
 *
 * @retval  true   Valid signature encoded in EC-DSA.
 * @retval  false  Invalid signature or invalid EC context.
 **/
extern bool libspdm_ecdsa_verify(void *ec_context, size_t hash_nid,
                                 const uint8_t *message_hash, size_t hash_size,
                                 const uint8_t *signature, size_t sig_size);
#endif /* LIBSPDM_ECDSA_SUPPORT */
#endif /* CRYPTLIB_EC_H */
