/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * SPDM common library.
 * It follows the SPDM Specification.
 **/

#include "library/spdm_device_secret_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP
libspdm_return_t libspdm_measurement_collection(
    spdm_version_number_t spdm_version,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t mesurements_index,
    uint8_t request_attribute,
    uint8_t *content_changed,
    uint8_t *device_measurement_count,
    void *device_measurement,
    size_t *device_measurement_size)
{
    return LIBSPDM_STATUS_UNSUPPORTED_CAP;
}

bool libspdm_generate_measurement_summary_hash(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_summary_hash_type,
    uint8_t  *measurement_summary_hash,
    size_t *measurement_summary_hash_size)
{
    return false;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
bool libspdm_requester_data_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, bool is_data_hash,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size)
{
    return false;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

bool libspdm_responder_data_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo,
    uint32_t base_hash_algo, bool is_data_hash,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size)
{
    return false;
}

#if LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP
bool libspdm_psk_handshake_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    const uint8_t *psk_hint,
    size_t psk_hint_size,
    const uint8_t *info,
    size_t info_size,
    uint8_t *out, size_t out_size)
{
    return false;
}

bool libspdm_psk_master_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    const uint8_t *psk_hint,
    size_t psk_hint_size,
    const uint8_t *info,
    size_t info_size, uint8_t *out,
    size_t out_size)
{
    return false;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_SET_CERTIFICATE_CAP
bool libspdm_write_certificate_to_nvm(uint8_t slot_id, const void * cert_chain,
                                      size_t cert_chain_size)
{
    return false;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_SET_CERTIFICATE_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP
bool libspdm_gen_csr(uint32_t base_hash_algo, uint32_t base_asym_algo, bool *need_reset,
                     uint8_t *requester_info, size_t requester_info_length,
                     size_t *csr_len, uint8_t **csr_pointer)
{
    return false;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP */
