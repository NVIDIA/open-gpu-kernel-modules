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
 */


//
// Included libspdm copyright alongside NVIDIA copyright, as these functions
// are copied from libspdm cryptlib_null files. Meant to more easily stub out
// cryptography functions when not needed.
//

/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/


// Use headers directly so we don't rely on internal_crypt_lib.h
#include <base.h>
#include "library/debuglib.h"
#include "library/memlib.h"
#include "library/cryptlib.h"
#include <rmconfig.h>
#include "core/prelude.h"
// Stub out all crypto functions that we do not support.

bool libspdm_ec_get_private_key_from_pem(const uint8_t *pem_data, size_t pem_size,
                                         const char *password,
                                         void **ec_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_ec_get_public_key_from_der(const uint8_t *der_data,
                                        size_t der_size,
                                        void **ec_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_rsa_get_public_key_from_der(const uint8_t *der_data,
                                         size_t der_size,
                                         void **rsa_context)
{
    return true;
}

#if defined(NV_USE_MBEDTLS) && NV_USE_MBEDTLS
bool libspdm_check_crypto_backend(void)
{
    NV_PRINTF(LEVEL_INFO, "libspdm_check_crypto_backend: MbedTLS found\n");
    return true;
}
#endif

