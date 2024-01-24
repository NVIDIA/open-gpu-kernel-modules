/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * Definitions of DSP0274 Security Protocol & data Model Specification (SPDM)
 * version 1.2.0 in Distributed Management Task Force (DMTF).
 **/

#ifndef SPDM_H
#define SPDM_H

#pragma pack(1)

/* 3 means SPDM spec 1.0, 1.1, 1.2 */
#define SPDM_MAX_VERSION_COUNT 3
#define SPDM_MAX_SLOT_COUNT 8
#define SPDM_MAX_OPAQUE_DATA_SIZE 1024

#define SPDM_NONCE_SIZE 32
#define SPDM_RANDOM_DATA_SIZE 32

/* SPDM response code (1.0) */
#define SPDM_DIGESTS 0x01
#define SPDM_CERTIFICATE 0x02
#define SPDM_CHALLENGE_AUTH 0x03
#define SPDM_VERSION 0x04
#define SPDM_MEASUREMENTS 0x60
#define SPDM_CAPABILITIES 0x61
#define SPDM_ALGORITHMS 0x63
#define SPDM_VENDOR_DEFINED_RESPONSE 0x7E
#define SPDM_ERROR 0x7F

/* SPDM response code (1.1) */
#define SPDM_KEY_EXCHANGE_RSP 0x64
#define SPDM_FINISH_RSP 0x65
#define SPDM_PSK_EXCHANGE_RSP 0x66
#define SPDM_PSK_FINISH_RSP 0x67
#define SPDM_HEARTBEAT_ACK 0x68
#define SPDM_KEY_UPDATE_ACK 0x69
#define SPDM_ENCAPSULATED_REQUEST 0x6A
#define SPDM_ENCAPSULATED_RESPONSE_ACK 0x6B
#define SPDM_END_SESSION_ACK 0x6C

/* SPDM response code (1.2) */
#define SPDM_CSR 0x6D
#define SPDM_SET_CERTIFICATE_RSP 0x6E
#define SPDM_CHUNK_SEND_ACK 0x05
#define SPDM_CHUNK_RESPONSE 0x06

/* SPDM request code (1.0) */
#define SPDM_GET_DIGESTS 0x81
#define SPDM_GET_CERTIFICATE 0x82
#define SPDM_CHALLENGE 0x83
#define SPDM_GET_VERSION 0x84
#define SPDM_GET_MEASUREMENTS 0xE0
#define SPDM_GET_CAPABILITIES 0xE1
#define SPDM_NEGOTIATE_ALGORITHMS 0xE3
#define SPDM_VENDOR_DEFINED_REQUEST 0xFE
#define SPDM_RESPOND_IF_READY 0xFF

/* SPDM request code (1.1) */
#define SPDM_KEY_EXCHANGE 0xE4
#define SPDM_FINISH 0xE5
#define SPDM_PSK_EXCHANGE 0xE6
#define SPDM_PSK_FINISH 0xE7
#define SPDM_HEARTBEAT 0xE8
#define SPDM_KEY_UPDATE 0xE9
#define SPDM_GET_ENCAPSULATED_REQUEST 0xEA
#define SPDM_DELIVER_ENCAPSULATED_RESPONSE 0xEB
#define SPDM_END_SESSION 0xEC

/* SPDM request code (1.2) */
#define SPDM_GET_CSR 0xED
#define SPDM_SET_CERTIFICATE 0xEE
#define SPDM_CHUNK_SEND 0x85
#define SPDM_CHUNK_GET 0x86

/* SPDM message header*/
typedef struct {
    uint8_t spdm_version;
    uint8_t request_response_code;
    uint8_t param1;
    uint8_t param2;
} spdm_message_header_t;

#define SPDM_MESSAGE_VERSION_10 0x10
#define SPDM_MESSAGE_VERSION_11 0x11
#define SPDM_MESSAGE_VERSION_12 0x12
#define SPDM_MESSAGE_VERSION SPDM_MESSAGE_VERSION_10

/* SPDM GET_VERSION request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_get_version_request_t;


/* SPDM GET_VERSION response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint8_t reserved;
    uint8_t version_number_entry_count;
    /*spdm_version_number_t  version_number_entry[version_number_entry_count];*/
} spdm_version_response_t;

/* SPDM VERSION structure
 * bit[15:12] major_version
 * bit[11:8]  minor_version
 * bit[7:4]   update_version_number
 * bit[3:0]   alpha*/
typedef uint16_t spdm_version_number_t;
#define SPDM_VERSION_NUMBER_SHIFT_BIT 8

#define SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT "dmtf-spdm-v1.2.*"
#define SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT_SIZE \
    (sizeof(SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT) - 1)
#define SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE 100

/* SPDM GET_CAPABILITIES request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD
     * Below field is added in 1.1.*/
    uint8_t reserved;
    uint8_t ct_exponent;
    uint16_t reserved2;
    uint32_t flags;
    /* Below field is added in 1.2.*/
    uint32_t data_transfer_size;
    uint32_t max_spdm_msg_size;
} spdm_get_capabilities_request_t;

/* SPDM GET_CAPABILITIES response*/

typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint8_t reserved;
    uint8_t ct_exponent;
    uint16_t reserved2;
    uint32_t flags;
    /* Below field is added in 1.2.*/
    uint32_t data_transfer_size;
    uint32_t max_spdm_msg_size;
} spdm_capabilities_response_t;

#define SPDM_MIN_DATA_TRANSFER_SIZE_VERSION_12  42

/* SPDM GET_CAPABILITIES request flags (1.1) */
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP 0x00000002
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHAL_CAP 0x00000004
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP 0x00000040
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP 0x00000080
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MUT_AUTH_CAP 0x00000100
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP 0x00000200
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_PSK_CAP (0x00000400 | 0x00000800)
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_PSK_CAP_REQUESTER 0x00000400
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCAP_CAP 0x00001000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HBEAT_CAP 0x00002000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP 0x00004000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP 0x00008000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_PUB_KEY_ID_CAP 0x00010000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_11_MASK ( \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHAL_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MUT_AUTH_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_PSK_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCAP_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HBEAT_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_PUB_KEY_ID_CAP)

/* SPDM GET_CAPABILITIES request flags (1.2) */
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHUNK_CAP 0x00020000
#define SPDM_GET_CAPABILITIES_REQUEST_FLAGS_12_MASK ( \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_11_MASK | \
        SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CHUNK_CAP)

/* SPDM GET_CAPABILITIES response flags (1.0) */
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CACHE_CAP 0x00000001
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP 0x00000002
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHAL_CAP 0x00000004
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP (0x00000008 | 0x00000010)
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_NO_SIG 0x00000008
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_SIG 0x00000010
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_FRESH_CAP 0x00000020
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_10_MASK ( \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CACHE_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHAL_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_FRESH_CAP)

/* SPDM GET_CAPABILITIES response flags (1.1) */
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP 0x00000040
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP 0x00000080
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MUT_AUTH_CAP 0x00000100
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP 0x00000200
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP (0x00000400 | 0x00000800)
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP_RESPONDER 0x00000400
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP_RESPONDER_WITH_CONTEXT 0x00000800
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCAP_CAP 0x00001000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HBEAT_CAP 0x00002000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP 0x00004000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP 0x00008000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PUB_KEY_ID_CAP 0x00010000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_11_MASK ( \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_10_MASK | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MUT_AUTH_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PSK_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCAP_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HBEAT_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HANDSHAKE_IN_THE_CLEAR_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_PUB_KEY_ID_CAP)

/* SPDM GET_CAPABILITIES request flags (1.2) */
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP 0x00020000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ALIAS_CERT_CAP 0x00040000

/* SPDM GET_CAPABILITIES response flags (1.2.1)*/
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_SET_CERT_CAP 0x00080000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CSR_CAP 0x00100000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_INSTALL_RESET_CAP 0x00200000
#define SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_12_MASK ( \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_11_MASK | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHUNK_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ALIAS_CERT_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_SET_CERT_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CSR_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_INSTALL_RESET_CAP)

/* SPDM NEGOTIATE_ALGORITHMS request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == Number of Algorithms Structure Tables
     * param2 == RSVD*/
    uint16_t length;
    uint8_t measurement_specification;
    /* other_params_support is added in 1.2.
     * BIT[0:3]=opaque_data_format support, BIT[4:7]=reserved*/
    uint8_t other_params_support;
    uint32_t base_asym_algo;
    uint32_t base_hash_algo;
    uint8_t reserved2[12];
    uint8_t ext_asym_count;
    uint8_t ext_hash_count;
    uint16_t reserved3;
    /*spdm_extended_algorithm_t                 ext_asym[ext_asym_count];
     * spdm_extended_algorithm_t                 ext_hash[ext_hash_count];
     * Below field is added in 1.1.
     * spdm_negotiate_algorithms_struct_table_t  alg_struct[param1];*/
} spdm_negotiate_algorithms_request_t;

#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_LENGTH_VERSION_10 0x40
#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_LENGTH_VERSION_11 0x80
#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_LENGTH_VERSION_12 0x80
#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_EXT_ALG_COUNT_VERSION_10 0x08
#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_EXT_ALG_COUNT_VERSION_11 0x14
#define SPDM_NEGOTIATE_ALGORITHMS_REQUEST_MAX_EXT_ALG_COUNT_VERSION_12 0x14

typedef struct {
    uint8_t alg_type;
    uint8_t alg_count; /* BIT[0:3]=ext_alg_count, BIT[4:7]=fixed_alg_byte_count*/
    /*uint8_t                alg_supported[fixed_alg_byte_count];
     * uint32_t               alg_external[ext_alg_count];*/
} spdm_negotiate_algorithms_struct_table_t;

#define SPDM_NEGOTIATE_ALGORITHMS_MAX_NUM_STRUCT_TABLE_ALG 4
#define SPDM_NEGOTIATE_ALGORITHMS_STRUCT_TABLE_ALG_TYPE_DHE 2
#define SPDM_NEGOTIATE_ALGORITHMS_STRUCT_TABLE_ALG_TYPE_AEAD 3
#define SPDM_NEGOTIATE_ALGORITHMS_STRUCT_TABLE_ALG_TYPE_REQ_BASE_ASYM_ALG 4
#define SPDM_NEGOTIATE_ALGORITHMS_STRUCT_TABLE_ALG_TYPE_KEY_SCHEDULE 5

