/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_CRYPT_LIB_H
#define SPDM_CRYPT_LIB_H

#include "internal/libspdm_lib_config.h"

#include "hal/base.h"
#include "industry_standard/spdm.h"

#if (LIBSPDM_FFDHE_4096_SUPPORT)
#define LIBSPDM_MAX_DHE_KEY_SIZE 512
#elif (LIBSPDM_FFDHE_3072_SUPPORT)
#define LIBSPDM_MAX_DHE_KEY_SIZE 384
#elif (LIBSPDM_FFDHE_2048_SUPPORT)
#define LIBSPDM_MAX_DHE_KEY_SIZE 256
#elif (LIBSPDM_ECDHE_P521_SUPPORT)
#define LIBSPDM_MAX_DHE_KEY_SIZE (66 * 2)
#elif (LIBSPDM_ECDHE_P384_SUPPORT)
#define LIBSPDM_MAX_DHE_KEY_SIZE (48 * 2)
#elif ((LIBSPDM_ECDHE_P256_SUPPORT) || (LIBSPDM_SM2_KEY_EXCHANGE_P256_SUPPORT))
#define LIBSPDM_MAX_DHE_KEY_SIZE (32 * 2)
#else
/* set 1 to pass build only */
#define LIBSPDM_MAX_DHE_KEY_SIZE 1
#endif /* LIBSPDM_MAX_DHE_KEY_SIZE */

#if ((LIBSPDM_RSA_SSA_4096_SUPPORT) || (LIBSPDM_RSA_PSS_4096_SUPPORT))
#define LIBSPDM_MAX_ASYM_KEY_SIZE 512
#define LIBSPDM_MAX_ASYM_SIG_SIZE 512
#elif ((LIBSPDM_RSA_SSA_3072_SUPPORT) || (LIBSPDM_RSA_PSS_3072_SUPPORT))
#define LIBSPDM_MAX_ASYM_KEY_SIZE 384
#define LIBSPDM_MAX_ASYM_SIG_SIZE 384
#elif ((LIBSPDM_RSA_SSA_2048_SUPPORT) || (LIBSPDM_RSA_PSS_2048_SUPPORT))
#define LIBSPDM_MAX_ASYM_KEY_SIZE 256
#define LIBSPDM_MAX_ASYM_SIG_SIZE 256
#elif (LIBSPDM_ECDSA_P521_SUPPORT)
#define LIBSPDM_MAX_ASYM_KEY_SIZE (66 * 2)
#define LIBSPDM_MAX_ASYM_SIG_SIZE (66 * 2)
#elif (LIBSPDM_EDDSA_ED448_SUPPORT)
#define LIBSPDM_MAX_ASYM_KEY_SIZE (57 * 2)
#define LIBSPDM_MAX_ASYM_SIG_SIZE (57 * 2)
#elif (LIBSPDM_ECDSA_P384_SUPPORT)
#define LIBSPDM_MAX_ASYM_KEY_SIZE (48 * 2)
#define LIBSPDM_MAX_ASYM_SIG_SIZE (48 * 2)
#elif ((LIBSPDM_ECDSA_P256_SUPPORT) || (LIBSPDM_SM2_DSA_P256_SUPPORT) || \
    (LIBSPDM_EDDSA_ED25519_SUPPORT))
#define LIBSPDM_MAX_ASYM_KEY_SIZE (32 * 2)
#define LIBSPDM_MAX_ASYM_SIG_SIZE (32 * 2)
#else
/* set 1 to pass build only */
#define LIBSPDM_MAX_ASYM_KEY_SIZE 1
#define LIBSPDM_MAX_ASYM_SIG_SIZE 1
#endif /* LIBSPDM_MAX_ASYM_KEY_SIZE */

#if ((LIBSPDM_SHA512_SUPPORT) || (LIBSPDM_SHA3_512_SUPPORT))
#define LIBSPDM_MAX_HASH_SIZE 64
#elif ((LIBSPDM_SHA384_SUPPORT) || (LIBSPDM_SHA3_384_SUPPORT))
#define LIBSPDM_MAX_HASH_SIZE 48
#elif ((LIBSPDM_SHA256_SUPPORT) || (LIBSPDM_SHA3_256_SUPPORT) || (LIBSPDM_SM3_256_SUPPORT))
#define LIBSPDM_MAX_HASH_SIZE 32
#endif /* LIBSPDM_MAX_HASH_SIZE */

