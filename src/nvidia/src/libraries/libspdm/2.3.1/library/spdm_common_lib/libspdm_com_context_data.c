/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_common_lib.h"
#include "internal/libspdm_secured_message_lib.h"

/**
 * Returns if an SPDM data_type requires session info.
 *
 * @param data_type  SPDM data type.
 *
 * @retval true  session info is required.
 * @retval false session info is not required.
 **/
static bool need_session_info_for_data(libspdm_data_type_t data_type)
{
    switch (data_type) {
    case LIBSPDM_DATA_SESSION_USE_PSK:
    case LIBSPDM_DATA_SESSION_MUT_AUTH_REQUESTED:
    case LIBSPDM_DATA_SESSION_END_SESSION_ATTRIBUTES:
    case LIBSPDM_DATA_SESSION_POLICY:
        return true;
    default:
        return false;
    }
}

/**
 * Set an SPDM context data.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  data_type                     Type of the SPDM context data.
 * @param  parameter                    Type specific parameter of the SPDM context data.
 * @param  data                         A pointer to the SPDM context data.
 * @param  data_size                     size in bytes of the SPDM context data.
 *
 * @retval RETURN_SUCCESS               The SPDM context data is set successfully.
 * @retval RETURN_INVALID_PARAMETER     The data is NULL or the data_type is zero.
 * @retval RETURN_UNSUPPORTED           The data_type is unsupported.
 * @retval RETURN_ACCESS_DENIED         The data_type cannot be set.
 * @retval RETURN_NOT_READY             data is not ready to set.
 **/
libspdm_return_t libspdm_set_data(void *context, libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter, void *data,
                                  size_t data_size)
{
    libspdm_context_t *spdm_context;
    uint32_t session_id;
    libspdm_session_info_t *session_info;
    uint8_t slot_id;
    uint8_t mut_auth_requested;
    uint8_t root_cert_index;
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT)
    bool status;
#endif

    if (context == NULL || data == NULL || data_type >= LIBSPDM_DATA_MAX) {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    spdm_context = context;

    if (need_session_info_for_data(data_type)) {
        if (parameter->location != LIBSPDM_DATA_LOCATION_SESSION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_id = *(const uint32_t *)parameter->additional_data;
        session_info = libspdm_get_session_info_via_session_id(spdm_context, session_id);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
    } else {
        session_info = NULL;
    }

    switch (data_type) {
    case LIBSPDM_DATA_SPDM_VERSION:
        LIBSPDM_ASSERT (data_size <= sizeof(spdm_version_number_t) * SPDM_MAX_VERSION_COUNT);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            /* Only have one connected version */
            LIBSPDM_ASSERT (data_size == sizeof(spdm_version_number_t));
            libspdm_copy_mem(&(spdm_context->connection_info.version),
                             sizeof(spdm_context->connection_info.version),
                             data,
                             sizeof(spdm_version_number_t));
        } else {
            spdm_context->local_context.version.spdm_version_count =
                (uint8_t)(data_size / sizeof(spdm_version_number_t));
            libspdm_copy_mem(spdm_context->local_context.version.spdm_version,
                             sizeof(spdm_context->local_context.version.spdm_version),
                             data,
                             spdm_context->local_context.version.spdm_version_count *
                             sizeof(spdm_version_number_t));
        }
        break;
    case LIBSPDM_DATA_SECURED_MESSAGE_VERSION:
        LIBSPDM_ASSERT (data_size <= sizeof(spdm_version_number_t) * SPDM_MAX_VERSION_COUNT);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            /* Only have one connected version */
            LIBSPDM_ASSERT (data_size == sizeof(spdm_version_number_t));
            libspdm_copy_mem(&(spdm_context->connection_info.secured_message_version),
                             sizeof(spdm_context->connection_info.secured_message_version),
                             data,
                             sizeof(spdm_version_number_t));
        } else {
            spdm_context->local_context.secured_message_version
            .spdm_version_count = (uint8_t)(data_size / sizeof(spdm_version_number_t));
            libspdm_copy_mem(spdm_context->local_context
                             .secured_message_version.spdm_version,
                             sizeof(spdm_context->local_context
                                    .secured_message_version.spdm_version),
                             data,
                             spdm_context->local_context.secured_message_version.
                             spdm_version_count * sizeof(spdm_version_number_t));
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_FLAGS:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }

    #if !(LIBSPDM_ENABLE_CAPABILITY_CERT_CAP)
        LIBSPDM_ASSERT(((*(uint32_t *)data) & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP) == 0);
    #endif /* !LIBSPDM_ENABLE_CAPABILITY_CERT_CAP */

    #if !(LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP)
        LIBSPDM_ASSERT(((*(uint32_t *)data) & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHAL_CAP) == 0);
    #endif /* !LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP */

    #if !(LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP)
        LIBSPDM_ASSERT(((*(uint32_t *)data) & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP) == 0);
    #endif /* !LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP */

    #if !(LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP)
        LIBSPDM_ASSERT(((*(uint32_t *)data) & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP) ==
                       0);
    #endif /* !LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP */

    #if !(LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP)
        LIBSPDM_ASSERT(((*(uint32_t *)data) & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP) == 0);
    #endif /* !LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP */

        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.capability.flags = *(uint32_t *)data;
        } else {
            spdm_context->local_context.capability.flags = *(uint32_t *)data;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_CT_EXPONENT:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.capability.ct_exponent = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_CAPABILITY_RTT_US:
        if (data_size != sizeof(uint64_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.capability.rtt = *(uint64_t *)data;
        break;
    case LIBSPDM_DATA_CAPABILITY_DATA_TRANSFER_SIZE:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        /* Only allow set smaller value*/
        LIBSPDM_ASSERT (*(uint32_t *)data <= LIBSPDM_MAX_MESSAGE_BUFFER_SIZE);
        spdm_context->local_context.capability.data_transfer_size = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_CAPABILITY_MAX_SPDM_MSG_SIZE:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        /* Only allow set smaller value. Need different value for CHUNK - TBD*/
        LIBSPDM_ASSERT (*(uint32_t *)data <= LIBSPDM_MAX_MESSAGE_BUFFER_SIZE);
        spdm_context->local_context.capability.max_spdm_msg_size = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_MEASUREMENT_SPEC:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.measurement_spec = *(uint8_t *)data;
        } else {
            spdm_context->local_context.algorithm.measurement_spec = *(uint8_t *)data;
        }
        break;
    case LIBSPDM_DATA_MEASUREMENT_HASH_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.measurement_hash_algo = *(uint32_t *)data;
        } else {
            spdm_context->local_context.algorithm.measurement_hash_algo = *(uint32_t *)data;
        }
        break;
    case LIBSPDM_DATA_BASE_ASYM_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.base_asym_algo = *(uint32_t *)data;
        } else {
            spdm_context->local_context.algorithm.base_asym_algo = *(uint32_t *)data;
        }
        break;
    case LIBSPDM_DATA_BASE_HASH_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.base_hash_algo = *(uint32_t *)data;
        } else {
            spdm_context->local_context.algorithm.base_hash_algo = *(uint32_t *)data;
        }
        break;
    case LIBSPDM_DATA_DHE_NAME_GROUP:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.dhe_named_group = *(uint16_t *)data;
        } else {
            spdm_context->local_context.algorithm.dhe_named_group = *(uint16_t *)data;
        }
        break;
    case LIBSPDM_DATA_AEAD_CIPHER_SUITE:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.aead_cipher_suite = *(uint16_t *)data;
        } else {
            spdm_context->local_context.algorithm.aead_cipher_suite = *(uint16_t *)data;
        }
        break;
    case LIBSPDM_DATA_REQ_BASE_ASYM_ALG:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.req_base_asym_alg = *(uint16_t *)data;
        } else {
            spdm_context->local_context.algorithm.req_base_asym_alg = *(uint16_t *)data;
        }
        break;
    case LIBSPDM_DATA_KEY_SCHEDULE:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.key_schedule = *(uint16_t *)data;
        } else {
            spdm_context->local_context.algorithm.key_schedule = *(uint16_t *)data;
        }
        break;
    case LIBSPDM_DATA_OTHER_PARAMS_SUPPORT:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            spdm_context->connection_info.algorithm.other_params_support = *(uint8_t *)data;
        } else {
            spdm_context->local_context.algorithm.other_params_support = *(uint8_t *)data;
        }
        break;
    case LIBSPDM_DATA_CONNECTION_STATE:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->connection_info.connection_state = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_RESPONSE_STATE:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->response_state = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_PEER_PUBLIC_ROOT_CERT:
        root_cert_index = 0;
        while (spdm_context->local_context.peer_root_cert_provision[root_cert_index] != NULL) {
            root_cert_index++;
            if (root_cert_index >= LIBSPDM_MAX_ROOT_CERT_SUPPORT) {
                return LIBSPDM_STATUS_BUFFER_FULL;
            }
        }
        spdm_context->local_context.peer_root_cert_provision_size[root_cert_index] = data_size;
        spdm_context->local_context.peer_root_cert_provision[root_cert_index] = data;
        break;
    case LIBSPDM_DATA_PEER_PUBLIC_CERT_CHAIN:
        spdm_context->local_context.peer_cert_chain_provision_size = data_size;
        spdm_context->local_context.peer_cert_chain_provision = data;
        break;
    case LIBSPDM_DATA_LOCAL_PUBLIC_CERT_CHAIN:
        slot_id = parameter->additional_data[0];
        if (slot_id >= SPDM_MAX_SLOT_COUNT) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.local_cert_chain_provision_size[slot_id] = data_size;
        spdm_context->local_context.local_cert_chain_provision[slot_id] = data;
        break;
    case LIBSPDM_DATA_LOCAL_PUBLIC_CERT_CHAIN_DEFAULT_SLOT_ID:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (*(uint8_t *)data >= SPDM_MAX_SLOT_COUNT) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.provisioned_slot_id = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_LOCAL_USED_CERT_CHAIN_BUFFER:
        if (data_size > LIBSPDM_MAX_CERT_CHAIN_SIZE) {
            return LIBSPDM_STATUS_BUFFER_FULL;
        }
        spdm_context->connection_info.local_used_cert_chain_buffer_size = data_size;
        spdm_context->connection_info.local_used_cert_chain_buffer = data;
        break;
    case LIBSPDM_DATA_PEER_USED_CERT_CHAIN_BUFFER:
        slot_id = parameter->additional_data[0];
        if (slot_id >= SPDM_MAX_SLOT_COUNT) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (data_size > LIBSPDM_MAX_CERT_CHAIN_SIZE) {
            return LIBSPDM_STATUS_BUFFER_FULL;
        }
        spdm_context->connection_info.peer_used_cert_chain_slot_id = slot_id;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_size = data_size;
        libspdm_copy_mem(spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer,
                         sizeof(spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer),
                         data, data_size);
