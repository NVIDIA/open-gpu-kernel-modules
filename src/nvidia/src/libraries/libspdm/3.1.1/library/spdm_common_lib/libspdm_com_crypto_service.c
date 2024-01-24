/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_common_lib.h"

/**
 * This function returns peer certificate chain buffer including spdm_cert_chain_t header.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_buffer              Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size          size in bytes of the certificate chain buffer.
 *
 * @retval true  Peer certificate chain buffer including spdm_cert_chain_t header is returned.
 * @retval false Peer certificate chain buffer including spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_peer_cert_chain_buffer(void *spdm_context,
                                        const void **cert_chain_buffer,
                                        size_t *cert_chain_buffer_size)
{
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_context_t *context;
    uint8_t slot_id;

    context = spdm_context;
    slot_id = context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);
    if (context->connection_info.peer_used_cert_chain[slot_id].buffer_size != 0) {
        *cert_chain_buffer = context->connection_info.peer_used_cert_chain[slot_id].buffer;
        *cert_chain_buffer_size = context->connection_info
                                  .peer_used_cert_chain[slot_id].buffer_size;
        return true;
    }
#endif
    return false;
}

/**
 * This function returns peer certificate chain data without spdm_cert_chain_t header.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_data                Certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size            size in bytes of the certificate chain data.
 *
 * @retval true  Peer certificate chain data without spdm_cert_chain_t header is returned.
 * @retval false Peer certificate chain data without spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_peer_cert_chain_data(void *spdm_context,
                                      const void **cert_chain_data,
                                      size_t *cert_chain_data_size)
{
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_context_t *context;
    size_t hash_size;
    bool result;

    context = spdm_context;
    hash_size = libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo);

    result = libspdm_get_peer_cert_chain_buffer(context, cert_chain_data,
                                                cert_chain_data_size);
    if (result) {
        *cert_chain_data =
            (const uint8_t *)*cert_chain_data + sizeof(spdm_cert_chain_t) + hash_size;
        *cert_chain_data_size =
            *cert_chain_data_size - (sizeof(spdm_cert_chain_t) + hash_size);
        return true;
    }
#endif
    return false;
}

/**
 * This function returns local used certificate chain buffer including spdm_cert_chain_t header.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_buffer              Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size          size in bytes of the certificate chain buffer.
 *
 * @retval true  Local used certificate chain buffer including spdm_cert_chain_t header is returned.
 * @retval false Local used certificate chain buffer including spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_local_cert_chain_buffer(void *spdm_context,
                                         const void **cert_chain_buffer,
                                         size_t *cert_chain_buffer_size)
{
    libspdm_context_t *context;

    context = spdm_context;
    if (context->connection_info.local_used_cert_chain_buffer_size != 0) {
        *cert_chain_buffer = context->connection_info.local_used_cert_chain_buffer;
        *cert_chain_buffer_size = context->connection_info.local_used_cert_chain_buffer_size;
        return true;
    }
    return false;
}

/**
 * This function returns local used certificate chain data without spdm_cert_chain_t header.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_data                Certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size            size in bytes of the certificate chain data.
 *
 * @retval true  Local used certificate chain data without spdm_cert_chain_t header is returned.
 * @retval false Local used certificate chain data without spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_local_cert_chain_data(void *spdm_context,
                                       const void **cert_chain_data,
                                       size_t *cert_chain_data_size)
{
    libspdm_context_t *context;
    bool result;
    size_t hash_size;

    context = spdm_context;

    result = libspdm_get_local_cert_chain_buffer(context, cert_chain_data,
                                                 cert_chain_data_size);
    if (!result) {
        return false;
    }

    hash_size = libspdm_get_hash_size(context->connection_info.algorithm.base_hash_algo);

    *cert_chain_data = (const uint8_t *)*cert_chain_data + sizeof(spdm_cert_chain_t) + hash_size;
    *cert_chain_data_size = *cert_chain_data_size - (sizeof(spdm_cert_chain_t) + hash_size);
    return true;
}

/**
 * This function returns peer public key buffer.
 *
 * @param  spdm_context                 A pointer to the SPDM context.
 * @param  peer_public_key_buffer       Peer public key buffer.
 * @param  peer_public_key_buffer_size  Size in bytes of peer public key buffer.
 *
 * @retval true  Peer public key buffer is returned.
 * @retval false Peer public key buffer is not found.
 **/
