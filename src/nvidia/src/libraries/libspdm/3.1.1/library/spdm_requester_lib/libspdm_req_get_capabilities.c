/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

/**
 * This function validates the Responder's capabilities.
 *
 * @param  capabilities_flag The Responder's CAPABILITIES.Flags field.
 * @param  version           The SPDM message version.
 *
 * @retval true  The field is valid.
 * @retval false The field is invalid.
 **/
static bool validate_responder_capability(uint32_t capabilities_flag, uint8_t version)
{
    /*uint8_t cache_cap = (uint8_t)(capabilities_flag)&0x01;*/
    const uint8_t cert_cap = (uint8_t)(capabilities_flag >> 1) & 0x01;
    const uint8_t chal_cap = (uint8_t)(capabilities_flag >> 2) & 0x01;
    const uint8_t meas_cap = (uint8_t)(capabilities_flag >> 3) & 0x03;
    const uint8_t meas_fresh_cap = (uint8_t)(capabilities_flag >> 5) & 0x01;
    const uint8_t encrypt_cap = (uint8_t)(capabilities_flag >> 6) & 0x01;
    const uint8_t mac_cap = (uint8_t)(capabilities_flag >> 7) & 0x01;
    const uint8_t mut_auth_cap = (uint8_t)(capabilities_flag >> 8) & 0x01;
    const uint8_t key_ex_cap = (uint8_t)(capabilities_flag >> 9) & 0x01;
    const uint8_t psk_cap = (uint8_t)(capabilities_flag >> 10) & 0x03;
    const uint8_t encap_cap = (uint8_t)(capabilities_flag >> 12) & 0x01;
    const uint8_t hbeat_cap = (uint8_t)(capabilities_flag >> 13) & 0x01;
    const uint8_t key_upd_cap = (uint8_t)(capabilities_flag >> 14) & 0x01;
    const uint8_t handshake_in_the_clear_cap = (uint8_t)(capabilities_flag >> 15) & 0x01;
    const uint8_t pub_key_id_cap = (uint8_t)(capabilities_flag >> 16) & 0x01;
    /* uint8_t chunk_cap = (uint8_t)(capabilities_flag >> 17) & 0x01; */
    const uint8_t alias_cert_cap = (uint8_t)(capabilities_flag >> 18) & 0x01;
    const uint8_t set_cert_cap = (uint8_t)(capabilities_flag >> 19) & 0x01;
    const uint8_t csr_cap = (uint8_t)(capabilities_flag >> 20) & 0x01;
    const uint8_t cert_install_reset_cap = (uint8_t)(capabilities_flag >> 21) & 0x01;

    /* Checks common to all SPDM versions. */

    /* Illegal to return reserved value. */
    if (meas_cap == 3) {
        return false;
    }

    /* If MEAS_FRESH_CAP is set then MEAS_CAP must be set. */
    if ((meas_cap == 0) && (meas_fresh_cap == 1)) {
        return false;
    }

    if (version == SPDM_MESSAGE_VERSION_10) {
        /* If measurements are not signed then CERT_CAP must equal CHAL_CAP.
         * If measurements are signed then CERT_CAP must be set. */
        if ((meas_cap == 0) || (meas_cap == 1)) {
            if (cert_cap != chal_cap) {
                return false;
            }
        } else if (meas_cap == 2) {
            if (cert_cap == 0) {
                return false;
            }
        }
        return true;
    }

    /* Checks common to 1.1 and 1.2. */
    if ((version == SPDM_MESSAGE_VERSION_11) || (version == SPDM_MESSAGE_VERSION_12)) {
        /* Illegal to return reserved values. */
        if (psk_cap == 3) {
            return false;
        }

        /* Checks that originate from key exchange capabilities. */
        if ((key_ex_cap == 1) || (psk_cap != 0)) {
            /* While clearing MAC_CAP and setting ENCRYPT_CAP is legal according to DSP0274, libspdm
             * also implements DSP0277 secure messages, which requires at least MAC_CAP to be set.
             */
            if (mac_cap == 0) {
                return false;
            }
        } else {
            if ((mac_cap == 1) || (encrypt_cap == 1) || (handshake_in_the_clear_cap == 1) ||
                (hbeat_cap == 1) || (key_upd_cap == 1)) {
                return false;
            }
        }
        if ((key_ex_cap == 0) && (psk_cap != 0)) {
            if (handshake_in_the_clear_cap == 1) {
                return false;
            }
        }

        /* Checks that originate from certificate or public key capabilities. */
        if ((cert_cap == 1) || (pub_key_id_cap == 1)) {
            /* Certificate capabilities and public key capabilities cannot both be set. */
            if ((cert_cap == 1) && (pub_key_id_cap == 1)) {
                return false;
            }
            if ((chal_cap == 0) && (key_ex_cap == 0) && ((meas_cap == 0) || (meas_cap == 1))) {
                return false;
            }
        } else {
            /* If certificates or public keys are not enabled then these capabilities
             * cannot be enabled. */
            if ((chal_cap == 1) || (key_ex_cap == 1) || (meas_cap == 2) || (mut_auth_cap == 1)) {
                return false;
            }
        }

        /* Checks that originate from mutual authentication capabilities. */
        if (mut_auth_cap == 1) {
            if ((key_ex_cap == 0) && (chal_cap == 0)) {
                return false;
            }
        }
    }

    /* Checks specific to 1.1. */
    if (version == SPDM_MESSAGE_VERSION_11) {
        if ((mut_auth_cap == 1) && (encap_cap == 0)) {
            return false;
        }
    }

    /* Checks specific to 1.2. */
    if (version == SPDM_MESSAGE_VERSION_12) {
        if ((cert_cap == 0) && ((alias_cert_cap == 1) || (set_cert_cap == 1))) {
            return false;
        }
        if ((csr_cap == 1) && (set_cert_cap == 0)) {
            return false;
        }
        if ((cert_install_reset_cap == 1) && (csr_cap == 0) && (set_cert_cap == 0)) {
            return false;
        }
    }

    return true;
}

