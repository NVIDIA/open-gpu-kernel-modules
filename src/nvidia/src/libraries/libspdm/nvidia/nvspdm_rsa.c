/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 * Comments, prototypes and checks taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
 * License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 */
#include <base.h>
#include "library/debuglib.h"
#include "library/memlib.h"
#include "library/cryptlib.h"
#include "libraries/utils/nvprintf.h"
#include <rmconfig.h>
#include "industry_standard/spdm.h"
#include "spdm/rmspdmrsakeys.h"

bool libspdm_requester_data_sign
(
    spdm_version_number_t spdm_version,
    uint8_t               op_code,
    uint16_t              req_base_asym_alg,
    uint32_t              base_hash_algo,
    bool                  is_data_hash,
    uint8_t              *message,
    size_t                message_size,
    uint8_t              *signature,
    size_t               *sig_size
)
{
    void      *rsa = NULL;
    bool       status;

    if (op_code           != SPDM_FINISH                                        ||
        req_base_asym_alg != SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072 ||
        base_hash_algo    != SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384)
    {
        return false;
    }

#if LIBSPDM_RSA_PSS_SUPPORT

    /* Generate & Initialize RSA context*/
    rsa = libspdm_rsa_new();

    if (rsa == NULL)
    {
        return false;
    }

    //
    // Set/Get RSA key N modulus, we hardcode prod key now.
    // mbedtls needs to reverse key component endian.
    //
    status = libspdm_rsa_set_key(rsa, LIBSPDM_RSA_KEY_N,
                                 g_rsa3k_modulus_mutual_authentication_prod,
                                 sizeof(g_rsa3k_modulus_mutual_authentication_prod));

    if (!status)
    {
        libspdm_rsa_free(rsa);
        return false;
    }

    //
    // Set/Get RSA key D private exponent, we hardcode prod key now.
    // mbedtls needs to reverse key component endian.
    //
    status = libspdm_rsa_set_key(rsa, LIBSPDM_RSA_KEY_D,
                                 g_rsa3k_private_exponent_mutual_authentication_prod,
                                 sizeof(g_rsa3k_private_exponent_mutual_authentication_prod));

    if (!status)
    {
        libspdm_rsa_free(rsa);
        return false;
    }

    //
    // Set/Get RSA key E public exponent, we hardcode prod key now.
    // mbedtls needs to reverse key component endian.
    // Though we don't need KEY_E for sign process; but we still need to setup KEY_E to
    // make medtls pass sanity check.
    //
    status = libspdm_rsa_set_key(rsa, LIBSPDM_RSA_KEY_E,
                                 g_rsa3k_public_exponent_mutual_authentication_prod,
                                 sizeof(g_rsa3k_public_exponent_mutual_authentication_prod));

    if (!status)
    {
        libspdm_rsa_free(rsa);
        return false;
    }

    // RSA-PSS signature generation.
    status = libspdm_rsa_pss_sign(rsa, LIBSPDM_CRYPTO_NID_SHA384,
                                  message, message_size,
                                  signature, sig_size);

    if (!status)
    {
        libspdm_rsa_free(rsa);
        return false;
    }

    libspdm_rsa_free(rsa);

    return true;
#else
    return false;
#endif //LIBSPDM_RSA_PSS_SUPPORT
}
