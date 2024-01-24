/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_secured_message_lib.h"

/**
 * This function concatenates binary data, which is used as info in HKDF expand later.
 *
 * @param  label        An ascii string label for the libspdm_bin_concat.
 * @param  label_size   The size in bytes of the ASCII string label, not including NULL terminator.
 * @param  context      A pre-defined hash value as the context for the libspdm_bin_concat.
 * @param  length       16 bits length for the libspdm_bin_concat.
 * @param  hash_size    The size in bytes of the context hash.
 * @param  out_bin      The buffer to store the output binary.
 * @param  out_bin_size The size in bytes for the out_bin.
 **/
void libspdm_bin_concat(spdm_version_number_t spdm_version,
                        const char *label, size_t label_size,
                        const uint8_t *context, uint16_t length,
                        size_t hash_size, uint8_t *out_bin,
                        size_t *out_bin_size)
{
    size_t final_size;

    /* The correct version characters (1.1 or 1.2) will replace the x.x. */
    #define LIBSPDM_BIN_CONCAT_LABEL "spdmx.x "

    final_size = sizeof(uint16_t) + sizeof(LIBSPDM_BIN_CONCAT_LABEL) - 1 + label_size;
    if (context != NULL) {
        final_size += hash_size;
    }

    LIBSPDM_ASSERT(*out_bin_size >= final_size);

    *out_bin_size = final_size;

    libspdm_copy_mem(out_bin, *out_bin_size, &length, sizeof(uint16_t));
    libspdm_copy_mem(out_bin + sizeof(uint16_t), *out_bin_size - sizeof(uint16_t),
                     LIBSPDM_BIN_CONCAT_LABEL, sizeof(LIBSPDM_BIN_CONCAT_LABEL) - 1);

    /* Patch the version. */
    out_bin[6] = (char)('0' + ((spdm_version >> 12) & 0xF));
    out_bin[8] = (char)('0' + ((spdm_version >> 8) & 0xF));
    libspdm_copy_mem(out_bin + sizeof(uint16_t) + sizeof(LIBSPDM_BIN_CONCAT_LABEL) - 1,
                     *out_bin_size - (sizeof(uint16_t) + sizeof(LIBSPDM_BIN_CONCAT_LABEL) - 1),
                     label, label_size);

    if (context != NULL) {
        libspdm_copy_mem(out_bin + sizeof(uint16_t) + sizeof(LIBSPDM_BIN_CONCAT_LABEL) -
                         1 + label_size,
                         *out_bin_size - (sizeof(uint16_t) + sizeof(LIBSPDM_BIN_CONCAT_LABEL) -
                                          1 + label_size), context, hash_size);
    }

    #undef LIBSPDM_BIN_CONCAT_LABEL
}

/**
 * This function generates SPDM AEAD key and IV for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  major_secret                  The major secret.
 * @param  key                          The buffer to store the AEAD key.
 * @param  iv                           The buffer to store the AEAD IV.
 *
 * @retval RETURN_SUCCESS  SPDM AEAD key and IV for a session is generated.
 **/
bool libspdm_generate_aead_key_and_iv(
    libspdm_secured_message_context_t *secured_message_context,
    const uint8_t *major_secret, uint8_t *key, uint8_t *iv)
{
    bool status;
    size_t hash_size;
    size_t key_length;
    size_t iv_length;
    uint8_t bin_str5[128];
    size_t bin_str5_size;
    uint8_t bin_str6[128];
    size_t bin_str6_size;

    hash_size = secured_message_context->hash_size;
    key_length = secured_message_context->aead_key_size;
    iv_length = secured_message_context->aead_iv_size;

    bin_str5_size = sizeof(bin_str5);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_5_LABEL, sizeof(SPDM_BIN_STR_5_LABEL) - 1,
                       NULL, (uint16_t)key_length, hash_size, bin_str5,
                       &bin_str5_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str5 (0x%x):\n", bin_str5_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str5, bin_str5_size);
    status = libspdm_hkdf_expand(secured_message_context->base_hash_algo,
                                 major_secret, hash_size, bin_str5,
                                 bin_str5_size, key, key_length);
    if (!status) {
        return false;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "key (0x%x) - ", key_length));
    LIBSPDM_INTERNAL_DUMP_DATA(key, key_length);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    bin_str6_size = sizeof(bin_str6);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_6_LABEL, sizeof(SPDM_BIN_STR_6_LABEL) - 1,
                       NULL, (uint16_t)iv_length, hash_size, bin_str6,
                       &bin_str6_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str6 (0x%x):\n", bin_str6_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str6, bin_str6_size);
    status = libspdm_hkdf_expand(secured_message_context->base_hash_algo,
                                 major_secret, hash_size, bin_str6,
                                 bin_str6_size, iv, iv_length);
    if (!status) {
        return false;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "iv (0x%x) - ", iv_length));
    LIBSPDM_INTERNAL_DUMP_DATA(iv, iv_length);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    return true;
}

/**
 * This function generates SPDM finished_key for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  handshake_secret              The handshake secret.
 * @param  finished_key                  The buffer to store the finished key.
 *
 * @retval RETURN_SUCCESS  SPDM finished_key for a session is generated.
 **/
bool libspdm_generate_finished_key(
    libspdm_secured_message_context_t *secured_message_context,
    const uint8_t *handshake_secret, uint8_t *finished_key)
{
    bool status;
    size_t hash_size;
    uint8_t bin_str7[128];
    size_t bin_str7_size;

    hash_size = secured_message_context->hash_size;

    bin_str7_size = sizeof(bin_str7);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_7_LABEL, sizeof(SPDM_BIN_STR_7_LABEL) - 1,
                       NULL, (uint16_t)hash_size, hash_size, bin_str7,
                       &bin_str7_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str7 (0x%x):\n", bin_str7_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str7, bin_str7_size);
    status = libspdm_hkdf_expand(secured_message_context->base_hash_algo,
                                 handshake_secret, hash_size, bin_str7,
                                 bin_str7_size, finished_key, hash_size);
    if (!status) {
        return false;
    }
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "finished_key (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(finished_key, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    return true;
}

/**
 * This function generates SPDM HandshakeKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  th1_hash_data                  th1 hash
 *
 * @retval RETURN_SUCCESS  SPDM HandshakeKey for a session is generated.
 **/
bool libspdm_generate_session_handshake_key(void *spdm_secured_message_context,
                                            const uint8_t *th1_hash_data)
{
    bool status;
    size_t hash_size;
    uint8_t bin_str1[128];
    size_t bin_str1_size;
    uint8_t bin_str2[128];
    size_t bin_str2_size;
    libspdm_secured_message_context_t *secured_message_context;
    uint8_t zero_filled_buffer[LIBSPDM_MAX_HASH_SIZE];

    secured_message_context = spdm_secured_message_context;

    hash_size = secured_message_context->hash_size;

    if (!(secured_message_context->use_psk)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "[DHE Secret]: "));
        LIBSPDM_INTERNAL_DUMP_HEX_STR(
            secured_message_context->master_secret.dhe_secret,
            secured_message_context->dhe_key_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
        libspdm_zero_mem(zero_filled_buffer, sizeof(zero_filled_buffer));
        status = libspdm_hkdf_extract(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.dhe_secret,
            secured_message_context->dhe_key_size,
            zero_filled_buffer, hash_size,
            secured_message_context->master_secret.handshake_secret, hash_size);
        if (!status) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "handshake_secret (0x%x) - ", hash_size));
        LIBSPDM_INTERNAL_DUMP_DATA(
            secured_message_context->master_secret.handshake_secret,
            hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    bin_str1_size = sizeof(bin_str1);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_1_LABEL, sizeof(SPDM_BIN_STR_1_LABEL) - 1,
                       th1_hash_data, (uint16_t)hash_size, hash_size,
                       bin_str1, &bin_str1_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str1 (0x%x):\n", bin_str1_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str1, bin_str1_size);

    #if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
    if (secured_message_context->use_psk) {
        status = libspdm_psk_handshake_secret_hkdf_expand(
            secured_message_context->version,
            secured_message_context->base_hash_algo,
            secured_message_context->psk_hint,
            secured_message_context->psk_hint_size, bin_str1,
            bin_str1_size,
            secured_message_context->handshake_secret.request_handshake_secret,
            hash_size);

        if (!status) {
            return false;
        }
    }
    #endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */
    if (!(secured_message_context->use_psk)) {
        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.handshake_secret,
            hash_size, bin_str1, bin_str1_size,
            secured_message_context->handshake_secret.request_handshake_secret,
            hash_size);

        if (!status) {
            return false;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "request_handshake_secret (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(secured_message_context->handshake_secret
                               .request_handshake_secret,
                               hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    bin_str2_size = sizeof(bin_str2);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_2_LABEL, sizeof(SPDM_BIN_STR_2_LABEL) - 1,
                       th1_hash_data, (uint16_t)hash_size, hash_size,
                       bin_str2, &bin_str2_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str2 (0x%x):\n", bin_str2_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str2, bin_str2_size);

    #if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
    if (secured_message_context->use_psk) {
        status = libspdm_psk_handshake_secret_hkdf_expand(
            secured_message_context->version,
            secured_message_context->base_hash_algo,
            secured_message_context->psk_hint,
            secured_message_context->psk_hint_size, bin_str2,
            bin_str2_size,
            secured_message_context->handshake_secret.response_handshake_secret,
            hash_size);

        if (!status) {
            return false;
        }
    }
    #endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */
    if (!(secured_message_context->use_psk)) {
        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.handshake_secret,
            hash_size, bin_str2, bin_str2_size,
            secured_message_context->handshake_secret.response_handshake_secret,
            hash_size);

        if (!status) {
            return false;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "response_handshake_secret (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(secured_message_context->handshake_secret.response_handshake_secret,
                               hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    status = libspdm_generate_finished_key(
        secured_message_context,
        secured_message_context->handshake_secret
        .request_handshake_secret,
        secured_message_context->handshake_secret.request_finished_key);
    if (!status) {
        return status;
    }

    status = libspdm_generate_finished_key(
        secured_message_context,
        secured_message_context->handshake_secret.response_handshake_secret,
        secured_message_context->handshake_secret.response_finished_key);
    if (!status) {
        return status;
    }

    status = libspdm_generate_aead_key_and_iv(secured_message_context,
                                              secured_message_context->handshake_secret
                                              .request_handshake_secret,
                                              secured_message_context->handshake_secret
                                              .request_handshake_encryption_key,
                                              secured_message_context->handshake_secret
                                              .request_handshake_salt);
    if (!status) {
        return status;
    }
    secured_message_context->handshake_secret.request_handshake_sequence_number = 0;

    status = libspdm_generate_aead_key_and_iv(
        secured_message_context,
        secured_message_context->handshake_secret.response_handshake_secret,
        secured_message_context->handshake_secret.response_handshake_encryption_key,
        secured_message_context->handshake_secret.response_handshake_salt);
    if (!status) {
        return status;
    }

    secured_message_context->handshake_secret.response_handshake_sequence_number = 0;
    libspdm_zero_mem(secured_message_context->master_secret.dhe_secret, LIBSPDM_MAX_DHE_KEY_SIZE);

    return true;
}

/**
 * This function generates SPDM DataKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  th2_hash_data                  th2 hash
 *
 * @retval RETURN_SUCCESS  SPDM DataKey for a session is generated.
 **/
bool libspdm_generate_session_data_key(void *spdm_secured_message_context,
                                       const uint8_t *th2_hash_data)
{
    bool status;
    size_t hash_size;
    uint8_t salt1[LIBSPDM_MAX_HASH_SIZE];
    uint8_t bin_str0[128];
    size_t bin_str0_size;
    uint8_t bin_str3[128];
    size_t bin_str3_size;
    uint8_t bin_str4[128];
    size_t bin_str4_size;
    uint8_t bin_str8[128];
    size_t bin_str8_size;
    libspdm_secured_message_context_t *secured_message_context;
    uint8_t zero_filled_buffer[LIBSPDM_MAX_HASH_SIZE];

    secured_message_context = spdm_secured_message_context;

    hash_size = secured_message_context->hash_size;

    if (!(secured_message_context->use_psk)) {
        bin_str0_size = sizeof(bin_str0);
        libspdm_bin_concat(secured_message_context->version,
                           SPDM_BIN_STR_0_LABEL,
                           sizeof(SPDM_BIN_STR_0_LABEL) - 1, NULL,
                           (uint16_t)hash_size, hash_size, bin_str0,
                           &bin_str0_size);

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str0 (0x%x):\n", bin_str0_size));
        LIBSPDM_INTERNAL_DUMP_HEX(bin_str0, bin_str0_size);

        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.handshake_secret,
            hash_size, bin_str0, bin_str0_size, salt1, hash_size);
        if (!status) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "salt1 (0x%x) - ", hash_size));
        LIBSPDM_INTERNAL_DUMP_DATA(salt1, hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

        libspdm_zero_mem(zero_filled_buffer, sizeof(zero_filled_buffer));
        status = libspdm_hkdf_extract(
            secured_message_context->base_hash_algo,
            zero_filled_buffer, hash_size, salt1, hash_size,
            secured_message_context->master_secret.master_secret, hash_size);
        if (!status) {
            goto cleanup;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "master_secret (0x%x) - ", hash_size));
        LIBSPDM_INTERNAL_DUMP_DATA(
            secured_message_context->master_secret.master_secret,
            hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    bin_str3_size = sizeof(bin_str3);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_3_LABEL, sizeof(SPDM_BIN_STR_3_LABEL) - 1,
                       th2_hash_data, (uint16_t)hash_size, hash_size,
                       bin_str3, &bin_str3_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str3 (0x%x):\n", bin_str3_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str3, bin_str3_size);

    #if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
    if (secured_message_context->use_psk) {
        status = libspdm_psk_master_secret_hkdf_expand(
            secured_message_context->version,
            secured_message_context->base_hash_algo,
            secured_message_context->psk_hint,
            secured_message_context->psk_hint_size, bin_str3,
            bin_str3_size,
            secured_message_context->application_secret.request_data_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }
    #endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */
    if (!(secured_message_context->use_psk)) {
        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.master_secret,
            hash_size, bin_str3, bin_str3_size,
            secured_message_context->application_secret.request_data_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "request_data_secret (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(
        secured_message_context->application_secret.request_data_secret,
        hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    bin_str4_size = sizeof(bin_str4);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_4_LABEL, sizeof(SPDM_BIN_STR_4_LABEL) - 1,
                       th2_hash_data, (uint16_t)hash_size, hash_size,
                       bin_str4, &bin_str4_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str4 (0x%x):\n", bin_str4_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str4, bin_str4_size);

    #if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
    if (secured_message_context->use_psk) {
        status = libspdm_psk_master_secret_hkdf_expand(
            secured_message_context->version,
            secured_message_context->base_hash_algo,
            secured_message_context->psk_hint,
            secured_message_context->psk_hint_size, bin_str4,
            bin_str4_size,
            secured_message_context->application_secret.response_data_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }
    #endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */
    if (!(secured_message_context->use_psk)) {
        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.master_secret,
            hash_size, bin_str4, bin_str4_size,
            secured_message_context->application_secret.response_data_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "response_data_secret (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(
        secured_message_context->application_secret.response_data_secret,
        hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    bin_str8_size = sizeof(bin_str8);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_8_LABEL, sizeof(SPDM_BIN_STR_8_LABEL) - 1,
                       th2_hash_data, (uint16_t)hash_size, hash_size,
                       bin_str8, &bin_str8_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str8 (0x%x):\n", bin_str8_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str8, bin_str8_size);

    #if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
    if (secured_message_context->use_psk) {
        status = libspdm_psk_master_secret_hkdf_expand(
            secured_message_context->version,
            secured_message_context->base_hash_algo,
            secured_message_context->psk_hint,
            secured_message_context->psk_hint_size, bin_str8,
            bin_str8_size,
            secured_message_context->export_master_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }
    #endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */
    if (!(secured_message_context->use_psk)) {
        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->master_secret.master_secret,
            hash_size, bin_str8, bin_str8_size,
            secured_message_context->export_master_secret,
            hash_size);

        if (!status) {
            goto cleanup;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "export_master_secret (0x%x) - ", hash_size));
    LIBSPDM_INTERNAL_DUMP_DATA(
        secured_message_context->export_master_secret, hash_size);
    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

    status = libspdm_generate_aead_key_and_iv(
        secured_message_context,
        secured_message_context->application_secret.request_data_secret,
        secured_message_context->application_secret.request_data_encryption_key,
        secured_message_context->application_secret.request_data_salt);
    if (!status) {
        goto cleanup;
    }
    secured_message_context->application_secret.request_data_sequence_number = 0;

    status = libspdm_generate_aead_key_and_iv(
        secured_message_context,
        secured_message_context->application_secret.response_data_secret,
        secured_message_context->application_secret.response_data_encryption_key,
        secured_message_context->application_secret.response_data_salt);
    if (!status) {
        goto cleanup;
    }
    secured_message_context->application_secret.response_data_sequence_number = 0;

cleanup:
    /*zero salt1 for security*/
    libspdm_zero_mem(salt1, hash_size);
    return status;
}

/**
 * This function creates the updates of SPDM DataKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  action                       Indicate of the key update action.
 *
 * @retval RETURN_SUCCESS  SPDM DataKey update is created.
 **/
bool libspdm_create_update_session_data_key(void *spdm_secured_message_context,
                                            libspdm_key_update_action_t action)
{
    bool status;
    size_t hash_size;
    uint8_t bin_str9[128];
    size_t bin_str9_size;
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;

    hash_size = secured_message_context->hash_size;

    bin_str9_size = sizeof(bin_str9);
    libspdm_bin_concat(secured_message_context->version,
                       SPDM_BIN_STR_9_LABEL, sizeof(SPDM_BIN_STR_9_LABEL) - 1,
                       NULL, (uint16_t)hash_size, hash_size, bin_str9,
                       &bin_str9_size);

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "bin_str9 (0x%x):\n", bin_str9_size));
    LIBSPDM_INTERNAL_DUMP_HEX(bin_str9, bin_str9_size);

    if (action == LIBSPDM_KEY_UPDATE_ACTION_REQUESTER) {
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .request_data_secret,
                         sizeof(secured_message_context->application_secret_backup
                                .request_data_secret),
                         &secured_message_context->application_secret
                         .request_data_secret,
                         LIBSPDM_MAX_HASH_SIZE);
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .request_data_encryption_key,
                         sizeof(secured_message_context->application_secret_backup
                                .request_data_encryption_key),
                         &secured_message_context->application_secret
                         .request_data_encryption_key,
                         LIBSPDM_MAX_AEAD_KEY_SIZE);
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .request_data_salt,
                         sizeof(secured_message_context->application_secret_backup
                                .request_data_salt),
                         &secured_message_context->application_secret
                         .request_data_salt,
                         LIBSPDM_MAX_AEAD_IV_SIZE);
        secured_message_context->application_secret_backup
        .request_data_sequence_number =
            secured_message_context->application_secret.request_data_sequence_number;

        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->application_secret.request_data_secret,
            hash_size, bin_str9, bin_str9_size,
            secured_message_context->application_secret.request_data_secret,
            hash_size);
        if (!status) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "RequestDataSecretUpdate (0x%x) - ", hash_size));
        LIBSPDM_INTERNAL_DUMP_DATA(secured_message_context->application_secret.request_data_secret,
                                   hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

        status = libspdm_generate_aead_key_and_iv(
            secured_message_context,
            secured_message_context->application_secret.request_data_secret,
            secured_message_context->application_secret.request_data_encryption_key,
            secured_message_context->application_secret.request_data_salt);
        if (!status) {
            return status;
        }
        secured_message_context->application_secret.request_data_sequence_number = 0;

        secured_message_context->requester_backup_valid = true;
    } else if (action == LIBSPDM_KEY_UPDATE_ACTION_RESPONDER) {
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .response_data_secret,
                         sizeof(secured_message_context->application_secret_backup
                                .response_data_secret),
                         &secured_message_context->application_secret
                         .response_data_secret,
                         LIBSPDM_MAX_HASH_SIZE);
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .response_data_encryption_key,
                         sizeof(secured_message_context->application_secret_backup
                                .response_data_encryption_key),
                         &secured_message_context->application_secret
                         .response_data_encryption_key,
                         LIBSPDM_MAX_AEAD_KEY_SIZE);
        libspdm_copy_mem(&secured_message_context->application_secret_backup
                         .response_data_salt,
                         sizeof(secured_message_context->application_secret_backup
                                .response_data_salt),
                         &secured_message_context->application_secret
                         .response_data_salt,
                         LIBSPDM_MAX_AEAD_IV_SIZE);
        secured_message_context->application_secret_backup
        .response_data_sequence_number =
            secured_message_context->application_secret.response_data_sequence_number;

        status = libspdm_hkdf_expand(
            secured_message_context->base_hash_algo,
            secured_message_context->application_secret.response_data_secret,
            hash_size, bin_str9, bin_str9_size,
            secured_message_context->application_secret.response_data_secret,
            hash_size);
        if (!status) {
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "ResponseDataSecretUpdate (0x%x) - ",
                       hash_size));
        LIBSPDM_INTERNAL_DUMP_DATA(secured_message_context->application_secret.response_data_secret,
                                   hash_size);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));

        status = libspdm_generate_aead_key_and_iv(
            secured_message_context,
            secured_message_context->application_secret.response_data_secret,
            secured_message_context->application_secret.response_data_encryption_key,
            secured_message_context->application_secret.response_data_salt);
        if (!status) {
            return status;
        }
        secured_message_context->application_secret.response_data_sequence_number = 0;

        secured_message_context->responder_backup_valid = true;
    } else {
        return false;
    }

    return true;
}

void libspdm_clear_handshake_secret(void *spdm_secured_message_context)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;

    libspdm_zero_mem(secured_message_context->master_secret.handshake_secret,
                     LIBSPDM_MAX_HASH_SIZE);
    libspdm_zero_mem(&(secured_message_context->handshake_secret),
                     sizeof(libspdm_session_info_struct_handshake_secret_t));

    secured_message_context->requester_backup_valid = false;
    secured_message_context->responder_backup_valid = false;
}

void libspdm_clear_master_secret(void *spdm_secured_message_context)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;

    libspdm_zero_mem(secured_message_context->master_secret.master_secret, LIBSPDM_MAX_HASH_SIZE);
}

/**
 * This function activates the update of SPDM DataKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  action                       Indicate of the key update action.
 * @param  use_new_key                    Indicate if the new key should be used.
 *
 * @retval RETURN_SUCCESS  SPDM DataKey update is activated.
 **/
bool libspdm_activate_update_session_data_key(void *spdm_secured_message_context,
                                              libspdm_key_update_action_t action,
                                              bool use_new_key)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;

    if (!use_new_key) {
        if ((action == LIBSPDM_KEY_UPDATE_ACTION_REQUESTER) &&
            secured_message_context->requester_backup_valid) {
            libspdm_copy_mem(&secured_message_context->application_secret
                             .request_data_secret,
                             sizeof(secured_message_context->application_secret
                                    .request_data_secret),
                             &secured_message_context
                             ->application_secret_backup
                             .request_data_secret,
                             LIBSPDM_MAX_HASH_SIZE);
            libspdm_copy_mem(&secured_message_context->application_secret
                             .request_data_encryption_key,
                             sizeof(secured_message_context->application_secret
                                    .request_data_encryption_key),
                             &secured_message_context
                             ->application_secret_backup
                             .request_data_encryption_key,
                             LIBSPDM_MAX_AEAD_KEY_SIZE);
            libspdm_copy_mem(&secured_message_context->application_secret
                             .request_data_salt,
                             sizeof(secured_message_context->application_secret
                                    .request_data_salt),
                             &secured_message_context
                             ->application_secret_backup
                             .request_data_salt,
                             LIBSPDM_MAX_AEAD_IV_SIZE);
            secured_message_context->application_secret
            .request_data_sequence_number =
                secured_message_context->application_secret_backup.request_data_sequence_number;
        } else if ((action == LIBSPDM_KEY_UPDATE_ACTION_RESPONDER) &&
                   secured_message_context->responder_backup_valid) {
            libspdm_copy_mem(&secured_message_context->application_secret
                             .response_data_secret,
                             sizeof(secured_message_context->application_secret
                                    .response_data_secret),
                             &secured_message_context
                             ->application_secret_backup
                             .response_data_secret,
                             LIBSPDM_MAX_HASH_SIZE);
            libspdm_copy_mem(&secured_message_context->application_secret
                             .response_data_encryption_key,
                             sizeof(secured_message_context->application_secret
                                    .response_data_encryption_key),
                             &secured_message_context
                             ->application_secret_backup
                             .response_data_encryption_key,
                             LIBSPDM_MAX_AEAD_KEY_SIZE);
            libspdm_copy_mem(&secured_message_context->application_secret
                             .response_data_salt,
                             sizeof(secured_message_context->application_secret
                                    .response_data_salt),
                             &secured_message_context
                             ->application_secret_backup
                             .response_data_salt,
                             LIBSPDM_MAX_AEAD_IV_SIZE);
            secured_message_context->application_secret.response_data_sequence_number =
                secured_message_context->application_secret_backup.response_data_sequence_number;
        }
    }

    if (action == LIBSPDM_KEY_UPDATE_ACTION_REQUESTER) {
        libspdm_zero_mem(&secured_message_context->application_secret_backup.request_data_secret,
                         LIBSPDM_MAX_HASH_SIZE);
        libspdm_zero_mem(&secured_message_context->application_secret_backup
                         .request_data_encryption_key,
                         LIBSPDM_MAX_AEAD_KEY_SIZE);
        libspdm_zero_mem(&secured_message_context->application_secret_backup.request_data_salt,
                         LIBSPDM_MAX_AEAD_IV_SIZE);
        secured_message_context->application_secret_backup.request_data_sequence_number = 0;
        secured_message_context->requester_backup_valid = false;
    } else if (action == LIBSPDM_KEY_UPDATE_ACTION_RESPONDER) {
        libspdm_zero_mem(&secured_message_context->application_secret_backup.response_data_secret,
                         LIBSPDM_MAX_HASH_SIZE);
        libspdm_zero_mem(&secured_message_context->application_secret_backup
                         .response_data_encryption_key,
                         LIBSPDM_MAX_AEAD_KEY_SIZE);
        libspdm_zero_mem(&secured_message_context->application_secret_backup.response_data_salt,
                         LIBSPDM_MAX_AEAD_IV_SIZE);
        secured_message_context->application_secret_backup.response_data_sequence_number = 0;
        secured_message_context->responder_backup_valid = false;
    }

    return true;
}

/**
 * Allocates and initializes one HMAC context for subsequent use, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return Pointer to the HMAC context that has been initialized.
 **/
void *libspdm_hmac_new_with_request_finished_key(void *spdm_secured_message_context)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_new(secured_message_context->base_hash_algo);
}

/**
 * Release the specified HMAC context, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx                   Pointer to the HMAC context to be released.
 **/
void libspdm_hmac_free_with_request_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    libspdm_hmac_free(secured_message_context->base_hash_algo, hmac_ctx);
}

/**
 * Set request_finished_key for subsequent use. It must be done before any
 * calling to hmac_update().
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx  Pointer to HMAC context.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 **/
bool libspdm_hmac_init_with_request_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_init(
        secured_message_context->base_hash_algo, hmac_ctx,
        secured_message_context->handshake_secret.request_finished_key,
        secured_message_context->hash_size);
}

/**
 * Makes a copy of an existing HMAC context, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  new_hmac_ctx  Pointer to new HMAC context.
 *
 * @retval true   HMAC context copy succeeded.
 * @retval false  HMAC context copy failed.
 **/
bool libspdm_hmac_duplicate_with_request_finished_key(
    void *spdm_secured_message_context,
    const void *hmac_ctx, void *new_hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_duplicate(secured_message_context->base_hash_algo, hmac_ctx, new_hmac_ctx);
}

/**
 * Digests the input data and updates HMAC context, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  data              Pointer to the buffer containing the data to be digested.
 * @param  data_size          size of data buffer in bytes.
 *
 * @retval true   HMAC data digest succeeded.
 * @retval false  HMAC data digest failed.
 **/
bool libspdm_hmac_update_with_request_finished_key(
    void *spdm_secured_message_context,
    void *hmac_ctx, const void *data,
    size_t data_size)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_update(secured_message_context->base_hash_algo, hmac_ctx, data, data_size);
}

/**
 * Completes computation of the HMAC digest value, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  hmac_value          Pointer to a buffer that receives the HMAC digest value
 *
 * @retval true   HMAC data digest succeeded.
 * @retval false  HMAC data digest failed.
 **/
bool libspdm_hmac_final_with_request_finished_key(
    void *spdm_secured_message_context,
    void *hmac_ctx,  uint8_t *hmac_value)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_final(secured_message_context->base_hash_algo, hmac_ctx, hmac_value);
}

/**
 * Computes the HMAC of a input data buffer, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  data                         Pointer to the buffer containing the data to be HMACed.
 * @param  data_size                     size of data buffer in bytes.
 * @param  hash_value                    Pointer to a buffer that receives the HMAC value.
 *
 * @retval true   HMAC computation succeeded.
 * @retval false  HMAC computation failed.
 **/