bool libspdm_get_peer_public_key_buffer(void *spdm_context,
                                        const void **peer_public_key_buffer,
                                        size_t *peer_public_key_buffer_size)
{
    libspdm_context_t *context;

    context = spdm_context;
    if (context->local_context.peer_public_key_provision_size != 0) {
        *peer_public_key_buffer = context->local_context.peer_public_key_provision;
        *peer_public_key_buffer_size = context->local_context.peer_public_key_provision_size;
        return true;
    }
    return false;
}

/**
 * This function returns local public key buffer.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  local_public_key_buffer       Local public key buffer.
 * @param  local_public_key_buffer_size  Size in bytes of local public key buffer.
 *
 * @retval true  Local public key buffer is returned.
 * @retval false Local public key buffer is not found.
 **/
bool libspdm_get_local_public_key_buffer(void *spdm_context,
                                         const void **local_public_key_buffer,
                                         size_t *local_public_key_buffer_size)
{
    libspdm_context_t *context;

    context = spdm_context;
    if (context->local_context.peer_public_key_provision_size != 0) {
        *local_public_key_buffer = context->local_context.local_public_key_provision;
        *local_public_key_buffer_size = context->local_context.local_public_key_provision_size;
        return true;
    }
    return false;
}

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates l1l2.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  A pointer to the SPDM session context.
 * @param  l1l2                          The buffer to store the l1l2.
 *
 * @retval RETURN_SUCCESS  l1l2 is calculated.
 */
bool libspdm_calculate_l1l2(libspdm_context_t *spdm_context,
                            void *session_info,
                            libspdm_l1l2_managed_buffer_t *l1l2)
{
    libspdm_return_t status;
    libspdm_session_info_t *spdm_session_info;

    spdm_session_info = session_info;

    libspdm_init_managed_buffer(l1l2, sizeof(l1l2->buffer));

    if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
        SPDM_MESSAGE_VERSION_11) {

        /* Need append VCA since 1.2 script*/

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_a data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
        status = libspdm_append_managed_buffer(
            l1l2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }
    }

    if (spdm_session_info == NULL) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_m data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_m),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_m));
        status = libspdm_append_managed_buffer(
            l1l2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_m),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_m));
    } else {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "use message_m in session :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_session_info->session_transcript.message_m),
            libspdm_get_managed_buffer_size(&spdm_session_info->session_transcript.message_m));
        status = libspdm_append_managed_buffer(
            l1l2,
            libspdm_get_managed_buffer(&spdm_session_info->session_transcript.message_m),
            libspdm_get_managed_buffer_size(&spdm_session_info->session_transcript.message_m));
    }
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    /* Debug code only - calculate and print value of l1l2 hash*/
    LIBSPDM_DEBUG_CODE(
        uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
        uint32_t hash_size = libspdm_get_hash_size(
            spdm_context->connection_info.algorithm.base_hash_algo);
        if (!libspdm_hash_all(
                spdm_context->connection_info.algorithm.base_hash_algo,
                libspdm_get_managed_buffer(l1l2),
                libspdm_get_managed_buffer_size(l1l2), hash_data)) {
        return false;
    }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "l1l2 hash - "));
        LIBSPDM_INTERNAL_DUMP_DATA(hash_data, hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
        );

    return true;
}
#else
/*
 * This function calculates l1l2 hash.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  A pointer to the SPDM session context.
 * @param  l1l2_hash_size               size in bytes of the l1l2 hash
 * @param  l1l2_hash                   The buffer to store the l1l2 hash
 *
 * @retval RETURN_SUCCESS  l1l2 is calculated.
 */
bool libspdm_calculate_l1l2_hash(libspdm_context_t *spdm_context,
                                 void *session_info,
                                 size_t *l1l2_hash_size, void *l1l2_hash)
{
    libspdm_session_info_t *spdm_session_info;
    bool result;

    uint32_t hash_size;

    spdm_session_info = session_info;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    if (spdm_session_info == NULL) {
        result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                     spdm_context->transcript.digest_context_l1l2, l1l2_hash);
    } else {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "use message_m in session :\n"));
        result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                     spdm_session_info->session_transcript.digest_context_l1l2,
                                     l1l2_hash);
    }
    if (!result) {
        return false;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "l1l2 hash - "));
    LIBSPDM_INTERNAL_DUMP_DATA(l1l2_hash, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    *l1l2_hash_size = hash_size;

    return true;
}
#endif /* LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT */

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates m1m2.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_mut                        Indicate if this is from mutual authentication.
 * @param  m1m2                          The buffer to store the m1m2
 *
 * @retval RETURN_SUCCESS  m1m2 is calculated.
 */
