/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"
#include "internal/libspdm_secured_message_lib.h"

libspdm_return_t libspdm_send_request(void *spdm_context, const uint32_t *session_id,
                                      bool is_app_message,
                                      size_t request_size, void *request)
{
    libspdm_context_t *context;
    libspdm_return_t status;
    uint8_t *message;
    size_t message_size;
    uint64_t timeout;
    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;
    size_t transport_header_size;
    uint8_t *sender_buffer;
    size_t sender_buffer_size;

    context = spdm_context;

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                   "libspdm_send_spdm_request[%x] msg %s(0x%x), size (0x%x): \n",
                   (session_id != NULL) ? *session_id : 0x0,
                   libspdm_get_code_str(((spdm_message_header_t *)request)->
                                        request_response_code),
                   ((spdm_message_header_t *)request)->request_response_code,
                   request_size));
    LIBSPDM_INTERNAL_DUMP_HEX(request, request_size);

    transport_header_size = context->local_context.capability.transport_header_size;
    libspdm_get_scratch_buffer(context, (void**) &scratch_buffer, &scratch_buffer_size);
    libspdm_get_sender_buffer(context, (void**) &sender_buffer, &sender_buffer_size);

    /* This is a problem because original code assumes request is in the sender buffer,
     * when it can really be using the scratch space for chunking.
     * Did not want to modify ally request handlers to pass this information,
     * so just making the determination here by examining scratch/sender buffers.
     * This may be something that should be refactored in the future. */
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    if ((uint8_t*) request >= sender_buffer &&
        (uint8_t*)request < sender_buffer + sender_buffer_size) {
        message = sender_buffer;
        message_size = sender_buffer_size;
    } else {
        if ((uint8_t*)request >=
            scratch_buffer + libspdm_get_scratch_buffer_sender_receiver_offset(spdm_context)
            && (uint8_t*)request <
            scratch_buffer + libspdm_get_scratch_buffer_sender_receiver_offset(spdm_context)
            + libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context)) {
            message = scratch_buffer +
                      libspdm_get_scratch_buffer_sender_receiver_offset(spdm_context);
            message_size = libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context);
        } else if ((uint8_t*)request >=
                   scratch_buffer +
                   libspdm_get_scratch_buffer_large_sender_receiver_offset(spdm_context)
                   && (uint8_t*)request <
                   scratch_buffer +
                   libspdm_get_scratch_buffer_large_sender_receiver_offset(spdm_context) +
                   libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context)) {
            message = scratch_buffer +
                      libspdm_get_scratch_buffer_large_sender_receiver_offset(spdm_context);
            message_size = libspdm_get_scratch_buffer_large_sender_receiver_capacity(spdm_context);
        }
    }
    #else /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */
    message = sender_buffer;
    message_size = sender_buffer_size;
    #endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

    if (session_id != NULL) {
        /* For secure message, message is in sender buffer, we need copy it to scratch buffer.
         * transport_message is always in sender buffer. */

        libspdm_copy_mem (scratch_buffer + transport_header_size,
                          scratch_buffer_size - transport_header_size,
                          request, request_size);
        request = scratch_buffer + transport_header_size;
    }

    /* backup it to last_spdm_request, because the caller wants to compare it with response */
    if (((const spdm_message_header_t *)request)->request_response_code != SPDM_RESPOND_IF_READY
        && ((const spdm_message_header_t *)request)->request_response_code != SPDM_CHUNK_GET
        && ((const spdm_message_header_t*) request)->request_response_code != SPDM_CHUNK_SEND) {
        libspdm_copy_mem (context->last_spdm_request,
                          libspdm_get_scratch_buffer_last_spdm_request_capacity(context),
                          request,
                          request_size
                          );
        context->last_spdm_request_size = request_size;
    }

    status = context->transport_encode_message(
        context, session_id, is_app_message, true, request_size,
        request, &message_size, (void **)&message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "transport_encode_message status - %p\n",
                       status));
        if ((session_id != NULL) &&
            ((status == LIBSPDM_STATUS_SEQUENCE_NUMBER_OVERFLOW) ||
             (status == LIBSPDM_STATUS_CRYPTO_ERROR))) {
            libspdm_free_session_id(context, *session_id);
        }
        return status;
    }

    timeout = context->local_context.capability.rtt;

    status = context->send_message(context, message_size, message,
                                   timeout);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "libspdm_send_spdm_request[%x] status - %p\n",
                       (session_id != NULL) ? *session_id : 0x0, status));
    }

    return status;
}

