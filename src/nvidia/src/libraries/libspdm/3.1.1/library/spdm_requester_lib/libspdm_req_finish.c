/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"
#include "internal/libspdm_secured_message_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP

#pragma pack(1)
typedef struct {
    spdm_message_header_t header;
    uint8_t signature[LIBSPDM_MAX_ASYM_KEY_SIZE];
    uint8_t verify_data[LIBSPDM_MAX_HASH_SIZE];
} libspdm_finish_request_mine_t;

typedef struct {
    spdm_message_header_t header;
    uint8_t verify_data[LIBSPDM_MAX_HASH_SIZE];
} libspdm_finish_response_mine_t;
#pragma pack()

bool libspdm_verify_finish_rsp_hmac(libspdm_context_t *spdm_context,
                                    libspdm_session_info_t *session_info,
                                    const void *hmac_data, size_t hmac_data_size)
{
    size_t hash_size;
    uint8_t calc_hmac_data[LIBSPDM_MAX_HASH_SIZE];
    bool result;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    uint8_t slot_id;
    uint8_t *cert_chain_buffer;
    size_t cert_chain_buffer_size;
    uint8_t *mut_cert_chain_buffer;
    size_t mut_cert_chain_buffer_size;
    uint8_t *th_curr_data;
    size_t th_curr_data_size;
    libspdm_th_managed_buffer_t th_curr;
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
#endif

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
    LIBSPDM_ASSERT(hash_size == hmac_data_size);

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
    if (slot_id == 0xFF) {
        result = libspdm_get_peer_public_key_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    } else {
        result = libspdm_get_peer_cert_chain_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    }
    if (!result) {
        return false;
    }

    if (session_info->mut_auth_requested) {
        slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
        LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
        if (slot_id == 0xFF) {
            result = libspdm_get_local_public_key_buffer(
                spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
        } else {
            result = libspdm_get_local_cert_chain_buffer(
                spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
        }
        if (!result) {
            return false;
        }
    } else {
        mut_cert_chain_buffer = NULL;
        mut_cert_chain_buffer_size = 0;
    }

    result = libspdm_calculate_th_for_finish(
        spdm_context, session_info, cert_chain_buffer,
        cert_chain_buffer_size, mut_cert_chain_buffer,
        mut_cert_chain_buffer_size, &th_curr);
    if (!result) {
        return false;
    }
    th_curr_data = libspdm_get_managed_buffer(&th_curr);
    th_curr_data_size = libspdm_get_managed_buffer_size(&th_curr);

    result = libspdm_hash_all (spdm_context->connection_info.algorithm.base_hash_algo,
                               th_curr_data, th_curr_data_size, hash_data);
    if (!result) {
        return false;
    }

    result = libspdm_hmac_all_with_response_finished_key(
        session_info->secured_message_context, hash_data,
        hash_size, calc_hmac_data);
    if (!result) {
        return false;
    }
#else
    result = libspdm_calculate_th_hmac_for_finish_rsp(
        spdm_context, session_info, &hash_size, calc_hmac_data);
    if (!result) {
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_curr hmac - "));
    LIBSPDM_INTERNAL_DUMP_DATA(calc_hmac_data, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    if (!libspdm_consttime_is_mem_equal(calc_hmac_data, hmac_data, hash_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_finish_rsp_hmac - FAIL !!!\n"));
        return false;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_finish_rsp_hmac - PASS !!!\n"));

    return true;
}

bool libspdm_generate_finish_req_hmac(libspdm_context_t *spdm_context,
                                      libspdm_session_info_t *session_info,
                                      void *hmac)
{
    size_t hash_size;
    uint8_t calc_hmac_data[LIBSPDM_MAX_HASH_SIZE];
    bool result;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    uint8_t slot_id;
    uint8_t *cert_chain_buffer;
    size_t cert_chain_buffer_size;
    uint8_t *mut_cert_chain_buffer;
    size_t mut_cert_chain_buffer_size;
    uint8_t *th_curr_data;
    size_t th_curr_data_size;
    libspdm_th_managed_buffer_t th_curr;
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
#endif

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
    if (slot_id == 0xFF) {
        result = libspdm_get_peer_public_key_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    } else {
        result = libspdm_get_peer_cert_chain_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    }
    if (!result) {
        return false;
    }

    if (session_info->mut_auth_requested) {
        slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
        LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
        if (slot_id == 0xFF) {
            result = libspdm_get_local_public_key_buffer(
                spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
        } else {
            result = libspdm_get_local_cert_chain_buffer(
                spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
        }
        if (!result) {
            return false;
        }
    } else {
        mut_cert_chain_buffer = NULL;
        mut_cert_chain_buffer_size = 0;
    }

    result = libspdm_calculate_th_for_finish(
        spdm_context, session_info, cert_chain_buffer,
        cert_chain_buffer_size, mut_cert_chain_buffer,
        mut_cert_chain_buffer_size, &th_curr);
    if (!result) {
        return false;
    }
    th_curr_data = libspdm_get_managed_buffer(&th_curr);
    th_curr_data_size = libspdm_get_managed_buffer_size(&th_curr);

    result = libspdm_hash_all (spdm_context->connection_info.algorithm.base_hash_algo,
                               th_curr_data, th_curr_data_size, hash_data);
    if (!result) {
        return false;
    }

    result = libspdm_hmac_all_with_request_finished_key(
        session_info->secured_message_context, hash_data,
        hash_size, calc_hmac_data);
    if (!result) {
        return false;
    }
#else
    result = libspdm_calculate_th_hmac_for_finish_req(
        spdm_context, session_info, &hash_size, calc_hmac_data);
    if (!result) {
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_curr hmac - "));
    LIBSPDM_INTERNAL_DUMP_DATA(calc_hmac_data, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    libspdm_copy_mem(hmac, hash_size, calc_hmac_data, hash_size);

    return true;
}

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP

bool libspdm_generate_finish_req_signature(libspdm_context_t *spdm_context,
                                           libspdm_session_info_t *session_info,
                                           uint8_t *signature)
{
    bool result;
    size_t signature_size;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    uint8_t slot_id;
    uint8_t *cert_chain_buffer;
    size_t cert_chain_buffer_size;
    uint8_t *mut_cert_chain_buffer;
    size_t mut_cert_chain_buffer_size;
    uint8_t *th_curr_data;
    size_t th_curr_data_size;
    libspdm_th_managed_buffer_t th_curr;
#endif
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    size_t hash_size;
#endif
#if ((LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) && (LIBSPDM_DEBUG_BLOCK_ENABLE)) || \
    !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT)
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
#endif

    signature_size = libspdm_get_req_asym_signature_size(
        spdm_context->connection_info.algorithm.req_base_asym_alg);

#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
    if (slot_id == 0xFF) {
        result = libspdm_get_peer_public_key_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    } else {
        result = libspdm_get_peer_cert_chain_buffer(
            spdm_context, (const void **)&cert_chain_buffer, &cert_chain_buffer_size);
    }
    if (!result) {
        return false;
    }

    slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
    if (slot_id == 0xFF) {
        result = libspdm_get_local_public_key_buffer(
            spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
    } else {
        result = libspdm_get_local_cert_chain_buffer(
            spdm_context, (const void **)&mut_cert_chain_buffer, &mut_cert_chain_buffer_size);
    }
    if (!result) {
        return false;
    }

    result = libspdm_calculate_th_for_finish(
        spdm_context, session_info, cert_chain_buffer,
        cert_chain_buffer_size, mut_cert_chain_buffer,
        mut_cert_chain_buffer_size, &th_curr);
    if (!result) {
        return false;
    }
    th_curr_data = libspdm_get_managed_buffer(&th_curr);
    th_curr_data_size = libspdm_get_managed_buffer_size(&th_curr);

    /* Debug code only - required for debug print of th_curr below*/
    LIBSPDM_DEBUG_CODE(
        if (!libspdm_hash_all(
                spdm_context->connection_info.algorithm.base_hash_algo,
                th_curr_data, th_curr_data_size, hash_data)) {
        return false;
    }
        );
#else
    result = libspdm_calculate_th_hash_for_finish(
        spdm_context, session_info, &hash_size, hash_data);
    if (!result) {
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_curr hash - "));
    LIBSPDM_INTERNAL_DUMP_DATA(hash_data, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_requester_data_sign(
        spdm_context->connection_info.version, SPDM_FINISH,
        spdm_context->connection_info.algorithm.req_base_asym_alg,
        spdm_context->connection_info.algorithm.base_hash_algo,
        false, th_curr_data, th_curr_data_size, signature, &signature_size);
#else
    result = libspdm_requester_data_sign(
        spdm_context->connection_info.version, SPDM_FINISH,
        spdm_context->connection_info.algorithm.req_base_asym_alg,
        spdm_context->connection_info.algorithm.base_hash_algo,
        true, hash_data, hash_size, signature, &signature_size);
#endif
    if (result) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "signature - "));
        LIBSPDM_INTERNAL_DUMP_DATA(signature, signature_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    return result;
}
#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

/**
 * This function sends FINISH and receives FINISH_RSP for SPDM finish.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  session_id         session_id to the FINISH request.
 * @param  req_slot_id_param  req_slot_id_param to the FINISH request.
 *
 * @retval RETURN_SUCCESS               The FINISH is sent and the FINISH_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
static libspdm_return_t libspdm_try_send_receive_finish(libspdm_context_t *spdm_context,
                                                        uint32_t session_id,
                                                        uint8_t req_slot_id_param)
{
    libspdm_return_t status;
    libspdm_finish_request_mine_t *spdm_request;
    size_t spdm_request_size;
    size_t signature_size;
    size_t hmac_size;
    libspdm_finish_response_mine_t *spdm_response;
    size_t spdm_response_size;
    libspdm_session_info_t *session_info;
    uint8_t *ptr;
    bool result;
    uint8_t th2_hash_data[LIBSPDM_MAX_HASH_SIZE];
    libspdm_session_state_t session_state;
    uint8_t *message;
    size_t message_size;
    size_t transport_header_size;

    /* -=[Check Parameters Phase]=- */
    if (libspdm_get_connection_version(spdm_context) < SPDM_MESSAGE_VERSION_11) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    session_info = libspdm_get_session_info_via_session_id(spdm_context, session_id);
    if (session_info == NULL) {
        status = LIBSPDM_STATUS_INVALID_PARAMETER;
        goto error;
    }

    /* -=[Verify State Phase]=- */
    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP)) {
        status = LIBSPDM_STATUS_UNSUPPORTED_CAP;
        goto error;
    }

    if (spdm_context->connection_info.connection_state < LIBSPDM_CONNECTION_STATE_NEGOTIATED) {
        status = LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        goto error;
    }

    session_state = libspdm_secured_message_get_session_state(
        session_info->secured_message_context);
    if (session_state != LIBSPDM_SESSION_STATE_HANDSHAKING) {
        status = LIBSPDM_STATUS_INVALID_STATE_LOCAL;
        goto error;
    }
    if (session_info->mut_auth_requested != 0) {
        if ((req_slot_id_param >= SPDM_MAX_SLOT_COUNT) && (req_slot_id_param != 0xFF)) {
            status = LIBSPDM_STATUS_INVALID_PARAMETER;
            goto error;
        }
    } else {
        if (req_slot_id_param != 0) {
            status = LIBSPDM_STATUS_INVALID_PARAMETER;
            goto error;
        }
    }

    /* -=[Construct Request Phase]=- */
    transport_header_size = spdm_context->local_context.capability.transport_header_size;
    status = libspdm_acquire_sender_buffer (spdm_context, &message_size, (void **)&message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto error;
    }
    LIBSPDM_ASSERT (message_size >= transport_header_size +
                    spdm_context->local_context.capability.transport_tail_size);
    spdm_request = (void *)(message + transport_header_size);
    spdm_request_size = message_size - transport_header_size -
                        spdm_context->local_context.capability.transport_tail_size;

    spdm_request->header.spdm_version = libspdm_get_connection_version (spdm_context);
    spdm_request->header.request_response_code = SPDM_FINISH;
    spdm_request->header.param1 = 0;
    spdm_request->header.param2 = 0;
    signature_size = 0;
#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    if (session_info->mut_auth_requested) {
        spdm_request->header.param1 = SPDM_FINISH_REQUEST_ATTRIBUTES_SIGNATURE_INCLUDED;
        spdm_request->header.param2 = req_slot_id_param;
        signature_size = libspdm_get_req_asym_signature_size(
            spdm_context->connection_info.algorithm.req_base_asym_alg);
    }
#endif

    spdm_context->connection_info.local_used_cert_chain_slot_id = req_slot_id_param;
    if (session_info->mut_auth_requested && (req_slot_id_param != 0xFF)) {
        LIBSPDM_ASSERT(req_slot_id_param < SPDM_MAX_SLOT_COUNT);
        spdm_context->connection_info.local_used_cert_chain_buffer =
            spdm_context->local_context.local_cert_chain_provision[req_slot_id_param];
        spdm_context->connection_info.local_used_cert_chain_buffer_size =
            spdm_context->local_context.local_cert_chain_provision_size[req_slot_id_param];
    }

    hmac_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
    spdm_request_size = sizeof(spdm_finish_request_t) + signature_size + hmac_size;
    ptr = spdm_request->signature;

    status = libspdm_append_message_f(spdm_context, session_info, true, (uint8_t *)spdm_request,
                                      sizeof(spdm_finish_request_t));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        goto error;
    }
#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    if (session_info->mut_auth_requested) {
        result = libspdm_generate_finish_req_signature(spdm_context, session_info, ptr);
        if (!result) {
            libspdm_release_sender_buffer (spdm_context);
            status = LIBSPDM_STATUS_CRYPTO_ERROR;
            goto error;
        }
        status = libspdm_append_message_f(spdm_context, session_info, true, ptr, signature_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            libspdm_release_sender_buffer (spdm_context);
            goto error;
        }
        ptr += signature_size;
    }
#endif

    result = libspdm_generate_finish_req_hmac(spdm_context, session_info, ptr);
    if (!result) {
        libspdm_release_sender_buffer (spdm_context);
        status = LIBSPDM_STATUS_CRYPTO_ERROR;
        goto error;
    }

    status = libspdm_append_message_f(spdm_context, session_info, true, ptr, hmac_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        goto error;
    }

    /* -=[Send Request Phase]=- */
    status = libspdm_send_spdm_request(spdm_context, &session_id, spdm_request_size, spdm_request);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        libspdm_release_sender_buffer (spdm_context);
        goto error;
    }

    libspdm_reset_message_buffer_via_request_code(spdm_context, session_info, SPDM_FINISH);

    libspdm_release_sender_buffer (spdm_context);
    spdm_request = (void *)spdm_context->last_spdm_request;

    /* -=[Receive Response Phase]=- */
    status = libspdm_acquire_receiver_buffer (spdm_context, &message_size, (void **)&message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto error;
    }
    LIBSPDM_ASSERT (message_size >= transport_header_size);
    spdm_response = (void *)(message);
    spdm_response_size = message_size;

    libspdm_zero_mem(spdm_response, spdm_response_size);
    status = libspdm_receive_spdm_response(
        spdm_context, &session_id, &spdm_response_size, (void **)&spdm_response);
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
        if (spdm_response->header.param1 == SPDM_ERROR_CODE_DECRYPT_ERROR) {
            status = LIBSPDM_STATUS_SESSION_MSG_ERROR;
            goto receive_done;
        }
        if (spdm_response->header.param1 != SPDM_ERROR_CODE_RESPONSE_NOT_READY) {
            libspdm_reset_message_f (spdm_context, session_info);
        }
        status = libspdm_handle_error_response_main(
            spdm_context, &session_id,
            &spdm_response_size, (void **)&spdm_response,
            SPDM_FINISH, SPDM_FINISH_RSP);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    } else if (spdm_response->header.request_response_code != SPDM_FINISH_RSP) {
        status = LIBSPDM_STATUS_INVALID_MSG_FIELD;
        goto receive_done;
    }

    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP)) {
        hmac_size = 0;
    }

    if (spdm_response_size < sizeof(spdm_finish_response_t) + hmac_size) {
        status = LIBSPDM_STATUS_INVALID_MSG_SIZE;
        goto receive_done;
    }

    status = libspdm_append_message_f(spdm_context, session_info, true, spdm_response,
                                      sizeof(spdm_finish_response_t));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        goto receive_done;
    }

    if (libspdm_is_capabilities_flag_supported(
            spdm_context, true,
            SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "verify_data (0x%x):\n", hmac_size));
        LIBSPDM_INTERNAL_DUMP_HEX(spdm_response->verify_data, hmac_size);
        result = libspdm_verify_finish_rsp_hmac(spdm_context, session_info,
                                                spdm_response->verify_data,
                                                hmac_size);
        if (!result) {
            status = LIBSPDM_STATUS_VERIF_FAIL;
            goto receive_done;
        }

        status = libspdm_append_message_f(
            spdm_context, session_info, true,
            (uint8_t *)spdm_response +
            sizeof(spdm_finish_response_t),
            hmac_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            goto receive_done;
        }
    }

    /* -=[Process Response Phase]=- */
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "libspdm_generate_session_data_key[%x]\n", session_id));
    result = libspdm_calculate_th2_hash(spdm_context, session_info, true, th2_hash_data);
    if (!result) {
        status = LIBSPDM_STATUS_CRYPTO_ERROR;
        goto receive_done;
    }
    result = libspdm_generate_session_data_key(
        session_info->secured_message_context, th2_hash_data);
    if (!result) {
        status = LIBSPDM_STATUS_CRYPTO_ERROR;
        goto receive_done;
    }

    /* -=[Update State Phase]=- */
    libspdm_secured_message_set_session_state(
        session_info->secured_message_context, LIBSPDM_SESSION_STATE_ESTABLISHED);

    /* -=[Log Message Phase]=- */
    #if LIBSPDM_ENABLE_MSG_LOG
    libspdm_append_msg_log(spdm_context, spdm_response, spdm_response_size);
    #endif /* LIBSPDM_ENABLE_MSG_LOG */

    libspdm_release_receiver_buffer (spdm_context);

    return LIBSPDM_STATUS_SUCCESS;

receive_done:
    libspdm_release_receiver_buffer (spdm_context);
error:
    if (status != LIBSPDM_STATUS_BUSY_PEER) {
        libspdm_free_session_id(spdm_context, session_id);
    }

    return status;
}

libspdm_return_t libspdm_send_receive_finish(libspdm_context_t *spdm_context,
                                             uint32_t session_id,
                                             uint8_t req_slot_id_param)
{
    size_t retry;
    uint64_t retry_delay_time;
    libspdm_return_t status;

    spdm_context->crypto_request = true;
    retry = spdm_context->retry_times;
    retry_delay_time = spdm_context->retry_delay_time;
    do {
        status = libspdm_try_send_receive_finish(spdm_context, session_id,
                                                 req_slot_id_param);
        if ((status != LIBSPDM_STATUS_BUSY_PEER) || (retry == 0)) {
            return status;
        }

        libspdm_sleep(retry_delay_time);
    } while (retry-- != 0);

    return status;
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP*/
