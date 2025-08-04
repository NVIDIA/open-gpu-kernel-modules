/*
* SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* libspdm_x509_verify_cert_chain, libspdm_x509_get_cert_from_cert_chain, check
* and prototypes taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
* License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
*/

#include "internal_crypt_lib.h"

#ifdef USE_LKCA
#include <crypto/public_key.h>
#include <keys/asymmetric-type.h>
#endif

bool libspdm_x509_construct_certificate(const uint8_t *cert, size_t cert_size,
                                        uint8_t **single_x509_cert)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_construct_certificate_stack(uint8_t **x509_stack, ...)
{
    LIBSPDM_ASSERT(false);
    return false;
}

void libspdm_x509_free(void *x509_cert)
{
    LIBSPDM_ASSERT(false);
}

void libspdm_x509_stack_free(void *x509_stack)
{
    LIBSPDM_ASSERT(false);
}

#ifdef USE_LKCA
bool libspdm_encode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    static const uint8_t base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i;
    size_t tmp;
    size_t size;
    uint8_t *ptr = dst;

    for (i = 0; (i + 2) < srclen; i += 3)
    {
        if (ptr - dst + 4 > *p_dstlen)
        {
            goto Exit;
        }
        tmp = (src[i] << 16) | (src[i+1] << 8) | (src[i+2]);
        *ptr++ = base64[(tmp >> 18) & 63];
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[tmp & 63];
    }

    // 1 byte extra
    if (i == srclen - 1)
    {
        if (ptr - dst + 4 > *p_dstlen)
        {
            goto Exit;
        }
        tmp = src[i] << 4;
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[tmp & 63];
        *ptr++ = '=';
        *ptr++ = '=';
    }

    // 2 byte extra
    if (i == srclen - 2)
    {
        if (ptr - dst + 4 > *p_dstlen)
        {
            goto Exit;
        }
        tmp = ((src[i] << 8) | (src[i+1])) << 2;
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[tmp & 63];
        *ptr++ = '=';
    }

    *p_dstlen = ptr - dst;
    return true;
Exit:
    *p_dstlen = 0;
    return false;
}

typedef enum {
    BASE64_CONV_VALID,
    BASE64_CONV_PAD,
    BASE64_CONV_INVALID
} BASE64_CONV;

static BASE64_CONV libspdm_decode_base64_chr(uint8_t b64_chr, uint8_t *value)
{
    if (b64_chr >= 'A' && b64_chr <= 'Z')
    {
        *value = b64_chr - 'A';
    }
    else if (b64_chr >= 'a' && b64_chr <= 'z')
    {
        *value = b64_chr - 'a' + 26;
    }
    else if (b64_chr >= '0' && b64_chr <= '9')
    {
        *value = b64_chr -'0' + 52;
    }
    else if (b64_chr == '+' || b64_chr == '-')
    {
        *value = 62;
    }
    else if (b64_chr == '/' || b64_chr == '_')
    {
        *value = 63;
    }
    else if (b64_chr == '=')
    {
        *value = 0;
        return BASE64_CONV_PAD;
    }
    else
    {
        return BASE64_CONV_INVALID;
    }

    return BASE64_CONV_VALID;
}

