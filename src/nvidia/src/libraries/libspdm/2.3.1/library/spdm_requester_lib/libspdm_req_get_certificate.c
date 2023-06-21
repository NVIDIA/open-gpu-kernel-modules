/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_CERT_CAP

// NVIDIA_EDIT: Include malloclib for dynamic allocation.
#include "library/malloclib.h"

#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint16_t portion_length;
    uint16_t remainder_length;
    uint8_t cert_chain[LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN];
} libspdm_certificate_response_max_t;
#pragma pack()

/**
 * This function sends GET_CERTIFICATE and receives CERTIFICATE.
 *
 * This function verify the integrity of the certificate chain.
 * root_hash -> Root certificate -> Intermediate certificate -> Leaf certificate.
 *
 * If the peer root certificate hash is deployed,
 * this function also verifies the digest with the root hash in the certificate chain.
 *
 * @param  spdm_context      A pointer to the SPDM context.
 * @param  slot_id           The number of slot for the certificate chain.
 * @param  cert_chain_size   On input, indicate the size in bytes of the destination buffer to store
 *                           the digest buffer.
 *                           On output, indicate the size in bytes of the certificate chain.
 * @param  cert_chain        A pointer to a destination buffer to store the certificate chain.
 * @param  trust_anchor      A buffer to hold the trust_anchor which is used to validate the peer
 *                           certificate, if not NULL.
 * @param  trust_anchor_size A buffer to hold the trust_anchor_size, if not NULL.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_CERTIFICATE was sent and CERTIFICATE was received.
 * @retval LIBSPDM_STATUS_INVALID_STATE_LOCAL
 *         Cannot send GET_CERTIFICATE due to Requester's state.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP
 *         Cannot send GET_CERTIFICATE because the Requester's and/or Responder's CERT_CAP = 0.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the CERTIFICATE response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The CERTIFICATE response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_BUFFER_FULL
 *         The buffer used to store transcripts is exhausted.
 * @retval LIBSPDM_STATUS_VERIF_FAIL
 *         Verification of the certificate chain failed.
 * @retval LIBSPDM_STATUS_INVALID_CERT
 *         The certificate is unable to be parsed or contains invalid field values.
 * @retval LIBSPDM_STATUS_CRYPTO_ERROR
 *         A generic cryptography error occurred.
 **/
