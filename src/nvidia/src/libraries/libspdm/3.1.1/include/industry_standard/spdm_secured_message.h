/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * Definitions of DSP0277 Secured Messages using SPDM Specification
 * version 1.0.0 in Distributed Management Task Force (DMTF).
 **/

#ifndef SPDM_SECURED_MESSAGE_H
#define SPDM_SECURED_MESSAGE_H

#pragma pack(1)


/* ENC+AUTH session:*/

/* +-----------------+
 * | ApplicationData |-----------------------------------------------------
 * +-----------------+                                                     |
 *                                                                         V
 * +---------------------------------+----------------------------------+-------+------+---+
 * |SPDM_SECURED_MESSAGE_ADATA_HEADER|SPDM_SECURED_MESSAGE_CIPHER_HEADER|AppData|Random|MAC|
 * | SessionId | SeqNum (O) | Length |       ApplicationDataLength      |       |  (O) |   |
 * +---------------------------------+----------------------------------+-------+------+---+
 * |                                 |                                                 |   |
 *  --------------------------------- ------------------------------------------------- ---
 *                  |                                         |                          |
 *                  V                                         V                          V
 *            AssociatedData                            EncryptedData                 AeadTag
 */

/* (O) means Optional or Transport Layer Specific.*/

/* AUTH session:*/

/* +-----------------+
 * | ApplicationData |------------------
 * +-----------------+                  |
 *                                      V
 * +---------------------------------+-------+---+
 * |SPDM_SECURED_MESSAGE_ADATA_HEADER|AppData|MAC|
 * | SessionId | SeqNum (O) | length |       |   |
 * +---------------------------------+-------+---+
 * |                                         |   |
 *  ----------------------------------------- ---
 *                      |                     |
 *                      V                     V
 *                AssociatedData           AeadTag
 */

/* 2 means SPDM secured message version 1.0, 1.1 */
#define SECURED_SPDM_MAX_VERSION_COUNT 2
/* the DSP0277 specification version */
#define SECURED_SPDM_VERSION_10 0x10
#define SECURED_SPDM_VERSION_11 0x11

typedef struct {
    uint32_t session_id;
} spdm_secured_message_a_data_header1_t;

/* The length of sequence_number between HEADER_1 and HEADER_2 is transport specific.*/

typedef struct {
    uint16_t length; /* The length of the remaining data, including application_data_length(O), payload, Random(O) and MAC.*/
} spdm_secured_message_a_data_header2_t;

typedef struct {
    uint16_t application_data_length; /* The length of the payload*/
} spdm_secured_message_cipher_header_t;


/* Secured Messages opaque data format*/

#define SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID 0x444D5446
#define SECURED_MESSAGE_OPAQUE_VERSION 0x1

typedef struct {
    uint32_t spec_id; /* SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID*/
    uint8_t opaque_version; /* SECURED_MESSAGE_OPAQUE_VERSION*/
    uint8_t total_elements;
    uint16_t reserved;
    /*opaque_element_table_t  opaque_list[];*/
} secured_message_general_opaque_data_table_header_t;

typedef struct {
    uint8_t id;
    uint8_t vendor_len;
    /*uint8_t    vendor_id[vendor_len];
     * uint16_t   opaque_element_data_len;
     * uint8_t    opaque_element_data[opaque_element_data_len];
     * uint8_t    align_padding[];*/
} opaque_element_table_header_t;

#define SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION 0x1

typedef struct {
    uint8_t id; /* SPDM_REGISTRY_ID_DMTF*/
    uint8_t vendor_len;
    uint16_t opaque_element_data_len;
    /*uint8_t    sm_data_version;
     * uint8_t    sm_data_id;
     * uint8_t    sm_data[];*/
} secured_message_opaque_element_table_header_t;

typedef struct {
    uint8_t sm_data_version; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION*/
    uint8_t sm_data_id; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION*/
} secured_message_opaque_element_header_t;

#define SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION 0x0

typedef struct {
    uint8_t sm_data_version; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION*/
    uint8_t sm_data_id; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION*/
    spdm_version_number_t selected_version;
} secured_message_opaque_element_version_selection_t;

#define SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_SUPPORTED_VERSION 0x1

typedef struct {
    uint8_t sm_data_version; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION*/
    uint8_t sm_data_id; /* SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_SUPPORTED_VERSION*/
    uint8_t version_count;
    /*spdm_version_number_t   versions_list[version_count];*/
} secured_message_opaque_element_supported_version_t;

#pragma pack()

#endif /* SPDM_SECURED_MESSAGE_H */
