/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_COMMON_LIB_INTERNAL_H
#define SPDM_COMMON_LIB_INTERNAL_H

#include "library/spdm_common_lib.h"
#include "library/spdm_secured_message_lib.h"
#include "library/spdm_return_status.h"
#include "library/spdm_crypt_lib.h"
#include "hal/library/debuglib.h"
#include "hal/library/memlib.h"
#include "hal/library/requester/reqasymsignlib.h"
#include "hal/library/requester/psklib.h"
#include "hal/library/responder/asymsignlib.h"
#include "hal/library/responder/csrlib.h"
#include "hal/library/responder/measlib.h"
#include "hal/library/responder/psklib.h"
#include "hal/library/responder/setcertlib.h"
#include "hal/library/cryptlib.h"

#define INVALID_SESSION_ID 0
#define LIBSPDM_MAX_CT_EXPONENT 31
#define LIBSPDM_MAX_RDT_EXPONENT 31

#define LIBSPDM_MAX_SPDM_SESSION_SEQUENCE_NUMBER 0xFFFFFFFFFFFFFFFFull

typedef struct {
    uint8_t spdm_version_count;
    spdm_version_number_t spdm_version[SPDM_MAX_VERSION_COUNT];
} libspdm_device_version_t;

typedef struct {
    uint8_t ct_exponent;
    uint64_t rtt;
    uint32_t st1;
    uint32_t flags;
    uint32_t data_transfer_size;
    uint32_t sender_data_transfer_size;
    uint32_t max_spdm_msg_size;
    uint32_t transport_header_size;
    uint32_t transport_tail_size;
} libspdm_device_capability_t;

typedef struct {
    uint8_t measurement_spec;
    uint8_t other_params_support;
    uint32_t measurement_hash_algo;
    uint32_t base_asym_algo;
    uint32_t base_hash_algo;
    uint16_t dhe_named_group;
    uint16_t aead_cipher_suite;
    uint16_t req_base_asym_alg;
    uint16_t key_schedule;
} libspdm_device_algorithm_t;

typedef struct {
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    uint8_t buffer[LIBSPDM_MAX_CERT_CHAIN_SIZE];
    size_t buffer_size;
#else
    uint8_t buffer_hash[LIBSPDM_MAX_HASH_SIZE];
    uint32_t buffer_hash_size;
    /* leaf cert public key of the peer */
    void *leaf_cert_public_key;
#endif
} libspdm_peer_used_cert_chain_t;

typedef struct {
    /* Local device info */
    libspdm_device_version_t version;
    libspdm_device_capability_t capability;
    libspdm_device_algorithm_t algorithm;
    libspdm_device_version_t secured_message_version;

    /* My Certificate */
    const void *local_cert_chain_provision[SPDM_MAX_SLOT_COUNT];
    size_t local_cert_chain_provision_size[SPDM_MAX_SLOT_COUNT];
    /* My raw public key (slot_id - 0xFF) */
    const void *local_public_key_provision;
    size_t local_public_key_provision_size;

    /* Peer Root Certificate */
    const void *peer_root_cert_provision[LIBSPDM_MAX_ROOT_CERT_SUPPORT];
    size_t peer_root_cert_provision_size[LIBSPDM_MAX_ROOT_CERT_SUPPORT];
    /* Peer raw public key (slot_id - 0xFF) */
    const void *peer_public_key_provision;
    size_t peer_public_key_provision_size;

    /* Peer Cert verify*/
    libspdm_verify_spdm_cert_chain_func verify_peer_spdm_cert_chain;

    /* Responder policy*/
    bool basic_mut_auth_requested;
    uint8_t mut_auth_requested;
    uint8_t heartbeat_period;

    /*The device role*/
    bool is_requester;
} libspdm_local_context_t;

typedef struct {
    /* Connection State */
    libspdm_connection_state_t connection_state;

    /* Peer device info (negotiated) */
    spdm_version_number_t version;
    libspdm_device_capability_t capability;
    libspdm_device_algorithm_t algorithm;
    spdm_version_number_t secured_message_version;

    /* Peer digests buffer */
    uint8_t peer_digest_slot_mask;
    uint8_t peer_total_digest_buffer[LIBSPDM_MAX_HASH_SIZE * SPDM_MAX_SLOT_COUNT];

    /* Peer CertificateChain */
    libspdm_peer_used_cert_chain_t peer_used_cert_chain[SPDM_MAX_SLOT_COUNT];
    uint8_t peer_used_cert_chain_slot_id;

    /* Local Used CertificateChain (for responder, or requester in mut auth) */
    const uint8_t *local_used_cert_chain_buffer;
    size_t local_used_cert_chain_buffer_size;
    uint8_t local_used_cert_chain_slot_id;

    /* Specifies whether the cached negotiated state should be invalidated. (responder only)
     * This is a "sticky" bit wherein if it is set to 1 then it cannot be set to 0. */
    uint8_t end_session_attributes;
} libspdm_connection_info_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    /*uint8_t   buffer[max_buffer_size];*/
} libspdm_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_VCA_BUFFER_SIZE];
} libspdm_vca_managed_buffer_t;

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT

