/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/usermode_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "class/clc661.h" // HOPPER_USERMODE_A
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl003f.h" // NV01_MEMORY_LOCAL_PRIVILEGED

NV_STATUS
usrmodeConstruct_IMPL
(
    UserModeApi                  *pUserModeApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                      *pMemory        = staticCast(pUserModeApi, Memory);
    OBJGPU                      *pGpu           = pMemory->pGpu;
    KernelFifo                  *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NV_HOPPER_USERMODE_A_PARAMS *pAllocParams   = (NV_HOPPER_USERMODE_A_PARAMS*) pParams->pAllocParams;
    NvU32                        hClass         = pCallContext->pResourceRef->externalClassId;
    NvBool                       bBar1Mapping   = NV_FALSE;
    NvBool                       bPrivMapping   = NV_FALSE;
    MEMORY_DESCRIPTOR           *pMemDesc       = pKernelFifo->pRegVF;
    NvU32                        memClassId     = NV01_MEMORY_LOCAL_PRIVILEGED;

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
    {
        UserModeApi *pUserModeSrc = dynamicCast(pParams->pSrcRef->pResource, UserModeApi);

        pUserModeApi->bInternalMmio = pUserModeSrc->bInternalMmio;
        pUserModeApi->bPrivMapping = pUserModeSrc->bPrivMapping;

        return NV_OK;
    }

    if (hClass >= HOPPER_USERMODE_A && pAllocParams != NULL)
    {
        bBar1Mapping = pAllocParams->bBar1Mapping;
        bPrivMapping = pAllocParams->bPriv;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        !bPrivMapping || bBar1Mapping,
        NV_ERR_INVALID_PARAMETER);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        !bPrivMapping || pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL,
        NV_ERR_INSUFFICIENT_PERMISSIONS);

    //
    // In order to map only internal MMIO, pAllocParams->bBar1Mapping must be set to true,
    // and clients need not map to CPU. This parameter might be renamed in the future to indicate
    // that it is both for BAR1 use and internal MMIO use.
    //
    pUserModeApi->bInternalMmio = bBar1Mapping;
    pUserModeApi->bPrivMapping = bPrivMapping;

    //
    // On platforms where BAR1 is disabled, such as coherent platforms, we don't support creating BAR1
    // mappings of the doorbell page, but we still support internal MMIO. This check transparently
    // returns a BAR0 CPU mapping on these platforms.
    //
    if (kbusIsBar1Disabled(GPU_GET_KERNEL_BUS(pGpu)) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        bBar1Mapping = NV_FALSE;
    }

    if (bBar1Mapping)
    {
        pMemDesc = bPrivMapping ? pKernelFifo->pBar1PrivVF : pKernelFifo->pBar1VF;
        memClassId = NV01_MEMORY_SYSTEM;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memConstructCommon(pMemory, memClassId, 0, pMemDesc, 0, NULL,
                               0, 0, 0, 0, NVOS32_MEM_TAG_NONE, NULL));
    memdescAddRef(pMemDesc);
    return NV_OK;
}

NvBool
usrmodeCanCopy_IMPL(UserModeApi *pUserModeApi){
    return NV_TRUE;
}

NV_STATUS
usrmodeGetMemInterMapParams_IMPL
(
    UserModeApi *pUserModeApi,
    RMRES_MEM_INTER_MAP_PARAMS *pParams
)
{
    //
    // We need to always return the correct memdesc for GPU mapping, regardless of what is used on the CPU.
    // We do this by overriding the regular pParams returned from memGetMemInterMapParams
    //
    Memory     *pMemory     = staticCast(pUserModeApi, Memory);
    OBJGPU     *pGpu        = pMemory->pGpu;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, memGetMemInterMapParams_IMPL(pMemory, pParams));

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pUserModeApi->bInternalMmio, NV_ERR_INVALID_PARAMETER);

    pParams->pSrcMemDesc = pUserModeApi->bPrivMapping ? pKernelFifo->pBar1PrivVF : pKernelFifo->pBar1VF;

    return NV_OK;

}
