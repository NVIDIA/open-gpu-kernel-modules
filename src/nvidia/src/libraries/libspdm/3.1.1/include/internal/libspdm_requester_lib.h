/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_REQUESTER_LIB_INTERNAL_H
#define SPDM_REQUESTER_LIB_INTERNAL_H

#include "library/spdm_requester_lib.h"
#include "library/spdm_secured_message_lib.h"
#include "internal/libspdm_common_lib.h"
#include "hal/library/requester/timelib.h"

/**
 * Send an SPDM or an APP request to a device.
 *
 * @param  spdm_context    The SPDM context for the device.
 * @param  session_id      Indicate if the request is a secured message.
 *                         If session_id is NULL, it is a normal message.
 *                         If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message  Indicates if it is an APP message or SPDM message.
 * @param  request_size    Size in bytes of the request data buffer.
 * @param  request         A pointer to a destination buffer to store the request.
 *                         The caller is responsible for having either implicit or explicit ownership
 *                         of the buffer.
 *                         For normal message, requester pointer point to transport_message + transport header size
 *                         For secured message, requester pointer will point to the scratch buffer + transport header size in spdm_context.
 **/
libspdm_return_t libspdm_send_request(void *spdm_context, const uint32_t *session_id,
                                      bool is_app_message,
                                      size_t request_size, void *request);

/**
 * Receive an SPDM or an APP response from a device.
 *
 * @param  spdm_context    The SPDM context for the device.
 * @param  session_id      Indicate if the response is a secured message.
 *                         If session_id is NULL, it is a normal message.
 *                         If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message  Indicates if it is an APP message or SPDM message.
 * @param  response_size   Size in bytes of the response data buffer.
 * @param  response        A pointer to a destination buffer to store the response.
 *                         The caller is responsible for having either implicit or explicit
 *                         ownership of the buffer.
 *                         For normal message, response pointer still point to original transport_message.
 *                         For secured message, response pointer will point to the scratch buffer in spdm_context.
 **/
libspdm_return_t libspdm_receive_response(void *spdm_context, const uint32_t *session_id,
                                          bool is_app_message,
                                          size_t *response_size, void **response);

/**
 * This function handles simple error code.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  error_code                    Indicate the error code.
 *
 * @retval RETURN_NO_RESPONSE           If the error code is BUSY.
 * @retval RETURN_DEVICE_ERROR          If the error code is REQUEST_RESYNCH or others.
 **/
libspdm_return_t libspdm_handle_simple_error_response(libspdm_context_t *spdm_context,
                                                      uint8_t error_code);

/**
 * This function handles the error response.
 *
 * The SPDM response code must be SPDM_ERROR.
 * For error code RESPONSE_NOT_READY, this function sends RESPOND_IF_READY and receives an expected SPDM response.
 * For error code BUSY, this function shrinks the managed buffer, and return RETURN_NO_RESPONSE.
 * For error code REQUEST_RESYNCH, this function shrinks the managed buffer, clears connection_state, and return RETURN_DEVICE_ERROR.
 * For error code DECRYPT_ERROR, end the session: free session id and session key, return RETURN_SECURITY_VIOLATION.
 * For any other error code, this function shrinks the managed buffer, and return RETURN_DEVICE_ERROR.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicates if it is a secured message protected via SPDM session.
 *                                       If session_id is NULL, it is a normal message.
 *                                       If session_id is NOT NULL, it is a secured message.
 * @param  response_size                 The size of the response.
 *                                     On input, it means the size in bytes of response data buffer.
 *                                     On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned.
 * @param  response                     The SPDM response message.
 * @param  original_request_code          Indicate the original request code.
 * @param  expected_response_code         Indicate the expected response code.
 *
 * @retval RETURN_SUCCESS               The error code is RESPONSE_NOT_READY. The RESPOND_IF_READY is sent and an expected SPDM response is received.
 * @retval RETURN_NO_RESPONSE           The error code is BUSY.
 * @retval RETURN_DEVICE_ERROR          The error code is REQUEST_RESYNCH or others.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    The error code is DECRYPT_ERROR and session_id is NOT NULL.
 **/