static bool libspdm_decode_base64_stripped(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    const uint8_t *p_read;
    uint8_t *p_write;
    uint8_t i;
    uint8_t bytes;
    uint32_t bin_value;
    uint8_t char_value;

    if (src == NULL || dst == NULL || srclen % 4 != 0)
    {
        return false;
    }
    for (p_read = src, p_write = dst; p_read < src + srclen; p_read += 4)
    {
        for (i = 0, bytes = 3, bin_value = 0; i < 4; i++)
        {
            if (libspdm_decode_base64_chr(p_read[i], &char_value) == BASE64_CONV_PAD)
            {
                bytes--;
                // fallthrough
                bin_value <<= 6;
                bin_value |= char_value;
            }
            else if (libspdm_decode_base64_chr(p_read[i], &char_value) == BASE64_CONV_VALID)
            {
                bin_value <<= 6;
                bin_value |= char_value;
            }
            else
            {
                // attempting to decode an invalid character
                goto Exit;
            }
        }

        if (p_write - dst + bytes > *p_dstlen)
        {
            // buffer too small
            goto Exit;
        }

        switch (bytes)
        {
        case 3:
            *p_write++ = (bin_value & 0x00ff0000) >> 16;
            *p_write++ = (bin_value & 0x0000ff00) >> 8;
            *p_write++ = (bin_value & 0x000000ff);
            break;
        case 2:
            *p_write++ = (bin_value & 0x00ff0000) >> 16;
            *p_write++ = (bin_value & 0x0000ff00) >> 8;
            break;
        case 1:
            *p_write++ = (bin_value & 0x00ff0000) >> 16;
            break;
        default:
            // invalid state in base64
            goto Exit;
        }
    }
    *p_dstlen = p_write - dst;
    return true;
Exit:
    *p_dstlen = 0;
    return false;
}

bool libspdm_decode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    size_t s_progress;
    size_t d_progress;
    size_t decode_size;
    size_t decoded_size;

    // for each round we decode 64 bytes and skip the linebreaks
    for (s_progress = d_progress = 0; s_progress < srclen; s_progress += 65)
    {
        if (s_progress + 65 < srclen)
        {
            decode_size = 64;
        }
        else
        {
            // -1 to avoid decoding the '\n' byte in the end
            decode_size = srclen - s_progress - 1;
        }
        // calculate the size after decoding
        decoded_size = (decode_size / 4) * 3;
        if (src[decode_size - 1] == '=')
        {
            decoded_size--;
        }
        if (src[decoded_size - 2] == '=')
        {
            decoded_size--;
        }
        // break early if the buffer is too small
        if (*p_dstlen - d_progress < decoded_size)
        {
            break;
        }
        if (!libspdm_decode_base64_stripped(src + s_progress, dst + d_progress, decode_size, &decoded_size))
        {
            return false;
        }
        d_progress += decoded_size;
    }
    if (s_progress < srclen)
    {
        *p_dstlen = 0;
        return false;
    }
    *p_dstlen = d_progress;
    return true;
}
#else // USE_LKCA
bool libspdm_encode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return false;
}

bool libspdm_decode_base64(const uint8_t *src, uint8_t *dst, size_t srclen, size_t *p_dstlen)
{
    return false;
}
#endif // USE_LKCA

static bool lkca_asn1_get_tag(uint8_t const *ptr, uint8_t const *end,
                              size_t *length, uint32_t tag)
{
    uint64_t max_len = end - ptr;

    // Chain must be less than 1 GB
    if ((max_len < 2) || (max_len > (1024 * 1024 * 1024))) {
        return false;
    }

    // We only deal with universal and application tags
    if (ptr[0] != tag) {
        return false;
    }

    if (ptr[1] < 0x80) {
        *length = ptr[1] + 2;
    } else if (ptr[1] == 0x81) {
        if (max_len < 3) {
            return false;
        }
        *length = ptr[2] + 3;
    } else if (ptr[1] == 0x82) {
        if (max_len < 4) {
            return false;
        }
        *length = (ptr[2] << 8) + ptr[3] + 4;
    } else {
        // In theory it could be bigger than 64KB
        return false;
    }

    if (*length > max_len) {
        return false;
    }

    return true;
}

bool libspdm_asn1_get_tag(uint8_t **ptr, const uint8_t *end, size_t *length,
                          uint32_t tag)
{
    return lkca_asn1_get_tag(*ptr, end, length, tag);
}