static libspdm_return_t libspdm_try_get_certificate(void *context, const uint32_t *session_id,
                                                    uint8_t slot_id,
                                                    uint16_t length,
                                                    size_t *cert_chain_size,
                                                    void *cert_chain,
                                                    const void **trust_anchor,
                                                    size_t *trust_anchor_size)
{
    bool result;
    libspdm_return_t status;
    spdm_get_certificate_request_t *spdm_request;
    size_t spdm_request_size;
    libspdm_certificate_response_max_t *spdm_response;
    size_t spdm_response_size;
    libspdm_large_managed_buffer_t *certificate_chain_buffer;
    libspdm_context_t *spdm_context;
    uint16_t total_responder_cert_chain_buffer_length;
    size_t cert_chain_capacity;
    uint16_t remainder_length;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;
    libspdm_session_info_t *session_info;
    libspdm_session_state_t session_state;
    bool chunk_enabled;

    //
    // NVIDIA_EDIT: To avoid massive stack usage, changed the certificate buffer from
    // allocation on stack to dynamic memory. Changed return patterns where
    // necessary to ensure memory is freed.
    //
    certificate_chain_buffer = (libspdm_large_managed_buffer_t *)
        allocate_zero_pool(sizeof(libspdm_large_managed_buffer_t));
    if (certificate_chain_buffer == NULL) {
        return LIBSPDM_STATUS_BUFFER_FULL;
    }

    /* -=[Check Parameters Phase]=- */
    LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);

    /* -=[Verify State Phase]=- */
    spdm_context = context;

    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true, 0,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP)) {
        status = LIBSPDM_STATUS_UNSUPPORTED_CAP;
        goto done;
    }
    if (spdm_context->connection_info.connection_state < LIBSPDM_CONNECTION_STATE_NEGOTIATED) {
        status = LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        goto done;
    }

    session_info = NULL;
    if (session_id != NULL) {
        session_info = libspdm_get_session_info_via_session_id(
            spdm_context, *session_id);
        if (session_info == NULL) {
            LIBSPDM_ASSERT(false);
            status = LIBSPDM_STATUS_INVALID_STATE_LOCAL;
            goto done;
        }
        session_state = libspdm_secured_message_get_session_state(
            session_info->secured_message_context);
        if (session_state != LIBSPDM_SESSION_STATE_ESTABLISHED) {
            status = LIBSPDM_STATUS_INVALID_STATE_LOCAL;
            goto done;
        }
    }

    libspdm_reset_message_buffer_via_request_code(spdm_context, session_info, SPDM_GET_CERTIFICATE);

    libspdm_init_managed_buffer(certificate_chain_buffer, LIBSPDM_MAX_MESSAGE_BUFFER_SIZE);

    chunk_enabled =
        libspdm_is_capabilities_flag_supported(spdm_context, true,
                                               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHUNK_CAP,
                                               SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP);

    if (chunk_enabled) {
        length = 0xffff;
    } else {
        length = LIBSPDM_MIN(length, LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN);
    }

    remainder_length = 0;
    total_responder_cert_chain_buffer_length = 0;

    transport_header_size = spdm_context->transport_get_header_size(spdm_context);

    do {
        /* -=[Construct Request Phase]=- */
        status = libspdm_acquire_sender_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto done;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size);
        spdm_request = (void *)(message + transport_header_size);
        spdm_request_size = message_size - transport_header_size;

        spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
        spdm_request->header.request_response_code = SPDM_GET_CERTIFICATE;
        spdm_request->header.param1 = slot_id;
        spdm_request->header.param2 = 0;
        spdm_request->offset = (uint16_t)libspdm_get_managed_buffer_size(certificate_chain_buffer);
        if (spdm_request->offset == 0) {
            spdm_request->length = length;
        } else {
            spdm_request->length = LIBSPDM_MIN(length, remainder_length);
        }
        spdm_request_size = sizeof(spdm_get_certificate_request_t);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "request (offset 0x%x, size 0x%x):\n",
                       spdm_request->offset, spdm_request->length));

        /* -=[Send Request Phase]=- */
        status =
            libspdm_send_spdm_request(spdm_context, session_id, spdm_request_size, spdm_request);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            status = LIBSPDM_STATUS_SEND_FAIL;
            goto done;
        }
        libspdm_release_sender_buffer (spdm_context);
        spdm_request = (void *)spdm_context->last_spdm_request;

        /* -=[Receive Response Phase]=- */
        status = libspdm_acquire_receiver_buffer (spdm_context, &message_size, (void **)&message);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto done;
        }
        LIBSPDM_ASSERT (message_size >= transport_header_size);
        spdm_response = (void *)(message);
        spdm_response_size = message_size;

        libspdm_zero_mem(spdm_response, spdm_response_size);
        status = libspdm_receive_spdm_response(spdm_context, session_id,
                                               &spdm_response_size,
                                               (void **)&spdm_response);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_RECEIVE_FAIL;
            goto done;
        }

        /* -=[Validate Response Phase]=- */
        if (spdm_response_size < sizeof(spdm_message_header_t)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto done;
        }
        if (spdm_response->header.spdm_version != spdm_request->header.spdm_version) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }
        if (spdm_response->header.request_response_code == SPDM_ERROR) {
            status = libspdm_handle_error_response_main(
                spdm_context, session_id,
                &spdm_response_size,
                (void **)&spdm_response, SPDM_GET_CERTIFICATE,
                SPDM_CERTIFICATE,
                sizeof(libspdm_certificate_response_max_t));
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                libspdm_release_receiver_buffer (spdm_context);
                goto done;
            }
        } else if (spdm_response->header.request_response_code !=
                   SPDM_CERTIFICATE) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }
        if (spdm_response_size < sizeof(spdm_certificate_response_t)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto done;
        }
        if ((spdm_response->portion_length > spdm_request->length) ||
            (spdm_response->portion_length == 0)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }
        if ((spdm_response->header.param1 & SPDM_CERTIFICATE_RESPONSE_SLOT_ID_MASK) != slot_id) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }
        if (spdm_response_size < sizeof(spdm_certificate_response_t) +
            spdm_response->portion_length) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
            goto done;
        }
        if (spdm_request->offset == 0) {
            total_responder_cert_chain_buffer_length = spdm_response->portion_length +
                                                       spdm_response->remainder_length;
        } else if (spdm_request->offset + spdm_response->portion_length +
                   spdm_response->remainder_length != total_responder_cert_chain_buffer_length) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }
        if (chunk_enabled && (spdm_response->remainder_length != 0)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
            goto done;
        }

        /* -=[Process Response Phase]=- */
        remainder_length = spdm_response->remainder_length;
        spdm_response_size = sizeof(spdm_certificate_response_t) + spdm_response->portion_length;

        if (session_id == NULL) {
            status = libspdm_append_message_b(spdm_context, spdm_request, spdm_request_size);
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                libspdm_release_receiver_buffer (spdm_context);
                status = LIBSPDM_STATUS_BUFFER_FULL;
                goto done;
            }
            status = libspdm_append_message_b(spdm_context, spdm_response, spdm_response_size);
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                libspdm_release_receiver_buffer (spdm_context);
                status = LIBSPDM_STATUS_BUFFER_FULL;
                goto done;
            }
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "Certificate (offset 0x%x, size 0x%x):\n",
                       spdm_request->offset, spdm_response->portion_length));
        LIBSPDM_INTERNAL_DUMP_HEX(spdm_response->cert_chain, spdm_response->portion_length);

        status = libspdm_append_managed_buffer(certificate_chain_buffer,
                                               spdm_response->cert_chain,
                                               spdm_response->portion_length);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_receiver_buffer (spdm_context);
            status = LIBSPDM_STATUS_BUFFER_FULL;
            goto done;
        }
        if (spdm_context->connection_info.connection_state <
            LIBSPDM_CONNECTION_STATE_AFTER_CERTIFICATE) {
            spdm_context->connection_info.connection_state =
                LIBSPDM_CONNECTION_STATE_AFTER_CERTIFICATE;
        }

        libspdm_release_receiver_buffer (spdm_context);
    } while (remainder_length != 0);

    if (spdm_context->local_context.verify_peer_spdm_cert_chain != NULL) {
        result = spdm_context->local_context.verify_peer_spdm_cert_chain (
            spdm_context, slot_id, libspdm_get_managed_buffer_size(certificate_chain_buffer),
            libspdm_get_managed_buffer(certificate_chain_buffer),
            trust_anchor, trust_anchor_size);
        if (!result) {
            status = LIBSPDM_STATUS_VERIF_FAIL;
            goto done;
        }
    } else {
        result = libspdm_verify_peer_cert_chain_buffer(
            spdm_context, libspdm_get_managed_buffer(certificate_chain_buffer),
            libspdm_get_managed_buffer_size(certificate_chain_buffer),
            trust_anchor, trust_anchor_size, true);
        if (!result) {
            status = LIBSPDM_STATUS_VERIF_FAIL;
            goto done;
        }
    }

    spdm_context->connection_info.peer_used_cert_chain_slot_id = slot_id;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_size =
        libspdm_get_managed_buffer_size(certificate_chain_buffer);
    libspdm_copy_mem(spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer,
                     sizeof(spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer),
                     libspdm_get_managed_buffer(certificate_chain_buffer),
                     libspdm_get_managed_buffer_size(certificate_chain_buffer));