static bool libspdm_calculate_m1m2(void *context, bool is_mut,
                                   libspdm_m1m2_managed_buffer_t *m1m2)
{
    libspdm_context_t *spdm_context;
    libspdm_return_t status;

    spdm_context = context;

    libspdm_init_managed_buffer(m1m2, sizeof(m1m2->buffer));

    if (is_mut) {
        if ((spdm_context->connection_info.version >> SPDM_VERSION_NUMBER_SHIFT_BIT) >
            SPDM_MESSAGE_VERSION_11) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_a data :\n"));
            LIBSPDM_INTERNAL_DUMP_HEX(
                libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
                libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
            status = libspdm_append_managed_buffer(
                m1m2,
                libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
                libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                return false;
            }
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_mut_b data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_mut_b),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_mut_b));
        status = libspdm_append_managed_buffer(
            m1m2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_mut_b),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_mut_b));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_mut_c data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_mut_c),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_mut_c));
        status = libspdm_append_managed_buffer(
            m1m2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_mut_c),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_mut_c));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }

        /* Debug code only - calculate and print value of m1m2 mut hash*/
        LIBSPDM_DEBUG_CODE(
            uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
            uint32_t hash_size = libspdm_get_hash_size(
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (!libspdm_hash_all(
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    libspdm_get_managed_buffer(m1m2),
                    libspdm_get_managed_buffer_size(m1m2), hash_data)) {
            return false;
        }
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "m1m2 Mut hash - "));
            LIBSPDM_INTERNAL_DUMP_DATA(hash_data, hash_size);
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
            );

    } else {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_a data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
        status = libspdm_append_managed_buffer(
            m1m2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_a),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_a));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_b data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_b),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_b));
        status = libspdm_append_managed_buffer(
            m1m2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_b),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_b));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "message_c data :\n"));
        LIBSPDM_INTERNAL_DUMP_HEX(
            libspdm_get_managed_buffer(&spdm_context->transcript.message_c),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_c));
        status = libspdm_append_managed_buffer(
            m1m2,
            libspdm_get_managed_buffer(&spdm_context->transcript.message_c),
            libspdm_get_managed_buffer_size(&spdm_context->transcript.message_c));
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            return false;
        }

        /* Debug code only - calculate and print value of m1m2 hash*/
        LIBSPDM_DEBUG_CODE(
            uint8_t hash_data[LIBSPDM_MAX_HASH_SIZE];
            uint32_t hash_size = libspdm_get_hash_size(
                spdm_context->connection_info.algorithm.base_hash_algo);
            if (!libspdm_hash_all(
                    spdm_context->connection_info.algorithm.base_hash_algo,
                    libspdm_get_managed_buffer(m1m2),
                    libspdm_get_managed_buffer_size(m1m2), hash_data)) {
            return false;
        }
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "m1m2 hash - "));
            LIBSPDM_INTERNAL_DUMP_DATA(hash_data, hash_size);
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
            );
    }

    return true;
}
#else
/*
 * This function calculates m1m2 hash.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_mut                        Indicate if this is from mutual authentication.
 * @param  m1m2_hash_size               size in bytes of the m1m2 hash
 * @param  m1m2_hash                   The buffer to store the m1m2 hash
 *
 * @retval RETURN_SUCCESS  m1m2 is calculated.
 */
static bool libspdm_calculate_m1m2_hash(void *context, bool is_mut,
                                        size_t *m1m2_hash_size,
                                        void *m1m2_hash)
{
    libspdm_context_t *spdm_context;
    uint32_t hash_size;
    bool result;

    spdm_context = context;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    if (is_mut) {
        result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                     spdm_context->transcript.digest_context_mut_m1m2, m1m2_hash);
        if (!result) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "m1m2 Mut hash - "));
        LIBSPDM_INTERNAL_DUMP_DATA(m1m2_hash, hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    } else {
        result = libspdm_hash_final (spdm_context->connection_info.algorithm.base_hash_algo,
                                     spdm_context->transcript.digest_context_m1m2, m1m2_hash);
        if (!result) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "m1m2 hash - "));
        LIBSPDM_INTERNAL_DUMP_DATA(m1m2_hash, hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    *m1m2_hash_size = hash_size;

    return true;
}
#endif

