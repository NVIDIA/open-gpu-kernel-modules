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

#ifndef KERNEL_CE_PRIVATE_H
#define KERNEL_CE_PRIVATE_H

#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#define CE_GET_LOWEST_AVAILABLE_IDX(x) portUtilCountTrailingZeros32(x)

/*!
 * @brief Obtain relative CE index.
 *
 * @param localEngType NV2080_ENGINE_TYPE_ for this CE, or partition-local engine type.
 * @param ceIdx CE index in 0..GPU_MAX_CES-1
 *
 * @return NV_OK if the conversion is successful.
 */
static NV_INLINE
NV_STATUS ceIndexFromType(OBJGPU *pGpu, NvHandle hClient, NvU32 localEngType, NvU32 *ceIdx)
{
    //
    // If MIG is enabled, client passes a logical engineId w.r.t its own partition
    // we need to convert this logical Id to a physical engine Id as we use it
    // to set runlistId
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager,
                                            hClient, &ref));

        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              localEngType,
                                              ceIdx));
    }
    else
    {
        *ceIdx = localEngType;
    }

    if (!NV2080_ENGINE_TYPE_IS_COPY(*ceIdx))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *ceIdx = NV2080_ENGINE_TYPE_COPY_IDX(*ceIdx);
    return NV_OK;
}

#endif // KERNEL_CE_PRIVATE_H
