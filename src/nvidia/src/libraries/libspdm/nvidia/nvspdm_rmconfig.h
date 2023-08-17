/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVSPDM_RMCONFIG_H_
#define _NVSPDM_RMCONFIG_H_

//
// NOTE: Any updates to macros must first test a local build with LIBSPDM_CHECK_MACRO
// set to 1! This ensures the macro combination is valid. Kept at 0 since adds
// overhead to build otherwise.
//
#define LIBSPDM_CHECK_MACRO 0

//
// RM's configuration values for libspdm. Replaces default values in spdm_lib_config.h.
// Must define LIBSPDM_CONFIG in CFLAGS during build in order to successfully replace.
//
#define LIBSPDM_MAX_VERSION_COUNT 5
#define LIBSPDM_MAX_SESSION_COUNT 1

#define LIBSPDM_PSK_CONTEXT_LENGTH  LIBSPDM_MAX_HASH_SIZE
#define LIBSPDM_PSK_MAX_HINT_LENGTH 16

#define LIBSPDM_MAX_ROOT_CERT_SUPPORT 1

#define LIBSPDM_MAX_MEASUREMENT_BLOCK_COUNT 64
#define LIBSPDM_MAX_MEASUREMENT_RECORD_SIZE 0x1000

#define LIBSPDM_MAX_CERT_CHAIN_SIZE      0x1000
#define LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN 1024

#define LIBSPDM_MAX_MESSAGE_BUFFER_SIZE        0x1100
#define LIBSPDM_MAX_MESSAGE_SMALL_BUFFER_SIZE  0x100
#define LIBSPDM_MAX_MESSAGE_MEDIUM_BUFFER_SIZE 0x300

// Do not retry messages.
#define LIBSPDM_MAX_REQUEST_RETRY_TIMES 0

// We don't expect to use callbacks, nonzero value is required for compilation.
#define LIBSPDM_MAX_SESSION_STATE_CALLBACK_NUM    1
#define LIBSPDM_MAX_CONNECTION_STATE_CALLBACK_NUM 1
#define LIBSPDM_MAX_KEY_UPDATE_CALLBACK_NUM       1

// CSR's are not supported.
#define LIBSPDM_MAX_CSR_SIZE 0x0

// Use running hash over recording all transcript data.
#define LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT 0

#define LIBSPDM_ECDSA_SUPPORT         1
#define LIBSPDM_RSA_SSA_SUPPORT       0
#define LIBSPDM_RSA_PSS_SUPPORT       0
#define LIBSPDM_SM2_DSA_SUPPORT       0
#define LIBSPDM_EDDSA_ED25519_SUPPORT 0
#define LIBSPDM_EDDSA_ED448_SUPPORT   0

#define LIBSPDM_ECDHE_SUPPORT            1
#define LIBSPDM_FFDHE_SUPPORT            0
#define LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT 0

#define LIBSPDM_AEAD_GCM_SUPPORT               1
#define LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT 0
#define LIBSPDM_AEAD_SM4_SUPPORT               0

#define LIBSPDM_SHA256_SUPPORT   1
#define LIBSPDM_SHA384_SUPPORT   1
#define LIBSPDM_SHA512_SUPPORT   0
#define LIBSPDM_SHA3_256_SUPPORT 0
#define LIBSPDM_SHA3_384_SUPPORT 0
#define LIBSPDM_SHA3_512_SUPPORT 0
#define LIBSPDM_SM3_256_SUPPORT  0

#define LIBSPDM_ENABLE_CAPABILITY_CERT_CAP            1
#define LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP            1
#define LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP          1
#define LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP            0
#define LIBSPDM_ENABLE_CAPABILITY_PSK_EX_CAP          0
#define LIBSPDM_ENABLE_CAPABILITY_HBEAT_CAP           0
#define LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP        0
#define LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP           0
#define LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP         0
#define LIBSPDM_ENABLE_CAPABILITY_SET_CERTIFICATE_CAP 0
#define LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP           0

// Compiler will complain if these deprecated aliases of above macros are not defined.
#define LIBSPDM_ENABLE_CHUNK_CAP           0
#define LIBSPDM_ENABLE_SET_CERTIFICATE_CAP 0

#define LIBSPDM_MAX_SPDM_MSG_SIZE  LIBSPDM_MAX_MESSAGE_BUFFER_SIZE
#define LIBSPDM_DATA_TRANSFER_SIZE LIBSPDM_MAX_MESSAGE_BUFFER_SIZE

#define LIBSPDM_TRANSPORT_ADDITIONAL_SIZE  64
#define LIBSPDM_SENDER_RECEIVE_BUFFER_SIZE (LIBSPDM_DATA_TRANSFER_SIZE + \
                                            LIBSPDM_TRANSPORT_ADDITIONAL_SIZE)

#define LIBSPDM_SCRATCH_BUFFER_SIZE        (LIBSPDM_SENDER_RECEIVE_BUFFER_SIZE)

//
// We want debug asserts enabled, but we don't care about debug prints or the debug blocks.
// Don't define LIBSPDM_DEBUG_ENABLE, as it will override these settings below.
//
#define LIBSPDM_DEBUG_ASSERT_ENABLE 1
#define LIBSPDM_DEBUG_PRINT_ENABLE  0
#define LIBSPDM_DEBUG_BLOCK_ENABLE  0

// Enable in version 3.0 when we can use to replace transcript WAR.
#define LIBSPDM_ENABLE_MSG_LOG 0

#endif // _NVSPDM_RMCONFIG_H_