#else
        status = libspdm_hash_all(
            spdm_context->connection_info.algorithm.base_hash_algo,
            data, data_size,
            spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash);
        if (!status) {
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size =
            libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

        status = false;
#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
        if (!status) {
            status = libspdm_rsa_get_public_key_from_x509(
                data, data_size,
                &spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if LIBSPDM_ECDSA_SUPPORT
        if (!status) {
            status = libspdm_ec_get_public_key_from_x509(
                data, data_size,
                &spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
        if (!status) {
            status = libspdm_ecd_get_public_key_from_x509(
                data, data_size,
                &spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if LIBSPDM_SM2_DSA_SUPPORT
        if (!status) {
            status = libspdm_sm2_get_public_key_from_x509(
                data, data_size,
                &spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
        if (!status) {
            return LIBSPDM_STATUS_INVALID_CERT;
        }
#endif
        break;
    case LIBSPDM_DATA_BASIC_MUT_AUTH_REQUESTED:
        if (data_size != sizeof(bool)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        mut_auth_requested = *(uint8_t *)data;
        if (((mut_auth_requested != 0) && (mut_auth_requested != 1))) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.basic_mut_auth_requested = mut_auth_requested;
        spdm_context->encap_context.request_id = 0;
        spdm_context->encap_context.req_slot_id = parameter->additional_data[0];
        break;
    case LIBSPDM_DATA_MUT_AUTH_REQUESTED:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        mut_auth_requested = *(uint8_t *)data;
        if (((mut_auth_requested != 0) &&
             (mut_auth_requested !=
              SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED) &&
             (mut_auth_requested !=
              SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_ENCAP_REQUEST) &&
             (mut_auth_requested !=
              SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_GET_DIGESTS))) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.mut_auth_requested = mut_auth_requested;
        spdm_context->encap_context.request_id = 0;
        spdm_context->encap_context.req_slot_id = parameter->additional_data[0];
        break;
    case LIBSPDM_DATA_HEARTBEAT_PERIOD:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.heartbeat_period = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_PSK_HINT:
        if (data_size > LIBSPDM_PSK_MAX_HINT_LENGTH) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.psk_hint_size = data_size;
        spdm_context->local_context.psk_hint = data;
        break;
    case LIBSPDM_DATA_SESSION_USE_PSK:
        if (data_size != sizeof(bool)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_info->use_psk = *(bool *)data;
        break;
    case LIBSPDM_DATA_SESSION_MUT_AUTH_REQUESTED:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_info->mut_auth_requested = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_SESSION_END_SESSION_ATTRIBUTES:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_info->end_session_attributes = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_SESSION_POLICY:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_info->session_policy = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_APP_CONTEXT_DATA:
        if (data_size != sizeof(void *) || *(void **)data == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->app_context_data_ptr = *(void **)data;
        break;
    case LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->handle_error_return_policy = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_VCA_CACHE:
        if (data_size > sizeof(spdm_context->transcript.message_a.buffer)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->transcript.message_a.buffer_size = data_size;
        libspdm_copy_mem(spdm_context->transcript.message_a.buffer,
                         sizeof(spdm_context->transcript.message_a.buffer),
                         data, data_size);
        break;
    case LIBSPDM_DATA_IS_REQUESTER:
        if (data_size != sizeof(bool)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        spdm_context->local_context.is_requester = *(bool *)data;
        break;
    default:
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
        break;
    }

    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Get an SPDM context data.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  data_type                     Type of the SPDM context data.
 * @param  parameter                    Type specific parameter of the SPDM context data.
 * @param  data                         A pointer to the SPDM context data.
 * @param  data_size                     size in bytes of the SPDM context data.
 *                                     On input, it means the size in bytes of data buffer.
 *                                     On output, it means the size in bytes of copied data buffer if RETURN_SUCCESS,
 *                                     and means the size in bytes of desired data buffer if RETURN_BUFFER_TOO_SMALL.
 *
 * @retval RETURN_SUCCESS               The SPDM context data is set successfully.
 * @retval RETURN_INVALID_PARAMETER     The data_size is NULL or the data is NULL and *data_size is not zero.
 * @retval RETURN_UNSUPPORTED           The data_type is unsupported.
 * @retval RETURN_NOT_FOUND             The data_type cannot be found.
 * @retval RETURN_NOT_READY             The data is not ready to return.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_get_data(void *context, libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter,
                                  void *data, size_t *data_size)
{
    libspdm_context_t *spdm_context;
    size_t target_data_size;
    void *target_data;
    uint32_t session_id;
    libspdm_session_info_t *session_info;
    size_t digest_size;
    size_t digest_count;
    size_t index;

    if (context == NULL || data == NULL || data_size == NULL || data_type >= LIBSPDM_DATA_MAX) {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    spdm_context = context;

    if (need_session_info_for_data(data_type)) {
        if (parameter->location != LIBSPDM_DATA_LOCATION_SESSION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_id = *(const uint32_t *)parameter->additional_data;
        session_info = libspdm_get_session_info_via_session_id(spdm_context, session_id);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
    } else {
        session_info = NULL;
    }

    switch (data_type) {
    case LIBSPDM_DATA_SPDM_VERSION:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(spdm_version_number_t);
        target_data = &(spdm_context->connection_info.version);
        break;
    case LIBSPDM_DATA_SECURED_MESSAGE_VERSION:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(spdm_version_number_t);
        target_data = &(spdm_context->connection_info.secured_message_version);
        break;
    case LIBSPDM_DATA_CAPABILITY_FLAGS:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &spdm_context->connection_info.capability.flags;
        } else {
            target_data = &spdm_context->local_context.capability.flags;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_CT_EXPONENT:
        target_data_size = sizeof(uint8_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &spdm_context->connection_info.capability.ct_exponent;
        } else {
            target_data = &spdm_context->local_context.capability.ct_exponent;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_DATA_TRANSFER_SIZE:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &spdm_context->connection_info.capability.data_transfer_size;
        } else {
            target_data = &spdm_context->local_context.capability.data_transfer_size;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_MAX_SPDM_MSG_SIZE:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &spdm_context->connection_info.capability.max_spdm_msg_size;
        } else {
            target_data = &spdm_context->local_context.capability.max_spdm_msg_size;
        }
        break;
    case LIBSPDM_DATA_MEASUREMENT_SPEC:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &spdm_context->connection_info.algorithm.measurement_spec;
        break;
    case LIBSPDM_DATA_MEASUREMENT_HASH_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &spdm_context->connection_info.algorithm.measurement_hash_algo;
        break;
    case LIBSPDM_DATA_BASE_ASYM_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &spdm_context->connection_info.algorithm.base_asym_algo;
        break;
    case LIBSPDM_DATA_BASE_HASH_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &spdm_context->connection_info.algorithm.base_hash_algo;
        break;
    case LIBSPDM_DATA_DHE_NAME_GROUP:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &spdm_context->connection_info.algorithm.dhe_named_group;
        break;
    case LIBSPDM_DATA_AEAD_CIPHER_SUITE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &spdm_context->connection_info.algorithm.aead_cipher_suite;
        break;
    case LIBSPDM_DATA_REQ_BASE_ASYM_ALG:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &spdm_context->connection_info.algorithm.req_base_asym_alg;
        break;
    case LIBSPDM_DATA_KEY_SCHEDULE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &spdm_context->connection_info.algorithm.key_schedule;
        break;
    case LIBSPDM_DATA_OTHER_PARAMS_SUPPORT:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &spdm_context->connection_info.algorithm.other_params_support;
        break;
    case LIBSPDM_DATA_CONNECTION_STATE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &spdm_context->connection_info.connection_state;
        break;
    case LIBSPDM_DATA_RESPONSE_STATE:
        target_data_size = sizeof(uint32_t);
        target_data = &spdm_context->response_state;
        break;
    case LIBSPDM_DATA_PEER_SLOT_MASK:
        target_data_size = sizeof(uint8_t);
        target_data = &spdm_context->connection_info.peer_digest_slot_mask;
        break;
    case LIBSPDM_DATA_PEER_TOTAL_DIGEST_BUFFER:
        digest_count = 0;
        for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
            if (spdm_context->connection_info.peer_digest_slot_mask & (1 << index)) {
                digest_count++;
            }
        }
        digest_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
        target_data_size = digest_size * digest_count;
        target_data = spdm_context->connection_info.peer_total_digest_buffer;
        break;
    case LIBSPDM_DATA_SESSION_USE_PSK:
        target_data_size = sizeof(bool);
        target_data = &session_info->use_psk;
        break;
    case LIBSPDM_DATA_SESSION_MUT_AUTH_REQUESTED:
        target_data_size = sizeof(uint8_t);
        target_data = &session_info->mut_auth_requested;
        break;
    case LIBSPDM_DATA_SESSION_END_SESSION_ATTRIBUTES:
        target_data_size = sizeof(uint8_t);
        target_data = &session_info->end_session_attributes;
        break;
    case LIBSPDM_DATA_SESSION_POLICY:
        target_data_size = sizeof(uint8_t);
        target_data = &session_info->session_policy;
        break;
    case LIBSPDM_DATA_APP_CONTEXT_DATA:
        target_data_size = sizeof(void *);
        target_data = &spdm_context->app_context_data_ptr;
        break;
    case LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY:
        target_data_size = sizeof(uint8_t);
        target_data = &spdm_context->handle_error_return_policy;
        break;
    case LIBSPDM_DATA_VCA_CACHE:
        target_data_size = spdm_context->transcript.message_a.buffer_size;
        target_data = spdm_context->transcript.message_a.buffer;
        break;
    default:
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
        break;
    }

    if (*data_size < target_data_size) {
        *data_size = target_data_size;
        return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
    }
    libspdm_copy_mem(data, *data_size, target_data, target_data_size);
    *data_size = target_data_size;

    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Reset message A cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_a(void *context)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_a);
}

/**
 * Reset message B cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_b(void *context)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_b);
#else
    if (spdm_context->transcript.digest_context_m1m2 != NULL) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           spdm_context->transcript.digest_context_m1m2);
        spdm_context->transcript.digest_context_m1m2 = NULL;
    }
#endif
}

/**
 * Reset message C cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_c(void *context)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_c);
#else
    if (spdm_context->transcript.digest_context_m1m2 != NULL) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           spdm_context->transcript.digest_context_m1m2);
        spdm_context->transcript.digest_context_m1m2 = NULL;
    }
#endif
}

/**
 * Reset message MutB cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_mut_b(void *context)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_mut_b);
#else
    if (spdm_context->transcript.digest_context_mut_m1m2 != NULL) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           spdm_context->transcript.digest_context_mut_m1m2);
        spdm_context->transcript.digest_context_mut_m1m2 = NULL;
    }
#endif
}

/**
 * Reset message MutC cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_mut_c(void *context)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_mut_c);
#else
    if (spdm_context->transcript.digest_context_mut_m1m2 != NULL) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           spdm_context->transcript.digest_context_mut_m1m2);
        spdm_context->transcript.digest_context_mut_m1m2 = NULL;
    }
#endif
}

/**
 * Reset message M cache in SPDM context.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  A pointer to the SPDM session context.
 **/
void libspdm_reset_message_m(void *context, void *session_info)
{
    libspdm_context_t *spdm_context;
    libspdm_session_info_t *spdm_session_info;

    spdm_context = context;
    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    if (spdm_session_info == NULL) {
        libspdm_reset_managed_buffer(&spdm_context->transcript.message_m);
    } else {
        libspdm_reset_managed_buffer(&spdm_session_info->session_transcript.message_m);
    }
#else
    if (spdm_session_info == NULL) {
        if (spdm_context->transcript.digest_context_l1l2 != NULL) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_context->transcript.digest_context_l1l2);
            spdm_context->transcript.digest_context_l1l2 = NULL;
        }
    } else {
        if (spdm_session_info->session_transcript.digest_context_l1l2 != NULL) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_l1l2);
            spdm_session_info->session_transcript.digest_context_l1l2 = NULL;
        }
    }
#endif
}

/**
 * Reset message K cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_session_info              A pointer to the SPDM session context.
 **/
void libspdm_reset_message_k(void *context, void *session_info)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_session_info->session_transcript.message_k);
#else
    {
        libspdm_context_t *spdm_context;

        spdm_context = context;

        if (spdm_session_info->session_transcript.digest_context_th != NULL) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_th);
            spdm_session_info->session_transcript.digest_context_th = NULL;
        }
        if (spdm_session_info->session_transcript.digest_context_th_backup != NULL) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_th_backup);
            spdm_session_info->session_transcript.digest_context_th_backup = NULL;
        }
    }
#endif
}

/**
 * Reset message F cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_session_info              A pointer to the SPDM session context.
 **/
void libspdm_reset_message_f(void *context, void *session_info)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_session_info->session_transcript.message_f);
#else
    {
        libspdm_context_t *spdm_context;

        spdm_context = context;

        if (spdm_session_info->session_transcript.digest_context_th != NULL) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_th);
            spdm_session_info->session_transcript.digest_context_th =
                spdm_session_info->session_transcript.digest_context_th_backup;
            spdm_session_info->session_transcript.digest_context_th_backup = NULL;
        }
        spdm_session_info->session_transcript.message_f_initialized = false;
    }