bool libspdm_hmac_all_with_request_finished_key(void *spdm_secured_message_context,
                                                const void *data, size_t data_size,
                                                uint8_t *hmac_value)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_all(
        secured_message_context->base_hash_algo, data, data_size,
        secured_message_context->handshake_secret.request_finished_key,
        secured_message_context->hash_size, hmac_value);
}

/**
 * Allocates and initializes one HMAC context for subsequent use, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return Pointer to the HMAC context that has been initialized.
 **/
void *libspdm_hmac_new_with_response_finished_key(void *spdm_secured_message_context)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_new(secured_message_context->base_hash_algo);
}

/**
 * Release the specified HMAC context, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx                   Pointer to the HMAC context to be released.
 **/
void libspdm_hmac_free_with_response_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    libspdm_hmac_free(secured_message_context->base_hash_algo, hmac_ctx);
}

/**
 * Set response_finished_key for subsequent use. It must be done before any
 * calling to hmac_update().
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx  Pointer to HMAC context.
 *
 * @retval true   The key is set successfully.
 * @retval false  The key is set unsuccessfully.
 **/
bool libspdm_hmac_init_with_response_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_init(
        secured_message_context->base_hash_algo, hmac_ctx,
        secured_message_context->handshake_secret.response_finished_key,
        secured_message_context->hash_size);
}

