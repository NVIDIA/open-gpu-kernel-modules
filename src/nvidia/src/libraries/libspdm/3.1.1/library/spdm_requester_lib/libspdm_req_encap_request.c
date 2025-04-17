/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP

typedef struct {
    uint8_t request_response_code;
    libspdm_get_encap_response_func get_encap_response_func;
} libspdm_get_encap_response_struct_t;

void libspdm_register_get_encap_response_func(void *spdm_context,
                                              const libspdm_get_encap_response_func
                                              get_encap_response_func)
{
    libspdm_context_t *context;

    context = spdm_context;
    context->get_encap_response_func = (void *)get_encap_response_func;
}

/**
 * Return the GET_ENCAP_RESPONSE function via request code.
 *
 * @param  request_code                  The SPDM request code.
 *
 * @return GET_ENCAP_RESPONSE function according to the request code.
 **/
static libspdm_get_encap_response_func
libspdm_get_encap_response_func_via_request_code(uint8_t request_response_code)
{
    size_t index;

    libspdm_get_encap_response_struct_t get_encap_response_struct[] = {
        #if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
        #if LIBSPDM_ENABLE_CAPABILITY_CERT_CAP
        { SPDM_GET_DIGESTS, libspdm_get_encap_response_digest },
        { SPDM_GET_CERTIFICATE, libspdm_get_encap_response_certificate },
        #endif /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP*/

        #if LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP
        { SPDM_CHALLENGE, libspdm_get_encap_response_challenge_auth },
        #endif /* LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP*/
        #endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

        { SPDM_KEY_UPDATE, libspdm_get_encap_response_key_update },
    };

    for (index = 0; index < sizeof(get_encap_response_struct) /
         sizeof(get_encap_response_struct[0]); index++) {
        if (request_response_code == get_encap_response_struct[index].request_response_code) {
            return get_encap_response_struct[index].get_encap_response_func;
        }
    }
    return NULL;
}

/**
 * This function processes encapsulated request.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  encap_request_size             size in bytes of the request data buffer.
 * @param  encap_request                 A pointer to a destination buffer to store the request.
 * @param  encap_response_size            size in bytes of the response data buffer.
 * @param  encap_response                A pointer to a destination buffer to store the response.
 *
 * @retval RETURN_SUCCESS               The SPDM response is processed successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM response is sent to the device.
 **/
static libspdm_return_t libspdm_process_encapsulated_request(libspdm_context_t *spdm_context,
                                                             size_t encap_request_size,
                                                             void *encap_request,
                                                             size_t *encap_response_size,
                                                             void *encap_response)
{
    libspdm_get_encap_response_func get_encap_response_func;
    spdm_message_header_t *spdm_requester;

    spdm_requester = encap_request;
    if (encap_request_size < sizeof(spdm_message_header_t)) {
        return libspdm_generate_encap_error_response(
            spdm_context, SPDM_ERROR_CODE_UNSUPPORTED_REQUEST,
            spdm_requester->request_response_code,
            encap_response_size, encap_response);
    }

    get_encap_response_func = libspdm_get_encap_response_func_via_request_code(
        spdm_requester->request_response_code);
    if (get_encap_response_func == NULL) {
        get_encap_response_func =
            (libspdm_get_encap_response_func)
            spdm_context->get_encap_response_func;
    }
    if (get_encap_response_func != NULL) {
        return get_encap_response_func(
            spdm_context, encap_request_size, encap_request,
            encap_response_size, encap_response);
    } else {
        return libspdm_generate_encap_error_response(
            spdm_context, SPDM_ERROR_CODE_UNEXPECTED_REQUEST,
            0,
            encap_response_size, encap_response);
    }
}

