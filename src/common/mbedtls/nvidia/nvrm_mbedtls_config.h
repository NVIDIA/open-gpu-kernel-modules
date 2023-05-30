/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

//
// This file provides custom configuration options for MbedTLS. It is included
// via the MBEDTLS_USER_CONFIG_FILE macro, which means it supplements the
// MbedTLS default "config.h" file. Therefore, in this file, we modify defaults
// as set by config.h, rather than edit them directly.
//
// These configurations were originally tuned specifically for libspdm on RM,
// so the defines may need to be modified if more functionality is desired.
//
// NOTE: Since RM cannot include freestanding headers like stdlib.h and string.h,
// we have removed or hijacked their inclusion and provided replacement
// definitions at the bottom of this file.
//

#ifndef _NVRM_MBEDTLS_CONFIG_H_
#define _NVRM_MBEDTLS_CONFIG_H_

// First, disable all features that we are not using.
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE
#undef MBEDTLS_CIPHER_MODE_CBC
#undef MBEDTLS_CIPHER_MODE_CFB
#undef MBEDTLS_CIPHER_MODE_OFB
#undef MBEDTLS_CIPHER_MODE_XTS
#undef MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#undef MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#undef MBEDTLS_CIPHER_PADDING_ZEROS
#undef MBEDTLS_ECP_DP_SECP192R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP192K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP256K1_ENABLED
#undef MBEDTLS_ECP_DP_BP256R1_ENABLED
#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
#undef MBEDTLS_ECP_DP_CURVE25519_ENABLED
#undef MBEDTLS_ECP_DP_CURVE448_ENABLED
#undef MBEDTLS_ECP_NIST_OPTIM
#undef MBEDTLS_ECDSA_DETERMINISTIC
#undef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#undef MBEDTLS_PK_PARSE_EC_EXTENDED
#undef MBEDTLS_GENPRIME
#undef MBEDTLS_FS_IO
#undef MBEDTLS_MPI_WINDOWS_SIZE
#undef MBEDTLS_PK_RSA_ALT_SUPPORT
#undef MBEDTLS_PKCS1_V15
#undef MBEDTLS_PKCS1_V21
#undef MBEDTLS_SELF_TEST
#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES
#undef MBEDTLS_SSL_ENCRYPT_THEN_MAC
#undef MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#undef MBEDTLS_SSL_FALLBACK_SCSV
#undef MBEDTLS_SSL_CBC_RECORD_SPLITTING
#undef MBEDTLS_SSL_RENEGOTIATION
#undef MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#undef MBEDTLS_SSL_PROTO_TLS1
#undef MBEDTLS_SSL_PROTO_TLS1_1
#undef MBEDTLS_SSL_PROTO_TLS1_2
#undef MBEDTLS_SSL_PROTO_DTLS
#undef MBEDTLS_SSL_ALPN
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#undef MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#undef MBEDTLS_SSL_SESSION_TICKETS
#undef MBEDTLS_SSL_EXPORT_KEYS
#undef MBEDTLS_SSL_SERVER_NAME_INDICATION
#undef MBEDTLS_SSL_TRUNCATED_HMAC
#undef MBEDTLS_X509_CHECK_KEY_USAGE
#undef MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE
#undef MBEDTLS_X509_RSASSA_PSS_SUPPORT
#undef MBEDTLS_AESNI_C
#undef MBEDTLS_ARC4_C
#undef MBEDTLS_BLOWFISH_C
#undef MBEDTLS_CAMELLIA_C
#undef MBEDTLS_CCM_C
#undef MBEDTLS_CERTS_C
#undef MBEDTLS_CHACHA20_C
#undef MBEDTLS_CHACHAPOLY_C
#undef MBEDTLS_CTR_DRBG_C
#undef MBEDTLS_DEBUG_C
#undef MBEDTLS_DES_C
#undef MBEDTLS_DHM_C
#undef MBEDTLS_ENTROPY_C
#undef MBEDTLS_ERROR_C
#undef MBEDTLS_HMAC_DRBG_C
#undef MBEDTLS_MD5_C
#undef MBEDTLS_NET_C
#undef MBEDTLS_PADLOCK_C
#undef MBEDTLS_PEM_PARSE_C
#undef MBEDTLS_PK_WRITE_C
#undef MBEDTLS_PKCS5_C
#undef MBEDTLS_PKCS12_C
#undef MBEDTLS_POLY1305_C
#undef MBEDTLS_RIPEMD160_C
#undef MBEDTLS_RSA_C
#undef MBEDTLS_SHA1_C
#undef MBEDTLS_SSL_CACHE_C
#undef MBEDTLS_SSL_COOKIE_C
#undef MBEDTLS_SSL_TICKET_C
#undef MBEDTLS_SSL_CLI_C
#undef MBEDTLS_SSL_SRV_C
#undef MBEDTLS_SSL_TLS_C
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_X509_CSR_PARSE_C
#undef MBEDTLS_X509_CREATE_C
#undef MBEDTLS_X509_CRT_WRITE_C
#undef MBEDTLS_X509_CSR_WRITE_C
#undef MBEDTLS_XTEA_C