#else
    result = libspdm_hash_all(
        spdm_context->connection_info.algorithm.base_hash_algo,
        libspdm_get_managed_buffer(certificate_chain_buffer),
        libspdm_get_managed_buffer_size(certificate_chain_buffer),
        spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash);
    if (!result) {
        status = LIBSPDM_STATUS_CRYPTO_ERROR;
        goto done;
    }

    spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size =
        libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    result = libspdm_get_leaf_cert_public_key_from_cert_chain(
        spdm_context->connection_info.algorithm.base_hash_algo,
        spdm_context->connection_info.algorithm.base_asym_algo,
        libspdm_get_managed_buffer(certificate_chain_buffer),
        libspdm_get_managed_buffer_size(certificate_chain_buffer),
        &spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key);
    if (!result) {
        status = LIBSPDM_STATUS_INVALID_CERT;
        goto done;
    }
#endif

    if (cert_chain_size != NULL) {
        if (*cert_chain_size <
            libspdm_get_managed_buffer_size(certificate_chain_buffer)) {
            *cert_chain_size = libspdm_get_managed_buffer_size(
                certificate_chain_buffer);
            status = LIBSPDM_STATUS_BUFFER_FULL;
            goto done;
        }
        cert_chain_capacity = *cert_chain_size;
        *cert_chain_size = libspdm_get_managed_buffer_size(certificate_chain_buffer);
        if (cert_chain != NULL) {
            libspdm_copy_mem(cert_chain,
                             cert_chain_capacity,
                             libspdm_get_managed_buffer(certificate_chain_buffer),
                             libspdm_get_managed_buffer_size(certificate_chain_buffer));
        }
    }

    status = LIBSPDM_STATUS_SUCCESS;