#if ((LIBSPDM_AEAD_AES_256_GCM_SUPPORT) || (LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT))
#define LIBSPDM_MAX_AEAD_KEY_SIZE 32
#define LIBSPDM_MAX_AEAD_IV_SIZE 12
#define LIBSPDM_MAX_AEAD_TAG_SIZE 16
#elif ((LIBSPDM_AEAD_AES_128_GCM_SUPPORT) || (LIBSPDM_AEAD_SM4_128_GCM_SUPPORT))
#define LIBSPDM_MAX_AEAD_KEY_SIZE 16
#define LIBSPDM_MAX_AEAD_IV_SIZE 12
#define LIBSPDM_MAX_AEAD_TAG_SIZE 16
#else
/* set 1 to pass build only */
#define LIBSPDM_MAX_AEAD_KEY_SIZE 1
#define LIBSPDM_MAX_AEAD_IV_SIZE 1
#define LIBSPDM_MAX_AEAD_TAG_SIZE 1
#endif /* LIBSPDM_MAX_AEAD_KEY_SIZE */

/**
 * This function returns the SPDM hash algorithm size.
 *
 * @param  base_hash_algo SPDM base_hash_algo
 *
 * @return SPDM hash algorithm size.
 **/
uint32_t libspdm_get_hash_size(uint32_t base_hash_algo);

/**
 * Return cipher ID, based upon the negotiated hash algorithm.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 *
 * @return hash cipher ID
 **/
size_t libspdm_get_hash_nid(uint32_t base_hash_algo);

/**
 * Allocates and initializes one HASH_CTX context for subsequent hash use.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 *
 * @return  Pointer to the HASH_CTX context that has been initialized.
 *         If the allocations fails, libspdm_hash_new() returns NULL.
 **/
void *libspdm_hash_new(uint32_t base_hash_algo);

/**
 * Release the specified HASH_CTX context.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  hash_context    Pointer to the HASH_CTX context to be released.
 **/
void libspdm_hash_free(uint32_t base_hash_algo, void *hash_context);

/**
 * Initializes user-supplied memory pointed by hash_context as hash context for
 * subsequent use.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  hash_context    Pointer to hash context being initialized.
 *
 * @retval true   Hash context initialization succeeded.
 * @retval false  Hash context initialization failed.
 **/
bool libspdm_hash_init(uint32_t base_hash_algo, void *hash_context);

/**
 * Makes a copy of an existing hash context.
 *
 * If hash_ctx is NULL, then return false.
 * If new_hash_ctx is NULL, then return false.
 *
 * @param[in]  hash_ctx      Pointer to hash context being copied.
 * @param[out] new_hash_ctx  Pointer to new hash context.
 *
 * @retval true   Hash context copy succeeded.
 * @retval false  Hash context copy failed.
 **/
bool libspdm_hash_duplicate(uint32_t base_hash_algo, const void *hash_ctx, void *new_hash_ctx);

/**
 * Digests the input data and updates hash context.
 *
 * This function performs hash digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * Hash context should be already correctly initialized by hash_init(), and should not be finalized
 * by hash_final(). Behavior with invalid context is undefined.
 *
 * If hash_context is NULL, then return false.
 *
 * @param[in, out]  hash_context  Pointer to the MD context.
 * @param[in]       data          Pointer to the buffer containing the data to be hashed.
 * @param[in]       data_size     Size of data buffer in bytes.
 *
 * @retval true   Hash data digest succeeded.
 * @retval false  Hash data digest failed.
 **/
bool libspdm_hash_update(uint32_t base_hash_algo, void *hash_context,
                         const void *data, size_t data_size);

/**
 * Completes computation of the hash digest value.
 *
 * This function completes hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the hash context cannot
 * be used again.
 * hash context should be already correctly initialized by hash_init(), and should not be
 * finalized by hash_final(). Behavior with invalid hash context is undefined.
 *
 * If hash_context is NULL, then return false.
 * If hash_value is NULL, then return false.
 *
 * @param[in, out]  hash_context  Pointer to the hash context.
 * @param[out]      hash_value    Pointer to a buffer that receives the hash digest value.
 *
 * @retval true   Hash digest computation succeeded.
 * @retval false  Hash digest computation failed.
 **/
bool libspdm_hash_final(uint32_t base_hash_algo, void *hash_context, uint8_t *hash_value);

/**
 * Allocates and initializes one HMAC context for subsequent use.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 *
 * @return  Pointer to the HMAC context that has been initialized.
 *          If the allocations fails, libspdm_hash_new() returns NULL.
 **/
void *libspdm_hmac_new(uint32_t base_hash_algo);

/**
 * Release the specified HMAC context.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  hmac_ctx        Pointer to the HMAC context to be released.
 **/
