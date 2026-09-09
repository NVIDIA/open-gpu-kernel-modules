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

#include "core/core.h"
#include "core/gmcapi_impl.h"
#include "gpu/gpu.h"
#include "nvctassert.h"

//
// This file must have access to all types used by GMCAPIs
//
#include "rmgspseq.h" // TODO: Move into interface/gmcapi/

#include "gmcapi/gmcapi_vgpu.h"
#include "gmcapi/gmcapi_gsp.h"

// Declare all handler functions..
#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)      \
    extern NV_STATUS handler_func(GMCAPI_CONTEXT*);

#include "gmcapi/gmcapi_table.h"

NV_STATUS gmcapiDispatch(GMCAPI_CONTEXT *pCtx)
{
    NV_STATUS status;
    NvLength requestSize = 0;
    NvLength responseSize = 0;
    NvU32 flags = 0;
    GmcApiHandler *handler;
    NvBool bRpc;

    // Sanity check the RM caller-filled info
    NV_ASSERT_OR_RETURN(pCtx->command != 0, NV_ERR_GMCAPI_INTERNAL_RM_ERROR);
    NV_ASSERT_OR_RETURN(pCtx->pInParams != NULL  || pCtx->inParamSize == 0,     NV_ERR_GMCAPI_INTERNAL_RM_ERROR);
    NV_ASSERT_OR_RETURN(pCtx->pOutParams != NULL || pCtx->maxOutParamSize == 0, NV_ERR_GMCAPI_INTERNAL_RM_ERROR);

    // At this point, we don't allow in and out to alias
    if (pCtx->pInParams != NULL)
        NV_ASSERT_OR_RETURN(pCtx->pInParams != pCtx->pOutParams, NV_ERR_GMCAPI_INTERNAL_RM_ERROR);

    switch (pCtx->command)
    {
        //
        // For now, we generate a simple switch for all the possible GMC APIs.
        // As the number grows we will look into building a more optimal dispatch table
        //
        #define GMCAPI_LOCAL(numeric_id, symbolic_id, handler_func, input_type, output_type, apiflags, ...)        \
            case numeric_id:                                                                                 \
                handler = &handler_func;                                                                     \
                bRpc = NV_FALSE;                                                                             \
                flags = apiflags;                                                                            \
                requestSize  = (sizeof(input_type)  == sizeof(GMCAPI_NONE)) ? 0 : sizeof(input_type);        \
                responseSize = (sizeof(output_type) == sizeof(GMCAPI_NONE)) ? 0 : sizeof(output_type);       \
                break;

        #define GMCAPI_REMOTE(numeric_id, symbolic_id, handler_func, input_type, output_type, apiflags, ...)        \
            case numeric_id:                                                                                 \
                handler = &handler_func;                                                                     \
                bRpc = NV_TRUE;                                                                              \
                flags = apiflags;                                                                            \
                requestSize  = (sizeof(input_type)  == sizeof(GMCAPI_NONE)) ? 0 : sizeof(input_type);        \
                responseSize = (sizeof(output_type) == sizeof(GMCAPI_NONE)) ? 0 : sizeof(output_type);       \
                break;

        #define GMCAPI_KERNEL   GMCAPI_LOCAL
        #define GMCAPI_PHYSICAL GMCAPI_REMOTE
        #include "gmcapi/gmcapi_table.h"

        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown GMCAPI command ID 0x%08x\n", pCtx->command);
            return NV_ERR_GMCAPI_INVALID_COMMAND_ID;
    }
    const NvBool bDynamicRequest  = (requestSize  == sizeof(GMCAPI_DYNAMIC));
    const NvBool bDynamicResponse = (responseSize == sizeof(GMCAPI_DYNAMIC));

    if ((pCtx->inParamSize < requestSize) && !bDynamicRequest)
    {
        NV_PRINTF(LEVEL_ERROR, "Request size of %llu bytes is too small for GMCAPI command 0x%08x. Need at least %llu bytes.\n",
            (NvU64)pCtx->inParamSize, pCtx->command, (NvU64)requestSize);
        return NV_ERR_GMCAPI_REQUEST_SIZE_TOO_SMALL;
    }
    if (pCtx->inParamSize > requestSize)
    {
        // This can happen if the caller has a newer/extended struct; worth logging.
        NV_PRINTF(LEVEL_NOTICE, "GMCAPI command 0x%08x passed %llu bytes of input, which is more than expected %llu\n",
            pCtx->command, (NvU64)pCtx->inParamSize, (NvU64)requestSize);
    }
    if ((pCtx->maxOutParamSize < responseSize) && !bDynamicResponse)
    {
        NV_PRINTF(LEVEL_ERROR, "Max response size of %llu bytes is too small for GMCAPI command 0x%08x. Need at least %llu bytes.\n",
            (NvU64)pCtx->maxOutParamSize, pCtx->command, (NvU64)responseSize);
        return NV_ERR_GMCAPI_MAX_RESPONSE_SIZE_TOO_SMALL;
    }

    pCtx->flags = flags;

    if (bRpc)
    {
        GMCAPI_HEADER gmchdr = {0};
        gmchdr.command = pCtx->command;
        gmchdr.size = (NvU32)pCtx->inParamSize;
        gmchdr.request.max_response_size = (NvU32)pCtx->maxOutParamSize;

        NvBool bSync = !(flags & GMCAPI_FLAGS_FIRE_AND_FORGET);
        status = gpuSendGmcRpc(pCtx->pGpu, &gmchdr, pCtx->pInParams, pCtx->pOutParams, bSync);

        if (status == NV_OK && bSync)
        {
            status = gmchdr.response.status;
            pCtx->outParamSize = gmchdr.size;
        }
        return status;
    }

    NV_ASSERT_OR_RETURN(handler, NV_ERR_GMCAPI_INTERNAL_RM_ERROR);
    pCtx->outParamSize = responseSize;

    status = (*handler)(pCtx);

    if (status == NV_OK)
    {
        if (bDynamicResponse && (pCtx->outParamSize == sizeof(GMCAPI_DYNAMIC)))
        {
            NV_PRINTF(LEVEL_ERROR, "Command 0x%08x has a dynamic response, but the handler didn't set the size.\n",
                pCtx->command);
            return NV_ERR_GMCAPI_INTERNAL_RM_ERROR; // This is always an internal RM bug, not a bad call
        }
        else if (pCtx->outParamSize > pCtx->maxOutParamSize)
        {
            NV_PRINTF(LEVEL_ERROR, "Command 0x%08x tried to return %llu bytes of response, which is more than max requested of %llu\n",
                pCtx->command, (NvU64)pCtx->outParamSize, (NvU64)pCtx->maxOutParamSize);
            return NV_ERR_GMCAPI_INTERNAL_RM_ERROR; // This is always an internal RM bug, not a bad call
        }
    }
    return status;
}

//
// GMCAPI is often invoked over an RPC, and the actual handler functions might
// not be defined in the build that is invoking them, so we have to provide
// fallback stubs. These should never be called, but the linker needs to be
// able to resolve them.
//
#if PORT_COMPILER_IS_MSVC
NV_STATUS gmcapiInvalidHandler(GMCAPI_CONTEXT *pCtx)
{
    return NV_ERR_GMCAPI_INVALID_COMMAND_ID;
}
#if NVCPU_IS_X86
// On 32-bit x86 (yes, we still have those!), symbols get an underscore prepended
#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)      \
    __pragma(comment(linker, "/alternatename:_" #handler_func "=_gmcapiInvalidHandler"));
#else
#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)      \
    __pragma(comment(linker, "/alternatename:" #handler_func "=gmcapiInvalidHandler"));
#endif // NVCPU_IS_X86

#else
#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)      \
    __attribute__((weak)) NV_STATUS handler_func(GMCAPI_CONTEXT *pCtx) { return NV_ERR_GMCAPI_INVALID_COMMAND_ID; }
#endif // PORT_COMPILER_IS_MSVC

#include "gmcapi/gmcapi_table.h"


//
// Some sanity checks of the table and related stuff
//

// These structs are ABI stable:
ct_assert(sizeof(GMCAPI_HEADER) == 40);

// No commands use a reserved ID:
#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...) \
    ct_assert(numeric_id > GMCAPI_CMD_RESERVED_LAST);
#include "gmcapi/gmcapi_table.h"

// Each ID is unique (no duplicate case values in switch):
NvBool gmcapiIsValidNumericId(NvU32 id)
{
    switch (id)
    {
        #define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)      \
        case numeric_id:
        #include "gmcapi/gmcapi_table.h"
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

// All APIs implemented on kernel must be fire-and-forget; GSP can't block on Kernel
#define GMCAPI_KERNEL(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...) \
    ct_assert(flags & GMCAPI_FLAGS_FIRE_AND_FORGET);
#include "gmcapi/gmcapi_table.h"
