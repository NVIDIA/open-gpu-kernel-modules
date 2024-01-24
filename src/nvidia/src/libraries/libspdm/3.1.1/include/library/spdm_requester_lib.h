/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_REQUESTER_LIB_H
#define SPDM_REQUESTER_LIB_H

#include "library/spdm_common_lib.h"

/**
 * This function sends GET_VERSION, GET_CAPABILITIES, NEGOTIATE_ALGORITHMS
 * to initialize the connection with SPDM responder.
 *
 * Before this function, the requester configuration data can be set via libspdm_set_data.
 * After this function, the negotiated configuration data can be got via libspdm_get_data.
 *
 * @param  spdm_context      A pointer to the SPDM context.
 * @param  get_version_only  If the requester sends GET_VERSION only or not.
 *
 * @retval RETURN_SUCCESS               The connection is initialized successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_init_connection(void *spdm_context, bool get_version_only);

#if LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT
/**
 * This function sends GET_DIGEST to get all digest of the certificate chains from device.
 *
 * TotalDigestSize = sizeof(digest) * count in slot_mask
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_id           Indicates if it is a secured message protected via SPDM session.
 *                              If session_id is NULL, it is a normal message.
 *                              If session_id is not NULL, it is a secured message.
 * @param  slot_mask            Bitmask of the slots that contain certificates.
 * @param  total_digest_buffer  A pointer to a destination buffer to store the digests.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_DIGESTS was sent and DIGESTS was received.
 * @retval LIBSPDM_STATUS_INVALID_STATE_LOCAL
 *         Cannot send GET_DIGESTS due to Requester's state.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP
 *         Cannot send GET_DIGESTS because the Requester's and/or Responder's CERT_CAP = 0.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the DIGESTS response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The DIGESTS response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_BUFFER_FULL
 *         The buffer used to store transcripts is exhausted.
 **/
libspdm_return_t libspdm_get_digest(void *spdm_context, const uint32_t *session_id,
                                    uint8_t *slot_mask, void *total_digest_buffer);

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
 * @param  cert_chain_size  On input, indicate the size in bytes of the destination buffer to store the digest buffer.
 *                          On output, indicate the size in bytes of the certificate chain.
 * @param  cert_chain       A pointer to a destination buffer to store the certificate chain.
 *
 * @retval LIBSPDM_STATUS_SUCCESS
 *         GET_CERTIFICATE was sent and CERTIFICATE was received.
 * @retval LIBSPDM_STATUS_INVALID_STATE_LOCAL
 *         Cannot send GET_CERTIFICATE due to Requester's state.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP
 *         Cannot send GET_CERTIFICATE because the Requester's and/or Responder's CERT_CAP = 0.
 * @retval LIBSPDM_STATUS_INVALID_MSG_SIZE
 *         The size of the CERTIFICATE response is invalid.
 * @retval LIBSPDM_STATUS_INVALID_MSG_FIELD
 *         The CERTIFICATE response contains one or more invalid fields.
 * @retval LIBSPDM_STATUS_ERROR_PEER
 *         The Responder returned an unexpected error.
 * @retval LIBSPDM_STATUS_BUSY_PEER
 *         The Responder continually returned Busy error messages.
 * @retval LIBSPDM_STATUS_RESYNCH_PEER
 *         The Responder returned a RequestResynch error message.
 * @retval LIBSPDM_STATUS_BUFFER_FULL
 *         The buffer used to store transcripts is exhausted.
 * @retval LIBSPDM_STATUS_VERIF_FAIL
 *         Verification of the certificate chain failed.
 * @retval LIBSPDM_STATUS_INVALID_CERT
 *         The certificate is unable to be parsed or contains invalid field values.
 * @retval LIBSPDM_STATUS_CRYPTO_ERROR
 *         A generic cryptography error occurred.
 **/
libspdm_return_t libspdm_get_certificate(void *spdm_context,
                                         const uint32_t *session_id,
                                         uint8_t slot_id,
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
 * @param  cert_chain_size    On input, indicate the size in bytes of the destination buffer to store the digest buffer.
 *                            On output, indicate the size in bytes of the certificate chain.
 * @param  cert_chain         A pointer to a destination buffer to store the certificate chain.
 * @param  trust_anchor       A buffer to hold the trust_anchor which is used to validate the peer certificate, if not NULL.
 * @param  trust_anchor_size  A buffer to hold the trust_anchor_size, if not NULL.
 *
 * @retval RETURN_SUCCESS               The certificate chain is got successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_get_certificate_ex(void *spdm_context,
                                            const uint32_t *session_id,
                                            uint8_t slot_id,
                                            size_t *cert_chain_size,
                                            void *cert_chain,
                                            const void **trust_anchor,
                                            size_t *trust_anchor_size);
#endif /* LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT */

/**
 * This function sends CHALLENGE to authenticate the device based upon the key in one slot.
 *
 * This function verifies the signature in the challenge auth.
 *
 * If basic mutual authentication is requested from the responder,
 * this function also perform the basic mutual authentication.
 *
 * @param  spdm_context           A pointer to the SPDM context.
 * @param  reserved               Reserved for session_id and is ignored.
 * @param  slot_id                The number of slot for the challenge.
 * @param  measurement_hash_type  The type of the measurement hash.
 * @param  measurement_hash       A pointer to a destination buffer to store the measurement hash.
 * @param  slot_mask              A pointer to a destination to store the slot mask.
 *
 * @retval RETURN_SUCCESS               The challenge auth is got successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_challenge(void *spdm_context, void *reserved,
                                   uint8_t slot_id,
                                   uint8_t measurement_hash_type,
                                   void *measurement_hash,
                                   uint8_t *slot_mask);

/**
 * This function sends CHALLENGE to authenticate the device based upon the key in one slot.
 *
 * This function verifies the signature in the challenge auth.
 *
 * If basic mutual authentication is requested from the responder,
 * this function also performs the basic mutual authentication.
 *
 * @param  spdm_context           A pointer to the SPDM context.
 * @param  reserved               Reserved for session_id and is ignored.
 * @param  slot_id                The number of slot for the challenge.
 * @param  measurement_hash_type  The type of the measurement hash.
 * @param  measurement_hash       A pointer to a destination buffer to store the measurement hash.
 * @param  slot_mask              A pointer to a destination to store the slot mask.
 * @param  requester_nonce_in     A buffer to hold the requester nonce (32 bytes) as input, if not NULL.
 * @param  requester_nonce        A buffer to hold the requester nonce (32 bytes), if not NULL.
 * @param  responder_nonce        A buffer to hold the responder nonce (32 bytes), if not NULL.
 * @param  opaque_data            A buffer to hold the responder opaque data, if not NULL.
 * @param  opaque_data_size       On input, the size of the opaque data buffer.
 *                                Responder opaque data should be less than 1024 bytes.
 *                                On output, the size of the opaque data.
 **/
libspdm_return_t libspdm_challenge_ex(void *spdm_context, void *reserved,
                                      uint8_t slot_id,
                                      uint8_t measurement_hash_type,
                                      void *measurement_hash,
                                      uint8_t *slot_mask,
                                      const void *requester_nonce_in,
                                      void *requester_nonce,
                                      void *responder_nonce,
                                      void *opaque_data,
                                      size_t *opaque_data_size);

/**
 * This function sends GET_MEASUREMENT
 * to get measurement from the device.
 *
 * If the signature is requested, this function verifies the signature of the measurement.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  session_id                 Indicates if it is a secured message protected via SPDM session.
 *                                    If session_id is NULL, it is a normal message.
 *                                    If session_id is NOT NULL, it is a secured message.
 * @param  request_attribute          The request attribute of the request message.
 * @param  measurement_operation      The measurement operation of the request message.
 * @param  slot_id                    The number of slot for the certificate chain.
 * @param  content_changed            The measurement content changed output param.
 * @param  number_of_blocks           The number of blocks of the measurement record.
 * @param  measurement_record_length  On input, indicate the size in bytes of the destination buffer to store the measurement record.
 *                                    On output, indicate the size in bytes of the measurement record.
 * @param  measurement_record         A pointer to a destination buffer to store the measurement record.
 *
 * @retval RETURN_SUCCESS               The measurement is got successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_get_measurement(void *spdm_context, const uint32_t *session_id,
                                         uint8_t request_attribute,
                                         uint8_t measurement_operation,
                                         uint8_t slot_id,
                                         uint8_t *content_changed,
                                         uint8_t *number_of_blocks,
                                         uint32_t *measurement_record_length,
                                         void *measurement_record);

/**
 * This function sends GET_MEASUREMENT to get measurement from the device.
 *
 * If the signature is requested, this function verifies the signature of the measurement.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  session_id                 Indicates if it is a secured message protected via SPDM session.
 *                                    If session_id is NULL, it is a normal message.
 *                                    If session_id is NOT NULL, it is a secured message.
 * @param  request_attribute          The request attribute of the request message.
 * @param  measurement_operation      The measurement operation of the request message.
 * @param  slot_id                    The number of slot for the certificate chain.
 * @param  content_changed            The measurement content changed output param.
 * @param  number_of_blocks           The number of blocks of the measurement record.
 * @param  measurement_record_length  On input, indicate the size in bytes of the destination buffer to store the measurement record.
 *                                    On output, indicate the size in bytes of the measurement record.
 * @param  measurement_record         A pointer to a destination buffer to store the measurement record.
 * @param  requester_nonce_in         A buffer to hold the requester nonce (32 bytes) as input, if not NULL.
 * @param  requester_nonce            A buffer to hold the requester nonce (32 bytes), if not NULL.
 * @param  responder_nonce            A buffer to hold the responder nonce (32 bytes), if not NULL.
 * @param  opaque_data                A buffer to hold the responder opaque data, if not NULL.
 * @param  opaque_data_size           On input, the size of the opaque data buffer.
 *                                    Responder opaque data should be less than 1024 bytes.
 *                                    On output, the size of the opaque data.
 **/
libspdm_return_t libspdm_get_measurement_ex(void *spdm_context, const uint32_t *session_id,
                                            uint8_t request_attribute,
                                            uint8_t measurement_operation,
                                            uint8_t slot_id,
                                            uint8_t *content_changed,
                                            uint8_t *number_of_blocks,
                                            uint32_t *measurement_record_length,
                                            void *measurement_record,
                                            const void *requester_nonce_in,
                                            void *requester_nonce,
                                            void *responder_nonce,
                                            void *opaque_data,
                                            size_t *opaque_data_size);

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) || (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP)
/**
 * This function sends KEY_EXCHANGE/FINISH or PSK_EXCHANGE/PSK_FINISH
 * to start an SPDM Session.
 *
 * If encapsulated mutual authentication is requested from the responder,
 * this function also perform the encapsulated mutual authentication.
 *
 * @param  spdm_context           A pointer to the SPDM context.
 * @param  use_psk                False means to use KEY_EXCHANGE/FINISH to start a session.
 *                                True means to use PSK_EXCHANGE/PSK_FINISH to start a session.
 * @param  psk_hint               The psk_hint in PSK_EXCHANGE. It is ignored if use_psk is false.
 * @param  psk_hint_size          The size in bytes of psk_hint. It is ignored if use_psk is false.
 * @param  measurement_hash_type  The type of the measurement hash.
 * @param  slot_id                The number of slot for the certificate chain.
 * @param  session_policy         The policy for the session.
 * @param  session_id             The session ID of the session.
 * @param  heartbeat_period       The heartbeat period for the session.
 * @param  measurement_hash       A pointer to a destination buffer to store the measurement hash.
 *
 * @retval RETURN_SUCCESS               The SPDM session is started.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_start_session(void *spdm_context, bool use_psk,
                                       const void *psk_hint,
                                       uint16_t psk_hint_size,
                                       uint8_t measurement_hash_type,
                                       uint8_t slot_id,
                                       uint8_t session_policy,
                                       uint32_t *session_id,
                                       uint8_t *heartbeat_period,
                                       void *measurement_hash);

/**
 * This function sends KEY_EXCHANGE/FINISH or PSK_EXCHANGE/PSK_FINISH to start an SPDM Session.
 *
 * If encapsulated mutual authentication is requested from the responder,
 * this function also perform the encapsulated mutual authentication.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  use_psk                    False means to use KEY_EXCHANGE/FINISH to start a session.
 *                                    True means to use PSK_EXCHANGE/PSK_FINISH to start a session.
 * @param  psk_hint                   The psk_hint in PSK_EXCHANGE. It is ignored if use_psk is false.
 * @param  psk_hint_size              The size in bytes of psk_hint. It is ignored if use_psk is false.
 * @param  measurement_hash_type      The type of the measurement hash.
 * @param  slot_id                    The number of slot for the certificate chain.
 * @param  session_policy             The policy for the session.
 * @param  session_id                 The session ID of the session.
 * @param  heartbeat_period           The heartbeat period for the session.
 * @param  measurement_hash           A pointer to a destination buffer to store the measurement hash.
 * @param  requester_random_in        A buffer to hold the requester random as input, if not NULL.
 * @param  requester_random_in_size   The size of requester_random_in.
 *                                    If use_psk is false, it must be 32 bytes.
 *                                    If use_psk is true, it means the PSK context and must be 32 bytes at least,
 *                                    but not exceed LIBSPDM_PSK_CONTEXT_LENGTH.
 * @param  requester_random           A buffer to hold the requester random, if not NULL.
 * @param  requester_random_size      On input, the size of requester_random buffer.
 *                                    On output, the size of data returned in requester_random buffer.
 *                                    If use_psk is false, it must be 32 bytes.
 *                                    If use_psk is true, it means the PSK context and must be 32 bytes at least.
 * @param  responder_random           A buffer to hold the responder random, if not NULL.
 * @param  responder_random_size      On input, the size of requester_random buffer.
 *                                    On output, the size of data returned in requester_random buffer.
 *                                    If use_psk is false, it must be 32 bytes.
 *                                    If use_psk is true, it means the PSK context. It could be 0 if device does not support context.
 * @param  requester_opaque_data      A buffer to hold the requester opaque data, if not NULL.
 *                                    If not NULL, this function will not generate any opaque data,
 *                                    including secured message versions.
 * @param  requester_opaque_data_size The size of the opaque data, if requester_opaque_data is not NULL.
 * @param  responder_opaque_data      A buffer to hold the responder opaque data, if not NULL.
 * @param  responder_opaque_data_size On input, the size of the opaque data buffer.
 *                                    Opaque data should be less than 1024 bytes.
 *                                    On output, the size of the opaque data.
 **/
