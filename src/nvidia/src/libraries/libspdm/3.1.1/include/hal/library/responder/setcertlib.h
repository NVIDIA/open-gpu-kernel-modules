/**
 *  Copyright Notice:
 *  Copyright 2021-2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef RESPONDER_SETCERTLIB_H
#define RESPONDER_SETCERTLIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"
#include "library/spdm_return_status.h"
#include "industry_standard/spdm.h"

#if LIBSPDM_ENABLE_CAPABILITY_SET_CERT_CAP
/**
 * return if current code is running in a trusted environment.
 *
 * @retval  true   It is in a trusted environment.
 * @retval  false  It is not in a trusted environment.
 **/
extern bool libspdm_is_in_trusted_environment();

/**
 * Stores a certificate chain in non-volatile memory.
 *
 *
 * @param[in]  slot_id          The number of slot for the certificate chain.
 * @param[in]  cert_chain       The pointer for the certificate chain to set.
 * @param[in]  cert_chain_size  The size of the certificate chain to set.
 * @param[in]  base_hash_algo   Indicates the negotiated hash algorithm.
 * @param[in]  base_asym_algo   Indicates the negotiated signing algorithms.
 *
 * @retval true   The certificate chain was successfully written to non-volatile memory.
 * @retval false  Unable to write certificate chain to non-volatile memory.
 **/
extern bool libspdm_write_certificate_to_nvm(uint8_t slot_id, const void * cert_chain,
                                             size_t cert_chain_size,
                                             uint32_t base_hash_algo, uint32_t base_asym_algo);

#endif /* LIBSPDM_ENABLE_CAPABILITY_SET_CERT_CAP */

#endif /* RESPONDER_SETCERTLIB_H */
