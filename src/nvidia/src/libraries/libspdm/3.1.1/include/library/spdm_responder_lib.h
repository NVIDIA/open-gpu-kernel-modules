/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_RESPONDER_LIB_H
#define SPDM_RESPONDER_LIB_H

#include "library/spdm_common_lib.h"
#include "library/spdm_secured_message_lib.h"

/**
 * Process the SPDM or APP request and return the response.
 *
 * The APP message is encoded to a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicates if it is a secured message protected via SPDM session.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message                 Indicates if it is an APP message or SPDM message.
 * @param  request_size                  size in bytes of the request data.
 * @param  request                      A pointer to the request data.
 * @param  response_size                 size in bytes of the response data.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
 *                                     and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  response                     A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The request is processed and the response is returned.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 * @retval RETURN_UNSUPPORTED           Just ignore this message: return UNSUPPORTED and clear response_size.
 *                                      Continue the dispatch without send response.
 **/
typedef libspdm_return_t (*libspdm_get_response_func)(
    void *spdm_context, const uint32_t *session_id, bool is_app_message,
    size_t request_size, const void *request, size_t *response_size,
    void *response);

/**
 * Register an SPDM or APP message process function.
 *
 * If the default message process function cannot handle the message,
 * this function will be invoked.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  get_response_func              The function to process the encapsuled message.
 **/
void libspdm_register_get_response_func(
    void *spdm_context, libspdm_get_response_func get_response_func);

/**
 * Process a SPDM request from a device.
 *
 * @param  spdm_context                  The SPDM context for the device.
 * @param  session_id                    Indicate if the request is a secured message.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message                 Indicates if it is an APP message or SPDM message.
 * @param  request_size                  size in bytes of the request data buffer.
 * @param  request                      A pointer to a destination buffer to store the request.
 *                                     The caller is responsible for having
 *                                     either implicit or explicit ownership of the buffer.
 *
 * @retval RETURN_SUCCESS               The SPDM request is received successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM request is received from the device.
 **/
libspdm_return_t libspdm_process_request(void *spdm_context,
                                         uint32_t **session_id,
                                         bool *is_app_message,
                                         size_t request_size, void *request);

/**
 * Build a SPDM response to a device.
 *
 * @param  spdm_context                  The SPDM context for the device.
 * @param  session_id                    Indicate if the response is a secured message.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message                 Indicates if it is an APP message or SPDM message.
 * @param  response_size                 size in bytes of the response data buffer.
 * @param  response                     A pointer to a destination buffer to store the response.
 *                                     The caller is responsible for having
 *                                     either implicit or explicit ownership of the buffer.
 *
 * @retval RETURN_SUCCESS               The SPDM response is sent successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM response is sent to the device.
 * @retval RETURN_UNSUPPORTED           Just ignore this message: return UNSUPPORTED and clear response_size.
 *                                      Continue the dispatch without send response.
 **/
libspdm_return_t libspdm_build_response(void *spdm_context, const uint32_t *session_id,
                                        bool is_app_message,
                                        size_t *response_size,
                                        void **response);

/**
 * This is the main dispatch function in SPDM responder.
 *
 * It receives one request message, processes it and sends the response message.
 *
 * It should be called in a while loop or an timer/interrupt handler.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               One SPDM request message is processed.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_UNSUPPORTED           One request message is not supported.
 **/
libspdm_return_t libspdm_responder_dispatch_message(void *spdm_context);

