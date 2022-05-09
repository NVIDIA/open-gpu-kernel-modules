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

#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/bus/kern_bus.h"

NvBool ceIsCeGrce(OBJGPU *pGpu, NvU32 ceEngineType)
{
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS partnerParams = {0};
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (IsAMODEL(pGpu) || IsT234D(pGpu))
        return NV_FALSE;

    NV_ASSERT_OR_RETURN(NV2080_ENGINE_TYPE_IS_COPY(ceEngineType), NV_FALSE);

    NvU32   i;
    NV_STATUS status = NV_OK;

    partnerParams.engineType = ceEngineType;
    partnerParams.numPartners = 0;

    // See if the hal wants to handle this
    status = kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, &partnerParams);
    if (status != NV_OK)
    {
        // For channels that the hal didnt handle, we should just return
        // all of the supported engines except for the target engine.
        //
        // Update the engine Database
        status = gpuUpdateEngineTable(pGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not update the engine db. This is fatal\n");
            DBG_BREAKPOINT();
            return NV_FALSE;
        }

        // Make sure it all will fit
        if (pGpu->engineDB.size > NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "PartnerList space too small. This is fatal\n");
            DBG_BREAKPOINT();
            return NV_FALSE;
        }

        // Copy over all of the engines except the target
        for (i = 0; i < pGpu->engineDB.size; i++)
        {
            // Skip the engine handed in
            if (pGpu->engineDB.pType[i] != partnerParams.engineType )
            {
                partnerParams.partnerList[partnerParams.numPartners++] = pGpu->engineDB.pType[i];
            }
        }
    }

    // check if gr is in the partnerList
    for (i = 0; i < partnerParams.numPartners; i++)
    {
        if (partnerParams.partnerList[i] == NV2080_ENGINE_TYPE_GRAPHICS)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NvU32 ceCountGrCe(OBJGPU *pGpu)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32      engIdx;
    NvU32      grCeCount;

    if (pKernelBus == NULL || IsAMODEL(pGpu))
        return 0;

    grCeCount = 0;

    //
    // bug 2188230: Until FIFO reports max instance id for a given engine type,
    // start by iterating over all CE indices supported by POBJGPU
    //
    for (engIdx = 0; engIdx < GPU_MAX_CES; ++engIdx)
    {
        if (kbusCheckEngine_HAL(pGpu, pKernelBus, ENG_CE(engIdx)) &&
            ceIsCeGrce(pGpu, NV2080_ENGINE_TYPE_COPY(engIdx)))
        {
            grCeCount++;
        }
    }

    return grCeCount;
}
