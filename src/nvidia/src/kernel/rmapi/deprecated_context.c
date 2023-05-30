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

#include "core/core.h"
#include "rmapi/rmapi.h"
#include "rmapi/param_copy.h"
#include "os/os.h"
#include "deprecated_context.h"

static NV_STATUS
_rmAllocForDeprecatedApi(DEPRECATED_CONTEXT *_pContext, NvHandle hClient, NvHandle hParent,
                         NvHandle *phObject, NvU32 hClass, void *pAllocParams, NvU32 paramsSize)
{
    DEPRECATED_CONTEXT_EXT *pContext = (DEPRECATED_CONTEXT_EXT *)_pContext;
    RM_API                 *pRmApi   = pContext->pRmApi;

    return pRmApi->AllocWithSecInfo(pRmApi, hClient, hParent, phObject,
                                    hClass, NV_PTR_TO_NvP64(pAllocParams), paramsSize,
                                    RMAPI_ALLOC_FLAGS_NONE, NvP64_NULL, &pContext->secInfo);
}

static NV_STATUS
_rmControlForDeprecatedApi(DEPRECATED_CONTEXT *_pContext, NvHandle hClient, NvHandle hObject,
                           NvU32 cmd, void *pParams, NvU32 paramsSize)
{
    DEPRECATED_CONTEXT_EXT *pContext = (DEPRECATED_CONTEXT_EXT *)_pContext;
    RM_API                 *pRmApi   = pContext->pRmApi;

    return pRmApi->ControlWithSecInfo(pRmApi, hClient, hObject, cmd,
                                      NV_PTR_TO_NvP64(pParams), paramsSize, 0,
                                      &pContext->secInfo);
}

static NV_STATUS
_rmFreeForDeprecatedApi(DEPRECATED_CONTEXT *_pContext, NvHandle hClient, NvHandle hObject)
{
    DEPRECATED_CONTEXT_EXT *pContext = (DEPRECATED_CONTEXT_EXT *)_pContext;
    RM_API                 *pRmApi   = pContext->pRmApi;

    return pRmApi->FreeWithSecInfo(pRmApi, hClient, hObject,
                                   RMAPI_FREE_FLAGS_NONE, &pContext->secInfo);
}

static NV_STATUS
_rmMapMemoryForDeprecatedApi(DEPRECATED_CONTEXT *_pContext, NvHandle hClient, NvHandle hDevice,
                             NvHandle hMemory, NvU64 offset, NvU64 length, NvP64 *ppCpuVirtAddr, NvU32 flags)
{
    DEPRECATED_CONTEXT_EXT *pContext = (DEPRECATED_CONTEXT_EXT *)_pContext;
    RM_API                 *pRmApi   = pContext->pRmApi;

    return pRmApi->MapToCpuWithSecInfo(pRmApi, hClient, hDevice, hMemory, offset, length, ppCpuVirtAddr, flags, &pContext->secInfo);
}

NV_STATUS
RmCopyUserForDeprecatedApi
(
    RMAPI_DEPRECATED_COPY_OP op,
    RMAPI_DEPRECATED_BUFFER_POLICY bufPolicy,
    NvP64 dataPtr,
    NvU32 dataSize,
    void **ppKernelPtr,
    NvBool bUserModeArgs
)
{
    NV_STATUS status = NV_OK;

    switch (op)
    {
        case RMAPI_DEPRECATED_COPYIN:
            if (bufPolicy == RMAPI_DEPRECATED_BUFFER_ALLOCATE)
            {
                *ppKernelPtr = portMemAllocNonPaged(dataSize);

                if (*ppKernelPtr == NULL)
                    return NV_ERR_NO_MEMORY;
            }

            status = rmapiParamsCopyIn(NULL,
                                       *ppKernelPtr,
                                       dataPtr,
                                       dataSize,
                                       bUserModeArgs);

            if (bufPolicy == RMAPI_DEPRECATED_BUFFER_ALLOCATE)
            {
                if (status != NV_OK)
                {
                    portMemFree(*ppKernelPtr);
                    *ppKernelPtr = NULL;
                }
            }
            break;
        case RMAPI_DEPRECATED_COPYOUT:
            status = rmapiParamsCopyOut(NULL,
                                        *ppKernelPtr,
                                        dataPtr,
                                        dataSize,
                                        bUserModeArgs);

            // intentionally fall through to release memory...
        case RMAPI_DEPRECATED_COPYRELEASE:
            if (bufPolicy == RMAPI_DEPRECATED_BUFFER_ALLOCATE)
            {
                portMemFree(*ppKernelPtr);
                *ppKernelPtr = NULL;
            }
            break;
    }

    return status;
}

static NV_STATUS
_rmCopyUserForDeprecatedApi
(
    DEPRECATED_CONTEXT *_pContext,
    RMAPI_DEPRECATED_COPY_OP op,
    RMAPI_DEPRECATED_BUFFER_POLICY bufPolicy,
    NvP64 dataPtr,
    NvU32 dataSize,
    void **ppKernelPtr
)
{
    return RmCopyUserForDeprecatedApi(op, bufPolicy, dataPtr, dataSize,
                                      ppKernelPtr,
                                      ((DEPRECATED_CONTEXT_EXT *)_pContext)->bUserModeArgs);
}

static void *
_rmAllocMemForDeprecatedApi(NvU32 length)
{
    return portMemAllocNonPaged(length);
}

static void
_rmFreeMemForDeprecatedApi(void *pAddress)
{
    portMemFree(pAddress);
}

/**
 * Setting bUserModeArgs to NV_FALSE can lead to Security issues where
 * Privileged RM CTRL APIs are accessible by non-admin users.
 * Please find more details in Bug: 3136168.
 */
void rmapiInitDeprecatedContext
(
    DEPRECATED_CONTEXT_EXT *pContext,
    API_SECURITY_INFO *pSecInfo,
    NvBool bUserModeArgs,
    NvBool bInternal
)
{
    if (pSecInfo == NULL)
    {
        portMemSet(&pContext->secInfo, 0, sizeof(pContext->secInfo));

        if (bUserModeArgs)
        {
            pContext->secInfo.privLevel = osIsAdministrator() ? RS_PRIV_LEVEL_USER_ROOT : RS_PRIV_LEVEL_USER;
        }
        else
        {
            pContext->secInfo.privLevel = RS_PRIV_LEVEL_KERNEL;
        }
    }
    else
    {
        pContext->secInfo = *pSecInfo;
    }

    pContext->secInfo.paramLocation = PARAM_LOCATION_KERNEL;

    pContext->bInternal             = bInternal;
    pContext->pRmApi                = rmapiGetInterface(bInternal ? RMAPI_GPU_LOCK_INTERNAL : RMAPI_EXTERNAL);
    pContext->bUserModeArgs         = bUserModeArgs;

    pContext->parent.RmAlloc        = _rmAllocForDeprecatedApi;
    pContext->parent.RmControl      = _rmControlForDeprecatedApi;
    pContext->parent.RmFree         = _rmFreeForDeprecatedApi;
    pContext->parent.RmMapMemory    = _rmMapMemoryForDeprecatedApi;
    pContext->parent.CopyUser       = _rmCopyUserForDeprecatedApi;
    pContext->parent.AllocMem       = _rmAllocMemForDeprecatedApi;
    pContext->parent.FreeMem        = _rmFreeMemForDeprecatedApi;
}