/**
 * Generate ERROR message.
 *
 * This function can be called in libspdm_get_response_func.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  error_code                    The error code of the message.
 * @param  error_data                    The error data of the message.
 * @param  spdm_response_size             size in bytes of the response data.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
 *                                     and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  spdm_response                 A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The error message is generated.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_generate_error_response(const void *spdm_context,
                                                 uint8_t error_code,
                                                 uint8_t error_data,
                                                 size_t *spdm_response_size,
                                                 void *spdm_response);

/**
 * Generate ERROR message with extended error data.
 *
 * This function can be called in libspdm_get_response_func.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  error_code                    The error code of the message.
 * @param  error_data                    The error data of the message.
 * @param  extended_error_data_size        The size in bytes of the extended error data.
 * @param  extended_error_data            A pointer to the extended error data.
 * @param  spdm_response_size             size in bytes of the response data.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
 *                                     and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  spdm_response                 A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The error message is generated.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_generate_extended_error_response(
    const void *spdm_context, uint8_t error_code, uint8_t error_data,
    size_t extended_error_data_size, const uint8_t *extended_error_data,
    size_t *spdm_response_size, void *spdm_response);

/**
 * Notify the session state to a session APP.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    The session_id of a session.
 * @param  session_state                 The state of a session.
 **/
typedef void (*libspdm_session_state_callback_func)(
    void *spdm_context, uint32_t session_id,
    libspdm_session_state_t session_state);

/**
 * Register an SPDM state callback function.
 *
 * This function can be called multiple times to let different session APPs register its own callback.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_session_state_callback     The function to be called in SPDM session state change.
 **/
void libspdm_register_session_state_callback_func(
    void *spdm_context,
    libspdm_session_state_callback_func spdm_session_state_callback);

/**
 * Notify the connection state to an SPDM context register.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  connection_state              Indicate the SPDM connection state.
 **/
typedef void (*libspdm_connection_state_callback_func)(
    void *spdm_context, libspdm_connection_state_t connection_state);

/**
 * Register an SPDM connection state callback function.
 *
 * This function can be called multiple times to let different register its own callback.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  spdm_connection_state_callback  The function to be called in SPDM connection state change.
 **/
void libspdm_register_connection_state_callback_func(
    void *spdm_context,
    libspdm_connection_state_callback_func spdm_connection_state_callback);

/**
 * Notify the key update operation to an SPDM context register.
 *
 * @param  spdm_context           A pointer to the SPDM context.
 * @param  session_id             Session ID for the keys being updated.
 * @param  key_update_operation   Indicate the key update operation.
 * @param  key_update_action      Indicate the direction of the key update.
 **/
typedef void (*libspdm_key_update_callback_func)(
    void *spdm_context, uint32_t session_id, libspdm_key_update_operation_t key_update_op,
    libspdm_key_update_action_t key_update_action);

/**
 * Notify the key update operation to an SPDM context register.
 *
 * @param  spdm_context           A pointer to the SPDM context.
 * @param  session_id             Session ID for the keys being updated.
 * @param  key_update_operation   Indicate the key update operation.
 * @param  key_update_action      Indicate the direction of the key update.
 **/
void libspdm_trigger_key_update_callback(
    void *spdm_context, uint32_t session_id, libspdm_key_update_operation_t key_update_op,
    libspdm_key_update_action_t key_update_action);

/**
 * Register a key update callback function.
 *
 * This function can be called multiple times to register multiple callbacks.
 *
 * @param  spdm_context              A pointer to the SPDM context.
 * @param  spdm_key_update_callback  The function to be called in key update operation.
 **/
void libspdm_register_key_update_callback_func(
    void *spdm_context, libspdm_key_update_callback_func spdm_key_update_callback);

/**
 * This function initializes the key_update encapsulated state.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 **/
void libspdm_init_key_update_encap_state(void *spdm_context);

#if (LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP) && (LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP) && \
    (LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT)
/**
 * Register a buffer to store the Requester's certificate chain during mutual authentication.
 *
 * This is only required if the Requester is providing a certificate chain (Requester's CERT_CAP
 * is 1) during mutual authentication.
 *
 * @param spdm_context                A pointer to the SPDM context.
 * @param cert_chain_buffer           A pointer to a buffer to store the certificate chain.
 * @param cert_chain_buffer_max_size  The maximum size, in bytes, of cert_chain_buffer.
 */
void libspdm_register_cert_chain_buffer(
    void *spdm_context, void *cert_chain_buffer, size_t cert_chain_buffer_max_size);
#endif

#endif /* SPDM_RESPONDER_LIB_H */
