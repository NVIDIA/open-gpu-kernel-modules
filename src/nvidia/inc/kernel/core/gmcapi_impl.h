/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _GMCAPI_IMPL_H_
#define _GMCAPI_IMPL_H_

//
// This file contains the RM-internal definitions of GMCAPI
// That is, definitions needed to write the implementations of GMCAPI,
//

#include "gmcapi/gmcapi_base.h"
#include "nvstatus.h"

typedef struct OBJGPU OBJGPU;

typedef struct GMCAPI_CONTEXT
{
    //
    // Fields marked as 'const' are not meant to be changed by the handler
    // Fields not marked as 'const' are considered in/out
    //

    //
    // The GPU this GMCAPI is tied to
    //
    OBJGPU * const pGpu;
    //
    // Command ID of the API being called.
    // Useful if the same handler function is used for multiple APIs
    //
    const NvU32 command;
    //
    // API flags
    // NOTE: This field is supposed to be const, but the different levels of
    //       GMCAPI infrastructure need to set it, so it has to stay mutable.
    //       The handlers should consider it const and never write to it.
    //
    /*const*/ NvU32 flags;
    //
    // Size of the input buffer.
    // For APIs that take a fixed size input, this has already been checked
    // against the sizeof() of the input type.
    //
    const NvLength inParamSize;
    //
    // Pointer to the input buffer. May or may not be NULL if inParamSize==0
    //
    const void * const pInParams;
    //
    // Size of the output buffer. This is the maximum size you can write to it
    //
    const NvLength maxOutParamSize;
    //
    // Actual size of the output data. Must be <= maxOutParamSize.
    // For APIs that return a fixed size output, this is initialized to the
    // sizeof the output type, and the handler doesn't touch it.
    // For APIs that deal with dynamic data, this field is initalized to the
    // GMCAPI_DYNAMIC_PAYLOAD special value and has to be set by the handler
    // function to the correct value.
    //
    NvLength outParamSize;
    //
    // Pointer to the output buffer. May or may not be NULL if maxOutParamSize==0
    //
    void * const pOutParams;
} GMCAPI_CONTEXT;

typedef NV_STATUS GmcApiHandler(GMCAPI_CONTEXT *pCtx);

//
// Special type values for use in the gmcapi_table
// These use magic size values because there is no good cross platform way to
// define structs where (NvU32)sizeof(x) returns what we want.
//
typedef struct GMCAPI_NONE    { NvU8 x[0x10000000]; } GMCAPI_NONE;
typedef struct GMCAPI_DYNAMIC { NvU8 x[0x10000008]; } GMCAPI_DYNAMIC;

NvBool gmcapiIsValidNumericId(NvU32 id);
NV_STATUS gmcapiDispatch(GMCAPI_CONTEXT *pCtx);

// TODO: Avoid handler lookup costs if cmd is known at compile time
static NV_INLINE NV_STATUS gmcapiCommand
(
    OBJGPU *pGpu,
    NvU32 cmd,
    const void *pInParams,
    NvLength inParamSize,
    void *pOutParams,
    NvLength *pOutParamSize
)
{
    if (inParamSize >= sizeof(GMCAPI_NONE))
        return NV_ERR_GMCAPI_REQUEST_SIZE_TOO_LARGE;

    GMCAPI_CONTEXT ctx = {
        .pGpu = pGpu,
        .command = cmd,
        .inParamSize = inParamSize,
        .pInParams = pInParams,
        .maxOutParamSize = pOutParamSize ? *pOutParamSize : 0,
        .pOutParams = pOutParams,
        .flags = 0  // Filled in by gmcapiDispatch
    };
    NV_STATUS status = gmcapiDispatch(&ctx);
    if (pOutParamSize)
    {
        *pOutParamSize = (status == NV_OK) ? ctx.outParamSize : 0;
    }
    return status;
}
#endif // _GMCAPI_IMPL_H_
