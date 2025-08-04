/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <base.h>
#include "library/debuglib.h"
#include "library/memlib.h"
#include "library/cryptlib.h"
#include "nvspdm_cryptlib_extensions.h"
#include <rmconfig.h>
#include "core/prelude.h"
#include "nvport/nvport.h"

static const uint8_t SPDM_PEM_BEGIN_CERTIFICATE[] = "-----BEGIN CERTIFICATE-----\n";
static const uint8_t SPDM_PEM_END_CERTIFICATE[]   = "-----END CERTIFICATE-----\n";

#if defined(NV_USE_MBEDTLS) && NV_USE_MBEDTLS
#include <mbedtls/base64.h>
bool libspdm_encode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return mbedtls_base64_encode(dst, *p_dstlen, p_dstlen, src, srclen) == 0;
}

bool libspdm_decode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return mbedtls_base64_decode(dst, *p_dstlen, p_dstlen, src, srclen) == 0;
}
#endif

static bool libspdm_encode_base64_with_newline(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    size_t s_progress;
    size_t d_progress;
    size_t d_bufsize = *p_dstlen;
    size_t encode_size;
    size_t encoded_size;

    // for each round we encode 48 bytes and make a linebreak
    for (s_progress = d_progress = 0; s_progress < srclen; s_progress += 48)
    {
        if (s_progress + 48 < srclen)
        {
            encode_size = 48;
        }
        else
        {
            encode_size = srclen - s_progress;
        }
        encoded_size = d_bufsize - d_progress;
        if (!libspdm_encode_base64(src + s_progress, dst + d_progress, encode_size, &encoded_size))
        {
            return false;
        }
        d_progress += encoded_size;
        dst[d_progress++] = '\n';
    }
    *p_dstlen = d_progress;
    return true;
}

bool libspdm_pem_to_der(const uint8_t *pem_cert, uint8_t *der_cert, size_t pem_size, size_t *p_der_size)
{
    const uint8_t *b64_buf;
    uint64_t       b64_buf_size;
    // Check if pointers are valid.
    if (pem_cert == NULL || der_cert == NULL)
    {
        return false;
    }

    // If the size is smaller than the header size than it's definitely not valid.
    if (pem_size < sizeof(SPDM_PEM_BEGIN_CERTIFICATE) + sizeof(SPDM_PEM_END_CERTIFICATE) + 2)
    {
        return false;
    }

    b64_buf_size = pem_size - (sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1) - (sizeof(SPDM_PEM_END_CERTIFICATE) - 1);

    // Check if cert headers are valid.
    if (portMemCmp(pem_cert, SPDM_PEM_BEGIN_CERTIFICATE, sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1) != 0)
    {
        return false;
    }

    b64_buf = pem_cert + sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1;

    if (portMemCmp(pem_cert + pem_size - sizeof(SPDM_PEM_END_CERTIFICATE) + 1,
                   SPDM_PEM_END_CERTIFICATE,
                   sizeof(SPDM_PEM_END_CERTIFICATE) - 1) != 0)
    {
        return false;
    }

    if (!libspdm_decode_base64(b64_buf, der_cert, b64_buf_size, p_der_size))
    {
        return false;
    }

    return true;
}

bool libspdm_der_to_pem(const uint8_t *der_cert, uint8_t *pem_cert, size_t der_size, size_t *p_pem_size)
{
    uint8_t *ptr = pem_cert;
    size_t remaining_buffer_size = *p_pem_size;
    size_t encoded_size;

    if (remaining_buffer_size < sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1)
    {
        return false;
    }

    portMemCopy(ptr, *p_pem_size, SPDM_PEM_BEGIN_CERTIFICATE,
                sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1);
    ptr += sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1;
    remaining_buffer_size -= sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1;

    encoded_size = remaining_buffer_size;
    if (!libspdm_encode_base64_with_newline(der_cert, ptr, der_size, &encoded_size))
    {
        return false;
    }

    ptr += encoded_size;
    remaining_buffer_size -= encoded_size;

    if (remaining_buffer_size < sizeof(SPDM_PEM_END_CERTIFICATE))
    {
        return false;
    }

    // Include the null byte at the end
    portMemCopy(ptr, remaining_buffer_size, SPDM_PEM_END_CERTIFICATE, sizeof(SPDM_PEM_END_CERTIFICATE));
    ptr += sizeof(SPDM_PEM_END_CERTIFICATE);
    remaining_buffer_size -= sizeof(SPDM_PEM_END_CERTIFICATE);

    *p_pem_size = ptr - pem_cert;
    return true;
}