/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_SECURED_MESSAGE_LIB_INTERNAL_H
#define SPDM_SECURED_MESSAGE_LIB_INTERNAL_H

#include "library/spdm_secured_message_lib.h"
#include "library/spdm_crypt_lib.h"
#include "library/spdm_device_secret_lib.h"
#include "internal/libspdm_common_lib.h"
#include "hal/library/memlib.h"
#include "hal/library/cryptlib.h"

typedef struct {
    uint8_t dhe_secret[LIBSPDM_MAX_DHE_KEY_SIZE];
    uint8_t handshake_secret[LIBSPDM_MAX_HASH_SIZE];
    uint8_t master_secret[LIBSPDM_MAX_HASH_SIZE];
} libspdm_session_info_struct_master_secret_t;

typedef struct {
    uint8_t request_handshake_secret[LIBSPDM_MAX_HASH_SIZE];
    uint8_t response_handshake_secret[LIBSPDM_MAX_HASH_SIZE];
    uint8_t request_finished_key[LIBSPDM_MAX_HASH_SIZE];
    uint8_t response_finished_key[LIBSPDM_MAX_HASH_SIZE];
    uint8_t request_handshake_encryption_key[LIBSPDM_MAX_AEAD_KEY_SIZE];
    uint8_t request_handshake_salt[LIBSPDM_MAX_AEAD_IV_SIZE];
    uint64_t request_handshake_sequence_number;
    uint8_t response_handshake_encryption_key[LIBSPDM_MAX_AEAD_KEY_SIZE];
    uint8_t response_handshake_salt[LIBSPDM_MAX_AEAD_IV_SIZE];
    uint64_t response_handshake_sequence_number;
} libspdm_session_info_struct_handshake_secret_t;

typedef struct {
    uint8_t request_data_secret[LIBSPDM_MAX_HASH_SIZE];
    uint8_t response_data_secret[LIBSPDM_MAX_HASH_SIZE];
    uint8_t request_data_encryption_key[LIBSPDM_MAX_AEAD_KEY_SIZE];
    uint8_t request_data_salt[LIBSPDM_MAX_AEAD_IV_SIZE];
    uint64_t request_data_sequence_number;
    uint8_t response_data_encryption_key[LIBSPDM_MAX_AEAD_KEY_SIZE];
    uint8_t response_data_salt[LIBSPDM_MAX_AEAD_IV_SIZE];
    uint64_t response_data_sequence_number;
} libspdm_session_info_struct_application_secret_t;

typedef struct {
    libspdm_session_type_t session_type;
    spdm_version_number_t version;
    spdm_version_number_t secured_message_version;
    uint32_t base_hash_algo;
    uint16_t dhe_named_group;
    uint16_t aead_cipher_suite;
    uint16_t key_schedule;
    size_t hash_size;
    size_t dhe_key_size;
    size_t aead_key_size;
    size_t aead_iv_size;
    size_t aead_tag_size;
    bool use_psk;
    libspdm_session_state_t session_state;
    libspdm_session_info_struct_master_secret_t master_secret;
    libspdm_session_info_struct_handshake_secret_t handshake_secret;
    libspdm_session_info_struct_application_secret_t application_secret;
    libspdm_session_info_struct_application_secret_t application_secret_backup;
    bool requester_backup_valid;
    bool responder_backup_valid;
    size_t psk_hint_size;
    uint8_t psk_hint[LIBSPDM_PSK_MAX_HINT_LENGTH];
    uint8_t export_master_secret[LIBSPDM_MAX_HASH_SIZE];

    /* Cache the error in libspdm_decode_secured_message.
     * It is handled in libspdm_build_response. */
    libspdm_error_struct_t last_spdm_error;
} libspdm_secured_message_context_t;

#define LIBSPDM_SECURED_MESSAGE_CONTEXT_SIZE (sizeof(libspdm_secured_message_context_t))

#endif /* SPDM_SECURED_MESSAGE_LIB_INTERNAL_H */
