/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_COMMON_LIB_H
#define SPDM_COMMON_LIB_H

#include "internal/libspdm_lib_config.h"
#include "hal/base.h"
#include "library/spdm_secured_message_lib.h"
#include "library/spdm_return_status.h"

#define LIBSPDM_MAJOR_VERSION 0x03
#define LIBSPDM_MINOR_VERSION 0x01
#define LIBSPDM_PATCH_VERSION 0x01
#define LIBSPDM_ALPHA         0x00

/* Connection: When a host sends messages to a device, they create a connection.
 *             The host can and only can create one connection with one device.
 *             The host may create multiple connections with multiple devices at same time.
 *             A connection can be unique identified by the connected device.
 *             The message exchange in a connection is plain text.*/

/* Session: In one connection with one device, a host may create multiple sessions.
 *          The session starts with via KEY_EXCHANGE or PSK_EXCHANGE, and step with END_SESSION.
 *          A session can be unique identified by a session ID, returned from the device.
 *          The message exchange in a session is cipher text.*/

typedef enum {
    /* SPDM parameter */
    LIBSPDM_DATA_SPDM_VERSION,
    LIBSPDM_DATA_SECURED_MESSAGE_VERSION,

    /* SPDM capability */
    LIBSPDM_DATA_CAPABILITY_FLAGS,
    LIBSPDM_DATA_CAPABILITY_CT_EXPONENT,
    LIBSPDM_DATA_CAPABILITY_RTT_US,
    LIBSPDM_DATA_CAPABILITY_DATA_TRANSFER_SIZE,
    LIBSPDM_DATA_CAPABILITY_MAX_SPDM_MSG_SIZE,
    LIBSPDM_DATA_CAPABILITY_SENDER_DATA_TRANSFER_SIZE,

    /* SPDM algorithm setting */
    LIBSPDM_DATA_MEASUREMENT_SPEC,
    LIBSPDM_DATA_MEASUREMENT_HASH_ALGO,
    LIBSPDM_DATA_BASE_ASYM_ALGO,
    LIBSPDM_DATA_BASE_HASH_ALGO,
    LIBSPDM_DATA_DHE_NAME_GROUP,
    LIBSPDM_DATA_AEAD_CIPHER_SUITE,
    LIBSPDM_DATA_REQ_BASE_ASYM_ALG,
    LIBSPDM_DATA_KEY_SCHEDULE,
    LIBSPDM_DATA_OTHER_PARAMS_SUPPORT,

    /* Connection State */
    LIBSPDM_DATA_CONNECTION_STATE,

    /* response_state */
    LIBSPDM_DATA_RESPONSE_STATE,

    /* Certificate info */
    LIBSPDM_DATA_LOCAL_PUBLIC_CERT_CHAIN,
    LIBSPDM_DATA_PEER_PUBLIC_ROOT_CERT,
    LIBSPDM_DATA_PEER_PUBLIC_KEY,
    LIBSPDM_DATA_LOCAL_PUBLIC_KEY,

    LIBSPDM_DATA_BASIC_MUT_AUTH_REQUESTED,
    LIBSPDM_DATA_MUT_AUTH_REQUESTED,
    LIBSPDM_DATA_HEARTBEAT_PERIOD,

    /* Negotiated result */
    LIBSPDM_DATA_PEER_USED_CERT_CHAIN_BUFFER,
    LIBSPDM_DATA_PEER_SLOT_MASK,
    LIBSPDM_DATA_PEER_TOTAL_DIGEST_BUFFER,

    /* SessionData */
    LIBSPDM_DATA_SESSION_USE_PSK,
    LIBSPDM_DATA_SESSION_MUT_AUTH_REQUESTED,
    LIBSPDM_DATA_SESSION_END_SESSION_ATTRIBUTES,
    LIBSPDM_DATA_SESSION_POLICY,

    /* App context data that can be used by the application
     * during callback functions such libspdm_device_send_message_func. */
    LIBSPDM_DATA_APP_CONTEXT_DATA,

    /**
     * The LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY 0x00000001 control to generate SPDM_ERROR_CODE_DECRYPT_ERROR response or drop the request silently.
     * If the 0x00000001 is not set, generate SPDM_ERROR_CODE_DECRYPT_ERROR response.
     * If the 0x00000001 set, drop the request silently.
     **/
    LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY,

    /* VCA cached for CACHE_CAP in 1.2 for transcript. */
    LIBSPDM_DATA_VCA_CACHE,

    /* if the context is for a requester. It only needs to be set in VCA cache.
     * In normal flow, the value is set in GET_VERSION or VERSION automatically.
     * false means responder
     * true means requester
     **/
    LIBSPDM_DATA_IS_REQUESTER,

    /* If the Responder replies with a Busy `ERROR` response to a request
     * then the Requester is free to retry sending the request.
     * This value specifies the maximum number of times libspdm will retry
     * sending the request before returning an error.
     * If its value is 0 then libspdm will not send any retry requests.
     **/
    LIBSPDM_DATA_REQUEST_RETRY_TIMES,

    /* If the Responder replies with a Busy `ERROR` response to a request
     * then the Requester is free to retry sending the request.
     * This value specifies the delay time in microseconds between each retry requests.
     * If its value is 0 then libspdm will send retry request immediately.
     **/
    LIBSPDM_DATA_REQUEST_RETRY_DELAY_TIME,

    /* Below two entries are used to limit the number of DHE session and PSK session separately.
     * When set a new value, below rule is applied:
     *     new MaxDheSessionCount <= LIBSPDM_MAX_SESSION_COUNT - current MaxPskSessionCount
     *     new MaxPskSessionCount <= LIBSPDM_MAX_SESSION_COUNT - current MaxDheSessionCount
     * 0 means no limitation for the specific DHE or PSK session, as long as
     *     PskSessionCount + DheSessionCount <= LIBSPDM_MAX_SESSION_COUNT.
     * If these values are modified while there are active sessions then the active sessions
     * aren't terminated.
     **/
    LIBSPDM_DATA_MAX_DHE_SESSION_COUNT,
    LIBSPDM_DATA_MAX_PSK_SESSION_COUNT,

    LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_RSP_DIR,
    LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_REQ_DIR,
    LIBSPDM_DATA_MAX_SPDM_SESSION_SEQUENCE_NUMBER,

    /* For SPDM 1.0 and 1.1, allow signature verification in big, little, or both endians. */
    LIBSPDM_DATA_SPDM_VERSION_10_11_VERIFY_SIGNATURE_ENDIAN,

    LIBSPDM_DATA_SEQUENCE_NUMBER_ENDIAN,
    LIBSPDM_DATA_SESSION_SEQUENCE_NUMBER_ENDIAN,

    /* MAX */
    LIBSPDM_DATA_MAX
} libspdm_data_type_t;

