/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_ctrl.h"
#include "published/turing/tu102/dev_vm.h"

//
// HAL routines for Turing software interrupts
//

/*!
 * @brief Set stalling software interrupt
 */
NV_STATUS
intrSetStallSWIntr_TU102
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_TRIGGER,
                  NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT);
    return NV_OK;
}

/*!
 * @brief Clear stalling software interrupt
 */
NV_STATUS
intrClearStallSWIntr_TU102
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{

    intrClearLeafVector_HAL(pGpu, pIntr, NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT, NULL);

    return NV_OK;
}

/*!
 * @brief Enable stalling software interrupt
 */
void
intrEnableStallSWIntr_TU102
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    NvU32 subtree = NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT);
    NvU32 topIdx = NV_CTRL_INTR_SUBTREE_TO_TOP_IDX(subtree);
    NvU32 topBit = NV_CTRL_INTR_SUBTREE_TO_TOP_BIT(subtree);

    intrEnableLeaf_HAL(pGpu, pIntr, NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT);
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(topIdx), NVBIT(topBit));
}

/*!
 * @brief Disable stalling software interrupt
 * Note: This disables the entire subtree for the stalling SW interrupt and as
 * such, should only be used in _osVerifyInterrupts, where it is guaranteed that
 * interrupts will be re-ennabled at a later time.
 */
void
intrDisableStallSWIntr_TU102
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    NvU32 subtree = NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT);
    NvU32 topIdx = NV_CTRL_INTR_SUBTREE_TO_TOP_IDX(subtree);
    NvU32 topBit = NV_CTRL_INTR_SUBTREE_TO_TOP_BIT(subtree);

    intrDisableLeaf_HAL(pGpu, pIntr, NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT);
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR(topIdx), NVBIT(topBit));
}

/*!
 * @brief Get interrupt mode (mode & pending)
 *
 * @param[in]   pGpu        GPU Object
 * @param[in]   pIntr       Intr Object
 * @param[out]  intrmode    Interrupt mode
 *                          0 = disabled
 *                          1 = hardware
 *                          2 = software
 * @param[out]  pending     Interrupt pending?
 */
void
intrGetStallInterruptMode_TU102
(
    OBJGPU   *pGpu,
    Intr     *pIntr,
    NvU32    *pIntrmode,
    NvBool   *pPending
)
{
    *pIntrmode = INTERRUPT_TYPE_SOFTWARE; // value put in to match with legacy

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
            *pPending = NV_FALSE;
        return;
    }

    *pPending = intrIsVectorPending_HAL(pGpu, pIntr, NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT, NULL);
}
