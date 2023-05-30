/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef CCSL_H
#define CCSL_H

#include "nvstatus.h"
#include "nvmisc.h"
#include "kernel/gpu/conf_compute/conf_compute.h"

typedef struct ccslContext_t *pCcslContext;

/*
 * Initializes a context by providing client and channel information.
 *
 * ccslContext [in / out]
 * hClient     [in]
 * hChannel    [in]
 */
NV_STATUS
ccslContextInitViaChannel
(
    pCcslContext *ppCtx,
    NvHandle      hClient,
    NvHandle      hChannel
);

/*
 * Initializes a context by providing key ID information.
 *
 * ConfidentialCompute [in]
 * ccslContext         [in / out]
 * globalKeyId         [in]
 */
NV_STATUS
ccslContextInitViaKeyId
(
    ConfidentialCompute *pConfCompute,
    pCcslContext        *ppCtx,
    NvU32                globalKeyId
);

/*
 * Clears the context and erases sensitive material such as keys.
 *
 * ccslContext [in / out]
 */
void
ccslContextClear
(
    pCcslContext ctx
);

/* To be called before library client triggers a Device-side encryption.
 * Attempts to increment the library's Device-side message counter and returns an error if it will overflow.
 *
 * ccslContext [in]
 * decryptIv   [in]
 *
 * Returns NV_ERR_INSUFFICIENT_RESOURCES if the next Device-side encryption will overflow.
 * Returns NV_OK otherwise.
 */
NV_STATUS
ccslLogDeviceEncryption
(
    pCcslContext  ctx,
    NvU8         *decryptIv
);

/* Request the next IV to be used in encryption. Storing it explicitly enables the caller
 * to perform encryption out of order using EncryptWithIv
 *
 * ccslContext [in / out]
 * encryptIv   [out]
 *
 * Returns NV_ERR_INSUFFICIENT_RESOURCES if the next encryption will overflow.
 * Returns NV_OK otherwise.
 */
NV_STATUS
ccslAcquireEncryptionIv
(
    pCcslContext  ctx,
    NvU8         *encryptIv
);

/* Rotate the IV for the given direction.
 *
 * ccslContext [in / out]
 * direction   [in]
 */
NV_STATUS
ccslRotateIv
(
    pCcslContext ctx,
    NvU8         direction
);

/*
 * Encrypt and sign data using provided IV
 *
 * ccslContext   [in]
 * bufferSize    [in]     - Size of buffer to be encrypted in units of bytes.
 * inputBuffer   [in]     - Address of plaintext input buffer. For performance it should be 16-byte aligned.
 * encryptionIv  [in/out] - IV to use for encryption. The IV will be "dirtied" after this operation.
 * outputBuffer  [in/out] - Address of ciphertext output buffer.
 * authTagBuffer [in/out] - Address of authentication tag. In APM it is 32 bytes. In HCC it is 16 bytes.
 *
 * Returns NV_OK.
 */
NV_STATUS
ccslEncryptWithIv
(
    pCcslContext  ctx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8         *encryptIv,
    NvU8         *outputBuffer,
    NvU8         *authTagBuffer
);

/*
 * If message counter will not overflow then encrypt and sign data.
 *
 * ccslContext   [in]
 * bufferSize    [in]     - Size of buffer to be encrypted in units of bytes.
 * inputBuffer   [in]     - Address of plaintext input buffer. For performance it should be 16-byte aligned.
 * outputBuffer  [in/out] - Address of ciphertext output buffer.
 * authTagBuffer [in/out] - Address of authentication tag. In APM it is 32 bytes. In HCC it is 16 bytes.
 *
 * Returns NV_ERR_INSUFFICIENT_RESOURCES if message counter will overflow.
 * Returns NV_OK otherwise.
 */
NV_STATUS
ccslEncrypt
(
    pCcslContext  ctx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8         *outputBuffer,
    NvU8         *authTagBuffer
);

/*
 * First verify authentication tag. If authentication passes then the data is decrypted.
 *
 * ccslContext   [in]
 * bufferSize    [in]     - Size of buffer to be decrypted in units of bytes.
 * inputBuffer   [in]     - Address of ciphertext input buffer. For performance it should be 16-byte aligned.
 * outputBuffer  [in/out] - Address of plaintext output buffer.
 * authTagBuffer [in]     - Address of authentication tag. In APM it is 32 bytes. In HCC it is 16 bytes.
 *
 * Returns NV_ERR_INVALID_DATA if verification of the authentication tag fails.
 * Returns NV_OK otherwise.
 */
NV_STATUS
ccslDecrypt
(
    pCcslContext  ctx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8 const   *decryptIv,
    NvU8         *outputBuffer,
    NvU8 const   *authTagBuffer
);

/*
 * Sign the plaintext message.
 *
 * ccslContext   [in]
 * bufferSize    [in]     - Size of buffer to be signed in units of bytes.
 * inputBuffer   [in]     - Address of input buffer. For performance it should be 16-byte aligned.
 * authTagBuffer [in/out] - Address of authentication tag. In HCC it is 32 bytes.
 *
 * Returns NV_OK
 */
NV_STATUS
ccslSign
(
    pCcslContext  ctx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8         *authTagBuffer
);

#define CCSL_DIR_HOST_TO_DEVICE 0
#define CCSL_DIR_DEVICE_TO_HOST 1

/*
 * Returns the number of messages that can be encrypted by the CPU (CCSL_DIR_HOST_TO_DEVICE)
 * or encrypted by the GPU (CCSL_DIR_DEVICE_TO_HOST) before the message counter will overflow.
 *
 * ccslContext [in]
 * direction   [in]  - Either CCSL_DIR_HOST_TO_DEVICE or CCSL_DIR_DEVICE_TO_HOST.
 * messageNum  [out] - Number of messages that can be encrypted before overflow.
 */
NV_STATUS
ccslQueryMessagePool
(
    pCcslContext  ctx,
    NvU8          direction,
    NvU64        *messageNum
);

#endif // CCSL_H
