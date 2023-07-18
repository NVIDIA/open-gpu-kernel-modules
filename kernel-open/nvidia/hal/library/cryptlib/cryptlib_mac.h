/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_MAC_H
#define CRYPTLIB_MAC_H

/*=====================================================================================
 *    Message Authentication Code (MAC) Primitives
 *=====================================================================================
 */

#if LIBSPDM_SHA256_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA256 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha256_new() returns NULL.
 **/
extern void *libspdm_hmac_sha256_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha256_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha256_free(void *hmac_sha256_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha256_update().
 *
 * If hmac_sha256_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  hmac_sha256_ctx  Pointer to HMAC-SHA256 context.
 * @param[in]   key              Pointer to the user-supplied key.
 * @param[in]   key_size         Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha256_set_key(void *hmac_sha256_ctx, const uint8_t *key, size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA256 context.
 *
 * If hmac_sha256_ctx is NULL, then return false.
 * If new_hmac_sha256_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  hmac_sha256_ctx      Pointer to HMAC-SHA256 context being copied.
 * @param[out] new_hmac_sha256_ctx  Pointer to new HMAC-SHA256 context.
 *
 * @retval true   HMAC-SHA256 context copy succeeded.
 * @retval false  HMAC-SHA256 context copy failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha256_duplicate(const void *hmac_sha256_ctx, void *new_hmac_sha256_ctx);

/**
 * Digests the input data and updates HMAC-SHA256 context.
 *
 * This function performs HMAC-SHA256 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA256 context should be initialized by libspdm_hmac_sha256_new(), and should not be
 * finalized by libspdm_hmac_sha256_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha256_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha256_ctx Pointer to the HMAC-SHA256 context.
 * @param[in]       data            Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size       Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA256 data digest succeeded.
 * @retval false  HMAC-SHA256 data digest failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha256_update(void *hmac_sha256_ctx, const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SHA256 digest value.
 *
 * This function completes HMAC-SHA256 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA256 context cannot
 * be used again. HMAC-SHA256 context should be initialized by libspdm_hmac_sha256_new(), and should
 * not be finalized by libspdm_hmac_sha256_final(). Behavior with invalid HMAC-SHA256 context is
 * undefined.
 *
 * If hmac_sha256_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha256_ctx  Pointer to the HMAC-SHA256 context.
 * @param[out]      hmac_value       Pointer to a buffer that receives the HMAC-SHA256 digest
 *                                   value (32 bytes).
 *
 * @retval true   HMAC-SHA256 digest computation succeeded.
 * @retval false  HMAC-SHA256 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha256_final(void *hmac_sha256_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA256 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA256 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA256 digest
 *                          value (32 bytes).
 *
 * @retval true   HMAC-SHA256 digest computation succeeded.
 * @retval false  HMAC-SHA256 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha256_all(const void *data, size_t data_size,
                                    const uint8_t *key, size_t key_size,
                                    uint8_t *hmac_value);
#endif /* LIBSPDM_SHA256_SUPPORT */

#if LIBSPDM_SHA384_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA384 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha384_new() returns NULL.
 **/
extern void *libspdm_hmac_sha384_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha384_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha384_free(void *hmac_sha384_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha384_update().
 *
 * If hmac_sha384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  hmac_sha384_ctx  Pointer to HMAC-SHA384 context.
 * @param[in]   key                Pointer to the user-supplied key.
 * @param[in]   key_size            key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha384_set_key(void *hmac_sha384_ctx, const uint8_t *key, size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA384 context.
 *
 * If hmac_sha384_ctx is NULL, then return false.
 * If new_hmac_sha384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  hmac_sha384_ctx      Pointer to HMAC-SHA384 context being copied.
 * @param[out] new_hmac_sha384_ctx  Pointer to new HMAC-SHA384 context.
 *
 * @retval true   HMAC-SHA384 context copy succeeded.
 * @retval false  HMAC-SHA384 context copy failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha384_duplicate(const void *hmac_sha384_ctx, void *new_hmac_sha384_ctx);

/**
 * Digests the input data and updates HMAC-SHA384 context.
 *
 * This function performs HMAC-SHA384 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA384 context should be initialized by libspdm_hmac_sha384_new(), and should not be
 * finalized by libspdm_hmac_sha384_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha384_ctx  Pointer to the HMAC-SHA384 context.
 * @param[in]       data             Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size        Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA384 data digest succeeded.
 * @retval false  HMAC-SHA384 data digest failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha384_update(void *hmac_sha384_ctx, const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SHA384 digest value.
 *
 * This function completes HMAC-SHA384 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA384 context cannot
 * be used again. HMAC-SHA384 context should be initialized by libspdm_hmac_sha384_new(), and should
 * not be finalized by libspdm_hmac_sha384_final(). Behavior with invalid HMAC-SHA384 context is
 * undefined.
 *
 * If hmac_sha384_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha384_ctx  Pointer to the HMAC-SHA384 context.
 * @param[out]      hmac_value       Pointer to a buffer that receives the HMAC-SHA384 digest
 *                                   value (48 bytes).
 *
 * @retval true   HMAC-SHA384 digest computation succeeded.
 * @retval false  HMAC-SHA384 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha384_final(void *hmac_sha384_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA384 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA384 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA384 digest
 *                          value (48 bytes).
 *
 * @retval true   HMAC-SHA384 digest computation succeeded.
 * @retval false  HMAC-SHA384 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha384_all(const void *data, size_t data_size,
                                    const uint8_t *key, size_t key_size,
                                    uint8_t *hmac_value);
#endif /* LIBSPDM_SHA384_SUPPORT */

#if LIBSPDM_SHA512_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA512 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha512_new() returns NULL.
 **/
extern void *libspdm_hmac_sha512_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha512_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha512_free(void *hmac_sha512_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha512_update().
 *
 * If hmac_sha512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  hmac_sha512_ctx  Pointer to HMAC-SHA512 context.
 * @param[in]   key              Pointer to the user-supplied key.
 * @param[in]   key_size         Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha512_set_key(void *hmac_sha512_ctx, const uint8_t *key, size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA512 context.
 *
 * If hmac_sha512_ctx is NULL, then return false.
 * If new_hmac_sha512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  hmac_sha512_ctx      Pointer to HMAC-SHA512 context being copied.
 * @param[out] new_hmac_sha512_ctx  Pointer to new HMAC-SHA512 context.
 *
 * @retval true   HMAC-SHA512 context copy succeeded.
 * @retval false  HMAC-SHA512 context copy failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha512_duplicate(const void *hmac_sha512_ctx, void *new_hmac_sha512_ctx);

/**
 * Digests the input data and updates HMAC-SHA512 context.
 *
 * This function performs HMAC-SHA512 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA512 context should be initialized by libspdm_hmac_sha512_new(), and should not be
 * finalized by libspdm_hmac_sha512_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha512_ctx  Pointer to the HMAC-SHA512 context.
 * @param[in]       data             Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size        Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA512 data digest succeeded.
 * @retval false  HMAC-SHA512 data digest failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha512_update(void *hmac_sha512_ctx, const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SHA512 digest value.
 *
 * This function completes HMAC-SHA512 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA512 context cannot
 * be used again. HMAC-SHA512 context should be initialized by libspdm_hmac_sha512_new(), and should
 * not be finalized by libspdm_hmac_sha512_final(). Behavior with invalid HMAC-SHA512 context is
 * undefined.
 *
 * If hmac_sha512_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha512_ctx  Pointer to the HMAC-SHA512 context.
 * @param[out]      hmac_value       Pointer to a buffer that receives the HMAC-SHA512 digest
 *                                   value (64 bytes).
 *
 * @retval true   HMAC-SHA512 digest computation succeeded.
 * @retval false  HMAC-SHA512 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha512_final(void *hmac_sha512_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA512 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA512 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA512 digest
 *                          value (64 bytes).
 *
 * @retval true   HMAC-SHA512 digest computation succeeded.
 * @retval false  HMAC-SHA512 digest computation failed.
 * @retval false  This interface is not supported.
 *
 **/
extern bool libspdm_hmac_sha512_all(const void *data, size_t data_size,
                                    const uint8_t *key, size_t key_size,
                                    uint8_t *hmac_value);
#endif /* LIBSPDM_SHA512_SUPPORT */

#if LIBSPDM_SHA3_256_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA3-256 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha3_256_new() returns NULL.
 **/
extern void *libspdm_hmac_sha3_256_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha3_256_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha3_256_free(void *hmac_sha3_256_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha3_256_update().
 *
 * If hmac_sha3_256_ctx is NULL, then return false.
 *
 * @param[out]  hmac_sha3_256_ctx  Pointer to HMAC-SHA3-256 context.
 * @param[in]   key                Pointer to the user-supplied key.
 * @param[in]   key_size           Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 **/
extern bool libspdm_hmac_sha3_256_set_key(void *hmac_sha3_256_ctx,
                                          const uint8_t *key,
                                          size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA3-256 context.
 *
 * If hmac_sha3_256_ctx is NULL, then return false.
 * If new_hmac_sha3_256_ctx is NULL, then return false.
 *
 * @param[in]  hmac_sha3_256_ctx      Pointer to HMAC-SHA3-256 context being copied.
 * @param[out] new_hmac_sha3_256_ctx  Pointer to new HMAC-SHA3-256 context.
 *
 * @retval true   HMAC-SHA3-256 context copy succeeded.
 * @retval false  HMAC-SHA3-256 context copy failed.
 **/
extern bool libspdm_hmac_sha3_256_duplicate(const void *hmac_sha3_256_ctx,
                                            void *new_hmac_sha3_256_ctx);

/**
 * Digests the input data and updates HMAC-SHA3-256 context.
 *
 * This function performs HMAC-SHA3-256 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA3-256 context should be initialized by libspdm_hmac_sha3_256_new(), and should not be
 * finalized by libspdm_hmac_sha3_256_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha3_256_ctx is NULL, then return false.
 *
 * @param[in, out]  hmac_sha3_256_ctx  Pointer to the HMAC-SHA3-256 context.
 * @param[in]       data               Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size          Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA3-256 data digest succeeded.
 * @retval false  HMAC-SHA3-256 data digest failed.
 **/
extern bool libspdm_hmac_sha3_256_update(void *hmac_sha3_256_ctx,
                                         const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SHA3-256 digest value.
 *
 * This function completes HMAC-SHA3-256 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA3-256 context cannot
 * be used again. HMAC-SHA3-256 context should be initialized by libspdm_hmac_sha3_256_new(), and
 * should not be finalized by libspdm_hmac_sha3_256_final(). Behavior with invalid HMAC-SHA3-256
 * context is undefined.
 *
 * If hmac_sha3_256_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 *
 * @param[in, out]  hmac_sha3_256_ctx  Pointer to the HMAC-SHA3-256 context.
 * @param[out]      hmac_value          Pointer to a buffer that receives the HMAC-SHA3-256 digest
 *                                    value (32 bytes).
 *
 * @retval true   HMAC-SHA3-256 digest computation succeeded.
 * @retval false  HMAC-SHA3-256 digest computation failed.
 **/
extern bool libspdm_hmac_sha3_256_final(void *hmac_sha3_256_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA3-256 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA3-256 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA3-256 digest
 *                          value (32 bytes).
 *
 * @retval true   HMAC-SHA3-256 digest computation succeeded.
 * @retval false  HMAC-SHA3-256 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_256_all(const void *data, size_t data_size,
                                      const uint8_t *key, size_t key_size,
                                      uint8_t *hmac_value);
#endif /* LIBSPDM_SHA3_256_SUPPORT */

#if LIBSPDM_SHA3_384_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA3-384 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha3_384_new() returns NULL.
 **/
extern void *libspdm_hmac_sha3_384_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha3_384_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha3_384_free(void *hmac_sha3_384_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha3_384_update().
 *
 * If hmac_sha3_384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  hmac_sha3_384_ctx  Pointer to HMAC-SHA3-384 context.
 * @param[in]   key                Pointer to the user-supplied key.
 * @param[in]   key_size           Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_384_set_key(void *hmac_sha3_384_ctx,
                                          const uint8_t *key,
                                          size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA3-384 context.
 *
 * If hmac_sha3_384_ctx is NULL, then return false.
 * If new_hmac_sha3_384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  hmac_sha3_384_ctx      Pointer to HMAC-SHA3-384 context being copied.
 * @param[out] new_hmac_sha3_384_ctx  Pointer to new HMAC-SHA3-384 context.
 *
 * @retval true   HMAC-SHA3-384 context copy succeeded.
 * @retval false  HMAC-SHA3-384 context copy failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_384_duplicate(const void *hmac_sha3_384_ctx,
                                            void *new_hmac_sha3_384_ctx);

/**
 * Digests the input data and updates HMAC-SHA3-384 context.
 *
 * This function performs HMAC-SHA3-384 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA3-384 context should be initialized by libspdm_hmac_sha3_384_new(), and should not be
 * finalized by libspdm_hmac_sha3_384_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha3_384_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha3_384_ctx  Pointer to the HMAC-SHA3-384 context.
 * @param[in]       data               Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size          Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA3-384 data digest succeeded.
 * @retval false  HMAC-SHA3-384 data digest failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_384_update(void *hmac_sha3_384_ctx, const void *data,
                                         size_t data_size);

/**
 * Completes computation of the HMAC-SHA3-384 digest value.
 *
 * This function completes HMAC-SHA3-384 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA3-384 context cannot
 * be used again. HMAC-SHA3-384 context should be initialized by libspdm_hmac_sha3_384_new(), and
 * should not be finalized by libspdm_hmac_sha3_384_final(). Behavior with invalid HMAC-SHA3-384
 * context is undefined.
 *
 * If hmac_sha3_384_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha3_384_ctx  Pointer to the HMAC-SHA3-384 context.
 * @param[out]      hmac_value         Pointer to a buffer that receives the HMAC-SHA3-384 digest
 *                                     value (48 bytes).
 *
 * @retval true   HMAC-SHA3-384 digest computation succeeded.
 * @retval false  HMAC-SHA3-384 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_384_final(void *hmac_sha3_384_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA3-384 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA3-384 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA3-384 digest
 *                          value (48 bytes).
 *
 * @retval true   HMAC-SHA3-384 digest computation succeeded.
 * @retval false  HMAC-SHA3-384 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_384_all(const void *data, size_t data_size,
                                      const uint8_t *key, size_t key_size,
                                      uint8_t *hmac_value);
#endif /* LIBSPDM_SHA3_384_SUPPORT */

#if LIBSPDM_SHA3_512_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SHA3-512 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sha3_512_new() returns NULL.
 **/
extern void *libspdm_hmac_sha3_512_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sha3_512_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sha3_512_free(void *hmac_sha3_512_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sha3_512_update().
 *
 * If hmac_sha3_512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  hmac_sha3_512_ctx  Pointer to HMAC-SHA3-512 context.
 * @param[in]   key                Pointer to the user-supplied key.
 * @param[in]   key_size           Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_512_set_key(void *hmac_sha3_512_ctx,
                                          const uint8_t *key,
                                          size_t key_size);

/**
 * Makes a copy of an existing HMAC-SHA3-512 context.
 *
 * If hmac_sha3_512_ctx is NULL, then return false.
 * If new_hmac_sha3_512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  hmac_sha3_512_ctx      Pointer to HMAC-SHA3-512 context being copied.
 * @param[out] new_hmac_sha3_512_ctx  Pointer to new HMAC-SHA3-512 context.
 *
 * @retval true   HMAC-SHA3-512 context copy succeeded.
 * @retval false  HMAC-SHA3-512 context copy failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_512_duplicate(const void *hmac_sha3_512_ctx,
                                            void *new_hmac_sha3_512_ctx);

/**
 * Digests the input data and updates HMAC-SHA3-512 context.
 *
 * This function performs HMAC-SHA3-512 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SHA3-512 context should be initialized by libspdm_hmac_sha3_512_new(), and should not be
 * finalized by libspdm_hmac_sha3_512_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sha3_512_ctx is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha3_512_ctx  Pointer to the HMAC-SHA3-512 context.
 * @param[in]       data               Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size          Size of data buffer in bytes.
 *
 * @retval true   HMAC-SHA3-512 data digest succeeded.
 * @retval false  HMAC-SHA3-512 data digest failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_512_update(void *hmac_sha3_512_ctx,
                                         const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SHA3-512 digest value.
 *
 * This function completes HMAC-SHA3-512 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SHA3-512 context cannot
 * be used again. HMAC-SHA3-512 context should be initialized by libspdm_hmac_sha3_512_new(), and
 * should not be finalized by libspdm_hmac_sha3_512_final(). Behavior with invalid HMAC-SHA3-512
 * context is undefined.
 *
 * If hmac_sha3_512_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  hmac_sha3_512_ctx  Pointer to the HMAC-SHA3-512 context.
 * @param[out]      hmac_value         Pointer to a buffer that receives the HMAC-SHA3-512 digest
 *                                     value (64 bytes).
 *
 * @retval true   HMAC-SHA3-512 digest computation succeeded.
 * @retval false  HMAC-SHA3-512 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_512_final(void *hmac_sha3_512_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SHA3-512 digest of a input data buffer.
 *
 * This function performs the HMAC-SHA3-512 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SHA3-512 digest
 *                          value (64 bytes).
 *
 * @retval true   HMAC-SHA3-512 digest computation succeeded.
 * @retval false  HMAC-SHA3-512 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sha3_512_all(const void *data, size_t data_size,
                                      const uint8_t *key, size_t key_size,
                                      uint8_t *hmac_value);
#endif /* LIBSPDM_SHA3_512_SUPPORT */

#if LIBSPDM_SM3_256_SUPPORT
/**
 * Allocates and initializes one HMAC_CTX context for subsequent HMAC-SM3-256 use.
 *
 * @return  Pointer to the HMAC_CTX context that has been initialized.
 *          If the allocations fails, libspdm_hmac_sm3_256_new() returns NULL.
 **/
extern void *libspdm_hmac_sm3_256_new(void);

/**
 * Release the specified HMAC_CTX context.
 *
 * @param[in]  hmac_sm3_256_ctx  Pointer to the HMAC_CTX context to be released.
 **/
extern void libspdm_hmac_sm3_256_free(void *hmac_sm3_256_ctx);

/**
 * Set user-supplied key for subsequent use. It must be done before any
 * calling to libspdm_hmac_sm3_256_update().
 *
 * If hmac_sm3_256_ctx is NULL, then return false.
 *
 * @param[out]  hmac_sm3_256_ctx  Pointer to HMAC-SM3-256 context.
 * @param[in]   key               Pointer to the user-supplied key.
 * @param[in]   key_size          Key size in bytes.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 **/
extern bool libspdm_hmac_sm3_256_set_key(void *hmac_sm3_256_ctx,
                                         const uint8_t *key, size_t key_size);

/**
 * Makes a copy of an existing HMAC-SM3-256 context.
 *
 * If hmac_sm3_256_ctx is NULL, then return false.
 * If new_hmac_sm3_256_ctx is NULL, then return false.
 *
 * @param[in]  hmac_sm3_256_ctx      Pointer to HMAC-SM3-256 context being copied.
 * @param[out] new_hmac_sm3_256_ctx  Pointer to new HMAC-SM3-256 context.
 *
 * @retval true   HMAC-SM3-256 context copy succeeded.
 * @retval false  HMAC-SM3-256 context copy failed.
 **/
extern bool libspdm_hmac_sm3_256_duplicate(const void *hmac_sm3_256_ctx,
                                           void *new_hmac_sm3_256_ctx);

/**
 * Digests the input data and updates HMAC-SM3-256 context.
 *
 * This function performs HMAC-SM3-256 digest on a data buffer of the specified size.
 * It can be called multiple times to compute the digest of long or discontinuous data streams.
 * HMAC-SM3-256 context should be initialized by libspdm_hmac_sm3_256_new(), and should not be
 * finalized by libspdm_hmac_sm3_256_final(). Behavior with invalid context is undefined.
 *
 * If hmac_sm3_256_ctx is NULL, then return false.
 *
 * @param[in, out]  hmac_sm3_256_ctx  Pointer to the HMAC-SM3-256 context.
 * @param[in]       data              Pointer to the buffer containing the data to be digested.
 * @param[in]       data_size         Size of data buffer in bytes.
 *
 * @retval true   HMAC-SM3-256 data digest succeeded.
 * @retval false  HMAC-SM3-256 data digest failed.
 **/
extern bool libspdm_hmac_sm3_256_update(void *hmac_sm3_256_ctx, const void *data, size_t data_size);

/**
 * Completes computation of the HMAC-SM3-256 digest value.
 *
 * This function completes HMAC-SM3-256 hash computation and retrieves the digest value into
 * the specified memory. After this function has been called, the HMAC-SM3-256 context cannot
 * be used again. HMAC-SM3-256 context should be initialized by libspdm_hmac_sm3_256_new(), and
 * should not be finalized by libspdm_hmac_sm3_256_final(). Behavior with invalid HMAC-SM3-256
 * context is undefined.
 *
 * If hmac_sm3_256_ctx is NULL, then return false.
 * If hmac_value is NULL, then return false.
 *
 * @param[in, out]  hmac_sm3_256_ctx  Pointer to the HMAC-SM3-256 context.
 * @param[out]      hmac_value        Pointer to a buffer that receives the HMAC-SM3-256 digest
 *                                    value (32 bytes).
 *
 * @retval true   HMAC-SM3-256 digest computation succeeded.
 * @retval false  HMAC-SM3-256 digest computation failed.
 **/
extern bool libspdm_hmac_sm3_256_final(void *hmac_sm3_256_ctx, uint8_t *hmac_value);

/**
 * Computes the HMAC-SM3-256 digest of a input data buffer.
 *
 * This function performs the HMAC-SM3-256 digest of a given data buffer, and places
 * the digest value into the specified memory.
 *
 * If this interface is not supported, then return false.
 *
 * @param[in]   data        Pointer to the buffer containing the data to be digested.
 * @param[in]   data_size   Size of data buffer in bytes.
 * @param[in]   key         Pointer to the user-supplied key.
 * @param[in]   key_size    Key size in bytes.
 * @param[out]  hmac_value  Pointer to a buffer that receives the HMAC-SM3-256 digest
 *                          value (32 bytes).
 *
 * @retval true   HMAC-SM3-256 digest computation succeeded.
 * @retval false  HMAC-SM3-256 digest computation failed.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_hmac_sm3_256_all(const void *data, size_t data_size,
                                     const uint8_t *key, size_t key_size,
                                     uint8_t *hmac_value);
#endif /* LIBSPDM_SM3_256_SUPPORT */

#endif /* CRYPTLIB_MAC_H */
