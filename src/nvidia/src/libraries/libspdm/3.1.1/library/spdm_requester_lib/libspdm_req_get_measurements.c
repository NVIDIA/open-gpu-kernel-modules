/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP

bool libspdm_verify_measurement_signature(libspdm_context_t *spdm_context,
                                          libspdm_session_info_t *session_info,
                                          const void *sign_data,
                                          size_t sign_data_size)
{
    bool result;
    void *context;
    uint8_t slot_id;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_l1l2_managed_buffer_t l1l2;
    uint8_t *l1l2_buffer;
    size_t l1l2_buffer_size;
    const uint8_t *cert_chain_data;
    size_t cert_chain_data_size;
    const uint8_t *cert_buffer;
    size_t cert_buffer_size;
#else
    uint8_t l1l2_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t l1l2_hash_size;
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_calculate_l1l2(spdm_context, session_info, &l1l2);
    l1l2_buffer = libspdm_get_managed_buffer(&l1l2);
    l1l2_buffer_size = libspdm_get_managed_buffer_size(&l1l2);
#else
    l1l2_hash_size = sizeof(l1l2_hash);
    result = libspdm_calculate_l1l2_hash(spdm_context, session_info, &l1l2_hash_size, l1l2_hash);
#endif
    libspdm_reset_message_m(spdm_context, session_info);
    if (!result) {
        return false;
    }

    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xF));

    if (slot_id == 0xF) {
        result = libspdm_asym_get_public_key_from_der(
            spdm_context->connection_info.algorithm.base_asym_algo,
            spdm_context->local_context.peer_public_key_provision,
            spdm_context->local_context.peer_public_key_provision_size,
            &context);
        if (!result) {
            return false;
        }
    } else {
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        result = libspdm_get_peer_cert_chain_data(
            spdm_context, (const void **)&cert_chain_data, &cert_chain_data_size);
        if (!result) {
            return false;
        }

        /* Get leaf cert from cert chain*/
        result = libspdm_x509_get_cert_from_cert_chain(cert_chain_data,
                                                       cert_chain_data_size, -1,
                                                       &cert_buffer, &cert_buffer_size);
        if (!result) {
            return false;
        }

        result = libspdm_asym_get_public_key_from_x509(
            spdm_context->connection_info.algorithm.base_asym_algo,
            cert_buffer, cert_buffer_size, &context);
        if (!result) {
            return false;
        }
#else
        context = spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key;
        LIBSPDM_ASSERT(context != NULL);
#endif
    }

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_asym_verify_ex(
        spdm_context->connection_info.version, SPDM_MEASUREMENTS,
        spdm_context->connection_info.algorithm.base_asym_algo,
        spdm_context->connection_info.algorithm.base_hash_algo,
        context, l1l2_buffer, l1l2_buffer_size, sign_data, sign_data_size,
        &spdm_context->spdm_10_11_verify_signature_endian);
    libspdm_asym_free(spdm_context->connection_info.algorithm.base_asym_algo, context);
#else
    result = libspdm_asym_verify_hash_ex(
        spdm_context->connection_info.version, SPDM_MEASUREMENTS,
        spdm_context->connection_info.algorithm.base_asym_algo,
        spdm_context->connection_info.algorithm.base_hash_algo,
        context, l1l2_hash, l1l2_hash_size, sign_data, sign_data_size,
        &spdm_context->spdm_10_11_verify_signature_endian);
    if (slot_id == 0xF) {
        libspdm_asym_free(spdm_context->connection_info.algorithm.base_asym_algo, context);
    }
#endif
    if (!result) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_measurement_signature - FAIL !!!\n"));
        return false;
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_measurement_signature - PASS !!!\n"));
    return true;
}