void libspdm_hmac_free(uint32_t base_hash_algo, void *hmac_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to hmac_update().
 *
 * If hmac_ctx is NULL, then return false.
 *
 * @param[out]  hmac_ctx  Pointer to HMAC context.
 * @param[in]   key       Pointer to the user-supplied key.
 * @param[in]   key_size  Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 *
 **/
bool libspdm_hmac_init(uint32_t base_hash_algo,
                       void *hmac_ctx,
                       const uint8_t *key,
                       size_t key_size);

/**
 * Makes a copy of an existing HMAC context.
 *
 * If hmac_ctx is NULL, then return false.
 * If new_hmac_ctx is NULL, then return false.
 *
 * @param[in]  hmac_ctx      Pointer to HMAC context being copied.
 * @param[out] new_hmac_ctx  Pointer to new HMAC context.
 *
 * @retval true   HMAC context copy succeeded.
 * @retval false  HMAC context copy failed.
 *
 **/
bool libspdm_hmac_duplicate(uint32_t base_hash_algo, const void *hmac_ctx, void *new_hmac_ctx);

/**
 * Digests the input data and updates HMAC context.
 *
 * This function performs HMAC digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC context should be initialized by hmac_new(), and should not be finalized
 * by hmac_final(). Behavior with invalid context is undefined.
 *
 * If hmac_ctx is NULL, then return false.
 *
 * @param[in, out]  hmac_ctx   Pointer to the HMAC context.
 * @param[in]       data       Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size  Size of data buffer in bytes.
 *
 * @retval true   HMAC data digest succeeded.
 * @retval false  HMAC data digest failed.
 *
 **/
bool libspdm_hmac_update(uint32_t base_hash_algo,
                         void *hmac_ctx, const void *data,
                         size_t data_size);

/**
 * Completes computation of the HMAC digest value.
 *
 * This function completes HMAC hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC context cannot
 * be used again.
 *
 * If hmac_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 *
 * @param[in, out]  hmac_ctx    Pointer to the HMAC context.
 * @param[out]      hmac_value  Pointer to a buffer that receives the HMAC digest value.
 *
 * @retval true   HMAC digest computation succeeded.
 * @retval false  HMAC digest computation failed.
 *
 **/
bool libspdm_hmac_final(uint32_t base_hash_algo, void *hmac_ctx,  uint8_t *hmac_value);

/**
 * Computes the hash of a input data buffer, based upon the negotiated hash algorithm.
 *
 * This function performs the hash of a given data buffer, and return the hash value.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  data            Pointer to the buffer containing the data to be hashed.
 * @param  data_size       Size of data buffer in bytes.
 * @param  hash_value      Pointer to a buffer that receives the hash value.
 *
 * @retval true   Hash computation succeeded.
 * @retval false  Hash computation failed.
 **/
bool libspdm_hash_all(uint32_t base_hash_algo, const void *data,
                      size_t data_size, uint8_t *hash_value);

/**
 * This function returns the SPDM measurement hash algorithm size.
 *
 * @param  measurement_hash_algo  SPDM measurement_hash_algo.
 *
 * @return SPDM measurement hash algorithm size.
 * @return 0xFFFFFFFF for RAW_BIT_STREAM_ONLY.
 **/
uint32_t libspdm_get_measurement_hash_size(uint32_t measurement_hash_algo);

/**
 * Computes the HMAC of a input data buffer, based upon the negotiated HMAC algorithm.
 *
 * This function performs the HMAC of a given data buffer, and return the hash value.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  data            Pointer to the buffer containing the data to be HMACed.
 * @param  data_size       Size of data buffer in bytes.
 * @param  key             Pointer to the user-supplied key.
 * @param  key_size        Key size in bytes.
 * @param  hash_value      Pointer to a buffer that receives the HMAC value.
 *
 * @retval true   HMAC computation succeeded.
 * @retval false  HMAC computation failed.
 **/
bool libspdm_hmac_all(uint32_t base_hash_algo, const void *data,
                      size_t data_size, const uint8_t *key,
                      size_t key_size, uint8_t *hmac_value);

/**
 * Derive HMAC-based Extract key Derivation Function (HKDF) Extract, based upon the negotiated HKDF
 * algorithm.
 *
 * @param  ikm           Pointer to the input key material.
 * @param  ikm_size      Key size in bytes.
 * @param  salt          Pointer to the salt value.
 * @param  salt_size     Salt size in bytes.
 * @param  prk_out       Pointer to buffer to receive hkdf value.
 * @param  prk_out_size  Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
bool libspdm_hkdf_extract(uint32_t base_hash_algo, const uint8_t *ikm, size_t ikm_size,
                          const uint8_t *salt, size_t salt_size,
                          uint8_t *prk_out, size_t prk_out_size);

/**
 * Derive HMAC-based Expand key Derivation Function (HKDF) Expand, based upon the negotiated HKDF
 * algorithm.
 *
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  prk             Pointer to the user-supplied key.
 * @param  prk_size        Key size in bytes.
 * @param  info            Pointer to the application specific info.
 * @param  info_size       Info size in bytes.
 * @param  out             Pointer to buffer to receive hkdf value.
 * @param  out_size        Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
bool libspdm_hkdf_expand(uint32_t base_hash_algo, const uint8_t *prk,
                         size_t prk_size, const uint8_t *info,
                         size_t info_size, uint8_t *out, size_t out_size);

/**
 * This function returns the SPDM asymmetric algorithm size.
 *
 * @param  base_asym_algo  SPDM base_hash_algo
 *
 * @return SPDM asymmetric algorithm size.
 **/
uint32_t libspdm_get_asym_signature_size(uint32_t base_asym_algo);

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate,
 * based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  cert            Pointer to the DER-encoded X509 certificate.
 * @param  cert_size       Size of the X509 certificate in bytes.
 * @param  context         Pointer to newly generated asymmetric context which contain the retrieved
 *                         public key component. Use libspdm_asym_free() function to free the
 *                         resource.
 *
 * @retval  true   Public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
bool libspdm_asym_get_public_key_from_x509(uint32_t base_asym_algo,
                                           const uint8_t *cert,
                                           size_t cert_size,
                                           void **context);

/**
 * Retrieve the asymmetric public key from the DER-encoded public key data,
 * based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  der_data        Pointer to the DER-encoded public key data.
 * @param  der_size        Size of the DER-encoded public key data in bytes.
 * @param  context         Pointer to newly generated asymmetric context which contain the
 *                         retrieved public key component.
 *                         Use libspdm_asym_free() function to free the resource.
 *
 * @retval  true   Private key was retrieved successfully.
 * @retval  false  Invalid DER key data.
 **/
bool libspdm_asym_get_public_key_from_der(uint32_t base_asym_algo,
                                          const uint8_t *der_data,
                                          size_t der_size,
                                          void **context);

/**
 * Release the specified asymmetric context, based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  context         Pointer to the asymmetric context to be released.
 **/
void libspdm_asym_free(uint32_t base_asym_algo, void *context);


/**
 * Copies signature buffers from src to dst swapping endianness in the process.
 */
void libspdm_copy_signature_swap_endian(
    uint32_t base_asym_algo,
    uint8_t* dst,
    size_t dst_size,
    const uint8_t* src,
    size_t src_size);


/**
 * Verifies the asymmetric signature, based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  context         Pointer to asymmetric context for signature verification.
 * @param  message         Pointer to octet message to be checked (before hash).
 * @param  message_size    Size of the message in bytes.
 * @param  signature       Pointer to asymmetric signature to be verified.
 * @param  sig_size        Size of signature in bytes.
 * @param  endian          Endian to be tried. If both endians are selected,
 *                         the one actually used successfully is returned.
 *
 * @retval  true   Valid asymmetric signature.
 * @retval  false  Invalid asymmetric signature or invalid asymmetric context.
 **/
bool libspdm_asym_verify(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context,
    const uint8_t *message, size_t message_size,
    const uint8_t *signature, size_t sig_size);

bool libspdm_asym_verify_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void* context,
    const uint8_t* message, size_t message_size,
    const uint8_t* signature, size_t sig_size,
    uint8_t *endian);

