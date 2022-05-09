/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _PARAM_COPY_H_
#define _PARAM_COPY_H_

//
// RMAPI_PARAM_COPY - a mechanism for getting user params in and out of resman.
//
// The struct RMAPI_PARAM_COPY keeps track of current API params for eventual
// copyout and free as needed.
//

#include <core/core.h>

struct API_STATE
{
    NvP64       pUserParams;           // ptr to params in client's addr space
    void      **ppKernelParams;        // ptr to current 'pKernelParams'
    NvU32       paramsSize;            // # bytes
    NvU32       flags;
    NvBool      bSizeValid;
    const char *msgTag;
};
typedef struct API_STATE RMAPI_PARAM_COPY;

#define RMAPI_PARAM_COPY_FLAGS_NONE                    0x00000000
#define RMAPI_PARAM_COPY_FLAGS_IS_DIRECT_USAGE         NVBIT(0)
#define RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN             NVBIT(1)
#define RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT            NVBIT(2)
#define RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER             NVBIT(3)
//
// Only set this if the paramsSize member of RMAPI_PARAM_COPY has been validated for
// correctness before calling apiParamAccess.  There is a default cap on the
// largest size allowed in order to avoid huge memory allocations triggering
// out of memory scenarios if the user passes in a bogus size.
//
#define RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK  NVBIT(4)
//
// 1MB is the largest size allowed for an embedded pointer accessed through
// apiParamAccess unless RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK is specified
// and the size is validated before calling apiParamsAcquire.
//
#define RMAPI_PARAM_COPY_MAX_PARAMS_SIZE              (1*1024*1024)

#if NV_PRINTF_STRINGS_ALLOWED
#define RMAPI_PARAM_COPY_MSG_TAG(x)         x
#define RMAPI_PARAM_COPY_SET_MSG_TAG(paramCopy, theMsgTag)  (paramCopy).msgTag = theMsgTag
#else
#define RMAPI_PARAM_COPY_MSG_TAG(x)   ((const char *) 0)
#define RMAPI_PARAM_COPY_SET_MSG_TAG(paramCopy, theMsgTag) (paramCopy).msgTag = ((const char *) 0)
#endif

//
// Initializes the RMAPI_PARAM_COPY structure. Sets bValid to false if calculating size
// caused an overflow. This makes the rmapiParamsAcquire() call fail with
// NV_ERR_INVALID_ARGUMENT. Since rmapiParamsAcquire() always directly follows
// this initialization, there is no need to make it return a status and
// duplicate error checking.
//
#define RMAPI_PARAM_COPY_INIT(paramCopy, pKernelParams, theUserParams, numElems, sizeOfElem) \
    do {                                                                 \
        RMAPI_PARAM_COPY_SET_MSG_TAG((paramCopy), __FUNCTION__);                \
        (paramCopy).ppKernelParams = (void **) &(pKernelParams);         \
        (paramCopy).pUserParams    = (theUserParams);                    \
        (paramCopy).flags          = RMAPI_PARAM_COPY_FLAGS_NONE;               \
        (paramCopy).bSizeValid     = portSafeMulU32((numElems), (sizeOfElem), &(paramCopy).paramsSize); \
    } while(0)

// Routines for alloc/copyin/copyout/free sequences
NV_STATUS rmapiParamsAcquire(RMAPI_PARAM_COPY *, NvBool);
NV_STATUS rmapiParamsRelease(RMAPI_PARAM_COPY *);

NV_STATUS rmapiParamsCopyOut(const char *msgTag, void *pKernelParams, NvP64 pUserParams, NvU32 paramsSize, NvBool);
NV_STATUS rmapiParamsCopyIn(const char *msgTag, void *pKernelParams, NvP64 pUserParams, NvU32 paramsSize, NvBool);

// Init copy_param structure
NV_STATUS rmapiParamsCopyInit(RMAPI_PARAM_COPY *, NvU32 hClass);

#endif // _PARAM_COPY_H_
