/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/thread_state.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "gpu/timer/objtmr.h"
#include "vgpu/vgpu_events.h"

#include "published/pascal/gp100/dev_boot.h"

static void
_intrSetIntrEnInHw_GP100
(
    OBJGPU *pGpu,
    Intr *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32       pmcIntrEnSet;
    NvU32       pmcIntrEnClear;
    //
    // If the GPU is in GC6 (aka powered down or rail-gated state), return
    // early from this routine. We don't want to touch any GPU registers while
    // its powered off as it will hang the system.
    //
    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return;
    }

    // Only set the mask interrupt line and clear the rest all interrupt lines.
    pmcIntrEnSet   = pIntr->intrCachedEnSet   &   pIntr->intrMask.cached;
    pmcIntrEnClear = pIntr->intrCachedEnClear | (~pIntr->intrMask.cached);

    // Mask the leaf level interrupts for cases where top PMC intr is not toggeled
    intrSetHubLeafIntr_HAL(pGpu, pIntr, pIntr->intrCachedEn0, &pmcIntrEnClear, &pmcIntrEnSet, pThreadState);

    GPU_REG_WR32_EX(pGpu, NV_PMC_INTR_EN_CLEAR(0), pmcIntrEnClear, pThreadState);
    GPU_REG_WR32_EX(pGpu, NV_PMC_INTR_EN_SET(0),   pmcIntrEnSet,   pThreadState);
}

/**
 * @brief Write the top level intr enable 0 state to hardware
 *        Also Set HUB leaf interrupts if supported according to
 *        RM owned mask.
 * @param[in] pGpu
 * @param[in] pIntr
 * @param[in] flags Select BROADCAST (default) or UNICAST
 * @param[in] intrEn0 The value to write
 * @param[in] pThreadState thread state node pointer
 */
void
intrSetIntrEnInHw_GP100
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvU32              intrEn0,
    THREAD_STATE_NODE *pThreadState
)
{
    NV_ASSERT(intrEn0 <= INTERRUPT_TYPE_MAX);

    pIntr->intrCachedEn0 = intrEn0;

    intrEncodeStallIntrEn_HAL(pGpu, pIntr, intrEn0, &pIntr->intrCachedEnSet, &pIntr->intrCachedEnClear);

    _intrSetIntrEnInHw_GP100(pGpu, pIntr, pThreadState);
}

NV_STATUS
intrSetIntrMask_GP100
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngMask,
    THREAD_STATE_NODE   *pThreadState
)
{
    NvU32 intrMask = 0;

    if (bitVectorTestAllSet(pEngMask))
    {
        intrMask = INTERRUPT_MASK_ENABLED;
    }
    else if (bitVectorTestAllCleared(pEngMask))
    {
        intrMask = INTERRUPT_MASK_DISABLED;
    }
    else
    {
        intrMask = intrConvertEngineMaskToPmcIntrMask(pGpu, pIntr, pEngMask);
    }

    pIntr->intrMask.cached = intrMask;

    _intrSetIntrEnInHw_GP100(pGpu, pIntr, pThreadState);
    return NV_OK;
}

/**
 * @brief Get the top level intr enable 0 state from hardware
 * @param[in] pGpu
 * @param[in] pIntr
 */
NvU32
intrGetIntrEnFromHw_GP100
(
    OBJGPU *pGpu,
    Intr *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32       intrEn;
    //
    // If the GPU is in GC6 (aka powered down or rail-gated state), return
    // early from this routine. We don't want to touch any GPU registers while
    // its powered off as it will hang the system.
    //
    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return INTERRUPT_TYPE_DISABLED;
    }
    intrEn = GPU_REG_RD32_EX(pGpu, NV_PMC_INTR_EN(0), pThreadState);
    return intrDecodeStallIntrEn_HAL(pGpu, pIntr, intrEn);
}

NV_STATUS
intrGetIntrMask_GP100
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngMask,
    THREAD_STATE_NODE   *pThreadState
)
{
    NvU32 intrMask = pIntr->intrMask.cached;

    if (intrMask == 0x0)
    {
        bitVectorClrAll(pEngMask);
    }
    else if (intrMask == INTERRUPT_MASK_ENABLED)
    {
        bitVectorSetAll(pEngMask);
    }
    else
    {
        intrConvertPmcIntrMaskToEngineMask(pGpu, pIntr, intrMask, pEngMask);
    }

    return NV_OK;
}

/**
 * @brief Returns a bitfield with the MC_ENGINES that have pending interrupts
 */
NV_STATUS
intrGetPendingStall_GP100
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    extern NV_STATUS intrGetPendingStall_GM107(OBJGPU *pGpu, Intr *pIntr, MC_ENGINE_BITVECTOR *pEngines, THREAD_STATE_NODE *pThreadState);
    OBJTMR              *pTmr    = GPU_GET_TIMER(pGpu);
    MC_ENGINE_BITVECTOR  pendingGmmuEngines;

    NV_ASSERT_OK_OR_RETURN(intrGetPendingStall_GM107(pGpu, pIntr, pEngines, pThreadState));

    if (pTmr)
    {
        NvU32   retVal;

        tmrGetIntrStatus_HAL(pGpu, pTmr, &retVal, pThreadState);
        if (retVal != 0)
        {
            bitVectorSet(pEngines, MC_ENGINE_IDX_TMR);
        }
    }

    intrGetGmmuInterrupts(pGpu, pIntr, &pendingGmmuEngines, pThreadState);
    bitVectorOr(pEngines, pEngines, &pendingGmmuEngines);

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    return NV_OK;
}

/*!
 * @brief Encode the interrupt mode to be able to write it out to HW
 *
 * @param[in]   pGpu         OBJGPU pointer
 * @param[in]   pIntr        Intr pointer
 * @param[in]   intrEn       the enable value to encode
 * @param[out]  pIntrEnSet   the value to write to the set register
 * @param[out]  pIntrEnClear the value to write to the clear register
 *
 */
void
intrEncodeStallIntrEn_GP100
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    intrEn,
    NvU32   *pIntrEnSet,
    NvU32   *pIntrEnClear
)
{
    if (intrEn == INTERRUPT_TYPE_SOFTWARE)
    {
        *pIntrEnSet = INTERRUPT_MASK_SOFTWARE;
        *pIntrEnClear = INTERRUPT_MASK_HARDWARE;
    }
    else if (intrEn == INTERRUPT_TYPE_HARDWARE)
     {
        *pIntrEnSet = INTERRUPT_MASK_HARDWARE;
        *pIntrEnClear = INTERRUPT_MASK_SOFTWARE;
    }
    else if (intrEn == INTERRUPT_TYPE_DISABLED)
    {
        *pIntrEnSet = INTERRUPT_MASK_DISABLED;
        *pIntrEnClear = INTERRUPT_MASK_ENABLED;
    }
    else
    {
        *pIntrEnSet = INTERRUPT_MASK_ENABLED;
        *pIntrEnClear = INTERRUPT_MASK_DISABLED;
    }

    // Only toggle those interrupts that RM owns.
    if (pGpu->pmcRmOwnsIntrMask != 0)
    {
        *pIntrEnSet &= pGpu->pmcRmOwnsIntrMask;
        *pIntrEnClear &= pGpu->pmcRmOwnsIntrMask;
    }
}
