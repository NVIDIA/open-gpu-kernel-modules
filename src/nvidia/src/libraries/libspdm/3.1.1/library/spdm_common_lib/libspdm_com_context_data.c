/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_common_lib.h"
#include "internal/libspdm_secured_message_lib.h"
#include "internal/libspdm_fips_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
/* first section */
uint32_t libspdm_get_scratch_buffer_secure_message_offset(libspdm_context_t *spdm_context) {
    return 0;
}

uint32_t libspdm_get_scratch_buffer_secure_message_capacity(libspdm_context_t *spdm_context) {
    return spdm_context->local_context.capability.max_spdm_msg_size +
           spdm_context->local_context.capability.transport_header_size +
           spdm_context->local_context.capability.transport_tail_size;
}

/* second section */
uint32_t libspdm_get_scratch_buffer_large_message_offset(libspdm_context_t *spdm_context) {
    return libspdm_get_scratch_buffer_secure_message_capacity(spdm_context);
}

uint32_t libspdm_get_scratch_buffer_large_message_capacity(libspdm_context_t *spdm_context) {
    return spdm_context->local_context.capability.max_spdm_msg_size;
}
#endif

/* third section */
uint32_t libspdm_get_scratch_buffer_sender_receiver_offset(libspdm_context_t *spdm_context) {
    return 0 +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_secure_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_large_message_capacity(spdm_context) +
#endif
           0;
}

uint32_t libspdm_get_scratch_buffer_sender_receiver_capacity(libspdm_context_t *spdm_context) {
    return spdm_context->local_context.capability.max_spdm_msg_size +
           spdm_context->local_context.capability.transport_header_size +
           spdm_context->local_context.capability.transport_tail_size;
}

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
/* fourth section */
uint32_t libspdm_get_scratch_buffer_large_sender_receiver_offset(libspdm_context_t *spdm_context) {
    return libspdm_get_scratch_buffer_secure_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_large_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context);
}

uint32_t libspdm_get_scratch_buffer_large_sender_receiver_capacity(libspdm_context_t *spdm_context)
{
    return spdm_context->local_context.capability.max_spdm_msg_size +
           spdm_context->local_context.capability.transport_header_size +
           spdm_context->local_context.capability.transport_tail_size;
}
#endif

/* fifth section */
uint32_t libspdm_get_scratch_buffer_last_spdm_request_offset(libspdm_context_t *spdm_context) {
    return 0 +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_secure_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_large_message_capacity(spdm_context) +
#endif
           libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context) +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context) +
#endif
           0;
}

uint32_t libspdm_get_scratch_buffer_last_spdm_request_capacity(libspdm_context_t *spdm_context) {
    return spdm_context->local_context.capability.max_spdm_msg_size;
}

#if LIBSPDM_RESPOND_IF_READY_SUPPORT
/* sixth section */
uint32_t libspdm_get_scratch_buffer_cache_spdm_request_offset(libspdm_context_t *spdm_context) {
    return 0 +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_secure_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_large_message_capacity(spdm_context) +
#endif
           libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context) +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context) +
#endif
           libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context) +
           0;
}

uint32_t libspdm_get_scratch_buffer_cache_spdm_request_capacity(libspdm_context_t *spdm_context) {
    return spdm_context->local_context.capability.max_spdm_msg_size;
}
#endif

/* combination */
uint32_t libspdm_get_scratch_buffer_capacity(libspdm_context_t *spdm_context) {
    return 0 +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_secure_message_capacity(spdm_context) +
           libspdm_get_scratch_buffer_large_message_capacity(spdm_context) +
#endif
           libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context) +
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
           libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context) +
#endif
           libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context) +
#if LIBSPDM_RESPOND_IF_READY_SUPPORT
           libspdm_get_scratch_buffer_cache_spdm_request_capacity(spdm_context) +
#endif
           0;
}

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
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_RSP_DIR:
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_REQ_DIR:
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_ENDIAN:
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
libspdm_return_t libspdm_set_data(void *spdm_context, libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter, void *data,
                                  size_t data_size)
{
    libspdm_context_t *context;
    uint32_t session_id;
    uint32_t data32;
    libspdm_session_info_t *session_info;
    uint8_t slot_id;
    uint8_t mut_auth_requested;
    uint8_t root_cert_index;
    uint16_t data16;
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) && LIBSPDM_CERT_PARSE_SUPPORT
    bool status;
    const uint8_t *cert_buffer;
    size_t cert_buffer_size;
