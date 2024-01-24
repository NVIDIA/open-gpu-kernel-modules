/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef SPDM_RETURN_STATUS_H
#define SPDM_RETURN_STATUS_H

#ifndef LIBSPDM_STDINT_ALT
#include <stdint.h>
#else
#include LIBSPDM_STDINT_ALT
#endif

/* The layout of libspdm_return_t is
 * [31:28] - Severity
 * [27:24] - Reserved
 * [23:16] - Source
 * [15:00] - Code
 */
typedef uint32_t libspdm_return_t;

/* Returns 1 if severity is LIBSPDM_SEVERITY_SUCCESS else it returns 0. */
#define LIBSPDM_STATUS_IS_SUCCESS(status) \
    (LIBSPDM_STATUS_SEVERITY(status) == LIBSPDM_SEVERITY_SUCCESS)

/* Returns 1 if severity is LIBSPDM_SEVERITY_ERROR else it returns 0. */
#define LIBSPDM_STATUS_IS_ERROR(status) \
    (LIBSPDM_STATUS_SEVERITY(status) == LIBSPDM_SEVERITY_ERROR)

/* Returns 1 if severity is LIBSPDM_SEVERITY_WARNING else it returns 0. */
#define LIBSPDM_STATUS_IS_WARNING(status) \
    (LIBSPDM_STATUS_SEVERITY(status) == LIBSPDM_SEVERITY_WARNING)

/* Returns the severity of the status. */
#define LIBSPDM_STATUS_SEVERITY(status) (((status) >> 28) & 0xf)

/* Returns the source of the status. */
#define LIBSPDM_STATUS_SOURCE(status) (((status) >> 16) & 0xff)

#define LIBSPDM_SEVERITY_SUCCESS 0x0
#define LIBSPDM_SEVERITY_WARNING 0x4
#define LIBSPDM_SEVERITY_ERROR 0x8

#define LIBSPDM_SOURCE_SUCCESS 0x00
#define LIBSPDM_SOURCE_CORE 0x01
#define LIBSPDM_SOURCE_CRYPTO 0x02
#define LIBSPDM_SOURCE_CERT_PARSE 0x03
#define LIBSPDM_SOURCE_TRANSPORT 0x04
#define LIBSPDM_SOURCE_MEAS_COLLECT 0x05
#define LIBSPDM_SOURCE_RNG 0x06

#define LIBSPDM_STATUS_CONSTRUCT(severity, source, code) \
    ((libspdm_return_t)(((severity) << 28) | ((source) << 16) | (code)))

/* Success status is always 0x00000000. */
#define LIBSPDM_STATUS_SUCCESS \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_SUCCESS, LIBSPDM_SOURCE_SUCCESS, 0x0000)

#define LIBSPDM_RET_ON_ERR(status) \
    do { \
        if (LIBSPDM_STATUS_IS_ERROR(status)) { \
            return (status); \
        } \
    } \
    while (0)

/* - Core Errors - */

/* The function input parameter is invalid. */
#define LIBSPDM_STATUS_INVALID_PARAMETER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0001)

/* Unable to complete operation due to unsupported capabilities by either the caller, the peer,
 * or both. */
#define LIBSPDM_STATUS_UNSUPPORTED_CAP \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0002)

/* Unable to complete operation due to caller's state. */
#define LIBSPDM_STATUS_INVALID_STATE_LOCAL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0003)

/* Unable to complete operation due to peer's state. */
#define LIBSPDM_STATUS_INVALID_STATE_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0004)

/* The received message contains one or more invalid message fields. */
#define LIBSPDM_STATUS_INVALID_MSG_FIELD \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0005)

/* The received message's size is invalid. */
#define LIBSPDM_STATUS_INVALID_MSG_SIZE \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0006)

/* Unable to derive a common set of versions, algorithms, etc. */
#define LIBSPDM_STATUS_NEGOTIATION_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0007)

