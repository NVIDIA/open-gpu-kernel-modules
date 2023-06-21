/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_SECURED_MESSAGE_LIB_H
#define SPDM_SECURED_MESSAGE_LIB_H

#include "hal/base.h"
#include "industry_standard/spdm.h"
#include "industry_standard/spdm_secured_message.h"
#include "library/spdm_return_status.h"

typedef enum {
    LIBSPDM_SESSION_TYPE_NONE,
    LIBSPDM_SESSION_TYPE_MAC_ONLY,
    LIBSPDM_SESSION_TYPE_ENC_MAC,
    LIBSPDM_SESSION_TYPE_MAX
} libspdm_session_type_t;

typedef enum {
    /* Before send KEY_EXCHANGE/PSK_EXCHANGE or after END_SESSION */
    LIBSPDM_SESSION_STATE_NOT_STARTED,

    /* After send KEY_EXHCNAGE, before send FINISH */
    LIBSPDM_SESSION_STATE_HANDSHAKING,

    /* After send FINISH, before END_SESSION */
    LIBSPDM_SESSION_STATE_ESTABLISHED,

    /* MAX */
    LIBSPDM_SESSION_STATE_MAX
} libspdm_session_state_t;

/**
 * Return the size in bytes of a single SPDM secured message context.
 *
 * @return the size in bytes of a single SPDM secured message context.
 **/
size_t libspdm_secured_message_get_context_size(void);

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
 * Return session_state of an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return the SPDM session state.
 */
libspdm_session_state_t
libspdm_secured_message_get_session_state(void *spdm_secured_message_context);

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
 * Import the DHE Secret to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  dhe_secret                    Indicate the DHE secret.
 * @param  dhe_secret_size                The size in bytes of the DHE secret.
 *
 * @retval RETURN_SUCCESS  DHE Secret is imported.
 */
bool
libspdm_secured_message_import_dhe_secret(void *spdm_secured_message_context,
                                          const void *dhe_secret,
                                          size_t dhe_secret_size);

/**
 * Export the Export Master Secret from an SPDM secured message context.
 *
 * The size of the Export Master Secret is the size of the digest of the negotiated hash algorithm.
 * If the size of the destination buffer is less than the size of the Export Master Secret then
 * the first export_master_secret_size bytes are copied.
 *
 * @param  spdm_secured_message_context  A pointer to the SPDM secured message context.
 * @param  export_master_secret          A pointer to the buffer to store the export_master_secret.
 * @param  export_master_secret_size     On input, the size of the destination buffer.
 *                                       On output, the lesser of either the size of the destination
 *                                       buffer or the size of the Export Master Secret.
 */
bool libspdm_secured_message_export_master_secret(
    void *spdm_secured_message_context, void *export_master_secret,
    size_t *export_master_secret_size);

/**
 * Erase the Export Master Secret from an SPDM secured message context. This is typically called
 * after libspdm_secured_message_export_master_secret().
 *
 * @param  spdm_secured_message_context  A pointer to the SPDM secured message context.
 */
void libspdm_secured_message_clear_export_master_secret(void *spdm_secured_message_context);

#define LIBSPDM_SECURE_SESSION_KEYS_STRUCT_VERSION 1

#pragma pack(1)
typedef struct {
    uint32_t version;
    uint32_t aead_key_size;
    uint32_t aead_iv_size;
    /*  uint8_t                request_data_encryption_key[aead_key_size];
     *  uint8_t                request_data_salt[aead_iv_size];
     *  uint64_t               request_data_sequence_number;
     *  uint8_t                response_data_encryption_key[aead_key_size];
     *  uint8_t                response_data_salt[aead_iv_size];
     *  uint64_t               response_data_sequence_number;*/
} libspdm_secure_session_keys_struct_t;
#pragma pack()

/**
 * Export the session_keys from an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_keys                  Indicate the buffer to store the session_keys in libspdm_secure_session_keys_struct_t.
 * @param  session_keys_size              The size in bytes of the session_keys in libspdm_secure_session_keys_struct_t.
 *
 * @retval RETURN_SUCCESS  session_keys are exported.
 */
bool libspdm_secured_message_export_session_keys(void *spdm_secured_message_context,
                                                 void *session_keys,
                                                 size_t *session_keys_size);

/**
 * Import the session_keys from an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_keys                  Indicate the buffer to store the session_keys in libspdm_secure_session_keys_struct_t.
 * @param  session_keys_size              The size in bytes of the session_keys in libspdm_secure_session_keys_struct_t.
 *
 * @retval RETURN_SUCCESS  session_keys are imported.
 */
bool libspdm_secured_message_import_session_keys(void *spdm_secured_message_context,
                                                 const void *session_keys,
                                                 size_t session_keys_size);

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
 * This function is used to clear handshake secret.
 *
 * @param spdm_secured_message_context  A pointer to the SPDM secured message context.
 **/
void libspdm_clear_handshake_secret(void *spdm_secured_message_context);

/**
 * This function is used to clear the master secret;
 *
 * @param spdm_secured_message_context  A pointer to the SPDM secured message context.
 **/
void libspdm_clear_master_secret(void *spdm_secured_message_context);

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
                        size_t *out_bin_size);

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

typedef enum {
    LIBSPDM_KEY_UPDATE_OPERATION_CREATE_UPDATE,
    LIBSPDM_KEY_UPDATE_OPERATION_COMMIT_UPDATE,
    LIBSPDM_KEY_UPDATE_OPERATION_DISCARD_UPDATE,
    LIBSPDM_KEY_UPDATE_OPERATION_MAX
} libspdm_key_update_operation_t;

typedef enum {
    LIBSPDM_KEY_UPDATE_ACTION_REQUESTER,
    LIBSPDM_KEY_UPDATE_ACTION_RESPONDER,
    LIBSPDM_KEY_UPDATE_ACTION_MAX
} libspdm_key_update_action_t;

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

/**
 * Get sequence number in an SPDM secure message.
 *
 * This value is transport layer specific.
 *
 * @param sequence_number        The current sequence number used to encode or decode message.
 * @param sequence_number_buffer  A buffer to hold the sequence number output used in the secured message.
 *                             The size in byte of the output buffer shall be 8.
 *
 * @return size in byte of the sequence_number_buffer.
 *        It shall be no greater than 8.
 *        0 means no sequence number is required.
 **/
typedef uint8_t (*libspdm_secured_message_get_sequence_number_func)(
    uint64_t sequence_number, uint8_t *sequence_number_buffer);

/**
 * Return max random number count in an SPDM secure message.
 *
 * This value is transport layer specific.
 *
 * @return Max random number count in an SPDM secured message.
 *        0 means no randum number is required.
 **/
typedef uint32_t (*libspdm_secured_message_get_max_random_number_count_func)(void);

#define SPDM_SECURED_MESSAGE_CALLBACKS_VERSION 1

typedef struct {
    uint32_t version;
    libspdm_secured_message_get_sequence_number_func get_sequence_number;
    libspdm_secured_message_get_max_random_number_count_func get_max_random_number_count;
} libspdm_secured_message_callbacks_t;

typedef struct {
    uint8_t error_code;
    uint32_t session_id;
} libspdm_error_struct_t;

/**
 * Encode an application message to a secured message.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_id                    The session ID of the SPDM session.
 * @param  is_requester                  Indicates if it is a requester message.
 * @param  app_message_size               size in bytes of the application message data buffer.
 * @param  app_message                   A pointer to a source buffer to store the application message.
 *                                       It shall point to the scratch buffer in spdm_context.
 *                                       On input, the app_message pointer shall point to a big enough buffer.
 *                                         Before app_message, there is room for spdm_secured_message_cipher_header_t.
 *                                         After (app_message + app_message_size), there is room for random bytes.
 * @param  secured_message_size           size in bytes of the secured message data buffer.
 * @param  secured_message               A pointer to a destination buffer to store the secured message.
 *                                       It shall point to the acquired sender buffer.
 * @param  spdm_secured_message_callbacks  A pointer to a secured message callback functions structure.
 *
 * @retval RETURN_SUCCESS               The application message is encoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 **/
libspdm_return_t libspdm_encode_secured_message(
    void *spdm_secured_message_context, uint32_t session_id,
    bool is_requester, size_t app_message_size,
    void *app_message, size_t *secured_message_size,
    void *secured_message,
    const libspdm_secured_message_callbacks_t *spdm_secured_message_callbacks);

/**
 * Decode an application message from a secured message.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_id                    The session ID of the SPDM session.
 * @param  is_requester                  Indicates if it is a requester message.
 * @param  secured_message_size           size in bytes of the secured message data buffer.
 * @param  secured_message               A pointer to a source buffer to store the secured message.
 *                                       It shall point to the acquired receiver buffer.
 * @param  app_message_size               size in bytes of the application message data buffer.
 * @param  app_message                   A pointer to a destination buffer to store the application message.
 *                                       It shall point to the scratch buffer in spdm_context.
 *                                       On input, the app_message pointer shall point to a big enough buffer to hold the decrypted message
 *                                       On output, the app_message pointer shall be inside of [app_message, app_message + app_message_size]
 * @param  spdm_secured_message_callbacks  A pointer to a secured message callback functions structure.
 *
 * @retval RETURN_SUCCESS               The application message is decoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 * @retval RETURN_UNSUPPORTED           The secured_message is unsupported.
 **/
libspdm_return_t libspdm_decode_secured_message(
    void *spdm_secured_message_context, uint32_t session_id,
    bool is_requester, size_t secured_message_size,
    void *secured_message, size_t *app_message_size,
    void **app_message,
    const libspdm_secured_message_callbacks_t *spdm_secured_message_callbacks);

/**
 * Get the last SPDM error struct of an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM secured message context.
 */
void libspdm_secured_message_get_last_spdm_error_struct(
    void *spdm_secured_message_context,
    libspdm_error_struct_t *last_spdm_error);

/**
 * Set the last SPDM error struct of an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  last_spdm_error                Last SPDM error struct of an SPDM secured message context.
 */
void libspdm_secured_message_set_last_spdm_error_struct(
    void *spdm_secured_message_context,
    const libspdm_error_struct_t *last_spdm_error);

#endif /* SPDM_SECURED_MESSAGE_LIB_H */
