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

#include "core/core.h"
#include "kernel/gpu/intr/intr.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "gpu/disp/kern_disp.h"

/*!
 * @brief Returns a bitfield with only MC_ENGINE_IDX_DISP set if it's pending in hardware
 *        On Turing+, there are multiple stall interrupt registers, and reading them
 *        all in the top half would be expensive. To saitsfy bug 3220319, only find out
 *        if display interrupt is pending. Fix this in bug 3279300.
 *        GB20X+, we we report a separate low latency DISP bit that is only pending if the
 *        low latency interrupt is pending.
 *
 * @param[in]  pGpu
 * @param[in]  pMc
 * @param[out] pEngines     List of engines that have pending stall interrupts
 * @param[in]  pThreadState
 *
 * @return NV_OK if the list of engines that have pending stall interrupts was retrieved
 */
NV_STATUS
intrGetPendingLowLatencyHwDisplayIntr_GB202
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    bitVectorClrAll(pEngines);
    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return NV_ERR_GPU_NOT_FULL_POWER;
    }
    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    if (pIntr->displayLowLatencyIntrVector == NV_INTR_VECTOR_INVALID)
    {
        return NV_OK;
    }

    if (intrIsVectorPending_HAL(pGpu, pIntr, pIntr->displayLowLatencyIntrVector, pThreadState))
    {
        bitVectorSet(pEngines, MC_ENGINE_IDX_DISP_LOW);
    }

    return NV_OK;
}

void
intrSetDisplayInterruptEnable_GB202
(
    OBJGPU               *pGpu,
    Intr                 *pIntr,
    NvBool                bEnable,
    THREAD_STATE_NODE    *pThreadState
)
{
    if (pIntr->displayLowLatencyIntrVector == NV_INTR_VECTOR_INVALID)
    {
        return;
    }

    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->displayLowLatencyIntrVector);
    NvU32 leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(pIntr->displayLowLatencyIntrVector);
    if (bEnable)
    {
        intrWriteRegLeafEnSet_HAL(pGpu, pIntr, reg, NVBIT(leafBit), pThreadState);
    }
    else
    {
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr, reg, NVBIT(leafBit), pThreadState);
    }
}

void intrCacheDispIntrVectors_GB202(OBJGPU *pGpu, Intr *pIntr)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (pKernelDisplay != NULL && !RMCFG_FEATURE_PLATFORM_GSP)
    {
        pIntr->displayIntrVector           = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_DISP, NV_FALSE);
        pIntr->displayLowLatencyIntrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_DISP_LOW, NV_FALSE);
    }
    else
    {
        pIntr->displayIntrVector           = NV_INTR_VECTOR_INVALID;
        pIntr->displayLowLatencyIntrVector = NV_INTR_VECTOR_INVALID;
    }
}