/* Received a Busy error message. */
#define LIBSPDM_STATUS_BUSY_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0008)

/* Received a NotReady error message. */
#define LIBSPDM_STATUS_NOT_READY_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0009)

/* Received an unexpected error message. */
#define LIBSPDM_STATUS_ERROR_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000a)

/* Received a RequestResynch error message. */
#define LIBSPDM_STATUS_RESYNCH_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000b)

/* Unable to append new data to buffer due to resource exhaustion. */
#define LIBSPDM_STATUS_BUFFER_FULL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000c)

/* Unable to return data because caller does not provide big enough buffer. */
#define LIBSPDM_STATUS_BUFFER_TOO_SMALL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000d)

/* Unable to allocate more session. */
#define LIBSPDM_STATUS_SESSION_NUMBER_EXCEED \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000e)

/* Decrypt error from peer. */
#define LIBSPDM_STATUS_SESSION_MSG_ERROR \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x000f)

/* Unable to acquire resource. */
#define LIBSPDM_STATUS_ACQUIRE_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0010)

/* Re-triable decrypt error from peer - must rollback to backup keys. */
#define LIBSPDM_STATUS_SESSION_TRY_DISCARD_KEY_UPDATE \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0011)

/* The device needs a reset after the operation (such as set certificate). */
#define LIBSPDM_STATUS_RESET_REQUIRED_PEER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0012)

/* Message is too large to send to peer. */
#define LIBSPDM_STATUS_PEER_BUFFER_TOO_SMALL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CORE, 0x0013)

/* A parameter passed by the Integrator was overridden. */
#define LIBSPDM_STATUS_OVERRIDDEN_PARAMETER \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_WARNING, LIBSPDM_SOURCE_CORE, 0x0014)

/* - Cryptography Errors - */

/* Generic failure originating from the cryptography module. */
#define LIBSPDM_STATUS_CRYPTO_ERROR \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CRYPTO, 0x0000)

/* Verification of the provided signature digest, signature, or AEAD tag failed. */
#define LIBSPDM_STATUS_VERIF_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CRYPTO, 0x0001)

/* AEAD sequence number overflow. */
#define LIBSPDM_STATUS_SEQUENCE_NUMBER_OVERFLOW \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CRYPTO, 0x0002)

/* Provided cert is valid but is not authoritative(mismatch the root cert). */
#define LIBSPDM_STATUS_VERIF_NO_AUTHORITY \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_WARNING, LIBSPDM_SOURCE_CRYPTO, 0x0003)

/* FIPS test failed */
#define LIBSPDM_STATUS_FIPS_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CRYPTO, 0x0004)

/* - Certificate Parsing Errors - */

/* Certificate is malformed or does not comply to x.509 standard. */
#define LIBSPDM_STATUS_INVALID_CERT \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_CERT_PARSE, 0x0000)

/* - Transport Errors - */

/* Unable to send message to peer. */
#define LIBSPDM_STATUS_SEND_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_TRANSPORT, 0x0000)

/* Unable to receive message from peer. */
#define LIBSPDM_STATUS_RECEIVE_FAIL \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_TRANSPORT, 0x0001)

/* - Measurement Collection Errors - */

/* Unable to collect measurement because of invalid index. */
#define LIBSPDM_STATUS_MEAS_INVALID_INDEX \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_MEAS_COLLECT, 0x0000)

/* Unable to collect measurement because of internal error. */
#define LIBSPDM_STATUS_MEAS_INTERNAL_ERROR \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_MEAS_COLLECT, 0x0001)

/* - Random Number Generation Errors - */

/* Unable to produce random number due to lack of entropy. */
#define LIBSPDM_STATUS_LOW_ENTROPY \
    LIBSPDM_STATUS_CONSTRUCT(LIBSPDM_SEVERITY_ERROR, LIBSPDM_SOURCE_RNG, 0x0000)

#endif