/*
 * +--------------------------+------------------------------------------+---------+
 * | GET_DIGESTS 1.2          | 4                                        | 1       |
 * | DIGESTS 1.2              | 4 + H * SlotNum = [36, 516]              | [1, 18] |
 * +--------------------------+------------------------------------------+---------+
 * | GET_CERTIFICATE 1.2      | 8                                        | 1       |
 * | CERTIFICATE 1.2          | 8 + PortionLen                           | [1, ]   |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_B_BUFFER_SIZE (24 + \
                                           LIBSPDM_MAX_HASH_SIZE * SPDM_MAX_SLOT_COUNT + \
                                           LIBSPDM_MAX_CERT_CHAIN_SIZE)

/*
 * +--------------------------+------------------------------------------+---------+
 * | CHALLENGE 1.2            | 40                                       | 1       |
 * | CHALLENGE_AUTH 1.2       | 38 + H * 2 + S [+ O] = [166, 678]        | [6, 23] |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_C_BUFFER_SIZE (78 + \
                                           LIBSPDM_MAX_HASH_SIZE * 2 + \
                                           LIBSPDM_MAX_ASYM_KEY_SIZE + SPDM_MAX_OPAQUE_DATA_SIZE)

/*
 * +--------------------------+------------------------------------------+---------+
 * | GET_MEASUREMENTS 1.2     | 5 + Nonce (0 or 32)                      | 1       |
 * | MEASUREMENTS 1.2         | 42 + MeasRecLen (+ S) [+ O] = [106, 554] | [4, 19] |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_M_BUFFER_SIZE (47 + SPDM_NONCE_SIZE + \
                                           LIBSPDM_MAX_MEASUREMENT_RECORD_SIZE + \
                                           LIBSPDM_MAX_ASYM_KEY_SIZE + SPDM_MAX_OPAQUE_DATA_SIZE)

/*
 * +--------------------------+------------------------------------------+---------+
 * | KEY_EXCHANGE 1.2         | 42 + D [+ O] = [106, 554]                | [4, 19] |
 * | KEY_EXCHANGE_RSP 1.2     | 42 + D + H + S (+ H) [+ O] = [234, 1194] | [8, 40] |
 * +--------------------------+------------------------------------------+---------+
 * | PSK_EXCHANGE 1.2         | 12 [+ PSKHint] + R [+ O] = 44            | 2       |
 * | PSK_EXCHANGE_RSP 1.2     | 12 + R + H (+ H) [+ O] = [108, 172]      | [4, 6]  |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_K_BUFFER_SIZE (84 + LIBSPDM_MAX_DHE_KEY_SIZE * 2 + \
                                           LIBSPDM_MAX_HASH_SIZE * 2 + LIBSPDM_MAX_ASYM_KEY_SIZE + \
                                           SPDM_MAX_OPAQUE_DATA_SIZE * 2)

/*
 * +--------------------------+------------------------------------------+---------+
 * | FINISH 1.2               | 4 (+ S) + H = [100, 580]                 | [4, 20] |
 * | FINISH_RSP 1.2           | 4 (+ H) = [36, 69]                       | [1, 3]  |
 * +--------------------------+------------------------------------------+---------+
 * | PSK_FINISH 1.2           | 4 + H = [36, 68]                         | [1, 3]  |
 * | PSK_FINISH_RSP 1.2       | 4                                        | 1       |
 * +--------------------------+------------------------------------------+---------+
 */
#define LIBSPDM_MAX_MESSAGE_F_BUFFER_SIZE (8 + LIBSPDM_MAX_HASH_SIZE * 2 + \
                                           LIBSPDM_MAX_ASYM_KEY_SIZE)

#define LIBSPDM_MAX_MESSAGE_L1L2_BUFFER_SIZE \
    (LIBSPDM_MAX_MESSAGE_VCA_BUFFER_SIZE + LIBSPDM_MAX_MESSAGE_M_BUFFER_SIZE)

#define LIBSPDM_MAX_MESSAGE_M1M2_BUFFER_SIZE \
    (LIBSPDM_MAX_MESSAGE_VCA_BUFFER_SIZE + \
     LIBSPDM_MAX_MESSAGE_B_BUFFER_SIZE + LIBSPDM_MAX_MESSAGE_C_BUFFER_SIZE)

#define LIBSPDM_MAX_MESSAGE_TH_BUFFER_SIZE \
    (LIBSPDM_MAX_MESSAGE_VCA_BUFFER_SIZE + \
     LIBSPDM_MAX_CERT_CHAIN_SIZE + LIBSPDM_MAX_MESSAGE_K_BUFFER_SIZE + \
     LIBSPDM_MAX_CERT_CHAIN_SIZE + LIBSPDM_MAX_MESSAGE_F_BUFFER_SIZE)

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_B_BUFFER_SIZE];
} libspdm_message_b_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_C_BUFFER_SIZE];
} libspdm_message_c_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_M_BUFFER_SIZE];
} libspdm_message_m_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_K_BUFFER_SIZE];
} libspdm_message_k_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_F_BUFFER_SIZE];
} libspdm_message_f_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_L1L2_BUFFER_SIZE];
} libspdm_l1l2_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_M1M2_BUFFER_SIZE];
} libspdm_m1m2_managed_buffer_t;

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_MESSAGE_TH_BUFFER_SIZE];
} libspdm_th_managed_buffer_t;

#endif /* LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT */

typedef struct {
    size_t max_buffer_size;
    size_t buffer_size;
    uint8_t buffer[LIBSPDM_MAX_CERT_CHAIN_SIZE];
} libspdm_cert_chain_managed_buffer_t;

/* signature = Sign(SK, hash(M1))
 * Verify(PK, hash(M2), signature)*/

/* M1/M2 = Concatenate (A, B, C)
 * A = Concatenate (GET_VERSION, VERSION, GET_CAPABILITIES, CAPABILITIES, NEGOTIATE_ALGORITHMS, ALGORITHMS)
 * B = Concatenate (GET_DIGEST, DIGEST, GET_CERTIFICATE, CERTIFICATE)
 * C = Concatenate (CHALLENGE, CHALLENGE_AUTH\signature)*/

/* Mut M1/M2 = Concatenate (MutB, MutC)
 * MutB = Concatenate (GET_DIGEST, DIGEST, GET_CERTIFICATE, CERTIFICATE)
 * MutC = Concatenate (CHALLENGE, CHALLENGE_AUTH\signature)*/

/* signature = Sign(SK, hash(L1))
 * Verify(PK, hash(L2), signature)*/

/* L1/L2 = Concatenate (M)
 * M = Concatenate (GET_MEASUREMENT, MEASUREMENT\signature)*/

typedef struct {
    /* the message_a must be plan text because we do not know the algorithm yet.*/
    libspdm_vca_managed_buffer_t message_a;
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_message_b_managed_buffer_t message_b;
    libspdm_message_c_managed_buffer_t message_c;
    libspdm_message_b_managed_buffer_t message_mut_b;
    libspdm_message_c_managed_buffer_t message_mut_c;
    libspdm_message_m_managed_buffer_t message_m;
#else
    void *digest_context_m1m2;
    void *digest_context_mut_m1m2;
    void *digest_context_l1l2;
#endif
} libspdm_transcript_t;

/* TH for KEY_EXCHANGE response signature: Concatenate (A, Ct, K)
 * Ct = certificate chain
 * K  = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response\signature+verify_data)*/

/* TH for KEY_EXCHANGE response HMAC: Concatenate (A, Ct, K)
 * Ct = certificate chain
 * K  = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response\verify_data)*/

/* TH for FINISH request signature: Concatenate (A, Ct, K, CM, F)
 * Ct = certificate chain
 * K  = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response)*/
/* CM = mutual certificate chain *
 * F  = Concatenate (FINISH request\signature+verify_data)*/

