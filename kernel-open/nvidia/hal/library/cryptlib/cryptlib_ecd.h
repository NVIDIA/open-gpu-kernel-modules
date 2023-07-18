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
 * Release the specified Ed context.
 *
 * @param[in]  ecd_context  Pointer to the Ed context to be released.
 **/
extern void libspdm_ecd_free(void *ecd_context);

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