libspdm_return_t libspdm_handle_error_response_main(
    libspdm_context_t *spdm_context, const uint32_t *session_id,
    size_t *response_size, void **response,
    uint8_t original_request_code, uint8_t expected_response_code);

/**
 * This function handles the error response handling for large responses.
 * Will retrieve the large response in chunks if supported and necessary.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicates if it is a secured message protected via SPDM session.
 *                                       If session_id is NULL, it is a normal message.
 *                                       If session_id is NOT NULL, it is a secured message.
 * @param  inout_response_size           The size of the response.
 *                                       On input, expected to be the size of an error response.
 *                                       On output, the large response size after being retrieved in chunks.
 * @param  response                      The error response on input. Large response on output.
 * @param  response_capacity             The maximum capacity of the response buffer.
 *
 * @retval libspdm_return_t              An error value or success.
 **/
libspdm_return_t libspdm_handle_error_large_response(
    libspdm_context_t* spdm_context,
    const uint32_t* session_id,
    size_t* inout_response_size,
    void* inout_response,
    size_t response_capacity);

/**
 * This function sends GET_VERSION and receives VERSION.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The GET_VERSION is sent and the VERSION is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_get_version(libspdm_context_t *spdm_context,
                                     uint8_t *version_number_entry_count,
                                     spdm_version_number_t *version_number_entry);

/**
 * This function sends GET_CAPABILITIES and receives CAPABILITIES.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  RequesterCTExponent          RequesterCTExponent to the GET_CAPABILITIES request.
 * @param  RequesterFlags               RequesterFlags to the GET_CAPABILITIES request.
 * @param  ResponderCTExponent          ResponderCTExponent from the CAPABILITIES response.
 * @param  ResponderFlags               ResponderFlags from the CAPABILITIES response.
 *
 * @retval RETURN_SUCCESS               The GET_CAPABILITIES is sent and the CAPABILITIES is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_get_capabilities(libspdm_context_t *spdm_context);

/**
 * This function sends NEGOTIATE_ALGORITHMS and receives ALGORITHMS.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The NEGOTIATE_ALGORITHMS is sent and the ALGORITHMS is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_negotiate_algorithms(libspdm_context_t *spdm_context);

/**
 * This function sends GET_CERTIFICATE to get certificate chain in one slot from device.
 *
 * This function verify the integrity of the certificate chain.
 * root_hash -> Root certificate -> Intermediate certificate -> Leaf certificate.
 *
 * If the peer root certificate hash is deployed,
 * this function also verifies the digest with the root hash in the certificate chain.
 *
 * @param  spdm_context     A pointer to the SPDM context.
 * @param  session_id       Indicates if it is a secured message protected via SPDM session.
 *                          If session_id is NULL, it is a normal message.
 * @param  slot_id          The number of slot for the certificate chain.
 * @param  length           LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN.
 * @param  cert_chain_size  On input, indicate the size in bytes of the destination buffer to store the digest buffer.
 *                          On output, indicate the size in bytes of the certificate chain.
 * @param  cert_chain       A pointer to a destination buffer to store the certificate chain.
 **/
libspdm_return_t libspdm_get_certificate_choose_length(void *spdm_context,
                                                       const uint32_t *session_id,
                                                       uint8_t slot_id,
                                                       uint16_t length,
                                                       size_t *cert_chain_size,
                                                       void *cert_chain);

/**
 * This function sends GET_CERTIFICATE to get certificate chain in one slot from device.
 *
 * This function verify the integrity of the certificate chain.
 * root_hash -> Root certificate -> Intermediate certificate -> Leaf certificate.
 *
 * If the peer root certificate hash is deployed,
 * this function also verifies the digest with the root hash in the certificate chain.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  session_id         Indicates if it is a secured message protected via SPDM session.
 *                            If session_id is NULL, it is a normal message.
 * @param  slot_id            The number of slot for the certificate chain.
 * @param  length             LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN.
 * @param  cert_chain_size    On input, indicate the size in bytes of the destination buffer to store the digest buffer.
 *                            On output, indicate the size in bytes of the certificate chain.
 * @param  cert_chain         A pointer to a destination buffer to store the certificate chain.
 * @param  trust_anchor       A buffer to hold the trust_anchor which is used to validate the peer certificate, if not NULL.
 * @param  trust_anchor_size  A buffer to hold the trust_anchor_size, if not NULL.
 **/
