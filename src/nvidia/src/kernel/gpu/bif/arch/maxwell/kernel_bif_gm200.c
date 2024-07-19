/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------- System Includes -------------------------------- */
// TODO CLEANUP
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include "platform/chipset/chipset.h"
#include "nvdevid.h"

#include "published/maxwell/gm200/dev_boot.h"
#include "published/maxwell/gm200/dev_nv_xp.h"
#include "published/maxwell/gm200/dev_nv_xve.h"
#include "published/maxwell/gm200/dev_nv_xve_addendum.h"

/*!
 * @brief  Get the PMC bit of the valid Engines to reset.
 *
 * @return All valid engines
 */
NvU32
kbifGetValidEnginesToReset_GM200
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    return (DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PWR, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PDISP, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _SEC, _ENABLED));
}


/*!
 * @brief  Reset the chip.
 *
 * Use the XVE sw reset logic to reset as much of the chip as possible.
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif KernelBif object pointer
 *
 * @return  NV_STATUS
 */
NV_STATUS
kbifDoFullChipReset_GM200
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);
    NvU32  tempRegVal;
    NvU32  oldPmc, newPmc;
    NV_STATUS status;

    // First Reset PMC
    oldPmc = GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    newPmc = oldPmc;

    newPmc &= ~(kbifGetValidEnginesToReset_HAL(pGpu, GPU_GET_KERNEL_BIF(pGpu)));

    GPU_REG_WR32(pGpu, NV_PMC_ENABLE, newPmc);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);

    //
    // Before doing SW_RESET, init NV_XP_PL_CYA_1_BLOCK_HOST2XP_HOLD_LTSSM to 1.
    // else when NV_XVE_SW_RESET to 0, host will try to do rom init and will assert
    // HOST2XP_HOLD_LTSSM to 0 which will cause ltssm to goto detect.
    //
    tempRegVal = GPU_REG_RD32(pGpu, NV_XP_PL_CYA_1(0));
    do
    {
        tempRegVal |= DRF_NUM(_XP, _PL_CYA_1, _BLOCK_HOST2XP_HOLD_LTSSM, 1);
        GPU_REG_WR32(pGpu, NV_XP_PL_CYA_1(0), tempRegVal);
        tempRegVal = GPU_REG_RD32(pGpu, NV_XP_PL_CYA_1(0));
    } while ((tempRegVal & DRF_NUM(_XP, _PL_CYA_1, _BLOCK_HOST2XP_HOLD_LTSSM, 1)) == 0);

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_SW_RESET, &tempRegVal));
    tempRegVal = FLD_SET_DRF(_XVE, _SW_RESET, _RESET, _ENABLE, tempRegVal);
    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_SW_RESET, tempRegVal);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);

    // wait a bit to make sure GPU is reset
    osDelay(1);

    //
    // Come out of reset. Note that when SW/full-chip reset is triggered by the
    // above write to NV_XVE_SW_RESET, BAR0 priv writes do not work and thus
    // this write must be a PCI config bus write.
    //
    // Undefined bit fields ( bit 3, 31:28 ) are set after reset enable, therefore,
    // only retrieve defined bit fields and set NV_XVE_SW_RESET_RESET_DISABLE to
    // come out of reset.
    //
    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_SW_RESET, &tempRegVal));
    tempRegVal &= DRF_SHIFTMASK(NV_XVE_SW_RESET_RESET) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_GPU_ON_SW_RESET) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_COUNTER_EN) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_COUNTER_VAL) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_CLOCK_ON_SW_RESET) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_CLOCK_COUNTER_EN) |
                  DRF_SHIFTMASK(NV_XVE_SW_RESET_CLOCK_COUNTER_VAL);
    tempRegVal = FLD_SET_DRF(_XVE, _SW_RESET, _RESET, _DISABLE, tempRegVal);
    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_SW_RESET, tempRegVal);

    //
    // When bug 1511451 is present, SW_RESET will clear BAR3, and IO accesses
    // will fail when legacy VBIOS is called. Apply the related SW WAR now.
    //
    status = kbifApplyWarForBug1511451_HAL(pGpu, pKernelBif);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed while applying WAR for Bug 1511451\n");
        NV_ASSERT(0);
    }

    return status;
}
