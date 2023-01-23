/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"

#include "published/turing/tu102/dev_ctrl.h"
#include "published/turing/tu102/dev_vm.h"

//
// Turing HAL routines required for hypervisor/guest support
//

/*!
 * @brief Enables virtual interrupts to be sent from a virtual function with
 * the specified GFID.
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   pIntr       Intr pointer
 * @param[in]   gfid        GFID assigned to the virtual function
 */
NV_STATUS
intrEnableVirtualIntrLeaf_TU102
(
    OBJGPU  *pGpu,
    Intr    *pIntr,
    NvU32    gfid
)
{
    GFID_ALLOC_STATUS gfidState;
    NvU32 reg = gfid / (DRF_SIZE(NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VALUE));
    NvU32 bit = gfid % (DRF_SIZE(NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VALUE));

    NV_ASSERT_OK_OR_RETURN(gpuGetGfidState(pGpu, gfid, &gfidState));
    if (gfidState == GFID_FREE)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid GFID 0x%x\n", gfid);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (reg >= NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET__SIZE_1)
    {
        NV_PRINTF(LEVEL_ERROR, "Computed register index exceeds valid available "
            "range of NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET. reg idx = 0x%x\n", reg);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    GPU_REG_WR32(pGpu, NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET(reg),
        (NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VECTOR_ENABLE << bit));
    return NV_OK;
}

/*!
 * @brief Services virtual interrupts, i.e. interrupts triggered by PRIV_DOORBELL
 * writes from virtual functions
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   pIntr       Intr pointer
 */
void
intrServiceVirtual_TU102
(
    OBJGPU  *pGpu,
    Intr    *pIntr
)
{
    NvU32 i;

    // For now, servicing only involves clearing interrupts from all GFIDs
    for (i = 0; i < NV_CTRL_VIRTUAL_INTR_LEAF__SIZE_1; i++)
    {
        NvU32 val = GPU_REG_RD32(pGpu, NV_CTRL_VIRTUAL_INTR_LEAF(i));
        if (val != 0)
        {
            GPU_REG_WR32(pGpu, NV_CTRL_VIRTUAL_INTR_LEAF(i), val);
        }
    }
}

/*!
 * @brief Trigger the PRIV doorbell
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] pIntr  Intr pointer
 * @param[in] handle Handle to pass along with the interrupt
 */
NV_STATUS
intrTriggerPrivDoorbell_TU102
(
    OBJGPU  *pGpu,
    Intr    *pIntr,
    NvU32    handle
)
{
    if (!gpuIsSriovEnabled(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_DOORBELL, handle);

    return NV_OK;
}