/**
 * It controls to generate SPDM_ERROR_CODE_DECRYPT_ERROR response or drop the request silently.
 * If the 0x1 is not set, generate SPDM_ERROR_CODE_DECRYPT_ERROR response.
 * If the 0x1 set, drop the request silently.
 **/
#define LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY_DROP_ON_DECRYPT_ERROR 0x1

#define LIBSPDM_MSG_LOG_STATUS_BUFFER_FULL 1
#define LIBSPDM_MSG_LOG_MODE_ENABLE 1

typedef enum {
    LIBSPDM_DATA_LOCATION_LOCAL,
    LIBSPDM_DATA_LOCATION_CONNECTION,
    LIBSPDM_DATA_LOCATION_SESSION,
    LIBSPDM_DATA_LOCATION_MAX
} libspdm_data_location_t;

typedef struct {
    libspdm_data_location_t location;
    /* data_type specific:
     *   session_id for the negotiated key.
     *   SlotId for the certificate.
     *   req_slot_id + measurement_hash_type for LIBSPDM_DATA_MUT_AUTH_REQUESTED*/
    uint8_t additional_data[4];
} libspdm_data_parameter_t;

typedef enum {
    /* Before GET_VERSION/VERSION */
    LIBSPDM_CONNECTION_STATE_NOT_STARTED,

    /* After GET_VERSION/VERSION */
    LIBSPDM_CONNECTION_STATE_AFTER_VERSION,

    /* After GET_CAPABILITIES/CAPABILITIES */
    LIBSPDM_CONNECTION_STATE_AFTER_CAPABILITIES,

    /* After NEGOTIATE_ALGORITHMS/ALGORITHMS */
    LIBSPDM_CONNECTION_STATE_NEGOTIATED,

    /* After GET_DIGESTS/DIGESTS */
    LIBSPDM_CONNECTION_STATE_AFTER_DIGESTS,

    /* After GET_CERTIFICATE/CERTIFICATE */
    LIBSPDM_CONNECTION_STATE_AFTER_CERTIFICATE,

    /* After CHALLENGE/CHALLENGE_AUTH, and ENCAP CHALLENGE/CHALLENGE_AUTH if MUT_AUTH is enabled. */
    LIBSPDM_CONNECTION_STATE_AUTHENTICATED,

    /* MAX */
    LIBSPDM_CONNECTION_STATE_MAX
} libspdm_connection_state_t;

typedef enum {
    /* Normal response. */
    LIBSPDM_RESPONSE_STATE_NORMAL,

    /* Other component is busy. */
    LIBSPDM_RESPONSE_STATE_BUSY,

    #if LIBSPDM_RESPOND_IF_READY_SUPPORT
    /* Hardware is not ready. */
    LIBSPDM_RESPONSE_STATE_NOT_READY,
    #endif /* LIBSPDM_RESPOND_IF_READY_SUPPORT */

    /* Firmware Update is done. Need resync. */
    LIBSPDM_RESPONSE_STATE_NEED_RESYNC,

    /* Processing Encapsulated message. */
    LIBSPDM_RESPONSE_STATE_PROCESSING_ENCAP,

    /* MAX */
    LIBSPDM_RESPONSE_STATE_MAX
} libspdm_response_state_t;

