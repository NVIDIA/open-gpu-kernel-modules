/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _DEPRECATED_CONTEXT_
#define _DEPRECATED_CONTEXT_

#include "deprecated/rmapi_deprecated.h"

typedef struct
{
    DEPRECATED_CONTEXT  parent;
    API_SECURITY_INFO   secInfo;
    NvBool              bInternal;
    NvBool              bUserModeArgs;
    RM_API             *pRmApi;
} DEPRECATED_CONTEXT_EXT;

void rmapiInitDeprecatedContext (DEPRECATED_CONTEXT_EXT *pContext,
                                 API_SECURITY_INFO *pSecInfo,
                                 NvBool bUserModeArgs,
                                 NvBool bInternal);

#endif // _DEPRECATED_CONTEXT_