libspdm_return_t libspdm_start_session_ex(void *spdm_context, bool use_psk,
                                          const void *psk_hint,
                                          uint16_t psk_hint_size,
                                          uint8_t measurement_hash_type,
                                          uint8_t slot_id,
                                          uint8_t session_policy,
                                          uint32_t *session_id,
                                          uint8_t *heartbeat_period,
                                          void *measurement_hash,
                                          const void *requester_random_in,
                                          size_t requester_random_in_size,
                                          void *requester_random,
                                          size_t *requester_random_size,
                                          void *responder_random,
                                          size_t *responder_random_size,
                                          const void *requester_opaque_data,
                                          size_t requester_opaque_data_size,
                                          void *responder_opaque_data,
                                          size_t *responder_opaque_data_size);

/**
 * This function sends END_SESSION to stop an SPDM Session.
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  session_id              The session ID of the session.
 * @param  end_session_attributes  The end session attribute for the session.
 *
 * @retval RETURN_SUCCESS               The SPDM session is stopped.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_stop_session(void *spdm_context, uint32_t session_id,
                                      uint8_t end_session_attributes);
#endif /* (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) || (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP) */

/**
 * Send an SPDM or APP message.
 *
 * The SPDM message can be a normal message or a secured message in SPDM session.
 *
 * The APP message is encoded to a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context    A pointer to the SPDM context.
 * @param  session_id      Indicates if it is a secured message protected via SPDM session.
 *                         If session_id is NULL, it is a normal message.
 *                         If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message  Indicates if it is an APP message or SPDM message.
 * @param  request         A pointer to the request data.
 * @param  request_size    Size in bytes of the request data.
 **/
libspdm_return_t libspdm_send_data(void *spdm_context, const uint32_t *session_id,
                                   bool is_app_message,
                                   const void *request, size_t request_size);

/**
 * Receive an SPDM or APP message.
 *
 * The SPDM message can be a normal message or a secured message in SPDM session.
 *
 * The APP message is encoded to a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context    A pointer to the SPDM context.
 * @param  session_id      Indicates if it is a secured message protected via SPDM session.
 *                         If session_id is NULL, it is a normal message.
 *                         If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message  Indicates if it is an APP message or SPDM message.
 * @param  response        A pointer to the response data.
 * @param  response_size   Size in bytes of the response data.
 *                         On input, it means the size in bytes of response data buffer.
 *                         On output, it means the size in bytes of copied response data buffer if
 *                         LIBSPDM_STATUS_SUCCESS is returned, and means the size in bytes of
 *                         desired response data buffer if LIBSPDM_STATUS_BUFFER_TOO_SMALL is
 *                         returned.
 **/
libspdm_return_t libspdm_receive_data(void *spdm_context, const uint32_t *session_id,
                                      bool is_app_message,
                                      void *response, size_t *response_size);

/**
 * Send and receive an SPDM or APP message.
 *
 * The SPDM message can be a normal message or a secured message in SPDM session.
 *
 * The APP message is encoded to a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context    A pointer to the SPDM context.
 * @param  session_id      Indicates if it is a secured message protected via SPDM session.
 *                         If session_id is NULL, it is a normal message.
 *                         If session_id is NOT NULL, it is a secured message.
 * @param  is_app_message  Indicates if it is an APP message or SPDM message.
 * @param  request         A pointer to the request data.
 * @param  request_size    Size in bytes of the request data.
 * @param  response        A pointer to the response data.
 * @param  response_size   Size in bytes of the response data.
 *                         On input, it means the size in bytes of response data buffer.
 *                         On output, it means the size in bytes of copied response data buffer if
 *                         LIBSPDM_STATUS_SUCCESS is returned, and means the size in bytes of
 *                         desired response data buffer if LIBSPDM_STATUS_BUFFER_TOO_SMALL is
 *                         returned.
 **/
libspdm_return_t libspdm_send_receive_data(void *spdm_context,
                                           const uint32_t *session_id,
                                           bool is_app_message,
                                           const void *request, size_t request_size,
                                           void *response, size_t *response_size);

/**
 * This function sends HEARTBEAT
 * to an SPDM Session.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    The session ID of the session.
 *
 * @retval RETURN_SUCCESS               The heartbeat is sent and received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_heartbeat(void *spdm_context, uint32_t session_id);

/**
 * This function sends KEY_UPDATE
 * to update keys for an SPDM Session.
 *
 * After keys are updated, this function also uses VERIFY_NEW_KEY to verify the key.
 *
 * @param  spdm_context      A pointer to the SPDM context.
 * @param  session_id        The session ID of the session.
 * @param  single_direction  true means the operation is UPDATE_KEY.
 *                           false means the operation is UPDATE_ALL_KEYS.
 *
 * @retval RETURN_SUCCESS               The keys of the session are updated.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_key_update(void *spdm_context, uint32_t session_id, bool single_direction);

/**
 * This function executes a series of SPDM encapsulated requests and receives SPDM encapsulated responses.
 *
 * This function starts with the first encapsulated request (such as GET_ENCAPSULATED_REQUEST)
 * and ends with last encapsulated response (such as RESPONSE_PAYLOAD_TYPE_ABSENT or RESPONSE_PAYLOAD_TYPE_SLOT_NUMBER).
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    Indicate if the encapsulated request is a secured message.
 *                       If session_id is NULL, it is a normal message.
 *                       If session_id is not NULL, it is a secured message.
 *
 * @retval RETURN_SUCCESS               The SPDM Encapsulated requests are sent and the responses are received.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 **/