libspdm_return_t libspdm_receive_response(void *spdm_context, const uint32_t *session_id,
                                          bool is_app_message,
                                          size_t *response_size,
                                          void **response)
{
    libspdm_context_t *context;
    void *temp_session_context;
    libspdm_return_t status;
    uint8_t *message;
    size_t message_size;
    uint32_t *message_session_id;
    bool is_message_app_message;
    uint64_t timeout;
    size_t transport_header_size;
    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;
    void *backup_response;
    size_t backup_response_size;
    bool reset_key_update;
    bool result;

    context = spdm_context;

    if (context->crypto_request) {
        timeout = context->local_context.capability.rtt +
                  ((uint64_t)1 << context->connection_info.capability.ct_exponent);
    } else {
        timeout = context->local_context.capability.rtt +
                  context->local_context.capability.st1;
    }

    message = *response;
    message_size = *response_size;
    status = context->receive_message(context, &message_size,
                                      (void **)&message, timeout);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_receive_spdm_response[%x] status - %p\n",
                       (session_id != NULL) ? *session_id : 0x0, status));
        return status;
    }

    message_session_id = NULL;
    is_message_app_message = false;

    /* always use scratch buffer to response.
     * if it is secured message, this scratch buffer will be used.
     * if it is normal message, the response ptr will point to receiver buffer. */
    transport_header_size = context->local_context.capability.transport_header_size;
    libspdm_get_scratch_buffer (context, (void **)&scratch_buffer, &scratch_buffer_size);
    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    *response = scratch_buffer + libspdm_get_scratch_buffer_secure_message_offset(context) +
                transport_header_size;
    *response_size = libspdm_get_scratch_buffer_secure_message_capacity(context) -
                     transport_header_size;
    #else
    *response = scratch_buffer + transport_header_size;
    *response_size = scratch_buffer_size - transport_header_size;
    #endif

    backup_response = *response;
    backup_response_size = *response_size;

    status = context->transport_decode_message(
        context, &message_session_id, &is_message_app_message,
        false, message_size, message, response_size, response);

    reset_key_update = false;
    temp_session_context = NULL;

    if (status == LIBSPDM_STATUS_SESSION_TRY_DISCARD_KEY_UPDATE) {
        /* Failed to decode, but have backup keys. Try rolling back before aborting.
         * message_session_id must be valid for us to have attempted decryption. */
        if (message_session_id == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        temp_session_context = libspdm_get_secured_message_context_via_session_id(
            context, *message_session_id);
        if (temp_session_context == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }

        result = libspdm_activate_update_session_data_key(
            temp_session_context, LIBSPDM_KEY_UPDATE_ACTION_RESPONDER, false);
        if (!result) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }

        /* Retry decoding message with backup Requester key.
         * Must reset some of the parameters in case they were modified */
        message_session_id = NULL;
        is_message_app_message = false;
        *response = backup_response;
        *response_size = backup_response_size;
        status = context->transport_decode_message(
            context, &message_session_id, &is_message_app_message,
            false, message_size, message, response_size, response);

        reset_key_update = true;
    }

    if (session_id != NULL) {
        if (message_session_id == NULL) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "libspdm_receive_spdm_response[%x] GetSessionId - NULL\n",
                           (session_id != NULL) ? *session_id : 0x0));
            goto error;
        }
        if (*message_session_id != *session_id) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "libspdm_receive_spdm_response[%x] GetSessionId - %x\n",
                           (session_id != NULL) ? *session_id : 0x0,
                           *message_session_id));
            goto error;
        }
    } else {
        if (message_session_id != NULL) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "libspdm_receive_spdm_response[%x] GetSessionId - %x\n",
                           (session_id != NULL) ? *session_id : 0x0,
                           *message_session_id));
            goto error;
        }
    }

    if ((is_app_message && !is_message_app_message) ||
        (!is_app_message && is_message_app_message)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_receive_spdm_response[%x] app_message mismatch\n",
                       (session_id != NULL) ? *session_id : 0x0));
        goto error;
    }

    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        if ((session_id != NULL) &&
            (context->last_spdm_error.error_code == SPDM_ERROR_CODE_DECRYPT_ERROR)) {
            libspdm_free_session_id(context, *session_id);
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_receive_spdm_response[%x] status - %p\n",
                       (session_id != NULL) ? *session_id : 0x0, status));
    } else {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_receive_spdm_response[%x] msg %s(0x%x), size (0x%x): \n",
                       (session_id != NULL) ? *session_id : 0x0,
                       libspdm_get_code_str(((spdm_message_header_t *)*response)->
                                            request_response_code),
                       ((spdm_message_header_t *)*response)->request_response_code,
                       *response_size));
        LIBSPDM_INTERNAL_DUMP_HEX(*response, *response_size);
    }

    /* Handle special case:
     * If the Responder returns RESPONSE_NOT_READY error to KEY_UPDATE, the Requester needs
     * to activate backup key to parse the error. Then later the Responder will return SUCCESS,
     * the Requester needs new key. So we need to restore the environment by
     * libspdm_create_update_session_data_key() again.*/
    if (reset_key_update) {
        /* temp_session_context and message_session_id must necessarily
         * be valid for us to reach here. */
        if (temp_session_context == NULL || message_session_id == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        result = libspdm_create_update_session_data_key(
            temp_session_context, LIBSPDM_KEY_UPDATE_ACTION_RESPONDER);
        if (!result) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
    }

    return status;

error:
    if (context->last_spdm_error.error_code == SPDM_ERROR_CODE_DECRYPT_ERROR) {
        return LIBSPDM_STATUS_SESSION_MSG_ERROR;
    } else {
        return LIBSPDM_STATUS_RECEIVE_FAIL;
    }
}

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
libspdm_return_t libspdm_handle_large_request(
    libspdm_context_t *spdm_context,
    const uint32_t *session_id,
    size_t request_size, void *request)
{
    libspdm_return_t status;

    spdm_chunk_send_request_t *spdm_request;
    size_t spdm_request_size;
    spdm_chunk_send_ack_response_t *spdm_response;
    uint8_t *message;
    size_t message_size;
    void *response;
    size_t response_size;
    size_t transport_header_size;

    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;

    uint8_t *chunk_ptr;
    size_t copy_size;
    libspdm_chunk_info_t *send_info;
    uint32_t min_data_transfer_size;
    spdm_error_response_t *spdm_error;

    if (libspdm_get_connection_version(spdm_context) < SPDM_MESSAGE_VERSION_12) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    /* Fail if requester or responder does not support chunk cap */
    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHUNK_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP)) {
        return LIBSPDM_STATUS_ERROR_PEER;
    }

    /* now we can get sender buffer */
    transport_header_size = spdm_context->local_context.capability.transport_header_size;

    libspdm_get_scratch_buffer(spdm_context, (void**) &scratch_buffer, &scratch_buffer_size);

    /* Temporary send/receive buffers for chunking are in the scratch space */
    message = scratch_buffer + libspdm_get_scratch_buffer_sender_receiver_offset(spdm_context);
    message_size = libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context);

    send_info = &spdm_context->chunk_context.send;
    send_info->chunk_in_use = true;

    /* The first section of the scratch
     * buffer may be used for other purposes. Use only after that section. */
    send_info->large_message = scratch_buffer +
                               libspdm_get_scratch_buffer_large_message_offset(spdm_context);
    send_info->large_message_capacity =
        libspdm_get_scratch_buffer_large_message_capacity(spdm_context);

    libspdm_zero_mem(send_info->large_message, send_info->large_message_capacity);
    libspdm_copy_mem(send_info->large_message, send_info->large_message_capacity,
                     request, request_size);

    send_info->large_message_size = request_size;
    send_info->chunk_bytes_transferred = 0;
    send_info->chunk_seq_no = 0;
    request = NULL; /* Invalidate to prevent accidental use. */
    request_size = 0;

    min_data_transfer_size = LIBSPDM_MIN(
        spdm_context->connection_info.capability.data_transfer_size,
        spdm_context->local_context.capability.sender_data_transfer_size);

    do {
        LIBSPDM_ASSERT(send_info->large_message_capacity >= transport_header_size);
        spdm_request = (spdm_chunk_send_request_t*) ((uint8_t*) message + transport_header_size);
        spdm_request_size = message_size - transport_header_size;

        spdm_request->header.spdm_version = libspdm_get_connection_version(spdm_context);
        spdm_request->header.request_response_code = SPDM_CHUNK_SEND;
        spdm_request->header.param1 = 0;
        spdm_request->header.param2 = send_info->chunk_handle;
        spdm_request->chunk_seq_no = send_info->chunk_seq_no;
        spdm_request->reserved = 0;
        chunk_ptr = (uint8_t*) (spdm_request + 1);

        if (min_data_transfer_size
            - sizeof(spdm_chunk_send_request_t)
            < (send_info->large_message_size - send_info->chunk_bytes_transferred)) {

            copy_size = min_data_transfer_size
                        - sizeof(spdm_chunk_send_request_t);
        } else {
            copy_size = (send_info->large_message_size - send_info->chunk_bytes_transferred);
        }

        if (send_info->chunk_seq_no == 0) {
            *(uint32_t*) (spdm_request + 1) = (uint32_t) send_info->large_message_size;
            chunk_ptr += sizeof(uint32_t);
            copy_size -= sizeof(uint32_t);
        }

        spdm_request->chunk_size = (uint32_t) copy_size;

        libspdm_copy_mem(
            chunk_ptr, spdm_request_size - ((uint8_t*) spdm_request - (uint8_t*) message),
            (uint8_t*)send_info->large_message + send_info->chunk_bytes_transferred, copy_size);

        send_info->chunk_bytes_transferred += copy_size;
        if (send_info->chunk_bytes_transferred >= send_info->large_message_size) {
            spdm_request->header.param1 |= SPDM_CHUNK_SEND_REQUEST_ATTRIBUTE_LAST_CHUNK;
        }

        spdm_request_size = (chunk_ptr + copy_size) - (uint8_t*)spdm_request;
        status = libspdm_send_request(
            spdm_context, session_id, false,
            spdm_request_size, spdm_request);

        spdm_request = NULL;
        spdm_request_size = 0;

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            break;
        }

        response = message;
        response_size = message_size;

        libspdm_zero_mem(response, response_size);

        status = libspdm_receive_response(
            spdm_context, session_id, false,
            &response_size, &response);

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            break;
        }
        spdm_response = (void*) (response);

        if (response_size < sizeof(spdm_message_header_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            break;
        }
        if (spdm_response->header.spdm_version != libspdm_get_connection_version(spdm_context)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            break;
        }

        if (spdm_response->header.request_response_code == SPDM_ERROR
            && spdm_response->header.param1 == SPDM_ERROR_CODE_LARGE_RESPONSE) {

            /* It is possible that the CHUNK_SEND_ACK + chunk response is larger
             * than the DATA_TRANSFER_SIZE. In this case an ERROR_LARGE_RESPONSE
             * is returned directly in the response buffer rather than part of
             * the CHUNK_SEND_ACK. Store this error response in scratch buffer
             * to be handled when reading response. Also note that in this case
             * of large response, the CHUNK_SEND_ACK portion is not sent.
             * Only the response portion that requires the CHUNK_GET is sent */
            if (response_size < send_info->large_message_capacity) {
                libspdm_copy_mem(
                    send_info->large_message, send_info->large_message_capacity,
                    spdm_response, response_size);
                send_info->large_message_size = response_size;
                break;
            } else {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }
        } else {
            if (spdm_response->header.request_response_code != SPDM_CHUNK_SEND_ACK) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                break;
            }

            if (response_size < sizeof(spdm_chunk_send_ack_response_t)) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }
            if (spdm_response->header.param1
                & SPDM_CHUNK_SEND_ACK_RESPONSE_ATTRIBUTE_EARLY_ERROR_DETECTED) {

                spdm_error = (spdm_error_response_t *) (spdm_response + 1);
                if (response_size < (sizeof(spdm_chunk_send_ack_response_t) +
                                     sizeof(spdm_error_response_t))) {
                    status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                    break;
                }
                if ((spdm_error->header.spdm_version !=
                     libspdm_get_connection_version(spdm_context)) ||
                    (spdm_error->header.request_response_code != SPDM_ERROR)) {
                    status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                    break;
                }
                if (spdm_error->header.param1 == SPDM_ERROR_CODE_LARGE_RESPONSE) {
                    status = LIBSPDM_STATUS_ERROR_PEER;
                    break;
                }

                /* Store the error response in scratch buffer to be read by
                 * libspdm_receive_spdm_response and returned to its caller
                 * and handled in the error response handling flow */
                libspdm_copy_mem(
                    send_info->large_message,
                    send_info->large_message_capacity,
                    (uint8_t*) (spdm_response + 1),
                    response_size - sizeof(spdm_chunk_send_ack_response_t));

                send_info->large_message_size =
                    (response_size - sizeof(spdm_chunk_send_ack_response_t));

                status = LIBSPDM_STATUS_SUCCESS;
                break;
            }
            if (spdm_response->header.param2 != send_info->chunk_handle) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                break;
            }
            if (send_info->chunk_seq_no != spdm_response->chunk_seq_no) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                break;
            }

            chunk_ptr = (uint8_t*) (spdm_response + 1);
            send_info->chunk_seq_no++;

            if (send_info->chunk_bytes_transferred >= send_info->large_message_size) {

                /* All bytes have been transferred. Store response in scratch buffer
                 * to be read by libspdm_receive_spdm_response */
                libspdm_copy_mem(
                    send_info->large_message, send_info->large_message_capacity,
                    chunk_ptr, response_size - sizeof(spdm_chunk_send_ack_response_t));
                send_info->large_message_size =
                    (response_size - sizeof(spdm_chunk_send_ack_response_t));
                break;
            }
        }

    } while (LIBSPDM_STATUS_IS_SUCCESS(status)
             && send_info->chunk_bytes_transferred < send_info->large_message_size);

    if (LIBSPDM_STATUS_IS_ERROR(status)) {

        send_info->chunk_in_use = false;
        send_info->chunk_handle++; /* Implicit wrap-around*/
        send_info->chunk_seq_no = 0;
        send_info->chunk_bytes_transferred = 0;
        send_info->large_message = NULL;
        send_info->large_message_size = 0;
    }

    return status;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