/**
 * Makes a copy of an existing HMAC context, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  new_hmac_ctx  Pointer to new HMAC context.
 *
 * @retval true   HMAC context copy succeeded.
 * @retval false  HMAC context copy failed.
 **/
bool libspdm_hmac_duplicate_with_response_finished_key(
    void *spdm_secured_message_context,
    const void *hmac_ctx, void *new_hmac_ctx)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_duplicate(secured_message_context->base_hash_algo, hmac_ctx, new_hmac_ctx);
}

/**
 * Digests the input data and updates HMAC context, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  data              Pointer to the buffer containing the data to be digested.
 * @param  data_size          size of data buffer in bytes.
 *
 * @retval true   HMAC data digest succeeded.
 * @retval false  HMAC data digest failed.
 **/
bool libspdm_hmac_update_with_response_finished_key(
    void *spdm_secured_message_context,
    void *hmac_ctx, const void *data,
    size_t data_size)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_update(secured_message_context->base_hash_algo, hmac_ctx, data, data_size);
}

/**
 * Completes computation of the HMAC digest value, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx     Pointer to HMAC context being copied.
 * @param  hmac_value          Pointer to a buffer that receives the HMAC digest value
 *
 * @retval true   HMAC data digest succeeded.
 * @retval false  HMAC data digest failed.
 **/
bool libspdm_hmac_final_with_response_finished_key(
    void *spdm_secured_message_context,
    void *hmac_ctx,  uint8_t *hmac_value)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_final(secured_message_context->base_hash_algo, hmac_ctx, hmac_value);
}

/**
 * Computes the HMAC of a input data buffer, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  data                         Pointer to the buffer containing the data to be HMACed.
 * @param  data_size                     size of data buffer in bytes.
 * @param  hash_value                    Pointer to a buffer that receives the HMAC value.
 *
 * @retval true   HMAC computation succeeded.
 * @retval false  HMAC computation failed.
 **/
bool libspdm_hmac_all_with_response_finished_key(
    void *spdm_secured_message_context, const void *data,
    size_t data_size, uint8_t *hmac_value)
{
    libspdm_secured_message_context_t *secured_message_context;

    secured_message_context = spdm_secured_message_context;
    return libspdm_hmac_all(
        secured_message_context->base_hash_algo, data, data_size,
        secured_message_context->handshake_secret.response_finished_key,
        secured_message_context->hash_size, hmac_value);
}
