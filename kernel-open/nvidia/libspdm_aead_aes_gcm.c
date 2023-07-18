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
*
* Prototypes and checks taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
* License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
*/

#include "internal_crypt_lib.h"

bool libspdm_aead_aes_gcm_encrypt(const uint8_t *key, size_t key_size,
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

    ret = libspdm_aead(key, key_size, iv, iv_size, a_data, a_data_size,
                       data_in, data_in_size, tag_out, tag_size,
                       data_out, data_out_size, true, "gcm(aes)");

    *data_out_size = data_in_size;

    return ret == 0;
}

bool libspdm_aead_aes_gcm_decrypt(const uint8_t *key, size_t key_size,
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

    ret = libspdm_aead(key, key_size, iv, iv_size, a_data, a_data_size,
                       data_in, data_in_size, tag, tag_size,
                       data_out, data_out_size, false, "gcm(aes)");

    *data_out_size = data_in_size;

    return ret == 0;

}
