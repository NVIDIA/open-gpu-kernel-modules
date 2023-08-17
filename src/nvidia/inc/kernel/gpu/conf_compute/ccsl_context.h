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

#ifndef CCSL_CONTEXT_H
#define CCSL_CONTEXT_H

#include "nvtypes.h"
#include "cc_drv.h"

struct ccslContext_t
{
    NvHandle hClient;
    NvHandle hChannel;

    enum {CSL_MSG_CTR_32, CSL_MSG_CTR_64} msgCounterSize;

    NvU8 keyIn[CC_AES_256_GCM_KEY_SIZE_BYTES];
    union
    {
        struct
        {
            NvU8 ivIn[CC_AES_256_GCM_IV_SIZE_BYTES];
            NvU8 ivMaskIn[CC_AES_256_GCM_IV_SIZE_BYTES];
        };
        NvU8 nonce[CC_HMAC_NONCE_SIZE_BYTES];
    };

    NvU8 keyOut[CC_AES_256_GCM_KEY_SIZE_BYTES];
    NvU8 ivOut[CC_AES_256_GCM_IV_SIZE_BYTES];
    NvU8 ivMaskOut[CC_AES_256_GCM_IV_SIZE_BYTES];

    NvU64 keyHandleIn;
    NvU64 keyHandleOut;

    void *openrmCtx;
};

typedef struct ccslContext_t *pCcslContext;
typedef struct ccslContext_t CCSL_CONTEXT;

#endif // CCSL_CONTEXT_H
