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


bool libspdm_aead_gcm_prealloc(void **context);
void libspdm_aead_free(void *context);
bool libspdm_aead_aes_gcm_encrypt_prealloc(void *context,
        const uint8_t *key, size_t key_size,
        const uint8_t *iv, size_t iv_size,
        const uint8_t *a_data, size_t a_data_size,
        const uint8_t *data_in, size_t data_in_size,
        uint8_t *tag_out, size_t tag_size,
        uint8_t *data_out, size_t *data_out_size);
bool libspdm_aead_aes_gcm_decrypt_prealloc(void *context,
        const uint8_t *key, size_t key_size,
        const uint8_t *iv, size_t iv_size,
        const uint8_t *a_data, size_t a_data_size,
        const uint8_t *data_in, size_t data_in_size,
        const uint8_t *tag, size_t tag_size,
        uint8_t *data_out, size_t *data_out_size);
bool libspdm_check_crypto_backend(void);