libspdm_return_t libspdm_send_spdm_request(libspdm_context_t *spdm_context,
                                           const uint32_t *session_id,
                                           size_t request_size, void *request)
{
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;
    libspdm_return_t status;
    #if LIBSPDM_ENABLE_MSG_LOG
    size_t msg_log_size;
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    /* large SPDM message is the SPDM message whose size is greater than the DataTransferSize of the receiving
     * SPDM endpoint or greater than the transmit buffer size of the sending SPDM endpoint */
    if (((spdm_context->connection_info.capability.data_transfer_size != 0 &&
          request_size > spdm_context->connection_info.capability.data_transfer_size) ||
         (spdm_context->local_context.capability.sender_data_transfer_size != 0 &&
          request_size > spdm_context->local_context.capability.sender_data_transfer_size)) &&
        !libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHUNK_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP)) {
        return LIBSPDM_STATUS_SEND_FAIL;
    }

    if ((session_id != NULL) &&
        libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP)) {
        session_info = libspdm_get_session_info_via_session_id(
            spdm_context, *session_id);
        LIBSPDM_ASSERT(session_info != NULL);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        session_state = libspdm_secured_message_get_session_state(
            session_info->secured_message_context);
        if ((session_state == LIBSPDM_SESSION_STATE_HANDSHAKING) &&
            !session_info->use_psk) {
            session_id = NULL;
        }
    }

    if ((spdm_context->connection_info.capability.max_spdm_msg_size != 0) &&
        (request_size > spdm_context->connection_info.capability.max_spdm_msg_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "request_size > rsp max_spdm_msg_size\n"));
        return LIBSPDM_STATUS_PEER_BUFFER_TOO_SMALL;
    }
    LIBSPDM_ASSERT (request_size <= spdm_context->local_context.capability.max_spdm_msg_size);

    #if LIBSPDM_ENABLE_MSG_LOG
    /* First save the size of the message log buffer. If there is an error it will be reverted. */
    msg_log_size = libspdm_get_msg_log_size(spdm_context);
    libspdm_append_msg_log(spdm_context, request, request_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    /* large SPDM message is the SPDM message whose size is greater than the DataTransferSize of the receiving
     * SPDM endpoint or greater than the transmit buffer size of the sending SPDM endpoint */
    if (((const spdm_message_header_t*) request)->request_response_code != SPDM_GET_VERSION
        && ((const spdm_message_header_t*) request)->request_response_code != SPDM_GET_CAPABILITIES
        && ((spdm_context->connection_info.capability.data_transfer_size != 0 &&
             request_size > spdm_context->connection_info.capability.data_transfer_size) ||
            (spdm_context->local_context.capability.sender_data_transfer_size != 0 &&
             request_size > spdm_context->local_context.capability.sender_data_transfer_size))) {

        #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
        /* libspdm_send_request is not called with the original request in this flow.
         * This leads to the last_spdm_request field not having the original request value.
         * The caller assumes the request has been copied to last_spdm_request,
         * so that it can compare last_spdm_request's fields with response fields
         * Therefore the request must be copied to last_spdm_request here. */

        if (((const spdm_message_header_t*) request)->request_response_code != SPDM_RESPOND_IF_READY
            && ((const spdm_message_header_t*) request)->request_response_code != SPDM_CHUNK_GET
            && ((const spdm_message_header_t*) request)->request_response_code != SPDM_CHUNK_SEND) {
            libspdm_copy_mem(
                spdm_context->last_spdm_request,
                libspdm_get_scratch_buffer_last_spdm_request_capacity(spdm_context),
                request, request_size);
            spdm_context->last_spdm_request_size = request_size;
        }

        status = libspdm_handle_large_request(
            spdm_context, session_id, request_size, request);
        #else  /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP*/
        status = LIBSPDM_STATUS_BUFFER_TOO_SMALL;
        #endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP*/
    } else {
        status = libspdm_send_request(spdm_context, session_id, false, request_size, request);
    }

    #if LIBSPDM_ENABLE_MSG_LOG
    /* If there is an error in sending the request then revert the request in the message log. */
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        spdm_context->msg_log.buffer_size = msg_log_size;
    }
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    return status;
}