libspdm_return_t libspdm_send_receive_encap_request(void *spdm_context, const uint32_t *session_id);

/**
 * Process the encapsulated request and return the encapsulated response.
 *
 * @param  spdm_context        A pointer to the SPDM context.
 * @param  spdm_request_size   Size in bytes of the request data.
 * @param  spdm_request        A pointer to the request data.
 * @param  spdm_response_size  Size in bytes of the response data.
 *                             On input, it means the size in bytes of response data buffer.
 *                             On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
 *                             and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  spdm_response       A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The request is processed and the response is returned.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
typedef libspdm_return_t (*libspdm_get_encap_response_func)(
    void *spdm_context, size_t spdm_request_size,
    void *spdm_request, size_t *spdm_response_size,
    void *spdm_response);

/**
 * Register an SPDM encapsulated message process function.
 *
 * If the default encapsulated message process function cannot handle the encapsulated message,
 * this function will be invoked.
 *
 * @param  spdm_context             A pointer to the SPDM context.
 * @param  get_encap_response_func  The function to process the encapsuled message.
 **/
void libspdm_register_get_encap_response_func(void *spdm_context,
                                              const libspdm_get_encap_response_func
                                              get_encap_response_func);

/**
 * Generate encapsulated ERROR message.
 *
 * This function can be called in libspdm_get_encap_response_func.
 *
 * @param  spdm_context        A pointer to the SPDM context.
 * @param  error_code          The error code of the message.
 * @param  error_data          The error data of the message.
 * @param  spdm_response_size  Size in bytes of the response data.
 *                             On input, it means the size in bytes of data buffer.
 *                             On output, it means the size in bytes of copied data buffer if RETURN_SUCCESS is returned,
 *                             and means the size in bytes of desired data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  spdm_response       A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The error message is generated.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_generate_encap_error_response(
    const void *spdm_context, uint8_t error_code, uint8_t error_data,
    size_t *spdm_response_size, void *spdm_response);

/**
 * Generate encapsulated ERROR message with extended error data.
 *
 * This function can be called in libspdm_get_encap_response_func.
 *
 * @param  spdm_context              A pointer to the SPDM context.
 * @param  error_code                The error code of the message.
 * @param  error_data                The error data of the message.
 * @param  extended_error_data_size  The size in bytes of the extended error data.
 * @param  extended_error_data       A pointer to the extended error data.
 * @param  spdm_response_size        Size in bytes of the response data.
 *                                   On input, it means the size in bytes of response data buffer.
 *                                   On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
 *                                   and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
 * @param  spdm_response             A pointer to the response data.
 *
 * @retval RETURN_SUCCESS               The error message is generated.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_generate_encap_extended_error_response(
    const void *spdm_context, uint8_t error_code, uint8_t error_data,
    size_t extended_error_data_size, const uint8_t *extended_error_data,
    size_t *spdm_response_size, void *spdm_response);

#if LIBSPDM_ENABLE_CAPABILITY_CSR_CAP
/**
 * This function sends GET_CSR
 * to get csr from the device.
 *
 * @param[in]  context                A pointer to the SPDM context.
 * @param[in]  session_id             Indicates if it is a secured message protected via SPDM session.
 *                                    If session_id is NULL, it is a normal message.
 *                                    If session_id is NOT NULL, it is a secured message.
 * @param[in]  requester_info         Requester info to gen CSR
 * @param[in]  requester_info_length  The length of requester info
 * @param[in]  opaque_data            Opaque data from requester.
 * @param[in]  opaque_data_length     The length of opaque_data.
 * @param[out] csr                    Address to store CSR.
 * @param[out] csr_len                On input, *csr_len indicates the max csr buffer size.
 *                                    On output, *csr_len indicates the actual csr buffer size.
 **/