/**
 * Verifies the asymmetric signature, based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  context         Pointer to asymmetric context for signature verification.
 * @param  message_hash    Pointer to octet message hash to be checked (after hash).
 * @param  hash_size       Size of the hash in bytes.
 * @param  signature       Pointer to asymmetric signature to be verified.
 * @param  sig_size        Size of signature in bytes.
 * @param  endian          Endian to be tried. If both endians are selected,
 *                         the one actually used successfully is returned.
 *
 * @retval  true   Valid asymmetric signature.
 * @retval  false  Invalid asymmetric signature or invalid asymmetric context.
 **/
bool libspdm_asym_verify_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo, void *context,
    const uint8_t *message_hash, size_t hash_size,
    const uint8_t *signature, size_t sig_size);

bool libspdm_asym_verify_hash_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo, void* context,
    const uint8_t* message_hash, size_t hash_size,
    const uint8_t* signature, size_t sig_size,
    uint8_t *endian);

/**
 * Carries out the signature generation.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  context         Pointer to asymmetric context for signature generation.
 * @param  message         Pointer to octet message to be signed (before hash).
 * @param  message_size    Size of the message in bytes.
 * @param  signature       Pointer to buffer to receive signature.
 * @param  sig_size        On input, the size of signature buffer in bytes.
 *                         On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   Signature successfully generated.
 * @retval  false  Signature generation failed.
 * @retval  false  sig_size is too small.
 **/
bool libspdm_asym_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context, const uint8_t *message,
    size_t message_size, uint8_t *signature,
    size_t *sig_size);

/**
 * Carries out the signature generation.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * @param  base_asym_algo  SPDM base_asym_algo
 * @param  base_hash_algo  SPDM base_hash_algo
 * @param  context         Pointer to asymmetric context for signature generation.
 * @param  message_hash    Pointer to octet message hash to be signed (after hash).
 * @param  hash_size       Size of the hash in bytes.
 * @param  signature       Pointer to buffer to receive signature.
 * @param  sig_size        On input, the size of signature buffer in bytes.
 *                         On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   Signature successfully generated.
 * @retval  false  Signature generation failed.
 * @retval  false  sig_size is too small.
 **/
