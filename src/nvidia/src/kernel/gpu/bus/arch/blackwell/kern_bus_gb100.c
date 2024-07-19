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

#include "published/blackwell/gb100/pri_nv_xal_ep.h"
#include "published/blackwell/gb100/pri_nv_xal_ep_p2p.h"
#include "published/blackwell/gb100/dev_pcfg_pf0.h"

// Minimum bar size
#define NV_PF0_PF_RESIZABLE_BAR_CONTROL_BAR_SIZE_MIN 0x6

/*!
 * @brief Gets the P2P write mailbox address size (NV_XAL_EP_P2P_WMBOX_ADDR_ADDR)
 *
 * @returns P2P write mailbox address size (NV_XAL_EP_P2P_WMBOX_ADDR_ADDR)
 */
NvU32
kbusGetP2PWriteMailboxAddressSize_GB100(OBJGPU *pGpu)
{
    return DRF_SIZE(NV_XAL_EP_P2P_WMBOX_ADDR_ADDR);
}

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
kbusWriteBAR0WindowBase_GB100
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
kbusReadBAR0WindowBase_GB100
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
kbusValidateBAR0WindowBase_GB100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      base
)
{
    return base <= DRF_MASK(NV_XAL_EP_BAR0_WINDOW_BASE);
}

/*!
* @brief Cache the value of NV_PF0_PF_RESIZABLE_BAR_CONTROL_BAR_SIZE
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*/
void
kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GB100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_PF_RESIZABLE_BAR_CONTROL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_PF_RESIZABLE_BAR_CONTROL\n");
        return;
    }
    pKernelBus->bar1ResizeSizeIndex = DRF_VAL(_PF0_PF, _RESIZABLE_BAR_CONTROL, _BAR_SIZE, regVal);
}

/*!
* @brief  Restore the value of NV_PF0_PF_RESIZABLE_BAR_CONTROL if different from the
* cached value. Windows has a strict requirement that the PCIE config has to stay the
* same across power transitions. Early SBIOS implementing resize BAR do not restore properly
* the value of NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE. The reason of this WAR is to not
* crash the systems that have not beed updated - yet.
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*
* @returns   NV_OK
*/
NV_STATUS
kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GB100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;
    NvU32 bar1ResizeSizeIndex;

    if (!pKernelBus->getProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pKernelBus->bar1ResizeSizeIndex >= NV_PF0_PF_RESIZABLE_BAR_CONTROL_BAR_SIZE_MIN,
                        NV_ERR_INVALID_DATA);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_PF_RESIZABLE_BAR_CONTROL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_PF_RESIZABLE_BAR_CONTROL\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    bar1ResizeSizeIndex = DRF_VAL(_PF0_PF, _RESIZABLE_BAR_CONTROL, _BAR_SIZE, regVal);

    if (bar1ResizeSizeIndex == pKernelBus->bar1ResizeSizeIndex)
    {
        // BAR1 size match. Nothing to do
        return NV_OK;
    }

    // BAR1 size changed. Warn and update
    NV_PRINTF(LEVEL_WARNING, "BAR1 size mismatch: current: 0x%x, expected: 0x%x\n",
              bar1ResizeSizeIndex, pKernelBus->bar1ResizeSizeIndex);
    NV_PRINTF(LEVEL_WARNING, "Most likely SBIOS did not restore the BAR1 size\n");
    NV_PRINTF(LEVEL_WARNING, "Please update your SBIOS!\n");

    regVal = FLD_SET_DRF_NUM(_PF0_PF, _RESIZABLE_BAR_CONTROL, _BAR_SIZE,
                             pKernelBus->bar1ResizeSizeIndex, regVal);
    GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_PF_RESIZABLE_BAR_CONTROL, regVal);

    return NV_OK;
}