/* These macros apply only if the negotiated SPDM version is 1.0 or 1.1.
 * The default verification mode is big endian only. */
#define LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY 0
#define LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY 1
#define LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE 2

/* Set the endianness of the AEAD sequence number. These macros apply only if the negotiated
 * SPDM Secured Message version is 1.0 or 1.1.
 * *_LITTLE and *_BIG immediately return an error on decryption failure.
 * *_BOTH tries the opposite endianness on decryption failure.
 * The default is LIBSPDM_DATA_SESSION_SEQ_NUM_ENC_LITTLE_DEC_LITTLE. */
#define LIBSPDM_DATA_SESSION_SEQ_NUM_ENC_LITTLE_DEC_LITTLE 0
#define LIBSPDM_DATA_SESSION_SEQ_NUM_ENC_LITTLE_DEC_BOTH 1
#define LIBSPDM_DATA_SESSION_SEQ_NUM_ENC_BIG_DEC_BIG 2
#define LIBSPDM_DATA_SESSION_SEQ_NUM_ENC_BIG_DEC_BOTH 3

/*
 * +--------------------------+------------------------------------------+---------+
 * | GET_VERSION              | 4                                        | 1       |
 * | VERSION {1.0, 1.1, 1.2}  | 6 + 2 * 3 = 12                           | 1       |
 * +--------------------------+------------------------------------------+---------+
 * | GET_CAPABILITIES 1.2     | 20                                       | 1       |
 * | CAPABILITIES 1.2         | 20                                       | 1       |
 * +--------------------------+------------------------------------------+---------+
 * | NEGOTIATE_ALGORITHMS 1.2 | 32 + 4 * 4 = 48                          | 2       |
 * | ALGORITHMS 1.2           | 36 + 4 * 4 = 52                          | 2       |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_VCA_BUFFER_SIZE (150 + 2 * LIBSPDM_MAX_VERSION_COUNT)

/**
 * Set an SPDM context data.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  data_type     Type of the SPDM context data.
 * @param  parameter     Type specific parameter of the SPDM context data.
 * @param  data          A pointer to the SPDM context data.
 * @param  data_size     Size in bytes of the SPDM context data.
 *
 * @retval RETURN_SUCCESS               The SPDM context data is set successfully.
 * @retval RETURN_INVALID_PARAMETER     The data is NULL or the data_type is zero.
 * @retval RETURN_UNSUPPORTED           The data_type is unsupported.
 * @retval RETURN_ACCESS_DENIED         The data_type cannot be set.
 * @retval RETURN_NOT_READY             data is not ready to set.
 **/
libspdm_return_t libspdm_set_data(void *spdm_context,
                                  libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter, void *data,
                                  size_t data_size);

/**
 * Get an SPDM context data.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  data_type     Type of the SPDM context data.
 * @param  parameter     Type specific parameter of the SPDM context data.
 * @param  data          A pointer to the SPDM context data.
 * @param  data_size     Size in bytes of the SPDM context data.
 *                       On input, it means the size in bytes of data buffer.
 *                       On output, it means the size in bytes of copied data buffer if RETURN_SUCCESS,
 *                       and means the size in bytes of desired data buffer if RETURN_BUFFER_TOO_SMALL.
 *
 * @retval RETURN_SUCCESS               The SPDM context data is set successfully.
 * @retval RETURN_INVALID_PARAMETER     The data_size is NULL or the data is NULL and *data_size is not zero.
 * @retval RETURN_UNSUPPORTED           The data_type is unsupported.
 * @retval RETURN_NOT_FOUND             The data_type cannot be found.
 * @retval RETURN_NOT_READY             The data is not ready to return.
 * @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
 **/
libspdm_return_t libspdm_get_data(void *spdm_context,
                                  libspdm_data_type_t data_type,
                                  const libspdm_data_parameter_t *parameter,
                                  void *data, size_t *data_size);

#if LIBSPDM_CHECK_SPDM_CONTEXT
/**
 * Check that the SPDM context is in a valid state.
 *
 * This is typically called after all context data has been populated via the libspdm_set_data
 * function. The function returns on first failure. If LIBSPDM_DEBUG_ENABLE or
 * LIBSPDM_DEBUG_PRINT_ENABLE are enabled then it also prints the failing condition.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 *
 * @retval true   The SPDM context is in a valid state.
 * @retval false  The SPDM context is in an invalid state.
 **/
bool libspdm_check_context (void *spdm_context);
#endif /* LIBSPDM_CHECK_SPDM_CONTEXT */