/**
 * This function sends GET_CAPABILITIES and receives CAPABILITIES.
 *
 * @param  spdm_context A pointer to the SPDM context.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_CAPABILITIES was sent and CAPABILITIES was received.
 * @retval LIBSPDM_STATUS_INVALID_STATE_LOCAL
 *         Cannot send GET_CAPABILITIES due to Requester's state. Send GET_VERSION first.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the CAPABILITIES response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The CAPABILITIES response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_BUFFER_FULL
 *         The buffer used to store transcripts is exhausted.
 **/
static libspdm_return_t libspdm_try_get_capabilities(libspdm_context_t *spdm_context)
{
    libspdm_return_t status;
    spdm_get_capabilities_request_t *spdm_request;
    size_t spdm_request_size;
    spdm_capabilities_response_t *spdm_response;
    size_t spdm_response_size;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    /* -=[Verify State Phase]=- */
    if (spdm_context->connection_info.connection_state != LIBSPDM_CONNECTION_STATE_AFTER_VERSION) {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }
    libspdm_reset_message_buffer_via_request_code(spdm_context, NULL, SPDM_GET_CAPABILITIES);

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

    libspdm_zero_mem(spdm_request, sizeof(spdm_get_capabilities_request_t));
    spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
    if (spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
        spdm_request_size = sizeof(spdm_get_capabilities_request_t);
    } else if (spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_11) {
        spdm_request_size = sizeof(spdm_get_capabilities_request_t) -
                            sizeof(spdm_request->data_transfer_size) -
                            sizeof(spdm_request->max_spdm_msg_size);
    } else {
        spdm_request_size = sizeof(spdm_request->header);
    }
    spdm_request->header.request_response_code = SPDM_GET_CAPABILITIES;
    spdm_request->header.param1 = 0;
    spdm_request->header.param2 = 0;
    spdm_request->ct_exponent = spdm_context->local_context.capability.ct_exponent;
    spdm_request->flags = spdm_context->local_context.capability.flags;
    spdm_request->data_transfer_size = spdm_context->local_context.capability.data_transfer_size;
    spdm_request->max_spdm_msg_size = spdm_context->local_context.capability.max_spdm_msg_size;

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
    if (spdm_response->header.spdm_version != spdm_request->header.spdm_version) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->header.request_response_code == SPDM_ERROR) {
        status = libspdm_handle_simple_error_response(
            spdm_context, spdm_response->header.param1);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    } else if (spdm_response->header.request_response_code != SPDM_CAPABILITIES) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
        if (spdm_response_size < sizeof(spdm_capabilities_response_t)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
    } else {
        if (spdm_response_size < sizeof(spdm_capabilities_response_t) -
            sizeof(spdm_response->data_transfer_size) - sizeof(spdm_response->max_spdm_msg_size)) {
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto receive_done;
        }
    }
    if (spdm_request->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
        spdm_response_size = sizeof(spdm_capabilities_response_t);
    } else {
        spdm_response_size = sizeof(spdm_capabilities_response_t) -
                             sizeof(spdm_response->data_transfer_size) -
                             sizeof(spdm_response->max_spdm_msg_size);
    }

    if (!validate_responder_capability(spdm_response->flags, spdm_response->header.spdm_version)) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }
    if (spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
        if ((spdm_response->data_transfer_size < SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12) ||
            (spdm_response->data_transfer_size > spdm_response->max_spdm_msg_size)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }

        if (((spdm_response->flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP) == 0) &&
            (spdm_response->data_transfer_size != spdm_response->max_spdm_msg_size)) {
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto receive_done;
        }
    }

    if (spdm_response->ct_exponent > LIBSPDM_MAX_CT_EXPONENT) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }

    /* -=[Process Response Phase]=- */
    status = libspdm_append_message_a(spdm_context, spdm_request, spdm_request_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    status = libspdm_append_message_a(spdm_context, spdm_response, spdm_response_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    spdm_context->connection_info.capability.ct_exponent = spdm_response->ct_exponent;

    if (spdm_response->header.spdm_version == SPDM_MESSAGE_VERSION_10) {
        spdm_context->connection_info.capability.flags =
            spdm_response->flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_10_MASK;
    } else if (spdm_response->header.spdm_version == SPDM_MESSAGE_VERSION_11) {
        spdm_context->connection_info.capability.flags =
            spdm_response->flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_11_MASK;
    } else {
        spdm_context->connection_info.capability.flags =
            spdm_response->flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_12_MASK;
    }

    if (spdm_response->header.spdm_version >= SPDM_MESSAGE_VERSION_12) {
        spdm_context->connection_info.capability.data_transfer_size =
            spdm_response->data_transfer_size;
        spdm_context->connection_info.capability.max_spdm_msg_size =
            spdm_response->max_spdm_msg_size;
    } else {
        spdm_context->connection_info.capability.data_transfer_size = 0;
        spdm_context->connection_info.capability.max_spdm_msg_size = 0;
    }

    /* -=[Update State Phase]=- */
    spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_AFTER_CAPABILITIES;
    status = LIBSPDM_STATUS_SUCCESS;

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

receive_done:
    libspdm_release_receiver_buffer (spdm_context);
    return status;
}

libspdm_return_t libspdm_get_capabilities(libspdm_context_t *spdm_context)
{
    size_t retry;
    uint64_t retry_delay_time;
    libspdm_return_t status;

    spdm_context->crypto_request = false;
    retry = spdm_context->retry_times;
    retry_delay_time = spdm_context->retry_delay_time;
    do {
        status = libspdm_try_get_capabilities(spdm_context);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}
