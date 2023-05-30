/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_DEVICE_SECRET_LIB_H
#define SPDM_DEVICE_SECRET_LIB_H

#include "internal/libspdm_lib_config.h"

#include "hal/base.h"
#include "industry_standard/spdm.h"
#include "hal/library/debuglib.h"
#include "hal/library/memlib.h"
#include "hal/library/cryptlib.h"
#include "library/spdm_crypt_lib.h"
#include "library/spdm_return_status.h"

#if LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP
/**
 * Collect the device measurement.
 *
 * libspdm will call this function to retrieve the measurements for a device.
 * The "measurement_index" parameter indicates the measurement requested.
 *
 * @param  measurement_specification     Indicates the measurement specification.
 * Must be a SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_* value in spdm.h.
 *
 * @param  measurement_hash_algo         Indicates the measurement hash algorithm.
 * Must be SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_* value in spdm.h.
 *
 * @param  measurement_index      The index of the measurement to collect.
 *
 * A value of [0x0] requests only the total number of measurements to be returned
 * in "measurements_count". The parameters "measurements" and
 * "measurements_size" will be left unmodified.
 *
 * A value of [1-0xFE] requests a single measurement for that measurement index
 * be returned. On success, "measurements_count" will be set to 1 and the
 * "measurements" and "measurements_size" fields will be set based
 * on the single measurement. An invalid measurement index will cause
 * "measurements_count" to return 0.
 *
 * A value of [0xFF] requests all measurements be returned.
 * On success, "measurements_count", "measurements", and "measurements_size"
 * fields will be set with data from all measurements.
 *
 * @param  measurements_count
 *
 * When "measurement_index" is zero, returns the total count of
 * measurements available for the device. None of the actual measurements are
 * returned however, and "measurements" and "measurements_size" are unmodified.
 *
 * When "measurement_index" is non-zero, returns the number of measurements
 * returned in "measurements" and "measurements_size". If "measurements_index"
 * is an invalid index not supported by the device, "measurements_count" will
 * return 0.
 *
 * @param  measurements
 *
 * A pointer to a destination buffer to store the concatenation of all device
 * measurement blocks. This buffer will only be modified if
 * "measurement_index" is non-zero.
 *
 * @param  measurements_size
 *
 * On input, indicates the size in bytes of the destination buffer.
 * On output, indicates the total size in bytes of all device measurement
 * blocks in the buffer. This field should only be modified if
 * "measurement_index" is non-zero.
 *
 * @retval RETURN_SUCCESS             Successfully returned measurement_count,
 *                                   measurements, measurements_size.
 * @retval RETURN_BUFFER_TOO_SMALL    measurements buffer too small for measurements.
 * @retval RETURN_INVALID_PARAMETER   Invalid parameter passed to function.
 * @retval RETURN_NOT_FOUND           Unsupported measurement index.
 * @retval RETURN_***                 Any other RETURN_ error from base.h
 *                                   indicating the type of failure
 **/
libspdm_return_t libspdm_measurement_collection(
    spdm_version_number_t spdm_version,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_index,
    uint8_t request_attribute,
    uint8_t *content_changed,
    uint8_t *measurements_count,
    void *measurements,
    size_t *measurements_size);

/**
 * This function calculates the measurement summary hash.
 *
 * @param  spdm_version                  The spdm version.
 * @param  base_hash_algo                The hash algo to use on summary.
 * @param  measurement_specification     Indicates the measurement specification.
 *                                      It must align with measurement_specification
 *                                      (SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_*)
 * @param  measurement_hash_algo         Indicates the measurement hash algorithm.
 *                                      It must align with measurement_hash_alg
 *                                      (SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_*)
 *
 * @param  measurement_summary_hash_type   The type of the measurement summary hash.
 * @param  measurement_summary_hash        The buffer to store the measurement summary hash.
 * @param  measurement_summary_hash_size   The size in bytes of the buffer.
 *
 * @retval true  measurement summary hash is generated or skipped.
 * @retval false measurement summary hash is not generated.
 **/
