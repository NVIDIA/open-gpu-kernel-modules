/**
 *  Copyright Notice:
 *  Copyright 2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef REQUESTER_REQASYMSIGNLIB_H
#define REQUESTER_REQASYMSIGNLIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
/**
 * Sign an SPDM message data.
 *
 * @param  req_base_asym_alg Indicates the signing algorithm.
 * @param  base_hash_algo    Indicates the hash algorithm.
 * @param  is_data_hash      Indicates the message type.
 *                           If true, raw message before hash.
 *                           If false, message hash.
 * @param  message           A pointer to a message to be signed.
 * @param  message_size      The size in bytes of the message to be signed.
 * @param  signature         A pointer to a destination buffer to store the signature.
 * @param  sig_size          On input, indicates the size, in bytes, of the destination buffer to
 *                           store the signature.
 *                           On output, indicates the size, in bytes, of the signature in the
 *                           buffer.
 *
 * @retval true  signing success.
 * @retval false signing fail.
 **/
extern bool libspdm_requester_data_sign(
    spdm_version_number_t spdm_version,
    uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, bool is_data_hash,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size);

#if LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP
/**
 * This functions returns the opaque data in a CHALLENGE_AUTH response.
 *
 * @param spdm_version  Indicates the negotiated s version.
 *
 * @param  slot_id       The number of slot for the certificate chain.
 *
 * @param  measurement_summary_hash        The measurement summary hash.
 * @param  measurement_summary_hash_size   The size of measurement summary hash.
 *
 * @param opaque_data
 * A pointer to a destination buffer whose size, in bytes, is opaque_data_size. The opaque data is
 * copied to this buffer.
 *
 * @param opaque_data_size
 * On input, indicates the size, in bytes, of the destination buffer.
 * On output, indicates the size of the opaque data.
 **/
extern bool libspdm_encap_challenge_opaque_data(
    spdm_version_number_t spdm_version,
    uint8_t slot_id,
    uint8_t *measurement_summary_hash,
    size_t measurement_summary_hash_size,
    void *opaque_data,
    size_t *opaque_data_size);
#endif/*LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP*/

#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

#endif /* REQUESTER_REQASYMSIGNLIB_H */
