/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "common_nvswitch.h"
#include "ls10/ls10.h"
#include "ls10/clock_ls10.h"

#include "nvswitch/ls10/dev_trim.h"
#include "nvswitch/ls10/dev_soe_ip.h"
#include "nvswitch/ls10/dev_npgperf_ip.h"
#include "nvswitch/ls10/dev_nvlw_ip.h"
#include "nvswitch/ls10/dev_nport_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_timer_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_pri_hub_prt_ip.h"
#include "nvswitch/ls10/dev_pri_masterstation_ip.h"

//
// Initialize the software state of the switch PLL
//
NvlStatus
nvswitch_init_pll_config_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PLL_LIMITS pll_limits;
    NVSWITCH_PLL_INFO pll;
    NvlStatus retval = NVL_SUCCESS;

    //
    // These parameters could come from schmoo'ing API, settings file or a ROM.
    // If no configuration ROM settings are present, use the PLL documentation
    //
    // PLL40G_SMALL_ESD.doc
    //

    pll_limits.ref_min_mhz = 100;
    pll_limits.ref_max_mhz = 100;
    pll_limits.vco_min_mhz = 1750;
    pll_limits.vco_max_mhz = 3800;
    pll_limits.update_min_mhz = 13;         // 13.5MHz
    pll_limits.update_max_mhz = 38;         // 38.4MHz

    pll_limits.m_min = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_MDIV_MIN;
    pll_limits.m_max = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_MDIV_MAX;
    pll_limits.n_min = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_NDIV_MIN;
    pll_limits.n_max = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_NDIV_MAX;
    pll_limits.pl_min = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_PLDIV_MIN;
    pll_limits.pl_max = NV_CLOCK_NVSW_SYS_SWITCHPLL_COEFF_PLDIV_MAX;
    pll_limits.valid = NV_TRUE;

    //
    // set well known coefficients to achieve frequency
    //

    pll.src_freq_khz = 100000;        // 100MHz
    pll.M = 3;
    pll.N = 80;
    pll.PL = 2;
    pll.dist_mode = 0;      // Ignored.  Only 1x supported
    pll.refclk_div = NV_CLOCK_NVSW_SYS_RX_BYPASS_REFCLK_DIV_INIT;

    retval = nvswitch_validate_pll_config(device, &pll, pll_limits);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "Selecting default PLL setting.\n");

        // Select default, safe clock
        pll.src_freq_khz = 100000;        // 100MHz
        pll.M = 3;
        pll.N = 80;
        pll.PL = 2;
        pll.dist_mode = 0;      // Ignored.  Only 1x supported
        pll.refclk_div = NV_CLOCK_NVSW_SYS_RX_BYPASS_REFCLK_DIV_INIT;

        retval = nvswitch_validate_pll_config(device, &pll, pll_limits);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Default PLL setting failed.\n");
            return retval;
        }
    }

    device->switch_pll = pll;

    return NVL_SUCCESS;
}

//
// Check that the PLLs are initialized. VBIOS is expected to configure PLLs
//
NvlStatus
nvswitch_init_pll_ls10
(
    nvswitch_device *device
)
{
    NvU32   pllRegVal;

    //
    // Clocks should only be initialized on silicon or a clocks netlist on emulation
    //
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, WARN,
        "%s: Skipping setup of NVSwitch clocks\n",
            __FUNCTION__);
        return NVL_SUCCESS;
    }

    pllRegVal = NVSWITCH_ENG_RD32(device, CLKS_SYS,  , 0, _CLOCK_NVSW_SYS, _SWITCHPLL_CFG);
    if (!FLD_TEST_DRF(_CLOCK_NVSW_SYS, _SWITCHPLL_CFG, _PLL_LOCK, _TRUE, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _PLL_LOCK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pllRegVal = NVSWITCH_ENG_RD32(device, CLKS_SYS,  , 0, _CLOCK_NVSW_SYS, _SWITCHPLL_CTRL);
    if (!FLD_TEST_DRF_NUM(_CLOCK_NVSW_SYS, _SWITCHPLL_CTRL, _PLL_FREQLOCK, 1, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _PLL_FREQLOCK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pllRegVal = NVSWITCH_ENG_RD32(device, CLKS_SYS,  , 0, _CLOCK_NVSW_SYS, _SWITCHCLK_SWITCH_DIVIDER);
    if (!FLD_TEST_DRF_NUM(_CLOCK_NVSW_SYS, _SWITCHCLK_SWITCH_DIVIDER, _SWITCH_DIVIDER_DONE, 1, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _SWITCH_DIVIDER_DONE failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pllRegVal = NVSWITCH_ENG_RD32(device, CLKS_SYS,  , 0, _CLOCK_NVSW_SYS, _SYSTEM_CLK_SWITCH_DIVIDER);
    if (!FLD_TEST_DRF_NUM(_CLOCK_NVSW_SYS, _SYSTEM_CLK_SWITCH_DIVIDER, _SWITCH_DIVIDER_DONE, 1, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _SWITCH_DIVIDER_DONE for SYSTEMCLK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

//
// Initialize clock gating.
//
void
nvswitch_init_clock_gating_ls10
(
    nvswitch_device *device
)
{
    //
    // CG and PROD settings were already handled by:
    //    - nvswitch_nvs_top_prod_ls10
    //    - nvswitch_npg_prod_ls10
    //    - nvswitch_apply_prod_nvlw_ls10
    //    - nvswitch_apply_prod_nxbar_ls10
    //
    // which were all called by nvswitch_initialize_ip_wrappers_ls10

    return;
}