typedef struct {
    uint8_t alg_type;
    uint8_t alg_count;
    uint16_t alg_supported;
} spdm_negotiate_algorithms_common_struct_table_t;


/* SPDM NEGOTIATE_ALGORITHMS request base_asym_algo/REQ_BASE_ASYM_ALG */
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048 0x00000001
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048 0x00000002
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072 0x00000004
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072 0x00000008
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256 0x00000010
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096 0x00000020
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096 0x00000040
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 0x00000080
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521 0x00000100

/* SPDM NEGOTIATE_ALGORITHMS request base_asym_algo/REQ_BASE_ASYM_ALG (1.2) */
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256 0x00000200
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519 0x00000400
#define SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448 0x00000800

/* SPDM NEGOTIATE_ALGORITHMS request base_hash_algo */
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256 0x00000001
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384 0x00000002
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_512 0x00000004
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_256 0x00000008
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_384 0x00000010
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA3_512 0x00000020

/* SPDM NEGOTIATE_ALGORITHMS request base_hash_algo (1.2) */
#define SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SM3_256 0x00000040

/* SPDM NEGOTIATE_ALGORITHMS request DHE */
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048 0x00000001
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072 0x00000002
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096 0x00000004
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1 0x00000008
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1 0x00000010
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1 0x00000020

/* SPDM NEGOTIATE_ALGORITHMS request DHE (1.2) */
#define SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256 0x00000040

/* SPDM NEGOTIATE_ALGORITHMS request AEAD */
#define SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_128_GCM 0x00000001
#define SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM 0x00000002
#define SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_CHACHA20_POLY1305 0x00000004

/* SPDM NEGOTIATE_ALGORITHMS request AEAD (1.2) */
#define SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AEAD_SM4_GCM 0x00000008

/* SPDM NEGOTIATE_ALGORITHMS request KEY_SCHEDULE */
#define SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH 0x00000001

/* SPDM NEGOTIATE_ALGORITHMS response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == Number of Algorithms Structure Tables
     * param2 == RSVD*/
    uint16_t length;
    uint8_t measurement_specification_sel;
    /* other_params_selection is added in 1.2.
     * BIT[0:3]=opaque_data_format select, BIT[4:7]=reserved*/
    uint8_t other_params_selection;
    uint32_t measurement_hash_algo;
    uint32_t base_asym_sel;
    uint32_t base_hash_sel;
    uint8_t reserved2[12];
    uint8_t ext_asym_sel_count;
    uint8_t ext_hash_sel_count;
    uint16_t reserved3;
    /*spdm_extended_algorithm_t                 ext_asym_sel[ext_asym_sel_count];
     * spdm_extended_algorithm_t                 ext_hash_sel[ext_hash_sel_count];
     * Below field is added in 1.1.
     * spdm_negotiate_algorithms_struct_table_t  alg_struct[param1];*/
} spdm_algorithms_response_t;

/* SPDM NEGOTIATE_ALGORITHMS response measurement_hash_algo */
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_RAW_BIT_STREAM_ONLY 0x00000001
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA_256 0x00000002
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA_384 0x00000004
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA_512 0x00000008
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA3_256 0x00000010
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA3_384 0x00000020
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA3_512 0x00000040

/* SPDM NEGOTIATE_ALGORITHMS response measurement_hash_algo (1.2) */
#define SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SM3_256 0x00000080

/* SPDM Opaque Data Format (1.2) */
#define SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_NONE 0x0
#define SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_0 0x1
#define SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_1 0x2
#define SPDM_ALGORITHMS_OPAQUE_DATA_FORMAT_MASK 0xF

/* SPDM Opaque Data Format 1 (1.2) */
typedef struct {
    uint8_t total_elements;
    uint8_t reserved[3];
    /*opaque_element_table_t  opaque_list[];*/
} spdm_general_opaque_data_table_header_t;

/* SPDM extended algorithm */
typedef struct {
    uint8_t registry_id;
    uint8_t reserved;
    uint16_t algorithm_id;
} spdm_extended_algorithm_t;

/* SPDM registry_id */
#define SPDM_REGISTRY_ID_DMTF 0x0
#define SPDM_REGISTRY_ID_TCG 0x1
#define SPDM_REGISTRY_ID_USB 0x2
#define SPDM_REGISTRY_ID_PCISIG 0x3
#define SPDM_REGISTRY_ID_IANA 0x4
#define SPDM_REGISTRY_ID_HDBASET 0x5
#define SPDM_REGISTRY_ID_MIPI 0x6
#define SPDM_REGISTRY_ID_CXL 0x7
#define SPDM_REGISTRY_ID_JEDEC 0x8
#define SPDM_REGISTRY_ID_VESA 0x9
#define SPDM_REGISTRY_ID_IANA_CBOR 0xa
#define SPDM_REGISTRY_ID_MAX  0xa

/* SPDM GET_DIGESTS request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_get_digest_request_t;

/* SPDM GET_DIGESTS response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == slot_mask
     * uint8_t                digest[digest_size][slot_count];*/
} spdm_digest_response_t;