/**
 * Get the last SPDM error struct of an SPDM context.
 *
 * @param  spdm_context     A pointer to the SPDM context.
 * @param  last_spdm_error  Last SPDM error struct of an SPDM context.
 */
void libspdm_get_last_spdm_error_struct(void *spdm_context,
                                        libspdm_error_struct_t *last_spdm_error);

/**
 * Set the last SPDM error struct of an SPDM context.
 *
 * @param  spdm_context     A pointer to the SPDM context.
 * @param  last_spdm_error  Last SPDM error struct of an SPDM context.
 */
void libspdm_set_last_spdm_error_struct(void *spdm_context,
                                        libspdm_error_struct_t *last_spdm_error);

/**
 * Initialize an SPDM context, as well as all secured message contexts,
 * in the specified locations.
 *
 * The size in bytes of the spdm_context can be returned by
 * libspdm_get_context_size_without_secured_context.
 *
 * The size in bytes of a single secured message context can be returned by
 * libspdm_secured_message_get_context_size.
 *
 * @param  spdm_context          A pointer to the SPDM context.
 * @param  secured_contexts      An array of pointers, with each entry containing
 *                               the location of a secured message context.
 * @param  num_secured_contexts  Number of secured message contexts to initialize.
 *                               Currently, only LIBSPDM_MAX_SESSION_COUNT is supported.
 *                               In future releases, lesser values may be supported.
 *
 * @retval RETURN_SUCCESS        Contexts are initialized.
 * @retval RETURN_DEVICE_ERROR   Context initialization failed.
 */
libspdm_return_t libspdm_init_context_with_secured_context(void *spdm_context,
                                                           void **secured_contexts,
                                                           size_t num_secured_contexts);

#if LIBSPDM_FIPS_MODE
/**
 * Initialize an libspdm_fips_selftest_context.
 *
 * The
 *
 * @param  spdm_context         A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS       context is initialized.
 * @retval RETURN_DEVICE_ERROR  context initialization failed.
 */
libspdm_return_t libspdm_init_fips_selftest_context(void *fips_selftest_context);

/**
 * Return the size in bytes of the fips_selftest_context.
 *
 * @return the size in bytes of the fips_selftest_context.
 **/
size_t libspdm_get_fips_selftest_context_size(void);

/**
 * import fips_selftest_context to spdm_context;
 *
 * @param[in,out]  spdm_context                A pointer to the spdm_context.
 * @param[in]      fips_selftest_context       A pointer to the fips_selftest_context.
 * @param[in]      fips_selftest_context_size  The size of fips_selftest_context.
 *
 * @retval true   import fips_selftest_context successful.
 * @retval false  spdm_context or fips_selftest_context is null.
 */
bool libspdm_import_fips_selftest_context_to_spdm_context(void *spdm_context,
                                                          void *fips_selftest_context,
                                                          size_t fips_selftest_context_size);

/**
 * export fips_selftest_context from spdm_context;
 *
 * @param[in]          spdm_context                A pointer to the spdm_context.
 * @param[in,out]      fips_selftest_context       A pointer to the fips_selftest_context.
 * @param[in]          fips_selftest_context_size  The size of fips_selftest_context.
 *
 * @retval true   export fips_selftest_context successful.
 * @retval false  spdm_context or fips_selftest_context is null.
 */
bool libspdm_export_fips_selftest_context_from_spdm_context(void *spdm_context,
                                                            void *fips_selftest_context,
                                                            size_t fips_selftest_context_size);

#endif /* LIBSPDM_FIPS_MODE */

/**
 * Initialize an SPDM context, as well as secured message contexts.
 * The secured message contexts are appended to the context structure.
 *
 * The total size in bytes of the spdm_context and all secured message
 * contexts can be returned by libspdm_get_context_size().
 *
 * @param  spdm_context         A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS       context is initialized.
 * @retval RETURN_DEVICE_ERROR  context initialization failed.
 */
libspdm_return_t libspdm_init_context(void *spdm_context);

/**
 * Reset an SPDM context.
 *
 * Note that message logging is not reset by this function. To reset the message logger call
 * libspdm_reset_msg_log.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 */
void libspdm_reset_context(void *spdm_context);

/**
 * Free the memory of contexts within the SPDM context.
 * These are typically contexts whose memory has been allocated by the cryptography library.
 * This function does not free the SPDM context itself.
 *
 * @param[in]  spdm_context         A pointer to the SPDM context.
 *
 */
void libspdm_deinit_context(void *spdm_context);

/**
 * Return the size in bytes of the SPDM context. This includes all
 * secured message context data as well.
 *
 * For just the SPDM context size, use libspdm_get_context_size_without_secured_context.
 *
 * @return the size in bytes of the SPDM context and secured message contexts.
 **/
size_t libspdm_get_context_size(void);

/**
 * Return the size in bytes of just the SPDM context, without secured message context.
 *
 * For the complete context size, use libspdm_get_context_size.
 *
 * @return the size in bytes of the SPDM context.
 **/