bool libspdm_asym_sign_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context, const uint8_t *message_hash,
    size_t hash_size, uint8_t *signature,
    size_t *sig_size);

/**
 * This function returns the SPDM requester asymmetric algorithm size.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 *
 * @return SPDM requester asymmetric algorithm size.
 **/
uint32_t libspdm_get_req_asym_signature_size(uint16_t req_base_asym_alg);

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate,
 * based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  cert               Pointer to the DER-encoded X509 certificate.
 * @param  cert_size          Size of the X509 certificate in bytes.
 * @param  context            Pointer to newly generated asymmetric context which contain the
 *                            retrieved public key component. Use libspdm_asym_free() function to
 *                            free the resource.
 *
 * @retval  true   Public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
bool libspdm_req_asym_get_public_key_from_x509(uint16_t req_base_asym_alg,
                                               const uint8_t *cert,
                                               size_t cert_size,
                                               void **context);

/**
 * Retrieve the asymmetric public key from the DER-encoded public key data,
 * based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  der_data           Pointer to the DER-encoded public key data.
 * @param  der_size           Size of the DER-encoded public key data in bytes.
 * @param  context            Pointer to newly generated asymmetric context which contain the
 *                            retrieved public key component.
 *                            Use libspdm_req_asym_free() function to free the resource.
 *
 * @retval  true   Public key was retrieved successfully.
 * @retval  false  Invalid DER key data.
 **/
bool libspdm_req_asym_get_public_key_from_der(uint16_t req_base_asym_alg,
                                              const uint8_t *der_data,
                                              size_t der_size,
                                              void **context);

/**
 * Release the specified asymmetric context, based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  context            Pointer to the asymmetric context to be released.
 **/
void libspdm_req_asym_free(uint16_t req_base_asym_alg, void *context);

/**
 * Verifies the asymmetric signature, based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  base_hash_algo     SPDM base_hash_algo
 * @param  context            Pointer to asymmetric context for signature verification.
 * @param  message            Pointer to octet message to be checked (before hash).
 * @param  message_size       Size of the message in bytes.
 * @param  signature          Pointer to asymmetric signature to be verified.
 * @param  sig_size           Size of signature in bytes.
 * @param  endian             Endian to be tried. If both endians are selected,
 *                            the one actually used successfully is returned.
 *
 * @retval  true   Valid asymmetric signature.
 * @retval  false  Invalid asymmetric signature or invalid asymmetric context.
 **/
bool libspdm_req_asym_verify(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message, size_t message_size,
    const uint8_t *signature, size_t sig_size);

bool libspdm_req_asym_verify_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void* context,
    const uint8_t* message, size_t message_size,
    const uint8_t* signature, size_t sig_size, uint8_t *endian);

/**
 * Verifies the asymmetric signature, based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  base_hash_algo     SPDM base_hash_algo
 * @param  context            Pointer to asymmetric context for signature verification.
 * @param  message_hash       Pointer to octet message hash to be checked (after hash).
 * @param  hash_size          Size of the hash in bytes.
 * @param  signature          Pointer to asymmetric signature to be verified.
 * @param  sig_size           Size of signature in bytes.
 * @param  endian             Endian to be tried. If both endians are selected,
 *                            the one actually used successfully is returned.
 *
 * @retval  true   Valid asymmetric signature.
 * @retval  false  Invalid asymmetric signature or invalid asymmetric context.
 **/
bool libspdm_req_asym_verify_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message_hash, size_t hash_size,
    const uint8_t *signature, size_t sig_size);

bool libspdm_req_asym_verify_hash_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void* context,
    const uint8_t* message_hash, size_t hash_size,
    const uint8_t* signature, size_t sig_size, uint8_t *endian);

/**
 * Carries out the signature generation.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  base_hash_algo     SPDM base_hash_algo
 * @param  context            Pointer to asymmetric context for signature generation.
 * @param  message            Pointer to octet message to be signed (before hash).
 * @param  message_size       Size of the message in bytes.
 * @param  signature          Pointer to buffer to receive signature.
 * @param  sig_size           On input, the size of signature buffer in bytes.
 *                            On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   Signature successfully generated.
 * @retval  false  Signature generation failed.
 * @retval  false  sig_size is too small.
 **/
bool libspdm_req_asym_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size);

/**
 * Carries out the signature generation.
 *
 * If the signature buffer is too small to hold the contents of signature, false
 * is returned and sig_size is set to the required buffer size to obtain the signature.
 *
 * @param  req_base_asym_alg  SPDM req_base_asym_alg
 * @param  base_hash_algo     SPDM base_hash_algo
 * @param  context            Pointer to asymmetric context for signature generation.
 * @param  message_hash       Pointer to octet message hash to be signed (after hash).
 * @param  hash_size          Size of the hash in bytes.
 * @param  signature          Pointer to buffer to receive signature.
 * @param  sig_size           On input, the size of signature buffer in bytes.
 *                            On output, the size of data returned in signature buffer in bytes.
 *
 * @retval  true   Signature successfully generated.
 * @retval  false  Signature generation failed.
 * @retval  false  sig_size is too small.
 **/