/* SPDM GET_CERTIFICATE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == BIT[0:3]=slot_id, BIT[4:7]=RSVD
     * param2 == RSVD*/
    uint16_t offset;
    uint16_t length;
} spdm_get_certificate_request_t;

#define SPDM_GET_CERTIFICATE_REQUEST_SLOT_ID_MASK 0xF

/* SPDM GET_CERTIFICATE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == BIT[0:3]=slot_id, BIT[4:7]=RSVD
     * param2 == RSVD*/
    uint16_t portion_length;
    uint16_t remainder_length;
    /*uint8_t                cert_chain[portion_length];*/
} spdm_certificate_response_t;

#define SPDM_CERTIFICATE_RESPONSE_SLOT_ID_MASK 0xF
typedef struct {
    /* Total length of the certificate chain, in bytes,
     * including all fields in this table.*/

    uint16_t length;
    uint16_t reserved;

    /* digest of the Root Certificate.
     * Note that Root Certificate is ASN.1 DER-encoded for this digest.
     * The hash size is determined by the SPDM device.*/

    /*uint8_t    root_hash[hash_size];*/

    /* One or more ASN.1 DER-encoded X509v3 certificates where the first certificate is signed by the Root
     * Certificate or is the Root Certificate itself and each subsequent certificate is signed by the preceding
     * certificate. The last certificate is the Leaf Certificate.*/

    /*uint8_t    certificates[length - 4 - hash_size];*/
} spdm_cert_chain_t;

/* Maximum size, in bytes, of a certificate chain. */
#define SPDM_MAX_CERTIFICATE_CHAIN_SIZE 65535

/* SPDM CHALLENGE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == slot_id
     * param2 == HashType*/
    uint8_t nonce[32];
} spdm_challenge_request_t;

/* SPDM CHALLENGE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == ResponseAttribute, BIT[0:3]=slot_id, BIT[4:6]=RSVD, BIT[7]=basic_mut_auth(deprecated in 1.2)
     * param2 == slot_mask
     * uint8_t                cert_chain_hash[digest_size];
     * uint8_t                nonce[32];
     * uint8_t                measurement_summary_hash[digest_size];
     * uint16_t               opaque_length;
     * uint8_t                opaque_data[opaque_length];
     * uint8_t                signature[key_size];*/
} spdm_challenge_auth_response_t;

/* SPDM generic request measurement summary HashType */
#define SPDM_REQUEST_NO_MEASUREMENT_SUMMARY_HASH 0
#define SPDM_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH 1
#define SPDM_REQUEST_ALL_MEASUREMENTS_HASH 0xFF

/* SPDM CHALLENGE request measurement summary HashType */
#define SPDM_CHALLENGE_REQUEST_NO_MEASUREMENT_SUMMARY_HASH SPDM_REQUEST_NO_MEASUREMENT_SUMMARY_HASH
#define SPDM_CHALLENGE_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH \
    SPDM_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH
#define SPDM_CHALLENGE_REQUEST_ALL_MEASUREMENTS_HASH SPDM_REQUEST_ALL_MEASUREMENTS_HASH

#define SPDM_CHALLENGE_AUTH_RESPONSE_ATTRIBUTE_SLOT_ID_MASK 0xF
#define SPDM_CHALLENGE_AUTH_RESPONSE_ATTRIBUTE_BASIC_MUT_AUTH_REQ 0x00000080 /* Deprecated in SPDM 1.2*/

#define SPDM_CHALLENGE_AUTH_SIGN_CONTEXT "responder-challenge_auth signing"
#define SPDM_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE (sizeof(SPDM_CHALLENGE_AUTH_SIGN_CONTEXT) - 1)
#define SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT "requester-challenge_auth signing"
#define SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE (sizeof(SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT) - 1)

/* SPDM GET_MEASUREMENTS request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == Attributes
     * param2 == measurement_operation*/
    uint8_t nonce[32];
    /* Below field is added in 1.1.*/
    uint8_t slot_id_param; /* BIT[0:3]=slot_id, BIT[4:7]=RSVD*/
} spdm_get_measurements_request_t;

#define SPDM_GET_MEASUREMENTS_REQUEST_SLOT_ID_MASK 0xF

/* SPDM GET_MEASUREMENTS request Attributes */
#define SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE 0x00000001
#define SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_RAW_BIT_STREAM_REQUESTED 0x00000002

/* SPDM GET_MEASUREMENTS request measurement_operation */
#define SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_TOTAL_NUMBER_OF_MEASUREMENTS 0

/*SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_INDEX */
#define SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_ALL_MEASUREMENTS 0xFF


/* SPDM MEASUREMENTS block common header */
typedef struct {
    uint8_t index;
    uint8_t measurement_specification;
    uint16_t measurement_size;
    /*uint8_t                measurement[measurement_size];*/
} spdm_measurement_block_common_header_t;

#define SPDM_MEASUREMENT_SPECIFICATION_DMTF 0x01