/**
 * This function sends GET_MEASUREMENT to get measurement from the device.
 * If the signature is requested this function verifies the signature of the measurement.
 *
 * @param  context                    A pointer to the SPDM context.
 * @param  session_id                 Indicates if it is a secured message protected via SPDM session.
 *                                    If session_id is NULL, it is a normal message.
 *                                    If session_id is not NULL, it is a secured message.
 * @param  request_attribute          The request attribute of the request message.
 * @param  measurement_operation      The measurement operation of the request message.
 * @param  slot_id                    The number of slot for the certificate chain.
 * @param  content_changed            The measurement content changed output param.
 * @param  number_of_blocks           The number of blocks of the measurement record.
 * @param  measurement_record_length  On input, indicate the size in bytes of the destination buffer
 *                                    to store the measurement record.
 *                                    On output, indicate the size in bytes of the measurement record.
 * @param  measurement_record         A pointer to a destination buffer to store the measurement record.
 * @param  requester_nonce_in         If not NULL, a buffer that holds the requester nonce (32 bytes)
 * @param  requester_nonce            If not NULL, a buffer to hold the requester nonce (32 bytes).
 * @param  responder_nonce            If not NULL, a buffer to hold the responder nonce (32 bytes).
 *
 **/
static libspdm_return_t libspdm_try_get_measurement(libspdm_context_t *spdm_context,
                                                    const uint32_t *session_id,
                                                    uint8_t request_attribute,
                                                    uint8_t measurement_operation,
                                                    uint8_t slot_id_param,
                                                    uint8_t *content_changed,
                                                    uint8_t *number_of_blocks,
                                                    uint32_t *measurement_record_length,
                                                    void *measurement_record,
                                                    const void *requester_nonce_in,
                                                    void *requester_nonce,
                                                    void *responder_nonce,
                                                    void *opaque_data,
                                                    size_t *opaque_data_size)
{
    bool result;
    libspdm_return_t status;
    spdm_get_measurements_request_t *spdm_request;
    size_t spdm_request_size;
    spdm_measurements_response_t *spdm_response;
    size_t spdm_response_size;
    uint32_t measurement_record_data_length;
    uint8_t *measurement_record_data;
    spdm_measurement_block_common_header_t *measurement_block_header;
    uint32_t measurement_block_size;
    uint8_t measurement_block_count;
    uint8_t *ptr;
    void *nonce;
    uint16_t opaque_length;
    void *signature;
    size_t signature_size;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    /* -=[Check Parameters Phase]=- */
    LIBSPDM_ASSERT((slot_id_param < SPDM_MAX_SLOT_COUNT) || (slot_id_param == 0xF));
    LIBSPDM_ASSERT((slot_id_param != 0xf) ||
                   (spdm_context->local_context.peer_public_key_provision_size != 0));

    /* -=[Verify State Phase]=- */
    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true, 0,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    LIBSPDM_ASSERT(spdm_context->local_context.algorithm.measurement_spec != 0);

    if (spdm_context->connection_info.connection_state < LIBSPDM_CONNECTION_STATE_NEGOTIATED) {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    if (session_id == NULL) {
        session_info = NULL;
    } else {
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

    if (libspdm_is_capabilities_flag_supported(
            spdm_context, true, 0,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_NO_SIG) &&
        ((request_attribute & SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE) != 0)) {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    if ((request_attribute & SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE) != 0) {
        signature_size = libspdm_get_asym_signature_size(
            spdm_context->connection_info.algorithm.base_asym_algo);
    } else {
        signature_size = 0;
    }

    libspdm_reset_message_buffer_via_request_code(spdm_context, NULL, SPDM_GET_MEASUREMENTS);

    /* -=[Construct Request Phase]=- */
    spdm_context->connection_info.peer_used_cert_chain_slot_id = slot_id_param;
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
    spdm_request->header.request_response_code = SPDM_GET_MEASUREMENTS;
    spdm_request->header.param1 = request_attribute;
    spdm_request->header.param2 = measurement_operation;
    if ((request_attribute & SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE) != 0) {
        if (spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_11) {
            spdm_request_size = sizeof(spdm_get_measurements_request_t);
        } else {
            spdm_request_size = sizeof(spdm_get_measurements_request_t) -
                                sizeof(spdm_request->slot_id_param);
        }

        if (requester_nonce_in == NULL) {
            if(!libspdm_get_random_number(SPDM_NONCE_SIZE, spdm_request->nonce)) {
                libspdm_release_sender_buffer (spdm_context);
                return LIBSPDM_STATUS_LOW_ENTROPY;
            }
        } else {
            libspdm_copy_mem(spdm_request->nonce, sizeof(spdm_request->nonce),
                             requester_nonce_in, SPDM_NONCE_SIZE);
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "RequesterNonce - "));
        LIBSPDM_INTERNAL_DUMP_DATA(spdm_request->nonce, SPDM_NONCE_SIZE);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
        spdm_request->slot_id_param = slot_id_param;

        if (requester_nonce != NULL) {
            libspdm_copy_mem(requester_nonce, SPDM_NONCE_SIZE,
                             spdm_request->nonce, SPDM_NONCE_SIZE);
        }
    } else {
        spdm_request_size = sizeof(spdm_request->header);

        if (requester_nonce != NULL) {
            libspdm_zero_mem (requester_nonce, SPDM_NONCE_SIZE);
        }
    }

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
            &spdm_response_size, (void **)&spdm_response,
            SPDM_GET_MEASUREMENTS, SPDM_MEASUREMENTS);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    } else if (spdm_response->header.request_response_code != SPDM_MEASUREMENTS) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response_size < sizeof(spdm_measurements_response_t)) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }
    if (measurement_operation ==
        SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_TOTAL_NUMBER_OF_MEASUREMENTS) {
        if (spdm_response->number_of_blocks != 0) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    } else if (measurement_operation ==
               SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_ALL_MEASUREMENTS) {
        if ((spdm_response->number_of_blocks == 0) || (spdm_response->number_of_blocks == 0xff)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    } else {
        if (spdm_response->number_of_blocks != 1) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    }

    measurement_record_data_length = libspdm_read_uint24(spdm_response->measurement_record_length);
    if (measurement_operation ==
        SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_TOTAL_NUMBER_OF_MEASUREMENTS) {
        if (measurement_record_data_length != 0) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    } else {
        if (spdm_response_size <
            sizeof(spdm_measurements_response_t) +
            measurement_record_data_length) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "measurement_record_length - 0x%06x\n",
                       measurement_record_data_length));
    }

    measurement_record_data = (void *)(spdm_response + 1);

    if ((request_attribute & SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE) != 0) {
        if (spdm_response_size <
            sizeof(spdm_measurements_response_t) +
            measurement_record_data_length + SPDM_NONCE_SIZE + sizeof(uint16_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
        if ((spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_11) &&
            ((spdm_response->header.param2 & SPDM_MEASUREMENTS_RESPONSE_SLOT_ID_MASK)
             != slot_id_param)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
        ptr = measurement_record_data + measurement_record_data_length;
        nonce = ptr;
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "nonce (0x%x) - ", SPDM_NONCE_SIZE));
        LIBSPDM_INTERNAL_DUMP_DATA(nonce, SPDM_NONCE_SIZE);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
        ptr += SPDM_NONCE_SIZE;
        if (responder_nonce != NULL) {
            libspdm_copy_mem(responder_nonce, SPDM_NONCE_SIZE, nonce, SPDM_NONCE_SIZE);
        }

        opaque_length = libspdm_read_uint16((const uint8_t *)ptr);
        if (opaque_length > SPDM_MAX_OPAQUE_DATA_SIZE) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
        if (spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
            if (((spdm_context->connection_info.algorithm.other_params_support &
                  SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_MASK) ==
                 SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_NONE)
                && (opaque_length != 0)) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
        }
        ptr += sizeof(uint16_t);
        if (opaque_length != 0) {
            result = libspdm_process_general_opaque_data_check(spdm_context, opaque_length, ptr);
            if (!result) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
        }

        if (spdm_response_size <
            sizeof(spdm_measurements_response_t) +
            measurement_record_data_length + SPDM_NONCE_SIZE +
            sizeof(uint16_t) + opaque_length + signature_size) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
        spdm_response_size = sizeof(spdm_measurements_response_t) +
                             measurement_record_data_length +
                             SPDM_NONCE_SIZE + sizeof(uint16_t) +
                             opaque_length + signature_size;

        /* -=[Process Response Phase]=- */
        status = libspdm_append_message_m(spdm_context, session_info, spdm_request,
                                          spdm_request_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }

        status = libspdm_append_message_m(spdm_context, session_info, spdm_response,
                                          spdm_response_size - signature_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }

        if ((opaque_data != NULL) && (opaque_data_size != NULL)) {
            if (opaque_length >= *opaque_data_size) {
                status = LIBSPDM_STATUS_BUFFER_TOO_SMALL;
                goto receive_done;
            }
            libspdm_copy_mem(opaque_data, *opaque_data_size, ptr, opaque_length);
            *opaque_data_size = opaque_length;
        }
        ptr += opaque_length;

        signature = ptr;
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "signature (0x%x):\n", signature_size));
        LIBSPDM_INTERNAL_DUMP_HEX(signature, signature_size);

        result = libspdm_verify_measurement_signature(
            spdm_context, session_info, signature, signature_size);
        if (!result) {
            status = LIBSPDM_STATUS_VERIF_FAIL;
            goto receive_done;
        }

        libspdm_reset_message_m(spdm_context, session_info);
    } else {
        if (spdm_response_size <
            sizeof(spdm_measurements_response_t) +
            measurement_record_data_length + sizeof(uint16_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
        ptr = measurement_record_data + measurement_record_data_length;

        nonce = ptr;
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "nonce (0x%x) - ", SPDM_NONCE_SIZE));
        LIBSPDM_INTERNAL_DUMP_DATA(nonce, SPDM_NONCE_SIZE);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
        ptr += SPDM_NONCE_SIZE;
        if (responder_nonce != NULL) {
            libspdm_copy_mem(responder_nonce, SPDM_NONCE_SIZE, nonce, SPDM_NONCE_SIZE);
        }

        opaque_length = libspdm_read_uint16((const uint8_t *)ptr);
        if (opaque_length > SPDM_MAX_OPAQUE_DATA_SIZE) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
        ptr += sizeof(uint16_t);
        if (opaque_length != 0) {
            result = libspdm_process_general_opaque_data_check(spdm_context, opaque_length, ptr);
            if (!result) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
        }

        if (spdm_response_size <
            sizeof(spdm_measurements_response_t) +
            measurement_record_data_length + SPDM_NONCE_SIZE +
            sizeof(uint16_t) + opaque_length) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
        spdm_response_size = sizeof(spdm_measurements_response_t) +
                             measurement_record_data_length +
                             SPDM_NONCE_SIZE + sizeof(uint16_t) +
                             opaque_length;

        /* If a signature is not requested then content_changed must be 0. */
        if (spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
            if ((spdm_response->header.param2 & SPDM_MEASUREMENTS_RESPONSE_CONTENT_CHANGE_MASK)
                != 0) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
        }

        status = libspdm_append_message_m(spdm_context, session_info, spdm_request,
                                          spdm_request_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }

        status = libspdm_append_message_m(spdm_context, session_info, spdm_response,
                                          spdm_response_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    }

    if (content_changed != NULL) {
        *content_changed = 0;
        if ((spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) &&
            ((request_attribute &
              SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE) != 0)) {
            *content_changed =
                (spdm_response->header.param2 & SPDM_MEASUREMENTS_RESPONSE_CONTENT_CHANGE_MASK);
        }
    }
    if (measurement_operation ==
        SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_TOTAL_NUMBER_OF_MEASUREMENTS) {
        *number_of_blocks = spdm_response->header.param1;
        if (*number_of_blocks == 0xFF) {
            /* the number of block cannot be 0xFF, because index 0xFF will brings confusing.*/
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
        if (*number_of_blocks == 0x0) {
            /* the number of block cannot be 0x0, because a responder without measurement should clear capability flags.*/
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    } else {
        *number_of_blocks = spdm_response->number_of_blocks;
        if (*measurement_record_length < measurement_record_data_length) {
            status = LIBSPDM_STATUS_BUFFER_TOO_SMALL;
            goto receive_done;
        }
        if (measurement_record_data_length < sizeof(spdm_measurement_block_common_header_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }

        measurement_block_size = 0;
        measurement_block_count = 1;
        while (measurement_block_size < measurement_record_data_length) {
            measurement_block_header =
                (spdm_measurement_block_common_header_t *)&measurement_record_data
                [measurement_block_size];
            if (measurement_block_header->measurement_size >
                measurement_record_data_length -
                ((uint8_t *)measurement_block_header -
                 (uint8_t *)measurement_record_data)) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
            if (measurement_block_header->measurement_specification == 0 ||
                (measurement_block_header->measurement_specification &
                 (measurement_block_header->measurement_specification - 1))) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
            if (measurement_block_header->measurement_specification !=
                spdm_context->connection_info.algorithm.measurement_spec) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
            if (measurement_block_header->index == 0 || measurement_block_header->index == 0xFF) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
            if (measurement_operation !=
                SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_ALL_MEASUREMENTS) {
                if (measurement_block_header->index != measurement_operation) {
                    status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                    goto receive_done;
                }
            }
            if (measurement_block_count > *number_of_blocks) {
                status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
                goto receive_done;
            }
            measurement_block_count++;
            measurement_block_size = (uint32_t)(
                measurement_block_size +
                sizeof(spdm_measurement_block_common_header_t) +
                measurement_block_header->measurement_size);
        }

        *measurement_record_length = measurement_record_data_length;
        libspdm_copy_mem(measurement_record,
                         measurement_record_data_length,
                         measurement_record_data,
                         measurement_record_data_length);
    }

    status = LIBSPDM_STATUS_SUCCESS;

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

receive_done:
    if ((status != LIBSPDM_STATUS_SUCCESS) &&
        (status != LIBSPDM_STATUS_NOT_READY_PEER)) {
        libspdm_reset_message_m(spdm_context, session_info);
    }
    libspdm_release_receiver_buffer (spdm_context);
    return status;
}

libspdm_return_t libspdm_get_measurement(void *spdm_context, const uint32_t *session_id,
                                         uint8_t request_attribute,
                                         uint8_t measurement_operation,
                                         uint8_t slot_id_param,
                                         uint8_t *content_changed,
                                         uint8_t *number_of_blocks,
                                         uint32_t *measurement_record_length,
                                         void *measurement_record)
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
        status = libspdm_try_get_measurement(
            context, session_id, request_attribute,
            measurement_operation, slot_id_param, content_changed, number_of_blocks,
            measurement_record_length, measurement_record,
            NULL, NULL, NULL, NULL, NULL);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}

libspdm_return_t libspdm_get_measurement_ex(void *spdm_context, const uint32_t *session_id,
                                            uint8_t request_attribute,
                                            uint8_t measurement_operation,
                                            uint8_t slot_id_param,
                                            uint8_t *content_changed,
                                            uint8_t *number_of_blocks,
                                            uint32_t *measurement_record_length,
                                            void *measurement_record,
                                            const void *requester_nonce_in,
                                            void *requester_nonce,
                                            void *responder_nonce,
                                            void *opaque_data,
                                            size_t *opaque_data_size)
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
        status = libspdm_try_get_measurement(
            context, session_id, request_attribute,
            measurement_operation, slot_id_param, content_changed, number_of_blocks,
            measurement_record_length, measurement_record,
            requester_nonce_in,
            requester_nonce, responder_nonce,
            opaque_data, opaque_data_size);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP*/
