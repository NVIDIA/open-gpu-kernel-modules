/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/blackwell/gb100/dev_nv_xal_ep_zb.h"
#include "published/blackwell/gb100/dev_nv_xal_ep_p2p_zb.h"
#include "published/blackwell/gb100/dev_pcfg_pf0.h"

#include "published/blackwell/gb100/dev_hubmmu_base.h"
#include "published/blackwell/gb100/dev_top_zb.h"

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
    return DRF_SIZE(NV_XAL_EP_P2P_ZB_WMBOX_ADDR_ADDR);
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
    IoAperture *pXalAperture = kbusGetXalAperture_HAL(pGpu, pKernelBus, XAL_BASE);
    REG_FLD_WR_DRF_NUM(pXalAperture, _XAL_EP_ZB, _BAR0_WINDOW, _BASE, base);
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
    IoAperture *pXalAperture = kbusGetXalAperture_HAL(pGpu, pKernelBus, XAL_BASE);
    return REG_RD_DRF(pXalAperture, _XAL_EP_ZB, _BAR0_WINDOW, _BASE);
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
    return base <= DRF_MASK(NV_XAL_EP_ZB_BAR0_WINDOW_BASE);
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

    if ((GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_PF_RESIZABLE_BAR_CAPABILITY, &regVal) != NV_OK) ||
        (regVal == 0))
    {
        NV_PRINTF(LEVEL_INFO, "Resizable bar capability is absent\n");
        pKernelBus->setProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_FALSE);
        return;
    }

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_PF_RESIZABLE_BAR_CONTROL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_PF_RESIZABLE_BAR_CONTROL\n");
        pKernelBus->setProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_FALSE);
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

/*
 * @brief Get GPU PF BAR1 SPA from physical RM
 *
 * BAR1 SPA is required inside passthrough VM to provide
 * BAR1 mapping of FB for DirectNIC DMA.
 *
 * @param[in]  pGpu       OBJGPU pointer
 * @param[in]  pKernelBus KernelBus pointer
 * @param[out] pSpaValue  SPA of GPU PF BAR1
 *
 * @returns   NV_OK on success.
 */
NV_STATUS
kbusGetPFBar1Spa_GB100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU64       *pSpaValue
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS params = {0};

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_GPU_GET_PF_BAR1_SPA, &params, sizeof(params)));

    *pSpaValue = params.spaValue;

    return NV_OK;
}

void
kbusCarveoutWprs_GB100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NV_RANGE  *pWprRegions
)
{
    NvU32 data;
    NvU64 start;
    NvU64 end;
    const DEVICE_INFO_ENTRY *pEntry;
    NV_STATUS status = gpuGetOneDeviceEntry(pGpu,
                                            NV_PTOP_ZB_DEVICE_INFO_DEV_TYPE_ENUM_HUBMMU,
                                            DEVICE_INFO_DIELET_INSTANCE_ANY,
                                            0,
                                            DEVICE_INFO_DIE_LOCAL_INSTANCE_ID_ANY,
                                            &pEntry);
    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);
    data = GPU_REG_RD32(pGpu, pEntry->devicePriBase + NV_HUBMMU_PRI_MMU_WPR1_ADDR_LO);
    data = DRF_VAL(_HUBMMU, _PRI_MMU_WPR1_ADDR_LO, _VAL, data);
    start = (NvU64)data << NV_HUBMMU_PRI_MMU_WPR1_ADDR_LO_ALIGNMENT;

    data = GPU_REG_RD32(pGpu, pEntry->devicePriBase + NV_HUBMMU_PRI_MMU_WPR1_ADDR_HI);
    data = DRF_VAL(_HUBMMU, _PRI_MMU_WPR1_ADDR_HI, _VAL, data);
    end = (NvU64)data << NV_HUBMMU_PRI_MMU_WPR1_ADDR_HI_ALIGNMENT;

    if (start != end && end != 0)
        pWprRegions[0] = rangeMake(start, end - 1);
    else
        pWprRegions[0] = NV_RANGE_EMPTY;

    data = GPU_REG_RD32(pGpu, pEntry->devicePriBase + NV_HUBMMU_PRI_MMU_WPR2_ADDR_LO);
    data = DRF_VAL(_HUBMMU, _PRI_MMU_WPR2_ADDR_LO, _VAL, data);
    start = (NvU64)data << NV_HUBMMU_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT;

    data = GPU_REG_RD32(pGpu, pEntry->devicePriBase + NV_HUBMMU_PRI_MMU_WPR2_ADDR_HI);
    data = DRF_VAL(_HUBMMU, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
    end = (NvU64)data << NV_HUBMMU_PRI_MMU_WPR2_ADDR_HI_ALIGNMENT;

    if (start != end && end != 0)
        pWprRegions[1] = rangeMake(start, end - 1);
    else
        pWprRegions[1] = NV_RANGE_EMPTY;
}