bool libspdm_req_asym_sign_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message_hash, size_t hash_size,
    uint8_t *signature, size_t *sig_size);

/**
 * This function returns the SPDM DHE algorithm key size.
 *
 * @param  dhe_named_group  SPDM dhe_named_group
 *
 * @return SPDM DHE algorithm key size.
 **/
uint32_t libspdm_get_dhe_pub_key_size(uint16_t dhe_named_group);

/**
 * Allocates and Initializes one Diffie-Hellman Ephemeral (DHE) context for subsequent use,
 * based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group  SPDM dhe_named_group
 * @param  is_initiator     If the caller is initiator.
 *
 * @return  Pointer to the Diffie-Hellman context that has been initialized.
 **/
void *libspdm_dhe_new(spdm_version_number_t spdm_version,
                      uint16_t dhe_named_group, bool is_initiator);

/**
 * Release the specified DHE context, based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group  SPDM dhe_named_group
 * @param  context          Pointer to the DHE context to be released.
 **/
void libspdm_dhe_free(uint16_t dhe_named_group, void *context);

/**
 * Generates DHE public key, based upon negotiated DHE algorithm.
 *
 * This function generates random secret exponent, and computes the public key, which is
 * returned via parameter public_key and public_key_size. DH context is updated accordingly.
 * If the public_key buffer is too small to hold the public key, false is returned and
 * public_key_size is set to the required buffer size to obtain the public key.
 *
 * @param  dhe_named_group   SPDM dhe_named_group
 * @param  context           Pointer to the DHE context.
 * @param  public_key        Pointer to the buffer to receive generated public key.
 * @param  public_key_size   On input, the size of public_key buffer in bytes.
 *                           On output, the size of data returned in public_key buffer in bytes.
 *
 * @retval true   DHE public key generation succeeded.
 * @retval false  DHE public key generation failed.
 * @retval false  public_key_size is not large enough.
 **/
bool libspdm_dhe_generate_key(uint16_t dhe_named_group, void *context,
                              uint8_t *public_key,
                              size_t *public_key_size);

/**
 * Computes exchanged common key, based upon negotiated DHE algorithm.
 *
 * Given peer's public key, this function computes the exchanged common key, based on its own
 * context including value of prime modulus and random secret exponent.
 *
 * @param  dhe_named_group       SPDM dhe_named_group
 * @param  context               Pointer to the DHE context.
 * @param  peer_public_key       Pointer to the peer's public key.
 * @param  peer_public_key_size  Size of peer's public key in bytes.
 * @param  key                   Pointer to the buffer to receive generated key.
 * @param  key_size              On input, the size of key buffer in bytes.
 *                               On output, the size of data returned in key buffer in bytes.
 *
 * @retval true   DHE exchanged key generation succeeded.
 * @retval false  DHE exchanged key generation failed.
 * @retval false  key_size is not large enough.
 **/
bool libspdm_dhe_compute_key(uint16_t dhe_named_group, void *context,
                             const uint8_t *peer_public,
                             size_t peer_public_size, uint8_t *key,
                             size_t *key_size);

/**
 * This function returns the SPDM AEAD algorithm key size.
 *
 * @param  aead_cipher_suite  SPDM aead_cipher_suite
 *
 * @return SPDM AEAD algorithm key size.
 **/
uint32_t libspdm_get_aead_key_size(uint16_t aead_cipher_suite);

/**
 * This function returns the SPDM AEAD algorithm iv size.
 *
 * @param  aead_cipher_suite  SPDM aead_cipher_suite
 *
 * @return SPDM AEAD algorithm iv size.
 **/
uint32_t libspdm_get_aead_iv_size(uint16_t aead_cipher_suite);

/**
 * This function returns the SPDM AEAD algorithm tag size.
 *
 * @param  aead_cipher_suite  SPDM aead_cipher_suite
 *
 * @return SPDM AEAD algorithm tag size.
 **/
uint32_t libspdm_get_aead_tag_size(uint16_t aead_cipher_suite);

/**
 * Performs AEAD authenticated encryption on a data buffer and additional authenticated data (AAD),
 * based upon negotiated AEAD algorithm.
 *
 * @param  aead_cipher_suite  SPDM aead_cipher_suite
 * @param  key                Pointer to the encryption key.
 * @param  key_size           Size of the encryption key in bytes.
 * @param  iv                 Pointer to the IV value.
 * @param  iv_size            Size of the IV value in bytes.
 * @param  a_data             Pointer to the additional authenticated data (AAD).
 * @param  a_data_size        Size of the additional authenticated data (AAD) in bytes.
 * @param  data_in            Pointer to the input data buffer to be encrypted.
 * @param  data_in_size       Size of the input data buffer in bytes.
 * @param  tag_out            Pointer to a buffer that receives the authentication tag output.
 * @param  tag_size           Size of the authentication tag in bytes.
 * @param  data_out           Pointer to a buffer that receives the encryption output.
 * @param  data_out_size      Size of the output data buffer in bytes.
 *
 * @retval true   AEAD authenticated encryption succeeded.
 * @retval false  AEAD authenticated encryption failed.
 **/