#endif
}

/**
 * Reset message buffer in SPDM context according to request code.
 *
 * @param  spdm_context                   A pointer to the SPDM context.
 * @param  spdm_session_info             A pointer to the SPDM session context.
 * @param  spdm_request                   The SPDM request code.
 */
void libspdm_reset_message_buffer_via_request_code(void *context, void *session_info,
                                                   uint8_t request_code)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    /**
     * Any request other than SPDM_GET_MEASUREMENTS resets L1/L2
     */
    if (request_code != SPDM_GET_MEASUREMENTS) {
        libspdm_reset_message_m(spdm_context, session_info);
    }
    /**
     * If the Requester issued GET_MEASUREMENTS or KEY_EXCHANGE or FINISH or PSK_EXCHANGE
     * or PSK_FINISH or KEY_UPDATE or HEARTBEAT or GET_ENCAPSULATED_REQUEST or DELIVER_ENCAPSULATED_RESPONSE
     * or END_SESSSION request(s) and skipped CHALLENGE completion, M1 and M2 are reset to null.
     */
    switch (request_code)
    {
    case SPDM_KEY_EXCHANGE:
    case SPDM_GET_MEASUREMENTS:
    case SPDM_FINISH:
    case SPDM_PSK_EXCHANGE:
    case SPDM_PSK_FINISH:
    case SPDM_KEY_UPDATE:
    case SPDM_HEARTBEAT:
    case SPDM_GET_ENCAPSULATED_REQUEST:
    case SPDM_END_SESSION:
        if (spdm_context->connection_info.connection_state <
            LIBSPDM_CONNECTION_STATE_AUTHENTICATED) {
            libspdm_reset_message_b(spdm_context);
            libspdm_reset_message_c(spdm_context);
            libspdm_reset_message_mut_b(spdm_context);
            libspdm_reset_message_mut_c(spdm_context);
        }
        break;
    case SPDM_DELIVER_ENCAPSULATED_RESPONSE:
        if (spdm_context->connection_info.connection_state <
            LIBSPDM_CONNECTION_STATE_AUTHENTICATED) {
            libspdm_reset_message_b(spdm_context);
            libspdm_reset_message_c(spdm_context);
        }
        break;
    case SPDM_GET_DIGESTS:
        libspdm_reset_message_b(spdm_context);
        break;
    default:
        break;
    }
}
/**
 * Append message A cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_a(void *context, const void *message, size_t message_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    return libspdm_append_managed_buffer(&spdm_context->transcript.message_a,
                                         message, message_size);
}

/**
 * Append message B cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_b(void *context, const void *message, size_t message_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(&spdm_context->transcript.message_b,
                                         message, message_size);
#else
    {
        bool result;

        if (spdm_context->transcript.digest_context_m1m2 == NULL) {
            spdm_context->transcript.digest_context_m1m2 = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_context->transcript.digest_context_m1m2 == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                        spdm_context->transcript.digest_context_m1m2);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_m1m2);
                spdm_context->transcript.digest_context_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                          spdm_context->transcript.digest_context_m1m2,
                                          libspdm_get_managed_buffer(&spdm_context->transcript.
                                                                     message_a),
                                          libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                                          message_a));
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_m1m2);
                spdm_context->transcript.digest_context_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }

        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_context->transcript.digest_context_m1m2, message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_context->transcript.digest_context_m1m2);
            spdm_context->transcript.digest_context_m1m2 = NULL;
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message C cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_c(void *context, const void *message, size_t message_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(&spdm_context->transcript.message_c,
                                         message, message_size);
#else
    {
        bool result;

        if (spdm_context->transcript.digest_context_m1m2 == NULL) {
            spdm_context->transcript.digest_context_m1m2 = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_context->transcript.digest_context_m1m2 == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                        spdm_context->transcript.digest_context_m1m2);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_m1m2);
                spdm_context->transcript.digest_context_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                          spdm_context->transcript.digest_context_m1m2,
                                          libspdm_get_managed_buffer(&spdm_context->transcript.
                                                                     message_a),
                                          libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                                          message_a));
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_m1m2);
                spdm_context->transcript.digest_context_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }

        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_context->transcript.digest_context_m1m2, message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_context->transcript.digest_context_m1m2);
            spdm_context->transcript.digest_context_m1m2 = NULL;
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message MutB cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_mut_b(void *context, const void *message,
                                              size_t message_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(&spdm_context->transcript.message_mut_b,
                                         message, message_size);
#else
    {
        bool result;

        if (spdm_context->transcript.digest_context_mut_m1m2 == NULL) {
            spdm_context->transcript.digest_context_mut_m1m2 = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_context->transcript.digest_context_mut_m1m2 == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                        spdm_context->transcript.digest_context_mut_m1m2);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_mut_m1m2);
                spdm_context->transcript.digest_context_mut_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
                SPDM_MESSAGE_VERSION_11) {

                /* Need append VCA since 1.2 script */
                result = libspdm_hash_update (
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    spdm_context->transcript.digest_context_mut_m1m2,
                    libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
                    libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                    message_a));
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_context->transcript.digest_context_mut_m1m2);
                    spdm_context->transcript.digest_context_mut_m1m2 = NULL;
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
            }
        }

        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_context->transcript.digest_context_mut_m1m2, message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_context->transcript.digest_context_mut_m1m2);
            spdm_context->transcript.digest_context_mut_m1m2 = NULL;
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message MutC cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_mut_c(void *context, const void *message,
                                              size_t message_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(&spdm_context->transcript.message_mut_c,
                                         message, message_size);
#else
    {
        bool result;

        if (spdm_context->transcript.digest_context_mut_m1m2 == NULL) {
            spdm_context->transcript.digest_context_mut_m1m2 = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_context->transcript.digest_context_mut_m1m2 == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                        spdm_context->transcript.digest_context_mut_m1m2);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_mut_m1m2);
                spdm_context->transcript.digest_context_mut_m1m2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
                SPDM_MESSAGE_VERSION_11) {

                /* Need append VCA since 1.2 script */
                result = libspdm_hash_update (
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    spdm_context->transcript.digest_context_mut_m1m2,
                    libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
                    libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                    message_a));
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_context->transcript.digest_context_mut_m1m2);
                    spdm_context->transcript.digest_context_mut_m1m2 = NULL;
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
            }
        }

        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_context->transcript.digest_context_mut_m1m2, message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_context->transcript.digest_context_mut_m1m2);
            spdm_context->transcript.digest_context_mut_m1m2 = NULL;
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message M cache in SPDM context.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  A pointer to the SPDM session context.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_m(void *context, void *session_info,
                                          const void *message, size_t message_size)
{
    libspdm_context_t *spdm_context;
    libspdm_session_info_t *spdm_session_info;

    spdm_context = context;
    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    if (spdm_session_info == NULL) {
        return libspdm_append_managed_buffer(&spdm_context->transcript.message_m,
                                             message, message_size);
    } else {
        return libspdm_append_managed_buffer(&spdm_session_info->session_transcript.message_m,
                                             message, message_size);
    }
#else
    {
        bool result;

        if (spdm_session_info == NULL) {
            if (spdm_context->transcript.digest_context_l1l2 == NULL) {
                spdm_context->transcript.digest_context_l1l2 = libspdm_hash_new (
                    spdm_context->connection_info.algorithm.base_hash_algo);
                if (spdm_context->transcript.digest_context_l1l2 == NULL) {
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
                result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                            spdm_context->transcript.digest_context_l1l2);
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_context->transcript.digest_context_l1l2);
                    spdm_context->transcript.digest_context_l1l2 = NULL;
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
                if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
                    SPDM_MESSAGE_VERSION_11) {

                    /* Need append VCA since 1.2 script */
                    result = libspdm_hash_update (
                        spdm_context->connection_info.algorithm.base_hash_algo,
                        spdm_context->transcript.digest_context_l1l2,
                        libspdm_get_managed_buffer(
                            &spdm_context->transcript.message_a),
                        libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                        message_a));
                    if (!result) {
                        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                           spdm_context->transcript.digest_context_l1l2);
                        spdm_context->transcript.digest_context_l1l2 = NULL;
                        return LIBSPDM_STATUS_CRYPTO_ERROR;
                    }
                }
            }
            result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                          spdm_context->transcript.digest_context_l1l2, message,
                                          message_size);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_context->transcript.digest_context_l1l2);
                spdm_context->transcript.digest_context_l1l2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        } else {
            if (spdm_session_info->session_transcript.digest_context_l1l2 == NULL) {
                spdm_session_info->session_transcript.digest_context_l1l2 = libspdm_hash_new (
                    spdm_context->connection_info.algorithm.base_hash_algo);
                if (spdm_session_info->session_transcript.digest_context_l1l2 == NULL) {
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
                result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                            spdm_session_info->session_transcript.digest_context_l1l2);
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_session_info->session_transcript.digest_context_l1l2);
                    spdm_session_info->session_transcript.digest_context_l1l2 = NULL;
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
                if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
                    SPDM_MESSAGE_VERSION_11) {

                    /* Need append VCA since 1.2 script*/

                    result = libspdm_hash_update (
                        spdm_context->connection_info.algorithm.base_hash_algo,
                        spdm_session_info->session_transcript.digest_context_l1l2,
                        libspdm_get_managed_buffer(
                            &spdm_context->transcript.message_a),
                        libspdm_get_managed_buffer_size(&spdm_context->transcript.
                                                        message_a));
                    if (!result) {
                        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                           spdm_session_info->session_transcript.digest_context_l1l2);
                        spdm_session_info->session_transcript.digest_context_l1l2 = NULL;
                        return LIBSPDM_STATUS_CRYPTO_ERROR;
                    }
                }
            }
            result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                          spdm_session_info->session_transcript.digest_context_l1l2,
                                          message, message_size);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_session_info->session_transcript.digest_context_l1l2);
                spdm_session_info->session_transcript.digest_context_l1l2 = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }

        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message K cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_session_info              A pointer to the SPDM session context.
 * @param  is_requester                  Indicate of the key generation for a requester or a responder.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_k(void *context, void *session_info,
                                          bool is_requester, const void *message,
                                          size_t message_size)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(
        &spdm_session_info->session_transcript.message_k, message,
        message_size);