size_t libspdm_get_context_size_without_secured_context(void);

/**
 * Send an SPDM transport layer message to a device.
 *
 * The message is an SPDM message with transport layer wrapper,
 * or a secured SPDM message with transport layer wrapper.
 *
 * For requester, the message is a transport layer SPDM request.
 * For responder, the message is a transport layer SPDM response.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message_size  Size in bytes of the message data buffer.
 * @param  message       A pointer to a destination buffer to store the message.
 *                       The caller is responsible for having either implicit or explicit ownership
 *                       of the buffer. The message pointer shall be inside of
 *                       [msg_buf_ptr, msg_buf_ptr + max_msg_size] from acquired sender_buffer.
 * @param  timeout       The timeout, in us units, to use for the execution of the message. A
 *                       timeout value of 0 means that this function will wait indefinitely for the
 *                       message to execute. If timeout is greater than zero, then this function
 *                       will return RETURN_TIMEOUT if the time required to execute the message is
 *                       greater than timeout.
 *
 * @retval RETURN_SUCCESS            The SPDM message is sent successfully.
 * @retval RETURN_DEVICE_ERROR       A device error occurs when the SPDM message is sent to the device.
 * @retval RETURN_INVALID_PARAMETER  The message is NULL or the message_size is zero.
 * @retval RETURN_TIMEOUT            A timeout occurred while waiting for the SPDM message
 *                                   to execute.
 **/
typedef libspdm_return_t (*libspdm_device_send_message_func)(void *spdm_context,
                                                             size_t message_size,
                                                             const void *message,
                                                             uint64_t timeout);

/**
 * Receive an SPDM transport layer message from a device.
 *
 * The message is an SPDM message with transport layer wrapper,
 * or a secured SPDM message with transport layer wrapper.
 *
 * For requester, the message is a transport layer SPDM response.
 * For responder, the message is a transport layer SPDM request.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message_size  Size in bytes of the message data buffer.
 * @param  message       A pointer to a destination buffer to store the message.
 *                       The caller is responsible for having either implicit or explicit ownership
 *                       of the buffer. On input, the message pointer shall be msg_buf_ptr from
 *                       acquired receiver_buffer. On output, the message pointer shall be inside of
 *                       [msg_buf_ptr, msg_buf_ptr + max_msg_size] from acquired receiver_buffer.
 * @param  timeout       The timeout, in us units, to use for the execution of the message. A
 *                       timeout value of 0 means that this function will wait indefinitely for the
 *                       message to execute. If timeout is greater than zero, then this function
 *                       will return RETURN_TIMEOUT if the time required to execute the message is
 *                       greater than timeout.
 *
 * @retval RETURN_SUCCESS               The SPDM message is received successfully.
 * @retval RETURN_DEVICE_ERROR          A device error occurs when the SPDM message is received from the device.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL, message_size is NULL or
 *                                     the *message_size is zero.
 * @retval RETURN_TIMEOUT              A timeout occurred while waiting for the SPDM message
 *                                     to execute.
 **/
typedef libspdm_return_t (*libspdm_device_receive_message_func)(
    void *spdm_context, size_t *message_size, void **message,
    uint64_t timeout);

/**
 * Register SPDM device input/output functions.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  send_message                  The fuction to send an SPDM transport layer message.
 * @param  receive_message               The fuction to receive an SPDM transport layer message.
 **/
void libspdm_register_device_io_func(
    void *spdm_context, libspdm_device_send_message_func send_message,
    libspdm_device_receive_message_func receive_message);

/**
 * Acquire a device sender buffer for transport layer message.
 *
 * @param  context       A pointer to the SPDM context.
 * @param  msg_buf_ptr   A pointer to a sender buffer.
 *
 * @retval LIBSPDM_STATUS_SUCCESS       The sender buffer has been acquired.
 * @retval LIBSPDM_STATUS_ACQUIRE_FAIL  Unable to acquire sender buffer.
 **/
typedef libspdm_return_t (*libspdm_device_acquire_sender_buffer_func)(
    void *spdm_context, void **msg_buf_ptr);

/**
 * Release a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  msg_buf_ptr                   A pointer to a sender buffer.
 *
 * @retval RETURN_SUCCESS               The sender buffer is Released.
 **/
typedef void (*libspdm_device_release_sender_buffer_func)(void *spdm_context,
                                                          const void *msg_buf_ptr);

/**
 * Acquire a device receiver buffer for transport layer message.
 *
 * @param  context       A pointer to the SPDM context.
 * @param  msg_buf_pt    A pointer to a receiver buffer.
 *
 * @retval LIBSPDM_STATUS_SUCCESS       The receiver buffer has been acquired.
 * @retval LIBSPDM_STATUS_ACQUIRE_FAIL  Unable to acquire receiver buffer.
 **/