/* SPDM MEASUREMENTS block DMTF header */
typedef struct {
    uint8_t dmtf_spec_measurement_value_type;
    uint16_t dmtf_spec_measurement_value_size;
    /*uint8_t                Dmtf_spec_measurement_value[dmtf_spec_measurement_value_size];*/
} spdm_measurement_block_dmtf_header_t;

typedef struct {
    spdm_measurement_block_common_header_t measurement_block_common_header;
    spdm_measurement_block_dmtf_header_t measurement_block_dmtf_header;
    /*uint8_t                                 hash_value[hash_size];*/
} spdm_measurement_block_dmtf_t;

/* SPDM MEASUREMENTS block MeasurementValueType */
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_IMMUTABLE_ROM 0
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_MUTABLE_FIRMWARE 1
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_HARDWARE_CONFIGURATION 2
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_FIRMWARE_CONFIGURATION 3
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_MEASUREMENT_MANIFEST 4
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_DEVICE_MODE 5
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_VERSION 6
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_SECURE_VERSION_NUMBER 7
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_MASK 0x7
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_TYPE_RAW_BIT_STREAM 0x00000080

/* SPDM MEASUREMENTS block index */
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_INDEX_MEASUREMENT_MANIFEST 0xFD
#define SPDM_MEASUREMENT_BLOCK_MEASUREMENT_INDEX_DEVICE_MODE 0xFE

/* SPDM MEASUREMENTS device mode */
typedef struct {
    uint32_t operational_mode_capabilities;
    uint32_t operational_mode_state;
    uint32_t device_mode_capabilities;
    uint32_t device_mode_state;
} spdm_measurements_device_mode_t;

#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_MANUFACTURING_MODE 0x00000001
#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_VALIDATION_MODE 0x00000002
#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_NORMAL_MODE 0x00000004
#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_RECOVERY_MODE 0x00000008
#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_RMA_MODE 0x00000010
#define SPDM_MEASUREMENT_DEVICE_OPERATION_MODE_DECOMMISSIONED_MODE 0x00000020

#define SPDM_MEASUREMENT_DEVICE_MODE_NON_INVASIVE_DEBUG_MODE_IS_ACTIVE 0x00000001
#define SPDM_MEASUREMENT_DEVICE_MODE_INVASIVE_DEBUG_MODE_IS_ACTIVE 0x00000002
#define SPDM_MEASUREMENT_DEVICE_MODE_NON_INVASIVE_DEBUG_MODE_HAS_BEEN_ACTIVE 0x00000004
#define SPDM_MEASUREMENT_DEVICE_MODE_INVASIVE_DEBUG_MODE_HAS_BEEN_ACTIVE 0x00000008
#define SPDM_MEASUREMENT_DEVICE_MODE_INVASIVE_DEBUG_MODE_HAS_BEEN_ACTIVE_AFTER_MFG 0x00000010

/* SPDM MEASUREMENTS SVN */
typedef uint64_t spdm_measurements_secure_version_number_t;

/* SPDM GET_MEASUREMENTS response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == TotalNumberOfMeasurement/RSVD
     * param2 == BIT[0:3]=slot_id, BIT[4:5]=content changed, BIT[6:7]=RSVD*/
    uint8_t number_of_blocks;
    uint8_t measurement_record_length[3];
    /*uint8_t                measurement_record[measurement_record_length];
     * uint8_t                nonce[32];
     * uint16_t               opaque_length;
     * uint8_t                opaque_data[opaque_length];
     * uint8_t                signature[key_size];*/
} spdm_measurements_response_t;

#define SPDM_MEASUREMENTS_RESPONSE_SLOT_ID_MASK 0xF

/* SPDM MEASUREMENTS content changed */
#define SPDM_MEASUREMENTS_RESPONSE_CONTENT_CHANGE_MASK          0x30
#define SPDM_MEASUREMENTS_RESPONSE_CONTENT_CHANGE_NO_DETECTION  0x00
#define SPDM_MEASUREMENTS_RESPONSE_CONTENT_CHANGE_DETECTED      0x10
#define SPDM_MEASUREMENTS_RESPONSE_CONTENT_NO_CHANGE_DETECTED   0x20

#define SPDM_MEASUREMENTS_SIGN_CONTEXT "responder-measurements signing"
#define SPDM_MEASUREMENTS_SIGN_CONTEXT_SIZE (sizeof(SPDM_MEASUREMENTS_SIGN_CONTEXT) - 1)

/* SPDM ERROR response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == Error Code
     * param2 == Error data
     * uint8_t                extended_error_data[32];*/
} spdm_error_response_t;

#define SPDM_EXTENDED_ERROR_DATA_MAX_SIZE 32

/* SPDM error code */
#define SPDM_ERROR_CODE_INVALID_REQUEST 0x01
#define SPDM_ERROR_CODE_BUSY 0x03
#define SPDM_ERROR_CODE_UNEXPECTED_REQUEST 0x04
#define SPDM_ERROR_CODE_UNSPECIFIED 0x05
#define SPDM_ERROR_CODE_UNSUPPORTED_REQUEST 0x07
#define SPDM_ERROR_CODE_VERSION_MISMATCH 0x41
#define SPDM_ERROR_CODE_RESPONSE_NOT_READY 0x42
#define SPDM_ERROR_CODE_REQUEST_RESYNCH 0x43
#define SPDM_ERROR_CODE_VENDOR_DEFINED 0xFF