#else
    {
        libspdm_context_t *spdm_context;
        uint8_t *cert_chain_buffer;
        size_t cert_chain_buffer_size;
        bool result;
        uint8_t cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
        uint32_t hash_size;
        uint8_t slot_id;

        hash_size = 0;
        spdm_context = context;
        slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;

        if (spdm_session_info->session_transcript.digest_context_th == NULL) {
            if (!spdm_session_info->use_psk) {
                LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);
                if (is_requester) {
                    if(spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size
                       != 0) {
                        hash_size =
                            spdm_context->connection_info.peer_used_cert_chain[slot_id].
                            buffer_hash_size;
                        libspdm_copy_mem(cert_chain_buffer_hash,
                                         sizeof(cert_chain_buffer_hash),
                                         spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash,
                                         hash_size);
                    } else {
                        result = libspdm_get_peer_cert_chain_buffer(
                            spdm_context, (const void **)&cert_chain_buffer,
                            &cert_chain_buffer_size);
                        if (!result) {
                            return LIBSPDM_STATUS_INVALID_STATE_PEER;
                        }
                        hash_size = libspdm_get_hash_size(
                            spdm_context->connection_info.algorithm.base_hash_algo);
                        result = libspdm_hash_all(
                            spdm_context->connection_info.algorithm.base_hash_algo,
                            cert_chain_buffer, cert_chain_buffer_size,
                            cert_chain_buffer_hash);
                        if (!result) {
                            return LIBSPDM_STATUS_CRYPTO_ERROR;
                        }

                    }
                } else {
                    result = libspdm_get_local_cert_chain_buffer(
                        spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);

                    if (!result) {
                        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
                    }
                    hash_size = libspdm_get_hash_size(
                        spdm_context->connection_info.algorithm.base_hash_algo);
                    result = libspdm_hash_all(
                        spdm_context->connection_info.algorithm.base_hash_algo,
                        cert_chain_buffer, cert_chain_buffer_size,
                        cert_chain_buffer_hash);
                    if (!result) {
                        return LIBSPDM_STATUS_CRYPTO_ERROR;
                    }
                }
            }
        }


        /* prepare digest_context_th*/

        if (spdm_session_info->session_transcript.digest_context_th == NULL) {
            spdm_session_info->session_transcript.digest_context_th = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_session_info->session_transcript.digest_context_th == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_init (spdm_context->connection_info.algorithm.base_hash_algo,
                                        spdm_session_info->session_transcript.digest_context_th);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_session_info->session_transcript.digest_context_th);
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                          spdm_session_info->session_transcript.digest_context_th,
                                          libspdm_get_managed_buffer(&spdm_context->transcript.
                                                                     message_a),
                                          libspdm_get_managed_buffer_size(
                                              &spdm_context->transcript.message_a));
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_session_info->session_transcript.digest_context_th);
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            if (!spdm_session_info->use_psk) {
                result = libspdm_hash_update (
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    spdm_session_info->session_transcript.digest_context_th,
                    cert_chain_buffer_hash, hash_size);
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_session_info->session_transcript.digest_context_th);
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
            }
        }
        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_session_info->session_transcript.digest_context_th,
                                      message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_th);
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }
        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * Append message F cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_session_info              A pointer to the SPDM session context.
 * @param  is_requester                  Indicate of the key generation for a requester or a responder.
 * @param  message                      message buffer.
 * @param  message_size                  size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_f(void *context, void *session_info,
                                          bool is_requester, const void *message,
                                          size_t message_size)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    return libspdm_append_managed_buffer(
        &spdm_session_info->session_transcript.message_f, message,
        message_size);
