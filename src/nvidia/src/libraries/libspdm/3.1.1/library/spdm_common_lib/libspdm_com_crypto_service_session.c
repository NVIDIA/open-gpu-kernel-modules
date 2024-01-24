/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_secured_message_lib.h"

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates current TH data with message A and message K.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  cert_chain_buffer                Certificate chain buffer with spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size            size in bytes of the certificate chain buffer.
 * @param  th_data_buffer_size             size in bytes of the th_data_buffer
 * @param  th_data_buffer                 The buffer to store the th_data_buffer
 *
 * @retval RETURN_SUCCESS  current TH data is calculated.
 */
bool libspdm_calculate_th_for_exchange(
    libspdm_context_t *spdm_context, void *spdm_session_info, const uint8_t *cert_chain_buffer,
    size_t cert_chain_buffer_size,
    libspdm_th_managed_buffer_t *th_curr)
{
    libspdm_session_info_t *session_info;
    uint8_t cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    uint32_t hash_size;
    libspdm_return_t status;
    bool result;

    session_info = spdm_session_info;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    libspdm_init_managed_buffer(th_curr, sizeof(th_curr->buffer));

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_a data :\n"));
    LIBSPDM_INTERNAL_DUMP_HEX(
        libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
        libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
    status = libspdm_append_managed_buffer(
        th_curr,
        libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
        libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    if (cert_chain_buffer != NULL) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_message_ct data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(cert_chain_buffer, cert_chain_buffer_size);
        result = libspdm_hash_all(
            spdm_context->connection_info.algorithm.base_hash_algo,
            cert_chain_buffer, cert_chain_buffer_size,
            cert_chain_buffer_hash);
        if (!result) {
            return false;
        }
        status = libspdm_append_managed_buffer(th_curr, cert_chain_buffer_hash, hash_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_k data :\n"));
    LIBSPDM_INTERNAL_DUMP_HEX(
        libspdm_get_managed_buffer(&session_info->session_transcript.message_k),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_k));
    status = libspdm_append_managed_buffer(
        th_curr,
        libspdm_get_managed_buffer(&session_info->session_transcript.message_k),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_k));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    return true;
}
#else
/*
 * This function calculates current TH hash with message A and message K.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  th_hash_buffer_size             size in bytes of the th_hash_buffer
 * @param  th_hash_buffer                 The buffer to store the th_hash_buffer
 *
 * @retval RETURN_SUCCESS  current TH hash is calculated.
 */
bool libspdm_calculate_th_hash_for_exchange(
    libspdm_context_t *spdm_context, void *spdm_session_info,
    size_t *th_hash_buffer_size, void *th_hash_buffer)
{
    libspdm_session_info_t *session_info;
    uint32_t hash_size;
    void *digest_context_th;
    bool result;

    session_info = spdm_session_info;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    LIBSPDM_ASSERT(*th_hash_buffer_size >= hash_size);

    /* duplicate the th context, because we still need use original context to continue.*/
    digest_context_th = libspdm_hash_new (spdm_context->connection_info.algorithm.base_hash_algo);
    if (digest_context_th == NULL) {
        return false;
    }
    result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                     session_info->session_transcript.digest_context_th,
                                     digest_context_th);
    if (!result) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           digest_context_th);
        return false;
    }
    result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                 digest_context_th, th_hash_buffer);
    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, digest_context_th);
    if (!result) {
        return false;
    }

    *th_hash_buffer_size = hash_size;

    return true;
}

/*
 * This function calculates current TH hmac with message A and message K, with response finished_key.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  th_hmac_buffer_size             size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer                 The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_exchange_rsp(
    libspdm_context_t *spdm_context, void *spdm_session_info, bool is_requester,
    size_t *th_hmac_buffer_size, void *th_hmac_buffer)
{
    libspdm_session_info_t *session_info;
    void *secured_message_context;
    uint32_t hash_size;
    void *hash_context_th;
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
    bool result;

    session_info = spdm_session_info;
    secured_message_context = session_info->secured_message_context;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    LIBSPDM_ASSERT(*th_hmac_buffer_size >= hash_size);

    /* duplicate the th context, because we still need use original context to continue.*/
    hash_context_th = libspdm_hash_new (spdm_context->connection_info.algorithm.base_hash_algo);
    if (hash_context_th == NULL) {
        return false;
    }
    result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                     session_info->session_transcript.digest_context_th,
                                     hash_context_th);
    if (!result) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
        return false;
    }
    result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                 hash_context_th, hash_data);
    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
    if (!result) {
        return false;
    }

    result = libspdm_hmac_all_with_response_finished_key (secured_message_context,
                                                          hash_data, hash_size, th_hmac_buffer);
    if (!result) {
        return false;
    }
    *th_hmac_buffer_size = hash_size;

    return true;
}
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates current TH data with message A, message K and message F.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  cert_chain_buffer                Certificate chain buffer with spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size            size in bytes of the certificate chain buffer.
 * @param  mut_cert_chain_buffer             Certificate chain buffer with spdm_cert_chain_t header in mutual authentication.
 * @param  mut_cert_chain_buffer_size         size in bytes of the certificate chain buffer in mutual authentication.
 * @param  th_data_buffer_size             size in bytes of the th_data_buffer
 * @param  th_data_buffer                 The buffer to store the th_data_buffer
 *
 * @retval RETURN_SUCCESS  current TH data is calculated.
 */