/* SPDM error code (1.1) */
#define SPDM_ERROR_CODE_DECRYPT_ERROR 0x06
#define SPDM_ERROR_CODE_REQUEST_IN_FLIGHT 0x08
#define SPDM_ERROR_CODE_INVALID_RESPONSE_CODE 0x09
#define SPDM_ERROR_CODE_SESSION_LIMIT_EXCEEDED 0x0A

/* SPDM error code (1.2) */
#define SPDM_ERROR_CODE_SESSION_REQUIRED 0x0B
#define SPDM_ERROR_CODE_RESET_REQUIRED 0x0C
#define SPDM_ERROR_CODE_RESPONSE_TOO_LARGE 0x0D
#define SPDM_ERROR_CODE_REQUEST_TOO_LARGE 0x0E
#define SPDM_ERROR_CODE_LARGE_RESPONSE 0x0F
#define SPDM_ERROR_CODE_MESSAGE_LOST 0x10

/* SPDM ResponseNotReady extended data */
typedef struct {
    uint8_t rd_exponent;
    uint8_t request_code;
    uint8_t token;
    uint8_t rd_tm;
} spdm_error_data_response_not_ready_t;

typedef struct {
    spdm_message_header_t header;
    /* param1 == Error Code
     * param2 == Error data*/
    spdm_error_data_response_not_ready_t extend_error_data;
} spdm_error_response_data_response_not_ready_t;

/* SPDM LargeResponse extended data */
typedef struct {
    uint8_t handle;
} spdm_error_data_large_response_t;

typedef struct {
    spdm_message_header_t header;
    /* param1 == Error Code
     * param2 == Error data*/
    spdm_error_data_large_response_t extend_error_data;
} spdm_error_response_large_response_t;

/* SPDM RESPONSE_IF_READY request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == request_code
     * param2 == token*/
} spdm_response_if_ready_request_t;

/* SPDM VENDOR_DEFINED request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint16_t standard_id;
    uint8_t len;
    /*uint8_t                vendor_id[len];
     * uint16_t               payload_length;
     * uint8_t                vendor_defined_payload[payload_length];*/
} spdm_vendor_defined_request_msg_t;

/* SPDM VENDOR_DEFINED response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint16_t standard_id;
    uint8_t len;
    /*uint8_t                vendor_id[len];
     * uint16_t               payload_length;
     * uint8_t                vendor_defined_payload[payload_length];*/
} spdm_vendor_defined_response_msg_t;

/* Below command is defined in SPDM 1.1 */

/* SPDM KEY_EXCHANGE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == HashType
    * param2 == slot_id*/
    uint16_t req_session_id;
    /* session_policy is added in 1.2.*/
    uint8_t session_policy;
    uint8_t reserved;
    uint8_t random_data[32];
    /*uint8_t                exchange_data[D];
     * uint16_t               opaque_length;
     * uint8_t                opaque_data[opaque_length];*/
} spdm_key_exchange_request_t;

/* SPDM KEY_EXCHANGE request session_policy */
#define SPDM_KEY_EXCHANGE_REQUEST_SESSION_POLICY_TERMINATION_POLICY_RUNTIME_UPDATE 0x00000001

/* SPDM KEY_EXCHANGE request measurement summary HashType */
#define SPDM_KEY_EXCHANGE_REQUEST_NO_MEASUREMENT_SUMMARY_HASH \
    SPDM_REQUEST_NO_MEASUREMENT_SUMMARY_HASH
#define SPDM_KEY_EXCHANGE_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH \
    SPDM_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH
#define SPDM_KEY_EXCHANGE_REQUEST_ALL_MEASUREMENTS_HASH SPDM_REQUEST_ALL_MEASUREMENTS_HASH

/* SPDM KEY_EXCHANGE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == heartbeat_period
     * param2 == RSVD*/
    uint16_t rsp_session_id;
    uint8_t mut_auth_requested;
    uint8_t req_slot_id_param;
    uint8_t random_data[32];
    /*uint8_t                exchange_data[D];
    * uint8_t                measurement_summary_hash[digest_size];
    * uint16_t               opaque_length;
    * uint8_t                opaque_data[opaque_length];
    * uint8_t                signature[S];
    * uint8_t                verify_data[H];*/
} spdm_key_exchange_response_t;

/* SPDM KEY_EXCHANGE response mut_auth_requested */
#define SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED 0x00000001
#define SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_ENCAP_REQUEST 0x00000002
#define SPDM_KEY_EXCHANGE_RESPONSE_MUT_AUTH_REQUESTED_WITH_GET_DIGESTS 0x00000004

#define SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT "responder-key_exchange_rsp signing"
#define SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT_SIZE \
    (sizeof(SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT) - 1)

#define SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT "Requester-KEP-dmtf-spdm-v1.2"
#define SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT_SIZE \
    (sizeof(SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT) - 1)

#define SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT "Responder-KEP-dmtf-spdm-v1.2"
#define SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT_SIZE \
    (sizeof(SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT) - 1)