libspdm_return_t libspdm_get_certificate_choose_length_ex(void *spdm_context,
                                                          const uint32_t *session_id,
                                                          uint8_t slot_id,
                                                          uint16_t length,
                                                          size_t *cert_chain_size,
                                                          void *cert_chain,
                                                          const void **trust_anchor,
                                                          size_t *trust_anchor_size);

#if LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP
/**
 * This function sends KEY_EXCHANGE and receives KEY_EXCHANGE_RSP for SPDM key exchange.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  measurement_hash_type          measurement_hash_type to the KEY_EXCHANGE request.
 * @param  slot_id                      slot_id to the KEY_EXCHANGE request.
 * @param  session_policy               The policy for the session.
 * @param  session_id                    session_id from the KEY_EXCHANGE_RSP response.
 * @param  heartbeat_period              heartbeat_period from the KEY_EXCHANGE_RSP response.
 * @param  req_slot_id_param               req_slot_id_param from the KEY_EXCHANGE_RSP response.
 * @param  measurement_hash              measurement_hash from the KEY_EXCHANGE_RSP response.
 *
 * @retval RETURN_SUCCESS               The KEY_EXCHANGE is sent and the KEY_EXCHANGE_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_key_exchange(
    libspdm_context_t *spdm_context, uint8_t measurement_hash_type,
    uint8_t slot_id, uint8_t session_policy, uint32_t *session_id,
    uint8_t *heartbeat_period,
    uint8_t *req_slot_id_param, void *measurement_hash);

/**
 * This function sends KEY_EXCHANGE and receives KEY_EXCHANGE_RSP for SPDM key exchange.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  measurement_hash_type          measurement_hash_type to the KEY_EXCHANGE request.
 * @param  slot_id                      slot_id to the KEY_EXCHANGE request.
 * @param  session_policy               The policy for the session.
 * @param  session_id                    session_id from the KEY_EXCHANGE_RSP response.
 * @param  heartbeat_period              heartbeat_period from the KEY_EXCHANGE_RSP response.
 * @param  req_slot_id_param               req_slot_id_param from the KEY_EXCHANGE_RSP response.
 * @param  measurement_hash              measurement_hash from the KEY_EXCHANGE_RSP response.
 * @param  requester_random_in           A buffer to hold the requester random (32 bytes) as input, if not NULL.
 * @param  requester_random              A buffer to hold the requester random (32 bytes), if not NULL.
 * @param  responder_random              A buffer to hold the responder random (32 bytes), if not NULL.
 *
 * @retval RETURN_SUCCESS               The KEY_EXCHANGE is sent and the KEY_EXCHANGE_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_key_exchange_ex(
    libspdm_context_t *spdm_context, uint8_t measurement_hash_type,
    uint8_t slot_id, uint8_t session_policy, uint32_t *session_id,
    uint8_t *heartbeat_period,
    uint8_t *req_slot_id_param, void *measurement_hash,
    const void *requester_random_in,
    void *requester_random, void *responder_random,
    const void *requester_opaque_data, size_t requester_opaque_data_size,
    void *responder_opaque_data, size_t *responder_opaque_data_size);

/**
 * This function sends FINISH and receives FINISH_RSP for SPDM finish.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    session_id to the FINISH request.
 * @param  req_slot_id_param               req_slot_id_param to the FINISH request.
 *
 * @retval RETURN_SUCCESS               The FINISH is sent and the FINISH_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_finish(libspdm_context_t *spdm_context,
                                             uint32_t session_id,
                                             uint8_t req_slot_id_param);
#endif /* LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_PSK_CAP
/**
 * This function sends PSK_EXCHANGE and receives PSK_EXCHANGE_RSP for SPDM PSK exchange.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  measurement_hash_type          measurement_hash_type to the PSK_EXCHANGE request.
 * @param  session_policy               The policy for the session.
 * @param  session_id                    session_id from the PSK_EXCHANGE_RSP response.
 * @param  heartbeat_period              heartbeat_period from the PSK_EXCHANGE_RSP response.
 * @param  measurement_hash              measurement_hash from the PSK_EXCHANGE_RSP response.
 *
 * @retval RETURN_SUCCESS               The PSK_EXCHANGE is sent and the PSK_EXCHANGE_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_psk_exchange(libspdm_context_t *spdm_context,
                                                   const void *psk_hint,
                                                   uint16_t psk_hint_size,
                                                   uint8_t measurement_hash_type,
                                                   uint8_t session_policy,
                                                   uint32_t *session_id,
                                                   uint8_t *heartbeat_period,
                                                   void *measurement_hash);

/**
 * This function sends PSK_EXCHANGE and receives PSK_EXCHANGE_RSP for SPDM PSK exchange.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  measurement_hash_type          measurement_hash_type to the PSK_EXCHANGE request.
 * @param  session_policy               The policy for the session.
 * @param  session_id                    session_id from the PSK_EXCHANGE_RSP response.
 * @param  heartbeat_period              heartbeat_period from the PSK_EXCHANGE_RSP response.
 * @param  measurement_hash              measurement_hash from the PSK_EXCHANGE_RSP response.
 * @param  requester_context_in          A buffer to hold the requester context as input, if not NULL.
 * @param  requester_context_in_size     The size of requester_context_in.
 *                                      It must be 32 bytes at least, but not exceed LIBSPDM_PSK_CONTEXT_LENGTH.
 * @param  requester_context             A buffer to hold the requester context, if not NULL.
 * @param  requester_context_size        On input, the size of requester_context buffer.
 *                                      On output, the size of data returned in requester_context buffer.
 *                                      It must be 32 bytes at least.
 * @param  responder_context             A buffer to hold the responder context, if not NULL.
 * @param  responder_context_size        On input, the size of requester_context buffer.
 *                                      On output, the size of data returned in requester_context buffer.
 *                                      It could be 0 if device does not support context.
 **/
