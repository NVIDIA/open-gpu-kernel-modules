/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/intr/intr_service.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr.h"

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 *
 * @param[in] pGpu
 * @param[in] pIntrService The IntrService object discovered as a GPU child;
 *                         not necessarily the one to be registered.
 * @param[in] pRecords
 */
void
intrservRegisterIntrService_IMPL(OBJGPU *pGpu, IntrService *pIntrService, IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX])
{
    return;
}

/**
 * @brief Clears the stall interrupt leaf vector and return whether to call ServiceStall.
 * @details Normally there's no need to override this function as its default is used by almost all handlers.
 *
 * @param[in] pGpu
 * @param[in] pIntrService The IntrService object registered to handle the engineIdx stall interrupt.
 * @param[in] pParams
 *
 * @returns A boolean which is NV_FALSE if the stall interrupt should not actually be handled.
 */
NvBool
intrservClearInterrupt_IMPL(OBJGPU *pGpu, IntrService *pIntrService, IntrServiceClearInterruptArguments *pParams)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_FALSE);

    Intr *pIntr = GPU_GET_INTR(pGpu);
    intrClearLeafVector_HAL(pGpu, pIntr,
                               intrGetVectorFromEngineId(pGpu, pIntr, pParams->engineIdx, NV_FALSE),
                               NULL);
    return NV_TRUE;
}

/**
 * @brief Services the stall interrupt.
 *
 * @param[in] pGpu
 * @param[in] pIntrService The IntrService object registered to handle the engineIdx stall interrupt.
 * @param[in] pParams
 *
 * @returns Zero, or any implementation-chosen nonzero value. If the same nonzero value is returned enough
 *          times the interrupt is considered stuck.
 */
NvU32
intrservServiceInterrupt_IMPL(OBJGPU *pGpu, IntrService *pIntrService, IntrServiceServiceInterruptArguments *pParams)
{
    NV_ASSERT_FAILED("intrservServiceInterrupt called but not implemented");
    // Return 0; no need to redundantly report stuck interrupt.
    return 0;
}

/**
 * @brief Services the nonstall interrupt.
 *
 * @param[in] pGpu
 * @param[in] pIntrService The IntrService object registered to handle the engineIdx nonstall interrupt.
 * @param[in] pParams
 *
 */
NV_STATUS
intrservServiceNotificationInterrupt_IMPL(OBJGPU *pGpu, IntrService *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams)
{
    NV_ASSERT_FAILED("intrservServiceNotificationInterrupt called but not implemented");
    return NV_ERR_INVALID_STATE;
}