bool libspdm_aead_encryption(const spdm_version_number_t secured_message_version,
                             uint16_t aead_cipher_suite, const uint8_t *key,
                             size_t key_size, const uint8_t *iv,
                             size_t iv_size, const uint8_t *a_data,
                             size_t a_data_size, const uint8_t *data_in,
                             size_t data_in_size, uint8_t *tag_out,
                             size_t tag_size, uint8_t *data_out,
                             size_t *data_out_size);

/**
 * Performs AEAD authenticated decryption on a data buffer and additional authenticated data (AAD),
 * based upon negotiated AEAD algorithm.
 *
 * @param  aead_cipher_suite  SPDM aead_cipher_suite
 * @param  key                Pointer to the encryption key.
 * @param  key_size           Size of the encryption key in bytes.
 * @param  iv                 Pointer to the IV value.
 * @param  iv_size            Size of the IV value in bytes.
 * @param  a_data             Pointer to the additional authenticated data (AAD).
 * @param  a_data_size        Size of the additional authenticated data (AAD) in bytes.
 * @param  data_in            Pointer to the input data buffer to be decrypted.
 * @param  data_in_size       Size of the input data buffer in bytes.
 * @param  tag                Pointer to a buffer that contains the authentication tag.
 * @param  tag_size           Size of the authentication tag in bytes.
 * @param  data_out           Pointer to a buffer that receives the decryption output.
 * @param  data_out_size      Size of the output data buffer in bytes.
 *
 * @retval true   AEAD authenticated decryption succeeded.
 * @retval false  AEAD authenticated decryption failed.
 **/
bool libspdm_aead_decryption(const spdm_version_number_t secured_message_version,
                             uint16_t aead_cipher_suite, const uint8_t *key,
                             size_t key_size, const uint8_t *iv,
                             size_t iv_size, const uint8_t *a_data,
                             size_t a_data_size, const uint8_t *data_in,
                             size_t data_in_size, const uint8_t *tag,
                             size_t tag_size, uint8_t *data_out,
                             size_t *data_out_size);

/**
 * Generates a random byte stream of the specified size.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  size          Size of random bytes to generate.
 * @param  rand          Pointer to buffer to receive random value.
 **/
bool libspdm_get_random_number(size_t size, uint8_t *rand);

#if LIBSPDM_CERT_PARSE_SUPPORT

/**
 * Certificate Check for SPDM leaf cert.
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 * @param[in]  base_hash_algo        SPDM base_hash_algo
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 * @param[in]  is_device_cert_model  If true, the local endpoint uses the DeviceCert model.
 *                                   If false, the local endpoint uses the AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Certificate is not valid.
 **/
bool libspdm_x509_certificate_check(const uint8_t *cert, size_t cert_size,
                                    uint32_t base_asym_algo, uint32_t base_hash_algo,
                                    bool is_requester_cert, bool is_device_cert_model);

/**
 * Certificate Check for SPDM leaf cert when set_cert.
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 * @param[in]  base_hash_algo        SPDM base_hash_algo
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 * @param[in]  is_device_cert_model  If true, the local endpoint uses the DeviceCert model.
 *                                   If false, the local endpoint uses the AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Certificate is not valid.
 **/
bool libspdm_x509_set_cert_certificate_check(const uint8_t *cert, size_t cert_size,
                                             uint32_t base_asym_algo, uint32_t base_hash_algo,
                                             bool is_requester_cert, bool is_device_cert_model);

/**
 * Return certificate is root cert or not.
 * Certificate is considered as a root certificate if the subjectname equal issuername.
 *
 * @param[in]  cert       Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size  The size of certificate data in bytes.
 *
 * @retval  true   Certificate is self-signed.
 * @retval  false  Certificate is not self-signed.
 **/
bool libspdm_is_root_certificate(const uint8_t *cert, size_t cert_size);

/**
 * Retrieve the SubjectAltName from SubjectAltName Bytes.
 *
 * @param[in]      buffer            Pointer to subjectAltName oct bytes.
 * @param[in]      len               Size of buffer in bytes.
 * @param[out]     name_buffer       Buffer to contain the retrieved certificate
 *                                   SubjectAltName. At most name_buffer_size bytes will be
 *                                   written. Maybe NULL in order to determine the size
 *                                   buffer needed.
 * @param[in,out]  name_buffer_size  The size in bytes of the name buffer on input,
 *                                   and the size of buffer returned name on output.
 *                                   If name_buffer is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 * @param[out]     oid               OID of otherName
 * @param[in,out]  oid_size          The buffersize for required OID
 *
 * @retval true    Get the subjectAltName string successfully
 * @retval failed  Get the subjectAltName string failed
 **/
