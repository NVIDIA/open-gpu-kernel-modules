/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define CE_GET_LOWEST_AVAILABLE_IDX(x) \
    (x == 0 || x == 0xFFFFFFFF) ? 0xFFFFFFFF : portUtilCountTrailingZeros32(x)

/*!
 * @brief Obtain relative CE index.
 *
 * @param rmEngineType RM_ENGINE_TYPE_ for this CE, or partition-local engine type.
 * @param ceIdx CE index in 0..GPU_MAX_CES-1
 *
 * @return NV_OK if the conversion is successful.
 */
static NV_INLINE
NV_STATUS ceIndexFromType(OBJGPU *pGpu, Device *pDevice, RM_ENGINE_TYPE rmEngineType, NvU32 *ceIdx)
{
    NV_STATUS status = NV_OK;
    RM_ENGINE_TYPE localRmEngType = rmEngineType;

    *ceIdx = GPU_MAX_CES;

    //
    // If MIG is enabled, client passes a logical engineId w.r.t its own partition
    // we need to convert this logical Id to a physical engine Id as we use it
    // to set runlistId
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;
        NvBool bEnginePresent;

        status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref);
        if (status != NV_OK)
            return status;

        bEnginePresent = kmigmgrIsLocalEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref);
        if (!bEnginePresent)
            return NV_ERR_INVALID_ARGUMENT;

        status = kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, rmEngineType, &localRmEngType);
        if (status != NV_OK)
            return status;
    }

    if (!RM_ENGINE_TYPE_IS_COPY(localRmEngType))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *ceIdx = RM_ENGINE_TYPE_COPY_IDX(localRmEngType);
    return status;
}

#endif // KERNEL_CE_PRIVATE_H
