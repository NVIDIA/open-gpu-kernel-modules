/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// RM's configuration values for libspdm. Replaces default values in spdm_lib_config.h.
// Must define LIBSPDM_CONFIG in CFLAGS during build in order to successfully replace.
//
// NOTE: Any updates to macros must first test a local build with LIBSPDM_CHECK_MACRO
// set to 1! This ensures the macro combination is valid. Kept at 0 since adds
// overhead to build otherwise.
//
#define LIBSPDM_CHECK_MACRO                       1

//
// Enable FIPS 140-3 mode.
// TODO: Because chips_a doesn't get the latest mbedtls yet, if we enable LIBSPDM_FIPS_MODE, we'll hit
//       compiler error. Need to disable this flag until mbedtls get update.
//
#define LIBSPDM_FIPS_MODE                         0

//
// We want debug asserts enabled, but we don't care about debug prints or the debug blocks.
// Don't define LIBSPDM_DEBUG_ENABLE, as it will override these settings below.
//
#define LIBSPDM_DEBUG_ASSERT_ENABLE               1
#define LIBSPDM_DEBUG_PRINT_ENABLE                1
#define LIBSPDM_DEBUG_BLOCK_ENABLE                0

// These settings are all libspdm default.
#define LIBSPDM_MAX_VERSION_COUNT                 5
#define LIBSPDM_PSK_CONTEXT_LENGTH                LIBSPDM_MAX_HASH_SIZE
#define LIBSPDM_PSK_MAX_HINT_LENGTH               16
#define LIBSPDM_MAX_ROOT_CERT_SUPPORT             1

// Limit to one session at a time.
#define LIBSPDM_MAX_SESSION_COUNT                 1

#define LIBSPDM_MAX_CERT_CHAIN_SIZE               0x1000
#define LIBSPDM_MAX_MEASUREMENT_RECORD_SIZE       0x1000
#define LIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN          LIBSPDM_MAX_CERT_CHAIN_SIZE

// Use running hash rather than recording all transcript data.
#define LIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT    0

#define LIBSPDM_ECDSA_P384_SUPPORT                1
#define LIBSPDM_RSA_SSA_2048_SUPPORT              0
#define LIBSPDM_RSA_SSA_3072_SUPPORT              0
#define LIBSPDM_RSA_SSA_4096_SUPPORT              0
#define LIBSPDM_RSA_PSS_2048_SUPPORT              0
#define LIBSPDM_RSA_PSS_4096_SUPPORT              0
#define LIBSPDM_ECDSA_P256_SUPPORT                0
#define LIBSPDM_ECDSA_P521_SUPPORT                0
#define LIBSPDM_SM2_DSA_P256_SUPPORT              0
#define LIBSPDM_EDDSA_ED25519_SUPPORT             0
#define LIBSPDM_EDDSA_ED448_SUPPORT               0

#define LIBSPDM_ECDHE_P384_SUPPORT                1
#define LIBSPDM_FFDHE_2048_SUPPORT                0
#define LIBSPDM_FFDHE_3072_SUPPORT                0
#define LIBSPDM_FFDHE_4096_SUPPORT                0
#define LIBSPDM_ECDHE_P256_SUPPORT                0
#define LIBSPDM_ECDHE_P521_SUPPORT                0
#define LIBSPDM_SM2_KEY_EXCHANGE_P256_SUPPORT     0

#define LIBSPDM_AEAD_AES_256_GCM_SUPPORT          1
#define LIBSPDM_AEAD_AES_128_GCM_SUPPORT          0
#define LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT    0
#define LIBSPDM_AEAD_SM4_128_GCM_SUPPORT          0

#define LIBSPDM_SHA256_SUPPORT                    1
#define LIBSPDM_SHA384_SUPPORT                    1
#define LIBSPDM_SHA512_SUPPORT                    0
#define LIBSPDM_SHA3_256_SUPPORT                  0
#define LIBSPDM_SHA3_384_SUPPORT                  0
#define LIBSPDM_SHA3_512_SUPPORT                  0
#define LIBSPDM_SM3_256_SUPPORT                   0

#define LIBSPDM_CERT_PARSE_SUPPORT                1

#define LIBSPDM_ENABLE_CAPABILITY_CERT_CAP        1
#define LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP        1
#define LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP      1
#define LIBSPDM_ENABLE_CAPABILITY_HBEAT_CAP       1
#define LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP        0
#define LIBSPDM_ENABLE_CAPABILITY_PSK_CAP         0
#define LIBSPDM_ENABLE_CAPABILITY_CSR_CAP         0
#define LIBSPDM_ENABLE_CAPABILITY_SET_CERT_CAP    0
#define LIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP       0

#define LIBSPDM_SEND_GET_CERTIFICATE_SUPPORT      1
#define LIBSPDM_SEND_CHALLENGE_SUPPORT            0
#define LIBSPDM_RESPOND_IF_READY_SUPPORT          0

// Enable message log to simplify Attestation Report retrieval.
#define LIBSPDM_ENABLE_MSG_LOG                    1

// Enable runtime checks to ensure SPDM context validity.
#define LIBSPDM_CHECK_SPDM_CONTEXT                1

// Enable mutual authentication and encapsulated process both.
#define LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP        1
#define LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP           1
#define LIBSPDM_RSA_PSS_3072_SUPPORT                  1

#define LIBSPDM_RSA_PSS_SUPPORT ((LIBSPDM_RSA_PSS_2048_SUPPORT) || \
                                 (LIBSPDM_RSA_PSS_3072_SUPPORT) || \
                                 (LIBSPDM_RSA_PSS_4096_SUPPORT))

#endif // _NVSPDM_RMCONFIG_H_
