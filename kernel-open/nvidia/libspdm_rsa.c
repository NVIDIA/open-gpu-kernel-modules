/*
* SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "internal_crypt_lib.h"
#include "library/cryptlib.h"

#ifdef USE_LKCA
#include <linux/module.h>
#include <linux/mpi.h>
#include <linux/random.h>

#include <crypto/akcipher.h>
#include <crypto/internal/rsa.h>

/* ------------------------ Macros & Defines ------------------------------- */
#define GET_MOST_SIGNIFICANT_BIT(keySize)      (keySize > 0 ? ((keySize - 1) & 7) : 0)
#define GET_ENC_MESSAGE_SIZE_BYTE(keySize)     (keySize + 7) >> 3;
#define PKCS1_MGF1_COUNTER_SIZE_BYTE           (4)
#define RSA_PSS_PADDING_ZEROS_SIZE_BYTE        (8)
#define RSA_PSS_TRAILER_FIELD                  (0xbc)
#define SHIFT_RIGHT_AND_GET_BYTE(val, x)       ((val >> x) & 0xFF)
#ifndef BITS_TO_BYTES
#define BITS_TO_BYTES(b)                       (b >> 3)
#endif

static const unsigned char zeroes[RSA_PSS_PADDING_ZEROS_SIZE_BYTE] = { 0 };

struct rsa_ctx
{
    struct rsa_key key;
    bool   pub_key_set;
    bool   priv_key_set;
    int    size;
};
#endif // #ifdef USE_LKCA

/*!
 * Creating and initializing a RSA context.
 *
 * @return  : A void pointer points to a RSA context
 *
*/
void *libspdm_rsa_new
(
    void
)
{
#ifndef USE_LKCA
    return NULL;
#else
    struct rsa_ctx *ctx;

    ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);

    if (ctx == NULL)
    {
        return NULL;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->pub_key_set = false;
    ctx->priv_key_set = false;

    return ctx;
#endif
}

/*!
 * To free a RSA context.
 *
 * @param rsa_context   : A RSA context pointer
 *
*/
void libspdm_rsa_free
(
    void *rsa_context
)
{
#ifdef USE_LKCA
    struct rsa_ctx *ctx = rsa_context;

    if (ctx != NULL)
    {
        if (ctx->key.n) kfree(ctx->key.n);
        if (ctx->key.e) kfree(ctx->key.e);
        if (ctx->key.d) kfree(ctx->key.d);
        if (ctx->key.q) kfree(ctx->key.q);
        if (ctx->key.p) kfree(ctx->key.p);
        if (ctx->key.dq) kfree(ctx->key.dq);
        if (ctx->key.dp) kfree(ctx->key.dp);
        if (ctx->key.qinv) kfree(ctx->key.qinv);
        kfree(ctx);
    }
#endif
}

#define rsa_set_key_case(a, a_sz, A) \
    case A: \
    { \
        if (ctx->key.a) { \
            kfree(ctx->key.a); \
        } \
        ctx->key.a = shadow_num; \
        ctx->key.a_sz = bn_size; \
        break; \
    }
/*!
 * To set key into RSA context.
 *
 * @param rsa_context   : A RSA context pointer
 * @param key_tag       : Indicate key tag for RSA key
 * @param big_number    : A big nuMber buffer to store rsa KEY
 * @param bn_size       : The size of bug number
 *
 * @Return              : True if OK; otherwise return False
*/
bool libspdm_rsa_set_key
(
    void                         *rsa_context,
    const libspdm_rsa_key_tag_t  key_tag,
    const uint8_t               *big_number,
    size_t                       bn_size
)
{
#ifndef USE_LKCA
    return false;
#else
    struct rsa_ctx *ctx = rsa_context;
    uint8_t *shadow_num;

    if (ctx == NULL)
    {
        return false;
    }

    // Quick sanity check if tag is valid
    switch (key_tag)
    {
        case LIBSPDM_RSA_KEY_N:
        case LIBSPDM_RSA_KEY_E:
        case LIBSPDM_RSA_KEY_D:
        case LIBSPDM_RSA_KEY_Q:
        case LIBSPDM_RSA_KEY_P:
        case LIBSPDM_RSA_KEY_DP:
        case LIBSPDM_RSA_KEY_DQ:
        case LIBSPDM_RSA_KEY_Q_INV:
            break;
        default:
            return false;
            break;
    }

    if (big_number != NULL)
    {
        shadow_num = kmalloc(bn_size, GFP_KERNEL);
        if (shadow_num == NULL)
        {
            return false;
        }
        memcpy(shadow_num, big_number, bn_size);
    }
    else
    {
        shadow_num = NULL;
        bn_size = 0;
    }

    switch (key_tag)
    {
        rsa_set_key_case(n, n_sz, LIBSPDM_RSA_KEY_N)
        rsa_set_key_case(e, e_sz, LIBSPDM_RSA_KEY_E)
        rsa_set_key_case(d, d_sz, LIBSPDM_RSA_KEY_D)
        rsa_set_key_case(q, q_sz, LIBSPDM_RSA_KEY_Q)
        rsa_set_key_case(p, p_sz, LIBSPDM_RSA_KEY_P)
        rsa_set_key_case(dq, dq_sz, LIBSPDM_RSA_KEY_DQ)
        rsa_set_key_case(dp, dp_sz, LIBSPDM_RSA_KEY_DP)
        rsa_set_key_case(qinv, qinv_sz, LIBSPDM_RSA_KEY_Q_INV)
        default:
            // We can't get here ever
            break;
    }

    return true;
#endif
}