bool libspdm_calculate_th_for_finish(libspdm_context_t *spdm_context,
                                     void *spdm_session_info,
                                     const uint8_t *cert_chain_buffer,
                                     size_t cert_chain_buffer_size,
                                     const uint8_t *mut_cert_chain_buffer,
                                     size_t mut_cert_chain_buffer_size,
                                     libspdm_th_managed_buffer_t *th_curr)
{
    libspdm_session_info_t *session_info;
    uint8_t cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    uint8_t mut_cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    uint32_t hash_size;
    libspdm_return_t status;
    bool result;

    session_info = spdm_session_info;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    libspdm_init_managed_buffer(th_curr, sizeof(th_curr->buffer));

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_a data :\n"));
    LIBSPDM_INTERNAL_DUMP_HEX(
        libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
        libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
    status = libspdm_append_managed_buffer(
        th_curr,
        libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
        libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    if (cert_chain_buffer != NULL) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_message_ct data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(cert_chain_buffer, cert_chain_buffer_size);
        result = libspdm_hash_all(
            spdm_context->connection_info.algorithm.base_hash_algo,
            cert_chain_buffer, cert_chain_buffer_size,
            cert_chain_buffer_hash);
        if (!result) {
            return false;
        }
        status = libspdm_append_managed_buffer(th_curr, cert_chain_buffer_hash, hash_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_k data :\n"));
    LIBSPDM_INTERNAL_DUMP_HEX(
        libspdm_get_managed_buffer(&session_info->session_transcript.message_k),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_k));
    status = libspdm_append_managed_buffer(
        th_curr,
        libspdm_get_managed_buffer(&session_info->session_transcript.message_k),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_k));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    if (mut_cert_chain_buffer != NULL) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th_message_cm data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(mut_cert_chain_buffer, mut_cert_chain_buffer_size);
        result = libspdm_hash_all(
            spdm_context->connection_info.algorithm.base_hash_algo,
            mut_cert_chain_buffer, mut_cert_chain_buffer_size,
            mut_cert_chain_buffer_hash);
        if (!result) {
            return false;
        }
        status = libspdm_append_managed_buffer(th_curr, mut_cert_chain_buffer_hash, hash_size);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_f data :\n"));
    LIBSPDM_INTERNAL_DUMP_HEX(
        libspdm_get_managed_buffer(&session_info->session_transcript.message_f),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_f));
    status = libspdm_append_managed_buffer(
        th_curr,
        libspdm_get_managed_buffer(&session_info->session_transcript.message_f),
        libspdm_get_managed_buffer_size(&session_info->session_transcript.message_f));
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    return true;
}
#else
/*
 * This function calculates current TH hash with message A, message K and message F.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  th_hash_buffer_size             size in bytes of the th_hash_buffer
 * @param  th_hash_buffer                 The buffer to store the th_hash_buffer
 *
 * @retval RETURN_SUCCESS  current TH hash is calculated.
 */
bool libspdm_calculate_th_hash_for_finish(libspdm_context_t *spdm_context,
                                          void *spdm_session_info,
                                          size_t *th_hash_buffer_size,
                                          void *th_hash_buffer)
{
    libspdm_session_info_t *session_info;
    uint32_t hash_size;
    void *digest_context_th;
    bool result;

    session_info = spdm_session_info;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    LIBSPDM_ASSERT(*th_hash_buffer_size >= hash_size);

    /* duplicate the th context, because we still need use original context to continue.*/
    digest_context_th = libspdm_hash_new (spdm_context->connection_info.algorithm.base_hash_algo);
    if (digest_context_th == NULL) {
        return false;
    }
    result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                     session_info->session_transcript.digest_context_th,
                                     digest_context_th);
    if (!result) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo,
                           digest_context_th);
        return false;
    }
    result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                 digest_context_th, th_hash_buffer);
    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, digest_context_th);
    if (!result) {
        return false;
    }

    *th_hash_buffer_size = hash_size;

    return true;
}