#else
    {
        libspdm_context_t *spdm_context;
        const uint8_t *mut_cert_chain_buffer;
        size_t mut_cert_chain_buffer_size;
        bool result;
        uint8_t mut_cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
        uint32_t hash_size;
        libspdm_return_t status;
        uint8_t slot_id;

        hash_size = 0;
        spdm_context = context;
        slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;

        if (!spdm_session_info->session_transcript.message_f_initialized) {

            /* digest_context_th might be NULL in unit test, where message_k is hardcoded. */
            if (spdm_session_info->session_transcript.digest_context_th == NULL) {
                status = libspdm_append_message_k (context, session_info, is_requester, NULL, 0);
                if (LIBSPDM_STATUS_IS_ERROR(status)) {
                    return status;
                }
            }

            if (!spdm_session_info->use_psk && spdm_session_info->mut_auth_requested) {
                if (is_requester) {
                    result = libspdm_get_local_cert_chain_buffer(
                        spdm_context,
                        (const void **)&mut_cert_chain_buffer,
                        &mut_cert_chain_buffer_size);
                    if (!result) {
                        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
                    }
                    hash_size = libspdm_get_hash_size(
                        spdm_context->connection_info.algorithm.base_hash_algo);
                    result = libspdm_hash_all(
                        spdm_context->connection_info.algorithm.base_hash_algo,
                        mut_cert_chain_buffer, mut_cert_chain_buffer_size,
                        mut_cert_chain_buffer_hash);
                    if (!result) {
                        return LIBSPDM_STATUS_CRYPTO_ERROR;
                    }
                } else {
                    LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);
                    if (spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size
                        != 0) {
                        hash_size =
                            spdm_context->connection_info.peer_used_cert_chain[slot_id].
                            buffer_hash_size;
                        libspdm_copy_mem(mut_cert_chain_buffer_hash,
                                         sizeof(mut_cert_chain_buffer_hash),
                                         spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash,
                                         hash_size);
                    } else {
                        result = libspdm_get_peer_cert_chain_buffer(
                            spdm_context,
                            (const void **)&mut_cert_chain_buffer,
                            &mut_cert_chain_buffer_size);
                        if (!result) {
                            return LIBSPDM_STATUS_INVALID_STATE_PEER;
                        }
                        hash_size = libspdm_get_hash_size(
                            spdm_context->connection_info.algorithm.base_hash_algo);
                        result = libspdm_hash_all(
                            spdm_context->connection_info.algorithm.base_hash_algo,
                            mut_cert_chain_buffer, mut_cert_chain_buffer_size,
                            mut_cert_chain_buffer_hash);
                        if (!result) {
                            return LIBSPDM_STATUS_CRYPTO_ERROR;
                        }
                    }
                }
            }

            /* It is first time call, backup current message_k context
             * this backup will be used in reset_message_f.*/

            LIBSPDM_ASSERT (spdm_session_info->session_transcript.digest_context_th != NULL);
            spdm_session_info->session_transcript.digest_context_th_backup = libspdm_hash_new (
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (spdm_session_info->session_transcript.digest_context_th_backup == NULL) {
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
            result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                             spdm_session_info->session_transcript.digest_context_th,
                                             spdm_session_info->session_transcript.digest_context_th_backup);
            if (!result) {
                libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                   spdm_session_info->session_transcript.digest_context_th_backup);
                spdm_session_info->session_transcript.digest_context_th_backup = NULL;
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }


        /* prepare digest_context_th*/

        LIBSPDM_ASSERT (spdm_session_info->session_transcript.digest_context_th != NULL);
        if (!spdm_session_info->session_transcript.message_f_initialized) {
            if (!spdm_session_info->use_psk && spdm_session_info->mut_auth_requested) {
                result = libspdm_hash_update (
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    spdm_session_info->session_transcript.digest_context_th,
                    mut_cert_chain_buffer_hash, hash_size);
                if (!result) {
                    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                                       spdm_session_info->session_transcript.digest_context_th);
                    spdm_session_info->session_transcript.digest_context_th = NULL;
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
            }
        }
        result = libspdm_hash_update (spdm_context->connection_info.algorithm.base_hash_algo,
                                      spdm_session_info->session_transcript.digest_context_th,
                                      message,
                                      message_size);
        if (!result) {
            libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                               spdm_session_info->session_transcript.digest_context_th);
            spdm_session_info->session_transcript.digest_context_th = NULL;
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        spdm_session_info->session_transcript.message_f_initialized = true;
        return LIBSPDM_STATUS_SUCCESS;
    }
#endif
}

/**
 * This function returns if a given version is supported based upon the GET_VERSION/VERSION.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  version                      The SPDM version.
 *
 * @retval true  the version is supported.
 * @retval false the version is not supported.
 **/
bool libspdm_is_version_supported(const libspdm_context_t *spdm_context, uint8_t version)
{
    if (version == (spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT)) {
        return true;
    }

    return false;
}

/**
 * This function returns connection version negotiated by GET_VERSION/VERSION.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @return the connection version.
 **/
uint8_t libspdm_get_connection_version(const libspdm_context_t *spdm_context)
{
    return (uint8_t)(spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT);
}

