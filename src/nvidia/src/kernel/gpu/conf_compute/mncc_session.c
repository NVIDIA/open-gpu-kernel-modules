/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   File: mncc_session.c
 *
 *   Description:
 *       MnccSession class implementation.  Validates
 *       NV_RM_CAP_SYS_MNCC_MGMT on allocation and keeps the duped capability
 *       descriptor for the lifetime of the session.
 *
 *****************************************************************************/



#include "core/core.h"
#include "core/system.h"
#include "class/cl77cc.h"
#include "kernel/gpu/conf_compute/mncc_session.h"
#include "os/os.h"
#include "rmapi/client.h"

NV_STATUS
mnccSessionConstruct_IMPL
(
    MnccSession *pMnccSession,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV77CC_ALLOC_PARAMETERS *pUserParams = pParams->pAllocParams;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);
    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    osRmCapInitDescriptor(&pMnccSession->dupedCapDescriptor);

    status = osRmCapAcquire(pSys->pOsRmCaps,
                            NV_RM_CAP_SYS_MNCC_MGMT,
                            pUserParams->capDescriptor,
                            &pMnccSession->dupedCapDescriptor);
    //
    // On platforms where capability isn't implemented,
    // enforce the admin-only check.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        if (!rmclientIsAdmin(pRmClient, pCallContext->secInfo.privLevel))
        {
            NV_PRINTF(LEVEL_ERROR, "insufficient permissions\n");
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }
    else if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Capability validation failed\n");
        return status;
    }

    return NV_OK;
}

void
mnccSessionDestruct_IMPL
(
    MnccSession *pMnccSession
)
{
    osRmCapRelease(pMnccSession->dupedCapDescriptor);
}