#endif

    if (spdm_context == NULL || data == NULL || data_type >= LIBSPDM_DATA_MAX) {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    context = spdm_context;

    if (need_session_info_for_data(data_type)) {
        if (parameter->location != LIBSPDM_DATA_LOCATION_SESSION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_id = libspdm_read_uint32(parameter->additional_data);
        session_info = libspdm_get_session_info_via_session_id(context, session_id);
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
            libspdm_copy_mem(&(context->connection_info.version),
                             sizeof(context->connection_info.version),
                             data,
                             sizeof(spdm_version_number_t));
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.version.spdm_version_count =
                (uint8_t)(data_size / sizeof(spdm_version_number_t));
            libspdm_copy_mem(context->local_context.version.spdm_version,
                             sizeof(context->local_context.version.spdm_version),
                             data,
                             context->local_context.version.spdm_version_count *
                             sizeof(spdm_version_number_t));
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_SECURED_MESSAGE_VERSION:
        LIBSPDM_ASSERT (data_size <=
                        sizeof(spdm_version_number_t) * SECURED_SPDM_MAX_VERSION_COUNT);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            /* Only have one connected version */
            LIBSPDM_ASSERT (data_size == sizeof(spdm_version_number_t));
            libspdm_copy_mem(&(context->connection_info.secured_message_version),
                             sizeof(context->connection_info.secured_message_version),
                             data,
                             sizeof(spdm_version_number_t));
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.secured_message_version
            .spdm_version_count = (uint8_t)(data_size / sizeof(spdm_version_number_t));
            libspdm_copy_mem(context->local_context
                             .secured_message_version.spdm_version,
                             sizeof(context->local_context
                                    .secured_message_version.spdm_version),
                             data,
                             context->local_context.secured_message_version.
                             spdm_version_count * sizeof(spdm_version_number_t));
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_FLAGS:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }

        data32 = libspdm_read_uint32((const uint8_t *)data);

        if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            #if !(LIBSPDM_ENABLE_CAPABILITY_CERT_CAP)
            LIBSPDM_ASSERT((data32 & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP) == 0);
            #endif /* !LIBSPDM_ENABLE_CAPABILITY_CERT_CAP */

            #if !(LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP)
            LIBSPDM_ASSERT((data32 & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHAL_CAP) == 0);
            #endif /* !LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP */

            #if !(LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP)
            LIBSPDM_ASSERT((data32 & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP) == 0);
            #endif /* !LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP */

            #if !(LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP)
            LIBSPDM_ASSERT((data32 & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP) == 0);
            #endif /* !LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP */

            #if !(LIBSPDM_ENABLE_CAPABILITY_PSK_CAP)
            LIBSPDM_ASSERT((data32 & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP) == 0);
            #endif /* !LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */

            context->local_context.capability.flags = data32;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.capability.flags = data32;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_CT_EXPONENT:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.capability.ct_exponent = *(uint8_t *)data;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.capability.ct_exponent = *(uint8_t *)data;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_RTT_US:
        if (data_size != sizeof(uint64_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.capability.rtt = libspdm_read_uint64((const uint8_t *)data);
        break;
    case LIBSPDM_DATA_CAPABILITY_MAX_SPDM_MSG_SIZE:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        /* The local max_spdm_msg_size is set by libspdm_register_transport_layer_func.
         * Only the connection's max_spdm_msg_size is settable here. */
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data32 = libspdm_read_uint32((const uint8_t *)data);
        LIBSPDM_ASSERT (data32 >= SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12);
        context->connection_info.capability.max_spdm_msg_size = data32;
        break;
    case LIBSPDM_DATA_MEASUREMENT_SPEC:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.measurement_spec = *(uint8_t *)data;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.measurement_spec = *(uint8_t *)data;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_MEASUREMENT_HASH_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data32 = libspdm_read_uint32((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.measurement_hash_algo = data32;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.measurement_hash_algo = data32;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_BASE_ASYM_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data32 = libspdm_read_uint32((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.base_asym_algo = data32;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.base_asym_algo = data32;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_BASE_HASH_ALGO:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data32 = libspdm_read_uint32((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.base_hash_algo = data32;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.base_hash_algo = data32;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_DHE_NAME_GROUP:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data16 = libspdm_read_uint16((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.dhe_named_group = data16;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.dhe_named_group = data16;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_AEAD_CIPHER_SUITE:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data16 = libspdm_read_uint16((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.aead_cipher_suite = data16;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.aead_cipher_suite = data16;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_REQ_BASE_ASYM_ALG:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data16 = libspdm_read_uint16((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.req_base_asym_alg = data16;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.req_base_asym_alg = data16;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_KEY_SCHEDULE:
        if (data_size != sizeof(uint16_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        data16 = libspdm_read_uint16((const uint8_t *)data);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.key_schedule = data16;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.key_schedule = data16;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_OTHER_PARAMS_SUPPORT:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            context->connection_info.algorithm.other_params_support = *(uint8_t *)data;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            context->local_context.algorithm.other_params_support = *(uint8_t *)data;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CONNECTION_STATE:
        if (data_size != sizeof(libspdm_connection_state_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->connection_info.connection_state = libspdm_read_uint32((const uint8_t *)data);
        break;
    case LIBSPDM_DATA_RESPONSE_STATE:
        if (data_size != sizeof(libspdm_response_state_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->response_state = libspdm_read_uint32((const uint8_t *)data);
        break;
    case LIBSPDM_DATA_PEER_PUBLIC_ROOT_CERT:
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        root_cert_index = 0;
        while (context->local_context.peer_root_cert_provision[root_cert_index] != NULL) {
            root_cert_index++;
            if (root_cert_index >= LIBSPDM_MAX_ROOT_CERT_SUPPORT) {
                return LIBSPDM_STATUS_BUFFER_FULL;
            }
        }
        context->local_context.peer_root_cert_provision_size[root_cert_index] = data_size;
        context->local_context.peer_root_cert_provision[root_cert_index] = data;
        break;
    case LIBSPDM_DATA_LOCAL_PUBLIC_CERT_CHAIN:
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        slot_id = parameter->additional_data[0];
        if (slot_id >= SPDM_MAX_SLOT_COUNT) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.local_cert_chain_provision_size[slot_id] = data_size;
        context->local_context.local_cert_chain_provision[slot_id] = data;
        break;
    case LIBSPDM_DATA_PEER_USED_CERT_CHAIN_BUFFER:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        slot_id = parameter->additional_data[0];
        if (slot_id >= SPDM_MAX_SLOT_COUNT) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (data_size > LIBSPDM_MAX_CERT_CHAIN_SIZE) {
            return LIBSPDM_STATUS_BUFFER_FULL;
        }
        context->connection_info.peer_used_cert_chain_slot_id = slot_id;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        context->connection_info.peer_used_cert_chain[slot_id].buffer_size = data_size;
        libspdm_copy_mem(context->connection_info.peer_used_cert_chain[slot_id].buffer,
                         sizeof(context->connection_info.peer_used_cert_chain[slot_id].buffer),
                         data, data_size);
#else
#if LIBSPDM_CERT_PARSE_SUPPORT
        status = libspdm_hash_all(
            context->connection_info.algorithm.base_hash_algo,
            data, data_size,
            context->connection_info.peer_used_cert_chain[slot_id].buffer_hash);
        if (!status) {
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size =
            libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo);

        /*process the SPDM cert header and hash*/
        data = (uint8_t *)data + sizeof(spdm_cert_chain_t) +
               libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo);
        data_size = data_size -
                    (sizeof(spdm_cert_chain_t) +
                     libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo));

        /* Get leaf cert from cert chain */
        status = libspdm_x509_get_cert_from_cert_chain(data, data_size, -1,
                                                       &cert_buffer, &cert_buffer_size);
        if (!status) {
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        status = false;
#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
        if (!status) {
            status = libspdm_rsa_get_public_key_from_x509(
                cert_buffer, cert_buffer_size,
                &context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if LIBSPDM_ECDSA_SUPPORT
        if (!status) {
            status = libspdm_ec_get_public_key_from_x509(
                cert_buffer, cert_buffer_size,
                &context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
        if (!status) {
            status = libspdm_ecd_get_public_key_from_x509(
                cert_buffer, cert_buffer_size,
                &context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
#if LIBSPDM_SM2_DSA_SUPPORT
        if (!status) {
            status = libspdm_sm2_get_public_key_from_x509(
                cert_buffer, cert_buffer_size,
                &context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
        }
#endif
        if (!status) {
            return LIBSPDM_STATUS_INVALID_CERT;
        }
#else
        LIBSPDM_ASSERT (false);
#endif /* LIBSPDM_CERT_PARSE_SUPPORT */
#endif /* LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT */
        break;
    case LIBSPDM_DATA_PEER_PUBLIC_KEY:
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.peer_public_key_provision_size = data_size;
        context->local_context.peer_public_key_provision = data;
        break;
    case LIBSPDM_DATA_LOCAL_PUBLIC_KEY:
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.local_public_key_provision_size = data_size;
        context->local_context.local_public_key_provision = data;
        break;
    case LIBSPDM_DATA_BASIC_MUT_AUTH_REQUESTED:
        if (data_size != sizeof(bool)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        mut_auth_requested = *(uint8_t *)data;
        if (((mut_auth_requested != 0) && (mut_auth_requested != 1))) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.basic_mut_auth_requested = mut_auth_requested;
        context->encap_context.request_id = 0;
        slot_id = parameter->additional_data[0];
        if ((slot_id >= SPDM_MAX_SLOT_COUNT) && (slot_id != 0xFF)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->encap_context.req_slot_id = slot_id;

        #if LIBSPDM_DEBUG_PRINT_ENABLE
        if (mut_auth_requested) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "Basic mutual authentication is a deprecated feature.\n"));
        }
        #endif /* LIBSPDM_DEBUG_PRINT_ENABLE */
        break;
    case LIBSPDM_DATA_MUT_AUTH_REQUESTED:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
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
        context->local_context.mut_auth_requested = mut_auth_requested;
        context->encap_context.request_id = 0;
        slot_id = parameter->additional_data[0];
        if ((slot_id >= SPDM_MAX_SLOT_COUNT) && (slot_id != 0xFF)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->encap_context.req_slot_id = slot_id;
        break;
    case LIBSPDM_DATA_HEARTBEAT_PERIOD:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.heartbeat_period = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_APP_CONTEXT_DATA:
        if (data_size != sizeof(void *) || *(void **)data == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->app_context_data_ptr = *(void **)data;
        break;
    case LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->handle_error_return_policy = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_VCA_CACHE:
        if (data_size > sizeof(context->transcript.message_a.buffer)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->transcript.message_a.buffer_size = data_size;
        libspdm_copy_mem(context->transcript.message_a.buffer,
                         sizeof(context->transcript.message_a.buffer),
                         data, data_size);
        break;
    case LIBSPDM_DATA_IS_REQUESTER:
        if (data_size != sizeof(bool)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (parameter->location != LIBSPDM_DATA_LOCATION_LOCAL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->local_context.is_requester = *(bool *)data;
        break;
    case LIBSPDM_DATA_REQUEST_RETRY_TIMES:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->retry_times = *(uint8_t *)data;
        break;
    case LIBSPDM_DATA_REQUEST_RETRY_DELAY_TIME:
        if (data_size != sizeof(uint64_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->retry_delay_time = *(uint64_t *)data;
        break;
    case LIBSPDM_DATA_MAX_DHE_SESSION_COUNT:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (*(uint32_t *)data > LIBSPDM_MAX_SESSION_COUNT - context->max_psk_session_count) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->max_dhe_session_count = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_MAX_PSK_SESSION_COUNT:
        if (data_size != sizeof(uint32_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (*(uint32_t *)data > LIBSPDM_MAX_SESSION_COUNT - context->max_dhe_session_count) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->max_psk_session_count = *(uint32_t *)data;
        break;
    case LIBSPDM_DATA_MAX_SPDM_SESSION_SEQUENCE_NUMBER:
        if (data_size != sizeof(uint64_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->max_spdm_session_sequence_number = *(uint64_t *)data;
        if (context->max_spdm_session_sequence_number == 0) {
            context->max_spdm_session_sequence_number = LIBSPDM_MAX_SPDM_SESSION_SEQUENCE_NUMBER;
        }
        break;
    case LIBSPDM_DATA_SPDM_VERSION_10_11_VERIFY_SIGNATURE_ENDIAN:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        if (*(uint8_t*)data != LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY &&
            *(uint8_t*)data != LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY &&
            *(uint8_t*)data != LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->spdm_10_11_verify_signature_endian = *(uint8_t*)data;
        break;
    case LIBSPDM_DATA_SEQUENCE_NUMBER_ENDIAN:
        if (data_size != sizeof(uint8_t)) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        context->sequence_number_endian = *(uint8_t *)data;
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
libspdm_return_t libspdm_get_data(void *spdm_context, libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter,
                                  void *data, size_t *data_size)
{
    libspdm_context_t *context;
    libspdm_secured_message_context_t *secured_context;
    size_t target_data_size;
    void *target_data;
    uint32_t session_id;
    libspdm_session_info_t *session_info;
    size_t digest_size;
    size_t digest_count;
    size_t index;

    if (spdm_context == NULL || data == NULL || data_size == NULL ||
        data_type >= LIBSPDM_DATA_MAX) {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    context = spdm_context;

    //
    // NVIDIA_EDIT: While in practice, secured_context will never be NULL with below logic,
    // compiler does not see this and complains of potential uninitialized use.
    //
    secured_context = NULL;

    if (data_type == LIBSPDM_DATA_SESSION_END_SESSION_ATTRIBUTES) {
        /* end_session_attributes is present in both a session context as well as an
         * spdm context. */
        session_id = libspdm_read_uint32(parameter->additional_data);
        session_info = libspdm_get_session_info_via_session_id(context, session_id);
    } else if (need_session_info_for_data(data_type)) {
        if (parameter->location != LIBSPDM_DATA_LOCATION_SESSION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        session_id = libspdm_read_uint32(parameter->additional_data);
        session_info = libspdm_get_session_info_via_session_id(context, session_id);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        secured_context = session_info->secured_message_context;
    } else {
        session_info = NULL;
    }

    switch (data_type) {
    case LIBSPDM_DATA_SPDM_VERSION:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(spdm_version_number_t);
        target_data = &(context->connection_info.version);
        break;
    case LIBSPDM_DATA_SECURED_MESSAGE_VERSION:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(spdm_version_number_t);
        target_data = &(context->connection_info.secured_message_version);
        break;
    case LIBSPDM_DATA_CAPABILITY_FLAGS:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &context->connection_info.capability.flags;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            target_data = &context->local_context.capability.flags;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_CT_EXPONENT:
        target_data_size = sizeof(uint8_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &context->connection_info.capability.ct_exponent;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            target_data = &context->local_context.capability.ct_exponent;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_DATA_TRANSFER_SIZE:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &context->connection_info.capability.data_transfer_size;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            target_data = &context->local_context.capability.data_transfer_size;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_MAX_SPDM_MSG_SIZE:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            target_data = &context->connection_info.capability.max_spdm_msg_size;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            target_data = &context->local_context.capability.max_spdm_msg_size;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_CAPABILITY_SENDER_DATA_TRANSFER_SIZE:
        target_data_size = sizeof(uint32_t);
        if (parameter->location == LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        } else if (parameter->location == LIBSPDM_DATA_LOCATION_LOCAL) {
            target_data = &context->local_context.capability.sender_data_transfer_size;
        } else {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        break;
    case LIBSPDM_DATA_MEASUREMENT_SPEC:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &context->connection_info.algorithm.measurement_spec;
        break;
    case LIBSPDM_DATA_MEASUREMENT_HASH_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &context->connection_info.algorithm.measurement_hash_algo;
        break;
    case LIBSPDM_DATA_BASE_ASYM_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &context->connection_info.algorithm.base_asym_algo;
        break;
    case LIBSPDM_DATA_BASE_HASH_ALGO:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint32_t);
        target_data = &context->connection_info.algorithm.base_hash_algo;
        break;
    case LIBSPDM_DATA_DHE_NAME_GROUP:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &context->connection_info.algorithm.dhe_named_group;
        break;
    case LIBSPDM_DATA_AEAD_CIPHER_SUITE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &context->connection_info.algorithm.aead_cipher_suite;
        break;
    case LIBSPDM_DATA_REQ_BASE_ASYM_ALG:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &context->connection_info.algorithm.req_base_asym_alg;
        break;
    case LIBSPDM_DATA_KEY_SCHEDULE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint16_t);
        target_data = &context->connection_info.algorithm.key_schedule;
        break;
    case LIBSPDM_DATA_OTHER_PARAMS_SUPPORT:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &context->connection_info.algorithm.other_params_support;
        break;
    case LIBSPDM_DATA_CONNECTION_STATE:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(libspdm_connection_state_t);
        target_data = &context->connection_info.connection_state;
        break;
    case LIBSPDM_DATA_RESPONSE_STATE:
        target_data_size = sizeof(libspdm_response_state_t);
        target_data = &context->response_state;
        break;
    case LIBSPDM_DATA_PEER_SLOT_MASK:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &context->connection_info.peer_digest_slot_mask;
        break;
    case LIBSPDM_DATA_PEER_TOTAL_DIGEST_BUFFER:
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        digest_count = 0;
        for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
            if (context->connection_info.peer_digest_slot_mask & (1 << index)) {
                digest_count++;
            }
        }
        digest_size = libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo);
        target_data_size = digest_size * digest_count;
        target_data = context->connection_info.peer_total_digest_buffer;
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
        if (parameter->location != LIBSPDM_DATA_LOCATION_CONNECTION) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        if (session_info == NULL) {
            target_data = &context->connection_info.end_session_attributes;
        } else {
            target_data = &session_info->end_session_attributes;
        }
        break;
    case LIBSPDM_DATA_SESSION_POLICY:
        target_data_size = sizeof(uint8_t);
        target_data = &session_info->session_policy;
        break;
    case LIBSPDM_DATA_APP_CONTEXT_DATA:
        target_data_size = sizeof(void *);
        target_data = &context->app_context_data_ptr;
        break;
    case LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY:
        target_data_size = sizeof(uint8_t);
        target_data = &context->handle_error_return_policy;
        break;
    case LIBSPDM_DATA_MAX_DHE_SESSION_COUNT:
        target_data_size = sizeof(uint32_t);
        target_data = &context->max_dhe_session_count;
        break;
    case LIBSPDM_DATA_MAX_PSK_SESSION_COUNT:
        target_data_size = sizeof(uint32_t);
        target_data = &context->max_psk_session_count;
        break;
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_REQ_DIR:
        // NVIDIA_EDIT: Appease compiler that we will not use secured_context uninitialized.
        if (secured_context == NULL)
        {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint64_t);
        target_data = &secured_context->application_secret.request_data_sequence_number;
        break;
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_RSP_DIR:
        // NVIDIA_EDIT: Appease compiler that we will not use secured_context uninitialized.
        if (secured_context == NULL)
        {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint64_t);
        target_data = &secured_context->application_secret.response_data_sequence_number;
        break;
    case LIBSPDM_DATA_MAX_SPDM_SESSION_SEQUENCE_NUMBER:
        target_data_size = sizeof(uint64_t);
        target_data = &context->max_spdm_session_sequence_number;
        break;
    case LIBSPDM_DATA_VCA_CACHE:
        target_data_size = context->transcript.message_a.buffer_size;
        target_data = context->transcript.message_a.buffer;
        break;
    case LIBSPDM_DATA_SPDM_VERSION_10_11_VERIFY_SIGNATURE_ENDIAN:
        target_data_size = sizeof(uint8_t);
        target_data = &context->spdm_10_11_verify_signature_endian;
        break;
    case LIBSPDM_DATA_SEQUENCE_NUMBER_ENDIAN:
        target_data_size = sizeof(uint8_t);
        target_data = &context->sequence_number_endian;
    case LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_ENDIAN:
        // NVIDIA_EDIT: Appease compiler that we will not use secured_context uninitialized.
        if (secured_context == NULL)
        {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }
        target_data_size = sizeof(uint8_t);
        target_data = &secured_context->sequence_number_endian;
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

#if LIBSPDM_CHECK_SPDM_CONTEXT
bool libspdm_check_context (void *spdm_context)
{
    libspdm_context_t *context;
    size_t index;

    context = spdm_context;

    if (context->local_context.capability.data_transfer_size <
        SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "data_transfer_size must be greater than or equal "
                       "to SPDM_MIN_DATA_TRANSFER_SIZE (%d).\n",
                       SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12));
        return false;
    }

    if (context->local_context.capability.max_spdm_msg_size <
        context->local_context.capability.data_transfer_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "max_spdm_msg_size (%d) must be greater than or "
                       "equal to data_transfer_size (%d).\n",
                       context->local_context.capability.max_spdm_msg_size,
                       context->local_context.capability.data_transfer_size));
        return false;
    }

    if (context->local_context.capability.sender_data_transfer_size <
        SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "sender_data_transfer_size must be greater than or equal "
                       "to %d.\n", SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12));
        return false;
    }

    if (context->local_context.capability.max_spdm_msg_size <
        context->local_context.capability.sender_data_transfer_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "max_spdm_msg_size (%d) must be greater than or "
                       "equal to sender_data_transfer_size (%d).\n",
                       context->local_context.capability.max_spdm_msg_size,
                       context->local_context.capability.sender_data_transfer_size));
        return false;
    }

    if (((context->local_context.capability.flags &
          SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP) != 0) &&
        (context->local_context.capability.max_spdm_msg_size != 0)) {
        for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
            if ((context->local_context.local_cert_chain_provision_size[index] != 0) &&
                (context->local_context.local_cert_chain_provision_size[index] +
                 sizeof(spdm_certificate_response_t) >
                 context->local_context.capability.max_spdm_msg_size)) {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                               "max_spdm_msg_size (%d) must be greater than or "
                               "equal to local_cert_chain_provision_size[%d] (%d).\n",
                               context->local_context.capability.max_spdm_msg_size, index,
                               context->local_context.local_cert_chain_provision_size[index]));
                return false;
            }
        }
    }

    return true;
}
#endif /* LIBSPDM_CHECK_CONTEXT */

/**
 * Reset message A cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_a(libspdm_context_t *spdm_context)
{
    libspdm_reset_managed_buffer(&spdm_context->transcript.message_a);
}

/**
 * Reset message B cache in SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_reset_message_b(libspdm_context_t *spdm_context)
{
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
void libspdm_reset_message_c(libspdm_context_t *spdm_context)
{
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
void libspdm_reset_message_mut_b(libspdm_context_t *spdm_context)
{
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
void libspdm_reset_message_mut_c(libspdm_context_t *spdm_context)
{
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
void libspdm_reset_message_m(libspdm_context_t *spdm_context, void *session_info)
{
    libspdm_session_info_t *spdm_session_info;

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
void libspdm_reset_message_k(libspdm_context_t *spdm_context, void *session_info)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_session_info->session_transcript.message_k);
#else
    {
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
void libspdm_reset_message_f(libspdm_context_t *spdm_context, void *session_info)
{
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_reset_managed_buffer(&spdm_session_info->session_transcript.message_f);
#else
    {
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
     * or END_SESSION request(s) and skipped CHALLENGE completion, M1 and M2 are reset to null.
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
libspdm_return_t libspdm_append_message_a(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size)
{
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
libspdm_return_t libspdm_append_message_b(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size)
{
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
libspdm_return_t libspdm_append_message_c(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size)
{
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
libspdm_return_t libspdm_append_message_mut_b(libspdm_context_t *spdm_context, const void *message,
                                              size_t message_size)
{
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
libspdm_return_t libspdm_append_message_mut_c(libspdm_context_t *spdm_context, const void *message,
                                              size_t message_size)
{
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
libspdm_return_t libspdm_append_message_m(libspdm_context_t *spdm_context, void *session_info,
                                          const void *message, size_t message_size)
{
    libspdm_session_info_t *spdm_session_info;

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
libspdm_return_t libspdm_append_message_k(libspdm_context_t *spdm_context,
                                          void *session_info,
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
        uint8_t *cert_chain_buffer;
        size_t cert_chain_buffer_size;
        bool result;
        uint8_t cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
        uint32_t hash_size;
        uint8_t slot_id;

        hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

        if (spdm_session_info->session_transcript.digest_context_th == NULL) {
            if (!spdm_session_info->use_psk) {
                if (is_requester) {
                    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
                    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                    if (slot_id == 0xFF) {
                        result = libspdm_get_peer_public_key_buffer(
                            spdm_context, (const void **)&cert_chain_buffer,
                            &cert_chain_buffer_size);
                        if (!result) {
                            return LIBSPDM_STATUS_INVALID_STATE_PEER;
                        }

                        result = libspdm_hash_all(
                            spdm_context->connection_info.algorithm.base_hash_algo,
                            cert_chain_buffer, cert_chain_buffer_size,
                            cert_chain_buffer_hash);
                        if (!result) {
                            return LIBSPDM_STATUS_CRYPTO_ERROR;
                        }
                    } else {
                        LIBSPDM_ASSERT(
                            hash_size ==
                            spdm_context->connection_info
                            .peer_used_cert_chain[slot_id].buffer_hash_size);

                        libspdm_copy_mem(cert_chain_buffer_hash,
                                         sizeof(cert_chain_buffer_hash),
                                         spdm_context->connection_info
                                         .peer_used_cert_chain[slot_id].buffer_hash,
                                         hash_size);
                    }
                } else {
                    slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
                    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                    if (slot_id == 0xFF) {
                        result = libspdm_get_local_public_key_buffer(
                            spdm_context, (const void **)&cert_chain_buffer,
                            &cert_chain_buffer_size);
                    } else {
                        result = libspdm_get_local_cert_chain_buffer(
                            spdm_context, (const void **)&cert_chain_buffer,
                            &cert_chain_buffer_size);
                    }
                    if (!result) {
                        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
                    }

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
libspdm_return_t libspdm_append_message_f(libspdm_context_t *spdm_context,
                                          void *session_info,
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
        const uint8_t *mut_cert_chain_buffer;
        size_t mut_cert_chain_buffer_size;
        bool result;
        uint8_t mut_cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
        uint32_t hash_size;
        libspdm_return_t status;
        uint8_t slot_id;

        hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

        if (!spdm_session_info->session_transcript.message_f_initialized) {
            /* digest_context_th might be NULL in unit test, where message_k is hardcoded. */
            if (spdm_session_info->session_transcript.digest_context_th == NULL) {
                status =
                    libspdm_append_message_k (spdm_context, session_info, is_requester, NULL, 0);
                if (LIBSPDM_STATUS_IS_ERROR(status)) {
                    return status;
                }
            }

            if (!spdm_session_info->use_psk && spdm_session_info->mut_auth_requested) {
                if (is_requester) {
                    slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
                    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                    if (slot_id == 0xFF) {
                        result = libspdm_get_local_public_key_buffer(
                            spdm_context,
                            (const void **)&mut_cert_chain_buffer,
                            &mut_cert_chain_buffer_size);
                    } else {
                        result = libspdm_get_local_cert_chain_buffer(
                            spdm_context,
                            (const void **)&mut_cert_chain_buffer,
                            &mut_cert_chain_buffer_size);
                    }
                    if (!result) {
                        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
                    }

                    result = libspdm_hash_all(
                        spdm_context->connection_info.algorithm.base_hash_algo,
                        mut_cert_chain_buffer, mut_cert_chain_buffer_size,
                        mut_cert_chain_buffer_hash);
                    if (!result) {
                        return LIBSPDM_STATUS_CRYPTO_ERROR;
                    }
                } else {
                    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
                    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                    if (slot_id == 0xFF) {
                        result = libspdm_get_peer_public_key_buffer(
                            spdm_context,
                            (const void **)&mut_cert_chain_buffer,
                            &mut_cert_chain_buffer_size);
                        if (!result) {
                            return LIBSPDM_STATUS_INVALID_STATE_PEER;
                        }

                        result = libspdm_hash_all(
                            spdm_context->connection_info.algorithm.base_hash_algo,
                            mut_cert_chain_buffer, mut_cert_chain_buffer_size,
                            mut_cert_chain_buffer_hash);
                        if (!result) {
                            return LIBSPDM_STATUS_CRYPTO_ERROR;
                        }
                    } else {
                        LIBSPDM_ASSERT(
                            hash_size ==
                            spdm_context->connection_info
                            .peer_used_cert_chain[slot_id].buffer_hash_size);

                        libspdm_copy_mem(mut_cert_chain_buffer_hash,
                                         sizeof(mut_cert_chain_buffer_hash),
                                         spdm_context->connection_info
                                         .peer_used_cert_chain[slot_id].buffer_hash,
                                         hash_size);
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
 * This function returns if a capabilities flag is supported in current SPDM connection.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Is the function called from a requester.
 * @param  requester_capabilities_flag    The requester capabilities flag to be checked
 * @param  responder_capabilities_flag    The responder capabilities flag to be checked
 *
 * @retval true  the capabilities flag is supported.
 * @retval false the capabilities flag is not supported.
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
    void *spdm_context, libspdm_device_send_message_func send_message,
    libspdm_device_receive_message_func receive_message)
{
    libspdm_context_t *context;

    context = spdm_context;
    context->send_message = send_message;
    context->receive_message = receive_message;
}

/**
 * Register SPDM device buffer management functions.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * The sender_buffer_size and receiver_buffer_size must be no smaller than
 * MAX (non-secure Transport Message Header Size +
 *          SPDM_CAPABILITIES.DataTransferSize +
 *          max alignment pad size (transport specific),
 *      secure Transport Message Header Size +
 *          sizeof(spdm_secured_message_a_data_header1_t) +
 *          length of sequence_number (transport specific) +
 *          sizeof(spdm_secured_message_a_data_header2_t) +
 *          sizeof(spdm_secured_message_cipher_header_t) +
 *          App Message Header Size (transport specific) +
 *          SPDM_CAPABILITIES.DataTransferSize +
 *          maximum random data size (transport specific) +
 *          AEAD MAC size (16) +
 *          max alignment pad size (transport specific)).
 *
 * Finally, the SPDM_CAPABILITIES.DataTransferSize will be calculated based upon it.
 *
 *   For MCTP,
 *          Transport Message Header Size = sizeof(mctp_message_header_t)
 *          length of sequence_number = 2
 *          App Message Header Size = sizeof(mctp_message_header_t)
 *          maximum random data size = MCTP_MAX_RANDOM_NUMBER_COUNT
 *          max alignment pad size = 0
 *   For PCI_DOE,
 *          Transport Message Header Size = sizeof(pci_doe_data_object_header_t)
 *          length of sequence_number = 0
 *          App Message Header Size = 0
 *          maximum random data size = 0
 *          max alignment pad size = 3
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  sender_buffer_size            Size in bytes of the sender buffer.
 * @param  receiver_buffer_size          Size in bytes of the receiver buffer.
 * @param  acquire_sender_buffer         The fuction to acquire transport layer sender buffer.
 * @param  release_sender_buffer         The fuction to release transport layer sender buffer.
 * @param  acquire_receiver_buffer       The fuction to acquire transport layer receiver buffer.
 * @param  release_receiver_buffer       The fuction to release transport layer receiver buffer.
 **/
void libspdm_register_device_buffer_func(
    void *spdm_context,
    uint32_t sender_buffer_size,
    uint32_t receiver_buffer_size,
    libspdm_device_acquire_sender_buffer_func acquire_sender_buffer,
    libspdm_device_release_sender_buffer_func release_sender_buffer,
    libspdm_device_acquire_receiver_buffer_func acquire_receiver_buffer,
    libspdm_device_release_receiver_buffer_func release_receiver_buffer)
{
    libspdm_context_t *context;

    context = spdm_context;
    context->sender_buffer_size = sender_buffer_size;
    context->receiver_buffer_size = receiver_buffer_size;
    context->acquire_sender_buffer = acquire_sender_buffer;
    context->release_sender_buffer = release_sender_buffer;
    context->acquire_receiver_buffer = acquire_receiver_buffer;
    context->release_receiver_buffer = release_receiver_buffer;

    LIBSPDM_ASSERT (sender_buffer_size >=
                    context->local_context.capability.transport_header_size +
                    context->local_context.capability.transport_tail_size);
    sender_buffer_size -= (context->local_context.capability.transport_header_size +
                           context->local_context.capability.transport_tail_size);
    LIBSPDM_ASSERT (sender_buffer_size >= SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12);
    context->local_context.capability.sender_data_transfer_size = sender_buffer_size;

    LIBSPDM_ASSERT(receiver_buffer_size >=
                   context->local_context.capability.transport_header_size +
                   context->local_context.capability.transport_tail_size);
    receiver_buffer_size -= (context->local_context.capability.transport_header_size +
                             context->local_context.capability.transport_tail_size);
    LIBSPDM_ASSERT (receiver_buffer_size >= SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12);
    context->local_context.capability.data_transfer_size = receiver_buffer_size;
}

/**
 * Register SPDM transport layer encode/decode functions for SPDM or APP messages.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  transport_encode_message       The fuction to encode an SPDM or APP message to a transport layer message.
 * @param  transport_decode_message       The fuction to decode an SPDM or APP message from a transport layer message.
 **/
void libspdm_register_transport_layer_func(
    void *spdm_context,
    uint32_t max_spdm_msg_size,
    uint32_t transport_header_size,
    uint32_t transport_tail_size,
    libspdm_transport_encode_message_func transport_encode_message,
    libspdm_transport_decode_message_func transport_decode_message)
{
    libspdm_context_t *context;

    context = spdm_context;

    /* fix the data_transfer_size if it is set before */
    if ((context->local_context.capability.data_transfer_size != 0) &&
        (context->local_context.capability.data_transfer_size ==
         context->receiver_buffer_size)) {
        context->local_context.capability.data_transfer_size =
            (uint32_t)(context->receiver_buffer_size -
                       (transport_header_size + transport_tail_size));
    }
    if ((context->local_context.capability.sender_data_transfer_size != 0) &&
        (context->local_context.capability.sender_data_transfer_size ==
         context->sender_buffer_size)) {
        context->local_context.capability.sender_data_transfer_size =
            (uint32_t)(context->sender_buffer_size -
                       (transport_header_size + transport_tail_size));
    }

    context->local_context.capability.max_spdm_msg_size = max_spdm_msg_size;
    context->local_context.capability.transport_header_size = transport_header_size;
    context->local_context.capability.transport_tail_size = transport_tail_size;
    context->transport_encode_message = transport_encode_message;
    context->transport_decode_message = transport_decode_message;
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
    void *spdm_context,
    const libspdm_verify_spdm_cert_chain_func verify_spdm_cert_chain)
{
    libspdm_context_t *context;

    context = spdm_context;
    context->local_context.verify_peer_spdm_cert_chain = verify_spdm_cert_chain;
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
size_t libspdm_get_sizeof_required_scratch_buffer (void *spdm_context)
{
    libspdm_context_t *context;
    size_t scratch_buffer_size;

    context = spdm_context;
    LIBSPDM_ASSERT (context->local_context.capability.max_spdm_msg_size != 0);

    scratch_buffer_size = libspdm_get_scratch_buffer_capacity(context);
    return scratch_buffer_size;
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
    void *spdm_context,
    void *scratch_buffer,
    size_t scratch_buffer_size)
{
    libspdm_context_t *context;

    context = spdm_context;
    LIBSPDM_ASSERT (context->local_context.capability.max_spdm_msg_size != 0);
    LIBSPDM_ASSERT (scratch_buffer_size >= libspdm_get_scratch_buffer_capacity(spdm_context));
    context->scratch_buffer = scratch_buffer;
    context->scratch_buffer_size = scratch_buffer_size;
    context->last_spdm_request = (uint8_t *)scratch_buffer +
                                 libspdm_get_scratch_buffer_last_spdm_request_offset(spdm_context);
#if LIBSPDM_RESPOND_IF_READY_SUPPORT
    context->cache_spdm_request = (uint8_t *)scratch_buffer +
                                  libspdm_get_scratch_buffer_cache_spdm_request_offset(spdm_context);
#endif
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
    void *spdm_context,
    void **scratch_buffer,
    size_t *scratch_buffer_size)
{
    libspdm_context_t *context;

    context = spdm_context;
    LIBSPDM_ASSERT (context->scratch_buffer != NULL);
    LIBSPDM_ASSERT (context->scratch_buffer_size >=
                    libspdm_get_scratch_buffer_capacity(spdm_context));
    *scratch_buffer = context->scratch_buffer;
    *scratch_buffer_size = context->scratch_buffer_size;
    /* need to remove last 2 sections, because they are for libspdm internal state track. */
    *scratch_buffer_size -= libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context);
#if LIBSPDM_RESPOND_IF_READY_SUPPORT
    *scratch_buffer_size -= libspdm_get_scratch_buffer_cache_spdm_request_capacity(spdm_context);
#endif
}

/**
 * Acquire a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  msg_buf_ptr                   A pointer to a sender buffer.
 *
 * @retval RETURN_SUCCESS               The sender buffer is acquired.
 **/
libspdm_return_t libspdm_acquire_sender_buffer (
    libspdm_context_t *spdm_context, size_t *max_msg_size, void **msg_buf_ptr)
{
    libspdm_return_t status;

    LIBSPDM_ASSERT (spdm_context->sender_buffer == NULL);
    LIBSPDM_ASSERT (spdm_context->sender_buffer_size != 0);
    status = spdm_context->acquire_sender_buffer (spdm_context, msg_buf_ptr);
    if (status != LIBSPDM_STATUS_SUCCESS) {
        return status;
    }
    spdm_context->sender_buffer = *msg_buf_ptr;
    *max_msg_size = spdm_context->sender_buffer_size;
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    /* it return scratch buffer, because the requester need build message there.*/
    *msg_buf_ptr = spdm_context->scratch_buffer +
                   libspdm_get_scratch_buffer_large_sender_receiver_offset(spdm_context);
    *max_msg_size = libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context);
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
    LIBSPDM_ASSERT (spdm_context->receiver_buffer_size != 0);
    status = spdm_context->acquire_receiver_buffer (spdm_context, msg_buf_ptr);
    if (status != LIBSPDM_STATUS_SUCCESS) {
        return status;
    }
    spdm_context->receiver_buffer = *msg_buf_ptr;
    *max_msg_size = spdm_context->receiver_buffer_size;
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    /* it return scratch buffer, because the requester need build message there.*/
    *msg_buf_ptr = spdm_context->scratch_buffer +
                   libspdm_get_scratch_buffer_large_sender_receiver_offset(spdm_context);
    *max_msg_size = libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context);
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
void libspdm_get_last_spdm_error_struct(void *spdm_context, libspdm_error_struct_t *last_spdm_error)
{
    libspdm_context_t *context;

    context = spdm_context;
    libspdm_copy_mem(last_spdm_error, sizeof(libspdm_error_struct_t),
                     &context->last_spdm_error,sizeof(libspdm_error_struct_t));
}

/**
 * Set the last SPDM error struct of an SPDM context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM context.
 */
void libspdm_set_last_spdm_error_struct(void *spdm_context, libspdm_error_struct_t *last_spdm_error)
{
    libspdm_context_t *context;

    context = spdm_context;
    libspdm_copy_mem(&context->last_spdm_error, sizeof(context->last_spdm_error),
                     last_spdm_error, sizeof(libspdm_error_struct_t));
}

#if LIBSPDM_FIPS_MODE
/**
 * Initialize an libspdm_fips_selftest_context.
 *
 * @param  fips_selftest_context       A pointer to the fips_selftest_context.
 *
 * @retval RETURN_SUCCESS       context is initialized.
 * @retval RETURN_DEVICE_ERROR  context initialization failed.
 */
libspdm_return_t libspdm_init_fips_selftest_context(void *fips_selftest_context)
{
    libspdm_fips_selftest_context *context;
    LIBSPDM_ASSERT(fips_selftest_context != NULL);

    context = fips_selftest_context;

    /*No tested for every uesd algo*/
    context->tested_algo = 0;
    /*self_test reuslt is false for every uesd algo*/
    context->self_test_result = 0;

    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Return the size in bytes of the fips_selftest_context.
 *
 * @return the size in bytes of the fips_selftest_context.
 **/
size_t libspdm_get_fips_selftest_context_size(void)
{
    size_t size;

    size = sizeof(libspdm_fips_selftest_context);
    return size;
}

/**
 * import fips_selftest_context to spdm_context;
 *
 * @param[in,out]  spdm_context                A pointer to the spdm_context.
 * @param[in]      fips_selftest_context       A pointer to the fips_selftest_context.
 * @param[in]      fips_selftest_context_size  The size of fips_selftest_context.
 *
 * @retval true   import fips_selftest_context successful.
 * @retval false  spdm_context or fips_selftest_context is null.
 */
bool libspdm_import_fips_selftest_context_to_spdm_context(void *spdm_context,
                                                          void *fips_selftest_context,
                                                          size_t fips_selftest_context_size)
{
    libspdm_fips_selftest_context *libspdm_fips_selftest_context;
    libspdm_context_t *libspdm_context;

    libspdm_context = spdm_context;
    libspdm_fips_selftest_context = fips_selftest_context;

    if ((libspdm_context == NULL) || (libspdm_fips_selftest_context == NULL)) {
        return false;
    }
    if (fips_selftest_context_size != sizeof(libspdm_fips_selftest_context)) {
        return false;
    }

    libspdm_copy_mem(&(libspdm_context->fips_selftest_context),
                     sizeof(libspdm_fips_selftest_context),
                     libspdm_fips_selftest_context, sizeof(libspdm_fips_selftest_context));
    return true;
}

/**
 * export fips_selftest_context from spdm_context;
 *
 * @param[in]          spdm_context                A pointer to the spdm_context.
 * @param[in,out]      fips_selftest_context       A pointer to the fips_selftest_context.
 * @param[in]          fips_selftest_context_size  The size of fips_selftest_context.
 *
 * @retval true   export fips_selftest_context successful.
 * @retval false  spdm_context or fips_selftest_context is null.
 */
bool libspdm_export_fips_selftest_context_from_spdm_context(void *spdm_context,
                                                            void *fips_selftest_context,
                                                            size_t fips_selftest_context_size)
{
    libspdm_fips_selftest_context *libspdm_fips_selftest_context;
    libspdm_context_t *libspdm_context;

    libspdm_context = spdm_context;
    libspdm_fips_selftest_context = fips_selftest_context;

    if ((libspdm_context == NULL) || (libspdm_fips_selftest_context == NULL)) {
        return false;
    }
    if (fips_selftest_context_size != sizeof(libspdm_fips_selftest_context)) {
        return false;
    }

    libspdm_copy_mem(libspdm_fips_selftest_context,
                     sizeof(libspdm_fips_selftest_context),
                     &(libspdm_context->fips_selftest_context),
                     sizeof(libspdm_fips_selftest_context));
    return true;
}

#endif /* LIBSPDM_FIPS_MODE */

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
libspdm_return_t libspdm_init_context_with_secured_context(void *spdm_context,
                                                           void **secured_contexts,
                                                           size_t num_secured_contexts)
{
    libspdm_context_t *context;
    size_t index;

    LIBSPDM_ASSERT(spdm_context != NULL);
    LIBSPDM_ASSERT(secured_contexts != NULL);
    LIBSPDM_ASSERT(num_secured_contexts == LIBSPDM_MAX_SESSION_COUNT);

    context = spdm_context;
    libspdm_zero_mem(context, sizeof(libspdm_context_t));
    context->version = LIBSPDM_CONTEXT_STRUCT_VERSION;
    context->transcript.message_a.max_buffer_size =
        sizeof(context->transcript.message_a.buffer);
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    context->transcript.message_b.max_buffer_size =
        sizeof(context->transcript.message_b.buffer);
    context->transcript.message_c.max_buffer_size =
        sizeof(context->transcript.message_c.buffer);
    context->transcript.message_mut_b.max_buffer_size =
        sizeof(context->transcript.message_mut_b.buffer);
    context->transcript.message_mut_c.max_buffer_size =
        sizeof(context->transcript.message_mut_c.buffer);
    context->transcript.message_m.max_buffer_size =
        sizeof(context->transcript.message_m.buffer);
#endif
    context->response_state = LIBSPDM_RESPONSE_STATE_NORMAL;
    context->local_context.version.spdm_version_count = 3;
    context->local_context.version.spdm_version[0] = SPDM_MESSAGE_VERSION_10 <<
                                                     SPDM_VERSION_NUMBER_SHIFT_BIT;
    context->local_context.version.spdm_version[1] = SPDM_MESSAGE_VERSION_11 <<
                                                     SPDM_VERSION_NUMBER_SHIFT_BIT;
    context->local_context.version.spdm_version[2] = SPDM_MESSAGE_VERSION_12 <<
                                                     SPDM_VERSION_NUMBER_SHIFT_BIT;
    context->local_context.secured_message_version.spdm_version_count = 2;
    context->local_context.secured_message_version.spdm_version[0] =
        SPDM_MESSAGE_VERSION_10 << SPDM_VERSION_NUMBER_SHIFT_BIT;
    context->local_context.secured_message_version.spdm_version[1] =
        SPDM_MESSAGE_VERSION_11 << SPDM_VERSION_NUMBER_SHIFT_BIT;
    context->local_context.capability.st1 = SPDM_ST1_VALUE_US;

    context->mut_auth_cert_chain_buffer_size = 0;

    context->max_spdm_session_sequence_number = LIBSPDM_MAX_SPDM_SESSION_SEQUENCE_NUMBER;

    /* To be updated in libspdm_register_device_buffer_func */
    context->local_context.capability.data_transfer_size = 0;
    context->local_context.capability.sender_data_transfer_size = 0;
    context->local_context.capability.max_spdm_msg_size = 0;

    for (index = 0; index < num_secured_contexts; index++) {
        if (secured_contexts[index] == NULL) {
            return LIBSPDM_STATUS_INVALID_PARAMETER;
        }

        context->session_info[index].secured_message_context = secured_contexts[index];
        libspdm_secured_message_init_context(
            context->session_info[index].secured_message_context);
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
libspdm_return_t libspdm_init_context(void *spdm_context)
{
    libspdm_context_t *context;
    void *secured_context;
    void *secured_contexts[LIBSPDM_MAX_SESSION_COUNT];
    size_t secured_context_size;
    size_t index;

    LIBSPDM_ASSERT(spdm_context != NULL);

    /* libspdm_get_context_size() allocates space for all secured message
     * contexts. They are appended to the general SPDM context. */
    context = spdm_context;
    secured_context = (void *)((size_t)(context + 1));
    secured_context_size = libspdm_secured_message_get_context_size();

    for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++)
    {
        secured_contexts[index] = (uint8_t *)secured_context + secured_context_size * index;
    }

    return libspdm_init_context_with_secured_context(spdm_context,
                                                     secured_contexts,
                                                     LIBSPDM_MAX_SESSION_COUNT);
}

void libspdm_reset_context(void *spdm_context)
{
    libspdm_context_t *context;
    size_t index;

    context = spdm_context;

    /*Clear all info about last connection*/

    /*need clear session info to free context before algo is zeroed.*/
    for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++)
    {
        libspdm_session_info_init(context,
                                  &context->session_info[index],
                                  INVALID_SESSION_ID,
                                  false);
    }

    context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_NOT_STARTED;
    libspdm_zero_mem(&context->connection_info.version, sizeof(spdm_version_number_t));
    libspdm_zero_mem(&context->connection_info.capability,
                     sizeof(libspdm_device_capability_t));
    libspdm_zero_mem(&context->connection_info.algorithm, sizeof(libspdm_device_algorithm_t));
    libspdm_zero_mem(&context->last_spdm_error, sizeof(libspdm_error_struct_t));
    libspdm_zero_mem(&context->encap_context, sizeof(libspdm_encap_context_t));
    context->connection_info.local_used_cert_chain_buffer_size = 0;
    context->connection_info.local_used_cert_chain_buffer = NULL;
#if LIBSPDM_RESPOND_IF_READY_SUPPORT
    context->cache_spdm_request_size = 0;
#endif
    context->response_state = LIBSPDM_RESPONSE_STATE_NORMAL;
    context->current_token = 0;
    context->last_spdm_request_session_id = INVALID_SESSION_ID;
    context->last_spdm_request_session_id_valid = false;
    context->last_spdm_request_size = 0;
    context->mut_auth_cert_chain_buffer_size = 0;
    context->current_dhe_session_count = 0;
    context->current_psk_session_count = 0;
}

/**
 * Free the memory of contexts within the SPDM context.
 * These are typically contexts whose memory has been allocated by the cryptography library.
 * This function does not free the SPDM context itself.
 *
 * @param[in]  spdm_context         A pointer to the SPDM context.
 *
 */
void libspdm_deinit_context(void *spdm_context)
{
    uint32_t session_id;
    libspdm_context_t *context;
    libspdm_session_info_t *session_info;
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT)
    void *pubkey_context;
    bool is_requester;
    uint8_t slot_index;
#endif

    context = spdm_context;

#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT)
    is_requester = context->local_context.is_requester;

    for (slot_index = 0; slot_index < SPDM_MAX_SLOT_COUNT; slot_index++) {
        pubkey_context = context->connection_info.peer_used_cert_chain[slot_index].
                         leaf_cert_public_key;

        if (pubkey_context != NULL) {
            if (is_requester) {
                libspdm_asym_free(
                    context->connection_info.algorithm.base_asym_algo, pubkey_context);
            } else {
                libspdm_req_asym_free(
                    context->connection_info.algorithm.req_base_asym_alg, pubkey_context);
            }

            pubkey_context = NULL;
            context->connection_info.peer_used_cert_chain[slot_index].
            leaf_cert_public_key = NULL;
        }
    }
#endif

    libspdm_reset_message_a(context);
    libspdm_reset_message_b(context);
    libspdm_reset_message_c(context);
    libspdm_reset_message_mut_b(context);
    libspdm_reset_message_mut_c(context);
    for (session_id = 0; session_id < LIBSPDM_MAX_SESSION_COUNT; session_id++) {
        session_info = &context->session_info[session_id];
        libspdm_reset_message_m(context, session_info);
        libspdm_reset_message_k(context, session_info);
        libspdm_reset_message_f(context, session_info);
    }
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