bool libspdm_x509_get_subject_name(const uint8_t *cert, size_t cert_size,
                                   uint8_t *cert_subject,
                                   size_t *subject_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_common_name(const uint8_t *cert, size_t cert_size,
                                  char *common_name,
                                  size_t *common_name_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool
libspdm_x509_get_organization_name(const uint8_t *cert, size_t cert_size,
                                   char *name_buffer,
                                   size_t *name_buffer_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
bool libspdm_rsa_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                          void **rsa_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}
#endif /* (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) */

bool libspdm_ec_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                         void **ec_context)
{
#ifdef USE_LKCA
    bool ret = false;
    uint32_t key_size = 0;
    struct key_preparsed_payload lkca_cert;
    struct public_key *pub;

    lkca_cert.data = cert;
    lkca_cert.datalen = cert_size;

    if (cert == NULL) {
        return false;
    }

    if(key_type_asymmetric.preparse(&lkca_cert)) {
        return false;
    }

    pub = lkca_cert.payload.data[asym_crypto];
    // -1 is since lkca prepends '4' to public keys...
    key_size = pub->keylen - 1;

    if (key_size == (2 * 256 / 8)) {
        *ec_context = libspdm_ec_new_by_nid(LIBSPDM_CRYPTO_NID_SECP256R1);
    } else if (key_size == (2 * 384 / 8)) {
        *ec_context = libspdm_ec_new_by_nid(LIBSPDM_CRYPTO_NID_SECP384R1);
    } else {
        goto err;
    }

    if (*ec_context == NULL) {
        goto err;
    }

    // Again skip '4' in key to be in line with spdm protocol. We will add it
    // back in ecda_verify
    if (!lkca_ec_set_pub_key(*ec_context, (char *) pub->key + 1, key_size)) {
        libspdm_ec_free(*ec_context);
        goto err;
    }

    ret = true;
err:
    key_type_asymmetric.free_preparse(&lkca_cert);
    return ret;
#else
    return false;
#endif
}

bool libspdm_ecd_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                          void **ecd_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_sm2_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                          void **sm2_context)
{
    LIBSPDM_ASSERT(false);
    return false;
}

static int lkca_x509_verify_cert(const uint8_t *cert, size_t cert_size,
                                 const uint8_t *ca_cert, size_t ca_cert_size)
{
#ifdef USE_LKCA
    int ret;
    struct key_preparsed_payload lkca_cert;
    struct key_preparsed_payload lkca_ca_cert;

    lkca_cert.data = cert;
    lkca_cert.datalen = cert_size;
    lkca_ca_cert.data = ca_cert;
    lkca_ca_cert.datalen = ca_cert_size;

    ret = key_type_asymmetric.preparse(&lkca_cert);
    if (ret) {
        return ret;
    }

    ret = key_type_asymmetric.preparse(&lkca_ca_cert);
    if (ret) {
        key_type_asymmetric.free_preparse(&lkca_cert);
        return ret;
    }

    ret = public_key_verify_signature(lkca_ca_cert.payload.data[asym_crypto],
                                      lkca_cert.payload.data[asym_auth]);

    key_type_asymmetric.free_preparse(&lkca_cert);
    key_type_asymmetric.free_preparse(&lkca_ca_cert);

    return ret;
#else
    return false;
#endif
}

bool libspdm_x509_verify_cert(const uint8_t *cert, size_t cert_size,
                              const uint8_t *ca_cert, size_t ca_cert_size)
{
    return lkca_x509_verify_cert(cert, cert_size, ca_cert, ca_cert_size) == 0;
}

bool libspdm_x509_verify_cert_chain(const uint8_t *root_cert, size_t root_cert_length,
                                    const uint8_t *cert_chain, size_t cert_chain_length)
{
    size_t preceding_cert_len;
    const uint8_t *preceding_cert;
    size_t current_cert_len;
    const uint8_t *current_cert;
    bool verify_flag;
    int ret;

    verify_flag = false;
    preceding_cert = root_cert;
    preceding_cert_len = root_cert_length;

    current_cert = cert_chain;


    /* Get Current certificate from certificates buffer and Verify with preceding cert*/
    do {
        if (!lkca_asn1_get_tag(
            current_cert, cert_chain + cert_chain_length, &current_cert_len,
            LIBSPDM_CRYPTO_ASN1_CONSTRUCTED | LIBSPDM_CRYPTO_ASN1_SEQUENCE)) {
            break;
        }

        ret = lkca_x509_verify_cert(current_cert, current_cert_len,
                                    preceding_cert, preceding_cert_len);
        if (ret != 0) {
            verify_flag = false;
            break;
        } else {
            verify_flag = true;
        }

        preceding_cert = current_cert;
        preceding_cert_len = current_cert_len;

        current_cert = current_cert + current_cert_len;
    } while (true);

    return verify_flag;
}

