/**
 *  Copyright Notice:
 *  Copyright 2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef REQUESTER_PSKLIB_H
#define REQUESTER_PSKLIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"

#if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
/**
 * Derive HMAC-based Expand key Derivation Function (HKDF) Expand, based upon the negotiated HKDF
 * algorithm.
 *
 * @param  base_hash_algo  Indicates the hash algorithm.
 * @param  psk_hint        Pointer to the peer-provided PSK Hint.
 * @param  psk_hint_size   PSK Hint size in bytes.
 * @param  info            Pointer to the application specific info.
 * @param  info_size       Info size in bytes.
 * @param  out             Pointer to buffer to receive HKDF value.
 * @param  out_size        Size of HKDF bytes to generate.
 *
 * @retval true   HKDF generated successfully.
 * @retval false  HKDF generation failed.
 **/
extern bool libspdm_psk_handshake_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo, const uint8_t *psk_hint,
    size_t psk_hint_size, const uint8_t *info,
    size_t info_size, uint8_t *out, size_t out_size);

/**
 * Derive HMAC-based Expand key Derivation Function (HKDF) Expand, based upon the negotiated HKDF
 * algorithm.
 *
 * @param  base_hash_algo  Indicates the hash algorithm.
 * @param  psk_hint        Pointer to the peer-provided PSK Hint.
 * @param  psk_hint_size   PSK Hint size in bytes.
 * @param  info            Pointer to the application specific info.
 * @param  info_size       Info size in bytes.
 * @param  out             Pointer to buffer to receive HKDF value.
 * @param  out_size        Size of HKDF bytes to generate.
 *
 * @retval true   HKDF generated successfully.
 * @retval false  HKDF generation failed.
 **/
extern bool libspdm_psk_master_secret_hkdf_expand(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    const uint8_t *psk_hint, size_t psk_hint_size,
    const uint8_t *info, size_t info_size,
    uint8_t *out, size_t out_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */

#endif /* REQUESTER_PSKLIB_H */