/* TH for FINISH response HMAC: Concatenate (A, Ct, K, CM, F)
 * Ct = certificate chain
 * K = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response)*/
/* CM = mutual certificate chain *
 * F = Concatenate (FINISH request\verify_data)*/

/* th1: Concatenate (A, Ct, K)
 * Ct = certificate chain
 * K  = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response)*/

/* th2: Concatenate (A, Ct, K, CM, F)
 * Ct = certificate chain
 * K  = Concatenate (KEY_EXCHANGE request, KEY_EXCHANGE response)*/
/* CM = mutual certificate chain *
 * F  = Concatenate (FINISH request, FINISH response)*/

/* TH for PSK_EXCHANGE response HMAC: Concatenate (A, K)
 * K  = Concatenate (PSK_EXCHANGE request, PSK_EXCHANGE response\verify_data)*/

/* TH for PSK_FINISH response HMAC: Concatenate (A, K, F)
 * K  = Concatenate (PSK_EXCHANGE request, PSK_EXCHANGE response)
 * F  = Concatenate (PSK_FINISH request\verify_data)*/

/* TH1_PSK1: Concatenate (A, K)
 * K  = Concatenate (PSK_EXCHANGE request, PSK_EXCHANGE response\verify_data)*/

/* TH1_PSK2: Concatenate (A, K, F)
 * K  = Concatenate (PSK_EXCHANGE request, PSK_EXCHANGE response)
 * F  = Concatenate (PSK_FINISH request\verify_data)*/

/* TH2_PSK: Concatenate (A, K, F)
 * K  = Concatenate (PSK_EXCHANGE request, PSK_EXCHANGE response)
 * F  = Concatenate (PSK_FINISH request, PSK_FINISH response)*/

typedef struct {
#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
    libspdm_message_k_managed_buffer_t message_k;
    libspdm_message_f_managed_buffer_t message_f;
    libspdm_message_m_managed_buffer_t message_m;
#else
    bool message_f_initialized;
    void *digest_context_th;
    void *digest_context_l1l2;
    /* this is back up for message F reset.*/
    void *digest_context_th_backup;
#endif
} libspdm_session_transcript_t;

typedef struct {
    uint32_t session_id;
    bool use_psk;
    uint8_t mut_auth_requested;
    uint8_t end_session_attributes;
    uint8_t session_policy;
    uint8_t heartbeat_period;
    libspdm_session_transcript_t session_transcript;
    /* Register for the last KEY_UPDATE token and operation (responder only)*/
    spdm_key_update_request_t last_key_update_request;
    void *secured_message_context;
} libspdm_session_info_t;

#define LIBSPDM_MAX_ENCAP_REQUEST_OP_CODE_SEQUENCE_COUNT 3
typedef struct {
    /* Valid OpCode: GET_DIEGST/GET_CERTIFICATE/CHALLENGE/KEY_UPDATE
     * The last one is 0x00, as terminator.*/
    uint8_t request_op_code_sequence[LIBSPDM_MAX_ENCAP_REQUEST_OP_CODE_SEQUENCE_COUNT + 1];
    uint8_t request_op_code_count;
    uint8_t current_request_op_code;
    uint8_t request_id;
    uint8_t req_slot_id;
    spdm_message_header_t last_encap_request_header;
    size_t last_encap_request_size;
    uint16_t cert_chain_total_len;
} libspdm_encap_context_t;

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
typedef struct {
    bool chunk_in_use;
    uint8_t chunk_handle;
    uint16_t chunk_seq_no;
    size_t chunk_bytes_transferred;

    void* large_message;
    size_t large_message_size;
    size_t large_message_capacity;
} libspdm_chunk_info_t;

typedef struct {
    libspdm_chunk_info_t send;
    libspdm_chunk_info_t get;
} libspdm_chunk_context_t;
#endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

#if LIBSPDM_ENABLE_MSG_LOG
typedef struct {
    void *buffer;
    size_t max_buffer_size;
    uint32_t mode;
    size_t buffer_size;
    uint32_t status;
} libspdm_msg_log_t;
#endif /* LIBSPDM_ENABLE_MSG_LOG */

#if LIBSPDM_FIPS_MODE
typedef struct {
    /**
     * Tested algo flag: 0 represents that the algo is not tested.
     * See LIBSPDM_FIPS_SELF_TEST_xxx;
     **/
    uint32_t tested_algo;
    /**
     * Flag for the result of run algo self_test, 0 represents the result is failed.
     * See LIBSPDM_FIPS_SELF_TEST_xxx;
     **/
    uint32_t self_test_result;
} libspdm_fips_selftest_context;
#endif /* LIBSPDM_FIPS_MODE */

#define LIBSPDM_CONTEXT_STRUCT_VERSION 0x3