/**
 * This function generates the certificate chain hash.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  slot_id                    The slot index of the certificate chain.
 * @param  signature                    The buffer to store the certificate chain hash.
 *
 * @retval true  certificate chain hash is generated.
 * @retval false certificate chain hash is not generated.
 **/
bool libspdm_generate_cert_chain_hash(libspdm_context_t *spdm_context,
                                      size_t slot_id, uint8_t *hash)
{
    LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);
    return libspdm_hash_all(
        spdm_context->connection_info.algorithm.base_hash_algo,
        spdm_context->local_context.local_cert_chain_provision[slot_id],
        spdm_context->local_context.local_cert_chain_provision_size[slot_id], hash);
}

/**
 * This function generates the public key hash.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  hash                       The buffer to store the public key hash.
 *
 * @retval true  public key hash is generated.
 * @retval false public key hash is not generated.
 **/
bool libspdm_generate_public_key_hash(libspdm_context_t *spdm_context,
                                      uint8_t *hash)
{
    return libspdm_hash_all(
        spdm_context->connection_info.algorithm.base_hash_algo,
        spdm_context->local_context.local_public_key_provision,
        spdm_context->local_context.local_public_key_provision_size, hash);
}

/**
 * Get the certificate slot mask
 *
 * @param[in]   context              A pointer to the SPDM context.
 *
 * @retval slot_mask                 get slot mask
 **/
uint8_t libspdm_get_cert_slot_mask(libspdm_context_t *spdm_context)
{
    size_t index;
    uint8_t slot_mask;

    slot_mask = 0;
    for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
        if (spdm_context->local_context.local_cert_chain_provision[index] != NULL) {
            slot_mask |= (1 << index);
        }
    }

    return slot_mask;
}

/**
 * Get the certificate slot count
 *
 * @param[in]   context              A pointer to the SPDM context.
 *
 * @retval slot_count                get slot count
 **/
uint8_t libspdm_get_cert_slot_count(libspdm_context_t *spdm_context)
{
    size_t index;
    uint8_t slot_count;

    slot_count = 0;
    for (index = 0; index < SPDM_MAX_SLOT_COUNT; index++) {
        if (spdm_context->local_context.local_cert_chain_provision[index] != NULL) {
            slot_count++;
        }
    }

    return slot_count;
}

#if LIBSPDM_CERT_PARSE_SUPPORT
/**
 * This function verifies the integrity of peer certificate chain buffer including
 * spdm_cert_chain_t header.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_buffer              Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size          size in bytes of the certificate chain buffer.
 *
 * @retval true  Peer certificate chain buffer integrity verification passed.
 * @retval false Peer certificate chain buffer integrity verification failed.
 **/
bool libspdm_verify_peer_cert_chain_buffer_integrity(libspdm_context_t *spdm_context,
                                                     const void *cert_chain_buffer,
                                                     size_t cert_chain_buffer_size)
{
    bool result;
    bool is_device_cert_model;
    bool is_requester;

    is_requester = spdm_context->local_context.is_requester;

    is_device_cert_model = false;
    /* Responder does not determine Requester's certificate model */
    if (is_requester) {
        if((spdm_context->connection_info.capability.flags &
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ALIAS_CERT_CAP) == 0) {
            is_device_cert_model = true;
        }
    }

    if (is_requester) {
        result = libspdm_verify_certificate_chain_buffer(
            spdm_context->connection_info.algorithm.base_hash_algo,
            spdm_context->connection_info.algorithm.base_asym_algo,
            cert_chain_buffer, cert_chain_buffer_size,
            false, is_device_cert_model);
    } else {
        result = libspdm_verify_certificate_chain_buffer(
            spdm_context->connection_info.algorithm.base_hash_algo,
            spdm_context->connection_info.algorithm.req_base_asym_alg,
            cert_chain_buffer, cert_chain_buffer_size,
            true, is_device_cert_model);
    }

    return result;
}

