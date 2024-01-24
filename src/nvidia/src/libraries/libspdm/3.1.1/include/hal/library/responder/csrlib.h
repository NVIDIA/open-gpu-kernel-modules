/**
 *  Copyright Notice:
 *  Copyright 2021-2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef RESPONDER_CSRLIB_H
#define RESPONDER_CSRLIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"
#include "library/spdm_return_status.h"
#include "industry_standard/spdm.h"

#if LIBSPDM_ENABLE_CAPABILITY_CSR_CAP
/**
 * Gen CSR
 *
 * @param[in]      base_hash_algo        Indicates the signing algorithm.
 * @param[in]      base_asym_algo        Indicates the hash algorithm.
 * @param[in]      need_reset            If true, then device needs to be reset to complete the CSR.
 *                                       If false
 *
 * @param[in]      request                A pointer to the SPDM request data.
 * @param[in]      request_size           The size of SPDM request data.
 *
 * @param[in]      requester_info         Requester info to generate the CSR.
 * @param[in]      requester_info_length  The length of requester info.
 *
 * @param[in]      opaque_data            opaque data to generate the CSR.
 * @param[in]      opaque_data_length     The length of opaque data.
 *
 * @param[in]      csr_len      For input, csr_len is the size of store CSR buffer.
 *                              For output, csr_len is CSR len for DER format
 * @param[in]      csr_pointer  On input, csr_pointer is buffer address to store CSR.
 *                              On output, csr_pointer is address for stored CSR.
 *                              The csr_pointer address will be changed.
 *
 * @param[in]       is_device_cert_model  If true, the cert chain is DeviceCert model.
 *                                        If false, the cert chain is AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Failed to gen CSR.
 **/
extern bool libspdm_gen_csr(uint32_t base_hash_algo, uint32_t base_asym_algo, bool *need_reset,
                            const void *request, size_t request_size,
                            uint8_t *requester_info, size_t requester_info_length,
                            uint8_t *opaque_data, uint16_t opaque_data_length,
                            size_t *csr_len, uint8_t *csr_pointer,
                            bool is_device_cert_model);
#endif /* LIBSPDM_ENABLE_CAPABILITY_CSR_CAP */

#endif /* RESPONDER_CSRLIB_H */