typedef struct {
    uint32_t version;

    /* IO information */
    libspdm_device_send_message_func send_message;
    libspdm_device_receive_message_func receive_message;

    /*
     * reserved for request and response in the main dispatch function in SPDM responder.
     * this buffer is the transport message received from spdm_context->receive_message()
     * or sent to spdm_context->send_message().
     * This message may be SPDM transport message or secured SPDM transport message.
     **/
    libspdm_device_acquire_sender_buffer_func acquire_sender_buffer;
    libspdm_device_release_sender_buffer_func release_sender_buffer;
    libspdm_device_acquire_receiver_buffer_func acquire_receiver_buffer;
    libspdm_device_release_receiver_buffer_func release_receiver_buffer;

    /* Transport Layer information */
    libspdm_transport_encode_message_func transport_encode_message;
    libspdm_transport_decode_message_func transport_decode_message;

    /* Cached plain text command
     * If the command is cipher text, decrypt then cache it. */
    uint8_t *last_spdm_request;
    size_t last_spdm_request_size;

    /* scratch buffer */
    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;
    /* sender buffer */
    uint8_t *sender_buffer;
    size_t sender_buffer_size;
    /* receiver buffer */
    uint8_t *receiver_buffer;
    size_t receiver_buffer_size;

    /* Cache session_id in this spdm_message, only valid for secured message. */
    uint32_t last_spdm_request_session_id;
    bool last_spdm_request_session_id_valid;

    /* Cache the error in libspdm_process_request. It is handled in libspdm_build_response. */
    libspdm_error_struct_t last_spdm_error;

    /* Register GetResponse function (responder only) */
    void *get_response_func;

    /* Register GetEncapResponse function (requester only) */
    void *get_encap_response_func;
    libspdm_encap_context_t encap_context;

    /* Register spdm_session_state_callback function (responder only)
    * Register can know the state after StartSession / EndSession. */
    void *spdm_session_state_callback;

    /* Register spdm_connection_state_callback function (responder only)
     * Register can know the connection state such as negotiated. */
    void *spdm_connection_state_callback;

    /* Register libspdm_key_update_callback function (responder only)
     * Register can know when session keys are updated during KEY_UPDATE operations. */
    void *spdm_key_update_callback;

    libspdm_local_context_t local_context;

    libspdm_connection_info_t connection_info;
    libspdm_transcript_t transcript;

    libspdm_session_info_t session_info[LIBSPDM_MAX_SESSION_COUNT];

    /* Buffer that the Responder uses to store the Requester's certificate chain for
     * mutual authentication. */
    void *mut_auth_cert_chain_buffer;
    size_t mut_auth_cert_chain_buffer_size;
    size_t mut_auth_cert_chain_buffer_max_size;

    /* Cache latest session ID for HANDSHAKE_IN_THE_CLEAR */
    uint32_t latest_session_id;

    /* Register for Responder state, be initial to Normal (responder only) */
    libspdm_response_state_t response_state;

    /* Cached data for SPDM_ERROR_CODE_RESPONSE_NOT_READY/SPDM_RESPOND_IF_READY */
    spdm_error_data_response_not_ready_t error_data;
#if LIBSPDM_RESPOND_IF_READY_SUPPORT
    uint8_t *cache_spdm_request;
    size_t cache_spdm_request_size;
#endif
    uint8_t current_token;

    /* Register for the retry times when receive "BUSY" Error response (requester only) */
    uint8_t retry_times;
    /* Register for the delay time in microseconds between retry requests
     * when receive "BUSY" Error response (requester only) */
    uint64_t retry_delay_time;
    bool crypto_request;

    /* App context data for use by application */
    void *app_context_data_ptr;

    /* See LIBSPDM_DATA_HANDLE_ERROR_RETURN_POLICY_*. */
    uint8_t handle_error_return_policy;

    /* Max session count for DHE session and PSK session
     * Set via LIBSPDM_DATA_MAX_DHE_SESSION_COUNT and LIBSPDM_DATA_MAX_PSK_SESSION_COUNT */
    uint32_t max_dhe_session_count;
    uint32_t max_psk_session_count;

    /* Current session count for DHE session and PSK session */
    uint32_t current_dhe_session_count;
    uint32_t current_psk_session_count;

    /* see LIBSPDM_DATA_MAX_SPDM_SESSION_SEQUENCE_NUMBER */
    uint64_t max_spdm_session_sequence_number;

    uint8_t sequence_number_endian;

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
    /* Chunk specific context */
    libspdm_chunk_context_t chunk_context;
#endif /* LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP */

#if LIBSPDM_ENABLE_MSG_LOG
    libspdm_msg_log_t msg_log;
#endif /* LIBSPDM_ENABLE_MSG_LOG */

#if LIBSPDM_FIPS_MODE
    libspdm_fips_selftest_context fips_selftest_context;
#endif /* LIBSPDM_FIPS_MODE */

    /* Endianness (BE/LE/Both) to use for signature verification on SPDM 1.0 and 1.1
     * This field is ignored for other SPDM versions */
    uint8_t spdm_10_11_verify_signature_endian;

} libspdm_context_t;

#define LIBSPDM_CONTEXT_SIZE_WITHOUT_SECURED_CONTEXT (sizeof(libspdm_context_t))
#define LIBSPDM_CONTEXT_SIZE_ALL (LIBSPDM_CONTEXT_SIZE_WITHOUT_SECURED_CONTEXT + \
                                  LIBSPDM_SECURED_MESSAGE_CONTEXT_SIZE * LIBSPDM_MAX_SESSION_COUNT)

#if LIBSPDM_DEBUG_PRINT_ENABLE
/**
 * Return the request code name based on given request code.
 *
 * @param  request_code                  The SPDM request code.
 *
 * @return request code name according to the request code.
 **/
const char *libspdm_get_code_str(uint8_t request_code);