libspdm_return_t libspdm_encapsulated_request(libspdm_context_t *spdm_context,
                                              const uint32_t *session_id,
                                              uint8_t mut_auth_requested,
                                              uint8_t *req_slot_id_param)
{
    libspdm_return_t status;
    uint8_t *spdm_request;
    size_t spdm_request_size;
    spdm_get_encapsulated_request_request_t
    *spdm_get_encapsulated_request_request;
    spdm_deliver_encapsulated_response_request_t
    *spdm_deliver_encapsulated_response_request;
    uint8_t *spdm_response;
    size_t spdm_response_size;
    spdm_encapsulated_request_response_t *libspdm_encapsulated_request_response;
    spdm_encapsulated_response_ack_response_t
    *spdm_encapsulated_response_ack_response;
    libspdm_session_info_t *session_info;
    uint8_t request_id;
    void *encapsulated_request;
    size_t encapsulated_request_size;
    void *encapsulated_response;
    size_t encapsulated_response_size;
    size_t ack_header_size;

    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    #if LIBSPDM_ENABLE_CAPABILITY_CERT_CAP
    spdm_get_digest_request_t *get_digests;
    #endif /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP*/

    if (libspdm_get_connection_version(spdm_context) < SPDM_MESSAGE_VERSION_11) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCAP_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCAP_CAP)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    if (session_id != NULL) {
        session_info = libspdm_get_session_info_via_session_id(
            spdm_context, *session_id);
        if (session_info == NULL) {
            LIBSPDM_ASSERT(false);
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        LIBSPDM_ASSERT((mut_auth_requested == 0) ||
                       (mut_auth_requested ==
                        SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_ENCAP_REQUEST) ||
                       (mut_auth_requested ==
                        SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_GET_DIGESTS));
    } else {
        LIBSPDM_ASSERT(mut_auth_requested == 0);
    }


    /* Cache*/

    libspdm_reset_message_mut_b(spdm_context);
    libspdm_reset_message_mut_c(spdm_context);

    if (session_id == NULL) {
        spdm_context->last_spdm_request_session_id_valid = false;
        spdm_context->last_spdm_request_session_id = 0;
    } else {
        spdm_context->last_spdm_request_session_id_valid = true;
        spdm_context->last_spdm_request_session_id = *session_id;
    }

    if (mut_auth_requested ==
        SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_GET_DIGESTS) {

#if LIBSPDM_ENABLE_CAPABILITY_CERT_CAP

        get_digests = (void *)spdm_context->last_spdm_request;
        get_digests->header.spdm_version = libspdm_get_connection_version (spdm_context);
        get_digests->header.request_response_code = SPDM_GET_DIGESTS;
        get_digests->header.param1 = 0;
        get_digests->header.param2 = 0;
        spdm_context->last_spdm_request_size = sizeof(spdm_get_digest_request_t);
        encapsulated_request = (void *)spdm_context->last_spdm_request;
        encapsulated_request_size = spdm_context->last_spdm_request_size;
        request_id = 0;
#else /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP*/
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
#endif /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP*/
    } else {

        transport_header_size = spdm_context->local_context.capability.transport_header_size;
        status = libspdm_acquire_sender_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return status;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size +
                        spdm_context->local_context.capability.transport_tail_size);
        spdm_request = (void *)(message + transport_header_size);
        spdm_request_size = message_size - transport_header_size -
                            spdm_context->local_context.capability.transport_tail_size;

        spdm_context->crypto_request = true;
        spdm_get_encapsulated_request_request = (void *)spdm_request;
        spdm_get_encapsulated_request_request->header.spdm_version =
            libspdm_get_connection_version (spdm_context);
        spdm_get_encapsulated_request_request->header
        .request_response_code = SPDM_GET_ENCAPSULATED_REQUEST;
        spdm_get_encapsulated_request_request->header.param1 = 0;
        spdm_get_encapsulated_request_request->header.param2 = 0;
        spdm_request_size =
            sizeof(spdm_get_encapsulated_request_request_t);
        libspdm_reset_message_buffer_via_request_code(spdm_context, NULL,
                                                      spdm_get_encapsulated_request_request->header.request_response_code);
        status = libspdm_send_spdm_request(
            spdm_context, session_id, spdm_request_size,
            spdm_request);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            return status;
        }
        libspdm_release_sender_buffer (spdm_context);
        spdm_get_encapsulated_request_request = (void *)spdm_context->last_spdm_request;

        /* receive */

        status = libspdm_acquire_receiver_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return status;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size);
        spdm_response = (void *)(message);
        spdm_response_size = message_size;

        libspdm_zero_mem(spdm_response, spdm_response_size);
        status = libspdm_receive_spdm_response(
            spdm_context, session_id, &spdm_response_size,
            (void **)&spdm_response);
        libspdm_encapsulated_request_response = (void *)spdm_response;
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_receiver_buffer (spdm_context);
            return status;
        }
        if (libspdm_encapsulated_request_response->header
            .request_response_code !=
            SPDM_ENCAPSULATED_REQUEST) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
        if (libspdm_encapsulated_request_response->header.spdm_version !=
            spdm_get_encapsulated_request_request->header.spdm_version) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
        if (spdm_response_size < sizeof(spdm_encapsulated_request_response_t)) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_SIZE;
        }