/*
 * This function calculates current TH hmac with message A, message K and message F, with response finished_key.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  th_hmac_buffer_size             size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer                 The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_finish_rsp(libspdm_context_t *spdm_context,
                                              void *spdm_session_info,
                                              size_t *th_hmac_buffer_size,
                                              void *th_hmac_buffer)
{
    libspdm_session_info_t *session_info;
    void *secured_message_context;
    uint32_t hash_size;
    void *hash_context_th;
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
    bool result;

    session_info = spdm_session_info;
    secured_message_context = session_info->secured_message_context;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    LIBSPDM_ASSERT(*th_hmac_buffer_size >= hash_size);
    LIBSPDM_ASSERT(session_info->session_transcript.digest_context_th != NULL);

    /* duplicate the th context, because we still need use original context to continue.*/
    hash_context_th = libspdm_hash_new (spdm_context->connection_info.algorithm.base_hash_algo);
    if (hash_context_th == NULL) {
        return false;
    }
    result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                     session_info->session_transcript.digest_context_th,
                                     hash_context_th);
    if (!result) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
        return false;
    }
    result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                 hash_context_th, hash_data);
    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
    if (!result) {
        return false;
    }

    result = libspdm_hmac_all_with_response_finished_key (secured_message_context,
                                                          hash_data, hash_size, th_hmac_buffer);
    if (!result) {
        return false;
    }
    *th_hmac_buffer_size = hash_size;

    return true;
}

/*
 * This function calculates current TH hmac with message A, message K and message F, with request finished_key.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  th_hmac_buffer_size             size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer                 The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_finish_req(libspdm_context_t *spdm_context,
                                              void *spdm_session_info,
                                              size_t *th_hmac_buffer_size,
                                              void *th_hmac_buffer)
{
    libspdm_session_info_t *session_info;
    void *secured_message_context;
    uint32_t hash_size;
    void *hash_context_th;
    uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
    bool result;

    session_info = spdm_session_info;
    secured_message_context = session_info->secured_message_context;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    LIBSPDM_ASSERT(*th_hmac_buffer_size >= hash_size);
    LIBSPDM_ASSERT(session_info->session_transcript.digest_context_th != NULL);

    /* duplicate the th context, because we still need use original context to continue.*/
    hash_context_th = libspdm_hash_new (spdm_context->connection_info.algorithm.base_hash_algo);
    if (hash_context_th == NULL) {
        return false;
    }
    result = libspdm_hash_duplicate (spdm_context->connection_info.algorithm.base_hash_algo,
                                     session_info->session_transcript.digest_context_th,
                                     hash_context_th);
    if (!result) {
        libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
        return false;
    }
    result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                 hash_context_th, hash_data);
    libspdm_hash_free (spdm_context->connection_info.algorithm.base_hash_algo, hash_context_th);
    if (!result) {
        return false;
    }

    result = libspdm_hmac_all_with_request_finished_key (secured_message_context,
                                                         hash_data, hash_size, th_hmac_buffer);
    if (!result) {
        return false;
    }
    *th_hmac_buffer_size = hash_size;

    return true;
}
#endif

/*
 * This function calculates th1 hash.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  is_requester                  Indicate of the key generation for a requester or a responder.
 * @param  th1_hash_data                  th1 hash
 *
 * @retval RETURN_SUCCESS  th1 hash is calculated.
 */
bool libspdm_calculate_th1_hash(libspdm_context_t *spdm_context,
                                void *spdm_session_info,
                                bool is_requester,
                                uint8_t *th1_hash_data)
{
    libspdm_session_info_t *session_info;
    bool result;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    uint8_t slot_id;
    uint8_t *cert_chain_buffer;
    size_t cert_chain_buffer_size;
    uint8_t *th_curr_data;
    size_t th_curr_data_size;
    libspdm_th_managed_buffer_t th_curr;
#endif
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    size_t hash_size;
#endif

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "Calc th1 hash ...\n"));

    session_info = spdm_session_info;

