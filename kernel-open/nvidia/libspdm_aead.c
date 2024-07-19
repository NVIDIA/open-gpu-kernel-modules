/*
* SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvspdm_cryptlib_extensions.h"

#ifdef USE_LKCA
#define BUFFER_SIZE (2 * 1024 * 1024)
#define AUTH_TAG_SIZE 16
struct lkca_aead_ctx
{
    struct crypto_aead *aead;
    struct aead_request *req;
    char *a_data_buffer;
    char *in_buffer;
    char *out_buffer;
    char tag[AUTH_TAG_SIZE];
};
#endif

int libspdm_aead_prealloc(void **context, char const *alg)
{
#ifndef USE_LKCA
    return -ENODEV;
#else
    struct lkca_aead_ctx *ctx;

    ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
    if (ctx == NULL) {
        return -ENOMEM;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->aead = crypto_alloc_aead(alg, CRYPTO_ALG_TYPE_AEAD, 0);
    if (IS_ERR(ctx->aead)) {
        pr_notice("could not allocate AEAD algorithm\n");
        kfree(ctx);
        return -ENODEV;
    }

    ctx->req = aead_request_alloc(ctx->aead, GFP_KERNEL);
    if (ctx->req == NULL) {
        pr_info("could not allocate skcipher request\n");
        crypto_free_aead(ctx->aead);
        kfree(ctx);
        return -ENOMEM;
    }

    ctx->a_data_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (ctx->a_data_buffer == NULL) {
        aead_request_free(ctx->req);
        crypto_free_aead(ctx->aead);
        kfree(ctx);
        return -ENOMEM;
    }

    ctx->in_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (ctx->in_buffer == NULL) {
        kfree(ctx->a_data_buffer);
        aead_request_free(ctx->req);
        crypto_free_aead(ctx->aead);
        kfree(ctx);
        return -ENOMEM;
    }

    ctx->out_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (ctx->out_buffer == NULL) {
        kfree(ctx->a_data_buffer);
        kfree(ctx->in_buffer);
        aead_request_free(ctx->req);
        crypto_free_aead(ctx->aead);
        kfree(ctx);
        return -ENOMEM;
    }

    *context = ctx;
    return 0;
#endif
}

void libspdm_aead_free(void *context)
{
#ifdef USE_LKCA
    struct lkca_aead_ctx *ctx = context;
    crypto_free_aead(ctx->aead);
    aead_request_free(ctx->req);
    kfree(ctx->a_data_buffer);
    kfree(ctx->in_buffer);
    kfree(ctx->out_buffer);
    kfree(ctx);
#endif
}

#define SG_AEAD_AAD 0
#define SG_AEAD_TEXT 1
#define SG_AEAD_SIG 2
// Number of fields in AEAD scatterlist
#define SG_AEAD_LEN 3

#ifdef USE_LKCA
// This function doesn't do any allocs, it uses temp buffers instead
static int lkca_aead_internal(struct crypto_aead *aead,
                              struct aead_request *req,
                              const uint8_t *key, size_t key_size,
                              const uint8_t *iv, size_t iv_size,
                              struct scatterlist sg_in[],
                              struct scatterlist sg_out[],
                              size_t a_data_size,
                              size_t data_in_size,
                              size_t *data_out_size,
                              size_t tag_size,
                              bool enc)
{
    DECLARE_CRYPTO_WAIT(wait);
    int rc = 0;

    if (crypto_aead_setkey(aead, key, key_size)) {
        pr_info("key could not be set\n");
        return -EINVAL;
    }

    if (crypto_aead_ivsize(aead) != iv_size) {
        pr_info("iv could not be set\n");
        return -EINVAL;
    }

    aead_request_set_ad(req, a_data_size);

    aead_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
            CRYPTO_TFM_REQ_MAY_SLEEP, crypto_req_done, &wait);

    if (enc) {
        aead_request_set_crypt(req, sg_in, sg_out, data_in_size, (u8 *) iv);
        rc = crypto_wait_req(crypto_aead_encrypt(req), &wait);
    } else {
        aead_request_set_crypt(req, sg_in, sg_out, data_in_size + tag_size, (u8 *) iv);
        rc = crypto_wait_req(crypto_aead_decrypt(req), &wait);
    }

    if (rc != 0) {
        if (enc) {
            pr_info("aead.c: Encryption failed with error %i\n", rc);
        } else {
            pr_info("aead.c: Decryption failed with error %i\n", rc);
            if (rc == -EBADMSG) {
                pr_info("aead.c: Authentication tag mismatch!\n");
            }
        }
    }

    *data_out_size = data_in_size;

    return rc;
}
#endif

int libspdm_aead_prealloced(void *context,
                            const uint8_t *key, size_t key_size,
                            const uint8_t *iv, size_t iv_size,
                            const uint8_t *a_data, size_t a_data_size,
                            const uint8_t *data_in, size_t data_in_size,
                            uint8_t *tag, size_t tag_size,
                            uint8_t *data_out, size_t *data_out_size,
                            bool enc)
{
#ifndef USE_LKCA
    return -ENODEV;
#else
    int rc = 0;
    struct scatterlist sg_in[SG_AEAD_LEN];
    struct scatterlist sg_out[SG_AEAD_LEN];
    struct lkca_aead_ctx *ctx = context;


    sg_init_table(sg_in, SG_AEAD_LEN);
    sg_init_table(sg_out, SG_AEAD_LEN);

    if (!virt_addr_valid(a_data)) {
        if (a_data_size > BUFFER_SIZE) {
            return -ENOMEM;
        }
        sg_set_buf(&sg_in[SG_AEAD_AAD], ctx->a_data_buffer, a_data_size);
        sg_set_buf(&sg_out[SG_AEAD_AAD], ctx->a_data_buffer, a_data_size);

        memcpy(ctx->a_data_buffer, a_data, a_data_size);
    } else {
        sg_set_buf(&sg_in[SG_AEAD_AAD], a_data, a_data_size);
        sg_set_buf(&sg_out[SG_AEAD_AAD], a_data, a_data_size);
    }

    if (!virt_addr_valid(data_in)) {
        if (data_in_size > BUFFER_SIZE) {
            return -ENOMEM;
        }
        sg_set_buf(&sg_in[SG_AEAD_TEXT], ctx->in_buffer, data_in_size);
        memcpy(ctx->in_buffer, data_in, data_in_size);
    } else {
        sg_set_buf(&sg_in[SG_AEAD_TEXT], data_in, data_in_size);
    }

    if (!virt_addr_valid(data_out)) {
        if (data_in_size > BUFFER_SIZE) {
            return -ENOMEM;
        }
        sg_set_buf(&sg_out[SG_AEAD_TEXT], ctx->out_buffer, data_in_size);
    } else {
        sg_set_buf(&sg_out[SG_AEAD_TEXT], data_out, data_in_size);
    }

    // Tag is small enough that memcpy is cheaper than checking if page is virtual
    if(tag_size > AUTH_TAG_SIZE) {
        return -ENOMEM;
    }
    sg_set_buf(&sg_in[SG_AEAD_SIG], ctx->tag, tag_size);
    sg_set_buf(&sg_out[SG_AEAD_SIG], ctx->tag, tag_size);

    if(!enc)
        memcpy(ctx->tag, tag, tag_size);

    rc = lkca_aead_internal(ctx->aead, ctx->req, key, key_size, iv, iv_size,
                            sg_in, sg_out, a_data_size, data_in_size,
                            data_out_size, tag_size, enc);

    if (enc) {
        memcpy(tag, ctx->tag, tag_size);
    }

    if (!virt_addr_valid(data_out)) {
        memcpy(data_out, ctx->out_buffer, data_in_size);
    }

    return rc;
#endif
}

int libspdm_aead(const uint8_t *key, size_t key_size,
                 const uint8_t *iv, size_t iv_size,
                 const uint8_t *a_data, size_t a_data_size,
                 const uint8_t *data_in, size_t data_in_size,
                 const uint8_t *tag, size_t tag_size,
                 uint8_t *data_out, size_t *data_out_size,
                 bool enc, char const *alg)
{
#ifndef USE_LKCA
    return -ENODEV;
#else
    struct crypto_aead *aead = NULL;
    struct aead_request *req = NULL;
    struct scatterlist sg_in[SG_AEAD_LEN];
    struct scatterlist sg_out[SG_AEAD_LEN];
    uint8_t *a_data_shadow = NULL;
    uint8_t *data_in_shadow = NULL;
    uint8_t *data_out_shadow = NULL;
    uint8_t *tag_shadow = NULL;
    int rc = 0;

    aead = crypto_alloc_aead(alg, CRYPTO_ALG_TYPE_AEAD, 0);
    if (IS_ERR(aead)) {
        pr_notice("could not allocate AEAD algorithm\n");
        return -ENODEV;
    }

    req = aead_request_alloc(aead, GFP_KERNEL);
    if (req == NULL) {
        pr_info("could not allocate skcipher request\n");
        rc = -ENOMEM;
        goto out;
    }

    sg_init_table(sg_in, SG_AEAD_LEN);
    sg_init_table(sg_out, SG_AEAD_LEN);

    if (!virt_addr_valid(a_data)) {
        a_data_shadow = kmalloc(a_data_size, GFP_KERNEL);
        if (a_data_shadow == NULL) {
            rc = -ENOMEM;
            goto out;
        }

        sg_set_buf(&sg_in[SG_AEAD_AAD], a_data_shadow, a_data_size);
        sg_set_buf(&sg_out[SG_AEAD_AAD], a_data_shadow, a_data_size);

        memcpy(a_data_shadow, a_data, a_data_size);
    } else {
        sg_set_buf(&sg_in[SG_AEAD_AAD], a_data, a_data_size);
        sg_set_buf(&sg_out[SG_AEAD_AAD], a_data, a_data_size);
    }

    if (!virt_addr_valid(data_in)) {
        data_in_shadow = kmalloc(data_in_size, GFP_KERNEL);
        if (data_in_shadow == NULL) {
            rc = -ENOMEM;
            goto out;
        }

        sg_set_buf(&sg_in[SG_AEAD_TEXT], data_in_shadow, data_in_size);

        memcpy(data_in_shadow, data_in, data_in_size);
    } else {
        sg_set_buf(&sg_in[SG_AEAD_TEXT], data_in, data_in_size);
    }

    if (!virt_addr_valid(data_out)) {
        data_out_shadow = kmalloc(data_in_size, GFP_KERNEL);
        if (data_out_shadow == NULL) {
            rc = -ENOMEM;
            goto out;
        }

        sg_set_buf(&sg_out[SG_AEAD_TEXT], data_out_shadow, data_in_size);
    } else {
        sg_set_buf(&sg_out[SG_AEAD_TEXT], data_out, data_in_size);
    }

    if (!virt_addr_valid(tag)) {
        tag_shadow = kmalloc(tag_size, GFP_KERNEL);
        if (tag_shadow == NULL) {
            rc = -ENOMEM;
            goto out;
        }

        sg_set_buf(&sg_in[SG_AEAD_SIG], tag_shadow, tag_size);
        sg_set_buf(&sg_out[SG_AEAD_SIG], tag_shadow, tag_size);

        if(!enc)
            memcpy(tag_shadow, tag, tag_size);
    } else {
        sg_set_buf(&sg_in[SG_AEAD_SIG], tag, tag_size);
        sg_set_buf(&sg_out[SG_AEAD_SIG], tag, tag_size);
    }

    rc = lkca_aead_internal(aead, req, key, key_size, iv, iv_size,
                            sg_in, sg_out, a_data_size, data_in_size,
                            data_out_size, tag_size, enc);

    if (enc && (tag_shadow != NULL))
        memcpy((uint8_t *) tag, tag_shadow, tag_size);

    if (data_out_shadow != NULL)
        memcpy(data_out, data_out_shadow, data_in_size);

out:
    if (a_data_shadow != NULL)
        kfree(a_data_shadow);
    if (data_in_shadow != NULL)
        kfree(data_in_shadow);
    if (data_out != NULL)
       kfree(data_out_shadow);
    if (tag != NULL)
        kfree(tag_shadow);
    if (aead != NULL)
        crypto_free_aead(aead);
    if (req != NULL)
        aead_request_free(req);
    return rc;
#endif
}

// Wrapper to make look like libspdm
bool libspdm_aead_gcm_prealloc(void **context)
{
    return libspdm_aead_prealloc(context, "gcm(aes)") == 0;
}

bool libspdm_aead_aes_gcm_encrypt_prealloc(void *context,
                                  const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  uint8_t *tag_out, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    int32_t ret;

    if (data_in_size > INT_MAX) {
        return false;
    }
    if (a_data_size > INT_MAX) {
        return false;
    }
    if (iv_size != 12) {
        return false;
    }
    switch (key_size) {
    case 16:
    case 24:
    case 32:
        break;
    default:
        return false;
    }
    if ((tag_size < 12) || (tag_size > 16)) {
        return false;
    }
    if (data_out_size != NULL) {
        if ((*data_out_size > INT_MAX) ||
            (*data_out_size < data_in_size)) {
            return false;
        }
    }

    ret = libspdm_aead_prealloced(context, key, key_size, iv, iv_size,
                       a_data, a_data_size, data_in, data_in_size,
                       tag_out, tag_size, data_out, data_out_size, true);

    *data_out_size = data_in_size;

    return ret == 0;
}

bool libspdm_aead_aes_gcm_decrypt_prealloc(void *context,
                                  const uint8_t *key, size_t key_size,
                                  const uint8_t *iv, size_t iv_size,
                                  const uint8_t *a_data, size_t a_data_size,
                                  const uint8_t *data_in, size_t data_in_size,
                                  const uint8_t *tag, size_t tag_size,
                                  uint8_t *data_out, size_t *data_out_size)
{
    int ret;
    if (data_in_size > INT_MAX) {
        return false;
    }
    if (a_data_size > INT_MAX) {
        return false;
    }
    if (iv_size != 12) {
        return false;
    }
    switch (key_size) {
    case 16:
    case 24:
    case 32:
        break;
    default:
        return false;
    }
    if ((tag_size < 12) || (tag_size > 16)) {
        return false;
    }
    if (data_out_size != NULL) {
        if ((*data_out_size > INT_MAX) ||
            (*data_out_size < data_in_size)) {
            return false;
        }
    }

    ret = libspdm_aead_prealloced(context, key, key_size, iv, iv_size,
                       a_data, a_data_size, data_in, data_in_size,
                       (uint8_t *) tag, tag_size, data_out, data_out_size, false);

    *data_out_size = data_in_size;

    return ret == 0;

}