/* SPDM FINISH request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == signature_included
     * param2 == req_slot_id
     * uint8_t                signature[S];
     * uint8_t                verify_data[H];*/
} spdm_finish_request_t;

/* SPDM FINISH request signature_included */
#define SPDM_FINISH_REQUEST_ATTRIBUTES_SIGNATURE_INCLUDED 0x00000001

/* SPDM FINISH response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD
     * uint8_t                verify_data[H];*/
} spdm_finish_response_t;

#define SPDM_FINISH_SIGN_CONTEXT "requester-finish signing"
#define SPDM_FINISH_SIGN_CONTEXT_SIZE (sizeof(SPDM_FINISH_SIGN_CONTEXT) - 1)

/* SPDM PSK_EXCHANGE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == HashType
     * param2 == RSVD/session_policy (1.2)*/
    uint16_t req_session_id;
    uint16_t psk_hint_length;
    uint16_t context_length;
    uint16_t opaque_length;
    /*uint8_t                psk_hint[psk_hint_length];
     * uint8_t                context[context_length];
     * uint8_t                opaque_data[opaque_length];*/
} spdm_psk_exchange_request_t;

/* SPDM PSK_EXCHANGE request measurement summary HashType */
#define SPDM_PSK_EXCHANGE_REQUEST_NO_MEASUREMENT_SUMMARY_HASH \
    SPDM_REQUEST_NO_MEASUREMENT_SUMMARY_HASH
#define SPDM_PSK_EXCHANGE_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH \
    SPDM_REQUEST_TCB_COMPONENT_MEASUREMENT_HASH
#define SPDM_PSK_EXCHANGE_REQUEST_ALL_MEASUREMENTS_HASH SPDM_REQUEST_ALL_MEASUREMENTS_HASH

/* SPDM PSK_EXCHANGE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == heartbeat_period
     * param2 == RSVD*/
    uint16_t rsp_session_id;
    uint16_t reserved;
    uint16_t context_length;
    uint16_t opaque_length;
    /*uint8_t                measurement_summary_hash[digest_size];
     * uint8_t                context[context_length];
     * uint8_t                opaque_data[opaque_length];
     * uint8_t                verify_data[H];*/
} spdm_psk_exchange_response_t;

/* SPDM PSK_FINISH request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD
     * uint8_t                verify_data[H];*/
} spdm_psk_finish_request_t;

/* SPDM PSK_FINISH response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_psk_finish_response_t;


/* SPDM HEARTBEAT request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_heartbeat_request_t;

/* SPDM HEARTBEAT response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_heartbeat_response_t;

/* SPDM KEY_UPDATE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == key_operation
     * param2 == tag*/
} spdm_key_update_request_t;

/* SPDM KEY_UPDATE Operations table */
#define SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_KEY 1
#define SPDM_KEY_UPDATE_OPERATIONS_TABLE_UPDATE_ALL_KEYS 2
#define SPDM_KEY_UPDATE_OPERATIONS_TABLE_VERIFY_NEW_KEY 3

/* SPDM KEY_UPDATE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == key_operation
     * param2 == tag*/
} spdm_key_update_response_t;

/* SPDM GET_ENCAPSULATED_REQUEST request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_get_encapsulated_request_request_t;

/* SPDM ENCAPSULATED_REQUEST response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == request_id
     * param2 == RSVD
     * uint8_t                encapsulated_request[];*/
} spdm_encapsulated_request_response_t;

/* SPDM DELIVER_ENCAPSULATED_RESPONSE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == request_id
     * param2 == RSVD
     * uint8_t                encapsulated_response[];*/
} spdm_deliver_encapsulated_response_request_t;

/* SPDM ENCAPSULATED_RESPONSE_ACK response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == request_id
     * param2 == payload_type*/

    /* below 4 bytes are added in 1.2.*/
    uint8_t ack_request_id;
    uint8_t reserved[3];

    /*uint8_t                encapsulated_request[];*/
} spdm_encapsulated_response_ack_response_t;

/* SPDM ENCAPSULATED_RESPONSE_ACK_RESPONSE payload Type */
#define SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_ABSENT 0
#define SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_PRESENT 1
#define SPDM_ENCAPSULATED_RESPONSE_ACK_RESPONSE_PAYLOAD_TYPE_REQ_SLOT_NUMBER 2

/* SPDM END_SESSION request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == end_session_request_attributes
     * param2 == RSVD*/
} spdm_end_session_request_t;

/* SPDM END_SESSION request Attributes */
#define SPDM_END_SESSION_REQUEST_ATTRIBUTES_PRESERVE_NEGOTIATED_STATE_CLEAR 0x00000001

/* SPDM END_SESSION response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
} spdm_end_session_response_t;

/* SPDM SET_CERTIFICATE request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == BIT[0:3]=slot_id, BIT[4:7]=RSVD
     * param2 == RSVD
     * void * cert_chain*/
} spdm_set_certificate_request_t;

/* SPDM SET_CERTIFICATE_RSP response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == BIT[0:3]=slot_id, BIT[4:7]=RSVD
     * param2 == RSVD*/
} spdm_set_certificate_response_t;