#ifdef LIBSPDM_INTERNAL_DUMP_HEX_STR_OVERRIDE
extern void LIBSPDM_INTERNAL_DUMP_HEX_STR_OVERRIDE(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_HEX_STR(data, size) LIBSPDM_INTERNAL_DUMP_HEX_STR_OVERRIDE(data, size)
#else
/**
 * This function dump raw data.
 *
 * @param  data  raw data
 * @param  size  raw data size
 **/
void libspdm_internal_dump_hex_str(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_HEX_STR(data, size) libspdm_internal_dump_hex_str(data, size)
#endif /* LIBSPDM_INTERNAL_DUMP_HEX_STR_OVERRIDE */

#ifdef LIBSPDM_INTERNAL_DUMP_DATA_OVERRIDE
extern void LIBSPDM_INTERNAL_DUMP_DATA_OVERRIDE(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_DATA(data, size) LIBSPDM_INTERNAL_DUMP_DATA_OVERRIDE(data, size)
#else
/**
 * This function dump raw data.
 *
 * @param  data  raw data
 * @param  size  raw data size
 **/
void libspdm_internal_dump_data(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_DATA(data, size) libspdm_internal_dump_data(data, size)
#endif /* LIBSPDM_INTERNAL_DUMP_DATA_OVERRIDE */

#ifdef LIBSPDM_INTERNAL_DUMP_HEX_OVERRIDE
extern void LIBSPDM_INTERNAL_DUMP_HEX_OVERRIDE(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_HEX(data, size) LIBSPDM_INTERNAL_DUMP_HEX_OVERRIDE(data, size)
#else
/**
 * This function dump raw data with column format.
 *
 * @param  data  raw data
 * @param  size  raw data size
 **/
void libspdm_internal_dump_hex(const uint8_t *data, size_t size);
#define LIBSPDM_INTERNAL_DUMP_HEX(data, size) libspdm_internal_dump_hex(data, size)
#endif /* LIBSPDM_INTERNAL_DUMP_HEX_OVERRIDE */

#else /* LIBSPDM_DEBUG_PRINT_ENABLE */
#define LIBSPDM_INTERNAL_DUMP_HEX(data, size)
#define LIBSPDM_INTERNAL_DUMP_HEX_STR(data, size)
#define LIBSPDM_INTERNAL_DUMP_DATA(data, size)
#endif /* LIBSPDM_DEBUG_PRINT_ENABLE */

/* Required scratch buffer size for libspdm internal usage.
 * It may be used to hold the encrypted/decrypted message and/or last sent/received message.
 * It may be used to hold the large request/response and intermediate send/receive buffer
 * in case of chunking.
 *
 * If chunking is not supported, it should be at least below.
 * +--------------------------+-----------------+-----------------+
 * |    SENDER_RECEIVER       |MAX_SPDM_MSG_SIZE|MAX_SPDM_MSG_SIZE|
 * +--------------------------+-----------------+-----------------+
 * |<-Snd/Rcv buf for chunk ->|<-last request ->|<-cache request->|
 *
 *
 * If chunking is supported, it should be at least below.
 * +---------------+--------------+--------------------------+------------------------------+-----------------+-----------------+
 * |SECURE_MESSAGE |LARGE_MESSAGE |    SENDER_RECEIVER       | LARGE SENDER_RECEIVER        |MAX_SPDM_MSG_SIZE|MAX_SPDM_MSG_SIZE|
 * +---------------+--------------+--------------------------+------------------------------+-----------------+-----------------+
 * |<-Secure msg ->|<-Large msg ->|<-Snd/Rcv buf for chunk ->|<-Snd/Rcv buf for large msg ->|<-last request ->|<-cache request->|
 *
 *
 * The value is configurable based on max_spdm_msg_size.
 * The value MAY be changed in different libspdm version.
 * It is exposed here, just in case the libspdm consumer wants to configure the setting at build time.
 */
#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
/* first section */
uint32_t libspdm_get_scratch_buffer_secure_message_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_secure_message_capacity(libspdm_context_t *spdm_context);

/* second section */
uint32_t libspdm_get_scratch_buffer_large_message_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_large_message_capacity(libspdm_context_t *spdm_context);
#endif

/* third section */
uint32_t libspdm_get_scratch_buffer_sender_receiver_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_sender_receiver_capacity(libspdm_context_t *spdm_context);

#if LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP
/* fourth section */
uint32_t libspdm_get_scratch_buffer_large_sender_receiver_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_large_sender_receiver_capacity(libspdm_context_t *spdm_context);
#endif

/* fifth section */
uint32_t libspdm_get_scratch_buffer_last_spdm_request_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_last_spdm_request_capacity(libspdm_context_t *spdm_context);

#if LIBSPDM_RESPOND_IF_READY_SUPPORT
/* sixth section */
uint32_t libspdm_get_scratch_buffer_cache_spdm_request_offset(libspdm_context_t *spdm_context);
uint32_t libspdm_get_scratch_buffer_cache_spdm_request_capacity(libspdm_context_t *spdm_context);
#endif

/* combination */
uint32_t libspdm_get_scratch_buffer_capacity(libspdm_context_t *spdm_context);

/**
 * Append a new data buffer to the managed buffer.
 *
 * @param  managed_buffer                The managed buffer to be appended.
 * @param  buffer                       The address of the data buffer to be appended to the managed buffer.
 * @param  buffer_size                   The size in bytes of the data buffer to be appended to the managed buffer.
 *
 * @retval RETURN_SUCCESS               The new data buffer is appended to the managed buffer.
 * @retval RETURN_BUFFER_TOO_SMALL      The managed buffer is too small to be appended.
 **/
libspdm_return_t libspdm_append_managed_buffer(void *managed_buffer,
                                               const void *buffer, size_t buffer_size);

/**
 * Reset the managed buffer.
 * The buffer_size is reset to 0.
 * The max_buffer_size is unchanged.
 * The buffer is not freed.
 *
 * @param  managed_buffer                The managed buffer to be shrinked.
 **/
void libspdm_reset_managed_buffer(void *managed_buffer);

/**
 * Return the size of managed buffer.
 *
 * @param  managed_buffer                The managed buffer.
 *
 * @return the size of managed buffer.
 **/
size_t libspdm_get_managed_buffer_size(void *managed_buffer);

/**
 * Return the address of managed buffer.
 *
 * @param  managed_buffer                The managed buffer.
 *
 * @return the address of managed buffer.
 **/
void *libspdm_get_managed_buffer(void *managed_buffer);

/**
 * Init the managed buffer.
 *
 * @param  managed_buffer                The managed buffer.
 * @param  max_buffer_size                The maximum size in bytes of the managed buffer.
 **/
void libspdm_init_managed_buffer(void *managed_buffer, size_t max_buffer_size);

/**
 * Reset message buffer in SPDM context according to request code.
 *
 * @param  spdm_context                   A pointer to the SPDM context.
 * @param  spdm_session_info             A pointer to the SPDM session context.
 * @param  spdm_request                   The SPDM request code.
 */
void libspdm_reset_message_buffer_via_request_code(void *context, void *session_info,
                                                   uint8_t request_code);

/**
 * This function initializes the session info.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    The SPDM session ID.
 **/
void libspdm_session_info_init(libspdm_context_t *spdm_context,
                               libspdm_session_info_t *session_info,
                               uint32_t session_id, bool use_psk);

/**
 * Set the psk_hint to a session info.
 *
 * @param  session_info                  A pointer to a session info.
 * @param  psk_hint                      Indicate the PSK hint.
 * @param  psk_hint_size                  The size in bytes of the PSK hint.
 */
void libspdm_session_info_set_psk_hint(libspdm_session_info_t *session_info,
                                       const void *psk_hint,
                                       size_t psk_hint_size);

/**
 * This function returns if a given version is supported based upon the GET_VERSION/VERSION.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  version                      The SPDM version.
 *
 * @retval true  the version is supported.
 * @retval false the version is not supported.
 **/
bool libspdm_is_version_supported(const libspdm_context_t *spdm_context, uint8_t version);

/**
 * This function returns connection version negotiated by GET_VERSION/VERSION.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 *
 * @return the connection version.
 **/
uint8_t libspdm_get_connection_version(const libspdm_context_t *spdm_context);

/**
 * This function returns if a capabilities flag is supported in current SPDM connection.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  is_requester                  Is the function called from a requester.
 * @param  requester_capabilities_flag    The requester capabilities flag to be checked
 * @param  responder_capabilities_flag    The responder capabilities flag to be checked
 *
 * @retval true  the capabilities flag is supported.
 * @retval false the capabilities flag is not supported.
 **/
bool libspdm_is_capabilities_flag_supported(const libspdm_context_t *spdm_context,
                                            bool is_requester,
                                            uint32_t requester_capabilities_flag,
                                            uint32_t responder_capabilities_flag);

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
                                      size_t slot_id, uint8_t *hash);

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
                                      uint8_t *hash);

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
                                                     size_t cert_chain_buffer_size);

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
                                                     size_t *trust_anchor_size);
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
                                               uint8_t *signature);

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
                                           size_t certificate_chain_hash_size);

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
                                    size_t public_key_hash_size);

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
                                             size_t sign_data_size);

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
uint32_t libspdm_get_measurement_summary_hash_size(libspdm_context_t *spdm_context,
                                                   bool is_requester,
                                                   uint8_t measurement_summary_hash_type);

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
                            libspdm_l1l2_managed_buffer_t *l1l2);
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
                                 size_t *l1l2_hash_size, void *l1l2_hash);