/**
 * This function verifies peer certificate chain authority.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  cert_chain_buffer              Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size          size in bytes of the certificate chain buffer.
 * @param  trust_anchor                  A buffer to hold the trust_anchor which is used to validate the peer certificate, if not NULL.
 * @param  trust_anchor_size             A buffer to hold the trust_anchor_size, if not NULL.
 *
 * @retval true  Peer certificate chain buffer authority verification passed.
 *               Or there is no root_cert in local_context.
 * @retval false Peer certificate chain buffer authority verification failed.
 **/
bool libspdm_verify_peer_cert_chain_buffer_authority(libspdm_context_t *spdm_context,
                                                     const void *cert_chain_buffer,
                                                     size_t cert_chain_buffer_size,
                                                     const void **trust_anchor,
                                                     size_t *trust_anchor_size)
{
    const uint8_t *root_cert;
    size_t root_cert_size;
    uint8_t root_cert_index;
    size_t root_cert_hash_size;
    uint8_t root_cert_hash[LIBSPDM_MAX_HASH_SIZE];
    const uint8_t *received_root_cert;
    size_t received_root_cert_size;
    bool result;

    root_cert_index = 0;
    root_cert = spdm_context->local_context.peer_root_cert_provision[root_cert_index];
    root_cert_size = spdm_context->local_context.peer_root_cert_provision_size[root_cert_index];

    root_cert_hash_size = libspdm_get_hash_size(
        spdm_context->connection_info.algorithm.base_hash_algo);

    if ((root_cert != NULL) && (root_cert_size != 0)) {
        while ((root_cert != NULL) && (root_cert_size != 0)) {
            result = libspdm_hash_all(
                spdm_context->connection_info.algorithm.base_hash_algo,
                root_cert, root_cert_size, root_cert_hash);
            if (!result) {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                               "!!! verify_peer_cert_chain_buffer - FAIL (hash calculation) !!!\n"));
                return false;
            }

            if (libspdm_consttime_is_mem_equal((const uint8_t *)cert_chain_buffer +
                                               sizeof(spdm_cert_chain_t),
                                               root_cert_hash, root_cert_hash_size)) {
                break;
            }

            #if (LIBSPDM_MAX_ROOT_CERT_SUPPORT) > 1
            if ((root_cert_index < ((LIBSPDM_MAX_ROOT_CERT_SUPPORT) -1)) &&
                (spdm_context->local_context.peer_root_cert_provision[root_cert_index + 1] !=
                 NULL)) {
                root_cert_index++;
                root_cert = spdm_context->local_context.peer_root_cert_provision[root_cert_index];
                root_cert_size =
                    spdm_context->local_context.peer_root_cert_provision_size[root_cert_index];
            } else
            #endif /* LIBSPDM_MAX_ROOT_CERT_SUPPORT */
            {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                               "!!! verify_peer_cert_chain_buffer - "
                               "FAIL (all root cert hash mismatch) !!!\n"));
                return false;
            }
        }

        result = libspdm_x509_get_cert_from_cert_chain(
            (const uint8_t *)cert_chain_buffer + sizeof(spdm_cert_chain_t) + root_cert_hash_size,
            cert_chain_buffer_size - sizeof(spdm_cert_chain_t) - root_cert_hash_size,
            0, &received_root_cert, &received_root_cert_size);
        if (!result) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                           "!!! verify_peer_cert_chain_buffer - FAIL (cert retrieval fail) !!!\n"));
            return false;
        }
        if (libspdm_is_root_certificate(received_root_cert, received_root_cert_size)) {
            if ((root_cert != NULL) &&
                !libspdm_consttime_is_mem_equal(received_root_cert, root_cert, root_cert_size)) {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                               "!!! verify_peer_cert_chain_buffer - "
                               "FAIL (root cert mismatch) !!!\n"));
                return false;
            }
        } else {
            if (!libspdm_x509_verify_cert(received_root_cert, received_root_cert_size,
                                          root_cert, root_cert_size)) {
                LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                               "!!! verify_peer_cert_chain_buffer - "
                               "FAIL (received root cert verify failed)!!!\n"));
                return false;
            }
        }
        if (trust_anchor != NULL) {
            *trust_anchor = root_cert;
        }
        if (trust_anchor_size != NULL) {
            *trust_anchor_size = root_cert_size;
        }
    }
    /*
     * When there is no root_cert in local_context, the return is true too.
     * No root_cert means the caller wants to verify the trust anchor of the cert chain.
     */
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_peer_cert_chain_buffer - PASS !!!\n"));

    return true;
}
#endif