libspdm_return_t libspdm_receive_spdm_response(libspdm_context_t *spdm_context,
                                               const uint32_t *session_id,
                                               size_t *response_size,
                                               void **response)
{
    libspdm_return_t status;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;

    #if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    spdm_message_header_t *spdm_response;
    size_t response_capacity;
    libspdm_chunk_info_t *send_info;
    #endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

    if ((session_id != NULL) &&
        libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP)) {
        session_info = libspdm_get_session_info_via_session_id(
            spdm_context, *session_id);
        LIBSPDM_ASSERT(session_info != NULL);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        session_state = libspdm_secured_message_get_session_state(
            session_info->secured_message_context);
        if ((session_state == LIBSPDM_SESSION_STATE_HANDSHAKING) &&
            !session_info->use_psk) {
            session_id = NULL;
        }
    }

    #if !(LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP)
    status = libspdm_receive_response(spdm_context, session_id, false, response_size, response);
    #else /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */
    send_info = &spdm_context->chunk_context.send;
    if (send_info->chunk_in_use) {
        libspdm_copy_mem(*response, *response_size,
                         send_info->large_message, send_info->large_message_size);
        *response_size = send_info->large_message_size;
        response_capacity = send_info->large_message_capacity;

        /* This response may either be an actual response or ERROR_LARGE_RESPONSE,
         * the latter which should be handled in the large response handler. */

        send_info->chunk_in_use = false;
        send_info->chunk_handle++; /* Implicit wrap-around*/
        send_info->chunk_seq_no = 0;
        send_info->chunk_bytes_transferred = 0;
        send_info->large_message = NULL;
        send_info->large_message_size = 0;
        send_info->large_message_capacity = 0;
        status = LIBSPDM_STATUS_SUCCESS;
    } else {
        response_capacity = *response_size;
        status = libspdm_receive_response(spdm_context, session_id, false,
                                          response_size, response);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    }

    spdm_response = (spdm_message_header_t*) (*response);

    if (*response_size < sizeof(spdm_message_header_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }

    if (spdm_response->request_response_code == SPDM_ERROR
        && spdm_response->param1 == SPDM_ERROR_CODE_LARGE_RESPONSE) {
        status = libspdm_handle_error_large_response(
            spdm_context, session_id,
            response_size, (void*) spdm_response, response_capacity);

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }

        if (*response_size < sizeof(spdm_message_header_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }

        /* Per the spec, SPDM_VERSION and SPDM_CAPABILITIES shall not be chunked
         * and should be an unexpected error. */
        if (spdm_response->request_response_code == SPDM_VERSION ||
            spdm_response->request_response_code == SPDM_CAPABILITIES
            ) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    }

receive_done:
    #endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

    return status;
}