/*!
 * Perform PKCS1 MGF1 operation.
 *
 * @param mask            : A mask pointer to store return data
 * @param maskedDB_length : Indicate mask data block length
 * @param seed            : A seed pointer to store random values
 * @param seed_length     : The seed length
 * @param hash_nid        : The hash NID
 *
 * @Return                : True if OK; otherwise return False
 */
static bool NV_PKCS1_MGF1
(
    uint8_t       *mask,
    size_t         maskedDB_length,
    const uint8_t *seed,
    size_t         seed_length,
    size_t         hash_nid
)
{
#ifndef USE_LKCA
    return false;
#else
    size_t   mdLength;
    size_t   counter;
    size_t   outLength;
    uint8_t  counterBuf[4];
    void    *sha384_ctx = NULL;
    uint8_t  hash_value[LIBSPDM_SHA384_DIGEST_SIZE];
    bool     status = false;

    if (mask == NULL || seed == NULL)
    {
        return false;
    }

    // Only support SHA384 for MGF1 now.
    if (hash_nid == LIBSPDM_CRYPTO_NID_SHA384)
    {
        mdLength = LIBSPDM_SHA384_DIGEST_SIZE;
    }
    else
    {
        return false;
    }

    sha384_ctx = libspdm_sha384_new();

    if (sha384_ctx == NULL)
    {
       pr_err("%s : libspdm_sha384_new() failed \n", __FUNCTION__);
       return false;
    }

    for (counter = 0, outLength = 0; outLength < maskedDB_length; counter++)
    {
        counterBuf[0] = (uint8_t)SHIFT_RIGHT_AND_GET_BYTE(counter, 24);
        counterBuf[1] = (uint8_t)SHIFT_RIGHT_AND_GET_BYTE(counter, 16);
        counterBuf[2] = (uint8_t)SHIFT_RIGHT_AND_GET_BYTE(counter, 8);
        counterBuf[3] = (uint8_t)SHIFT_RIGHT_AND_GET_BYTE(counter, 0);

        status = libspdm_sha384_init(sha384_ctx);

        if (!status)
        {
            pr_err("%s: libspdm_sha384_init() failed !! \n", __FUNCTION__);
            goto _error_exit;
        }

        status = libspdm_sha384_update(sha384_ctx, seed, seed_length);

        if (!status)
        {
            pr_err("%s: libspdm_sha384_update() failed(seed) !! \n", __FUNCTION__);
            goto _error_exit;
        }

        status = libspdm_sha384_update(sha384_ctx, counterBuf, 4);

        if (!status)
        {
            pr_err("%s: libspdm_sha384_update() failed(counterBuf) !! \n", __FUNCTION__);
            goto _error_exit;
        }

        if (outLength + mdLength <= maskedDB_length)
        {
            status = libspdm_sha384_final(sha384_ctx, mask + outLength);

            if (!status)
            {
                pr_err("%s: libspdm_sha384_final() failed (<= maskedDB_length) !! \n", __FUNCTION__);
                goto _error_exit;
            }
            outLength += mdLength;
        }
        else
        {
            status = libspdm_sha384_final(sha384_ctx, hash_value);

            if (!status)
            {
                pr_err("%s: libspdm_sha384_final() failed(> maskedDB_length) !! \n", __FUNCTION__);
                goto _error_exit;
            }

            memcpy(mask + outLength, hash_value, maskedDB_length - outLength);
            outLength = maskedDB_length;
        }
    }
    status = true;

_error_exit:
    libspdm_sha384_free(sha384_ctx);
    return status;
#endif
}

