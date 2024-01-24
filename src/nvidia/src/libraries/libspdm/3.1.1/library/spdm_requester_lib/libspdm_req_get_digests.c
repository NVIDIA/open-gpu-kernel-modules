/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT

#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint8_t digest[LIBSPDM_MAX_HASH_SIZE * SPDM_MAX_SLOT_COUNT];
} libspdm_digests_response_max_t;
#pragma pack()

/**
 * This function sends GET_DIGESTS and receives DIGESTS *
 *
 * @param  context             A pointer to the SPDM context.
 * @param  slot_mask           Bitmask of the slots that contain certificates.
 * @param  total_digest_buffer A pointer to a destination buffer to store the digests.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_DIGESTS was sent and DIGESTS was received.
 * @retval LIBSPDM_STATUS_INVALID_STATE_LOCAL
 *         Cannot send GET_DIGESTS due to Requester's state.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP
 *         Cannot send GET_DIGESTS because the Requester's and/or Responder's CERT_CAP = 0.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the DIGESTS response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The DIGESTS response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_BUFFER_FULL
 *         The buffer used to store transcripts is exhausted.
 **/
static libspdm_return_t libspdm_try_get_digest(libspdm_context_t *spdm_context,
                                               const uint32_t *session_id,
                                               uint8_t *slot_mask,
                                               void *total_digest_buffer)
{
    libspdm_return_t status;
    spdm_get_digest_request_t *spdm_request;
    size_t spdm_request_size;
    libspdm_digests_response_max_t *spdm_response;
    size_t spdm_response_size;
    size_t digest_size;
    size_t digest_count;
    size_t index;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;

    /* -=[Verify State Phase]=- */
    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true, 0,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }
    if (spdm_context->connection_info.connection_state < LIBSPDM_CONNECTION_STATE_NEGOTIATED) {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    session_info = NULL;
    if (session_id != NULL) {
        session_info = libspdm_get_session_info_via_session_id(spdm_context, *session_id);
        if (session_info == NULL) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
        session_state = libspdm_secured_message_get_session_state(
            session_info->secured_message_context);
        if (session_state != LIBSPDM_SESSION_STATE_ESTABLISHED) {
            return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        }
    }

    libspdm_reset_message_buffer_via_request_code(spdm_context, session_info, SPDM_GET_DIGESTS);

    /* -=[Construct Request Phase]=- */
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
    spdm_request->header.request_response_code = SPDM_GET_DIGESTS;
    spdm_request->header.param1 = 0;
    spdm_request->header.param2 = 0;
    spdm_request_size = sizeof(spdm_get_digest_request_t);

    /* -=[Send Request Phase]=- */
    status = libspdm_send_spdm_request(spdm_context, session_id, spdm_request_size, spdm_request);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        return status;
    }
    libspdm_release_sender_buffer (spdm_context);
    spdm_request = (void *)spdm_context->last_spdm_request;

    /* -=[Receive Response Phase]=- */
    status = libspdm_acquire_receiver_buffer (spdm_context, &message_size, (void **)&message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }
    LIBSPDM_ASSERT (message_size >= transport_header_size);
    spdm_response = (void *)(message);
    spdm_response_size = message_size;

    libspdm_zero_mem(spdm_response, spdm_response_size);
    status = libspdm_receive_spdm_response(
        spdm_context, session_id, &spdm_response_size, (void **)&spdm_response);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    /* -=[Validate Response Phase]=- */
    if (spdm_response_size < sizeof(spdm_message_header_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    if (spdm_response->header.spdm_version != spdm_request->header.spdm_version) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->header.request_response_code == SPDM_ERROR) {
        status = libspdm_handle_error_response_main(
            spdm_context, session_id,
            &spdm_response_size,
            (void **)&spdm_response, SPDM_GET_DIGESTS, SPDM_DIGESTS);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    } else if (spdm_response->header.request_response_code != SPDM_DIGESTS) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response_size < sizeof(spdm_digest_response_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }

    digest_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
    if (slot_mask != NULL) {
        *slot_mask = spdm_response->header.param2;
    }

    digest_count = 0;
    for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
        if (spdm_response->header.param2 & (1 << index)) {
            digest_count++;
        }
    }
    if (digest_count == 0) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }

    if (spdm_response_size < sizeof(spdm_digest_response_t) + digest_count * digest_size) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    spdm_response_size = sizeof(spdm_digest_response_t) + digest_count * digest_size;

    /* -=[Process Response Phase]=- */
    if (session_id == NULL) {
        status = libspdm_append_message_b(spdm_context, spdm_request, spdm_request_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }

        status = libspdm_append_message_b(spdm_context, spdm_response, spdm_response_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    }

    for (index = 0; index < digest_count; index++) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "digest (0x%x) - ", index));
        LIBSPDM_INTERNAL_DUMP_DATA(&spdm_response->digest[digest_size * index], digest_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    if (total_digest_buffer != NULL) {
        libspdm_copy_mem(total_digest_buffer, digest_size * digest_count,
                         spdm_response->digest, digest_size * digest_count);
    }

    spdm_context->connection_info.peer_digest_slot_mask = spdm_response->header.param2;
    libspdm_copy_mem(
        spdm_context->connection_info.peer_total_digest_buffer,
        sizeof(spdm_context->connection_info.peer_total_digest_buffer),
        spdm_response->digest, digest_size * digest_count);

    /* -=[Update State Phase]=- */
    if (spdm_context->connection_info.connection_state < LIBSPDM_CONNECTION_STATE_AFTER_DIGESTS) {
        spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_AFTER_DIGESTS;
    }
    status = LIBSPDM_STATUS_SUCCESS;

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

receive_done:
    libspdm_release_receiver_buffer (spdm_context);
    return status;
}

libspdm_return_t libspdm_get_digest(void *spdm_context, const uint32_t *session_id,
                                    uint8_t *slot_mask, void *total_digest_buffer)
{
    libspdm_context_t *context;
    size_t retry;
    uint64_t retry_delay_time;
    libspdm_return_t status;

    context = spdm_context;
    context->crypto_request = true;
    retry = context->retry_times;
    retry_delay_time = context->retry_delay_time;
    do {
        status = libspdm_try_get_digest(context, session_id, slot_mask, total_digest_buffer);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}

#endif /* LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT */
