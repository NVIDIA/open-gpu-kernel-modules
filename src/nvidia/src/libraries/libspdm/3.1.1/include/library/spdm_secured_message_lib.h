/**
 *  Copyright Notice:
 *  Copyright 2021-2023 DMTF. All rights reserved.
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

    /* After send KEY_EXCHANGE, before send FINISH */
    LIBSPDM_SESSION_STATE_HANDSHAKING,

    /* After send FINISH, before END_SESSION */
    LIBSPDM_SESSION_STATE_ESTABLISHED,

    /* MAX */
    LIBSPDM_SESSION_STATE_MAX
} libspdm_session_state_t;

/* The InvalidSession error code was removed from the specification. This define was originally
 * in spdm.h and has moved here since it is used in transport decode functions to convey that
 * the session ID is not recognizable.
 */
#define SPDM_ERROR_CODE_INVALID_SESSION 0x02

/**
 * Return the size in bytes of a single SPDM secured message context.
 *
 * @return the size in bytes of a single SPDM secured message context.
 **/
size_t libspdm_secured_message_get_context_size(void);

/**
 * Return session_state of an SPDM secured message context.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @return the SPDM session state.
 */
libspdm_session_state_t libspdm_secured_message_get_session_state(
    void *spdm_secured_message_context);

/**
 * Import the DHE Secret to an SPDM secured message context.
 *
 * @param  spdm_secured_message_context  A pointer to the SPDM secured message context.
 * @param  dhe_secret                    Indicate the DHE secret.
 * @param  dhe_secret_size               The size, in bytes, of the DHE secret.
 */
bool libspdm_secured_message_import_dhe_secret(void *spdm_secured_message_context,
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
 * Erase the Export Master Secret from an SPDM secured message context.
 *
 * This is typically called after libspdm_secured_message_export_master_secret().
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
 * @param  spdm_secured_message_context  A pointer to the SPDM secured message context.
 * @param  session_keys                  Indicate the buffer to store the session_keys in
 *                                       libspdm_secure_session_keys_struct_t.
 * @param  session_keys_size             The size in bytes of the session_keys in
 *                                       libspdm_secure_session_keys_struct_t.
 */
bool libspdm_secured_message_export_session_keys(void *spdm_secured_message_context,
                                                 void *session_keys,
                                                 size_t *session_keys_size);

/**
 * Import the session_keys from an SPDM secured message context.
 *
 * @param  spdm_secured_message_context  A pointer to the SPDM secured message context.
 * @param  session_keys                  Indicate the buffer to store the session_keys in
 *                                       libspdm_secure_session_keys_struct_t.
 * @param  session_keys_size             The size in bytes of the session_keys in
 *                                       libspdm_secure_session_keys_struct_t.
 */
bool libspdm_secured_message_import_session_keys(void *spdm_secured_message_context,
                                                 const void *session_keys,
                                                 size_t session_keys_size);

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
 *        0 means no random number is required.
 **/
typedef uint32_t (*libspdm_secured_message_get_max_random_number_count_func)(void);

/**
 * This function translates the negotiated secured_message_version to a DSP0277 version.
 *
 * @param  secured_message_version  The version specified in binding specification and
 *                                  negotiated in KEY_EXCHANGE/KEY_EXCHANGE_RSP.
 *
 * @return The DSP0277 version specified in binding specification,
 *         which is bound to secured_message_version.
 */
typedef spdm_version_number_t (*libspdm_secured_message_get_secured_spdm_version)(
    spdm_version_number_t secured_message_version);

#define LIBSPDM_SECURED_MESSAGE_CALLBACKS_VERSION 2

typedef struct {
    uint32_t version;
    libspdm_secured_message_get_sequence_number_func get_sequence_number;
    libspdm_secured_message_get_max_random_number_count_func get_max_random_number_count;
    libspdm_secured_message_get_secured_spdm_version get_secured_spdm_version;
} libspdm_secured_message_callbacks_t;

typedef struct {
    uint8_t error_code;
    uint32_t session_id;
} libspdm_error_struct_t;

/**
 * Encode an application message to a secured message.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_id                      The session ID of the SPDM session.
 * @param  is_request_message              Indicates if it is a request message.
 * @param  app_message_size                size in bytes of the application message data buffer.
 * @param  app_message                     A pointer to a source buffer to store the application message.
 *                                         It shall point to the scratch buffer in spdm_context.
 *                                         On input, the app_message pointer shall point to a big enough buffer.
 *                                         Before app_message, there is room for spdm_secured_message_cipher_header_t.
 *                                         After (app_message + app_message_size), there is room for random bytes.
 * @param  secured_message_size            size in bytes of the secured message data buffer.
 * @param  secured_message                 A pointer to a destination buffer to store the secured message.
 *                                         It shall point to the acquired sender buffer.
 * @param  spdm_secured_message_callbacks  A pointer to a secured message callback functions structure.
 *
 * @retval RETURN_SUCCESS               The application message is encoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 **/
libspdm_return_t libspdm_encode_secured_message(
    void *spdm_secured_message_context, uint32_t session_id,
    bool is_request_message, size_t app_message_size,
    void *app_message, size_t *secured_message_size,
    void *secured_message,
    const libspdm_secured_message_callbacks_t *spdm_secured_message_callbacks);

/**
 * Decode an application message from a secured message.
 *
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 * @param  session_id                      The session ID of the SPDM session.
 * @param  is_request_message              Indicates if it is a request message.
 * @param  secured_message_size            size in bytes of the secured message data buffer.
 * @param  secured_message                 A pointer to a source buffer to store the secured message.
 *                                         It shall point to the acquired receiver buffer.
 * @param  app_message_size                size in bytes of the application message data buffer.
 * @param  app_message                     A pointer to a destination buffer to store the application message.
 *                                         It shall point to the scratch buffer in spdm_context.
 *                                         On input, the app_message pointer shall point to a big enough buffer to hold the decrypted message
 *                                         On output, the app_message pointer shall be inside of [app_message, app_message + app_message_size]
 * @param  spdm_secured_message_callbacks  A pointer to a secured message callback functions structure.
 *
 * @retval RETURN_SUCCESS               The application message is decoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 * @retval RETURN_UNSUPPORTED           The secured_message is unsupported.
 **/
libspdm_return_t libspdm_decode_secured_message(
    void *spdm_secured_message_context, uint32_t session_id,
    bool is_request_message, size_t secured_message_size,
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

#endif /* SPDM_SECURED_MESSAGE_LIB_H */