bool libspdm_x509_get_cert_from_cert_chain(const uint8_t *cert_chain,
                                           size_t cert_chain_length,
                                           const int32_t cert_index, const uint8_t **cert,
                                           size_t *cert_length)
{
    size_t asn1_len;
    int32_t current_index;
    size_t current_cert_len;
    const uint8_t *current_cert;

    current_cert_len = 0;

    /* Check input parameters.*/
    if ((cert_chain == NULL) || (cert == NULL) || (cert_index < -1) ||
        (cert_length == NULL)) {
        return false;
    }

    current_cert = cert_chain;
    current_index = -1;

    /* Traverse the certificate chain*/
    while (true) {
        /* Get asn1 tag len*/
        if (!lkca_asn1_get_tag(
            current_cert, cert_chain + cert_chain_length, &asn1_len,
            LIBSPDM_CRYPTO_ASN1_CONSTRUCTED | LIBSPDM_CRYPTO_ASN1_SEQUENCE)) {
            break;
        }

        current_cert_len = asn1_len;
        current_index++;

        if (current_index == cert_index) {
            *cert = current_cert;
            *cert_length = current_cert_len;
            return true;
        }

        current_cert = current_cert + current_cert_len;
    }

    /* If cert_index is -1, Return the last certificate*/
    if (cert_index == -1 && current_index >= 0) {
        *cert = current_cert - current_cert_len;
        *cert_length = current_cert_len;
        return true;
    }

    return false;
}

bool libspdm_x509_get_tbs_cert(const uint8_t *cert, size_t cert_size,
                               uint8_t **tbs_cert, size_t *tbs_cert_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_version(const uint8_t *cert, size_t cert_size,
                              size_t *version)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_serial_number(const uint8_t *cert, size_t cert_size,
                                    uint8_t *serial_number,
                                    size_t *serial_number_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_issuer_name(const uint8_t *cert, size_t cert_size,
                                  uint8_t *cert_issuer,
                                  size_t *issuer_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool
libspdm_x509_get_issuer_common_name(const uint8_t *cert, size_t cert_size,
                                    char *common_name,
                                    size_t *common_name_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool
libspdm_x509_get_issuer_orgnization_name(const uint8_t *cert, size_t cert_size,
                                         char *name_buffer,
                                         size_t *name_buffer_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_signature_algorithm(const uint8_t *cert,
                                          size_t cert_size, uint8_t *oid,
                                          size_t *oid_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extension_data(const uint8_t *cert, size_t cert_size,
                                     const uint8_t *oid, size_t oid_size,
                                     uint8_t *extension_data,
                                     size_t *extension_data_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_validity(const uint8_t *cert, size_t cert_size,
                               uint8_t *from, size_t *from_size, uint8_t *to,
                               size_t *to_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_key_usage(const uint8_t *cert, size_t cert_size,
                                size_t *usage)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extended_key_usage(const uint8_t *cert,
                                         size_t cert_size, uint8_t *usage,
                                         size_t *usage_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_get_extended_basic_constraints(const uint8_t *cert,
                                                 size_t cert_size,
                                                 uint8_t *basic_constraints,
                                                 size_t *basic_constraints_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

bool libspdm_x509_set_date_time(char const *date_time_str, void *date_time, size_t *date_time_size)
{
    LIBSPDM_ASSERT(false);
    return false;
}

int32_t libspdm_x509_compare_date_time(const void *date_time1, const void *date_time2)
{
    LIBSPDM_ASSERT(false);
    return -3;
}

bool libspdm_gen_x509_csr(size_t hash_nid, size_t asym_nid,
                          uint8_t *requester_info, size_t requester_info_length,
                          void *context, char *subject_name,
                          size_t *csr_len, uint8_t **csr_pointer)
{
    LIBSPDM_ASSERT(false);
    return false;
}