/*
    0xbc : Trailer Field
                          +-----------+
                          |     M     |
                          +-----------+
                                |
                                V
                              Hash
                                |
                                V
                  +--------+----------+----------+
             M' = |Padding1|  mHash   |   salt   |
                  +--------+----------+----------+
                  |--------------|---------------|
                                 |
       +--------+----------+     V
 DB =  |Padding2|   salt   |   Hash
       +--------+----------+     |
                 |               |
                 V               |
                xor <--- MGF <---|
                 |               |
                 |               |
                 V               V
       +-------------------+----------+----+
 EM =  |    maskedDB       |     H    |0xbc|
       +-------------------+----------+----+

salt : The random number, we hardcode its size as hash size here.
M'   : The concatenation of padding1 + message hash  + salt
MGF  : Mask generation function.
       A mask generation function takes an octet string of variable length
       and a desired output length as input, and outputs an octet string of
       the desired length
       MGF1 is a Mask Generation Function based on a hash function.

Padding1 : 8 zeros
Padding2 : 0x01

The detail spec is at https://datatracker.ietf.org/doc/html/rfc2437
*/

/*!
 * Set keys and call PKCS1_MGF1 to generate signature.
 *
 * @param rsa_context     : A RSA context pointer
 * @param hash_nid        : The hash NID
 * @param message_hash    : The pointer to message hash
 * @param signature       : The pointer is used to store generated signature
 * @param sig_size        : For input, a pointer store signature buffer size.
 *                          For output, a pointer store generate signature size.
 * @param salt_Length     : The salt length for RSA-PSS algorithm
 *
 * @Return                : True if OK; otherwise return False
 */