#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
#endif
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    if (!session_info->use_psk) {
        if (is_requester) {
            slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
            LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
            if (slot_id == 0xFF) {
                result = libspdm_get_peer_public_key_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            } else {
                result = libspdm_get_peer_cert_chain_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            }
        } else {
            slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
            LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
            if (slot_id == 0xFF) {
                result = libspdm_get_local_public_key_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            } else {
                result = libspdm_get_local_cert_chain_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            }
        }
        if (!result) {
            return false;
        }
    } else {
        cert_chain_buffer = NULL;
        cert_chain_buffer_size = 0;
    }

    result = libspdm_calculate_th_for_exchange(
        spdm_context, session_info, cert_chain_buffer,
        cert_chain_buffer_size, &th_curr);
    if (!result) {
        return false;
    }
    th_curr_data = libspdm_get_managed_buffer(&th_curr);
    th_curr_data_size = libspdm_get_managed_buffer_size(&th_curr);

    result = libspdm_hash_all(spdm_context->connection_info.algorithm.base_hash_algo,
                              th_curr_data, th_curr_data_size, th1_hash_data);
    if (!result) {
        return false;
    }
#else
    result = libspdm_calculate_th_hash_for_exchange(
        spdm_context, session_info, &hash_size, th1_hash_data);
    if (!result) {
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th1 hash - "));
    LIBSPDM_INTERNAL_DUMP_DATA(th1_hash_data, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    return true;
}

/*
 * This function calculates th2 hash.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The SPDM session ID.
 * @param  is_requester                  Indicate of the key generation for a requester or a responder.
 * @param  th1_hash_data                  th2 hash
 *
 * @retval RETURN_SUCCESS  th2 hash is calculated.
 */
bool libspdm_calculate_th2_hash(libspdm_context_t *spdm_context,
                                void *spdm_session_info,
                                bool is_requester,
                                uint8_t *th2_hash_data)
{
    libspdm_session_info_t *session_info;
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
#endif
#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    size_t hash_size;
#endif

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "Calc th2 hash ...\n"));

    session_info = spdm_session_info;

#if !(LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT) || (LIBSPDM_DEBUG_PRINT_ENABLE)
    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    if (!session_info->use_psk) {
        if (is_requester) {
            slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
            LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
            if (slot_id == 0xFF) {
                result = libspdm_get_peer_public_key_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            } else {
                result = libspdm_get_peer_cert_chain_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            }
        } else {
            slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
            LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
            if (slot_id == 0xFF) {
                result = libspdm_get_local_public_key_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            } else {
                result = libspdm_get_local_cert_chain_buffer(
                    spdm_context, (const void **)&cert_chain_buffer,
                    &cert_chain_buffer_size);
            }
        }
        if (!result) {
            return false;
        }
        if (session_info->mut_auth_requested) {
            if (is_requester) {
                slot_id = spdm_context->connection_info.local_used_cert_chain_slot_id;
                LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                if (slot_id == 0xFF) {
                    result = libspdm_get_local_public_key_buffer(
                        spdm_context, (const void **)&mut_cert_chain_buffer,
                        &mut_cert_chain_buffer_size);
                } else {
                    result = libspdm_get_local_cert_chain_buffer(
                        spdm_context, (const void **)&mut_cert_chain_buffer,
                        &mut_cert_chain_buffer_size);
                }
            } else {
                slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
                LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));
                if (slot_id == 0xFF) {
                    result = libspdm_get_peer_public_key_buffer(
                        spdm_context, (const void **)&mut_cert_chain_buffer,
                        &mut_cert_chain_buffer_size);
                } else {
                    result = libspdm_get_peer_cert_chain_buffer(
                        spdm_context, (const void **)&mut_cert_chain_buffer,
                        &mut_cert_chain_buffer_size);
                }
            }
            if (!result) {
                return false;
            }
        } else {
            mut_cert_chain_buffer = NULL;
            mut_cert_chain_buffer_size = 0;
        }
    } else {
        cert_chain_buffer = NULL;
        cert_chain_buffer_size = 0;
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

    result = libspdm_hash_all(spdm_context->connection_info.algorithm.base_hash_algo,
                              th_curr_data, th_curr_data_size, th2_hash_data);
    if (!result) {
        return false;
    }
#else
    result = libspdm_calculate_th_hash_for_finish(
        spdm_context, session_info, &hash_size, th2_hash_data);
    if (!result) {
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "th2 hash - "));
    LIBSPDM_INTERNAL_DUMP_DATA(th2_hash_data, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    return true;
}