/**
 * This function generates the challenge signature based upon m1m2 for authentication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Indicate of the signature generation for a requester or a responder.
 * @param  signature                    The buffer to store the challenge signature.
 *
 * @retval true  challenge signature is generated.
 * @retval false challenge signature is not generated.
 **/
bool libspdm_generate_challenge_auth_signature(libspdm_context_t *spdm_context,
                                               bool is_requester,
                                               uint8_t *signature)
{
    bool result;
    size_t signature_size;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_m1m2_managed_buffer_t m1m2;
    uint8_t *m1m2_buffer;
    size_t m1m2_buffer_size;
#else
    uint8_t m1m2_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t m1m2_hash_size;
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_calculate_m1m2(spdm_context, is_requester, &m1m2);
    m1m2_buffer = libspdm_get_managed_buffer(&m1m2);
    m1m2_buffer_size = libspdm_get_managed_buffer_size(&m1m2);
#else
    m1m2_hash_size = sizeof(m1m2_hash);
    result = libspdm_calculate_m1m2_hash(spdm_context, is_requester, &m1m2_hash_size, &m1m2_hash);
#endif
    if (is_requester) {
        libspdm_reset_message_mut_b(spdm_context);
        libspdm_reset_message_mut_c(spdm_context);
    } else {
        libspdm_reset_message_b(spdm_context);
        libspdm_reset_message_c(spdm_context);
    }
    if (!result) {
        return false;
    }

    if (is_requester) {
#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
        signature_size = libspdm_get_req_asym_signature_size(
            spdm_context->connection_info.algorithm.req_base_asym_alg);
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        result = libspdm_requester_data_sign(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.req_base_asym_alg,
            spdm_context->connection_info.algorithm.base_hash_algo,
            false, m1m2_buffer, m1m2_buffer_size, signature, &signature_size);
#else
        result = libspdm_requester_data_sign(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.req_base_asym_alg,
            spdm_context->connection_info.algorithm.base_hash_algo,
            true, m1m2_hash, m1m2_hash_size, signature, &signature_size);
#endif
#else /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */
        result = false;
#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */
    } else {
        signature_size = libspdm_get_asym_signature_size(
            spdm_context->connection_info.algorithm.base_asym_algo);
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        result = libspdm_responder_data_sign(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.base_asym_algo,
            spdm_context->connection_info.algorithm.base_hash_algo,
            false, m1m2_buffer, m1m2_buffer_size, signature,
            &signature_size);
#else
        result = libspdm_responder_data_sign(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.base_asym_algo,
            spdm_context->connection_info.algorithm.base_hash_algo,
            true, m1m2_hash, m1m2_hash_size, signature,
            &signature_size);
#endif
    }

    return result;
}

/**
 * This function verifies the certificate chain hash.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  certificate_chain_hash         The certificate chain hash data buffer.
 * @param  certificate_chain_hash_size     size in bytes of the certificate chain hash data buffer.
 *
 * @retval true  hash verification pass.
 * @retval false hash verification fail.
 **/