/**
 * This function returns if a capablities flag is supported in current SPDM connection.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Is the function called from a requester.
 * @param  requester_capabilities_flag    The requester capabilities flag to be checked
 * @param  responder_capabilities_flag    The responder capabilities flag to be checked
 *
 * @retval true  the capablities flag is supported.
 * @retval false the capablities flag is not supported.
 **/
bool libspdm_is_capabilities_flag_supported(const libspdm_context_t *spdm_context,
                                            bool is_requester,
                                            uint32_t requester_capabilities_flag,
                                            uint32_t responder_capabilities_flag)
{
    uint32_t negotiated_requester_capabilities_flag;
    uint32_t negotiated_responder_capabilities_flag;

    if (is_requester) {
        negotiated_requester_capabilities_flag = spdm_context->local_context.capability.flags;
        negotiated_responder_capabilities_flag = spdm_context->connection_info.capability.flags;
    } else {
        negotiated_requester_capabilities_flag = spdm_context->connection_info.capability.flags;
        negotiated_responder_capabilities_flag = spdm_context->local_context.capability.flags;
    }

    if (((requester_capabilities_flag == 0) ||
         ((negotiated_requester_capabilities_flag &
           requester_capabilities_flag) != 0)) &&
        ((responder_capabilities_flag == 0) ||
         ((negotiated_responder_capabilities_flag &
           responder_capabilities_flag) != 0))) {
        return true;
    } else {
        return false;
    }
}

/**
 * Register SPDM device input/output functions.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  send_message                  The fuction to send an SPDM transport layer message.
 * @param  receive_message               The fuction to receive an SPDM transport layer message.
 **/
void libspdm_register_device_io_func(
    void *context, libspdm_device_send_message_func send_message,
    libspdm_device_receive_message_func receive_message)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    spdm_context->send_message = send_message;
    spdm_context->receive_message = receive_message;
}

/**
 * Register SPDM device buffer management functions.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  acquire_sender_buffer         The fuction to acquire transport layer sender buffer.
 * @param  release_sender_buffer         The fuction to release transport layer sender buffer.
 * @param  acquire_receiver_buffer       The fuction to acquire transport layer receiver buffer.
 * @param  release_receiver_buffer       The fuction to release transport layer receiver buffer.
 **/
void libspdm_register_device_buffer_func(
    void *context,
    libspdm_device_acquire_sender_buffer_func acquire_sender_buffer,
    libspdm_device_release_sender_buffer_func release_sender_buffer,
    libspdm_device_acquire_receiver_buffer_func acquire_receiver_buffer,
    libspdm_device_release_receiver_buffer_func release_receiver_buffer)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    spdm_context->acquire_sender_buffer = acquire_sender_buffer;
    spdm_context->release_sender_buffer = release_sender_buffer;
    spdm_context->acquire_receiver_buffer = acquire_receiver_buffer;
    spdm_context->release_receiver_buffer = release_receiver_buffer;
}

/**
 * Register SPDM transport layer encode/decode functions for SPDM or APP messages.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  transport_encode_message       The fuction to encode an SPDM or APP message to a transport layer message.
 * @param  transport_decode_message       The fuction to decode an SPDM or APP message from a transport layer message.
 * @param  transport_get_header_size      The fuction to get the maximum transport layer message header size.
 **/
void libspdm_register_transport_layer_func(
    void *context,
    libspdm_transport_encode_message_func transport_encode_message,
    libspdm_transport_decode_message_func transport_decode_message,
    libspdm_transport_get_header_size_func transport_get_header_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    spdm_context->transport_encode_message = transport_encode_message;
    spdm_context->transport_decode_message = transport_decode_message;
    spdm_context->transport_get_header_size = transport_get_header_size;
}

/**
 * Register SPDM certificate verification functions for SPDM GET_CERTIFICATE in requester or responder.
 * It is called after GET_CERTIFICATE gets a full certificate chain from peer.
 *
 * If it is NOT registered, the default verification in SPDM lib will be used. It verifies:
 *    1) The integrity of the certificate chain, (Root Cert Hash->Root Cert->Cert Chain), according to X.509.
 *  2) The trust anchor, according LIBSPDM_DATA_PEER_PUBLIC_ROOT_CERT or LIBSPDM_DATA_PEER_PUBLIC_CERT_CHAIN.
 * If it is registered, SPDM lib will use this function to verify the certificate.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  verify_spdm_cert_chain   The fuction to verify an SPDM certificate after GET_CERTIFICATE.
 **/
void libspdm_register_verify_spdm_cert_chain_func(
    void *context,
    const libspdm_verify_spdm_cert_chain_func verify_spdm_cert_chain)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    spdm_context->local_context.verify_peer_spdm_cert_chain = verify_spdm_cert_chain;
}

/**
 * Get the size of required scratch buffer.
 *
 * The SPDM Integrator must call libspdm_get_sizeof_required_scratch_buffer to get the size,
 * then allocate enough scratch buffer and call libspdm_set_scratch_buffer().
 *
 * @param  context                  A pointer to the SPDM context.
 *
 * @return the size of required scratch buffer.
 **/
size_t libspdm_get_sizeof_required_scratch_buffer (
    void *context)
{
    return LIBSPDM_SCRATCH_BUFFER_SIZE;
}

/**
 * Set the scratch buffer.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  scratch_buffer           Buffer address of the scratch buffer.
 * @param  scratch_buffer_size      Size of the scratch buffer.
 *
 **/
void libspdm_set_scratch_buffer (
    void *context,
    void *scratch_buffer,
    size_t scratch_buffer_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    LIBSPDM_ASSERT (scratch_buffer_size >= LIBSPDM_SCRATCH_BUFFER_SIZE);
    spdm_context->scratch_buffer = scratch_buffer;
    spdm_context->scratch_buffer_size = scratch_buffer_size;
}

/**
 * Get the scratch buffer.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  scratch_buffer           Buffer address of the scratch buffer.
 * @param  scratch_buffer_size      Size of the scratch buffer.
 *
 **/
void libspdm_get_scratch_buffer (
    void *context,
    void **scratch_buffer,
    size_t *scratch_buffer_size)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    LIBSPDM_ASSERT (spdm_context->scratch_buffer != NULL);
    LIBSPDM_ASSERT (spdm_context->scratch_buffer_size >= LIBSPDM_SCRATCH_BUFFER_SIZE);
    *scratch_buffer = spdm_context->scratch_buffer;
    *scratch_buffer_size = spdm_context->scratch_buffer_size;
}

/**
 * Acquire a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  max_msg_size                  size in bytes of the maximum size of sender buffer.
 * @param  msg_buf_ptr                   A pointer to a sender buffer.
 *
 * @retval RETURN_SUCCESS               The sender buffer is acquired.
 **/
