/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_RESPOND_IF_READY_SUPPORT
/**
 * This function sends RESPOND_IF_READY and receives an expected SPDM response.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  response_size                 The size of the response.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned.
 * @param  response                     The SPDM response message.
 * @param  expected_response_code         Indicate the expected response code.
 *
 * @retval RETURN_SUCCESS               The RESPOND_IF_READY is sent and an expected SPDM response is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
static libspdm_return_t libspdm_requester_respond_if_ready(libspdm_context_t *spdm_context,
                                                           const uint32_t *session_id,
                                                           size_t *response_size,
                                                           void **response,
                                                           uint8_t expected_response_code)
{
    libspdm_return_t status;
    spdm_response_if_ready_request_t *spdm_request;
    size_t spdm_request_size;
    spdm_message_header_t *spdm_response;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    /* the response might be in response buffer in normal SPDM message
     * or it is in scratch buffer in case of secure SPDM message
     * the response buffer is in acquired state, so we release it*/
    libspdm_release_receiver_buffer (spdm_context);

    /* now we can get sender buffer */
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
    spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
    spdm_request->header.request_response_code = SPDM_RESPOND_IF_READY;
    spdm_request->header.param1 = spdm_context->error_data.request_code;
    spdm_request->header.param2 = spdm_context->error_data.token;
    spdm_request_size = sizeof(spdm_response_if_ready_request_t);
    status = libspdm_send_spdm_request(spdm_context, session_id, spdm_request_size, spdm_request);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        /* need acquire response buffer, so that the caller can release it */
        status = libspdm_acquire_receiver_buffer (spdm_context, response_size, response);
        return status;
    }
    libspdm_release_sender_buffer (spdm_context);
    spdm_request = (void *)spdm_context->last_spdm_request;

    /* receive
     * do not release response buffer in case of error, because caller will release it*/

    status = libspdm_acquire_receiver_buffer (spdm_context, response_size, response);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }
    LIBSPDM_ASSERT (*response_size >= transport_header_size);

    libspdm_zero_mem(*response, *response_size);
    status = libspdm_receive_spdm_response(spdm_context, session_id,
                                           response_size, response);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }
    spdm_response = (void *)(*response);
    if (*response_size < sizeof(spdm_message_header_t)) {
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }
    if (spdm_response->spdm_version != spdm_request->header.spdm_version) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    if (spdm_response->request_response_code == SPDM_ERROR) {
        status = libspdm_handle_simple_error_response(spdm_context, spdm_response->param1);
        return status;
    }
    if (spdm_response->request_response_code != expected_response_code) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    return LIBSPDM_STATUS_SUCCESS;
}
#endif /* LIBSPDM_RESPOND_IF_READY_SUPPORT */

libspdm_return_t libspdm_handle_simple_error_response(libspdm_context_t *spdm_context,
                                                      uint8_t error_code)
{
    spdm_set_certificate_request_t *last_spdm_request;

    if (error_code == SPDM_ERROR_CODE_RESPONSE_NOT_READY) {
        return LIBSPDM_STATUS_NOT_READY_PEER;
    }

    if (error_code == SPDM_ERROR_CODE_BUSY) {
        return LIBSPDM_STATUS_BUSY_PEER;
    }

    last_spdm_request = (void *)spdm_context->last_spdm_request;
    if ((last_spdm_request->header.request_response_code == SPDM_SET_CERTIFICATE) ||
        (last_spdm_request->header.request_response_code == SPDM_GET_CSR)) {

        /* Do not check the Responder's CERT_INSTALL_RESET_CAP capability as it may be a 1.2.0
         * Responder and that capability does not exist. */

        if (error_code == SPDM_ERROR_CODE_RESET_REQUIRED) {
            return LIBSPDM_STATUS_RESET_REQUIRED_PEER;
        }
    }

    if (error_code == SPDM_ERROR_CODE_REQUEST_RESYNCH) {
        spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_NOT_STARTED;
        return LIBSPDM_STATUS_RESYNCH_PEER;
    }

    return LIBSPDM_STATUS_ERROR_PEER;
}