libspdm_return_t libspdm_send_receive_psk_exchange_ex(libspdm_context_t *spdm_context,
                                                      const void *psk_hint,
                                                      uint16_t psk_hint_size,
                                                      uint8_t measurement_hash_type,
                                                      uint8_t session_policy,
                                                      uint32_t *session_id,
                                                      uint8_t *heartbeat_period,
                                                      void *measurement_hash,
                                                      const void *requester_context_in,
                                                      size_t requester_context_in_size,
                                                      void *requester_context,
                                                      size_t *requester_context_size,
                                                      void *responder_context,
                                                      size_t *responder_context_size,
                                                      const void *requester_opaque_data,
                                                      size_t requester_opaque_data_size,
                                                      void *responder_opaque_data,
                                                      size_t *responder_opaque_data_size);

/**
 * This function sends PSK_FINISH and receives PSK_FINISH_RSP for SPDM PSK finish.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    session_id to the PSK_FINISH request.
 *
 * @retval RETURN_SUCCESS               The PSK_FINISH is sent and the PSK_FINISH_RSP is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_psk_finish(libspdm_context_t *spdm_context,
                                                 uint32_t session_id);
#endif /* LIBSPDM_ENABLE_CAPABILITY_PSK_CAP */

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) || (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP)
/**
 * This function sends END_SESSION and receives END_SESSION_ACK for SPDM session end.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    session_id to the END_SESSION request.
 * @param  end_session_attributes         end_session_attributes to the END_SESSION_ACK request.
 *
 * @retval RETURN_SUCCESS               The END_SESSION is sent and the END_SESSION_ACK is received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_end_session(libspdm_context_t *spdm_context,
                                                  uint32_t session_id,
                                                  uint8_t end_session_attributes);
#endif /* (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) || (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP) */