libspdm_return_t libspdm_acquire_sender_buffer (
    libspdm_context_t *spdm_context, size_t *max_msg_size, void **msg_buf_ptr)
{
    libspdm_return_t status;

    LIBSPDM_ASSERT (spdm_context->sender_buffer == NULL);
    LIBSPDM_ASSERT (spdm_context->sender_buffer_size == 0);
    status = spdm_context->acquire_sender_buffer (spdm_context, max_msg_size, msg_buf_ptr);
    if (status != LIBSPDM_STATUS_SUCCESS) {
        return status;
    }
    spdm_context->sender_buffer = *msg_buf_ptr;
    spdm_context->sender_buffer_size = *max_msg_size;
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP || LIBSPDM_ENABLE_CHUNK_CAP
    /* it return scratch buffer, because the requester need build message there.*/
    *msg_buf_ptr = spdm_context->scratch_buffer +
                   LIBSPDM_SCRATCH_BUFFER_LARGE_SENDER_RECEIVER_OFFSET;
    *max_msg_size = LIBSPDM_SCRATCH_BUFFER_LARGE_SENDER_RECEIVER_CAPACITY;
    #endif
    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Release a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The sender buffer is Released.
 **/
void libspdm_release_sender_buffer (libspdm_context_t *spdm_context)
{
    LIBSPDM_ASSERT(spdm_context->sender_buffer != NULL);
    LIBSPDM_ASSERT(spdm_context->sender_buffer_size != 0);

    spdm_context->release_sender_buffer (spdm_context, spdm_context->sender_buffer);
    spdm_context->sender_buffer = NULL;
    spdm_context->sender_buffer_size = 0;
}

/**
 * Get the sender buffer.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  receiver_buffer            Buffer address of the sender buffer.
 * @param  receiver_buffer_size       Size of the sender buffer.
 *
 **/
void libspdm_get_sender_buffer (
    libspdm_context_t *spdm_context,
    void **sender_buffer,
    size_t *sender_buffer_size)
{
    *sender_buffer = spdm_context->sender_buffer;
    *sender_buffer_size = spdm_context->sender_buffer_size;
}

/**
 * Acquire a device receiver buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  max_msg_size                  size in bytes of the maximum size of receiver buffer.
 * @param  msg_buf_pt                    A pointer to a receiver buffer.
 *
 * @retval RETURN_SUCCESS               The receiver buffer is acquired.
 **/
libspdm_return_t libspdm_acquire_receiver_buffer (
    libspdm_context_t *spdm_context, size_t *max_msg_size, void **msg_buf_ptr)
{
    libspdm_return_t status;

    LIBSPDM_ASSERT (spdm_context->receiver_buffer == NULL);
    LIBSPDM_ASSERT (spdm_context->receiver_buffer_size == 0);
    status = spdm_context->acquire_receiver_buffer (spdm_context, max_msg_size, msg_buf_ptr);
    if (status != LIBSPDM_STATUS_SUCCESS) {
        return status;
    }
    spdm_context->receiver_buffer = *msg_buf_ptr;
    spdm_context->receiver_buffer_size = *max_msg_size;
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP || LIBSPDM_ENABLE_CHUNK_CAP
    /* it return scratch buffer, because the requester need build message there.*/
    *msg_buf_ptr = spdm_context->scratch_buffer +
                   LIBSPDM_SCRATCH_BUFFER_LARGE_SENDER_RECEIVER_OFFSET;
    *max_msg_size = LIBSPDM_SCRATCH_BUFFER_LARGE_SENDER_RECEIVER_CAPACITY;
    #endif
    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Release a device receiver buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The receiver buffer is Released.
 **/
void libspdm_release_receiver_buffer (libspdm_context_t *spdm_context)
{
    LIBSPDM_ASSERT(spdm_context->receiver_buffer != NULL);
    LIBSPDM_ASSERT(spdm_context->receiver_buffer_size != 0);

    spdm_context->release_receiver_buffer (spdm_context, spdm_context->receiver_buffer);
    spdm_context->receiver_buffer = NULL;
    spdm_context->receiver_buffer_size = 0;
}

/**
 * Get the receiver buffer.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  receiver_buffer            Buffer address of the receiver buffer.
 * @param  receiver_buffer_size       Size of the receiver buffer.
 *
 **/
void libspdm_get_receiver_buffer (
    libspdm_context_t *spdm_context,
    void **receiver_buffer,
    size_t *receiver_buffer_size)
{
    *receiver_buffer = spdm_context->receiver_buffer;
    *receiver_buffer_size = spdm_context->receiver_buffer_size;
}

/**
 * Get the last SPDM error struct of an SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM context.
 */
void libspdm_get_last_spdm_error_struct(void *context, libspdm_error_struct_t *last_spdm_error)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    libspdm_copy_mem(last_spdm_error, sizeof(libspdm_error_struct_t),
                     &spdm_context->last_spdm_error,sizeof(libspdm_error_struct_t));
}

/**
 * Set the last SPDM error struct of an SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM context.
 */
void libspdm_set_last_spdm_error_struct(void *context, libspdm_error_struct_t *last_spdm_error)
{
    libspdm_context_t *spdm_context;

    spdm_context = context;
    libspdm_copy_mem(&spdm_context->last_spdm_error, sizeof(spdm_context->last_spdm_error),
                     last_spdm_error, sizeof(libspdm_error_struct_t));
}

/**
 * Initialize an SPDM context, as well as all secured message contexts,
 * in the specified locations.
 *
 * The size in bytes of the spdm_context can be returned by
 * libspdm_get_context_size_without_secured_context.
 *
 * The size in bytes of a single secured message context can be returned by
 * libspdm_secured_message_get_context_size.
 *
 * @param  spdm_context          A pointer to the SPDM context.
 * @param  secured_contexts      An array of pointers, with each entry containing
 *                               the location of a secured message context.
 * @param  num_secured_contexts  Number of secured message contexts to initialize.
 *                               Currently, only LIBSPDM_MAX_SESSION_COUNT is supported.
 *
 * @retval RETURN_SUCCESS        Contexts are initialized.
 * @retval RETURN_DEVICE_ERROR   Context initialization failed.
 */
libspdm_return_t libspdm_init_context_with_secured_context(void *context,
                                                           void **secured_contexts,
                                                           size_t num_secured_contexts)
{
    libspdm_context_t *spdm_context;
    size_t index;

    LIBSPDM_ASSERT(context != NULL);
    LIBSPDM_ASSERT(secured_contexts != NULL);
    LIBSPDM_ASSERT(num_secured_contexts == LIBSPDM_MAX_SESSION_COUNT);

    spdm_context = context;
    libspdm_zero_mem(spdm_context, sizeof(libspdm_context_t));
    spdm_context->version = libspdm_context_struct_version;
    spdm_context->transcript.message_a.max_buffer_size =
        sizeof(spdm_context->transcript.message_a.buffer);
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    spdm_context->transcript.message_b.max_buffer_size =
        sizeof(spdm_context->transcript.message_b.buffer);
    spdm_context->transcript.message_c.max_buffer_size =
        sizeof(spdm_context->transcript.message_c.buffer);
    spdm_context->transcript.message_mut_b.max_buffer_size =
        sizeof(spdm_context->transcript.message_mut_b.buffer);
    spdm_context->transcript.message_mut_c.max_buffer_size =
        sizeof(spdm_context->transcript.message_mut_c.buffer);
    spdm_context->transcript.message_m.max_buffer_size =
        sizeof(spdm_context->transcript.message_m.buffer);
#endif
    spdm_context->retry_times = LIBSPDM_MAX_REQUEST_RETRY_TIMES;
    spdm_context->response_state = LIBSPDM_RESPONSE_STATE_NORMAL;
    spdm_context->current_token = 0;
    spdm_context->local_context.version.spdm_version_count = 3;
    spdm_context->local_context.version.spdm_version[0] = SPDM_MESSAGE_VERSION_10 <<
                                                          SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->local_context.version.spdm_version[1] = SPDM_MESSAGE_VERSION_11 <<
                                                          SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->local_context.version.spdm_version[2] = SPDM_MESSAGE_VERSION_12 <<
                                                          SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->local_context.secured_message_version.spdm_version_count = 2;
    spdm_context->local_context.secured_message_version.spdm_version[0] =
        SPDM_MESSAGE_VERSION_10 << SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->local_context.secured_message_version.spdm_version[1] =
        SPDM_MESSAGE_VERSION_11 << SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->local_context.capability.st1 = SPDM_ST1_VALUE_US;

    spdm_context->encap_context.certificate_chain_buffer.max_buffer_size =
        sizeof(spdm_context->encap_context.certificate_chain_buffer.buffer);

    /* From the config.h, need different value for CHUNK - TBD*/
    spdm_context->local_context.capability.data_transfer_size = LIBSPDM_DATA_TRANSFER_SIZE;
    spdm_context->local_context.capability.max_spdm_msg_size = LIBSPDM_MAX_SPDM_MSG_SIZE;

    for (index = 0; index < num_secured_contexts; index++) {
        if (secured_contexts[index] == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }

        spdm_context->session_info[index].secured_message_context = secured_contexts[index];
        libspdm_secured_message_init_context(
            spdm_context->session_info[index]
            .secured_message_context);
    }

    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Initialize an SPDM context, as well as secured message contexts.
 * The secured message contexts are appended to the context structure.
 *
 * The total size in bytes of the spdm_context and all secured message
 * contexts can be returned by libspdm_get_context_size().
 *
 * @param  spdm_context         A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS       context is initialized.
 * @retval RETURN_DEVICE_ERROR  context initialization failed.
 */
libspdm_return_t libspdm_init_context(void *context)
{
    libspdm_context_t *spdm_context;
    void *secured_context;
    void *secured_contexts[LIBSPDM_MAX_SESSION_COUNT];
    size_t secured_context_size;
    size_t index;

    LIBSPDM_ASSERT(context != NULL);

    /* libspdm_get_context_size() allocates space for all secured message
     * contexts. They are appended to the general SPDM context. */
    spdm_context = context;
    secured_context = (void *)((size_t)(spdm_context + 1));
    secured_context_size = libspdm_secured_message_get_context_size();

    for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++)
    {
        secured_contexts[index] =
            (uint8_t *)secured_context + secured_context_size * index;
    }

    return libspdm_init_context_with_secured_context(context,
                                                     secured_contexts,
                                                     LIBSPDM_MAX_SESSION_COUNT);
}

/**
 * Reset an SPDM context.
 *
 * The size in bytes of the spdm_context can be returned by libspdm_get_context_size.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 */
void libspdm_reset_context(void *context)
{
    libspdm_context_t *spdm_context;
    size_t index;

    spdm_context = context;

    /*Clear all info about last connection*/

    /*need clear session info to free context before algo is zeroed.*/
    for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++)
    {
        libspdm_session_info_init(spdm_context,
                                  &spdm_context->session_info[index],
                                  INVALID_SESSION_ID,
                                  false);
    }

    spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_NOT_STARTED;
    libspdm_zero_mem(&spdm_context->connection_info.version, sizeof(spdm_version_number_t));
    libspdm_zero_mem(&spdm_context->connection_info.capability,
                     sizeof(libspdm_device_capability_t));
    libspdm_zero_mem(&spdm_context->connection_info.algorithm, sizeof(libspdm_device_algorithm_t));
    libspdm_zero_mem(&spdm_context->last_spdm_error, sizeof(libspdm_error_struct_t));
    libspdm_zero_mem(&spdm_context->encap_context, sizeof(libspdm_encap_context_t));
    spdm_context->connection_info.local_used_cert_chain_buffer_size = 0;
    spdm_context->connection_info.local_used_cert_chain_buffer = NULL;
    spdm_context->cache_spdm_request_size = 0;
    spdm_context->retry_times = LIBSPDM_MAX_REQUEST_RETRY_TIMES;
    spdm_context->response_state = LIBSPDM_RESPONSE_STATE_NORMAL;
    spdm_context->current_token = 0;
    spdm_context->last_spdm_request_session_id = INVALID_SESSION_ID;
    spdm_context->last_spdm_request_session_id_valid = false;
    spdm_context->last_spdm_request_size = 0;
    spdm_context->encap_context.certificate_chain_buffer.max_buffer_size =
        sizeof(spdm_context->encap_context.certificate_chain_buffer.buffer);

    #if LIBSPDM_ENABLE_MSG_LOG
    spdm_context->msg_log.buffer = NULL;
    spdm_context->msg_log.max_buffer_size = 0;
    spdm_context->msg_log.buffer_size = 0;
    spdm_context->msg_log.mode = 0;
    spdm_context->msg_log.status = 0;
    #endif /* LIBSPDM_ENABLE_MSG_LOG */
}

