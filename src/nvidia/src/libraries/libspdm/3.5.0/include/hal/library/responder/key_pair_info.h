/**
 *  Copyright Notice:
 *  Copyright 2024 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef RESPONDER_KEY_PAIR_INFO_H
#define RESPONDER_KEY_PAIR_INFO_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"
#include "industry_standard/spdm.h"

#if LIBSPDM_ENABLE_CAPABILITY_GET_KEY_PAIR_INFO_CAP

/**
 * read the key pair info of the key_pair_id.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  key_pair_id                Indicate which key pair ID's information to retrieve.
 *
 * @param  capabilities               Indicate the capabilities of the requested key pairs.
 * @param  key_usage_capabilities     Indicate the key usages the responder allows.
 * @param  current_key_usage          Indicate the currently configured key usage for the requested key pairs ID.
 * @param  asym_algo_capabilities     Indicate the asymmetric algorithms the Responder supports for this key pair ID.
 * @param  current_asym_algo          Indicate the currently configured asymmetric algorithm for this key pair ID.
 * @param  assoc_cert_slot_mask       This field is a bit mask representing the currently associated certificate slots.
 * @param  public_key_info_len        On input, indicate the size in bytes of the destination buffer to store.
 *                                    On output, indicate the size in bytes of the public_key_info.
 *                                    It can be NULL, if public_key_info is not required.
 * @param  public_key_info            A pointer to a destination buffer to store the public_key_info.
 *                                    It can be NULL, if public_key_info is not required.
 *
 * @retval true  get key pair info successfully.
 * @retval false get key pair info failed.
 **/
extern bool libspdm_read_key_pair_info(
    void *spdm_context,
    uint8_t key_pair_id,
    uint16_t *capabilities,
    uint16_t *key_usage_capabilities,
    uint16_t *current_key_usage,
    uint32_t *asym_algo_capabilities,
    uint32_t *current_asym_algo,
    uint8_t *assoc_cert_slot_mask,
    uint16_t *public_key_info_len,
    uint8_t *public_key_info);
#endif /* LIBSPDM_ENABLE_CAPABILITY_GET_KEY_PAIR_INFO_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_SET_KEY_PAIR_INFO_CAP
/**
 * write the key pair info of the key_pair_id.
 *
 * @param  spdm_context                 A pointer to the SPDM context.
 * @param  key_pair_id                  Indicate which key pair ID's information to retrieve.
 *
 * @param  key_pair_id                  Indicate which key pair ID's information to retrieve.
 * @param  operation                    Set key pair info operation: change/erase/generate.
 * @param  desired_key_usage            Indicate the desired key usage for the requested key pair ID.
 * @param  desired_asym_algo            Indicate the desired asymmetric algorithm for the requested key pair ID.
 * @param  desired_assoc_cert_slot_mask Indicate the desired certificate slot association for the requested key pair ID.
 * @param  need_reset                   For input, it gives the value of CERT_INSTALL_RESET_CAP:
 *                                                  If true, then device needs to be reset to complete the set_key_pair_info.
 *                                                  If false, the device doesn`t need to be reset to complete the set_key_pair_info.
 *                                       For output, it specifies whether the device needs to be reset to complete the set_key_pair_info or not.
 *
 * @retval true  set key pair info successfully.
 * @retval false set key pair info failed.
 **/
extern bool libspdm_write_key_pair_info(
    void *spdm_context,
    uint8_t key_pair_id,
    uint8_t operation,
    uint16_t desired_key_usage,
    uint32_t desired_asym_algo,
    uint8_t desired_assoc_cert_slot_mask,
    bool * need_reset);
#endif /* LIBSPDM_ENABLE_CAPABILITY_SET_KEY_PAIR_INFO_CAP */

#endif /* RESPONDER_KEY_PAIR_INFO_H */