bool libspdm_generate_measurement_summary_hash(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_summary_hash_type,
    uint8_t  *measurement_summary_hash,
    size_t *measurement_summary_hash_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
/**
 * Sign an SPDM message data.
 *
 * @param  req_base_asym_alg               Indicates the signing algorithm.
 * @param  base_hash_algo                 Indicates the hash algorithm.
 * @param  is_data_hash                   Indicate the message type. true: raw message before hash, false: message hash.
 * @param  message                      A pointer to a message to be signed.
 * @param  message_size                  The size in bytes of the message to be signed.
 * @param  signature                    A pointer to a destination buffer to store the signature.
 * @param  sig_size                      On input, indicates the size in bytes of the destination buffer to store the signature.
 *                                     On output, indicates the size in bytes of the signature in the buffer.
 *
 * @retval true  signing success.
 * @retval false signing fail.
 **/
bool libspdm_requester_data_sign(
    spdm_version_number_t spdm_version,
    uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, bool is_data_hash,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

/**
 * Sign an SPDM message data.
 *
 * @param  base_asym_algo                 Indicates the signing algorithm.
 * @param  base_hash_algo                 Indicates the hash algorithm.
 * @param  is_data_hash                   Indicate the message type. true: raw message before hash, false: message hash.
 * @param  message                      A pointer to a message to be signed.
 * @param  message_size                  The size in bytes of the message to be signed.
 * @param  signature                    A pointer to a destination buffer to store the signature.
 * @param  sig_size                      On input, indicates the size in bytes of the destination buffer to store the signature.
 *                                     On output, indicates the size in bytes of the signature in the buffer.
 *
 * @retval true  signing success.
 * @retval false signing fail.
 **/
bool libspdm_responder_data_sign(
    spdm_version_number_t spdm_version,
    uint8_t op_code,
    uint32_t base_asym_algo,
    uint32_t base_hash_algo, bool is_data_hash,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size);

#if LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP
/**
 * Derive HMAC-based Expand key Derivation Function (HKDF) Expand, based upon the negotiated HKDF algorithm.
 *
 * @param  base_hash_algo                 Indicates the hash algorithm.
 * @param  psk_hint                      Pointer to the user-supplied PSK Hint.
 * @param  psk_hint_size                  PSK Hint size in bytes.
 * @param  info                         Pointer to the application specific info.
 * @param  info_size                     info size in bytes.
 * @param  out                          Pointer to buffer to receive hkdf value.
 * @param  out_size                      size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
bool libspdm_psk_handshake_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo, const uint8_t *psk_hint,
    size_t psk_hint_size, const uint8_t *info,
    size_t info_size, uint8_t *out, size_t out_size);

/**
 * Derive HMAC-based Expand key Derivation Function (HKDF) Expand, based upon the negotiated HKDF algorithm.
 *
 * @param  base_hash_algo                 Indicates the hash algorithm.
 * @param  psk_hint                      Pointer to the user-supplied PSK Hint.
 * @param  psk_hint_size                  PSK Hint size in bytes.
 * @param  info                         Pointer to the application specific info.
 * @param  info_size                     info size in bytes.
 * @param  out                          Pointer to buffer to receive hkdf value.
 * @param  out_size                      size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
bool libspdm_psk_master_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    const uint8_t *psk_hint,
    size_t psk_hint_size,
    const uint8_t *info,
    size_t info_size, uint8_t *out,
    size_t out_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_SET_CERTIFICATE_CAP
/**
 * This function sends SET_CERTIFICATE
 * to set certificate from the device.
 *
 *
 * @param[in]  slot_id                      The number of slot for the certificate chain.
 * @param[in]  cert_chain                   The pointer for the certificate chain to set.
 * @param[in]  cert_chain_size              The size of the certificate chain to set.
 *
 * @retval true                         Set certificate to NV successfully.
 * @retval false                        Set certificate to NV unsuccessfully.
 **/
bool libspdm_write_certificate_to_nvm(uint8_t slot_id, const void * cert_chain,
                                      size_t cert_chain_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_SET_CERTIFICATE_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP
/**
 * Gen CSR
 *
 * @param[in]      base_hash_algo        hash algo for sign
 * @param[in]      base_asym_algo        asym public key to set
 * @param[in]      need_reset            device need reset for gen csr
 *
 * @param[in]      requester_info        requester info to gen CSR
 * @param[in]      requester_info_length The len of requester info
 *
 * @param[in]      csr_len               For input, csr_len is the size of store CSR buffer.
 *                                       For output, csr_len is CSR len for DER format
 * @param[in]      csr_pointer           For input, csr_pointer is buffer address to store CSR.
 *                                       For output, csr_pointer is address for stored CSR.
 *                                       The csr_pointer address will be changed.
 *
 * @retval  true   Success.
 * @retval  false  Failed to gen CSR.
 **/
bool libspdm_gen_csr(uint32_t base_hash_algo, uint32_t base_asym_algo, bool *need_reset,
                     uint8_t *requester_info, size_t requester_info_length,
                     size_t *csr_len, uint8_t **csr_pointer);
#endif /* LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP */

#endif /* SPDM_DEVICE_SECRET_LIB_H */