#if LIBSPDM_RESPOND_IF_READY_SUPPORT
/**
 * This function handles RESPONSE_NOT_READY error code.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  response_size                 The size of the response.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned.
 * @param  response                     The SPDM response message.
 * @param  original_request_code          Indicate the orginal request code.
 * @param  expected_response_code         Indicate the expected response code.
 *
 * @retval RETURN_SUCCESS               The RESPOND_IF_READY is sent and an expected SPDM response is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
static libspdm_return_t libspdm_handle_response_not_ready(libspdm_context_t *spdm_context,
                                                          const uint32_t *session_id,
                                                          size_t *response_size,
                                                          void **response,
                                                          uint8_t original_request_code,
                                                          uint8_t expected_response_code)
{
    spdm_error_response_t *spdm_response;
    spdm_error_data_response_not_ready_t *extend_error_data;

    if(*response_size < sizeof(spdm_error_response_t) +
       sizeof(spdm_error_data_response_not_ready_t)) {
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }

    spdm_response = *response;
    extend_error_data = (spdm_error_data_response_not_ready_t *)(spdm_response + 1);
    LIBSPDM_ASSERT(spdm_response->header.request_response_code == SPDM_ERROR);
    LIBSPDM_ASSERT(spdm_response->header.param1 == SPDM_ERROR_CODE_RESPONSE_NOT_READY);

    if (extend_error_data->request_code != original_request_code) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    if (extend_error_data->rd_tm <= 1) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    if (extend_error_data->rd_exponent > LIBSPDM_MAX_RDT_EXPONENT) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    spdm_context->error_data.rd_exponent = extend_error_data->rd_exponent;
    spdm_context->error_data.request_code = extend_error_data->request_code;
    spdm_context->error_data.token = extend_error_data->token;
    spdm_context->error_data.rd_tm = extend_error_data->rd_tm;

    libspdm_sleep((uint64_t)1 << extend_error_data->rd_exponent);

    return libspdm_requester_respond_if_ready(spdm_context, session_id,
                                              response_size, response,
                                              expected_response_code);
}
#endif /* LIBSPDM_RESPOND_IF_READY_SUPPORT */

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
libspdm_return_t libspdm_handle_error_large_response(
    libspdm_context_t *spdm_context,
    const uint32_t *session_id,
    size_t *inout_response_size,
    void *inout_response,
    size_t response_capacity)
{
    libspdm_return_t status;
    uint8_t chunk_handle;
    spdm_error_response_t* error_response;
    spdm_error_data_large_response_t* extend_error_data;

    spdm_chunk_get_request_t* spdm_request;
    size_t spdm_request_size;
    spdm_chunk_response_response_t* spdm_response;
    uint8_t* message;
    size_t message_size;
    size_t transport_header_size;

    uint8_t* scratch_buffer;
    size_t scratch_buffer_size;
    uint16_t chunk_seq_no;
    uint8_t* chunk_ptr;
    uint8_t* large_response;
    size_t large_response_capacity;
    size_t large_response_size;
    size_t large_response_size_so_far;

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

    if (*inout_response_size < sizeof(spdm_error_response_t) +
        sizeof(spdm_error_data_large_response_t)) {
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }

    error_response = inout_response;
    extend_error_data =
        (spdm_error_data_large_response_t*)(error_response + 1);
    chunk_handle = extend_error_data->handle;

    /* now we can get sender buffer */
    transport_header_size = spdm_context->local_context.capability.transport_header_size;

    libspdm_get_scratch_buffer(spdm_context, (void**)&scratch_buffer, &scratch_buffer_size);

    /* The first section of the scratch
     * buffer may be used for other purposes. Use only after that section. */
    large_response = scratch_buffer + libspdm_get_scratch_buffer_large_message_offset(spdm_context);
    large_response_capacity = libspdm_get_scratch_buffer_large_message_capacity(spdm_context);

    /* Temporary send/receive buffers for chunking are in the scratch space */
    message = scratch_buffer + libspdm_get_scratch_buffer_sender_receiver_offset(spdm_context);
    message_size = libspdm_get_scratch_buffer_sender_receiver_capacity(spdm_context);

    libspdm_zero_mem(large_response, large_response_capacity);
    large_response_size = 0;
    large_response_size_so_far = 0;
    chunk_seq_no = 0;

    do {
        LIBSPDM_ASSERT(message_size >= transport_header_size);
        spdm_request = (spdm_chunk_get_request_t*)(void*) (message + transport_header_size);
        spdm_request_size = message_size - transport_header_size;

        spdm_request->header.spdm_version = libspdm_get_connection_version(spdm_context);
        spdm_request->header.request_response_code = SPDM_CHUNK_GET;
        spdm_request->header.param1 = 0;
        spdm_request->header.param2 = chunk_handle;
        spdm_request->chunk_seq_no = chunk_seq_no;
        spdm_request_size = sizeof(spdm_chunk_get_request_t);

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "CHUNK_GET Handle %d SeqNo %d\n", chunk_handle, chunk_seq_no));

        status = libspdm_send_spdm_request(spdm_context, session_id,
                                           spdm_request_size, spdm_request);
        spdm_request = NULL;
        spdm_request_size = 0;
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            break;
        }

        libspdm_zero_mem(message, message_size);
        void* response = message;
        size_t response_size = message_size;

        status = libspdm_receive_spdm_response(
            spdm_context, session_id,
            &response_size, &response);

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            break;
        }
        spdm_response = (void*) (response);
        if (spdm_response->header.spdm_version != libspdm_get_connection_version(spdm_context)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            break;
        }
        if (spdm_response->header.request_response_code == SPDM_ERROR) {
            status = libspdm_handle_simple_error_response(spdm_context,
                                                          spdm_response->header.param1);
            break;
        }
        if (spdm_response->header.request_response_code != SPDM_CHUNK_RESPONSE) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            break;
        }
        if (chunk_seq_no == 0) {

            if (response_size
                < (sizeof(spdm_chunk_response_response_t) + sizeof(uint32_t))) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }
            if (response_size < SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }

            if (spdm_response->chunk_size
                < SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12
                - sizeof(spdm_chunk_response_response_t)
                - sizeof(uint32_t)) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }

            large_response_size = *(uint32_t*) (spdm_response + 1);
            chunk_ptr = (uint8_t*) (((uint32_t*) (spdm_response + 1)) + 1);

            if (spdm_response->chunk_size > large_response_size) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                break;
            }
            if (large_response_size > spdm_context->local_context.capability.max_spdm_msg_size) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                break;
            }
        } else {
            if (response_size < sizeof(spdm_chunk_response_response_t)) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }
            if (spdm_response->chunk_size + large_response_size_so_far > large_response_size) {
                status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                break;
            }

            if (!(spdm_response->header.param1 & SPDM_CHUNK_GET_RESPONSE_ATTRIBUTE_LAST_CHUNK)) {
                if (response_size < SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12) {
                    status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                    break;
                }
                if (spdm_response->chunk_size
                    < SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12
                    - sizeof(spdm_chunk_response_response_t)) {
                    status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
                    break;
                }
            }

            chunk_ptr = (uint8_t*) (spdm_response + 1);
        }

        if (spdm_response->chunk_seq_no != chunk_seq_no) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            break;
        }

        libspdm_copy_mem(large_response + large_response_size_so_far,
                         large_response_size - large_response_size_so_far,
                         chunk_ptr, spdm_response->chunk_size);

        large_response_size_so_far += spdm_response->chunk_size;

        chunk_seq_no++;

    } while (LIBSPDM_STATUS_IS_SUCCESS(status)
             && large_response_size_so_far < large_response_size
             && !(spdm_response->header.param1 & SPDM_CHUNK_GET_RESPONSE_ATTRIBUTE_LAST_CHUNK));


    if (LIBSPDM_STATUS_IS_SUCCESS(status)) {
        if (large_response_size_so_far != large_response_size) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        } else if (large_response_size <= response_capacity) {
            libspdm_copy_mem(inout_response, response_capacity,
                             large_response, large_response_size);
            *inout_response_size = large_response_size;

            LIBSPDM_INTERNAL_DUMP_HEX(large_response, large_response_size);
        }
    }

    return status;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

libspdm_return_t libspdm_handle_error_response_main(
    libspdm_context_t *spdm_context, const uint32_t *session_id,
    size_t *response_size, void **response,
    uint8_t original_request_code, uint8_t expected_response_code)
{
    spdm_message_header_t *spdm_response;

    spdm_response = *response;
    LIBSPDM_ASSERT(spdm_response->request_response_code == SPDM_ERROR);

    if ((spdm_response->param1 == SPDM_ERROR_CODE_DECRYPT_ERROR) && (session_id != NULL)) {
        libspdm_free_session_id(spdm_context, *session_id);
        return LIBSPDM_STATUS_SESSION_MSG_ERROR;
    } else if(spdm_response->param1 == SPDM_ERROR_CODE_RESPONSE_NOT_READY) {
        #if LIBSPDM_RESPOND_IF_READY_SUPPORT
        return libspdm_handle_response_not_ready(spdm_context, session_id,
                                                 response_size, response,
                                                 original_request_code,
                                                 expected_response_code);
        #else
        return LIBSPDM_STATUS_NOT_READY_PEER;
        #endif /* LIBSPDM_RESPOND_IF_READY_SUPPORT */
    } else {
        return libspdm_handle_simple_error_response(spdm_context, spdm_response->param1);
    }
}