#endif /* LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT */

/**
 * Get element from multi element opaque data by element id.
 *
 * This function should be called in
 * libspdm_process_opaque_data_supported_version_data/libspdm_process_opaque_data_version_selection_data.
 *
 * @param[in]  data_in_size     Size of multi element opaque data.
 * @param[in]  data_in          A pointer to the multi element opaque data.
 * @param[in]  element_id       Element id.
 * @param[in]  sm_data_id       ID for the Secured Message data type.
 * @param[out] get_element_ptr  Pointer to store finded element.
 *
 * @retval true   Get element successfully
 * @retval false  Get element failed
 **/
bool libspdm_get_element_from_opaque_data(libspdm_context_t *spdm_context,
                                          size_t data_in_size, const void *data_in,
                                          uint8_t element_id, uint8_t sm_data_id,
                                          const void **get_element_ptr, size_t *get_element_len);

/**
 *  Process general opaque data check
 *
 * @param  data_in_size                  size in bytes of the data_in.
 * @param  data_in                       A pointer to the buffer to store the opaque data version selection.
 *
 * @retval true                           check opaque data successfully
 * @retval false                          check opaque data failed
 **/
bool libspdm_process_general_opaque_data_check(libspdm_context_t *spdm_context,
                                               size_t data_in_size,
                                               const void *data_in);

/**
 * Return the size in bytes of opaque data supported version.
 *
 * This function should be called in libspdm_process_opaque_data_supported_version_data.
 *
 * @param  version_count  Secure version count.
 *
 * @return The size in bytes of opaque data supported version.
 **/
size_t libspdm_get_untrusted_opaque_data_supported_version_data_size(
    libspdm_context_t *spdm_context, uint8_t version_count);

/**
 * Return the size in bytes of opaque data supported version.
 *
 * This function should be called in KEY_EXCHANGE/PSK_EXCHANGE request generation.
 *
 * @return the size in bytes of opaque data supported version.
 **/
size_t libspdm_get_opaque_data_supported_version_data_size(libspdm_context_t *spdm_context);

/**
 * Return the size in bytes of opaque data version selection.
 *
 * This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response generation.
 *
 * @return the size in bytes of opaque data version selection.
 **/
size_t libspdm_get_opaque_data_version_selection_data_size(const libspdm_context_t *spdm_context);

/**
 * Return the SPDMversion field of the version number struct.
 *
 * @param  ver                Spdm version number struct.
 *
 * @return the SPDMversion of the version number struct.
 **/
uint8_t libspdm_get_version_from_version_number(const spdm_version_number_t ver);

/**
 * Sort SPDMversion in descending order.
 *
 * @param  spdm_context                A pointer to the SPDM context.
 * @param  ver_set                    A pointer to the version set.
 * @param  ver_num                    Version number.
 */
void libspdm_version_number_sort(spdm_version_number_t *ver_set, size_t ver_num);

/**
 * Negotiate SPDMversion for connection.
 * ver_set is the local version set of requester, res_ver_set is the version set of responder.
 *
 * @param  common_version             A pointer to store the common version.
 * @param  req_ver_set                A pointer to the requester version set.
 * @param  req_ver_num                Version number of requester.
 * @param  res_ver_set                A pointer to the responder version set.
 * @param  res_ver_num                Version number of responder.
 *
 * @retval true                       Negotiation successfully, connect version be saved to common_version.
 * @retval false                      Negotiation failed.
 */
bool libspdm_negotiate_connection_version(spdm_version_number_t *common_version,
                                          spdm_version_number_t *req_ver_set,
                                          size_t req_ver_num,
                                          const spdm_version_number_t *res_ver_set,
                                          size_t res_ver_num);

/**
 * Acquire a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  max_msg_size                  size in bytes of the maximum size of sender buffer.
 * @param  msg_buf_ptr                   A pointer to a sender buffer.
 *
 * @retval RETURN_SUCCESS               The sender buffer is acquired.
 **/
libspdm_return_t libspdm_acquire_sender_buffer (
    libspdm_context_t *spdm_context, size_t *max_msg_size, void **msg_buf_ptr);

/**
 * Release a device sender buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The sender buffer is Released.
 **/
void libspdm_release_sender_buffer (libspdm_context_t *spdm_context);

/**
 * Get the sender buffer.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  sender_buffer            Buffer address of the sender buffer.
 * @param  sender_buffer_size       Size of the sender buffer.
 *
 **/
void libspdm_get_sender_buffer (
    libspdm_context_t *spdm_context,
    void **sender_buffer,
    size_t *sender_buffer_size);

/**
 * Acquire a device receiver buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 * @param  max_msg_size                  size in bytes of the maximum size of receiver buffer.
 * @param  msg_buf_pt                    A pointer to a receiver buffer.
 *
 * @retval RETURN_SUCCESS               The receiver buffer is acquired.
 **/
libspdm_return_t libspdm_acquire_receiver_buffer (
    libspdm_context_t *spdm_context, size_t *max_msg_size, void **msg_buf_ptr);

/**
 * Release a device receiver buffer for transport layer message.
 *
 * @param  context                       A pointer to the SPDM context.
 *
 * @retval RETURN_SUCCESS               The receiver buffer is Released.
 **/
void libspdm_release_receiver_buffer (libspdm_context_t *spdm_context);

/**
 * Get the receiver buffer.
 *
 * @param  context                  A pointer to the SPDM context.
 * @param  receiver_buffer            Buffer address of the receiver buffer.
 * @param  receiver_buffer_size       Size of the receiver buffer.
 *
 **/
