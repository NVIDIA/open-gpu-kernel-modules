/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_ECD_H
#define CRYPTLIB_ECD_H

/*=====================================================================================
 *    Edwards-Curve Primitives
 *=====================================================================================*/

#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
/**
 * Allocates and Initializes one Edwards-Curve context for subsequent use with the NID.
 *
 * @param nid cipher NID
 *
 * @return  Pointer to the Edwards-Curve context that has been initialized.
 *          If the allocations fails, libspdm_ecd_new_by_nid() returns NULL.
 **/
extern void *libspdm_ecd_new_by_nid(size_t nid);

/**
 * Generates Edwards-Curve context from DER-encoded public key data.
 *
 * The public key is ASN.1 DER-encoded as RFC7250 describes,
 * namely, the SubjectPublicKeyInfo structure of a X.509 certificate.
 *
 * @param[in]  der_data    Pointer to the DER-encoded public key data.
 * @param[in]  der_size    Size of the DER-encoded public key data in bytes.
 * @param[out] ec_context  Pointer to newly generated Ed context which contains the
 *                         Ed public key component.
 *                         Use libspdm_ecd_free() function to free the resource.
 *
 * If der_data is NULL, then return false.
 * If ecd_context is NULL, then return false.
 *
 * @retval  true   Ed context was generated successfully.
 * @retval  false  Invalid DER public key data.
 *
 **/
extern bool libspdm_ecd_get_public_key_from_der(const uint8_t *der_data,
                                                size_t der_size,
                                                void **ecd_context);

/**
 * Release the specified Ed context.
 *
 * @param[in]  ecd_context  Pointer to the Ed context to be released.
 **/
extern void libspdm_ecd_free(void *ecd_context);

/**
 * Sets the public key component into the established Ed context.
 *
 * For ed25519, the public_size is 32.
 * For ed448, the public_size is 57.
 *
 * @param[in, out]  ecd_context    Pointer to Ed context being set.
 * @param[in]       public_key     Pointer to the buffer to receive generated public X,Y.
 * @param[in]       public_size    The size of public buffer in bytes.
 *
 * @retval  true   Ed public key component was set successfully.
 * @retval  false  Invalid EC public key component.
 **/
extern bool libspdm_ecd_set_pub_key(void *ecd_context, const uint8_t *public_key,
                                    size_t public_key_size);

/**
 * Sets the private key component into the established Ed context.
 *
 * For ed25519, the private_size is 32.
 * For ed448, the private_size is 57.
 *
 * @param[in, out]  ecd_context      Pointer to Ed context being set.
 * @param[in]       private         Pointer to the buffer to receive generated private X,Y.
 * @param[in]       private_size     The size of private buffer in bytes.
 *
 * @retval  true   Ed private key component was set successfully.
 * @retval  false  Invalid EC private key component.
 *
 **/
bool libspdm_ecd_set_pri_key(void *ecd_context, const uint8_t *private_key,
                             size_t private_key_size);

/**
 * Gets the public key component from the established Ed context.
 *
 * For ed25519, the public_size is 32.
 * For ed448, the public_size is 57.
 *
 * @param[in, out]  ecd_context    Pointer to Ed context being set.
 * @param[out]      public         Pointer to the buffer to receive generated public X,Y.
 * @param[in, out]  public_size    On input, the size of public buffer in bytes.
 *                                 On output, the size of data returned in public buffer in bytes.
 *
 * @retval  true   Ed key component was retrieved successfully.
 * @retval  false  Invalid EC public key component.
 **/
extern bool libspdm_ecd_get_pub_key(void *ecd_context, uint8_t *public_key,
                                    size_t *public_key_size);

/**
 * Carries out the Ed-DSA signature.
 *
 * This function carries out the Ed-DSA signature.
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * If ecd_context is NULL, then return false.
 * If message is NULL, then return false.
 * hash_nid must be NULL.
 * If sig_size is large enough but signature is NULL, then return false.
 *
 * For ed25519, context must be NULL and context_size must be 0.
 * For ed448, context must be maximum of 255 octets.
 *
 * For ed25519, the sig_size is 64. first 32-byte is R, second 32-byte is S.
 * For ed448, the sig_size is 114. first 57-byte is R, second 57-byte is S.
 *
 * @param[in]       ecd_context   Pointer to Ed context for signature generation.
 * @param[in]       hash_nid      hash NID
 * @param[in]       context       The EDDSA signing context.
 * @param[in]       context_size  Size of EDDSA signing context.
 * @param[in]       message       Pointer to octet message to be signed (before hash).
 * @param[in]       size          size of the message in bytes.
 * @param[out]      signature     Pointer to buffer to receive Ed-DSA signature.
 * @param[in, out]  sig_size      On input, the size of signature buffer in bytes.
 *                                On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   signature successfully generated in Ed-DSA.
 * @retval  false  signature generation failed.
 * @retval  false  sig_size is too small.
 **/
extern bool libspdm_eddsa_sign(const void *ecd_context, size_t hash_nid,
                               const uint8_t *context, size_t context_size,
                               const uint8_t *message, size_t size, uint8_t *signature,
                               size_t *sig_size);

/**
 * Verifies the Ed-DSA signature.
 *
 * If ecd_context is NULL, then return false.
 * If message is NULL, then return false.
 * If signature is NULL, then return false.
 * hash_nid must be NULL.
 *
 * For ed25519, context must be NULL and context_size must be 0.
 * For ed448, context must be maximum of 255 octets.
 *
 * For ed25519, the sig_size is 64. first 32-byte is R, second 32-byte is S.
 * For ed448, the sig_size is 114. first 57-byte is R, second 57-byte is S.
 *
 * @param[in]  ecd_context   Pointer to Ed context for signature verification.
 * @param[in]  hash_nid      hash NID
 * @param[in]  context       The EDDSA signing context.
 * @param[in]  context_size  Size of EDDSA signing context.
 * @param[in]  message       Pointer to octet message to be checked (before hash).
 * @param[in]  size          Size of the message in bytes.
 * @param[in]  signature     Pointer to Ed-DSA signature to be verified.
 * @param[in]  sig_size      Size of signature in bytes.
 *
 * @retval  true   Valid signature encoded in Ed-DSA.
 * @retval  false  Invalid signature or invalid Ed context.
 **/
extern bool libspdm_eddsa_verify(const void *ecd_context, size_t hash_nid,
                                 const uint8_t *context, size_t context_size,
                                 const uint8_t *message, size_t size,
                                 const uint8_t *signature, size_t sig_size);
#endif /* (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT) */
#endif /* CRYPTLIB_ECD_H */