/* SPDM GET_CSR request */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint16_t requester_info_length;
    uint16_t opaque_data_length;
    /* uint8_t RequesterInfo[requester_info_length];
     * uint8_t opaque_data[opaque_data_length]; */
} spdm_get_csr_request_t;

/* Maximum size, in bytes, of a CSR. */
#define SPDM_MAX_CSR_SIZE 65535

/* SPDM CSR response */
typedef struct {
    spdm_message_header_t header;
    /* param1 == RSVD
     * param2 == RSVD*/
    uint16_t csr_length;
    uint16_t reserved;
} spdm_csr_response_t;

/* SPDM CHUNK_SEND request */
typedef struct {
    spdm_message_header_t header;
    /* param1 - Request Attributes
     * param2 - Handle */
    uint16_t chunk_seq_no;
    uint16_t reserved;
    uint32_t chunk_size;

    /* uint32_t large_message_size;
     * uint8_t  spdm_chunk[chunk_size]; */
} spdm_chunk_send_request_t;

#define SPDM_CHUNK_SEND_REQUEST_ATTRIBUTE_LAST_CHUNK (1 << 0)

/* SPDM CHUNK_SEND_ACK response */
typedef struct {
    spdm_message_header_t header;
    /* param1 - Response Attributes
     * param2 - Handle */
    uint16_t chunk_seq_no;
    /* uint8_t response_to_large_request[variable] */
} spdm_chunk_send_ack_response_t;

#define SPDM_CHUNK_SEND_ACK_RESPONSE_ATTRIBUTE_EARLY_ERROR_DETECTED (1 << 0)

/* SPDM CHUNK_GET request */
typedef struct {
    spdm_message_header_t header;
    /* param1 - Reserved
    * param2 - Handle */
    uint16_t chunk_seq_no;
} spdm_chunk_get_request_t;

/* SPDM CHUNK_RESPONSE response */
typedef struct {
    spdm_message_header_t header;
    /* param1 - Response Attributes
     * param2 - Handle */
    uint16_t chunk_seq_no;
    uint16_t reserved;
    uint32_t chunk_size;

    /* uint32_t large_message_size;
     * uint8_t  spdm_chunk[chunk_size]; */
} spdm_chunk_response_response_t;

#define SPDM_CHUNK_GET_RESPONSE_ATTRIBUTE_LAST_CHUNK (1 << 0)

#pragma pack()

#define SPDM_VERSION_1_1_BIN_CONCAT_LABEL "spdm1.1 "
#define SPDM_VERSION_1_2_BIN_CONCAT_LABEL "spdm1.2 "
#define SPDM_BIN_STR_0_LABEL "derived"
#define SPDM_BIN_STR_1_LABEL "req hs data"
#define SPDM_BIN_STR_2_LABEL "rsp hs data"
#define SPDM_BIN_STR_3_LABEL "req app data"
#define SPDM_BIN_STR_4_LABEL "rsp app data"
#define SPDM_BIN_STR_5_LABEL "key"
#define SPDM_BIN_STR_6_LABEL "iv"
#define SPDM_BIN_STR_7_LABEL "finished"
#define SPDM_BIN_STR_8_LABEL "exp master"
#define SPDM_BIN_STR_9_LABEL "traffic upd"

/**
 * The maximum amount of time in microseconds the Responder has to provide a response
 * to requests that do not require cryptographic processing.
 **/
#define SPDM_ST1_VALUE_US 100000

/* id-DMTF 1.3.6.1.4.1.412 */
#define SPDM_OID_DMTF \
    { /*0x06, 0x07,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C }
/* id-DMTF-spdm, { id-DMTF 274 }, 1.3.6.1.4.1.412.274 */
#define SPDM_OID_DMTF_SPDM \
    { /*0x06, 0x09,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12 }
/* id-DMTF-device-info, { id-DMTF-spdm 1 }, 1.3.6.1.4.1.412.274.1 */
#define SPDM_OID_DMTF_DEVICE_INFO \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x01 }
/* id-DMTF-hardware-identity, { id-DMTF-spdm 2 }, 1.3.6.1.4.1.412.274.2 */
#define SPDM_OID_DMTF_HARDWARE_IDENTITY \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x02 }
/* id-DMTF-eku-responder-auth, { id-DMTF-spdm 3 }, 1.3.6.1.4.1.412.274.3 */
#define SPDM_OID_DMTF_EKU_RESPONDER_AUTH \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x03 }
/* id-DMTF-eku-requester-auth, { id-DMTF-spdm 4 }, 1.3.6.1.4.1.412.274.4 */
#define SPDM_OID_DMTF_EKU_REQUESTER_AUTH \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x04 }
/* id-DMTF-mutable-certificate, { id-DMTF-spdm 5 }, 1.3.6.1.4.1.412.274.5 */
#define SPDM_OID_DMTF_MUTABLE_CERTIFICATE \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x05 }
/* id-DMTF-SPDM-extension, { id-DMTF-spdm 6 }, 1.3.6.1.4.1.412.274.6 */
#define SPDM_OID_DMTF_SPDM_EXTENSION \
    { /*0x06, 0x0A,*/ 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x1C, 0x82, 0x12, 0x06 }

#endif /* SPDM_H */