bool libspdm_verify_certificate_chain_hash(libspdm_context_t *spdm_context,
                                           const void *certificate_chain_hash,
                                           size_t certificate_chain_hash_size)
{
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    size_t hash_size;
    uint8_t cert_chain_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    const uint8_t *cert_chain_buffer;
    size_t cert_chain_buffer_size;
    bool result;
#else
    uint8_t slot_id;
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_get_peer_cert_chain_buffer(spdm_context,
                                                (const void **)&cert_chain_buffer,
                                                &cert_chain_buffer_size);
    if (!result) {
        return false;
    }

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    result = libspdm_hash_all(spdm_context->connection_info.algorithm.base_hash_algo,
                              cert_chain_buffer, cert_chain_buffer_size,
                              cert_chain_buffer_hash);
    if (!result) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "!!! verify_certificate_chain_hash - FAIL (hash calculation) !!!\n"));
        return false;
    }

    if (hash_size != certificate_chain_hash_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_certificate_chain_hash - FAIL !!!\n"));
        return false;
    }
    if (!libspdm_consttime_is_mem_equal(certificate_chain_hash, cert_chain_buffer_hash,
                                        certificate_chain_hash_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_certificate_chain_hash - FAIL !!!\n"));
        return false;
    }
#else
    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT(slot_id < SPDM_MAX_SLOT_COUNT);

    LIBSPDM_ASSERT(
        spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size != 0);

    if (spdm_context->connection_info.peer_used_cert_chain[slot_id].buffer_hash_size !=
        certificate_chain_hash_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_certificate_chain_hash - FAIL !!!\n"));
        return false;
    }

    if (!libspdm_consttime_is_mem_equal(certificate_chain_hash,
                                        spdm_context->connection_info.peer_used_cert_chain[slot_id].
                                        buffer_hash, certificate_chain_hash_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_certificate_chain_hash - FAIL !!!\n"));
        return false;
    }
#endif
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_certificate_chain_hash - PASS !!!\n"));
    return true;
}

/**
 * This function verifies the public key hash.
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  public_key_hash         The public key hash data buffer.
 * @param  public_key_hash_size    size in bytes of the public key hash data buffer.
 *
 * @retval true  hash verification pass.
 * @retval false hash verification fail.
 **/
bool libspdm_verify_public_key_hash(libspdm_context_t *spdm_context,
                                    const void *public_key_hash,
                                    size_t public_key_hash_size)
{
    size_t hash_size;
    uint8_t public_key_buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    bool result;

    hash_size = libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);

    result = libspdm_hash_all(spdm_context->connection_info.algorithm.base_hash_algo,
                              spdm_context->local_context.peer_public_key_provision,
                              spdm_context->local_context.peer_public_key_provision_size,
                              public_key_buffer_hash);
    if (!result) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "!!! verify_public_key_hash - FAIL (hash calculation) !!!\n"));
        return false;
    }

    if (hash_size != public_key_hash_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_public_key_hash - FAIL !!!\n"));
        return false;
    }
    if (!libspdm_consttime_is_mem_equal(public_key_hash, public_key_buffer_hash,
                                        public_key_hash_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "!!! verify_public_key_hash - FAIL !!!\n"));
        return false;
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_public_key_hash - PASS !!!\n"));
    return true;
}

/**
 * This function verifies the challenge signature based upon m1m2.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Indicate of the signature verification for a requester or a responder.
 * @param  sign_data                     The signature data buffer.
 * @param  sign_data_size                 size in bytes of the signature data buffer.
 *
 * @retval true  signature verification pass.
 * @retval false signature verification fail.
 **/
