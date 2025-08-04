/**
 *  Copyright Notice:
 *  Copyright 2021-2024 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if (LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP) && (LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP) && \
    (LIBSPDM_ENABLE_CAPABILITY_CERT_CAP)

libspdm_return_t libspdm_get_encap_response_digest(void *spdm_context,
                                                   size_t request_size,
                                                   void *request,
                                                   size_t *response_size,
                                                   void *response)
{
    spdm_get_digest_request_t *spdm_request;
    spdm_digest_response_t *spdm_response;
    size_t index;
    uint32_t hash_size;
    uint8_t *digest;
    libspdm_context_t *context;
    libspdm_return_t status;
    bool result;
    /*total populated slot count*/
    uint8_t slot_count;
    /*populated slot index*/
    uint8_t slot_index;
    uint32_t session_id;
    libspdm_session_info_t *session_info;
    size_t additional_size;
    spdm_key_pair_id_t *key_pair_id;
    spdm_certificate_info_t *cert_info;
    spdm_key_usage_bit_mask_t *key_usage_bit_mask;

    context = spdm_context;
    spdm_request = request;

    if (libspdm_get_connection_version(context) < SPDM_MESSAGE_VERSION_11) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST,
            SPDM_GET_DIGESTS, response_size, response);
    }

    if (spdm_request->header.spdm_version != libspdm_get_connection_version(context)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_VERSION_MISMATCH,
            0, response_size, response);
    }

    if (!libspdm_is_capabilities_flag_supported(
            context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP, 0)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST,
            SPDM_GET_DIGESTS, response_size, response);
    }

    if (request_size < sizeof(spdm_get_digest_request_t)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }

    libspdm_reset_message_buffer_via_request_code(context, NULL,
                                                  spdm_request->header.request_response_code);

    hash_size = libspdm_get_hash_size(
        context->connection_info.algorithm.base_hash_algo);

    slot_count = libspdm_get_cert_slot_count(context);
    additional_size = 0;
    if ((spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_13) &&
        context->connection_info.multi_key_conn_req) {
        additional_size = sizeof(spdm_key_pair_id_t) + sizeof(spdm_certificate_info_t) +
                          sizeof(spdm_key_usage_bit_mask_t);
    }
    LIBSPDM_ASSERT(*response_size >=
                   sizeof(spdm_digest_response_t) + (hash_size + additional_size) * slot_count);
    *response_size = sizeof(spdm_digest_response_t) + (hash_size + additional_size) * slot_count;
    libspdm_zero_mem(response, *response_size);
    spdm_response = response;

    spdm_response->header.spdm_version = spdm_request->header.spdm_version;
    spdm_response->header.request_response_code = SPDM_DIGESTS;
    spdm_response->header.param1 = 0;
    spdm_response->header.param2 = 0;

    if (spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_13) {
        spdm_response->header.param1 = context->local_context.local_supported_slot_mask;
    }

    digest = (void *)(spdm_response + 1);
    key_pair_id = (spdm_key_pair_id_t *)((uint8_t *)digest + hash_size * slot_count);
    cert_info = (spdm_certificate_info_t *)((uint8_t *)key_pair_id +
                                            sizeof(spdm_key_pair_id_t) * slot_count);
    key_usage_bit_mask = (spdm_key_usage_bit_mask_t *)((uint8_t *)cert_info +
                                                       sizeof(spdm_certificate_info_t) *
                                                       slot_count);

    slot_index = 0;
    for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
        if (context->local_context
            .local_cert_chain_provision[index] != NULL) {
            spdm_response->header.param2 |= (1 << index);
            result = libspdm_generate_cert_chain_hash(context, index,
                                                      &digest[hash_size * slot_index]);
            if ((spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_13) &&
                context->connection_info.multi_key_conn_req) {
                key_pair_id[slot_index] = context->local_context.local_key_pair_id[index];
                cert_info[slot_index] = context->local_context.local_cert_info[index];
                key_usage_bit_mask[slot_index] =
                    context->local_context.local_key_usage_bit_mask[index];
            }
            slot_index++;
            if (!result) {
                return libspdm_generate_encap_error_response(
                    context, SPDM_ERROR_CODE_UNSPECIFIED,
                    0, response_size, response);
            }
        }
    }

    /* Cache*/

    status = libspdm_append_message_mut_b(context, spdm_request,
                                          request_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSPECIFIED, 0,
            response_size, response);
    }

    status = libspdm_append_message_mut_b(context, spdm_response,
                                          *response_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSPECIFIED, 0,
            response_size, response);
    }

    if (context->last_spdm_request_session_id_valid) {
        session_id = context->last_spdm_request_session_id;
    } else {
        session_id = context->latest_session_id;
    }
    if (session_id != INVALID_SESSION_ID) {
        session_info = libspdm_get_session_info_via_session_id(context, session_id);
    } else {
        session_info = NULL;
    }
    if (session_info != NULL) {
        if (context->connection_info.multi_key_conn_req) {
            status = libspdm_append_message_encap_d(context, session_info, true,
                                                    spdm_response, *response_size);
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                return libspdm_generate_encap_error_response(
                    context, SPDM_ERROR_CODE_UNSPECIFIED, 0,
                    response_size, response);
            }
        }
    }

    return LIBSPDM_STATUS_SUCCESS;
}

#endif /* (LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP) && (..) */