typedef libspdm_return_t (*libspdm_device_acquire_receiver_buffer_func)(
    void *spdm_context, void **msg_buf_ptr);

/**
 * Release a device receiver buffer for transport layer message.
 *
 * @param  context      A pointer to the SPDM context.
 * @param  msg_buf_ptr  A pointer to a receiver buffer.
 *
 * @retval RETURN_SUCCESS  The receiver buffer is Released.
 **/
typedef void (*libspdm_device_release_receiver_buffer_func)(void *spdm_context,
                                                            const void *msg_buf_ptr);

/**
 * Register SPDM device buffer management functions.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * The sender_buffer_size and receiver_buffer_size must be no smaller than
 * MAX (non-secure Transport Message Header Size +
 *          SPDM_CAPABILITIES.DataTransferSize +
 *          max alignment pad size (transport specific),
 *      secure Transport Message Header Size +
 *          sizeof(spdm_secured_message_a_data_header1_t) +
 *          length of sequence_number (transport specific) +
 *          sizeof(spdm_secured_message_a_data_header2_t) +
 *          sizeof(spdm_secured_message_cipher_header_t) +
 *          App Message Header Size (transport specific) +
 *          SPDM_CAPABILITIES.DataTransferSize +
 *          maximum random data size (transport specific) +
 *          AEAD MAC size (16) +
 *          max alignment pad size (transport specific)).
 *
 * Finally, the SPDM_CAPABILITIES.DataTransferSize will be calculated based upon it.
 *
 *   For MCTP,
 *          Transport Message Header Size = sizeof(mctp_message_header_t)
 *          length of sequence_number = 2
 *          App Message Header Size = sizeof(mctp_message_header_t)
 *          maximum random data size = MCTP_MAX_RANDOM_NUMBER_COUNT
 *          max alignment pad size = 0
 *   For PCI_DOE,
 *          Transport Message Header Size = sizeof(pci_doe_data_object_header_t)
 *          length of sequence_number = 0
 *          App Message Header Size = 0
 *          maximum random data size = 0
 *          max alignment pad size = 3
 *
 * @param  spdm_context             A pointer to the SPDM context.
 * @param  sender_buffer_size       Size in bytes of the sender buffer.
 * @param  receiver_buffer_size     Size in bytes of the receiver buffer.
 * @param  acquire_sender_buffer    The fuction to acquire transport layer sender buffer.
 * @param  release_sender_buffer    The fuction to release transport layer sender buffer.
 * @param  acquire_receiver_buffer  The fuction to acquire transport layer receiver buffer.
 * @param  release_receiver_buffer  The fuction to release transport layer receiver buffer.
 **/
void libspdm_register_device_buffer_func(
    void *spdm_context,
    uint32_t sender_buffer_size,
    uint32_t receiver_buffer_size,
    libspdm_device_acquire_sender_buffer_func acquire_sender_buffer,
    libspdm_device_release_sender_buffer_func release_sender_buffer,
    libspdm_device_acquire_receiver_buffer_func acquire_receiver_buffer,
    libspdm_device_release_receiver_buffer_func release_receiver_buffer);

/**
 * Encode an SPDM or APP message to a transport layer message.
 *
 * For normal SPDM message, it adds the transport layer wrapper.
 * For secured SPDM message, it encrypts a secured message then adds the transport layer wrapper.
 * For secured APP message, it encrypts a secured message then adds the transport layer wrapper.
 *
 * The APP message is encoded to a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  session_id              Indicates if it is a secured message protected via SPDM session.
 *                                 If session_id is NULL, it is a normal message.
 *                                 If session_id is not NULL, it is a secured message.
 * @param  is_app_message          Indicates if it is an APP message or SPDM message.
 * @param  is_request_message      Indicates if it is a request message.
 * @param  message_size            Size in bytes of the message data buffer.
 * @param  message                 A pointer to a source buffer to store the message.
 *                                 For normal message, it shall point to the acquired sender buffer.
 *                                 For secured message, it shall point to the scratch buffer in spdm_context.
 * @param  transport_message_size  Size in bytes of the transport message data buffer.
 * @param  transport_message       A pointer to a destination buffer to store the transport message.
 *                                 On input, it shall be msg_buf_ptr from sender buffer.
 *                                 On output, it will point to acquired sender buffer.
 *
 * @retval RETURN_SUCCESS               The message is encoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 **/
typedef libspdm_return_t (*libspdm_transport_encode_message_func)(
    void *spdm_context, const uint32_t *session_id, bool is_app_message,
    bool is_request_message, size_t message_size,
    void *message, size_t *transport_message_size,
    void **transport_message);

