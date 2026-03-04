/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/system.h"
#include "rmapi/rmapi.h"
#include "rmapi/param_copy.h"
#include "rmapi/alloc_size.h"
#include "rmapi/control.h"
#include "os/os.h"

NV_STATUS rmapiParamsAcquire
(
    RMAPI_PARAM_COPY  *pParamCopy,
    NvBool             bUserModeArgs
)
{
    NvBool      bUseParamsDirectly;
    void       *pKernelParams = NULL;
    NV_STATUS   rmStatus = NV_OK;
    OBJSYS     *pSys = SYS_GET_INSTANCE();

    // Error check parameters
    if (((pParamCopy->paramsSize != 0) && (pParamCopy->pUserParams == NvP64_NULL)) ||
        ((pParamCopy->paramsSize == 0) && (pParamCopy->pUserParams != NvP64_NULL)) ||
        !pParamCopy->bSizeValid)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "%s:  bad params from client: ptr " NvP64_fmt " size: 0x%x (%s)\n",
                  pParamCopy->msgTag, pParamCopy->pUserParams, pParamCopy->paramsSize,
                  pParamCopy->bSizeValid ? "valid" : "invalid");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    bUseParamsDirectly = (pParamCopy->paramsSize == 0) || (!bUserModeArgs);

    // if we can use client params directly, we're done.
    if (bUseParamsDirectly)
    {
        if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS))
        {
            // Check that its a kernel pointer
            rmStatus = osIsKernelBuffer((void*)NvP64_VALUE(pParamCopy->pUserParams),
                                        pParamCopy->paramsSize);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Error validating kernel pointer. Status 0x%x\n",
                          rmStatus);
                goto done;
            }
        }

        pParamCopy->flags |= RMAPI_PARAM_COPY_FLAGS_IS_DIRECT_USAGE;
        pKernelParams     = NvP64_VALUE(pParamCopy->pUserParams);

        if (pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER)
            portMemSet(pKernelParams, 0, pParamCopy->paramsSize);

        goto done;
    }

    if (!(pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK))
    {
        if (pParamCopy->paramsSize > RMAPI_PARAM_COPY_MAX_PARAMS_SIZE)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "(%s): Requested size exceeds max (%ud > %ud)\n",
                      pParamCopy->msgTag, pParamCopy->paramsSize,
                      RMAPI_PARAM_COPY_MAX_PARAMS_SIZE);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

    pKernelParams = portMemAllocNonPaged(pParamCopy->paramsSize);
    if (pKernelParams == NULL)
    {
        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        NV_PRINTF(LEVEL_WARNING, "(%s): portMemAllocNonPaged failure: status 0x%x\n",
                  pParamCopy->msgTag, rmStatus);
        goto done;
    }

    // Copyin unless directed otherwise
    if (pParamCopy->pUserParams)
    {
        if (pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN)
        {
            if (pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER)
                portMemSet(pKernelParams, 0, pParamCopy->paramsSize);
        }
        else
        {
            rmStatus = portMemExCopyFromUser(pParamCopy->pUserParams, pKernelParams, pParamCopy->paramsSize);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "(%s): portMemExCopyFromUser failure: status 0x%x\n",
                          pParamCopy->msgTag, rmStatus);
                goto done;
            }
        }
    }

done:
    if (rmStatus != NV_OK) // There was an error, be sure to free the buffer
    {
        if (pKernelParams != NULL)
        {
            portMemFree(pKernelParams);
            pKernelParams = NULL;
        }
    }

    NV_ASSERT(pParamCopy->ppKernelParams != NULL);
    *(pParamCopy->ppKernelParams) = pKernelParams;
    return rmStatus;
}

//
// Copyout if needed and free any tmp param buffer
// Skips copyout if API_PARAMS_SKIP_COPYOUT is set.
//
NV_STATUS rmapiParamsRelease
(
    RMAPI_PARAM_COPY  *pParamCopy
)
{
    NV_STATUS rmStatus = NV_OK;

    // nothing to do, rmapiParamsAcquire() is either not called or not completed
    if (NULL == pParamCopy->ppKernelParams)
        return NV_OK;

    // if using the client's buffer directly, there's nothing to do
    if (pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_IS_DIRECT_USAGE)
        goto done;

    // if no kernel param ptr, there must be nothing to copy out
    // This can only happen if rmapiParamsAccess() returned an error,
    // but we need to handle it since rmapiParamsRelease() might be
    // called anyway.
    if (NULL == *pParamCopy->ppKernelParams)
        goto done;

    // do the copyout if something to copy, unless told to skip it...
    if (pParamCopy->pUserParams &&  ! (pParamCopy->flags & RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT))
    {
        rmStatus = portMemExCopyToUser(*(pParamCopy->ppKernelParams), pParamCopy->pUserParams, pParamCopy->paramsSize);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "(%s): portMemExCopyToUser failure: status 0x%x\n",
                      pParamCopy->msgTag, rmStatus);

            // even if the copyout fails, we still need to free the kernel mem
        }
    }

    portMemFree(*pParamCopy->ppKernelParams);

done:
    // no longer ok to use the ptr, even if it was a direct usage
    *pParamCopy->ppKernelParams = NULL;
    return rmStatus;
}

// This is a one-shot suitable for a case where we already have a kernel
// buffer and just need to copy into it from a user buffer.
// Not for general use...
//
// It uses the same logic as rmapiParamsAccess(), but does not maintain
// an RMAPI_PARAM_COPY container.
NV_STATUS rmapiParamsCopyIn
(
    const char *msgTag,
    void       *pKernelParams,
    NvP64       pUserParams,
    NvU32       paramsSize,
    NvBool      bUserModeArgs
)
{
    NV_STATUS  rmStatus;

    // error check parameters
    if ((paramsSize == 0)                                    ||
        (pKernelParams == NULL)                              ||
        (pUserParams == NvP64_NULL))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "(%s):  bad params from client: ptr " NvP64_fmt " size: 0x%x\n",
                  msgTag, pUserParams, paramsSize);

        return NV_ERR_INVALID_ARGUMENT;
    }

    // if we can use client params directly, just memcpy()
    if (bUserModeArgs == NV_FALSE)
    {
        // If the same ptr we can skip the memcpy
        if (pKernelParams != NvP64_VALUE(pUserParams))
        {
            (void) portMemCopy(pKernelParams, paramsSize, NvP64_VALUE(pUserParams), paramsSize);
        }
        rmStatus = NV_OK;
    }
    else
    {
        rmStatus = portMemExCopyFromUser(pUserParams, pKernelParams, paramsSize);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "(%s): portMemExCopyFromUser failure: status 0x%x\n",
                      msgTag, rmStatus);
        }
    }

    return rmStatus;
}

// This is a one-shot suitable for a case where we already have a kernel
// buffer and just need to copy it out correctly to a user buffer.
// Not for general use...
//
// It uses the same logic as rmapiParamsAccess(), but does not maintain
// an RMAPI_PARAM_COPY container.

NV_STATUS rmapiParamsCopyOut
(
    const char *msgTag,
    void       *pKernelParams,
    NvP64       pUserParams,
    NvU32       paramsSize,
    NvBool      bUserModeArgs
)
{
    NV_STATUS  rmStatus;

    // error check parameters
    if ((paramsSize == 0)                                    ||
        (pKernelParams == NULL)                              ||
        (pUserParams == NvP64_NULL))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "(%s):  bad params from client: ptr " NvP64_fmt " size: 0x%x\n",
                  msgTag, pUserParams, paramsSize);

        return NV_ERR_INVALID_ARGUMENT;
    }

    // if we can use client params directly, just memcpy()
    if (bUserModeArgs == NV_FALSE)
    {
        // If the same ptr we can skip the memcpy
        if (pKernelParams != NvP64_VALUE(pUserParams))
        {
            (void) portMemCopy(NvP64_VALUE(pUserParams), paramsSize, pKernelParams, paramsSize);
        }
        rmStatus = NV_OK;
    }
    else
    {
        rmStatus = portMemExCopyToUser(pKernelParams, pUserParams, paramsSize);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "(%s): portMemExCopyToUser failure: status 0x%x\n",
                      msgTag, rmStatus);
        }
    }

    return rmStatus;
}

NV_STATUS
rmapiParamsCopyInit
(
    RMAPI_PARAM_COPY *pParamCopy,
    NvU32             hClass
)
{
    NvU32             status;
    NvBool            bAllowNull;

    status = rmapiGetClassAllocParamSize(&pParamCopy->paramsSize,
                                         pParamCopy->pUserParams,
                                         &bAllowNull,
                                         hClass);
    if (status != NV_OK)
        return status;

    // NULL pUserParams is not allowed for given class
    if (bAllowNull == NV_FALSE && pParamCopy->pUserParams == NvP64_NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pParamCopy->bSizeValid = NV_TRUE;
    return NV_OK;
}


