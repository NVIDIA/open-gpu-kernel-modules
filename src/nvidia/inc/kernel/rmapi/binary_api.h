/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_binary_api_nvoc.h"

#ifndef BINARY_API_H
#define BINARY_API_H

#include "core/core.h"
#include "rmapi/resource.h"
#include "gpu/gpu_resource.h"
#include "resserv/rs_resource.h"
#include "rmapi/control.h"

NVOC_PREFIX(binapi) class BinaryApi : GpuResource
{
public:
    NV_STATUS binapiConstruct(BinaryApi *pResource,
                              CALL_CONTEXT   *pCallContext,
                              RS_RES_ALLOC_PARAMS_INTERNAL *pParams) :
                              GpuResource(pCallContext, pParams);

    virtual NV_STATUS binapiControl(BinaryApi *pResource, CALL_CONTEXT *pCallContext,
                                    RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

};

NVOC_PREFIX(binapipriv) class BinaryApiPrivileged : BinaryApi
{
public:
    NV_STATUS binapiprivConstruct(BinaryApiPrivileged *pResource, CALL_CONTEXT *pCallContext,
                                  RS_RES_ALLOC_PARAMS_INTERNAL *pParams) :
                                  BinaryApi(pCallContext, pParams);

    virtual NV_STATUS binapiprivControl(BinaryApiPrivileged *pResource, CALL_CONTEXT *pCallContext,
                                        RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

};

#endif

