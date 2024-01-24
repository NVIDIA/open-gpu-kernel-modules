/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint8_t reserved;
    uint8_t version_number_entry_count;
    spdm_version_number_t version_number_entry[LIBSPDM_MAX_VERSION_COUNT];
} libspdm_version_response_max_t;
#pragma pack()

/**
 * This function sends GET_VERSION and receives VERSION.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  version_count        The number of SPDM versions that the Responder supports.
 * @param  VersionNumberEntries The list of SPDM versions that the Responder supports.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_VERSION was sent and VERSION was received.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the VERSION response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The VERSION response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_NEGOTIATION_FAIL
 *         The Requester and Responder do not support a common SPDM version.
 **/
static libspdm_return_t libspdm_try_get_version(libspdm_context_t *spdm_context,
                                                uint8_t *version_number_entry_count,
                                                spdm_version_number_t *version_number_entry)
{
    libspdm_return_t status;
    bool result;
    spdm_get_version_request_t *spdm_request;
    size_t spdm_request_size;
    libspdm_version_response_max_t *spdm_response;
    size_t spdm_response_size;
    spdm_version_number_t common_version;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    /* -=[Set State Phase]=- */
    libspdm_reset_message_a(spdm_context);
    libspdm_reset_message_b(spdm_context);
    libspdm_reset_message_c(spdm_context);
    libspdm_reset_context(spdm_context);
    libspdm_reset_message_buffer_via_request_code(spdm_context, NULL, SPDM_GET_VERSION);

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

    spdm_request->header.spdm_version = SPDM_MESSAGE_VERSION_10;
    spdm_request->header.request_response_code = SPDM_GET_VERSION;
    spdm_request->header.param1 = 0;
    spdm_request->header.param2 = 0;
    spdm_request_size = sizeof(spdm_get_version_request_t);

    /* -=[Send Request Phase]=- */
    status = libspdm_send_spdm_request(spdm_context, NULL, spdm_request_size, spdm_request);
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
    status = libspdm_receive_spdm_response(spdm_context, NULL, &spdm_response_size,
                                           (void **)&spdm_response);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    /* -=[Validate Response Phase]=- */
    if (spdm_response_size < sizeof(spdm_message_header_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    if (spdm_response->header.spdm_version != SPDM_MESSAGE_VERSION_10) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->header.request_response_code == SPDM_ERROR) {
        /* Responder shall not respond to the GET_VERSION request message with ErrorCode=ResponseNotReady.*/
        if (spdm_response->header.param1 == SPDM_ERROR_CODE_RESPONSE_NOT_READY) {
            /* Received an unexpected error message. */
            status = LIBSPDM_STATUS_ERROR_PEER;
            goto receive_done;
        }
        status = libspdm_handle_simple_error_response(spdm_context, spdm_response->header.param1);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    } else if (spdm_response->header.request_response_code != SPDM_VERSION) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response_size < sizeof(spdm_version_response_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    if (spdm_response->version_number_entry_count > LIBSPDM_MAX_VERSION_COUNT) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->version_number_entry_count == 0) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response_size < sizeof(spdm_version_response_t) +
        spdm_response->version_number_entry_count * sizeof(spdm_version_number_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    spdm_response_size = sizeof(spdm_version_response_t) +
                         spdm_response->version_number_entry_count * sizeof(spdm_version_number_t);

    /* -=[Process Response Phase]=- */
    status = libspdm_append_message_a(spdm_context, spdm_request, spdm_request_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    status = libspdm_append_message_a(spdm_context, spdm_response, spdm_response_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_reset_message_a(spdm_context);
        goto receive_done;
    }

    result = libspdm_negotiate_connection_version (
        &common_version,
        spdm_context->local_context.version.spdm_version,
        spdm_context->local_context.version.spdm_version_count,
        spdm_response->version_number_entry,
        spdm_response->version_number_entry_count);
    if (result == false) {
        libspdm_reset_message_a(spdm_context);
        status = LIBSPDM_STATUS_NEGOTIATION_FAIL;
        goto receive_done;
    }

    libspdm_copy_mem(&(spdm_context->connection_info.version),
                     sizeof(spdm_context->connection_info.version),
                     &(common_version), sizeof(spdm_version_number_t));

    if (version_number_entry_count != NULL && version_number_entry != NULL) {
        if (*version_number_entry_count < spdm_response->version_number_entry_count) {
            *version_number_entry_count = spdm_response->version_number_entry_count;
            libspdm_reset_message_a(spdm_context);
            status = LIBSPDM_STATUS_BUFFER_TOO_SMALL;
            goto receive_done;
        } else {
            *version_number_entry_count = spdm_response->version_number_entry_count;
            libspdm_copy_mem(version_number_entry,
                             spdm_response->version_number_entry_count *
                             sizeof(spdm_version_number_t),
                             spdm_response->version_number_entry,
                             spdm_response->version_number_entry_count *
                             sizeof(spdm_version_number_t));
            libspdm_version_number_sort (version_number_entry, *version_number_entry_count);
        }
    }

    /* -=[Update State Phase]=- */
    spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_AFTER_VERSION;
    status = LIBSPDM_STATUS_SUCCESS;

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    /*Set the role of device*/
    spdm_context->local_context.is_requester = true;

receive_done:
    libspdm_release_receiver_buffer (spdm_context);
    return status;
}

libspdm_return_t libspdm_get_version(libspdm_context_t *spdm_context,
                                     uint8_t *version_number_entry_count,
                                     spdm_version_number_t *version_number_entry)
{
    size_t retry;
    uint64_t retry_delay_time;
    libspdm_return_t status;

    spdm_context->crypto_request = false;
    retry = spdm_context->retry_times;
    retry_delay_time = spdm_context->retry_delay_time;
    do {
        status = libspdm_try_get_version(spdm_context,
                                         version_number_entry_count, version_number_entry);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}
