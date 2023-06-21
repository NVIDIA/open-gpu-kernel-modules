/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *   File: mig_config_session.c
 *
 *   Description:
 *       Upon successful allocation of this class, a client is granted
 *       permission to invoke the privileged GPU instance configuration
 *       control calls.
 *
 *****************************************************************************/

#define NVOC_MIG_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "gpu/gpu.h"
#include "kernel/gpu/mig_mgr/mig_config_session.h"
#include "class/clc639.h"
#include "os/os.h"
#include "rmapi/client.h"

NV_STATUS
migconfigsessionConstruct_IMPL
(
    MIGConfigSession             *pMIGConfigSession,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    NVC639_ALLOCATION_PARAMETERS *pUserParams = pRmAllocParams->pAllocParams;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvHandle hClient = pCallContext->pClient->hClient;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    osRmCapInitDescriptor(&pMIGConfigSession->dupedCapDescriptor);

    status = osRmCapAcquire(pSys->pOsRmCaps,
                            NV_RM_CAP_SYS_SMC_CONFIG,
                            pUserParams->capDescriptor,
                            &pMIGConfigSession->dupedCapDescriptor);

    //
    // On platforms where capability isn't implemented,
    // enforce the admin-only check.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        if (!rmclientIsAdminByHandle(hClient, pCallContext->secInfo.privLevel))
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
migconfigsessionDestruct_IMPL
(
    MIGConfigSession *pMIGConfigSession
)
{
    osRmCapRelease(pMIGConfigSession->dupedCapDescriptor);
}

