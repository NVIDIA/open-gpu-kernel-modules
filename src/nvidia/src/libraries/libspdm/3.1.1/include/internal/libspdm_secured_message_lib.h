/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_SECURED_MESSAGE_LIB_INTERNAL_H
#define SPDM_SECURED_MESSAGE_LIB_INTERNAL_H

#include "library/spdm_secured_message_lib.h"
#include "library/spdm_crypt_lib.h"
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
    uint64_t max_spdm_session_sequence_number;
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
    uint8_t sequence_number_endian;

    /* Cache the error in libspdm_decode_secured_message.
     * It is handled in libspdm_build_response. */
    libspdm_error_struct_t last_spdm_error;
} libspdm_secured_message_context_t;

#define LIBSPDM_SECURED_MESSAGE_CONTEXT_SIZE (sizeof(libspdm_secured_message_context_t))

/**
 * Initialize an SPDM secured message context.
 *
 * The size in bytes of the spdm_secured_message_context can be returned by libspdm_secured_message_get_context_size.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 */
void libspdm_secured_message_init_context(void *spdm_secured_message_context);

/**
 * Set use_psk to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  use_psk                       Indicate if the SPDM session use PSK.
 */
void libspdm_secured_message_set_use_psk(void *spdm_secured_message_context, bool use_psk);

/**
 * Set session_state to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_state                 Indicate the SPDM session state.
 */
void libspdm_secured_message_set_session_state(
    void *spdm_secured_message_context,
    libspdm_session_state_t session_state);

/**
 * Set session_type to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_type                  Indicate the SPDM session type.
 */
void libspdm_secured_message_set_session_type(void *spdm_secured_message_context,
                                              libspdm_session_type_t session_type);

/**
 * Set algorithm to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  base_hash_algo                 Indicate the negotiated base_hash_algo for the SPDM session.
 * @param  dhe_named_group                Indicate the negotiated dhe_named_group for the SPDM session.
 * @param  aead_cipher_suite              Indicate the negotiated aead_cipher_suite for the SPDM session.
 * @param  key_schedule                  Indicate the negotiated key_schedule for the SPDM session.
 */
void libspdm_secured_message_set_algorithms(void *spdm_secured_message_context,
                                            const spdm_version_number_t version,
                                            const spdm_version_number_t secured_message_version,
                                            uint32_t base_hash_algo,
                                            uint16_t dhe_named_group,
                                            uint16_t aead_cipher_suite,
                                            uint16_t key_schedule);

/**
 * Set the psk_hint to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  psk_hint                      Indicate the PSK hint.
 * @param  psk_hint_size                  The size in bytes of the PSK hint.
 */
void libspdm_secured_message_set_psk_hint(void *spdm_secured_message_context,
                                          const void *psk_hint,
                                          size_t psk_hint_size);

/**
 * Set the maximum sequence_number to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context      A pointer to the SPDM secured message context.
 * @param  max_spdm_session_sequence_number  Indicate the maximum sequence_number in SPDM session.
 */
void libspdm_secured_message_set_max_spdm_session_sequence_number(
    void *spdm_secured_message_context,
    uint64_t max_spdm_session_sequence_number);

/**
 * Set the endianness of the sequence number used to construct the AEAD IV.
 *
 * @param spdm_secured_message_context A pointer to the SPDM secured message context.
 * @param endian_value                 The endianness value.
 *
 */
void libspdm_secured_message_set_sequence_number_endian(
    void *spdm_secured_message_context,
    uint8_t endian_value);

/**
 * Allocates and Initializes one Diffie-Hellman Ephemeral (DHE) context for subsequent use,
 * based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  is_initiator                   if the caller is initiator.
 *                                       true: initiator
 *                                       false: not an initiator
 *
 * @return  Pointer to the Diffie-Hellman context that has been initialized.
 **/
void *libspdm_secured_message_dhe_new(spdm_version_number_t spdm_version,
                                      uint16_t dhe_named_group, bool is_initiator);

/**
 * Release the specified DHE context,
 * based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context to be released.
 **/
void libspdm_secured_message_dhe_free(uint16_t dhe_named_group, void *dhe_context);

/**
 * Generates DHE public key,
 * based upon negotiated DHE algorithm.
 *
 * This function generates random secret exponent, and computes the public key, which is
 * returned via parameter public_key and public_key_size. DH context is updated accordingly.
 * If the public_key buffer is too small to hold the public key, false is returned and
 * public_key_size is set to the required buffer size to obtain the public key.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context.
 * @param  public_key                    Pointer to the buffer to receive generated public key.
 * @param  public_key_size                On input, the size of public_key buffer in bytes.
 *                                     On output, the size of data returned in public_key buffer in bytes.
 *
 * @retval true   DHE public key generation succeeded.
 * @retval false  DHE public key generation failed.
 * @retval false  public_key_size is not large enough.
 **/
bool libspdm_secured_message_dhe_generate_key(uint16_t dhe_named_group,
                                              void *dhe_context,
                                              uint8_t *public_key,
                                              size_t *public_key_size);

/**
 * Computes exchanged common key,
 * based upon negotiated DHE algorithm.
 *
 * Given peer's public key, this function computes the exchanged common key, based on its own
 * context including value of prime modulus and random secret exponent.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context.
 * @param  peer_public_key                Pointer to the peer's public key.
 * @param  peer_public_key_size            size of peer's public key in bytes.
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @retval true   DHE exchanged key generation succeeded.
 * @retval false  DHE exchanged key generation failed.
 * @retval false  key_size is not large enough.
 **/
bool libspdm_secured_message_dhe_compute_key(
    uint16_t dhe_named_group, void *dhe_context,
    const uint8_t *peer_public, size_t peer_public_size,
    void *spdm_secured_message_context);

/**
 * Allocates and initializes one HMAC context for subsequent use, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return Pointer to the HMAC context that has been initialized.
 **/
void *libspdm_hmac_new_with_request_finished_key(void *spdm_secured_message_context);

/**
 * Release the specified HMAC context, with request_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx                   Pointer to the HMAC context to be released.
 **/
void libspdm_hmac_free_with_request_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx);

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
    void *spdm_secured_message_context, void *hmac_ctx);

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
    const void *hmac_ctx, void *new_hmac_ctx);

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
    size_t data_size);

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
    void *hmac_ctx,  uint8_t *hmac_value);

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
                                                uint8_t *hmac_value);

/**
 * Allocates and initializes one HMAC context for subsequent use, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return Pointer to the HMAC context that has been initialized.
 **/
void *libspdm_hmac_new_with_response_finished_key(void *spdm_secured_message_context);

/**
 * Release the specified HMAC context, with response_finished_key.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  hmac_ctx                   Pointer to the HMAC context to be released.
 **/
void libspdm_hmac_free_with_response_finished_key(
    void *spdm_secured_message_context, void *hmac_ctx);

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
    void *spdm_secured_message_context, void *hmac_ctx);

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
    const void *hmac_ctx, void *new_hmac_ctx);

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
    size_t data_size);

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
    void *hmac_ctx,  uint8_t *hmac_value);

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
    size_t data_size, uint8_t *hmac_value);

/**
 * Set the last SPDM error struct of an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM secured message context.
 */
void libspdm_secured_message_set_last_spdm_error_struct(
    void *spdm_secured_message_context,
    const libspdm_error_struct_t *last_spdm_error);

/**
 * This function generates SPDM HandshakeKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  th1_hash_data                  th1 hash
 *
 * @retval RETURN_SUCCESS  SPDM HandshakeKey for a session is generated.
 **/
bool libspdm_generate_session_handshake_key(void *spdm_secured_message_context,
                                            const uint8_t *th1_hash_data);

/**
 * This function generates SPDM DataKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  th2_hash_data                  th2 hash
 *
 * @retval RETURN_SUCCESS  SPDM DataKey for a session is generated.
 **/
bool libspdm_generate_session_data_key(void *spdm_secured_message_context,
                                       const uint8_t *th2_hash_data);

/**
 * This function creates the updates of SPDM DataKey for a session.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  action                       Indicate of the key update action.
 *
 * @retval RETURN_SUCCESS  SPDM DataKey update is created.
 **/
bool libspdm_create_update_session_data_key(void *spdm_secured_message_context,
                                            libspdm_key_update_action_t action);

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
                                              bool use_new_key);

#endif /* SPDM_SECURED_MESSAGE_LIB_INTERNAL_H */
