/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/generic_engine.h"
#include "rmapi/client.h"

#include <class/cl90e6.h>
#include <class/cl90e7.h>


NV_STATUS
genapiConstruct_IMPL
(
    GenericEngineApi *pGenericEngineApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RS_ITERATOR it;
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pGenericEngineApi);

    if (!gpuIsClassSupported(pGpu, pCallContext->pResourceRef->externalClassId))
        return NV_ERR_INVALID_CLASS;

    //
    // We allow multiple instances of GenericEngineApi class, however, only want
    // to allow a single instance of each external class id type. E.g.:
    // GF100_SUBDEVICE_GRAPHICS is allowed alongside GF100_SUBDEVICE_FB.
    //
    it = clientRefIter(pCallContext->pClient,
                       pCallContext->pResourceRef->pParentRef,
                       classId(GenericEngineApi), RS_ITERATE_CHILDREN, NV_TRUE);

    while (clientRefIterNext(pCallContext->pClient, &it))
    {
         if (it.pResourceRef->externalClassId == pCallContext->pResourceRef->externalClassId &&
             it.pResourceRef != pCallContext->pResourceRef)
         {
             return NV_ERR_STATE_IN_USE;
         }
    }

    return NV_OK;
}

void
genapiDestruct_IMPL
(
    GenericEngineApi *pGenericEngineApi
)
{
}

NV_STATUS
genapiControl_IMPL
(
    GenericEngineApi *pGenericEngineApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    if (RES_GET_EXT_CLASS_ID(pGenericEngineApi) == GF100_SUBDEVICE_INFOROM)
    {
        OBJGPU     *pGpu     = GPU_RES_GET_GPU(pGenericEngineApi);
        OBJINFOROM *pInforom = GPU_GET_INFOROM(pGpu);
        if (!IS_GSP_CLIENT(pGpu) && pInforom == NULL)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    return gpuresControl_IMPL(staticCast(pGenericEngineApi, GpuResource),
                              pCallContext, pParams);
}

NV_STATUS
genapiMap_IMPL
(
    GenericEngineApi *pGenericEngineApi,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    OBJGPU *pGpu;
    NvU32 engineOffset, regSize, regBase;
    NvU32 protect;
    NV_STATUS rmStatus;

    pGpu = GPU_RES_GET_GPU(pGenericEngineApi);

    // XXX The default should really be more restrictive
    protect = NV_PROTECT_READ_WRITE;

    switch (RES_GET_EXT_CLASS_ID(pGenericEngineApi))
    {
        case GF100_SUBDEVICE_MASTER:
        {
            regSize = sizeof(GF100MASTERMap);
            regBase = NV_REG_BASE_MASTER;
            protect = NV_PROTECT_READABLE;
            break;
        }
        default:
            return NV_ERR_INVALID_CLASS;
    }

    // Get the offset to the engine registers
    rmStatus = gpuGetRegBaseOffset_HAL(pGpu, regBase, &engineOffset);
    if (rmStatus != NV_OK)
        return rmStatus;

    // Round down to nearest 4k page
    engineOffset &= ~(0x1000-1);

    // Check the caller is requesting more privilieges than we allow
    if (pCpuMapping->pPrivate->protect & ~protect)
    {
        NV_PRINTF(LEVEL_ERROR, "%s%saccess not allowed on class 0x%x\n",
                  (pCpuMapping->pPrivate->protect & ~protect) & NV_PROTECT_READABLE ? "Read " : "",
                  (pCpuMapping->pPrivate->protect & ~protect) & NV_PROTECT_WRITEABLE ? "Write " : "",
                  RES_GET_EXT_CLASS_ID(pGenericEngineApi));

        return NV_ERR_PROTECTION_FAULT;
    }

    // Create mapping
    rmStatus = rmapiMapGpuCommon(staticCast(pGenericEngineApi, RsResource),
                                 pCallContext,
                                 pCpuMapping,
                                 pGpu,
                                 engineOffset,
                                 regSize);
    pCpuMapping->processId = osGetCurrentProcess();

    if (pParams->ppCpuVirtAddr)
        *pParams->ppCpuVirtAddr = pCpuMapping->pLinearAddress;

    return rmStatus;
}

NV_STATUS
genapiGetMapAddrSpace_IMPL
(
    GenericEngineApi *pGenericEngineApi,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    if (pAddrSpace)
        *pAddrSpace = ADDR_REGMEM;

    return NV_OK;
}

