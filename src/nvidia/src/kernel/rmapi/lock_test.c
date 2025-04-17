/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "core/locks.h"
#include "rmapi/client.h"
#include "rmapi/lock_test.h"
#include "rmapi/rs_utils.h"

#include "gpu/gpu.h"

#include "class/cl0101.h"

#include "g_finn_rm_api.h"

NV_STATUS
lockTestRelaxedDupObjConstruct_IMPL
(
    LockTestRelaxedDupObject               *pResource,
    CALL_CONTEXT                           *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL           *pParams
)
{
    OBJGPU *pParentGpu;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    RsResourceRef *pParentRef;
    GpuResource *pParentGpuRes;

    //
    // This is an off-by-default object since its only use is with testing. Return
    // an error unless the RM test code registry key is turned on by the user.
    //
    if (!pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE))
        return NV_ERR_TEST_ONLY_CODE_NOT_ENABLED;

    NV_ASSERT_OR_RETURN(pParams->pClient != NULL, NV_ERR_INVALID_PARAMETER);
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(pParams->pClient, pParams->hParent, &pParentRef));
    NV_ASSERT_OR_RETURN(pParentRef != NULL, NV_ERR_INVALID_PARAMETER);

    pParentGpuRes = dynamicCast(pParentRef->pResource, GpuResource);
    NV_ASSERT_OR_RETURN(pParentGpuRes != NULL, NV_ERR_INVALID_PARAMETER);

    pParentGpu = GPU_RES_GET_GPU(pParentGpuRes);

    if (RS_IS_COPY_CTOR(pParams))
    {
        NvU32 gpuMask = 0;

        LockTestRelaxedDupObject *pSrcObj = dynamicCast(pParams->pSrcRef->pResource, LockTestRelaxedDupObject);
        if (pSrcObj == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid source object\n");
            return NV_ERR_INVALID_PARAMETER;
        }

        if (pParentGpu == GPU_RES_GET_GPU(pSrcObj))
        {
            NV_ASSERT_OR_RETURN(
                rmGpuGroupLockIsOwner(pParentGpu->gpuInstance, GPU_LOCK_GRP_DEVICE, &gpuMask),
                NV_ERR_INVALID_LOCK_STATE);
        }
        else
        {
            NV_ASSERT_OR_RETURN(
                rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_ALL, &gpuMask),
                NV_ERR_INVALID_LOCK_STATE);
        }

        // Also verify the mask since all GPU lock is a superset of GPU device lock.
        NV_ASSERT_OR_RETURN(gpuMask == rmGpuLocksGetOwnedMask(), NV_ERR_INVALID_LOCK_STATE);
    }

    return NV_OK;
}

void
lockTestRelaxedDupObjDestruct_IMPL
(
    LockTestRelaxedDupObject *pResource
)
{
    return;
}