bool libspdm_get_dmtf_subject_alt_name_from_bytes(
    uint8_t *buffer, const size_t len, char *name_buffer,
    size_t *name_buffer_size, uint8_t *oid,
    size_t *oid_size);

/**
 * Retrieve the SubjectAltName from one X.509 certificate.
 *
 * @param[in]      cert              Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         Size of the X509 certificate in bytes.
 * @param[out]     name_buffer       Buffer to contain the retrieved certificate
 *                                   SubjectAltName. At most name_buffer_size bytes will be
 *                                   written. Maybe NULL in order to determine the size
 *                                   buffer needed.
 * @param[in,out]  name_buffer_size  The size in bytes of the name buffer on input,
 *                                   and the size of buffer returned name on output.
 *                                   If name_buffer is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 * @param[out]     oid               OID of otherName
 * @param[in,out]  oid_size          The buffersize for required OID
 *
 * @retval true    Get the subjectAltName string successfully
 * @retval failed  Get the subjectAltName string failed
 **/
bool libspdm_get_dmtf_subject_alt_name(const uint8_t *cert, const size_t cert_size,
                                       char *name_buffer,
                                       size_t *name_buffer_size,
                                       uint8_t *oid, size_t *oid_size);

/**
 * This function verifies the integrity of certificate chain data without spdm_cert_chain_t header.
 *
 * @param  cert_chain_data       The certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size  Size in bytes of the certificate chain data.
 * @param  base_asym_algo        SPDM base_asym_algo
 * @param  base_hash_algo        SPDM base_hash_algo
 * @param  is_requester_cert     Is the function verifying requester or responder cert.
 * @param  is_device_cert_model  If true, the cert chain is DeviceCert model.
 *                               If false, the cert chain is AliasCert model.
 *
 * @retval true  Certificate chain data integrity verification pass.
 * @retval false Certificate chain data integrity verification fail.
 **/
bool libspdm_verify_cert_chain_data(uint8_t *cert_chain_data, size_t cert_chain_data_size,
                                    uint32_t base_asym_algo, uint32_t base_hash_algo,
                                    bool is_requester_cert, bool is_device_cert_model);

/**
 * This function verifies the integrity of certificate chain buffer including
 * spdm_cert_chain_t header.
 *
 * @param  base_hash_algo          SPDM base_hash_algo
 * @param  base_asym_algo          SPDM base_asym_algo
 * @param  cert_chain_buffer       The certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size  Size in bytes of the certificate chain buffer.
 * @param  is_requester_cert       Is the function verifying requester or responder cert.
 * @param  is_device_cert_model    If true, the cert chain is DeviceCert model.
 *                                 If false, the cert chain is AliasCert model.
 *
 * @retval true   Certificate chain buffer integrity verification pass.
 * @retval false  Certificate chain buffer integrity verification fail.
 **/
bool libspdm_verify_certificate_chain_buffer(uint32_t base_hash_algo, uint32_t base_asym_algo,
                                             const void *cert_chain_buffer,
                                             size_t cert_chain_buffer_size,
                                             bool is_requester_cert, bool is_device_cert_model);

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate,
 * based upon negotiated asymmetric or requester asymmetric algorithm.
 *
 *
 * @param  base_hash_algo        SPDM base_hash_algo.
 * @param  base_asym_alg         SPDM base_asym_algo or req_base_asym_alg.
 * @param  cert_chain_data       Certificate chain data with spdm_cert_chain_t header.
 * @param  cert_chain_data_size  Size in bytes of the certificate chain data.
 * @param  public_key            Pointer to newly generated asymmetric context which contain the
 *                               retrieved public key component.
 *
 * @retval  true   Public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
bool libspdm_get_leaf_cert_public_key_from_cert_chain(uint32_t base_hash_algo,
                                                      uint32_t base_asym_alg,
                                                      uint8_t *cert_chain_data,
                                                      size_t cert_chain_data_size,
                                                      void **public_key);

#endif

/**
 * Verify req info format refer to PKCS#10.
 *
 * @param[in]      req_info      Requester info to gen CSR.
 * @param[in]      req_info_len  The len of requester info.
 *
 * @retval  true    Valid req info, have three situations:
 *                                  1: no req_info
 *                                  2: good format req_info without attributes
 *                                  3: good format req_info with good format attributes
 * @retval  false   Invalid req info.
 **/
bool libspdm_verify_req_info(uint8_t *req_info, uint16_t req_info_len);

#if LIBSPDM_FIPS_MODE
/*run all of the self-tests and returns the results.*/
bool libspdm_fips_run_selftest(void *fips_selftest_context);
#endif

#endif /* SPDM_CRYPT_LIB_H */
