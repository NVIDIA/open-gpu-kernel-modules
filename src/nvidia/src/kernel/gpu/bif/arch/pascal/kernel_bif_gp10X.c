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
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"

#include "published/pascal/gp102/dev_boot.h"
#include "published/pascal/gp102/dev_nv_xp.h"
#include "published/pascal/gp102/dev_nv_xve.h"
#include "published/pascal/gp102/dev_nv_xve_addendum.h"

/* ------------------------ Public Functions -------------------------------- */


/*!
 * @brief  Get the PMC bit of the valid Engines to reset.
 *
 * @return All valid engines
 */
NvU32
kbifGetValidEnginesToReset_GP10X
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    return (DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PDISP, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _SEC, _ENABLED));
}


/*!
 * @brief  Reset the chip.
 *
 * Use Function Level Reset(FLR) to reset as much of the chip as possible.
 * If FLR is not supported use the XVE sw reset logic .
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif KernelBif object pointer
 *
 * @return  NV_STATUS
 */
NV_STATUS
kbifDoFullChipReset_GP10X
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS    *pSys       = SYS_GET_INSTANCE();
    OBJCL     *pCl        = SYS_GET_CL(pSys);
    NV_STATUS  status     = NV_OK;
    NvU32      tempRegVal;
    NvU32      oldPmc;
    NvU32      newPmc;
    NvBool     bIsFLRSupportedAndEnabled;

    //
    // We support FLR for SKUs which are FLR capable.
    // Also check if we want to enforce legacy reset behavior by disabling FLR
    //
    bIsFLRSupportedAndEnabled =
        pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED) &&
        !pKernelBif->bForceDisableFLR;

    // First Reset PMC
    oldPmc = GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    newPmc = oldPmc;

    newPmc &= ~(kbifGetValidEnginesToReset_HAL(pGpu, pKernelBif));

    GPU_REG_WR32(pGpu, NV_PMC_ENABLE, newPmc);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);

     if (pCl && !pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE)
         )
    {
        //
        // We can not issue SW_RESET or Function Level Reset(FLR) as we are not able to access PCI
        // config space. Now we have disabled engines, please re-enable them before
        // tearing down RM. To be safe, do it right here.
        //
        GPU_REG_WR32(pGpu, NV_PMC_ENABLE, oldPmc);
    }
    else
    {
        //
        // If FLR is supported, issue FLR otherwise issue SW_RESET
        // There is no point in issuing SW_RESET if FLR fails because both of them reset Fn0
        // except that FLR resets XVE unit as well
        //
        if (bIsFLRSupportedAndEnabled)
        {
            NV_ASSERT_OK(status = kbifDoFunctionLevelReset_HAL(pGpu, pKernelBif));
        }
        else
        {
            //
            // NV_XP_CYA_1_BLOCK_HOST2XP_HOLD_LTSSM to be set to 1 before enabling
            // NV_XVE_SW_RESET (set to 3). If not, when NV_XVE_SW_RESET change back to 0, host will
            // try to do rom init and will assert HOST2XP_HOLD_LTSSM to 0 which will cause ltssm to
            // goto detect which will cause all kinds of other issues.
            //
            do
            {
                GPU_FLD_IDX_WR_DRF_DEF(pGpu, _XP, _PL_CYA_1, 0, _BLOCK_HOST2XP_HOLD_LTSSM, _ENABLE);
                tempRegVal = GPU_REG_RD32(pGpu, NV_XP_PL_CYA_1(0));
            } while (!FLD_TEST_DRF(_XP, _PL_CYA_1, _BLOCK_HOST2XP_HOLD_LTSSM, _ENABLE, tempRegVal));

            NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_SW_RESET, &tempRegVal));
            tempRegVal = FLD_SET_DRF(_XVE, _SW_RESET, _RESET, _ENABLE, tempRegVal);
            clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                             gpuGetDevice(pGpu), 0, NV_XVE_SW_RESET, tempRegVal);
        }
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);
    }

    // execute rest of the sequence for SW_RESET only if we have not issued FLR above
    if (!bIsFLRSupportedAndEnabled)
    {
        {
            osDelay(4);
        }

        if (pCl && !pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE)
        )
        {
            // We have not enabled NV_XVE_SW_RESET. So early return.
            return status;
        }
        else
        {
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
        }
    } // if (!bIsFLRSupportedAndEnabled)

    return status;
}