void libspdm_get_receiver_buffer (
    libspdm_context_t *spdm_context,
    void **receiver_buffer,
    size_t *receiver_buffer_size);

/**
 * Get the certificate slot mask
 *
 * @param[in]   context              A pointer to the SPDM context.
 *
 * @retval slot_mask                 get slot mask
 **/
uint8_t libspdm_get_cert_slot_mask (libspdm_context_t *spdm_context);

/**
 * Get the certificate slot count
 *
 * @param[in]   context              A pointer to the SPDM context.
 *
 * @retval slot_count                get slot count
 **/
uint8_t libspdm_get_cert_slot_count(libspdm_context_t *spdm_context);

#if LIBSPDM_ENABLE_MSG_LOG
void libspdm_append_msg_log(libspdm_context_t *spdm_context, void *message, size_t message_size);
#endif

/**
 * Reset message A cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 **/
void libspdm_reset_message_a(libspdm_context_t *spdm_context);

/**
 * Reset message B cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 **/
void libspdm_reset_message_b(libspdm_context_t *spdm_context);

/**
 * Reset message C cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 **/
void libspdm_reset_message_c(libspdm_context_t *spdm_context);

/**
 * Reset message MutB cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 **/
void libspdm_reset_message_mut_b(libspdm_context_t *spdm_context);

/**
 * Reset message MutC cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 **/
void libspdm_reset_message_mut_c(libspdm_context_t *spdm_context);

/**
 * Reset message M cache in SPDM context.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_info  A pointer to the SPDM session context.
 **/
void libspdm_reset_message_m(libspdm_context_t *spdm_context, void *session_info);

/**
 * Reset message K cache in SPDM context.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  spdm_session_info  A pointer to the SPDM session context.
 **/
void libspdm_reset_message_k(libspdm_context_t *spdm_context, void *spdm_session_info);

/**
 * Reset message F cache in SPDM context.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  spdm_session_info  A pointer to the SPDM session context.
 **/
void libspdm_reset_message_f(libspdm_context_t *spdm_context, void *spdm_session_info);

/**
 * Append message A cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_a(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size);
/**
 * Append message B cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_b(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size);

/**
 * Append message C cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_c(libspdm_context_t *spdm_context, const void *message,
                                          size_t message_size);

/**
 * Append message MutB cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_mut_b(libspdm_context_t *spdm_context, const void *message,
                                              size_t message_size);

/**
 * Append message MutC cache in SPDM context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_mut_c(libspdm_context_t *spdm_context, const void *message,
                                              size_t message_size);

/**
 * Append message M cache in SPDM context.
 * If session_info is NULL, this function will use M cache of SPDM context,
 * else will use M cache of SPDM session context.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_info  A pointer to the SPDM session context.
 * @param  message       Message buffer.
 * @param  message_size  Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_m(libspdm_context_t *spdm_context,
                                          void *session_info,
                                          const void *message, size_t message_size);

/**
 * Append message K cache in SPDM context.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  spdm_session_info  A pointer to the SPDM session context.
 * @param  is_requester       Indicate of the key generation for a requester or a responder.
 * @param  message            Message buffer.
 * @param  message_size       Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_k(libspdm_context_t *spdm_context,
                                          void *spdm_session_info,
                                          bool is_requester, const void *message,
                                          size_t message_size);

/**
 * Append message F cache in SPDM context.
 *
 * @param  spdm_context       A pointer to the SPDM context.
 * @param  spdm_session_info  A pointer to the SPDM session context.
 * @param  is_requester       Indicate of the key generation for a requester or a responder.
 * @param  message            Message buffer.
 * @param  message_size       Size in bytes of message buffer.
 *
 * @return RETURN_SUCCESS          message is appended.
 * @return RETURN_OUT_OF_RESOURCES message is not appended because the internal cache is full.
 **/
libspdm_return_t libspdm_append_message_f(libspdm_context_t *spdm_context,
                                          void *spdm_session_info,
                                          bool is_requester, const void *message,
                                          size_t message_size);

/**
 * This function generates a session ID by concatenating req_session_id and rsp_session_id.
 *
 * @param[in]  req_session_id
 * @param[in]  rsp_session_id
 *
 * @return Session ID.
 **/
uint32_t libspdm_generate_session_id(uint16_t req_session_id, uint16_t rsp_session_id);

/**
 * This function assigns a new session ID.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    The SPDM session ID.
 *
 * @return session info associated with this new session ID.
 **/
void *libspdm_assign_session_id(libspdm_context_t *spdm_context, uint32_t session_id, bool use_psk);

/**
 * This function frees a session ID.
 *
 * @param  spdm_context  A pointer to the SPDM context.
 * @param  session_id    The SPDM session ID.
 **/
void libspdm_free_session_id(libspdm_context_t *spdm_context, uint32_t session_id);

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates current TH data with message A and message K.
 *
 * @param  spdm_context            A pointer to the SPDM context.
 * @param  session_info            The SPDM session ID.
 * @param  cert_chain_buffer       Certificate chain buffer with spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size  Size in bytes of the certificate chain buffer.
 * @param  th_data_buffer_size     Size in bytes of the th_data_buffer
 * @param  th_data_buffer          The buffer to store the th_data_buffer
 *
 * @retval RETURN_SUCCESS  current TH data is calculated.
 */
bool libspdm_calculate_th_for_exchange(
    libspdm_context_t *spdm_context, void *spdm_session_info,
    const uint8_t *cert_chain_buffer, size_t cert_chain_buffer_size,
    libspdm_th_managed_buffer_t *th_curr);
#else
/*
 * This function calculates current TH hash with message A and message K.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_info         The SPDM session ID.
 * @param  th_hash_buffer_size  Size in bytes of the th_hash_buffer
 * @param  th_hash_buffer       The buffer to store the th_hash_buffer
 *
 * @retval RETURN_SUCCESS  current TH hash is calculated.
 */
bool libspdm_calculate_th_hash_for_exchange(
    libspdm_context_t *spdm_context, void *spdm_session_info,
    size_t *th_hash_buffer_size, void *th_hash_buffer);