bool libspdm_verify_challenge_auth_signature(libspdm_context_t *spdm_context,
                                             bool is_requester,
                                             const void *sign_data,
                                             size_t sign_data_size)
{
    bool result;
    void *context;
    uint8_t slot_id;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_m1m2_managed_buffer_t m1m2;
    uint8_t *m1m2_buffer;
    size_t m1m2_buffer_size;
    const uint8_t *cert_buffer;
    size_t cert_buffer_size;
    const uint8_t *cert_chain_data;
    size_t cert_chain_data_size;
#else
    uint8_t m1m2_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t m1m2_hash_size;
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    result = libspdm_calculate_m1m2(spdm_context, !is_requester, &m1m2);
    m1m2_buffer = libspdm_get_managed_buffer(&m1m2);
    m1m2_buffer_size = libspdm_get_managed_buffer_size(&m1m2);
#else
    m1m2_hash_size = sizeof(m1m2_hash);
    result = libspdm_calculate_m1m2_hash(spdm_context, !is_requester, &m1m2_hash_size, &m1m2_hash);
#endif
    if (is_requester) {
        libspdm_reset_message_b(spdm_context);
        libspdm_reset_message_c(spdm_context);
    } else {
        libspdm_reset_message_mut_b(spdm_context);
        libspdm_reset_message_mut_c(spdm_context);
    }
    if (!result) {
        return false;
    }

    slot_id = spdm_context->connection_info.peer_used_cert_chain_slot_id;
    LIBSPDM_ASSERT((slot_id < SPDM_MAX_SLOT_COUNT) || (slot_id == 0xFF));

    if (slot_id == 0xFF) {
        if (is_requester) {
            result = libspdm_asym_get_public_key_from_der(
                spdm_context->connection_info.algorithm.base_asym_algo,
                spdm_context->local_context.peer_public_key_provision,
                spdm_context->local_context.peer_public_key_provision_size,
                &context);
        } else {
            result = libspdm_req_asym_get_public_key_from_der(
                spdm_context->connection_info.algorithm.req_base_asym_alg,
                spdm_context->local_context.peer_public_key_provision,
                spdm_context->local_context.peer_public_key_provision_size,
                &context);
        }
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
        result = libspdm_x509_get_cert_from_cert_chain(
            cert_chain_data, cert_chain_data_size, -1, &cert_buffer, &cert_buffer_size);
        if (!result) {
            return false;
        }

        if (is_requester) {
            result = libspdm_asym_get_public_key_from_x509(
                spdm_context->connection_info.algorithm.base_asym_algo,
                cert_buffer, cert_buffer_size, &context);
        } else {
            result = libspdm_req_asym_get_public_key_from_x509(
                spdm_context->connection_info.algorithm.req_base_asym_alg,
                cert_buffer, cert_buffer_size, &context);
        }
        if (!result) {
            return false;
        }
#else
        context = spdm_context->connection_info.peer_used_cert_chain[slot_id].leaf_cert_public_key;
        LIBSPDM_ASSERT(context != NULL);
#endif
    }

    if (is_requester) {
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        result = libspdm_asym_verify_ex(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.base_asym_algo,
            spdm_context->connection_info.algorithm.base_hash_algo,
            context, m1m2_buffer, m1m2_buffer_size, sign_data, sign_data_size,
            &spdm_context->spdm_10_11_verify_signature_endian);
        libspdm_asym_free(
            spdm_context->connection_info.algorithm.base_asym_algo, context);
#else
        result = libspdm_asym_verify_hash_ex(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.base_asym_algo,
            spdm_context->connection_info.algorithm.base_hash_algo,
            context, m1m2_hash, m1m2_hash_size, sign_data, sign_data_size,
            &spdm_context->spdm_10_11_verify_signature_endian);
        if (slot_id == 0xFF) {
            libspdm_asym_free(
                spdm_context->connection_info.algorithm.base_asym_algo, context);
        }
#endif
    } else {
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
        result = libspdm_req_asym_verify_ex(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.req_base_asym_alg,
            spdm_context->connection_info.algorithm.base_hash_algo,
            context, m1m2_buffer, m1m2_buffer_size, sign_data, sign_data_size,
            &spdm_context->spdm_10_11_verify_signature_endian);
        libspdm_req_asym_free(
            spdm_context->connection_info.algorithm.req_base_asym_alg, context);
#else
        result = libspdm_req_asym_verify_hash_ex(
            spdm_context->connection_info.version, SPDM_CHALLENGE_AUTH,
            spdm_context->connection_info.algorithm.req_base_asym_alg,
            spdm_context->connection_info.algorithm.base_hash_algo,
            context, m1m2_hash, m1m2_hash_size, sign_data, sign_data_size,
            &spdm_context->spdm_10_11_verify_signature_endian);
        if (slot_id == 0xFF) {
            libspdm_req_asym_free(
                spdm_context->connection_info.algorithm.req_base_asym_alg, context);
        }
#endif
    }
    if (!result) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "!!! verify_challenge_signature - FAIL !!!\n"));
        return false;
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "!!! verify_challenge_signature - PASS !!!\n"));

    return true;
}

/**
 * This function calculate the measurement summary hash size.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Is the function called from a requester.
 * @param  measurement_summary_hash_type   The type of the measurement summary hash.
 *
 * @return 0 measurement summary hash type is invalid, NO_MEAS hash type or no MEAS capabilities.
 * @return measurement summary hash size according to type.
 **/
uint32_t
libspdm_get_measurement_summary_hash_size(libspdm_context_t *spdm_context,
                                          bool is_requester,
                                          uint8_t measurement_summary_hash_type)
{
    if (!libspdm_is_capabilities_flag_supported(
            spdm_context, is_requester, 0,
            SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP)) {
        return 0;
    }

    switch (measurement_summary_hash_type) {
    case SPDM_REQUEST_NO_MEASUREMENT_SUMMARY_HASH:
        return 0;
        break;

    case SPDM_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH:
    case SPDM_REQUEST_ALL_MEASUREMENTS_HASH:
        return libspdm_get_hash_size(spdm_context->connection_info.algorithm.base_hash_algo);
        break;
    default:
        return 0;
        break;
    }
}