done:
    free_pool(certificate_chain_buffer);

    return status;
}

libspdm_return_t libspdm_get_certificate(void *context, uint8_t slot_id,
                                         size_t *cert_chain_size,
                                         void *cert_chain)
{
    return libspdm_get_certificate_choose_length(context, slot_id,
                                                 LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN,
                                                 cert_chain_size, cert_chain);
}

libspdm_return_t libspdm_get_certificate_ex(void *context, uint8_t slot_id,
                                            size_t *cert_chain_size,
                                            void *cert_chain,
                                            const void **trust_anchor,
                                            size_t *trust_anchor_size)
{
    return libspdm_get_certificate_choose_length_ex(context, slot_id,
                                                    LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN,
                                                    cert_chain_size, cert_chain,
                                                    trust_anchor, trust_anchor_size);
}

libspdm_return_t libspdm_get_certificate_in_session(void *context, const uint32_t *session_id,
                                                    uint8_t slot_id,
                                                    size_t *cert_chain_size,
                                                    void *cert_chain,
                                                    const void **trust_anchor,
                                                    size_t *trust_anchor_size)
{
    return libspdm_get_certificate_choose_length_in_session(context,
                                                            session_id,
                                                            slot_id,
                                                            LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN,
                                                            cert_chain_size, cert_chain,
                                                            trust_anchor, trust_anchor_size);
}

libspdm_return_t libspdm_get_certificate_choose_length(void *context,
                                                       uint8_t slot_id,
                                                       uint16_t length,
                                                       size_t *cert_chain_size,
                                                       void *cert_chain)
{
    libspdm_context_t *spdm_context;
    size_t retry;
    libspdm_return_t status;

    spdm_context = context;
    spdm_context->crypto_request = true;
    retry = spdm_context->retry_times;
    do {
        status = libspdm_try_get_certificate(spdm_context, NULL, slot_id, length,
                                             cert_chain_size, cert_chain, NULL, NULL);
        if (status != LIBSPDM_STATUS_BUSY_PEER) {
            return status;
        }
    } while (retry-- != 0);

    return status;
}

libspdm_return_t libspdm_get_certificate_choose_length_ex(void *context,
                                                          uint8_t slot_id,
                                                          uint16_t length,
                                                          size_t *cert_chain_size,
                                                          void *cert_chain,
                                                          const void **trust_anchor,
                                                          size_t *trust_anchor_size)
{
    libspdm_context_t *spdm_context;
    size_t retry;
    libspdm_return_t status;

    spdm_context = context;
    spdm_context->crypto_request = true;
    retry = spdm_context->retry_times;
    do {
        status = libspdm_try_get_certificate(spdm_context, NULL, slot_id, length,
                                             cert_chain_size, cert_chain, trust_anchor,
                                             trust_anchor_size);
        if (status != LIBSPDM_STATUS_BUSY_PEER) {
            return status;
        }
    } while (retry-- != 0);

    return status;
}

libspdm_return_t libspdm_get_certificate_choose_length_in_session(void *context,
                                                                  const uint32_t *session_id,
                                                                  uint8_t slot_id,
                                                                  uint16_t length,
                                                                  size_t *cert_chain_size,
                                                                  void *cert_chain,
                                                                  const void **trust_anchor,
                                                                  size_t *trust_anchor_size)
{
    libspdm_context_t *spdm_context;
    size_t retry;
    libspdm_return_t status;

    spdm_context = context;
    spdm_context->crypto_request = true;
    retry = spdm_context->retry_times;
    do {
        status = libspdm_try_get_certificate(spdm_context, session_id, slot_id, length,
                                             cert_chain_size, cert_chain, trust_anchor,
                                             trust_anchor_size);
        if (status != LIBSPDM_STATUS_BUSY_PEER) {
            return status;
        }
    } while (retry-- != 0);

    return status;
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP*/