#if LIBSPDM_ENABLE_MSG_LOG
        // NVIDIA_EDIT: Need to support message log for testing purposes for all responses
        libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
#endif /* LIBSPDM_ENABLE_MSG_LOG */

        if (spdm_response_size == sizeof(spdm_encapsulated_request_response_t)) {

            /* Done*/
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_SUCCESS;
        }
        request_id = libspdm_encapsulated_request_response->header.param1;

        encapsulated_request = (void *)(libspdm_encapsulated_request_response + 1);
        encapsulated_request_size =
            spdm_response_size - sizeof(spdm_encapsulated_request_response_t);

        libspdm_copy_mem (spdm_context->last_spdm_request,
                          libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context),
                          encapsulated_request,
                          encapsulated_request_size);
        spdm_context->last_spdm_request_size = encapsulated_request_size;
        encapsulated_request = (void *)spdm_context->last_spdm_request;

        libspdm_release_receiver_buffer (spdm_context);
    }

    while (true) {

        /* Process request*/
        transport_header_size = spdm_context->local_context.capability.transport_header_size;
        status = libspdm_acquire_sender_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return status;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size +
                        spdm_context->local_context.capability.transport_tail_size);
        spdm_request = (void *)(message + transport_header_size);
        spdm_request_size = message_size - transport_header_size -
                            spdm_context->local_context.capability.transport_tail_size;

        spdm_context->crypto_request = true;
        spdm_deliver_encapsulated_response_request = (void *)spdm_request;
        spdm_deliver_encapsulated_response_request->header.spdm_version =
            libspdm_get_connection_version (spdm_context);
        spdm_deliver_encapsulated_response_request->header
        .request_response_code =
            SPDM_DELIVER_ENCAPSULATED_RESPONSE;
        spdm_deliver_encapsulated_response_request->header.param1 = request_id;
        spdm_deliver_encapsulated_response_request->header.param2 = 0;
        encapsulated_response = (void *)(spdm_deliver_encapsulated_response_request + 1);
        encapsulated_response_size =
            spdm_request_size - sizeof(spdm_deliver_encapsulated_response_request_t);

        status = libspdm_process_encapsulated_request(
            spdm_context, encapsulated_request_size,
            encapsulated_request, &encapsulated_response_size,
            encapsulated_response);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            return status;
        }

        spdm_request_size =
            sizeof(spdm_deliver_encapsulated_response_request_t) + encapsulated_response_size;
        status = libspdm_send_spdm_request(
            spdm_context, session_id, spdm_request_size,
            spdm_request);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            return status;
        }
        libspdm_release_sender_buffer (spdm_context);
        spdm_deliver_encapsulated_response_request = (void *)spdm_context->last_spdm_request;

        /* receive */

        status = libspdm_acquire_receiver_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return status;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size);
        spdm_response = (void *)(message);
        spdm_response_size = message_size;

        libspdm_zero_mem(spdm_response, spdm_response_size);

        status = libspdm_receive_spdm_response(
            spdm_context, session_id, &spdm_response_size,
            (void **)&spdm_response);
        spdm_encapsulated_response_ack_response = (void *)spdm_response;
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_receiver_buffer (spdm_context);
            return status;
        }
        if (spdm_encapsulated_response_ack_response->header
            .request_response_code !=
            SPDM_ENCAPSULATED_RESPONSE_ACK) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
        if (spdm_encapsulated_response_ack_response->header.spdm_version !=
            spdm_deliver_encapsulated_response_request->header.spdm_version) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
        if (spdm_encapsulated_response_ack_response->header.spdm_version >=
            SPDM_MESSAGE_VERSION_12) {
            ack_header_size = sizeof(spdm_encapsulated_response_ack_response_t);
        } else {
            ack_header_size = sizeof(spdm_message_header_t);
        }
        if (spdm_response_size < ack_header_size) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_SIZE;
        }

        if (spdm_encapsulated_response_ack_response->header.spdm_version >=
            SPDM_MESSAGE_VERSION_12) {
            if (spdm_encapsulated_response_ack_response->ack_request_id !=
                spdm_deliver_encapsulated_response_request->header.param1) {
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_INVALID_MSG_FIELD;
            }
        }

        switch (spdm_encapsulated_response_ack_response->header.param2) {
        case SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_ABSENT:
            if (spdm_response_size == ack_header_size) {
#if LIBSPDM_ENABLE_MSG_LOG
                // NVIDIA_EDIT: Need to support message log for testing purposes for all responses
                libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
#endif /* LIBSPDM_ENABLE_MSG_LOG */
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_SUCCESS;
            } else {
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_INVALID_MSG_SIZE;
            }
            break;
        case SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_PRESENT:
            break;
        case SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_REQ_SLOT_NUMBER:
            if (spdm_response_size >= ack_header_size + sizeof(uint8_t)) {
                if ((req_slot_id_param != NULL) &&
                    (*req_slot_id_param == 0)) {
                    *req_slot_id_param =
                        *((uint8_t *)spdm_encapsulated_response_ack_response + ack_header_size);
                    /* 0xFF or 0xF slot is not allowed. */
                    if (*req_slot_id_param >= SPDM_MAX_SLOT_COUNT) {
                        libspdm_release_receiver_buffer (spdm_context);
                        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
                    }
                }
#if LIBSPDM_ENABLE_MSG_LOG
                // NVIDIA_EDIT: Need to support message log for testing purposes for all responses
                libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
#endif /* LIBSPDM_ENABLE_MSG_LOG */
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_SUCCESS;
            } else {
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_INVALID_MSG_SIZE;
            }
            break;
        default:
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
        request_id =
            spdm_encapsulated_response_ack_response->header.param1;

#if LIBSPDM_ENABLE_MSG_LOG
        // NVIDIA_EDIT: Need to support message log for testing purposes for all responses
        libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
#endif /* LIBSPDM_ENABLE_MSG_LOG */

        encapsulated_request =
            ((uint8_t *)spdm_encapsulated_response_ack_response + ack_header_size);
        encapsulated_request_size = spdm_response_size - ack_header_size;

        libspdm_copy_mem (spdm_context->last_spdm_request,
                          libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context),
                          encapsulated_request,
                          encapsulated_request_size
                          );
        spdm_context->last_spdm_request_size = encapsulated_request_size;
        encapsulated_request = (void *)spdm_context->last_spdm_request;

        libspdm_release_receiver_buffer (spdm_context);
    }

    return LIBSPDM_STATUS_SUCCESS;
}

libspdm_return_t libspdm_send_receive_encap_request(void *spdm_context, const uint32_t *session_id)
{
    return libspdm_encapsulated_request(spdm_context, session_id, 0, NULL);
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP */
