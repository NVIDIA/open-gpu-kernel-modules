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

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
    {
        return NV_OK;
    }

    //
    // We check pKernelFifo->pBar1VF because for some reason RM allows HOPPER_USERMODE_A on ADA.
    // This is a WAR until we root cause.
    //
    if (hClass >= HOPPER_USERMODE_A && pAllocParams != NULL && pKernelFifo->pBar1VF != NULL)
    {
        bBar1Mapping = pAllocParams->bBar1Mapping;
        bPrivMapping = pAllocParams->bPriv;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        bBar1Mapping = NV_FALSE;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        !bPrivMapping || bBar1Mapping,
        NV_ERR_INVALID_PARAMETER);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        !bPrivMapping || pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL,
        NV_ERR_INSUFFICIENT_PERMISSIONS);

    if (bBar1Mapping)
    {
        pMemDesc = bPrivMapping ? pKernelFifo->pBar1PrivVF : pKernelFifo->pBar1VF;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memConstructCommon(pMemory,
                bBar1Mapping? NV01_MEMORY_SYSTEM : NV01_MEMORY_LOCAL_PRIVILEGED,
                0, pMemDesc, 0, NULL, 0, 0, 0, 0, NVOS32_MEM_TAG_NONE, NULL));
    memdescAddRef(pMemDesc);
    return NV_OK;
}

NvBool
usrmodeCanCopy_IMPL(UserModeApi *pUserModeApi){
    return NV_TRUE;
}