static bool nvRsaPaddingAddPkcs1PssMgf1
(
    void          *rsa_context,
    size_t         hash_nid,
    const uint8_t *message_hash,
    size_t         hash_size,
    uint8_t       *signature,
    size_t        *sig_size,
    int            salt_length
)
{
#ifndef USE_LKCA
    return false;
#else
    bool     status = false;
    struct   rsa_ctx *ctx = rsa_context;
    void    *sha384_ctx = NULL;
    uint32_t keySize;
    uint32_t msBits;
    size_t   emLength;
    uint8_t  saltBuf[64];
    size_t   maskedDB_length;
    size_t   i;
    uint8_t  *tmp_H;
    uint8_t  *tmp_P;
    int       rc;
    unsigned int ret_data_size;
    MPI  mpi_n = NULL;
    MPI  mpi_d = NULL;
    MPI  mpi_c = mpi_alloc(0);
    MPI  mpi_p = mpi_alloc(0);

    // read modulus to BN struct
    mpi_n = mpi_read_raw_data(ctx->key.n, ctx->key.n_sz);
    if (mpi_n == NULL)
    {
        pr_err("%s : mpi_n create failed !! \n", __FUNCTION__);
        goto _error_exit;
    }

    // read private exponent to BN struct
    mpi_d = mpi_read_raw_data(ctx->key.d, ctx->key.d_sz);
    if (mpi_d == NULL)
    {
        pr_err("%s : mpi_d create failed !! \n", __FUNCTION__);
        goto _error_exit;
    }

    keySize  = mpi_n->nbits;
    msBits   = GET_MOST_SIGNIFICANT_BIT(keySize);
    emLength = BITS_TO_BYTES(keySize);

    if (msBits == 0)
    {
        *signature++ = 0;
        emLength--;
    }

    if (emLength < hash_size + 2)
    {
        pr_err("%s : emLength < hash_size + 2 !! \n", __FUNCTION__);
        goto _error_exit;
    }

    // Now, we only support salt_length == LIBSPDM_SHA384_DIGEST_SIZE
    if (salt_length != LIBSPDM_SHA384_DIGEST_SIZE ||
        hash_nid    != LIBSPDM_CRYPTO_NID_SHA384)
    {
        pr_err("%s : Invalid salt_length (%x) \n", __FUNCTION__, salt_length);
        goto _error_exit;
    }

    get_random_bytes(saltBuf, salt_length);

    maskedDB_length = emLength - hash_size - 1;
    tmp_H = signature + maskedDB_length;
    sha384_ctx = libspdm_sha384_new();

    if (sha384_ctx == NULL)
    {
        pr_err("%s : libspdm_sha384_new() failed !! \n", __FUNCTION__);
        goto _error_exit;
    }

    status = libspdm_sha384_init(sha384_ctx);
    if (!status)
    {
        pr_err("%s : libspdm_sha384_init() failed !! \n", __FUNCTION__);
        goto _error_exit;
    }

    status = libspdm_sha384_update(sha384_ctx, zeroes, sizeof(zeroes));

    if (!status)
    {
        pr_err("%s : libspdm_sha384_update() with zeros failed !!\n", __FUNCTION__);
        goto _error_exit;
    }

    status = libspdm_sha384_update(sha384_ctx, message_hash, hash_size);

    if (!status)
    {
        pr_err("%s: libspdm_sha384_update() with message_hash failed !!\n", __FUNCTION__);
        goto _error_exit;
    }

    if (salt_length)
    {
        status = libspdm_sha384_update(sha384_ctx, saltBuf, salt_length);
        if (!status)
        {
            pr_err("%s : libspdm_sha384_update() with saltBuf failed !!\n", __FUNCTION__);
            goto _error_exit;
        }
    }

    status = libspdm_sha384_final(sha384_ctx, tmp_H);
    if (!status)
    {
        pr_err("%s : libspdm_sha384_final() with tmp_H failed !!\n", __FUNCTION__);
        goto _error_exit;
    }

    /* Generate dbMask in place then perform XOR on it */
    status = NV_PKCS1_MGF1(signature, maskedDB_length, tmp_H, hash_size, hash_nid);

    if (!status)
    {
        pr_err("%s : NV_PKCS1_MGF1() failed \n", __FUNCTION__);
        goto _error_exit;
    }

    tmp_P = signature;
    tmp_P += emLength - salt_length - hash_size - 2;
    *tmp_P++ ^= 0x1;

    if (salt_length > 0)
    {
        for (i = 0; i < salt_length; i++)
        {
            *tmp_P++ ^= saltBuf[i];
        }
    }

    if (msBits)
    {
        signature[0] &= 0xFF >> (8 - msBits);
    }

    /* H is already in place so just set final 0xbc */
    signature[emLength - 1] = RSA_PSS_TRAILER_FIELD;

    // read signature to BN struct
    mpi_p = mpi_read_raw_data(signature, emLength);
    if (mpi_p == NULL)
    {
        pr_err("%s : mpi_p() create failed !!\n", __FUNCTION__);
        goto _error_exit;
    }

    // Staring RSA encryption with private key over signature.
    rc = mpi_powm(mpi_c, mpi_p, mpi_d, mpi_n);
    if (rc != 0)
    {
        pr_err("%s : mpi_powm()  failed \n", __FUNCTION__);
        goto _error_exit;
    }

    rc = mpi_read_buffer(mpi_c, signature, *sig_size, &ret_data_size, NULL);
    if (rc != 0)
    {
        pr_err("%s : mpi_read_buffer()  failed \n", __FUNCTION__);
        goto _error_exit;
    }

    if (ret_data_size > *sig_size)
    {
        goto _error_exit;
    }

    *sig_size = ret_data_size;
    status = true;

_error_exit:

   mpi_free(mpi_n);
   mpi_free(mpi_d);
   mpi_free(mpi_c);
   mpi_free(mpi_p);

   libspdm_sha384_free(sha384_ctx);

   return status;

#endif
}

/*!
 * Perform RSA-PSS sigaature sign process with LKCA library.
 *
 * @param rsa_context     : A RSA context pointer
 * @param hash_nid        : The hash NID
 * @param message_hash    : The pointer to  message hash
 * @param signature       : The pointer is used to store generated signature
 * @param sig_size        : For input, a pointer store signature buffer size.
 *                          For output, a pointer store generate signature size.
 *
 * @Return                : True if OK; otherwise return False
 */
bool lkca_rsa_pss_sign
(
    void          *rsa_context,
    size_t         hash_nid,
    const uint8_t *message_hash,
    size_t         hash_size,
    uint8_t       *signature,
    size_t        *sig_size
)
{
#ifndef USE_LKCA
    return true;
#else
    return  nvRsaPaddingAddPkcs1PssMgf1(rsa_context,
                                        hash_nid,
                                        message_hash,
                                        hash_size,
                                        signature,
                                        sig_size,
                                        LIBSPDM_SHA384_DIGEST_SIZE);
#endif
}

