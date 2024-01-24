/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"
#include "internal/libspdm_secured_message_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP

libspdm_return_t libspdm_get_encap_response_key_update(void *spdm_context,
                                                       size_t request_size,
                                                       void *request,
                                                       size_t *response_size,
                                                       void *response)
{
    uint32_t session_id;
    spdm_key_update_response_t *spdm_response;
    spdm_key_update_request_t *spdm_request;
    libspdm_context_t *context;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;
    spdm_key_update_request_t *prev_spdm_request;
    spdm_key_update_request_t spdm_key_init_update_operation;
    bool result;

    context = spdm_context;
    spdm_request = request;

    if (libspdm_get_connection_version(context) < SPDM_MESSAGE_VERSION_11) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST,
            0, response_size, response);
    }

    if (spdm_request->header.spdm_version != libspdm_get_connection_version(context)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_VERSION_MISMATCH,
            0, response_size, response);
    }

    if (!libspdm_is_capabilities_flag_supported(
            context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST,
            SPDM_KEY_UPDATE, response_size, response);
    }

    if (!context->last_spdm_request_session_id_valid) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }
    session_id = context->last_spdm_request_session_id;
    session_info =
        libspdm_get_session_info_via_session_id(context, session_id);
    if (session_info == NULL) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }
    session_state = libspdm_secured_message_get_session_state(
        session_info->secured_message_context);
    if (session_state != LIBSPDM_SESSION_STATE_ESTABLISHED) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }

    /* this message can only be in secured session
     * thus don't need to consider transport layer padding, just check its exact size */
    if (request_size != sizeof(spdm_key_update_request_t)) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }

    /*last key operation*/
    prev_spdm_request = &(session_info->last_key_update_request);

    /*the end status of the successful key update overall flow*/
    libspdm_zero_mem(&spdm_key_init_update_operation, sizeof(spdm_key_update_request_t));

    result = true;
    switch (spdm_request->header.param1) {
    case SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_KEY:
        if(!libspdm_consttime_is_mem_equal(prev_spdm_request,
                                           &spdm_key_init_update_operation,
                                           sizeof(spdm_key_update_request_t))) {
            result = false;
            break;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_create_update_session_data_key[%x] Responder\n",
                       session_id));
        result = libspdm_create_update_session_data_key(
            session_info->secured_message_context,
            LIBSPDM_KEY_UPDATE_ACTION_RESPONDER);

        /*save the last update operation*/
        libspdm_copy_mem(prev_spdm_request, sizeof(spdm_key_update_request_t),
                         spdm_request, request_size);
        break;
    case SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_ALL_KEYS:
        result = false;
        break;
    case SPDM_KEY_UPDATE_OPERATIONS_TABLE_VERIFY_NEW_KEY:
        if(prev_spdm_request->header.param1 !=
           SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_KEY) {
            result = false;
            break;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_activate_update_session_data_key[%x] Responder new\n",
                       session_id));
        result = libspdm_activate_update_session_data_key(
            session_info->secured_message_context,
            LIBSPDM_KEY_UPDATE_ACTION_RESPONDER, true);

        /*clear last_key_update_request*/
        libspdm_zero_mem(prev_spdm_request, sizeof(spdm_key_update_request_t));
        break;
    default:
        result = false;
        break;
    }

    if (!result) {
        return libspdm_generate_encap_error_response(
            context, SPDM_ERROR_CODE_INVALID_REQUEST, 0,
            response_size, response);
    }

    libspdm_reset_message_buffer_via_request_code(context, session_info,
                                                  spdm_request->header.request_response_code);

    LIBSPDM_ASSERT(*response_size >= sizeof(spdm_key_update_response_t));
    *response_size = sizeof(spdm_key_update_response_t);
    libspdm_zero_mem(response, *response_size);
    spdm_response = response;

    spdm_response->header.spdm_version = spdm_request->header.spdm_version;
    spdm_response->header.request_response_code = SPDM_KEY_UPDATE_ACK;
    spdm_response->header.param1 = spdm_request->header.param1;
    spdm_response->header.param2 = spdm_request->header.param2;

    return LIBSPDM_STATUS_SUCCESS;
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP*/
