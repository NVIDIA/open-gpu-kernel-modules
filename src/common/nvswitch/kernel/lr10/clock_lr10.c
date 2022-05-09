/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "lr10/lr10.h"
#include "lr10/clock_lr10.h"
#include "lr10/soe_lr10.h"
#include "nvswitch/lr10/dev_soe_ip.h"
#include "nvswitch/lr10/dev_pri_ringstation_sys.h"
#include "nvswitch/lr10/dev_trim.h"
#include "nvswitch/lr10/dev_nvs.h"
#include "nvswitch/lr10/dev_nvlperf_ip.h"
#include "nvswitch/lr10/dev_npgperf_ip.h"
#include "nvswitch/lr10/dev_nvlctrl_ip.h"
#include "nvswitch/lr10/dev_nv_xp.h"
#include "nvswitch/lr10/dev_nv_xve.h"
#include "nvswitch/lr10/dev_nport_ip.h"
#include "nvswitch/lr10/dev_minion_ip.h"
#include "nvswitch/lr10/dev_timer.h"
#include "nvswitch/lr10/dev_pri_ringmaster.h"
#include "nvswitch/lr10/dev_pri_ringstation_prt.h"

//
// Initialize the software state of the switch PLL
//
NvlStatus
nvswitch_init_pll_config_lr10
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
    // Refer to the PLL35G_DYN_PRB_ESD_B2 cell Vbios Table, in the PLL datasheet
    // for restrictions on MDIV, NDIV and PLDIV to satisfy the pll's frequency limitation.
    //
    // PLL35G_DYN_PRB_ESD_B1.doc
    //

    pll_limits.ref_min_mhz = 100;
    pll_limits.ref_max_mhz = 100;
    pll_limits.vco_min_mhz = 1750;
    pll_limits.vco_max_mhz = 3800;
    pll_limits.update_min_mhz = 13;         // 13.5MHz
    pll_limits.update_max_mhz = 38;         // 38.4MHz
    pll_limits.m_min = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_MDIV_MIN;
    pll_limits.m_max = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_MDIV_MAX;
    pll_limits.n_min = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_NDIV_MIN;
    pll_limits.n_max = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_NDIV_MAX;
    pll_limits.pl_min = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_PLDIV_MIN;
    pll_limits.pl_max = NV_PCLOCK_NVSW_SWITCHPLL_COEFF_PLDIV_MAX;
    pll_limits.valid = NV_TRUE;

    //
    // set well known coefficients to achieve frequency
    // MDIV: need to set > 1 to achieve update_rate < 38.4 MHz
    // 100 / 5 = 20 MHz update rate, therefore MDIV = 5
    // NDIV needs to take us all the way to 1640 MHz
    // 1640 / 20 = 82.  But 100*82/5 < 1.75GHz VCOmin,
    // therefore double NDIV to 164 and set PDIV to 2.
    //

    pll.src_freq_khz = 100000;        // 100MHz
    pll.M = 5;
    pll.N = 164;
    pll.PL = 1;
    pll.dist_mode = NV_PCLOCK_NVSW_CLK_DIST_MODE_SWITCH2CLK_DIST_MODE_2XCLK;
    pll.refclk_div = 15;

    retval = nvswitch_validate_pll_config(device, &pll, pll_limits);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "Selecting default PLL setting.\n");

        // Select default, safe clock (1.64GHz)
        pll.src_freq_khz = 100000;        // 100MHz
        pll.M = 5;
        pll.N = 164;
        pll.PL = 2;
        pll.dist_mode =
             NV_PCLOCK_NVSW_CLK_DIST_MODE_SWITCH2CLK_DIST_MODE_1XCLK;
        pll.refclk_div = NV_PCLOCK_NVSW_RX_BYPASS_REFCLK_DIV_INIT;

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
nvswitch_init_pll_lr10
(
    nvswitch_device *device
)
{
    NvU32   pllRegVal;

    //
    // Clocks should only be initialized on silicon or a clocks netlist on emulation
    // Unfortunately, we don't have a full robust infrastructure for detecting the
    // runtime environment as we do on GPU.
    //
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, WARN,
        "%s: Skipping setup of NVSwitch clocks\n",
            __FUNCTION__);
        return NVL_SUCCESS;
    }

    pllRegVal = NVSWITCH_REG_RD32(device, _PCLOCK, _NVSW_SWITCHPLL_CFG);
    if (!FLD_TEST_DRF(_PCLOCK, _NVSW_SWITCHPLL_CFG, _PLL_LOCK, _TRUE, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _PLL_LOCK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }
    if (!FLD_TEST_DRF(_PCLOCK, _NVSW_SWITCHPLL_CFG, _PLL_FREQLOCK, _YES, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _PLL_FREQLOCK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pllRegVal = NVSWITCH_REG_RD32(device, _PCLOCK, _NVSW_SWITCHCLK);
    if (!FLD_TEST_DRF_NUM(_PCLOCK, _NVSW_SWITCHCLK, _RDY_SWITCHPLL, 1, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _RDY_SWITCHPLL failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pllRegVal = NVSWITCH_REG_RD32(device, _PCLOCK, _NVSW_SYSTEMCLK);
    if (!FLD_TEST_DRF_NUM(_PCLOCK, _NVSW_SYSTEMCLK, _SYSTEMCLK_RDY_SWITCHPLL, 1, pllRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: _RDY_SWITCHPLL for SYSTEMCLK failed\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

//
// Timer functions
//

void
nvswitch_init_hw_counter_lr10
(
    nvswitch_device *device
)
{
    return;
}

void
nvswitch_hw_counter_shutdown_lr10
(
    nvswitch_device *device
)
{
    return;
}

//
// Reads the 36-bit free running counter
//
NvU64
nvswitch_hw_counter_read_counter_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

//
// Initialize clock gating.
//
void
nvswitch_init_clock_gating_lr10
(
    nvswitch_device *device
)
{
    NvU32 regval;
    NvU32 i;

    // BUS
    NVSWITCH_REG_WR32(device, _PBUS, _EXT_CG1,
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG, __PROD)         |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_C11, __PROD)     |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_PRI, __PROD)     |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_UNROLL, __PROD)  |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_ROLL, __PROD)    |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_IFR, __PROD)     |
        DRF_DEF(_PBUS, _EXT_CG1, _SLCG_PMC, __PROD));

    // PRI
    NVSWITCH_REG_WR32(device, _PPRIV_MASTER, _CG1,
        DRF_DEF(_PPRIV_MASTER, _CG1, _SLCG, __PROD));

    regval = 
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _SLOWCLK, __PROD)              |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_CONFIG_REGS, __PROD)     |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_FUNNEL_DECODER, __PROD)  |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_FUNNEL_ARB, __PROD)      |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_HISTORY_BUFFER, __PROD)  |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_MASTER, __PROD)          |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_SLAVE, __PROD)           |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV_UCODE_TRAP, __PROD)      |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PRIV, __PROD)                 |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _LOC_PRIV, __PROD)             |
        DRF_DEF(_PPRIV_PRT, _CG1_SLCG, _PM, __PROD);

    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT0, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT1, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT2, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT3, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT4, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT5, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT6, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT7, _CG1, regval);
    NVSWITCH_REG_WR32(device, _PPRIV_PRT_PRT8, _CG1, regval);

    // XP3G
    NVSWITCH_REG_WR32(device, _XP, _PRI_XP3G_CG,
        DRF_DEF(_XP, _PRI_XP3G_CG, _IDLE_CG_DLY_CNT, __PROD)    |
        DRF_DEF(_XP, _PRI_XP3G_CG, _IDLE_CG_EN, __PROD)         |
        DRF_DEF(_XP, _PRI_XP3G_CG, _STATE_CG_EN, __PROD)        |
        DRF_DEF(_XP, _PRI_XP3G_CG, _STALL_CG_DLY_CNT, __PROD)   |
        DRF_DEF(_XP, _PRI_XP3G_CG, _STALL_CG_EN, __PROD)        |
        DRF_DEF(_XP, _PRI_XP3G_CG, _QUIESCENT_CG_EN, __PROD)    |
        DRF_DEF(_XP, _PRI_XP3G_CG, _WAKEUP_DLY_CNT, __PROD)     |
        DRF_DEF(_XP, _PRI_XP3G_CG, _THROT_CLK_CNT, __PROD)      |
        DRF_DEF(_XP, _PRI_XP3G_CG, _DI_DT_SKEW_VAL, __PROD)     |
        DRF_DEF(_XP, _PRI_XP3G_CG, _THROT_CLK_EN, __PROD)       |
        DRF_DEF(_XP, _PRI_XP3G_CG, _THROT_CLK_SW_OVER, __PROD)  |
        DRF_DEF(_XP, _PRI_XP3G_CG, _PAUSE_CG_EN, __PROD)        |
        DRF_DEF(_XP, _PRI_XP3G_CG, _HALT_CG_EN, __PROD));

    NVSWITCH_REG_WR32(device, _XP, _PRI_XP3G_CG1,
        DRF_DEF(_XP, _PRI_XP3G_CG1, _MONITOR_CG_EN, __PROD));

    // XVE
    NVSWITCH_ENG_WR32_LR10(device, XVE, , 0, _XVE, _PRI_XVE_CG,
        DRF_DEF(_XVE, _PRI_XVE_CG, _IDLE_CG_DLY_CNT, __PROD)    |
        DRF_DEF(_XVE, _PRI_XVE_CG, _IDLE_CG_EN, __PROD)         |
        DRF_DEF(_XVE, _PRI_XVE_CG, _STATE_CG_EN, __PROD)        |
        DRF_DEF(_XVE, _PRI_XVE_CG, _STALL_CG_DLY_CNT, __PROD)   |
        DRF_DEF(_XVE, _PRI_XVE_CG, _STALL_CG_EN, __PROD)        |
        DRF_DEF(_XVE, _PRI_XVE_CG, _QUIESCENT_CG_EN, __PROD)    |
        DRF_DEF(_XVE, _PRI_XVE_CG, _WAKEUP_DLY_CNT, __PROD)     |
        DRF_DEF(_XVE, _PRI_XVE_CG, _THROT_CLK_CNT, __PROD)      |
        DRF_DEF(_XVE, _PRI_XVE_CG, _DI_DT_SKEW_VAL, __PROD)     |
        DRF_DEF(_XVE, _PRI_XVE_CG, _THROT_CLK_EN, __PROD)       |
        DRF_DEF(_XVE, _PRI_XVE_CG, _THROT_CLK_SW_OVER, __PROD)  |
        DRF_DEF(_XVE, _PRI_XVE_CG, _PAUSE_CG_EN, __PROD)        |
        DRF_DEF(_XVE, _PRI_XVE_CG, _HALT_CG_EN, __PROD));

    NVSWITCH_ENG_WR32_LR10(device, XVE, , 0, _XVE, _PRI_XVE_CG1,
        DRF_DEF(_XVE, _PRI_XVE_CG1, _MONITOR_CG_EN, __PROD)     |
        DRF_DEF(_XVE, _PRI_XVE_CG1, _SLCG, __PROD));

    // NPORT
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _CTRL_SLCG,
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _INGRESS,  __PROD)  |
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _ROUTE,    __PROD)  |
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _EGRESS,   __PROD)  |
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _STRACK,   __PROD)  |
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _TAGSTATE, __PROD)  |
        DRF_DEF(_NPORT, _CTRL_SLCG_DIS_CG, _TREX,     __PROD));

    // NPG_PERFMON
    NVSWITCH_BCAST_WR32_LR10(device, NPG_PERFMON, _NPGPERF, _CTRL_CLOCK_GATING,
        DRF_DEF(_NPGPERF, _CTRL_CLOCK_GATING, _CG1_SLCG, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NPG_PERFMON, _NPGPERF, _PERF_CTRL_CLOCK_GATING,
        DRF_DEF(_NPGPERF, _PERF_CTRL_CLOCK_GATING, _CG1_SLCG, __PROD) |
        DRF_DEF(_NPGPERF, _PERF_CTRL_CLOCK_GATING, _CONTEXT_FREEZE, __PROD));

    //
    // NVLW_PERFMON
    //
    // There registers are protected by PRIV_LEVEL_MASK6.
    // PLM6 will not be blown on Production fuses.
    //
    NVSWITCH_BCAST_WR32_LR10(device, NVLW_PERFMON, _NVLPERF, _CTRL_CLOCK_GATING,
        DRF_DEF(_NVLPERF, _CTRL_CLOCK_GATING, _CG1_SLCG, __PROD) |
        DRF_DEF(_NVLPERF, _CTRL_CLOCK_GATING, _CG1_SLCG_CTRL, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NVLW_PERFMON, _NVLPERF, _PERF_CTRL_CLOCK_GATING,
        DRF_DEF(_NVLPERF, _PERF_CTRL_CLOCK_GATING, _CG1_SLCG, __PROD) |
        DRF_DEF(_NVLPERF, _PERF_CTRL_CLOCK_GATING, _CONTEXT_FREEZE, __PROD));

    // NVLCTRL
    NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL, _PLL_PRI_CLOCK_GATING,
        DRF_DEF(_NVLCTRL, _PLL_PRI_CLOCK_GATING, _CG1_SLCG, __PROD));

    // MINION
    for (i = 0; i < NVSWITCH_ENG_COUNT(device, MINION, ); i++)
    {
        regval = NVSWITCH_ENG_RD32_LR10(device, MINION, i, _CMINION_FALCON, _CG2);

        NVSWITCH_ENG_WR32_LR10(device, MINION, , i, _CMINION_FALCON, _CG2,
            FLD_SET_DRF(_CMINION_FALCON, _CG2, _SLCG, __PROD, regval));
    }

    // PTIMER
    NVSWITCH_REG_WR32(device, _PTIMER, _PRI_TMR_CG1,
        DRF_DEF(_PTIMER, _PRI_TMR_CG1, _MONITOR_CG_EN, __PROD) |
        DRF_DEF(_PTIMER, _PRI_TMR_CG1, _SLCG, __PROD));

    // SOE
    regval = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE, _FBIF_CG1);
    regval = FLD_SET_DRF(_SOE, _FBIF_CG1, _SLCG, __PROD, regval);
    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE, _FBIF_CG1, regval);

    regval = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE, _FALCON_CG2);
    regval = FLD_SET_DRF(_SOE, _FALCON_CG2, _SLCG, __PROD, regval);
    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE_FALCON, _CG2, regval);

    regval = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_MISC, _CG1);
    regval = FLD_SET_DRF(_SOE, _MISC_CG1, _SLCG, __PROD, regval);
    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE_MISC, _CG1, regval);

    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE_MISC, _TOP_CG,
        DRF_DEF(_SOE_MISC, _TOP_CG, _IDLE_CG_DLY_CNT, __PROD));

    return;
}