//
// The commented out '#undef's below are features set by MbedTLS's config.h
// that we want to keep enabled. Listed here for tracking purposes.
//
//#undef MBEDTLS_ECP_DP_SECP256R1_ENABLED
//#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#undef MBEDTLS_ASN1_WRITE_C
//#undef MBEDTLS_BASE64_C
//#undef MBEDTLS_BIGNUM_C
//#undef MBEDTLS_ECDH_C
//#undef MBEDTLS_ECDSA_C
//#undef MBEDTLS_ECP_C
//#undef MBEDTLS_HKDF_C
//#undef MBEDTLS_OID_C
//#undef MBEDTLS_PEM_WRITE_C
//#undef MBEDTLS_PK_C
//#undef MBEDTLS_SHA512_C
//#undef MBEDTLS_X509_USE_C
//#undef MBEDTLS_X509_CRT_PARSE_C
//#undef MBEDTLS_X509_CRL_PARSE_C

// Explicitly define any features we need that aren't already set.
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_PK_PARSE_C

// Kernel doesn't have 128bit division.
#define MBEDTLS_NO_UDBL_DIVISION

// Define for hopeful perf boost due to better memory layout.
#define MBEDTLS_AES_ROM_TABLES

// Stack size is too large with default window size
#define MBEDTLS_MPI_WINDOW_SIZE 1

//
// Use custom memory allocation. These functions will be provided
// by libspdm wrappers around NvPort implementations.
//
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_PLATFORM_ZEROIZE_ALT
#define MBEDTLS_PLATFORM_SNPRINTF_ALT
#define MBEDTLS_PLATFORM_STD_SNPRINTF mbedtls_snprintf

//
// These changes are all WARs for system headers, required for Linux RM builds.
// We prevent their inclusion in MbedTLS source, either via editing MbedTLS
// source, or hijacking their compile guards before they are included.
// We provide definitions of any required macros, types, or functions manually.
//

// WARs for limits.h
#if !defined(_GCC_LIMITS_H_)
#define _GCC_LIMITS_H_
#define INT_MAX +2147483647
#endif // _GCC_LIMITS_H_

// WAR to waive self-test in check_config.h.
#define CHAR_BIT 8U

// Include stdint.h directly here to avoid missing dependencies.
#include <stdint.h>

// WARs for string.h - we will use NvPort functions in their place.
#include "nvport/nvport.h"

#define memcmp(str1, str2, n) portMemCmp((str1), (str2), (NvLength)(n))
#define memcpy(dest, src, n)  portMemCopy((dest), (NvLength)(n), (src), (NvLength)(n))
#define memset(str, c, n)     portMemSet((str), (NvU8)(c), (NvLength)(n))
#define memmove(dest, src, n) portMemMove((dest), (NvLength)(n), (src), (NvLength)(n))
#define strlen(str)           portStringLength(str)

// NvPort implementation doesn't have max length parameter, so set arbitrary number.
#define strcmp(str1, str2)    portStringCompare ((str1), (str2), (0x1000))

#endif // _NVRM_MBEDTLS_CONFIG_H_
