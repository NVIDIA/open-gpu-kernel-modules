/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"

#include "published/blackwell/gb10b/pri_nv_xal_ep.h"

/*!
 * @brief Writes NV_XAL_EP_BAR0_WINDOW_BASE
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] base       base address to write
 *
 * @returns NV_OK
 */
NV_STATUS
kbusWriteBAR0WindowBase_GB10B
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      base
)
{
    GPU_FLD_WR_DRF_NUM(pGpu, _XAL_EP, _BAR0_WINDOW, _BASE, base);
    return NV_OK;
}

/*!
 * @brief Reads NV_XAL_EP_BAR0_WINDOW_BASE
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns Contents of NV_XAL_EP_BAR0_WINDOW_BASE
 */
NvU32
kbusReadBAR0WindowBase_GB10B
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    return GPU_REG_RD_DRF(pGpu, _XAL_EP, _BAR0_WINDOW, _BASE);
}

/*!
 * @brief Validates that the given base fits within the width of the window base
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] base       base offset to validate
 *
 * @returns Whether given base fits within the width of the window base.
 */
NvBool
kbusValidateBAR0WindowBase_GB10B
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      base
)
{
    return base <= DRF_MASK(NV_XAL_EP_BAR0_WINDOW_BASE);
}

/*!
 * @brief Ensure MC point of coherency for volatile GPU writes
 *
 * NOTE: Must call kbusFlush BEFORE any calls to busInvalidate
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] ApertureFlags NvU32 aperture flags
 */
NV_STATUS
kbusFlush_GB10B
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    NvU32       ApertureFlags
)
{
    //
    // All memory in GB10B is local but we need to pass in VIDEO_MEMORY to have
    // the kbusFlushSingle implementation do a sysmembar flush.
    //
    return kbusFlushSingle_HAL(pGpu, pKernelBus, ApertureFlags | BUS_FLUSH_VIDEO_MEMORY);
}
