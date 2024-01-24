/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"
#include "internal/libspdm_secured_message_lib.h"

#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint8_t dummy_data[sizeof(spdm_error_data_response_not_ready_t)];
} libspdm_key_update_response_mine_t;
#pragma pack()

/**
 * This function sends KEY_UPDATE
 * to update keys for an SPDM Session.
 *
 * After keys are updated, this function also uses VERIFY_NEW_KEY to verify the key.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    The session ID of the session.
 * @param  single_direction              true means the operation is UPDATE_KEY.
 *                                     false means the operation is UPDATE_ALL_KEYS.
 * @param  key_updated                   true means the operation is to verify key(s).
 *                                     false means the operation is to update and verify key(s).
 *
 * @retval RETURN_SUCCESS               The keys of the session are updated.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
static libspdm_return_t libspdm_try_key_update(libspdm_context_t *spdm_context,
                                               uint32_t session_id,
                                               bool single_direction, bool *key_updated)
{
    libspdm_return_t status;
    bool result;
    spdm_key_update_request_t *spdm_request;
    size_t spdm_request_size;
    libspdm_key_update_response_mine_t *spdm_response;
    size_t spdm_response_size;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    if (libspdm_get_connection_version(spdm_context) < SPDM_MESSAGE_VERSION_11) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    if (spdm_context->connection_info.connection_state <
        LIBSPDM_CONNECTION_STATE_NEGOTIATED) {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }
    session_info =
        libspdm_get_session_info_via_session_id(spdm_context, session_id);
    if (session_info == NULL) {
        LIBSPDM_ASSERT(false);
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }
    session_state = libspdm_secured_message_get_session_state(
        session_info->secured_message_context);
    if (session_state != LIBSPDM_SESSION_STATE_ESTABLISHED) {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    libspdm_reset_message_buffer_via_request_code(spdm_context, session_info, SPDM_KEY_UPDATE);

    if(!(*key_updated)) {

        /* Update key*/

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

        spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
        spdm_request->header.request_response_code = SPDM_KEY_UPDATE;
        if (single_direction) {
            spdm_request->header.param1 = SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_KEY;
        } else {
            spdm_request->header.param1 = SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_ALL_KEYS;
        }
        spdm_request->header.param2 = 0;

        if(!libspdm_get_random_number(sizeof(spdm_request->header.param2),
                                      &spdm_request->header.param2)) {
            libspdm_release_sender_buffer (spdm_context);
            return LIBSPDM_STATUS_LOW_ENTROPY;
        }

        spdm_request_size = sizeof(spdm_key_update_request_t);

        /* If updating both, create new responder key*/
        if (!single_direction) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "libspdm_create_update_session_data_key[%x] Responder\n",
                           session_id));
            result = libspdm_create_update_session_data_key(
                session_info->secured_message_context,
                LIBSPDM_KEY_UPDATE_ACTION_RESPONDER);
            if (!result) {
                libspdm_release_sender_buffer (spdm_context);
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }

        status = libspdm_send_spdm_request(spdm_context, &session_id,
                                           spdm_request_size, spdm_request);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            return status;
        }

        libspdm_release_sender_buffer (spdm_context);
        spdm_request = (void *)spdm_context->last_spdm_request;

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
            spdm_context, &session_id, &spdm_response_size, (void **)&spdm_response);

        if (!LIBSPDM_STATUS_IS_ERROR(status)) {
            if (spdm_response->header.request_response_code == SPDM_ERROR) {
                status = libspdm_handle_error_response_main(
                    spdm_context, &session_id,
                    &spdm_response_size, (void **)&spdm_response,
                    SPDM_KEY_UPDATE, SPDM_KEY_UPDATE_ACK);
            } else if (spdm_response_size != sizeof(spdm_key_update_response_t)) {
                /* this message can only be in secured session thus
                 * don't need to consider transport layer padding, just check its exact size */
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            } else if (spdm_response->header.spdm_version != spdm_request->header.spdm_version) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            } else if ((spdm_response->header.request_response_code !=
                        SPDM_KEY_UPDATE_ACK) ||
                       (spdm_response->header.param1 != spdm_request->header.param1) ||
                       (spdm_response->header.param2 != spdm_request->header.param2)) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            }
        }

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            if (!single_direction) {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                               "libspdm_activate_update_session_data_key[%x] Responder old\n",
                               session_id));
                result = libspdm_activate_update_session_data_key(
                    session_info->secured_message_context,
                    LIBSPDM_KEY_UPDATE_ACTION_RESPONDER, false);
                if (!result) {
                    libspdm_release_receiver_buffer (spdm_context);
                    return LIBSPDM_STATUS_CRYPTO_ERROR;
                }
            }
            libspdm_release_receiver_buffer (spdm_context);
            return status;
        }

        if (!single_direction) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "libspdm_activate_update_session_data_key[%x] Responder new\n",
                           session_id, LIBSPDM_KEY_UPDATE_ACTION_RESPONDER));
            result = libspdm_activate_update_session_data_key(
                session_info->secured_message_context,
                LIBSPDM_KEY_UPDATE_ACTION_RESPONDER, true);
            if (!result) {
                libspdm_release_receiver_buffer (spdm_context);
                return LIBSPDM_STATUS_CRYPTO_ERROR;
            }
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_create_update_session_data_key[%x] Requester\n",
                       session_id));
        result = libspdm_create_update_session_data_key(
            session_info->secured_message_context,
            LIBSPDM_KEY_UPDATE_ACTION_REQUESTER);
        if (!result) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "libspdm_activate_update_session_data_key[%x] Requester new\n",
                       session_id));
        result = libspdm_activate_update_session_data_key(
            session_info->secured_message_context,
            LIBSPDM_KEY_UPDATE_ACTION_REQUESTER, true);
        if (!result) {
            libspdm_release_receiver_buffer (spdm_context);
            return LIBSPDM_STATUS_CRYPTO_ERROR;
        }

        /* -=[Log Message Phase]=- */
        #if LIBSPDM_ENABLE_MSG_LOG
        libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
        #endif /* LIBSPDM_ENABLE_MSG_LOG */

        libspdm_release_receiver_buffer (spdm_context);
    }

    *key_updated = true;


    /* Verify key*/

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

    spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
    spdm_request->header.request_response_code = SPDM_KEY_UPDATE;
    spdm_request->header.param1 = SPDM_KEY_UPDATE_OPERATIONS_TABLE_VERIFY_NEW_KEY;
    spdm_request->header.param2 = 1;
    if(!libspdm_get_random_number(sizeof(spdm_request->header.param2),
                                  &spdm_request->header.param2)) {
        libspdm_release_sender_buffer (spdm_context);
        return LIBSPDM_STATUS_LOW_ENTROPY;
    }
    spdm_request_size = sizeof(spdm_key_update_request_t);

    status = libspdm_send_spdm_request(spdm_context, &session_id,
                                       spdm_request_size, spdm_request);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        return status;
    }
    libspdm_release_sender_buffer (spdm_context);
    spdm_request = (void *)spdm_context->last_spdm_request;

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
        spdm_context, &session_id, &spdm_response_size, (void **)&spdm_response);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_receiver_buffer (spdm_context);
        return status;
    }

    if (spdm_response->header.request_response_code == SPDM_ERROR) {
        status = libspdm_handle_error_response_main(
            spdm_context, &session_id,
            &spdm_response_size, (void **)&spdm_response,
            SPDM_KEY_UPDATE, SPDM_KEY_UPDATE_ACK);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "SpdmVerifyKey[%x] Failed\n", session_id));
            libspdm_release_receiver_buffer (spdm_context);
            return status;
        }
    }

    /* this message can only be in secured session
     * thus don't need to consider transport layer padding, just check its exact size */
    if (spdm_response_size != sizeof(spdm_key_update_response_t)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "SpdmVerifyKey[%x] Failed\n", session_id));
        libspdm_release_receiver_buffer (spdm_context);
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }

    if (spdm_response->header.spdm_version != spdm_request->header.spdm_version) {
        libspdm_release_receiver_buffer (spdm_context);
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    if ((spdm_response->header.request_response_code !=
         SPDM_KEY_UPDATE_ACK) ||
        (spdm_response->header.param1 != spdm_request->header.param1) ||
        (spdm_response->header.param2 != spdm_request->header.param2)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "SpdmVerifyKey[%x] Failed\n", session_id));
        libspdm_release_receiver_buffer (spdm_context);
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "SpdmVerifyKey[%x] Success\n", session_id));

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    libspdm_release_receiver_buffer (spdm_context);
    return LIBSPDM_STATUS_SUCCESS;
}

libspdm_return_t libspdm_key_update(void *spdm_context, uint32_t session_id,
                                    bool single_direction)
{
    libspdm_context_t *context;
    size_t retry;
    uint64_t retry_delay_time;
    libspdm_return_t status;
    bool key_updated;

    context = spdm_context;
    key_updated = false;
    context->crypto_request = true;
    retry = context->retry_times;
    retry_delay_time = context->retry_delay_time;
    do {
        status = libspdm_try_key_update(spdm_context, session_id,
                                        single_direction, &key_updated);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}