/**
 * Decode an SPDM or APP message from a transport layer message.
 *
 * For normal SPDM message, it removes the transport layer wrapper,
 * For secured SPDM message, it removes the transport layer wrapper, then decrypts and verifies a secured message.
 * For secured APP message, it removes the transport layer wrapper, then decrypts and verifies a secured message.
 *
 * The APP message is decoded from a secured message directly in SPDM session.
 * The APP message format is defined by the transport layer.
 * Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  session_id              Indicates if it is a secured message protected via SPDM session.
 *                                 If session_id is NULL, it is a normal message.
 *                                 If session_id is not NULL, it is a secured message.
 * @param  is_app_message          Indicates if it is an APP message or SPDM message.
 * @param  is_request_message      Indicates if it is a request message.
 * @param  transport_message_size  Size in bytes of the transport message data buffer.
 * @param  transport_message       A pointer to a source buffer to store the transport message.
 *                                 For normal message or secured message, it shall point to acquired receiver buffer.
 * @param  message_size            Size in bytes of the message data buffer.
 * @param  message                 A pointer to a destination buffer to store the message.
 *                                 On input, it shall point to the scratch buffer in spdm_context.
 *                                 On output, for normal message, it will point to the original receiver buffer.
 *                                 On output, for secured message, it will point to the scratch buffer in spdm_context.
 *
 * @retval RETURN_SUCCESS               The message is decoded successfully.
 * @retval RETURN_INVALID_PARAMETER     The message is NULL or the message_size is zero.
 * @retval RETURN_UNSUPPORTED           The transport_message is unsupported.
 **/
typedef libspdm_return_t (*libspdm_transport_decode_message_func)(
    void *spdm_context, uint32_t **session_id,
    bool *is_app_message, bool is_request_message,
    size_t transport_message_size, void *transport_message,
    size_t *message_size, void **message);

/**
 * Return the maximum transport layer message header size.
 * Transport Message Header Size + sizeof(spdm_secured_message_cipher_header_t))
 *
 * For MCTP, Transport Message Header Size = sizeof(mctp_message_header_t)
 * For PCI_DOE, Transport Message Header Size = sizeof(pci_doe_data_object_header_t)
 *
 * @param  spdm_context  A pointer to the SPDM context.
 *
 * @return size of maximum transport layer message header size
 **/
typedef uint32_t (*libspdm_transport_get_header_size_func)(void *spdm_context);

/**
 * Register SPDM transport layer encode/decode functions for SPDM or APP messages.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context               A pointer to the SPDM context.
 * @param  transport_encode_message   The fuction to encode an SPDM or APP message to a transport layer message.
 * @param  transport_decode_message   The fuction to decode an SPDM or APP message from a transport layer message.
 **/
void libspdm_register_transport_layer_func(
    void *spdm_context,
    uint32_t max_spdm_msg_size,
    uint32_t transport_header_size,
    uint32_t transport_tail_size,
    libspdm_transport_encode_message_func transport_encode_message,
    libspdm_transport_decode_message_func transport_decode_message);

/**
 * Get the size of required scratch buffer.
 *
 * The SPDM Integrator must call libspdm_get_sizeof_required_scratch_buffer to get the size,
 * then allocate enough scratch buffer and call libspdm_set_scratch_buffer().
 *
 * @param  context  A pointer to the SPDM context.
 *
 * @return the size of required scratch buffer.
 **/
size_t libspdm_get_sizeof_required_scratch_buffer (void *spdm_context);

/**
 * Set the scratch buffer.
 *
 * The size of scratch buffer must be larger than the value returned in
 * libspdm_get_sizeof_required_scratch_buffer().
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  scratch_buffer       Buffer address of the scratch buffer.
 * @param  scratch_buffer_size  Size of the scratch buffer.
 *
 **/
void libspdm_set_scratch_buffer (
    void *spdm_context,
    void *scratch_buffer,
    size_t scratch_buffer_size);

/**
 * Get the scratch buffer.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  scratch_buffer       Buffer address of the scratch buffer.
 * @param  scratch_buffer_size  Size of the scratch buffer.
 *
 **/
void libspdm_get_scratch_buffer (
    void *spdm_context,
    void **scratch_buffer,
    size_t *scratch_buffer_size);

/**
 * Verify a SPDM cert chain in a slot.
 *
 * This function shall verify:
 *  1) The integrity of the certificate chain. (Root Cert Hash->Root Cert->Cert Chain)
 *  2) The trust anchor. (Root Cert Hash/Root cert matches the trust anchor)
 *
 * The function shall check the negotiated hash algorithm to check root cert hash.
 * The function shall check the negotiated (req) asym algorithm to determine if it is right cert chain.
 *
 * The function returns error if either of above is not satisfied.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  slot_id            The number of slot for the certificate chain.
 * @param  cert_chain_size    Indicate the size in bytes of the certificate chain.
 * @param  cert_chain         A pointer to the buffer storing the certificate chain
 *                            returned from GET_CERTIFICATE. It starts with spdm_cert_chain_t.
 * @param  trust_anchor       A buffer to hold the trust_anchor which is used to validate the peer certificate, if not NULL.
 * @param  trust_anchor_size  A buffer to hold the trust_anchor_size, if not NULL.
 *
 * @retval RETURN_SUCCESS                The cert chain verification pass.
 * @retval RETURN_SECURIY_VIOLATION      The cert chain verification fail.
 **/