/*
 * This function calculates current TH hmac with message A and message K, with response finished_key.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_info         The SPDM session ID.
 * @param  th_hmac_buffer_size  Size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer       The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_exchange_rsp(
    libspdm_context_t *spdm_context, void *spdm_session_info, bool is_requester,
    size_t *th_hmac_buffer_size, void *th_hmac_buffer);
#endif

#if LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT
/*
 * This function calculates current TH data with message A, message K and message F.
 *
 * @param  spdm_context                A pointer to the SPDM context.
 * @param  session_info                The SPDM session ID.
 * @param  cert_chain_buffer           Certificate chain buffer with spdm_cert_chain_t header.
 * @param  cert_chain_buffer_size      Size in bytes of the certificate chain buffer.
 * @param  mut_cert_chain_buffer       Certificate chain buffer with spdm_cert_chain_t header in mutual authentication.
 * @param  mut_cert_chain_buffer_size  Size in bytes of the certificate chain buffer in mutual authentication.
 * @param  th_data_buffer_size         Size in bytes of the th_data_buffer.
 * @param  th_data_buffer              The buffer to store the th_data_buffer
 *
 * @retval RETURN_SUCCESS  current TH data is calculated.
 */
bool libspdm_calculate_th_for_finish(libspdm_context_t *spdm_context,
                                     void *spdm_session_info,
                                     const uint8_t *cert_chain_buffer,
                                     size_t cert_chain_buffer_size,
                                     const uint8_t *mut_cert_chain_buffer,
                                     size_t mut_cert_chain_buffer_size,
                                     libspdm_th_managed_buffer_t *th_curr);
#else
/*
 * This function calculates current TH hash with message A, message K and message F.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_info         The SPDM session ID.
 * @param  th_hash_buffer_size  Size in bytes of the th_hash_buffer
 * @param  th_hash_buffer       The buffer to store the th_hash_buffer
 *
 * @retval RETURN_SUCCESS  current TH hash is calculated.
 */
bool libspdm_calculate_th_hash_for_finish(libspdm_context_t *spdm_context,
                                          void *spdm_session_info,
                                          size_t *th_hash_buffer_size,
                                          void *th_hash_buffer);

/*
 * This function calculates current TH hmac with message A, message K and message F, with response finished_key.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_info         The SPDM session ID.
 * @param  th_hmac_buffer_size  Size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer       The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_finish_rsp(libspdm_context_t *spdm_context,
                                              void *spdm_session_info,
                                              size_t *th_hmac_buffer_size,
                                              void *th_hmac_buffer);

/*
 * This function calculates current TH hmac with message A, message K and message F, with request finished_key.
 *
 * @param  spdm_context         A pointer to the SPDM context.
 * @param  session_info         The SPDM session ID.
 * @param  th_hmac_buffer_size  Size in bytes of the th_hmac_buffer
 * @param  th_hmac_buffer       The buffer to store the th_hmac_buffer
 *
 * @retval RETURN_SUCCESS  current TH hmac is calculated.
 */
bool libspdm_calculate_th_hmac_for_finish_req(libspdm_context_t *spdm_context,
                                              void *spdm_session_info,
                                              size_t *th_hmac_buffer_size,
                                              void *th_hmac_buffer);
#endif

/*
 * This function calculates th1 hash.
 *
 * @param  spdm_context   A pointer to the SPDM context.
 * @param  session_info   The SPDM session ID.
 * @param  is_requester   Indicate of the key generation for a requester or a responder.
 * @param  th1_hash_data  Th1 hash.
 *
 * @retval RETURN_SUCCESS  th1 hash is calculated.
 */
bool libspdm_calculate_th1_hash(libspdm_context_t *spdm_context,
                                void *spdm_session_info,
                                bool is_requester,
                                uint8_t *th1_hash_data);

/*
 * This function calculates th2 hash.
 *
 * @param  spdm_context   A pointer to the SPDM context.
 * @param  session_info   The SPDM session ID.
 * @param  is_requester   Indicate of the key generation for a requester or a responder.
 * @param  th1_hash_data  Th2 hash
 *
 * @retval RETURN_SUCCESS  th2 hash is calculated.
 */
bool libspdm_calculate_th2_hash(libspdm_context_t *spdm_context,
                                void *spdm_session_info,
                                bool is_requester,
                                uint8_t *th2_hash_data);

/**
 * Reads a 24-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 24-bit value that may be unaligned.
 *
 * @return The 24-bit value read from buffer.
 **/
uint32_t libspdm_read_uint24(const uint8_t *buffer);

/**
 * Writes a 24-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 24-bit value that may be unaligned.
 * @param  value   24-bit value to write to buffer.
 **/
void libspdm_write_uint24(uint8_t *buffer, uint32_t value);

/**
 * Reads a 16-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 16-bit value that may be unaligned.
 *
 * @return The 16-bit value read from buffer.
 **/
uint16_t libspdm_read_uint16(const uint8_t *buffer);

/**
 * Writes a 16-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 16-bit value that may be unaligned.
 * @param  value   16-bit value to write to buffer.
 **/
void libspdm_write_uint16(uint8_t *buffer, uint16_t value);

/**
 * Reads a 32-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 32-bit value that may be unaligned.
 *
 * @return The 32-bit value read from buffer.
 **/
uint32_t libspdm_read_uint32(const uint8_t *buffer);

/**
 * Writes a 32-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 32-bit value that may be unaligned.
 * @param  value   32-bit value to write to buffer.
 **/
void libspdm_write_uint32(uint8_t *buffer, uint32_t value);

/**
 * Reads a 64-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 64-bit value that may be unaligned.
 *
 * @return The 64-bit value read from buffer.
 **/
uint64_t libspdm_read_uint64(const uint8_t *buffer);

/**
 * Writes a 64-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 64-bit value that may be unaligned.
 * @param  value   64-bit value to write to buffer.
 **/
void libspdm_write_uint64(uint8_t *buffer, uint64_t value);

/**
 * Determine if bitmask has at most one bit set.
 *
 * @param mask  The bitmask to be tested.
 *
 * @return true   At most one bit is set.
 * @return false  More than one bit is set.
 */
static inline bool libspdm_onehot0(uint32_t mask)
{
    return !mask || !(mask & (mask - 1));
}

static inline uint64_t libspdm_le_to_be_64(uint64_t value)
{
    return (((value & 0x00000000000000ff) << 56) |
            ((value & 0x000000000000ff00) << 40) |
            ((value & 0x0000000000ff0000) << 24) |
            ((value & 0x00000000ff000000) << 8) |
            ((value & 0x000000ff00000000) >> 8) |
            ((value & 0x0000ff0000000000) >> 24) |
            ((value & 0x00ff000000000000) >> 40) |
            ((value & 0xff00000000000000) >> 56));
}

#endif /* SPDM_COMMON_LIB_INTERNAL_H */