#if LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP
/**
 * This function executes a series of SPDM encapsulated requests and receives SPDM encapsulated responses.
 *
 * This function starts with the first encapsulated request (such as GET_ENCAPSULATED_REQUEST)
 * and ends with last encapsulated response (such as RESPONSE_PAYLOAD_TYPE_ABSENT or RESPONSE_PAYLOAD_TYPE_SLOT_NUMBER).
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicate if the encapsulated request is a secured message.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  mut_auth_requested             Indicate of the mut_auth_requested through KEY_EXCHANGE or CHALLENG response.
 * @param  req_slot_id_param               req_slot_id_param from the RESPONSE_PAYLOAD_TYPE_REQ_SLOT_NUMBER.
 *
 * @retval RETURN_SUCCESS               The SPDM Encapsulated requests are sent and the responses are received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_encapsulated_request(libspdm_context_t *spdm_context,
                                              const uint32_t *session_id,
                                              uint8_t mut_auth_requested,
                                              uint8_t *req_slot_id_param);

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
#if LIBSPDM_ENABLE_CAPABILITY_CERT_CAP
/**
 * Process the SPDM encapsulated GET_DIGESTS request and return the response.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
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
 **/
libspdm_return_t libspdm_get_encap_response_digest(void *spdm_context,
                                                   size_t request_size,
                                                   void *request,
                                                   size_t *response_size,
                                                   void *response);

/**
 * Process the SPDM encapsulated GET_CERTIFICATE request and return the response.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
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
 **/
libspdm_return_t libspdm_get_encap_response_certificate(void *spdm_context,
                                                        size_t request_size,
                                                        void *request,
                                                        size_t *response_size,
                                                        void *response);
#endif /* LIBSPDM_ENABLE_CAPABILITY_CERT_CAP */

#if LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP
/**
 * Process the SPDM encapsulated CHALLENGE request and return the response.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
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
 **/
libspdm_return_t libspdm_get_encap_response_challenge_auth(
    void *spdm_context, size_t request_size, void *request,
    size_t *response_size, void *response);
#endif /* LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP */
#endif /* LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP */

/**
 * Process the SPDM encapsulated KEY_UPDATE request and return the response.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
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
 **/
libspdm_return_t libspdm_get_encap_response_key_update(void *spdm_context,
                                                       size_t request_size,
                                                       void *request,
                                                       size_t *response_size,
                                                       void *response);
#endif /* LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP */

/**
 * Send an SPDM request to a device.
 *
 * @param  spdm_context                  The SPDM context for the device.
 * @param  session_id                    Indicate if the request is a secured message.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  request_size                  size in bytes of the request data buffer.
 * @param  request                      A pointer to a destination buffer to store the request.
 *                                     The caller is responsible for having
 *                                     either implicit or explicit ownership of the buffer.
 *                                      For normal message, requester pointer point to transport_message + transport header size
 *                                      For secured message, requester pointer will point to the scratch buffer + transport header size in spdm_context.
 *
 * @retval RETURN_SUCCESS               The SPDM request is sent successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM request is sent to the device.
 **/
libspdm_return_t libspdm_send_spdm_request(libspdm_context_t *spdm_context,
                                           const uint32_t *session_id,
                                           size_t request_size, void *request);

/**
 * Receive an SPDM response from a device.
 *
 * @param  spdm_context                  The SPDM context for the device.
 * @param  session_id                    Indicate if the response is a secured message.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 * @param  response_size                 size in bytes of the response data buffer.
 * @param  response                     A pointer to a destination buffer to store the response.
 *                                     The caller is responsible for having
 *                                     either implicit or explicit ownership of the buffer.
 *                                      For normal message, response pointer still point to original transport_message.
 *                                      For secured message, response pointer will point to the scratch buffer in spdm_context.
 *
 * @retval RETURN_SUCCESS               The SPDM response is received successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM response is received from the device.
 **/
libspdm_return_t libspdm_receive_spdm_response(libspdm_context_t *spdm_context,
                                               const uint32_t *session_id,
                                               size_t *response_size,
                                               void **response);

/**
 * This function allocates half of session ID for a requester.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @return half of session ID for a requester.
 **/
uint16_t libspdm_allocate_req_session_id(libspdm_context_t *spdm_context, bool use_psk);