/**
 * Free the memory of contexts within the SPDM context.
 * These are typically contexts whose memory has been allocated by the cryptography library.
 * This function does not free the SPDM context itself.
 *
 * @param[in]  spdm_context         A pointer to the SPDM context.
 *
 */
void libspdm_deinit_context(void *context)
{
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT)
    libspdm_context_t *spdm_context;
    void *pubkey_context;
    bool is_requester;
    uint8_t slot_index;

    spdm_context = context;
    is_requester = spdm_context->local_context.is_requester;

    for (slot_index = 0; slot_index < SPDM_MAX_SLOT_COUNT; slot_index++) {
        pubkey_context = spdm_context->connection_info.peer_used_cert_chain[slot_index].
                         leaf_cert_public_key;

        if (pubkey_context != NULL) {
            if (is_requester) {
                libspdm_asym_free(
                    spdm_context->connection_info.algorithm.base_asym_algo, pubkey_context);
            } else {
                libspdm_req_asym_free(
                    spdm_context->connection_info.algorithm.req_base_asym_alg, pubkey_context);
            }

            pubkey_context = NULL;
            spdm_context->connection_info.peer_used_cert_chain[slot_index].
            leaf_cert_public_key = NULL;
        }
    }

#endif
}

/**
 * Return the size in bytes of the SPDM context. This includes all
 * secured message context data as well.
 *
 * For just the SPDM context size, use libspdm_get_context_size_without_secured_context.
 *
 * @return the size in bytes of the SPDM context and secured message contexts.
 **/
size_t libspdm_get_context_size(void)
{
    size_t size;

    size = sizeof(libspdm_context_t) +
           libspdm_secured_message_get_context_size() * LIBSPDM_MAX_SESSION_COUNT;
    LIBSPDM_ASSERT (size == LIBSPDM_CONTEXT_SIZE_ALL);
    return size;
}

/**
 * Return the size in bytes of just the SPDM context, without secured message context.
 *
 * For the complete context size, use libspdm_get_context_size.
 *
 * @return the size in bytes of the SPDM context.
 **/
size_t libspdm_get_context_size_without_secured_context(void)
{
    size_t size;

    size = sizeof(libspdm_context_t);
    LIBSPDM_ASSERT (size == LIBSPDM_CONTEXT_SIZE_WITHOUT_SECURED_CONTEXT);
    return size;
}

/**
 * Return the SPDMversion field of the version number struct.
 *
 * @param  ver                Spdm version number struct.
 *
 * @return the SPDMversion of the version number struct.
 **/
uint8_t libspdm_get_version_from_version_number(const spdm_version_number_t ver)
{
    return (uint8_t)(ver >> SPDM_VERSION_NUMBER_SHIFT_BIT);
}

/**
 * Sort SPDMversion in descending order.
 *
 * @param  spdm_context                A pointer to the SPDM context.
 * @param  ver_set                    A pointer to the version set.
 * @param  ver_num                    Version number.
 */
void libspdm_version_number_sort(spdm_version_number_t *ver_set, size_t ver_num)
{
    size_t index;
    size_t index_sort;
    size_t index_max;
    spdm_version_number_t version;

    /* Select sort */
    if (ver_num > 1) {
        for (index_sort = 0; index_sort < ver_num; index_sort++) {
            index_max = index_sort;
            for (index = index_sort + 1; index < ver_num; index++) {
                /* if ver_ser[index] higher than ver_set[index_max] */
                if (ver_set[index] > ver_set[index_max]) {
                    index_max = index;
                }
            }
            /* swap ver_ser[index_min] and ver_set[index_sort] */
            version = ver_set[index_sort];
            ver_set[index_sort] = ver_set[index_max];
            ver_set[index_max] = version;
        }
    }
}

/**
 * Negotiate SPDMversion for connection.
 * ver_set is the local version set of requester, res_ver_set is the version set of responder.
 *
 * @param  common_version             A pointer to store the common version.
 * @param  req_ver_set                A pointer to the requester version set.
 * @param  req_ver_num                Version number of requester.
 * @param  res_ver_set                A pointer to the responder version set.
 * @param  res_ver_num                Version number of responder.
 *
 * @retval true                       Negotiation successfully, connect version be saved to common_version.
 * @retval false                      Negotiation failed.
 */
bool libspdm_negotiate_connection_version(spdm_version_number_t *common_version,
                                          spdm_version_number_t *req_ver_set,
                                          size_t req_ver_num,
                                          const spdm_version_number_t *res_ver_set,
                                          size_t res_ver_num)
{
    spdm_version_number_t req_version_list[LIBSPDM_MAX_VERSION_COUNT];
    spdm_version_number_t res_version_list[LIBSPDM_MAX_VERSION_COUNT];
    size_t req_index;
    size_t res_index;

    if (req_ver_num > LIBSPDM_MAX_VERSION_COUNT || res_ver_num > LIBSPDM_MAX_VERSION_COUNT) {
        return false;
    }

    if (req_ver_set == NULL || req_ver_num == 0 || res_ver_set == NULL || res_ver_num == 0) {
        return false;
    }

    libspdm_zero_mem(req_version_list, sizeof(spdm_version_number_t) * LIBSPDM_MAX_VERSION_COUNT);
    libspdm_zero_mem(res_version_list, sizeof(spdm_version_number_t) * LIBSPDM_MAX_VERSION_COUNT);

    libspdm_copy_mem(req_version_list, sizeof(spdm_version_number_t) * LIBSPDM_MAX_VERSION_COUNT,
                     req_ver_set, sizeof(spdm_version_number_t) * req_ver_num);
    libspdm_copy_mem(res_version_list, sizeof(spdm_version_number_t) * LIBSPDM_MAX_VERSION_COUNT,
                     res_ver_set, sizeof(spdm_version_number_t) * res_ver_num);

    /* Sort SPDMversion in descending order. */
    libspdm_version_number_sort(req_version_list, req_ver_num);
    libspdm_version_number_sort(res_version_list, res_ver_num);

    /**
     * Find highest same version and make req_index point to it.
     * If not found, return false.
     **/
    for (res_index = 0; res_index < res_ver_num; res_index++) {
        for (req_index = 0; req_index < req_ver_num; req_index++) {
            if (libspdm_get_version_from_version_number(req_version_list[req_index]) ==
                libspdm_get_version_from_version_number(res_version_list[res_index])) {
                *common_version = req_version_list[req_index];
                return true;
            }
        }
    }
    return false;
}