libspdm_return_t libspdm_get_csr(void *spdm_context,
                                 const uint32_t *session_id,
                                 void *requester_info, uint16_t requester_info_length,
                                 void *opaque_data, uint16_t opaque_data_length,
                                 void *csr, size_t *csr_len);
#endif /*LIBSPDM_ENABLE_CAPABILITY_CSR_CAP*/

#if LIBSPDM_ENABLE_CAPABILITY_SET_CERT_CAP
/**
 * This function try to send SET_CERTIFICATE
 * to set certificate from the device.
 *
 * @param  context          A pointer to the SPDM context.
 * @param  session_id       Indicates if it is a secured message protected via SPDM session.
 *                          If session_id is NULL, it is a normal message.
 *                          If session_id is NOT NULL, it is a secured message.
 * @param  slot_id          The number of slot for the certificate chain.
 * @param  cert_chain       The pointer for the certificate chain to set.
 *                          The cert chain is a full SPDM certificate chain, including Length and Root Cert Hash.
 * @param  cert_chain_size  The size of the certificate chain to set.
 *
 * @retval RETURN_SUCCESS               The measurement is got successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
 * @retval RETURN_SECURITY_VIOLATION    Any verification fails.
 **/
libspdm_return_t libspdm_set_certificate(void *spdm_context,
                                         const uint32_t *session_id, uint8_t slot_id,
                                         void *cert_chain, size_t cert_chain_size);

#endif /* LIBSPDM_ENABLE_CAPABILITY_SET_CERT_CAP */

#if LIBSPDM_ENABLE_MSG_LOG
/* For now these functions are only available to the Requester. They may become available to the
 * Responder at a later time.
 */

/**
 * This function initializes message logging. The caller must provide a buffer and the buffer's
 * size.
 *
 * @param  context          A pointer to the SPDM context.
 * @param  msg_buffer       A pointer to a caller-provided buffer.
 * @param  msg_buffer_size  The size of the buffer in bytes. It must be greater than zero.
 **/
void libspdm_init_msg_log (void *spdm_context, void *msg_buffer, size_t msg_buffer_size);

/**
 * This function sets the mode in which the message logger operates.
 *
 * @param  context A pointer to the SPDM context.
 * @param  mode    A bitmask specifying the mode in which the message logger operates.
 *                 LIBSPDM_MSG_LOG_MODE_ENABLE - when set the message logger is active.
 */
void libspdm_set_msg_log_mode (void *spdm_context, uint32_t mode);

/**
 * This function returns the status of the message logger.
 *
 * @param  context  A pointer to the SPDM context.
 *
 * @retval uint32_t A bitmask giving the status of the message logger.
 *                  LIBSPDM_MSG_LOG_STATUS_BUFFER_FULL - if set the message logging buffer has
 *                                                       reached capacity.
 */
uint32_t libspdm_get_msg_log_status (void *spdm_context);

/**
 * This function returns the size of the message log.
 *
 * @param  context  A pointer to the SPDM context.
 * @retval size_t   The size of the message log in bytes.
 */
size_t libspdm_get_msg_log_size (void *spdm_context);

/**
 * This function resets the message log while retaining the message buffer and maximum size given in
 * libspdm_init_msg_log.
 *
 * @param context  A pointer to the SPDM context.
 */
void libspdm_reset_msg_log (void *spdm_context);
#endif /* LIBSPDM_ENABLE_MSG_LOG */

#endif /* SPDM_REQUESTER_LIB_H */