/**
 * Build opaque data supported version.
 *
 * This function should be called in KEY_EXCHANGE/PSK_EXCHANGE request generation.
 *
 * @param  data_out_size  Size in bytes of the data_out.
 *                        On input, it means the size in bytes of data_out buffer.
 *                        On output, it means the size in bytes of copied data_out buffer.
 * @param  data_out       A pointer to the destination buffer to store the opaque data supported version.
 **/
void libspdm_build_opaque_data_supported_version_data(libspdm_context_t *spdm_context,
                                                      size_t *data_out_size,
                                                      void *data_out);

/**
 * Process opaque data version selection.
 *
 * This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response parsing in requester.
 *
 * @param  data_in_size  Size in bytes of the data_in.
 * @param  data_in       A pointer to the buffer to store the opaque data version selection.
 **/
libspdm_return_t libspdm_process_opaque_data_version_selection_data(libspdm_context_t *spdm_context,
                                                                    size_t data_in_size,
                                                                    void *data_in);

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
/**
 * This function generates the finish signature based upon TH.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_info  The session info of an SPDM session.
 * @param  signature     The buffer to store the finish signature.
 *
 * @retval true  finish signature is generated.
 * @retval false finish signature is not generated.
 **/
bool libspdm_generate_finish_req_signature(libspdm_context_t *spdm_context,
                                           libspdm_session_info_t *session_info,
                                           uint8_t *signature);
#endif

/**
 * This function generates the finish HMAC based upon TH.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The session info of an SPDM session.
 * @param  hmac                         The buffer to store the finish HMAC.
 *
 * @retval true  finish HMAC is generated.
 * @retval false finish HMAC is not generated.
 **/
bool libspdm_generate_finish_req_hmac(libspdm_context_t *spdm_context,
                                      libspdm_session_info_t *session_info,
                                      void *hmac);

/**
 * This function verifies the finish HMAC based upon TH.
 *
 * @param  spdm_context    A pointer to the SPDM context.
 * @param  session_info    The session info of an SPDM session.
 * @param  hmac_data       The HMAC data buffer.
 * @param  hmac_data_size  Size in bytes of the HMAC data buffer.
 *
 * @retval true  HMAC verification pass.
 * @retval false HMAC verification fail.
 **/
bool libspdm_verify_finish_rsp_hmac(libspdm_context_t *spdm_context,
                                    libspdm_session_info_t *session_info,
                                    const void *hmac_data, size_t hmac_data_size);

/**
 * This function verifies the key exchange HMAC based upon TH.
 *
 * @param  spdm_context    A pointer to the SPDM context.
 * @param  session_info    The session info of an SPDM session.
 * @param  hmac_data       The HMAC data buffer.
 * @param  hmac_data_size  Size in bytes of the HMAC data buffer.
 *
 * @retval true  HMAC verification pass.
 * @retval false HMAC verification fail.
 **/
bool libspdm_verify_key_exchange_rsp_hmac(libspdm_context_t *spdm_context,
                                          libspdm_session_info_t *session_info,
                                          const void *hmac_data,
                                          size_t hmac_data_size);

/**
 * This function verifies the key exchange signature based upon TH.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  The session info of an SPDM session.
 * @param  sign_data                     The signature data buffer.
 * @param  sign_data_size                 size in bytes of the signature data buffer.
 *
 * @retval true  signature verification pass.
 * @retval false signature verification fail.
 **/
bool libspdm_verify_key_exchange_rsp_signature(
    libspdm_context_t *spdm_context, libspdm_session_info_t *session_info,
    const void *sign_data, const size_t sign_data_size);

/**
 * This function verifies the measurement signature based upon l1l2.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_info                  A pointer to the SPDM session context.
 * @param  sign_data                     The signature data buffer.
 * @param  sign_data_size                 size in bytes of the signature data buffer.
 *
 * @retval true  signature verification pass.
 * @retval false signature verification fail.
 **/
bool libspdm_verify_measurement_signature(libspdm_context_t *spdm_context,
                                          libspdm_session_info_t *session_info,
                                          const void *sign_data,
                                          size_t sign_data_size);

#endif /* SPDM_REQUESTER_LIB_INTERNAL_H */