typedef bool (*libspdm_verify_spdm_cert_chain_func)(
    void *spdm_context, uint8_t slot_id,
    size_t cert_chain_size, const void *cert_chain,
    const void **trust_anchor,
    size_t *trust_anchor_size);

/**
 * Register SPDM certificate verification functions for SPDM GET_CERTIFICATE in requester or responder.
 * It is called after GET_CERTIFICATE gets a full certificate chain from peer.
 *
 * If it is NOT registered, the default verification in SPDM lib will be used. It verifies:
 *  1) The integrity of the certificate chain, (Root Cert Hash->Root Cert->Cert Chain), according to X.509.
 *  2) The trust anchor, according LIBSPDM_DATA_PEER_PUBLIC_ROOT_CERT or LIBSPDM_DATA_PEER_PUBLIC_CERT_CHAIN.
 * If it is registered, SPDM lib will use this function to verify the certificate.
 *
 * This function must be called after libspdm_init_context, and before any SPDM communication.
 *
 * @param  spdm_context        A pointer to the SPDM context.
 * @param  verify_certificate  The fuction to verify an SPDM certificate after GET_CERTIFICATE.
 **/
void libspdm_register_verify_spdm_cert_chain_func(
    void *spdm_context,
    const libspdm_verify_spdm_cert_chain_func verify_spdm_cert_chain);

/**
 * This function gets the session info via session ID.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    The SPDM session ID.
 *
 * @return session info.
 **/
void *libspdm_get_session_info_via_session_id(void *spdm_context, uint32_t session_id);

/**
 * This function gets the secured message context via session ID.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    The SPDM session ID.
 *
 * @return secured message context.
 **/
void *libspdm_get_secured_message_context_via_session_id(void *spdm_context, uint32_t session_id);

/**
 * This function gets the secured message context via session ID.
 *
 * @param  spdm_session_info  A pointer to the SPDM context.
 *
 * @return secured message context.
 **/
void *libspdm_get_secured_message_context_via_session_info(void *spdm_session_info);

/**
 * This function returns peer certificate chain buffer including spdm_cert_chain_t header.
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  cert_chain_buffer       Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size  Size in bytes of the certificate chain buffer.
 *
 * @retval true  Peer certificate chain buffer including spdm_cert_chain_t header is returned.
 * @retval false Peer certificate chain buffer including spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_peer_cert_chain_buffer(void *spdm_context,
                                        const void **cert_chain_buffer,
                                        size_t *cert_chain_buffer_size);

/**
 * This function returns peer certificate chain data without spdm_cert_chain_t header.
 *
 * @param  spdm_context          A pointer to the SPDM context.
 * @param  cert_chain_data       Certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size  Size in bytes of the certificate chain data.
 *
 * @retval true  Peer certificate chain data without spdm_cert_chain_t header is returned.
 * @retval false Peer certificate chain data without spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_peer_cert_chain_data(void *spdm_context,
                                      const void **cert_chain_data,
                                      size_t *cert_chain_data_size);

/**
 * This function returns local used certificate chain buffer including spdm_cert_chain_t header.
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  cert_chain_buffer       Certificate chain buffer including spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size  Size in bytes of the certificate chain buffer.
 *
 * @retval true  Local used certificate chain buffer including spdm_cert_chain_t header is returned.
 * @retval false Local used certificate chain buffer including spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_local_cert_chain_buffer(void *spdm_context,
                                         const void **cert_chain_buffer,
                                         size_t *cert_chain_buffer_size);

/**
 * This function returns local used certificate chain data without spdm_cert_chain_t header.
 *
 * @param  spdm_context          A pointer to the SPDM context.
 * @param  cert_chain_data       Certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size  Size in bytes of the certificate chain data.
 *
 * @retval true  Local used certificate chain data without spdm_cert_chain_t header is returned.
 * @retval false Local used certificate chain data without spdm_cert_chain_t header is not found.
 **/
bool libspdm_get_local_cert_chain_data(void *spdm_context,
                                       const void **cert_chain_data,
                                       size_t *cert_chain_data_size);

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
                                        size_t *peer_public_key_buffer_size);

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
                                         size_t *local_public_key_buffer_size);

/**
 * byte3 - libspdm major version
 * byte2 - libspdm minor version
 * byte1 - libspdm patch version
 * byte0 - libspdm alpha
 *         (office release with tag: 0, release candidate with tag: 1, non official release: 0xFF)
 **/
uint32_t libspdm_module_version(void);

/*true: FIPS enabled, false: FIPS disabled*/
bool libspdm_get_fips_mode(void);

#endif /* SPDM_COMMON_LIB_H */
